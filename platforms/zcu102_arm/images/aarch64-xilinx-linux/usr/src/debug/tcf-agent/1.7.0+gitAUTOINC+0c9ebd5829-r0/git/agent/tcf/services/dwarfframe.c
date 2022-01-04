/*******************************************************************************
 * Copyright (c) 2007-2020 Wind River Systems, Inc. and others.
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

#include <tcf/config.h>

#if ENABLE_ELF && ENABLE_DebugContext

#include <assert.h>
#include <stdio.h>
#include <tcf/framework/exceptions.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/trace.h>
#include <tcf/services/dwarf.h>
#include <tcf/services/dwarfio.h>
#include <tcf/services/dwarfframe.h>

#define EH_PE_omit              0xff

#define EH_PE_absptr            0x00
#define EH_PE_uleb128           0x01
#define EH_PE_udata2            0x02
#define EH_PE_udata4            0x03
#define EH_PE_udata8            0x04
#define EH_PE_sleb128           0x09
#define EH_PE_sdata2            0x0a
#define EH_PE_sdata4            0x0b
#define EH_PE_sdata8            0x0c

#define EH_PB_pcrel             0x01
#define EH_PB_textrel           0x02
#define EH_PB_datarel           0x03
#define EH_PB_funcrel           0x04
#define EH_PB_aligned           0x05

#define EH_PE_indirect          0x80

#define RULE_OFFSET             1
#define RULE_SAME_VALUE         2
#define RULE_REGISTER           3
#define RULE_EXPRESSION         4
#define RULE_VAL_OFFSET         5
#define RULE_VAL_EXPRESSION     6

struct FrameInfoRange {
    U4_T mSection;
    ContextAddress mAddr;
    ContextAddress mSize;
    U8_T mOffset;
};

typedef struct RegisterRules {
    int rule;
    I4_T offset;
    U8_T expression;
} RegisterRules;

typedef struct StackFrameRegisters {
    int cfa_rule;
    I4_T cfa_offset;
    U4_T cfa_register;
    U8_T cfa_expression;
    RegisterRules * regs;
    int regs_cnt;
    int regs_max;
} StackFrameRegisters;

typedef struct StackFrameRules {
    Context * ctx;
    ELF_Section * section;
    ELF_Section * text_section;
    RegisterIdScope reg_id_scope;
    int eh_frame;
    U1_T version;
    U1_T address_size;
    U1_T segment_size;
    U4_T code_alignment;
    I4_T data_alignment;
    U8_T cie_pos;
    char * cie_aug;
    U8_T cie_eh_data;
    ELF_Section * cie_eh_data_section;
    U4_T fde_aug_length;
    U1_T * fde_aug_data;
    U1_T lsda_encoding;
    U1_T prh_encoding;
    U1_T addr_encoding;
    ELF_Section * loc_section;
    U8_T location;
    int return_address_register;
} StackFrameRules;

static StackFrameRegisters frame_regs;
static StackFrameRegisters cie_regs;
static StackFrameRegisters * regs_stack = NULL;
static int regs_stack_max = 0;
static int regs_stack_pos = 0;

static StackFrameRules rules;

U8_T dwarf_stack_trace_addr = 0;
U8_T dwarf_stack_trace_size = 0;

StackFrameRegisterLocation * dwarf_stack_trace_fp = NULL;

int dwarf_stack_trace_regs_cnt = 0;
StackFrameRegisterLocation ** dwarf_stack_trace_regs = NULL;

static int trace_regs_max = 0;
static unsigned trace_cmds_max = 0;
static unsigned trace_cmds_cnt = 0;
static LocationExpressionCommand * trace_cmds = NULL;

static RegisterRules * get_reg(StackFrameRegisters * regs, int reg) {
    int min_reg_cnt = 0;
    while (regs->regs_cnt <= reg || regs->regs_cnt < min_reg_cnt) {
        int n = regs->regs_cnt++;
        if (n >= regs->regs_max) {
            regs->regs_max = regs->regs_max == 0 ? 32 : regs->regs_max * 2;
            regs->regs = (RegisterRules *)loc_realloc(regs->regs, sizeof(RegisterRules) * regs->regs_max);
        }
        memset(regs->regs + n, 0, sizeof(RegisterRules));
        /* Architecture specific implied rules */
        switch (rules.reg_id_scope.machine) {
        case EM_386:
            min_reg_cnt = 8;
            switch (n) {
            case 4: /* SP */
                regs->regs[n].rule = RULE_VAL_OFFSET;
                break;
            case 3: /* BX */
            case 5: /* BP */
            case 6: /* SI */
            case 7: /* DI */
                regs->regs[n].rule = RULE_SAME_VALUE;
                break;
            }
            break;
        case EM_X86_64:
            min_reg_cnt = 16;
            switch (n) {
            case 3: /* BX */
            case 6: /* BP */
            case 12: /* R12 */
            case 13: /* R13 */
            case 14: /* R14 */
            case 15: /* R15 */
                regs->regs[n].rule = RULE_SAME_VALUE;
                break;
            case 7: /* SP */
                regs->regs[n].rule = RULE_VAL_OFFSET;
                break;
            }
            break;
        case EM_PPC:
            min_reg_cnt = 64;
            if (n == 1) {
                regs->regs[n].rule = RULE_VAL_OFFSET;
            }
            else if ((n >= 14 && n <= 31) || (n >= 46 && n <= 63)) {
                regs->regs[n].rule = RULE_SAME_VALUE;
            }
            else if (n == rules.return_address_register) {
                regs->regs[n].rule = RULE_REGISTER;
                regs->regs[n].offset = 108; /* LR */
            }
            break;
        case EM_PPC64:
            min_reg_cnt = 64;
            if (n == 1) {
                regs->regs[n].rule = RULE_VAL_OFFSET;
            }
            else if ((n >= 14 && n <= 31) || (n >= 46 && n <= 63)) {
                regs->regs[n].rule = RULE_SAME_VALUE;
            }
            else if (n == rules.return_address_register) {
                regs->regs[n].rule = RULE_REGISTER;
                regs->regs[n].offset = 65; /* LR */
            }
            break;
        case EM_ARM:
            min_reg_cnt = 129;
            if (n >= 4 && n <= 11) { /* Local variables */
                regs->regs[n].rule = RULE_SAME_VALUE;
            }
            else if (n == 13) { /* Stack pointer */
                regs->regs[n].rule = RULE_VAL_OFFSET;
            }
            else if (n == 128) { /* CPSR, it is needed for stack crawl */
                regs->regs[n].rule = RULE_SAME_VALUE;
            }
            else if (n == rules.return_address_register) {
                regs->regs[n].rule = RULE_REGISTER;
                regs->regs[n].offset = 14; /* LR */
            }
            break;
        case EM_TRICORE:
            min_reg_cnt = 50;
            if ((n >= 0 && n <= 7) ||   /* D[0] through D[7] */
                (n >= 18 && n <= 23)) { /* A[2] through A[7] */
                /* Must be saved across function calls. Callee-save */
                regs->regs[n].rule = RULE_SAME_VALUE;
            }
            else if (n == 26) { /* Stack pointer */
                regs->regs[n].rule = RULE_VAL_OFFSET;
            }
            else if (n == rules.return_address_register) {
                regs->regs[n].rule = RULE_REGISTER;
                regs->regs[n].offset = 27; /* RA */
            }
            break;
        case EM_V800:
        case EM_V850:
            min_reg_cnt = 32;
            if (n == 0) { /* Always same - reads as zero */
                regs->regs[n].rule = RULE_SAME_VALUE;
            }
            else if (n >= 6 && n <= 29) { /* Must be saved across function calls. Callee-save */
                regs->regs[n].rule = RULE_SAME_VALUE;
            }
            else if (n == 3) { /* Stack pointer */
                regs->regs[n].rule = RULE_VAL_OFFSET;
            }
            else if (n == rules.return_address_register) {
                regs->regs[n].rule = RULE_REGISTER;
                regs->regs[n].offset = 31; /* Link */
            }
            break;
        case EM_MICROBLAZE:
            min_reg_cnt = 32;
            if (n == 0) { /* Always same - reads as zero */
                regs->regs[n].rule = RULE_SAME_VALUE;
            }
            else if (n >= 19 && n <= 31) { /* Must be saved across function calls. Callee-save */
                regs->regs[n].rule = RULE_SAME_VALUE;
            }
            else if (n == 2) { /* Read-only small data area anchor */
                regs->regs[n].rule = RULE_SAME_VALUE;
            }
            else if (n == 13) { /* Read-write small data area anchor */
                regs->regs[n].rule = RULE_SAME_VALUE;
            }
            else if (n == 14) { /* Used to store return addresses for interrupts */
                regs->regs[n].rule = RULE_SAME_VALUE;
            }
            else if (n == 1) { /* Stack pointer */
                regs->regs[n].rule = RULE_VAL_OFFSET;
            }
            else if (n == rules.return_address_register) {
                regs->regs[n].rule = RULE_REGISTER;
                regs->regs[n].offset = 15; /* R15 is used for func return address */
            }
            break;
        case EM_AARCH64:
            min_reg_cnt = 32;
            if (n >= 19 && n <= 29) { /* Callee-saved registers */
                regs->regs[n].rule = RULE_SAME_VALUE;
            }
            else if (n == 31) { /* Stack pointer */
                regs->regs[n].rule = RULE_VAL_OFFSET;
            }
            else if (n == rules.return_address_register) {
                regs->regs[n].rule = RULE_REGISTER;
                regs->regs[n].offset = 30; /* LR */
            }
            break;
        case EM_RISCV:
            min_reg_cnt = 28;
            if (n == 0) { /* Always same - reads as zero */
                regs->regs[n].rule = RULE_SAME_VALUE;
            }
            else if ((n >= 8 && n <= 9) || (n >= 18 && n <= 27)) { /* Callee-saved registers */
                regs->regs[n].rule = RULE_SAME_VALUE;
            }
            else if (n == 2) { /* Stack pointer */
                regs->regs[n].rule = RULE_VAL_OFFSET;
            }
            else if (n == rules.return_address_register) {
                regs->regs[n].rule = RULE_REGISTER;
                regs->regs[n].offset = 1; /* RA */
            }
            break;
        }
    }
    return regs->regs + reg;
}

static void clear_frame_registers(StackFrameRegisters * rgs) {
    rgs->cfa_rule = 0;
    rgs->cfa_offset = 0;
    rgs->cfa_register = 0;
    rgs->cfa_expression = 0;
    rgs->regs_cnt = 0;
}

static void copy_register_rules(StackFrameRegisters * dst, StackFrameRegisters * src) {
    int n;
    clear_frame_registers(dst);
    dst->cfa_rule = src->cfa_rule;
    dst->cfa_offset = src->cfa_offset;
    dst->cfa_register = src->cfa_register;
    dst->cfa_expression = src->cfa_expression;
    for (n = 0; n < src->regs_cnt; n++) {
        *get_reg(dst, n) = *get_reg(src, n);
    }
}

static StackFrameRegisters * get_regs_stack_item(int n) {
    while (n >= regs_stack_max) {
        int max = regs_stack_max;
        regs_stack_max = regs_stack_max == 0 ? 8 : regs_stack_max * 2;
        regs_stack = (StackFrameRegisters *)loc_realloc(regs_stack, sizeof(StackFrameRegisters) * regs_stack_max);
        memset(regs_stack + max, 0, sizeof(StackFrameRegisters) * (regs_stack_max - max));
    }
    return regs_stack + n;
}

static U8_T read_frame_data_pointer(U1_T encoding, ELF_Section ** sec, U8_T func_addr) {
    U8_T v = 0;
    U8_T pos;
    unsigned idx;
    ELF_File * file;

    if (encoding == EH_PE_omit) return 0;
    pos = dio_GetPos();
    /* Decode the base or adjust the offset */
    switch ((encoding >> 4) & 0x7) {
    case 0:
    case EH_PB_funcrel:
        v = func_addr;
        break;
    case EH_PB_pcrel:
        if (sec != NULL) {
            v = pos + rules.section->addr;
        }
        break;
    case EH_PB_datarel:
        if (sec != NULL) {
            v = rules.section->addr;
        }
        break;
    case EH_PB_textrel:
        if (sec != NULL && rules.text_section != NULL) {
            v = rules.text_section->addr;
        }
        break;
    case EH_PB_aligned:
        if ((pos % rules.address_size) != 0) dio_SetPos(pos + (rules.address_size - (pos % rules.address_size)));
        break;
    default:
        str_exception(ERR_INV_DWARF, "Unknown encoding of .eh_frame section pointers");
        break;
    }
    /* Decode the value */
    switch (encoding & 0xf) {
    case EH_PE_absptr:
        v += dio_ReadAddress(sec);
        break;
    case EH_PE_uleb128:
        v += dio_ReadU8LEB128();
        break;
    case EH_PE_udata2:
        v += dio_ReadAddressX(sec, 2);
        break;
    case EH_PE_udata4:
        v += dio_ReadAddressX(sec, 4);
        break;
    case EH_PE_udata8:
        v += dio_ReadAddressX(sec, 8);
        break;
    case EH_PE_sleb128:
        v += dio_ReadS8LEB128();
        break;
    case EH_PE_sdata2:
        v += (I2_T)dio_ReadAddressX(sec, 2);
        break;
    case EH_PE_sdata4:
        v += (I4_T)dio_ReadAddressX(sec, 4);
        break;
    case EH_PE_sdata8:
        v += (I8_T)dio_ReadAddressX(sec, 8);
        break;
    default:
        str_exception(ERR_INV_DWARF, "Unknown encoding of .eh_frame section pointers");
        break;
    }
    if (encoding & EH_PE_indirect) {
        size_t size = rules.address_size;
        U8_T res = 0;
        file = rules.section->file;
        for (idx = 1; idx < file->section_cnt; idx++) {
            ELF_Section * sec = file->sections + idx;
            if ((sec->flags & SHF_ALLOC) == 0) continue;
            if (sec->addr <= v && sec->addr + sec->size >= v + size) {
                U1_T * p;
                size_t i;
                if (sec->data == NULL && elf_load(sec) < 0) exception(errno);
                p = (U1_T *)sec->data + (uintptr_t)(v - sec->addr);
                for (i = 0; i < size; i++) {
                    res = (res << 8) | p[file->big_endian ? i : size - i - 1];
                }
                break;
            }
        }
        v = res;
    }
    return v;
}

static void exec_stack_frame_instruction(U8_T func_addr) {
    RegisterRules * reg;
    U1_T op = dio_ReadU1();
    U4_T n;

    switch (op) {
    case CFA_nop:
        break;
    case CFA_set_loc:
        rules.location = read_frame_data_pointer(rules.addr_encoding, &rules.loc_section, func_addr);
        break;
    case CFA_advance_loc1:
        rules.location += dio_ReadU1() * rules.code_alignment;
        break;
    case CFA_advance_loc2:
        rules.location += dio_ReadU2() * rules.code_alignment;
        break;
    case CFA_advance_loc4:
        rules.location += dio_ReadU4() * rules.code_alignment;
        break;
    case CFA_offset_extended:
        reg = get_reg(&frame_regs, dio_ReadULEB128());
        reg->rule = RULE_OFFSET;
        reg->offset = dio_ReadULEB128() * rules.data_alignment;
        break;
    case CFA_restore_extended:
        n = dio_ReadULEB128();
        reg = get_reg(&frame_regs, n);
        *reg = *get_reg(&cie_regs, n);
        break;
    case CFA_undefined:
        reg = get_reg(&frame_regs, dio_ReadULEB128());
        memset(reg, 0, sizeof(*reg));
        break;
    case CFA_same_value:
        reg = get_reg(&frame_regs, dio_ReadULEB128());
        reg->rule = RULE_SAME_VALUE;
        break;
    case CFA_register:
        reg = get_reg(&frame_regs, dio_ReadULEB128());
        reg->rule = RULE_REGISTER;
        reg->offset = dio_ReadULEB128();
        break;
    case CFA_remember_state:
        copy_register_rules(get_regs_stack_item(regs_stack_pos++), &frame_regs);
        break;
    case CFA_restore_state:
        if (regs_stack_pos <= 0) {
            str_exception(ERR_INV_DWARF, "Invalid DW_CFA_restore_state instruction");
        }
        copy_register_rules(&frame_regs, get_regs_stack_item(--regs_stack_pos));
        break;
    case CFA_def_cfa:
        frame_regs.cfa_rule = RULE_OFFSET;
        frame_regs.cfa_register = dio_ReadULEB128();
        frame_regs.cfa_offset = dio_ReadULEB128();
        break;
    case CFA_def_cfa_register:
        frame_regs.cfa_rule = RULE_OFFSET;
        frame_regs.cfa_register = dio_ReadULEB128();
        break;
    case CFA_def_cfa_offset:
        frame_regs.cfa_rule = RULE_OFFSET;
        frame_regs.cfa_offset = dio_ReadULEB128();
        break;
    case CFA_def_cfa_expression:
        frame_regs.cfa_rule = RULE_EXPRESSION;
        frame_regs.cfa_offset = dio_ReadULEB128();
        frame_regs.cfa_expression = dio_GetPos();
        dio_Skip(frame_regs.cfa_offset);
        break;
    case CFA_expression:
        reg = get_reg(&frame_regs, dio_ReadULEB128());
        reg->rule = RULE_EXPRESSION;
        reg->offset = dio_ReadULEB128();
        reg->expression = dio_GetPos();
        dio_Skip(reg->offset);
        break;
    case CFA_offset_extended_sf:
        reg = get_reg(&frame_regs, dio_ReadULEB128());
        reg->rule = RULE_OFFSET;
        reg->offset = dio_ReadSLEB128() * rules.data_alignment;
        break;
    case CFA_def_cfa_sf:
        frame_regs.cfa_rule = RULE_OFFSET;
        frame_regs.cfa_register = dio_ReadULEB128();
        frame_regs.cfa_offset = dio_ReadSLEB128() * rules.data_alignment;
        break;
    case CFA_def_cfa_offset_sf:
        frame_regs.cfa_rule = RULE_OFFSET;
        frame_regs.cfa_offset = dio_ReadSLEB128() * rules.data_alignment;
        break;
    case CFA_val_offset:
        reg = get_reg(&frame_regs, dio_ReadULEB128());
        reg->rule = RULE_VAL_OFFSET;
        reg->offset = dio_ReadULEB128() * rules.data_alignment;
        break;
    case CFA_val_offset_sf:
        reg = get_reg(&frame_regs, dio_ReadULEB128());
        reg->rule = RULE_VAL_OFFSET;
        reg->offset = dio_ReadSLEB128() * rules.data_alignment;
        break;
    case CFA_val_expression:
        reg = get_reg(&frame_regs, dio_ReadULEB128());
        reg->rule = RULE_VAL_EXPRESSION;
        reg->offset = dio_ReadULEB128();
        reg->expression = dio_GetPos();
        dio_Skip(reg->offset);
        break;
    case CFA_CFA_GNU_window_save:
        /* SPARC-specific code */
        for (n = 8; n < 16; n++) {
            reg = get_reg(&frame_regs, n);
            reg->rule = RULE_REGISTER;
            reg->offset = n + 16;
        }
        for (n = 16; n < 32; n++) {
            reg = get_reg(&frame_regs, n);
            reg->rule = RULE_OFFSET;
            reg->offset = (n - 16) * (rules.reg_id_scope.machine == EM_SPARCV9 ? 8 : 4);
        }
        break;
    case CFA_GNU_args_size:
        /* This instruction specifies the total size of the arguments
         * which have been pushed onto the stack. Not used by the debugger. */
        dio_ReadULEB128();
        break;
    case CFA_GNU_negative_offset_ext:
        /* This instruction is identical to DW_CFA_offset_extended_sf
         * except that the operand is subtracted to produce the offset. */
        reg = get_reg(&frame_regs, dio_ReadULEB128());
        reg->rule = RULE_OFFSET;
        reg->offset = -dio_ReadSLEB128() * rules.data_alignment;
        break;
    default:
        switch (op >> 6) {
        case 0:
            str_exception(ERR_INV_DWARF, "Unsupported instruction in Call Frame Information");
            break;
        case 1: /* DW_CFA_advance_loc */
            rules.location += (op & 0x3f) * rules.code_alignment;
            break;
        case 2: /* DW_CFA_offset */
            reg = get_reg(&frame_regs, op & 0x3f);
            reg->rule = RULE_OFFSET;
            reg->offset = dio_ReadULEB128() * rules.data_alignment;
            break;
        case 3: /* DW_CFA_restore */
            n = op & 0x3f;
            reg = get_reg(&frame_regs, n);
            *reg = *get_reg(&cie_regs, n);
            break;
        }
    }
}

static LocationExpressionCommand * add_command(int op) {
    LocationExpressionCommand * cmd = NULL;
    if (trace_cmds_cnt >= trace_cmds_max) {
        trace_cmds_max += 16;
        trace_cmds = (LocationExpressionCommand *)loc_realloc(trace_cmds, trace_cmds_max * sizeof(LocationExpressionCommand));
    }
    cmd = trace_cmds + trace_cmds_cnt++;
    memset(cmd, 0, sizeof(*cmd));
    cmd->cmd = op;
    return cmd;
}

static void add_command_sequence(StackFrameRegisterLocation ** ptr, RegisterDefinition * reg) {
    StackFrameRegisterLocation * seq = *ptr;
    if (seq == NULL || seq->cmds_max < trace_cmds_cnt) {
        *ptr = seq = (StackFrameRegisterLocation *)loc_realloc(seq, sizeof(StackFrameRegisterLocation) + (trace_cmds_cnt - 1) * sizeof(LocationExpressionCommand));
        seq->cmds_max = trace_cmds_cnt;
    }
    seq->reg = reg;
    seq->cmds_cnt = trace_cmds_cnt;
    memcpy(seq->cmds, trace_cmds, trace_cmds_cnt * sizeof(LocationExpressionCommand));
}

static void add_dwarf_expression_commands(U8_T cmds_offs, U4_T cmds_size) {
    dio_EnterSection(NULL, rules.section, cmds_offs);
    while (dio_GetPos() < cmds_offs + cmds_size) {
        U1_T op = dio_ReadU1();

        switch (op) {
        case OP_addr:
            {
                ELF_Section * section = NULL;
                U8_T lt_addr = dio_ReadAddress(&section);
                ContextAddress rt_addr = elf_map_to_run_time_address(
                    rules.ctx, rules.section->file, section, (ContextAddress)lt_addr);
                if (errno) str_exception(errno, "Cannot get object run-time address");
                add_command(SFT_CMD_NUMBER)->args.num = rt_addr;
            }
            break;
        case OP_deref:
            {
                LocationExpressionCommand * cmd = add_command(SFT_CMD_RD_MEM);
                cmd->args.mem.size = rules.address_size;
                cmd->args.mem.big_endian = rules.reg_id_scope.big_endian;
            }
            break;
        case OP_deref_size:
            {
                LocationExpressionCommand * cmd = add_command(SFT_CMD_RD_MEM);
                cmd->args.mem.size = dio_ReadU1();
                cmd->args.mem.big_endian = rules.reg_id_scope.big_endian;
            }
            break;
        case OP_const1u:
            add_command(SFT_CMD_NUMBER)->args.num = dio_ReadU1();
            break;
        case OP_const1s:
            add_command(SFT_CMD_NUMBER)->args.num = (I1_T)dio_ReadU1();
            break;
        case OP_const2u:
            add_command(SFT_CMD_NUMBER)->args.num = dio_ReadU2();
            break;
        case OP_const2s:
            add_command(SFT_CMD_NUMBER)->args.num = (I2_T)dio_ReadU2();
            break;
        case OP_const4u:
            add_command(SFT_CMD_NUMBER)->args.num = dio_ReadU4();
            break;
        case OP_const4s:
            add_command(SFT_CMD_NUMBER)->args.num = (I4_T)dio_ReadU4();
            break;
        case OP_const8u:
            add_command(SFT_CMD_NUMBER)->args.num = dio_ReadU8();
            break;
        case OP_const8s:
            add_command(SFT_CMD_NUMBER)->args.num = (I8_T)dio_ReadU8();
            break;
        case OP_constu:
            add_command(SFT_CMD_NUMBER)->args.num = dio_ReadU8LEB128();
            break;
        case OP_consts:
            add_command(SFT_CMD_NUMBER)->args.num = dio_ReadS8LEB128();
            break;
        case OP_and:
            add_command(SFT_CMD_AND);
            break;
        case OP_minus:
            add_command(SFT_CMD_SUB);
            break;
        case OP_or:
            add_command(SFT_CMD_OR);
            break;
        case OP_plus:
            add_command(SFT_CMD_ADD);
            break;
        case OP_plus_uconst:
            add_command(SFT_CMD_NUMBER)->args.num = dio_ReadU8LEB128();
            add_command(SFT_CMD_ADD);
            break;
        case OP_ge:
            add_command(SFT_CMD_GE);
            break;
        case OP_gt:
            add_command(SFT_CMD_GT);
            break;
        case OP_le:
            add_command(SFT_CMD_LE);
            break;
        case OP_lt:
            add_command(SFT_CMD_LT);
            break;
        case OP_shl:
            add_command(SFT_CMD_SHL);
            break;
        case OP_lit0:
        case OP_lit1:
        case OP_lit2:
        case OP_lit3:
        case OP_lit4:
        case OP_lit5:
        case OP_lit6:
        case OP_lit7:
        case OP_lit8:
        case OP_lit9:
        case OP_lit10:
        case OP_lit11:
        case OP_lit12:
        case OP_lit13:
        case OP_lit14:
        case OP_lit15:
        case OP_lit16:
        case OP_lit17:
        case OP_lit18:
        case OP_lit19:
        case OP_lit20:
        case OP_lit21:
        case OP_lit22:
        case OP_lit23:
        case OP_lit24:
        case OP_lit25:
        case OP_lit26:
        case OP_lit27:
        case OP_lit28:
        case OP_lit29:
        case OP_lit30:
        case OP_lit31:
            add_command(SFT_CMD_NUMBER)->args.num = op - OP_lit0;
            break;
        case OP_reg0:
        case OP_reg1:
        case OP_reg2:
        case OP_reg3:
        case OP_reg4:
        case OP_reg5:
        case OP_reg6:
        case OP_reg7:
        case OP_reg8:
        case OP_reg9:
        case OP_reg10:
        case OP_reg11:
        case OP_reg12:
        case OP_reg13:
        case OP_reg14:
        case OP_reg15:
        case OP_reg16:
        case OP_reg17:
        case OP_reg18:
        case OP_reg19:
        case OP_reg20:
        case OP_reg21:
        case OP_reg22:
        case OP_reg23:
        case OP_reg24:
        case OP_reg25:
        case OP_reg26:
        case OP_reg27:
        case OP_reg28:
        case OP_reg29:
        case OP_reg30:
        case OP_reg31:
            {
                RegisterDefinition * def = get_reg_by_id(rules.ctx, op - OP_reg0, &rules.reg_id_scope);
                if (def == NULL) str_exception(errno, "Cannot read DWARF frame info");
                add_command(SFT_CMD_RD_REG)->args.reg = def;
            }
            break;
        case OP_regx:
            {
                unsigned n = (unsigned)dio_ReadULEB128();
                RegisterDefinition * def = get_reg_by_id(rules.ctx, n, &rules.reg_id_scope);
                if (def == NULL) str_exception(errno, "Cannot read DWARF frame info");
                add_command(SFT_CMD_RD_REG)->args.reg = def;
            }
            break;
        case OP_breg0:
        case OP_breg1:
        case OP_breg2:
        case OP_breg3:
        case OP_breg4:
        case OP_breg5:
        case OP_breg6:
        case OP_breg7:
        case OP_breg8:
        case OP_breg9:
        case OP_breg10:
        case OP_breg11:
        case OP_breg12:
        case OP_breg13:
        case OP_breg14:
        case OP_breg15:
        case OP_breg16:
        case OP_breg17:
        case OP_breg18:
        case OP_breg19:
        case OP_breg20:
        case OP_breg21:
        case OP_breg22:
        case OP_breg23:
        case OP_breg24:
        case OP_breg25:
        case OP_breg26:
        case OP_breg27:
        case OP_breg28:
        case OP_breg29:
        case OP_breg30:
        case OP_breg31:
            {
                I8_T offs = dio_ReadS8LEB128();
                RegisterDefinition * def = get_reg_by_id(rules.ctx, op - OP_breg0, &rules.reg_id_scope);
                if (def == NULL) str_exception(errno, "Cannot read DWARF frame info");
                add_command(SFT_CMD_RD_REG)->args.reg = def;
                if (offs != 0) {
                    add_command(SFT_CMD_NUMBER)->args.num = offs;
                    add_command(SFT_CMD_ADD);
                }
            }
            break;
        case OP_bregx:
            {
                unsigned n = (unsigned)dio_ReadULEB128();
                I8_T offs = dio_ReadS8LEB128();
                RegisterDefinition * def = get_reg_by_id(rules.ctx, n, &rules.reg_id_scope);
                if (def == NULL) str_exception(errno, "Cannot read DWARF frame info");
                add_command(SFT_CMD_RD_REG)->args.reg = def;
                if (offs != 0) {
                    add_command(SFT_CMD_NUMBER)->args.num = offs;
                    add_command(SFT_CMD_ADD);
                }
            }
            break;
        case OP_nop:
            break;
        default:
            trace(LOG_ALWAYS, "Unsupported DWARF expression op 0x%02x", op);
            str_exception(ERR_UNSUPPORTED, "Unsupported DWARF expression op");
        }
    }
}

static void generate_register_commands(RegisterRules * reg, RegisterDefinition * dst_reg_def, RegisterDefinition * src_reg_def) {
    if (dst_reg_def == NULL) return;
    trace_cmds_cnt = 0;
    switch (reg->rule) {
    case RULE_VAL_OFFSET:
    case RULE_OFFSET:
        add_command(SFT_CMD_FP);
        if (reg->offset != 0) {
            add_command(SFT_CMD_NUMBER)->args.num = reg->offset;
            add_command(SFT_CMD_ADD);
        }
        if (reg->rule == RULE_OFFSET) {
            LocationExpressionCommand * cmd = add_command(SFT_CMD_RD_MEM);
            cmd->args.mem.size = dst_reg_def->size;
            cmd->args.mem.big_endian = rules.reg_id_scope.big_endian;
        }
        break;
    case RULE_SAME_VALUE:
        if (src_reg_def == NULL) return;
        add_command(SFT_CMD_RD_REG)->args.reg = src_reg_def;
        break;
    case RULE_REGISTER:
        {
            RegisterDefinition * src_sef = get_reg_by_id(rules.ctx, reg->offset, &rules.reg_id_scope);
            if (src_sef != NULL) add_command(SFT_CMD_RD_REG)->args.reg = src_sef;
        }
        break;
    case RULE_EXPRESSION:
    case RULE_VAL_EXPRESSION:
        add_command(SFT_CMD_FP);
        add_dwarf_expression_commands(reg->expression, reg->offset);
        if (reg->rule == RULE_EXPRESSION) {
            LocationExpressionCommand * cmd = add_command(SFT_CMD_RD_MEM);
            cmd->args.mem.size = dst_reg_def->size;
            cmd->args.mem.big_endian = rules.reg_id_scope.big_endian;
        }
        break;
    default:
        str_exception(ERR_INV_DWARF, "Invalid .debug_frame");
        break;
    }
    if (rules.reg_id_scope.machine == EM_MICROBLAZE &&
            dst_reg_def != NULL && dst_reg_def->dwarf_id == 32 &&
            rules.return_address_register == 15) {
        add_command(SFT_CMD_NUMBER)->args.num = 8;
        add_command(SFT_CMD_ADD);
    }
    if (rules.reg_id_scope.machine == EM_ARM &&
            dst_reg_def != NULL && dst_reg_def->dwarf_id == 15) {
        add_command(SFT_CMD_NUMBER)->args.num = 0xfffffffe;
        add_command(SFT_CMD_AND);
    }
    if (dwarf_stack_trace_regs_cnt >= trace_regs_max) {
        int i;
        trace_regs_max += 16;
        dwarf_stack_trace_regs = (StackFrameRegisterLocation **)loc_realloc(dwarf_stack_trace_regs, trace_regs_max * sizeof(StackFrameRegisterLocation *));
        for (i = dwarf_stack_trace_regs_cnt; i < trace_regs_max; i++) dwarf_stack_trace_regs[i] = NULL;
    }
    if (trace_cmds_cnt == 0) return;
    add_command_sequence(dwarf_stack_trace_regs + dwarf_stack_trace_regs_cnt++, dst_reg_def);
}

static void generate_commands(void) {
    int i;
    RegisterRules * reg;
    RegisterDefinition * reg_def;

    reg = get_reg(&frame_regs, rules.return_address_register);
    if (reg->rule != 0) {
        reg_def = get_reg_by_id(rules.ctx, rules.return_address_register, &rules.reg_id_scope);
        generate_register_commands(reg, get_PC_definition(rules.ctx), reg_def);
    }
    for (i = 0; i < frame_regs.regs_cnt; i++) {
        reg = get_reg(&frame_regs, i);
        if (reg->rule == 0) continue;
        reg_def = get_reg_by_id(rules.ctx, i, &rules.reg_id_scope);
        generate_register_commands(reg, reg_def, reg_def);
    }

    trace_cmds_cnt = 0;
    switch (frame_regs.cfa_rule) {
    case RULE_OFFSET:
        reg_def = get_reg_by_id(rules.ctx, frame_regs.cfa_register, &rules.reg_id_scope);
        if (reg_def != NULL) {
            /* TriCore : PCXI needs to be decyphered so it will point ot the CSA
             * which is an area of the memory where registers were saved.
             */
            if ((rules.reg_id_scope.machine == EM_TRICORE) && (reg_def->dwarf_id == 41))
                add_command(SFT_CMD_RD_REG_PCXI_TRICORE)->args.reg = reg_def;
            else
                add_command(SFT_CMD_RD_REG)->args.reg = reg_def;
            if (frame_regs.cfa_offset != 0) {
                add_command(SFT_CMD_NUMBER)->args.num = frame_regs.cfa_offset;
                add_command(SFT_CMD_ADD);
            }
        }
        break;
    case RULE_EXPRESSION:
        add_dwarf_expression_commands(frame_regs.cfa_expression, frame_regs.cfa_offset);
        break;
    default:
        str_exception(ERR_INV_DWARF, "Invalid .debug_frame");
        break;
    }
    add_command_sequence(&dwarf_stack_trace_fp, NULL);
}

static void generate_plt_section_commands(Context * ctx, ELF_File * file, U8_T offs) {
    RegisterRules * reg = NULL;

    memset(&rules, 0, sizeof(StackFrameRules));
    rules.ctx = ctx;
    rules.reg_id_scope.big_endian = file->big_endian;
    rules.reg_id_scope.machine = file->machine;
    rules.reg_id_scope.os_abi = file->os_abi;
    rules.reg_id_scope.elf64 = file->elf64;
    rules.reg_id_scope.id_type = REGNUM_DWARF;
    rules.address_size = file->elf64 ? 8 : 4;

    clear_frame_registers(&cie_regs);
    clear_frame_registers(&frame_regs);
    switch (rules.reg_id_scope.machine) {
    case EM_386:
        frame_regs.cfa_rule = RULE_OFFSET;
        frame_regs.cfa_register = 4; /* esp */
        if (offs == 0) {
            frame_regs.cfa_offset = 8;
        }
        else if (offs < 16) {
            frame_regs.cfa_offset = 12;
        }
        else if ((offs - 16) % 16 < 11) {
            frame_regs.cfa_offset = 4;
        }
        else {
            frame_regs.cfa_offset = 8;
        }
        rules.return_address_register = 8; /* eip */
        reg = get_reg(&frame_regs, rules.return_address_register);
        reg->rule = RULE_OFFSET;
        reg->offset = -4;
        generate_commands();
        break;
    case EM_X86_64:
        frame_regs.cfa_rule = RULE_OFFSET;
        frame_regs.cfa_register = 7; /* rsp */
        if (offs == 0) {
            frame_regs.cfa_offset = 16;
        }
        else if (offs < 16) {
            frame_regs.cfa_offset = 24;
        }
        else if ((offs - 16) % 16 < 11) {
            frame_regs.cfa_offset = 8;
        }
        else {
            frame_regs.cfa_offset = 16;
        }
        rules.return_address_register = 16; /* rip */
        reg = get_reg(&frame_regs, rules.return_address_register);
        reg->rule = RULE_OFFSET;
        reg->offset = -8;
        generate_commands();
        break;
    case EM_PPC:
        rules.return_address_register = 108; /* LR */
        frame_regs.cfa_rule = RULE_OFFSET;
        frame_regs.cfa_register = 1; /* R1 */
        generate_commands();
        break;
    case EM_PPC64:
        rules.return_address_register = 65; /* LR */
        frame_regs.cfa_rule = RULE_OFFSET;
        frame_regs.cfa_register = 1; /* R1 */
        generate_commands();
        break;
    case EM_ARM:
        rules.return_address_register = 14; /* LR */
        frame_regs.cfa_rule = RULE_OFFSET;
        frame_regs.cfa_register = 13; /* SP */
        generate_commands();
        break;
    case EM_MICROBLAZE:
        rules.return_address_register = 15; /* R15 */
        frame_regs.cfa_rule = RULE_OFFSET;
        frame_regs.cfa_register = 1; /* R1 */
        generate_commands();
        break;
    case EM_AARCH64:
        rules.return_address_register = 30; /* LR */
        frame_regs.cfa_rule = RULE_OFFSET;
        frame_regs.cfa_register = 31; /* SP */
        generate_commands();
        break;
    case EM_RISCV:
        rules.return_address_register = 1; /* RA */
        frame_regs.cfa_rule = RULE_OFFSET;
        frame_regs.cfa_register = 2; /* SP */
        generate_commands();
        break;
    }
}

static void read_frame_cie(U8_T fde_pos, U8_T pos) {
    int cie_dwarf64 = 0;
    U8_T saved_pos = dio_GetPos();
    U8_T cie_length = 0;
    U8_T cie_end = 0;

    rules.cie_pos = pos;
    if (pos >= rules.section->size) {
        str_fmt_exception(ERR_INV_DWARF,
            "Invalid CIE pointer %#" PRIx64
            " in FDE at %#" PRIx64, pos, fde_pos);
    }
    dio_SetPos(pos);
    cie_length = dio_ReadU4();
    if (cie_length == ~(U4_T)0) {
        cie_length = dio_ReadU8();
        cie_dwarf64 = 1;
    }
    cie_end = dio_GetPos() + cie_length;
    dio_Skip(cie_dwarf64 ? 8 : 4);
    rules.version = dio_ReadU1();
    if (rules.version != 1 && rules.version != 3 && rules.version != 4) {
        str_fmt_exception(ERR_INV_DWARF,
            "Unsupported version of Call Frame Information: %d", rules.version);
    }
    rules.cie_aug = dio_ReadString();
    if (rules.cie_aug != NULL && strcmp(rules.cie_aug, "eh") == 0) {
        rules.cie_eh_data = dio_ReadAddress(&rules.cie_eh_data_section);
    }
    if (rules.version >= 4) {
        rules.address_size = dio_ReadU1();
        rules.segment_size = dio_ReadU1();
    }
    else {
        rules.address_size = rules.section->file->elf64 ? 8 : 4;
        rules.segment_size = 0;
    }
    if (rules.segment_size != 0) {
        str_exception(ERR_INV_DWARF,
            "Unsupported Call Frame Information: segment size != 0");
    }
    rules.code_alignment = dio_ReadULEB128();
    rules.data_alignment = dio_ReadSLEB128();
    rules.return_address_register = dio_ReadULEB128();

    rules.lsda_encoding = 0;
    rules.prh_encoding = 0;
    rules.addr_encoding = 0;
    if (rules.cie_aug != NULL && rules.cie_aug[0] == 'z') {
        U4_T aug_length = dio_ReadULEB128();
        U8_T aug_pos = dio_GetPos();
        char * p = rules.cie_aug + 1;
        while (*p) {
            switch (*p++) {
            case 'L':
                rules.lsda_encoding = dio_ReadU1();
                break;
            case 'P':
                {
                    Trap trap;
                    U8_T addr_pos;
                    rules.prh_encoding = dio_ReadU1();
                    addr_pos = dio_GetPos();
                    if (set_trap(&trap)) {
                        read_frame_data_pointer(rules.prh_encoding, NULL, 0);
                        clear_trap(&trap);
                    }
                    else {
                        dio_SetPos(addr_pos + rules.address_size);
                    }
                }
                break;
            case 'R':
                rules.addr_encoding = dio_ReadU1();
                break;
            }
        }
        dio_SetPos(aug_pos + aug_length);
    }
    clear_frame_registers(&cie_regs);
    clear_frame_registers(&frame_regs);
    regs_stack_pos = 0;
    while (dio_GetPos() < cie_end) {
        exec_stack_frame_instruction(0);
    }
    copy_register_rules(&cie_regs, &frame_regs);
    dio_SetPos(saved_pos);
}

static void read_frame_fde(U8_T IP, U8_T fde_pos) {
    int fde_dwarf64 = 0;
    U8_T fde_length = 0;
    U8_T fde_end = 0;
    U8_T ref_pos = 0;
    U8_T cie_ref = 0;
    int fde_flag = 0;

    dio_EnterSection(NULL, rules.section, fde_pos);
    fde_length = dio_ReadU4();
    assert(fde_length > 0);
    if (fde_length == ~(U4_T)0) {
        fde_length = dio_ReadU8();
        fde_dwarf64 = 1;
    }
    ref_pos = dio_GetPos();
    fde_end = ref_pos + fde_length;
    cie_ref = fde_dwarf64 ? dio_ReadU8() : dio_ReadU4();
    if (rules.eh_frame) fde_flag = cie_ref != 0;
    else if (fde_dwarf64) fde_flag = cie_ref != ~(U8_T)0;
    else fde_flag = cie_ref != ~(U4_T)0;
    assert(fde_flag);
    if (fde_flag) {
        U8_T Addr, Range;
        if (rules.eh_frame) cie_ref = ref_pos - cie_ref;
        if (cie_ref != rules.cie_pos) read_frame_cie(fde_pos, cie_ref);
        Addr = read_frame_data_pointer(rules.addr_encoding, &rules.loc_section, 0);
        Range = read_frame_data_pointer(rules.addr_encoding, NULL, 0);
        assert(Addr <= IP && Addr + Range > IP);
        if (Addr <= IP && Addr + Range > IP) {
            U8_T location0 = Addr;
            if (rules.cie_aug != NULL && rules.cie_aug[0] == 'z') {
                rules.fde_aug_length = dio_ReadULEB128();
                rules.fde_aug_data = dio_GetDataPtr();
                dio_Skip(rules.fde_aug_length);
            }
            copy_register_rules(&frame_regs, &cie_regs);
            rules.location = Addr;
            regs_stack_pos = 0;
            for (;;) {
                if (dio_GetPos() >= fde_end) {
                    rules.location = Addr + Range;
                    break;
                }
                exec_stack_frame_instruction(Addr);
                assert(location0 <= IP);
                if (rules.location > IP) break;
                location0 = rules.location;
            }
            dwarf_stack_trace_addr = location0;
            dwarf_stack_trace_size = rules.location - location0;
            if (rules.reg_id_scope.machine == EM_ARM && IP + 4 == Addr + Range) {
                /* GCC generates invalid frame info for ARM function epilogue */
                /* Ignore frame info, fall-back to stack crawl logic */
                dio_ExitSection();
                return;
            }
            generate_commands();
            if (dwarf_stack_trace_regs_cnt == 0) {
                /* GHS generates dummy frame info with all registers marked undefined */
                /* Ignore frame info, fall-back to stack crawl logic */
                dwarf_stack_trace_fp->cmds_cnt = 0;
                dwarf_stack_trace_addr = 0;
                dwarf_stack_trace_size = 0;
                dio_ExitSection();
                return;
            }
        }
    }
    dio_ExitSection();
}

static int cmp_frame_info_ranges(const void * x, const void * y) {
    FrameInfoRange * rx = (FrameInfoRange *)x;
    FrameInfoRange * ry = (FrameInfoRange *)y;
    if (rx->mSection < ry->mSection) return -1;
    if (rx->mSection > ry->mSection) return +1;
    if (rx->mAddr < ry->mAddr) return -1;
    if (rx->mAddr > ry->mAddr) return +1;
    return 0;
}

static void create_search_index(DWARFCache * cache, FrameInfoIndex * index) {
    ELF_Section * section = index->mSection;
    dio_EnterSection(NULL, section, 0);
    while (dio_GetPos() < section->size) {
        int fde_dwarf64 = 0;
        U8_T fde_length = 0;
        U8_T fde_pos = 0;
        U8_T fde_end = 0;
        U8_T ref_pos = 0;
        U8_T cie_ref = 0;
        int fde_flag = 0;

        fde_pos = dio_GetPos();
        fde_length = dio_ReadU4();
        if (fde_length == 0) continue;
        if (fde_length == ~(U4_T)0) {
            fde_length = dio_ReadU8();
            fde_dwarf64 = 1;
        }
        ref_pos = dio_GetPos();
        fde_end = ref_pos + fde_length;
        if (fde_end > rules.section->size) {
            U4_T alignment = section->alignment;
            if (alignment > 1 && fde_pos % alignment != 0) {
                /* Workaround for sections with invalid alignment */
                dio_SetPos(fde_pos + alignment - fde_pos % alignment);
                continue;
            }
            else {
                str_fmt_exception(ERR_INV_DWARF,
                    "Invalid length %#" PRIx64
                    " in FDE at %#" PRIx64, fde_length, fde_pos);
            }
        }
        cie_ref = fde_dwarf64 ? dio_ReadU8() : dio_ReadU4();
        if (rules.eh_frame) fde_flag = cie_ref != 0;
        else if (fde_dwarf64) fde_flag = cie_ref != ~(U8_T)0;
        else fde_flag = cie_ref != ~(U4_T)0;
        if (fde_flag) {
            ELF_Section * sec = NULL;
            FrameInfoRange * range = NULL;
            if (rules.eh_frame) cie_ref = ref_pos - cie_ref;
            if (cie_ref != rules.cie_pos) read_frame_cie(fde_pos, cie_ref);
            if (index->mFrameInfoRangesCnt >= index->mFrameInfoRangesMax) {
                index->mFrameInfoRangesMax += 512;
                if (index->mFrameInfoRanges == NULL) index->mFrameInfoRangesMax += (unsigned)(section->size / 32);
                index->mFrameInfoRanges = (FrameInfoRange *)loc_realloc(index->mFrameInfoRanges,
                    index->mFrameInfoRangesMax * sizeof(FrameInfoRange));
            }
            range = index->mFrameInfoRanges + index->mFrameInfoRangesCnt++;
            memset(range, 0, sizeof(FrameInfoRange));
            range->mAddr = (ContextAddress)read_frame_data_pointer(rules.addr_encoding, &sec, 0);
            range->mSize = (ContextAddress)read_frame_data_pointer(rules.addr_encoding, NULL, 0);
            if (sec != NULL) {
                range->mSection = sec->index;
                index->mRelocatable = 1;
            }
            range->mOffset = fde_pos;
        }
        dio_SetPos(fde_end);
    }
    dio_ExitSection();
    qsort(index->mFrameInfoRanges, index->mFrameInfoRangesCnt, sizeof(FrameInfoRange), cmp_frame_info_ranges);
}

static void read_frame_info_section(Context * ctx, ELF_Section * text_section,
                                    U8_T IP, DWARFCache * cache, FrameInfoIndex * index) {
    unsigned l, h;
    ELF_Section * section = index->mSection;
    ELF_File * file = section->file;
    U4_T sec_idx = 0;

    if (text_section != NULL && text_section->file != file) {
        unsigned i;
        assert(get_dwarf_file(text_section->file) == file);
        for (i = 1; i < file->section_cnt; i++) {
            ELF_Section * sec = cache->mFile->sections + i;
            if (sec->name == NULL) continue;
            if (strcmp(sec->name, text_section->name) == 0) {
                text_section = sec;
                break;
            }
        }
    }

    memset(&rules, 0, sizeof(StackFrameRules));
    rules.ctx = ctx;
    rules.section = section;
    rules.text_section = text_section;
    rules.eh_frame = strcmp(section->name, ".eh_frame") == 0;
    rules.reg_id_scope.big_endian = file->big_endian;
    rules.reg_id_scope.machine = file->machine;
    rules.reg_id_scope.os_abi = file->os_abi;
    rules.reg_id_scope.elf64 = file->elf64;
    rules.reg_id_scope.id_type = rules.eh_frame ? REGNUM_EH_FRAME : REGNUM_DWARF;
    rules.cie_pos = ~(U8_T)0;

    if (index->mFrameInfoRanges == NULL) {
        Trap trap;
        if (set_trap(&trap)) {
            create_search_index(cache, index);
            clear_trap(&trap);
        }
        else {
            loc_free(index->mFrameInfoRanges);
            index->mFrameInfoRanges = NULL;
            index->mFrameInfoRangesCnt = 0;
            index->mFrameInfoRangesMax = 0;
            exception(trap.error);
        }
    }
    l = 0;
    h = index->mFrameInfoRangesCnt;
    if (index->mRelocatable && text_section != NULL) sec_idx = text_section->index;
    while (l < h) {
        unsigned k = (l + h) / 2;
        FrameInfoRange * range = index->mFrameInfoRanges + k;
        if (sec_idx < range->mSection) {
            h = k;
        }
        else if (sec_idx > range->mSection) {
            l = k + 1;
        }
        else if (range->mAddr > IP) {
            h = k;
        }
        else if (range->mAddr + range->mSize < range->mAddr) {
            read_frame_fde(IP, range->mOffset);
            return;
        }
        else if (range->mAddr + range->mSize <= IP) {
            l = k + 1;
        }
        else {
            read_frame_fde(IP, range->mOffset);
            return;
        }
    }
}

void get_dwarf_stack_frame_info(Context * ctx, ELF_File * file, ELF_Section * text_section, U8_T addr) {
    DWARFCache * cache = NULL;
    FrameInfoIndex * index = NULL;
    ELF_File * dwarf_file = NULL;

    dwarf_stack_trace_regs_cnt = 0;
    if (dwarf_stack_trace_fp == NULL) {
        dwarf_stack_trace_fp = (StackFrameRegisterLocation *)loc_alloc_zero(sizeof(StackFrameRegisterLocation));
        dwarf_stack_trace_fp->cmds_max = 1;
    }
    dwarf_stack_trace_fp->cmds_cnt = 0;
    dwarf_stack_trace_addr = 0;
    dwarf_stack_trace_size = 0;

    dwarf_file = get_dwarf_file(file);
    if (dwarf_file != file) {
        cache = get_dwarf_cache(dwarf_file);
        index = cache->mFrameInfo;
        while (index != NULL) {
            read_frame_info_section(ctx, text_section, addr, cache, index);
            if (dwarf_stack_trace_fp->cmds_cnt > 0) return;
            index = index->mNext;
        }
    }

    cache = get_dwarf_cache(file);
    index = cache->mFrameInfo;
    while (index != NULL) {
        read_frame_info_section(ctx, text_section, addr, cache, index);
        if (dwarf_stack_trace_fp->cmds_cnt > 0) return;
        index = index->mNext;
    }

    if (text_section != NULL && text_section->name != NULL && strcmp(text_section->name, ".plt") == 0) {
        assert(addr >= text_section->addr);
        assert(addr < text_section->addr + text_section->size);
        generate_plt_section_commands(ctx, file, addr - text_section->addr);
        if (dwarf_stack_trace_fp->cmds_cnt > 0) {
            dwarf_stack_trace_addr = addr;
            dwarf_stack_trace_size = 1;
        }
    }
}

#endif /* ENABLE_ELF && ENABLE_DebugContext */
