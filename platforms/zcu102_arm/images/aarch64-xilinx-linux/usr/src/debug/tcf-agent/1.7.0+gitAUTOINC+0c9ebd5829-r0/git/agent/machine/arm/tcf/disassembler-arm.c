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

#include <tcf/config.h>

#if SERVICE_Disassembly

#include <assert.h>
#include <stdio.h>
#include <tcf/framework/context.h>
#include <tcf/services/symbols.h>
#include <machine/arm/tcf/disassembler-arm.h>

static char buf[128];
static size_t buf_pos = 0;
static Context * ctx = NULL;

static const char * shift_names[] = { "lsl", "lsr", "asr", "ror" };

static const char * op_names[] = {
    "and", "eor", "sub", "rsb", "add", "adc", "sbc", "rsc",
    "tst", "teq", "cmp", "cmn", "orr", "mov", "bic", "mvn"
};

static const char * cond_names[] = {
    "eq", "ne", "cs", "cc", "mi", "pl", "vs", "vc",
    "hi", "ls", "ge", "lt", "gt", "le", "", "nv"
};

static const char * reg_names[] = {
    "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
    "r8", "r9", "r10", "r11", "r12", "sp", "lr", "pc"
};

static const char * proc_modes [] = {
    "UNDEFINED", "UNDEFINED", "UNDEFINED", "UNDEFINED", "UNDEFINED",
    "UNDEFINED", "UNDEFINED", "UNDEFINED", "UNDEFINED", "UNDEFINED",
    "UNDEFINED", "UNDEFINED", "UNDEFINED", "UNDEFINED", "UNDEFINED",
    "UNDEFINED", "usr", "fiq", "irq", "svc", "UNDEFINED",
    "UNDEFINED", "mon", "abc", "UNDEFINED", "UNDEFINED",
    "hyp", "und", "UNDEFINED", "UNDEFINED", "UNDEFINED", "sys"
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

static void add_hex_uint32(uint32_t n) {
    char s[32];
    size_t i = 0;
    while (i < 8) {
        uint32_t d = n & 0xf;
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
    snprintf(str, sizeof(str), "%g", u.f);
    add_str(str);
}

static void add_flt_uint64(uint64_t n) {
    char str[32];
    union {
        uint64_t n;
        double d;
    } u;
    u.n = n;
    snprintf(str, sizeof(str), "%g", u.d);
    add_str(str);
}

static void add_addr(uint32_t addr) {
    while (buf_pos < 16) add_char(' ');
    add_str("; addr=0x");
    add_hex_uint32(addr);
#if ENABLE_Symbols
    if (ctx != NULL) {
        Symbol * sym = NULL;
        char * name = NULL;
        ContextAddress sym_addr = 0;
        if (find_symbol_by_addr(ctx, STACK_NO_FRAME, addr, &sym) < 0) return;
        if (get_symbol_name(sym, &name) < 0 || name == NULL) return;
        if (get_symbol_address(sym, &sym_addr) < 0) return;
        if (sym_addr <= addr) {
            add_str(": ");
            add_str(name);
            if (sym_addr < addr) {
                add_str(" + 0x");
                add_hex_uint32(addr - (uint32_t)sym_addr);
            }
        }
    }
#endif
}

#define add_reg_name(reg) add_str(reg_names[(reg) & 0xf])

static void add_modifed_immediate_constant(uint32_t n) {
    uint32_t rot = ((n >> 8) & 0xf) * 2;
    uint32_t val = n & 0xff;
    val = (val >> rot) | (val << (32 - rot));
    add_char('#');
    if (val & 0x80000000) {
        add_char('-');
        val = ~val + 1;
    }
    add_dec_uint32(val);
}

/**
 * Add the label of an instruction or literal data item whose address is to be
 * loaded into <Rd>. The assembler calculates the required value of the offset
 * from the Align(PC, 4) value of the ADR instruction to this label.
 *
 * If the offset is zero or positive, encoding A1 is used, with imm32 equal
 * to the offset.
 *
 * If the offset is negative, encoding A2 is used, with imm32 equal to the size
 * of the offset.
 *
 * That is, the use of encoding A2 indicates that the required offset is minus
 * the value of imm32.
 *
 * Permitted values of the size of the offset are any of the constants
 * described in Modified immediate constants in A32 instructions.
 */
static void add_modifed_immediate_address(uint32_t n, uint32_t addr, uint32_t add, uint32_t alignment) {
    uint32_t rot = ((n >> 8) & 0xf) * 2;
    uint32_t val = n & 0xff;
    val = (val >> rot) | (val << (32 - rot));
    if (add == 0) {
        add_char('-');
        add_dec_uint32(val);
        add_addr((alignment * (addr / alignment)) - val);
    }
    else {
        add_dec_uint32(val);
        add_addr((alignment * (addr / alignment)) + val);
    }
}

static void add_shift(uint32_t instr, int no_shift_name) {
    uint8_t rm = (instr & 0x000f);
    uint32_t shift_imm = (instr >> 7) & 0x1f;
    uint32_t shift_type = (instr >> 5) & 3;
    int reg_shift = (instr & 0x00000010) != 0;

    add_reg_name(rm);
    if (reg_shift || shift_type != 0 || shift_imm != 0) {
        if (reg_shift) {
            add_str(", ");
            if (!no_shift_name) {
                add_str(shift_names[shift_type]);
                add_char(' ');
            }
            add_reg_name((instr & 0x0f00) >> 8);
        }
        else if (shift_type == 3 && shift_imm == 0) {
            if (!no_shift_name) {
                add_str(", ");
                add_str("rrx");
            }
        }
        else {
            add_str(", ");
            if (!no_shift_name) {
                add_str(shift_names[shift_type]);
                add_char(' ');
            }
            add_char('#');
            if (shift_type >= 1 && shift_imm == 0) shift_imm = 32;
            add_dec_uint32(shift_imm);
        }
    }
}

static void add_addressing_mode(uint32_t instr) {
    int I = (instr & (1 << 25)) != 0;
    int P = (instr & (1 << 24)) != 0;
    int U = (instr & (1 << 23)) != 0;
    int W = (instr & (1 << 21)) != 0;
    uint32_t rn = (instr & 0x000f0000) >> 16;

    add_char('[');
    add_reg_name(rn);

    if (!I && P) {
        uint32_t offs = instr & 0xfff;
        if (offs != 0 || W) {
            add_str(", #");
            add_char(U ? '+' : '-');
            add_dec_uint32(offs);
        }
        else if (W == 0 && U ==0 && offs == 0) {
            /* Special case [reg,#-0] : P==1, W==0, U==0, IMM==0 */
            add_str(", #-0");
        }
        add_char(']');
        if (W) add_char('!');
    }
    else if (I && P) {
        add_str(", ");
        add_char(U ? '+' : '-');
        add_shift(instr, 0);
        add_char(']');
        if (W) add_char('!');
    }
    else if (!I && !P && !W) {
        add_str("], #");
        add_char(U ? '+' : '-');
        add_dec_uint32(instr & 0xfff);
    }
    else if (I && !P && !W) {
        add_str("], ");
        add_char(U ? '+' : '-');
        add_shift(instr, 0);
    }
    else {
        add_str(" ?]");
    }
}

static void add_auto_inc_mode(uint32_t instr, int no_ia) {
    switch ((instr >> 23) & 3) {
    case 0: add_str("da"); break;
    case 1: if (!no_ia) add_str("ia"); break;
    case 2: add_str("db"); break;
    case 3: add_str("ib"); break;
    }
}

/**
 * al_reg_align - determine all lanes register alignment.
 */
static uint32_t al_reg_align(uint32_t type, uint32_t size, uint32_t index_align) {
    uint32_t a = index_align & 0x1;
    uint32_t align = 0;
    if ((type == 0) && (size == 1) && (a == 1)) align = 1;
    else if ((type == 0) && (size == 2) && (a == 1)) align = 2;
    else if ((type == 1) && (size == 0) && (a == 1)) align = 1;
    else if ((type == 1) && (size == 1) && (a == 1)) align = 2;
    else if ((type == 1) && (size == 2) && (a == 1)) align = 3;
    else if ((type == 3) && (size == 0) && (a == 1)) align = 2;
    else if ((type == 3) && (size == 1) && (a == 1)) align = 3;
    else if ((type == 3) && (size == 2) && (a == 1)) align = 3;
    else if ((type == 3) && (size == 3) && (a == 1)) align = 4;
    return align;
}

/**
 * count_bits - count the number of bits in 32-bit value <v>.
 */
static int count_bits(uint32_t v) {
    int ix = 0;
    int nbits = 0;
    while (ix < 32) {nbits += ((v >> ix++) & 0x00000001);}
    return nbits;
}

/**
 * ix_reg_align - determine indexed register alignment.
 */
static uint32_t ix_reg_align(uint32_t type, uint32_t size, uint32_t index_align) {
    uint32_t align = 0;
    if ((type == 0) && (size == 1) && ((index_align & 0x01) == 1)) align = 1;
    else if ((type == 0) && (size == 2) && ((index_align & 0x03) == 3)) align = 2;
    else if ((type == 1) && (size == 0) && ((index_align & 0x01) == 1)) align = 1;
    else if ((type == 1) && (size == 1) && ((index_align & 0x01) == 1)) align = 2;
    else if ((type == 1) && (size == 2) && ((index_align & 0x01) == 1)) align = 3;
    else if ((type == 3) && (size == 0) && ((index_align & 0x01) == 1)) align = 2;
    else if ((type == 3) && (size == 1) && ((index_align & 0x01) == 1)) align = 3;
    else if ((type == 3) && (size == 2) && ((index_align & 0x03) == 1)) align = 3;
    else if ((type == 3) && (size == 2) && ((index_align & 0x03) == 2)) align = 4;
    return align;
}


static uint32_t vfp_expand_imm32(uint32_t n) {
    uint32_t v = 0;
    if (n & (1 << 7)) v |= 1 << 31;
    if (n & (1 << 6)) v |= 0x1f << 25;
    else v |= 1 << 30;
    v |= (n & 0x3f) << 19;
    return v;
}

static uint64_t vfp_expand_imm64(uint32_t n) {
    uint64_t v = 0;
    if (n & (1 << 7)) v |= (uint64_t)1 << 63;
    if (n & (1 << 6)) v |= (uint64_t)0xff << 54;
    else v |= (uint64_t)1 << 62;
    v |= (uint64_t)(n & 0x3f) << 48;
    return v;
}

static uint64_t adv_simd_expand_imm(uint32_t instr) {
    unsigned i;
    uint64_t imm64 = 0;
    int op = (instr & (1 << 5)) != 0;
    uint32_t cmode = (instr >> 8) & 0xf;
    uint32_t imm8 = instr & 0xf;
    imm8 |= (instr >> 12) & 0x70;
    if (instr & (1 << 24)) imm8 |= 0x80;
    switch (cmode / 2) {
    case 0:
        return (imm8 << 0) | ((uint64_t)imm8 << 32);
    case 1:
        return (imm8 << 8) | ((uint64_t)imm8 << 40);
    case 2:
        return (imm8 << 16) | ((uint64_t)imm8 << 48);
    case 3:
        return (imm8 << 24) | ((uint64_t)imm8 << 56);
    case 4:
        return (imm8 << 0) | ((uint64_t)imm8 << 32) |
               (imm8 << 16) | ((uint64_t)imm8 << 48);
    case 5:
        return (imm8 << 8) | ((uint64_t)imm8 << 40) |
               (imm8 << 24) | ((uint64_t)imm8 << 56);
    case 6:
        if ((cmode & 1) == 0) {
            imm64 = (imm8 << 8) | 0xff;
        }
        else {
            imm64 = (imm8 << 16) | 0xffff;
        }
        return imm64 | (imm64 << 32);
    case 7:
        if ((cmode & 1) == 0 && op == 0) {
            for (i = 0; i < 8; i++) imm64 |= (uint64_t)imm8 << i * 8;
            return imm64;
        }
        if ((cmode & 1) == 0 && op == 1) {
            for (i = 0; i < 8; i++) {
                if (imm8 & (1 << i)) imm64 |= ((uint64_t)0xff << (i * 8));
            }
            return imm64;
        }
        if ((cmode & 1) == 1 && op == 0) {
            if (imm8 & (1 << 7)) imm64 |= ((uint64_t)1 << 31);
            if (imm8 & (1 << 6)) imm64 |= ((uint64_t)0x1f << 25);
            else imm64 |= ((uint64_t)1 << 30);
            imm64 |= (imm8 & (uint64_t)0x3f) << 19;
            return imm64 | ((uint64_t)(imm64 << 32));
        }
        break;
    }
    return imm64;
}

static void disassemble_advanced_simd_data_processing(uint32_t instr) {
    if ((instr & 0xfe800000) == 0xf2000000) {
        /* Three registers of the same length */
        uint32_t sz = (instr >> 20) & 3;
        int U = (instr & (1 << 24)) != 0;
        int D = (instr & (1 << 22)) != 0;
        int N = (instr & (1 << 7)) != 0;
        int Q = (instr & (1 << 6)) != 0;
        int M = (instr & (1 << 5)) != 0;
        uint32_t A = (instr >> 8) & 0xf;
        uint32_t vd = (instr >> 12) & 0xf;
        uint32_t vn = (instr >> 16) & 0xf;
        uint32_t vm = instr & 0xf;
        int no_dt = 0;
        int no_sz = 0;
        char fmt = 0;
        if (D) vd |= 0x10;
        if (N) vn |= 0x10;
        if (M) vm |= 0x10;
        if (instr & (1 << 23)) {
        }
        else {
            uint32_t B = (instr >> 4) & 1;
            switch (A) {
            case 0:
                add_str(B ? "vqadd" : "vhadd");
                break;
            case 1:
                if (!B) {
                    add_str("vrhadd");
                }
                else {
                    no_dt = 1;
                    no_sz = 1;
                    if (!U) {
                        switch (sz) {
                        case 0: add_str("vand"); break;
                        case 1: add_str("vbic"); break;
                        case 2: add_str("vorr"); break;
                        case 3: add_str("vorn"); break;
                        }
                    }
                    else {
                        switch (sz) {
                        case 0: add_str("veor"); break;
                        case 1: add_str("vbsl"); break;
                        case 2: add_str("vbit"); break;
                        case 3: add_str("vbif"); break;
                        }
                    }
                }
                break;
            case 2:
                add_str(B ? "vqsub" : "vhsub");
                break;
            case 3:
                add_str(B ? "vcge" : "vcgt");
                break;
            case 4:
                add_str(B ? "vqshl" : "vshl");
                break;
            case 5:
                add_str(B ? "vqrshl" : "vrshl");
                break;
            case 6:
                add_str(B ? "vmin" : "vmax");
                break;
            case 7:
                add_str(B ? "vaba" : "vabd");
                break;
            case 8:
                fmt = 'i';
                if (!B) {
                    add_str(U ? "vsub" : "vadd");
                }
                else {
                    if (U) add_str("vceq");
                    else {
                        add_str("vtst");
                        no_dt = 1;
                        no_sz = 0;
                    }
                }
                break;
            case 9:
                if ((U == 1) && (sz == 0)) fmt = 'p';
                else fmt = 'i';
                if (B) add_str("vmul");
                else {
                    if ((instr >> 8) & 1) {
                        /* Encoding A1 */
                        add_str((instr >> 24) & 1 ? "vmls" : "vmla");
                        fmt = 'i';
                    }
                    else {
                        /* Encoding A2 */
                        add_str((instr >> 9) & 1 ? "vmls" : "vmla");
                        fmt = ((U == 1) ? 'u' : 's');
                    }
                }
                break;
            case 10:
                add_str(B ? "vpmin" : "vpmax");
                break;
            case 11:
                fmt = 's';
                if (!B) {
                    add_str(U ? "vqrdmulh" : "vqdmulh");
                }
                else if (!U) {
                    fmt = 'i';
                    add_str("vpadd");
                }
                break;
            case 13:
                if (!B) {
                    if (!U) {
                        add_str(sz & 2 ? "vsub" : "vadd");
                    }
                    else {
                        add_str(sz & 2 ? "vabd" : "vpadd");
                    }
                }
                else {
                    if (!U) {
                        add_str(instr & (1 << 21) ? "vmls" : "vmla");
                    }
                    else {
                        add_str("vmul");
                    }
                }
                fmt = 'f';
                sz = (sz & 1) + 2;
                break;
            case 14:
                if (!B) {
                    if (!U) {
                        add_str(sz & 2 ? "" : "vceq");
                    }
                    else {
                        add_str(sz & 2 ? "vcgt" : "vcge");
                    }
                }
                else {
                    if (!U) {
                        add_str("");
                    }
                    else {
                        add_str(sz & 2 ? "vacgt" : "vacge");
                    }
                }
                fmt = 'f';
                sz = (sz & 1) + 2;
                break;
            case 15:
                if (!B) {
                    if (!U) {
                        add_str(sz & 2 ? "vmin" : "vmax");
                    }
                    else {
                        add_str(sz & 2 ? "vpmin" : "vpmax");
                    }
                }
                else {
                    if (!U) {
                        add_str(sz & 2 ? "vrsqrts" : "vrecps");
                    }
                    else {
                        add_str("");
                    }
                }
                fmt = 'f';
                sz = (sz & 1) + 2;
                break;
            }
        }
        if (buf_pos > 0) {
            if (!no_dt) {
                add_char('.');
                if (fmt) add_char(fmt);
                else add_char(U ? 'u' : 's');
            }
            if (!no_sz) {
                if (no_dt) add_char('.');
                add_dec_uint32(8 << sz);
            }
            add_char(' ');
            add_char(Q ? 'q' : 'd');
            add_dec_uint32(Q ? vd / 2 : vd);
            add_str(", ");
            add_char(Q ? 'q' : 'd');
            if (A == 4 || A == 5) {
                add_dec_uint32(Q ? vm / 2 : vm);
            }
            else {
                add_dec_uint32(Q ? vn / 2 : vn);
            }
            add_str(", ");
            add_char(Q ? 'q' : 'd');
            if (A == 4 || A == 5) {
                add_dec_uint32(Q ? vn / 2 : vn);
            }
            else {
                add_dec_uint32(Q ? vm / 2 : vm);
            }
            return;
        }
    }

    if ((instr & 0xfeb80090) == 0xf2800010) {
        /* One register and a modified immediate value */
        int Q = (instr & (1 << 6)) != 0;
        int op = (instr & (1 << 5)) != 0;
        uint32_t vd = (instr >> 12) & 0xf;
        uint32_t cmode = (instr >> 8) & 0xf;
        if (instr & (1 << 22)) vd |= 0x10;
        if (Q) vd /= 2;
        if ((instr & 0x00000900) == 0x00000100 || (instr & 0x00000d00) == 0x00000900) {
            add_str(op ? "vbic" : "vorr");
        }
        else if (op && ((cmode == 0) || (cmode == 2) || (cmode == 4) ||
                        (cmode == 6) || (cmode == 8) || (cmode == 10) ||
                        (cmode == 12) || (cmode == 13))) {
            add_str("vmvn");
        }
        else {
            add_str("vmov");
        }
        if (cmode == 0xe) add_str(op == 0 ? ".i8" : ".i64");
        else if (cmode == 0xf) add_str(op == 0 ? ".f32" : "");
        else if ((cmode & 0xc) == 0x8) add_str(".i16");
        else add_str(".i32");
        add_char(' ');
        add_char(Q ? 'q' : 'd');
        add_dec_uint32(vd);
        add_str(", #0x");
        add_hex_uint64(adv_simd_expand_imm(instr));
        return;
    }

    if ((instr & 0xfe800010) == 0xf2800010) {
        /* Two registers and a shift amount */
        uint32_t A = (instr >> 8) & 0xf;
        int B = (instr & (1 << 6)) != 0;
        int L = (instr & (1 << 7)) != 0;
        int U = (instr & (1 << 24)) != 0;
        uint32_t imm = (instr >> 19) & 7;
        uint32_t imm6 = (instr >> 16) & 0x3f;
        int D = (instr & (1 << 22)) != 0;
        int M = (instr & (1 << 5)) != 0;
        uint32_t vd = (instr >> 12) & 0xf;
        uint32_t vm = instr & 0xf;
        uint32_t size = 0;
        uint32_t shift = 0;
        int shift_left = (A >= 5 && A <= 7) || A == 10;

        if (D) vd |= 0x10;
        if (M) vm |= 0x10;

        if (L) {
            size = 8;
            shift = shift_left ? imm6 : 64 - imm6;
        }
        else if (imm6 & 0x20) {
            size = 4;
            shift = shift_left ? imm6 - 32 : 64 - imm6;
        }
        else if (imm6 & 0x10) {
            size = 2;
            shift = shift_left ? imm6 - 16 : 32 - imm6;
        }
        else if (imm6 & 0x08) {
            size = 1;
            shift = shift_left ? imm6 - 8 : 16 - imm6;
        }

        switch (A) {
        case 0: add_str("vshr"); break;
        case 1: add_str("vsra"); break;
        case 2: add_str("vrshr"); break;
        case 3: add_str("vrsra"); break;
        case 4: add_str(U ? "vsri" : ""); break;
        case 5: add_str(U ? "vsli" : "vshl"); break;
        case 6: add_str("vqshlu"); break;
        case 7: add_str("vqshl"); break;
        case 8:
            if (L) break;
            if (!U) add_str(B ? "vrshrn" : "vshrn");
            else add_str(B ? "vqrshrun" : "vqshrun");
            break;
        case 9:
            if (L) break;
            add_str(B ? "vqrshrn" : "vqshrn");
            break;
        case 10:
            if (B || L) break;
            if (imm == 0) break;
            add_str(shift ? "vshll" : "vmovl");
            break;
        case 14:
        case 15:
            if (L) break;
            add_str("vcvt");
            break;
        }
        if (buf_pos > 0) {
            add_char('.');
            if (A == 14) {
                add_str(U ? "f32.u32" : "f32.s32");
            }
            else if (A == 15) {
                add_str(U ? "u32.f32" : "s32.f32");
            }
            else {
                if (A == 5) {
                    if (!U) add_char('i');
                }
                else if (A == 6) {
                    add_char('s');
                }
                else if (A == 8) {
                    if (!U) add_char('i');
                    else add_char('s');
                    size *= 2;
                }
                else if (A != 4) {
                    add_char(U ? 'u' : 's');
                    if (A == 9) size *= 2;
                }
                add_dec_uint32(size << 3);
            }
            add_char(' ');
            if ((B && A != 8 && A != 9) || A == 10) {
                add_char('q');
                add_dec_uint32(vd / 2);
            }
            else {
                add_char('d');
                add_dec_uint32(vd);
            }
            add_str(", ");
            if (B || A == 9 || A == 8) {
                add_char('q');
                add_dec_uint32(vm / 2);
            }
            else {
                add_char('d');
                add_dec_uint32(vm);
            }
            if (A == 10 && shift == 0) return;
            add_str(", #");
            add_dec_uint32(shift);
            return;
        }
    }

    if ((instr & 0xfea00050) == 0xf2800000 || (instr & 0xfeb00050) == 0xf2a00000) {
        /* Three registers of different lengths */
        uint32_t A = (instr >> 8) & 0xf;
        int U = (instr & (1 << 24)) != 0;
        switch (A) {
        case 0: add_str("vaddl"); break;
        case 1: add_str("vaddw"); break;
        case 2: add_str("vsubl"); break;
        case 3: add_str("vsubw"); break;
        case 4: add_str(U ? "vraddhn" : "vaddhn"); break;
        case 5: add_str("vabal"); break;
        case 6: add_str(U ? "vrsubhn" : "vsubhn"); break;
        case 7: add_str("vabdl"); break;
        case 8: add_str("vmlal"); break;
        case 9: add_str("vqdmlal"); break;
        case 10: add_str("vmlsl"); break;
        case 11: add_str("vqdmlsl"); break;
        case 12: add_str("vmull"); break;
        case 13: add_str("vqdmull"); break;
        case 14: add_str("vmull"); break;
        }
        if (buf_pos > 0) {
            uint32_t size = (instr >> 20) & 0x3;
            uint32_t vn = ((instr >> 3) & 0x10) + ((instr >> 16) & 0xf);
            uint32_t vd = ((instr >> 18) & 0x10) + ((instr >> 12) & 0xf);
            uint32_t vm = ((instr >> 1) & 0x10) + (instr & 0xf);
            if (instr & (1 << 22)) vd |= 0x10;
            add_char('.');
            if (A == 4 || A == 6) {
                add_char('i');
                add_dec_uint32(16 << size);
            }
            else if (A == 14) {
                add_char('p');
                add_dec_uint32(8 << size);
            }
            else {
                add_char(U ? 'u' : 's');
                add_dec_uint32(8 << size);
            }
            add_char(' ');
            if (A == 4 || A == 6) {
                add_char('d');
                add_dec_uint32(vd);
            }
            else {
                add_char('q');
                add_dec_uint32(vd / 2);
            }
            add_str(", ");
            if (A == 1 || A == 3 || A == 4 || A == 6) {
                add_char('q');
                add_dec_uint32(vn / 2);
            }
            else {
                add_char('d');
                add_dec_uint32(vn);
            }
            add_str(", ");
            if (A == 4 || A == 6) {
                add_char('q');
                add_dec_uint32(vm / 2);
            }
            else {
                add_char('d');
                add_dec_uint32(vm);
            }
        }
        return;
    }

    if ((instr & 0xfea00050) == 0xf2800040 || (instr & 0xfeb00050) == 0xf2a00040) {
        /* Two registers and a scalar */
        uint32_t A = (instr >> 8) & 0xf;
        switch (A) {
        case 0:
        case 1:
        case 2:
            /* bit 9 gives encoding A1 (vmla) or encoding A2 (vmlal) */
            add_str((instr >> 9) & 1 ? "vmlal" : "vmla");
            break;
        case 4:
        case 5:
        case 6:
            /* bit 9 gives encoding A1 (vmls) or encoding A2 (vmlsl) */
            add_str((instr >> 9) & 1 ? "vmlsl" : "vmls");
            break;
        case 3:
            add_str("vqdmlal");
            break;
        case 7:
            add_str("vqdmlsl");
            break;
        case 8:
        case 9:
            add_str("vmul");
            break;
        case 10:
            add_str("vmull");
            break;
        case 11:
            add_str("vqdmull");
            break;
        case 12:
            add_str("vqdmulh");
            break;
        case 13:
            add_str("vqrdmulh");
            break;
        }
        if (buf_pos > 0) {
            uint32_t size = (instr >> 20) & 0x3;
            uint32_t vn = (instr >> 16) & 0xf;
            uint32_t vd = (instr >> 12) & 0xf;
            uint32_t vm = instr & 0xf;
            uint32_t x = 0;
            int U = (instr & (1 << 24)) != 0;
            if (instr & (1 << 22)) vd |= 0x10;
            if (instr & (1 << 7)) vn |= 0x10;
            if (instr & (1 << 5)) x = 1;
            if (size < 2) {
                x = x << 1;
                if (vm & 0x8) x += 1;
                vm &= 0x7;
            }
            add_char('.');
            if (A == 0 || A == 4 || A == 8) {
                add_char('i');
            }
            else if (A == 1 || A == 5 || A == 9) {
                add_char('f');
            }
            else if (A == 12 || A == 13) {
                add_char('s');
            }
            else {
                add_char(U ? 'u' : 's');
            }
            add_dec_uint32(8 << size);
            add_char(' ');
            if ((A == 0 || A == 1 || A == 4 || A == 5 || A == 8 || A == 9 || A == 12 || A == 13) && !U) {
                add_char('d');
                add_dec_uint32(vd);
            }
            else {
                add_char('q');
                add_dec_uint32(vd / 2);
            }
            add_str(", ");
            if (((A == 0 || A == 1 || A == 4 || A == 5 || A == 8 || A == 9 || A == 12 || A == 13) && !U) ||
                    A == 2 || A == 3 || A == 6 || A == 7 || A == 10 || A == 11) {
                add_char('d');
                add_dec_uint32(vn);
            }
            else {
                add_char('q');
                add_dec_uint32(vn / 2);
            }
            add_str(", ");
            add_char('d');
            add_dec_uint32(vm);
            add_char('[');
            add_dec_uint32(x);
            add_char(']');
        }
        return;
    }

    if ((instr & 0xffb00010) == 0xf2b00000) {
        /* Vector Extract */
        uint32_t vn = (instr >> 16) & 0xf;
        uint32_t vd = (instr >> 12) & 0xf;
        uint32_t vm = instr & 0xf;
        uint32_t imm4 = (instr >> 8) & 0xf;
        int Q = (instr & (1 << 6)) != 0;
        if (instr & (1 << 22)) vd |= 0x10;
        if (instr & (1 << 7)) vn |= 0x10;
        if (instr & (1 << 5)) vm |= 0x10;
        add_str("vext");
        add_str(".8 ");
        if (Q) {
            add_char('q');
            add_dec_uint32(vd / 2);
        }
        else {
            add_char('d');
            add_dec_uint32(vd);
        }
        add_str(", ");
        if (Q) {
            add_char('q');
            add_dec_uint32(vn / 2);
        }
        else {
            add_char('d');
            add_dec_uint32(vn);
        }
        add_str(", ");
        if (Q) {
            add_char('q');
            add_dec_uint32(vm / 2);
        }
        else {
            add_char('d');
            add_dec_uint32(vm);
        }
        add_str(", #");
        add_dec_uint32(imm4);
        return;
    }

    if ((instr & 0xffb00810) == 0xf3b00000) {
        /* Two registers, miscellaneous */
        uint32_t size = (instr >> 18) & 3;
        uint32_t A = (instr >> 16) & 3;
        uint32_t B = (instr >> 6) & 0x1f;
        int Q = (instr & (1 << 6)) != 0;
        uint32_t vd = (instr >> 12) & 0xf;
        uint32_t vm = instr & 0xf;
        if (instr & (1 << 22)) vd |= 0x10;
        if (instr & (1 << 5)) vm |= 0x10;
        if (A == 0) {
            if ((B & 0x1e) == 0) add_str("vrev64");
            else if ((B & 0x1e) == 2) add_str("vrev32");
            else if ((B & 0x1e) == 4) add_str("vrev16");
            else if (B >= 8 && B <= 0xb) add_str("vpaddl");
            else if ((B & 0x1e) == 0x10) add_str("vcls");
            else if ((B & 0x1e) == 0x12) add_str("vclz");
            else if ((B & 0x1e) == 0x14) add_str("vcnt");
            else if ((B & 0x1e) == 0x16) add_str("vmvn");
            else if (B >= 0x18 && B <= 0x1b) add_str("vpadal");
            else if ((B & 0x1e) == 0x1c) add_str("vqabs");
            else if ((B & 0x1e) == 0x1e) add_str("vqneg");
        }
        else if (A == 1) {
            if ((B & 0xe) == 0) add_str("vcgt");
            else if ((B & 0xe) == 2) add_str("vcge");
            else if ((B & 0xe) == 4) add_str("vceq");
            else if ((B & 0xe) == 6) add_str("vcle");
            else if ((B & 0xe) == 8) add_str("vclt");
            else if ((B & 0xe) == 0xc) add_str("vabs");
            else if ((B & 0xe) == 0xe) add_str("vneg");
        }
        else if (A == 2) {
            if ((B & 0x1e) == 0) add_str("vswp");
            else if ((B & 0x1e) == 2) add_str("vtrn");
            else if ((B & 0x1e) == 4) add_str("vuzp");
            else if ((B & 0x1e) == 6) add_str("vzip");
            else if ((B & 0x1f) == 8) add_str("vmovn");
            else if ((B & 0x1f) == 9) add_str("vqmovun");
            else if ((B & 0x1e) == 0xa) add_str("vqmovn");
            else if ((B & 0x1f) == 0xc) add_str("vshll");
            else if ((B & 0x1b) == 0x18) add_str("vcvt");
        }
        else {
            if ((B & 0x1a) == 0x10) add_str("vrecpe");
            else if ((B & 0x1a) == 0x12) add_str("vrsqrte");
            else if ((B & 0x18) == 0x18) add_str("vcvt");
        }
        if (A == 0 && B >= 0x16 && B <= 0x17) {
        }
        else if (A == 2 && (B & 0x1e) == 0) {
        }
        else if (A == 2 && (B & 0x1f) == 0x18) {
            add_str(".f16.f32");
        }
        else if (A == 2 && (B & 0x1f) == 0x1c) {
            add_str(".f32.f16");
        }
        else if (A == 3 && (B & 0x18) == 0x18) {
            if (size != 2) return;
            switch ((instr >> 7) & 3) {
            case 0: add_str(".f32.s32"); break;
            case 1: add_str(".f32.u32"); break;
            case 2: add_str(".s32.f32"); break;
            case 3: add_str(".u32.f32"); break;
            }
        }
        else {
            add_char('.');
            if (A == 0) {
                if (B >= 8 && B <= 0xb) add_char(instr & (1 << 7) ? 'u' : 's');
                else if ((B & 0x1e) == 0x10) add_char('s');
                else if ((B & 0x1e) == 0x12) add_char('i');
                else if (B >= 0x18 && B <= 0x1b) add_char(instr & (1 << 7) ? 'u' : 's');
                else if ((B & 0x1e) == 0x1c) add_char('s');
                else if ((B & 0x1e) == 0x1e) add_char('s');
            }
            else if (A == 1) {
                if (B == 4 || B == 5) add_char('i');
                else add_char(instr & (1 << 10) ? 'f' : 's');
            }
            else if (A == 2) {
                if ((B & 0x1f) == 8) {
                    add_char('i');
                    size++;
                }
                else if ((B & 0x1f) == 9) {
                    add_char('s');
                    size++;
                }
                else if ((B & 0x1e) == 0xa) {
                    add_char(instr & (1 << 6) ? 'u' : 's');
                    size++;
                }
                else if ((B & 0x1f) == 0xc) add_char('i');
            }
            else {
                if ((B & 0x18) == 0x10) add_char(instr & (1 << 8) ? 'f' : 'u');
            }
            add_dec_uint32(8 << size);
        }
        add_char(' ');
        if (A == 2 && (B & 0x1f) == 8) add_char('d');
        else if (A == 2 && (B & 0x1f) == 9) add_char('d');
        else if (A == 2 && (B & 0x1e) == 0xa) add_char('d');
        else if (A == 2 && (B & 0x1f) == 0x18) add_char('d');
        else if (A == 2 && (B & 0x1f) == 0x1c) add_char('q');
        else if (A == 2 && (B & 0x1f) == 0xc) add_char('q');
        else add_char(Q ? 'q' : 'd');
        if (buf[buf_pos - 1] == 'q') vd /= 2;
        add_dec_uint32(vd);
        add_str(", ");
        if (A == 2 && (B & 0x1f) == 8) add_char('q');
        else if (A == 2 && (B & 0x1e) == 0xa) add_char('q');
        else if (A == 2 && (B & 0x1f) == 0x18) add_char('q');
        else if (A == 2 && (B & 0x1f) == 0x1c) add_char('d');
        else add_char(Q ? 'q' : 'd');
        if (buf[buf_pos - 1] == 'q') vm /= 2;
        add_dec_uint32(vm);
        if (A == 1 && (B & 0xf) <= 9) {
            add_str(", #0");
        }
        else if (A == 2 && (B & 0x1f) == 0xc) {
            add_str(", #");
            add_dec_uint32(8 << size);
        }
        return;
    }

    if ((instr & 0xffb00c10) == 0xf3b00800) {
        /* Vector Table Lookup */
        uint32_t vn = (instr >> 16) & 0xf;
        uint32_t vd = (instr >> 12) & 0xf;
        uint32_t vm = instr & 0xf;
        uint32_t len = (instr >> 8) & 0x3;
        if (instr & (1 << 22)) vd |= 0x10;
        if (instr & (1 << 7)) vn |= 0x10;
        if (instr & (1 << 5)) vm |= 0x10;
        add_str(instr & (1 << 6) ? "vtbx" : "vtbl");
        add_str(".8 ");
        add_char('d');
        add_dec_uint32(vd);
        add_str(", {");
        add_char('d');
        add_dec_uint32(vn);
        if (len > 0) {
            add_char('-');
            add_char('d');
            add_dec_uint32(vn + len);
        }
        add_str("}, ");
        add_char('d');
        add_dec_uint32(vm);
        return;
    }

    if ((instr & 0xffb00f90) == 0xf3b00c00) {
        /* Vector Duplicate */
        uint32_t vd = (instr >> 12) & 0xf;
        uint32_t vm = instr & 0xf;
        uint32_t imm4 = (instr >> 16) & 0xf;
        uint32_t size = 0;
        int Q = (instr & (1 << 6)) != 0;
        if (instr & (1 << 22)) vd |= 0x10;
        if (instr & (1 << 5)) vm |= 0x10;
        add_str("vdup");
        if (imm4 & 1) size = 0;
        else if (imm4 & 2) size = 1;
        else if (imm4 & 4) size = 2;
        else return;
        add_char('.');
        add_dec_uint32(8 << size);
        add_char(' ');
        if (Q) {
            add_char('q');
            add_dec_uint32(vd / 2);
        }
        else {
            add_char('d');
            add_dec_uint32(vd);
        }
        add_str(", d");
        add_dec_uint32(vm);
        add_char('[');
        add_dec_uint32(imm4 >> (size + 1));
        add_char(']');
        return;
    }
}

static void disassemble_advanced_simd_load_store(uint32_t instr) {
    if ((instr & 0xff100000) == 0xf4000000) {
        uint32_t align = 0;
        if (instr & (1 << 23)) {
            uint32_t size = (instr >> 10) & 3;
            uint32_t index_align = (instr >> 4) & 0xf;
            int D = (instr & (1 << 22)) != 0;
            int L = (instr & (1 << 21)) != 0;
            int all_lanes = 0;
            uint32_t index = 0;
            uint32_t vd = (instr >> 12) & 0xf;
            uint32_t n = (instr >> 8) & 3;
            int double_spaced = 0;
            if (D) vd |= 0x10;
            if (size == 3) {
                /* single structure to all lanes */
                int a = (instr & (1 << 4)) != 0;
                if (!L) return;
                size = (instr >> 6) & 3;
                /*
                 * VLD1 (single element to all lanes):
                 * if size == '11' || (size == '00' && a == '1') then UNDEFINED;
                 */
                if (n == 0 && (size == 3 || (size == 0 && a == 1))) return;
                all_lanes = 1;
                align = al_reg_align(n, size, index_align);
                if (n == 3 && size == 3) size -= 1;
            }
            else {
                /* single structure to one lane */
                switch (size) {
                case 0:
                    if (index_align & 1 && n == 0) return;
                    index = index_align >> 1;
                    break;
                case 1:
                    double_spaced = (index_align & 2) != 0;
                    if (n == 0 && double_spaced) return;
                    index = index_align >> 2;
                    break;
                case 2:
                    double_spaced = (index_align & 4) != 0;
                    if (n == 0 && double_spaced) return;
                    index = index_align >> 3;
                    break;
                }
                align = ix_reg_align(n, size, index_align);
                if (n == 2 && align != 0) return;
            }
            add_str(L ? "vld" : "vst");
            add_dec_uint32(n + 1);
            add_char('.');
            add_dec_uint32(8 << size);
            add_str(" {d");
            add_dec_uint32(vd);
            if (all_lanes) {
                int T = (instr & (1 << 5)) != 0;
                if (!T) {
                    if (n > 0) {
                        uint32_t ix = 0;
                        for (ix = 0 ; ix < n ; ix ++) {
                            add_str("[],d");
                            add_dec_uint32(vd + ix + 1);
                        }
                    }
                }
                else {
                    if (n == 0) {
                        add_str("[],d");
                        add_dec_uint32(vd + 1);
                    }
                    else {
                        /* double-spaced register transfer */
                        unsigned i;
                        for (i = 1; i <= n; i++) {
                            add_str("[],d");
                            add_dec_uint32(vd + i * 2);
                        }
                    }
                }
                add_str("[]");
            }
            else {
                unsigned i;
                add_char('[');
                add_dec_uint32(index);
                add_char(']');
                for (i = 1; i <= n; i++) {
                    add_str(",d");
                    add_dec_uint32(vd + i * (double_spaced ? 2 : 1));
                    add_char('[');
                    add_dec_uint32(index);
                    add_char(']');
                }
            }
        }
        else {
            uint32_t type = (instr >> 8) & 0xf;
            uint32_t size = (instr >> 6) & 3;
            int D = (instr & (1 << 22)) != 0;
            int L = (instr & (1 << 21)) != 0;
            uint32_t vd = (instr >> 12) & 0xf;
            if (D) vd |= 0x10;
            align = (instr >> 4) & 3;
            switch (type) {
            case 2:
            case 6:
            case 7:
            case 10:
                add_str(L ? "vld1." : "vst1.");
                break;
            case 8:
            case 9:
            case 3:
                add_str(L ? "vld2." : "vst2.");
                break;
            case 4:
            case 5:
                add_str(L ? "vld3." : "vst3.");
                break;
            case 0:
            case 1:
                add_str(L ? "vld4." : "vst4.");
                break;
            }
            add_dec_uint32(8 << size);
            add_str(" {d");
            add_dec_uint32(vd);
            switch (type) {
            case 0:
            case 2:
            case 3:
                /* nregs = 4, inc = 1 */
                add_str(",d");
                add_dec_uint32(vd + 1);
                add_str(",d");
                add_dec_uint32(vd + 2);
                add_str(",d");
                add_dec_uint32(vd + 3);
                break;
            case 1:
                /* nregs = 4, inc = 2 */
                add_str(",d");
                add_dec_uint32(vd + 2);
                add_str(",d");
                add_dec_uint32(vd + 4);
                add_str(",d");
                add_dec_uint32(vd + 6);
                break;
            case 5:
                /* nregs = 3, inc = 2 */
                add_str(",d");
                add_dec_uint32(vd + 2);
                add_str(",d");
                add_dec_uint32(vd + 4);
                break;
            case 4:
            case 6:
                /* nregs = 3, inc = 1 */
                add_str(",d");
                add_dec_uint32(vd + 1);
                add_str(",d");
                add_dec_uint32(vd + 2);
                break;
            case 9:
                /* nregs = 2, inc = 2 */
                add_str(",d");
                add_dec_uint32(vd + 2);
                break;
            case 7:
                /* nregs = 1 */
                break;
            case 8:
            case 10:
                /* nregs = 2, inc = 1 */
                add_str(",d");
                add_dec_uint32(vd + 1);
                break;
            default:
                buf_pos = 0;
                break;
            }
            if (align) align += 2;
        }
        if (buf_pos > 0) {
            uint32_t rm = instr & 0xf;
            add_str("}, [");
            add_reg_name((instr >> 16) & 0xf);
            if (align) {
                add_char('@');
                add_dec_uint32(8 << align);
            }
            if (rm == 0xf) {
                add_char(']');
            }
            else if (rm == 0xd) {
                add_str("]!");
            }
            else {
                add_str("], ");
                add_reg_name(rm);
            }
            return;
        }
    }
}

static void disassemble_vfp_other_data_processing_instr(uint32_t instr, const char * cond) {
    uint32_t op2 = (instr >> 16) & 0xf;
    int T = (instr & (1 << 7)) != 0;
    int sz = (instr & (1 << 8)) != 0;
    uint32_t vd = (instr >> 12) & 0xf;
    uint32_t vm = instr & 0xf;
    uint32_t imm4i = ((instr & 0xf) << 1) + ((instr >> 5) & 1);
    uint32_t sx = ((instr >> 7) & 1);
    uint32_t fbits = 0;

    if (sz) {
        if (instr & (1 << 5)) vm |= 0x10;
        if (instr & (1 << 22)) vd |= 0x10;
    }
    else {
        vm *= 2;
        vd *= 2;
        if (instr & (1 << 5)) vm++;
        if (instr & (1 << 22)) vd++;
    }

    if (instr & (1 << 6)) {
        switch (op2) {
        case 0:
            add_str(T ? "vabs" : "vmov");
            break;
        case 1:
            add_str(T ? "vsqrt" : "vneg");
            break;
        case 2:
        case 3:
            /* VCVTB, VCVTT (between half-precision and single-precision, VFP) */
            add_str("vcvt");
            add_char(T ? 't' : 'b');
            add_str(cond);
            add_str(instr & (1 << 16) ? ".f16.f32 s" : ".f32.f16 s");
            add_dec_uint32(vd);
            add_str(", s");
            add_dec_uint32(vm);
            return;
        case 4:
        case 5:
            /* VCMP, VCMPE */
            add_str("vcmp");
            if (T) add_char('e');
            add_str(cond);
            add_str(sz ? ".f64 d" : ".f32 s");
            add_dec_uint32(vd);
            if (instr & (1 << 16)) {
                add_str(", #0.0");
            }
            else {
                add_str(sz ? ", d" : ", s");
                add_dec_uint32(vm);
            }
            return;
        case 7:
            /* VCVT (between double-precision and single-precision) */
            add_str("vcvt");
            add_str(cond);
            if (sz) {
                vd = ((instr >> 12) & 0xf) * 2;
                if (instr & (1 << 22)) vd++;
            }
            else {
                vd = (instr >> 12) & 0xf;
                if (instr & (1 << 22)) vd |= 0x10;
            }
            add_str(sz ? ".f32.f64 s" : ".f64.f32 d");
            add_dec_uint32(vd);
            add_str(sz ? ", d" : ", s");
            add_dec_uint32(vm);
            return;
        case 8:
            /* VCVT, VCVTR (between floating-point and integer (to float), VFP) */
            add_str("vcvt");
            add_str(cond);
            add_str(sz ? ".f64" : ".f32");
            add_str(T ? ".s32" : ".u32");
            add_str(sz ? " d" : " s");
            add_dec_uint32(vd);
            add_str(", s");
            vm = (instr & 0xf) * 2;
            if (instr & (1 << 5)) vm++;
            add_dec_uint32(vm);
            return;
        case 10:
        case 11:
            /* VCVT (between floating-point and fixed-point (to float), VFP) */
            add_str("vcvt");
            add_str(cond);
            add_str(sz ? ".f64" : ".f32");
            if (op2 & 1) {
                add_str(T ? ".u32" : ".u16");
            }
            else {
                add_str(T ? ".s32" : ".s16");
            }
            add_str(sz ? " d" : " s");
            add_dec_uint32(vd);
            add_str(sz ? ", d" : ", s");
            add_dec_uint32(vd);
            add_str(", #");
            fbits = sx ? (32 - imm4i) : (16 - imm4i);
            add_dec_uint32(fbits);
            return;
        case 12:
        case 13:
            /* VCVT, VCVTR (between floating-point and integer (to integer), VFP) */
            add_str("vcvt");
            if (!T) add_char('r');
            add_str(cond);
            add_str(op2 == 12 ? ".u32" : ".s32");
            add_str(sz ? ".f64" : ".f32");
            add_str(" s");
            vd = ((instr >> 12) & 0xf) * 2;
            if (instr & (1 << 22)) vd++;
            add_dec_uint32(vd);
            add_str(sz ? ", d" : ", s");
            add_dec_uint32(vm);
            return;
        case 14:
        case 15:
            /* VCVT (between floating-point and fixed-point (to fixed), VFP) */
            add_str("vcvt");
            add_str(cond);
            if (op2 & 1) {
                add_str(T ? ".u32" : ".u16");
            }
            else {
                add_str(T ? ".s32" : ".s16");
            }
            add_str(sz ? ".f64" : ".f32");
            add_str(sz ? " d" : " s");
            add_dec_uint32(vd);
            add_str(sz ? ", d" : ", s");
            add_dec_uint32(vd);
            add_str(", #");
            fbits = sx ? (32 - imm4i) : (16 - imm4i);
            add_dec_uint32(fbits);
            return;
        }
        add_str(cond);
        add_str(sz ? ".f64 d" : ".f32 s");
        add_dec_uint32(vd);
        add_str(sz ? ", d" : ", s");
        add_dec_uint32(vm);
    }
    else {
        /* VMOV (immediate) */
        uint32_t imm = ((instr >> 12) & 0xf0) | (instr & 0xf);
        add_str("vmov");
        add_str(cond);
        add_str(sz ? ".f64 d" : ".f32 s");
        add_dec_uint32(vd);
        add_str(", #");
        if (!sz) {
            uint32_t v = vfp_expand_imm32(imm);
            add_flt_uint32(v);
            add_str(" ; 0x");
            add_hex_uint32(v);
        }
        else {
            uint64_t v = vfp_expand_imm64(imm);
            add_flt_uint64(v);
            add_str(" ; 0x");
            add_hex_uint64(v);
        }
    }
}

static void disassemble_vfp_data_processing_instr(uint32_t instr, const char * cond) {
    uint32_t op1 = (instr >> 20) & 0xf;
    uint32_t op = 0;
    uint32_t mode = (instr >> 8) & 0xf;
    uint32_t dest = (instr >> 25) & 0x7;
    uint32_t vn = (instr >> 16) & 0xf;
    uint32_t vd = (instr >> 12) & 0xf;
    uint32_t vm = instr & 0xf;

    switch (op1) {
    case 0:
    case 4:
        if (dest == 1 && mode == 9) op = (instr >> 24) & 1;
        else if (dest == 1 && (mode == 8 || mode == 10)) op = (instr >> 9) & 1;
        else if (dest == 1 && mode == 13) op = (instr >> 21) & 1;
        else if (dest == 7 && (mode == 10 || mode == 11)) op = (instr >> 6) & 1;
        add_str(op ? "vmls" : "vmla");
        break;
    case 1:
    case 5:
        add_str(instr & (1 << 6) ? "vnmla" : "vnmls");
        break;
    case 2:
    case 6:
        add_str(instr & (1 << 6) ? "vnmul" : "vmul");
        break;
    case 3:
    case 7:
        add_str(instr & (1 << 6) ? "vsub" : "vadd");
        break;
    case 8:
    case 12:
        add_str(instr & (1 << 6) ? "" : "vdiv");
        break;
    case 11:
    case 15:
        disassemble_vfp_other_data_processing_instr(instr, cond);
        return;
    }

    if (buf_pos > 0) {
        add_str(cond);
        add_str(instr & (1 << 8) ? ".f64" : ".f32");
        if (instr & (1 << 8)) {
            if (instr & (1 << 22)) vd |= 0x10;
            if (instr & (1 << 7)) vn |= 0x10;
            if (instr & (1 << 5)) vm |= 0x10;
            add_str(" d");
            add_dec_uint32(vd);
            add_str(", d");
            add_dec_uint32(vn);
            add_str(", d");
            add_dec_uint32(vm);
        }
        else {
            vd *= 2;
            vn *= 2;
            vm *= 2;
            if (instr & (1 << 22)) vd |= 1;
            if (instr & (1 << 7)) vn |= 1;
            if (instr & (1 << 5)) vm |= 1;
            add_str(" s");
            add_dec_uint32(vd);
            add_str(", s");
            add_dec_uint32(vn);
            add_str(", s");
            add_dec_uint32(vm);
        }
    }
}

static void disassemble_coprocessor_instr(uint32_t instr, const char * cond, unsigned cond_code) {
    if (cond_code != 15 && (instr & 0x0f000e10) == 0x0e000a00) {
        disassemble_vfp_data_processing_instr(instr, cond);
        if (buf_pos > 0) return;
    }

    if ((instr & 0x0e000000) == 0x0c000000) {
        int P = (instr & (1 << 24)) != 0;
        int U = (instr & (1 << 23)) != 0;
        int D = (instr & (1 << 22)) != 0;
        int W = (instr & (1 << 21)) != 0;
        int L = (instr & (1 << 20)) != 0;
        if ((instr & 0x00000e00) == 0x00000a00) {
            uint32_t rn = (instr >> 16) & 0xf;
            int V = (instr & (1 << 8)) != 0;
            if (!P && !U && !W) {
                if (D && (instr & 0x000000d0) == 0x00000010) {
                    /* 64-bit transfers between ARM core and extension registers */
                    int C = (instr & (1 << 8)) != 0;
                    uint32_t dn = instr & 0xf;
                    uint32_t sn = dn * 2;
                    if (instr & (1 << 5)) sn++;
                    add_str("vmov");
                    add_str(cond);
                    if (instr & (1 << 5)) dn |= 0x10;
                    add_char(' ');
                    if (L) {
                        add_reg_name((instr >> 12) & 0xf);
                        add_str(", ");
                        add_reg_name((instr >> 16) & 0xf);
                        add_str(", ");
                        if (!C) {
                            add_char('s');
                            add_dec_uint32(sn);
                            add_str(", s");
                            add_dec_uint32(sn + 1);
                        }
                        else {
                            add_char('d');
                            add_dec_uint32(dn);
                        }
                    }
                    else {
                        if (!C) {
                            add_char('s');
                            add_dec_uint32(sn);
                            add_str(", s");
                            add_dec_uint32(sn + 1);
                        }
                        else {
                            add_char('d');
                            add_dec_uint32(dn);
                        }
                        add_str(", ");
                        add_reg_name((instr >> 12) & 0xf);
                        add_str(", ");
                        add_reg_name((instr >> 16) & 0xf);
                    }
                }
            }
            else if (P && !W) {
                /* vldr, vstr */
                uint32_t dn = (instr >> 12) & 0xf;
                uint32_t imm8 = instr & 0xff;
                add_str(L ? "vldr" : "vstr");
                add_str(cond);
                add_char(' ');
                if (V) {
                    if (D) dn |= 0x10;
                    add_char('d');
                }
                else {
                    dn = (dn << 1) + D;
                    add_char('s');
                }
                add_dec_uint32(dn);
                add_str(", [");
                add_reg_name(rn);
                if (imm8 != 0) {
                    add_str(", #");
                    add_char(U ? '+' : '-');
                    add_dec_uint32(imm8 << 2);
                }
                else if (U == 0 && imm8 == 0) {
                    /* Special case [<reg>,#-0] : P==1, W==0, U==0, IMM==0 */
                    add_str(", #-0");
                }
                add_char(']');
            }
            else if (P == U && W) {
                /* Undefined */
            }
            else {
                uint32_t dn = (instr >> 12) & 0xf;
                uint32_t imm8 = instr & 0xff;
                uint32_t dm = 0;
                if (D) dn |= 0x10;
                dm = dn + (V ? imm8 / 2 : imm8) - 1;
                if ((L == 1 && P == 0 && U == 1 && W == 1 && rn == 13) ||
                    (L == 0 && P == 1 && U == 0 && W == 1 && rn == 13)) {
                    add_str(L ? "vpop" : "vpush");
                    add_str(cond);
                    add_str(" {");
                }
                else {
                    add_str(L ? "vldm" : "vstm");
                    add_str(P ? "db" : "ia");
                    add_str(cond);
                    add_char(' ');
                    add_reg_name(rn);
                    if (W) add_char('!');
                    add_str(", {");
                }
                add_char(V ? 'd' : 's');
                add_dec_uint32(dn);
                if (dn != dm) {
                    add_char('-');
                    add_char(V ? 'd' : 's');
                    add_dec_uint32(dm);
                }
                add_char('}');
            }
        }
        else if (!P && !U && D && !W) {
            add_str(L ? "mrrc" : "mcrr");
            add_str(cond_code == 15 ? "2" : cond);
            add_str(" p");
            add_dec_uint32((instr >> 8) & 0xf);
            add_str(", ");
            add_dec_uint32((instr >> 4) & 0xf);
            add_str(", ");
            add_reg_name((instr >> 12) & 0xf);
            add_str(", ");
            add_reg_name((instr >> 16) & 0xf);
            add_str(", c");
            add_dec_uint32(instr & 0xf);
        }
        else if (!P && !U && !D && !W) {
            /* Undefined */
        }
        else {
            uint32_t imm = instr & 0x000000ff;
            add_str(L ? "ldc" : "stc");
            if (cond_code == 15) add_char('2');
            if (D) add_char('l');
            if (cond_code != 15) add_str(cond);
            add_str(" p");
            add_dec_uint32((instr >> 8) & 0xf);
            add_str(", c");
            add_dec_uint32((instr >> 12) & 0xf);
            add_str(", [");
            if ((instr & 0x000f0000) == 0x000f0000 && !P && U && !W) {
                add_str("pc], {");
                add_dec_uint32(imm);
                add_char('}');
            }
            else {
                uint32_t rn = (instr & 0x000f0000) >> 16;
                add_reg_name(rn);
                if (P) {
                    if (imm != 0) {
                        add_str(", #");
                        add_char(U ? '+' : '-');
                        add_dec_uint32(imm << 2);
                    }
                    else if (U == 0 && imm == 0) {
                        /* Special case [reg,#-0] : P==1, U==0, IMM==0 */
                        add_str(", #-0");
                    }
                    add_char(']');
                    if (W) add_char('!');
                }
                else if (W) {
                    add_char(']');
                    if (imm != 0 || U == 0) {
                        add_str(", #");
                        add_char(U ? '+' : '-');
                        add_dec_uint32(imm << 2);
                    }
                }
                else if (U) {
                    add_str("], {");
                    add_dec_uint32(imm);
                    add_char('}');
                }
                else {
                    add_str("], ???");
                }
            }
        }
        if (buf_pos > 0) return;
    }

    if (((instr & 0x0f900f5f) == 0x0e800b10) &&
        ((instr & 0xf0000000) != 0xf0000000)) {
        /* VDUP (ARM core register) */
        int Q = instr & (1 << 21);
        uint32_t vd = (instr >> 16) & 0xf;
        if (instr & (1 << 7)) vd |= 0x10;
        if (Q) vd /= 2;
        add_str("vdup");
        add_str(cond);
        if (instr & (1 << 22)) {
            add_str(".8");
        }
        else if (instr & (1 << 5)) {
            add_str(".16");
        }
        else {
            add_str(".32");
        }
        add_str(Q ? " q" : " d");
        add_dec_uint32(vd);
        add_str(", ");
        add_reg_name((instr >> 12) & 0xf);
        return;
    }

    if ((instr & 0x0fe00f90) == 0x0ee00a10) {
        int A = (instr & (1 << 20)) != 0;
        uint32_t rt = (instr >> 12) & 0xf;
        const char * reg = NULL;
        switch ((instr >> 16) & 0xf) {
        case 0: reg = "fpsid"; break;
        case 1: reg = "fpscr"; break;
        case 6: reg = "mvfr1"; break;
        case 7: reg = "mvfr0"; break;
        case 8: reg = "fpexc"; break;
        case 9: reg = "fpinst"; break;
        case 10: reg = "fpinst2"; break;
        }
        if (reg != NULL) {
            add_str(A ? "vmrs" : "vmsr");
            add_str(cond);
            add_char(' ');
            if (!A) {
                add_str(reg);
                add_str(", ");
            }
            if (rt == 15) add_str("APSR_nzcv");
            else add_reg_name(rt);
            if (A) {
                add_str(", ");
                add_str(reg);
            }
            return;
        }
    }

    if (((instr & 0x0f900f1f) == 0x0e000b10) &&
        ((instr & 0xf0000000) != 0xf0000000)) {
        /* VMOV (ARM core register to scalar) */
        uint32_t size = 0;
        uint32_t dn = (instr >> 16) & 0xf;
        uint32_t x = 0;
        add_str("vmov");
        add_str(cond);
        if (instr & (1 << 22)) {
            size = 8;
            x = (instr >> 5) & 3;
            if (instr & (1 << 21)) x |= 4;
        }
        else if (instr & (1 << 5)) {
            size = 16;
            if (instr & (1 << 6)) x |= 1;
            if (instr & (1 << 21)) x |= 2;
        }
        else {
            size = 32;
            if (instr & (1 << 21)) x |= 1;
        }
        if (instr & (1 << 7)) dn |= 0x10;
        if (size != 0) {
            add_char('.');
            add_dec_uint32(size);
        }
        add_str(" d");
        add_dec_uint32(dn);
        add_char('[');
        add_dec_uint32(x);
        add_str("], ");
        add_reg_name((instr >> 12) & 0xf);
        return;
    }

    if ((instr & 0x0f100f10) == 0x0e100b10) {
        /* VMOV (scalar to ARM core register) */
        uint32_t size = 32;
        int U = (instr & (1 << 23)) != 0;
        uint32_t dn = (instr >> 16) & 0xf;
        uint32_t x = 0;
        add_str("vmov");
        add_str(cond);
        if (instr & (1 << 22)) {
            size = 8;
            x = (instr >> 5) & 3;
            if (instr & (1 << 21)) x |= 4;
        }
        else if (instr & (1 << 5)) {
            size = 16;
            if (instr & (1 << 6)) x |= 1;
            if (instr & (1 << 21)) x |= 2;
        }
        else {
            size = 32;
            if (instr & (1 << 21)) x |= 1;
        }
        if (instr & (1 << 7)) dn |= 0x10;
        if (size != 0) {
            add_char('.');
            if (size != 32) add_char(U ? 'u' : 's');
            add_dec_uint32(size);
        }
        add_char(' ');
        add_reg_name((instr >> 12) & 0xf);
        add_str(", ");
        add_str(" d");
        add_dec_uint32(dn);
        add_char('[');
        add_dec_uint32(x);
        add_char(']');
        return;
    }

    if ((instr & 0x0fe00f10) == 0x0e000a10 &&
        ((instr & 0xf0000000) != 0xf0000000)) {
        int L = (instr & (1 << 20)) != 0;
        uint32_t dn = (instr >> 15) & 0x1e;
        add_str("vmov");
        add_str(cond);
        if (instr & (1 << 7)) dn |= 1;
        add_char(' ');
        if (L) {
            add_reg_name((instr >> 12) & 0xf);
            add_str(", s");
            add_dec_uint32(dn);
        }
        else {
            add_char('s');
            add_dec_uint32(dn);
            add_str(", ");
            add_reg_name((instr >> 12) & 0xf);
        }
        return;
    }

    if ((instr & 0x0f000010) == 0x0e000010) {
        int A = (instr & (1 << 20)) != 0;
        add_str(A ? "mrc" : "mcr");
        add_str(cond_code == 15 ? "2" : cond);
        add_str(" p");
        add_dec_uint32((instr >> 8) & 0xf);
        add_str(", ");
        add_dec_uint32((instr >> 21) & 0x7);
        add_str(", ");
        add_reg_name((instr >> 12) & 0xf);
        add_str(", c");
        add_dec_uint32((instr >> 16) & 0xf);
        add_str(", c");
        add_dec_uint32(instr & 0xf);
        if (instr & 0x000000e0) {
            add_str(", ");
            add_dec_uint32((instr >> 5) & 0x7);
        }
        return;
    }

    if ((instr & 0x0f000010) == 0x0e000000) {
        add_str("cdp");
        add_str(cond_code == 15 ? "2" : cond);
        add_str(" p");
        add_dec_uint32((instr >> 8) & 0xf);
        add_str(", ");
        add_dec_uint32((instr >> 20) & 0xf);
        add_str(", c");
        add_dec_uint32((instr >> 12) & 0xf);
        add_str(", c");
        add_dec_uint32((instr >> 16) & 0xf);
        add_str(", c");
        add_dec_uint32(instr & 0xf);
        if (instr & 0x000000e0) {
            add_str(", ");
            add_dec_uint32((instr >> 5) & 0x7);
        }
        return;
    }
}

static void disassemble_unconditional_instr(uint32_t addr, uint32_t instr) {
    if ((instr & 0xfff10020) == 0xf1000000) {
        uint32_t imod = (instr >> 18) & 3;
        uint32_t mode = instr & 0x1f;
        add_str("cps");
        if (imod >= 2) {
            add_str(imod == 2 ? "ie" : "id");
            add_char(' ');
            if (instr & (1 << 8)) add_char('a');
            if (instr & (1 << 7)) add_char('i');
            if (instr & (1 << 6)) add_char('f');
            if (instr & (1 << 17)) {
                /* do not add the coma if there was no AIF. */
                if (((instr >> 6) & 0x7) != 0) {
                    add_str(", #");
                }
                else {
                    add_char('#');
                }
                add_dec_uint32(mode);
            }
        }
        else {
            add_str(" #");
            add_dec_uint32(mode);
        }
        return;
    }

    if ((instr & 0xffff00f0) == 0xf1010000) {
        add_str("setend ");
        add_str(instr & 0x00000200 ? "be" : "le");
        return;
    }

    if ((instr & 0xfe000000) == 0xf2000000) {
        disassemble_advanced_simd_data_processing(instr);
        return;
    }

    if ((instr & 0xff100000) == 0xf4000000) {
        disassemble_advanced_simd_load_store(instr);
        return;
    }

    if ((instr & 0xfd700000) == 0xf4100000) {
        /* TODO: Unallocated memory hint */
        return;
    }

    if ((instr & 0xfd700000) == 0xf4500000) {
        int U = (instr & (1 << 23)) != 0;
        int reg = (instr & (1 << 25)) != 0;
        add_str("pli [");
        add_reg_name((instr & 0x000f0000) >> 16);
        if (!reg) {
            uint32_t offs = instr & 0xfff;
            add_str(", #");
            add_char(U ? '+' : '-');
            add_dec_uint32(offs);
        }
        else {
            add_str(", ");
            add_char(U ? '+' : '-');
            add_shift(instr, 0);
        }
        add_char(']');
        return;
    }

    if ((instr & 0xfd300000) == 0xf5100000) {
        int U = (instr & (1 << 23)) != 0;
        int R = (instr & (1 << 22)) != 0;
        int reg = (instr & (1 << 25)) != 0;
        int rn = (instr & 0x000f0000) >> 16;
        add_str("pld");
        if (!R) add_char('w');
        add_str(" [");
        add_reg_name(rn);
        if (!reg) {
            uint32_t offs = instr & 0xfff;
            if (offs) {
                add_str(", #");
                add_char(U ? '+' : '-');
                add_dec_uint32(offs);
            }
            else if (U ==0 && offs == 0) {
                /* Special case [reg,#-0] : P==1, W==0, U==0, IMM==0 */
                add_str(", #-0");
            }
        }
        else {
            add_str(", ");
            add_char(U ? '+' : '-');
            add_shift(instr, 0);
        }
        add_char(']');
        return;
    }

    if ((instr & 0xfff00000) == 0xf5700000) {
        switch ((instr & 0x000000f0) >> 4) {
        case 1:
            add_str("clrex");
            return;
        case 4:
            add_str("dsb");
            break;
        case 5:
            add_str("dmb");
            break;
        case 6: {
            uint32_t option = (instr & 0x0000000f);
            add_str("isb");
            add_char(' ');
            if (option == 15) add_str("sy");
            else {
                add_char('#');
                add_dec_uint32(option);
            }
            return;
            break;
        }
        default:
            return;
        }
        add_char(' ');
        switch (instr & 0x0000000f) {
        case 15: add_str("sy"); break;
        case 14: add_str("st"); break;
        case 11: add_str("ish"); break;
        case 10: add_str("ishst"); break;
        case  7: add_str("nsh"); break;
        case  6: add_str("nshst"); break;
        case  3: add_str("osh"); break;
        case  2: add_str("oshst"); break;
        default: add_dec_uint32(instr & 0x0000000f);
        }
        return;
    }

    if ((instr & 0xfe500000) == 0xf8400000) {
        add_str("srs");
        add_auto_inc_mode(instr, 0);
        add_str(" sp");
        if (instr & (1 << 21)) add_char('!');
        add_str(", ");
        add_str(proc_modes[instr & 0x1f]);
        return;
    }

    if ((instr & 0xfe500000) == 0xf8100000) {
        add_str("rfe");
        add_auto_inc_mode(instr, 0);
        add_char(' ');
        add_reg_name((instr >> 16) & 0xf);
        if (instr & (1 << 21)) add_char('!');
        return;
    }

    if ((instr & 0xfe000000) == 0xfa000000) {
        int32_t offset = instr & 0x00ffffff;
        if (offset & 0x00800000) offset |= ~0x00ffffff;
        offset = offset << 2;
        if (instr & (1 << 24)) offset |= 2;
        add_str("blx");
        add_char(' ');
        if (offset < 0) {
            add_char('-');
            add_dec_uint32(-offset);
        }
        else {
            add_char('+');
            add_dec_uint32(offset);
        }
        add_addr(addr + offset + 8);
        return;
    }
}

static void disassemble_data_instr(uint32_t instr, const char * cond, uint32_t addr) {
    uint32_t op_code = (instr >> 21) & 0xf;
    int I = (instr & (1 << 25)) != 0;
    int S = (instr & (1 << 20)) != 0;
    uint32_t rn = (instr >> 16) & 0xf;
    uint32_t rd = (instr >> 12) & 0xf;
    int no_shift_name = 1;

    if ((instr & 0xffffffff) == 0xe1a00000) {
        add_str("nop");
        return;
    }

    if ((instr & 0x03ff0000) == 0x028f0000 || (instr & 0x03ff0000) == 0x024f0000) {
        /* ADR{<c>}{<q>} <Rd>, <label> */
        uint32_t add = (instr >> 23) & 0x1;
        add_str("adr");
        add_str(cond);
        add_char(' ');
        add_reg_name(rd);
        add_str(", ");
        add_modifed_immediate_address(instr & 0x00000fff, addr, add, 4);
        return;
    }

    if ((instr & 0x0fef0000) == 0x01a00000) {
        uint32_t shift_imm = (instr >> 7) & 0x1f;
        uint32_t shift_type = (instr >> 5) & 3;
        if ((instr & 0x00000010) != 0 || shift_imm != 0) {
            add_str(shift_names[shift_type]);
        }
        else if ((instr & 0x00000010) == 0 && shift_type == 3 && shift_imm == 0) {
            add_str("rrx");
        }
    }

    if (buf_pos == 0) {
        uint32_t shift_type = (instr >> 5) & 3;
        uint32_t shift_imm = (instr >> 7) & 0x1f;
        int reg_shift = (instr & 0x00000010) != 0;
        no_shift_name = 0;
        if (op_code >= 8 && op_code <= 11) {
            if (!S) return;
            S = 0;
        }
        if ((op_code == 13) && (I == 0) && (reg_shift || shift_type != 0 || shift_imm != 0)) {
            /* use pseudo instructions : asr, lsl, lsr, ror, rrx */
            if (shift_type == 3 && shift_imm == 0) add_str("rrx");
            else add_str(shift_names[shift_type]);
            no_shift_name = 1;
        }
        else {
            add_str(op_names[op_code]);
        }
    }

    if (S) add_char('s');
    add_str(cond);
    add_char(' ');
    if (op_code < 8 || op_code > 11) {
        add_reg_name(rd);
        add_str(", ");
    }
    if (op_code != 13 && op_code != 15) {
        add_reg_name(rn);
        add_str(", ");
    }

    if (I) {
        add_modifed_immediate_constant(instr & 0xfff);
    }
    else {
        /* Register and shift */
        add_shift(instr, no_shift_name);
    }
}

static void disassemble_misc_instr(uint32_t instr, const char * cond) {

    if ((instr & 0x0fffffd0) == 0x012fff10) {
        add_char('b');
        if (instr & 0x00000020) add_char('l');
        add_char('x');
        add_str(cond);
        add_char(' ');
        add_reg_name(instr & 0xf);
        return;
    }

    if ((instr & 0x0ffffff0) == 0x012fff20) {
        add_str("bxj");
        add_str(cond);
        add_char(' ');
        add_reg_name(instr & 0xf);
        return;
    }

    if ((instr & 0x0ff000f0) == 0x01200070) {
        uint32_t imm16 = (instr & 0xf) + ((instr &0xfff00) >> 4);
        add_str("bkpt");
        add_str(cond);
        add_str(" #");
        add_dec_uint32(imm16);
        return;
    }

    if ((instr & 0x0fff0000) == 0x03200000) {
        uint32_t op2 = instr & 0xff;
        if ((op2 & 0xf0) == 0xf0) {
            add_str("dbg");
            add_str(cond);
            add_str(" #");
            add_dec_uint32(op2 & 0xf);
            return;
        }
        switch (op2) {
        case 1: add_str("yield"); break;
        case 2: add_str("wfe"); break;
        case 3: add_str("wfi"); break;
        case 4: add_str("sev"); break;
        default: add_str("nop"); break;
        }
        add_str(cond);
        if (op2 > 4) {
            add_str(" #");
            add_dec_uint32(op2);
        }
        return;
    }

    if ((instr & 0x0fb000f0) == 0x01000000) {
        add_str("mrs");
        add_str(cond);
        add_char(' ');
        add_reg_name((instr & 0x0000f000) >> 12);
        add_str(", ");
        add_str(instr & (1 << 22) ? "spsr" : "cpsr");
        return;
    }

    if ((instr & 0x0db00000) == 0x01200000) {
        int R = (instr & (1 << 22)) != 0;
        uint32_t mask = (instr >> 16) & 0xf;
        if (R || mask) {
            add_str("msr");
            add_str(cond);
            add_char(' ');
            add_str(R ? "spsr" : "cpsr");
            add_char('_');
            if (mask & 8) add_char('f');
            if (mask & 4) add_char('s');
            if (mask & 2) add_char('x');
            if (mask & 1) add_char('c');
            add_str(", ");
            if (instr & (1 << 25)) {
                add_modifed_immediate_constant(instr & 0xfff);
                return;
            }
            if (instr & 0x000000f0) {
                buf_pos = 0;
            }
            else {
                add_reg_name(instr & 0xf);
                return;
            }
        }
    }

    if ((instr & 0x0ff000f0) == 0x01600010) {
        /* Count Leading Zeros */
        add_str("clz");
        add_str(cond);
        add_char(' ');
        add_reg_name((instr >> 12) & 0xf);
        add_str(", ");
        add_reg_name(instr & 0xf);
        return;
    }

    if ((instr & 0x0fb00000) == 0x03000000) {
        /* 16-bit immediate load */
        uint32_t imm = (instr & 0xfff) | ((instr & 0xf0000) >> 4);
        add_str(instr & (1 << 22) ? "movt" : "movw");
        add_str(cond);
        add_char(' ');
        add_reg_name((instr >> 12) & 0xf);
        add_str(", #");
        add_dec_uint32(imm);
        return;
    }

    if ((instr & 0x0fb000f0) == 0x01000090) {
        int B = (instr & (1 << 22)) != 0;
        add_str("swp");
        if (B) add_char('b');
        add_str(cond);
        add_char(' ');
        add_reg_name((instr >> 12) & 0xf);
        add_str(", ");
        add_reg_name(instr & 0xf);
        add_str(", [");
        add_reg_name((instr >> 16) & 0xf);
        add_char(']');
        return;
    }

    if ((instr & 0x0f8000f0) == 0x01800090) {
        int op = (instr >> 21) & 3;
        int L = (instr & (1 << 20)) != 0;
        add_str(L ? "ldrex" : "strex");
        switch (op) {
        case 0: break;
        case 1: add_char('d');break;
        case 2: add_char('b'); break;
        case 3: add_char('h'); break;
        }
        add_str(cond);
        add_char(' ');
        add_reg_name((instr >> 12) & 0xf);
        if (!L) {
            add_str(", ");
            add_reg_name(instr & 0xf);
            if (op == 1) {
                add_str(", ");
                add_reg_name((instr & 0xf) + 1);
            }
        }
        else if (op == 1) {
            add_str(", ");
            add_reg_name(((instr >> 12) & 0xf) + 1);
        }
        add_str(", [");
        add_reg_name((instr >> 16) & 0xf);
        add_char(']');
        return;
    }

    if ((instr & 0x0e0000f0) == 0x00000090) {
        uint32_t op = (instr >> 21) & 7;
        int S = (instr & (1 << 20)) != 0;
        switch (op) {
        case 0:
            add_str("mul");
            break;
        case 1:
            add_str("mla");
            break;
        case 2:
            if (S) return;
            add_str("umaal");
            break;
        case 3:
            if (S) return;
            add_str("mls");
            break;
        case 4:
            add_str("umull");
            break;
        case 5:
            add_str("umlal");
            break;
        case 6:
            add_str("smull");
            break;
        case 7:
            add_str("smlal");
            break;
        }
        if (S) add_char('s');
        add_str(cond);
        add_char(' ');
        if (op == 2 || op >= 4) {
            add_reg_name((instr >> 12) & 0xf);
            add_str(", ");
        }
        add_reg_name((instr >> 16) & 0xf);
        add_str(", ");
        add_reg_name(instr & 0xf);
        add_str(", ");
        add_reg_name((instr >> 8) & 0xf);
        if (op == 1 || op == 3) {
            add_str(", ");
            add_reg_name((instr >> 12) & 0xf);
        }
        return;
    }

    if ((instr & 0x0ff00090) == 0x01000080) {
        add_str("smla");
        add_char(instr & (1 << 5) ? 't' : 'b');
        add_char(instr & (1 << 6) ? 't' : 'b');
        add_str(cond);
        add_char(' ');
        add_reg_name((instr >> 16) & 0xf);
        add_str(", ");
        add_reg_name(instr & 0xf);
        add_str(", ");
        add_reg_name((instr >> 8) & 0xf);
        add_str(", ");
        add_reg_name((instr >> 12) & 0xf);
        return;
    }

    if ((instr & 0x0fd00090) == 0x01400080) {
        uint32_t ra = ((instr >> 12) & 0xf);
        add_str(((instr >> 21) & 1) ? "smul" : "smlal");
        add_char(instr & (1 << 5) ? 't' : 'b');
        add_char(instr & (1 << 6) ? 't' : 'b');
        add_str(cond);
        add_char(' ');
        if ((((instr >> 21) & 1) == 0) || (ra != 0)) {
            add_reg_name((instr >> 12) & 0xf);
            add_str(", ");
        }
        add_reg_name((instr >> 16) & 0xf);
        add_str(", ");
        add_reg_name(instr & 0xf);
        add_str(", ");
        add_reg_name((instr >> 8) & 0xf);
        return;
    }

    if ((instr & 0x0ff00090) == 0x01200080) {
        uint32_t ra = ((instr >> 12) & 0xf);
        add_str(((instr >> 5) & 1) ? "smulw" : "smlaw");
        add_char(instr & (1 << 6) ? 't' : 'b');
        add_str(cond);
        add_char(' ');
        add_reg_name((instr >> 16) & 0xf);
        add_str(", ");
        add_reg_name(instr & 0xf);
        add_str(", ");
        add_reg_name((instr >> 8) & 0xf);
        if ((ra != 0) || (((instr >> 5) & 1) == 0)) {
            add_str(", ");
            add_reg_name((instr >> 12) & 0xf);
        }
        return;
    }

    if ((instr & 0x0e000090) == 0x00000090) {
        /* Extra load/store instructions */
        int T = (instr & (1 << 24)) == 0 && (instr & (1 << 21));
        uint32_t op2 = (instr >> 5) & 3;
        int rt2 = -1;
        if (op2 == 2 || (instr & (1 << 20))) {
            add_str("ldr");
        }
        else {
            add_str("str");
        }
        if (op2 == 1) {
            add_char('h');
        }
        else if (instr & (1 << 20)) {
            add_str(op2 == 2 ? "sb" : "sh");
        }
        else {
            add_char('d');
            T = 0;
            rt2 = ((instr >> 12) & 0xf) + 1;
        }
        if (T) add_char('t');
        add_str(cond);
        add_char(' ');
        add_reg_name((instr >> 12) & 0xf);
        if (rt2 != -1) {
            add_str(", ");
            add_reg_name(rt2);
        }
        add_str(", ");
        if (instr & (1 << 22)) {
            int P = (instr & (1 << 24)) != 0;
            int U = (instr & (1 << 23)) != 0;
            int W = (instr & (1 << 21)) != 0;
            uint32_t rn = (instr >> 16) & 0xf;
            uint32_t imm8 = ((instr >> 4) & 0xf0) | (instr & 0x0f);
            add_char('[');
            add_reg_name(rn);
            if (P) {
                if (imm8 != 0) {
                    add_str(", #");
                    add_char(U ? '+' : '-');
                    add_dec_uint32(imm8);
                }
                else if (W == 0 && U ==0 && imm8 == 0) {
                    /* Special case [reg,#-0] : P==1, W==0, U==0, IMM==0 */
                    add_str(", #-0");
                }
                add_char(']');
                if (W) add_char('!');
            }
            else {
                add_char(']');
                if (imm8 != 0 || !U) {
                    add_str(", #");
                    add_char(U ? '+' : '-');
                    add_dec_uint32(imm8);
                }
            }
        }
        else if (T) {
            int U = (instr & (1 << 23)) != 0;
            add_char('[');
            add_reg_name((instr >> 16) & 0xf);
            add_str("], ");
            add_char(U ? '+' : '-');
            add_reg_name(instr & 0xf);
        }
        else {
            int P = (instr & (1 << 24)) != 0;
            int U = (instr & (1 << 23)) != 0;
            int W = (instr & (1 << 21)) != 0;
            uint32_t rn = (instr >> 16) & 0xf;

            add_char('[');
            add_reg_name(rn);

            if (P) {
                add_str(", ");
                add_char(U ? '+' : '-');
                add_reg_name(instr & 0xf);
                add_char(']');
                if (W) add_char('!');
            }
            else {
                add_str("], ");
                add_char(U ? '+' : '-');
                add_reg_name(instr & 0xf);
            }
        }
        return;
    }

    if ((instr & 0x0f900ff0) == 0x01000050) {
        add_char('q');
        if ((instr >> 22) & 1) add_char('d');
        add_str(((instr >> 21) & 1) ? "sub" : "add");
        add_str(cond);
        add_char(' ');
        add_reg_name((instr >> 12) & 0xf);
        add_str(", ");
        add_reg_name(instr & 0xf);
        add_str(", ");
        add_reg_name((instr >> 16) & 0xf);
        return;
    }

    if ((instr & 0x0ffffff0) == 0x01600070) {
        add_str("smc");
        add_str(cond);
        add_str(" #");
        add_dec_uint32(instr & 0xf);
        return;
    }
}

static void disassemble_media_instr(uint32_t instr, const char * cond) {
    if ((instr & 0x0f800000) == 0x06000000) {
        /* Parallel addition and subtraction */
        if ((instr & (1 << 22)) == 0) {
            /* signed */
            switch ((instr >> 20) & 3) {
            case 1: add_str("s"); break;
            case 2: add_str("q"); break;
            case 3: add_str("sh"); break;
            default: buf_pos = 0; return;
            }
        }
        else {
            /* unsigned */
            switch ((instr >> 20) & 3) {
            case 1: add_str("u"); break;
            case 2: add_str("uq"); break;
            case 3: add_str("uh"); break;
            default: buf_pos = 0; return;
            }
        }
        switch ((instr >> 5) & 7) {
        case 0: add_str("add16"); break;
        case 1: add_str("asx"); break;
        case 2: add_str("sax"); break;
        case 3: add_str("sub16"); break;
        case 4: add_str("add8"); break;
        case 7: add_str("sub8"); break;
        default: buf_pos = 0; return;
        }
        add_str(cond);
        add_char(' ');
        add_reg_name((instr >> 12) & 0xf);
        add_str(", ");
        add_reg_name((instr >> 16) & 0xf);
        add_str(", ");
        add_reg_name(instr & 0xf);
        return;
    }
    if ((instr & 0x0f800000) == 0x06800000) {
        /* Packing, unpacking, saturation, and reversal */
        switch ((instr >> 20) & 7) {
        case 0:
            if ((instr & (1 << 5)) == 0) {
                /* Pack Halfword */
                uint32_t imm = (instr >> 7) & 0x1f;
                add_str("pkh");
                add_str(instr & (1 << 6) ? "tb" : "bt");
                add_str(cond);
                add_char(' ');
                add_reg_name((instr >> 12) & 0xf);
                add_str(", ");
                add_reg_name((instr >> 16) & 0xf);
                add_str(", ");
                add_reg_name(instr & 0xf);
                if (imm) {
                    add_str(", ");
                    add_str(instr & (1 << 6) ? "asr" : "lsl");
                    add_str(" #");
                    add_dec_uint32(imm);
                }
                else if (imm == 0 && ((instr & (1 << 6)) != 0)) {
                    add_str(", asr #");
                    add_dec_uint32(32);
                }
                return;
            }
            if (((instr >> 5) & 7) == 5) {
                /* Select Bytes */
                add_str("sel");
                add_str(cond);
                add_char(' ');
                add_reg_name((instr >> 12) & 0xf);
                add_str(", ");
                add_reg_name((instr >> 16) & 0xf);
                add_str(", ");
                add_reg_name(instr & 0xf);
                return;
            }
            break;
        case 3:
            if (((instr >> 5) & 3) == 1) {
                /* Reverse */
                add_str("rev");
                if (instr & (1 << 7)) add_str("16");
                add_str(cond);
                add_char(' ');
                add_reg_name((instr >> 12) & 0xf);
                add_str(", ");
                add_reg_name(instr & 0xf);
                return;
            }
            break;
        case 7:
            if (((instr >> 5) & 3) == 1) {
                /* Reverse */
                add_str(instr & (1 << 7) ? "revsh" : "rbit");
                add_str(cond);
                add_char(' ');
                add_reg_name((instr >> 12) & 0xf);
                add_str(", ");
                add_reg_name(instr & 0xf);
                return;
            }
            break;
        }

        add_char(instr & (1 << 22) ? 'u' : 's');
        if ((instr & (1 << 21)) != 0 && ((instr >> 5) & 7) != 3) {
            /* Saturate */
            uint32_t imm = (instr >> 7) & 0x1f;
            add_str("sat");
            if (instr & (1 << 5)) add_str("16");
            add_str(cond);
            add_char(' ');
            add_reg_name((instr >> 12) & 0xf);
            add_str(", #");
            if ((instr & (1 << 22)) == 0) {
                add_dec_uint32(((instr >> 16) & 0x1f) + 1);
            }
            else {
                add_dec_uint32((instr >> 16) & 0x1f);
            }
            add_str(", ");
            add_reg_name(instr & 0xf);
            if ((instr & (1 << 5)) == 0) {
                if (imm) {
                    add_str(", ");
                    add_str(instr & (1 << 6) ? "asr" : "lsl");
                    add_str(" #");
                    add_dec_uint32(imm);
                }
                else if (imm ==0 && ((instr & (1 << 6)) != 0)) {
                    add_str(", asr #");
                    add_dec_uint32(32);
                }
            }
            return;
        }

        if (((instr >> 5) & 7) == 3) {
            /* Extend */
            uint32_t rm = (instr >> 16) & 0xf;
            uint32_t rt = (instr >> 10) & 3;
            add_str("xt");
            if (rm != 0xf) add_char('a');
            switch ((instr >> 20) & 3) {
            case 0: add_str("b16"); break;
            case 1: buf_pos = 0; return;
            case 2: add_str("b"); break;
            case 3: add_str("h"); break;
            }
            add_str(cond);
            add_char(' ');
            add_reg_name((instr >> 12) & 0xf);
            add_str(", ");
            if (rm != 0xf) {
                add_reg_name(rm);
                add_str(", ");
            }
            add_reg_name(instr & 0xf);
            if (rt) {
                add_str(", ror #");
                add_dec_uint32(rt * 8);
            }
            return;

        }
        buf_pos = 0;
        return;
    }

    if ((instr & 0x0fa00070) == 0x07a00050) {
        /* Bit Field Extract */
        uint32_t lsb = (instr >> 7) & 0x1f;
        uint32_t width = ((instr >> 16) & 0x1f) + 1;
        add_char(instr & (1 << 22) ? 'u' : 's');
        add_str("bfx");
        add_str(cond);
        add_char(' ');
        add_reg_name((instr >> 12) & 0xf);
        add_str(", ");
        add_reg_name(instr & 0xf);
        add_str(", #");
        add_dec_uint32(lsb);
        add_str(", #");
        add_dec_uint32(width);
        return;
    }

    if ((instr & 0x0fe00070) == 0x07c00010) {
        /* Bit Field Clear/Insert */
        uint32_t lsb = (instr >> 7) & 0x1f;
        uint32_t msb = (instr >> 16) & 0x1f;
        uint32_t rn = instr & 0xf;
        add_str(rn == 15 ? "bfc" : "bfi");
        add_str(cond);
        add_char(' ');
        add_reg_name((instr >> 12) & 0xf);
        if (rn != 15) {
            add_str(", ");
            add_reg_name(rn);
        }
        add_str(", #");
        add_dec_uint32(lsb);
        add_str(", #");
        add_dec_uint32(msb - lsb + 1);
        return;
    }

    if ((instr & 0x0fb000d0) == 0x07000010) {
        uint32_t L = (instr >> 22) & 1;
        uint32_t ra = (instr >> 12) & 0xf;
        add_str((ra == 15 && !L) ? "smua" : "smla");
        if (L) add_char('l');
        add_char('d');
        if ((instr >> 5) & 1) add_char('x');
        add_str(cond);
        add_char(' ');
        if (L) {
            add_reg_name((instr >> 12) & 0xf);
            add_str(", ");
        }
        add_reg_name((instr >> 16) & 0xf);
        add_str(", ");
        add_reg_name(instr & 0xf);
        add_str(", ");
        add_reg_name((instr >> 8) & 0xf);
        if (!L && (ra != 15)) {
            add_str(", ");
            add_reg_name(ra);
        }
        return;
    }

    if ((instr & 0x0fb000d0) == 0x07000050) {
        uint32_t L = (instr >> 22) & 1;
        uint32_t ra = (instr >> 12) & 0xf;
        add_str((ra == 15 && !L) ? "smus" : "smls");
        if (L) add_char('l');
        add_char('d');
        if ((instr >> 5) & 1) add_char('x');
        add_str(cond);
        add_char(' ');
        if (L) {
            add_reg_name((instr >> 12) & 0xf);
            add_str(", ");
        }
        add_reg_name((instr >> 16) & 0xf);
        add_str(", ");
        add_reg_name(instr & 0xf);
        add_str(", ");
        add_reg_name((instr >> 8) & 0xf);
        if (!L && (ra != 15)) {
            add_str(", ");
            add_reg_name(ra);
        }
        return;
    }

    if ((instr & 0x0ff00010) == 0x07500010) {
        uint32_t R = (instr >> 5) & 1;
        uint32_t ra = (instr >> 12) & 0xf;
        uint32_t mode = ((instr >> 6) & 3);

        if (mode == 0) add_str((ra == 15) ? "smmul" : "smmla");
        else if (mode == 3) add_str("smmls");

        if (R) add_char('r');
        add_str(cond);
        add_char(' ');
        add_reg_name((instr >> 16) & 0xf);
        add_str(", ");
        add_reg_name(instr & 0xf);
        add_str(", ");
        add_reg_name((instr >> 8) & 0xf);
        if ((mode != 0) || (ra != 15)) {
            add_str(", ");
            add_reg_name(ra);
        }
        return;
    }

    if ((instr & 0x0ff00010) == 0x07800010) {
        uint32_t ra = (instr >> 12) & 0xf;
        add_str((ra == 15) ? "usad8" : "usada8");
        add_str(cond);
        add_char(' ');
        add_reg_name((instr >> 16) & 0xf);
        add_str(", ");
        add_reg_name(instr & 0xf);
        add_str(", ");
        add_reg_name((instr >> 8) & 0xf);
        if (ra != 15) {
            add_str(", ");
            add_reg_name((instr >> 12) & 0xf);
        }
        return;
    }

    if ((instr & 0x0fd000f0) == 0x07100010) {
        add_str(instr & (1 << 21) ? "udiv" : "sdiv");
        add_str(cond);
        add_char(' ');
        add_reg_name((instr >> 16) & 0xf);
        add_str(", ");
        add_reg_name(instr & 0xf);
        add_str(", ");
        add_reg_name((instr >> 8) & 0xf);
        return;
    }
}

static void disassemble_load_store_instr(uint32_t instr, const char * cond) {
    int B = (instr & (1 << 22)) != 0;
    int L = (instr & (1 << 20)) != 0;
    int T = (instr & 0x0d200000) == 0x04200000;
    uint32_t rd = (instr & 0x0000f000) >> 12;

    if ((instr & 0x0fff0fff) == 0x052d0004) {
        add_str("push");
        add_str(cond);
        add_str(" {");
        add_reg_name(rd);
        add_char('}');
        return;
    }
    /* POP : if BitCount(register_list) < 2 then SEE LDM / LDMIA / LDMFD; */

    if ((instr & 0x0fff0fff) == 0x049d0004) {
        add_str("pop");
        add_str(cond);
        add_str(" {");
        add_reg_name(rd);
        add_char('}');
        return;
    }

    if ((instr & 0x0ff000d0) == 0x07000010) {
        add_str("smlad");
        return;
    }

    add_str(L ? "ldr" : "str");
    if (B) add_char('b');
    if (T) add_char('t');
    add_str(cond);
    add_char(' ');
    add_reg_name(rd);
    add_str(", ");
    if (T) {
        int R = (instr & (1 << 25)) != 0;
        int U = (instr & (1 << 23)) != 0;
        add_char('[');
        add_reg_name((instr >> 16) & 0xf);
        add_str("], ");
        if (!R) {
            add_char('#');
            add_char(U ? '+' : '-');
            add_dec_uint32(instr & 0xfff);
        }
        else {
            add_char(U ? '+' : '-');
            add_shift(instr, 0);
        }
    }
    else {
        add_addressing_mode(instr);
    }
}

static void disassemble_branch_and_block_data_transfer(uint32_t addr, uint32_t instr, const char * cond) {
    int bitcount = count_bits((instr & 0x0000ffff));

    if ((instr & 0x0e000000) == 0x0a000000) { /* Branch */
        int L = (instr & 0x01000000) != 0;
        int32_t offset = instr & 0x00ffffff;
        if (offset & 0x00800000) offset |= ~0x00ffffff;
        offset = offset << 2;
        add_char('b');
        if (L) add_char('l');
        add_str(cond);
        add_char(' ');
        if (offset < 0) {
            add_char('-');
            add_dec_uint32(-offset);
        }
        else {
            add_char('+');
            add_dec_uint32(offset);
        }
        add_addr(addr + offset + 8);
        return;
    }

    if ((instr & 0x0c000000) == 0x08000000) {
        unsigned i, j;
        if ((instr & 0x0fff0000) == 0x092d0000  && (bitcount > 1)) {
            add_str("push");
            add_str(cond);
        }
        else if (((instr & 0x0fff0000) == 0x08bd0000) && (bitcount > 1)) {
            add_str("pop");
            add_str(cond);
        }
        else {
            add_str(instr & (1 << 20) ? "ldm" : "stm");
            add_auto_inc_mode(instr, 1);
            add_str(cond);
            add_char(' ');
            add_reg_name((instr & 0x000f0000) >> 16);
            if (instr & (1 << 21)) add_char('!');
            add_char(',');
        }
        add_str(" {");
        for (i = 0, j = 0; i < 16; i++) {
            if (instr & (1 << i)) {
                if (j) add_char(',');
                add_reg_name(i);
                j++;
            }
        }
        add_char('}');
        if (instr & (1 << 22)) add_char('^');
        return;
    }
}

static void disassemble_supervisor_and_ext_load_store(uint32_t instr, const char * cond) {
    if ((instr & 0x0f000000) == 0x0f000000) {
        add_str("svc");
        add_str(cond);
        add_str(" 0x");
        add_hex_uint32(instr & 0x00ffffff);
        return;
    }
}

static DisassemblyResult * disassemble_instr(ContextAddress addr, uint32_t instr) {
    uint8_t cond = 0;
    const char * cond_name = NULL;
    static DisassemblyResult dr;

    memset(&dr, 0, sizeof(dr));
    dr.size = 4;
    buf_pos = 0;

    cond = (instr >> 28) & 0xf;
    cond_name = cond_names[cond];

    if ((instr & 0x0c000000) == 0x0c000000) {
        disassemble_coprocessor_instr(instr, cond_name, cond);
    }

    if (buf_pos == 0) {
        if (cond == 15) disassemble_unconditional_instr((uint32_t)addr, instr);
        else if ((instr & 0x0c000000) == 0x00000000) disassemble_misc_instr(instr, cond_name);
        else if ((instr & 0x0e000010) == 0x06000010) disassemble_media_instr(instr, cond_name);
        else if ((instr & 0x0c000000) == 0x04000000) disassemble_load_store_instr(instr, cond_name);
        else if ((instr & 0x0c000000) == 0x08000000) disassemble_branch_and_block_data_transfer((uint32_t)addr, instr, cond_name);
        else if ((instr & 0x0c000000) == 0x0c000000) disassemble_supervisor_and_ext_load_store(instr, cond_name);
    }

    if (buf_pos == 0 && (instr & 0x0c000000) == 0x00000000) {
        disassemble_data_instr(instr, cond_name, (uint32_t)addr);
    }

    dr.text = buf;
    if (buf_pos == 0) {
        snprintf(buf, sizeof(buf), ".word 0x%08x", (unsigned)instr);
    }
    else {
        buf[buf_pos] = 0;
    }
    return &dr;
}

DisassemblyResult * disassemble_arm(uint8_t * code,
        ContextAddress addr, ContextAddress size, DisassemblerParams * params) {
    unsigned i;
    uint32_t instr = 0;

    ctx = params->ctx;
    if (size < 4) return NULL;
    for (i = 0; i < 4; i++) instr |= (uint32_t)*code++ << (i * 8);
    return disassemble_instr(addr, instr);
}

DisassemblyResult * disassemble_arm_big_endian_code(uint8_t * code,
    ContextAddress addr, ContextAddress size, DisassemblerParams * params) {
    unsigned i;
    uint32_t instr = 0;

    ctx = params->ctx;
    if (size < 4) return NULL;
    for (i = 0; i < 4; i++) instr |= (uint32_t)*code++ << ((3 - i) * 8);
    return disassemble_instr(addr, instr);
}

#endif /* SERVICE_Disassembly */
