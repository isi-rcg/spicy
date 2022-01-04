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
 * This file provides debug context register definitions and lookup functions.
 * It is included into cpudefs.c.
 * The code assumes that all contexts share same register definitions.
 * If it is not the case, this file needs to be substituted with alternative implementation.
 */

#include <tcf/config.h>

/* This file is only used when native debugger back-end is enabled - ENABLE_ContextProxy=0 */
#if !ENABLE_ContextProxy

#if ENABLE_ContextMux
#include <tcf/framework/cpudefs-mux.h>
#endif
#include <tcf/cpudefs-mdep.h>

typedef struct SysRegisterData {
    REG_SET data;
    REG_SET mask;
} SysRegisterData;

RegisterDefinition * get_reg_definitions(Context * ctx) {
    if (!context_has_state(ctx)) return NULL;
#ifdef ENABLE_cpu_alt_isa_mode
    if (is_alt_isa_thread(ctx)) return get_alt_reg_definitions(ctx);
#endif
    return regs_index;
}

uint8_t * get_break_instruction(Context * ctx, size_t * size) {
#ifdef ENABLE_cpu_alt_isa_mode
    if (is_alt_isa_thread(ctx)) return get_alt_break_instruction(ctx, size);
#endif
    *size = sizeof(BREAK_INST);
    return BREAK_INST;
}

static RegisterDefinition * get_sys_reg_by_dwarf_id(unsigned id) {
    static RegisterDefinition ** map = NULL;
    static unsigned map_length = 0;

    if (map == NULL) {
        RegisterDefinition * r;
        for (r = regs_index; r->name != NULL; r++) {
            if (r->dwarf_id >= (int)map_length) map_length = r->dwarf_id + 1;
        }
        map = (RegisterDefinition **)loc_alloc_zero(sizeof(RegisterDefinition *) * map_length);
        for (r = regs_index; r->name != NULL; r++) {
            if (r->dwarf_id >= 0) map[r->dwarf_id] = r;
        }
    }
    return id < map_length ? map[id] : NULL;
}

static RegisterDefinition * get_sys_reg_by_eh_frame_id(unsigned id) {
    static RegisterDefinition ** map = NULL;
    static unsigned map_length = 0;

    if (map == NULL) {
        RegisterDefinition * r;
        for (r = regs_index; r->name != NULL; r++) {
            if (r->eh_frame_id >= (int)map_length) map_length = r->eh_frame_id + 1;
        }
        map = (RegisterDefinition **)loc_alloc_zero(sizeof(RegisterDefinition *) * map_length);
        for (r = regs_index; r->name != NULL; r++) {
            if (r->eh_frame_id >= 0) map[r->eh_frame_id] = r;
        }
    }
    return id < map_length ? map[id] : NULL;
}

RegisterDefinition * get_reg_by_id(Context * ctx, unsigned id, RegisterIdScope * scope) {
    RegisterDefinition * def = NULL;
#ifdef GET_REG_BY_ID_HOOK
    GET_REG_BY_ID_HOOK;
#endif
    if (context_has_state(ctx)) {
#ifdef ENABLE_cpu_alt_isa_mode
        if (is_alt_isa_thread(ctx)) return get_alt_reg_by_id(ctx, id, scope);
#endif
        switch (scope->id_type) {
        case REGNUM_DWARF: def = get_sys_reg_by_dwarf_id(id); break;
        case REGNUM_EH_FRAME: def = get_sys_reg_by_eh_frame_id(id); break;
        }
    }
    if (def == NULL) set_errno(ERR_OTHER, "Invalid register ID");
    return def;
}

int read_reg_bytes(StackFrame * frame, RegisterDefinition * reg_def, unsigned offs, unsigned size, uint8_t * buf) {
    if (reg_def != NULL && frame != NULL) {
        if (frame->is_top_frame) {
            return context_read_reg(frame->ctx, reg_def, offs, size, buf);
        }
        if (frame->regs != NULL) {
            size_t i;
            uint8_t * r_addr = (uint8_t *)&((SysRegisterData *)frame->regs)->data + reg_def->offset;
            uint8_t * m_addr = (uint8_t *)&((SysRegisterData *)frame->regs)->mask + reg_def->offset;
            for (i = 0; i < size; i++) {
                if (m_addr[offs + i] != 0xff) {
                    set_fmt_errno(ERR_OTHER, "Value of register %s is unknown in the selected frame", reg_def->name);
                    return -1;
                }
            }
            assert(reg_def->offset + reg_def->size <= sizeof(REG_SET));
            if (offs + size > reg_def->size) {
                errno = ERR_INV_DATA_SIZE;
                return -1;
            }
            memcpy(buf, r_addr + offs, size);
            return 0;
        }
    }
    errno = ERR_INV_CONTEXT;
    return -1;
}

int write_reg_bytes(StackFrame * frame, RegisterDefinition * reg_def, unsigned offs, unsigned size, uint8_t * buf) {
    if (reg_def != NULL && frame != NULL) {
        if (frame->is_top_frame) {
            return context_write_reg(frame->ctx, reg_def, offs, size, buf);
        }
        if (frame->regs == NULL && context_has_state(frame->ctx)) {
            frame->regs = (RegisterData *)loc_alloc_zero(sizeof(SysRegisterData));
        }
        if (frame->regs != NULL) {
            uint8_t * r_addr = (uint8_t *)&((SysRegisterData *)frame->regs)->data + reg_def->offset;
            uint8_t * m_addr = (uint8_t *)&((SysRegisterData *)frame->regs)->mask + reg_def->offset;

            assert(reg_def->offset + reg_def->size <= sizeof(REG_SET));
            if (offs + size > reg_def->size) {
                errno = ERR_INV_DATA_SIZE;
                return -1;
            }
            memcpy(r_addr + offs, buf, size);
            memset(m_addr + offs, 0xff, size);
            return 0;
        }
    }
    errno = ERR_INV_CONTEXT;
    return -1;
}

#if ENABLE_ContextMux
#undef get_reg_definitions
#undef get_reg_by_id
#undef read_reg_bytes
#undef write_reg_bytes
#undef get_break_instruction
#endif

#endif /* !ENABLE_ContextProxy */
