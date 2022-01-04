/*******************************************************************************
 * Copyright (c) 2012-2017 Xilinx, Inc. and others.
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

/*
 * Shutdown manager.
 */

#include <tcf/config.h>
#include <tcf/framework/trace.h>
#include <tcf/framework/events.h>
#include <tcf/framework/shutdown.h>

#include <assert.h>

static void (*shutdown_complete_handler)(void *shutdown_complete_arg);
static void * shutdown_complete_arg;
static int notify_pending;

LINK shutdown_active = TCF_LIST_INIT(shutdown_active);

static void notify_all(void * arg) {
    LINK * l;

    assert(notify_pending);
    notify_pending = 0;

    if (list_is_empty(&shutdown_active)) {
        /* All subsystems are stopped, cancel event dispatching and
         * notify shutdown initiator. */
        trace(LOG_SHUTDOWN, "shutdown complete");
        cancel_event_loop();
        shutdown_complete_handler(shutdown_complete_arg);
        return;
    }

    l = shutdown_active.next;
    while (l != &shutdown_active) {
        ShutdownInfo *obj = active2ShutdownInfo(l);
        l = l->next;
        trace(LOG_SHUTDOWN, "shutdown notify %p %d", obj, obj->state);
        if (obj->state == SHUTDOWN_STATE_NORMAL) {
            obj->state = SHUTDOWN_STATE_PENDING;
            obj->notify(obj);
        }
        else {
            assert(obj->state == SHUTDOWN_STATE_PENDING);
        }
    }
}

int shutdown_start(void (*handler)(void *arg), void * arg) {
    assert(is_dispatch_thread());
    assert(handler != NULL);
    assert(!notify_pending);

    trace(LOG_SHUTDOWN, "shutdown start");
    if (shutdown_complete_handler) {
        return 1;
    }
    shutdown_complete_handler = handler;
    shutdown_complete_arg = arg;
    notify_pending = 1;
    post_event(notify_all, NULL);
    return 0;
}

void shutdown_set_stopped(ShutdownInfo *obj) {
    assert(is_dispatch_thread());
    assert(obj != NULL);

    trace(LOG_SHUTDOWN, "shutdown_set_stopped %p", obj);
    if (obj->state == SHUTDOWN_STATE_STOPPED) {
        /* Noop */
        return;
    }

    assert(obj->state == SHUTDOWN_STATE_NORMAL ||
           obj->state == SHUTDOWN_STATE_PENDING);
    obj->state = SHUTDOWN_STATE_STOPPED;
    list_remove(&obj->link_active);
    if (shutdown_complete_handler && !notify_pending) {
        notify_pending = 1;
        post_event(notify_all, NULL);
    }
}

void shutdown_set_normal(ShutdownInfo *obj) {
    assert(is_dispatch_thread());
    assert(obj != NULL);

    trace(LOG_SHUTDOWN, "shutdown_set_normal %p", obj);
    if (obj->state == SHUTDOWN_STATE_NORMAL) {
        /* Noop */
        return;
    }

    if (obj->state == SHUTDOWN_STATE_STOPPED) {
        list_add_last(&obj->link_active, &shutdown_active);
    }
    else {
        assert(obj->state == SHUTDOWN_STATE_PENDING);
    }
    obj->state = SHUTDOWN_STATE_NORMAL;
}
