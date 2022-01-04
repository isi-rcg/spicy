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

#include <stdio.h>
#include <assert.h>
#include <tcf/framework/context.h>
#include <tcf/services/symbols.h>
#include <machine/arm/tcf/disassembler-arm.h>

static char buf[128];
static size_t buf_pos = 0;
static int code_be = 0;
static uint16_t instr = 0;
static uint32_t instr_addr = 0;
static ContextAddress instr_size = 0;
static const char * it_cond_name = NULL;
static unsigned it_cnt = 0;
static unsigned it_pos = 0;
static unsigned it_mask = 0;
static unsigned it_cond = 0;
static DisassemblerParams * params;

static const char * shift_names[] = { "lsl", "lsr", "asr", "ror" };

static const char * cond_names[] = {
    "eq", "ne", "cs", "cc", "mi", "pl", "vs", "vc",
    "hi", "ls", "ge", "lt", "gt", "le", "", "nv"
};

static const char * reg_names[] = {
    "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
    "r8", "r9", "r10", "r11", "r12", "sp", "lr", "pc"
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
        s[i++] = '0' + n % 10;
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

#define add_reg_name(reg) add_str(reg_names[(reg) & 0xf])

static void add_modifed_immediate_constant(uint16_t suffix) {
    uint32_t rot = (suffix >> 12) & 7;
    uint32_t val = suffix & 0xff;
    uint32_t dec = 0;
    if (instr & (1 << 10)) rot |= 8;
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
    add_char('#');
    dec = val;
#if 0
    if (dec & 0x80000000) {
        add_char('-');
        dec = ~dec + 1;
    }
#endif
    add_dec_uint32(dec);
#if 0
    if (val > 0x10) {
        add_str(" ; 0x");
        add_hex_uint32(val);
    }
#endif
}

static void add_addr(uint32_t addr) {
    while (buf_pos < 16) add_char(' ');
    add_str("; addr=0x");
    add_hex_uint32(addr);
#if ENABLE_Symbols
    if (params->ctx != NULL) {
        Symbol * sym = NULL;
        char * name = NULL;
        ContextAddress sym_addr = 0;
        if (find_symbol_by_addr(params->ctx, STACK_NO_FRAME, addr, &sym) < 0) return;
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

static void add_branch_address(int32_t offset) {
    add_char(' ');
    if (offset < 0) {
        add_char('-');
        add_dec_uint32((-offset & 0xffffffff));
    }
    else {
        add_char('+');
        add_dec_uint32(offset);
    }
    add_addr(instr_addr + offset + 4);
}

static void disassemble_thumb0(void) {
    uint32_t op;
    uint32_t imm;

    if ((instr & 0xf800) == 0x1800) {
        /* Add/substruct register/immediate */
        add_str(instr & (1 << 9) ? "sub" : "add");
        add_str(it_cond_name ? it_cond_name : "s");
        add_char(' ');
        add_reg_name(instr & 7);
        add_str(", ");
        add_reg_name((instr >> 3) & 7);
        add_str(", ");
        if (instr & (1 << 10)) {
            add_char('#');
            add_dec_uint32((instr >> 6) & 7);
        }
        else {
            add_reg_name((instr >> 6) & 7);
        }
        return;
    }

    /* Shift by immediate */
    op = (instr >> 11) & 3;
    imm = (instr >> 6) & 0x1f;
    switch (op) {
    case 0: add_str(imm ? "lsl" : "mov"); break;
    case 1: add_str("lsr"); break;
    case 2: add_str("asr"); break;
    }
    add_str(it_cond_name ? it_cond_name : "s");
    add_char(' ');
    add_reg_name(instr & 7);
    add_str(", ");
    add_reg_name((instr >> 3) & 7);
    if (imm || op) {
        add_str(", #");
        if (op >= 1 && imm == 0) imm = 32;
        add_dec_uint32(imm);
    }
}

static void disassemble_thumb1(void) {
    /* Add/substruct/compare/move immediate */
    uint32_t op = (instr >> 11) & 3;
    switch(op) {
    case 0: add_str("mov"); break;
    case 1: add_str("cmp"); break;
    case 2: add_str("add"); break;
    case 3: add_str("sub"); break;
    }
    if (it_cond_name) add_str(it_cond_name);
    else if (op != 1) add_char('s');
    add_char(' ');
    add_reg_name((instr >> 8) & 7);
    add_str(", #");
    add_dec_uint32(instr & 0xff);
}

static void disassemble_thumb2(void) {
    if ((instr & 0xfc00) == 0x4000) {
        /* Data-processing register */
        uint32_t op = (instr >> 6) & 0xf;
        switch (op) {
        case  0: add_str("and"); break;
        case  1: add_str("eor"); break;
        case  2: add_str("lsl"); break;
        case  3: add_str("lsr"); break;
        case  4: add_str("asr"); break;
        case  5: add_str("adc"); break;
        case  6: add_str("sbc"); break;
        case  7: add_str("ror"); break;
        case  8: add_str("tst"); break;
        case  9: add_str("rsb"); break;
        case 10: add_str("cmp"); break;
        case 11: add_str("cmn"); break;
        case 12: add_str("orr"); break;
        case 13: add_str("mul"); break;
        case 14: add_str("bic"); break;
        case 15: add_str("mvn"); break;
        }
        if (it_cond_name) add_str(it_cond_name);
        else if (op != 8 && op != 10 && op != 11) add_char('s');
        add_char(' ');
        add_reg_name(instr & 7);
        add_str(", ");
        add_reg_name((instr >> 3) & 7);
        if (op == 9) add_str(", #0");
        return;
    }

    if ((instr & 0xff00) == 0x4700) {
        /* Branch/exchange */
        add_char('b');
        if (instr & (1 << 7)) add_char('l');
        add_char('x');
        if (it_cond_name) add_str(it_cond_name);
        add_char(' ');
        add_reg_name((instr >> 3) & 0xf);
        return;
    }

    if ((instr & 0xfc00) == 0x4400) {
        /* Special data processing */
        unsigned rd = instr & 7;
        if (instr & (1 << 7)) rd += 8;
        switch ((instr >> 8) & 3) {
        case 0: add_str("add"); break;
        case 1: add_str("cmp"); break;
        case 2: add_str("mov"); break;
        }
        if (it_cond_name) add_str(it_cond_name);
        add_char(' ');
        add_reg_name(rd);
        add_str(", ");
        add_reg_name((instr >> 3) & 0xf);
        return;
    }

    if ((instr & 0xf800) == 0x4800) {
        /* Load from literal pool */
        add_str("ldr");
        if (it_cond_name) add_str(it_cond_name);
        add_char(' ');
        add_reg_name((instr >> 8) & 7);
        add_str(", [pc, #");
        add_dec_uint32((instr & 0xff) << 2);
        add_char(']');
        return;
    }

    /* Load/store register offset */
    switch ((instr >> 9) & 7) {
    case 0: add_str("str"); break;
    case 1: add_str("strh"); break;
    case 2: add_str("strb"); break;
    case 3: add_str("ldrsb"); break;
    case 4: add_str("ldr"); break;
    case 5: add_str("ldrh"); break;
    case 6: add_str("ldrb"); break;
    case 7: add_str("ldrsh"); break;
    }
    if (it_cond_name) add_str(it_cond_name);
    add_char(' ');
    add_reg_name(instr & 7);
    add_str(", [");
    add_reg_name((instr >> 3) & 7);
    add_str(", ");
    add_reg_name((instr >> 6) & 7);
    add_char(']');
}

static void disassemble_thumb3(void) {
    /* Load/store word/byte immediate offset */
    uint32_t imm = (instr >> 6) & 0x1f;
    int B = (instr & (1 << 12)) != 0;
    if (!B) imm = imm << 2;
    add_str(instr & (1 << 11) ? "ldr" : "str");
    if (B) add_char('b');
    if (it_cond_name) add_str(it_cond_name);
    add_char(' ');
    add_reg_name(instr & 7);
    add_str(", [");
    add_reg_name((instr >> 3) & 7);
    add_str(", #");
    add_dec_uint32(imm);
    add_char(']');
}

static void disassemble_thumb4(void) {
    add_str(instr & (1 << 11) ? "ldr" : "str");

    if ((instr & 0xf000) == 0x8000) {
        /* Load/store halfword immediate offset */
        add_char('h');
        if (it_cond_name) add_str(it_cond_name);
        add_char(' ');
        add_reg_name(instr & 7);
        add_str(", [");
        add_reg_name((instr >> 3) & 7);
        add_str(", #");
        add_dec_uint32(((instr >> 6) & 0x1f) << 1);
        add_char(']');
        return;
    }

    /* Load/store to/from stack */
    if (it_cond_name) add_str(it_cond_name);
    add_char(' ');
    add_reg_name((instr >> 8) & 7);
    add_str(", [sp, #");
    add_dec_uint32((instr & 0xff) << 2);
    add_char(']');
}

static void disassemble_thumb5(void) {
    if ((instr & 0xf000) == 0xa000) {
        /* Add to SP or PC */
        add_str("add");
        if (it_cond_name) add_str(it_cond_name);
        add_char(' ');
        add_reg_name((instr >> 8) & 7);
        add_str(", ");
        add_str(instr & (1 << 11) ? "sp" : "pc");
        add_str(", #");
        add_dec_uint32((instr & 0xff) << 2);
        return;
    }

    if ((instr & 0xff00) == 0xb000) {
        /* Adjust stack pointer */
        add_str(instr & (1 << 7) ? "sub" : "add");
        if (it_cond_name) add_str(it_cond_name);
        add_str(" sp, #");
        add_dec_uint32((instr & 0x7f) << 2);
        return;
    }

    if ((instr & 0xf600) == 0xb400) {
        /* Push/pop register list */
        int cnt = 0;
        unsigned reg;
        add_str(instr & (1 << 11) ? "pop" : "push");
        if (it_cond_name) add_str(it_cond_name);
        add_str(" {");
        for (reg = 0; reg < 8; reg++) {
            if ((instr & (1 << reg)) == 0) continue;
            if (cnt > 0) add_char(',');
            add_reg_name(reg);
            cnt++;
        }
        if (instr & (1 << 8)) {
            if (cnt > 0) add_char(',');
            add_str(instr & (1 << 11) ? "pc" : "lr");
        }
        add_char('}');
        return;
    }

    if ((instr & 0xff00) == 0xbe00) {
        /* Software breakpoint */
        add_str("bkpt");
        if (it_cond_name) add_str(it_cond_name);
        add_char(' ');
        add_dec_uint32(instr & 0xff);
        return;
    }

    if ((instr & 0xffe0) == 0xb640) {
        add_str("setend");
        if (it_cond_name) add_str(it_cond_name);
        add_char(' ');
        add_str(instr & (1 << 3) ? "be" : "le");
        return;
    }

    if ((instr & 0xffe0) == 0xb660) {
        add_str("cps");
        add_str(instr & (1 << 4) ? "id" : "ie");
        if (it_cond_name) add_str(it_cond_name);
        add_char(' ');
        if (instr & (1 << 2)) add_char('a');
        if (instr & (1 << 1)) add_char('i');
        if (instr & (1 << 0)) add_char('f');
        return;
    }

    if ((instr & 0xf500) == 0xb100) {
        uint32_t offs = (instr >> 3) & 0x1f;
        if (instr & (1 << 9)) offs |= 0x20;
        offs = offs << 1;
        add_str("cb");
        if (instr & (1 << 11)) add_char('n');
        add_char('z');
        add_char(' ');
        add_reg_name(instr & 7);
        add_str(", +");
        add_dec_uint32(offs);
        add_char(' ');
        add_addr(instr_addr + offs + 4);
        return;
    }

    if ((instr & 0xff00) == 0xb200) {
        add_char(instr & (1 << 7) ? 'u' : 's');
        add_str("xt");
        add_char(instr & (1 << 6) ? 'b' : 'h');
        if (it_cond_name) add_str(it_cond_name);
        add_char(' ');
        add_reg_name(instr & 7);
        add_str(", ");
        add_reg_name((instr >> 3) & 7);
        return;
    }

    if ((instr & 0xff00) == 0xba00) {
        add_str("rev");
        switch ((instr >> 6) & 3) {
        case 1: add_str("16"); break;
        case 3: add_str("sh"); break;
        }
        if (it_cond_name) add_str(it_cond_name);
        add_char(' ');
        add_reg_name(instr & 7);
        add_str(", ");
        add_reg_name((instr >> 3) & 7);
        return;
    }

    if ((instr & 0xff00) == 0xbf00) {
        if (instr & 0x000f) {
            /* If-Then */
            it_mask = instr & 0xf;
            it_cond = (instr >> 4) & 0xf;
            add_str("it");
            it_pos = 0;
            it_cnt = 1;
            if (it_mask & 7) {
                char a = it_cond & 1 ? 't' : 'e';
                char b = it_cond & 1 ? 'e' : 't';
                add_char(it_mask & 0x8 ? a : b);
                it_cnt++;
                if (it_mask & 3) {
                    add_char(it_mask & 0x4 ? a : b);
                    it_cnt++;
                    if (it_mask & 1) {
                        add_char(it_mask & 0x2 ? a : b);
                        it_cnt++;
                    }
                }
            }
            add_char(' ');
            add_str(cond_names[it_cond]);
            return;
        }
        switch ((instr >> 4) & 0xf) {
        case 0: add_str("nop"); break;
        case 1: add_str("yield"); break;
        case 2: add_str("wfe"); break;
        case 3: add_str("wfi"); break;
        case 4: add_str("sev"); break;
        }
        if (buf_pos > 0) {
            if (it_cond_name) add_str(it_cond_name);
            return;
        }
    }
}

static void disassemble_thumb6(void) {
    if ((instr & 0xff00) == 0xdf00) {
        /* Software interrupt */
        add_str("svc");
        if (it_cond_name) add_str(it_cond_name);
        add_char(' ');
        add_dec_uint32(instr & 0xff);
        return;
    }

    if ((instr & 0xff00) == 0xde00) {
        /* Undefined instruction */
        return;
    }

    if ((instr & 0xf000) == 0xd000) {
        /* Conditional branch */
        int32_t offset = instr & 0x00ff;
        if (offset & 0x0080) offset |= ~0x00ff;
        offset = offset << 1;
        add_char('b');
        add_str(cond_names[(instr >> 8) & 0xf]);
        add_str(".n");
        add_branch_address(offset);
        return;
    }

    {
        /* Load/store multiple */
        int cnt = 0;
        uint32_t rn = (instr >> 8) & 7;
        uint32_t regs = instr & 0xff;
        unsigned reg;
        add_str(instr & (1 << 11) ? "ldmia" : "stmia");
        if (it_cond_name) add_str(it_cond_name);
        add_char(' ');
        add_reg_name(rn);
        if (((regs & (1 << rn)) == 0) ||
            ((instr & (1 << 11)) == 0)) {
            add_char('!');
        }
        add_str(", {");
        for (reg = 0; reg < 8; reg++) {
            if ((regs & (1 << reg)) == 0) continue;
            if (cnt > 0) add_str(", ");
            add_reg_name(reg);
            cnt++;
        }
        add_char('}');
    }
}

static void disassemble_load_store_32(uint16_t suffix) {
    int W = (instr & (1u << 5)) != 0;

    if ((instr & (1 << 6)) == 0) {
        /* Load/store multiple */
        unsigned i, j;
        uint32_t op = (instr >> 7) & 3;
        int L = (instr & (1 << 4)) != 0;
        if (op == 0 || op == 3) {
            if (!L) {
                add_str("srs");
                if (op == 0) add_str("db");
                if (it_cond_name) add_str(it_cond_name);
                add_char(' ');
                add_str("sp");
                if (W) add_char('!');
                add_str(", #");
                add_dec_uint32(suffix & 0x1f);
            }
            else {
                add_str("rfe");
                if (op == 0) add_str("db");
                if (it_cond_name) add_str(it_cond_name);
                add_char(' ');
                add_reg_name(instr & 0xf);
                if (W) add_char('!');
            }
            return;
        }
        if (instr == 0xe92d) {
            add_str("push");
            if (it_cond_name) add_str(it_cond_name);
            add_str(".w");
        }
        else if (instr == 0xe8bd) {
            add_str("pop");
            if (it_cond_name) add_str(it_cond_name);
            add_str(".w");
        }
        else {
            add_str(L ? "ldm" : "stm");
            add_str(instr & (1 << 8) ? "db" : "ia");
            if (it_cond_name) add_str(it_cond_name);
            if ((instr & (1 << 8)) == 0) add_str(".w");
            add_char(' ');
            add_reg_name(instr & 0xf);
            if (W) add_char('!');
            add_char(',');
        }
        add_str(" {");
        for (i = 0, j = 0; i < 16; i++) {
            if (suffix & (1 << i)) {
                if (j) add_char(',');
                add_reg_name(i);
                j++;
            }
        }
        add_char('}');
        return;
    }

    if ((instr & 0xffe0) == 0xe840) {
        /* Load/store exclusive */
        uint32_t imm = (suffix & 0xff) << 2;
        add_str(instr & (1u << 4) ? "ldrex" : "strex");
        if (it_cond_name) add_str(it_cond_name);
        add_char(' ');
        if ((instr & (1u << 4)) == 0) {
            add_reg_name((suffix >> 8) & 0xf);
            add_str(", ");
        }
        add_reg_name((suffix >> 12) & 0xf);
        add_str(", [");
        add_reg_name(instr & 0xf);
        if (imm != 0) {
            add_str(", #");
            add_dec_uint32(imm);
        }
        add_char(']');
        return;
    }

    if ((instr & 0xffe0) == 0xe8c0) {
        /* Load/store exclusive */
        char sz = 0;
        switch ((suffix >> 4) & 0xf) {
        case 4: sz = 'b'; break;
        case 5: sz = 'h'; break;
        case 7: sz = 'd'; break;
        }
        if (sz != 0) {
            add_str(instr & (1u << 4) ? "ldrex" : "strex");
            add_char(sz);
            if (it_cond_name) add_str(it_cond_name);
            add_char(' ');
            if ((instr & (1u << 4)) == 0) {
                add_reg_name(suffix & 0xf);
                add_str(", ");
            }
            add_reg_name((suffix >> 12) & 0xf);
            if (sz == 'd') {
                add_str(", ");
                add_reg_name((suffix >> 8) & 0xf);
            }
            add_str(", [");
            add_reg_name(instr & 0xf);
            add_char(']');
            return;
        }
    }

    if ((instr & 0xff60) == 0xe860 || (instr & 0xff40) == 0xe940) {
        /* Load/store register dual */
        add_str(instr & (1u << 4) ? "ldrd" : "strd");
        if (it_cond_name) add_str(it_cond_name);
        add_char(' ');
        add_reg_name((suffix >> 12) & 0xf);
        add_str(", ");
        add_reg_name((suffix >> 8) & 0xf);
        add_str(", [");
        if ((instr & 0xf) != 0xf) {
            /* Immediate */
            uint32_t imm = (suffix & 0xff) << 2;
            int P = (instr & (1u << 8)) != 0;
            int U = (instr & (1u << 7)) != 0;
            add_reg_name(instr & 0xf);
            if (P) {
                if (imm != 0 || W) {
                    add_str(", #");
                    add_char(U ? '+' : '-');
                    add_dec_uint32(imm);
                }
                add_char(']');
                if (W) add_char('!');
            }
            else {
                add_str("], #");
                add_char(U ? '+' : '-');
                add_dec_uint32(imm);
            }
        }
        else {
            /* Literal */
            uint32_t imm = (suffix & 0xff) << 2;
            int U = (instr & (1u << 7)) != 0;
            add_str("pc, #");
            add_char(U ? '+' : '-');
            add_dec_uint32(imm);
            add_char(']');
            if (W) add_char('!');
            add_addr(U ? instr_addr + imm + 4 : instr_addr - imm + 4);
        }
        return;
    }

    if ((instr & 0xfff0) == 0xe8d0 && (suffix & 0x00e0) == 0x0000) {
        /* Table Branch */
        add_str("tb");
        add_char(suffix & (1 << 4) ? 'h' : 'b');
        if (it_cond_name) add_str(it_cond_name);
        add_str(" [");
        add_reg_name(instr & 0xf);
        add_str(", ");
        add_reg_name(suffix & 0xf);
        if (suffix & (1 << 4)) {
            add_str(", lsl #1");
        }
        add_char(']');
        return;
    }
}

static void disassemble_data_processing_32(uint16_t suffix) {
    uint32_t op_code = (instr >> 5) & 0xf;
    uint32_t shift_imm = ((suffix >> 10) & 0x1c) | ((suffix >> 6) & 3);
    uint32_t shift_type = (suffix >> 4) & 3;
    int I = (instr & (1 << 9)) == 0;
    int S = (instr & (1 << 4)) != 0;
    uint32_t rn = instr & 0xf;
    uint32_t rd = (suffix >> 8) & 0xf;
    int no_rd = 0;
    int no_rn = 0;
    int no_S = 0;
    int no_shift = 0;

    switch (op_code) {
    case 0:
        add_str(((rd == 15) && S) ? "tst" : "and");
        no_S = rd == 15 && S;
        no_rd = no_S;
        break;
    case 1:
        add_str("bic");
        break;
    case 2:
        /*
         * ASR<c>.W {<Rd>,} <Rm>, <Rs>
         * is equivalent to
         * MOV{<c>}{<q>} <Rd>, <Rm>, ASR <Rs>
         * and is always the preferred disassembly.
         */
        if (!I && (shift_type != 0 || shift_imm != 0) && (rn == 15)) {
            no_shift = 1;
            if (shift_type == 3 && shift_imm == 0) {
                add_str("rrx");
            }
            else {
                add_str(shift_names[shift_type]);
            }
        }
        else {
            add_str(rn == 15 ? "mov" : "orr");
        }
        no_rn = rn == 15;
        break;
    case 3:
        add_str(rn == 15 ? "mvn" : "orn");
        no_rn = rn == 15;
        break;
    case 4:
        add_str((rd == 15 && S)? "teq" : "eor");
        no_S = rd == 15 && S;
        no_rd = no_S;
        break;
    case 6:
        add_str("pkh");
        break;
    case 8:
        if (rd == 15 && S) {
            add_str("cmn");
            S = 0;
            no_rd = 1;
        }
        else add_str("add");
        break;
    case 10:
        add_str("adc");
        break;
    case 11:
        add_str("sbc");
        break;
    case 13:
        add_str((rd == 15 && S) ? "cmp" : "sub");
        if (rd == 15 && S) no_rd = 1;
        no_S = no_rd;
        break;
    case 14:
        add_str("rsb");
        break;
    default:
        return;
    }

    if (S && !no_S) add_char('s');
    if (it_cond_name) add_str(it_cond_name);
    if ((op_code != 14) && (op_code != 6) && (op_code != 4 || (rd != 15 || !S)) &&
        (op_code != 3 || rn == 15)) add_str(".w");
    if (op_code == 6) {
        /* add bt/tb */
        if ((suffix >> 5) & 1) add_str("tb");
        else add_str("bt");
    }
    add_char(' ');

    if (!no_rd) {
        add_reg_name(rd);
        add_str(", ");
    }

    if (!no_rn) {
        add_reg_name(rn);
        add_str(", ");
    }

    if (!I) {
        uint8_t rm = (suffix & 0xf);

        add_reg_name(rm);
        if (shift_type != 0 || shift_imm != 0) {
            if (shift_type == 3 && shift_imm == 0) {
                if (!no_shift) {
                    add_str(", ");
                    add_str("rrx");
                }
            }
            else {
                add_str(", ");
                if (!no_shift) {
                    add_str(shift_names[shift_type]);
                    add_char(' ');
                }
                add_char('#');
                if (shift_type >= 1 && shift_imm == 0) shift_imm = 32;
                add_dec_uint32(shift_imm);
            }
        }
    }
    else {
        add_modifed_immediate_constant(suffix);
    }
}

static void disassemble_data_processing_pbi_32(uint16_t suffix) {
    uint32_t op_code = (instr >> 4) & 0x1f;
    uint32_t rn = instr & 0xf;
    uint32_t imm = suffix & 0xff;
    int sat_inst = 0;

    imm |= (suffix & 0x7000) >> 4;
    if (instr & (1 << 10)) imm |= 0x800;

    switch (op_code) {
    case 0:
        add_str("addw");
        break;
    case 4:
        add_str("movw");
        break;
    case 10:
        add_str("subw");
        break;
    case 12:
        add_str("movt");
        break;
    case 16:
        add_str("ssat");
        sat_inst = 1;
        break;
    case 18:
        add_str(suffix & 0x70c0 ? "ssat" : "ssat16");
        sat_inst = 1;
        break;
    case 20:
        add_str("sbfx");
        break;
    case 22:
        add_str(rn == 15 ? "bfc" : "bfi");
        break;
    case 24:
        add_str("usat");
        sat_inst = 1;
        break;
    case 26:
        add_str(suffix & 0x70c0 ? "usat" : "usat16");
        sat_inst = 1;
        break;
    case 28:
        add_str("ubfx");
        break;
    default:
        buf_pos = 0;
        return;
    }

    if (it_cond_name) add_str(it_cond_name);
    add_char(' ');
    add_reg_name((suffix >> 8) & 0xf);

    if (sat_inst) {
        /* SSAT, SSAT16, USAT, USAT16 */
        uint32_t sat_imm = suffix & 0x1f;
        add_str(", #");
        if (op_code == 16 || op_code == 18) add_dec_uint32(sat_imm + 1);
        else  add_dec_uint32(sat_imm);
    }

    if (op_code == 4) {
        imm |= rn << 12;
    }
    else if (op_code == 22 && rn == 15) {
        /* Nothing */
    }
    else if (op_code == 12) {
        imm = ((instr & 0xf) << 12) + (((instr >> 10) & 1) << 11) +
              (((suffix >> 12) & 0x7) << 8) + (suffix & 0xff);
    }
    else {
        add_str(", ");
        add_reg_name(rn);
    }
    if (!sat_inst) add_str(", #");
    if (op_code == 22) {
        imm = (suffix >> 12) & 7;
        imm = (imm << 2) | ((suffix >> 6) & 3);
        add_dec_uint32(imm);
        add_str(", #");
        add_dec_uint32((suffix & 0x1f) + 1 - imm);
    }
    else if (op_code == 20 || op_code == 28) {
        imm = (suffix >> 12) & 7;
        imm = (imm << 2) | ((suffix >> 6) & 3);
        add_dec_uint32(imm);
        add_str(", #");
        add_dec_uint32((suffix & 0x1f) + 1);
    }
    else if (sat_inst) {
        /* SSAT, SSAT16, USAT, USAT16 */
        uint32_t sh = (instr >> 5) & 1;
        uint32_t imm3 = (suffix >> 12) & 7;
        uint32_t imm2 = (suffix >> 6) & 3;
        if (sh == 1 && imm2 == 0 && imm3 == 0) {
            /* SSAT16, USAT16 : nothing to add. */
            return;
        }
        else if (sh == 1) {
            add_str(", asr #");
        }
        else {
            add_str(", lsl #");
        }
        add_dec_uint32((imm3 << 2) + imm2);
    }
    else {
        add_dec_uint32(imm);
    }
}

static void disassemble_branches_and_misc_32(uint16_t suffix) {
    if ((suffix & 0xd000) == 0x8000) {
        uint16_t op = (instr >> 4) & 0x7f;
        if ((op & 0x38) != 0x38) {
            /* Conditional branch */
            int J1 = (suffix & (1 << 13)) != 0;
            int J2 = (suffix & (1 << 11)) != 0;
            int S = (instr & (1 << 10)) != 0;
            int32_t offset = suffix & 0x7ff;
            offset |= (instr & 0x3f) << 11;
            if (J1) offset |= 1 << 17;
            if (J2) offset |= 1 << 18;
            if (S) offset |= 0xfff80000;
            offset = offset << 1;
            add_char('b');
            add_str(cond_names[(instr >> 6) & 0xf]);
            add_str(".w");
            add_branch_address(offset);
            return;
        }
        if ((op & 0x7e) == 0x38) {
            /* Move to Special Register */
            int R = 0;
            uint32_t mask = (suffix >> 8) & 0xf;
            if ((suffix & 0x0300) != 0x0000) {
                R = (instr & (1 << 4)) != 0;
            }
            if (mask) {
                add_str("msr");
                if (it_cond_name) add_str(it_cond_name);
                add_char(' ');
                add_str(R ? "spsr" : "cpsr");
                add_char('_');
                if (mask & 8) add_char('f');
                if (mask & 4) add_char('s');
                if (mask & 2) add_char('x');
                if (mask & 1) add_char('c');
                add_str(", ");
                add_reg_name(instr & 0xf);
                return;
            }
            return;
        }
        if (op == 0x3a) {
            /* Change Processor State, and hints */
            uint32_t imod = (suffix >> 9) & 3;
            int M = (suffix & (1 << 8)) != 0;
            /* hint instructions if imod == 00 && M == 0 */
            if (imod || M) {
                uint32_t mode = suffix & 0x1f;
                add_str("cps");
                if (imod >= 2) add_str(imod == 2 ? "ie" : "id");
                if (imod >= 2) {
                    add_str(M ? " " : ".w ");
                    if (suffix & (1 << 7)) add_char('a');
                    if (suffix & (1 << 6)) add_char('i');
                    if (suffix & (1 << 5)) add_char('f');
                    if (((suffix >> 5) & 0x7) != 0 && M) add_str(", ");
                }
                else add_char(' ');
                if (M) {
                    add_char('#');
                    add_dec_uint32(mode);
                }
                return;
            }
            switch (suffix & 0xff) {
            case 0: add_str("nop"); break;
            case 1: add_str("yield"); break;
            case 2: add_str("wfe"); break;
            case 3: add_str("wfi"); break;
            case 4: add_str("sev"); break;
            }
            if (buf_pos > 0) {
                if (it_cond_name) add_str(it_cond_name);
                add_str(".w");
                return;
            }
            if ((suffix & 0x00f0) == 0x00f0) {
                add_str("dbg");
                if (it_cond_name) add_str(it_cond_name);
                add_str(" #");
                add_dec_uint32(suffix & 0xf);
                return;
            }
            return;
        }
        if (op == 0x3b) {
            /* Miscellaneous control instructions */
            uint32_t op2 = (suffix >> 4) & 0xf;
            switch (op2) {
            case 0: add_str("leavex"); break;
            case 1: add_str("enterx"); break;
            case 2: add_str("clrex"); break;
            case 4: add_str("dsb"); break;
            case 5: add_str("dmb"); break;
            case 6: add_str("isb"); break;
            }
            if (op2 >= 4 && op2 < 6) {
                if (it_cond_name) add_str(it_cond_name);
                add_char(' ');
                switch (suffix & 0xf) {
                case 15: add_str("sy"); break;
                case 14: add_str("st"); break;
                case 13: add_str("ld"); break;
                case 11: add_str("ish"); break;
                case 10: add_str("ishst"); break;
                case 9: add_str("ishld"); break;
                case 7: add_str("nsh"); break;
                case 6: add_str("nshst"); break;
                case 5: add_str("nshld"); break;
                case 3: add_str("osh"); break;
                case 2: add_str("oshst"); break;
                case 1: add_str("oshld"); break;
                default:
                    add_str(" #");
                    add_dec_uint32(suffix & 0xf);
                }
                return;
            }
            if (op2 == 6) {
                if (it_cond_name) add_str(it_cond_name);
                add_char(' ');
                switch (suffix & 0xf) {
                case 15: add_str("sy"); break;
                default:
                    add_str(" #");
                    add_dec_uint32(suffix & 0xf);
                }
                return;
            }
            return;
        }
        if (op == 0x3c) {
            /* Branch and Exchange Jazelle */
            add_str("bxj ");
            add_reg_name(instr & 0x0f);
            return;
        }
        if (op == 0x3d) {
            /* Exception Return */
            add_str("subs");
            if (it_cond_name) add_str(it_cond_name);
            add_char(' ');
            add_reg_name((suffix >> 8) & 0xf);
            add_str(", ");
            add_reg_name(instr & 0xf);
            add_str(", #");
            add_dec_uint32(suffix & 0xff);
            return;
        }
        if ((op & 0x7e) == 0x3e) {
            /* Move from Special Register */
            add_str("mrs");
            if (it_cond_name) add_str(it_cond_name);
            add_char(' ');
            add_reg_name((suffix >> 8) & 0xf);
            add_str(", ");
            add_str(instr & (1 << 4) ? "spsr" : "cpsr");
            return;
        }
        if (op == 0x7f) {
            if (suffix & 0x2000) {
                /* Permanently UNDEFINED.
                 * This space will not be allocated in future */
                return;
            }
            /* Secure Monitor Call */
            return;
        }
        /* Undefined */
        return;
    }

    {
        /* B/BL/BLX */
        int w = 0;
        int J1 = (suffix & (1 << 13)) != 0;
        int J2 = (suffix & (1 << 11)) != 0;
        int S = (instr & (1 << 10)) != 0;
        int32_t offset = suffix & 0x7ff;
        offset |= (instr & 0x3ff) << 11;
        if (S == J2) offset |= 1 << 21;
        if (S == J1) offset |= 1 << 22;
        if (S) offset |= 0xff800000;
        offset = offset << 1;
        if ((suffix & 0xd000) == 0x9000) {
            add_str("b");
            w = 1;
        }
        else if ((suffix & 0xd000) == 0xc000) {
            add_str("blx");
        }
        else {
            add_str("bl");
        }
        if (it_cond_name) add_str(it_cond_name);
        if (w) add_str(".w");
        add_branch_address(offset);
        return;
    }
}

static void disassemble_memory_hints(uint16_t suffix) {
    if ((instr & 0xfe50) == 0xf810) {
        int U = instr & (1 << 7);
        int W = instr & (1 << 5);
        uint32_t rn = instr & 0xf;
        uint32_t imm = suffix & 0xfff;
        add_str(((instr >>8) & 1) ? "pli" : "pld");
        if (W) add_char('w');
        add_str(" [");
        add_reg_name(rn);
        if (!U && rn != 15) {
            if ((suffix & 0xff00) != 0xfc00) {
                if ((suffix & 0xffc0) == 0xf000) {
                    uint32_t imm2 = (suffix >> 4) & 3;
                    add_str(", ");
                    add_reg_name(suffix & 0xf);
                    if (imm2) {
                        add_str(", lsl #");
                        add_dec_uint32(imm2);
                    }
                    add_char(']');
                    return;
                }
                buf_pos = 0;
                return;
            }
            imm &= 0xff;
        }
        if (imm != 0 || !U) {
            add_str(", #");
            if (!U) add_char('-');
            else add_char('+');
            add_dec_uint32(imm);
        }
        add_char(']');
        return;
    }
}

static void disassemble_data_processing_32_reg(uint16_t suffix) {
    uint32_t op1 = (instr >> 4) & 0xf;
    uint32_t op2 = (suffix >> 4) & 0xf;

    if (op2 == 0) {
        switch (op1 >> 1) {
        case 0: add_str("lsl"); break;
        case 1: add_str("lsr"); break;
        case 2: add_str("asr"); break;
        case 3: add_str("ror"); break;
        }
        if (buf_pos > 0) {
            if (instr & (1u << 4)) add_char('s');
            if (it_cond_name) add_str(it_cond_name);
            add_str(".w ");
            add_reg_name((suffix >> 8) & 0xf);
            add_str(", ");
            add_reg_name(instr & 0xf);
            add_str(", ");
            add_reg_name(suffix & 0xf);
            return;
        }
    }

    if (op2 >= 8) {
        int pc = (instr & 0xf) == 0xf;
        switch (op1) {
        case 0: add_str(pc ? "sxth" : "sxtah"); break;
        case 1: add_str(pc ? "uxth" : "uxtah"); break;
        case 2: add_str(pc ? "sxtb16" : "sxtab16"); break;
        case 3: add_str(pc ? "uxtb16" : "uxtab16"); break;
        case 4: add_str(pc ? "sxtb" : "sxtab"); break;
        case 5: add_str(pc ? "uxtb" : "uxtab"); break;
        }
        if (buf_pos > 0) {
            uint32_t rot = (suffix >> 4) & 3;
            if (it_cond_name) add_str(it_cond_name);
            if (pc) {
                switch (op1) {
                case 0:
                case 1:
                case 4:
                case 5:
                    add_str(".w");
                    break;
                }
            }
            add_char(' ');
            add_reg_name((suffix >> 8) & 0xf);
            if (!pc) {
                add_str(", ");
                add_reg_name(instr & 0xf);
            }
            add_str(", ");
            add_reg_name(suffix & 0xf);
            switch (rot) {
            case 1: add_str(", ror #8"); break;
            case 2: add_str(", ror #16"); break;
            case 3: add_str(", ror #24"); break;
            }
            return;
        }
    }

    if (op1 >= 8 && (op1 & 3) != 3 && op2 < 8 && (op2 & 3) != 3) {
        /* Parallel addition and subtraction */
        op1 &= 7;
        op2 &= 3;
        if (suffix & (1u << 6)) add_char('u');
        else if (op2 != 1) add_char('s');
        if (op2 == 1) add_char('q');
        if (op2 == 2) add_char('h');
        switch (op1) {
        case 1: add_str("add16"); break;
        case 2: add_str("asx"); break;
        case 6: add_str("sax"); break;
        case 5: add_str("sub16"); break;
        case 0: add_str("add8"); break;
        case 4: add_str("sub8"); break;
        }
        if (it_cond_name) add_str(it_cond_name);
        add_str(" ");
        add_reg_name((suffix >> 8) & 0xf);
        add_str(", ");
        add_reg_name(instr & 0xf);
        add_str(", ");
        add_reg_name(suffix & 0xf);
        return;
    }

    if ((op1 >> 2) == 2 && (op2 >> 2) == 2) {
        /* Miscellaneous operations */
        op1 &= 3;
        op2 &= 3;
        if (op1 == 0) {
            switch (op2) {
            case 0: add_str("qadd"); break;
            case 1: add_str("qdadd"); break;
            case 2: add_str("qsub"); break;
            case 3: add_str("qdsub"); break;
            }
            if (it_cond_name) add_str(it_cond_name);
            add_str(" ");
            add_reg_name((suffix >> 8) & 0xf);
            add_str(", ");
            add_reg_name(suffix & 0xf);
            add_str(", ");
            add_reg_name(instr & 0xf);
            return;
        }
        if (op1 == 1) {
            switch (op2) {
            case 0: add_str("rev"); break;
            case 1: add_str("rev16"); break;
            case 2: add_str("rbit"); break;
            case 3: add_str("revsh"); break;
            }
            if (it_cond_name) add_str(it_cond_name);
            if (op2 != 2) {
                add_str(".w ");
            }
            else {
                add_str(" ");
            }
            add_reg_name((suffix >> 8) & 0xf);
            add_str(", ");
            add_reg_name(suffix & 0xf);
            return;
        }
        if (op1 == 2 && op2 == 0) {
            add_str("sel");
            if (it_cond_name) add_str(it_cond_name);
            add_str(" ");
            add_reg_name((suffix >> 8) & 0xf);
            add_str(", ");
            add_reg_name(instr & 0xf);
            add_str(", ");
            add_reg_name(suffix & 0xf);
            return;
        }
        if (op1 == 3 && op2 == 0) {
            add_str("clz");
            if (it_cond_name) add_str(it_cond_name);
            add_str(" ");
            add_reg_name((suffix >> 8) & 0xf);
            add_str(", ");
            add_reg_name(suffix & 0xf);
            return;
        }
        return;
    }
}

static void disassemble_multiply_32(uint16_t suffix) {
    /* Multiply, multiply accumulate, and absolute difference */
    uint32_t op1 = (instr >> 4) & 7;
    uint32_t ra = (suffix >> 12) & 0xf;
    uint32_t op2 = (suffix >> 4) & 3;
    int no_ra = 0;
    int no_size = 0;
    switch (op1) {
    case 0:
        if (op2 == 0) {
            if (ra != 15) add_str("mla");
            else add_str("mul");
            no_ra = ra == 15;
        }
        else if (op2 == 1) {
            add_str("mls");
        }
        break;
    case 1:
        if (ra != 15) add_str("smla");
        else add_str("smul");
        no_ra = ra == 15;
        no_size = ra == 15;
        add_char(suffix & (1 << 5) ? 't' : 'b');
        add_char(suffix & (1 << 4) ? 't' : 'b');
        break;
    case 2:
        if (op2 < 2) {
            if (ra != 15) add_str("smlad");
            else add_str("smuad");
            no_ra = ra == 15;
            no_size = 1;
            if (suffix & (1 << 4)) add_char('x');
        }
        break;
    case 3:
        if (op2 < 2) {
            if (ra != 15) add_str("smlaw");
            else add_str("smulw");
            no_ra = ra == 15;
            no_size = 1;
            add_char(suffix & (1 << 4) ? 't' : 'b');
        }
        break;
    case 4:
        if (op2 < 2) {
            if (ra != 15) add_str("smlsd");
            else add_str("smusd");
            no_ra = ra == 15;
            no_size = 1;
            if (suffix & (1 << 4)) add_char('x');
        }
        break;
    case 5:
        if (op2 < 2) {
            if (ra != 15) add_str("smmla");
            else add_str("smmul");
            no_ra = ra == 15;
            no_size = 1;
            if (suffix & (1 << 4)) add_char('r');
        }
        break;
    case 6:
        if (op2 < 2) {
            add_str("smmls");
            if (suffix & (1 << 4)) add_char('r');
            no_size = 1;
        }
        break;
    case 7:
        if (op2 == 0) {
            if (ra != 15) add_str("usada8");
            else add_str("usad8");
            no_ra = ra == 15;
            no_size = no_ra;
        }
        break;
    }
    if (buf_pos > 0) {
        uint32_t rn = (instr >> 0) & 0xf;
        uint32_t rd = (suffix >> 8) & 0xf;
        uint32_t rm = (suffix >> 0) & 0xf;
        if (it_cond_name) add_str(it_cond_name);
        if (op2 == 0 && ra == 15 && no_size == 0) add_str(".w");
        add_char(' ');
        add_reg_name(rd);
        add_str(", ");
        add_reg_name(rn);
        add_str(", ");
        add_reg_name(rm);
        if (!no_ra) {
            add_str(", ");
            add_reg_name(ra);
        }
        return;
    }
}

static void disassemble_long_multiply_32(uint16_t suffix) {
    /* Long multiply, long multiply accumulate, and divide */
    uint32_t op1 = (instr >> 4) & 7;
    uint32_t op2 = (suffix >> 4) & 0xf;
    switch (op1) {
    case 0:
        if (op2 == 0) add_str("smull");
        break;
    case 1:
        if (op2 == 15) add_str("sdiv");
        break;
    case 2:
        if (op2 == 0) add_str("umull");
        break;
    case 3:
        if (op2 == 15) add_str("udiv");
        break;
    case 4:
        if (op2 == 0) {
            add_str("smlal");
        }
        else if ((op2 >> 2) == 2) {
            add_str("smlal");
            add_char(suffix & (1 << 5) ? 't' : 'b');
            add_char(suffix & (1 << 4) ? 't' : 'b');
        }
        else if ((op2 >> 1) == 6) {
            add_str("smlald");
            if (suffix & (1 << 4)) add_char('x');
        }
        break;
    case 5:
        if ((op2 >> 1) == 6) {
            add_str("smlsld");
            if (suffix & (1 << 4)) add_char('x');
        }
        break;
    case 6:
        if (op2 == 0) add_str("umlal");
        else if (op2 == 6) add_str("umaal");
        break;
    }
    if (buf_pos > 0) {
        if (it_cond_name) add_str(it_cond_name);
        switch (op1) {
        case 0:
        case 2:
        case 4:
        case 5:
        case 6:
            add_char(' ');
            add_reg_name((suffix >> 12) & 0xf);
            add_str(", ");
            add_reg_name((suffix >> 8) & 0xf);
            add_str(", ");
            add_reg_name(instr & 0xf);
            add_str(", ");
            add_reg_name(suffix & 0xf);
            return;
        case 1:
        case 3:
            add_char(' ');
            add_reg_name((suffix >> 8) & 0xf);
            add_str(", ");
            add_reg_name(instr & 0xf);
            add_str(", ");
            add_reg_name(suffix & 0xf);
            return;
        }
    }
}

static void disassemble_thumb7(uint8_t * code) {
    unsigned i;
    uint16_t suffix = 0;

    if ((instr & 0xf800) == 0xe000) {
        /* Unconditional branch */
        int32_t offset = instr & 0x07ff;
        if (offset & 0x0400) offset |= ~0x07ff;
        offset = offset << 1;
        add_char('b');
        if (it_cond_name) add_str(it_cond_name);
        add_str(".n");
        add_branch_address(offset);
        return;
    }

    instr_size = 4;
    for (i = 0; i < 2; i++) suffix |= (uint16_t)*code++ << ((code_be ? 1 - i : i) * 8);

    if ((instr & 0xfe00) == 0xe800) {
        disassemble_load_store_32(suffix);
        return;
    }

    if ((instr & 0xfe00) == 0xea00) {
        disassemble_data_processing_32(suffix);
        return;
    }

    if ((instr & 0xf800) == 0xf000) {
        if ((suffix & 0x8000) == 0) {
            if (instr & (1 << 9)) {
                disassemble_data_processing_pbi_32(suffix);
            }
            else {
                disassemble_data_processing_32(suffix);
            }
        }
        else {
            disassemble_branches_and_misc_32(suffix);
        }
        return;
    }

    if ((instr & 0xfe00) == 0xf800) {
        uint32_t rn = (instr & 0xf);
        int T = 0;
        /*
         * PLD = 0xf890f000
         *       0xf810fc00
         *       0xf81ff000
         *       0xf810f000
         * PLI = 0xf990f000
         *       0xf910fc00
         *       0xf91ff000
         *       0xf910f000
         */
        if ((((instr & 0xffd0) == 0xf890) && ((suffix & 0xf000) == 0xf000)) ||
            (((instr & 0xffd0) == 0xf810) && ((suffix & 0xff00) == 0xfc00)) ||
            (((instr & 0xff7f) == 0xf81f) && ((suffix & 0xf000) == 0xf000)) ||
            (((instr & 0xffd0) == 0xf810) && ((suffix & 0xffc0) == 0xf000)) ||
            (((instr & 0xfff0) == 0xf990) && ((suffix & 0xf000) == 0xf000)) ||
            (((instr & 0xfff0) == 0xf910) && ((suffix & 0xff00) == 0xfc00)) ||
            (((instr & 0xff7f) == 0xf91f) && ((suffix & 0xf000) == 0xf000)) ||
            (((instr & 0xfff0) == 0xf910) && ((suffix & 0xffc0) == 0xf000))) {
            /* Memory hints (PLI/PLD) */
            disassemble_memory_hints(suffix);
            return;
        }

        if (((instr & 0xf84d) == 0xf84d) &&
            (((suffix & 0x0bff) == 0x0b04) || ((suffix & 0x0dff) == 0x0d04))) {
            if ((suffix & 0x0bff) == 0x0b04) add_str("pop");
            else add_str("push");
            if (it_cond_name) add_str(it_cond_name);
            add_str(".w {");
            add_reg_name((suffix >> 12) & 0xf);
            add_char('}');
            return;
        }

        /* Load/Store single data item */

        T = (suffix & 0x0f00) == 0x0e00 && (instr & (1 << 7)) == 0;
        add_str(instr & (1 << 4) ? "ldr" : "str");
        if ((instr &  (1 << 6)) == 0) {
            if (instr & (1 << 8)) add_char('s');
            add_char(instr & (1 << 5) ? 'h' : 'b');
        }
        if (T && rn != 15) add_char('t');
        if (it_cond_name) add_str(it_cond_name);
        if (!T || rn == 15) add_str(".w");
        add_char(' ');
        add_reg_name((suffix >> 12) & 0xf);
        add_str(", [");
        add_reg_name(instr & 0xf);
        if ((instr & 0xf) == 15) {
            add_str(", #");
            add_char(instr & (1 << 7) ? '+' : '-');
            add_dec_uint32(suffix & 0xfff);
            add_char(']');
        }
        else if (instr & (1 << 7)) {
            uint32_t imm = suffix & 0xfff;
            if (imm) {
                add_str(", #");
                add_dec_uint32(imm);
            }
            add_char(']');
        }
        else if ((suffix & (1 << 11)) == 0) {
            uint32_t imm = (suffix >> 4) & 3;
            add_str(", ");
            add_reg_name(suffix & 0xf);
            if (imm) {
                add_str(", lsl #");
                add_dec_uint32(imm);
            }
            add_char(']');
        }
        else if ((suffix & (1 << 8)) == 0) {
            uint32_t imm = suffix & 0xff;
            if (imm) {
                add_str(", #");
                add_char(suffix & (1 << 9) ? '+' : '-');
                add_dec_uint32(imm);
            }
            add_char(']');
        }
        else if (suffix & (1 << 10)) {
            uint32_t imm = suffix & 0xff;
            if (imm) {
                add_str(", #");
                add_char(suffix & (1 << 9) ? '+' : '-');
                add_dec_uint32(imm);
            }
            add_str("]!");
        }
        else {
            uint32_t imm = suffix & 0xff;
            add_str("], #");
            add_char(suffix & (1 << 9) ? '+' : '-');
            add_dec_uint32(imm);
        }
        return;
    }

    if ((instr & 0xfe00) == 0xfa00) {
        if ((instr & (1 << 8)) == 0) {
            /* Data-processing (register) */
            disassemble_data_processing_32_reg(suffix);
        }
        else if ((instr & (1 << 7)) == 0) {
            /* Multiply, multiply accumulate, and absolute difference */
            disassemble_multiply_32(suffix);
        }
        else {
            /* Long multiply, long multiply accumulate, and divide */
            disassemble_long_multiply_32(suffix);
        }
        return;
    }
}

static DisassemblyResult * disassemble_arm_ti(uint8_t * code, ContextAddress addr, ContextAddress size) {
    const char * p;
    DisassemblyResult * dr = disassemble_arm(code, addr, size, params);
    if (dr == NULL || dr->text == NULL || it_cond_name == NULL) return dr;
    p = dr->text;
    while (*p && *p != ' ') buf[buf_pos++] = *p++;
    add_str(it_cond_name);
    while (*p == ' ') p++;
    buf[buf_pos++] = ' ';
    while (buf_pos < 8) buf[buf_pos++] = ' ';
    while (*p) buf[buf_pos++] = *p++;
    buf[buf_pos] = 0;
    dr->text = buf;
    return dr;
}

static DisassemblyResult * disassemble_thumb_any(uint8_t * code,
        ContextAddress addr, ContextAddress size, DisassemblerParams * prm) {
    unsigned i;
    static DisassemblyResult dr;

    if (size < 2) return NULL;

    instr = 0;
    for (i = 0; i < 2; i++) instr |= (uint16_t)code[i] << ((code_be ? 1 - i : i) * 8);
    memset(&dr, 0, sizeof(dr));
    params = prm;

    instr_size = 2;
    instr_addr = (uint32_t)addr;
    buf_pos = 0;

    it_cond_name = NULL;
    if (it_cnt > 0) {
        if (it_pos == 0) {
            it_cond_name = cond_names[it_cond];
        }
        else if (it_mask & (1 << (4 - it_pos))) {
            it_cond_name = cond_names[it_cond | 1u];
        }
        else {
            it_cond_name = cond_names[it_cond & ~1u];
        }
        it_pos++;
        it_cnt--;
    }

    if ((instr & 0xec00) == 0xec00 && size >= 4) {
        /* Coprocessor instructions - same as ARM encoding */
        uint8_t tmp[4];
        if (code_be) {
            tmp[2] = code[1];
            tmp[3] = code[0];
            tmp[0] = code[3];
            tmp[1] = code[2];
        }
        else {
            tmp[2] = code[0];
            tmp[3] = code[1];
            tmp[0] = code[2];
            tmp[1] = code[3];
        }
        if ((instr & 0xef00) == 0xef00) {
            /* Advanced SIMD data-processing instructions */
            tmp[3] = 0xf2 | ((instr >> 12) & 1);
        }
        return disassemble_arm_ti(tmp, addr, 4);
    }

    if ((instr & 0xff10) == 0xf900 && size >= 4) {
        /* Advanced SIMD element or structure load/store instructions */
        uint8_t tmp[4];
        if (code_be) {
            tmp[2] = code[1];
            tmp[3] = 0xf4;
            tmp[0] = code[3];
            tmp[1] = code[2];
        }
        else {
            tmp[2] = code[0];
            tmp[3] = 0xf4;
            tmp[0] = code[2];
            tmp[1] = code[3];
        }
        return disassemble_arm_ti(tmp, addr, 4);
    }

    switch ((instr >> 13) & 7) {
    case 0: disassemble_thumb0(); break;
    case 1: disassemble_thumb1(); break;
    case 2: disassemble_thumb2(); break;
    case 3: disassemble_thumb3(); break;
    case 4: disassemble_thumb4(); break;
    case 5: disassemble_thumb5(); break;
    case 6: disassemble_thumb6(); break;
    case 7: disassemble_thumb7(code + 2); break;
    }

    dr.text = buf;
    dr.size = instr_size;
    dr.incomplete = it_cnt > 0;
    if (buf_pos == 0) {
        if (dr.size == 2) {
            snprintf(buf, sizeof(buf), ".half 0x%04x", instr);
        }
        else if (dr.size == 4) {
            uint16_t suffix = 0;
            for (i = 0; i < 2; i++) suffix |= (uint16_t)code[i + 2] << ((code_be ? 1 - i : i) * 8);
            snprintf(buf, sizeof(buf), ".word 0x%04x%04x", instr, suffix);
        }
        else {
            return NULL;
        }
    }
    else {
        buf[buf_pos] = 0;
    }
    return &dr;
}

DisassemblyResult * disassemble_thumb(uint8_t * code,
        ContextAddress addr, ContextAddress size, DisassemblerParams * prm) {
    code_be = 0;
    return disassemble_thumb_any(code, addr, size, prm);
}

DisassemblyResult * disassemble_thumb_big_endian_code(uint8_t * code,
        ContextAddress addr, ContextAddress size, DisassemblerParams * prm) {
    code_be = 1;
    return disassemble_thumb_any(code, addr, size, prm);
}

#endif /* SERVICE_Disassembly */
