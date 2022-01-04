/*******************************************************************************
 * Copyright (c) 2008, 2016 Wind River Systems, Inc. and others.
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
 * This module implements DWARF expressions evaluation.
 */

#include <tcf/config.h>

#if ENABLE_ELF && ENABLE_DebugContext

#include <assert.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/exceptions.h>
#include <tcf/framework/errors.h>
#include <tcf/services/dwarf.h>
#include <tcf/services/dwarfexpr.h>
#include <tcf/services/dwarfecomp.h>
#include <tcf/services/stacktrace.h>
#include <tcf/services/elf-symbols.h>
#include <tcf/services/vm.h>

void dwarf_get_expression_list(PropertyValue * Value, DWARFExpressionInfo ** List) {
    CompUnit * Unit = Value->mObject->mCompUnit;

    if (Value->mAddr == NULL || Value->mSize == 0) str_exception(ERR_INV_DWARF, "Invalid format of location expression");

    if (Value->mForm == FORM_DATA4 || Value->mForm == FORM_DATA8 || Value->mForm == FORM_SEC_OFFSET) {
        U8_T Base = 0;
        U8_T Offset = 0;
        U8_T AddrMax = ~(U8_T)0;
        DWARFCache * Cache = (DWARFCache *)Unit->mFile->dwarf_dt_cache;
        DWARFExpressionInfo * Last = NULL;

        assert(Cache->magic == DWARF_CACHE_MAGIC);
        if (Cache->mDebugLoc == NULL) str_exception(ERR_INV_DWARF, "Missing .debug_loc section");
        dio_EnterSection(&Unit->mDesc, Unit->mDesc.mSection, Value->mAddr - (U1_T *)Unit->mDesc.mSection->data);
        Offset = dio_ReadAddressX(NULL, Value->mSize);
        dio_ExitSection();
        Base = Unit->mObject->u.mCode.mLowPC;
        if (Unit->mDesc.mAddressSize < 8) AddrMax = ((U8_T)1 << Unit->mDesc.mAddressSize * 8) - 1;
        dio_EnterSection(&Unit->mDesc, Cache->mDebugLoc, Offset);
        for (;;) {
            ELF_Section * S0 = NULL;
            ELF_Section * S1 = NULL;
            U8_T Addr0 = dio_ReadAddress(&S0);
            U8_T Addr1 = dio_ReadAddress(&S1);
            if (Addr0 == AddrMax) {
                Base = Addr1;
            }
            else if (Addr0 == 0 && Addr1 == 0) {
                break;
            }
            else if (Addr0 > Addr1) {
                str_exception(ERR_INV_DWARF, "Invalid .debug_loc section");
            }
            else {
                U2_T Size = dio_ReadU2();
                U8_T RT_Addr0 = 0;
                if (S0 == NULL) S0 = Unit->mTextSection;
                RT_Addr0 = elf_map_to_run_time_address(Value->mContext, Unit->mFile, S0, Base + Addr0);
                if (!errno) {
                    DWARFExpressionInfo * Info = (DWARFExpressionInfo *)tmp_alloc_zero(sizeof(DWARFExpressionInfo));
                    Info->object = Value->mObject;
                    Info->code_addr = RT_Addr0;
                    Info->code_size = Addr1 - Addr0;
                    Info->section = Cache->mDebugLoc;
                    Info->expr_addr = dio_GetDataPtr();
                    Info->expr_size = Size;
                    Info->attr = Value->mAttr;
                    Info->form = Value->mForm;
                    if (Last == NULL) *List = Info;
                    else Last->next = Info;
                    Last = Info;
                }
                dio_Skip(Size);
            }
        }
        dio_ExitSection();
        if (Last == NULL) str_exception(ERR_OTHER, "Object is not available at this location in the code");
    }
    else {
        DWARFExpressionInfo * Info = (DWARFExpressionInfo *)tmp_alloc_zero(sizeof(DWARFExpressionInfo));
        Info->object = Value->mObject;
        Info->section = Unit->mDesc.mSection;
        Info->expr_addr = Value->mAddr;
        Info->expr_size = Value->mSize;
        Info->attr = Value->mAttr;
        Info->form = Value->mForm;
        *List = Info;
    }
}

typedef struct ExpressionArgs {
    PropertyValue * value;
    uint64_t * args;
    unsigned args_cnt;
} ExpressionArgs;

static void evaluate_expression(void * x) {
    ExpressionArgs * expr_args = (ExpressionArgs *)x;
    PropertyValue * Value = expr_args->value;
    uint64_t * args = expr_args->args;
    unsigned args_cnt = expr_args->args_cnt;
    CompUnit * Unit = Value->mObject->mCompUnit;
    LocationExpressionState * State = NULL;
    DWARFExpressionInfo * Info = NULL;

    State = (LocationExpressionState *)tmp_alloc_zero(sizeof(LocationExpressionState));
    State->stk = (uint64_t *)tmp_alloc(sizeof(uint64_t) * (State->stk_max = 8));
    State->type_stk = (uint8_t *)tmp_alloc_zero(State->stk_max);
    State->ctx = Value->mContext;
    if (Value->mFrame != STACK_NO_FRAME &&
            get_frame_info(Value->mContext, Value->mFrame, &State->stack_frame) < 0)
        exception(errno);
    State->addr_size = Unit->mDesc.mAddressSize;
    State->reg_id_scope = Unit->mRegIdScope;
    State->args = args;
    State->args_cnt = args_cnt;

    if (Value->mAttr == AT_data_member_location) {
        if (args_cnt < 1) exception(ERR_INV_CONT_OBJ);
        State->stk[State->stk_pos++] = args[0];
    }
    else if (Value->mAttr == AT_use_location) {
        if (args_cnt < 2) exception(ERR_INV_CONT_OBJ);
        State->stk[State->stk_pos++] = args[1];
        State->stk[State->stk_pos++] = args[0];
    }
    if (Value->mPieces != NULL || Value->mAddr == NULL || Value->mSize == 0) {
        str_exception(ERR_INV_DWARF, "Invalid DWARF expression reference");
    }
    dwarf_get_expression_list(Value, &Info);
    dwarf_transform_expression(Value->mContext, Value->mFrame, Info);
    State->code = Info->expr_addr;
    State->code_len = Info->expr_size;
    if (evaluate_vm_expression(State) < 0) exception(errno);
    assert(State->code_pos == State->code_len);

    Value->mForm = FORM_EXPR_VALUE;
    Value->mAddr = NULL;
    Value->mValue = 0;
    Value->mSize = 0;
    Value->mBigEndian = State->reg_id_scope.big_endian;
    Value->mPieces = NULL;
    Value->mPieceCnt = 0;

    if (State->pieces_cnt) {
        Value->mPieces = State->pieces;
        Value->mPieceCnt = State->pieces_cnt;
    }
    else {
        Value->mValue = State->stk[--State->stk_pos];
    }

    if (State->stk_pos != 0) {
        str_exception(ERR_INV_DWARF, "Invalid DWARF expression stack");
    }
}

void dwarf_evaluate_expression(PropertyValue * value, uint64_t * args, unsigned args_cnt) {
    /* Need to save symbols state, because expression evaluation calls get_stack_tracing_info() */
    ExpressionArgs expr_args;
    expr_args.value = value;
    expr_args.args = args;
    expr_args.args_cnt = args_cnt;
    if (elf_save_symbols_state(evaluate_expression, &expr_args) < 0) exception(errno);
}


#endif /* ENABLE_ELF && ENABLE_DebugContext */
