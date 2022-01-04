/*******************************************************************************
 * Copyright (c) 2007-2019 Wind River Systems, Inc. and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 * The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 * http://www.eclipse.org/org/documents/edl-v10.php.
 * You may elect to redistribute this code under either of these licenses.
 *
 * Contributors:
 *     Wind River Systems - initial API and implementation
 *******************************************************************************/

/*
 * Transport agnostic channel implementation.
 */

/* TODO: Somehow we should make it clear what needs to be done to add another transport layer.
 * Perhaps have a template or a readme file for it. */

#if defined(__GNUC__) && !defined(_GNU_SOURCE)
/* pread() need _GNU_SOURCE */
#  define _GNU_SOURCE
#endif

#include <tcf/config.h>
#include <stddef.h>
#include <errno.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <tcf/framework/tcf.h>
#include <tcf/framework/channel.h>
#include <tcf/framework/channel_tcp.h>
#include <tcf/framework/channel_pipe.h>
#include <tcf/framework/protocol.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/events.h>
#include <tcf/framework/exceptions.h>
#include <tcf/framework/trace.h>
#include <tcf/framework/link.h>
#include <tcf/framework/json.h>

#ifndef DEFAULT_SERVER_NAME
#  define DEFAULT_SERVER_NAME "TCF Agent"
#endif

typedef struct ChannelLock {
    LINK link;
    const char * msg;
    unsigned cnt;
    unsigned timer;
} ChannelLock;

typedef struct ChannelTransport {
    char * transportname;
    ChannelServerCreate create;
    ChannelConnect connect;
} ChannelTransport;

static void trigger_channel_shutdown(ShutdownInfo * obj);

ShutdownInfo channel_shutdown = { trigger_channel_shutdown };
LINK channel_root = TCF_LIST_INIT(channel_root);
LINK channel_server_root = TCF_LIST_INIT(channel_server_root);

#define BCAST_MAGIC 0x1463e328

#define out2bcast(A)        ((TCFBroadcastGroup *)((char *)(A) - offsetof(TCFBroadcastGroup, out)))
#define bclink2channel(A)   ((Channel *)((char *)(A) - offsetof(Channel, bclink)))
#define susplink2channel(A) ((Channel *)((char *)(A) - offsetof(Channel, susplink)))
#define chan2lock(A)        ((ChannelLock *)((char *)(A) - offsetof(ChannelLock, link)))
#define client2channel(A)   ((Channel *)((char *)(A) - offsetof(Channel, client)))

static ChannelTransport * channel_transport = NULL;
static unsigned channel_transport_cnt = 0;

static ChannelCreateListener * create_listeners = NULL;
static unsigned create_listeners_cnt = 0;
static unsigned create_listeners_max = 0;

static ChannelOpenListener * open_listeners = NULL;
static unsigned open_listeners_cnt = 0;
static unsigned open_listeners_max = 0;

static ChannelCloseListener * close_listeners = NULL;
static unsigned close_listeners_cnt = 0;
static unsigned close_listeners_max = 0;
static size_t extension_size = 0;
static int channel_created = 0;

static const int BROADCAST_OK_STATES = (1 << ChannelStateConnected) | (1 << ChannelStateRedirectSent) | (1 << ChannelStateRedirectReceived);
#define isBoardcastOkay(c) ((1 << (c)->state) & BROADCAST_OK_STATES)

static void trigger_channel_shutdown(ShutdownInfo * obj) {
    LINK * l;

    l = channel_root.next;
    while (l != &channel_root) {
        Channel * c = chanlink2channelp(l);
        l = l->next;
        if (!is_channel_closed(c)) {
            channel_close(c);
        }
    }

    l = channel_server_root.next;
    while (l != &channel_server_root) {
        ChannelServer * s = servlink2channelserverp(l);
        l = l->next;
        s->close(s);
    }
}

static void flush_bcg_buf(TCFBroadcastGroup * bcg) {
    LINK * l = bcg->channels.next;
    size_t size = bcg->out.cur - bcg->buf;
    while (l != &bcg->channels) {
        Channel * c = bclink2channel(l);
        if (isBoardcastOkay(c)) c->out.write_block(&c->out, (char *)bcg->buf, size);
        l = l->next;
    }
    bcg->out.cur = bcg->buf;
}

static void write_all(OutputStream * out, int byte) {
    TCFBroadcastGroup * bcg = out2bcast(out);
    LINK * l = bcg->channels.next;

    assert(is_dispatch_thread());
    assert(bcg->magic == BCAST_MAGIC);
    if (bcg->out.cur != bcg->buf) flush_bcg_buf(bcg);
    while (l != &bcg->channels) {
        Channel * c = bclink2channel(l);
        if (isBoardcastOkay(c)) write_stream(&c->out, byte);
        l = l->next;
    }
}

static void write_block_all(OutputStream * out, const char * bytes, size_t size) {
    TCFBroadcastGroup * bcg = out2bcast(out);
    LINK * l = bcg->channels.next;

    assert(is_dispatch_thread());
    assert(bcg->magic == BCAST_MAGIC);
    if (bcg->out.cur != bcg->buf) flush_bcg_buf(bcg);
    while (l != &bcg->channels) {
        Channel * c = bclink2channel(l);
        if (isBoardcastOkay(c)) c->out.write_block(&c->out, bytes, size);
        l = l->next;
    }
}

static ssize_t splice_block_all(OutputStream * out, int fd, size_t size, int64_t * offset) {
    char buffer[0x400];
    ssize_t rd;

    assert(is_dispatch_thread());
    if (size > sizeof(buffer)) size = sizeof(buffer);
    if (offset != NULL) {
        rd = pread(fd, buffer, size, (off_t)*offset);
        if (rd > 0) *offset += rd;
    }
    else {
        rd = read(fd, buffer, size);
    }
    if (rd > 0) write_block_all(out, buffer, rd);
    return rd;
}

void add_channel_create_listener(ChannelCreateListener listener) {
    if (create_listeners_cnt >= create_listeners_max) {
        create_listeners_max += 8;
        create_listeners = (ChannelCreateListener *)loc_realloc(create_listeners, sizeof(ChannelCreateListener) * create_listeners_max);
    }
    create_listeners[create_listeners_cnt++] = listener;
}

void add_channel_open_listener(ChannelOpenListener listener) {
    if (open_listeners_cnt >= open_listeners_max) {
        open_listeners_max += 8;
        open_listeners = (ChannelOpenListener *)loc_realloc(open_listeners, sizeof(ChannelOpenListener) * open_listeners_max);
    }
    open_listeners[open_listeners_cnt++] = listener;
}

void add_channel_close_listener(ChannelCloseListener listener) {
    if (close_listeners_cnt >= close_listeners_max) {
        close_listeners_max += 8;
        close_listeners = (ChannelCloseListener *)loc_realloc(close_listeners, sizeof(ChannelCloseListener) * close_listeners_max);
    }
    close_listeners[close_listeners_cnt++] = listener;
}

static void client_connection_lcb(ClientConnection * c) {
    channel_lock(client2channel(c));
}

static void client_connection_ucb(ClientConnection * c) {
    channel_unlock(client2channel(c));
}

void notify_channel_created(Channel * c) {
    unsigned i;
    assert(channel_created);
    assert(client2channel(&c->client) == c);
    c->client.lock = client_connection_lcb;
    c->client.unlock = client_connection_ucb;
    for (i = 0; i < create_listeners_cnt; i++) {
        create_listeners[i](c);
    }
}

void notify_channel_opened(Channel * c) {
    unsigned i;
    assert(channel_created);
    assert(!is_channel_closed(c));
    assert(c->state == ChannelStateConnected);
    if (c->notified_open) return;
    c->notified_open = 1;
    channel_lock(c);
    for (i = 0; i < open_listeners_cnt; i++) {
        open_listeners[i](c);
    }
    notify_client_connected(&c->client);
    channel_unlock(c);
}

void notify_channel_closed(Channel * c) {
    unsigned i;
    assert(channel_created);
    assert(c->state != ChannelStateConnected);
    if (!c->notified_open) return;
    c->notified_open = 0;
    channel_lock(c);
    notify_client_disconnected(&c->client);
    for (i = 0; i < close_listeners_cnt; i++) {
        close_listeners[i](c);
    }
    channel_unlock(c);
}

TCFBroadcastGroup * broadcast_group_alloc(void) {
    TCFBroadcastGroup * p = (TCFBroadcastGroup*)loc_alloc_zero(sizeof(TCFBroadcastGroup));

    list_init(&p->channels);
    p->magic = BCAST_MAGIC;
    p->out.write = write_all;
    p->out.write_block = write_block_all;
    p->out.splice_block = splice_block_all;
    p->out.cur = p->buf;
    p->out.end = p->buf + sizeof(p->buf);
    p->ref_count = 1;
    return p;
}

static void broadcast_group_dispose(TCFBroadcastGroup * p) {
    LINK * l = p->channels.next;

    assert(p->ref_count == 0);
    assert(is_dispatch_thread());
    while (l != &p->channels) {
        Channel * c = bclink2channel(l);
        assert(c->bcg == p);
        l = l->next;
        c->bcg = NULL;
        list_remove(&c->bclink);
    }
    assert(list_is_empty(&p->channels));
    p->magic = 0;
    loc_free(p);
}

void broadcast_group_lock(TCFBroadcastGroup * p) {
    assert(p->ref_count > 0);
    assert(is_dispatch_thread());
    p->ref_count++;
}

void broadcast_group_unlock(TCFBroadcastGroup * p) {
    assert(p->ref_count > 0);
    assert(is_dispatch_thread());
    if (--(p->ref_count) == 0) {
        broadcast_group_dispose(p);
    }
}

void channel_set_broadcast_group(Channel * c, TCFBroadcastGroup * bcg) {
    if (c->bcg != NULL) channel_clear_broadcast_group(c);
    list_add_last(&c->bclink, &bcg->channels);
    c->bcg = bcg;
}

void channel_clear_broadcast_group(Channel * c) {
    if (c->bcg == NULL) return;
    list_remove(&c->bclink);
    c->bcg = NULL;
}

void channel_lock(Channel * c) {
    assert(channel_created);
    c->lock(c);
}

void channel_unlock(Channel * c) {
    assert(channel_created);
    c->unlock(c);
}

#ifndef NDEBUG
static int lock_timer_posted = 0;

static void lock_timer_event(void * args) {
    unsigned cnt = 0;
    LINK * l = channel_root.next;
    assert(lock_timer_posted);
    while (l != &channel_root) {
        Channel * c = chanlink2channelp(l);
        if (is_channel_closed(c) && c->locks.next != NULL) {
            LINK * p = c->locks.next;
            while (p != &c->locks) {
                ChannelLock * cl = chan2lock(p);
                assert(cl->cnt > 0);
                if (cl->timer >= 2) {
                    trace(LOG_ALWAYS, "Invalid channel lock: %s, count %d", cl->msg, cl->cnt);
                }
                cl->timer++;
                p = p->next;
                cnt++;
            }
        }
        l = l->next;
    }
    if (cnt > 0) {
        post_event_with_delay(lock_timer_event, NULL, 2000000);
    }
    else {
        lock_timer_posted = 0;
    }
}
#endif

void channel_lock_with_msg(Channel * c, const char * msg) {
    assert(channel_created);
    c->lock(c);
#ifndef NDEBUG
    if (msg != NULL) {
        int ok = 0;
        ChannelLock * l = NULL;
        if (c->locks.next != NULL) {
            LINK * p;
            for (p = c->locks.next; p != &c->locks; p = p->next) {
                l = chan2lock(p);
                if (l->msg == msg) {
                    l->cnt++;
                    ok = 1;
                    break;
                }
            }
        }
        else {
            list_init(&c->locks);
        }
        if (!ok) {
            l = (ChannelLock *)loc_alloc_zero(sizeof(ChannelLock));
            l->msg = msg;
            l->cnt = 1;
            list_add_first(&l->link, &c->locks);
            if (!lock_timer_posted) {
                post_event_with_delay(lock_timer_event, NULL, 2000000);
                lock_timer_posted = 1;
            }
        }
    }
#endif
}

void channel_unlock_with_msg(Channel * c, const char * msg) {
#ifndef NDEBUG
    assert(channel_created);
    if (msg != NULL) {
        int ok = 0;
        ChannelLock * l = NULL;
        if (c->locks.next != NULL) {
            LINK * p;
            for (p = c->locks.next; p != &c->locks; p = p->next) {
                l = chan2lock(p);
                if (l->msg == msg) {
                    l->cnt--;
                    ok = 1;
                    break;
                }
            }
        }
        if (!ok) {
            trace(LOG_ALWAYS, "Invalid channel unlock: %s", msg);
        }
        else if (l->cnt == 0) {
            list_remove(&l->link);
            loc_free(l);
        }
    }
#endif
    c->unlock(c);
}

int is_channel_closed(Channel * c) {
    if (c->is_closed) return c->is_closed(c);
    return c->state == ChannelStateDisconnected;
}

PeerServer * channel_peer_from_url(const char * url) {
    int i;
    const char * s;
    const char * user = get_user_name();
    char transport[16];
    PeerServer * ps = peer_server_alloc();

    peer_server_addprop(ps, loc_strdup("Name"), loc_strdup(DEFAULT_SERVER_NAME));
    peer_server_addprop(ps, loc_strdup("OSName"), loc_strdup(get_os_name()));
    if (user != NULL) peer_server_addprop(ps, loc_strdup("UserName"), loc_strdup(user));
    peer_server_addprop(ps, loc_strdup("AgentID"), loc_strdup(get_agent_id()));

    s = url;
    i = 0;
    while (*s && isalpha((int)*s) && i < (int)sizeof transport) transport[i++] = (char)toupper((int)*s++);
    if (*s == ':' && i < (int)sizeof transport) {
        s++;
        peer_server_addprop(ps, loc_strdup("TransportName"), loc_strndup(transport, i));
        url = s;
    }
    else {
        s = url;
    }
    while (*s && *s != ':' && *s != ';') s++;
    if (s != url) peer_server_addprop(ps, loc_strdup("Host"), loc_strndup(url, s - url));
    if (*s == ':') {
        s++;
        url = s;
        while (*s && *s != ';') s++;
        if (s != url) peer_server_addprop(ps, loc_strdup("Port"), loc_strndup(url, s - url));
    }

    while (*s == ';') {
        char * name;
        char * value;
        s++;
        url = s;
        while (*s && *s != '=') s++;
        if (*s != '=' || s == url) {
            s = url - 1;
            break;
        }
        name = loc_strndup(url, s - url);
        s++;
        url = s;
        while (*s && *s != ';') s++;
        value = loc_strndup(url, s - url);
        peer_server_addprop(ps, name, value);
    }
    if (*s) {
        peer_server_free(ps);
        return NULL;
    }
    return ps;
}

char * channel_peer_to_json(PeerServer * ps) {
    unsigned i;
    char * rval;
    ByteArrayOutputStream buf;
    OutputStream * out;

    out = create_byte_array_output_stream(&buf);
    write_stream(out, '{');
    for (i = 0; i < ps->ind; i++) {
        if (i > 0) write_stream(out, ',');
        json_write_string(out, ps->list[i].name);
        write_stream(out, ':');
        json_write_string(out, ps->list[i].value);
    }
    write_stream(out, '}');
    write_stream(out, 0);
    get_byte_array_output_stream_data(&buf, &rval, NULL);
    return rval;
}

/*
 * Start TCF channel server
 */
ChannelServer * channel_server(PeerServer * ps) {
    const char * transportname = peer_server_getprop(ps, "TransportName", NULL);
    const char * hidden = peer_server_getprop(ps, "Hidden", NULL);

    if (transportname == NULL) {
        transportname = "TCP";
        peer_server_addprop(ps, loc_strdup("TransportName"), loc_strdup(transportname));
    }

    ps->flags |= PS_FLAG_LOCAL;
    if (hidden == NULL || atoi(hidden) == 0) ps->flags |= PS_FLAG_DISCOVERABLE;

    if (strcmp(transportname, "TCP") == 0 || strcmp(transportname, "SSL") == 0) {
        return channel_tcp_server(ps);
    }
    else if (strcmp(transportname, "PIPE") == 0) {
        return channel_pipe_server(ps);
    }
    else if (strcmp(transportname, "UNIX") == 0) {
        return channel_unix_server(ps);
    }
    else {
        unsigned i;
        for (i = 0; i < channel_transport_cnt; i++) {
            if (strcmp(transportname, channel_transport[i].transportname) == 0) {
                return (channel_transport[i].create(ps));
            }
        }
        errno = ERR_INV_TRANSPORT;
        return NULL;
    }
}

/*
 * Connect to TCF channel server
 */
void channel_connect(PeerServer * ps, ChannelConnectCallBack callback, void * callback_args) {
    const char * transportname = peer_server_getprop(ps, "TransportName", NULL);

    if (transportname == NULL || strcmp(transportname, "TCP") == 0 || strcmp(transportname, "SSL") == 0) {
        channel_tcp_connect(ps, callback, callback_args);
    }
    else if (strcmp(transportname, "PIPE") == 0) {
        channel_pipe_connect(ps, callback, callback_args);
    }
    else if (strcmp(transportname, "UNIX") == 0) {
        channel_unix_connect(ps, callback, callback_args);
    }
    else {
        unsigned i;
        for (i = 0; i < channel_transport_cnt; i++) {
            if (strcmp(transportname, channel_transport[i].transportname) == 0) {
                channel_transport[i].connect(ps, callback, callback_args);
                return;
            }
        }
        callback(callback_args, ERR_INV_TRANSPORT, NULL);
    }
}

/*
 * Register an extension of struct Channel.
 * Return offset of extension data area.
 * Additional memory of given size will be allocated in each Channel struct.
 * Client are allowed to call this function only during initialization.
 */
size_t channel_extension(size_t size) {
    size_t offs;
    assert(!channel_created);
    while ((sizeof(Channel) + extension_size) % sizeof(void *) != 0) extension_size++;
    offs = sizeof(Channel) + extension_size;
    extension_size += size;
    return offs;
}

/*
 * Allocate a buffer to store the channel. This routine will take care of
 * allocating the various channel extensions defined using the
 * channel_extension() API.
 */
Channel * channel_alloc(void) {
    Channel * c = (Channel *)loc_alloc_zero(sizeof(Channel) + extension_size);
    channel_created = 1;
    return c;
}

/*
 * Release a buffer allocated using channel_alloc().
 */
void channel_free(Channel * c) {
    assert(channel_created);
    loc_free(c);
}

/*
 * Add a channel transport
 */
void add_channel_transport(const char * transportname, ChannelServerCreate create, ChannelConnect connect) {
     assert(transportname != NULL);
     assert(create != NULL);
     assert(connect != NULL);
     channel_transport_cnt++;
     channel_transport = (ChannelTransport *)loc_realloc(channel_transport, channel_transport_cnt * sizeof(ChannelTransport));
     channel_transport[channel_transport_cnt - 1].transportname = loc_strdup(transportname);
     channel_transport[channel_transport_cnt - 1].create = create;
     channel_transport[channel_transport_cnt - 1].connect = connect;
}

/*
 * Start communication of a newly created channel
 */
void channel_start(Channel * c) {
    trace(LOG_PROTOCOL, "Starting channel %#" PRIxPTR " %s", (uintptr_t)c, c->peer_name);
    assert(c->protocol != NULL);
    assert(c->state == ChannelStateStartWait);
    c->state = ChannelStateStarted;
    c->start_comm(c);
}

/*
 * Close communication channel
 */
void channel_close(Channel * c) {
    trace(LOG_PROTOCOL, "Closing channel %#" PRIxPTR " %s", (uintptr_t)c, c->peer_name);
    c->close(c, 0);
}
