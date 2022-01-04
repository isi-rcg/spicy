/*******************************************************************************
 * Copyright (c) 2015, 2016 Wind River Systems, Inc. and others.
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
 * This module implements port forwarding service.
 */

#ifndef D_port_forward_proxy
#define D_port_forward_proxy

#include <tcf/framework/context.h>
#include <tcf/framework/protocol.h>

#if ENABLE_PortForwardProxy || SERVICE_PortServer
typedef struct PortServer PortServer;

typedef void (*PortConnectCallback)(struct PortServer * /* server */, void * /* hook data */);
typedef void (*PortDisconnectCallback)(struct PortServer * server/* server */, void * /* hook data */);
typedef void (*PortRecvCallback)(struct PortServer * server/* server */, char * /* buffer */, size_t * /* size */, size_t /* buffer size */, void * /* hook data */);

typedef struct PortAttribute {
    struct PortAttribute * next; /* next attribute */
    char * name; /* Attribute name */
    char * value; /* Attribute value as JSON string */
} PortAttribute;

typedef struct PortServerInfo {
    int is_udp;         /* server port is UDP or TCP? */
    u_short port;       /* server port number */
} PortServerInfo;


/* Create a port redirection. 'attrs' are disposed by the PortForward proxy
 * using loc_free(). */
extern PortServer * create_port_server(Channel * c, PortAttribute * attrs, PortConnectCallback connect_callback, PortDisconnectCallback disconnect_callback, PortRecvCallback recv_callback, void * callback_data);

/* Destroy a previously created port server */
extern int destroy_port_server(PortServer * server);

/* Get information about specified port server. The info structure must
 * have been preallocated */
extern int get_port_server_info(PortServer * server, PortServerInfo * info);

#if defined(SERVICE_PortServer)
extern void ini_port_server_service(const char * name_ext, Protocol *proto, TCFBroadcastGroup * bcg);
#endif
#endif

#endif /* D_port_forward_proxy */
