/*******************************************************************************
 * Copyright (c) 2012 Xilinx, Inc. and others.
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
 * Shutdown manager allows subsystems to register for notification to
 * support controlled shutdown of the TCF framework and its add-ons.
 *
 * A subsystem registers for notificatios by calling
 * shutdown_set_normal() with a subsystem specific ShutdownInfo object
 * containing a subsystem callback function, client data and subsystem
 * shutdown state.
 *
 * The callback function should initiate shutdown of the subsystem and
 * when completed call shutdown_set_stopped(), or if the subsystem
 * cannot be stopped because other subsystems actively using it to
 * complete their shutdown sequence the subsystem may choose request
 * futher notification callbacks by calling shutdown_set_normal()
 * again, however this should be done carefully to avoid infinite
 * loops.
 */

#ifndef D_shutdown
#define D_shutdown

#include <tcf/framework/link.h>

#define active2ShutdownInfo(A)  ((ShutdownInfo *)((char *)(A) - offsetof(ShutdownInfo, link_active)))

typedef struct ShutdownInfo ShutdownInfo;

typedef enum {
    /* Shutdown object is stopped and is not on shutdown_active list */
    SHUTDOWN_STATE_STOPPED = 0,

    /* Shutdown object needs to be notified if shutdown is started or
     * the state of other shutdown objects is changed */
    SHUTDOWN_STATE_NORMAL,

    /* Shutdown object down has been notified and will updates its
     * state when possible */
    SHUTDOWN_STATE_PENDING
} ShutdownState;

struct ShutdownInfo {
    /* Called to notify of initial shutdown request or state changes
     * of other shutdown objects */
    void (*notify)(ShutdownInfo *);

    /* Client specific data */
    void * client_data;

    /* State of object */
    ShutdownState state;

    /* List of active object */
    LINK link_active;
};

/* List of active shutdown objects, i.e. states NORMAL or PENDING */
extern LINK shutdown_active;

/*
 * Initiates shutdown process.  Returns true if shutdown is already started.
 *
 * handler - the function that should called when shutdown is complete.
 * arg - pointer to shutdown data.
 */
extern int shutdown_start(void (*handler)(void *arg), void * arg);

/*
 * Set state of shutdown of object to stopped.
 *
 * obj - affected ShutdownInfo object.
 */
extern void shutdown_set_stopped(ShutdownInfo *obj);

/*
 * Set state of shutdown of object to normal.
 *
 * obj - affected ShutdownInfo object.
 */
extern void shutdown_set_normal(ShutdownInfo *obj);

#endif /* D_shutdown */
