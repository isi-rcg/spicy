/*******************************************************************************
 * Copyright (c) 2018 Xilinx, Inc. and others.
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
 * This module supports execution of TCF commands over HTTP connection.
 */

#ifndef D_http_tcf
#define D_http_tcf

#include <tcf/config.h>

#include <tcf/framework/channel.h>

extern void http_connection_closed(OutputStream * out);
extern ChannelServer * ini_http_tcf(int sock, PeerServer * ps);

#endif /* D_http_tcf */
