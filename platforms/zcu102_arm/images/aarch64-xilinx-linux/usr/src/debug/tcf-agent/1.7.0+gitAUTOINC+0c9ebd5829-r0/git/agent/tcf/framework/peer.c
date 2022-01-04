/*******************************************************************************
 * Copyright (c) 2007-2018 Wind River Systems, Inc. and others.
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
 * Implements peer server management.
 */

#include <tcf/config.h>
#include <stddef.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <tcf/framework/tcf.h>
#include <tcf/framework/peer.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/events.h>
#include <tcf/framework/protocol.h>
#include <tcf/framework/trace.h>

typedef struct PeersListener {
    peer_server_listener fnp;
    void * arg;
} PeersListener;

static PeerServer * peers;
static PeersListener * listeners;
static int listeners_cnt;
static int listeners_max;
static int stale_timer_active = 0;

static void notify_listeners(PeerServer * ps, int type) {
    int i;

    assert(ps->listed);
    trace(LOG_DISCOVERY, "Peer server change, id=%s, type=%d", ps->id, type);
    for (i = 0; i < listeners_cnt; i++) {
        listeners[i].fnp(ps, type, listeners[i].arg);
    }
}

static int is_same(PeerServer * a, PeerServer * b) {
    unsigned i;
    unsigned j;

    if (a->ind != b->ind) {
        return 0;
    }
    for (i = 0; i < a->ind; i++) {
        for (j = 0; j < b->ind; j++) {
            if (strcmp(a->list[i].name, b->list[j].name) == 0) {
                break;
            }
        }
        if (j >= b->ind) {
            /* Name from "a" not found in "b" */
            return 0;
        }
        if (strcmp(a->list[i].value, b->list[j].value) != 0) {
            return 0;
        }
    }
    return 1;
}

static void clear_stale_peers(void * x) {
    PeerServer ** sp = &peers;
    time_t timenow = time(NULL);

    assert(is_dispatch_thread());
    for (;;) {
        PeerServer * s = *sp;
        if (s == NULL) break;
        if (s->expiration_time <= timenow) {
            /* Delete stale entry */
            *sp = s->next;
            notify_listeners(s, PS_EVENT_REMOVED);
            s->listed = 0;
            peer_server_free(s);
        }
        else {
            sp = &s->next;
        }
    }
    if (peers != NULL) {
        post_event_with_delay(clear_stale_peers, NULL, PEER_DATA_REFRESH_PERIOD * 1000000);
    }
    else {
        stale_timer_active = 0;
    }
}

PeerServer * peer_server_alloc(void) {
    PeerServer * s = (PeerServer *)loc_alloc_zero(sizeof *s);

    s->max = 8;
    s->list = (PeerServerList *)loc_alloc(s->max * sizeof *s->list);
    return s;
}

void peer_server_addprop(PeerServer * s, const char * name, const char * value) {
    unsigned i;

    assert(!s->listed);
    if (strcmp(name, "ID") == 0) {
        loc_free(name);
        s->id = value;
        return;
    }
    for (i = 0; i < s->ind; i++) {
        if (strcmp(s->list[i].name, name) == 0) {
            loc_free(name);
            loc_free(s->list[i].value);
            s->list[i].value = value;
            return;
        }
    }
    if (s->ind == s->max) {
        s->max *= 2;
        s->list = (PeerServerList *)loc_realloc(s->list, s->max * sizeof *s->list);
    }
    s->list[s->ind].name = name;
    s->list[s->ind].value = value;
    s->ind++;
}

const char * peer_server_getprop(PeerServer * s, const char * name, const char * default_value) {
    unsigned i;

    for (i = 0; i < s->ind; i++) {
        if (strcmp(s->list[i].name, name) == 0) {
            return s->list[i].value;
        }
    }
    return default_value;
}

void peer_server_free(PeerServer * s) {
    assert(!s->listed);
    while (s->ind > 0) {
        s->ind--;
        loc_free(s->list[s->ind].name);
        loc_free(s->list[s->ind].value);
    }
    loc_free(s->list);
    if (s->id) loc_free(s->id);
    loc_free(s);
}

PeerServer * peer_server_add(PeerServer * n, unsigned stale_delta) {
    PeerServer ** sp = &peers;
    PeerServer * s;
    int type = PS_EVENT_ADDED;

    assert(!n->listed);
    assert(is_dispatch_thread());
    while ((s = *sp) != NULL) {
        if (strcmp(s->id, n->id) == 0) {
            if (((s->flags & PS_FLAG_LOCAL) && !(n->flags & PS_FLAG_LOCAL)) || is_same(s, n)) {
                /* Never replace local entries with discovered ones */
                s->expiration_time = time(NULL) + stale_delta;
                if (!(s->flags & PS_FLAG_LOCAL)) s->flags = n->flags;
                peer_server_free(n);
                notify_listeners(s, PS_EVENT_HEART_BEAT);
                return s;
            }
            *sp = s->next;
            s->listed = 0;
            peer_server_free(s);
            type = PS_EVENT_CHANGED;
            break;
        }
        sp = &s->next;
    }
    n->listed = 1;
    n->creation_time = time(NULL);
    n->expiration_time = n->creation_time + stale_delta;
    n->next = peers;
    peers = n;
    notify_listeners(n, type);
    if (!stale_timer_active) {
        stale_timer_active = 1;
        post_event_with_delay(clear_stale_peers, NULL, PEER_DATA_REFRESH_PERIOD * 1000000);
    }
    return n;
}

/* Find peer server based on ID */
PeerServer * peer_server_find(const char * id) {
    PeerServer * s;

    assert(is_dispatch_thread());
    for (s = peers; s != NULL; s = s->next) {
        if (strcmp(s->id, id) == 0) return s;
    }
    return NULL;
}

void peer_server_remove(const char *id) {
    PeerServer ** sp = &peers;
    PeerServer * s;

    assert(is_dispatch_thread());
    while ((s = *sp) != NULL) {
        if (strcmp(s->id, id) == 0) {
            *sp = s->next;
            notify_listeners(s, PS_EVENT_REMOVED);
            s->listed = 0;
            peer_server_free(s);
            break;
        }
        sp = &s->next;
    }
}

int peer_server_iter(peer_server_iter_fnp fnp, void * arg) {
    PeerServer * s = peers;
    int rval;

    assert(is_dispatch_thread());
    while (s != NULL) {
        rval = fnp(s, arg);
        if (rval != 0) {
            /* Abort iteration */
            return rval;
        }
        s = s->next;
    }
    return 0;
}

void peer_server_add_listener(peer_server_listener fnp, void * arg) {
    if (listeners_max == 0) {
        listeners_max = 4;
        listeners = (PeersListener *)loc_alloc(listeners_max * sizeof(PeersListener));
    }
    else if (listeners_cnt == listeners_max) {
        listeners_max *= 2;
        listeners = (PeersListener *)loc_realloc(listeners, listeners_max * sizeof(PeersListener));
    }
    listeners[listeners_cnt].fnp = fnp;
    listeners[listeners_cnt].arg = arg;
    listeners_cnt++;
}
