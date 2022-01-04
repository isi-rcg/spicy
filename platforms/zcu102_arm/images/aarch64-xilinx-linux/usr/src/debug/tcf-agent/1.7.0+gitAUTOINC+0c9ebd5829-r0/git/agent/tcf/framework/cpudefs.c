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
 * This module contains definitions of target CPU registers and stack frames.
 */

#include <tcf/config.h>

#if ENABLE_DebugContext

#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include <tcf/framework/cpudefs.h>
#include <tcf/framework/errors.h>
#include <tcf/framework/context.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/exceptions.h>
#include <tcf/services/symbols.h>

#include <tcf/framework/cpudefs-ext.h>

void create_reg_children_refs(RegisterDefinition * defs) {
#if ENABLE_RegisterChildrenRefs
    RegisterDefinition * r = NULL;
    RegisterDefinition ** p = NULL;
    RegisterDefinition * c = NULL;
    unsigned size = 0;
    assert(defs->parent == NULL);
    assert(defs->children == NULL);
    for (r = defs; r->name != NULL; r++) size++;
    p = (RegisterDefinition **)tmp_alloc_zero(sizeof(RegisterDefinition *) * size);
    for (r = defs; r->name != NULL; r++) {
        assert(r->sibling == NULL);
        if (r->parent == NULL) {
            if (c != NULL) c->sibling = r;
            c = r;
        }
        else {
            RegisterDefinition ** y = p + (r->parent - defs);
            if (*y == NULL) r->parent->children = r;
            else (*y)->sibling = r;
            *y = r;
        }
    }
#endif
}

int read_reg_value(StackFrame * frame, RegisterDefinition * reg_def, uint64_t * value) {
    uint8_t buf[8];
    if (reg_def == NULL) {
        set_errno(ERR_INV_CONTEXT, "Invalid register");
        return -1;
    }
    if (frame == NULL) {
        set_errno(ERR_INV_CONTEXT, "Invalid stack frame");
        return -1;
    }
    if (reg_def->size > sizeof(buf)) {
        errno = ERR_INV_DATA_SIZE;
        return -1;
    }
    if (read_reg_bytes(frame, reg_def, 0, reg_def->size, buf) < 0) return -1;
    if (value != NULL) {
        size_t i;
        uint64_t n = 0;
        for (i = 0; i < reg_def->size; i++) {
            n = n << 8;
            n |= buf[reg_def->big_endian ? i : reg_def->size - i - 1];
        }
        *value = n;
    }
    return 0;
}

int write_reg_value(StackFrame * frame, RegisterDefinition * reg_def, uint64_t value) {
    size_t i;
    uint8_t buf[8];
    if (reg_def == NULL) {
        set_errno(ERR_INV_CONTEXT, "Invalid register");
        return -1;
    }
    if (frame == NULL) {
        set_errno(ERR_INV_CONTEXT, "Invalid stack frame");
        return -1;
    }
    if (reg_def->size > sizeof(buf)) {
        errno = ERR_INV_DATA_SIZE;
        return -1;
    }
    for (i = 0; i < reg_def->size; i++) {
        buf[reg_def->big_endian ? reg_def->size - i - 1 : i] = (uint8_t)value;
        value = value >> 8;
    }
    if (write_reg_bytes(frame, reg_def, 0, reg_def->size, buf) < 0) return -1;
    if (!frame->is_top_frame) frame->has_reg_data = 1;
    return 0;
}

ContextAddress get_regs_PC(Context * ctx) {
    ContextAddress pc = 0;
    if (get_PC(ctx, &pc) < 0) return 0;
    return pc;
}

void set_regs_PC(Context * ctx, ContextAddress pc) {
    set_PC(ctx, pc);
}

int get_PC(Context * ctx, ContextAddress * p) {
    size_t i;
    uint8_t buf[8];
    ContextAddress pc = 0;
    RegisterDefinition * def = get_PC_definition(ctx);
    if (def == NULL) {
        set_errno(ERR_OTHER, "Cannot read PC: no such register");
        return -1;
    }
    if (def->size > sizeof(buf)) {
        set_errno(ERR_OTHER, "Cannot read PC: register is too large");
        return -1;
    }
    if (context_read_reg(ctx, def, 0, def->size, buf) < 0) return -1;
    for (i = 0; i < def->size; i++) {
        pc = pc << 8;
        pc |= buf[def->big_endian ? i : def->size - i - 1];
    }
    *p = pc;
    return 0;
}

int set_PC(Context * ctx, ContextAddress pc) {
    size_t i;
    uint8_t buf[8];
    RegisterDefinition * def = get_PC_definition(ctx);
    if (def == NULL) {
        set_errno(ERR_OTHER, "Cannot write PC: no such register");
        return -1;
    }
    if (def->size > sizeof(buf)) {
        set_errno(ERR_OTHER, "Cannot write PC: register is too large");
        return -1;
    }
    for (i = 0; i < def->size; i++) {
        buf[def->big_endian ? def->size - i - 1 : i] = (uint8_t)pc;
        pc = pc >> 8;
    }
    return context_write_reg(ctx, def, 0, def->size, buf);
}

int id2frame(const char * id, Context ** ctx, int * frame) {
    int f = 0;
    Context * c = NULL;

    if (*id++ != 'F') {
        errno = ERR_INV_CONTEXT;
        return -1;
    }
    if (*id++ != 'P') {
        errno = ERR_INV_CONTEXT;
        return -1;
    }
    while (*id != '.') {
        if (*id < '0' || *id > '9') {
            errno = ERR_INV_CONTEXT;
            return -1;
        }
        f = f * 10 + (*id++ - '0');
    }
    id++;
    c = id2ctx(id);
    if (c == NULL || !context_has_state(c)) {
        errno = ERR_INV_CONTEXT;
        return -1;
    }
    *ctx = c;
    *frame = f;
    return 0;
}

const char * frame2id(Context * ctx, int frame) {
    assert(frame >= 0);
    return tmp_printf("FP%d.%s", frame, ctx->id);
}

const char * register2id(Context * ctx, int frame, RegisterDefinition * reg) {
    RegisterDefinition * defs = get_reg_definitions(ctx);
    assert(defs != NULL);
    assert(reg >= defs);
    if (frame < 0) return tmp_printf("R%d.%s", (int)(reg - defs), ctx->id);
    return tmp_printf("R%d@%d.%s", (int)(reg - defs), frame, ctx->id);
}

int id2reg_num(const char * id, const char ** ctx_id, int * frame, unsigned * reg_num) {
    *ctx_id = NULL;
    *frame = STACK_TOP_FRAME;
    *reg_num = 0;
    if (*id++ != 'R') {
        errno = ERR_INV_CONTEXT;
        return -1;
    }
    while (*id != '.' && *id != '@') {
        if (*id >= '0' && *id <= '9') {
            *reg_num = *reg_num * 10 + (*id++ - '0');
        }
        else {
            errno = ERR_INV_CONTEXT;
            return -1;
        }
    }
    if (*id == '@') {
        int n = 0;
        id++;
        while (*id != '.') {
            if (*id >= '0' && *id <= '9') {
                n = n * 10 + (*id++ - '0');
            }
            else {
                errno = ERR_INV_CONTEXT;
                return -1;
            }
        }
        *frame = n;
    }
    id++;
    *ctx_id = id;
    return 0;
}

int id2register(const char * id, Context ** ctx, int * frame, RegisterDefinition ** reg_def) {
    const char * ctx_id = NULL;
    RegisterDefinition * defs = NULL;
    unsigned reg_num = 0;

    *ctx = NULL;
    *reg_def = NULL;

    if (id2reg_num(id, &ctx_id, frame, &reg_num) < 0) return -1;

    *ctx = id2ctx(ctx_id);
    if (*ctx == NULL) {
        errno = ERR_INV_CONTEXT;
        return -1;
    }
    if ((*ctx)->exited) {
        errno = ERR_ALREADY_EXITED;
        return -1;
    }
    defs = get_reg_definitions(*ctx);
    if (defs == NULL) {
        set_errno(ERR_OTHER, "Context has no registers");
        return -1;
    }
    *reg_def = defs + reg_num;
    return 0;
}

static void location_expression_error(void) {
    str_exception(ERR_OTHER, "Invalid location expression");
}

LocationExpressionState * evaluate_location_expression(Context * ctx, StackFrame * frame,
                                     LocationExpressionCommand * cmds, unsigned cmd_cnt,
                                     uint64_t * args, unsigned args_cnt) {
    unsigned i;
    unsigned stk_pos = 0;
    unsigned stk_max = 0;
    uint64_t * stk = NULL;
    LocationExpressionState * state = (LocationExpressionState *)tmp_alloc_zero(sizeof(LocationExpressionState));

    state->ctx = ctx;
    state->stack_frame = frame;
    state->args = args;
    state->args_cnt = args_cnt;
    for (i = 0; i < cmd_cnt && state->sft_cmd == NULL; i++) {
        LocationExpressionCommand * cmd = cmds + i;
        if (stk_pos >= stk_max) {
            stk_max += 4;
            stk = (uint64_t *)tmp_realloc(stk, sizeof(uint64_t) * stk_max);
        }
        switch (cmd->cmd) {
        case SFT_CMD_NUMBER:
            stk[stk_pos++] = cmd->args.num;
            break;
        case SFT_CMD_RD_REG:
            if (read_reg_value(frame, cmd->args.reg, stk + stk_pos) < 0) exception(errno);
            stk_pos++;
            break;
        case SFT_CMD_RD_REG_PCXI_TRICORE:
            {
            uint64_t offset;
            uint64_t segment;
            uint64_t pcxi;
            if (read_reg_value(frame, cmd->args.reg, &pcxi) < 0) exception(errno);
            /* PCXI is used as CFA and needs to be decyphered into order to point
             * to the CSA (area of memory where registered were saved when CALL is made) */
            offset = (pcxi & 0xFFFF) << 6;
            segment = (pcxi & 0xF0000) << 12;
            pcxi = offset | segment;
            *(stk + stk_pos) = pcxi;
            stk_pos++;
            }
            break;
        case SFT_CMD_WR_REG:
            if (stk_pos < 1) location_expression_error();
            if (write_reg_value(frame, cmd->args.reg, *(stk + stk_pos - 1)) < 0) exception(errno);
            stk_pos--;
            break;
        case SFT_CMD_FP:
            if (frame == NULL) str_exception(ERR_INV_CONTEXT, "Invalid stack frame");
            stk[stk_pos++] = frame->fp;
            break;
        case SFT_CMD_LOAD:
            if (state->pieces_cnt > 0) {
                size_t j;
                size_t size = 0;
                uint64_t n = 0;
                void * buf = NULL;

                read_location_pieces(state->ctx, state->stack_frame,
                    state->pieces, state->pieces_cnt, cmd->args.mem.big_endian, &buf, &size);
                state->pieces_cnt = 0;
                for (j = 0; j < size; j++) {
                    n = (n << 8) | ((uint8_t *)buf)[cmd->args.mem.big_endian ? j : size - j - 1];
                }
                stk[stk_pos++] = n;
                break;
            }
            /* Fall through */
        case SFT_CMD_RD_MEM:
            if (stk_pos < 1) location_expression_error();
            {
                size_t j;
                size_t size = cmd->args.mem.size;
                uint64_t n = 0;

                if (size <= sizeof(n)) {
                    uint8_t buf[8];
                    if (context_read_mem(ctx, (ContextAddress)stk[stk_pos - 1], buf, size) < 0) exception(errno);
                    for (j = 0; j < size; j++) {
                        n = (n << 8) | buf[cmd->args.mem.big_endian ? j : size - j - 1];
                    }
                    stk[stk_pos - 1] = n;
                }
                else if (state->pieces_cnt == 0 && i + 1 == cmd_cnt && stk_pos == 1) {
                    LocationPiece * piece = NULL;
                    if (state->pieces_cnt >= state->pieces_max) {
                        state->pieces_max += 4;
                        state->pieces = (LocationPiece *)tmp_realloc(state->pieces, state->pieces_max * sizeof(LocationPiece));
                    }
                    piece = state->pieces + state->pieces_cnt++;
                    memset(piece, 0, sizeof(LocationPiece));
                    piece->addr = (ContextAddress)stk[stk_pos - 1];
                    piece->size = size;
                    stk_pos--;
                }
                else {
                    exception(ERR_INV_DATA_SIZE);
                }
            }
            break;
        case SFT_CMD_WR_MEM:
            if (stk_pos < 2) location_expression_error();
            {
                size_t j;
                size_t size = cmd->args.mem.size;
                uint64_t n = stk[stk_pos - 1];
                uint8_t buf[8];

                assert(size <= sizeof(buf));
                for (j = 0; j < size; j++) {
                    buf[cmd->args.mem.big_endian ? size - j - 1 : j] = (uint8_t)n;
                    n >>= 8;
                }
                if (context_write_mem(ctx, (ContextAddress)stk[stk_pos - 2], buf, size) < 0) exception(errno);
                stk_pos -= 2;
            }
            break;
        case SFT_CMD_ADD:
            if (stk_pos < 2) location_expression_error();
            stk[stk_pos - 2] = stk[stk_pos - 2] + stk[stk_pos - 1];
            stk_pos--;
            break;
        case SFT_CMD_SUB:
            if (stk_pos < 2) location_expression_error();
            stk[stk_pos - 2] = stk[stk_pos - 2] - stk[stk_pos - 1];
            stk_pos--;
            break;
        case SFT_CMD_MUL:
            if (stk_pos < 2) location_expression_error();
            stk[stk_pos - 2] = stk[stk_pos - 2] * stk[stk_pos - 1];
            stk_pos--;
            break;
        case SFT_CMD_DIV:
            if (stk_pos < 2) location_expression_error();
            if (stk[stk_pos - 1] == 0) str_exception(ERR_OTHER, "Division by zero in location expression");
            stk[stk_pos - 2] = stk[stk_pos - 2] / stk[stk_pos - 1];
            stk_pos--;
            break;
        case SFT_CMD_AND:
            if (stk_pos < 2) location_expression_error();
            stk[stk_pos - 2] = stk[stk_pos - 2] & stk[stk_pos - 1];
            stk_pos--;
            break;
        case SFT_CMD_OR:
            if (stk_pos < 2) location_expression_error();
            stk[stk_pos - 2] = stk[stk_pos - 2] | stk[stk_pos - 1];
            stk_pos--;
            break;
        case SFT_CMD_XOR:
            if (stk_pos < 2) location_expression_error();
            stk[stk_pos - 2] = stk[stk_pos - 2] ^ stk[stk_pos - 1];
            stk_pos--;
            break;
        case SFT_CMD_NEG:
            if (stk_pos < 1) location_expression_error();
            stk[stk_pos - 1] = ~stk[stk_pos - 1];
            stk_pos--;
            break;
        case SFT_CMD_GE:
            if (stk_pos < 2) location_expression_error();
            stk[stk_pos - 2] = stk[stk_pos - 2] >= stk[stk_pos - 1];
            stk_pos--;
            break;
        case SFT_CMD_GT:
            if (stk_pos < 2) location_expression_error();
            stk[stk_pos - 2] = stk[stk_pos - 2] > stk[stk_pos - 1];
            stk_pos--;
            break;
        case SFT_CMD_LE:
            if (stk_pos < 2) location_expression_error();
            stk[stk_pos - 2] = stk[stk_pos - 2] <= stk[stk_pos - 1];
            stk_pos--;
            break;
        case SFT_CMD_LT:
            if (stk_pos < 2) location_expression_error();
            stk[stk_pos - 2] = stk[stk_pos - 2] < stk[stk_pos - 1];
            stk_pos--;
            break;
        case SFT_CMD_SHL:
            if (stk_pos < 2) location_expression_error();
            stk[stk_pos - 2] <<= stk[stk_pos - 1];
            stk_pos--;
            break;
        case SFT_CMD_SHR:
            if (stk_pos < 2) location_expression_error();
            stk[stk_pos - 2] >>= stk[stk_pos - 1];
            stk_pos--;
            break;
        case SFT_CMD_ARG:
            if (cmd->args.arg_no >= args_cnt) location_expression_error();
            stk[stk_pos++] = args[cmd->args.arg_no];
            break;
        case SFT_CMD_SET_ARG:
            if (stk_pos < 1) location_expression_error();
            if (cmd->args.arg_no >= args_cnt) {
                unsigned cnt = cmd->args.arg_no + 1;
                uint64_t * buf = (uint64_t *)tmp_alloc_zero(sizeof(uint64_t) * cnt);
                memcpy(buf, args, sizeof(uint64_t) * args_cnt);
                args_cnt = cnt;
                args = buf;
            }
            args[cmd->args.arg_no] = stk[stk_pos - 1];
            stk_pos--;
            break;
        case SFT_CMD_LOCATION:
            state->stk = stk;
            state->type_stk = (uint8_t *)tmp_alloc_zero(stk_max);
            state->stk_pos = stk_pos;
            state->stk_max = stk_max;
            state->reg_id_scope = cmd->args.loc.reg_id_scope;
            state->code = cmd->args.loc.code_addr;
            state->code_len = cmd->args.loc.code_size;
            state->code_pos = 0;
            state->addr_size = cmd->args.loc.addr_size;
            state->client_op = NULL;
            if (cmd->args.loc.func(state) < 0) exception(errno);
            memset(&state->reg_id_scope, 0, sizeof(state->reg_id_scope));
            state->code = NULL;
            state->code_len = 0;
            state->code_pos = 0;
            state->addr_size = 0;
            state->client_op = NULL;
            stk_max = state->stk_max;
            stk_pos = state->stk_pos;
            stk = state->stk;
            break;
        case SFT_CMD_FCALL:
            state->sft_cmd = cmd;
            break;
        case SFT_CMD_PIECE:
            {
                LocationPiece * piece = NULL;
                if (state->pieces_cnt >= state->pieces_max) {
                    state->pieces_max += 4;
                    state->pieces = (LocationPiece *)tmp_realloc(state->pieces, state->pieces_max * sizeof(LocationPiece));
                }
                piece = state->pieces + state->pieces_cnt++;
                memset(piece, 0, sizeof(LocationPiece));
                if (cmd->args.piece.bit_offs == 0 && cmd->args.piece.bit_size % 8 == 0) {
                    piece->size = cmd->args.piece.bit_size / 8;
                }
                else {
                    piece->bit_offs = cmd->args.piece.bit_offs;
                    piece->bit_size = cmd->args.piece.bit_size;
                }
                if (cmd->args.piece.reg != NULL || cmd->args.piece.value != NULL) {
                    piece->reg = cmd->args.piece.reg;
                    piece->value = cmd->args.piece.value;
                }
                else if (stk_pos == 0) {
                    location_expression_error();
                }
                else {
                    stk_pos--;
                    piece->addr = (ContextAddress)stk[stk_pos];
                }
            }
            break;
        default:
            location_expression_error();
            break;
        }
    }
    if (stk_pos == 2) {
        /* This looks like a bug in GNU Ada 4.9.2 20141023 for GNAT Pro 7.3.0w
         * See https://bugs.eclipse.org/bugs/show_bug.cgi?id=449802
         */
        stk[0] += stk[1];
        stk_pos = 1;
    }
    state->stk = stk;
    state->stk_pos = stk_pos;
    state->stk_max = stk_max;
    return state;
}

#define bit_mask(n) (1u << (big_endian ? 7 - (n) % 8 : (n) % 8))

void read_location_pieces(Context * ctx, StackFrame * frame,
            LocationPiece * pieces, unsigned pieces_cnt, int big_endian,
            void ** value, size_t * size) {
    /* Note: 'big_endian' should match endianness of LocationPiece.value in 'pieces' */
    uint8_t * bf = NULL;
    size_t bf_size = 0;
    unsigned bf_bits = 0;
    unsigned bf_offs = 0;
    unsigned n = 0;
    unsigned i = 0;
    while (i < pieces_cnt) {
        LocationPiece * piece = pieces + i++;
        bf_bits += piece->bit_size ? piece->bit_size : piece->size * 8;
    }
    bf_size = (size_t)((bf_bits + 7) / 8);
    bf = (uint8_t *)tmp_alloc_zero(bf_size);
    if (big_endian) bf_offs = bf_size * 8 - bf_bits;
    while (n < pieces_cnt) {
        LocationPiece * piece = pieces + n++;
        unsigned piece_size = piece->size ? piece->size : (piece->bit_offs + piece->bit_size + 7) / 8;
        unsigned piece_bits = piece->bit_size ? piece->bit_size : piece->size * 8;
        uint8_t * pbf = NULL;
        uint8_t * rbf = NULL;
        if (piece->optimized_away) {
            set_errno(ERR_OTHER, "Cannot get symbol value: optimized away");
            exception(errno);
        }
        if (piece->implicit_pointer) {
            set_errno(ERR_OTHER, "Cannot get symbol value: implicit pointer");
            exception(errno);
        }
        if (piece->reg) {
            if (piece->reg->size < piece_size) {
                rbf = pbf = (uint8_t *)tmp_alloc_zero(piece_size);
                if (big_endian) rbf += piece_size - piece->reg->size;
            }
            else {
                rbf = pbf = (uint8_t *)tmp_alloc(piece->reg->size);
            }
            if (frame == NULL) {
                if (context_read_reg(ctx, piece->reg, 0, piece->reg->size, rbf) < 0) exception(errno);
            }
            else {
                if (read_reg_bytes(frame, piece->reg, 0, piece->reg->size, rbf) < 0) exception(errno);
            }
            if (!piece->reg->big_endian != !big_endian) swap_bytes(rbf, piece->reg->size);
        }
        else if (piece->value) {
            pbf = (uint8_t *)piece->value;
        }
        else {
            pbf = (uint8_t *)tmp_alloc(piece_size);
            if (context_read_mem(ctx, piece->addr, pbf, piece_size) < 0) exception(errno);
        }
        for (i = piece->bit_offs; i < piece->bit_offs + piece_bits;  i++) {
            if (pbf[i / 8] & bit_mask(i)) bf[bf_offs / 8] |= bit_mask(bf_offs);
            bf_offs++;
        }
    }

    *value = bf;
    *size = bf_size;
}

void write_location_pieces(Context * ctx, StackFrame * frame,
            LocationPiece * pieces, unsigned pieces_cnt, int big_endian,
            void * value, size_t size) {
    uint8_t * bf = (uint8_t *)value;
    size_t bf_size = 0;
    unsigned bf_bits = 0;
    unsigned bf_offs = 0;
    unsigned n = 0;
    unsigned i = 0;
    while (i < pieces_cnt) {
        LocationPiece * piece = pieces + i++;
        bf_bits += piece->bit_size ? piece->bit_size : piece->size * 8;
    }
    bf_size = (size_t)((bf_bits + 7) / 8);
    if (big_endian && size >= bf_size) bf_offs = size * 8 - bf_bits;
    while (n < pieces_cnt) {
        LocationPiece * piece = pieces + n++;
        unsigned piece_size = piece->size ? piece->size : (piece->bit_offs + piece->bit_size + 7) / 8;
        unsigned piece_bits = piece->bit_size ? piece->bit_size : piece->size * 8;
        uint8_t * pbf = NULL;
        uint8_t * rbf = NULL;
        if (piece->optimized_away) {
            set_errno(ERR_OTHER, "Cannot set symbol value: optimized away");
            exception(errno);
        }
        if (piece->implicit_pointer) {
            set_errno(ERR_OTHER, "Cannot set symbol value: implicit pointer");
            exception(errno);
        }
        if (piece->reg) {
            if (piece->reg->size < piece_size) {
                rbf = pbf = (uint8_t *)tmp_alloc_zero(piece_size);
                if (big_endian) rbf += piece_size - piece->reg->size;
            }
            else {
                rbf = pbf = (uint8_t *)tmp_alloc(piece->reg->size);
            }
            if (frame == NULL) {
                if (context_read_reg(ctx, piece->reg, 0, piece->reg->size, rbf) < 0) exception(errno);
            }
            else {
                if (read_reg_bytes(frame, piece->reg, 0, piece->reg->size, rbf) < 0) exception(errno);
            }
            if (!piece->reg->big_endian != !big_endian) swap_bytes(rbf, piece->reg->size);
        }
        else if (piece->value) {
            str_exception(ERR_OTHER, "Cannot write to a constant value");
        }
        else {
            pbf = (uint8_t *)tmp_alloc(piece_size);
            if (context_read_mem(ctx, piece->addr, pbf, piece_size) < 0) exception(errno);
        }
        for (i = piece->bit_offs; i < piece->bit_offs + piece_bits;  i++) {
            if (bf_offs / 8 >= size) {
                /* Leave unchanged? */
            }
            else if (bf[bf_offs / 8] & bit_mask(bf_offs)) {
                pbf[i / 8] |=  bit_mask(i);
            }
            else {
                pbf[i / 8] &= ~bit_mask(i);
            }
            bf_offs++;
        }
        if (piece->reg) {
            if (!piece->reg->big_endian != !big_endian) swap_bytes(rbf, piece->reg->size);
            if (frame == NULL) {
                if (context_write_reg(ctx, piece->reg, 0, piece->reg->size, rbf) < 0) exception(errno);
            }
            else {
                if (write_reg_bytes(frame, piece->reg, 0, piece->reg->size, rbf) < 0) exception(errno);
                if (!frame->is_top_frame) frame->has_reg_data = 1;
            }
        }
        else if (piece->value) {
            assert(0);
        }
        else {
            if (context_write_mem(ctx, piece->addr, pbf, piece_size) < 0) exception(errno);
        }
    }
}

#if !defined(ENABLE_external_stepping_mode) || !ENABLE_external_stepping_mode
int cpu_enable_stepping_mode(Context * ctx, uint32_t * is_cont) {
    * is_cont = 0;
    return 0;
}

int cpu_disable_stepping_mode(Context * ctx) {
    return 0;
}
#endif

#if !defined(ENABLE_HardwareBreakpoints) || !ENABLE_HardwareBreakpoints
int cpu_bp_get_capabilities(Context * ctx) {
    return 0;
}

int cpu_bp_plant(ContextBreakpoint * bp) {
    errno = ERR_UNSUPPORTED;
    return -1;
}

int cpu_bp_remove(ContextBreakpoint * bp) {
    errno = ERR_UNSUPPORTED;
    return -1;
}

int cpu_bp_on_resume(Context * ctx, int * single_step) {
    return 0;
}

int cpu_bp_on_suspend(Context * ctx, int * triggered) {
    return 0;
}
#endif

void ini_cpu_disassembler(Context * cpu) {
#if defined(ENABLE_add_cpudefs_disassembler) && ENABLE_add_cpudefs_disassembler
    add_cpudefs_disassembler(cpu);
#endif
}

void ini_cpudefs(void) {
#if defined(ENABLE_ini_cpudefs_mdep) && ENABLE_ini_cpudefs_mdep
    ini_cpudefs_mdep();
#endif
}

#endif /* ENABLE_DebugContext */
