/*******************************************************************************
 * Copyright (c) 2007, 2016 Wind River Systems, Inc. and others.
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
 * TCP channel interface
 */

#ifndef D_channel_tcp
#define D_channel_tcp

#include <tcf/framework/channel.h>

/*
 * Start TCP (Internet) channel listener.
 * On error returns NULL and sets errno.
 */
extern ChannelServer * channel_tcp_server(PeerServer * server);

/*
 * Start TCP (Unix domain) channel listener.
 * On error returns NULL and sets errno.
 */
extern ChannelServer * channel_unix_server(PeerServer * server);

/*
 * Connect client side over TCP (Internet domain).
 *
 * On error returns NULL and sets errno.
 */
extern void channel_tcp_connect(PeerServer * server, ChannelConnectCallBack callback, void * callback_args);

/*
 * Connect client side over TCP (Unix domain).
 * On error returns NULL and sets errno.
 */
extern void channel_unix_connect(PeerServer * server, ChannelConnectCallBack callback, void * callback_args);

/*
 * Re-scan network interfaces for any changes in IP address, mask, etc.
 * Normally, network interfaces are scanned periodically few times a minute.
 * Calling this function requests immediate scan.
 */
extern void channel_tcp_network_changed(void);

/*
 * Generate SSL certificate to be used with SSL channels.
 */
extern void generate_ssl_certificate(void);

/*
 * Initialize channel TCP library.
 */
extern void ini_channel_tcp(void);

#endif /* D_channel_tcp */
