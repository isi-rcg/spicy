/*******************************************************************************
* Copyright (c) 2017 Xilinx, Inc. and others.
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
*     Xilinx - initial API and implementation
*******************************************************************************/

#include <tcf/config.h>

#include <assert.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/client.h>

typedef struct Listener {
    ClientConnectionListener * func;
    void * args;
} Listener;

static Listener * listeners = NULL;
static unsigned listener_cnt = 0;
static unsigned listener_max = 0;

LINK client_connection_root = TCF_LIST_INIT(client_connection_root);

void client_connection_lock(ClientConnection * c) {
    if (c->lock) {
        c->lock(c);
    }
    else {
        c->lock_cnt++;
    }
}

void client_connection_unlock(ClientConnection * c) {
    if (c->unlock) {
        c->unlock(c);
    }
    else {
        assert(c->lock_cnt > 0);
        c->lock_cnt--;
        if (c->lock_cnt == 0) {
            if (c->dispose) c->dispose(c);
        }
    }
}

void notify_client_connected(ClientConnection * c) {
    unsigned i;
    client_connection_lock(c);
    list_add_last(&c->link_all, &client_connection_root);
    for (i = 0; i < listener_cnt; i++) {
        Listener * l = listeners + i;
        if (l->func->connected == NULL) continue;
        l->func->connected(c, l->args);
    }
}

void notify_client_disconnected(ClientConnection * c) {
    unsigned i;
    list_remove(&c->link_all);
    for (i = 0; i < listener_cnt; i++) {
        Listener * l = listeners + i;
        if (l->func->disconnected == NULL) continue;
        l->func->disconnected(c, l->args);
    }
    client_connection_unlock(c);
}

void add_client_connection_listener(ClientConnectionListener * l, void * args) {
    if (listener_cnt >= listener_max) {
        listener_max += 8;
        listeners = (Listener *)loc_realloc(listeners, listener_max * sizeof(Listener));
    }
    listeners[listener_cnt].func = l;
    listeners[listener_cnt].args = args;
    listener_cnt++;
}
