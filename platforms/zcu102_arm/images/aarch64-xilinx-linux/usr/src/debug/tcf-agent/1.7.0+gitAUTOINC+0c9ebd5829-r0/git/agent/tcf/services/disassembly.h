/*******************************************************************************
 * Copyright (c) 2013-2019 Xilinx, Inc. and others.
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

#ifndef D_disassembly
#define D_disassembly

#include <tcf/config.h>
#include <tcf/framework/cpudefs.h>
#include <tcf/framework/protocol.h>
#include <tcf/framework/context.h>

typedef struct {
    const char * text;
    ContextAddress size;
    int incomplete;
} DisassemblyResult;

/*
 * Parameters to a disassembler.
 */
typedef struct DisassemblerParams {
    Context * ctx;      /* Debug context to be used to lookup symbols */
    int big_endian;     /* 0 - little endian, 1 -  big endian */
    int simplified;     /* If true, simplified mnemonics are specified */
    int pseudo_instr;   /* If true, pseudo-instructions are requested */
    void * state;
} DisassemblerParams;

typedef DisassemblyResult * Disassembler(uint8_t * /* code */, ContextAddress /* addr */,
                                ContextAddress /* size */, DisassemblerParams * /* param */);

#if SERVICE_Disassembly

extern void add_disassembler(Context * ctx, const char * isa, Disassembler disassembler);

extern Disassembler * find_disassembler(Context * ctx, const char * isa);

extern int get_disassembler_isa(Context * ctx, ContextAddress addr, ContextISA * isa);

extern void ini_disassembly_service(Protocol * proto);

#else /* SERVICE_Disassembly */

#define add_disassembler(ctx, isa, disassembler) (void)(disassembler)

#endif /* SERVICE_Disassembly */

#endif /* D_disassembly */
