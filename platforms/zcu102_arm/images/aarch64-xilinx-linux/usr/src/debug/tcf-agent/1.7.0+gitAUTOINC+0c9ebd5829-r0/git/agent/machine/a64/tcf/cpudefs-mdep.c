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

#include <tcf/config.h>

#if ENABLE_DebugContext && !ENABLE_ContextProxy

#include <stddef.h>
#include <assert.h>
#include <stdio.h>
#include <signal.h>
#include <tcf/framework/errors.h>
#include <tcf/framework/cpudefs.h>
#include <tcf/framework/context.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/trace.h>
#include <tcf/services/symbols.h>
#include <tcf/services/runctrl.h>
#include <machine/arm/tcf/disassembler-arm.h>
#include <machine/a64/tcf/disassembler-a64.h>
#include <machine/arm/tcf/stack-crawl-arm.h>
#include <machine/a64/tcf/stack-crawl-a64.h>
#if ENABLE_ContextMux
#include <tcf/framework/cpudefs-mdep-mux.h>
#endif
#include <tcf/cpudefs-mdep.h>

#define REG_OFFSET(name) offsetof(REG_SET, name)

static RegisterDefinition regs_def64[] = {
    { "x0",      REG_OFFSET(gp.regs[0]),              8, 0, 0 },
    { "sp",      REG_OFFSET(gp.sp),                   8, 31, 31 },
    { "pc",      REG_OFFSET(gp.pc),                   8, 33, 33 },
    { "cpsr",    REG_OFFSET(gp.pstate),               8, -1, -1 },
    { "orig_x0", REG_OFFSET(gp.orig_x0),              8, -1, -1 },
    { "tls",     REG_OFFSET(other.tls),               8, -1, -1, 0, 0, 0, 1 },
    { "vfp",     0, 0, -1, -1, 0, 0, 1, 1 },
    { NULL },
};

static RegisterDefinition regs_def32[] = {
    { "r0",      REG_OFFSET(other.gp32.regs[0]),      4, 0, 0 },
    { "fp",      REG_OFFSET(other.gp32.regs[11]),     4, 11, 11 },
    { "ip",      REG_OFFSET(other.gp32.regs[12]),     4, 12, 12 },
    { "sp",      REG_OFFSET(other.gp32.regs[13]),     4, 13, 13 },
    { "lr",      REG_OFFSET(other.gp32.regs[14]),     4, 14, 14 },
    { "pc",      REG_OFFSET(other.gp32.regs[15]),     4, 15, 15 },
    { "cpsr",    REG_OFFSET(other.gp32.cpsr),         4, 128, 128 },
    { "orig_r0", REG_OFFSET(other.gp32.orig_r0),      4, -1, -1 },
    { "tls",     REG_OFFSET(other.tls),               8, -1, -1, 0, 0, 0, 1 },
    { NULL,      0, 0,  0,  0 },
};

typedef struct BitFieldInfo {
    const char * name;
    const char * desc;
    int bits[10];
} BitFieldInfo;

static BitFieldInfo psr_defs[] = {
    { "n",  "Negative condition code flag", { 31, -1 } },
    { "z",  "Zero condition code flag", { 30, -1 } },
    { "c",  "Carry condition code flag", { 29, -1 } },
    { "v",  "Overflow condition code flag", { 28, -1 } },
    { "q",  "Cumulative saturation flag", { 27, -1 } },
    { "it", "If-Then execution state bits", { 25, 26, 10, 11, 12, 13, 14, 15, -1 } },
    { "j",  "Jazelle bit", { 24, -1 } },
    { "il", "Illegal Execution State bit", { 20, -1 } },
    { "ge", "SIMD Greater than or Equal flags", { 16, 17, 18, 19, -1 } },
    { "e",  "Endianness execution state bit", { 9, -1 } },
    { "a",  "Asynchronous abort disable bit", { 8, -1 } },
    { "i",  "Interrupt disable bit", { 7, -1 } },
    { "f",  "Fast interrupt disable bit", { 6, -1 } },
    { "t",  "Thumb execution state bit", { 5, -1 } },
    { "m",  "Mode field", { 0, 1, 2, 3, 4, -1 } },
    { NULL, NULL }
};

RegisterDefinition * regs_index = NULL;
static RegisterDefinition * regs_index_a32 = NULL;
static unsigned regs_cnt = 0;
static unsigned regs_max = 0;

unsigned char BREAK_INST[] = { 0x00, 0x00, 0x20, 0xd4 };

static RegisterDefinition * pc_def = NULL;
static RegisterDefinition * pc_def_a32 = NULL;

typedef struct ContextExtensionA64 {
    int is_a32_prs;
} ContextExtensionA64;

static size_t context_extension_offset = 0;
#define EXT(ctx) ((ContextExtensionA64 *)((char *)(ctx) + context_extension_offset))

#ifdef MDEP_OtherRegisters

int mdep_get_other_regs(pid_t pid, REG_SET * data,
        size_t data_offs, size_t data_size,
        size_t * done_offs, size_t * done_size) {
    assert(data_offs >= offsetof(REG_SET, other));
    assert(data_offs + data_size <= offsetof(REG_SET, other) + sizeof(data->other));
    if (data_offs >= REG_OFFSET(other.tls) && data_offs < REG_OFFSET(other.tls) + sizeof(data->other.tls)) {
        struct iovec iovec;
        iovec.iov_base = &data->other.tls;
        iovec.iov_len = sizeof(data->other.tls);
        if (ptrace(PTRACE_GETREGSET, pid, NT_ARM_TLS, &iovec) < 0) return -1;
        *done_offs = offsetof(REG_SET, other.tls);
        *done_size = sizeof(data->other.tls);
        return 0;
    }
    if (data_offs >= REG_OFFSET(other.gp32) && data_offs < REG_OFFSET(other.gp32) + sizeof(data->other.gp32)) {
        struct iovec iovec;
        iovec.iov_base = &data->other.gp32;
        iovec.iov_len = sizeof(data->other.gp32);
        if (ptrace(PTRACE_GETREGSET, pid, NT_PRSTATUS, &iovec) < 0) return -1;
        *done_offs = offsetof(REG_SET, other.gp32);
        *done_size = sizeof(data->other.gp32);
        return 0;
    }
    set_errno(ERR_OTHER, "Not supported");
    return -1;
}

int mdep_set_other_regs(pid_t pid, REG_SET * data,
        size_t data_offs, size_t data_size,
        size_t * done_offs, size_t * done_size) {
    if (data_offs >= REG_OFFSET(other.gp32) && data_offs < REG_OFFSET(other.gp32) + sizeof(data->other.gp32)) {
        struct iovec iovec;
        iovec.iov_base = &data->other.gp32;
        iovec.iov_len = sizeof(data->other.gp32);
        if (ptrace(PTRACE_SETREGSET, pid, NT_PRSTATUS, &iovec) < 0) return -1;
        *done_offs = offsetof(REG_SET, other.gp32);
        *done_size = sizeof(data->other.gp32);
        return 0;
    }
    set_errno(ERR_OTHER, "Not supported");
    return -1;
}

#endif

RegisterDefinition * get_PC_definition(Context * ctx) {
    if (!context_has_state(ctx)) return NULL;
    if (is_alt_isa_thread(ctx)) return pc_def_a32;
    return pc_def;
}

int crawl_stack_frame(StackFrame * frame, StackFrame * down) {
    if (is_alt_isa_thread(frame->ctx)) return crawl_stack_frame_arm(frame, down);
    return crawl_stack_frame_a64(frame, down);
}

#if defined(ENABLE_add_cpudefs_disassembler) && ENABLE_add_cpudefs_disassembler
void add_cpudefs_disassembler(Context * cpu_ctx) {
    add_disassembler(cpu_ctx, "A64", disassemble_a64);
    add_disassembler(cpu_ctx, "ARM", disassemble_arm);
    add_disassembler(cpu_ctx, "Thumb", disassemble_thumb);
}
#endif

static RegisterDefinition * alloc_reg(void) {
    RegisterDefinition * r = regs_index + regs_cnt++;
    assert(regs_cnt <= regs_max);
    r->dwarf_id = -1;
    r->eh_frame_id = -1;
    r->big_endian = big_endian_host();
    return r;
}

static void add_field(RegisterDefinition * parent, const char * name, const char * desc, int * list) {
    RegisterDefinition * fld = alloc_reg();
    unsigned size = 0;
    int * bits = NULL;
    while (list[size] >= 0) size++;
    size++;
    bits = (int *)loc_alloc(sizeof(int) * size);
    memcpy(bits, list, sizeof(int) * size);
    fld->name = name;
    fld->parent = parent;
    if (desc) fld->description = desc;
    if (fld->parent->no_read) fld->no_read = 1;
    if (fld->parent->no_write) fld->no_write = 1;
    if (fld->parent->read_once) fld->read_once = 1;
    if (fld->parent->write_once) fld->write_once = 1;
    fld->bits = bits;
}

static void add_psr_fields(RegisterDefinition * psr) {
    BitFieldInfo * d = psr_defs;
    unsigned i = 0;

    while (d[i].name) {
        add_field(psr, d[i].name, d[i].desc, d[i].bits);
        i++;
    }
}

static void ini_reg_defs(void) {
    RegisterDefinition * d;

    regs_cnt = 0;
    regs_max = 100;
    regs_index = (RegisterDefinition *)loc_alloc_zero(sizeof(RegisterDefinition) * regs_max);
    for (d = regs_def32; d->name != NULL; d++) {
        RegisterDefinition * r = alloc_reg();
        assert(d->parent == NULL);
        *r = *d;
        if (strcmp(r->name, "sp") == 0) {
            r->role = "SP";
        }
        else if (strcmp(r->name, "pc") == 0) {
            r->role = "PC";
            pc_def_a32 = r;
        }
        else if (strcmp(r->name, "r0") == 0) {
            unsigned i;
            for (i = 1; i <= 10; i++) {
                r = alloc_reg();
                *r = *d;
                r->name = loc_printf("r%d", i);
                r->offset = d->offset + i * 4;
                r->dwarf_id = d->dwarf_id + i;
                r->eh_frame_id = d->eh_frame_id + i;
            }
        }
        else if (strcmp(r->name, "cpsr") == 0) {
            add_psr_fields(r);
        }
    }
    regs_index_a32 = regs_index;

    regs_cnt = 0;
    regs_max = 400;
    regs_index = (RegisterDefinition *)loc_alloc_zero(sizeof(RegisterDefinition) * regs_max);
    for (d = regs_def64; d->name != NULL; d++) {
        RegisterDefinition * r = alloc_reg();
        assert(d->parent == NULL);
        *r = *d;
        if (strcmp(r->name, "sp") == 0) {
            r->role = "SP";
        }
        else if (strcmp(r->name, "pc") == 0) {
            r->role = "PC";
            pc_def = r;
        }
        else if (strcmp(r->name, "x0") == 0) {
            unsigned i;
            for (i = 1; i < 31; i++) {
                r = alloc_reg();
                *r = *d;
                r->name = loc_printf("x%d", i);
                r->offset = d->offset + i * 8;
                r->dwarf_id = d->dwarf_id + i;
                r->eh_frame_id = d->eh_frame_id + i;
            }
        }
        else if (strcmp(r->name, "cpsr") == 0) {
            add_psr_fields(r);
        }
        else if (strcmp(r->name, "vfp") == 0) {
            int n;
            RegisterDefinition * x = NULL;
            for (n = 0; n < 2; n++) {
                unsigned i;
                RegisterDefinition * w = alloc_reg();
                w->no_read = 1;
                w->no_write = 1;
                w->parent = r;
                switch (n) {
                case 0:
                    w->name = "64-bit";
                    for (i = 0; i < 64; i++) {
                        x = alloc_reg();
                        x->name = loc_printf("d%d", i);
                        x->offset = REG_OFFSET(fp.vregs) + i * 8;
                        x->size = 8;
                        x->fp_value = 1;
                        x->parent = w;
                    }
                    break;
                case 1:
                    w->name = "128-bit";
                    for (i = 0; i < 32; i++) {
                        x = alloc_reg();
                        x->name = loc_printf("v%d", i);
                        x->offset = REG_OFFSET(fp.vregs) + i * 16;
                        x->size = 16;
                        x->dwarf_id = 64 + i;
                        x->eh_frame_id = 64 + i;
                        x->fp_value = 1;
                        x->parent = w;
                    }
                    break;
                }
            }
            x = alloc_reg();
            x->name = "fpsr";
            x->offset = REG_OFFSET(fp.fpsr);
            x->size = 4;
            x->parent = r;
            x = alloc_reg();
            x->name = "fpcr";
            x->offset = REG_OFFSET(fp.fpcr);
            x->size = 4;
            x->parent = r;
        }
    }
}

int is_alt_isa_thread(Context * ctx) {
    return ctx->mem != ctx && EXT(ctx->mem)->is_a32_prs;
}

unsigned get_arm_word_size(Context * ctx) {
    return EXT(ctx->mem)->is_a32_prs ? 4 : 8;
}

uint8_t * get_alt_break_instruction(Context * ctx, size_t * size) {
    static uint8_t A32_BREAK_INST[] = { 0xf0, 0x01, 0xf0, 0xe7 };
    *size = sizeof(A32_BREAK_INST);
    return A32_BREAK_INST;
}

RegisterDefinition * get_alt_reg_definitions(Context * ctx) {
    return regs_index_a32;
}

RegisterDefinition * get_alt_reg_by_id(Context * ctx, unsigned id, RegisterIdScope * scope) {
    RegisterDefinition * def = regs_index_a32;
    while (def->name) {
        switch (scope->id_type) {
        case REGNUM_DWARF: if (def->dwarf_id == (int)id) return def; break;
        case REGNUM_EH_FRAME: if (def->eh_frame_id == (int)id) return def; break;
        }
        def++;
    }
    return NULL;
}

static void event_context_created(Context * ctx, void * args) {
    if (ctx->mem == ctx) {
        struct iovec iovec;
        struct regset_gp gp;
        iovec.iov_base = &gp;
        iovec.iov_len = sizeof(gp);
        if (ptrace(PTRACE_GETREGSET, id2pid(ctx->id, NULL), NT_PRSTATUS, &iovec) < 0) {
            trace(LOG_ALWAYS, "Cannot detect ARM registers size: %s", errno_to_str(errno));
        }
        else {
            EXT(ctx)->is_a32_prs = iovec.iov_len == 18 * 4;
        }
    }
}

static void event_context_exited(Context * ctx, void * args) {
}

static ContextEventListener context_listener = {
    event_context_created,
    event_context_exited,
    NULL,
    NULL,
    NULL,
    NULL
};

void ini_cpudefs_mdep(void) {
    ini_reg_defs();
    add_context_event_listener(&context_listener, NULL);
    context_extension_offset = context_extension(sizeof(ContextExtensionA64));
}

#endif
