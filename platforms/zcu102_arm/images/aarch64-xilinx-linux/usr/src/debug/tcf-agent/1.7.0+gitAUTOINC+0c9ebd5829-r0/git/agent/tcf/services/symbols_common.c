/*******************************************************************************
 * Copyright (c) 2012, 2017 Wind River Systems, Inc. and others.
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
 * Symbols service - common code that is shared between different implementations of the service.
 */

#include <tcf/config.h>

#if SERVICE_Symbols || ENABLE_SymbolsProxy

#include <tcf/framework/myalloc.h>
#include <tcf/framework/exceptions.h>
#include <tcf/services/stacktrace.h>
#include <tcf/services/symbols.h>
#include <tcf/services/vm.h>

static LocationInfo * loc_info = NULL;

static LocationExpressionState * evaluate_symbol_location(const Symbol * sym) {
    Trap trap;
    Context * ctx = NULL;
    int frame = STACK_NO_FRAME;
    StackFrame * frame_info = NULL;
    LocationExpressionState * state = NULL;

    if (get_symbol_frame(sym, &ctx, &frame) < 0) return NULL;
    if (get_location_info(sym, &loc_info) < 0) return NULL;
    if (loc_info->args_cnt > 0) {
        set_errno(ERR_OTHER, "Object location is relative to owner address");
        return NULL;
    }
    if (frame != STACK_NO_FRAME && get_frame_info(ctx, frame, &frame_info) < 0) return NULL;
    if (!set_trap(&trap)) return NULL;
    state = evaluate_location_expression(ctx, frame_info,
        loc_info->value_cmds.cmds, loc_info->value_cmds.cnt, NULL, 0);
    clear_trap(&trap);
    return state;
}

static const char * pieces_err_msg(LocationExpressionState * state) {
    if (state->pieces->optimized_away) return "is optimized away";
    if (state->pieces->reg != NULL) return "is located in a register";
    if (state->pieces->value != NULL) return "is a constant value";
    return "is a bit field";
}

int get_symbol_address(const Symbol * sym, ContextAddress * address) {
    LocationExpressionState * state = evaluate_symbol_location(sym);
    if (state == NULL) return -1;
    if (state->pieces_cnt == 1 &&
            state->pieces->implicit_pointer == 0 && state->pieces->optimized_away == 0 &&
            state->pieces->reg == NULL && state->pieces->value == NULL && state->pieces->bit_offs == 0) {
        *address = state->pieces->addr;
        return 0;
    }
    if (state->pieces_cnt > 0) {
        set_fmt_errno(ERR_OTHER, "Cannot get object address: the object %s", pieces_err_msg(state));
        return -1;
    }
    if (state->stk_pos == 1) {
        *address = (ContextAddress)state->stk[0];
        return 0;
    }
    set_errno(ERR_OTHER, "Object does not have memory address");
    return -1;
}

int get_symbol_offset(const Symbol * sym, ContextAddress * offset) {
    if (get_location_info(sym, &loc_info) < 0) return -1;
    if (loc_info->args_cnt == 1) {
        /* Relative location. Only static offset can be returned.
         * Dynamic offset can only be computed in an expression. */
        if (loc_info->value_cmds.cnt == 3 && loc_info->code_size == 0 &&
                loc_info->value_cmds.cmds[0].cmd == SFT_CMD_ARG &&
                loc_info->value_cmds.cmds[1].cmd == SFT_CMD_NUMBER &&
                loc_info->value_cmds.cmds[2].cmd == SFT_CMD_ADD) {
            *offset = (ContextAddress)loc_info->value_cmds.cmds[1].args.num;
            return 0;
        }
    }
    set_errno(ERR_OTHER, "Object does not have member offset");
    return -1;
}

int get_symbol_register(const Symbol * sym, Context ** ctx, int * frame, RegisterDefinition ** reg) {
    LocationExpressionState * state = evaluate_symbol_location(sym);
    if (state == NULL) return -1;
    if (state->pieces_cnt == 1 && state->pieces->reg != NULL && state->pieces->reg->size == state->pieces->size) {
        if (get_symbol_frame(sym, ctx, frame) < 0) return -1;
        *reg = state->pieces->reg;
        return 0;
    }
    set_errno(ERR_OTHER, "Symbol is not located in a register");
    return -1;
}

int get_symbol_value(const Symbol * sym, void ** value, size_t * size, int * big_endian) {
    Trap trap;
    LocationExpressionState * state = evaluate_symbol_location(sym);
    if (state == NULL) return -1;
    if (!set_trap(&trap)) return -1;
    if (state->pieces_cnt > 0) {
        read_location_pieces(state->ctx, state->stack_frame,
            state->pieces, state->pieces_cnt, loc_info->big_endian, value, size);
    }
    else {
        ContextAddress sym_size = 0;
        size_t buf_size = 0;
        void * buf = NULL;
        if (state->stk_pos != 1) str_exception(ERR_OTHER, "Invalid location expression");
        if (get_symbol_size(sym, &sym_size) < 0) exception(errno);
        buf_size = (size_t)sym_size;
        buf = tmp_alloc(buf_size);
        if (context_read_mem(state->ctx, (ContextAddress)state->stk[0], buf, buf_size) < 0) exception(errno);
        *value = buf;
        *size = buf_size;
    }
    *big_endian = loc_info->big_endian;
    clear_trap(&trap);
    return 0;
}

#endif
