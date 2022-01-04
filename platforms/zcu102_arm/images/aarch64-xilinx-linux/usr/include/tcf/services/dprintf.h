/*******************************************************************************
 * Copyright (c) 2013 Xilinx, Inc. and others.
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
 * "dynamic printf" service
 */

#ifndef D_dprintf
#define D_dprintf

#include <tcf/config.h>

#if SERVICE_DPrintf

#include <tcf/framework/protocol.h>
#include <tcf/services/expressions.h>

#define dprintf_expression(fmt, args, cnt) dprintf_expression_ctx(NULL, fmt, args, cnt)

extern void dprintf_expression_ctx(Context * ctx, const char * fmt, Value * args, unsigned args_cnt);

extern void ini_dprintf_service(Protocol * p);

#endif /* SERVICE_DPrintf */

#endif /* D_dprintf */
