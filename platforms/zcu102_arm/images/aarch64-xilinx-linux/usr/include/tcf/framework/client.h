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

/*
* Abstract representation of a client connection.
* It includes both TCF channels and other connection types, for example, GDB connections.
* Service implementations can use this API to track client connection events.
*/

#ifndef D_client
#define D_client

#include <tcf/config.h>
#include <tcf/framework/link.h>

typedef struct ClientConnection ClientConnection;

struct ClientConnection {
    LINK link_all;
    unsigned lock_cnt;
    void (*lock)(ClientConnection *);
    void (*unlock)(ClientConnection *);
    void (*dispose)(ClientConnection *);
};

extern LINK client_connection_root;
#define all2client_connection(x) ((ClientConnection *)((char *)(x) - offsetof(ClientConnection, link_all)))

/*
* Lock a connection. A closed client connection will not be deallocated until it is unlocked.
* Each call of this function increments the connection reference counter.
*/
extern void client_connection_lock(ClientConnection *);

/*
* Unlock a connection.
* Each call of this function decrements the connection reference counter.
* If client connection is closed and reference count is zero, then the connection object is disposed.
*/
extern void client_connection_unlock(ClientConnection *);

/*
* Notify listeners about client connection events.
* These functions are called from connection implementation code.
*/
extern void notify_client_connected(ClientConnection * c);
extern void notify_client_disconnected(ClientConnection * c);

typedef struct ClientConnectionListener {
    void (*connected)(ClientConnection *, void *);
    void (*disconnected)(ClientConnection *, void *);
} ClientConnectionListener;

extern void add_client_connection_listener(ClientConnectionListener * l, void * args);

#endif /* D_client */
