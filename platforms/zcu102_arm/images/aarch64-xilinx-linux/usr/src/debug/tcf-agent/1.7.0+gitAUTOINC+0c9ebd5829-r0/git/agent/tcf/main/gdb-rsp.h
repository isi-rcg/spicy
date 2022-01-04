/*******************************************************************************
 * Copyright (c) 2016 Xilinx, Inc. and others.
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


#ifndef D_gdb_rsp
#define D_gdb_rsp

#include <tcf/config.h>

#if !defined(ENABLE_GdbRemoteSerialProtocol)
#  define ENABLE_GdbRemoteSerialProtocol 0
#endif

#if ENABLE_GdbRemoteSerialProtocol

/*
 * Create and start GDB Remote Serial Protocol server listening on the given port.
 * 'conf' format is <port>:<isa>
 * Return 0 on success, return -1 and set errno if error.
 */
extern int ini_gdb_rsp(const char * conf);

#endif /* ENABLE_GdbRemoteSerialProtocol */
#endif /* D_gdb_rsp */
