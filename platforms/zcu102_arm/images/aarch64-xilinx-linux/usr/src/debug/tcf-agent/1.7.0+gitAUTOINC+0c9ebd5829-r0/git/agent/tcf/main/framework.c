/*******************************************************************************
 * Copyright (c) 2016-2018 Wind River Systems, Inc. and others.
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
 * Framework initialization code.
 */

#include <tcf/config.h>

#include <tcf/framework/mdep.h>
#include <tcf/framework/trace.h>
#include <tcf/framework/events.h>
#include <tcf/framework/asyncreq.h>
#include <tcf/framework/channel_lws.h>
#include <tcf/framework/channel_tcp.h>
#include <tcf/framework/channel_pipe.h>
#include <tcf/http/http.h>

#include <tcf/main/framework.h>
#include <tcf/main/framework-ext.h>

void ini_framework(void) {
    ini_mdep();
    ini_trace();
    ini_events_queue();
    ini_asyncreq();
    ini_channel_tcp();
    ini_channel_pipe();
#if ENABLE_LibWebSockets
    ini_channel_lws();
#endif
#if ENABLE_HttpServer
    ini_http();
#endif
    ini_ext_framework();
}
