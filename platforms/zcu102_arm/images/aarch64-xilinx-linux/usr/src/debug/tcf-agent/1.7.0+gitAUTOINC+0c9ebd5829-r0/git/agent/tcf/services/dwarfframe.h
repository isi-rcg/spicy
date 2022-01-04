/*******************************************************************************
 * Copyright (c) 2007, 2011 Wind River Systems, Inc. and others.
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
 * This module implements handling of .debug_frame and .eh_frame sections.
 *
 * Functions in this module use exceptions to report errors, see exceptions.h
 */

#ifndef D_dwarfframe
#define D_dwarfframe

#include <tcf/config.h>

#if ENABLE_ELF && ENABLE_DebugContext

#include <tcf/services/dwarfcache.h>
#include <tcf/services/symbols.h>

/*
 * Lookup stack tracing information in ELF file, in .debug_frame and .eh_frame sections.
 *
 * Given register values in one frame, stack tracing information allows to calculate
 * frame address and register values in the next frame.
 *
 * When function returns, dwarf_stack_trace_fp contains commands to calculate frame address,
 * and dwarf_stack_trace_regs contains commands to calculate register values.
 * In case of error reading frame data, the function throws an exception.
 *
 * 'ip' is link-time instruction address.
 */
extern void get_dwarf_stack_frame_info(Context * ctx, ELF_File * file, ELF_Section * sec, U8_T ip);

extern U8_T dwarf_stack_trace_addr;
extern U8_T dwarf_stack_trace_size;

extern StackFrameRegisterLocation * dwarf_stack_trace_fp;

extern int dwarf_stack_trace_regs_cnt;
extern StackFrameRegisterLocation ** dwarf_stack_trace_regs;

#endif /* ENABLE_ELF && ENABLE_DebugContext */

#endif /* D_dwarfframe */
