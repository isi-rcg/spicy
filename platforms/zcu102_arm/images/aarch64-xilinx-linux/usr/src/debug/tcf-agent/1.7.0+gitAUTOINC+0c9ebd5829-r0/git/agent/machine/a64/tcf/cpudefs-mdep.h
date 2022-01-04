/*******************************************************************************
 * Copyright (c) 2015-2020 Xilinx, Inc. and others.
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
 * This module provides CPU specific definitions for ARM 64.
 */

#if defined (__aarch64__)

#include <tcf/regset.h>

extern RegisterDefinition * regs_index;
extern unsigned char BREAK_INST[4];

#if !defined(ENABLE_ini_cpudefs_mdep)
#define ENABLE_ini_cpudefs_mdep 1
extern void ini_cpudefs_mdep(void);
#endif

#if !defined(ENABLE_add_cpudefs_disassembler)
#define ENABLE_add_cpudefs_disassembler 1
extern void add_cpudefs_disassembler(Context * cpu_ctx);
#endif


/*******************************************************************************/
/* Support for AArch32 Arm and Thumb execution modes                           */

#define ENABLE_cpu_alt_isa_mode 1

extern int is_alt_isa_thread(Context * ctx);

extern uint8_t * get_alt_break_instruction(Context * ctx, size_t * size);
extern RegisterDefinition * get_alt_reg_definitions(Context * ctx);
extern RegisterDefinition * get_alt_reg_by_id(Context * ctx, unsigned id, RegisterIdScope * scope);

/*******************************************************************************/

#endif
