/*******************************************************************************
 * Copyright (c) 2011-2018 Wind River Systems, Inc. and others.
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
 * A virtual machine that executes DWARF expressions.
 */

#include <tcf/config.h>

#if ENABLE_DebugContext

#include <errno.h>
#include <tcf/framework/trace.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/exceptions.h>
#include <tcf/services/stacktrace.h>
#include <tcf/services/symbols.h>
#include <tcf/services/dwarf.h>
#include <tcf/services/vm.h>

#define check_e_stack(n) { if (state->stk_pos < n) inv_dwarf("Invalid location expression stack"); }

static LocationExpressionState * state = NULL;
static RegisterDefinition * reg_def = NULL;
static void * value_addr = NULL;
static size_t value_size = 0;
static unsigned implicit_pointer = 0;
static uint8_t * code = NULL;
static size_t code_pos = 0;
static size_t code_len = 0;

static void inv_dwarf(const char * msg) {
    str_exception(ERR_INV_DWARF, msg);
}

static uint64_t read_memory(uint64_t addr, size_t size) {
    size_t i;
    uint64_t n = 0;
    uint8_t buf[8];

    if (context_read_mem(state->ctx, (ContextAddress)addr, buf, size) < 0) exception(errno);
    for (i = 0; i < size; i++) {
        n = (n << 8) | buf[state->reg_id_scope.big_endian ? i : size - i - 1];
    }
    return n;
}

static uint8_t read_u1(void) {
    if (code_pos >= code_len) inv_dwarf("Invalid command");
    return code[code_pos++];
}

static uint16_t read_u2(void) {
    uint16_t x0 = read_u1();
    uint16_t x1 = read_u1();
    return state->reg_id_scope.big_endian ? (x0 << 8) | x1 : x0 | (x1 << 8);
}

static uint32_t read_u4(void) {
    uint32_t x0 = read_u2();
    uint32_t x1 = read_u2();
    return state->reg_id_scope.big_endian ? (x0 << 16) | x1 : x0 | (x1 << 16);
}

static uint64_t read_u8(void) {
    uint64_t x0 = read_u4();
    uint64_t x1 = read_u4();
    return state->reg_id_scope.big_endian ? (x0 << 32) | x1 : x0 | (x1 << 32);
}

static uint32_t read_u4leb128(void) {
    uint32_t res = 0;
    int i = 0;
    for (;; i += 7) {
        uint8_t n = read_u1();
        res |= (uint32_t)(n & 0x7Fu) << i;
        if ((n & 0x80) == 0) break;
    }
    return res;
}

static uint64_t read_u8leb128(void) {
    uint64_t res = 0;
    int i = 0;
    for (;; i += 7) {
        uint8_t n = read_u1();
        res |= (uint64_t)(n & 0x7Fu) << i;
        if ((n & 0x80) == 0) break;
    }
    return res;
}

static int64_t read_i8leb128(void) {
    uint64_t res = 0;
    int i = 0;
    for (;; i += 7) {
        uint8_t n = read_u1();
        res |= (uint64_t)(n & 0x7Fu) << i;
        if ((n & 0x80) == 0) {
            res |= -(int64_t)(n & 0x40) << i;
            break;
        }
    }
    return (int64_t)res;
}

static uint64_t read_ia(void) {
    switch (state->addr_size) {
    case 1: return (int8_t)read_u1();
    case 2: return (int16_t)read_u2();
    case 4: return (int32_t)read_u4();
    case 8: return (int64_t)read_u8();
    default: inv_dwarf("Invalid address size");
    }
    return 0;
}

static uint64_t read_ua(void) {
    switch (state->addr_size) {
    case 1: return read_u1();
    case 2: return read_u2();
    case 4: return read_u4();
    case 8: return read_u8();
    default: inv_dwarf("Invalid address size");
    }
    return 0;
}

static LocationPiece * add_piece(void) {
    LocationPiece * piece = NULL;
    if (state->pieces_cnt >= state->pieces_max) {
        state->pieces_max += 4;
        state->pieces = (LocationPiece *)tmp_realloc(state->pieces, state->pieces_max * sizeof(LocationPiece));
    }
    piece = state->pieces + state->pieces_cnt++;
    memset(piece, 0, sizeof(LocationPiece));
    if (reg_def != NULL) {
        piece->reg = reg_def;
        piece->size = reg_def->size;
    }
    else if (value_addr != NULL) {
        piece->value = value_addr;
        piece->size = value_size;
    }
    else if (state->stk_pos == 0) {
        /* An empty location description represents a piece or all of an object that is
         * present in the source but not in the object code (perhaps due to optimization). */
        piece->optimized_away = 1;
    }
    else {
        state->stk_pos--;
        piece->addr = (ContextAddress)state->stk[state->stk_pos];
    }
    piece->implicit_pointer = implicit_pointer;
    implicit_pointer = 0;
    value_addr = NULL;
    reg_def = NULL;
    return piece;
}

static void set_state(LocationExpressionState * s) {
    state = s;
    code = state->code;
    code_pos = state->code_pos;
    code_len = state->code_len;
}

static void get_state(LocationExpressionState * s) {
    s->code_pos = code_pos;
    state = NULL;
    code = NULL;
    code_pos = 0;
    code_len = 0;
}

static int is_end_of_loc_expr(void) {
    return
        code_pos >= code_len ||
        code[code_pos] == OP_piece ||
        code[code_pos] == OP_bit_piece ||
        code[code_pos] == OP_TCF_offset;
}

static void evaluate_expression(void) {
    uint64_t data = 0;
    uint8_t type = 0;

    if (code_len == 0) inv_dwarf("location expression size = 0");

    while (code_pos < code_len) {
        LocationPiece * piece = NULL;
        uint8_t op = code[code_pos++];

        if (state->stk_pos + 4 > state->stk_max) {
            state->stk_max += 8;
            state->stk = (uint64_t *)tmp_realloc(state->stk, sizeof(uint64_t) * state->stk_max);
            state->type_stk = (uint8_t *)tmp_realloc(state->type_stk, sizeof(uint8_t) * state->stk_max);
        }

        switch (op) {
        case OP_deref:
            check_e_stack(1);
            state->stk[state->stk_pos - 1] = read_memory(state->stk[state->stk_pos - 1], state->addr_size);
            state->type_stk[state->stk_pos - 1] = TYPE_CLASS_CARDINAL;
            break;
        case OP_deref2:
            check_e_stack(1);
            state->stk[state->stk_pos - 1] = (int16_t)read_memory(state->stk[state->stk_pos - 1], 2);
            state->type_stk[state->stk_pos - 1] = TYPE_CLASS_CARDINAL;
            break;
        case OP_deref_size:
            check_e_stack(1);
            state->stk[state->stk_pos - 1] = read_memory(state->stk[state->stk_pos - 1], read_u1());
            state->type_stk[state->stk_pos - 1] = TYPE_CLASS_CARDINAL;
            break;
        case OP_const:
            state->stk[state->stk_pos++] = read_ia();
            state->type_stk[state->stk_pos - 1] = TYPE_CLASS_CARDINAL;
            break;
        case OP_const1u:
            state->stk[state->stk_pos++] = read_u1();
            state->type_stk[state->stk_pos - 1] = TYPE_CLASS_CARDINAL;
            break;
        case OP_const1s:
            state->stk[state->stk_pos++] = (int8_t)read_u1();
            state->type_stk[state->stk_pos - 1] = TYPE_CLASS_INTEGER;
            break;
        case OP_const2u:
            state->stk[state->stk_pos++] = read_u2();
            state->type_stk[state->stk_pos - 1] = TYPE_CLASS_CARDINAL;
            break;
        case OP_const2s:
            state->stk[state->stk_pos++] = (int16_t)read_u2();
            state->type_stk[state->stk_pos - 1] = TYPE_CLASS_INTEGER;
            break;
        case OP_const4u:
            state->stk[state->stk_pos++] = read_u4();
            state->type_stk[state->stk_pos - 1] = TYPE_CLASS_CARDINAL;
            break;
        case OP_const4s:
            state->stk[state->stk_pos++] = (int32_t)read_u4();
            state->type_stk[state->stk_pos - 1] = TYPE_CLASS_INTEGER;
            break;
        case OP_const8u:
            state->stk[state->stk_pos++] = read_u8();
            state->type_stk[state->stk_pos - 1] = TYPE_CLASS_CARDINAL;
            break;
        case OP_const8s:
            state->stk[state->stk_pos++] = (int64_t)read_u8();
            state->type_stk[state->stk_pos - 1] = TYPE_CLASS_INTEGER;
            break;
        case OP_constu:
            state->stk[state->stk_pos++] = read_u8leb128();
            state->type_stk[state->stk_pos - 1] = TYPE_CLASS_CARDINAL;
            break;
        case OP_consts:
            state->stk[state->stk_pos++] = read_i8leb128();
            state->type_stk[state->stk_pos - 1] = TYPE_CLASS_INTEGER;
            break;
        case OP_dup:
            check_e_stack(1);
            state->stk[state->stk_pos] = state->stk[state->stk_pos - 1];
            state->type_stk[state->stk_pos] = state->type_stk[state->stk_pos - 1];
            state->stk_pos++;
            break;
        case OP_drop:
            check_e_stack(1);
            state->stk_pos--;
            break;
        case OP_over:
            check_e_stack(2);
            state->stk[state->stk_pos] = state->stk[state->stk_pos - 2];
            state->type_stk[state->stk_pos] = state->type_stk[state->stk_pos - 2];
            state->stk_pos++;
            break;
        case OP_pick:
            {
                unsigned n = read_u1();
                check_e_stack(n + 1);
                state->stk[state->stk_pos] = state->stk[state->stk_pos - n - 1];
                state->type_stk[state->stk_pos] = state->type_stk[state->stk_pos - n - 1];
                state->stk_pos++;
            }
            break;
        case OP_swap:
            check_e_stack(2);
            data = state->stk[state->stk_pos - 1];
            type = state->type_stk[state->stk_pos - 1];
            state->stk[state->stk_pos - 1] = state->stk[state->stk_pos - 2];
            state->type_stk[state->stk_pos - 1] = state->type_stk[state->stk_pos - 2];
            state->stk[state->stk_pos - 2] = data;
            state->type_stk[state->stk_pos - 2] = type;
            break;
        case OP_rot:
            check_e_stack(3);
            data = state->stk[state->stk_pos - 1];
            type = state->type_stk[state->stk_pos - 1];
            state->stk[state->stk_pos - 1] = state->stk[state->stk_pos - 2];
            state->type_stk[state->stk_pos - 1] = state->type_stk[state->stk_pos - 2];
            state->stk[state->stk_pos - 2] = state->stk[state->stk_pos - 3];
            state->type_stk[state->stk_pos - 2] = state->type_stk[state->stk_pos - 3];
            state->stk[state->stk_pos - 3] = data;
            state->type_stk[state->stk_pos - 2] = type;
            break;
        case OP_xderef:
            check_e_stack(2);
            state->stk[state->stk_pos - 2] = read_memory(state->stk[state->stk_pos - 1], state->addr_size);
            state->type_stk[state->stk_pos - 2] = TYPE_CLASS_CARDINAL;
            state->stk_pos--;
            break;
        case OP_xderef_size:
            check_e_stack(2);
            state->stk[state->stk_pos - 2] = read_memory(state->stk[state->stk_pos - 1], read_u1());
            state->type_stk[state->stk_pos - 2] = TYPE_CLASS_CARDINAL;
            state->stk_pos--;
            break;
        case OP_abs:
            check_e_stack(1);
            if ((int64_t)state->stk[state->stk_pos - 1] < 0) {
                state->stk[state->stk_pos - 1] = ~state->stk[state->stk_pos - 1] + 1;
            }
            break;
        case OP_and:
            check_e_stack(2);
            state->stk_pos--;
            state->stk[state->stk_pos - 1] = state->stk[state->stk_pos - 1] & state->stk[state->stk_pos];
            break;
        case OP_div:
            check_e_stack(2);
            state->stk_pos--;
            if (state->stk[state->stk_pos] == 0) inv_dwarf("Division by zero in location expression");
            state->stk[state->stk_pos - 1] /= state->stk[state->stk_pos];
            break;
        case OP_minus:
            check_e_stack(2);
            state->stk_pos--;
            state->stk[state->stk_pos - 1] -= state->stk[state->stk_pos];
            break;
        case OP_mod:
            check_e_stack(2);
            state->stk_pos--;
            if (state->stk[state->stk_pos] == 0) inv_dwarf("Division by zero in location expression");
            state->stk[state->stk_pos - 1] %= state->stk[state->stk_pos];
            break;
        case OP_mul:
            check_e_stack(2);
            state->stk_pos--;
            state->stk[state->stk_pos - 1] *= state->stk[state->stk_pos];
            break;
        case OP_neg:
            check_e_stack(1);
            state->stk[state->stk_pos - 1] = ~state->stk[state->stk_pos - 1] + 1;
            break;
        case OP_not:
            check_e_stack(1);
            state->stk[state->stk_pos - 1] = ~state->stk[state->stk_pos - 1];
            break;
        case OP_or:
            check_e_stack(2);
            state->stk_pos--;
            state->stk[state->stk_pos - 1] = state->stk[state->stk_pos - 1] | state->stk[state->stk_pos];
            break;
        case OP_add:
        case OP_plus:
            check_e_stack(2);
            state->stk_pos--;
            state->stk[state->stk_pos - 1] += state->stk[state->stk_pos];
            break;
        case OP_plus_uconst:
            check_e_stack(1);
            state->stk[state->stk_pos - 1] += read_u8leb128();
            break;
        case OP_shl:
            check_e_stack(2);
            state->stk_pos--;
            state->stk[state->stk_pos - 1] <<= state->stk[state->stk_pos];
            break;
        case OP_shr:
            check_e_stack(2);
            state->stk_pos--;
            state->stk[state->stk_pos - 1] >>= state->stk[state->stk_pos];
            break;
        case OP_shra:
            {
                uint64_t cnt;
                check_e_stack(2);
                data = state->stk[state->stk_pos - 2];
                cnt = state->stk[state->stk_pos - 1];
                if (cnt >= 64) {
                    data = data & ((uint64_t)1 << 63) ? ~(uint64_t)0 : 0;
                }
                else {
                    while (cnt > 0) {
                        int s = (data & ((uint64_t)1 << 63)) != 0;
                        data >>= 1;
                        if (s) data |= (uint64_t)1 << 63;
                        cnt--;
                    }
                }
                state->stk[state->stk_pos - 2] = data;
                state->stk_pos--;
            }
            break;
        case OP_xor:
            check_e_stack(2);
            state->stk_pos--;
            state->stk[state->stk_pos - 1] = state->stk[state->stk_pos - 1] ^ state->stk[state->stk_pos];
            break;
        case OP_bra:
            check_e_stack(1);
            {
                size_t offs = (int16_t)read_u2();
                if (state->stk[state->stk_pos - 1]) {
                    code_pos += offs;
                    if (code_pos > code_len) inv_dwarf("Invalid command");
                }
                state->stk_pos--;
            }
            break;
        case OP_eq:
            check_e_stack(2);
            state->stk_pos--;
            state->stk[state->stk_pos - 1] = state->stk[state->stk_pos - 1] == state->stk[state->stk_pos];
            state->type_stk[state->stk_pos - 1] = TYPE_CLASS_CARDINAL;
            break;
        case OP_ge:
            check_e_stack(2);
            state->stk_pos--;
            state->stk[state->stk_pos - 1] = state->stk[state->stk_pos - 1] >= state->stk[state->stk_pos];
            state->type_stk[state->stk_pos - 1] = TYPE_CLASS_CARDINAL;
            break;
        case OP_gt:
            check_e_stack(2);
            state->stk_pos--;
            state->stk[state->stk_pos - 1] = state->stk[state->stk_pos - 1] > state->stk[state->stk_pos];
            state->type_stk[state->stk_pos - 1] = TYPE_CLASS_CARDINAL;
            break;
        case OP_le:
            check_e_stack(2);
            state->stk_pos--;
            state->stk[state->stk_pos - 1] = state->stk[state->stk_pos - 1] <= state->stk[state->stk_pos];
            state->type_stk[state->stk_pos - 1] = TYPE_CLASS_CARDINAL;
            break;
        case OP_lt:
            check_e_stack(2);
            state->stk_pos--;
            state->stk[state->stk_pos - 1] = state->stk[state->stk_pos - 1] < state->stk[state->stk_pos];
            state->type_stk[state->stk_pos - 1] = TYPE_CLASS_CARDINAL;
            break;
        case OP_ne:
            check_e_stack(2);
            state->stk_pos--;
            state->stk[state->stk_pos - 1] = state->stk[state->stk_pos - 1] != state->stk[state->stk_pos];
            state->type_stk[state->stk_pos - 1] = TYPE_CLASS_CARDINAL;
            break;
        case OP_skip:
            {
                size_t offs = (int16_t)read_u2();
                code_pos += offs;
                if (code_pos > code_len) inv_dwarf("Invalid command");
            }
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
            state->stk[state->stk_pos] = op - OP_lit0;
            state->type_stk[state->stk_pos++] = TYPE_CLASS_CARDINAL;
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
                unsigned n = op - OP_reg0;
                if (!is_end_of_loc_expr()) inv_dwarf("OP_reg* must be last instruction");
                reg_def = get_reg_by_id(state->ctx, n, &state->reg_id_scope);
                if (reg_def == NULL) exception(errno);
            }
            break;
        case OP_regx:
            {
                unsigned n = (unsigned)read_u4leb128();
                if (!is_end_of_loc_expr()) inv_dwarf("OP_regx must be last instruction");
                reg_def = get_reg_by_id(state->ctx, n, &state->reg_id_scope);
                if (reg_def == NULL) exception(errno);
            }
            break;
        case OP_reg:
            {
                unsigned n = (unsigned)read_ua();
                if (!is_end_of_loc_expr()) inv_dwarf("OP_reg must be last instruction");
                reg_def = get_reg_by_id(state->ctx, n, &state->reg_id_scope);
                if (reg_def == NULL) exception(errno);
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
                RegisterDefinition * def = get_reg_by_id(state->ctx, op - OP_breg0, &state->reg_id_scope);
                if (def == NULL) exception(errno);
                if (read_reg_value(state->stack_frame, def, state->stk + state->stk_pos) < 0) exception(errno);
                state->type_stk[state->stk_pos] = TYPE_CLASS_CARDINAL;
                state->stk[state->stk_pos++] += read_i8leb128();
            }
            break;
        case OP_bregx:
            {
                RegisterDefinition * def = get_reg_by_id(state->ctx, (unsigned)read_u4leb128(), &state->reg_id_scope);
                if (def == NULL) exception(errno);
                if (read_reg_value(state->stack_frame, def, state->stk + state->stk_pos) < 0) exception(errno);
                state->type_stk[state->stk_pos] = TYPE_CLASS_CARDINAL;
                state->stk[state->stk_pos++] += read_i8leb128();
            }
            break;
        case OP_basereg:
            {
                RegisterDefinition * def = get_reg_by_id(state->ctx, (unsigned)read_ua(), &state->reg_id_scope);
                if (def == NULL) exception(errno);
                if (read_reg_value(state->stack_frame, def, state->stk + state->stk_pos) < 0) exception(errno);
                state->type_stk[state->stk_pos++] = TYPE_CLASS_CARDINAL;
            }
            break;
        case OP_call_frame_cfa:
            {
                StackFrame * frame = state->stack_frame;
                if (frame == NULL) str_exception(ERR_INV_ADDRESS, "Stack frame address not available");
                state->type_stk[state->stk_pos] = TYPE_CLASS_CARDINAL;
                state->stk[state->stk_pos++] = frame->fp;
            }
            break;
        case OP_nop:
            break;
        case OP_push_object_address:
            if (state->args_cnt == 0) exception(ERR_INV_CONT_OBJ);
            state->type_stk[state->stk_pos] = TYPE_CLASS_CARDINAL;
            state->stk[state->stk_pos++] = state->args[0];
            break;
        case OP_piece:
            piece = add_piece();
            piece->size = read_u4leb128();
            break;
        case OP_bit_piece:
            piece = add_piece();
            piece->bit_size = read_u4leb128();
            piece->bit_offs = read_u4leb128();
            break;
        case OP_implicit_value:
            value_size = read_u4leb128();
            if (code_pos + value_size > code_len) inv_dwarf("Invalid command");
            value_addr = tmp_alloc(value_size);
            memcpy(value_addr, code + code_pos, value_size);
            code_pos += value_size;
            if (!is_end_of_loc_expr()) inv_dwarf("OP_implicit_value must be last instruction");
            break;
        case OP_stack_value:
            check_e_stack(1);
            value_size = state->addr_size;
            value_addr = tmp_alloc(value_size);
            {
                unsigned i;
                uint8_t * buf = (uint8_t *)value_addr;
                uint64_t n = state->stk[--state->stk_pos];
                for (i = 0; i < value_size; i++) {
                    buf[state->reg_id_scope.big_endian ? value_size - i - 1 : i] = (uint8_t)n;
                    n >>= 8;
                }
            }
            if (!is_end_of_loc_expr()) inv_dwarf("OP_stack_value must be last instruction");
            break;
        case OP_GNU_const_type:
            inv_dwarf("Unsupported type in OP_GNU_const_type");
            break;
        case OP_GNU_regval_type:
            inv_dwarf("Unsupported type in OP_GNU_regval_type");
            break;
        case OP_GNU_deref_type:
            check_e_stack(1);
            {
                size_t mem_size = (size_t)read_u8leb128();
                uint32_t fund_type = read_u4leb128();
                uint32_t type_size = read_u4leb128();
                switch (fund_type) {
                case ATE_address:
                case ATE_unsigned:
                case ATE_unsigned_char:
                case ATE_unsigned_fixed:
                case ATE_UTF:
                    if (mem_size > type_size) mem_size = (size_t)type_size;
                    state->stk[state->stk_pos - 1] = read_memory(state->stk[state->stk_pos - 1], mem_size);
                    state->type_stk[state->stk_pos - 1] = TYPE_CLASS_CARDINAL;
                    break;
                case ATE_boolean:
                    if (mem_size > type_size) mem_size = (size_t)type_size;
                    state->stk[state->stk_pos - 1] = read_memory(state->stk[state->stk_pos - 1], mem_size);
                    if (state->stk[state->stk_pos - 1] != 0) state->stk[state->stk_pos - 1] = 1;
                    state->type_stk[state->stk_pos - 1] = TYPE_CLASS_CARDINAL;
                    break;
                case ATE_signed:
                case ATE_signed_char:
                case ATE_signed_fixed:
                    if (mem_size > type_size) mem_size = (size_t)type_size;
                    state->stk[state->stk_pos - 1] = read_memory(state->stk[state->stk_pos - 1], mem_size);
                    if (mem_size < 8) {
                        uint64_t sign = (uint64_t)1 << (mem_size * 8 - 1);
                        if (state->stk[state->stk_pos - 1] & sign) {
                            state->stk[state->stk_pos - 1] |= ~(sign - 1);
                        }
                    }
                    state->type_stk[state->stk_pos - 1] = TYPE_CLASS_INTEGER;
                    break;
                default:
                    inv_dwarf("Unsupported type in OP_GNU_deref_type");
                    break;
                }
            }
            break;
        case OP_GNU_convert:
            check_e_stack(1);
            {
                uint32_t fund_type = read_u4leb128();
                uint32_t type_size = read_u4leb128();
                switch (fund_type) {
                case ATE_address:
                case ATE_unsigned:
                case ATE_unsigned_char:
                case ATE_unsigned_fixed:
                case ATE_UTF:
                    if (type_size < 8) {
                        uint64_t sign = (uint64_t)1 << (type_size * 8 - 1);
                        state->stk[state->stk_pos - 1] &= sign - 1;
                    }
                    state->type_stk[state->stk_pos - 1] = TYPE_CLASS_CARDINAL;
                    break;
                case ATE_boolean:
                    if (state->stk[state->stk_pos - 1] != 0) state->stk[state->stk_pos - 1] = 1;
                    state->type_stk[state->stk_pos - 1] = TYPE_CLASS_CARDINAL;
                    break;
                case ATE_signed:
                case ATE_signed_char:
                case ATE_signed_fixed:
                    if (type_size < 8) {
                        uint64_t sign = (uint64_t)1 << (type_size * 8 - 1);
                        if (state->stk[state->stk_pos - 1] & sign) {
                            state->stk[state->stk_pos - 1] |= ~(sign - 1);
                        }
                        else {
                            state->stk[state->stk_pos - 1] &= sign - 1;
                        }
                    }
                    state->type_stk[state->stk_pos - 1] = TYPE_CLASS_INTEGER;
                    break;
                default:
                    inv_dwarf("Unsupported type in OP_GNU_convert");
                    break;
                }
            }
            break;
#if ENABLE_Symbols
        case OP_GNU_variable_value:
            {
                const char * id = (const char *)(code + code_pos);
                Symbol * sym = NULL;
                void * value = NULL;
                size_t size = 0;
                int big_endian = 0;
                uint64_t v = 0;
                unsigned i;
                code_pos += strlen(id) + 1;
                if (id2symbol(id, &sym) < 0) exception(errno);
                if (get_symbol_value(sym, &value, &size, &big_endian) < 0) exception(errno);
                for (i = 0; i < size; i++) {
                    uint64_t b = *((uint8_t *)value + i);
                    v |= b << (big_endian ? (size - i - 1) * 8 : i * 8);
                }
                state->stk[state->stk_pos] = v;
                state->type_stk[state->stk_pos] = TYPE_CLASS_CARDINAL;
                state->stk_pos++;
            }
            break;
#endif
        case OP_TCF_switch:
            check_e_stack(1);
            {
                uint64_t n = state->stk[--state->stk_pos];
                size_t end_pos = read_u2();
                end_pos += code_pos;
                if (end_pos > code_len) inv_dwarf("Invalid command");
                for (;;) {
                    uint64_t addr, size;
                    size_t nxt_pos = read_u2();
                    nxt_pos += code_pos;
                    if (nxt_pos > end_pos) inv_dwarf("Invalid command");
                    if (nxt_pos == code_pos) {
                        str_exception(ERR_OTHER, "Object is not available at this location in the code");
                    }
                    addr = read_u8leb128();
                    size = read_u8leb128();
                    if (size == 0 || (n >= addr && n - addr < size)) {
                        Trap trap;
                        size_t code_len_org = code_len;
                        if (set_trap(&trap)) {
                            code_len = state->code_len = nxt_pos;
                            evaluate_expression();
                            clear_trap(&trap);
                        }
                        code_len = state->code_len = code_len_org;
                        if (trap.error) exception(trap.error);
                        break;
                    }
                    code_pos = nxt_pos;
                }
                code_pos = end_pos;
            }
            break;
        case OP_TCF_offset:
            if (reg_def != NULL || value_addr != NULL) add_piece();
            if (state->pieces_cnt) {
                unsigned cnt = 0;
                uint32_t bit_offs = 0;
                uint32_t offs = read_u4leb128();
                LocationPiece * pieces = state->pieces;
                unsigned pieces_cnt = state->pieces_cnt;
                state->pieces = NULL;
                state->pieces_cnt = state->pieces_max = 0;
                while (cnt < pieces_cnt) {
                    LocationPiece * org_piece = pieces + cnt++;
                    if (org_piece->bit_size == 0) org_piece->bit_size = org_piece->size * 8;
                    if (bit_offs + org_piece->bit_size > offs * 8) {
                        if (state->pieces_cnt >= state->pieces_max) {
                            state->pieces_max += 4;
                            state->pieces = (LocationPiece *)tmp_realloc(state->pieces, state->pieces_max * sizeof(LocationPiece));
                        }
                        piece = state->pieces + state->pieces_cnt++;
                        *piece = *org_piece;
                        if (bit_offs < offs * 8) {
                            piece->bit_offs += offs * 8 - bit_offs;
                            piece->bit_size -= offs * 8 - bit_offs;
                        }
                        if (piece->bit_offs == 0 && piece->bit_size % 8 == 0) {
                            piece->size = piece->bit_size / 8;
                            piece->bit_size = 0;
                        }
                        piece->implicit_pointer++;
                    }
                    bit_offs += org_piece->bit_size;
                }
                if (state->pieces_cnt == 0) inv_dwarf("Invalid size of implicit value");
            }
            else {
                check_e_stack(1);
                state->stk[state->stk_pos - 1] += read_u8leb128();
                implicit_pointer++;
            }
            break;
        case OP_GNU_entry_value:
            {
#if SERVICE_StackTrace || ENABLE_ContextProxy
                LocationExpressionState * s = state;
                uint32_t size = read_u4leb128();
                Trap trap;
                get_state(s);
                if (set_trap(&trap)) {
                    LocationExpressionState entry_state;
                    int frame = get_prev_frame(s->ctx, get_info_frame(s->ctx, s->stack_frame));
                    memset(&entry_state, 0, sizeof(entry_state));
                    entry_state.ctx = s->ctx;
                    if (get_frame_info(s->ctx, frame, &entry_state.stack_frame) < 0) exception(errno);
                    entry_state.reg_id_scope = s->reg_id_scope;
                    entry_state.addr_size = s->addr_size;
                    entry_state.code = s->code + s->code_pos;
                    entry_state.code_len = size;
                    entry_state.client_op = s->client_op;
                    if (evaluate_vm_expression(&entry_state) < 0) exception(errno);
                    if (entry_state.pieces_cnt > 0) {
                        size_t i;
                        uint64_t value = 0;
                        void * piece_buf = NULL;
                        size_t piece_size = 0;
                        read_location_pieces(entry_state.ctx, entry_state.stack_frame,
                            entry_state.pieces, entry_state.pieces_cnt, 0,
                            &piece_buf, &piece_size);
                        if (piece_size > sizeof(value)) inv_dwarf("Invalid OP_entry_value expression");
                        for (i = 0; i < piece_size; i++) {
                            value |= ((uint8_t *)piece_buf)[i] << (i * 8);
                        }
                        s->type_stk[s->stk_pos] = TYPE_CLASS_CARDINAL;
                        s->stk[s->stk_pos++] = value;
                    }
                    else if (entry_state.stk_pos == 1) {
                        s->type_stk[s->stk_pos] = entry_state.type_stk[entry_state.stk_pos - 1];
                        s->stk[s->stk_pos++] = entry_state.stk[entry_state.stk_pos - 1];
                    }
                    else {
                        inv_dwarf("Invalid OP_entry_value expression");
                    }
                    clear_trap(&trap);
                }
                s->code_pos += size;
                set_state(s);
                if (trap.error) exception(trap.error);
#else
                inv_dwarf("Cannot execute OP_entry_value: stack trace not available");
#endif
            }
            break;
        case OP_call2:
        case OP_call4:
        case OP_call_ref:
        default:
            if (state->client_op != NULL) {
                LocationExpressionState * s = state;
                Trap trap;
                get_state(s);
                if (set_trap(&trap)) {
                    s->client_op(op);
                    clear_trap(&trap);
                }
                set_state(s);
                if (trap.error) exception(trap.error);
            }
            else {
                str_fmt_exception(ERR_UNSUPPORTED, "Unsupported location expression op 0x%02x", op);
            }
        }
    }
}

int evaluate_vm_expression(LocationExpressionState * vm_state) {
    int error = 0;
    Trap trap;

    implicit_pointer = 0;
    value_addr = NULL;
    reg_def = NULL;

    set_state(vm_state);
    if (set_trap(&trap)) {
        evaluate_expression();
        if (reg_def != NULL || value_addr != NULL || implicit_pointer) add_piece();
        clear_trap(&trap);
    }
    else {
        error = trap.error;
    }
    get_state(vm_state);
    if (!error) return 0;
    errno = error;
    return -1;
}

#endif /* ENABLE_DebugContext */
