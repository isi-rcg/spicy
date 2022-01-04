/*******************************************************************************
 * Copyright (c) 2007, 2014 Wind River Systems, Inc. and others.
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
 * This module implements tunneling of TCF messages to another target on behalf of a client
 * This service intended to be used when a client has no direct access to a target.
 */

#include <tcf/config.h>
#include <assert.h>
#include <string.h>
#include <tcf/framework/json.h>
#include <tcf/framework/proxy.h>
#include <tcf/framework/protocol.h>
#include <tcf/framework/trace.h>
#include <tcf/framework/errors.h>
#include <tcf/framework/exceptions.h>
#include <tcf/framework/myalloc.h>

typedef struct Proxy {
    Channel * c;
    Protocol * proto;
    int other;
    int instance;
} Proxy;

typedef struct RedirectInfo {
    Channel * host;
    char token[256];
} RedirectInfo;

static ChannelRedirectionListener redirection_listeners[16];
static int redirection_listeners_cnt = 0;

static ProxyLogFilterListener proxy_log_filter_listener;
static ProxyLogFilterListener2 proxy_log_filter_listener2;

static const char * channel_lock_msg = "Proxy lock";

static void proxy_update(Channel * c1, Channel * c2);

static void proxy_connecting(Channel * c) {
    int i;
    Proxy * target = (Proxy *)c->client_data;
    Proxy * host = target + target->other;

    assert(c == target->c);
    assert(target->other == -1);
    assert(c->state == ChannelStateStarted);
    assert(host->c->state == ChannelStateHelloReceived);

    for (i = 0; i < redirection_listeners_cnt; i++) {
        redirection_listeners[i](host->c, target->c);
    }

    target->c->disable_zero_copy = !host->c->out.supports_zero_copy;
    send_hello_message(target->c);

    trace(LOG_PROXY, "Proxy waiting Hello from target");
}

static void command_redirect_done(Channel * c, void * client_data, int error) {
    RedirectInfo * info = (RedirectInfo *)client_data;

    if (!is_channel_closed(info->host)) {
        int err = error;

        if (err == 0) proxy_update (info->host, c);

        write_stringz(&info->host->out, "R");
        write_stringz(&info->host->out, info->token);
        write_errno(&info->host->out, err);
        write_stream(&info->host->out, MARKER_EOM);

#if ENABLE_Trace
        if (log_mode & LOG_TCFLOG) {
            Proxy * proxy = (Proxy *)info->host->client_data;
            trace(LOG_TCFLOG, "%d: R %s %s", proxy->instance, info->token, errno_to_str(err));
        }
#endif
    }

    channel_unlock_with_msg(info->host, channel_lock_msg);
    loc_free(info);
}

static void read_peer_attr(InputStream * inp, const char * name, void * x) {
    peer_server_addprop((PeerServer *)x, loc_strdup(name), json_read_alloc_string(inp));
}

static void command_locator_redirect(char * token, Channel * c, void * args) {
    char id[256];
    PeerServer * ps = NULL;
    Channel * target = (Channel *)args;
    RedirectInfo * info = (RedirectInfo *)loc_alloc_zero(sizeof(RedirectInfo));

    if (peek_stream(&c->inp) == '{') {
        ps = peer_server_alloc();
        json_read_struct(&c->inp, read_peer_attr, ps);
    }
    else {
        json_read_string(&c->inp, id, sizeof(id));
    }

    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

#if ENABLE_Trace
    if (log_mode & LOG_TCFLOG) {
        Proxy * proxy = (Proxy *)c->client_data;
        if (ps != NULL) {
            char * server_properties = channel_peer_to_json(ps);
            trace(LOG_TCFLOG, "%d: C %s Locator redirect %s", proxy->instance, token, server_properties);
            loc_free(server_properties);
        }
        else {
            trace(LOG_TCFLOG, "%d: C %s Locator redirect %s", proxy->instance, token, id);
        }
    }
#endif

    channel_lock_with_msg(c, channel_lock_msg);
    info->host = c;
    strlcpy(info->token, token, sizeof(info->token));

    /* Send the redirect command to the next TCF entity */

    if (ps != NULL) {
        send_redirect_command_by_props(target, ps, command_redirect_done, info);
    }
    else {
        send_redirect_command_by_id(target, id, command_redirect_done, info);
    }

    if (ps != NULL) peer_server_free(ps);
}

static void proxy_connected(Channel * c) {
    int i;
    Proxy * target = (Proxy *)c->client_data;
    Proxy * host = target + target->other;

    assert(target->c == c);
    if (target->other == 1) {
        /* We get here after sending hello to host */
        return;
    }
    assert(c->state == ChannelStateConnected);
    assert(host->c->state == ChannelStateHelloReceived);

    host->c->disable_zero_copy = !target->c->out.supports_zero_copy;

    trace(LOG_PROXY, "Proxy connected, target services:");
    for (i = 0; i < target->c->peer_service_cnt; i++) {
        char * nm = target->c->peer_service_list[i];
        trace(LOG_PROXY, "    %s", nm);
        if (strcmp(nm, "ZeroCopy") == 0) continue;
        protocol_get_service(host->proto, nm);
    }

    /*
     * Intercept the Locator.redirect command to update the local list of
     * services with the ones from the next TCF entity (agent), and send a
     * consolidate list to the previous TCF entity (client). This is
     * required in the case of more than one server between the client and
     * the agent.
     */

    add_command_handler2(host->c->protocol, "Locator", "redirect",
                         command_locator_redirect, target->c);

    for (i = 0; i < redirection_listeners_cnt; i++) {
        redirection_listeners[i](host->c, target->c);
    }

    send_hello_message(host->c);
}

static void proxy_disconnected(Channel * c) {
    Proxy * proxy = (Proxy *)c->client_data;

    assert(c == proxy->c);
    if (proxy[proxy->other].c->state == ChannelStateDisconnected) {
        trace(LOG_PROXY, "Proxy disconnected");
        if (proxy->other == -1) proxy--;
        broadcast_group_free(c->bcg);
        assert(proxy[0].c->bcg == NULL);
        assert(proxy[1].c->bcg == NULL);
        proxy[0].c->client_data = NULL;
        proxy[1].c->client_data = NULL;
        protocol_release(proxy[0].proto);
        protocol_release(proxy[1].proto);
        channel_unlock_with_msg(proxy[0].c, channel_lock_msg);
        channel_unlock_with_msg(proxy[1].c, channel_lock_msg);
        loc_free(proxy);
    }
    else {
        channel_close(proxy[proxy->other].c);
    }
}

#if ENABLE_Trace

static char log_buf[1024];
static size_t log_pos = 0;

static void log_chr(int c) {
    if (log_pos + 2 < sizeof log_buf) log_buf[log_pos++] = (char)c;
}

static void log_str(const char * s) {
    char c;
    while ((c = *s++) != '\0') {
        if (log_pos + 2 < sizeof log_buf) log_buf[log_pos++] = c;
    }
}

static void log_byte_func(int i) {
    if (i >= ' ' && i < 127) {
        /* Printable ASCII  */
        log_chr(i);
    }
    else if (i == 0) {
        log_chr(' ');
    }
    else if (i > 0) {
        char buf[16];
        snprintf(buf, sizeof buf, "\\x%02x", i);
        log_str(buf);
    }
    else if (i == MARKER_EOM) {
        log_str("<eom>");
    }
    else if (i == MARKER_EOS) {
        log_str("<eom>");
    }
    else {
        log_str("<?>");
    }
}

#define log_byte(b) { if (log_mode & LOG_TCFLOG) log_byte_func(b); }

static int log_start(Proxy * proxy, char ** argv, int argc, int * limit) {
    int i;
    int res = PROXY_FILTER_NOT_FILTERED;
    log_pos = 0;
    *limit = 0;

    if (log_mode & LOG_TCFLOG) {
        if (proxy_log_filter_listener) {
            res = proxy_log_filter_listener(proxy->c, proxy[proxy->other].c, argc, argv);
            if (res) return PROXY_FILTER_FILTERED;
        }
        else if (proxy_log_filter_listener2) {
            res = proxy_log_filter_listener2(proxy->c, proxy[proxy->other].c, argc, argv, limit);
            /* If we have PROXY_FILTER_LIMIT, we want to see --> or <-- */
            if (res == PROXY_FILTER_FILTERED) return res;
        }
        log_str(proxy->other > 0 ? "---> " : "<--- ");
        for (i = 0; i < argc; i++) {
            log_str(argv[i]);
            log_chr(' ');
        }
    }
    return res;
}

static void log_flush(Proxy * proxy) {
    if (log_mode & LOG_TCFLOG) {
        log_chr(0);
        trace(LOG_TCFLOG, "%d: %s", proxy->instance, log_buf);
    }
}

#else

#define log_start(a, b, c, d) 0
#define log_byte(a) do {} while(0)
#define log_flush(a) do {} while(0)

#endif

static void proxy_default_message_handler(Channel * c, char ** argv, int argc) {
    /* TODO: if proxy is connected to itself, it can deadlock when retransmitting a long message */
    Proxy * proxy = (Proxy *)c->client_data;
    Channel * otherc = proxy[proxy->other].c;
    InputStream * inp = &c->inp;
    OutputStream * out = &otherc->out;
    int i = 0;
    int filtered = 0;
    int filter_cnt = 0;
    int limit = 0;

    assert(c == proxy->c);
    assert(argc > 0 && strlen(argv[0]) == 1);
    if (proxy[proxy->other].c->state == ChannelStateDisconnected) return;

    if (argv[0][0] == 'C') {
        write_stringz(out, argv[0]);
        /* Prefix token with 'R'emote to distinguish from locally generated commands */
        write_stream(out, 'R');
        i = 1;
    }
    else if (argv[0][0] == 'R' || argv[0][0] == 'P' || argv[0][0] == 'N') {
        if (argv[1][0] != 'R') {
            trace(LOG_ALWAYS, "Reply with unexpected token: %s", argv[1]);
            exception(ERR_PROTOCOL);
        }
        argv[1]++;
    }

    while (i < argc) write_stringz(out, argv[i++]);

    filtered = log_start(proxy, argv, argc, &limit);
    /* Copy body of message */
    do {
        if (out->supports_zero_copy &&
#if ENABLE_Trace
               (log_mode & LOG_TCFLOG) == 0 &&
#endif
                inp->end - inp->cur >= 0x100) {
            write_block_stream(out, (char *)inp->cur, inp->end - inp->cur);
            inp->cur = inp->end;
        }
        else {
            i = read_stream(inp);
            if ((filtered == PROXY_FILTER_NOT_FILTERED) ||
                (filtered == PROXY_FILTER_LIMIT && filter_cnt < limit)) {
                log_byte(i);
                filter_cnt++;
#if ENABLE_Trace
                if (filtered == PROXY_FILTER_LIMIT && filter_cnt == limit) {
                    log_str("...");
                    /* Don't quit the loop, we need to write the entire message */
                }
#endif
            }
            write_stream(out, i);
        }
    }
    while (i != MARKER_EOM && i != MARKER_EOS);
    if (filtered == PROXY_FILTER_NOT_FILTERED ||
        filtered == PROXY_FILTER_LIMIT) log_flush(proxy);
}

static void proxy_update(Channel * c1, Channel * c2) {
    Proxy * proxy;

    /* c1 is host */
    assert (c1->state == ChannelStateConnected);
    /* c2 is target */
    assert (c2->state == ChannelStateHelloSent);

    /* Check that both channels form a proxy */
    assert(proxy_get_host_channel(c1) == c1);
    assert(proxy_get_target_channel(c1) == c2);
    assert(proxy_get_host_channel(c2) == c1);
    assert(proxy_get_target_channel(c2) == c2);

    proxy = (Proxy *)c1->client_data;
    if (proxy->other == -1) proxy--;

    /* Create new protocol object for the host channel.  Do this
     * before call to notify_channel_closed() below to be consistent
     * with proxy_create(). */
    proxy[0].proto = protocol_alloc();

    /* Update the state of the host channel to react correctly on the
     * hello message from the redirected target and notify listeners
     * of the new state to give them a chance to cleanup and be ready
     * for the upcoming channel redirection listener callback in
     * proxy_connected() when target hello message arrives. */
    c1->state = ChannelStateHelloReceived;
    notify_channel_closed(c1);

    /* Replace protocol object for the host channel to make sure it
     * does not contain any services or command handlers from before
     * the redirect. */
    protocol_release(c1->protocol);
    c1->protocol = proxy[0].proto;
    set_default_message_handler(proxy[0].proto, proxy_default_message_handler);
}

void proxy_create(Channel * c1, Channel * c2) {
    TCFBroadcastGroup * bcg = broadcast_group_alloc();
    Proxy * proxy = (Proxy *)loc_alloc_zero(2 * sizeof *proxy);
    int i;
    int c2_peer_service_cnt = 0;
    char ** c2_peer_service_list = NULL;
    int c2_connected = 0;

    static int instance;

    assert(c1->state == ChannelStateRedirectReceived);
    assert(c2->state == ChannelStateStartWait || c2->state == ChannelStateRedirectReceived);

    /* Allow redirections of two channels that are already connected */
    if (c2->state == ChannelStateRedirectReceived) c2_connected = 1;

    /* Host */
    channel_lock_with_msg(c1, channel_lock_msg);
    proxy[0].c = c1;
    proxy[0].proto = protocol_alloc();
    proxy[0].other = 1;
    proxy[0].instance = instance;

    /* Target */
    channel_lock_with_msg(c2, channel_lock_msg);
    proxy[1].c = c2;
    proxy[1].proto = protocol_alloc();
    proxy[1].other = -1;
    proxy[1].instance = instance++;

    trace(LOG_PROXY, "Proxy created, host services:");
    for (i = 0; i < c1->peer_service_cnt; i++) {
        char * nm = c1->peer_service_list[i];
        trace(LOG_PROXY, "    %s", nm);
        if (strcmp(nm, "ZeroCopy") == 0) continue;
        protocol_get_service(proxy[1].proto, nm);
    }

    if (c2_connected) {
        /* Save the list of target services to restore them once the channel
         * is opened again. */
        c2_peer_service_list = (char **)loc_alloc(c2->peer_service_cnt * sizeof(char *));
        c2_peer_service_cnt= c2->peer_service_cnt;
        for (i = 0; i < c2->peer_service_cnt; i++) {
            char * nm = c2->peer_service_list[i];
            c2_peer_service_list[i] = loc_strdup(nm);
            if (strcmp(nm, "ZeroCopy") == 0) continue;
            protocol_get_service(proxy[0].proto, nm);
        }
    }
    c1->state = ChannelStateHelloReceived;
    notify_channel_closed(c1);
    protocol_release(c1->protocol);
    c1->client_data = NULL;
    if (!c2_connected) {
        assert(c2->protocol == NULL);
    }
    else {
        c2->state = ChannelStateHelloReceived;
        notify_channel_closed(c2);
        protocol_release(c2->protocol);
        c2->client_data = NULL;
    }

    c1->connecting = proxy_connecting;
    c1->connected = proxy_connected;
    c1->disconnected = proxy_disconnected;
    c1->client_data = proxy;
    c1->protocol = proxy[0].proto;
    set_default_message_handler(proxy[0].proto, proxy_default_message_handler);

    c2->connecting = proxy_connecting;
    c2->connected = proxy_connected;
    c2->disconnected = proxy_disconnected;
    c2->client_data = proxy + 1;
    c2->protocol = proxy[1].proto;
    set_default_message_handler(proxy[1].proto, proxy_default_message_handler);

    channel_set_broadcast_group(c1, bcg);
    channel_set_broadcast_group(c2, bcg);
    if (c2_connected) {
        /* restore the target peer services list */
        c2->peer_service_cnt = c2_peer_service_cnt;
        c2->peer_service_list = c2_peer_service_list;

        /* emulate proxy_connecting() code */
        for (i = 0; i < redirection_listeners_cnt; i++) {
            redirection_listeners[i](c1, c2);
        }
        c2->disable_zero_copy = !c1->out.supports_zero_copy;

        /* indicate target channel is connected */
        send_hello_message(c2);
    }
    else {
        channel_start(c2);
    }
}

Channel * proxy_get_host_channel(Channel * c) {
    Proxy * proxy = (Proxy *)c->client_data;

    if (c->connecting != proxy_connecting || proxy == NULL || c != proxy->c) return NULL;
    if (proxy->other == -1) proxy--;
    return proxy[0].c;
}

Channel * proxy_get_target_channel(Channel * c) {
    Proxy * proxy = (Proxy *)c->client_data;

    if (c->connecting != proxy_connecting || proxy == NULL || c != proxy->c) return NULL;
    if (proxy->other == -1) proxy--;
    return proxy[1].c;
}

void add_channel_redirection_listener(ChannelRedirectionListener listener) {
    assert(redirection_listeners_cnt < (int)(sizeof(redirection_listeners) / sizeof(ChannelRedirectionListener)));
    redirection_listeners[redirection_listeners_cnt++] = listener;
}

ProxyLogFilterListener set_proxy_log_filter_listener(ProxyLogFilterListener listener) {
    ProxyLogFilterListener old = proxy_log_filter_listener;
    proxy_log_filter_listener = listener;
    return old;
}

ProxyLogFilterListener2 set_proxy_log_filter_listener2(ProxyLogFilterListener2 listener) {
    ProxyLogFilterListener2 old = proxy_log_filter_listener2;
    proxy_log_filter_listener2 = listener;
    return old;
}
