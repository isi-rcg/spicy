/*******************************************************************************
 * Copyright (c) 2014, 2017 Xilinx, Inc. and others.
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

#if SERVICE_Disassembly

#include <assert.h>
#include <stdio.h>
#include <tcf/framework/context.h>
#include <tcf/services/symbols.h>
#include <machine/a64/tcf/disassembler-a64.h>

static char buf[128];
static size_t buf_pos = 0;
static DisassemblerParams * params = NULL;
static uint64_t instr_addr = 0;
static uint32_t instr = 0;

static const char * cond_names[] = {
    "eq", "ne", "cs", "cc", "mi", "pl", "vs", "vc",
    "hi", "ls", "ge", "lt", "gt", "le", "", "nv"
};

static void add_char(char ch) {
    if (buf_pos >= sizeof(buf) - 1) return;
    buf[buf_pos++] = ch;
    if (ch == ' ') while (buf_pos < 8) buf[buf_pos++] = ch;
}

static void add_str(const char * s) {
    while (*s) add_char(*s++);
}

static void add_dec_uint32(uint32_t n) {
    char s[32];
    size_t i = 0;
    do {
        s[i++] = (char)('0' + n % 10);
        n = n / 10;
    }
    while (n != 0);
    while (i > 0) add_char(s[--i]);
}

static void add_dec_uint64(uint64_t n) {
    char s[64];
    size_t i = 0;
    do {
        s[i++] = (char)('0' + (int)(n % 10));
        n = n / 10;
    }
    while (n != 0);
    while (i > 0) add_char(s[--i]);
}

static void add_hex_uint32(uint32_t n) {
    char s[32];
    size_t i = 0;
    while (i < 8) {
        uint32_t d = n & 0xf;
        if (i > 0 && n == 0) break;
        s[i++] = (char)(d < 10 ? '0' + d : 'a' + d - 10);
        n = n >> 4;
    }
    while (i > 0) add_char(s[--i]);
}

static void add_hex_uint64(uint64_t n) {
    char s[64];
    size_t i = 0;
    while (i < 16) {
        uint32_t d = n & 0xf;
        if (i > 0 && n == 0) break;
        s[i++] = (char)(d < 10 ? '0' + d : 'a' + d - 10);
        n = n >> 4;
    }
    while (i > 0) add_char(s[--i]);
}

static void add_flt_uint32(uint32_t n) {
    char str[32];
    union {
        uint32_t n;
        float f;
    } u;
    u.n = n;
    snprintf(str, sizeof(str), "%#g", u.f);
    add_str(str);
}

static void add_flt_uint64(uint64_t n) {
    char str[32];
    union {
        uint64_t n;
        double d;
    } u;
    u.n = n;
    snprintf(str, sizeof(str), "%#g", u.d);
    add_str(str);
}

static void add_vfp_expand_imm(uint32_t imm8, int dbl) {
    int sign = (imm8 >> 7) & 1;
    if (!dbl) {
        unsigned e = 8;
        unsigned f = 23;
        unsigned exp = 1 << (e - 1);
        uint32_t res = 0;
        if ((imm8 >> 6) & 1) exp -= 4;
        exp |= (imm8 >> 4) & 3;
        res |= (uint32_t)sign << 31;
        res |= (uint32_t)exp << f;
        res |= (uint32_t)(imm8 & 0xf) << (f - 4);
        add_flt_uint32(res);
    }
    else {
        unsigned e = 11;
        unsigned f = 52;
        unsigned exp = 1 << (e - 1);
        uint64_t res = 0;
        if ((imm8 >> 6) & 1) exp -= 4;
        exp |= (imm8 >> 4) & 3;
        res |= (uint64_t)sign << 63;
        res |= (uint64_t)exp << f;
        res |= (uint64_t)(imm8 & 0xf) << (f - 4);
        add_flt_uint64(res);
    }
}

static void add_reg_name(uint32_t n, int sf, int sp) {
    if (n == 31) {
        if (sp) add_str(sf ? "sp" : "wsp");
        else add_str(sf ? "xzr" : "wzr");
        return;
    }
    add_char(sf ? 'x' : 'w');
    add_dec_uint32(n);
}

static void add_fp_reg_name_q(uint32_t n, uint32_t sz, int q) {
    add_char('v');
    add_dec_uint32(n);
    add_char('.');
    switch (sz) {
    case 0: add_str(q ? "16b" : "8b"); break;
    case 1: add_str(q ? "8h" : "4h"); break;
    case 2: add_str(q ? "4s" : "2s"); break;
    default: add_str(q ? "2d" : ""); break;
    }
}

static void add_fp_reg_name(uint32_t n, uint32_t sz) {
    add_char('v');
    add_dec_uint32(n);
    add_char('.');
    switch (sz) {
    case 0: add_char('b'); break;
    case 1: add_char('h'); break;
    case 2: add_char('s'); break;
    default: add_char('d'); break;
    }
}

static void add_prfm_name(uint32_t n) {
    switch (n) {
    case 0: add_str("pldl1keep"); break;
    case 1: add_str("pldl1strm"); break;
    case 2: add_str("pldl2keep"); break;
    case 3: add_str("pldl2strm"); break;
    case 4: add_str("pldl3keep"); break;
    case 5: add_str("pldl3strm"); break;
    case 8: add_str("plil1keep"); break;
    case 9: add_str("plil1strm"); break;
    case 10: add_str("plil2keep"); break;
    case 11: add_str("plil2strm"); break;
    case 12: add_str("plil3keep"); break;
    case 13: add_str("plil3strm"); break;
    case 16: add_str("pstl1keep"); break;
    case 17: add_str("pstl1strm"); break;
    case 18: add_str("pstl2keep"); break;
    case 19: add_str("pstl2strm"); break;
    case 20: add_str("pstl3keep"); break;
    case 21: add_str("pstl3strm"); break;
    default: add_char('#'); add_dec_uint32(instr & 0x1f); break;
    }
}

static void add_addr(uint64_t addr) {
    while (buf_pos < 16) add_char(' ');
    add_str("; addr=0x");
    add_hex_uint64(addr);
#if ENABLE_Symbols
    if (params->ctx != NULL) {
        Symbol * sym = NULL;
        char * name = NULL;
        ContextAddress sym_addr = 0;
        if (find_symbol_by_addr(params->ctx, STACK_NO_FRAME, (ContextAddress)addr, &sym) < 0) return;
        if (get_symbol_name(sym, &name) < 0 || name == NULL) return;
        if (get_symbol_address(sym, &sym_addr) < 0) return;
        if (sym_addr <= addr) {
            add_str(": ");
            add_str(name);
            if (sym_addr < addr) {
                add_str(" + 0x");
                add_hex_uint64(addr - (uint64_t)sym_addr);
            }
        }
    }
#endif
}

static void add_sys_reg_name(uint32_t reg) {
    switch (reg) {
    case 55303: add_str("dczid_el0"); return;
    case 55840: add_str("fpcr"); return;
    case 55841: add_str("fpsr"); return;
    case 56962: add_str("tpidr_el0"); return;
    }
    add_dec_uint32(reg);
}

static void add_sys_reg_info(uint32_t reg) {
    uint32_t op0 = (reg >> 14) & 3;
    uint32_t op1 = (reg >> 11) & 7;
    uint32_t CRn = (reg >> 7) & 0xf;
    uint32_t CRm = (reg >> 3) & 0xf;
    uint32_t op2 = (reg >> 0) & 7;
    while (buf_pos < 20) add_char(' ');
    add_str("; op0:op1:CRn:CRm:op2 = ");
    add_dec_uint32(op0);
    add_char(':');
    add_dec_uint32(op1);
    add_char(':');
    add_dec_uint32(CRn);
    add_char(':');
    add_dec_uint32(CRm);
    add_char(':');
    add_dec_uint32(op2);
}

static void add_data_barrier_option(void) {
    uint32_t imm = (instr >> 8) & 0xf;
    switch (imm) {
    case  1: add_str("oshld"); break;
    case  2: add_str("oshst"); break;
    case  3: add_str("osh"); break;
    case  5: add_str("nshld"); break;
    case  6: add_str("nshst"); break;
    case  7: add_str("nsh"); break;
    case  9: add_str("ishld"); break;
    case 10: add_str("ishst"); break;
    case 11: add_str("ish"); break;
    case 13: add_str("ld"); break;
    case 14: add_str("st"); break;
    case 15: add_str("sy"); break;
    default:
        add_char('#');
        add_dec_uint32(imm);
        break;
    }
}

static uint64_t decode_bit_mask(int sf, int n, uint32_t imms, uint32_t immr, int immediate, uint64_t * tmask_res) {
    unsigned len = 6;
    unsigned levels = 0;
    unsigned s, r, diff, esize, d;
    uint64_t welem, telem, wmask, tmask, w_ror;
    unsigned i;

    if (!n) {
        len--;
        while (len > 0 && (imms & (1u << len)) != 0) len--;
        if (len < 1) {
            /* Reserved value */
            return 0;
        }
    }
    levels = (1u << len) - 1;
    if (immediate && (imms & levels) == levels) {
        /* Reserved value */
        return 0;
    }
    s = imms & levels;
    r = immr & levels;
    diff = s - r;
    esize = 1u << len;
    d = diff & levels;
    welem = ((uint64_t)1 << (s + 1)) - 1;
    telem = ((uint64_t)1 << (d + 1)) - 1;
    w_ror = 0;
    for (i = 0; i < esize; i++) {
        if (welem & ((uint64_t)1 << i)) {
            w_ror |= (uint64_t)1 << ((esize + i - r) % esize);
        }
    }
    wmask = 0;
    tmask = 0;
    for (i = 0; i * esize < 64; i++) {
        wmask |= w_ror << i * esize;
        tmask |= telem << i * esize;
    }
    if (!sf) {
        wmask &= 0xffffffff;
        tmask &= 0xffffffff;
    }
    if (tmask_res) *tmask_res = tmask;
    return wmask;
}

static int bfx_preferred(int sf, int uns, uint32_t imms, uint32_t immr) {
    if (imms < immr) return 0;
    if (imms == (sf ? 0x3fu : 0x1fu)) return 0;
    if (immr == 0) {
        if (!sf && (imms == 7 || imms == 15)) return 0;
        if (sf && !uns && (imms == 7 || imms == 15 || imms == 31)) return 0;
    }
    return 1;
}

static void data_processing_immediate(void) {
    if ((instr & 0x1f000000) == 0x10000000) {
        /* PC-rel. addressing */
        uint64_t base = instr_addr;
        uint64_t imm = 0;
        add_str(instr & (1u << 31) ? "adrp" : "adr");
        add_char(' ');
        add_reg_name(instr & 0x1f, 1, 1);
        add_str(", ");
        imm |= ((instr >> 29) & 0x3);
        imm |= ((instr >> 5) & 0x7ffff) << 2;
        if (imm & (1u << 20)) imm |= ~((uint64_t)(1u << 20) - 1);
        if (instr & (1u << 31)) {
            imm = imm << 12;
            base &= ~((uint64_t)0xfff);
        }
        if (imm & ((uint64_t)1u << 63)) {
            add_char('-');
            add_dec_uint64(~imm + 1);
        }
        else {
            add_char('+');
            add_dec_uint64(imm);
        }
        add_addr(base + imm);
        return;
    }

    if ((instr & 0x1f000000) == 0x11000000) {
        /* Add/subtract (immediate) */
        int sf = (instr & (1u << 31)) != 0;
        uint32_t imm = (instr >> 10) & 0xfff;
        uint32_t op = (instr >> 29) & 3;
        uint32_t rt = instr & 0x1f;
        uint32_t rn = (instr >> 5) & 0x1f;
        if (op == 0 && imm == 0 && rt != rn) {
            add_str("mov ");
            add_reg_name(rt, sf, 1);
            add_str(", ");
            add_reg_name(rn, sf, 1);
            return;
        }
        if ((op == 1 || op == 3) && rt == 31) {
            add_str(op == 1 ? "cmn " : "cmp ");
            add_reg_name((instr >> 5) & 0x1f, sf, 1);
            add_str(", #0x");
            add_hex_uint32(imm);
            switch ((instr >> 22) & 3) {
            case 1: add_str(", lsl #12"); break;
            }
            return;
        }
        switch (op) {
        case 0: add_str("add"); break;
        case 1: add_str("adds"); break;
        case 2: add_str("sub"); break;
        case 3: add_str("subs"); break;
        }
        add_char(' ');
        add_reg_name(rt, sf, 1);
        add_str(", ");
        add_reg_name(rn, sf, 1);
        add_str(", #0x");
        add_hex_uint32(imm);
        switch ((instr >> 22) & 3) {
        case 1: add_str(", lsl #12"); break;
        }
        return;
    }

    if ((instr & 0x1f800000) == 0x12000000) {
        /* Logical (immediate) */
        int sf = (instr & (1u << 31)) != 0;
        int n = (instr & (1 << 22)) != 0;
        uint32_t opc = (instr >> 29) & 3;
        uint32_t immr = (instr >> 16) & 0x3f;
        uint32_t imms = (instr >> 10) & 0x3f;
        uint32_t rd = instr & 0x1f;
        uint32_t rn = (instr >> 5) & 0x1f;
        int no_rd = 0;
        int no_rn = 0;
        if (rd == 31 && opc == 3) {
            add_str("tst");
            no_rd = 1;
        }
        else if (rn == 31 && opc == 1) {
            add_str("mov");
            no_rn = 1;
        }
        else {
            switch (opc) {
            case 0: add_str("and"); break;
            case 1: add_str("orr"); break;
            case 2: add_str("eor"); break;
            case 3: add_str("ands"); break;
            }
        }
        add_char(' ');
        if (!no_rd) {
            add_reg_name(rd, sf, 0);
            add_str(", ");
        }
        if (!no_rn) {
            add_reg_name(rn, sf, 0);
            add_str(", ");
        }
        add_str("#0x");
        add_hex_uint64(decode_bit_mask(sf, n, imms, immr, 1, NULL));
        return;
    }

    if ((instr & 0x1f800000) == 0x12800000) {
        /* Move wide (immediate) */
        int sf = (instr & (1u << 31)) != 0;
        uint32_t op = (instr >> 29) & 3;
        uint32_t hw = (instr >> 21) & 3;
        uint64_t imm = (instr >> 5) & 0xffff;
        if (!sf && (hw & 2) != 0) return;
        if ((op == 0 || op == 2) && (imm > 0 || hw == 0)) {
            add_str("mov");
            imm = imm << (hw * 16);
            if (op == 0) imm = ~imm;
            if (!sf) imm &= 0xffffffff;
            hw = 0;
        }
        else {
            switch (op) {
            case 0: add_str("movn"); break;
            case 1: return;
            case 2: add_str("movz"); break;
            case 3: add_str("movk"); break;
            }
        }
        add_char(' ');
        add_reg_name(instr & 0x1f, sf, 1);
        add_str(", #0x");
        add_hex_uint64(imm);
        switch (hw) {
        case 1: add_str(", lsl #16"); break;
        case 2: add_str(", lsl #32"); break;
        case 3: add_str(", lsl #48"); break;
        }
        return;
    }

    if ((instr & 0x1f800000) == 0x13000000) {
        /* Bitfield */
        int sf = (instr & (1u << 31)) != 0;
        uint32_t opc = (instr >> 29) & 3;
        uint32_t imms = (instr >> 10) & 0x3f;
        uint32_t immr = (instr >> 16) & 0x3f;
        if (opc == 0) {
            if (imms == (sf ? 0x3fu : 0x1fu)) {
                add_str("asr ");
                add_reg_name(instr & 0x1f, sf, 0);
                add_str(", ");
                add_reg_name((instr >> 5) & 0x1f, sf, 0);
                add_str(", #");
                add_dec_uint32(immr);
                return;
            }
        }
        else if (opc == 1) {
            if (imms < immr) {
                add_str("bfi");
                immr = (~immr + 1) & (sf ? 0x3f : 0x1f);
                imms++;
            }
            else {
                add_str("bfxil");
                imms = imms - immr + 1;
            }
        }
        else if (opc == 2) {
            if (imms == (sf ? 0x3fu : 0x1fu)) {
                add_str("lsr ");
                add_reg_name(instr & 0x1f, sf, 0);
                add_str(", ");
                add_reg_name((instr >> 5) & 0x1f, sf, 0);
                add_str(", #");
                add_dec_uint32(immr);
                return;
            }
            if (imms != (sf ? 0x3fu : 0x1fu) && imms + 1 == immr) {
                add_str("lsl ");
                add_reg_name(instr & 0x1f, sf, 0);
                add_str(", ");
                add_reg_name((instr >> 5) & 0x1f, sf, 0);
                add_str(", #");
                add_dec_uint32((sf ? 0x3fu : 0x1fu) - imms);
                return;
            }
        }
        if (opc == 0 || opc == 2) {
            if (imms < immr) {
                add_str(opc ? "ubfiz" : "sbfiz");
                immr = (~immr + 1) & (sf ? 0x3f : 0x1f);
                imms = imms + 1;
            }
            else if (bfx_preferred(sf, opc != 0, imms, immr)) {
                add_str(opc ? "ubfx" : "sbfx");
                imms = imms - immr + 1;
            }
            else if (immr == 0 && imms == 7) {
                add_str(opc ? "uxtb " : "sxtb ");
                add_reg_name(instr & 0x1f, sf, 0);
                add_str(", ");
                add_reg_name((instr >> 5) & 0x1f, 0, 0);
                return;
            }
            else if (immr == 0 && imms == 15) {
                add_str(opc ? "uxth " : "sxth ");
                add_reg_name(instr & 0x1f, sf, 0);
                add_str(", ");
                add_reg_name((instr >> 5) & 0x1f, 0, 0);
                return;
            }
            else if (immr == 0 && imms == 31) {
                add_str(opc ? "uxtw " : "sxtw ");
                add_reg_name(instr & 0x1f, sf, 0);
                add_str(", ");
                add_reg_name((instr >> 5) & 0x1f, 0, 0);
                return;
            }
        }
        if (buf_pos == 0) {
            switch (opc) {
            case 0: add_str("sbfm"); break;
            case 1: add_str("bfm"); break;
            case 2: add_str("ubfm"); break;
            case 3: return;
            }
        }
        add_char(' ');
        add_reg_name(instr & 0x1f, sf, 0);
        add_str(", ");
        add_reg_name((instr >> 5) & 0x1f, sf, 0);
        add_str(", #");
        add_dec_uint32(immr);
        add_str(", #");
        add_dec_uint32(imms);
        return;
    }

    if ((instr & 0x1f800000) == 0x13800000) {
        /* Extract */
        uint32_t rn = (instr >> 5) & 0x1f;
        uint32_t rm = (instr >> 16) & 0x1f;
        int no_rm = 0;
        int sf = (instr & (1u << 31)) != 0;
        switch ((instr >> 29) & 3) {
        case 0:
            if (rn == rm) {
                add_str("ror");
                no_rm = 1;
            }
            else {
                add_str("extr");
            }
            break;
        case 1: return;
        case 2: return;
        case 3: return;
        }
        add_char(' ');
        add_reg_name(instr & 0x1f, sf, 0);
        add_str(", ");
        add_reg_name(rn, sf, 0);
        if (!no_rm) {
            add_str(", ");
            add_reg_name(rm, sf, 0);
        }
        add_str(", #");
        add_dec_uint32((instr >> 10) & 0x3f);
        return;
    }
}

static void branch_exception_system(void) {
    if ((instr & 0x7c000000) == 0x14000000) {
        /* Unconditional branch (immediate) */
        int32_t imm = instr & 0x3ffffff;
        add_str(instr & (1u << 31) ? "bl" : "b");
        add_char(' ');
        if (imm & 0x02000000) {
            imm |= 0xfc000000;
            add_char('-');
            add_dec_uint32(~imm + 1);
        }
        else {
            add_char('+');
            add_dec_uint32(imm);
        }
        add_addr(instr_addr + ((int64_t)imm << 2));
        return;
    }

    if ((instr & 0x7e000000) == 0x34000000) {
        /* Compare & branch (immediate) */
        int sf = (instr & (1u << 31)) != 0;
        int32_t imm = (instr >> 5) & 0x7ffff;
        add_str(instr & (1u << 24) ? "cbnz" : "cbz");
        add_char(' ');
        add_reg_name(instr & 0x1f, sf, 1);
        add_str(", ");
        if (imm & 0x00040000) {
            imm |= 0xfffc0000;
            add_char('-');
            add_dec_uint32(~imm + 1);
        }
        else {
            add_char('+');
            add_dec_uint32(imm);
        }
        add_addr(instr_addr + ((int64_t)imm << 2));
        return;
    }

    if ((instr & 0x7e000000) == 0x36000000) {
        /* Test & branch (immediate) */
        int sf = (instr & (1u << 31)) != 0;
        int32_t imm = (instr >> 5) & 0x3fff;
        add_str(instr & (1u << 24) ? "tbnz" : "tbz");
        add_char(' ');
        add_reg_name(instr & 0x1f, sf, 1);
        add_str(", #");
        add_dec_uint32(((instr >> 19) & 0x1f) | (((instr >> 31) & 0x1) << 5));
        add_str(", ");
        if (imm & 0x00002000) {
            imm |= 0xffffe000;
            add_char('-');
            add_dec_uint32(~imm + 1);
        }
        else {
            add_char('+');
            add_dec_uint32(imm);
        }
        add_addr(instr_addr + ((int64_t)imm << 2));
        return;
    }

    if ((instr & 0xfe000000) == 0x54000000) {
        /* Conditional branch (immediate) */
        int32_t imm = (instr >> 5) & 0x7ffff;
        add_str("b.");
        add_str(cond_names[instr & 0xf]);
        add_char(' ');
        if (imm & 0x00040000) {
            imm |= 0xfffc0000;
            add_char('-');
            add_dec_uint32(~imm + 1);
        }
        else {
            add_char('+');
            add_dec_uint32(imm);
        }
        add_addr(instr_addr + ((int64_t)imm << 2));
        return;
    }

    if ((instr & 0xff000000) == 0xd4000000) {
        /* Exception generation */
        uint32_t opc = (instr >> 21) & 0x7;
        uint32_t op2 = (instr >> 2) & 0x7;
        uint32_t ll = instr & 0x3;
        uint32_t imm = (instr >> 5) & 0xffff;
        if (opc == 0 && op2 == 0 && ll == 1) add_str("svc");
        else if (opc == 0 && op2 == 0 && ll == 2) add_str("hvc");
        else if (opc == 0 && op2 == 0 && ll == 3) add_str("smc");
        else if (opc == 1 && op2 == 0 && ll == 0) add_str("brk");
        else if (opc == 2 && op2 == 0 && ll == 0) add_str("hlt");
        else if (opc == 5 && op2 == 0 && ll == 1) add_str("dcps1");
        else if (opc == 5 && op2 == 0 && ll == 2) add_str("dcps2");
        else if (opc == 5 && op2 == 0 && ll == 3) add_str("dcps3");
        else return;
        if (imm != 0 || opc != 5) {
            add_str(" #0x");
            add_hex_uint32(imm);
        }
        return;
    }

    if ((instr & 0xffc00000) == 0xd5000000) {
        /* System */
        int l = (instr & (1u << 21)) != 0;
        uint32_t op0 = (instr >> 19) & 0x3;
        uint32_t op1 = (instr >> 16) & 0x7;
        uint32_t crn = (instr >> 12) & 0xf;
        uint32_t op2 = (instr >> 5) & 0x7;
        uint32_t rt = instr & 0x1f;
        if (l == 0 && op0 == 0 && crn == 4 && rt == 31) {
            /* MSR (immediate) */
            const char * psf = NULL;
            if (op1 == 0 && op2 == 5) psf = "spsel";
            else if (op1 == 3 && op2 == 6) psf = "daifset";
            else if (op1 == 3 && op2 == 7) psf = "daifclr";
            if (psf != NULL) {
                add_str("msr ");
                add_str(psf);
                add_str(", #");
                add_dec_uint32((instr >> 8) & 0xf);
            }
        }
        else if (l == 0 && op0 == 0 && op1 == 3 && crn == 2 && rt == 31) {
            /* HINT */
            switch (op2) {
            case 1: add_str("yield"); break;
            case 2: add_str("wfe"); break;
            case 3: add_str("wfi"); break;
            case 4: add_str("sev"); break;
            case 5: add_str("sevl"); break;
            default: add_str("nop"); break;
            }
        }
        else if (l == 0 && op0 == 0 && op1 == 3 && crn == 3 && op2 == 2 && rt == 31) {
            /* CLREX */
            uint32_t imm = (instr >> 8) & 0xf;
            add_str("clrex");
            if (imm != 15) {
                add_str(", #");
                add_dec_uint32(imm);
            }
        }
        else if (l == 0 && op0 == 0 && op1 == 3 && crn == 3 && op2 == 4 && rt == 31) {
            /* DSB */
            add_str("dsb ");
            add_data_barrier_option();
        }
        else if (l == 0 && op0 == 0 && op1 == 3 && crn == 3 && op2 == 5 && rt == 31) {
            /* DMB */
            add_str("dmb ");
            add_data_barrier_option();
        }
        else if (l == 0 && op0 == 0 && op1 == 3 && crn == 3 && op2 == 6 && rt == 31) {
            /* ISB */
            uint32_t imm = (instr >> 8) & 0xf;
            add_str("isb ");
            switch (imm) {
            case 15: add_str("sy"); break;
            default:
                add_char('#');
                add_dec_uint32(imm);
                break;
            }
        }
        else if (l == 0 && op0 == 1) {
            /* SYS */
            uint32_t crm = (instr >> 8) & 0xf;
            if (op1 == 3 && crn == 7 && crm == 4 && op2 == 1) {
                add_str("dc zva, ");
                add_reg_name(rt, 1, 1);
                return;
            }
            if (op1 == 0 && crn == 7 && crm == 6 && op2 == 1) {
                add_str("dc ivac, ");
                add_reg_name(rt, 1, 1);
                return;
            }
            if (op1 == 0 && crn == 7 && crm == 6 && op2 == 2) {
                add_str("dc isw, ");
                add_reg_name(rt, 1, 1);
                return;
            }
            if (op1 == 3 && crn == 7 && crm == 10 && op2 == 1) {
                add_str("dc cvac, ");
                add_reg_name(rt, 1, 1);
                return;
            }
            if (op1 == 0 && crn == 7 && crm == 10 && op2 == 2) {
                add_str("dc csw, ");
                add_reg_name(rt, 1, 1);
                return;
            }
            if (op1 == 3 && crn == 7 && crm == 11 && op2 == 1) {
                add_str("dc cvau, ");
                add_reg_name(rt, 1, 1);
                return;
            }
            if (op1 == 3 && crn == 7 && crm == 14 && op2 == 1) {
                add_str("dc civac, ");
                add_reg_name(rt, 1, 1);
                return;
            }
            if (op1 == 0 && crn == 7 && crm == 14 && op2 == 2) {
                add_str("dc cisw, ");
                add_reg_name(rt, 1, 1);
                return;
            }
            if (op1 == 0 && crn == 7 && crm == 1 && op2 == 0) {
                add_str("ic ialluis, ");
                return;
            }
            if (op1 == 0 && crn == 7 && crm == 5 && op2 == 0) {
                add_str("ic iallu, ");
                return;
            }
            if (op1 == 3 && crn == 7 && crm == 5 && op2 == 1) {
                add_str("ic ivau, ");
                add_reg_name(rt, 1, 1);
                return;
            }
            add_str("sys #");
            add_dec_uint32(op1);
            add_str(", c");
            add_dec_uint32(crn);
            add_str(", c");
            add_dec_uint32(crm);
            add_str(", #");
            add_dec_uint32(op2);
            if (rt != 31) {
                add_str(", ");
                add_reg_name(rt, 1, 1);
            }
        }
        else if (l == 0 && op0 >= 2) {
            /* MSR (register) */
            uint32_t reg = (instr >> 5) & 0xffff;
            add_str("msr ");
            add_sys_reg_name(reg);
            add_str(", ");
            add_reg_name(rt, 1, 1);
            add_sys_reg_info(reg);
        }
        else if (l == 1 && op0 == 1) {
            /* SYSL */
            uint32_t crm = (instr >> 8) & 0xf;
            add_str("sys ");
            add_reg_name(rt, 1, 1);
            add_str(", #");
            add_dec_uint32(op1);
            add_str(", c");
            add_dec_uint32(crn);
            add_str(", c");
            add_dec_uint32(crm);
            add_str(", #");
            add_dec_uint32(op2);
        }
        else if (l == 1 && op0 >= 2) {
            /* MRS */
            uint32_t reg = (instr >> 5) & 0xffff;
            add_str("mrs ");
            add_reg_name(rt, 1, 1);
            add_str(", ");
            add_sys_reg_name(reg);
            add_sys_reg_info(reg);
        }
        return;
    }

    if ((instr & 0xfe000000) == 0xd6000000) {
        /* Unconditional branch (register) */
        uint32_t opc = (instr >> 21) & 0xf;
        uint32_t op2 = (instr >> 16) & 0x1f;
        uint32_t op3 = (instr >> 10) & 0x3f;
        uint32_t op4 = (instr >>  0) & 0x1f;
        uint32_t rn = (instr >> 5) & 0x1f;
        if (op2 == 31 && op3 == 0 && op4 == 0) {
            switch (opc) {
            case 0: add_str("br"); break;
            case 1: add_str("blr"); break;
            case 2: add_str("ret"); break;
            }
            if (buf_pos > 0) {
                if (opc == 2 && rn == 30) return;
                add_char(' ');
                add_reg_name(rn, 1, 1);
            }
            else if (rn == 31) {
                switch (opc) {
                case 4: add_str("eret"); break;
                case 5: add_str("drps"); break;
                }
            }
        }
        return;
    }
}

static void loads_and_stores(void) {
    if ((instr & 0x3f000000) == 0x08000000) {
        /* Load/store exclusive */
        int sz64 = 0;
        int L = (instr & (1 << 22)) != 0;
        unsigned op = ((instr >> 15) & 1) + ((instr >> 20) & 2) + ((instr >> 21) & 4);
        add_str(L ? "ld" : "st");
        switch (op) {
        case 0: add_str("xr"); break;
        case 1: add_str(L ? "axr" : "lxr"); break;
        case 2: add_str("xp"); break;
        case 3: add_str(L ? "axp" : "lxp"); break;
        case 5: add_str(L ? "ar" : "lr"); break;
        }
        switch (instr >> 30) {
        case 0: add_char('b'); break;
        case 1: add_char('h'); break;
        case 3: sz64 = 1; break;
        }
        add_char(' ');
        if (!L && op != 5) {
            add_reg_name((instr >> 16) & 0x1f, 0, 1);
            add_str(", ");
        }
        add_reg_name(instr & 0x1f, sz64, 1);
        if (op == 2 || op == 3) {
            add_str(", ");
            add_reg_name((instr >> 10) & 0x1f, sz64, 1);
        }
        add_str(", [");
        add_reg_name((instr >> 5) & 0x1f, 1, 1);
        add_str("]");
        return;
    }

    if ((instr & 0x3b000000) == 0x18000000) {
        /* Load register (literal) */
        uint32_t opc = (instr >> 30) & 3;
        int V = (instr & (1 << 26)) != 0;
        uint32_t imm = (instr >> 5) & 0x7ffff;
        switch (opc) {
        case 0:
        case 1:
            add_str("ldr");
            break;
        case 2:
            add_str(V ? "ldr" : "ldrsw");
            break;
        case 3:
            add_str("prfm");
            break;
        }
        add_char(' ');
        if (V) {
            switch (opc) {
            case 0: add_char('s'); break;
            case 1: add_char('d'); break;
            case 2: add_char('q'); break;
            case 3: buf_pos = 0; return;
            }
            add_dec_uint32(instr & 0x1f);
        }
        else if (opc == 3) {
            add_prfm_name(instr & 0x1f);
        }
        else {
            add_reg_name(instr & 0x1f, opc == 1, 1);
        }
        add_str(", ");
        if (imm & 0x40000) {
            add_char('-');
            add_dec_uint32(0x80000 - imm);
            add_addr(instr_addr - (0x80000 - imm) * 4);
        }
        else {
            add_dec_uint32(imm);
            add_addr(instr_addr + imm * 4);
        }
        return;
    }

    if ((instr & 0x3a800000) == 0x28000000) {
        /* Load/store no-allocate pair (offset) */
        /* Load/store register pair (offset) */
        uint32_t opc = (instr >> 30) & 3;
        int V = (instr & (1 << 26)) != 0;
        int L = (instr & (1 << 22)) != 0;
        int N = (instr & (1 << 24)) == 0;
        uint32_t imm = (instr >> 15) & 0x7f;
        uint32_t shift = 0;

        add_str(L ? "ld" : "st");
        if (N) add_char('n');
        add_char('p');
        if (opc == 1 && L && !V) add_str("sw");
        add_char(' ');
        if (V) {
            char ch = 0;
            switch (opc) {
            case 0: ch = 's'; shift = 2; break;
            case 1: ch = 'd'; shift = 3; break;
            case 2: ch = 'q'; shift = 4; break;
            case 3: buf_pos = 0; return;
            }
            add_char(ch);
            add_dec_uint32(instr & 0x1f);
            add_str(", ");
            add_char(ch);
            add_dec_uint32((instr >> 10) & 0x1f);
        }
        else {
            add_reg_name(instr & 0x1f, opc >= 2, 0);
            add_str(", ");
            add_reg_name((instr >> 10) & 0x1f, opc >= 2, 0);
            shift = opc >= 2 ? 3 : 2;
        }
        add_str(", [");
        add_reg_name((instr >> 5) & 0x1f, 1, 1);
        if (imm != 0) {
            add_str(", #");
            if (imm & 0x40) {
                add_char('-');
                add_dec_uint32((0x80 - imm) << shift);
            }
            else {
                add_dec_uint32(imm << shift);
            }
        }
        add_char(']');
        return;
    }

    if ((instr & 0x3b800000) == 0x28800000) {
        /* Load/store register pair (post-indexed) */
        uint32_t opc = (instr >> 30) & 3;
        int V = (instr & (1 << 26)) != 0;
        int L = (instr & (1 << 22)) != 0;
        uint32_t imm = (instr >> 15) & 0x7f;
        uint32_t shift = 0;

        add_str(L ? "ldp" : "stp");
        if (opc == 1 && L && !V) add_str("sw");
        add_char(' ');
        if (V) {
            char ch = 0;
            switch (opc) {
            case 0: ch = 's'; shift = 2; break;
            case 1: ch = 'd'; shift = 3; break;
            case 2: ch = 'q'; shift = 4; break;
            case 3: buf_pos = 0; return;
            }
            add_char(ch);
            add_dec_uint32(instr & 0x1f);
            add_str(", ");
            add_char(ch);
            add_dec_uint32((instr >> 10) & 0x1f);
        }
        else {
            add_reg_name(instr & 0x1f, opc >= 2, 0);
            add_str(", ");
            add_reg_name((instr >> 10) & 0x1f, opc >= 2, 0);
            shift = opc >= 2 ? 3 : 2;
        }
        add_str(", [");
        add_reg_name((instr >> 5) & 0x1f, 1, 1);
        add_str("], #");
        if (imm & 0x40) {
            add_char('-');
            add_dec_uint32((0x80 - imm) << shift);
        }
        else {
            add_dec_uint32(imm << shift);
        }
        return;
    }

    if ((instr & 0x3b800000) == 0x29800000) {
        /* Load/store register pair (pre-indexed) */
        uint32_t opc = (instr >> 30) & 3;
        int V = (instr & (1 << 26)) != 0;
        int L = (instr & (1 << 22)) != 0;
        uint32_t imm = (instr >> 15) & 0x7f;
        uint32_t shift = 0;

        add_str(L ? "ldp" : "stp");
        if (opc == 1 && L && !V) add_str("sw");
        add_char(' ');
        if (V) {
            char ch = 0;
            switch (opc) {
            case 0: ch = 's'; shift = 2; break;
            case 1: ch = 'd'; shift = 3; break;
            case 2: ch = 'q'; shift = 4; break;
            case 3: buf_pos = 0; return;
            }
            add_char(ch);
            add_dec_uint32(instr & 0x1f);
            add_str(", ");
            add_char(ch);
            add_dec_uint32((instr >> 10) & 0x1f);
        }
        else {
            add_reg_name(instr & 0x1f, opc >= 2, 0);
            add_str(", ");
            add_reg_name((instr >> 10) & 0x1f, opc >= 2, 0);
            shift = opc >= 2 ? 3 : 2;
        }
        add_str(", [");
        add_reg_name((instr >> 5) & 0x1f, 1, 1);
        add_str(", #");
        if (imm & 0x40) {
            add_char('-');
            add_dec_uint32((0x80 - imm) << shift);
        }
        else {
            add_dec_uint32(imm << shift);
        }
        add_str("]!");
        return;
    }

    {
        char nm = 0;
        uint32_t size = (instr >> 30) & 3;
        uint32_t opc = (instr >> 22) & 3;
        int V = (instr & (1 << 26)) != 0;
        int shift = 0;

        /* if ((instr & 0x3b200c00) == 0x38000000) nm = 'u'; */
        if ((instr & 0x3b200c00) == 0x38000800) nm = 't';

        if (V) {
            add_str(opc == 0 || opc == 2 ? "st" : "ld");
            if (nm) add_char(nm);
            add_char('r');
        }
        else if (size == 3 && opc == 2) {
            add_str("prf");
            if (nm) add_char(nm);
            add_char('m');
        }
        else {
            add_str(opc == 0 ? "st" : "ld");
            if (nm) add_char(nm);
            add_char('r');
        }
        if (!V) {
            if (size == 0) {
                if (opc >= 2) add_char('s');
                add_char('b');
            }
            else if (size == 1) {
                if (opc >= 2) add_char('s');
                add_char('h');
            }
        }
        if (size == 2 && !V && opc == 2) add_str("sw");
        add_char(' ');

        if (V) {
            if (opc == 0 || opc == 1) {
                switch (size) {
                case 0: add_char('b'); break;
                case 1: add_char('h'); shift = 1; break;
                case 2: add_char('s'); shift = 2; break;
                case 3: add_char('d'); shift = 3; break;
                }
            }
            else {
                switch (size) {
                case 0: add_char('q'); shift = 4; break;
                default: shift = -1; break;
                }
            }
            add_dec_uint32(instr & 0x1f);
        }
        else if (size == 3 && opc == 2) {
            add_prfm_name(instr & 0x1f);
            shift = 3;
        }
        else {
            add_reg_name(instr & 0x1f, size == 3 || opc == 2, 0);
            shift = size;
        }

        if (shift >= 0) {
            if ((instr & 0x3b200c00) == 0x38000000) {
                /* Load/store register (unscaled immediate) */
                uint32_t imm = (instr >> 12) & 0x1ff;

                add_str(", [");
                add_reg_name((instr >> 5) & 0x1f, 1, 1);
                if (imm != 0) {
                    add_str(", #");
                    if (imm & 0x100) {
                        add_char('-');
                        add_dec_uint32(0x200 - imm);
                    }
                    else {
                        add_dec_uint32(imm);
                    }
                }
                add_char(']');
                return;
            }

            if ((instr & 0x3b200c00) == 0x38000400) {
                /* Load/store register (immediate post-indexed) */
                uint32_t imm = (instr >> 12) & 0x1ff;

                add_str(", [");
                add_reg_name((instr >> 5) & 0x1f, 1, 1);
                add_str("], #");
                if (imm & 0x100) {
                    add_char('-');
                    add_dec_uint32(0x200 - imm);
                }
                else {
                    add_dec_uint32(imm);
                }
                return;
            }

            if ((instr & 0x3b200c00) == 0x38000800) {
                /* Load/store register (unprivileged) */
                uint32_t imm = (instr >> 12) & 0x1ff;

                add_str(", [");
                add_reg_name((instr >> 5) & 0x1f, 1, 1);
                if (imm != 0) {
                    add_str(", #");
                    if (imm & 0x100) {
                        add_char('-');
                        add_dec_uint32(0x200 - imm);
                    }
                    else {
                        add_dec_uint32(imm);
                    }
                }
                add_char(']');
                return;
            }

            if ((instr & 0x3b200c00) == 0x38000c00) {
                /* Load/store register (immediate pre-indexed) */
                uint32_t imm = (instr >> 12) & 0x1ff;

                add_str(", [");
                add_reg_name((instr >> 5) & 0x1f, 1, 1);
                if (imm != 0) {
                    add_str(", #");
                    if (imm & 0x100) {
                        add_char('-');
                        add_dec_uint32(0x200 - imm);
                    }
                    else {
                        add_dec_uint32(imm);
                    }
                }
                add_str("]!");
                return;
            }

            if ((instr & 0x3b200c00) == 0x38200800) {
                /* Load/store register (register offset) */
                uint32_t option = (instr >> 13) & 7;
                uint32_t rm = (instr >> 16) & 0x1f;
                int s = (instr & (1 << 12)) != 0;
                add_str(", [");
                add_reg_name((instr >> 5) & 0x1f, 1, 1);
                add_str(", ");
                switch (option) {
                case 2:
                case 6:
                    add_reg_name(rm, 0, 1);
                    break;
                case 3:
                case 7:
                    add_reg_name(rm, 1, 1);
                    break;
                default:
                    buf_pos = 0;
                    return;
                }
                if (s || option != 3) {
                    add_str(", ");
                    switch (option) {
                    case 2: add_str("uxtw"); break;
                    case 3: add_str("lsl"); break;
                    case 6: add_str("sxtw"); break;
                    case 7: add_str("sxtx"); break;
                    default: buf_pos = 0; return;
                    }
                    if (s) {
                        add_str(" #");
                        add_dec_uint32(shift);
                    }
                }
                add_char(']');
                return;
            }

            if ((instr & 0x3b000000) == 0x39000000) {
                /* Load/store register (unsigned immediate) */
                uint32_t imm = (instr >> 10) & 0xfff;

                add_str(", [");
                add_reg_name((instr >> 5) & 0x1f, 1, 1);
                if (imm != 0) {
                    add_str(", #");
                    add_dec_uint32(imm << shift);
                }
                add_char(']');
                return;
            }
        }

        buf_pos = 0;
    }

    if ((instr & 0xbfbf0000) == 0x0c000000 || (instr & 0xbfa00000) == 0x0c800000) {
        /* AdvSIMD load/store multiple structures */
        /* AdvSIMD load/store multiple structures (post-indexed) */
        int post_indexed = (instr & (1 << 23)) != 0;
        int Q = (instr >> 30) & 1;
        int L = (instr >> 22) & 1;
        uint32_t opcode = (instr >> 12) & 0xf;
        uint32_t size = (instr >> 10) & 3;
        uint32_t rm = (instr >> 16) & 0x1f;
        uint32_t rn = (instr >> 5) & 0x1f;
        uint32_t rt = instr & 0x1f;
        unsigned rno = 0;
        unsigned sno = 0;
        switch (opcode) {
        case  0: rno = 4; sno = 4; break;
        case  2: rno = 4; sno = 1; break;
        case  4: rno = 3; sno = 3; break;
        case  6: rno = 3; sno = 1; break;
        case  7: rno = 1; sno = 1; break;
        case  8: rno = 2; sno = 2; break;
        case 10: rno = 2; sno = 1; break;
        default: return;
        }
        add_str(L ? "ld" : "st");
        if (instr & (1 << 14)) add_char('1');
        else add_char((char)('0' + sno));
        add_str(" { ");
        if (rno > 2 && rt < rt + rno - 1) {
            add_fp_reg_name_q(rt, size, Q);
            add_str("-");
            add_fp_reg_name_q(rt + rno - 1, size, Q);
        }
        else {
            unsigned i;
            for (i = 0; i < rno; i++) {
                if (i > 0) add_str(", ");
                add_fp_reg_name_q(rt + i, size, Q);
            }
        }
        add_str(" }, ");
        add_char('[');
        add_reg_name(rn, 1, 1);
        add_char(']');
        if (post_indexed) {
            add_str(", ");
            if (rm == 0x1f) {
                add_char('#');
                add_dec_uint32((Q ? 16 : 8) * rno);
            }
            else {
                add_reg_name(rm, 1, 0);
            }
        }
        return;
    }

    if ((instr & 0xbf9f0000) == 0x0d000000 || (instr & 0xbf800000) == 0x0d800000) {
        /* AdvSIMD load/store single structure */
        /* AdvSIMD load/store single structure (post-indexed) */
        int post_indexed = (instr & (1 << 23)) != 0;
        int Q = (instr >> 30) & 1;
        int L = (instr >> 22) & 1;
        int R = (instr >> 21) & 1;
        int S = (instr >> 12) & 1;
        uint32_t opcode = (instr >> 13) & 7;
        uint32_t size = (instr >> 10) & 3;
        uint32_t rm = (instr >> 16) & 0x1f;
        uint32_t rn = (instr >> 5) & 0x1f;
        unsigned sz = 0;
        unsigned n = 0;
        unsigned i;
        int r = 0;
        if (!R) {
            switch (opcode) {
            case 0: n = 1; break;
            case 1: n = 3; break;
            case 2: n = 1; break;
            case 3: n = 3; break;
            case 4: n = 1; break;
            case 5: n = 3; break;
            default: return;
            }
        }
        else {
            switch (opcode) {
            case 0: n = 2; break;
            case 1: n = 4; break;
            case 2: n = 2; break;
            case 3: n = 4; break;
            case 4: n = 2; break;
            case 5: n = 4; break;
            case 6: n = 2; r = 1; break;
            case 7: n = 4; r = 1; break;
            default: return;
            }
        }
        if (opcode == 0) {
            /* 8-bit variant */
            sz = 0;
        }
        else if (opcode == 2 && (size & 1) == 0) {
            /* 16-bit variant */
            sz = 1;
        }
        else if (opcode == 4 && size == 0) {
            /* 32-bit variant */
            sz = 2;
        }
        else {
            /* 64-bit variant */
            sz = 3;
        }
        add_str(L ? "ld" : "st");
        add_char((char)('0' + n));
        if (r) add_char('r');
        add_str(" { ");
        for (i = 0; i < n; i++) {
            if (i > 0) add_str(", ");
            add_fp_reg_name((instr & 0x1f) + i, sz);
        }
        add_str(" }[");
        add_dec_uint32(((Q << 3) | (S << 2) | size) >> sz);
        add_str("], [");
        add_reg_name(rn, 1, 1);
        add_char(']');
        if (post_indexed) {
            add_str(", ");
            if (rm == 0x1f) {
                add_char('#');
                add_dec_uint32(1 << sz);
            }
            else {
                add_reg_name(rm, 1, 0);
            }
        }
        return;
    }
}

static void data_processing_register(void) {
    if ((instr & 0x1f000000) == 0x0a000000) {
        /* Logical (shifted register) */
        int sf = (instr & (1 << 31)) != 0;
        uint32_t opc = (instr >> 29) & 3;
        uint32_t shift = (instr >> 22) & 3;
        int n = (instr & (1 << 21)) != 0;
        uint32_t imm = (instr >> 10) & 0x3f;
        uint32_t rn = (instr >> 5) & 0x1f;
        uint32_t rd = instr & 0x1f;
        int no_rd = 0;

        if (opc == 1 && shift == 0 && imm == 0 && rn == 31) {
            add_str(n ? "mvn " : "mov ");
            add_reg_name(instr & 0x1f, sf, 0);
            add_str(", ");
            add_reg_name((instr >> 16) & 0x1f, sf, 0);
            return;
        }
        if (rd == 31 && opc == 3 && !n) {
            add_str("tst");
            no_rd = 1;
        }
        else {
            switch (opc) {
            case 0: add_str(n ? "bic" : "and"); break;
            case 1: add_str(n ? "orn" : "orr"); break;
            case 2: add_str(n ? "eon" : "eor"); break;
            case 3: add_str(n ? "bics" : "ands"); break;
            }
        }
        add_char(' ');
        if (!no_rd) {
            add_reg_name(rd, sf, 0);
            add_str(", ");
        }
        add_reg_name(rn, sf, 0);
        add_str(", ");
        add_reg_name((instr >> 16) & 0x1f, sf, 0);
        if (shift != 0 || imm != 0) {
            add_str(", ");
            switch (shift) {
            case 0: add_str("lsl"); break;
            case 1: add_str("lsr"); break;
            case 2: add_str("asr"); break;
            case 3: add_str("ror"); break;
            }
            add_str(" #");
            add_dec_uint32(imm);
        }
        return;
    }

    if ((instr & 0x1f200000) == 0x0b000000) {
        /* Add/subtract (shifted register) */
        int sf = (instr & (1 << 31)) != 0;
        uint32_t imm = (instr >> 10) & 0x3f;
        uint32_t shift = (instr >> 22) & 3;
        int no_rd = 0;
        int no_rn = 0;
        if ((instr & 0x6000001f) == 0x6000001f) {
            add_str("cmp");
            no_rd = 1;
        }
        else if ((instr & 0x6000001f) == 0x2000001f) {
            add_str("cmn");
            no_rd = 1;
        }
        else if ((instr & 0x400003e0) == 0x400003e0) {
            add_str("neg");
            if (instr & (1 << 29)) add_char('s');
            no_rn = 1;
        }
        else {
            add_str(instr & (1 << 30) ? "sub" : "add");
            if (instr & (1 << 29)) add_char('s');
        }
        add_char(' ');
        if (!no_rd) {
            add_reg_name(instr & 0x1f, sf, 0);
            add_str(", ");
        }
        if (!no_rn) {
            add_reg_name((instr >> 5) & 0x1f, sf, 0);
            add_str(", ");
        }
        add_reg_name((instr >> 16) & 0x1f, sf, 0);
        if (imm != 0) {
            add_str(", ");
            switch (shift) {
            case 0: add_str("lsl"); break;
            case 1: add_str("lsr"); break;
            case 2: add_str("asr"); break;
            default: buf_pos = 0; return;
            }
            add_str(" #");
            add_dec_uint32(imm);
        }
        return;
    }

    if ((instr & 0x1f200000) == 0x0b200000) {
        /* Add/subtract (extended register) */
        int sf = (instr & (1 << 31)) != 0;
        uint32_t imm = (instr >> 10) & 7;
        uint32_t option = (instr >> 13) & 7;
        uint32_t rn = (instr >> 5) & 0x1f;
        uint32_t rd = instr & 0x1f;
        int s = instr & (1 << 29);
        int no_rd = 0;

        if ((instr & 0x6000001f) == 0x6000001f) {
            add_str("cmp");
            no_rd = 1;
        }
        else {
            add_str(instr & (1 << 30) ? "sub" : "add");
            if (s) add_char('s');
        }
        add_char(' ');
        if (!no_rd) {
            add_reg_name(rd, sf, !s);
            add_str(", ");
        }
        add_reg_name(rn, sf, 1);
        add_str(", ");
        add_reg_name((instr >> 16) & 0x1f, sf && (option == 3 || option == 7), 1);
        if (imm == 0 && !sf && option == 2) {
            /* Nothing */
        }
        else if (imm == 0 && sf && option == 3) {
            /* Nothing */
        }
        else {
            add_str(", ");
            switch (option) {
            case 0: add_str("uxtb"); break;
            case 1: add_str("uxth"); break;
            case 2: add_str("uxtw"); break;
            case 3: add_str("uxtx"); break;
            case 4: add_str("sxtb"); break;
            case 5: add_str("sxth"); break;
            case 6: add_str("sxtw"); break;
            case 7: add_str("sxtx"); break;
            }
            if (imm != 0) {
                add_str(" #");
                add_dec_uint32(imm);
            }
        }
        return;
    }

    if ((instr & 0x1fe00000) == 0x1a000000) {
        /* Add/subtract (with carry) */
        int sf = (instr & (1 << 31)) != 0;
        uint32_t opcode2 = (instr >> 10) & 0x3f;
        if (opcode2 == 0) {
            add_str(instr & (1 << 30) ? "sbc" : "adc");
            if (instr & (1 << 29)) add_char('s');
            add_char(' ');
            add_reg_name(instr & 0x1f, sf, 0);
            add_str(", ");
            add_reg_name((instr >> 5) & 0x1f, sf, 0);
            add_str(", ");
            add_reg_name((instr >> 16) & 0x1f, sf, 0);
        }
        return;
    }

    if ((instr & 0x1fe00800) == 0x1a400000) {
        /* Conditional compare (register) */
        int sf = (instr & (1 << 31)) != 0;
        int op = (instr & (1 << 30)) != 0;
        int s = (instr & (1 << 29)) != 0;
        int o2 = (instr & (1 << 10)) != 0;
        int o3 = (instr & (1 << 4)) != 0;
        if (s && !o2 && !o3) {
            uint32_t cond = (instr >> 12) & 0xf;
            add_str(op ? "ccmp" : "ccmn");
            add_char(' ');
            add_reg_name((instr >> 5) & 0x1f, sf, 0);
            add_str(", ");
            add_reg_name((instr >> 16) & 0x1f, sf, 0);
            add_str(", #0x");
            add_hex_uint32(instr & 0xf);
            add_str(", ");
            add_str(cond_names[cond]);
        }
        return;
    }

    if ((instr & 0x1fe00800) == 0x1a400800) {
        /* Conditional compare (immediate) */
        int sf = (instr & (1 << 31)) != 0;
        int op = (instr & (1 << 30)) != 0;
        int s = (instr & (1 << 29)) != 0;
        int o2 = (instr & (1 << 10)) != 0;
        int o3 = (instr & (1 << 4)) != 0;
        if (s && !o2 && !o3) {
            uint32_t cond = (instr >> 12) & 0xf;
            add_str(op ? "ccmp" : "ccmn");
            add_char(' ');
            add_reg_name((instr >> 5) & 0x1f, sf, 0);
            add_str(", #0x");
            add_hex_uint32((instr >> 16) & 0x1f);
            add_str(", #0x");
            add_hex_uint32(instr & 0xf);
            add_str(", ");
            add_str(cond_names[cond]);
        }
        return;
    }

    if ((instr & 0x1fe00000) == 0x1a800000) {
        /* Conditional select */
        int sf = (instr & (1 << 31)) != 0;
        int op = (instr & (1 << 30)) != 0;
        int s = (instr & (1 << 29)) != 0;
        uint32_t op2 = (instr >> 10) & 3;
        if (!s) {
            uint32_t cond = (instr >> 12) & 0xf;
            uint32_t rn = (instr >> 5) & 0x1f;
            uint32_t rm = (instr >> 16) & 0x1f;
            if (rn == rm && cond < 14 && op2 == 1) {
                if (op) {
                    add_str("cneg ");
                    add_reg_name(instr & 0x1f, sf, 0);
                    add_str(", ");
                    add_reg_name(rn, sf, 0);
                }
                else if (rn == 31) {
                    add_str("cset ");
                    add_reg_name(instr & 0x1f, sf, 0);
                }
                else {
                    add_str("cinc ");
                    add_reg_name(instr & 0x1f, sf, 0);
                    add_str(", ");
                    add_reg_name(rn, sf, 0);
                }
                add_str(", ");
                add_str(cond_names[cond ^ 1]);
                return;
            }
            if (rn == rm && cond < 14 && op2 == 0 && op) {
                if (rn == 31) {
                    add_str("csetm ");
                    add_reg_name(instr & 0x1f, sf, 0);
                }
                else {
                    add_str("cinv ");
                    add_reg_name(instr & 0x1f, sf, 0);
                    add_str(", ");
                    add_reg_name(rn, sf, 0);
                }
                add_str(", ");
                add_str(cond_names[cond ^ 1]);
                return;
            }
            switch (op2) {
            case 0: add_str(op ? "csinv" : "csel"); break;
            case 1: add_str(op ? "csneg" : "csinc"); break;
            default: buf_pos = 0; return;
            }
            add_char(' ');
            add_reg_name(instr & 0x1f, sf, 0);
            add_str(", ");
            add_reg_name(rn, sf, 0);
            add_str(", ");
            add_reg_name(rm, sf, 0);
            add_str(", ");
            add_str(cond_names[cond]);
        }
        return;
    }

    if ((instr & 0x1f000000) == 0x1b000000) {
        /* Data-processing (3 source) */
        int sf = (instr & (1 << 31)) != 0;
        uint32_t op54 = (instr >> 29) & 3;
        uint32_t op31 = (instr >> 21) & 7;
        int o0 = (instr & (1 << 15)) != 0;
        if (op54 == 0) {
            uint32_t ra = (instr >> 10) & 0x1f;
            int no_ra = 0;
            int no_sf = 0;
            if (op31 == 0) {
                if (!o0 && ra == 31) {
                    add_str("mul");
                    no_ra = 1;
                }
                else {
                    add_str(o0 ? "msub" : "madd");
                }
            }
            else if (sf) {
                if (op31 == 1 && !o0 && ra == 31) {
                    add_str("smull");
                    no_sf = 1;
                    no_ra = 1;
                }
                else {
                    switch (op31) {
                    case 1:
                        if (!o0 && ra == 0x1f) {
                            add_str("smull");
                            no_ra = 1;
                        }
                        else {
                            add_str(o0 ? "smsubl" : "smaddl");
                        }
                        no_sf = 1;
                        break;
                    case 2:
                        add_str(o0 ? "" : "smulh");
                        no_ra = 1;
                        break;
                    case 5:
                        if (!o0 && ra == 0x1f) {
                            add_str("umull");
                            no_ra = 1;
                        }
                        else {
                            add_str(o0 ? "umsubl" : "umaddl");
                        }
                        no_sf = 1;
                        break;
                    case 6:
                        add_str(o0 ? "" : "umulh");
                        no_ra = 1;
                        break;
                    }
                }
            }
            if (buf_pos > 0) {
                add_char(' ');
                add_reg_name(instr & 0x1f, sf, 0);
                add_str(", ");
                add_reg_name((instr >> 5) & 0x1f, !no_sf && sf, 0);
                add_str(", ");
                add_reg_name((instr >> 16) & 0x1f, !no_sf && sf, 0);
                if (!no_ra) {
                    add_str(", ");
                    add_reg_name(ra, sf, 0);
                }
                return;
            }
        }
        return;
    }

    if ((instr & 0x5fe00000) == 0x1ac00000) {
        /* Data-processing (2 source) */
        int sf = (instr & (1 << 31)) != 0;
        int s = (instr & (1 << 29)) != 0;
        uint32_t opcode = (instr >> 10) & 0x3f;

        if (!s) {
            int no_sf = 0;
            switch (opcode) {
            case 2: add_str("udiv"); break;
            case 3: add_str("sdiv"); break;
            case 8: add_str("lsl"); break;
            case 9: add_str("lsr"); break;
            case 10: add_str("asr"); break;
            case 11: add_str("ror"); break;
            case 16: add_str(sf ? "" : "crc32b"); no_sf = 1; break;
            case 17: add_str(sf ? "" : "crc32h"); no_sf = 1; break;
            case 18: add_str(sf ? "" : "crc32w"); no_sf = 1; break;
            case 19: add_str(sf ? "crc32x" : ""); no_sf = 1; break;
            case 20: add_str(sf ? "" : "crc32cb"); no_sf = 1; break;
            case 21: add_str(sf ? "" : "crc32ch"); no_sf = 1; break;
            case 22: add_str(sf ? "" : "crc32cw"); no_sf = 1; break;
            case 23: add_str(sf ? "crc32cx" : ""); no_sf = 1; break;
            }

            if (buf_pos > 0) {
                add_char(' ');
                add_reg_name(instr & 0x1f, !no_sf && sf, 0);
                add_str(", ");
                add_reg_name((instr >> 5) & 0x1f, !no_sf && sf, 0);
                add_str(", ");
                add_reg_name((instr >> 16) & 0x1f, sf, 0);
            }
        }

        return;
    }

    if ((instr & 0x5fe00000) == 0x5ac00000) {
        /* Data-processing (1 source) */
        int sf = (instr & (1 << 31)) != 0;
        int s = (instr & (1 << 29)) != 0;
        uint32_t opcode = (instr >> 10) & 0x3f;
        uint32_t opcode2 = (instr >> 16) & 0x1f;

        if (!s && opcode2 == 0) {
            switch (opcode) {
            case 0: add_str("rbit"); break;
            case 1: add_str("rev16"); break;
            case 2: add_str(sf ? "rev32" : "rev"); break;
            case 3: add_str(sf ? "rev" : ""); break;
            case 4: add_str("clz"); break;
            case 5: add_str("cls"); break;
            }
            if (buf_pos > 0) {
                add_char(' ');
                add_reg_name(instr & 0x1f, sf, 0);
                add_str(", ");
                add_reg_name((instr >> 5) & 0x1f, sf, 0);
            }
        }

        return;
    }
}

static void fp_to_fixed_point_conversions(void) {
    uint32_t sf = (instr >> 31) & 1;
    uint32_t type = (instr >> 22) & 3;
    uint32_t rmode = (instr >> 19) & 3;
    uint32_t opcode = (instr >> 16) & 7;
    uint32_t scale = (instr >> 10) & 0x3f;
    uint32_t Rn = (instr >> 5) & 0x1f;
    uint32_t Rd = instr & 0x1f;

    switch (opcode) {
    case 0:
        if (rmode == 3) add_str("fcvtzs");
        break;
    case 1:
        if (rmode == 3) add_str("fcvtzu");
        break;
    case 2:
        if (rmode == 0) add_str("scvtf");
        break;
    case 3:
        if (rmode == 0) add_str("ucvtf");
        break;
    }
    if (buf_pos == 0) return;
    add_char(' ');
    if (opcode == 2 || opcode == 3) {
        add_char(type & 1 ? 'd' : 's');
        add_dec_uint32(Rd);
        add_str(", ");
        add_reg_name(Rn, sf, 0);
    }
    else {
        add_reg_name(Rd, sf, 0);
        add_str(", ");
        add_char(type & 1 ? 'd' : 's');
        add_dec_uint32(Rn);
    }
    add_str(", #");
    add_dec_uint32(64 - scale);
}

static void fp_conditional_compare(void) {
    uint32_t M = (instr >> 31) & 1;
    uint32_t S = (instr >> 29) & 1;
    uint32_t type = (instr >> 22) & 3;
    uint32_t cond = (instr >> 12) & 0xf;
    uint32_t opcode = (instr >> 4) & 1;
    uint32_t Rm = (instr >> 16) & 0x1f;
    uint32_t Rn = (instr >> 5) & 0x1f;
    uint32_t nzcv = instr & 0xf;

    if (M || S) return;
    if (type >= 2) return;

    add_str(opcode ? "fccmpe" : "fccmp");
    add_char(' ');
    add_char(type & 1 ? 'd' : 's');
    add_dec_uint32(Rn);
    add_str(", ");
    add_char(type & 1 ? 'd' : 's');
    add_dec_uint32(Rm);
    add_str(", #");
    add_dec_uint32(nzcv);
    add_str(", ");
    add_str(cond_names[cond]);
}

static void fp_data_processing_2_source(void) {
    uint32_t type = (instr >> 22) & 3;
    uint32_t opcode = (instr >> 12) & 0xf;
    uint32_t Rm = (instr >> 16) & 0x1f;
    uint32_t Rn = (instr >> 5) & 0x1f;
    uint32_t Rd = instr & 0x1f;

    switch (opcode) {
    case 0: add_str("fmul"); break;
    case 1: add_str("fdiv"); break;
    case 2: add_str("fadd"); break;
    case 3: add_str("fsub"); break;
    case 4: add_str("fmax"); break;
    case 5: add_str("fmin"); break;
    case 6: add_str("fmaxnm"); break;
    case 7: add_str("fminmn"); break;
    case 8: add_str("fnmul"); break;
    }
    if (buf_pos == 0) return;
    add_char(' ');
    add_char(type & 1 ? 'd' : 's');
    add_dec_uint32(Rd);
    add_str(", ");
    add_char(type & 1 ? 'd' : 's');
    add_dec_uint32(Rn);
    add_str(", ");
    add_char(type & 1 ? 'd' : 's');
    add_dec_uint32(Rm);
}

static void fp_conditional_select(void) {
    uint32_t type = (instr >> 22) & 3;
    uint32_t cond = (instr >> 12) & 0xf;
    uint32_t Rm = (instr >> 16) & 0x1f;
    uint32_t Rn = (instr >> 5) & 0x1f;
    uint32_t Rd = instr & 0x1f;

    add_str("fcsel");
    add_char(' ');
    add_char(type & 1 ? 'd' : 's');
    add_dec_uint32(Rd);
    add_str(", ");
    add_char(type & 1 ? 'd' : 's');
    add_dec_uint32(Rn);
    add_str(", ");
    add_char(type & 1 ? 'd' : 's');
    add_dec_uint32(Rm);
    add_str(", ");
    add_str(cond_names[cond]);
}

static void fp_immediate(void) {
    uint32_t M = (instr >> 31) & 1;
    uint32_t S = (instr >> 29) & 1;
    uint32_t type = (instr >> 22) & 3;
    uint32_t imm8 = (instr >> 13) & 0xff;
    uint32_t imm5 = (instr >> 5) & 0x1f;
    uint32_t Rd = instr & 0x1f;

    if (M || S) return;
    if (type >= 2) return;
    if (imm5 != 0) return;

    add_str("fmov");
    add_char(' ');
    add_char(type & 1 ? 'd' : 's');
    add_dec_uint32(Rd);
    add_str(", #");
    add_vfp_expand_imm(imm8, type & 1);
}

static void fp_compare(void) {
    uint32_t M = (instr >> 31) & 1;
    uint32_t S = (instr >> 29) & 1;
    uint32_t type = (instr >> 22) & 3;
    uint32_t opcode = instr  & 0x1f;
    uint32_t Rn = (instr >> 5) & 0x1f;
    uint32_t Rm = (instr >> 16) & 0x1f;
    int zero = 0;
    char wn = 0;

    switch (type) {
    case 0: wn = 's'; break;
    case 1: wn = 'd'; break;
    default: return;
    }

    if (M) return;
    if (S) return;

    zero = (opcode & 0x08) != 0;

    switch (opcode & ~0x08) {
    case 0x00:
        add_str("fcmp");
        break;
    case 0x10:
        add_str("fcmpe");
        break;
    default:
        return;
    }

    add_char(' ');
    add_char(wn);
    add_dec_uint32(Rn);
    add_str(", ");
    if (zero) {
        add_str("#0.0");
    }
    else {
        add_char(wn);
        add_dec_uint32(Rm);
    }
}

static void fp_data_processing_1_source(void) {
    uint32_t M = (instr >> 31) & 1;
    uint32_t S = (instr >> 29) & 1;
    uint32_t type = (instr >> 22) & 3;
    uint32_t opcode = (instr >> 15) & 0x3f;
    uint32_t Rn = (instr >> 5) & 0x1f;
    uint32_t Rd = instr & 0x1f;
    char wd = 0;
    char wn = 0;

    switch (type) {
    case 0: wn = wd = 's'; break;
    case 1: wn = wd = 'd'; break;
    case 3: wn = wd = 'h'; break;
    default: return;
    }

    if (M) return;
    if (S) return;

    switch (opcode) {
    case 0x00:
        add_str("fmov");
        break;
    case 0x01:
        add_str("fabs");
        break;
    case 0x02:
        add_str("fneg");
        break;
    case 0x03:
        add_str("fsqrt");
        break;
    case 0x04:
        add_str("fcvt");
        wd = 's';
        break;
    case 0x05:
        add_str("fcvt");
        wd = 'd';
        break;
    case 0x07:
        add_str("fcvt");
        wd = 'h';
        break;
    case 0x08:
        add_str("frintn");
        break;
    case 0x09:
        add_str("frintp");
        break;
    case 0x0a:
        add_str("frintm");
        break;
    case 0x0b:
        add_str("frintz");
        break;
    case 0x0c:
        add_str("frinta");
        break;
    case 0x0e:
        add_str("frintx");
        break;
    case 0x0f:
        add_str("frinti");
        break;
    }

    add_char(' ');
    add_char(wd);
    add_dec_uint32(Rd);
    add_str(", ");
    add_char(wn);
    add_dec_uint32(Rn);
}

static void fp_to_integer_conversions(void) {
    uint32_t sf = (instr >> 31) & 1;
    uint32_t type = (instr >> 22) & 3;
    uint32_t rmode = (instr >> 19) & 3;
    uint32_t opcode = (instr >> 16) & 7;
    uint32_t Rn = (instr >> 5) & 0x1f;
    uint32_t Rd = instr & 0x1f;
    switch (opcode | (rmode << 3)) {
    case 0x00: add_str("fcvtns"); break;
    case 0x01: add_str("fcvtnu"); break;
    case 0x02: add_str("scvtf"); break;
    case 0x03: add_str("ucvtf"); break;
    case 0x04: add_str("fcvtas"); break;
    case 0x05: add_str("fcvtau"); break;
    case 0x06: add_str("fmov"); break;
    case 0x07: add_str("fmov"); break;
    case 0x08: add_str("fcvtps"); break;
    case 0x09: add_str("fcvtpu"); break;
    case 0x0e: add_str("fmov"); break;
    case 0x0f: add_str("fmov"); break;
    case 0x10: add_str("fcvtms"); break;
    case 0x11: add_str("fcvtmu"); break;
    case 0x18: add_str("fcvtzs"); break;
    case 0x19: add_str("fcvtzu"); break;
    }
    if (buf_pos == 0) return;
    add_char(' ');
    if (opcode == 2 || opcode == 3 || opcode == 7) {
        if (opcode == 7 && (rmode & 1)) {
            add_fp_reg_name(Rd, 3);
            add_str("[1]");
        }
        else {
            add_char(type & 1 ? 'd' : 's');
            add_dec_uint32(Rd);
        }
        add_str(", ");
        add_reg_name(Rn, sf, 0);
    }
    else {
        add_reg_name(Rd, sf, 0);
        add_str(", ");
        if (opcode == 6 && (rmode & 1)) {
            add_fp_reg_name(Rn, 3);
            add_str("[1]");
        }
        else {
            add_char(type & 1 ? 'd' : 's');
            add_dec_uint32(Rn);
        }
    }
}

static void AdvSIMD_three_same(void) {
    uint32_t Q = (instr >> 30) & 1;
    uint32_t U = (instr >> 29) & 1;
    uint32_t size = (instr >> 22) & 3;
    uint32_t opcode = (instr >> 11) & 0x1f;
    uint32_t Rm = (instr >> 16) & 0x1f;
    uint32_t Rn = (instr >> 5) & 0x1f;
    int no_size = 0;
    int no_rm = 0;
    switch (opcode | (U << 5)) {
    case 0x00: add_str("shadd"); break;
    case 0x01: add_str("sqadd"); break;
    case 0x02: add_str("srhadd"); break;
    case 0x03:
        if (size == 0) add_str("and");
        else if (size == 1) add_str("bic");
        else if (size == 2 && Rn == Rm) {
            add_str("mov");
            no_rm = 1;
        }
        else if (size == 2) add_str("orr");
        else add_str("orn");
        no_size = 1;
        break;
    case 0x04: add_str("shsub"); break;
    case 0x05: add_str("sqsub"); break;
    case 0x06: add_str("cmgt"); break;
    case 0x07: add_str("cmge"); break;
    case 0x08: add_str("sshl"); break;
    case 0x09: add_str("sqshl"); break;
    case 0x0a: add_str("srshl"); break;
    case 0x0b: add_str("sqrshl"); break;
    case 0x0c: add_str("smax"); break;
    case 0x0d: add_str("smin"); break;
    case 0x0e: add_str("sabd"); break;
    case 0x0f: add_str("saba"); break;
    case 0x10: add_str("add"); break;
    case 0x11: add_str("cmtst"); break;
    case 0x12: add_str("mla"); break;
    case 0x13: add_str("mul"); break;
    case 0x14: add_str("smaxp"); break;
    case 0x15: add_str("sminp"); break;
    case 0x16: add_str("sqdmulh"); break;
    case 0x17: add_str("addp"); break;
    case 0x18:
        if (size < 2) add_str("fmaxnm");
        else add_str("fminnm");
        break;
    case 0x19:
        if (size < 2) add_str("fmla");
        else add_str("fmls");
        break;
    case 0x1a:
        if (size < 2) add_str("fadd");
        else add_str("fsub");
        break;
    case 0x1b:
        if (size < 2) add_str("fmulx");
        break;
    case 0x1c:
        if (size < 2) add_str("fcmeq");
        break;
    case 0x1e:
        if (size < 2) add_str("fmax");
        else add_str("fmin");
        break;
    case 0x1f:
        if (size < 2) add_str("frecps");
        else add_str("frsqrts");
        break;
    case 0x20: add_str("uhadd"); break;
    case 0x21: add_str("uqadd"); break;
    case 0x22: add_str("urhadd"); break;
    case 0x23:
        if (size == 0) add_str("eor");
        else if (size == 1) add_str("bsl");
        else if (size == 2) add_str("bit");
        else add_str("bif");
        no_size = 1;
        break;
    case 0x24: add_str("uhsub"); break;
    case 0x25: add_str("uqsub"); break;
    case 0x26: add_str("cmhi"); break;
    case 0x27: add_str("cmhs"); break;
    case 0x28: add_str("ushl"); break;
    case 0x29: add_str("uqshl"); break;
    case 0x2a: add_str("urshl"); break;
    case 0x2b: add_str("uqrshl"); break;
    case 0x2c: add_str("umax"); break;
    case 0x2d: add_str("umin"); break;
    case 0x2e: add_str("uabd"); break;
    case 0x2f: add_str("uaba"); break;
    case 0x30: add_str("sub"); break;
    case 0x31: add_str("cmeq"); break;
    case 0x32: add_str("mls"); break;
    case 0x33: add_str("pmul"); break;
    case 0x34: add_str("umaxp"); break;
    case 0x35: add_str("uminp"); break;
    case 0x36: add_str("sqrdmulh"); break;
    case 0x38:
        if (size < 2) add_str("fmaxnmp");
        else add_str("fminnmp");
        break;
    case 0x3a:
        if (size < 2) add_str("faddp");
        else add_str("fabd");
        break;
    case 0x3b:
        if (size < 2) add_str("fmul");
        break;
    case 0x3c:
        if (size < 2) add_str("fcmge");
        else add_str("fcmgt");
        break;
    case 0x3d:
        if (size < 2) add_str("facge");
        else add_str("facgt");
        break;
    case 0x3e:
        if (size < 2) add_str("fmaxp");
        break;
    case 0x3f:
        if (size < 2) add_str("fdiv");
        break;
    }
    if (buf_pos == 0) return;
    add_char(' ');
    if (no_size) add_fp_reg_name_q(instr & 0x1f, 0, Q);
    else add_fp_reg_name_q(instr & 0x1f, size, Q);
    add_str(", ");
    if (no_size) add_fp_reg_name_q(Rn, 0, Q);
    else add_fp_reg_name_q(Rn, size, Q);
    if (!no_rm) {
        add_str(", ");
        if (no_size) add_fp_reg_name_q(Rm, 0, Q);
        else add_fp_reg_name_q(Rm, size, Q);
    }
}

static void AdvSIMD_two_reg_misc(void) {
    uint32_t Q = (instr >> 30) & 1;
    uint32_t U = (instr >> 29) & 1;
    uint32_t size = (instr >> 22) & 0x3;
    uint32_t opcode = (instr >> 12) & 0x1f;
    uint32_t Rn = (instr >> 5) & 0x1f;
    uint32_t Rd = instr & 0x1f;

    switch (opcode) {
    case 0x00:
        add_str(U ? "rev32" : "rev64");
        break;
    case 0x01:
        if (U) break;
        add_str("rev16");
        break;
    case 0x02:
        add_str(U ? "uaddlp" : "saddlp");
        break;
    case 0x03:
        add_str(U ? "usqadd" : "suqadd");
        break;
    case 0x04:
        add_str(U ? "cmge" : "cls");
        break;
    case 0x05:
        if (U) {
            if (size == 0) add_str("not");
            if (size == 1) add_str("rbit");
            break;
        }
        add_str("cnt");
        break;
    case 0x06:
        add_str(U ? "uadalp" : "sadalp");
        break;
    case 0x07:
        add_str(U ? "sqneg" : "sqabs");
        break;
    case 0x08:
        add_str(U ? "cmge" : "cmgt");
        break;
    case 0x09:
        add_str(U ? "cmle" : "cmeq");
        break;
    case 0x0a:
        if (U) break;
        add_str("cmlt");
        break;
    case 0x0b:
        add_str(U ? "neg" : "abs");
        break;
    case 0x0c:
        if (size < 2) break;
        add_str(U ? "fcmge" : "fcmgt");
        break;
    case 0x0d:
        if (size < 2) break;
        add_str(U ? "fcmle" : "fcmeq");
        break;
    case 0x0e:
        if (U) break;
        if (size < 2) break;
        add_str("fcmlt");
        break;
    case 0x0f:
        if (size < 2) break;
        add_str(U ? "fneg" : "fabs");
        break;
    case 0x12:
        add_str(U ? "sqxtun" : "xtn");
        if (Q) add_char('2');
        break;
    case 0x13:
        if (!U) break;
        add_str("shll");
        if (Q) add_char('2');
        break;
    case 0x14:
        add_str(U ? "uqxtn" : "sqxtn");
        if (Q) add_char('2');
        break;
    case 0x16:
        if (size < 2) {
            add_str(U ? "fcvtxn" : "fcvtn");
            if (Q) add_char('2');
        }
        break;
    case 0x17:
        if (U) break;
        if (size < 2) {
            add_str("fcvtl");
            if (Q) add_char('2');
        }
        break;
    case 0x18:
        if (size < 2) {
            add_str(U ? "frinta" : "frintn");
        }
        else {
            if (U) break;
            add_str("frintp");
        }
        break;
    case 0x19:
        if (size < 2) {
            add_str(U ? "frintx" : "frintm");
        }
        else {
            add_str(U ? "frinti" : "frintz");
        }
        break;
    case 0x1a:
        if (size < 2) {
            add_str(U ? "fcvtnu" : "fcvtns");
        }
        else {
            add_str(U ? "fcvtpu" : "fcvtps");
        }
        break;
    case 0x1b:
        if (size < 2) {
            add_str(U ? "fcvtmu" : "fcvtms");
        }
        else {
            add_str(U ? "fcvtzu" : "fcvtzs");
        }
        break;
    case 0x1c:
        if (size < 2) {
            add_str(U ? "fcvtau" : "fcvtas");
        }
        else {
            add_str(U ? "ursqrte" : "urecpe");
        }
        break;
    case 0x1d:
        if (size < 2) {
            add_str(U ? "ucvtf" : "scvtf");
        }
        else {
            add_str(U ? "frsqrte" : "frecpe");
        }
        break;
    case 0x1f:
        if (size < 2) break;
        if (!U) break;
        add_str("fsqrt");
        break;
    }
    if (buf_pos == 0) return;

    add_char(' ');
    add_fp_reg_name_q(Rd, size, Q);
    add_str(", ");
    add_fp_reg_name_q(Rn, size, Q);

    if (opcode >= 0x08 && opcode <= 0x0b) add_str(", #0");
}

static void AdvSIMD_copy(void) {
    uint32_t Q = (instr >> 30) & 1;
    uint32_t op = (instr >> 29) & 1;
    uint32_t imm5 = (instr >> 16) & 0x1f;
    uint32_t imm4 = (instr >> 11) & 0xf;
    uint32_t Rn = (instr >> 5) & 0x1f;
    uint32_t Rd = instr & 0x1f;
    uint32_t index = 0;
    uint32_t sz = 0;

    if (imm5 & 1) sz = 0;
    else if (imm5 & 2) sz = 1;
    else if (imm5 & 4) sz = 2;
    else if (imm5 & 8) sz = 3;
    else return;

    index = imm5 >> (sz + 1);

    if (!op && imm4 == 0) {
        add_str("dup ");
        add_fp_reg_name_q(Rd, sz, Q);
        add_str(", ");
        add_fp_reg_name(Rn, sz);
        add_char('[');
        add_dec_uint32(index);
        add_char(']');
    }
    else if (!op && imm4 == 1) {
        if (sz == 3 && !Q) return;
        add_str("dup ");
        add_fp_reg_name_q(Rd, sz, Q);
        add_str(", ");
        add_reg_name(Rn, sz == 3, 0);
    }
    else if (!op && imm4 == 5) {
        if (sz == 3 && !Q) return;
        add_str("smov ");
        add_reg_name(Rd, Q, 0);
        add_str(", ");
        add_fp_reg_name(Rn, sz);
        add_char('[');
        add_dec_uint32(index);
        add_char(']');
    }
    else if (!op && imm4 == 7) {
        if (sz == 3 && !Q) return;
        if (sz >= 2 || Q) add_str("mov ");
        else add_str("umov ");
        add_reg_name(Rd, Q, 0);
        add_str(", ");
        add_fp_reg_name(Rn, sz);
        add_char('[');
        add_dec_uint32(index);
        add_char(']');
    }
    else if (Q && !op && imm4 == 3) {
        add_str("ins ");
        add_fp_reg_name(Rd, sz);
        add_char('[');
        add_dec_uint32(index);
        add_char(']');
        add_str(", ");
        add_reg_name(Rd, sz == 3, 0);
    }
    else if (Q && op) {
        add_str("ins ");
        add_fp_reg_name(Rd, sz);
        add_char('[');
        add_dec_uint32(index);
        add_char(']');
        add_str(", ");
        add_fp_reg_name(Rn, sz);
        add_char('[');
        add_dec_uint32(imm4 >> sz);
        add_char(']');
    }
    else {
        return;
    }
}

static void scalar_three_same(void) {
    uint32_t U = (instr >> 29) & 1;
    uint32_t size = (instr >> 22) & 3;
    uint32_t opcode = (instr >> 11) & 0x1f;
    uint32_t Rm = (instr >> 16) & 0x1f;
    uint32_t Rn = (instr >> 5) & 0x1f;
    uint32_t Rd = instr & 0x1f;
    char w = 0;

    switch (opcode) {
    case  1: add_str(U ? "uqadd" : "sqadd"); break;
    case  5: add_str(U ? "uqsub" : "sqsub"); break;
    case  6: add_str(U ? "cmhi" : "cmgt"); break;
    case  7: add_str(U ? "cmhs" : "cmge"); break;
    case  8: add_str(U ? "ushl" : "sshl"); break;
    case  9: add_str(U ? "uqshl" : "sqshl"); break;
    case 10: add_str(U ? "urshl" : "srshl"); break;
    case 11: add_str(U ? "uqrshl" : "sqrshl"); break;
    case 16: add_str(U ? "sub" : "add"); break;
    case 17: add_str(U ? "cmeq" : "cmtst"); break;
    case 22: add_str(U ? "sqrdmulh" : "sqdmulh"); break;
    case 26:
        if (size < 2) add_str("");
        else add_str(U ? "fabd" : "");
        break;
    case 27: add_str(U ? "" : "fmulx"); break;
    case 28:
        if (size < 2) add_str(U ? "fcmge" : "fcmeq");
        else add_str(U ? "fcmgt" : "");
        break;
    case 29:
        if (size < 2) add_str(U ? "facge" : "");
        else add_str(U ? "facgt" : "");
        break;
    case 31:
        if (size < 2) add_str(U ? "" : "frecps");
        else add_str(U ? "" : "frsqrts");
        break;
    default:
        return;
    }

    if (buf_pos == 0) return;

    if (opcode >= 26) {
        switch (size) {
        case 0: w = 's'; break;
        case 1: w = 's'; break;
        case 2: w = 'd'; break;
        case 3: w = 'd'; break;
        }
    }
    else {
        switch (size) {
        case 0: w = 'b'; break;
        case 1: w = 'h'; break;
        case 2: w = 's'; break;
        case 3: w = 'd'; break;
        }
    }

    add_char(' ');
    add_char(w);
    add_dec_uint32(Rd);
    add_str(", ");
    add_char(w);
    add_dec_uint32(Rn);
    add_str(", ");
    add_char(w);
    add_dec_uint32(Rm);
}

static void scalar_shift_by_immediate(void) {
    uint32_t U = (instr >> 29) & 1;
    uint32_t immh = (instr >> 19) & 0xf;
    uint32_t immb = (instr >> 16) & 0x7;
    uint32_t opcode = (instr >> 11) & 0x1f;
    uint32_t Rd = instr & 0x1f;
    uint32_t Rn = (instr >> 5) & 0x1f;
    uint32_t shift = 0;
    int sh = 0;
    char w = 0;

    if (immh == 0) return;

    switch (opcode) {
    case 0:
        add_str(U ? "ushr" : "sshr");
        break;
    case 2:
        add_str(U ? "usra" : "ssra");
        break;
    case 4:
        add_str(U ? "urshr" : "srshr");
        break;
    case 6:
        add_str(U ? "ursra" : "srsra");
        break;
    case 8:
        add_str(U ? "sri" : "");
        break;
    case 10:
        add_str(U ? "sli" : "shl");
        sh = 1;
        break;
    case 12:
        add_str(U ? "sqshlu" : "");
        sh = 1;
        break;
    case 14:
        add_str(U ? "uqshr" : "sqshr");
        sh = 1;
        break;
    case 16:
        add_str(U ? "sqshrun" : "");
        break;
    case 17:
        add_str(U ? "sqrshrun" : "");
        break;
    case 18:
        add_str(U ? "uqshrn" : "sqshrn");
        break;
    case 19:
        add_str(U ? "uqrshrn" : "sqrshrn");
        break;
    case 28:
        add_str(U ? "ucvtf" : "scvtf");
        break;
    case 31:
        add_str(U ? "fcvtzu" : "fcvtzs");
        break;
    }

    if (buf_pos == 0) return;

    if (sh) {
        if (immh < 2) shift = (immh << 3) + immb - 8u;
        else if (immh < 4) shift = (immh << 3) + immb - 16u;
        else if (immh < 8) shift = (immh << 3) + immb - 32u;
        else shift = (immh << 3) + immb - 64u;
    }
    else {
        if (immh < 2) shift = 16u - ((immh << 3) + immb);
        else if (immh < 4) shift = 32u - ((immh << 3) + immb);
        else if (immh < 8) shift = 64u - ((immh << 3) + immb);
        else shift = 128u - ((immh << 3) + immb);
    }

    if (immh < 2) w = 'b';
    else if (immh < 4) w = 'h';
    else if (immh < 8) w = 's';
    else w = 'd';
    add_char(' ');
    add_char(w);
    add_dec_uint32(Rd);
    add_str(", ");
    add_char(w);
    add_dec_uint32(Rn);
    add_str(", #");
    add_dec_uint32(shift);
}

static void data_processing_simd_and_fp(void) {
    if ((instr & 0x5f200000) == 0x1e000000) {
        /* Floating-point<->fixed-point conversions */
        fp_to_fixed_point_conversions();
        return;
    }
    if ((instr & 0x5f200c00) == 0x1e200400) {
        /* Floating-point conditional compare */
        fp_conditional_compare();
        return;
    }
    if ((instr & 0x5f200c00) == 0x1e200800) {
        /* Floating-point data-processing (2 source) */
        fp_data_processing_2_source();
        return;
    }
    if ((instr & 0x5f200c00) == 0x1e200c00) {
        /* Floating-point conditional select */
        fp_conditional_select();
        return;
    }
    if ((instr & 0x5f201c00) == 0x1e201000) {
        /* Floating-point immediate */
        fp_immediate();
        return;
    }
    if ((instr & 0x5f203c00) == 0x1e202000) {
        /* Floating-point compare */
        fp_compare();
        return;
    }
    if ((instr & 0x5f207c00) == 0x1e204000) {
        /* Floating-point data-processing (1 source) */
        fp_data_processing_1_source();
        return;
    }
    if ((instr & 0x5f20fc00) == 0x1e200000) {
        /* Floating-point<->integer conversions */
        fp_to_integer_conversions();
        return;
    }
    if ((instr & 0x5f000000) == 0x1f000000) {
        /* Floating-point data-processing (3 source) */
        return;
    }
    if ((instr & 0x9f200400) == 0x0e200400) {
        /* AdvSIMD three same */
        AdvSIMD_three_same();
        return;
    }
    if ((instr & 0x9f200c00) == 0x0e200000) {
        /* AdvSIMD three different */
        return;
    }
    if ((instr & 0x9f3e0c00) == 0x0e200800) {
        /* AdvSIMD two-reg misc */
        AdvSIMD_two_reg_misc();
        return;
    }
    if ((instr & 0x9f3e0c00) == 0x0e300800) {
        /* AdvSIMD across lanes */
        return;
    }
    if ((instr & 0x9fe08400) == 0x0e000400) {
        /* AdvSIMD copy */
        AdvSIMD_copy();
        return;
    }
    if ((instr & 0x9f000001) == 0x0f000000) {
        /* AdvSIMD vector x indexed element */
        return;
    }
    if ((instr & 0x9f800400) == 0x0f000400) {
        if ((instr & 0x00780000) == 0x00000000) {
            /* AdvSIMD modified immediate */
        }
        else {
            /* AdvSIMD shift by immediate */
        }
        return;
    }
    if ((instr & 0xbf208c00) == 0x0e000000) {
        /* AdvSIMD TBL/TBX */
        return;
    }
    if ((instr & 0xbf208c00) == 0x0e000800) {
        /* AdvSIMD ZIP/UZP/TRN */
        return;
    }
    if ((instr & 0xbf208400) == 0x2e000000) {
        /* AdvSIMD EXT */
        return;
    }
    if ((instr & 0xdf200400) == 0x5e200400) {
        /* AdvSIMD scalar three same */
        scalar_three_same();
        return;
    }
    if ((instr & 0xdf200c00) == 0x5e200000) {
        /* AdvSIMD scalar three different */
        return;
    }
    if ((instr & 0xdf3e0c00) == 0x5e200800) {
        /* AdvSIMD scalar two-reg misc */
        return;
    }
    if ((instr & 0xdf3e0c00) == 0x5e300800) {
        /* AdvSIMD scalar pairwise */
        return;
    }
    if ((instr & 0xdfe08400) == 0x5e000400) {
        /* AdvSIMD scalar copy */
        return;
    }
    if ((instr & 0xdf000400) == 0x5f000000) {
        /* AdvSIMD scalar x indexed element */
        return;
    }
    if ((instr & 0xdf800400) == 0x5f000400) {
        /* AdvSIMD scalar shift by immediate */
        scalar_shift_by_immediate();
        return;
    }
    if ((instr & 0xff3e0c00) == 0x4e280800) {
        /* Crypto AES */
        return;
    }
    if ((instr & 0xff208c00) == 0x5e000000) {
        /* Crypto three-reg SHA */
        return;
    }
    if ((instr & 0xff3e0c00) == 0x5e280800) {
        /* Crypto two-reg SHA */
        return;
    }
}

DisassemblyResult * disassemble_a64(uint8_t * code,
        ContextAddress addr, ContextAddress size,
        DisassemblerParams * disass_params) {
    unsigned i;
    static DisassemblyResult dr;

    if (size < 4) return NULL;
    memset(&dr, 0, sizeof(dr));
    dr.size = 4;
    buf_pos = 0;
    instr = 0;
    instr_addr = addr;
    for (i = 0; i < 4; i++) instr |= (uint32_t)*code++ << (i * 8);
    params = disass_params;

    if ((instr & 0x1c000000) == 0x10000000) data_processing_immediate();
    else if ((instr & 0x1c000000) == 0x14000000) branch_exception_system();
    else if ((instr & 0x0a000000) == 0x08000000) loads_and_stores();
    else if ((instr & 0x0e000000) == 0x0a000000) data_processing_register();
    else if ((instr & 0x0e000000) == 0x0e000000) data_processing_simd_and_fp();

    dr.text = buf;
    if (buf_pos == 0) {
        snprintf(buf, sizeof(buf), ".word 0x%08x", (unsigned)instr);
    }
    else {
        buf[buf_pos] = 0;
    }
    return &dr;
}

#endif /* SERVICE_Disassembly */
