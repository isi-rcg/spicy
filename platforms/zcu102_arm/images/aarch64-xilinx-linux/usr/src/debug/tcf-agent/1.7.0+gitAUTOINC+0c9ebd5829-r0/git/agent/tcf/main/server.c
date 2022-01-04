/*******************************************************************************
 * Copyright (c) 2007, 2012 Wind River Systems, Inc. and others.
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
 * Server initialization code.
 */

#include <tcf/config.h>

#include <tcf/framework/myalloc.h>
#include <tcf/framework/exceptions.h>
#include <tcf/main/server.h>

static void channel_new_connection(ChannelServer * serv, Channel * c) {
    protocol_reference(serv->protocol);
    c->protocol = serv->protocol;
    channel_set_broadcast_group(c, serv->bcg);
    channel_start(c);
}

int ini_server(const char * url, Protocol * p, TCFBroadcastGroup * b) {
    ChannelServer * serv = NULL;
    PeerServer * ps = NULL;
    Trap trap;

    if (!set_trap(&trap)) {
        if (ps != NULL) peer_server_free(ps);
        errno = trap.error;
        return -1;
    }

    ps = channel_peer_from_url(url);
    if (ps == NULL) str_exception(ERR_OTHER, "Invalid server URL");
    peer_server_addprop(ps, loc_strdup("ServiceManagerID"), loc_strdup(get_service_manager_id(p)));
#if (ENABLE_SymbolsProxy && !SERVICE_Symbols) || (ENABLE_LineNumbersProxy && !SERVICE_LineNumbers)
    if (peer_server_getprop(ps, "NeedSyms", NULL) == NULL) {
        peer_server_addprop(ps, loc_strdup("NeedSyms"), loc_strdup("1"));
    }
#endif
    serv = channel_server(ps);
    if (serv == NULL) exception(errno);
    serv->new_conn = channel_new_connection;
    serv->protocol = p;
    serv->bcg = b;

    clear_trap(&trap);
    return 0;
}
