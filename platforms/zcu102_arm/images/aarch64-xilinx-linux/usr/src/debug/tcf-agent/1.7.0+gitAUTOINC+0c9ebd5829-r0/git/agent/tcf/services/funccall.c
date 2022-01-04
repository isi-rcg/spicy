/*******************************************************************************
 * Copyright (c) 2012-2020 Wind River Systems, Inc. and others.
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
 * This module implements preparation of a new stack before calling a function.
 * This code is a stub. Downstream code will provide real implementation.
 */

#include <tcf/config.h>

#if ENABLE_Symbols && ENABLE_DebugContext

#include <assert.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/cpudefs.h>
#include <tcf/services/funccall.h>

#define EM_386          3 /* Intel Architecture */
#define EM_X86_64      62 /* AMD x86-64 architecture */

static FunctionCallInfo * info = NULL;

static unsigned trace_cmds_max = 0;
static unsigned trace_cmds_cnt = 0;
static LocationExpressionCommand * trace_cmds = NULL;

static Symbol * func_type = NULL;
static Symbol * func_return_type = NULL;
static Symbol ** func_args = NULL;
static int func_args_cnt = 0;
static ContextAddress func_return_size = 0;
static int func_return_type_class = 0;
static ContextAddress * arg_size_formal = NULL;
static int * arg_class_formal = NULL;
static int * arg_class_actual = NULL;

static int reg_arg_ids[] = { 5, 4, 1, 2, 8, 9 };

static LocationExpressionCommand * add_command(int op) {
    LocationExpressionCommand * cmd = NULL;
    if (trace_cmds_cnt >= trace_cmds_max) {
        trace_cmds_max += 16;
        trace_cmds = (LocationExpressionCommand *)tmp_realloc(trace_cmds, trace_cmds_max * sizeof(LocationExpressionCommand));
    }
    cmd = trace_cmds + trace_cmds_cnt++;
    memset(cmd, 0, sizeof(*cmd));
    cmd->cmd = op;
    if (op == SFT_CMD_RD_MEM || op == SFT_CMD_WR_MEM) {
        cmd->args.mem.big_endian = info->scope.big_endian;
    }
    return cmd;
}

#if 0 /* Not used */
static LocationExpressionCommand * add_command_location(uint8_t * code, size_t code_size) {
    LocationExpressionCommand * cmd = NULL;
    cmd = add_command(SFT_CMD_LOCATION);
    cmd->args.loc.code_addr = code;
    cmd->args.loc.code_size = code_size;
    cmd->args.loc.reg_id_scope = info->scope;
    cmd->args.loc.addr_size =  info->scope.elf64? 64 : 32;
    return cmd;
}
#endif

static int get_stack_pointer_register_id(void) {
    switch (info->scope.machine) {
    case EM_386: return 4;
    case EM_X86_64: return 7;
    }
    return -1;
}

static int get_return_value_register_id(void) {
    switch (info->scope.machine) {
    case EM_386: return 0;
    case EM_X86_64: return 0;
    }
    return -1;
}

static RegisterDefinition * find_register(int id) {
    if (id < 0) return NULL;
    return get_reg_by_id(info->ctx, id, &info->scope);
}

static int c_call_cmds(void) {
    unsigned i;
    unsigned sp_offs = 0;
    int * reg_arg = (int *)tmp_alloc_zero(sizeof(int) * info->args_cnt);

    RegisterDefinition * reg_rv = find_register(get_return_value_register_id());
    RegisterDefinition * reg_sp = find_register(get_stack_pointer_register_id());
    RegisterDefinition * reg_pc = get_PC_definition(info->ctx);

    if (reg_sp == NULL) {
        set_errno(ERR_OTHER, "Don't know stack pointer register");
        return -1;
    }
    if (reg_pc == NULL) {
        set_errno(ERR_OTHER, "Don't know instruction pointer register");
        return -1;
    }
    if (reg_rv == NULL) {
        set_errno(ERR_OTHER, "Don't know function return value register");
        return -1;
    }

    info->stak_pointer = reg_sp;

    if (info->scope.machine == EM_386) {
        info->stack_alignment = 8;
    }
    else if (info->scope.machine == EM_X86_64) {
        unsigned reg_args_cnt = 0;

        info->stack_alignment = 16;
        info->red_zone_size = 128;

        /* Assign arguments to registers */
        for (i = 0; i < info->args_cnt; i++) {
            unsigned arg_no = i;
            switch (arg_class_actual[arg_no]) {
            case TYPE_CLASS_CARDINAL:
            case TYPE_CLASS_INTEGER:
            case TYPE_CLASS_POINTER:
            case TYPE_CLASS_ENUMERATION:
            case TYPE_CLASS_ARRAY:
                switch (arg_class_formal[arg_no]) {
                case TYPE_CLASS_CARDINAL:
                case TYPE_CLASS_INTEGER:
                case TYPE_CLASS_POINTER:
                case TYPE_CLASS_ENUMERATION:
                case TYPE_CLASS_ARRAY:
                    if (reg_args_cnt < sizeof(reg_arg_ids) / sizeof(int)) {
                        add_command(SFT_CMD_ARG)->args.arg_no = FUNCCALL_ARG_ARGS + arg_no;
                        add_command(SFT_CMD_WR_REG)->args.reg = find_register(reg_arg_ids[reg_args_cnt++]);
                        reg_arg[i] = 1;
                    }
                    break;
                }
                break;
            }
        }
    }

    /* Push arguments to call stack */
    for (i = 0; i < info->args_cnt; i++) {
        unsigned arg_no = info->args_cnt - i - 1;
        if (reg_arg[arg_no]) continue;
        switch (arg_class_actual[arg_no]) {
        case TYPE_CLASS_CARDINAL:
        case TYPE_CLASS_INTEGER:
        case TYPE_CLASS_POINTER:
        case TYPE_CLASS_ENUMERATION:
        case TYPE_CLASS_ARRAY:
            switch (arg_class_formal[arg_no]) {
            case TYPE_CLASS_CARDINAL:
            case TYPE_CLASS_INTEGER:
            case TYPE_CLASS_POINTER:
            case TYPE_CLASS_ENUMERATION:
            case TYPE_CLASS_ARRAY:
                sp_offs += (unsigned)arg_size_formal[arg_no];
                while (sp_offs % (info->scope.elf64 ? 8 : 4) != 0) sp_offs++;
                add_command(SFT_CMD_RD_REG)->args.reg = reg_sp;
                add_command(SFT_CMD_NUMBER)->args.num = sp_offs;
                add_command(SFT_CMD_SUB);
                add_command(SFT_CMD_ARG)->args.arg_no = FUNCCALL_ARG_ARGS + arg_no;
                add_command(SFT_CMD_WR_MEM)->args.mem.size = (size_t)arg_size_formal[arg_no];
                break;
            default:
                set_errno(ERR_OTHER, "Unsupported argument type");
                return -1;
            }
            break;
        default:
            set_errno(ERR_OTHER, "Unsupported argument type");
            return -1;
        }
    }

    /* Push current PC to the stack as return address */
    sp_offs += reg_pc->size;
    add_command(SFT_CMD_RD_REG)->args.reg = reg_sp;
    add_command(SFT_CMD_NUMBER)->args.num = sp_offs;
    add_command(SFT_CMD_SUB);
    add_command(SFT_CMD_RD_REG)->args.reg = reg_pc;
    add_command(SFT_CMD_WR_MEM)->args.mem.size = reg_pc->size;

    /* Update stack pointer register */
    add_command(SFT_CMD_RD_REG)->args.reg = reg_sp;
    add_command(SFT_CMD_NUMBER)->args.num = sp_offs;
    add_command(SFT_CMD_SUB);
    add_command(SFT_CMD_WR_REG)->args.reg = reg_sp;

    /* Execute the call */
    add_command(SFT_CMD_FCALL);

    /* Get function return value */
    if (func_return_size == 0) return 0;
    switch (func_return_type_class) {
    case TYPE_CLASS_CARDINAL:
    case TYPE_CLASS_INTEGER:
    case TYPE_CLASS_POINTER:
    case TYPE_CLASS_ENUMERATION:
    case TYPE_CLASS_ARRAY:
        add_command(SFT_CMD_RD_REG)->args.reg = reg_rv;
        break;
    default:
        set_errno(ERR_OTHER, "Unsupported return type");
        return -1;
    }
    return 0;
}

static void save_registers(void) {
    unsigned cnt = 0;
    RegisterDefinition * regs = get_reg_definitions(info->ctx);
    if (regs != NULL) {
        RegisterDefinition * r;
        for (r = regs; r->name != NULL; r++) {
            if (r->dwarf_id < 0) continue;
            if (r->size == 0) continue;
            cnt++;
        }
        info->saveregs = (RegisterDefinition **)tmp_alloc(sizeof(RegisterDefinition *) * cnt);
        for (r = regs; r->name != NULL; r++) {
            if (r->dwarf_id < 0) continue;
            if (r->size == 0) continue;
            info->saveregs[info->saveregs_cnt++] = r;
        }
    }
    assert(info->saveregs_cnt == cnt);
}

int get_function_call_location_expression(FunctionCallInfo * arg_info) {
    unsigned i;
    int sym_class = SYM_CLASS_UNKNOWN;

    info = arg_info;
    trace_cmds_cnt = 0;
    trace_cmds_max = 0;
    trace_cmds = NULL;

    if (get_symbol_class(info->func, &sym_class) < 0) return -1;
    if (sym_class == SYM_CLASS_FUNCTION) {
        if (get_symbol_type(info->func, &func_type) < 0) return -1;
    }
    else if (sym_class == SYM_CLASS_TYPE) {
        func_type = (Symbol *)info->func;
    }
    else {
        set_errno(ERR_OTHER, "Invalid function symbol");
        return -1;
    }
    if (get_symbol_children(func_type, &func_args, &func_args_cnt) < 0) return -1;
    if (get_symbol_base_type(func_type, &func_return_type) < 0) return -1;
    if (get_symbol_size(func_return_type, &func_return_size) < 0) return -1;
    if (get_symbol_type_class(func_return_type, &func_return_type_class) < 0) return -1;

    arg_class_formal = (int *)tmp_alloc(sizeof(int) * info->args_cnt);
    arg_class_actual = (int *)tmp_alloc(sizeof(int) * info->args_cnt);
    arg_size_formal = (ContextAddress *)tmp_alloc(sizeof(ContextAddress) * info->args_cnt);
    for (i = 0; i < info->args_cnt; i++) {
        const Symbol * s = info->args[i];
        arg_class_formal[i] = TYPE_CLASS_INTEGER;
        arg_class_actual[i] = TYPE_CLASS_INTEGER;
        arg_size_formal[i] = info->scope.elf64 ? 8 : 4;
        /* If argument type is not given, assume 'int' */
        if (s != NULL && get_symbol_type_class(s, arg_class_actual + i) < 0) return -1;
        if (i < (unsigned)func_args_cnt) {
            if (get_symbol_type_class(func_args[i], arg_class_formal + i) < 0) return -1;
            if (get_symbol_size(func_args[i], arg_size_formal + i) < 0) return -1;
        }
    }

    switch (info->scope.os_abi) {
    case 0:
        if (c_call_cmds() < 0) return -1;
        break;
    default:
        set_errno(ERR_OTHER, "Unsupported ABI code");
        return -1;
    }

    if (trace_cmds_cnt > 0) {
        save_registers();
        info->cmds = trace_cmds;
        info->cmds_cnt = trace_cmds_cnt;
        return 0;
    }
    set_errno(ERR_OTHER, "Calling functions is not supported");
    return -1;
}

#endif /* ENABLE_Symbols && ENABLE_DebugContext */
