/*******************************************************************************
 * Copyright (c) 2016 Wind River Systems, Inc. and others.
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
 * WebSocket channel interface based on libwebsockets
 */

#ifndef D_channel_lws
#define D_channel_lws

#include <tcf/config.h>

#if ENABLE_LibWebSockets
#include <tcf/framework/channel.h>

extern void channel_lws_get_properties(Channel * channel, char *** prop_names, char *** prop_values, unsigned * prop_cnt);
extern void ini_channel_lws(void);

#endif /* ENABLE_LibWebSockets */
#endif /* D_channel_lws */
