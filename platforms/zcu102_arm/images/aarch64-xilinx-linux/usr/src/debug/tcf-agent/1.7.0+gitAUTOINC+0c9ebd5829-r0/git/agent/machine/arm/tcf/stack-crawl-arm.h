/*******************************************************************************
 * Copyright (c) 2013-2018 Xilinx, Inc. and others.
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
 * This module implements stack crawl for ARM processor.
 */

#ifndef D_stack_crawl_arm
#define D_stack_crawl_arm

#include <tcf/config.h>
#include <tcf/framework/cpudefs.h>

/* ARMv7-A, ARMv7-R, AArch32 */
extern int crawl_stack_frame_arm(StackFrame * frame, StackFrame * down);

/* ARMv7-M */
extern int crawl_stack_frame_arm_v7m(StackFrame * frame, StackFrame * down);

#endif /* D_stack_crawl_arm */
