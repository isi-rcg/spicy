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

/* offset to be applied to the PC after a software trap */
#define TRAP_OFFSET 0

#if defined(__linux__)

#include <elf.h>
#include <tcf/framework/mdep-ptrace.h>

#ifndef PTRACE_GETREGSET
#  define PTRACE_GETREGSET 0x4204
#endif

#ifndef PTRACE_SETREGSET
#  define PTRACE_SETREGSET 0x4205
#endif

#if !defined(NT_ARM_TLS)
#define NT_ARM_TLS 0x401
#endif

#define MDEP_UseREGSET

struct regset_gp {
    uint64_t regs[31];
    uint64_t sp;
    uint64_t pc;
    uint64_t pstate;
    uint64_t orig_x0;
    uint64_t syscallno;
    uint64_t orig_addr_limit;
};

struct regset_fp_reg {
    uint8_t bytes[16];
};

struct regset_fp {
    struct regset_fp_reg vregs[32];
    uint32_t fpsr;
    uint32_t fpcr;
};

struct regset_gp32 {
    uint32_t regs[16];
    uint32_t cpsr;
    uint32_t orig_r0;
};

struct regset_extra {
    struct regset_gp32 gp32;
    uint64_t tls;
};

#define REGSET_GP NT_PRSTATUS
#define REGSET_FP NT_FPREGSET

#define MDEP_OtherRegisters struct regset_extra

extern unsigned get_arm_word_size(Context * ctx);
#define MDEP_WordSize(ctx) get_arm_word_size(ctx)

#endif
