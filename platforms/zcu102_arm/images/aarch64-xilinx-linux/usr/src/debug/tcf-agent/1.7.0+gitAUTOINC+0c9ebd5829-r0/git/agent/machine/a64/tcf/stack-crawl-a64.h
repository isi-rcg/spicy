/*******************************************************************************
 * Copyright (c) 2014-2018 Xilinx, Inc. and others.
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
 * This module implements stack crawl for ARM AArch64.
 */

#ifndef D_stack_crawl_a64
#define D_stack_crawl_a64

#include <tcf/config.h>
#include <tcf/framework/cpudefs.h>

extern int crawl_stack_frame_a64(StackFrame * frame, StackFrame * down);

#endif /* D_stack_crawl_a64 */
