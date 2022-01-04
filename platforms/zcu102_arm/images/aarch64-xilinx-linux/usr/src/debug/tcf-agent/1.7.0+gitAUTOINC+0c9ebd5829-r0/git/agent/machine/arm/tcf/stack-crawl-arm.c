/*******************************************************************************
 * Copyright (c) 2013-2020 Xilinx, Inc. and others.
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

/*
 * This module implements stack crawl for ARM processor.
 */

/*
 * This code is based on ideas from a work that was published by
 * Michael McTernan with following disclaimer:
 *
 * The source code for the stack unwinder is released as public domain.
 * This means that there is no copyright and anyone is able to take a
 * copy for free and use it as they wish, with or without modifications,
 * and in any context they like, commercially or otherwise.
 *
 * The only limitation is that I don't guarantee that the software is fit
 * for any purpose or accept any liability for it's use or misuse -
 * the software is without warranty.
 *
 * Michael McTernan
 * Michael.McTernan.2001@cs.bris.ac.uk
 */

#include <tcf/config.h>

#if ENABLE_DebugContext

#include <assert.h>
#include <tcf/framework/context.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/trace.h>
#include <tcf/services/symbols.h>
#include <tcf/services/stacktrace.h>
#include <machine/arm/tcf/stack-crawl-arm.h>

#define USE_MEM_CACHE        1
#define MEM_HASH_SIZE       61
#define BRANCH_LIST_SIZE    32
#define REG_DATA_SIZE      168

#define REG_VAL_FRAME        1
#define REG_VAL_ADDR         2
#define REG_VAL_STACK        3
#define REG_VAL_OTHER        4

#define REG_ID_CPSR        128

typedef struct {
    uint32_t v;
    uint32_t o;
} RegData;

typedef struct {
    uint32_t v[MEM_HASH_SIZE]; /* Value */
    uint32_t a[MEM_HASH_SIZE]; /* Address */
    uint8_t  used[MEM_HASH_SIZE];
    uint8_t  valid[MEM_HASH_SIZE];
} MemData;

typedef struct {
    uint32_t addr;
    RegData reg_data[REG_DATA_SIZE];
    MemData mem_data;
} BranchData;

#define CPU_ARMv7A  1
#define CPU_ARMv7R  2
#define CPU_ARMv7M  3
#define CPU_ARM32   4

enum ISA_TYPE { ISA_ARM, ISA_THUMB, ISA_JAZELLE };

static unsigned cpu_type = 0;
static Context * stk_ctx = NULL;
static StackFrame * stk_frame = NULL;
static RegData reg_data[REG_DATA_SIZE];
static MemData mem_data;
static unsigned mem_cache_idx = 0;
static int trace_return = 0;
static int trace_branch = 0;

static unsigned branch_pos = 0;
static unsigned branch_cnt = 0;
static BranchData branch_data[BRANCH_LIST_SIZE];

#if USE_MEM_CACHE

typedef struct {
    uint32_t addr;
    uint32_t size;
    uint8_t data[64];
} MemCache;

#define MEM_CACHE_SIZE       8
static MemCache mem_cache[MEM_CACHE_SIZE];

/* Extension can access static variables and static functions */
#include <machine/arm/tcf/stack-crawl-arm-ext.h>

static int read_mem(ContextAddress address, void * buf, size_t size) {
#if ENABLE_MemoryAccessModes
    static MemoryAccessMode mem_access_mode = { 0, 0, 0, 0, 0, 0, 1 };
    return context_read_mem_ext(stk_ctx, &mem_access_mode, address, buf, size);
#else
    return context_read_mem(stk_ctx, address, buf, size);
#endif
}

static int read_byte(uint32_t addr, uint8_t * bt) {
    unsigned i = 0;
    MemCache * c = NULL;

    if (addr == 0) {
        errno = ERR_INV_ADDRESS;
        return -1;
    }
    for (i = 0; i < MEM_CACHE_SIZE; i++) {
        c = mem_cache + mem_cache_idx;
        if (c->addr <= addr && (c->addr + c->size < c->addr || c->addr + c->size > addr)) {
            *bt = c->data[addr - c->addr];
            return 0;
        }
        mem_cache_idx = (mem_cache_idx + 1) % MEM_CACHE_SIZE;
    }
    mem_cache_idx = (mem_cache_idx + 1) % MEM_CACHE_SIZE;
    c = mem_cache + mem_cache_idx;
    c->addr = addr;
    c->size = sizeof(c->data);
    if (read_mem(addr, c->data, c->size) < 0) {
#if ENABLE_ExtendedMemoryErrorReports
        int error = errno;
        MemoryErrorInfo info;
        if (context_get_mem_error_info(&info) < 0 || info.size_valid == 0) {
            c->size = 0;
            errno = error;
            return -1;
        }
        c->size = info.size_valid;
#else
        c->size = 0;
        return -1;
#endif
    }
    *bt = c->data[0];
    return 0;
}

static int read_half(uint32_t addr, uint16_t * h) {
    unsigned i;
    uint16_t n = 0;
    for (i = 0; i < 2; i++) {
        uint8_t bt = 0;
        if (read_byte(addr + i, &bt) < 0) return -1;
        n |= (uint32_t)bt << (i * 8);
    }
    *h = n;
    return 0;
}

static int read_word(uint32_t addr, uint32_t * w) {
    unsigned i;
    uint32_t n = 0;
    for (i = 0; i < 4; i++) {
        uint8_t bt = 0;
        if (read_byte(addr + i, &bt) < 0) return -1;
        n |= (uint32_t)bt << (i * 8);
    }
    *w = n;
    return 0;
}

#else

static int read_half(uint32_t addr, uint16_t * h) {
    uint8_t buf[2];
    if (addr == 0) {
        errno = ERR_INV_ADDRESS;
        return -1;
    }
    if (read_mem(addr, buf, 2) < 0) return -1;
    *h = (uint32_t)buf[0] | (buf[1] << 8);
    return 0;
}

static int read_word(uint32_t addr, uint32_t * w) {
    uint8_t buf[4];
    if (addr == 0) {
        errno = ERR_INV_ADDRESS;
        return -1;
    }
    if (read_mem(addr, buf, 4) < 0) return -1;
    *w = (uint32_t)buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
    return 0;
}

#endif /* USE_MEM_CACHE */

static int mem_hash_index(const uint32_t addr) {
    int v = addr % MEM_HASH_SIZE;
    int s = v;

    do {
        /* Check if the element is occupied */
        if (mem_data.used[s]) {
            /* Check if it is occupied with the sought data */
            if (mem_data.a[s] == addr)  return s;
        }
        else {
            /* Item is free, this is where the item should be stored */
            return s;
        }

        /* Search the next entry */
        s++;
        if (s >= MEM_HASH_SIZE) s = 0;
    }
    while (s != v);

    /* Search failed, hash is full and the address not stored */
    errno = ERR_OTHER;
    return -1;
}

static int mem_hash_read(uint32_t addr, uint32_t * data, int * valid) {
    int i = mem_hash_index(addr);

    if (i >= 0 && mem_data.used[i] && mem_data.a[i] == addr) {
        *data  = mem_data.v[i];
        *valid = mem_data.valid[i];
        return 0;
    }

    /* Address not found in the hash */
    errno = ERR_OTHER;
    return -1;
}

static int load_reg(uint32_t addr, int r) {
    int valid = 0;

    reg_data[r].o = 0;
    reg_data[r].v = 0;
    /* Check if the value can be found in the hash */
    if (mem_hash_read(addr, &reg_data[r].v, &valid) == 0) {
        if (valid) reg_data[r].o = REG_VAL_OTHER;
    }
    else {
        /* Not in the hash, so read from real memory */
        if (read_word(addr, &reg_data[r].v) < 0) return -1;
        reg_data[r].o = REG_VAL_OTHER;
    }
    return 0;
}

static int load_reg_lazy(uint32_t addr, int r) {
    int valid = 0;
    if (mem_hash_read(addr, &reg_data[r].v, &valid) == 0) {
        if (valid) {
            reg_data[r].o = REG_VAL_OTHER;
            return 0;
        }
        reg_data[r].o = 0;
        reg_data[r].v = 0;
        return 0;
    }
    reg_data[r].o = REG_VAL_ADDR;
    reg_data[r].v = addr;
    return 0;
}

static int chk_loaded(int r) {
    /* Make sure register origin is either 0 or REG_VAL_OTHER */
    if (reg_data[r].o == 0) return 0;
    if (reg_data[r].o == REG_VAL_OTHER) return 0;
    if (reg_data[r].o == REG_VAL_FRAME) {
        uint64_t v = 0;
        RegisterDefinition * def = get_reg_definitions(stk_ctx) + reg_data[r].v;
        if (read_reg_value(stk_frame, def, &v) < 0) {
            if (stk_frame->is_top_frame) return -1;
            reg_data[r].o = 0;
            return 0;
        }
        reg_data[r].v = (uint32_t)v;
        reg_data[r].o = REG_VAL_OTHER;
        return 0;
    }
    return load_reg(reg_data[r].v, r);
}

static int mem_hash_write(uint32_t addr, uint32_t value, int valid) {
    int h = mem_hash_index(addr);
    unsigned i;

    if (h < 0) {
        set_errno(ERR_OTHER, "Memory hash overflow");
        return -1;
    }

    /* Fix lazy loaded registers */
    for (i = 0; i < REG_DATA_SIZE; i++) {
        if (reg_data[i].o != REG_VAL_ADDR && reg_data[i].o != REG_VAL_STACK) continue;
        if (reg_data[i].v >= addr + 4) continue;
        if (reg_data[i].v + 4 <= addr) continue;
        if (load_reg(reg_data[i].v, i) < 0) return -1;
    }

    /* Store the item */
    mem_data.used[h] = 1;
    mem_data.a[h] = addr;
    mem_data.v[h] = valid ? value : 0;
    mem_data.valid[h] = (uint8_t)valid;
    return 0;
}

static int store_reg(uint32_t addr, int r) {
    if (chk_loaded(r) < 0) return -1;
    assert(reg_data[r].o == 0 || reg_data[r].o == REG_VAL_OTHER);
    return mem_hash_write(addr, reg_data[r].v, reg_data[r].o != 0);
}

static int store_invalid(uint32_t addr) {
    return mem_hash_write(addr, 0, 0);
}

static void add_branch(uint32_t addr) {
    if (branch_cnt < BRANCH_LIST_SIZE) {
        int add = 1;
        unsigned i = 0;
        for (i = 0; i < branch_cnt; i++) {
            BranchData * b = branch_data + i;
            if (b->addr == addr) {
                add = 0;
                break;
            }
        }
        if (add) {
            BranchData * b = branch_data + branch_cnt++;
            b->addr = addr;
            b->mem_data = mem_data;
            memcpy(b->reg_data, reg_data, sizeof(reg_data));
            b->reg_data[15].v = addr;
        }
    }
}

static int is_banked_reg_visible(RegisterDefinition * def, unsigned mode) {
    int r = 15;
    switch (mode) {
    case 0x11: /* fiq */
        if (def->dwarf_id >= 151 && def->dwarf_id <= 157) {
            return def->dwarf_id - 151 + 8;
        }
        r = 8;
        break;
    case 0x12: /* irq */
        if (def->dwarf_id >= 158 && def->dwarf_id <= 159) {
            return def->dwarf_id - 158 + 13;
        }
        r = 13;
        break;
    case 0x13: /* svc */
        if (def->dwarf_id >= 164 && def->dwarf_id <= 165) {
            return def->dwarf_id - 164 + 13;
        }
        r = 13;
        break;
    case 0x16: /* mon */
        if (def->dwarf_id >= 166 && def->dwarf_id <= 167) {
            return def->dwarf_id - 166 + 13;
        }
        r = 13;
        break;
    case 0x17: /* abt */
        if (def->dwarf_id >= 160 && def->dwarf_id <= 161) {
            return def->dwarf_id - 160 + 13;
        }
        r = 13;
        break;
    case 0x1b: /* und */
        if (def->dwarf_id >= 162 && def->dwarf_id <= 163) {
            return def->dwarf_id - 162 + 13;
        }
        r = 13;
        break;
    }
    if (def->dwarf_id >= 144 && def->dwarf_id < 144 - 8 + r) {
        return def->dwarf_id - 144 + 8;
    }
    return -1;
}

static uint32_t calc_shift(uint32_t shift_type, uint32_t shift_imm, uint32_t val) {
    switch (shift_type) {
    case 0: /* logical left */
        val = val << shift_imm;
        break;
    case 1: /* logical right */
        if (shift_imm == 0) val = 0;
        else val = val >> shift_imm;
        break;
    case 2: /* arithmetic right */
        if (shift_imm == 0) shift_imm = 32;
        if (val & 0x80000000) {
            if (shift_imm > 32) {
                val = 0xffffffff;
            }
            else {
                val = val >> shift_imm;
                val |= 0xffffffff << (32 - shift_imm);
            }
        }
        else {
            val = val >> shift_imm;
        }
        break;
    case 3: /* rotate right */
        if (shift_imm == 0) {
            /* Rotate right with extend */
            val = val >> 1;
            assert(reg_data[REG_ID_CPSR].o == REG_VAL_OTHER);
            if (reg_data[REG_ID_CPSR].v & (1 << 29)) val |= 0x80000000;
        }
        else {
            shift_imm &= 0x1f;
            val = (val >> shift_imm) |
                  (val << (32 - shift_imm));
        }
        break;
    }
    return val;
}

static unsigned get_spsr_id(void) {
    chk_loaded(REG_ID_CPSR);
    if (reg_data[REG_ID_CPSR].o == 0) return 0;
    switch (reg_data[REG_ID_CPSR].v & 0x1f) {
    case 0x11: /* fiq */ return 129;
    case 0x12: /* irq */ return 130;
    case 0x13: /* svc */ return 133;
    case 0x16: /* mon */ return 134;
    case 0x17: /* abt */ return 131;
    case 0x1b: /* und */ return 132;
    }
    return 0;
}

static void return_from_exception(void) {
    /* Return from exception - copies the SPSR to the CPSR */
    RegisterDefinition * defs = get_reg_definitions(stk_ctx);
    RegisterDefinition * def;
    unsigned spsr_id = get_spsr_id();
    if (spsr_id == 0) {
        reg_data[REG_ID_CPSR].o = 0;
        return;
    }
    chk_loaded(spsr_id);
    for (def = defs; def->name; def++) {
        int r = is_banked_reg_visible(def, reg_data[spsr_id].v & 0x1f);
        if (r <= 0) continue;
        if (reg_data[def->dwarf_id].o) {
            reg_data[r] = reg_data[def->dwarf_id];
        }
        else {
            reg_data[r].v = (uint32_t)(def - defs);
            reg_data[r].o = REG_VAL_FRAME;
        }
    }
    reg_data[REG_ID_CPSR] = reg_data[spsr_id];
}

static void bx_write_pc(void) {
    chk_loaded(15);
    /* ARMv7-M only supports the Thumb execution state */
    if (cpu_type == CPU_ARMv7M) {
        reg_data[15].v &= ~0x1u;
        return;
    }
    /* Determine the new mode */
    if (reg_data[15].o) {
        int thumb_ee = (reg_data[REG_ID_CPSR].v & 0x01000000) && (reg_data[REG_ID_CPSR].v & 0x00000020);
        if (thumb_ee) {
            /* Remaining in ThumbEE state */
            reg_data[15].v &= ~0x1u;
        }
        else if ((reg_data[15].v & 0x1u) != 0) {
            /* Branch to Thumb */
            reg_data[REG_ID_CPSR].v |= 0x00000020;
            reg_data[15].v &= ~0x1u;
        }
        else if ((reg_data[15].v & 0x2u) == 0) {
            /* Branch to ARM */
            reg_data[REG_ID_CPSR].v &= ~0x00000020;
        }
    }
}

static void trace_bx(unsigned rn) {
    /* Set the new PC value */
    reg_data[15] = reg_data[rn];
    bx_write_pc();

    /* Check if the return value is from the stack */
    if (rn == 14 || reg_data[rn].o == REG_VAL_STACK) {
        /* Found the return address */
        trace_return = 1;
    }
    else {
        if (reg_data[15].o) add_branch(reg_data[15].v);
        trace_branch = 1;
    }
}

static void trace_srs(int wback, int inc, int wordhigher, uint32_t mode) {
    /* Store Return State */
#if 0
    /* TODO: to handle SRS, need to trace banked SP */
    int sp_id = -1;
    RegisterDefinition * def;
    switch (mode) {
    case 0x11: /* fiq */ sp_id = 156; break;
    case 0x12: /* irq */ sp_id = 158; break;
    case 0x13: /* svc */ sp_id = 164; break;
    case 0x16: /* mon */ sp_id = 166; break;
    case 0x17: /* abt */ sp_id = 160; break;
    case 0x1b: /* und */ sp_id = 162; break;
    default: return;
    }
    for (def = get_reg_definitions(stk_ctx); def->name; def++) {
        uint64_t v = 0;
        if (def->dwarf_id == sp_id) {
            if (search_reg_value(stk_frame, def, &v) == 0) {
                uint32_t base = (uint32_t)v;
                uint32_t addr = inc ? base : base - 8;
                if (wordhigher) addr += 4;
                store_reg(addr, 14);
                mem_hash_write(addr + 4, spsr_data.v, spsr_data.o != 0);
                if (wback) ... = inc ? base + 8 : base - 8;
            }
            break;
        }
    }
#endif
}

static void trace_rfe(int wback, int inc, int wordhigher, uint32_t mode) {
    /* Return From Exception */
}

static int trace_ldm_stm(int cond, uint32_t rn, uint32_t regs, int P, int U, int S, int W, int L) {
    uint32_t addr = 0;
    int addr_valid = 0;
    uint32_t rn_bank = reg_data[REG_ID_CPSR].v & 0x1f;
    unsigned banked = 0;
    uint8_t r;

    chk_loaded(rn);
    addr = reg_data[rn].v;
    addr_valid = reg_data[rn].o == REG_VAL_OTHER;

    /* S indicates that banked registers (untracked) are used, unless
     * this is a load including the PC when the S-bit indicates that
     * CPSR is loaded from SPSR.
     */
    if (S) {
        if (L && (regs & (1 << 15)) != 0) {
            return_from_exception();
        }
        else {
            switch (reg_data[REG_ID_CPSR].v & 0x1f) {
            case 0x11:
                banked = 0x7f00;
                break;
            case 0x12:
            case 0x13:
            case 0x16:
            case 0x17:
            case 0x1b:
                banked = 0x6000;
                break;
            }
        }
    }

    if (rn == 15) {
        set_errno(ERR_OTHER, "r15 used as base register");
        return -1;
    }

    /* Check if ascending or descending.
     *  Registers are loaded/stored in order of address.
     *  i.e. r0 is at the lowest address, r15 at the highest.
     */
    r = U ? 0 : 15;

    for (;;) {
        /* Check if the register is to be transferred */
        if (regs & (1 << r)) {
            if (P) addr = U ? addr + 4 : addr - 4;
            if (L) {
                if (banked & (1 << r)) {
                    /* Load user bank register */
                }
                else if (cond) {
                    reg_data[r].o = 0;
                }
                else if (addr_valid) {
                    reg_data[r].o = rn == 13 ? REG_VAL_STACK : REG_VAL_ADDR;
                    reg_data[r].v = addr;
                }
                else {
                    /* Invalidate the register as the base reg was invalid */
                    reg_data[r].o = 0;
                }
            }
            else if (banked & (1 << r)) {
                /* Store user bank register */
            }
            else if (addr_valid) {
                if (cond) store_invalid(addr);
                else store_reg(addr, r);
            }
            if (!P) addr = U ? addr + 4 : addr - 4;
        }
        /* Check the next register */
        if (U) {
            if (r == 15) break;
            r++;
        }
        else {
            if (r == 0) break;
            r--;
        }
    }

    /* Check the writeback bit */
    if (addr_valid && W && rn_bank == (reg_data[REG_ID_CPSR].v & 0x1f)) {
        reg_data[rn].o = cond ? 0: REG_VAL_OTHER;
        reg_data[rn].v = addr;
    }

    /* Check if the PC was loaded */
    if (L && (regs & (1 << 15))) {
        bx_write_pc();
        /* Found the return address */
        trace_return = 1;
    }
    return 0;
}

static int trace_thumb_data_processing_pbi_32(uint16_t instr, uint16_t suffix) {
    uint32_t op_code = (instr >> 4) & 0x1f;
    uint32_t rn = instr & 0xf;
    uint32_t rd = (suffix >> 8) & 0xf;
    uint32_t imm = suffix & 0xff;

    imm |= (suffix & 0x7000) >> 4;
    if (instr & (1 << 10)) imm |= 0x800;

    switch (op_code) {
    case 0:
        chk_loaded(rn);
        reg_data[rd].v = reg_data[rn].v + imm;
        reg_data[rd].o = reg_data[rn].o ? REG_VAL_OTHER : 0;
        break;
    case 4:
        imm |= rn << 12;
        reg_data[rd].v = imm;
        reg_data[rd].o = REG_VAL_OTHER;
        break;
    case 10:
        chk_loaded(rn);
        reg_data[rd].v = reg_data[rn].v - imm;
        reg_data[rd].o = reg_data[rn].o ? REG_VAL_OTHER : 0;
        break;
    default:
        reg_data[rd].o = 0;
        break;
    }

    return 0;
}

static int trace_thumb_branches_and_misc_32(uint16_t instr, uint16_t suffix) {
    return 0;
}

static int trace_thumb_load_store_32(uint16_t instr, uint16_t suffix) {
    if ((instr & (1 << 6)) == 0) {
        /* Load/store multiple */
        uint32_t op = (instr >> 7) & 3;
        int L = (instr & (1 << 4)) != 0;
        int W = (instr & (1 << 5)) != 0;
        int U = (instr & (1 << 8)) == 0;
        if (op == 0 || op == 3) {
            if (!L) trace_srs(W, op == 3, 0, suffix & 0x1f);
            else trace_rfe(instr & 0xf, W, op == 3, 0);
            return 0;
        }
        return trace_ldm_stm(0, instr & 0xf, suffix, !U, U, 0, W, L);
    }

    if ((instr & 0xffe0) == 0xe840) {
        /* Load/store exclusive */
        return 0;
    }

    if ((instr & 0xffe0) == 0xe8c0) {
        /* Load/store exclusive */
        return 0;
    }

    if ((instr & 0xff60) == 0xe860 || (instr & 0xff40) == 0xe940) {
        /* Load/store register dual */
        uint32_t rn = instr & 0xf;
        chk_loaded(rn);
        if (reg_data[rn].o) {
            int L = (instr & (1u << 4)) != 0;
            int W = (instr & (1u << 5)) != 0;
            int U = (instr & (1u << 7)) != 0;
            int P = (instr & (1u << 8)) != 0;
            uint32_t rt1 = (suffix >> 12) & 0xf;
            uint32_t rt2 = (suffix >> 8) & 0xf;
            uint32_t imm = suffix & 0xff;
            uint32_t addr = reg_data[rn].v;
            if (rn == 15) addr = addr - (addr & 0x3) + 4;
            if (P) addr = U ? addr + imm * 4 : addr - imm * 4;
            if (L) {
                load_reg_lazy(addr, rt1);
                load_reg_lazy(addr + 4, rt2);
            }
            else {
                store_reg(addr, rt1);
                store_reg(addr + 4, rt2);
            }
            if (W) {
                if (!P) addr = U ? addr + imm * 4 : addr - imm * 4;
                reg_data[rn].v = addr;
            }
        }
        return 0;
    }

    if ((instr & 0xfff0) == 0xe8d0 && (suffix & 0x00e0) == 0x0000) {
        /* Table Branch */
        uint32_t rn = instr & 0xf;
        uint32_t rm = suffix & 0xf;
        int H = (suffix & (1 << 4)) != 0;
        chk_loaded(rn);
        chk_loaded(rm);
        if (reg_data[rn].o && reg_data[rm].o) {
            uint32_t addr = reg_data[rn].v;
            uint32_t offs = reg_data[rm].v;
            if (rn == 15) addr += 4;
            if (H) {
                uint16_t offs16 = 0;
                if (read_half(addr + (offs << 1), &offs16) < 0) return -1;
                offs = offs16;
            }
            else {
                uint8_t offs8 = 0;
                if (read_byte(addr + offs, &offs8) < 0) return -1;
                offs = offs8;
            }
            reg_data[15].v = reg_data[15].v + offs * 2 + 4;
        }
        else {
            reg_data[15].o = 0;
        }
        return 0;
    }

    return 0;
}

static int trace_thumb_data_processing_32(uint16_t instr, uint16_t suffix) {
    uint16_t op = (instr >> 5) & 0xf;
    uint16_t rn = instr & 0xf;
    uint16_t rd = (suffix >> 8) & 0xf;
    int I = (instr & (1 << 9)) == 0;
    int S = (instr & (1 << 4)) != 0;
    int tb = (instr & (1 << 5)) != 0;
    uint32_t val = 0;
    int rn_ok = 0;
    int rm_ok = 0;

    chk_loaded(rn);
    rn_ok = reg_data[rn].o == REG_VAL_OTHER;

    if (I) {
        uint32_t rot = (suffix >> 12) & 7;
        if (instr & (1 << 10)) rot |= 8;
        val = suffix & 0xff;
        switch (rot) {
        case 0:
            break;
        case 1:
            val |= val << 16;
            break;
        case 2:
            val = (val << 8) | (val << 24);
            break;
        case 3:
            val |= (val << 8) | (val << 16) | (val << 24);
            break;
        default:
            rot = rot << 1;
            if (val & 0x80) rot |= 1;
            val |= 0x80;
            val = (val >> rot) | (val << (32 - rot));
            break;
        }
        rm_ok = 1;
    }
    else {
        uint16_t rm = suffix & 0xf;
        uint32_t shift_type = (suffix >> 4) & 0x3;
        uint32_t shift_imm = ((suffix >> 10) & 0x1c) | ((suffix >> 6) & 0x3);
        chk_loaded(rm);
        rm_ok = reg_data[rm].o == REG_VAL_OTHER;
        val = calc_shift(shift_type, shift_imm, reg_data[rm].v);
    }

    switch (op) {
    case 0:
        if (rd != 15) {
            /* AND (register) */
            reg_data[rd].v = reg_data[rn].v & val;
            reg_data[rd].o = rn_ok && rm_ok ? REG_VAL_OTHER : 0;
        }
        else if (!S) {
            /* UNPREDICTABLE */
            reg_data[rd].o = 0;
        }
        else {
            /* TST (register) */
        }
        break;
    case 1:
        /* BIC (register) */
        reg_data[rd].v = reg_data[rn].v & ~val;
        reg_data[rd].o = rn_ok && rm_ok ? REG_VAL_OTHER : 0;
        break;
    case 2:
        if (rn != 15) {
            /* ORR (register) */
            reg_data[rd].v = reg_data[rn].v | val;
            reg_data[rd].o = rn_ok && rm_ok ? REG_VAL_OTHER : 0;
        }
        else {
            /* MOV (register) or shift */
            reg_data[rd].v = val;
            reg_data[rd].o = rm_ok ? REG_VAL_OTHER : 0;
        }
        break;
    case 3:
        if (rn != 15) {
            /* ORN (register) */
            reg_data[rd].v = reg_data[rn].v | ~val;
            reg_data[rd].o = rn_ok && rm_ok ? REG_VAL_OTHER : 0;
        }
        else {
            /* MVN (register) */
            reg_data[rd].v = ~val;
            reg_data[rd].o = rm_ok ? REG_VAL_OTHER : 0;
        }
        break;
    case 4:
        if (rd != 15) {
            /* EOR (register) */
            reg_data[rd].v = reg_data[rn].v ^ val;
            reg_data[rd].o = rn_ok && rm_ok ? REG_VAL_OTHER : 0;
        }
        else if (!S) {
            /* UNPREDICTABLE */
            reg_data[rd].o = 0;
        }
        else {
            /* TEQ (register) */
        }
        break;
    case 6:
        /* PKH */
        if (tb) {
            reg_data[rd].v = (reg_data[rn].v & 0xffff0000) | (val & 0x0000ffff);
            reg_data[rd].o = rn_ok && rm_ok ? REG_VAL_OTHER : 0;
        }
        else {
            reg_data[rd].v = (reg_data[rn].v & 0x0000ffff) | (val & 0xffff0000);
            reg_data[rd].o = rn_ok && rm_ok ? REG_VAL_OTHER : 0;
        }
        break;
    case 8:
        if (rd != 15) {
            /* ADD (register) */
            reg_data[rd].v = reg_data[rn].v + val;
            reg_data[rd].o = rn_ok && rm_ok ? REG_VAL_OTHER : 0;
        }
        else if (!S) {
            /* UNPREDICTABLE */
            reg_data[rd].o = 0;
        }
        else {
            /* CMN (register) */
        }
        break;
    case 10:
        /* ADC (register) */
        reg_data[rd].o = 0;
        break;
    case 11:
        /* SBC (register) */
        reg_data[rd].o = 0;
        break;
    case 13:
        if (rd != 15) {
            /* SUB (register) */
            reg_data[rd].v = reg_data[rn].v - val;
            reg_data[rd].o = rn_ok && rm_ok ? REG_VAL_OTHER : 0;
        }
        else if (!S) {
            /* UNPREDICTABLE */
            reg_data[rd].o = 0;
        }
        else {
            /* CMP (register) */
        }
        break;
    case 14:
        /* RSB (register) */
        reg_data[rd].v = val - reg_data[rn].v;
        reg_data[rd].o = rn_ok && rm_ok ? REG_VAL_OTHER : 0;
        break;
    default:
        reg_data[rd].o = 0;
        break;
    }
    return 0;
}

static int trace_thumb(void) {
    uint16_t instr;

    assert(reg_data[15].o == REG_VAL_OTHER);

    /* Check that the PC is still on Thumb alignment */
    if (reg_data[15].v & 0x1) {
        set_errno(ERR_OTHER, "PC misalignment");
        return -1;
    }

    /* Attempt to read the instruction */
    if (read_half(reg_data[15].v, &instr) < 0) return -1;

    /* Move shifted register
     *  LSL Rd, Rs, #Offset5
     *  LSR Rd, Rs, #Offset5
     *  ASR Rd, Rs, #Offset5
     */
    if ((instr & 0xe000) == 0x0000 && (instr & 0x1800) != 0x1800) {
        int signExtend;
        uint32_t op      = (instr & 0x1800) >> 11;
        uint32_t offset5 = (instr & 0x07c0) >>  6;
        uint32_t rs      = (instr & 0x0038) >>  3;
        uint32_t rd      = (instr & 0x0007);

        chk_loaded(rs);
        reg_data[rd].o = 0;
        if (reg_data[rs].o) {
            switch(op) {
            case 0: /* LSL */
                reg_data[rd].v = reg_data[rs].v << offset5;
                reg_data[rd].o = REG_VAL_OTHER;
                break;

            case 1: /* LSR */
                reg_data[rd].v = reg_data[rs].v >> offset5;
                reg_data[rd].o = REG_VAL_OTHER;
                break;

            case 2: /* ASR */
                signExtend = (reg_data[rs].v & 0x8000) != 0;
                reg_data[rd].v = reg_data[rs].v >> offset5;
                if (signExtend) reg_data[rd].v |= 0xffffffff << (32 - offset5);
                reg_data[rd].o = REG_VAL_OTHER;
                break;
            }
        }
    }
    /* add/subtract
     *  ADD Rd, Rs, Rn
     *  ADD Rd, Rs, #Offset3
     *  SUB Rd, Rs, Rn
     *  SUB Rd, Rs, #Offset3
     */
    else if ((instr & 0xf800) == 0x1800) {
        int I  = (instr & 0x0400) != 0;
        int op = (instr & 0x0200) != 0;
        uint32_t rn = (instr & 0x01c0) >> 6;
        uint32_t rs = (instr & 0x0038) >> 3;
        uint32_t rd = (instr & 0x0007);

        if (!I) {
            chk_loaded(rs);
            chk_loaded(rn);
            reg_data[rd].o = 0;
            if (reg_data[rs].o && reg_data[rn].o) {
                reg_data[rd].v = op ? reg_data[rs].v - reg_data[rn].v : reg_data[rs].v + reg_data[rn].v;
                reg_data[rd].o = REG_VAL_OTHER;
            }
        }
        else {
            chk_loaded(rs);
            reg_data[rd].o = 0;
            if (reg_data[rs].o) {
                reg_data[rd].v = op ? reg_data[rs].v - rn : reg_data[rs].v + rn;
                reg_data[rd].o = REG_VAL_OTHER;
            }
        }
    }
    /* move/compare/add/subtract immediate
     *  MOV Rd, #Offset8
     *  CMP Rd, #Offset8
     *  ADD Rd, #Offset8
     *  SUB Rd, #Offset8
     */
    else if ((instr & 0xe000) == 0x2000) {
        uint8_t op      = (instr & 0x1800) >> 11;
        uint8_t rd      = (instr & 0x0700) >>  8;
        uint8_t offset8 = (instr & 0x00ff);

        switch(op) {
        case 0: /* MOV */
            reg_data[rd].v = offset8;
            reg_data[rd].o = REG_VAL_OTHER;
            break;

        case 1: /* CMP */
            /* Irrelevant to unwinding */
            break;

        case 2: /* ADD */
            chk_loaded(rd);
            reg_data[rd].v += offset8;
            break;

        case 3: /* SUB */
            chk_loaded(rd);
            reg_data[rd].v -= offset8;
            break;
        }
    }
    /* ALU operations
     *  AND Rd, Rs
     *  EOR Rd, Rs
     *  LSL Rd, Rs
     *  LSR Rd, Rs
     *  ASR Rd, Rs
     *  ADC Rd, Rs
     *  SBC Rd, Rs
     *  ROR Rd, Rs
     *  TST Rd, Rs
     *  NEG Rd, Rs
     *  CMP Rd, Rs
     *  CMN Rd, Rs
     *  ORR Rd, Rs
     *  MUL Rd, Rs
     *  BIC Rd, Rs
     *  MVN Rd, Rs
     */
    else if ((instr & 0xfc00) == 0x4000) {
        uint32_t op = (instr & 0x03c0) >> 6;
        uint32_t rs = (instr & 0x0038) >> 3;
        uint32_t rd = (instr & 0x0007);

        /* Propagate data origins */
        switch (op) {
        case 0: /* AND */
        case 1: /* EOR */
        case 2: /* LSL */
        case 3: /* LSR */
        case 4: /* ASR */
        case 7: /* ROR */
        case 12: /* ORR */
        case 13: /* MUL */
        case 14: /* BIC */
            chk_loaded(rd);
            chk_loaded(rs);
            reg_data[rd].o = reg_data[rd].o && reg_data[rs].o ? REG_VAL_OTHER : 0;
            break;

        case 5: /* ADC */
        case 6: /* SBC */
            /* C-bit not tracked */
            reg_data[rd].o = 0;
            break;

        case 8: /* TST */
        case 10: /* CMP */
        case 11: /* CMN */
            /* Nothing propagated */
            break;

        case 9: /* NEG */
        case 15: /* MVN */
            chk_loaded(rs);
            reg_data[rd].o = reg_data[rs].o ? REG_VAL_OTHER : 0;
            break;
        }

        /* Perform operation */
        switch (op) {
        case 0: /* AND */
            reg_data[rd].v &= reg_data[rs].v;
            break;

        case 1: /* EOR */
            reg_data[rd].v ^= reg_data[rs].v;
            break;

        case 2: /* LSL */
            reg_data[rd].v <<= reg_data[rs].v;
            break;

        case 3: /* LSR */
            reg_data[rd].v >>= reg_data[rs].v;
            break;

        case 4: /* ASR */
            if (reg_data[rd].v & 0x80000000) {
                reg_data[rd].v >>= reg_data[rs].v;
                reg_data[rd].v |= 0xffffffff << (32 - reg_data[rs].v);
            }
            else {
                reg_data[rd].v >>= reg_data[rs].v;
            }
            break;

        case 5: /* ADC */
        case 6: /* SBC */
        case 8: /* TST */
        case 10: /* CMP */
        case 11: /* CMN */
            break;

        case 7: /* ROR */
            reg_data[rd].v = (reg_data[rd].v >> reg_data[rs].v) |
                            (reg_data[rd].v << (32 - reg_data[rs].v));
            break;

        case 9: /* NEG */
            reg_data[rd].v = ~reg_data[rs].v + 1;
            break;

        case 12: /* ORR */
            reg_data[rd].v |= reg_data[rs].v;
            break;

        case 13: /* MUL */
            reg_data[rd].v *= reg_data[rs].v;
            break;

        case 14: /* BIC */
            reg_data[rd].v &= !reg_data[rs].v;
            break;

        case 15: /* MVN */
            reg_data[rd].v = !reg_data[rs].v;
            break;
        }
    }
    /* Special data instructions and branch and exchange */
    else if ((instr & 0xfc00) == 0x4400) {
        uint8_t op = (instr & 0x0300) >> 8;
        int h1 = (instr & 0x0080) != 0;
        int h2 = (instr & 0x0040) != 0;
        uint8_t rhs = (instr & 0x0038) >> 3;
        uint8_t rhd = (instr & 0x0007);

        /* Adjust the register numbers */
        if (h2) rhs += 8;
        if (h1) rhd += 8;

        switch (op) {
        case 0: /* ADD */
            chk_loaded(rhd);
            chk_loaded(rhs);
            reg_data[rhd].v += reg_data[rhs].v;
            reg_data[rhd].o = reg_data[rhd].o && reg_data[rhs].o ? REG_VAL_OTHER : 0;
            break;

        case 1: /* CMP */
            /* Irrelevant to unwinding */
            break;

        case 2: /* MOV */
            reg_data[rhd] = reg_data[rhs];
            break;

        case 3: /* BX */
            trace_bx(rhs);
            break;
        }
    }
    /* PC-relative load: LDR Rd,[PC, #imm] */
    else if ((instr & 0xf800) == 0x4800) {
        uint8_t  rd    = (instr & 0x0700) >> 8;
        uint8_t  word8 = (instr & 0x00ff);

        /* Compute load address, adding a word to account for prefetch */
        load_reg_lazy((reg_data[15].v & (~0x3)) + 4 + (word8 << 2), rd);
    }
    /* Load/Store Register (immediate) */
    else if ((instr & 0xf000) == 0x6000) {
        uint32_t rt = instr & 0x7;
        uint32_t rn = (instr >> 3) & 0x7;
        uint32_t imm = ((instr >> 6) & 0x1f) << 2;
        int L = (instr & (1 << 11)) != 0;
        chk_loaded(rn);
        if (L) {
            if (!reg_data[rn].o) reg_data[rt].o = 0;
            else load_reg_lazy(reg_data[rn].v + imm, rt);
        }
        else if (reg_data[rn].o) {
            store_reg(reg_data[rn].v + imm, rt);
        }
    }
    /* Load Register Byte (immediate) */
    else if ((instr & 0xf800) == 0x7800) {
        uint16_t rn  = (instr >> 3) & 0x7;
        uint16_t rt  = instr & 0x7;
        chk_loaded(rn);
        reg_data[rt].o = 0;
        if (reg_data[rn].o) {
            uint16_t imm = (instr >> 6) & 0x1f;
            uint32_t addr = reg_data[rn].v + imm;
            if (load_reg(addr & ~3, rt) == 0) {
                reg_data[rt].v = (reg_data[rt].v >> (addr & 3) * 8) & 0xff;
            }
        }
    }
    /* add offset to Stack Pointer
     *  ADD sp,#+imm
     *  ADD sp,#-imm
     */
    else if ((instr & 0xff00) == 0xb000) {
        uint8_t value = (instr & 0x7f) * 4;

        chk_loaded(13);
        /* Check the negative bit */
        if (instr & 0x80) {
            reg_data[13].v -= value;
        }
        else {
            reg_data[13].v += value;
        }
    }
    /* ADD (SP plus immediate)
     * ADD<c> <Rd>,SP,#<imm>
     */
    else if ((instr & 0xf800) == 0xa800) {
        uint32_t rd = (instr >> 8) & 0x7;
        uint32_t imm8 = instr & 0xff;
        chk_loaded(13);
        if (reg_data[13].o) {
            reg_data[rd].v = reg_data[13].v + (imm8 << 2);
            reg_data[rd].o = REG_VAL_OTHER;
        }
        else {
            reg_data[rd].o = 0;
        }
    }
    /* push/pop registers
     *  PUSH {Rlist}
     *  PUSH {Rlist, LR}
     *  POP {Rlist}
     *  POP {Rlist, PC}
     */
    else if ((instr & 0xf600) == 0xb400) {
        int L = (instr & (1 << 11)) != 0;
        int R = (instr & (1 << 8)) != 0;
        uint32_t regs = (instr & 0x00ff);
        if (R) regs |= 1 << (L ? 15 : 14);
        if (trace_ldm_stm(0, 13, regs, !L, L, 0, 1, L) < 0) return -1;
    }
    /* If-Then, and hints */
    else if ((instr & 0xff00) == 0xbf00) {
        /* Does not change registers */
    }
    /* Load/Store Multiple */
    else if ((instr & 0xf000) == 0xc000) {
        uint32_t rn = (instr >> 8) & 0x7;
        uint32_t regs = instr & 0xff;
        int L = (instr & (1 << 11)) != 0;
        int W = !L || (regs & (1 << rn)) == 0;
        if (trace_ldm_stm(0, rn, regs, 0, 1, 0, W, L) < 0) return -1;
    }
    /* Conditional branch */
    else if ((instr & 0xf000) == 0xd000) {
        uint16_t cond = (instr >> 8) & 0xf;
        if (cond == 15) {
            /* Supervisor Call */
        }
        else if (cond == 14) {
            /* Permanently UNDEFINED */
            set_errno(ERR_OTHER, "Undefined instruction");
            return -1;
        }
        else {
            int32_t offset = instr & 0xff;
            if (offset & 0x0080) offset |= ~0xff;
            add_branch(reg_data[15].v + offset * 2 + 4);
        }
    }
    /* Unconditional branch */
    else if ((instr & 0xf800) == 0xe000) {
        int32_t offset = instr & 0x07ff;
        if (offset & 0x400) offset |= ~0x7ff;
        add_branch(reg_data[15].v + offset * 2 + 4);
        trace_branch = 1;
    }
    /* 32-bit Thumb instructions */
    else if ((instr & 0xf800) == 0xf000) {
        uint16_t suffix = 0;
        if (read_half(reg_data[15].v + 2, &suffix) < 0) return -1;
        if ((suffix & 0x8000) == 0) {
            if (instr & (1 << 9)) {
                if (trace_thumb_data_processing_pbi_32(instr, suffix) < 0) return -1;
            }
            else {
                if (trace_thumb_data_processing_32(instr, suffix) < 0) return -1;
            }
        }
        else {
            if (trace_thumb_branches_and_misc_32(instr, suffix) < 0) return -1;
        }
    }
    /* Load/Store single data item, Memory hints */
    else if ((instr & 0xfe00) == 0xf800) {
        uint16_t suffix = 0;
        if (read_half(reg_data[15].v + 2, &suffix) < 0) return -1;
        if ((instr & 0x0070) == 0x0010 && (suffix & 0xf000) == 0xf000) {
            /* Memory hints - don't change registers */
        }
        else {
            /* Load/Store single data item */
            uint16_t rn = instr & 0xf;
            uint16_t rt = (suffix >> 12) & 0xf;
            int W = (instr & (1 << 7)) == 0 && (suffix & (1 << 8)) != 0;
            int U = (instr & (1 << 7)) != 0 || (suffix & (1 << 9)) != 0;
            int P = (instr & (1 << 7)) != 0 || (suffix & (1 << 10)) != 0;
            uint32_t imm32 = instr & (1 << 7) ? suffix & 0xfff : suffix & 0xff;
            uint32_t addr = 0;
            chk_loaded(rn);
            addr = reg_data[rn].v;
            if (P) addr = U ? addr + imm32 : addr - imm32;
            if (instr & (1 << 4)) { /* LDR */
                if (reg_data[rn].o) {
                    if ((instr & (1 << 6)) == 0) {
                        /* Byte or half - not supported */
                        reg_data[rt].o = 0;
                    }
                    else {
                        load_reg_lazy(addr, rt);
                    }
                }
                else {
                    reg_data[rt].o = 0;
                }
            }
            else { /* STR */
                if (reg_data[rn].o) {
                    if ((instr & (1 << 6)) == 0) {
                        /* Byte or half - not supported */
                        store_invalid(addr);
                    }
                    else {
                        store_reg(addr, rt);
                    }
                }
            }
            if (!P) addr = U ? addr + imm32 : addr - imm32;
            if (W) reg_data[rn].v = addr;
        }
    }
    /* Advanced SIMD data-processing instructions */
    else if ((instr & 0xef00) == 0xef00) {
    }
    /* Advanced SIMD element or structure load/store instructions */
    else if ((instr & 0xff10) == 0xf900) {
        uint16_t rn = instr & 0xf;
        reg_data[rn].o = 0;
    }
    /* 32-bit load/store instructions */
    else if ((instr & 0xfe00) == 0xe800) {
        uint16_t suffix = 0;
        if (read_half(reg_data[15].v + 2, &suffix) < 0) return -1;
        if (trace_thumb_load_store_32(instr, suffix) < 0) return -1;
    }
    /* Data-processing (shifted register) */
    else if ((instr & 0xfe00) == 0xea00) {
        uint16_t suffix = 0;
        if (read_half(reg_data[15].v + 2, &suffix) < 0) return -1;
        if (trace_thumb_data_processing_32(instr, suffix) < 0) return -1;
    }
    /* Unknown/undecoded.  May alter some register, so invalidate file */
    else {
        unsigned i;
        for (i = 0; i < 13; i++) reg_data[i].o = 0;
        trace(LOG_STACK, "Stack crawl: unknown Thumb instruction %04x", instr);
    }

    if (!trace_return && !trace_branch) {
        /* Check next address */
        if ((instr >> 11) >= 0x1d) {
            reg_data[15].v += 4;
        }
        else {
            reg_data[15].v += 2;
        }
    }
    return 0;
}

static int trace_jazelle(void) {
    set_errno(ERR_OTHER, "Jazelle stack crawl is not supported yet");
    return -1;
}

/* Check if some instruction is a data-processing instruction */
static int is_data_processing_instr(uint32_t instr) {
    uint32_t opcode = (instr & 0x01e00000) >> 21;
    int S = (instr & 0x00100000) != 0;

    if ((instr & 0x0c000000) != 0x00000000) return 0;
    if ((instr & 0xf0000000) == 0xf0000000) return 0;
    if (!S && opcode >= 8 && opcode <= 11) return 0;
    return 1;
}

static void function_call(void) {
    /* Subroutines are expected to preserve the contents of r4 to r11 and r13 */
    unsigned i;
    for (i = 0; i <= 14; i++) {
        if (i >= 4 && i <= 11) continue;
        if (i == 13) continue;
        reg_data[i].o = 0;
    }
}

static int trace_arm_bx(uint32_t instr) {
    uint8_t rn = instr & 0xf;

    if (!reg_data[rn].o) {
        /* If conditional, continue to next instruction */
        if ((instr & 0xf0000000) != 0xe0000000) return 0;
        set_errno(ERR_OTHER, "BX to untracked register");
        return -1;
    }

    trace_bx(rn);
    return 0;
}

static void trace_arm_branch_instruction(uint32_t instr) {
    uint32_t addr = reg_data[15].v;
    uint32_t offset = (instr & 0x00ffffff) << 2;
    if (offset & 0x02000000) offset |= 0xfc000000;
    addr += offset + 8;

    add_branch(addr);

    if ((instr & 0xf0000000) == 0xe0000000) {
        /* Unconditional branch */
        trace_branch = 1;
    }
}

static void trace_arm_mrs_msr(uint32_t instr) {
    uint32_t cond = (instr >> 28) & 0xf;
    RegData * psr = &reg_data[REG_ID_CPSR];
    if (instr & (1 << 22)) {
        unsigned spsr_id = get_spsr_id();
        if (spsr_id == 0) {
            if ((instr & (1 << 21)) == 0) {
                uint32_t rd = (instr & 0x0000f000) >> 12;
                reg_data[rd].o = 0;
            }
            return;
        }
        psr = &reg_data[spsr_id];
    }
    if (instr & (1 << 21)) {
        uint32_t rn = instr & 0xf;
        if (cond != 14) {
            psr->o = 0;
        }
        else {
            uint32_t mask = 0;
            if (instr & (1 << 19)) mask |= 0xff000000;
            if (instr & (1 << 18)) mask |= 0x00ff0000;
            if (instr & (1 << 17)) mask |= 0x0000ff00;
            if (instr & (1 << 16)) mask |= 0x000000ff;
            if (mask) {
                chk_loaded(rn);
                if (!reg_data[rn].o) {
                    psr->o = 0;
                }
                else {
                    psr->v &= ~mask;
                    psr->v |= reg_data[rn].v & mask;
                    psr->o = REG_VAL_OTHER;
                }
            }
        }
    }
    else {
        uint32_t rd = (instr & 0x0000f000) >> 12;
        if (cond != 14) {
            reg_data[rd].o = 0;
        }
        else {
            reg_data[rd] = *psr;
        }
    }
}

static void trace_arm_cps(uint32_t instr) {
    if (instr & (1 << 17)) {
        uint32_t m0 = reg_data[REG_ID_CPSR].v & 0x1f;
        uint32_t m1 = instr & 0x1f;
        RegisterDefinition * def;
        for (def = get_reg_definitions(stk_ctx); def->name; def++) {
            int r = is_banked_reg_visible(def, m0);
            if (r > 0) reg_data[def->dwarf_id] = reg_data[r];
        }
        for (def = get_reg_definitions(stk_ctx); def->name; def++) {
            int r = is_banked_reg_visible(def, m1);
            if (r > 0) reg_data[r] = reg_data[def->dwarf_id];
        }
        reg_data[REG_ID_CPSR].v = (reg_data[REG_ID_CPSR].v & ~m0) | m1;
    }
}

static void trace_arm_ldr_str(uint32_t instr) {
    uint32_t cond = (instr >> 28) & 0xf;
    int I = (instr & (1 << 25)) != 0;
    int P = (instr & (1 << 24)) != 0;
    int U = (instr & (1 << 23)) != 0;
    int B = (instr & (1 << 22)) != 0;
    int W = (instr & (1 << 21)) != 0;
    int L = (instr & (1 << 20)) != 0;
    uint32_t rn = (instr & 0x000f0000) >> 16;
    uint32_t rd = (instr & 0x0000f000) >> 12;
    RegData rd_org = reg_data[rd];
    uint32_t adr = 0;

    chk_loaded(rn);
    adr = reg_data[rn].v;

    if (rn == 15) adr += 8;

    if (B || !reg_data[rn].o) {
        if (L) reg_data[rd].o = 0;
    }
    else if (cond != 14 && rd != 15) {
        /* Keep current */
    }
    else if (!I && P) {
        uint32_t offs = instr & 0xfff;
        adr = U ? adr + offs : adr - offs;
        if (W) reg_data[rn].v = adr;
        if (L) load_reg_lazy(adr, rd);
        else store_reg(adr, rd);
    }
    else if (I && P) {
        uint8_t rm = instr & 0xf;
        chk_loaded(rm);
        if (!reg_data[rm].o) {
            if (L) reg_data[rd].o = 0;
            if (W) reg_data[rn].o = 0;
        }
        else if ((instr & 0x00000ff0) == 0x00000000) {
            adr = U ? adr + reg_data[rm].v : adr - reg_data[rm].v;
            if (W) reg_data[rn].v = adr;
            if (L) load_reg_lazy(adr, rd);
            else store_reg(adr, rd);
        }
        else {
            uint32_t shift_imm = (instr & 0x00000f80) >> 7;
            uint32_t shift_type = (instr & 0x00000060) >> 5;
            uint32_t val = calc_shift(shift_type, shift_imm, reg_data[rm].v);
            adr = U ? adr + val : adr - val;
            if (W) reg_data[rn].v = adr;
            if (L) load_reg_lazy(adr, rd);
            else store_reg(adr, rd);
        }
    }
    else if (!I && !P && !W) {
        uint32_t offs = instr & 0xfff;
        if (L) load_reg_lazy(adr, rd);
        else store_reg(adr, rd);
        adr = U ? adr + offs : adr - offs;
        reg_data[rn].v = adr;
    }
    else if (I && !P && !W) {
        uint8_t rm = instr & 0xf;
        chk_loaded(rm);
        if (!reg_data[rm].o) {
            if (L) reg_data[rd].o = 0;
            reg_data[rn].o = 0;
        }
        else if ((instr & 0x00000ff0) == 0x00000000) {
            if (L) load_reg_lazy(adr, rd);
            else store_reg(adr, rd);
            adr = U ? adr + reg_data[rm].v : adr - reg_data[rm].v;
            reg_data[rn].v = adr;
        }
        else {
            uint32_t shift_imm = (instr & 0x00000f80) >> 7;
            uint32_t shift_type = (instr & 0x00000060) >> 5;
            uint32_t val = 0;
            chk_loaded(rm);
            val = calc_shift(shift_type, shift_imm, reg_data[rm].v);
            if (L) load_reg_lazy(adr, rd);
            else store_reg(adr, rd);
            adr = U ? adr + val : adr - val;
            reg_data[rn].v = adr;
        }
    }
    else if (L) {
        reg_data[rd].o = 0;
    }
    if (rd == 15) bx_write_pc();
    if (rd == 15 && rn == 13 && !I && !P && !W) { /* pop {pc} */
        /* Found the return instruction */
        trace_return = 1;
    }
    else if (rd == 15) {
        chk_loaded(15);
        add_branch(reg_data[15].v);
        if (cond == 14) trace_branch = 1;
        else reg_data[15].v = rd_org.v + 4;
    }
}

static void trace_arm_extra_ldr_str(uint32_t instr) {
    /* Extra load/store instructions */
    uint32_t cond = (instr >> 28) & 0xf;
    int T = (instr & (1 << 24)) == 0 && (instr & (1 << 21));
    uint32_t op2 = (instr >> 5) & 3;
    uint32_t rd = (instr >> 12) & 0xf;
    int L =  op2 == 2 || (instr & (1 << 20));
    int P = (instr & (1 << 24)) != 0;
    int U = (instr & (1 << 23)) != 0;
    int W = (instr & (1 << 21)) != 0;
    int size = 0;
    int sign = 0;
    uint32_t addr = 0;
    int ok = 1;

    if (op2 == 1) {
        /* halfword */
        size = 2;
    }
    else if (instr & (1 << 20)) {
        /* signed byte and halfword */
        size = op2 == 2 ? 1 : 2;
        sign = 1;
    }
    else {
        size = 8;
        T = 0;
    }

    if (cond != 14 || size != 8 || sign || T) {
        /* May be next time */
        ok = 0;
    }
    else if (instr & (1 << 22)) {
        uint32_t rn = (instr >> 16) & 0xf;
        uint32_t imm8 = ((instr >> 4) & 0xf0) | (instr & 0x0f);
        chk_loaded(rn);
        if (reg_data[rn].o == 0) {
            ok = 0;
        }
        else if (P) {
            addr = reg_data[rn].v;
            addr = U ? addr + imm8 : addr - imm8;
            if (W) {
                reg_data[rn].o = REG_VAL_OTHER;
                reg_data[rn].v = addr;
            }
        }
        else {
            addr = reg_data[rn].v;
            reg_data[rn].v = U ? addr + imm8 : addr - imm8;
        }
    }
    else {
        uint32_t rn = (instr >> 16) & 0xf;
        uint32_t rm = instr & 0xf;
        uint32_t offs = reg_data[rm].v;
        chk_loaded(rn);
        chk_loaded(rm);
        if (reg_data[rn].o == 0 || reg_data[rm].o == 0) {
            ok = 0;
        }
        else if (P) {
            addr = reg_data[rn].v;
            addr = U ? addr + offs : addr - offs;
            if (W) reg_data[rn].v = addr;
        }
        else {
            addr = reg_data[rn].v;
            reg_data[rn].v = U ? addr + offs : addr - offs;
        }
    }

    if (ok && L) {
        load_reg_lazy(addr, rd);
        if (size == 8) load_reg_lazy(addr + 4, (rd + 1) & 0xf);
    }
    else if (ok) {
        store_reg(addr, rd);
        if (size == 8) store_reg(addr + 4, (rd + 1) & 0xf);
    }
    else if (L) {
        reg_data[rd].o = 0;
        if (size == 8) reg_data[(rd + 1) & 0xf].o = 0;
    }
}

static uint32_t modified_immediate_constant(uint32_t instr) {
    uint8_t shift_dist  = (instr & 0x0f00) >> 8;
    uint8_t shift_const = (instr & 0x00ff);

    /* rotate const right by 2 * shift_dist */
    shift_dist *= 2;
    return (shift_const >> shift_dist) | (shift_const << (32 - shift_dist));
}

static void trace_arm_data_processing_instr(uint32_t instr) {
    uint32_t cond = (instr >> 28) & 0xf;
    int I = (instr & 0x02000000) != 0;
    uint32_t opcode = (instr & 0x01e00000) >> 21;
    uint32_t rn = (instr & 0x000f0000) >> 16;
    uint32_t rd = (instr & 0x0000f000) >> 12;
    uint32_t operand2 = (instr & 0x00000fff);
    uint32_t op1val = 0;
    uint32_t op2val = 0;
    uint32_t op2origin = 0;

    /* Decode operand 2 */
    if (I) {
        op2val    = modified_immediate_constant(operand2);
        op2origin = REG_VAL_OTHER;
    }
    else {
        /* Register and shift */
        uint8_t rm = (operand2 & 0x000f);
        uint8_t reg_shift = (operand2 & 0x0010) != 0;
        uint8_t shift_type = (operand2 & 0x0060) >> 5;
        uint32_t shift_dist = 0;

        /* Get the shift distance */
        if (reg_shift) {
            uint8_t rs = (operand2 & 0x0f00) >> 8;
            if (operand2 & 0x00800) {
                op2origin = 0;
            }
            else if (rs == 15) {
                op2origin = 0;
            }
            else {
                chk_loaded(rs);
                shift_dist = reg_data[rs].v;
                op2origin = reg_data[rs].o;
            }
        }
        else {
            shift_dist  = (operand2 & 0x0f80) >> 7;
            op2origin = REG_VAL_OTHER;
        }

        if (!op2origin) {
            op2val = 0;
        }
        else if (shift_type == 0 && shift_dist == 0 && opcode == 13) {
            /* MOV rd,rm */
            if (rd == 15) chk_loaded(rm);
            op2origin = reg_data[rm].o;
            op2val = reg_data[rm].v;
            if (rm == 15) op2val += 8;
        }
        else {
            /* Apply the shift type to the source register */
            chk_loaded(rm);
            op2val = reg_data[rm].v;
            if (rm == 15) op2val += 8;
            switch (shift_type) {
            case 0: /* logical left */
                op2val = op2val << shift_dist;
                break;
            case 1: /* logical right */
                if (!reg_shift && shift_dist == 0) shift_dist = 32;
                op2val = op2val >> shift_dist;
                break;
            case 2: /* arithmetic right */
                if (!reg_shift && shift_dist == 0) shift_dist = 32;
                if (op2val & 0x80000000) {
                    /* Register shifts maybe greater than 32 */
                    if (shift_dist >= 32) {
                        op2val = 0xffffffff;
                    }
                    else {
                        op2val = op2val >> shift_dist;
                        op2val |= 0xffffffff << (32 - shift_dist);
                    }
                }
                else {
                    op2val = op2val >> shift_dist;
                }
                break;
            case 3: /* rotate right */
                if (!reg_shift && shift_dist == 0) {
                    /* Rotate right with extend */
                    if (reg_data[REG_ID_CPSR].o == 0) {
                        op2origin = 0;
                        op2val = 0;
                    }
                    else {
                        op2val = op2val >> 1;
                        assert(reg_data[REG_ID_CPSR].o == REG_VAL_OTHER);
                        if (reg_data[REG_ID_CPSR].v & (1 << 29)) op2val |= 0x80000000;
                    }
                }
                else {
                    /* Limit shift distance to 0-31 in case of register shift */
                    shift_dist &= 0x1f;
                    op2val = (op2val >> shift_dist) |
                             (op2val << (32 - shift_dist));
                }
                break;
            }

            /* Decide the data origin */
            op2origin = reg_data[rm].o ? REG_VAL_OTHER : 0;
        }
    }

    if (opcode == 2 && rd == 15 && (instr & (1 << 20)) == 0 && rn != 14 && I) {
        chk_loaded(14);
        if (reg_data[14].o && reg_data[14].v == reg_data[15].v + 4) {
            /* SUB PC,R0,#31 - special form of a function call */
            function_call();
            return;
        }
    }

    if (rd == 15 && cond != 14) {
        /* Conditional branch, trace both directions */
        if (op2origin) {
            assert(op2origin == REG_VAL_OTHER);
            if (opcode == 13) {
                add_branch(op2val);
            }
            else if (opcode == 15) {
                add_branch(~op2val);
            }
            else {
                chk_loaded(rn);
                if (reg_data[rn].o) {
                    switch (opcode) {
                    case  0: add_branch(reg_data[rn].v & op2val); break;
                    case  1: add_branch(reg_data[rn].v ^ op2val); break;
                    case  2: add_branch(reg_data[rn].v - op2val); break;
                    case  3: add_branch(op2val - reg_data[rn].v); break;
                    case  4: add_branch(reg_data[rn].v + op2val); break;
                    case 12: add_branch(reg_data[rn].v | op2val); break;
                    case 14: add_branch(reg_data[rn].v & ~op2val); break;
                    }
                }
            }
        }
        return;
    }

    /* Propagate register validity */
    switch (opcode) {
    case  0: /* AND: Rd:= Op1 AND Op2 */
    case  1: /* EOR: Rd:= Op1 EOR Op2 */
    case  2: /* SUB: Rd:= Op1 - Op2 */
    case  3: /* RSB: Rd:= Op2 - Op1 */
    case  4: /* ADD: Rd:= Op1 + Op2 */
    case 12: /* ORR: Rd:= Op1 OR Op2 */
    case 14: /* BIC: Rd:= Op1 AND NOT Op2 */
        chk_loaded(rn);
        if (reg_data[rn].o && op2origin && cond == 14) {
            reg_data[rd].o = REG_VAL_OTHER;
        }
        else {
            reg_data[rd].o = 0;
        }
        break;
    case  5: /* ADC: Rd:= Op1 + Op2 + C */
    case  6: /* SBC: Rd:= Op1 - Op2 + C */
    case  7: /* RSC: Rd:= Op2 - Op1 + C */
        chk_loaded(rn);
        if (reg_data[REG_ID_CPSR].o && reg_data[rn].o && op2origin && cond == 14) {
            reg_data[rd].o = REG_VAL_OTHER;
        }
        else {
            reg_data[rd].o = 0;
        }
        break;
    case  8: /* TST: set condition codes on Op1 AND Op2 */
    case  9: /* TEQ: set condition codes on Op1 EOR Op2 */
    case 10: /* CMP: set condition codes on Op1 - Op2 */
    case 11: /* CMN: set condition codes on Op1 + Op2 */
        break;
    case 13: /* MOV: Rd:= Op2 */
    case 15: /* MVN: Rd:= NOT Op2 */
        if (cond == 14) {
            reg_data[rd].o = op2origin;
        }
        else {
            reg_data[rd].o = 0;
        }
        break;
    }

    op1val = reg_data[rn].v;
    /* Account for pre-fetch by adjusting PC */
    if (rn == 15) {
        /* If the shift amount is specified in the instruction,
         *  the PC will be 8 bytes ahead. If a register is used
         *  to specify the shift amount the PC will be 12 bytes
         *  ahead.
         */
        if (!I && (operand2 & 0x0010))
            op1val += 12;
        else
            op1val += 8;
    }

    /* Compute values */
    switch (opcode) {
    case  0: /* AND: Rd:= Op1 AND Op2 */
        reg_data[rd].v = op1val & op2val;
        break;
    case  1: /* EOR: Rd:= Op1 EOR Op2 */
        reg_data[rd].v = op1val ^ op2val;
        break;
    case  2: /* SUB: Rd:= Op1 - Op2 */
        reg_data[rd].v = op1val - op2val;
        break;
    case  3: /* RSB: Rd:= Op2 - Op1 */
        reg_data[rd].v = op2val - op1val;
        break;
    case  4: /* ADD: Rd:= Op1 + Op2 */
        reg_data[rd].v = op1val + op2val;
        break;
    case  5: /* ADC: Rd:= Op1 + Op2 + C */
        reg_data[rd].v = op1val + op2val;
        if (reg_data[REG_ID_CPSR].v & (1 << 29)) reg_data[rd].v++;
        break;
    case  6: /* SBC: Rd:= Op1 - Op2 + C */
        reg_data[rd].v = op1val - op2val;
        if (reg_data[REG_ID_CPSR].v & (1 << 29)) reg_data[rd].v++;
        break;
    case  7: /* RSC: Rd:= Op2 - Op1 + C */
        reg_data[rd].v = op2val - op1val;
        if (reg_data[REG_ID_CPSR].v & (1 << 29)) reg_data[rd].v++;
        break;
    case  8: /* TST: set condition codes on Op1 AND Op2 */
    case  9: /* TEQ: set condition codes on Op1 EOR Op2 */
    case 10: /* CMP: set condition codes on Op1 - Op2 */
    case 11: /* CMN: set condition codes on Op1 + Op2 */
        break;
    case 12: /* ORR: Rd:= Op1 OR Op2 */
        reg_data[rd].v = op1val | op2val;
        break;
    case 13: /* MOV: Rd:= Op2 */
        if (rd == 15) {
            uint32_t prev_instr = 0;
            uint32_t pc = reg_data[15].v;
            if (pc >= 4 && read_word(pc - 4, &prev_instr) == 0 && prev_instr == 0xe1a0e00f) {
                /* Diab Data compiler generates "mov lr, pc; mov pc, ..." for indirect function call. */
                function_call();
                return;
            }
        }
        reg_data[rd].v = op2val;
        break;
    case 14: /* BIC: Rd:= Op1 AND NOT Op2 */
        reg_data[rd].v = op1val & (~op2val);
        break;
    case 15: /* MVN: Rd:= NOT Op2 */
        reg_data[rd].v = ~op2val;
        break;
    }

    if (rd == 15) bx_write_pc();

    if (rd == 15 && !I && !(operand2 & 0x0f90) && (operand2 & 0x000f) == 14 && op2origin) {
        /* move pc, lr - return */
        trace_return = 1;
    }

    if (rd == 15 && (instr & (1 << 20)) != 0) {
        return_from_exception();
        trace_return = 1;
    }
}

static int trace_arm_ldm_stm(uint32_t instr) {
    uint32_t cond = (instr >> 28) & 0xf;
    int P = (instr & (1 << 24)) != 0;
    int U = (instr & (1 << 23)) != 0;
    int S = (instr & (1 << 22)) != 0;
    int W = (instr & (1 << 21)) != 0;
    int L = (instr & (1 << 20)) != 0;
    uint16_t rn = (instr >> 16) & 0xf;
    uint16_t regs = (instr & 0x0000ffff);

    return trace_ldm_stm(cond != 14, rn, regs, P, U, S, W, L);
}

static void trace_arm_16bit_imm(uint32_t instr) {
    uint32_t rd = (instr >> 12) & 0xf;
    uint32_t cond = (instr >> 28) & 0xf;
    if (cond != 14) {
        reg_data[rd].o = 0;
    }
    else {
        uint32_t imm = (instr & 0xfff) | ((instr & 0xf0000) >> 4);
        if (instr & (1 << 22)) { /* MOVT */
            chk_loaded(rd);
            if (reg_data[rd].o) {
                imm = (reg_data[rd].v & 0xffff) | (imm << 16);
                reg_data[rd].o = REG_VAL_OTHER;
                reg_data[rd].v = imm;
            }
        }
        else { /* MOVW */
            if (imm & 0x8000) imm |= 0xffff0000;
            reg_data[rd].o = REG_VAL_OTHER;
            reg_data[rd].v = imm;
        }
    }
}

static void trace_coprocessor_instr(uint32_t instr) {
    uint32_t cond = (instr >> 28) & 0xf;
    unsigned i;

    if (cond != 15 && (instr & 0x0f000e10) == 0x0e000a00) {
        /* VFP data processing */
        return;
    }

    if ((instr & 0x0e000000) == 0x0c000000) {
        int P = (instr & (1 << 24)) != 0;
        int U = (instr & (1 << 23)) != 0;
        int D = (instr & (1 << 22)) != 0;
        int W = (instr & (1 << 21)) != 0;
        int L = (instr & (1 << 20)) != 0;
        if ((instr & 0x00000e00) == 0x00000a00) {
            uint32_t rn = (instr >> 16) & 0xf;
            if (!P && !U && !W) {
                if (D && (instr & 0x000000d0) == 0x00000010) {
                    /* 64-bit transfers between ARM core and extension registers */
                    if (L) {
                        reg_data[(instr >> 12) & 0xf].o = 0;
                        reg_data[(instr >> 16) & 0xf].o = 0;
                    }
                    return;
                }
            }
            else if (P && !W) {
                /* vldr, vstr */
                return;
            }
            else if (P == U && W) {
                /* Undefined */
            }
            else {
                /* vldm, vstm */
                uint32_t imm8 = instr & 0xff;
                if (W && reg_data[rn].o) {
                    if (cond != 14) {
                        reg_data[rn].o = 0;
                        return;
                    }
                    chk_loaded(rn);
                    if (U) {
                        reg_data[rn].v += imm8 * 4;
                    }
                    else {
                        reg_data[rn].v -= imm8 * 4;
                    }
                    reg_data[rn].o = REG_VAL_OTHER;
                }
                return;
            }
        }
        else if (!P && !U && D && !W) {
            /* mrrc, mcrr */
            if (L) {
                reg_data[(instr >> 12) & 0xf].o = 0;
                reg_data[(instr >> 16) & 0xf].o = 0;
            }
            return;
        }
        else if (!P && !U && !D && !W) {
            /* Undefined */
        }
        else {
            /* ldc, stc */
            if (W) reg_data[(instr >> 16) & 0xf].o = 0;
            return;
        }
    }

    if ((instr & 0x0fe00f90) == 0x0ee00a10) { /* VMRS/VMSR */
        int A = (instr & (1 << 20)) != 0;
        if (A) reg_data[(instr >> 12) & 0xf].o = 0;
        return;
    }

    if ((instr & 0x0f900f10) == 0x0e000b10) { /* VMOV.sz Dn, Rn */
        return;
    }

    if ((instr & 0x0f100f10) == 0x0e100b10) { /* VMOV.sz Rn, Dn */
        reg_data[(instr >> 12) & 0xf].o = 0;
        return;
    }

    if ((instr & 0x0fe00f10) == 0x0e000a10) { /* VMOV Sn */
        int L = (instr & (1 << 20)) != 0;
        if (L) reg_data[(instr >> 12) & 0xf].o = 0;
        return;
    }

    if ((instr & 0x0f000010) == 0x0e000010) { /* MRC, MCR */
        int A = (instr & (1 << 20)) != 0;
        if (A) reg_data[(instr >> 12) & 0xf].o = 0;
        return;
    }

    if ((instr & 0x0f000010) == 0x0e000000) { /* CDP */
        return;
    }

    /* Unknown/undecoded.  May alter some register, so invalidate file */
    for (i = 0; i < 13; i++) reg_data[i].o = 0;
}

static int trace_arm(void) {
    uint32_t instr;

    assert(reg_data[15].o == REG_VAL_OTHER);

    /* Check that the PC is still on Arm alignment */
    if (reg_data[15].v & 0x3) {
        set_errno(ERR_OTHER, "PC misalignment");
        return -1;
    }

    /* Read the instruction */
    if (read_word(reg_data[15].v, &instr) < 0) return -1;

    if ((instr & 0xfff10020) == 0xf1000000) { /* CPS */
        trace_arm_cps(instr);
    }
    else if ((instr & 0xfe500000) == 0xf8400000) { /* SRS */
        int W = (instr & (1 << 21)) != 0;
        int U = (instr & (1 << 23)) != 0;
        int P = (instr & (1 << 24)) != 0;
        trace_srs(W, U, P == U, instr & 0x1f);
    }
    else if ((instr & 0xfe500000) == 0xf8100000) { /* RFE */
        int W = (instr & (1 << 21)) != 0;
        int U = (instr & (1 << 23)) != 0;
        int P = (instr & (1 << 24)) != 0;
        trace_rfe((instr >> 16) & 0xf, W, U, P == U);
    }
    else if ((instr & 0xfff00000) == 0xf5700000) { /* CLREX, DSB, DMB, ISB */
        /* No register changes */
    }
    else if ((instr & 0xfd300000) == 0xf5100000) { /* PLD */
        /* No register changes */
    }
    else if ((instr & 0xfe000000) == 0xf2000000) {
        /* Advanced SIMD data processing - no register changes */
    }
    else if ((instr & 0x0ffffff0) == 0x012fff10) {
        /* Branch and Exchange (BX)
         *  This is tested prior to data processing to prevent
         *  mis-interpretation as an invalid TEQ instruction.
         */
        if (trace_arm_bx(instr) < 0) return -1;
    }
    else if ((instr & 0x0f000000) == 0x0a000000) { /* Branch */
        trace_arm_branch_instruction(instr);
    }
    else if ((instr & 0x0f000000) == 0x0b000000 ||
             (instr & 0x0ffffff0) == 0x012fff30 ) { /* BL */
        function_call();
    }
    else if ((instr & 0x0f9000f0) == 0x01000000) { /* MRS, MSR */
        trace_arm_mrs_msr(instr);
    }
    else if ((instr & 0x0c000000) == 0x04000000) { /* LDR, STR */
        trace_arm_ldr_str(instr);
    }
    else if ((instr & 0x0fb000f0) == 0x01000090) { /* SWP, SWPB */
        reg_data[(instr >> 12) & 0xf].o = 0;
    }
    else if ((instr & 0x0f8000f0) == 0x01800090) { /* LDREX, STREX */
        int L = (instr & (1 << 20)) != 0;
        if (L) {
            int op = (instr >> 21) & 3;
            reg_data[(instr >> 12) & 0xf].o = 0;
            if (op == 1) reg_data[((instr >> 12) + 1) & 0xf].o = 0;
        }
    }
    else if ((instr & 0x0e0000f0) == 0x00000090) { /* MUL, ... */
        reg_data[(instr >> 16) & 0xf].o = 0;
    }
    else if ((instr & 0x0e000090) == 0x00000090) {
        trace_arm_extra_ldr_str(instr);
    }
    else if (is_data_processing_instr(instr)) { /* Data processing */
        trace_arm_data_processing_instr(instr);
    }
    else if ((instr & 0x0e000000) == 0x08000000) { /* Block Data Transfer - LDM, STM */
        if (trace_arm_ldm_stm(instr) < 0) return -1;
    }
    else if ((instr & 0x0fb00000) == 0x03000000) { /* 16-bit immediate load */
        trace_arm_16bit_imm(instr);
    }
    else if ((instr & 0x0ff000f0) == 0x01600010) { /* CLZ - Count Leading Zeros */
        reg_data[(instr >> 12) & 0xf].o = 0;;
    }
    else if ((instr & 0x0c000000) == 0x0c000000) {
        trace_coprocessor_instr(instr);
    }
    else if ((instr & 0x0fff0000) == 0x03200000) { /* Hints: NOP, YIELD, etc. */
        /* No register changes */
    }
    else {
        unsigned i;
        /* Unknown/undecoded. May alter some register, so invalidate file */
        for (i = 0; i < 11; i++) reg_data[i].o = 0;
        trace(LOG_STACK, "Stack crawl: unknown ARM A32 instruction %08" PRIx32, instr);
    }

    if (!trace_return && !trace_branch) {
        /* Check next address */
        if (chk_loaded(15) < 0) return -1;
        reg_data[15].v += 4;
    }
    return 0;
}

static enum ISA_TYPE get_isa_type(void) {
    uint32_t flag_t = 1 << 5;
    uint32_t flag_j = 1 << 24;
    if (cpu_type == CPU_ARMv7M) {
        flag_t = 1 << 24;
        flag_j = 0;
    }
    if (reg_data[REG_ID_CPSR].v & flag_j) return ISA_JAZELLE;
    if (reg_data[REG_ID_CPSR].v & flag_t) return ISA_THUMB;
    return ISA_ARM;
}

static int trace_instructions(void) {
    unsigned i;
    RegData org_sp = reg_data[13];
    RegData org_lr = reg_data[14];
    RegData org_pc = reg_data[15];
    RegData org_4to11[8];
    RegData org_cpsr = reg_data[REG_ID_CPSR];
    unsigned org_spsr_id = get_spsr_id();
    RegData org_spsr;
    uint32_t func_addr = 0;
    uint32_t func_size = 0;

    memcpy(org_4to11, reg_data + 4, sizeof(org_4to11));
    memset(&org_spsr, 0, sizeof(org_spsr));
    if (org_spsr_id > 0) org_spsr = reg_data[org_spsr_id];

#if ENABLE_Symbols
    if (chk_loaded(15) == 0) {
        Symbol * sym = NULL;
        int sym_class = SYM_CLASS_UNKNOWN;
        ContextAddress sym_addr = reg_data[15].v;
        ContextAddress sym_size = 0;
        if (sym_addr > 0 && !stk_frame->is_top_frame) sym_addr--;
        if (find_symbol_by_addr(stk_ctx, STACK_NO_FRAME, sym_addr, &sym) == 0 &&
                get_symbol_class(sym, &sym_class) == 0 && sym_class == SYM_CLASS_FUNCTION &&
                get_symbol_size(sym, &sym_size) == 0 && sym_size != 0 &&
                get_symbol_address(sym, &sym_addr) == 0 && sym_addr != 0 &&
                sym_addr + sym_size <= 0x100000000) {
            func_addr = (uint32_t)sym_addr;
            func_size = (uint32_t)sym_size;
            trace(LOG_STACK, "Function symbol: addr 0x%08" PRIx32 ", size 0x%08" PRIx32, func_addr, func_size);
        }
    }
#endif

    TRACE_INSTRUCTIONS_HOOK;

    for (;;) {
        unsigned t = 0;
        BranchData * b = NULL;
        if (chk_loaded(13) < 0) return -1;
        if (chk_loaded(15) < 0) return -1;
        trace(LOG_STACK, "Stack crawl: pc 0x%08" PRIx32 ", sp 0x%08" PRIx32,
            reg_data[15].o ? reg_data[15].v : 0,
            reg_data[13].o ? reg_data[13].v : 0);
        for (t = 0; t < 200; t++) {
            int error = 0;
            trace_return = 0;
            trace_branch = 0;
            if (chk_loaded(15) < 0) {
                error = errno;
            }
            else if (!reg_data[15].o) {
                error = set_errno(ERR_OTHER, "PC value not available");
            }
            else if (!reg_data[15].v) {
                error = set_errno(ERR_OTHER, "PC == 0");
            }
            else if (chk_loaded(REG_ID_CPSR) < 0) {
                error = errno;
            }
            else if (!reg_data[REG_ID_CPSR].o) {
                error = set_errno(ERR_OTHER, "CPSR value not available");
            }
            else if (func_size > 0 && (reg_data[15].v < func_addr ||
                    (func_addr + func_size > func_addr && reg_data[15].v >= func_addr + func_size))) {
                error = set_errno(ERR_OTHER, "PC outside current function");
            }
            else {
                int r = 0;
                switch (get_isa_type()) {
                case ISA_JAZELLE: r = trace_jazelle(); break;
                case ISA_THUMB: r = trace_thumb(); break;
                default: r = trace_arm(); break;
                }
                if (r < 0) error = errno;
            }
            if (!error && trace_return) {
                if (chk_loaded(13) < 0 || !reg_data[13].o) {
                    error = set_errno(ERR_OTHER, "Stack crawl: invalid SP value");
                }
            }
            if (error) {
                trace(LOG_STACK, "Stack crawl: %s", errno_to_str(error));
                break;
            }
            if (trace_return) return 0;
            if (trace_branch) break;
#if 0  /* TODO: mem hash cleanup is incorrect - it destroies hash chains */
            if (reg_data[13].o) {
                /* Remove memory hash items that point to unused stack area */
                uint16_t i;
                uint32_t sp = reg_data[13].v;
                for (i = 0; i < MEM_HASH_SIZE; i++) {
                    if (mem_data.used[i] && mem_data.a[i] < sp) {
                        mem_data.used[i] = 0;
                    }
                }
            }
#endif
        }
        if (branch_pos >= branch_cnt) break;
        b = branch_data + branch_pos++;
        mem_data = b->mem_data;
        memcpy(reg_data, b->reg_data, sizeof(reg_data));
    }
    trace(LOG_STACK, "Stack crawl: Function epilogue not found");

    EPILOGUE_NOT_FOUND_HOOK;

    reg_data[REG_ID_CPSR] = org_cpsr;
    if (func_size > 12 && (func_addr & 0x3) == 0 && chk_loaded(REG_ID_CPSR) == 0 && reg_data[REG_ID_CPSR].o) {
        /* Check for common ARM prologue pattern */
        unsigned n = 0;
        while (n < 16 && n < func_size - 1) {
            uint32_t instr = 0;
            uint32_t push_regs = 0;
            if (read_word(func_addr + n, &instr) < 0) break;
            if (get_isa_type() == ISA_ARM) {
                if ((instr & 0xffffe000) == 0xe92d4000) {
                    /* PUSH {..., lr} */
                    push_regs = instr & 0xffff;
                }
                else if ((instr & 0xffffffff) == 0xe52de004) {
                    /* PUSH {lr} */
                    push_regs |= 1 << 14;
                }
                n += 4;
            }
            else if (get_isa_type() == ISA_THUMB) {
                if ((instr & 0xfe00) == 0xb400) {
                    push_regs = instr & 0xff;
                    if (instr & (1 << 8)) push_regs |= 1 << 14;
                }
                if (((instr & 0xffff) >> 11) >= 0x1d) {
                    n += 4;
                }
                else {
                    n += 2;
                }
            }
            else {
                break;
            }
            if (push_regs) {
                reg_data[13] = org_sp;
                if (chk_loaded(13) == 0) {
                    uint32_t addr = reg_data[13].v;
                    while (n < 32 && n < func_size - 1) {
                        if (read_word(func_addr + n, &instr) < 0) break;
                        if (get_isa_type() == ISA_ARM) {
                            if ((instr & 0xfffff000) == 0xe24dd000) {
                                /* SUB sp, sp, #... */
                                addr += modified_immediate_constant(instr);
                                break;
                            }
                            n += 4;
                        }
                        else if (get_isa_type() == ISA_THUMB) {
                            if ((instr & 0xff80) == 0xb080) {
                                /* SUB sp, #... */
                                addr += (instr & 0x7f) << 2;
                            }
                            break;
                        }
                        else {
                            break;
                        }
                    }
                    for (i = 0; i < 16; i++) {
                        if (push_regs & (1 << i)) {
                            reg_data[i].o = REG_VAL_STACK;
                            reg_data[i].v = addr;
                            addr += 4;
                        }
                        else if (i >= 4 && i <= 11) { /* Local variables */
                            reg_data[i] = org_4to11[i - 4];
                        }
                        else {
                            reg_data[i].o = 0;
                        }
                    }
                    reg_data[13].o = REG_VAL_OTHER;
                    reg_data[13].v = addr;
                    reg_data[15] = reg_data[14];
                    reg_data[14].o = 0;
                    bx_write_pc();
                    return 0;
                }
                break;
            }
        }
    }

    for (i = 0; i < REG_DATA_SIZE; i++) {
        if (i >= 4 && i <= 11) { /* Local variables */
            reg_data[i] = org_4to11[i - 4];
        }
        else {
            reg_data[i].o = 0;
        }
    }
    if (org_cpsr.o && org_spsr.o && ((org_cpsr.v & 0x1f) > 0x10) && org_lr.o) {
        reg_data[REG_ID_CPSR] = org_cpsr;
        reg_data[org_spsr_id] = org_spsr;
        reg_data[15] = org_lr;
        return_from_exception();
        bx_write_pc();
        return 0;
    }
    if (org_sp.v != 0 && org_lr.v != 0 && org_pc.v != org_lr.v) {
        reg_data[13] = org_sp;
        reg_data[15] = org_lr;
        bx_write_pc();
    }
    return 0;
}

static int trace_frame(StackFrame * frame, StackFrame * down) {
    RegisterDefinition * defs = get_reg_definitions(frame->ctx);
    RegisterDefinition * def = NULL;
    unsigned spsr_id = 0;

#if USE_MEM_CACHE
    unsigned i;
    for (i = 0; i < MEM_CACHE_SIZE; i++) mem_cache[i].size = 0;
#endif

    if (defs == NULL) {
        set_errno(ERR_OTHER, "Context has no registers");
        return -1;
    }

    stk_ctx = frame->ctx;
    stk_frame = frame;
    memset(&reg_data, 0, sizeof(reg_data));
    memset(&mem_data, 0, sizeof(mem_data));
    branch_pos = 0;
    branch_cnt = 0;

    for (def = defs; def->name; def++) {
        if ((def->dwarf_id >= 13 && def->dwarf_id <= 15) || def->dwarf_id == REG_ID_CPSR) {
            uint64_t v = 0;
            if (read_reg_value(frame, def, &v) < 0) continue;
            reg_data[def->dwarf_id].v = (uint32_t)v;
            reg_data[def->dwarf_id].o = REG_VAL_OTHER;
        }
        else if (def->dwarf_id >= 0 && def->dwarf_id < REG_DATA_SIZE) {
            reg_data[def->dwarf_id].v = (uint32_t)(def - defs);
            reg_data[def->dwarf_id].o = REG_VAL_FRAME;
        }
    }

    if (cpu_type == CPU_ARM32 && reg_data[15].o && reg_data[REG_ID_CPSR].o) {
        const char * isa = NULL;
        ContextAddress addr = 0;
        ContextAddress size = 0;
        assert(reg_data[15].o == REG_VAL_OTHER);
        assert(reg_data[REG_ID_CPSR].o == REG_VAL_OTHER);
        if (get_context_isa(stk_ctx, reg_data[15].v, &isa, &addr, &size) == 0 && isa != NULL) {
            if (strcmp(isa, "ARM") == 0) reg_data[REG_ID_CPSR].v &= ~(1 << 5);
            if (strcmp(isa, "Thumb") == 0) reg_data[REG_ID_CPSR].v |= (1 << 5);
        }
    }

    spsr_id = get_spsr_id();
    if (spsr_id == 0 || reg_data[spsr_id].o == 0 || reg_data[14].o == 0 || reg_data[14].v == 0) {
        if (reg_data[13].v == 0) return 0;
    }

    if (trace_instructions() < 0) return -1;

    for (def = defs; def->name; def++) {
        if (reg_data[REG_ID_CPSR].o && def->dwarf_id >= 144) {
            int r = is_banked_reg_visible(def, reg_data[REG_ID_CPSR].v & 0x1f);
            if (r > 0) reg_data[def->dwarf_id] = reg_data[r];
        }
        if (def->dwarf_id >= 0 && def->dwarf_id < REG_DATA_SIZE) {
            int r = def->dwarf_id;
#if ENABLE_StackRegisterLocations
            if (r < 13 && (reg_data[r].o == REG_VAL_ADDR || reg_data[r].o == REG_VAL_STACK)) {
                int valid = 0;
                uint32_t data = 0;
                uint32_t addr = reg_data[r].v;
                LocationExpressionCommand * cmds = NULL;
                if (mem_hash_read(addr, &data, &valid) == 0) {
                    if (valid && write_reg_value(down, def, data) < 0) return -1;
                    continue;
                }
                cmds = (LocationExpressionCommand *)tmp_alloc_zero(sizeof(LocationExpressionCommand) * 2);
                cmds[0].cmd = SFT_CMD_NUMBER;
                cmds[0].args.num = reg_data[r].v;
                cmds[1].cmd = SFT_CMD_RD_MEM;
                cmds[1].args.mem.size = 4;
                if (write_reg_location(down, def, cmds, 2) == 0) {
                    down->has_reg_data = 1;
                    continue;
                }
            }
            else if (r != 13 && reg_data[r].o == REG_VAL_FRAME) {
                LocationExpressionCommand * cmds = (LocationExpressionCommand *)tmp_alloc_zero(sizeof(LocationExpressionCommand));
                cmds[0].cmd = SFT_CMD_RD_REG;
                cmds[0].args.reg = defs + reg_data[r].v;
                if (write_reg_location(down, def, cmds, 1) == 0) {
                    down->has_reg_data = 1;
                    continue;
                }
            }
#endif
            if (chk_loaded(r) < 0) continue;
            if (!reg_data[r].o) continue;
            if (r == 13) frame->fp = reg_data[r].v;
            if (write_reg_value(down, def, reg_data[r].v) < 0) return -1;
        }
    }

    stk_frame = NULL;
    stk_ctx = NULL;
    return 0;
}

int crawl_stack_frame_arm(StackFrame * frame, StackFrame * down) {
    cpu_type = CPU_ARM32;
    return trace_frame(frame, down);
}

int crawl_stack_frame_arm_v7m(StackFrame * frame, StackFrame * down) {
    cpu_type = CPU_ARMv7M;
    return trace_frame(frame, down);
}

#endif
