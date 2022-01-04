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
 * Implements auto-discovery and Locator service.
 */

#include <tcf/config.h>

#include <stddef.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <tcf/framework/proxy.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/events.h>
#include <tcf/framework/trace.h>
#include <tcf/framework/errors.h>
#include <tcf/framework/exceptions.h>
#include <tcf/framework/json.h>
#include <tcf/framework/peer.h>
#include <tcf/services/discovery.h>
#include <tcf/services/discovery_udp.h>

#if SERVICE_Locator

static const char * LOCATOR = "Locator";
static int peer_cnt = 0;

static int write_peer_properties(PeerServer * ps, void * arg) {
    unsigned i;
    OutputStream * out = (OutputStream *)arg;

    if (peer_cnt > 0) write_stream(out, ',');
    write_stream(out, '{');
    json_write_string(out, "ID");
    write_stream(out, ':');
    json_write_string(out, ps->id);
    write_stream(out, ',');
    json_write_string(out, "Flags");
    write_stream(out, ':');
    json_write_ulong(out, ps->flags);
    for (i = 0; i < ps->ind; i++) {
        write_stream(out, ',');
        json_write_string(out, ps->list[i].name);
        write_stream(out, ':');
        json_write_string(out, ps->list[i].value);
    }
    write_stream(out, '}');
    peer_cnt++;
    return 0;
}

static void command_sync(char * token, Channel * c) {
    json_test_char(&c->inp, MARKER_EOM);
    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_errno(&c->out, 0);
    write_stream(&c->out, MARKER_EOM);
}

typedef struct RedirectInfo {
    Channel * channel;
    char token[256];
} RedirectInfo;

static void connect_done(void * args, int error, Channel * c2) {
    RedirectInfo * info = (RedirectInfo *)args;
    Channel * c1 = info->channel;

    if (!is_channel_closed(c1)) {
        assert(c1->state == ChannelStateRedirectReceived);
        if (!error) {
            proxy_create(c1, c2);
        }
        else {
            c1->state = ChannelStateConnected;
        }
        write_stringz(&c1->out, "R");
        write_stringz(&c1->out, info->token);
        write_errno(&c1->out, error);
        write_stream(&c1->out, MARKER_EOM);
    }
    else if (!error) {
        channel_close(c2);
    }
    channel_unlock_with_msg(c1, LOCATOR);
    loc_free(info);
}

static void read_peer_attr(InputStream * inp, const char * name, void * x) {
    peer_server_addprop((PeerServer *)x, loc_strdup(name), json_read_alloc_string(inp));
}

static void command_redirect(char * token, Channel * c) {
    PeerServer * ps;
    int free_ps = 0;

    assert(c->state == ChannelStateConnected);
    if (json_peek(&c->inp) == '{') {
        ps = peer_server_alloc();
        json_read_struct(&c->inp, read_peer_attr, ps);
        free_ps = 1;
    }
    else {
        char id[256];
        json_read_string(&c->inp, id, sizeof(id));
        ps = peer_server_find(id);
    }
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    if (ps != NULL) {
        RedirectInfo * info = (RedirectInfo *)loc_alloc_zero(sizeof(RedirectInfo));
        channel_lock_with_msg(c, LOCATOR);
        c->state = ChannelStateRedirectReceived;
        info->channel = c;
        strlcpy(info->token, token, sizeof(info->token));
        channel_connect(ps, connect_done, info);
    }
    else {
        write_stringz(&c->out, "R");
        write_stringz(&c->out, token);
        write_errno(&c->out, ERR_UNKNOWN_PEER);
        write_stream(&c->out, MARKER_EOM);
    }
    if (free_ps) peer_server_free(ps);
}

static void command_get_peers(char * token, Channel * c) {
    json_test_char(&c->inp, MARKER_EOM);

    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_errno(&c->out, 0);
    write_stream(&c->out, '[');
    peer_cnt = 0;
    peer_server_iter(write_peer_properties, &c->out);
    write_stream(&c->out, ']');
    write_stream(&c->out, 0);
    write_stream(&c->out, MARKER_EOM);
}

static void command_get_agent_id(char * token, Channel * c) {
    json_test_char(&c->inp, MARKER_EOM);
    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_errno(&c->out, 0);
    json_write_string(&c->out, get_agent_id());
    write_stream(&c->out, 0);
    write_stream(&c->out, MARKER_EOM);
}

static void peer_change_event(PeerServer * ps, int type, void * arg) {
    OutputStream * out = (OutputStream *)arg;

    if ((ps->flags & PS_FLAG_DISCOVERABLE) == 0) return;
    write_stringz(out, "E");
    write_stringz(out, LOCATOR);
    switch (type) {
    case PS_EVENT_ADDED:
        write_stringz(out, "peerAdded");
        peer_cnt = 0;
        write_peer_properties(ps, out);
        break;
    case PS_EVENT_CHANGED:
        write_stringz(out, "peerChanged");
        peer_cnt = 0;
        write_peer_properties(ps, out);
        break;
    case PS_EVENT_HEART_BEAT:
        write_stringz(out, "peerHeartBeat");
        json_write_string(out, ps->id);
        break;
    case PS_EVENT_REMOVED:
        write_stringz(out, "peerRemoved");
        json_write_string(out, ps->id);
        break;
    }
    write_stream(out, 0);
    write_stream(out, MARKER_EOM);
}

void ini_locator_service(Protocol * p, TCFBroadcastGroup * bcg) {
    assert(is_dispatch_thread());
    peer_server_add_listener(peer_change_event, &bcg->out);
    add_command_handler(p, LOCATOR, "sync", command_sync);
    add_command_handler(p, LOCATOR, "redirect", command_redirect);
    add_command_handler(p, LOCATOR, "getPeers", command_get_peers);
    add_command_handler(p, LOCATOR, "getAgentID", command_get_agent_id);
}
#endif /* SERVICE_Locator */

void discovery_start(void) {
#if ENABLE_Discovery
    discovery_start_udp();
#endif
}

void discovery_stop(void) {
#if ENABLE_Discovery
    discovery_stop_udp();
#endif
}
