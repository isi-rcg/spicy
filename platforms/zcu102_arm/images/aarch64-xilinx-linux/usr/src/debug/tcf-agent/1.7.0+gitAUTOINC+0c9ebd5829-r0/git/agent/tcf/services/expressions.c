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
 * Expression evaluation service.
 *
 * Extensions to regular C/C++ syntax:
 * 1. Special characters in identifiers: $"X", or just "X" if followed by ::
 *    where X is object name that can contain any characters.
 * 2. Symbol IDs in expressions: ${X}
 *    where X is symbol ID as returned by symbols service.
 * 3. CPU registers: $X
 *    where X is a register name, e.g. $ax
 */

#include <tcf/config.h>

#if !defined(ENABLE_Expressions)
#  define ENABLE_Expressions        (SERVICE_Expressions)
#endif

#if ENABLE_Expressions

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/exceptions.h>
#include <tcf/framework/json.h>
#include <tcf/framework/cache.h>
#include <tcf/framework/trace.h>
#include <tcf/services/runctrl.h>
#include <tcf/services/funccall.h>
#include <tcf/services/stacktrace.h>
#include <tcf/services/memoryservice.h>
#include <tcf/services/breakpoints.h>
#include <tcf/services/registers.h>
#include <tcf/services/expressions.h>
#include <tcf/main/test.h>

#define SY_LEQ   256
#define SY_GEQ   257
#define SY_EQU   258
#define SY_NEQ   259
#define SY_AND   260
#define SY_OR    261
#define SY_SHL   262
#define SY_SHR   263
#define SY_VAL   264
#define SY_ID    265
#define SY_REF   266
#define SY_DEC   267
#define SY_INC   268
#define SY_A_SUB 269
#define SY_A_ADD 270
#define SY_A_SHL 271
#define SY_A_SHR 272
#define SY_A_OR  273
#define SY_A_XOR 274
#define SY_A_AND 275
#define SY_A_MUL 276
#define SY_A_DIV 277
#define SY_A_MOD 278
#define SY_SIZEOF 279
#define SY_NAME  280
#define SY_SCOPE 281
#define SY_PM_D  282
#define SY_PM_R  283

#define MODE_NORMAL EXPRESSION_MODE_NORMAL
#define MODE_TYPE   EXPRESSION_MODE_TYPE
#define MODE_SKIP   EXPRESSION_MODE_SKIP

#define VAL_FLAG_X 0x01
#define VAL_FLAG_U 0x02
#define VAL_FLAG_L 0x04
#define VAL_FLAG_F 0x08
#define VAL_FLAG_C 0x10
#define VAL_FLAG_S 0x20

#define SYM_FLAG_TYPE 0xf0000000

static char * text = NULL;
static int text_pos = 0;
static int text_len = 0;
static int text_ch = 0;
static int text_sy = 0;
static int sy_pos = 0;
static Value text_val;
static int text_val_flags = 0;

/* Host endianness */
static int big_endian = 0;

static Context * expression_context = NULL;
static int expression_frame = STACK_NO_FRAME;
static ContextAddress expression_addr = 0;
static int expression_has_func_call = 0;

#define bit_mask(v, n) (1u << ((v)->big_endian ? 7 - (n) % 8 : (n) % 8))

#ifndef ENABLE_FuncCallInjection
#  define ENABLE_FuncCallInjection (ENABLE_Symbols && SERVICE_RunControl && SERVICE_Breakpoints && ENABLE_DebugContext)
#endif

#if ENABLE_FuncCallInjection
typedef struct FuncCallState {
    LINK link_all;
    unsigned id;            /* ACPM transaction ID */
    int pos;                /* Text position in the expression */
    int started;            /* Target has started to execute the function call */
    int intercepted;        /* Intercepted during or after the function call */
    int committed;          /* ACPM transaction finished */
    int finished;           /* Target has finished the function call */
    Context * ctx;
    uint64_t ret_addr;
    ContextAddress stk_addr;
    ContextAddress func_addr;
    AbstractCache cache;
    BreakpointInfo * bp;
    ErrorReport * error;

    /* Actual arguments */
    Value * args;
    unsigned args_cnt;
    unsigned args_max;

    /* Returned value */
    void * ret_value;
    size_t ret_size;
    int ret_big_endian;

    /* After call commands */
    LocationExpressionCommand * cmds;
    unsigned cmds_cnt;

    /* Saved registers */
    RegisterDefinition ** regs;
    unsigned regs_cnt;
    uint8_t * regs_data;
} FuncCallState;

static LINK func_call_state = TCF_LIST_INIT(func_call_state);

#define link_all2fc(A)  ((FuncCallState *)((char *)(A) - offsetof(FuncCallState, link_all)))

#endif /* ENABLE_FuncCallInjection */

static ExpressionIdentifierCallBack ** id_callbacks = NULL;
static int id_callback_max = 0;
static int id_callback_cnt = 0;

static void ini_value(Value * v) {
    memset(v, 0, sizeof(Value));
    v->ctx = expression_context;
}

void set_value(Value * v, void * data, size_t size, int be) {
    v->sym = NULL;
    v->reg = NULL;
    v->loc = NULL;
    v->remote = 0;
    v->address = 0;
    v->function = 0;
    v->func_cb = NULL;
    v->field_cb = NULL;
    v->sym_list = NULL;
    v->size = (ContextAddress)size;
    v->big_endian = be;
    v->binary_scale = 0;
    v->decimal_scale = 0;
    v->bit_stride = 0;
    v->value = tmp_alloc(size);
    if (data == NULL) memset(v->value, 0, size);
    else memcpy(v->value, data, size);
}

static void set_int_value(Value * v, size_t size, uint64_t n) {
    union {
        uint8_t  u8;
        uint16_t u16;
        uint32_t u32;
        uint64_t u64;
    } buf;
    switch (size) {
    case 0: break;
    case 1: buf.u8 = (uint8_t)n; break;
    case 2: buf.u16 = (uint16_t)n; break;
    case 4: buf.u32 = (uint32_t)n; break;
    case 8: buf.u64 = n; break;
    default: assert(0);
    }
    set_value(v, &buf, size, big_endian);
}

static void set_fp_value(Value * v, size_t size, double n) {
    union {
        float  f;
        double d;
    } buf;
    switch (size) {
    case 4: buf.f = (float)n; break;
    case 8: buf.d = n; break;
    default: assert(0);
    }
    set_value(v, &buf, size, big_endian);
}

static void set_complex_value(Value * v, size_t size, double r, double i) {
    float  f;
    uint8_t buf[16];
    switch (size) {
    case 8:
        assert(sizeof(f) == 4);
        f = (float)r;
        memcpy(buf, &f, 4);
        f = (float)i;
        memcpy(buf + 4, &f, 4);
        set_value(v, buf, size, big_endian);
        break;
    case 16:
        assert(sizeof(r) == 8);
        memcpy(buf, &r, 8);
        memcpy(buf + 8, &i, 8);
        set_value(v, buf, size, big_endian);
        break;
    default: assert(0);
    }
}

static void set_ctx_word_value(Value * v, ContextAddress data) {
    set_int_value(v, context_word_size(expression_context), data);
}

static void set_string_value(Value * v, char * str) {
    v->type_class = TYPE_CLASS_ARRAY;
    if (str != NULL) set_value(v, str, strlen(str) + 1, 0);
}

static void error(int no, const char * fmt, ...) {
    va_list ap;
    char * buf = NULL;
    va_start(ap, fmt);
    buf = tmp_vprintf(fmt, ap);
    va_end(ap);
    str_fmt_exception(no, "At col %d: %s", sy_pos, buf);
}

#define next_ch_fast() { \
    if (text_pos < text_len) text_ch = (unsigned char)text[text_pos++]; \
}

static void next_ch(void) {
    next_ch_fast();
}

static int next_hex(void) {
    int ch = text_ch;
    next_ch_fast();
    if (ch >= '0' && ch <= '9') return ch - '0';
    if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
    if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
    error(ERR_INV_EXPRESSION, "Invalid hexadecimal number");
    return 0;
}

static int next_oct(void) {
    int ch = text_ch;
    next_ch_fast();
    if (ch >= '0' && ch <= '7') return ch - '0';
    error(ERR_INV_EXPRESSION, "Invalid octal number");
    return 0;
}

static int next_dec(void) {
    int ch = text_ch;
    next_ch_fast();
    if (ch >= '0' && ch <= '9') return ch - '0';
    error(ERR_INV_EXPRESSION, "Invalid decimal number");
    return 0;
}

static int next_char_val(void) {
    int n = 0;
    if (text_ch == 0) {
        error(ERR_INV_EXPRESSION, "Unexpected end of expression");
    }
    else if (text_ch == '\\') {
        next_ch();
        switch (text_ch) {
        case 'n' : n = '\n'; break;
        case 't' : n = '\t'; break;
        case 'v' : n = '\v'; break;
        case 'b' : n = '\b'; break;
        case 'r' : n = '\r'; break;
        case 'f' : n = '\f'; break;
        case 'a' : n = '\a'; break;
        case '\\': n = '\\'; break;
        case '\'': n = '\''; break;
        case '"' : n = '"'; break;
        case 'x' :
            next_ch();
            n = next_hex() << 8;
            n |= next_hex() << 4;
            n |= next_hex();
            return n;
        case '0' :
        case '1' :
        case '2' :
        case '3' :
            n = next_oct() << 6;
            n |= next_oct() << 3;
            n |= next_oct();
            return n;
        default  :
            n = text_ch;
            break;
        }
    }
    else {
        n = text_ch;
    }
    next_ch_fast();
    return n;
}

static void set_string_text_val(int pos, int len, int in_quotes, size_t char_size) {
    int cnt = 0;
    memset(&text_val, 0, sizeof(text_val));
    text_val.type_class = TYPE_CLASS_ARRAY;
    text_val.size = (len + 1) * char_size;
    text_val.value = tmp_alloc((size_t)text_val.size);
    text_val.constant = 1;
    text_pos = pos - 1;
    next_ch();
    if (in_quotes) {
        while (cnt < len) {
            int ch = next_char_val();
            if (big_endian) swap_bytes(&ch, sizeof(ch));
            memcpy((uint8_t *)text_val.value + char_size * cnt++, &ch, char_size);
        }
    }
    else {
        while (cnt < len) {
            int ch = text_ch;
            if (big_endian) swap_bytes(&ch, sizeof(ch));
            memcpy((uint8_t *)text_val.value + char_size * cnt++, &ch, char_size);
            next_ch_fast();
        }
    }
    memset((uint8_t *)text_val.value + char_size * cnt++, 0, char_size);
}

static int is_name_character(int ch) {
    if (ch >= 'A' && ch <= 'Z') return 1;
    if (ch >= 'a' && ch <= 'z') return 1;
    if (ch >= '0' && ch <= '9') return 1;
    if (ch == '_') return 1;
    if (ch == '$') return 1;
    if (ch == '@') return 1;
    return 0;
}

static void parse_fp_number(int pos) {
    char * end = NULL;
    double x = str_to_double(text + pos, &end);
    text_pos = end - text;
    next_ch();
    text_val.type_class = TYPE_CLASS_REAL;
    if (text_ch == 'f' || text_ch == 'F') {
        next_ch();
        text_val_flags |= VAL_FLAG_F;
        set_fp_value(&text_val, sizeof(float), x);
    }
    else {
        if (text_ch == 'l' || text_ch == 'L') {
            next_ch();
            text_val_flags |= VAL_FLAG_L;
        }
        set_fp_value(&text_val, sizeof(double), x);
    }
}

static void parse_char_value(int l) {
    int value = next_char_val();
    memset(&text_val, 0, sizeof(text_val));
    if (l) {
        text_val.type_class = TYPE_CLASS_CARDINAL;
        if (value >= 0 && value <= 0xffff) set_int_value(&text_val, 2, value);
        else set_int_value(&text_val, 4, value);
        text_val_flags |= VAL_FLAG_L;
    }
    else {
        text_val.type_class = TYPE_CLASS_INTEGER;
        if (value >= 0 && value <= 0x7f) set_int_value(&text_val, 1, value);
        else set_int_value(&text_val, 2, value);
    }
    text_val_flags |= VAL_FLAG_C;
    text_val.constant = 1;
    if (text_ch != '\'') error(ERR_INV_EXPRESSION, "Missing 'single quote'");
    next_ch();
    text_sy = SY_VAL;
}

static void parse_string_value(int l) {
    int len = 0;
    int pos = text_pos;
    while (text_ch != '"') {
        next_char_val();
        len++;
    }
    set_string_text_val(pos, len, 1, l ? 4 : 1);
    text_val_flags |= VAL_FLAG_S;
    if (l) text_val_flags |= VAL_FLAG_L;
    text_val.constant = 1;
    text_sy = SY_VAL;
    next_ch();
}

static void next_sy(void) {
    for (;;) {
        int ch = text_ch;
        sy_pos = text_pos - 1;
        text_val_flags = 0;
        next_ch_fast();
        switch (ch) {
        case 0:
            text_sy = 0;
            return;
        case ' ':
        case '\r':
        case '\n':
        case '\t':
            continue;
        case '(':
        case ')':
        case '{':
        case '}':
        case '~':
        case '[':
        case ']':
        case ';':
        case '?':
        case ',':
            text_sy = ch;
            return;
        case '.':
            if (text_ch == '*') {
                next_ch();
                text_sy = SY_PM_D;
                return;
            }
            text_sy = ch;
            return;
        case ':':
            if (text_ch == ':') {
                next_ch();
                text_sy = SY_SCOPE;
                return;
            }
            text_sy = ch;
            return;
        case '-':
            if (text_ch == '>') {
                next_ch();
                if (text_ch == '*') {
                    next_ch();
                    text_sy = SY_PM_R;
                    return;
                }
                text_sy = SY_REF;
                return;
            }
            if (text_ch == '-') {
                next_ch();
                text_sy = SY_DEC;
                return;
            }
            if (text_ch == '=') {
                next_ch();
                text_sy = SY_A_SUB;
                return;
            }
            text_sy = ch;
            return;
        case '+':
            if (text_ch == '+') {
                next_ch();
                text_sy = SY_INC;
                return;
            }
            if (text_ch == '=') {
                next_ch();
                text_sy = SY_A_ADD;
                return;
            }
            text_sy = ch;
            return;
        case '<':
            if (text_ch == '<') {
                next_ch();
                if (text_ch == '=') {
                    next_ch();
                    text_sy = SY_A_SHL;
                    return;
                }
                text_sy = SY_SHL;
                return;
            }
            if (text_ch == '=') {
                next_ch();
                text_sy = SY_LEQ;
                return;
            }
            text_sy = ch;
            return;
        case '>':
            if (text_ch == '>') {
                next_ch();
                if (text_ch == '=') {
                    next_ch();
                    text_sy = SY_A_SHR;
                    return;
                }
                text_sy = SY_SHR;
                return;
            }
            if (text_ch == '=') {
                next_ch();
                text_sy = SY_GEQ;
                return;
            }
            text_sy = ch;
            return;
        case '=':
            if (text_ch == '=') {
                next_ch();
                text_sy = SY_EQU;
                return;
            }
            text_sy = ch;
            return;
        case '!':
            if (text_ch == '=') {
                next_ch();
                text_sy = SY_NEQ;
                return;
            }
            text_sy = ch;
            return;
        case '&':
            if (text_ch == '&') {
                next_ch();
                text_sy = SY_AND;
                return;
            }
            if (text_ch == '=') {
                next_ch();
                text_sy = SY_A_AND;
                return;
            }
            text_sy = ch;
            return;
        case '|':
            if (text_ch == '|') {
                next_ch();
                text_sy = SY_OR;
                return;
            }
            if (text_ch == '=') {
                next_ch();
                text_sy = SY_A_OR;
                return;
            }
            text_sy = ch;
            return;
        case '*':
            if (text_ch == '=') {
                next_ch();
                text_sy = SY_A_MUL;
                return;
            }
            text_sy = ch;
            return;
        case '/':
            if (text_ch == '|') {
                next_ch();
                text_sy = SY_A_DIV;
                return;
            }
            text_sy = ch;
            return;
        case '%':
            if (text_ch == '|') {
                next_ch();
                text_sy = SY_A_MOD;
                return;
            }
            text_sy = ch;
            return;
        case '^':
            if (text_ch == '=') {
                next_ch();
                text_sy = SY_A_XOR;
                return;
            }
            text_sy = ch;
            return;
        case '\'':
            parse_char_value(0);
            return;
        case '"':
            parse_string_value(0);
            return;
        case '0':
            {
                uint64_t value = 0;
                int pos = text_pos - 2;
                memset(&text_val, 0, sizeof(text_val));
                if (text_ch == '.' || text_ch == 'e' || text_ch == 'E' || text_ch == 'f' || text_ch == 'F') {
                    parse_fp_number(pos);
                    text_val.constant = 1;
                    text_sy = SY_VAL;
                    return;
                }
                else if (text_ch == 'x') {
                    next_ch();
                    text_val_flags |= VAL_FLAG_X;
                    text_val.type_class = TYPE_CLASS_CARDINAL;
                    while ((text_ch >= '0' && text_ch <= '9') ||
                           (text_ch >= 'A' && text_ch <= 'F') ||
                           (text_ch >= 'a' && text_ch <= 'f')) {
                        value = (value << 4) | next_hex();
                    }
                }
                else {
                    text_val.type_class = TYPE_CLASS_INTEGER;
                    while (text_ch >= '0' && text_ch <= '7') {
                        value = (value << 3) | next_oct();
                        text_val.type_class = TYPE_CLASS_CARDINAL;
                        text_val_flags |= VAL_FLAG_X;
                    }
                }
                if (value <= 0xff) set_int_value(&text_val, sizeof(uint8_t), value);
                else if (value <= 0xffff) set_int_value(&text_val, sizeof(uint16_t), value);
                else if (value <= 0xffffffff) set_int_value(&text_val, sizeof(uint32_t), value);
                else set_int_value(&text_val, sizeof(uint64_t), value);
                text_val.constant = 1;
                for (;;) {
                    if (text_ch == 'l' || text_ch == 'L') {
                        text_val_flags |= VAL_FLAG_L;
                        next_ch();
                    }
                    else if (text_ch == 'u' || text_ch == 'U') {
                        text_val_flags |= VAL_FLAG_U;
                        next_ch();
                    }
                    else {
                        break;
                    }
                }
            }
            text_sy = SY_VAL;
            return;
        default:
            if (ch >= '0' && ch <= '9') {
                int pos = text_pos - 2;
                uint64_t value = ch - '0';
                while (text_ch >= '0' && text_ch <= '9') {
                    value = (value * 10) + next_dec();
                }
                memset(&text_val, 0, sizeof(text_val));
                if (text_ch == '.' || text_ch == 'e' || text_ch == 'E' || text_ch == 'f' || text_ch == 'F') {
                    parse_fp_number(pos);
                }
                else {
                    text_val.type_class = TYPE_CLASS_INTEGER;
                    for (;;) {
                        if (text_ch == 'l' || text_ch == 'L') {
                            text_val_flags |= VAL_FLAG_L;
                            next_ch();
                        }
                        else if (text_ch == 'u' || text_ch == 'U') {
                            text_val.type_class = TYPE_CLASS_CARDINAL;
                            text_val_flags |= VAL_FLAG_U;
                            next_ch();
                        }
                        else {
                            break;
                        }
                    }
                    if (text_val.type_class == TYPE_CLASS_INTEGER) {
                        if (value <= 0x7f) set_int_value(&text_val, 1, value);
                        else if (value <= 0x7fff) set_int_value(&text_val, 2, value);
                        else if (value <= 0x7fffffff) set_int_value(&text_val, 4, value);
                        else set_int_value(&text_val, 8, value);
                    }
                    else {
                        if (value <= 0xff) set_int_value(&text_val, 1, value);
                        else if (value <= 0xffff) set_int_value(&text_val, 2, value);
                        else if (value <= 0xffffffff) set_int_value(&text_val, 4, value);
                        else set_int_value(&text_val, 8, value);
                    }
                }
                text_val.constant = 1;
                text_sy = SY_VAL;
                return;
            }
            if (ch == '$') {
                if (text_ch == '"') {
                    int len = 0;
                    int pos = text_pos + 1;
                    next_char_val();
                    while (text_ch != '"') {
                        next_char_val();
                        len++;
                    }
                    set_string_text_val(pos, len, 1, 1);
                    text_sy = SY_NAME;
                    next_ch();
                    return;
                }
                if (text_ch == '{') {
                    int len = 0;
                    int pos = text_pos + 1;
                    next_ch();
                    while (text_ch != '}') {
                        next_ch_fast();
                        len++;
                    }
                    set_string_text_val(pos, len, 0, 1);
                    text_sy = SY_ID;
                    next_ch();
                    return;
                }
            }
            if (ch == 'L' && text_ch == '\'') {
                next_ch();
                parse_char_value(1);
                return;
            }
            if (ch == 'L' && text_ch == '"') {
                next_ch();
                parse_string_value(1);
                return;
            }
            if (is_name_character(ch)) {
                int len = 1;
                int pos = text_pos - 1;
                while (is_name_character(text_ch)) {
                    next_ch_fast();
                    len++;
                }
                set_string_text_val(pos, len, 0, 1);
                if (strcmp((const char *)text_val.value, "sizeof") == 0) text_sy = (int)SY_SIZEOF;
                else text_sy = SY_NAME;
                return;
            }
            error(ERR_INV_EXPRESSION, "Illegal character");
            break;
        }
    }
}

static void reg2value(int mode, Context * ctx, int frame, RegisterDefinition * def, Value * v) {
    ini_value(v);
    set_value(v, NULL, def->size, def->big_endian);
    v->type_class = def->fp_value ? TYPE_CLASS_REAL : TYPE_CLASS_CARDINAL;
    v->reg = def;
    v->loc = (LocationExpressionState *)tmp_alloc_zero(sizeof(LocationExpressionState));
    v->loc->ctx = ctx;
    if (def->size == 0 && def->bits != NULL) {
        unsigned bit_cnt = 0;
        int * bits = def->bits;
        while (bits[bit_cnt] >= 0) bit_cnt++;
        for (;;) {
            unsigned i = 0;
            def = def->parent;
            if (def->bits != NULL) {
                /* The parent is also a bitfield */
                int * pbits = (int *)tmp_alloc_zero(sizeof(int) * (bit_cnt + 1));
                while (i < bit_cnt) {
                    pbits[i] = def->bits[bits[i]];
                    i++;
                }
                pbits[i] = -1;
                bits = pbits;
            }
            else {
                v->loc->pieces = (LocationPiece *)tmp_alloc_zero(sizeof(LocationPiece) * bit_cnt);
                v->loc->pieces_cnt = v->loc->pieces_max = bit_cnt;
                while (i < bit_cnt) {
                    v->loc->pieces[i].reg = def;
                    v->loc->pieces[i].bit_size = 1;
                    if ((def->big_endian && !def->left_to_right) || (!def->big_endian && def->left_to_right)) {
                        v->loc->pieces[i].bit_offs = (bits[i] & ~7) | (7 - (bits[i] & 7));
                    }
                    else {
                        v->loc->pieces[i].bit_offs = bits[i];
                    }
                    i++;
                }
                v->size = def->size;
                v->value = tmp_alloc((size_t)v->size);
                break;
            }
        }
    }
    else {
        v->loc->pieces = (LocationPiece *)tmp_alloc_zero(sizeof(LocationPiece));
        v->loc->pieces_cnt = v->loc->pieces_max = 1;
        v->loc->pieces->reg = def;
        v->loc->pieces->size = def->size;
    }
    assert(v->size == def->size);
    if (def->size > 0 && mode == MODE_NORMAL) {
        if (ctx->exited) {
            exception(ERR_ALREADY_EXITED);
        }
        else if (def->memory_context != NULL) {
            if (context_read_reg(ctx, def, 0, def->size, v->value) < 0) exception(errno);
        }
        else if (!context_has_state(ctx)) {
            if (frame != STACK_NO_FRAME) str_exception(ERR_INV_CONTEXT, "Invalid stack frame ID");
            if (context_read_reg(ctx, def, 0, def->size, v->value) < 0) exception(errno);
        }
        else if ((ctx->reg_access & REG_ACCESS_RD_RUNNING) == 0 && !is_ctx_stopped(ctx)) {
            str_exception(errno, "Cannot read CPU register");
        }
        else if (frame == STACK_TOP_FRAME || frame == STACK_NO_FRAME) {
            if (context_read_reg(ctx, def, 0, def->size, v->value) < 0) exception(errno);
        }
        else {
            StackFrame * info = NULL;
            if (get_frame_info(ctx, frame, &info) < 0) exception(errno);
            if (read_reg_bytes(info, def, 0, def->size, (uint8_t *)v->value) < 0) exception(errno);
            v->loc->stack_frame = info;
        }
        if (def != v->reg) {
            unsigned i;
            uint8_t * value = (uint8_t *)v->value;
            v->size = (v->loc->pieces_cnt + 7) / 8;
            v->value = tmp_alloc_zero((size_t)v->size);
            for (i = 0; i < v->loc->pieces_cnt; i++) {
                LocationPiece * p = v->loc->pieces + i;
                unsigned bit = p->bit_offs;
                assert(p->reg == def);
                assert(p->bit_size == 1);
                assert(p->bit_offs / 8 < def->size);
                if ((value[bit / 8] & bit_mask(v, bit)) == 0) continue;
                ((uint8_t *)v->value)[i / 8] |= bit_mask(v, i);
            }
        }
    }
}

#if ENABLE_Symbols
static void set_value_endianness(Value * v, Symbol * sym, Symbol * type) {
    SYM_FLAGS flags = 0;
    if (sym != NULL && get_symbol_flags(sym, &flags) < 0) {
        error(errno, "Cannot retrieve symbol flags");
    }
    if (flags & SYM_FLAG_BIG_ENDIAN) v->big_endian = 1;
    else if (flags & SYM_FLAG_LITTLE_ENDIAN) v->big_endian = 0;
    else {
        if (type != NULL && get_symbol_flags(type, &flags) < 0) {
            error(errno, "Cannot retrieve symbol flags");
        }
        if (flags & SYM_FLAG_BIG_ENDIAN) v->big_endian = 1;
        else if (flags & SYM_FLAG_LITTLE_ENDIAN) v->big_endian = 0;
        else v->big_endian = expression_context->big_endian;
    }
}

static void bit_sign_extend(Value * v, unsigned bit_cnt) {
    if (bit_cnt > 0 && bit_cnt < v->size * 8) {
        uint8_t * buf = (uint8_t *)v->value;
        if (v->big_endian) {
            unsigned offs = (unsigned)v->size * 8 - bit_cnt;
            if (buf[offs / 8] & (1 << (7 - offs % 8))) {
                /* Negative integer number */
                while (offs > 0) {
                    offs--;
                    buf[offs / 8] |= 1 << (7 - offs % 8);
                }
            }
        }
        else {
            unsigned offs = bit_cnt - 1;
            if (buf[offs / 8] & (1 << (offs % 8))) {
                /* Negative integer number */
                while (offs < v->size * 8 - 1) {
                    offs++;
                    buf[offs / 8] |= 1 << (offs % 8);
                }
            }
        }
    }
}

static void sign_extend(Value * v, LocationExpressionState * loc) {
    ContextAddress type_size = 0;
    assert(v->value != NULL);
    if (v->type == NULL) return;
    if (get_symbol_size(v->type, &type_size) < 0) {
        error(errno, "Cannot retrieve value type size");
    }
    if (type_size > v->size) {
        /* Extend size */
        uint8_t * buf = (uint8_t *)tmp_alloc_zero((size_t)type_size);
        if (!v->big_endian) memcpy(buf, v->value, (size_t)v->size);
        else memcpy(buf + (size_t)(type_size - v->size), v->value, (size_t)v->size);
        v->size = type_size;
        v->value = buf;
    }
    if (v->type_class == TYPE_CLASS_INTEGER) {
        /* Extend sign */
        unsigned i;
        unsigned bit_cnt = 0;
        for (i = 0; i < loc->pieces_cnt; i++) {
            LocationPiece * piece = loc->pieces + i;
            bit_cnt += piece->bit_size ? piece->bit_size : piece->size * 8;
        }
        bit_sign_extend(v, bit_cnt);
    }
}

static void check_hidden_redirection(Value * v) {
    Symbol * type = v->type;
    if (!v->remote) return;
    if (type == NULL) return;
    for (;;) {
        SYM_FLAGS flags = 0;
        Symbol * next = NULL;
        if (get_symbol_flags(type, &flags) < 0) {
            error(errno, "Cannot retrieve symbol flags");
        }
        if (flags & SYM_FLAG_INDIRECT) {
            LocationExpressionState * state = NULL;
            LocationInfo * loc_info = NULL;
            StackFrame * frame_info = NULL;
            uint64_t args[1];
            args[0] = v->address;
            if (get_location_info(type, &loc_info) < 0) {
                if (get_error_code(errno) == ERR_SYM_NOT_FOUND) break;
                error(errno, "Cannot get symbol data location information");
            }
            if (expression_frame != STACK_NO_FRAME && get_frame_info(expression_context, expression_frame, &frame_info) < 0) {
                error(errno, "Cannot get stack frame info");
            }
            state = evaluate_location_expression(expression_context, frame_info,
                loc_info->value_cmds.cmds, loc_info->value_cmds.cnt, args, 1);
            if (state->stk_pos == 1) {
                v->address = (ContextAddress)state->stk[0];
                v->remote = 1;
                v->loc = NULL;
            }
            else {
                if (state->pieces_cnt == 1 && state->pieces->implicit_pointer == 0 &&
                        state->pieces->reg != NULL && state->pieces->reg->size == state->pieces->size) {
                    v->reg = state->pieces->reg;
                }
                v->remote = 0;
                v->loc = state;
            }
            v->big_endian = loc_info->big_endian;
            if (v->sym != NULL && v->size == 0 && get_symbol_size(v->sym, &v->size) < 0) {
                error(errno, "Cannot retrieve symbol size");
            }
            v->sym = NULL;
            if (!v->remote) break;
        }
        if (get_symbol_type(type, &next) < 0) {
            error(errno, "Cannot retrieve symbol type");
        }
        if (next == type) break;
        type = next;
    }
}

static void set_value_props(Value * v) {
    v->bit_stride = 0;
    v->binary_scale = 0;
    v->decimal_scale = 0;
    if (v->type == NULL) return;
    if (v->type_class == TYPE_CLASS_ARRAY) {
        Symbol * type = v->type;
        for (;;) {
            SymbolProperties props;
            Symbol * next = NULL;
            if (get_symbol_props(type, &props) < 0) {
                error(errno, "Cannot get symbol properties");
            }
            if (props.bit_stride) {
                v->bit_stride = props.bit_stride;
                break;
            }
            if (get_symbol_type(type, &next) < 0) {
                error(errno, "Cannot retrieve symbol type");
            }
            if (next == type) break;
            type = next;
        }
    }
    if (v->type_class == TYPE_CLASS_CARDINAL || v->type_class == TYPE_CLASS_INTEGER) {
        Symbol * type = v->type;
        for (;;) {
            SymbolProperties props;
            Symbol * next = NULL;
            if (get_symbol_props(type, &props) < 0) {
                error(errno, "Cannot get symbol properties");
            }
            if (props.binary_scale) {
                v->binary_scale = props.binary_scale;
                break;
            }
            if (props.decimal_scale) {
                v->decimal_scale = props.decimal_scale;
                break;
            }
            if (get_symbol_type(type, &next) < 0) {
                error(errno, "Cannot retrieve symbol type");
            }
            if (next == type) break;
            type = next;
        }
    }
}

/* Note: sym2value() does NOT set v->size if v->sym != NULL */
static int sym2value(int mode, Symbol * sym, Value * v) {
    int sym_class = 0;
    ini_value(v);
    if (get_symbol_class(sym, &sym_class) < 0) {
        error(errno, "Cannot retrieve symbol class");
    }
    if (get_symbol_type(sym, &v->type) < 0) {
        error(errno, "Cannot retrieve symbol type");
    }
    if (get_symbol_type_class(sym, &v->type_class) < 0) {
        error(errno, "Cannot retrieve symbol type class");
    }
    switch (sym_class) {
    case SYM_CLASS_VALUE:
    case SYM_CLASS_REFERENCE:
    case SYM_CLASS_COMP_UNIT:
        if (mode == MODE_NORMAL) {
            LocationExpressionState * state = NULL;
            LocationInfo * loc_info = NULL;
            StackFrame * frame_info = NULL;
            if (get_location_info(sym, &loc_info) < 0) {
                error(errno, "Cannot get symbol location information");
            }
            if (expression_frame != STACK_NO_FRAME && get_frame_info(expression_context, expression_frame, &frame_info) < 0) {
                error(errno, "Cannot get stack frame info");
            }
            state = evaluate_location_expression(expression_context, frame_info,
                loc_info->value_cmds.cmds, loc_info->value_cmds.cnt, NULL, 0);
            if (state->stk_pos == 1) {
                v->address = (ContextAddress)state->stk[0];
                v->remote = 1;
            }
            else {
                if (state->pieces_cnt == 1 && state->pieces->implicit_pointer == 0 &&
                        state->pieces->reg != NULL && state->pieces->reg->size == state->pieces->size) {
                    v->reg = state->pieces->reg;
                }
                v->loc = state;
            }
            v->big_endian = loc_info->big_endian;
        }
        else {
            v->remote = 1;
        }
        v->constant = sym_class == SYM_CLASS_VALUE;
        set_value_props(v);
        break;
    case SYM_CLASS_FUNCTION:
        {
            ContextAddress word = 0;
            v->type_class = TYPE_CLASS_POINTER;
            if (v->type != NULL && get_array_symbol(v->type, 0, &v->type)) {
                error(errno, "Cannot get function type");
            }
            if (mode == MODE_NORMAL && get_symbol_address(sym, &word) < 0) {
                error(errno, "Cannot retrieve symbol address");
            }
            set_ctx_word_value(v, word);
            v->function = 1;
        }
        break;
    default:
        v->type = sym;
        if (mode == MODE_NORMAL) {
            LocationExpressionState * state = NULL;
            LocationInfo * loc_info = NULL;
            StackFrame * frame_info = NULL;
            if (get_location_info(sym, &loc_info) < 0) {
                break;
            }
            if (expression_frame != STACK_NO_FRAME && get_frame_info(expression_context, expression_frame, &frame_info) < 0) {
                error(errno, "Cannot get stack frame info");
            }
            state = evaluate_location_expression(expression_context, frame_info,
                loc_info->value_cmds.cmds, loc_info->value_cmds.cnt, NULL, 0);
            if (state->stk_pos == 1) {
                v->address = (ContextAddress)state->stk[0];
                v->remote = 1;
            }
            else {
                if (state->pieces_cnt == 1 && state->pieces->implicit_pointer == 0 &&
                    state->pieces->reg != NULL && state->pieces->reg->size == state->pieces->size) {
                    v->reg = state->pieces->reg;
                }
                v->loc = state;
            }
            v->big_endian = loc_info->big_endian;
        }
        break;
    }
    v->sym = sym;
    if (sym_class == SYM_CLASS_REFERENCE && mode == MODE_NORMAL) {
        check_hidden_redirection(v);
    }
    return sym_class;
}

static SYM_FLAGS get_all_symbol_flags(Symbol * sym) {
    SYM_FLAGS all_flags = 0;
    for (;;) {
        Symbol * nxt = NULL;
        SYM_FLAGS sym_flags = 0;
        int sym_class = 0;
        if (get_symbol_flags(sym, &sym_flags) < 0) error(errno, "Cannot get symbol flags");
        all_flags |= sym_flags;
        if (get_symbol_class(sym, &sym_class) < 0) error(errno, "Cannot get symbol class");
        if (sym_class != SYM_CLASS_TYPE) break;
        all_flags |= SYM_FLAG_TYPE;
        if (get_symbol_type(sym, &nxt) < 0) error(errno, "Cannot get symbol type");
        if (nxt == sym) break;
        sym = nxt;
    }
    return all_flags;
}

static unsigned flag_count(SYM_FLAGS flags) {
    unsigned i;
    unsigned cnt = 0;
    for (i = 0; i < sizeof(flags) * 8; i++) {
        if (flags & ((SYM_FLAGS)1 << i)) cnt++;
    }
    return cnt;
}
#endif /* ENABLE_Symbols */

static void expression(int mode, Value * v);
static uint64_t to_uns(int mode, Value * v);
static void load_value(Value * v);

static int builtin_identifier(int mode, char * name, Value * v) {
    Context * ctx = expression_context;
    for (;;) {
        RegisterDefinition * def = get_reg_definitions(ctx);
        if (def != NULL) {
            while (def->name != NULL) {
                if (strcmp(name + 1, def->name) == 0) {
                    int frame = STACK_NO_FRAME;
                    if (ctx == expression_context) frame = expression_frame;
                    reg2value(mode, ctx, frame, def, v);
                    return SYM_CLASS_REFERENCE;
                }
                def++;
            }
        }
        ctx = ctx->parent;
        if (ctx == NULL) break;
    }
    if (strcmp(name, "$thread") == 0) {
        set_string_value(v, expression_context->id);
        v->constant = 1;
        return SYM_CLASS_VALUE;
    }
#if ENABLE_Symbols
    if (strcmp(name, "$relocate") == 0 && text_sy == '(') {
        unsigned cnt;
        uint64_t addr = 0;
        const char * file_name = "";
        const char * sect_name = "";
        Symbol * sym = NULL;
        next_sy();
        for (cnt = 0;; cnt++) {
            switch (cnt) {
            case 0:
                expression(mode, v);
                addr = to_uns(mode, v);
                break;
            case 1:
                expression(mode, v);
                load_value(v);
                file_name = tmp_strndup((char *)v->value, (size_t)v->size);
                break;
            case 2:
                expression(mode, v);
                load_value(v);
                sect_name = tmp_strndup((char *)v->value, (size_t)v->size);
                break;
            default:
                error(ERR_INV_EXPRESSION, "Too many arguments");
            }
            if (text_sy != ',') break;
            next_sy();
        }
        if (text_sy != ')') error(ERR_INV_EXPRESSION, "Missing ')'");
        next_sy();
        ini_value(v);
        if (mode != MODE_NORMAL) {
            set_ctx_word_value(v, addr);
            v->type_class = TYPE_CLASS_POINTER;
            return SYM_CLASS_VALUE;
        }
        if (find_symbol_by_name(expression_context, STACK_NO_FRAME, 0,
            tmp_printf("$relocate:%s:%s:%" PRIX64, file_name, sect_name, addr), &sym) < 0) {
            error(errno, "Cannot read symbol data");
        }
        return sym2value(mode, sym, v);
    }
#endif
    return -1;
}

static int identifier(int mode, Value * scope, char * name, SYM_FLAGS flags, Value * v) {
    ini_value(v);
    if (scope == NULL) {
        int i;
        for (i = 0; i < id_callback_cnt; i++) {
            if (id_callbacks[i](expression_context, expression_frame, name, v)) return SYM_CLASS_VALUE;
        }
        if (expression_context == NULL) {
            exception(ERR_INV_CONTEXT);
        }
        if (name[0] == '$') {
            int id_class = builtin_identifier(mode, name, v);
            if (id_class >= 0) return id_class;
        }
    }
#if ENABLE_Symbols
    {
        Symbol * sym = NULL;
        int n = 0;

        if (scope != NULL) {
            int scope_class = 0;
            Symbol * scope_sym = scope->sym;
            if (scope->type != NULL) {
                if (scope_sym != NULL && get_symbol_class(scope_sym, &scope_class) < 0) {
                    error(errno, "Cannot retrieve symbol class");
                }
                if (scope_class != SYM_CLASS_FUNCTION) {
                    scope_sym = scope->type;
                }
            }
            n = find_symbol_in_scope(expression_context, expression_frame, expression_addr, scope_sym, name, &sym);
        }
        else {
            n = find_symbol_by_name(expression_context, expression_frame, expression_addr, name, &sym);
        }

        if (n < 0) {
            if (get_error_code(errno) != ERR_SYM_NOT_FOUND) error(errno, "Cannot read symbol data");
        }
        else {
            unsigned cnt = 0;
            unsigned max = 8;
            Symbol ** list = (Symbol **)tmp_alloc(sizeof(Symbol *) * max);
            unsigned val_cnt = 0;
            const SYM_FLAGS cmx_type = SYM_FLAG_STRUCT_TYPE | SYM_FLAG_CLASS_TYPE | SYM_FLAG_UNION_TYPE | SYM_FLAG_ENUM_TYPE;
            const SYM_FLAGS flag_mask = SYM_FLAG_TYPE | SYM_FLAG_CONST_TYPE | SYM_FLAG_VOLATILE_TYPE | cmx_type;
            SYM_FLAGS sym_flags;
            int sym_class;
            unsigned i;

            list[cnt++] = sym;
            while (find_next_symbol(&sym) == 0) {
                if (cnt + 1 >= max) list = (Symbol **)tmp_realloc(list, sizeof(Symbol *) * (max *= 2));
                list[cnt++] = sym;
            }
            assert(cnt < max);
            list[cnt] = NULL;
            /* Count variables. In C, variables eclipse composite types */
            for (i = 0; i < cnt; i++) {
                if (get_symbol_class(list[i], &sym_class) < 0) error(errno, "Cannot retrieve symbol class");
                if (sym_class == SYM_CLASS_VALUE || sym_class == SYM_CLASS_REFERENCE) val_cnt++;
            }
            /* Search for best match */
            sym = list[0];
            sym_flags = (get_all_symbol_flags(sym) ^ flags) & flag_mask;
            for (i = 1; i < cnt; i++) {
                SYM_FLAGS nxt_flags = (get_all_symbol_flags(list[i]) ^ flags) & flag_mask;
                if (val_cnt > 0 && (nxt_flags & cmx_type) != 0) continue;
                if (flag_count(nxt_flags) >= flag_count(sym_flags)) continue;
                sym_flags = nxt_flags;
                sym = list[i];
            }
            sym_class = sym2value(mode, sym, v);
            if (cnt > 1) v->sym_list = list;
            return sym_class;
        }
    }
#endif
#if ENABLE_RCBP_TEST
    {
        void * ptr = NULL;
        int sym_class = 0;
        if (find_test_symbol(expression_context, name, &ptr, &sym_class) >= 0) {
            if (sym_class == SYM_CLASS_FUNCTION) {
                set_ctx_word_value(v, (ContextAddress)(uintptr_t)ptr);
                v->type_class = TYPE_CLASS_POINTER;
                v->function = 1;
            }
            else {
                v->address = (ContextAddress)(uintptr_t)ptr;
                v->remote = 1;
            }
            return sym_class;
        }
    }
#endif
    return -1;
}

#ifndef NDEBUG
/* Check for bad recursion */
static int indentifier_checked(int mode, Value * scope, char * name, SYM_FLAGS flags, Value * v) {
    int text_pos_org = text_pos;
    int text_sy_org = text_sy;
    int res = identifier(mode, scope, name, flags, v);
    assert(text_pos_org == text_pos);
    assert(text_sy_org == text_sy);
    return res;
}
#define identifier(mode, scope, name, flags, v) indentifier_checked(mode, scope, name, flags, v)
#endif

static int qualified_name(int mode, Value * scope, SYM_FLAGS flags, Value * v) {
    int sym_class = 0;
    for (;;) {
        ini_value(v);
        if (text_sy == SY_NAME || (scope != NULL && text_sy == '~')) {
            int tilda = text_sy == '~';
            if (tilda) {
                next_sy();
                if (text_sy != SY_NAME) error(ERR_INV_EXPRESSION, "Identifier expected");
            }
            if (mode != MODE_SKIP) {
                SYM_FLAGS f = flags;
                char * name = (char *)text_val.value;
                name = tilda ? tmp_strdup2("~", name) : tmp_strdup(name);
                next_sy();
                if (text_sy == SY_SCOPE) f |= SYM_FLAG_TYPE;
                sym_class = identifier(text_sy != SY_SCOPE ? mode : MODE_TYPE, scope, name, f, v);
                if (sym_class < 0) error(ERR_INV_EXPRESSION, "Undefined identifier '%s'", name);
            }
            else {
                next_sy();
            }
        }
        else if (text_sy == SY_ID) {
            if (mode != MODE_SKIP) {
                int ok = 0;
                Context * ctx = NULL;
                int frame = STACK_NO_FRAME;
                RegisterDefinition * def = NULL;
                const char * id = tmp_strdup((char *)text_val.value);
                next_sy();
                if (id2register(id, &ctx, &frame, &def) >= 0) {
                    if (frame == STACK_TOP_FRAME) frame = expression_frame;
                    sym_class = SYM_CLASS_UNKNOWN;
                    reg2value(mode, ctx, frame, def, v);
                    ok = 1;
                }
#if ENABLE_Symbols
                if (!ok) {
                    Symbol * sym = NULL;
                    if (id2symbol(id, &sym) >= 0) {
                        sym_class = sym2value(text_sy != SY_SCOPE ? mode : MODE_TYPE, sym, v);
                        ok = 1;
                    }
                }
#endif
                if (!ok) error(ERR_INV_EXPRESSION, "Symbol not found: %s", id);
            }
            else {
                next_sy();
            }
        }
        else {
            error(ERR_INV_EXPRESSION, "Identifier expected");
        }
        if (text_sy != SY_SCOPE) break;
        next_sy();
        *(scope = (Value *)tmp_alloc(sizeof(Value))) = *v;
    }
    return sym_class;
}

static int64_t to_int(int mode, Value * v);
#define TYPE_EXPR_LENGTH 64

static int type_expression(int mode, int * buf) {
    int i = 0;
    int pos = 0;
    int expr_buf[TYPE_EXPR_LENGTH];
    int expr_len = 0;
    int arr_buf[TYPE_EXPR_LENGTH];
    int arr_len = 0;
    while (text_sy == '*') {
        next_sy();
        if (pos >= TYPE_EXPR_LENGTH) error(ERR_BUFFER_OVERFLOW, "Type expression is too long");
        buf[pos++] = 1;
    }
    if (text_sy == '(') {
        next_sy();
        expr_len = type_expression(mode, expr_buf);
        if (text_sy != ')') error(ERR_INV_EXPRESSION, "')' expected");
        next_sy();
    }
    while (text_sy == '[') {
        next_sy();
        if (text_sy != SY_VAL) error(ERR_INV_EXPRESSION, "Number expected");
        if (arr_len >= TYPE_EXPR_LENGTH) error(ERR_BUFFER_OVERFLOW, "Type expression is too long");
        arr_buf[arr_len] = (int)to_int(mode, &text_val);
        if (mode == MODE_NORMAL && arr_buf[arr_len] < 1) error(ERR_INV_EXPRESSION, "Positive number expected");
        arr_len++;
        next_sy();
        if (text_sy != ']') error(ERR_INV_EXPRESSION, "']' expected");
        next_sy();
    }
    for (i = 0; i < arr_len; i++) {
        if (pos >= TYPE_EXPR_LENGTH) error(ERR_BUFFER_OVERFLOW, "Type expression is too long");
        buf[pos++] = arr_buf[arr_len - i - 1];
    }
    for (i = 0; i < expr_len; i++) {
        if (pos >= TYPE_EXPR_LENGTH) error(ERR_BUFFER_OVERFLOW, "Type expression is too long");
        buf[pos++] = expr_buf[i];
    }
    return pos;
}

static int type_name(int mode, Symbol ** type) {
    Value v;
    int expr_buf[TYPE_EXPR_LENGTH];
    int expr_len = 0;
    char * name = NULL;
    int sym_class;
    SYM_FLAGS sym_flags = 0;
    int name_cnt = 0;

    while (text_sy == SY_NAME) {
        if (strcmp((const char *)(text_val.value), "const") == 0) {
            sym_flags |= SYM_FLAG_CONST_TYPE;
            next_sy();
        }
        else if (strcmp((const char *)(text_val.value), "volatile") == 0) {
            sym_flags |= SYM_FLAG_VOLATILE_TYPE;
            next_sy();
        }
        else {
            break;
        }
    }
    if (text_sy == SY_NAME) {
        if (strcmp((const char *)(text_val.value), "struct") == 0) {
            sym_flags |= SYM_FLAG_STRUCT_TYPE;
            next_sy();
        }
        else if (strcmp((const char *)(text_val.value), "class") == 0) {
            sym_flags |= SYM_FLAG_CLASS_TYPE;
            next_sy();
        }
        else if (strcmp((const char *)(text_val.value), "union") == 0) {
            sym_flags |= SYM_FLAG_UNION_TYPE;
            next_sy();
        }
        else if (strcmp((const char *)(text_val.value), "enum") == 0) {
            sym_flags |= SYM_FLAG_ENUM_TYPE;
            next_sy();
        }
    }

    if (text_sy == SY_NAME) {
        do {
            if (name == NULL) {
                name = tmp_strdup((char *)text_val.value);
            }
            else {
                name = tmp_strdup2(name, " ");
                name = tmp_strdup2(name, (char *)text_val.value);
            }
            name_cnt++;
            next_sy();
        }
        while (text_sy == SY_NAME);
        if (text_sy == '<') {
            int prev_sy = 0;
            unsigned cnt = 0;
            uint64_t val = 0;
            char tmp_buf[40];
            do {
                switch (text_sy) {
                case SY_NAME:
                    if (prev_sy == SY_NAME || prev_sy == '*' || prev_sy == '&')
                        name = tmp_strdup2(name, " ");
                    name = tmp_strdup2(name, (char *)text_val.value);
                    break;
                case SY_SCOPE:
                    name = tmp_strdup2(name, "::");
                    break;
                case SY_VAL:
                    value_to_unsigned(&text_val, &val);
                    snprintf(tmp_buf, sizeof(tmp_buf), "%lu", (unsigned long)val);
                    name = tmp_strdup2(name, tmp_buf);
                    break;
                case '*':
                case '&':
                case '[':
                case ']':
                case '(':
                case ')':
                case '{':
                case '}':
                    tmp_buf[0] = (char)text_sy;
                    tmp_buf[1] = 0;
                    name = tmp_strdup2(name, tmp_buf);
                    break;
                case ',':
                    name = tmp_strdup2(name, ", ");
                    break;
                case '<':
                    name = tmp_strdup2(name, "<");
                    cnt++;
                    break;
                case '>':
                    if (prev_sy == '>') name = tmp_strdup2(name, " ");
                    name = tmp_strdup2(name, ">");
                    cnt--;
                    break;
                default:
                    return 0;
                }
                prev_sy = text_sy;
                next_sy();
            }
            while (cnt > 0);
        }
        sym_class = identifier(mode, NULL, name, sym_flags | SYM_FLAG_TYPE, &v);
    }
#if ENABLE_Symbols
    else if (text_sy == SY_ID) {
        Symbol * sym = NULL;
        const char * id = (const char *)text_val.value;
        if (id2symbol(id, &sym) < 0) return 0;
        sym_class = sym2value(mode, sym, &v);
        name = tmp_strdup(id);
        next_sy();
    }
#endif
    else {
        if (sym_flags) error(ERR_INV_EXPRESSION, "Identifier expected");
        return 0;
    }

    if (sym_class < 0) return 0;
    if (text_sy == SY_SCOPE) {
        Value scope = v;
        next_sy();
        sym_class = qualified_name(mode, &scope, sym_flags | SYM_FLAG_TYPE, &v);
    }
    if (sym_class != SYM_CLASS_TYPE) {
        if (sym_flags) error(ERR_INV_EXPRESSION, "Type '%s' not found", name);
        return 0;
    }
    expr_len = type_expression(mode, expr_buf);
    if (mode != MODE_SKIP) {
        int i;
        for (i = 0; i < expr_len; i++) {
#if ENABLE_Symbols
            if (expr_buf[i] == 1) {
                if (get_array_symbol(v.type, 0, &v.type)) {
                    error(errno, "Cannot create pointer type");
                }
            }
            else {
                if (get_array_symbol(v.type, expr_buf[i], &v.type)) {
                    error(errno, "Cannot create array type");
                }
            }
#else
            v.type = NULL;
#endif
        }
    }
    *type = v.type;
    return 1;
}

static void load_value(Value * v) {
    if (v->remote) {
        size_t size = (size_t)v->size;
        void * buf = tmp_alloc(size);
        Context * ctx = expression_context;
        Context * mem = context_get_group(ctx, CONTEXT_GROUP_PROCESS);
        assert(!v->constant);
        if ((mem->mem_access & MEM_ACCESS_RD_RUNNING) == 0) {
            if (!is_all_stopped(ctx)) error(errno, "Cannot read memory if not stopped");
        }
        if (context_read_mem(ctx, v->address, buf, size) < 0) {
            error(errno, "Can't read variable value");
        }
        v->value = buf;
        v->remote = 0;
    }
    else if (v->value != NULL) {
        /* OK */
    }
#if ENABLE_Symbols
    else if (v->loc != NULL) {
        size_t size = 0;
        void * value = NULL;
        LocationExpressionState * loc = v->loc;
        read_location_pieces(expression_context, loc->stack_frame,
            loc->pieces, loc->pieces_cnt, v->big_endian, &value, &size);
        v->value = value;
        v->size = (ContextAddress)size;
        sign_extend(v, loc);
    }
#endif
    else {
        error(ERR_OTHER, "Has no value");
    }
}

static int is_number(Value * v) {
    switch (v->type_class) {
    case TYPE_CLASS_INTEGER:
    case TYPE_CLASS_CARDINAL:
    case TYPE_CLASS_REAL:
    case TYPE_CLASS_COMPLEX:
    case TYPE_CLASS_ENUMERATION:
        return 1;
    }
    return 0;
}

static int is_real_number(Value * v) {
    switch (v->type_class) {
    case TYPE_CLASS_INTEGER:
    case TYPE_CLASS_CARDINAL:
        if (v->binary_scale != 0) return 1;
        if (v->decimal_scale != 0) return 1;
        break;
    case TYPE_CLASS_REAL:
        return 1;
    }
    return 0;
}

static int is_whole_number(Value * v) {
    switch (v->type_class) {
    case TYPE_CLASS_INTEGER:
    case TYPE_CLASS_CARDINAL:
        if (v->binary_scale != 0) return 0;
        if (v->decimal_scale != 0) return 0;
        return 1;
    case TYPE_CLASS_ENUMERATION:
        return 1;
    }
    return 0;
}

static void to_host_endianness(Value * v) {
    assert(v->type_class != TYPE_CLASS_COMPOSITE);
    assert(v->type_class != TYPE_CLASS_ARRAY);
    assert(!v->remote);
    if (v->big_endian != big_endian) {
        size_t i = 0;
        size_t n = (size_t)v->size;
        uint8_t * buf = (uint8_t *)tmp_alloc(n);
        for (i = 0; i < n; i++) {
            buf[i] = ((uint8_t *)v->value)[n - i - 1];
        }
        v->value = buf;
        v->big_endian = big_endian;
        v->sym_list = NULL;
        v->sym = NULL;
        v->reg = NULL;
        v->loc = NULL;
    }
}

static int64_t to_int_fixed_point(int mode, Value * v) {
    if (v->size == 0 || mode != MODE_NORMAL) {
        if (v->remote) {
            v->value = tmp_alloc_zero((size_t)v->size);
            v->remote = 0;
        }
        return 0;
    }

    if (v->type_class == TYPE_CLASS_POINTER) {
        load_value(v);
        to_host_endianness(v);
        switch (v->size)  {
        case 1: return *(uint8_t *)v->value;
        case 2: return *(uint16_t *)v->value;
        case 4: return *(uint32_t *)v->value;
        case 8: return *(uint64_t *)v->value;
        }
    }
    if (is_number(v)) {
        load_value(v);
        to_host_endianness(v);

        if (v->type_class == TYPE_CLASS_REAL) {
            switch (v->size)  {
            case 4: return (int64_t)*(float *)v->value;
            case 8: return (int64_t)*(double *)v->value;
            }
        }
        else if (v->type_class == TYPE_CLASS_COMPLEX) {
            /* Not supported */
        }
        else if (v->type_class == TYPE_CLASS_CARDINAL) {
            switch (v->size)  {
            case 1: return (int64_t)*(uint8_t *)v->value;
            case 2: return (int64_t)*(uint16_t *)v->value;
            case 4: return (int64_t)*(uint32_t *)v->value;
            case 8: return (int64_t)*(uint64_t *)v->value;
            }
        }
        else {
            switch (v->size)  {
            case 1: return *(int8_t *)v->value;
            case 2: return *(int16_t *)v->value;
            case 4: return *(int32_t *)v->value;
            case 8: return *(int64_t *)v->value;
            }
        }
    }

    error(ERR_INV_EXPRESSION, "Operation is not applicable for the value type");
    return 0;
}

static uint64_t to_uns_fixed_point(int mode, Value * v) {
    if (v->size == 0 || mode != MODE_NORMAL) {
        if (v->remote) {
            v->value = tmp_alloc_zero((size_t)v->size);
            v->remote = 0;
        }
        return 0;
    }

    if (v->type_class == TYPE_CLASS_ARRAY && v->remote) {
        return (uint64_t)v->address;
    }
    if (v->type_class == TYPE_CLASS_POINTER || v->type_class == TYPE_CLASS_MEMBER_PTR) {
        load_value(v);
        to_host_endianness(v);
        switch (v->size)  {
        case 1: return *(uint8_t *)v->value;
        case 2: return *(uint16_t *)v->value;
        case 4: return *(uint32_t *)v->value;
        case 8: return *(uint64_t *)v->value;
        }
    }
    if (is_number(v)) {
        load_value(v);
        to_host_endianness(v);

        if (v->type_class == TYPE_CLASS_REAL) {
            switch (v->size)  {
            case 4: return (uint64_t)*(float *)v->value;
            case 8: return (uint64_t)*(double *)v->value;
            }
        }
        else if (v->type_class == TYPE_CLASS_COMPLEX) {
            /* Not supported */
        }
        else if (v->type_class == TYPE_CLASS_CARDINAL) {
            switch (v->size)  {
            case 1: return *(uint8_t *)v->value;
            case 2: return *(uint16_t *)v->value;
            case 4: return *(uint32_t *)v->value;
            case 8: return *(uint64_t *)v->value;
            }
        }
        else {
            switch (v->size)  {
            case 1: return (uint64_t)*(int8_t *)v->value;
            case 2: return (uint64_t)*(int16_t *)v->value;
            case 4: return (uint64_t)*(int32_t *)v->value;
            case 8: return (uint64_t)*(int64_t *)v->value;
            }
        }
    }
    if (v->type_class == TYPE_CLASS_UNKNOWN) {
        load_value(v);
        to_host_endianness(v);
        switch (v->size) {
        case 1: return *(uint8_t *)v->value;
        case 2: return *(uint16_t *)v->value;
        case 4: return *(uint32_t *)v->value;
        case 8: return *(uint64_t *)v->value;
        }
    }

    error(ERR_INV_EXPRESSION, "Operation is not applicable for the value type");
    return 0;
}

static int64_t to_int(int mode, Value * v) {
    if (v->type_class == TYPE_CLASS_INTEGER) {
        int64_t n = to_int_fixed_point(mode, v);
        int decimal_scale = v->decimal_scale;
        while (decimal_scale > 0) {
            decimal_scale--;
            n = n * 10;
        }
        while (decimal_scale < 0) {
            decimal_scale++;
            n = n / 10;
        }
        if (v->binary_scale > 0) n = n << +v->binary_scale;
        if (v->binary_scale < 0) n = n >> -v->binary_scale;
        return n;
    }
    if (v->type_class == TYPE_CLASS_CARDINAL) {
        uint64_t n = to_uns_fixed_point(mode, v);
        int decimal_scale = v->decimal_scale;
        while (decimal_scale > 0) {
            decimal_scale--;
            n = n * 10;
        }
        while (decimal_scale < 0) {
            decimal_scale++;
            n = n / 10;
        }
        if (v->binary_scale > 0) n = n << +v->binary_scale;
        if (v->binary_scale < 0) n = n >> -v->binary_scale;
        return n;
    }
    assert(v->binary_scale == 0);
    assert(v->decimal_scale == 0);
    return to_int_fixed_point(mode, v);
}

static uint64_t to_uns(int mode, Value * v) {
    if (v->type_class == TYPE_CLASS_INTEGER) {
        int64_t n = to_int_fixed_point(mode, v);
        int decimal_scale = v->decimal_scale;
        while (decimal_scale > 0) {
            decimal_scale--;
            n = n * 10;
        }
        while (decimal_scale < 0) {
            decimal_scale++;
            n = n / 10;
        }
        if (v->binary_scale > 0) n = n << +v->binary_scale;
        if (v->binary_scale < 0) n = n >> -v->binary_scale;
        return n;
    }
    if (v->type_class == TYPE_CLASS_CARDINAL) {
        uint64_t n = to_uns_fixed_point(mode, v);
        int decimal_scale = v->decimal_scale;
        while (decimal_scale > 0) {
            decimal_scale--;
            n = n * 10;
        }
        while (decimal_scale < 0) {
            decimal_scale++;
            n = n / 10;
        }
        if (v->binary_scale > 0) n = n << +v->binary_scale;
        if (v->binary_scale < 0) n = n >> -v->binary_scale;
        return n;
    }
    assert(v->binary_scale == 0);
    assert(v->decimal_scale == 0);
    return to_uns_fixed_point(mode, v);
}

static double to_double(int mode, Value * v) {
    if (v->size == 0 || mode != MODE_NORMAL) {
        if (v->remote) {
            v->value = tmp_alloc_zero((size_t)v->size);
            v->remote = 0;
        }
        return 0;
    }

    if (is_number(v)) {
        load_value(v);
        to_host_endianness(v);

        if (v->type_class == TYPE_CLASS_REAL) {
            switch (v->size)  {
            case 4: return *(float *)v->value;
            case 8: return *(double *)v->value;
            }
        }
        else if (v->type_class == TYPE_CLASS_COMPLEX) {
            /* Not supported */
        }
        else if (v->type_class == TYPE_CLASS_CARDINAL) {
            double n = (double)to_uns_fixed_point(mode, v);
            int binary_scale = v->binary_scale;
            int decimal_scale = v->decimal_scale;
            while (binary_scale > 0) {
                binary_scale--;
                n = n * 2;
            }
            while (n != 0 && binary_scale < 0) {
                binary_scale++;
                n = n / 2;
            }
            while (decimal_scale > 0) {
                decimal_scale--;
                n = n * 10;
            }
            while (n != 0 && decimal_scale < 0) {
                decimal_scale++;
                n = n / 10;
            }
            return n;
        }
        else {
            double n = (double)to_int_fixed_point(mode, v);
            int binary_scale = v->binary_scale;
            int decimal_scale = v->decimal_scale;
            while (binary_scale > 0) {
                binary_scale--;
                n = n * 2;
            }
            while (n != 0 && binary_scale < 0) {
                binary_scale++;
                n = n / 2;
            }
            while (decimal_scale > 0) {
                decimal_scale--;
                n = n * 10;
            }
            while (n != 0 && decimal_scale < 0) {
                decimal_scale++;
                n = n / 10;
            }
            return n;
        }
    }

    error(ERR_INV_EXPRESSION, "Operation is not applicable for the value type");
    return 0;
}

static double to_i_double(int mode, Value * v) {
    if (v->size == 0 || mode != MODE_NORMAL) {
        if (v->remote) {
            v->value = tmp_alloc_zero((size_t)v->size);
            v->remote = 0;
        }
        return 0;
    }

    if (is_number(v)) {
        load_value(v);
        to_host_endianness(v);

        if (v->type_class == TYPE_CLASS_COMPLEX) {
            switch (v->size)  {
            case 8: return *(float *)((char *)v->value + 4);
            case 16: return *(double *)((char *)v->value + 8);
            }
        }
        else {
            return 0.0;
        }
    }

    error(ERR_INV_EXPRESSION, "Operation is not applicable for the value type");
    return 0;
}

static double to_r_double(int mode, Value * v) {
    if (v->size == 0 || mode != MODE_NORMAL) {
        if (v->remote) {
            v->value = tmp_alloc_zero((size_t)v->size);
            v->remote = 0;
        }
        return 0;
    }

    if (is_number(v)) {
        load_value(v);
        to_host_endianness(v);

        if (v->type_class == TYPE_CLASS_COMPLEX) {
            switch (v->size)  {
            case 8: return *(float *)v->value;
            case 16: return *(double *)v->value;
            }
        }
        else {
            return to_double(mode, v);
        }
    }

    error(ERR_INV_EXPRESSION, "Operation is not applicable for the value type");
    return 0;
}

static int to_boolean(int mode, Value * v) {
    return to_int(mode, v) != 0;
}

static void qualified_name_expression(int mode, Value * scope, Value * v) {
    if (qualified_name(mode, scope, 0, v) != SYM_CLASS_TYPE) return;
    error(ERR_INV_EXPRESSION, "Illegal usage of a type in expression");
}

#if ENABLE_Symbols
static int get_std_type(const char * name, int type_class, Symbol ** type, size_t * size) {
    Symbol * sym = NULL;
    int sym_class = 0;
    ContextAddress sym_size = 0;
    if (find_symbol_by_name(expression_context,
        expression_frame, expression_addr, name, &sym) < 0) return 0;
    if (sym == NULL) return 0;
    if (get_symbol_class(sym, &sym_class) < 0 || sym_class != SYM_CLASS_TYPE) return 0;
    if (type_class != TYPE_CLASS_UNKNOWN) {
        int sym_type_class = TYPE_CLASS_UNKNOWN;
        if (get_symbol_type_class(sym, &sym_type_class) < 0 || sym_type_class != type_class) return 0;
    }
    if (get_symbol_size(sym, &sym_size) < 0 || sym_size == 0) return 0;
    *type = sym;
    *size = (size_t)sym_size;
    return 1;
}
#endif

static void set_fp_type(Value * v) {
#if ENABLE_Symbols
    Symbol * type = NULL;
    size_t size = 0;
    if (get_std_type("float", TYPE_CLASS_REAL, &type, &size) && v->size == size) {
        v->type = type;
        return;
    }
    if (get_std_type("double", TYPE_CLASS_REAL, &type, &size) && v->size == size) {
        v->type = type;
        return;
    }
#endif
}

static void set_complex_type(Value * v) {
#if ENABLE_Symbols
    Symbol * type = NULL;
    size_t size = 0;
    if (get_std_type("complex float", TYPE_CLASS_COMPLEX, &type, &size) && v->size == size) {
        v->type = type;
        return;
    }
    if (get_std_type("complex double", TYPE_CLASS_COMPLEX, &type, &size) && v->size == size) {
        v->type = type;
        return;
    }
#endif
}

static void primary_expression(int mode, Value * v) {
    if (text_sy == '(') {
        next_sy();
        for (;;) {
            expression(mode, v);
            if (text_sy != ',') break;
            next_sy();
        }
        if (text_sy != ')') error(ERR_INV_EXPRESSION, "Missing ')'");
        next_sy();
    }
    else if (text_sy == SY_VAL) {
#if ENABLE_Symbols
        int flags = text_val_flags;
#endif
        *v = text_val;
        next_sy();
#if ENABLE_Symbols
        if (v->type_class == TYPE_CLASS_INTEGER || v->type_class == TYPE_CLASS_CARDINAL) {
            size_t size = 0;
            uint64_t n = to_uns(MODE_NORMAL, v);
            if (flags & VAL_FLAG_C) {
                Symbol * type = NULL;
                if (get_std_type(flags & VAL_FLAG_L ? "wchar_t" : "char", TYPE_CLASS_UNKNOWN, &type, &size)) {
                    uint64_t m = ((uint64_t)1 << (size * 8 - 1)) - 1;
                    if (n <= m) {
                        v->type = type;
                        get_symbol_type_class(type, &v->type_class);
                    }
                }
            }
            else {
                if ((flags & (VAL_FLAG_L | VAL_FLAG_U)) == 0) {
                    Symbol * type = NULL;
                    if (get_std_type("int", TYPE_CLASS_INTEGER, &type, &size)) {
                        uint64_t m = ((uint64_t)1 << (size * 8 - 1)) - 1;
                        if (n <= m) {
                            v->type = type;
                            v->type_class = TYPE_CLASS_INTEGER;
                        }
                    }
                }
                if (v->type == NULL && (flags & VAL_FLAG_L) == 0 &&
                        (flags & (VAL_FLAG_X | VAL_FLAG_U)) != 0) {
                    Symbol * type = NULL;
                    if (get_std_type("unsigned int", TYPE_CLASS_CARDINAL, &type, &size)) {
                        uint64_t m = ((uint64_t)1 << (size * 8)) - 1;
                        if (n <= m) {
                            v->type = type;
                            v->type_class = TYPE_CLASS_CARDINAL;
                        }
                    }
                }
                if (v->type == NULL && (flags & VAL_FLAG_U) == 0) {
                    Symbol * type = NULL;
                    if (get_std_type("long int", TYPE_CLASS_INTEGER, &type, &size)) {
                        uint64_t m = ((uint64_t)1 << (size * 8 - 1)) - 1;
                        if (n <= m) {
                            v->type = type;
                            v->type_class = TYPE_CLASS_INTEGER;
                        }
                    }
                }
                if (v->type == NULL) {
                    Symbol * type = NULL;
                    if (get_std_type("long unsigned int", TYPE_CLASS_CARDINAL, &type, &size)) {
                        uint64_t m = ((uint64_t)1 << (size * 8)) - 1;
                        if (n <= m) {
                            v->type = type;
                            v->type_class = TYPE_CLASS_CARDINAL;
                        }
                    }
                }
            }
            if (v->type != NULL && size != v->size) set_int_value(v, size, n);
        }
        else if (v->type_class == TYPE_CLASS_REAL) {
            size_t size = 0;
            Symbol * type = NULL;
            const char * name = flags & VAL_FLAG_F ? "float" : "double";
            if (get_std_type(name, TYPE_CLASS_REAL, &type, &size)) {
                v->type = type;
                if (size != v->size) set_fp_value(v, size, to_double(MODE_NORMAL, v));
            }
        }
        else if (v->type_class == TYPE_CLASS_ARRAY && (flags & VAL_FLAG_S) != 0) {
            if (text_sy == SY_SCOPE) {
                Value * x = (Value *)tmp_alloc_zero(sizeof(Value));
                char * name = NULL;
                if (flags & VAL_FLAG_L) {
                    unsigned i = 0;
                    unsigned j = 0;
                    unsigned size = (unsigned)v->size;
                    name = (char *)tmp_alloc_zero(size + 1);
                    for (i = 0; i + 3 < size; i += 4) {
                        unsigned ch = ((uint8_t *)v->value)[i + 0];
                        ch |= (unsigned)((uint8_t *)v->value)[i + 1] << 8;
                        ch |= (unsigned)((uint8_t *)v->value)[i + 2] << 16;
                        ch |= (unsigned)((uint8_t *)v->value)[i + 3] << 24;
                        if (ch < 0x80) {
                            name[j++] = (char)ch;
                        }
                        else if (ch < 0x800) {
                            name[j++] = (char)((ch >> 6) | 0xc0);
                            name[j++] = (char)((ch & 0x3f) | 0x80);
                        }
                        else {
                            name[j++] = (char)((ch >> 12) | 0xe0);
                            name[j++] = (char)(((ch >> 6) & 0x3f) | 0x80);
                            name[j++] = (char)((ch & 0x3f) | 0x80);
                        }
                    }
                }
                else {
                    name = (char *)v->value;
                }
                if (identifier(mode, NULL, name, 0, x) < 0)
                    error(ERR_INV_EXPRESSION, "Undefined identifier '%s'", name);
                next_sy();
                qualified_name_expression(mode, x, v);
            }
            else {
                size_t size = 0;
                Symbol * type = NULL;
                if (get_std_type(flags & VAL_FLAG_L ? "wchar_t" : "char", TYPE_CLASS_UNKNOWN, &type, &size)) {
                    size_t def_size = flags & VAL_FLAG_L ? 4 : 1;
                    unsigned n = (unsigned)(v->size / def_size);
                    if (n > 0 && size > 0 && get_array_symbol(type, n, &type) >= 0) {
                        if (size != def_size) {
                            unsigned i;
                            uint8_t * buf = (uint8_t *)tmp_alloc_zero(size * n);
                            size_t min_size = size > def_size ? def_size : size;
                            for (i = 0; i < n; i++) {
                                memcpy(buf + i * size, (uint8_t *)v->value + i * def_size, min_size);
                            }
                            v->value = buf;
                            v->size = size * n;
                        }
                        v->type = type;
                    }
                }
            }
        }
#endif
    }
    else if (text_sy == SY_SCOPE) {
        next_sy();
        qualified_name_expression(mode, (Value *)tmp_alloc_zero(sizeof(Value)), v);
    }
    else if (text_sy == SY_NAME || text_sy == SY_ID) {
        qualified_name_expression(mode, NULL, v);
    }
    else {
        error(ERR_INV_EXPRESSION, "Syntax error");
    }
}

static void op_deref(int mode, Value * v) {
#if ENABLE_Symbols
    Symbol * type = NULL;
    if (mode == MODE_SKIP) return;
    if (v->type_class != TYPE_CLASS_ARRAY && v->type_class != TYPE_CLASS_POINTER) {
        error(ERR_INV_EXPRESSION, "Array or pointer type expected");
    }
    if (v->type != NULL && get_symbol_base_type(v->type, &type) < 0) {
        error(errno, "Cannot retrieve symbol type");
    }
    if (type == NULL) {
        error(ERR_OTHER, "Array or pointer base type is unknown");
    }
    if (v->type_class == TYPE_CLASS_POINTER) {
        if (v->loc && v->loc->pieces_cnt == 1 && v->loc->pieces->implicit_pointer) {
            v->loc->pieces->implicit_pointer--;
            assert(v->remote == 0);
            assert(v->value == NULL);
        }
        else {
            if (v->sym != NULL && v->size == 0 && get_symbol_size(v->sym, &v->size) < 0) {
                error(errno, "Cannot retrieve symbol size");
            }
            v->address = (ContextAddress)to_uns(mode, v);
            v->remote = 1;
            v->loc = NULL;
            v->value = NULL;
            v->constant = 0;
            set_value_endianness(v, NULL, type);
        }
        v->sym = NULL;
        v->reg = NULL;
        v->function = 0;
        v->func_cb = NULL;
        v->field_cb = NULL;
        v->sym_list = NULL;
    }
    v->type = type;
    if (get_symbol_type_class(v->type, &v->type_class) < 0) {
        error(errno, "Cannot retrieve symbol type class");
    }
    if (v->type_class == TYPE_CLASS_FUNCTION) {
        v->size = 0;
    }
    else if (get_symbol_size(v->type, &v->size) < 0) {
        error(errno, "Cannot retrieve symbol size");
    }
    set_value_props(v);
#else
    error(ERR_UNSUPPORTED, "Symbols service not available");
#endif
}

#if ENABLE_Symbols
static void evaluate_symbol_location(Symbol * sym, ContextAddress obj_addr,
        ContextAddress index, LocationExpressionState ** loc, int * be) {
    LocationInfo * loc_info = NULL;
    StackFrame * frame_info = NULL;
    uint64_t args[2];
    args[0] = obj_addr;
    args[1] = index;
    if (get_location_info(sym, &loc_info) < 0) {
        error(errno, "Cannot get symbol location information");
    }
    if (expression_frame != STACK_NO_FRAME && get_frame_info(expression_context, expression_frame, &frame_info) < 0) {
        error(errno, "Cannot get stack frame info");
    }
    *loc = evaluate_location_expression(expression_context, frame_info,
        loc_info->value_cmds.cmds, loc_info->value_cmds.cnt, args, 2);
    if (be != NULL) *be = loc_info->big_endian;
}

static void find_field(int mode,
        Symbol * class_sym, ContextAddress obj_addr, const char * name, const char * id,
        Symbol ** field_sym, LocationExpressionState ** field_loc, int * be) {
    Symbol ** children = NULL;
    Symbol ** inheritance = NULL;
    int count = 0;
    int h = 0;
    int i;

    if (get_symbol_children(class_sym, &children, &count) < 0) {
        error(errno, "Cannot retrieve field list");
    }
    for (i = 0; i < count; i++) {
        char * s = NULL;
        int sym_class = 0;
        if (get_symbol_class(children[i], &sym_class) < 0) {
            error(errno, "Cannot retrieve field symbol class");
        }
        if (get_symbol_name(children[i], &s) < 0) {
            error(errno, "Cannot retrieve field name");
        }
        if (s == NULL && sym_class == SYM_CLASS_REFERENCE) {
            if (inheritance == NULL) inheritance = (Symbol **)tmp_alloc(sizeof(Symbol *) * count);
            inheritance[h++] = children[i];
        }
        if ((name != NULL && s != NULL && strcmp(s, name) == 0) ||
                (id != NULL && strcmp(symbol2id(children[i]), id) == 0)) {
            if (mode == MODE_NORMAL) evaluate_symbol_location(children[i], obj_addr, 0, field_loc, be);
            *field_sym = children[i];
            return;
        }
        if (sym_class == SYM_CLASS_VARIANT_PART || sym_class == SYM_CLASS_VARIANT) {
            find_field(mode, children[i], obj_addr, name, id, field_sym, field_loc, be);
            if (*field_sym != NULL) return;
        }
    }
    for (i = 0; i < h; i++) {
        ContextAddress addr = obj_addr;
        if (mode == MODE_NORMAL) {
            LocationExpressionState * x = NULL;
            evaluate_symbol_location(inheritance[i], obj_addr, 0, &x, NULL);
            if (x->stk_pos != 1) error(ERR_INV_EXPRESSION, "Cannot evaluate symbol address");
            addr = (ContextAddress)x->stk[0];
        }
        find_field(mode, inheritance[i], addr, name, id, field_sym, field_loc, be);
        if (*field_sym != NULL) return;
    }
}
#endif

static void op_field(int mode, Value * v) {
    char * id = NULL;
    char * name = NULL;
    if (text_sy == SY_ID) id = (char *)text_val.value;
    else if (text_sy == SY_NAME) name = (char *)text_val.value;
    else error(ERR_INV_EXPRESSION, "Field name expected");
    next_sy();
    if (mode == MODE_SKIP) return;
    if (v->field_cb && name != NULL) {
        v->field_cb(mode, v, name);
    }
    else if (v->type_class == TYPE_CLASS_COMPOSITE) {
#if ENABLE_Symbols
        Symbol * sym = NULL;
        int sym_class = 0;
        LocationExpressionState * loc = NULL;
        int be = 0;
        void * struct_value = NULL;
        ContextAddress struct_size = 0;

        if (v->remote) {
            find_field(mode, v->type, v->address, name, id, &sym, &loc, &be);
        }
        else {
            load_value(v);
            struct_value = v->value;
            struct_size = v->size;
            find_field(mode, v->type, 0, name, id, &sym, &loc, &be);
        }
        if (sym == NULL) {
            error(ERR_SYM_NOT_FOUND, "Invalid field name or ID");
        }
        if (get_symbol_class(sym, &sym_class) < 0) {
            error(errno, "Cannot retrieve symbol class");
        }
        if (get_symbol_type(sym, &v->type) < 0) {
            error(errno, "Cannot retrieve symbol type");
        }
        v->reg = NULL;
        v->sym = NULL;
        v->sym_list = NULL;
        if (sym_class == SYM_CLASS_FUNCTION) {
            ContextAddress addr = 0;
            if (mode == MODE_NORMAL) {
                if (loc->stk_pos != 1) error(ERR_INV_EXPRESSION, "Invalid symbol location expression");
                addr = (ContextAddress)loc->stk[0];
            }
            v->type_class = TYPE_CLASS_POINTER;
            if (v->type != NULL && get_array_symbol(v->type, 0, &v->type)) {
                error(errno, "Cannot get function type");
            }
            set_ctx_word_value(v, addr);
            v->function = 1;
            v->sym = sym;
        }
        else {
            if (get_symbol_type_class(sym, &v->type_class) < 0) {
                error(errno, "Cannot retrieve symbol type class");
            }
            if (get_symbol_size(sym, &v->size) < 0) {
                error(errno, "Cannot retrieve field size");
            }
            if (mode == MODE_NORMAL) {
                if (struct_value == NULL && loc->pieces_cnt > 0) {
                    size_t size = 0;
                    void * value = NULL;
                    StackFrame * frame_info = NULL;
                    if (expression_frame != STACK_NO_FRAME && get_frame_info(expression_context, expression_frame, &frame_info) < 0) {
                        error(errno, "Cannot get stack frame info");
                    }
                    read_location_pieces(expression_context, frame_info,
                        loc->pieces, loc->pieces_cnt, be, &value, &size);
                    if (size > v->size) size = (size_t)v->size;
                    set_value(v, value, size, be);
                    sign_extend(v, loc);
                }
                else {
                    if (loc->stk_pos != 1) error(ERR_OTHER, "Invalid location expression");
                    if (struct_value != NULL) {
                        if (loc->stk[0] + v->size > struct_size) error(ERR_OTHER, "Invalid location expression");
                        v->value = (uint8_t *)struct_value + (size_t)loc->stk[0];
                        assert(!v->remote);
                    }
                    else {
                        v->address = (ContextAddress)loc->stk[0];
                        assert(v->remote);
                    }
                    set_value_endianness(v, sym, v->type);
                }
            }
        }
        v->loc = loc;
        if (sym_class == SYM_CLASS_REFERENCE && mode == MODE_NORMAL) {
            check_hidden_redirection(v);
        }
        set_value_props(v);
#else
        error(ERR_UNSUPPORTED, "Symbols service not available");
#endif
    }
    else if (v->reg != NULL) {
        if (id != NULL) {
            Context * ctx = NULL;
            int frame = STACK_NO_FRAME;
            RegisterDefinition * def = NULL;
            if (id2register(id, &ctx, &frame, &def) < 0) exception(errno);
            if (frame == STACK_TOP_FRAME) frame = expression_frame;
            reg2value(mode, ctx, frame, def, v);
        }
        else {
            RegisterDefinition * def = get_reg_definitions(v->loc->ctx);
            if (def != NULL) {
                while (def->name != NULL) {
                    if (def->parent == v->reg && strcmp(name, def->name) == 0) {
                        int frame = STACK_NO_FRAME;
                        if (v->loc->ctx == expression_context) frame = expression_frame;
                        reg2value(mode, v->loc->ctx, frame, def, v);
                        return;
                    }
                    def++;
                }
            }
            error(ERR_INV_EXPRESSION, "Unknown register: %s", name);
        }
    }
    else {
        error(ERR_INV_EXPRESSION, "Composite type expected");
    }
}

static void op_index(int mode, Value * v) {
#if ENABLE_Symbols
    Value i;
    ContextAddress size = 0;
    Symbol * type = NULL;
    int type_class = 0;

    expression(mode, &i);
    if (mode == MODE_SKIP) return;

    if (v->type_class != TYPE_CLASS_ARRAY && v->type_class != TYPE_CLASS_POINTER) {
        error(ERR_INV_EXPRESSION, "Array or pointer expected");
    }
    if (v->type == NULL) {
        error(ERR_INV_EXPRESSION, "Value type is unknown");
    }
    if (get_symbol_base_type(v->type, &type) < 0) {
        error(errno, "Cannot get array element type");
    }
    if (type == NULL) {
        error(ERR_INV_EXPRESSION, "Array element type is unknown");
    }
    if (get_symbol_type_class(type, &type_class) < 0) {
        error(errno, "Cannot get type class of the array element");
    }
    if (v->type_class == TYPE_CLASS_POINTER) {
        if (v->loc && v->loc->pieces_cnt == 1 && v->loc->pieces->implicit_pointer) {
            v->loc->pieces->implicit_pointer--;
        }
        else {
            /* Note: v->size is not set yet if v->sym != NULL */
            if (v->sym != NULL && v->size == 0 && get_symbol_size(v->sym, &v->size) < 0) {
                error(errno, "Cannot retrieve symbol size");
            }
            v->address = (ContextAddress)to_uns(mode, v);
            v->remote = 1;
            v->loc = NULL;
            v->value = NULL;
            v->constant = 0;
            set_value_endianness(v, NULL, type);
        }
        v->sym = NULL;
        v->reg = NULL;
        v->function = 0;
        v->func_cb = NULL;
        v->field_cb = NULL;
        v->sym_list = NULL;
    }
    if (type_class == TYPE_CLASS_FUNCTION) {
        size = 0;
    }
    else if (get_symbol_size(type, &size) < 0) {
        error(errno, "Cannot get array element size");
    }

    if (mode == MODE_NORMAL) {
        int64_t index = to_int(mode, &i);
        ContextAddress byte_offs = 0;
        ContextAddress bit_offs = 0;
        int64_t lower_bound = 0;
        if (v->type_class == TYPE_CLASS_ARRAY) {
            if (get_symbol_lower_bound(v->type, &lower_bound) < 0) {
                error(errno, "Cannot get array lower bound");
            }
            if (index < lower_bound) {
                error(ERR_INV_EXPRESSION, "Invalid index");
            }
        }
        if (v->bit_stride > 0) {
            bit_offs = (ContextAddress)(index - lower_bound) * v->bit_stride;
        }
        else {
            byte_offs = (ContextAddress)(index - lower_bound) * size;
        }
        if (v->remote && v->bit_stride == 0) {
            assert(bit_offs == 0);
            v->address += byte_offs;
        }
        else {
            if (v->sym != NULL && v->size == 0 && get_symbol_size(v->sym, &v->size) < 0) {
                error(errno, "Cannot retrieve symbol size");
            }
            load_value(v);
            v->value = (char *)v->value + byte_offs;
            if (v->bit_stride > 0) {
                unsigned x;
                uint8_t * buf = (uint8_t *)tmp_alloc_zero((size_t)size);
                uint8_t * val = (uint8_t *)v->value;
                unsigned buf_offs = v->big_endian ? (unsigned)(size * 8 - v->bit_stride) : 0;
                v->value = buf;
                for (x = 0; x < v->bit_stride; x++) {
                    unsigned y = (unsigned)(x + bit_offs);
                    unsigned z = (unsigned)(x + buf_offs);
                    if (val[y / 8] & bit_mask(v, y)) buf[z / 8] |= bit_mask(v, z);
                }
                if (type_class == TYPE_CLASS_INTEGER) {
                    /* Sign extension */
                    bit_sign_extend(v, v->bit_stride);
                }
            }
        }
    }
    v->sym_list = NULL;
    v->sym = NULL;
    v->reg = NULL;
    v->loc = NULL;
    v->size = size;
    v->type = type;
    v->type_class = type_class;
    set_value_props(v);
#else
    error(ERR_UNSUPPORTED, "Symbols service not available");
#endif
}

static void op_addr(int mode, Value * v) {
    if (mode == MODE_SKIP) return;
    if (v->function) {
        assert(v->type_class == TYPE_CLASS_POINTER);
    }
    else if (v->remote) {
        set_ctx_word_value(v, v->address);
        v->type_class = TYPE_CLASS_POINTER;
        v->constant = 0;
#if ENABLE_Symbols
        if (v->type != NULL) {
            if (get_array_symbol(v->type, 0, &v->type)) {
                error(errno, "Cannot get pointer type");
            }
        }
#else
        v->type = NULL;
#endif
    }
    else if (v->reg != NULL && v->reg->memory_context != NULL) {
        Context * ctx = id2ctx(v->reg->memory_context);
        set_ctx_word_value(v, v->reg->memory_address);
        v->type_class = TYPE_CLASS_POINTER;
        v->constant = 1;
        if (ctx != expression_context) {
            /* The address is in another address space */
            v->loc = (LocationExpressionState *)tmp_alloc_zero(sizeof(LocationExpressionState));
            v->loc->ctx = ctx;
            v->loc->pieces = (LocationPiece *)tmp_alloc_zero(sizeof(LocationPiece));
            v->loc->pieces_cnt = v->loc->pieces_max = 1;
            v->loc->pieces->value = v->value;
            v->loc->pieces->size = (size_t)v->size;
        }
    }
    else if (v->loc != NULL && v->loc->pieces_cnt == 1 &&
            v->loc->pieces->implicit_pointer == 0 && v->loc->pieces->optimized_away == 0 &&
            v->loc->pieces->reg == NULL && v->loc->pieces->value == NULL && v->loc->pieces->bit_offs == 0) {
        set_ctx_word_value(v, v->loc->pieces->addr);
        v->type_class = TYPE_CLASS_POINTER;
        v->constant = 0;
        v->type = NULL;
    }
    else {
        error(ERR_INV_EXPRESSION, "Invalid '&': the value has no address");
    }
}

static void unary_expression(int mode, Value * v);

static void op_sizeof(int mode, Value * v) {
    Symbol * type = NULL;
    int pos = 0;
    int p = text_sy == '(';

    if (p) next_sy();
    pos = sy_pos;
    if (type_name(mode, &type) && (!p || text_sy == ')')) {
        if (mode != MODE_SKIP) {
            ContextAddress type_size = 0;
            Symbol * s_type = NULL;
            size_t s_size = 0;
#if ENABLE_Symbols
            if (get_symbol_size(type, &type_size) < 0) {
                error(errno, "Cannot retrieve symbol size");
            }
            get_std_type("size_t", TYPE_CLASS_CARDINAL, &s_type, &s_size);
#endif
            v->type = s_type;
            v->type_class = TYPE_CLASS_CARDINAL;
            if (s_size == 0) s_size = context_word_size(expression_context);
            set_int_value(v, s_size, type_size);
            v->constant = 1;
        }
    }
    else {
        text_pos = pos;
        next_ch();
        next_sy();
        unary_expression(mode == MODE_NORMAL ? MODE_TYPE : mode, v);
        if (mode != MODE_SKIP) {
            Symbol * s_type = NULL;
            size_t s_size = 0;
#if ENABLE_Symbols
            get_std_type("size_t", TYPE_CLASS_CARDINAL, &s_type, &s_size);
#endif
            v->type = s_type;
            v->type_class = TYPE_CLASS_CARDINAL;
            if (s_size == 0) s_size = context_word_size(expression_context);
            set_int_value(v, s_size, v->size);
            v->constant = 1;
        }
    }
    if (p) {
        if (text_sy != ')') error(ERR_INV_EXPRESSION, "')' expected");
        next_sy();
    }
}

static void funccall_error(const char * msg) {
    set_errno(ERR_OTHER, msg);
    set_errno(errno, "Cannot inject a function call");
    exception(errno);
}

#if ENABLE_FuncCallInjection

static void free_funccall_state(FuncCallState * state) {
    assert(!state->started || state->intercepted);
    assert(state->committed);
    assert(state->regs_cnt == 0 || state->error);
    list_remove(&state->link_all);
    if (state->bp) destroy_eventpoint(state->bp);
    context_unlock(state->ctx);
    release_error_report(state->error);
    cache_dispose(&state->cache);
    loc_free(state->ret_value);
    loc_free(state->args);
    loc_free(state->cmds);
    loc_free(state->regs);
    loc_free(state);
}

static void funcccall_breakpoint(Context * ctx, void * args) {
    Trap trap;
    FuncCallState * state = (FuncCallState *)args;
    assert(state->ctx == ctx);
    assert(state->started);
    assert(!state->finished);
    ctx->stopped_by_funccall = 1;
    if (set_trap(&trap)) {
        if (!state->intercepted && state->cmds_cnt > 0) {
            /* Execute after call commands */
            StackFrame * frame_info = NULL;
            LocationExpressionState * vm = NULL;
            if (get_frame_info(ctx, STACK_TOP_FRAME, &frame_info) < 0) exception(errno);
            vm = evaluate_location_expression(ctx, frame_info,
                    state->cmds, state->cmds_cnt, NULL, 0);
            state->cmds_cnt = 0;

            /* Read function call returned value */
            if (vm->pieces_cnt > 0) {
                void * value = NULL;
                read_location_pieces(ctx, frame_info, vm->pieces, vm->pieces_cnt,
                        state->ret_big_endian, &value, &state->ret_size);
                state->ret_value = loc_alloc_zero(state->ret_size);
                memcpy(state->ret_value, value, state->ret_size);
            }
            else if (vm->stk_pos > 0) {
                state->ret_size = sizeof(uint64_t);
                state->ret_value = loc_alloc_zero(state->ret_size);
                memcpy(state->ret_value, vm->stk + vm->stk_pos - 1, state->ret_size);
            }
        }
        if (state->regs_cnt > 0) {
            /* Restore registers */
            unsigned i;
            unsigned offs = 0;
            for (i = 0; i < state->regs_cnt; i++) {
                RegisterDefinition * r = state->regs[i];
                if (context_write_reg(ctx, r, 0, r->size, state->regs_data + offs) < 0) exception(errno);
#if SERVICE_Registers
                send_event_register_changed(register2id(ctx, STACK_TOP_FRAME, r));
#endif
                offs += r->size;
            }
            state->regs_cnt = 0;
        }
        clear_trap(&trap);
    }
    else {
        release_error_report(state->error);
        state->error = get_error_report(trap.error);
    }
    if (!state->intercepted) {
        assert(!state->finished);
        state->finished = 1;
        suspend_debug_context(ctx);
    }
    else if (state->committed) {
        if (state->error) trace(LOG_ALWAYS, "Cannot restore state: %s",
            errno_to_str(set_error_report_errno(state->error)));
        free_funccall_state(state);
    }
}

static void funccall_check_recursion(uint64_t ret_addr) {
    LINK * l = func_call_state.next;
    while (l != &func_call_state) {
        FuncCallState * state = link_all2fc(l);
        if (state->started && !state->finished &&
                state->ctx == expression_context && state->ret_addr == ret_addr) {
            funccall_error("Recursive invocation");
        }
        l = l->next;
    }
}

static void op_funccall(int mode, Value * v) {
    unsigned id = cache_transaction_id();
    FuncCallState * state = NULL;
    Symbol * func = NULL;
    int type_class = 0;
    LINK * l;

    if (!context_has_state(expression_context)) funccall_error("Context is not a thread");
    if (is_safe_event()) funccall_error("Called from safe event handler");

    for (l = func_call_state.next; l != &func_call_state; l = l->next) {
        FuncCallState * s = link_all2fc(l);
        if (s->id == id && s->pos == text_pos) {
            state = s;
            break;
        }
    }

    if (state != NULL && state->started && !state->intercepted) cache_wait(&state->cache);
    if (state == NULL) {
        state = (FuncCallState *)loc_alloc_zero(sizeof(FuncCallState));
        state->id = id;
        state->pos = text_pos;
        context_lock(state->ctx = expression_context);
        list_add_first(&state->link_all, &func_call_state);
    }
    if (v->function) {
        func = v->sym;
    }
    else if (v->type != NULL && v->type_class == TYPE_CLASS_POINTER) {
        if (get_symbol_base_type(v->type, &func) < 0) {
            error(errno, "Cannot retrieve symbol base type");
        }
    }
    if (func != NULL && get_symbol_type_class(func, &type_class) < 0) {
        error(errno, "Cannot retrieve symbol type class");
    }
    if (type_class != TYPE_CLASS_FUNCTION) {
        error(ERR_INV_EXPRESSION, "Invalid '()': not a function");
    }
    if (get_symbol_address(func, &state->func_addr) < 0) {
        error(errno, "Cannot retrieve function address");
    }
    state->args_cnt = 0;
    if (text_sy != ')') {
        int args_mode = mode;
        if (state->started) args_mode = MODE_SKIP;
        for (;;) {
            if (state->args_cnt >= state->args_max) {
                state->args_max += 8;
                state->args = (Value *)loc_realloc(state->args, sizeof(Value) * state->args_max);
            }
            ini_value(state->args + state->args_cnt);
            expression(args_mode, state->args + state->args_cnt++);
            if (text_sy != ',') break;
            next_sy();
        }
    }
    if (get_symbol_base_type(func, &v->type) < 0) {
        error(errno, "Cannot retrieve function return type");
    }
    if (get_symbol_type_class(v->type, &v->type_class) < 0) {
        error(errno, "Cannot retrieve function return type class");
    }
    if (get_symbol_size(v->type, &v->size) < 0) {
        error(errno, "Cannot retrieve function return value size");
    }
    expression_has_func_call = 1;
    if (mode == MODE_NORMAL) {
        if (!state->started) {
            unsigned i;
            StackFrame * frame_info = NULL;
            FunctionCallInfo * call_info = NULL;
            LocationExpressionState * vm = NULL;
            RegisterDefinition * reg_pc = get_PC_definition(state->ctx);
            const Symbol ** arg_types = (const Symbol **)tmp_alloc_zero(sizeof(Symbol *) * state->args_cnt);
            uint64_t * arg_vals = (uint64_t *)tmp_alloc_zero(sizeof(uint64_t) * (FUNCCALL_ARG_ARGS + state->args_cnt));
            uint64_t sp = 0;

            if (get_frame_info(state->ctx, STACK_TOP_FRAME, &frame_info) < 0) exception(errno);
            if (read_reg_value(frame_info, reg_pc, &state->ret_addr) < 0) exception(errno);
            funccall_check_recursion(state->ret_addr);
            for (i = 0; i < state->args_cnt; i++) arg_types[i] = state->args[i].type;
            if (get_funccall_info(func, arg_types, state->args_cnt, &call_info) < 0) exception(errno);

            /* Save registers */
            if (call_info->saveregs_cnt > 0) {
                unsigned offs = 0;
                for (i = 0; i < call_info->saveregs_cnt; i++) offs += call_info->saveregs[i]->size;
                state->regs = (RegisterDefinition **)loc_alloc(sizeof(RegisterDefinition *) * call_info->saveregs_cnt);
                state->regs_data = (uint8_t *)loc_alloc_zero(offs);
                state->regs_cnt = call_info->saveregs_cnt;
                offs = 0;
                for (i = 0; i < call_info->saveregs_cnt; i++) {
                    RegisterDefinition * r = call_info->saveregs[i];
                    state->regs[i] = r;
                    if (context_read_reg(state->ctx, r, 0, r->size, state->regs_data + offs) < 0) exception(errno);
                    offs += r->size;
                }
            }

            /* get values of actual arguments */
            arg_vals[FUNCCALL_ARG_ADDR] = state->func_addr;
            arg_vals[FUNCCALL_ARG_RET] = state->ret_addr;
            if (read_reg_value(frame_info, call_info->stak_pointer, &sp) < 0) exception(errno);
            sp -= call_info->red_zone_size;
            for (i = 0; i < state->args_cnt; i++) {
                Value * arg = state->args + i;
                switch (arg->type_class) {
                case TYPE_CLASS_CARDINAL:
                case TYPE_CLASS_INTEGER:
                case TYPE_CLASS_POINTER:
                case TYPE_CLASS_ENUMERATION:
                    arg_vals[FUNCCALL_ARG_ARGS + i] = to_uns(MODE_NORMAL, arg);
                    break;
                default:
                    if (arg->remote) {
                        arg_vals[FUNCCALL_ARG_ARGS + i] = arg->address;
                    }
                    else {
                        sp -= arg->size;
                        while (sp % 8) sp--;
                        if (context_write_mem(state->ctx, (ContextAddress)sp,
                                arg->value, (size_t)arg->size) < 0) exception(errno);
                        arg_vals[FUNCCALL_ARG_ARGS + i] = sp;
                    }
                    break;
                }
            }
            if (write_reg_value(frame_info, call_info->stak_pointer, sp) < 0) exception(errno);

            /* Execute call injection commands */
            state->started = 1;
            vm = evaluate_location_expression(state->ctx, frame_info,
                    call_info->cmds, call_info->cmds_cnt, arg_vals, FUNCCALL_ARG_ARGS + state->args_cnt);
            state->ret_big_endian = call_info->scope.big_endian;
            if (vm->sft_cmd != NULL) {
                char ret_addr[64];
                if (vm->sft_cmd->cmd != SFT_CMD_FCALL || vm->stk_pos != 0) {
                    funccall_error("Invalid SFT instruction");
                }

                /* Create breakpoint at the function return address */
                assert(state->bp == NULL);
                snprintf(ret_addr, sizeof(ret_addr), "%#" PRIx64, state->ret_addr);
                state->bp = create_eventpoint(ret_addr, state->ctx, funcccall_breakpoint, state);

                /* Set PC to the function address */
                if (write_reg_value(frame_info, reg_pc, state->func_addr) < 0) exception(errno);
                state->ctx->stopped_by_bp = 0;

                /* Save rest of func call commands to be executed after the function returns */
                state->cmds_cnt = call_info->cmds_cnt - (vm->sft_cmd - call_info->cmds) - 1;
                state->cmds = (LocationExpressionCommand *)loc_alloc(sizeof(LocationExpressionCommand) * state->cmds_cnt);
                memcpy(state->cmds, vm->sft_cmd + 1, sizeof(LocationExpressionCommand) * state->cmds_cnt);

                /* Resume debug context */
                if (continue_debug_context(state->ctx, cache_channel(), RM_RESUME, 1, 0, 0) < 0) exception(errno);

                /* Wait until the function returns */
                cache_wait(&state->cache);
            }
            else {
                state->finished = 1;
            }
        }
        assert(state->started);
        assert(state->intercepted);
        if (state->error) exception(set_error_report_errno(state->error));
        set_value(v, state->ret_value, state->ret_size, state->ret_big_endian);
    }
    else {
        set_value(v, NULL, (size_t)v->size, 0);
    }
    set_value_props(v);
}

#else

static void op_funccall(int mode, Value * v) {
    funccall_error("Symbols service not available");
}

#endif /* ENABLE_FuncCallInjection */

static void op_call(int mode, Value * v) {
    if (v->func_cb || mode == MODE_SKIP) {
        Value * args = NULL;
        unsigned args_cnt = 0;
        unsigned args_max = 0;

        if (text_sy != ')') {
            for (;;) {
                if (args_cnt == args_max) {
                    args_max += 32;
                    args = (Value *)tmp_realloc(args, sizeof(Value) * args_max);
                }
                ini_value(args + args_cnt);
                expression(mode, args + args_cnt++);
                if (text_sy != ',') break;
                next_sy();
            }
        }
        if (mode == MODE_SKIP) {
            ini_value(v);
            return;
        }
        if (mode == MODE_NORMAL) {
            unsigned i;
            for (i = 0; i < args_cnt; i++) load_value(args + i);
        }
        v->func_cb(mode, v, args, args_cnt);
    }
    else {
        op_funccall(mode, v);
    }
}

static void resolve_ref_type(int mode, Value * v) {
#if ENABLE_Symbols
    Symbol * type = v->type;
    if (type == NULL || v->type_class != TYPE_CLASS_POINTER) return;
    for (;;) {
        SYM_FLAGS flags = 0;
        Symbol * next = NULL;
        if (get_symbol_flags(type, &flags) < 0) {
            error(errno, "Cannot retrieve symbol flags");
        }
        if (flags & SYM_FLAG_REFERENCE) {
            op_deref(mode, v);
            break;
        }
        if (get_symbol_type(type, &next) < 0) {
            error(errno, "Cannot retrieve symbol type");
        }
        if (next == NULL || next == type) break;
        type = next;
    }
#endif
}

static void postfix_expression(int mode, Value * v) {
    primary_expression(mode, v);
    for (;;) {
        resolve_ref_type(mode, v);
        if (text_sy == '.') {
            next_sy();
            op_field(mode, v);
        }
        else if (text_sy == '[') {
            next_sy();
            op_index(mode, v);
            if (text_sy != ']') {
                error(ERR_INV_EXPRESSION, "']' expected");
            }
            next_sy();
        }
        else if (text_sy == SY_REF) {
            next_sy();
            op_deref(mode, v);
            resolve_ref_type(mode, v);
            op_field(mode, v);
        }
        else if (text_sy == '(') {
            next_sy();
            op_call(mode, v);
            if (text_sy != ')') {
                error(ERR_INV_EXPRESSION, "')' expected");
            }
            next_sy();
        }
        else {
            if (v->func_cb) error(ERR_INV_EXPRESSION, "'(' expected");
            break;
        }
    }
}

static void set_bool_value(Value * v, uint64_t n) {
    Symbol * type = NULL;
    int type_class = TYPE_CLASS_ENUMERATION;
    size_t size = 0;
#if ENABLE_Symbols
    if (!get_std_type("bool", TYPE_CLASS_ENUMERATION, &type, &size)) {
        get_std_type("int", TYPE_CLASS_INTEGER, &type, &size);
        type_class = TYPE_CLASS_INTEGER;
    }
#endif
    if (size == 0) size = context_word_size(expression_context);
    v->type = type;
    v->type_class = type_class;
    set_int_value(v, size, n);
}

/* Note: lazy_unary_expression() does not set v->size if v->sym != NULL */
static void lazy_unary_expression(int mode, Value * v) {
    switch (text_sy) {
    case '*':
        next_sy();
        lazy_unary_expression(mode, v);
        op_deref(mode, v);
        break;
    case '&':
        next_sy();
        lazy_unary_expression(mode, v);
        op_addr(mode, v);
        break;
    case SY_SIZEOF:
        next_sy();
        op_sizeof(mode, v);
        break;
    case '+':
        next_sy();
        lazy_unary_expression(mode, v);
        break;
    case '-':
        next_sy();
        unary_expression(mode, v);
        if (mode != MODE_SKIP) {
            if (!is_number(v)) {
                error(ERR_INV_EXPRESSION, "Numeric types expected");
            }
            else if (v->type_class == TYPE_CLASS_REAL) {
                set_fp_value(v, (size_t)v->size, -to_double(mode, v));
            }
            else if (is_real_number(v)) {
                double n = -to_double(mode, v);
                v->type = NULL;
                v->type_class = TYPE_CLASS_REAL;
                set_fp_value(v, sizeof(double), n);
                set_fp_type(v);
            }
            else if (v->type_class == TYPE_CLASS_COMPLEX) {
                set_complex_value(v, (size_t)v->size, -to_r_double(mode, v), -to_i_double(mode, v));
            }
            else if (v->type_class != TYPE_CLASS_CARDINAL) {
                int64_t value = -to_int(mode, v);
                if (v->type_class == TYPE_CLASS_INTEGER) {
                    set_int_value(v, (size_t)v->size, value);
                }
                else {
                    v->type_class = TYPE_CLASS_INTEGER;
                    set_int_value(v, context_word_size(expression_context), value);
                    v->type = NULL;
                }
            }
            assert(!v->remote);
        }
        break;
    case '!':
        next_sy();
        unary_expression(mode, v);
        if (mode != MODE_SKIP) {
            if (!is_whole_number(v)) {
                error(ERR_INV_EXPRESSION, "Integral types expected");
            }
            else {
                set_bool_value(v, !to_int(mode, v));
            }
            assert(!v->remote);
        }
        break;
    case '~':
        next_sy();
#if ENABLE_Symbols
        /* Check for C++ destructor */
        if (text_sy == SY_NAME) {
            Value type;
            int sym_class = identifier(mode, NULL, (char *)text_val.value, SYM_FLAG_TYPE, &type);
            if (sym_class == SYM_CLASS_TYPE && type.type_class == TYPE_CLASS_COMPOSITE) {
                char * name = tmp_strdup2("~", (char *)text_val.value);
                sym_class = identifier(mode, &type, name, 0, v);
                if (sym_class < 0) error(ERR_INV_EXPRESSION, "Undefined identifier '%s'", name);
                next_sy();
                break;
            }
        }
#endif
        unary_expression(mode, v);
        if (mode != MODE_SKIP) {
            if (!is_whole_number(v)) {
                error(ERR_INV_EXPRESSION, "Integral types expected");
            }
            else {
                int64_t value = ~to_int(mode, v);
                set_int_value(v, (size_t)v->size, value);
            }
            assert(!v->remote);
        }
        break;
#if ENABLE_Symbols
    case '(':
    {
        Symbol * type = NULL;
        int type_class = TYPE_CLASS_UNKNOWN;
        ContextAddress type_size = 0;
        int pos = sy_pos;

        assert(text[pos] == '(');
        next_sy();
        if (!type_name(mode, &type)) {
            text_pos = pos;
            next_ch();
            next_sy();
            assert(text_sy == '(');
            postfix_expression(mode, v);
            break;
        }
        if (text_sy != ')') error(ERR_INV_EXPRESSION, "')' expected");
        next_sy();
        unary_expression(mode, v);
        if (mode == MODE_SKIP) break;
        if (get_symbol_type_class(type, &type_class) < 0) {
            error(errno, "Cannot retrieve symbol type class");
        }
        if (get_symbol_size(type, &type_size) < 0) {
            error(errno, "Cannot retrieve symbol size");
        }
        if (v->remote && v->size == type_size) {
            /* A type cast can be an l-value expression as long as the size does not change */
            int ok = 0;
            switch (type_class) {
            case TYPE_CLASS_CARDINAL:
            case TYPE_CLASS_POINTER:
            case TYPE_CLASS_INTEGER:
            case TYPE_CLASS_ENUMERATION:
            case TYPE_CLASS_COMPOSITE:
                switch (v->type_class) {
                case TYPE_CLASS_CARDINAL:
                case TYPE_CLASS_POINTER:
                case TYPE_CLASS_INTEGER:
                case TYPE_CLASS_ENUMERATION:
                case TYPE_CLASS_UNKNOWN:
                case TYPE_CLASS_COMPOSITE:
                    ok = 1;
                    break;
                }
                break;
            case TYPE_CLASS_REAL:
                ok = v->type_class == TYPE_CLASS_REAL;
                break;
            }
            if (ok) {
                v->type = type;
                v->type_class = type_class;
                break;
            }
        }
        switch (type_class) {
        case TYPE_CLASS_UNKNOWN:
            error(ERR_INV_EXPRESSION, "Unknown type class");
            break;
        case TYPE_CLASS_CARDINAL:
        case TYPE_CLASS_POINTER:
            {
                uint64_t value = to_uns(mode, v);
                v->type = type;
                v->type_class = type_class;
                set_int_value(v, (size_t)type_size, value);
            }
            break;
        case TYPE_CLASS_INTEGER:
        case TYPE_CLASS_ENUMERATION:
            {
                int64_t value = to_int(mode, v);
                v->type = type;
                v->type_class = type_class;
                set_int_value(v, (size_t)type_size, value);
            }
            break;
        case TYPE_CLASS_REAL:
            {
                double value = to_double(mode, v);
                v->type = type;
                v->type_class = type_class;
                set_fp_value(v, (size_t)type_size, value);
                set_fp_type(v);
            }
            break;
        case TYPE_CLASS_ARRAY:
            if (v->type_class == TYPE_CLASS_POINTER) {
                if (v->loc && v->loc->pieces_cnt == 1 && v->loc->pieces->implicit_pointer) {
                    v->loc->pieces->implicit_pointer--;
                }
                else {
                    v->address = (ContextAddress)to_uns(mode, v);
                    v->remote = 1;
                    v->value = NULL;
                    v->size = type_size;
                    v->big_endian = expression_context->big_endian;
                    v->constant = 0;
                }
                v->sym = NULL;
                v->reg = NULL;
                v->function = 0;
                v->func_cb = NULL;
                v->field_cb = NULL;
                v->sym_list = NULL;
                v->type = type;
                v->type_class = type_class;
            }
            else if (v->remote) {
                v->sym_list = NULL;
                v->sym = NULL;
                v->type = type;
                v->size = type_size;
                v->type_class = type_class;
            }
            else {
                error(ERR_INV_EXPRESSION, "Invalid type cast: illegal source type");
            }
            break;
        default:
            error(ERR_INV_EXPRESSION, "Invalid type cast: illegal destination type");
            break;
        }
        break;
    }
#endif
    default:
        postfix_expression(mode, v);
        break;
    }
}

static void unary_expression(int mode, Value * v) {
    lazy_unary_expression(mode, v);
#if ENABLE_Symbols
    if (mode != MODE_SKIP && v->sym != NULL && v->size == 0 && get_symbol_size(v->sym, &v->size) < 0) {
        error(errno, "Cannot retrieve symbol size");
    }
#endif
}

static void pm_expression(int mode, Value * v) {
    unary_expression(mode, v);
#if ENABLE_Symbols
    while (text_sy == SY_PM_D || text_sy == SY_PM_R) {
        Value * x = (Value *)tmp_alloc_zero(sizeof(Value));
        int sy = text_sy;
        next_sy();
        unary_expression(mode, x);
        if (x->type == NULL || x->type_class != TYPE_CLASS_MEMBER_PTR) {
            error(ERR_INV_EXPRESSION, "Invalid type: pointer to member expected");
        }
        if (mode != MODE_SKIP) {
            if (mode == MODE_NORMAL) {
                ContextAddress obj = 0;
                ContextAddress ptr = 0;
                LocationExpressionState * loc = NULL;
                if (sy == SY_PM_D) {
                    if (!v->remote) error(ERR_INV_EXPRESSION, "L-value expected");
                    obj = v->address;
                }
                else {
                    obj = (ContextAddress)to_uns(mode, v);
                }
                ptr = (ContextAddress)to_uns(mode, x);
                evaluate_symbol_location(x->type, obj, ptr, &loc, NULL);
                if (loc->stk_pos != 1) error(ERR_INV_EXPRESSION, "Cannot evaluate symbol address");
                v->address = (ContextAddress)loc->stk[0];
            }
            else {
                v->address = 0;
            }
            v->sym_list = NULL;
            v->sym = NULL;
            v->reg = NULL;
            v->loc = NULL;
            v->remote = 1;
            v->function = 0;
            v->func_cb = NULL;
            v->field_cb = NULL;
            v->value = NULL;
            v->constant = 0;
            if (get_symbol_base_type(x->type, &v->type) < 0) {
                error(ERR_INV_EXPRESSION, "Cannot get pointed type");
            }
            if (get_symbol_type_class(v->type, &v->type_class) < 0) {
                error(ERR_INV_EXPRESSION, "Cannot get pointed type class");
            }
            if (get_symbol_size(v->type, &v->size) < 0) {
                error(errno, "Cannot retrieve field size");
            }
            set_value_endianness(v, x->type, v->type);
        }
    }
#endif
}

static void multiplicative_expression(int mode, Value * v) {
    pm_expression(mode, v);
    while (text_sy == '*' || text_sy == '/' || text_sy == '%') {
        Value * x = (Value *)tmp_alloc_zero(sizeof(Value));
        int sy = text_sy;
        next_sy();
        pm_expression(mode, x);
        if (mode != MODE_SKIP) {
            if (!is_number(v) || !is_number(x)) {
                error(ERR_INV_EXPRESSION, "Numeric types expected");
            }
            else if (v->type_class == TYPE_CLASS_COMPLEX || x->type_class == TYPE_CLASS_COMPLEX) {
                double r_value = 0;
                double i_value = 0;
                if (mode == MODE_NORMAL) {
                    double d = 0;
                    switch (sy) {
                    case '*':
                        r_value =
                            to_r_double(mode, v) * to_r_double(mode, x) -
                            to_i_double(mode, v) * to_i_double(mode, x);
                        i_value =
                            to_r_double(mode, v) * to_i_double(mode, x) +
                            to_i_double(mode, v) * to_r_double(mode, x);
                        break;
                    case '/':
                        d =
                            to_r_double(mode, x) * to_r_double(mode, x) +
                            to_i_double(mode, x) * to_i_double(mode, x);
                        r_value =
                            (to_r_double(mode, v) * to_r_double(mode, x) +
                            to_i_double(mode, v) * to_i_double(mode, x)) / d;
                        i_value =
                            (to_i_double(mode, v) * to_r_double(mode, x) -
                            to_r_double(mode, v) * to_i_double(mode, x)) / d;
                        break;
                    default:
                        error(ERR_INV_EXPRESSION, "Invalid type");
                    }
                }
                v->type = NULL;
                v->type_class = TYPE_CLASS_COMPLEX;
                set_complex_value(v, sizeof(double) * 2, r_value, i_value);
                set_complex_type(v);
            }
            else if (is_real_number(v) || is_real_number(x)) {
                double value = 0;
                if (mode == MODE_NORMAL) {
                    switch (sy) {
                    case '*': value = to_double(mode, v) * to_double(mode, x); break;
                    case '/': value = to_double(mode, v) / to_double(mode, x); break;
                    default: error(ERR_INV_EXPRESSION, "Invalid type");
                    }
                }
                v->type = NULL;
                v->type_class = TYPE_CLASS_REAL;
                set_fp_value(v, sizeof(double), value);
                set_fp_type(v);
            }
            else if (v->type_class == TYPE_CLASS_CARDINAL || x->type_class == TYPE_CLASS_CARDINAL) {
                uint64_t value = 0;
                if (mode == MODE_NORMAL) {
                    uint64_t a = to_uns(mode, v);
                    uint64_t b = to_uns(mode, x);
                    if (sy != '*' && b == 0) error(ERR_INV_EXPRESSION, "Dividing by zero");
                    switch (sy) {
                    case '*': value = a * b; break;
                    case '/': value = a / b; break;
                    case '%': value = a % b; break;
                    }
                }
                v->type = NULL;
                v->type_class = TYPE_CLASS_CARDINAL;
                set_int_value(v, sizeof(uint64_t), value);
            }
            else {
                int64_t value = 0;
                if (mode == MODE_NORMAL) {
                    int64_t a = to_int(mode, v);
                    int64_t b = to_int(mode, x);
                    if (sy != '*' && b == 0) error(ERR_INV_EXPRESSION, "Dividing by zero");
                    switch (sy) {
                    case '*': value = a * b; break;
                    case '/': value = a / b; break;
                    case '%': value = a % b; break;
                    }
                }
                v->type = NULL;
                v->type_class = TYPE_CLASS_INTEGER;
                set_int_value(v, sizeof(int64_t), value);
            }
            v->constant = v->constant && x->constant;
        }
    }
}

static void additive_expression(int mode, Value * v) {
    multiplicative_expression(mode, v);
    while (text_sy == '+' || text_sy == '-') {
        Value * x = (Value *)tmp_alloc_zero(sizeof(Value));
        int sy = text_sy;
        next_sy();
        multiplicative_expression(mode, x);
        if (mode != MODE_SKIP) {
            if (v->function) {
                v->type_class = TYPE_CLASS_CARDINAL;
                v->type = NULL;
            }
            if (x->function) {
                x->type_class = TYPE_CLASS_CARDINAL;
                x->type = NULL;
            }
            if (sy == '+' && v->type_class == TYPE_CLASS_ARRAY && x->type_class == TYPE_CLASS_ARRAY) {
                if (mode == MODE_TYPE) {
                    v->remote = 0;
                    v->size = 0;
                    v->value = tmp_alloc_zero((size_t)v->size);
                }
                else {
                    char * value;
                    load_value(v);
                    load_value(x);
                    v->size = strlen((char *)v->value) + strlen((char *)x->value) + 1;
                    value = (char *)tmp_alloc((size_t)v->size);
                    strcpy(value, (const char *)(v->value));
                    strcat(value, (const char *)(x->value));
                    v->value = value;
                }
                v->type = NULL;
            }
#if ENABLE_Symbols
            else if ((v->type_class == TYPE_CLASS_POINTER || v->type_class == TYPE_CLASS_ARRAY) && is_number(x)) {
                uint64_t value = 0;
                Symbol * base = NULL;
                ContextAddress size = 0;
                if (v->type == NULL || get_symbol_base_type(v->type, &base) < 0 ||
                    base == NULL || get_symbol_size(base, &size) < 0 || size == 0) {
                    error(ERR_INV_EXPRESSION, "Unknown pointer base type size");
                }
                switch (sy) {
                case '+': value = to_uns(mode, v) + to_uns(mode, x) * size; break;
                case '-': value = to_uns(mode, v) - to_uns(mode, x) * size; break;
                }
                if (v->type_class == TYPE_CLASS_ARRAY) {
                    if (get_array_symbol(base, 0, &v->type) < 0 ||
                        get_symbol_size(v->type, &v->size) < 0) {
                        error(errno, "Cannot cast to pointer");
                    }
                    v->type_class = TYPE_CLASS_POINTER;
                }
                set_int_value(v, (size_t)v->size, value);
            }
            else if (is_number(v) && (x->type_class == TYPE_CLASS_POINTER || x->type_class == TYPE_CLASS_ARRAY) && sy == '+') {
                uint64_t value = 0;
                Symbol * base = NULL;
                ContextAddress size = 0;
                if (x->type == NULL || get_symbol_base_type(x->type, &base) < 0 ||
                    base == NULL || get_symbol_size(base, &size) < 0 || size == 0) {
                    error(ERR_INV_EXPRESSION, "Unknown pointer base type size");
                }
                value = to_uns(mode, x) + to_uns(mode, v) * size;
                v->type = x->type;
                if (x->type_class == TYPE_CLASS_ARRAY) {
                    if (get_array_symbol(base, 0, &v->type) < 0 ||
                        get_symbol_size(v->type, &v->size) < 0) {
                        error(errno, "Cannot cast to pointer");
                    }
                }
                v->type_class = TYPE_CLASS_POINTER;
                set_int_value(v, (size_t)x->size, value);
            }
#endif
            else if (!is_number(v) || !is_number(x)) {
                error(ERR_INV_EXPRESSION, "Numeric types expected");
            }
            else if (v->type_class == TYPE_CLASS_COMPLEX || x->type_class == TYPE_CLASS_COMPLEX) {
                double r_value = 0;
                double i_value = 0;
                switch (sy) {
                case '+':
                    r_value = to_r_double(mode, v) + to_r_double(mode, x);
                    i_value = to_i_double(mode, v) + to_i_double(mode, x);
                    break;
                case '-':
                    r_value = to_r_double(mode, v) - to_r_double(mode, x);
                    i_value = to_i_double(mode, v) - to_i_double(mode, x);
                    break;
                }
                v->type = NULL;
                v->type_class = TYPE_CLASS_COMPLEX;
                set_complex_value(v, sizeof(double) * 2, r_value, i_value);
                set_complex_type(v);
            }
            else if (is_real_number(v) || is_real_number(x)) {
                double value = 0;
                switch (sy) {
                case '+': value = to_double(mode, v) + to_double(mode, x); break;
                case '-': value = to_double(mode, v) - to_double(mode, x); break;
                }
                v->type = NULL;
                v->type_class = TYPE_CLASS_REAL;
                set_fp_value(v, sizeof(double), value);
                set_fp_type(v);
            }
            else if (v->type_class == TYPE_CLASS_CARDINAL || x->type_class == TYPE_CLASS_CARDINAL) {
                uint64_t value = 0;
                switch (sy) {
                case '+': value = to_uns(mode, v) + to_uns(mode, x); break;
                case '-': value = to_uns(mode, v) - to_uns(mode, x); break;
                }
                v->type = NULL;
                v->type_class = TYPE_CLASS_CARDINAL;
                set_int_value(v, sizeof(uint64_t), value);
            }
            else {
                int64_t value = 0;
                switch (sy) {
                case '+': value = to_int(mode, v) + to_int(mode, x); break;
                case '-': value = to_int(mode, v) - to_int(mode, x); break;
                }
                v->type = NULL;
                v->type_class = TYPE_CLASS_INTEGER;
                set_int_value(v, sizeof(int64_t), value);
            }
            v->constant = v->constant && x->constant;
        }
    }
}

static void shift_expression(int mode, Value * v) {
    additive_expression(mode, v);
    while (text_sy == SY_SHL || text_sy == SY_SHR) {
        Value * x = (Value *)tmp_alloc_zero(sizeof(Value));
        int sy = text_sy;
        next_sy();
        additive_expression(mode, x);
        if (mode != MODE_SKIP) {
            uint64_t value = 0;
            if (!is_whole_number(v) || !is_whole_number(x)) {
                error(ERR_INV_EXPRESSION, "Integral types expected");
            }
            if (x->type_class != TYPE_CLASS_CARDINAL && to_int(mode, x) < 0) {
                if (v->type_class == TYPE_CLASS_CARDINAL) {
                    switch (sy) {
                    case SY_SHL: value = to_uns(mode, v) >> -to_int(mode, x); break;
                    case SY_SHR: value = to_uns(mode, v) << -to_int(mode, x); break;
                    }
                }
                else {
                    switch (sy) {
                    case SY_SHL: value = to_int(mode, v) >> -to_int(mode, x); break;
                    case SY_SHR: value = to_int(mode, v) << -to_int(mode, x); break;
                    }
                    v->type_class = TYPE_CLASS_INTEGER;
                }
            }
            else {
                if (v->type_class == TYPE_CLASS_CARDINAL) {
                    switch (sy) {
                    case SY_SHL: value = to_uns(mode, v) << to_uns(mode, x); break;
                    case SY_SHR: value = to_uns(mode, v) >> to_uns(mode, x); break;
                    }
                }
                else {
                    switch (sy) {
                    case SY_SHL: value = to_int(mode, v) << to_uns(mode, x); break;
                    case SY_SHR: value = to_int(mode, v) >> to_uns(mode, x); break;
                    }
                    v->type_class = TYPE_CLASS_INTEGER;
                }
            }
            v->type = NULL;
            v->constant = v->constant && x->constant;
            set_int_value(v, sizeof(uint64_t), value);
        }
    }
}

static void relational_expression(int mode, Value * v) {
    shift_expression(mode, v);
    while (text_sy == '<' || text_sy == '>' || text_sy == SY_LEQ || text_sy == SY_GEQ) {
        Value * x = (Value *)tmp_alloc_zero(sizeof(Value));
        int sy = text_sy;
        next_sy();
        shift_expression(mode, x);
        if (mode != MODE_SKIP) {
            uint32_t value = 0;
            if (v->type_class == TYPE_CLASS_ARRAY && x->type_class == TYPE_CLASS_ARRAY) {
                int n = 0;
                load_value(v);
                load_value(x);
                n = strcmp((char *)v->value, (char *)x->value);
                switch (sy) {
                case '<': value = n < 0; break;
                case '>': value = n > 0; break;
                case SY_LEQ: value = n <= 0; break;
                case SY_GEQ: value = n >= 0; break;
                }
            }
            else if (is_real_number(v) || is_real_number(x)) {
                switch (sy) {
                case '<': value = to_double(mode, v) < to_double(mode, x); break;
                case '>': value = to_double(mode, v) > to_double(mode, x); break;
                case SY_LEQ: value = to_double(mode, v) <= to_double(mode, x); break;
                case SY_GEQ: value = to_double(mode, v) >= to_double(mode, x); break;
                }
            }
            else if (v->type_class == TYPE_CLASS_CARDINAL || x->type_class == TYPE_CLASS_CARDINAL) {
                switch (sy) {
                case '<': value = to_uns(mode, v) < to_uns(mode, x); break;
                case '>': value = to_uns(mode, v) > to_uns(mode, x); break;
                case SY_LEQ: value = to_uns(mode, v) <= to_uns(mode, x); break;
                case SY_GEQ: value = to_uns(mode, v) >= to_uns(mode, x); break;
                }
            }
            else {
                switch (sy) {
                case '<': value = to_int(mode, v) < to_int(mode, x); break;
                case '>': value = to_int(mode, v) > to_int(mode, x); break;
                case SY_LEQ: value = to_int(mode, v) <= to_int(mode, x); break;
                case SY_GEQ: value = to_int(mode, v) >= to_int(mode, x); break;
                }
            }
            if (mode != MODE_NORMAL) value = 0;
            v->constant = v->constant && x->constant;
            set_bool_value(v, value);
        }
    }
}

static void equality_expression(int mode, Value * v) {
    relational_expression(mode, v);
    while (text_sy == SY_EQU || text_sy == SY_NEQ) {
        Value * x = (Value *)tmp_alloc_zero(sizeof(Value));
        int sy = text_sy;
        next_sy();
        relational_expression(mode, x);
        if (mode != MODE_SKIP) {
            uint32_t value = 0;
            if (v->type_class == TYPE_CLASS_ARRAY && x->type_class == TYPE_CLASS_ARRAY) {
                load_value(v);
                load_value(x);
                value = strcmp((char *)v->value, (char *)x->value) == 0;
            }
            else if (v->type_class == TYPE_CLASS_COMPLEX || x->type_class == TYPE_CLASS_COMPLEX) {
                value =
                    to_r_double(mode, v) == to_r_double(mode, x) &&
                    to_i_double(mode, v) == to_i_double(mode, x);
            }
            else if (is_real_number(v) || is_real_number(x)) {
                value = to_double(mode, v) == to_double(mode, x);
            }
            else {
                value = to_int(mode, v) == to_int(mode, x);
            }
            if (sy == SY_NEQ) value = !value;
            if (mode != MODE_NORMAL) value = 0;
            v->constant = v->constant && x->constant;
            set_bool_value(v, value);
        }
    }
}

static void and_expression(int mode, Value * v) {
    equality_expression(mode, v);
    while (text_sy == '&') {
        Value * x = (Value *)tmp_alloc_zero(sizeof(Value));
        next_sy();
        equality_expression(mode, x);
        if (mode != MODE_SKIP) {
            int64_t value = 0;
            if (!is_whole_number(v) || !is_whole_number(x)) {
                error(ERR_INV_EXPRESSION, "Integral types expected");
            }
            if (v->type_class == TYPE_CLASS_CARDINAL || x->type_class == TYPE_CLASS_CARDINAL) {
                v->type_class = TYPE_CLASS_CARDINAL;
                value = to_uns(mode, v) & to_uns(mode, x);
            }
            else {
                v->type_class = TYPE_CLASS_INTEGER;
                value = to_int(mode, v) & to_int(mode, x);
            }
            if (mode != MODE_NORMAL) value = 0;
            v->type = NULL;
            v->constant = v->constant && x->constant;
            set_int_value(v, sizeof(int64_t), value);
        }
    }
}

static void exclusive_or_expression(int mode, Value * v) {
    and_expression(mode, v);
    while (text_sy == '^') {
        Value * x = (Value *)tmp_alloc_zero(sizeof(Value));
        next_sy();
        and_expression(mode, x);
        if (mode != MODE_SKIP) {
            int64_t value = 0;
            if (!is_whole_number(v) || !is_whole_number(x)) {
                error(ERR_INV_EXPRESSION, "Integral types expected");
            }
            if (v->type_class == TYPE_CLASS_CARDINAL || x->type_class == TYPE_CLASS_CARDINAL) {
                v->type_class = TYPE_CLASS_CARDINAL;
                value = to_uns(mode, v) ^ to_uns(mode, x);
            }
            else {
                v->type_class = TYPE_CLASS_INTEGER;
                value = to_int(mode, v) ^ to_int(mode, x);
            }
            if (mode != MODE_NORMAL) value = 0;
            v->type = NULL;
            v->constant = v->constant && x->constant;
            set_int_value(v, sizeof(int64_t), value);
        }
    }
}

static void inclusive_or_expression(int mode, Value * v) {
    exclusive_or_expression(mode, v);
    while (text_sy == '|') {
        Value * x = (Value *)tmp_alloc_zero(sizeof(Value));
        next_sy();
        exclusive_or_expression(mode, x);
        if (mode != MODE_SKIP) {
            int64_t value = 0;
            if (!is_whole_number(v) || !is_whole_number(x)) {
                error(ERR_INV_EXPRESSION, "Integral types expected");
            }
            if (v->type_class == TYPE_CLASS_CARDINAL || x->type_class == TYPE_CLASS_CARDINAL) {
                v->type_class = TYPE_CLASS_CARDINAL;
                value = to_uns(mode, v) | to_uns(mode, x);
            }
            else {
                v->type_class = TYPE_CLASS_INTEGER;
                value = to_int(mode, v) | to_int(mode, x);
            }
            if (mode != MODE_NORMAL) value = 0;
            v->type = NULL;
            v->constant = v->constant && x->constant;
            set_int_value(v, sizeof(int64_t), value);
        }
    }
}

static void logical_and_expression(int mode, Value * v) {
    inclusive_or_expression(mode, v);
    while (text_sy == SY_AND) {
        Value * x = (Value *)tmp_alloc_zero(sizeof(Value));
        int b = to_boolean(mode, v);
        next_sy();
        inclusive_or_expression(b ? mode : MODE_SKIP, x);
        if (b) {
            if (!v->constant) x->constant = 0;
            *v = *x;
        }
    }
}

static void logical_or_expression(int mode, Value * v) {
    logical_and_expression(mode, v);
    while (text_sy == SY_OR) {
        Value * x = (Value *)tmp_alloc_zero(sizeof(Value));
        int b = to_boolean(mode, v);
        next_sy();
        logical_and_expression(!b ? mode : MODE_SKIP, x);
        if (!b) {
            if (!v->constant) x->constant = 0;
            *v = *x;
        }
    }
}

static void conditional_expression(int mode, Value * v) {
    logical_or_expression(mode, v);
    if (text_sy == '?') {
        Value * x = (Value *)tmp_alloc_zero(sizeof(Value));
        Value * y = (Value *)tmp_alloc_zero(sizeof(Value));
        int b = to_boolean(mode, v);
        next_sy();
        expression(b ? mode : MODE_SKIP, x);
        if (text_sy != ':') error(ERR_INV_EXPRESSION, "Missing ':'");
        next_sy();
        conditional_expression(!b ? mode : MODE_SKIP, y);
        if (!v->constant) x->constant = y->constant = 0;
        *v = b ? *x : *y;
    }
}

static void expression(int mode, Value * v) {
    /* TODO: assignments in expressions */
    conditional_expression(mode, v);
}

static int evaluate_script(int mode, char * s, int load, Value * v) {
    Trap trap;

    expression_has_func_call = 0;
    if (set_trap(&trap)) {
        if (s == NULL || *s == 0) str_exception(ERR_INV_EXPRESSION, "Empty expression");
        text = s;
        text_pos = 0;
        text_len = strlen(s) + 1;
        next_ch();
        next_sy();
        for (;;) {
            expression(mode, v);
            if (text_sy != ',') break;
            next_sy();
        }
        if (text_sy != 0) error(ERR_INV_EXPRESSION, "Illegal characters at the end of expression");
        if (load) load_value(v);
        clear_trap(&trap);
    }

#if ENABLE_FuncCallInjection
    if (get_error_code(trap.error) != ERR_CACHE_MISS) {
        unsigned id = cache_transaction_id();
        LINK * l = func_call_state.next;
        while (l != &func_call_state) {
            FuncCallState * state = link_all2fc(l);
            l = l->next;
            if (state->id == id) {
                state->committed = 1;
                if (state->regs_cnt == 0) free_funccall_state(state);
            }
        }
    }
#endif /* ENABLE_FuncCallInjection */

    if (trap.error) {
        errno = trap.error;
        return -1;
    }

    return 0;
}

int evaluate_expression(Context * ctx, int frame, ContextAddress addr, char * s, int load, Value * v) {
#if !defined(SERVICE_Expressions)
    big_endian = big_endian_host();
#endif
    expression_context = ctx;
    expression_frame = frame;
    expression_addr = addr;
    return evaluate_script(MODE_NORMAL, s, load, v);
}

int value_to_boolean(Value * v, int * res) {
    Trap trap;
    if (!set_trap(&trap)) return -1;
    *res = to_boolean(MODE_NORMAL, v);
    clear_trap(&trap);
    return 0;
}

int value_to_address(Value * v, ContextAddress * res) {
    Trap trap;
    if (!set_trap(&trap)) return -1;
    *res = (ContextAddress)to_uns(MODE_NORMAL, v);
    clear_trap(&trap);
    return 0;
}

int value_to_signed(Value * v, int64_t *res) {
    Trap trap;
    if (!set_trap(&trap)) return -1;
    *res = to_int(MODE_NORMAL, v);
    clear_trap(&trap);
    return 0;
}

int value_to_unsigned(Value * v, uint64_t *res) {
    Trap trap;
    if (!set_trap(&trap)) return -1;
    *res = to_uns(MODE_NORMAL, v);
    clear_trap(&trap);
    return 0;
}

int value_to_double(Value * v, double *res) {
    Trap trap;
    if (!set_trap(&trap)) return -1;
    *res = to_double(MODE_NORMAL, v);
    clear_trap(&trap);
    return 0;
}

#if SERVICE_Expressions

/********************** Commands **************************/

typedef struct {
    char token[256];
    char id[256];
} CommandArgs;

typedef struct {
    char token[256];
    char id[256];
    int use_state;
    ContextAddress addr;
    char language[256];
    char * script;
} CommandCreateArgs;

typedef struct {
    char token[256];
    char id[256];
    char * value_buf;
    size_t value_size;
} CommandAssignArgs;

typedef struct {
    LINK link_all;
    LINK link_id;
    char id[256];
    char var_id[256];
    char parent[256];
    int use_state;
    ContextAddress addr;
    char language[256];
    Channel * channel;
    char * script;
    int can_assign;
    int has_func_call;
    ContextAddress size;
    int type_class;
    char type[256];
} Expression;

#define link_all2exp(A)  ((Expression *)((char *)(A) - offsetof(Expression, link_all)))
#define link_id2exp(A)   ((Expression *)((char *)(A) - offsetof(Expression, link_id)))

#define ID2EXP_HASH_SIZE (32 * MEM_USAGE_FACTOR - 1)

static LINK expressions = TCF_LIST_INIT(expressions);
static LINK id2exp[ID2EXP_HASH_SIZE];

static const char * EXPRESSIONS = "Expressions";
static unsigned expr_id_cnt = 0;

#define expression_hash(id) ((unsigned)atoi(id + 4) % ID2EXP_HASH_SIZE)

typedef struct {
    LINK link;
    CacheClient * client;
    Channel * channel;
    char args[sizeof(CommandCreateArgs)];
    size_t args_size;
} PendingCommand;

#define link_cmds2cmd(A) ((PendingCommand *)((char *)(A) - offsetof(PendingCommand, link)))

typedef struct {
    PendingCommand * pending_cmd;
    LINK cmd_queue;
} ChannelExtensionExpr;

static size_t channel_extension_offset = 0;
#define EXT_CH(ch) ((ChannelExtensionExpr *)((char *)(ch) + channel_extension_offset))

static void command_start(CacheClient * client, Channel * channel, void * args, size_t args_size) {
    ChannelExtensionExpr * ext = EXT_CH(channel);
    PendingCommand * cmd = (PendingCommand *)loc_alloc_zero(sizeof(PendingCommand));
    assert(args_size <= sizeof(cmd->args));
    channel_lock_with_msg(cmd->channel = channel, EXPRESSIONS);
    memcpy(cmd->args, args, args_size);
    cmd->args_size = args_size;
    cmd->client = client;
    if (ext->cmd_queue.next == NULL) list_init(&ext->cmd_queue);
    list_add_last(&cmd->link, &ext->cmd_queue);
    if (ext->cmd_queue.next == &cmd->link) {
        assert(ext->pending_cmd == NULL);
        ext->pending_cmd = cmd;
        cache_enter(cmd->client, cmd->channel, cmd->args, cmd->args_size);
    }
}

static void command_start_next(void * args) {
    Channel * channel = (Channel *)args;
    ChannelExtensionExpr * ext = EXT_CH(channel);
    PendingCommand * cmd = NULL;
    assert(ext->pending_cmd == NULL);
    assert(!list_is_empty(&ext->cmd_queue));
    ext->pending_cmd = cmd = link_cmds2cmd(ext->cmd_queue.next);
    cache_enter(cmd->client, cmd->channel, cmd->args, cmd->args_size);
}

static void command_done(Channel * channel) {
    ChannelExtensionExpr * ext = EXT_CH(channel);
    PendingCommand * cmd = ext->pending_cmd;
    ext->pending_cmd = NULL;
    assert(cmd != NULL);
    assert(cmd->channel == channel);
    assert(&cmd->link == ext->cmd_queue.next);
    list_remove(ext->cmd_queue.next);
    if (!list_is_empty(&ext->cmd_queue)) {
        post_event(command_start_next, channel);
    }
    channel_unlock_with_msg(cmd->channel, EXPRESSIONS);
    loc_free(cmd);
}

static Expression * find_expression(char * id) {
    if (id[0] == 'E' && id[1] == 'X' && id[2] == 'P' && id[3] == 'R') {
        unsigned hash = expression_hash(id);
        LINK * l = id2exp[hash].next;
        while (l != &id2exp[hash]) {
            Expression * e = link_id2exp(l);
            l = l->next;
            if (strcmp(e->id, id) == 0) return e;
        }
    }
    return NULL;
}

static int symbol_to_expression(char * expr_id, char * parent, char * sym_id, Expression ** res) {
#if ENABLE_Symbols
    Symbol * sym = NULL;
    Symbol * type = NULL;
    int sym_class = 0;
    size_t script_len = strlen(sym_id) + 8;
    char * script = (char *)tmp_alloc(script_len);
    Expression * expr = (Expression *)tmp_alloc_zero(sizeof(Expression));

    strlcpy(expr->id, expr_id, sizeof(expr->id));
    strlcpy(expr->var_id, sym_id, sizeof(expr->var_id));
    strlcpy(expr->parent, parent, sizeof(expr->parent));
    expr->use_state = 1;

    if (id2symbol(sym_id, &sym) < 0) return -1;

    snprintf(script, script_len, "${%s}", sym_id);
    expr->script = script;

    if (get_symbol_type_class(sym, &expr->type_class) < 0) return -1;
    if (get_symbol_size(sym, &expr->size) < 0) return -1;
    if (get_symbol_class(sym, &sym_class) < 0) return -1;
    if (get_symbol_type(sym, &type) < 0) return -1;

    expr->can_assign = sym_class == SYM_CLASS_REFERENCE;
    if (type != NULL) strlcpy(expr->type, symbol2id(type), sizeof(expr->type));

    *res = expr;
    return 0;
#else
    errno = ERR_UNSUPPORTED;
    return -1;
#endif
}

static int expression_context_id(char * id, Context ** ctx, int * frame, Expression ** expr) {
    int err = 0;
    Expression * e = NULL;

    if (id[0] == 'S') {
        char parent[256];
        char * s = id + 1;
        size_t i = 0;
        while (*s && i < sizeof(parent) - 1) {
            char ch = *s++;
            if (ch == '.') {
                if (*s == '.') {
                    parent[i++] = *s++;
                    continue;
                }
                break;
            }
            parent[i++] = ch;
        }
        parent[i] = 0;
        if (symbol_to_expression(id, parent, s, &e) < 0) err = errno;
    }
    else if ((e = find_expression(id)) == NULL) {
        err = ERR_INV_CONTEXT;
    }

    if (!err) {
        if ((*ctx = id2ctx(e->parent)) != NULL) {
            *frame = e->use_state && context_has_state(*ctx) ? STACK_TOP_FRAME : STACK_NO_FRAME;
        }
        else if (id2frame(e->parent, ctx, frame) < 0) {
            err = errno;
        }
    }

    if (err) {
        errno = err;
        return -1;
    }

    *expr = e;
    return 0;
}

static void write_context(OutputStream * out, Expression * expr) {
    write_stream(out, '{');
    json_write_string(out, "ID");
    write_stream(out, ':');
    json_write_string(out, expr->id);

    write_stream(out, ',');

    json_write_string(out, "ParentID");
    write_stream(out, ':');
    json_write_string(out, expr->parent);

    if (expr->var_id[0]) {
        write_stream(out, ',');

        json_write_string(out, "SymbolID");
        write_stream(out, ':');
        json_write_string(out, expr->var_id);
    }

    write_stream(out, ',');

    json_write_string(out, "Expression");
    write_stream(out, ':');
    json_write_string(out, expr->script);

    if (expr->can_assign) {
        write_stream(out, ',');

        json_write_string(out, "CanAssign");
        write_stream(out, ':');
        json_write_boolean(out, expr->can_assign);
    }

    if (expr->has_func_call) {
        write_stream(out, ',');

        json_write_string(out, "HasFuncCall");
        write_stream(out, ':');
        json_write_boolean(out, expr->has_func_call);
    }

    if (expr->type_class != TYPE_CLASS_UNKNOWN) {
        write_stream(out, ',');

        json_write_string(out, "Class");
        write_stream(out, ':');
        json_write_long(out, expr->type_class);
    }

    if (expr->type[0]) {
        write_stream(out, ',');

        json_write_string(out, "Type");
        write_stream(out, ':');
        json_write_string(out, expr->type);
    }

    write_stream(out, ',');

    json_write_string(out, "Size");
    write_stream(out, ':');
    json_write_uint64(out, expr->size);

    write_stream(out, '}');
}

static void get_context_cache_client(void * x) {
    CommandArgs * args = (CommandArgs *)x;
    Channel * c = cache_channel();
    Context * ctx = NULL;
    int frame = STACK_NO_FRAME;
    Expression * expr = NULL;
    int err = 0;

    if (expression_context_id(args->id, &ctx, &frame, &expr) < 0) err = errno;

    cache_exit();

    if (!is_channel_closed(c)) {
        write_stringz(&c->out, "R");
        write_stringz(&c->out, args->token);
        write_errno(&c->out, err);

        if (err) {
            write_stringz(&c->out, "null");
        }
        else {
            write_context(&c->out, expr);
            write_stream(&c->out, 0);
        }

        write_stream(&c->out, MARKER_EOM);
    }
}

static void command_get_context(char * token, Channel * c) {
    CommandArgs args;

    json_read_string(&c->inp, args.id, sizeof(args.id));
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    strlcpy(args.token, token, sizeof(args.token));
    cache_enter(get_context_cache_client, c, &args, sizeof(args));
}

#if ENABLE_Symbols && (SERVICE_StackTrace || ENABLE_ContextProxy)

static int sym_cnt = 0;
static int sym_max = 0;
static Symbol ** sym_buf = NULL;

static void get_children_callback(void * x, Symbol * symbol) {
    if (sym_cnt >= sym_max) {
        sym_max += 8;
        sym_buf = (Symbol **)loc_realloc(sym_buf, sizeof(Symbol *) * sym_max);
    }
    sym_buf[sym_cnt++] = symbol;
}

#endif

static void get_children_cache_client(void * x) {
    CommandArgs * args = (CommandArgs *)x;
    Channel * c = cache_channel();
    int err = 0;

    /* TODO: Expressions.getChildren - structures */
#if ENABLE_Symbols && (SERVICE_StackTrace || ENABLE_ContextProxy)
    char parent_id[256];
    {
        Context * ctx;
        int frame = STACK_NO_FRAME;

        sym_cnt = 0;

        if ((ctx = id2ctx(args->id)) != NULL && context_has_state(ctx)) {
            frame = get_top_frame(ctx);
            strlcpy(parent_id, frame2id(ctx, frame), sizeof(parent_id));
        }
        else if (id2frame(args->id, &ctx, &frame) == 0) {
            strlcpy(parent_id, args->id, sizeof(parent_id));
        }
        else {
            ctx = NULL;
        }

        if (ctx != NULL && err == 0 && enumerate_symbols(
                ctx, frame, get_children_callback, &args) < 0) err = errno;
    }
#else
    err = ERR_UNSUPPORTED;
#endif

    cache_exit();

    if (!is_channel_closed(c)) {
        write_stringz(&c->out, "R");
        write_stringz(&c->out, args->token);

        write_errno(&c->out, err);

        write_stream(&c->out, '[');
#if ENABLE_Symbols && (SERVICE_StackTrace || ENABLE_ContextProxy)
        {
            int i;
            for (i = 0; i < sym_cnt; i++) {
                const char * s = parent_id;
                if (i > 0) write_stream(&c->out, ',');
                write_stream(&c->out, '"');
                write_stream(&c->out, 'S');
                while (*s) {
                    if (*s == '.') write_stream(&c->out, '.');
                    json_write_char(&c->out, *s++);
                }
                write_stream(&c->out, '.');
                s = symbol2id(sym_buf[i]);
                while (*s) json_write_char(&c->out, *s++);
                write_stream(&c->out, '"');
            }
        }
#endif
        write_stream(&c->out, ']');
        write_stream(&c->out, 0);

        write_stream(&c->out, MARKER_EOM);
    }
}

static void command_get_children(char * token, Channel * c) {
    CommandArgs args;

    json_read_string(&c->inp, args.id, sizeof(args.id));
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    strlcpy(args.token, token, sizeof(args.token));
    cache_enter(get_children_cache_client, c, &args, sizeof(args));
}

static void command_create_cache_client(void * x) {
    CommandCreateArgs * args = (CommandCreateArgs *)x;
    Expression * e;
    Expression buf;
    Channel * c = cache_channel();
    int frame = STACK_NO_FRAME;
    int err = 0;

    memset(e = &buf, 0, sizeof(buf));

    if (is_channel_closed(c)) err = ERR_CHANNEL_CLOSED;

    if (!err) {
        do snprintf(e->id, sizeof(e->id), "EXPR%d", expr_id_cnt++);
        while (find_expression(e->id) != NULL);
        strlcpy(e->parent, args->id, sizeof(e->parent));
        strlcpy(e->language, args->language, sizeof(e->language));
        e->channel = c;
        e->script = args->script;
        e->use_state = args->use_state;
        e->addr = args->addr;
    }

    if (!err) {
        Value value;
        Context * ctx = NULL;
        memset(&value, 0, sizeof(value));
        if ((ctx = id2ctx(e->parent)) != NULL) {
            frame = args->use_state && context_has_state(ctx) ? STACK_TOP_FRAME : STACK_NO_FRAME;
        }
        else if (id2frame(e->parent, &ctx, &frame) < 0) {
            err = errno;
        }
        if (!err) {
            Context * mem = context_get_group(ctx, CONTEXT_GROUP_PROCESS);
            if ((mem->mem_access & MEM_ACCESS_RD_STOP) != 0 || (ctx->reg_access & REG_ACCESS_RD_STOP) != 0) {
                check_all_stopped(ctx);
            }
            expression_context = ctx;
            expression_frame = frame;
            expression_addr = e->addr;
            if (evaluate_script(MODE_TYPE, e->script, 0, &value) < 0) err = errno;
        }
        if (!err) {
            e->can_assign = value.remote || (value.loc != NULL && value.loc->pieces_cnt > 0);
            e->has_func_call = expression_has_func_call;
            e->type_class = value.type_class;
            e->size = value.size;
#if ENABLE_Symbols
            if (value.type != NULL) strlcpy(e->type, symbol2id(value.type), sizeof(e->type));
#endif
        }
    }

    cache_exit();

    if (!is_channel_closed(c)) {
        write_stringz(&c->out, "R");
        write_stringz(&c->out, args->token);
        write_errno(&c->out, err);

        if (err) {
            write_stringz(&c->out, "null");
            loc_free(args->script);
        }
        else {
            *(e = (Expression *)loc_alloc(sizeof(Expression))) = buf;
            list_add_last(&e->link_all, &expressions);
            list_add_last(&e->link_id, id2exp + expression_hash(e->id));
            write_context(&c->out, e);
            write_stream(&c->out, 0);
        }

        write_stream(&c->out, MARKER_EOM);
    }

    run_ctrl_unlock();
    command_done(c);
}

static void command_create(char * token, Channel * c) {
    CommandCreateArgs args;

    memset(&args, 0, sizeof(args));
    json_read_string(&c->inp, args.id, sizeof(args.id));
    json_test_char(&c->inp, MARKER_EOA);
    json_read_string(&c->inp, args.language, sizeof(args.language));
    json_test_char(&c->inp, MARKER_EOA);
    args.script = json_read_alloc_string(&c->inp);
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    run_ctrl_lock();
    args.use_state = 1;
    strlcpy(args.token, token, sizeof(args.token));
    command_start(command_create_cache_client, c, &args, sizeof(args));
}

static void read_expression_scope(InputStream * inp, const char * name, void * x) {
    CommandCreateArgs * args = (CommandCreateArgs *)x;
    if (strcmp(name, "ContextID") == 0) json_read_string(inp, args->id, sizeof(args->id));
    else if (strcmp(name, "Address") == 0) args->addr = (ContextAddress)json_read_uint64(inp);
    else if (strcmp(name, "Language") == 0) json_read_string(inp, args->language, sizeof(args->language));
    else json_skip_object(inp);
}

static void command_create_in_scope(char * token, Channel * c) {
    CommandCreateArgs args;

    memset(&args, 0, sizeof(args));
    json_read_struct(&c->inp, read_expression_scope, &args);
    json_test_char(&c->inp, MARKER_EOA);
    args.script = json_read_alloc_string(&c->inp);
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    run_ctrl_lock();
    strlcpy(args.token, token, sizeof(args.token));
    command_start(command_create_cache_client, c, &args, sizeof(args));
}

static void command_evaluate_cache_client(void * x) {
    CommandArgs * args = (CommandArgs *)x;
    Channel * c = cache_channel();
    Context * ctx = NULL;
    int frame = STACK_NO_FRAME;
    Expression * e = NULL;
    Value value;
    int value_ok = 0;
    void * buf = NULL;
    int implicit_pointer = 0;
    int err = 0;

    memset(&value, 0, sizeof(value));
    if (expression_context_id(args->id, &ctx, &frame, &e) < 0) err = errno;
    if (!err) {
        Context * mem = context_get_group(ctx, CONTEXT_GROUP_PROCESS);
        if ((mem->mem_access & MEM_ACCESS_RD_STOP) != 0 || (ctx->reg_access & REG_ACCESS_RD_STOP) != 0) {
            check_all_stopped(ctx);
        }
        expression_context = ctx;
        expression_frame = frame;
        expression_addr = e->addr;
        if (evaluate_script(MODE_NORMAL, e->script, 0, &value) < 0) err = errno;
        else value_ok = 1;
    }
    if (!err && value.remote && value.size <= 0x10000) {
        Context * mem = context_get_group(ctx, CONTEXT_GROUP_PROCESS);
        buf = tmp_alloc_zero((size_t)value.size);
        if ((mem->mem_access & MEM_ACCESS_RD_RUNNING) == 0) {
            if (!is_all_stopped(ctx)) err = set_errno(errno, "Cannot read memory if not stopped");
        }
        if (!err && context_read_mem(ctx, value.address, buf, (size_t)value.size) < 0) {
            err = set_errno(errno, "Cannot read target memory");
        }
    }
    if (!err && value.loc) {
        unsigned n;
        for (n = 0; n < value.loc->pieces_cnt; n++) {
            if (value.loc->pieces[n].implicit_pointer) {
                implicit_pointer = 1;
                break;
            }
        }
    }
    if (!err && !value.remote && value.value == NULL && !implicit_pointer) {
        Trap trap;
        if (set_trap(&trap)) {
            load_value(&value);
            clear_trap(&trap);
        }
        err = trap.error;
    }

    cache_exit();

    if (!is_channel_closed(c)) {
        write_stringz(&c->out, "R");
        write_stringz(&c->out, args->token);
        if (err) {
            write_stringz(&c->out, "null");
        }
        else if (!value.remote) {
            json_write_binary(&c->out, value.value, (size_t)value.size);
            write_stream(&c->out, 0);
        }
        else if (buf != NULL) {
            json_write_binary(&c->out, buf, (size_t)value.size);
            write_stream(&c->out, 0);
        }
        else {
            write_stringz(&c->out, "null");
        }
        write_errno(&c->out, err);
        if (!value_ok) {
            write_stringz(&c->out, "null");
        }
        else {
            unsigned cnt = 0;
            write_stream(&c->out, '{');

            if (value.type_class != TYPE_CLASS_UNKNOWN) {
                json_write_string(&c->out, "Class");
                write_stream(&c->out, ':');
                json_write_long(&c->out, value.type_class);
                cnt++;
            }

#if ENABLE_Symbols
            if (value.type != NULL) {
                if (cnt > 0) write_stream(&c->out, ',');
                json_write_string(&c->out, "Type");
                write_stream(&c->out, ':');
                json_write_string(&c->out, symbol2id(value.type));
                cnt++;
            }

            if (value.sym != NULL) {
                if (cnt > 0) write_stream(&c->out, ',');
                json_write_string(&c->out, "Symbol");
                write_stream(&c->out, ':');
                json_write_string(&c->out, symbol2id(value.sym));
                cnt++;
            }
#endif
            if (value.bit_stride != 0) {
                if (cnt > 0) write_stream(&c->out, ',');
                json_write_string(&c->out, "BitStride");
                write_stream(&c->out, ':');
                json_write_ulong(&c->out, value.bit_stride);
                cnt++;
            }

            if (value.binary_scale != 0) {
                if (cnt > 0) write_stream(&c->out, ',');
                json_write_string(&c->out, "BinaryScale");
                write_stream(&c->out, ':');
                json_write_long(&c->out, value.binary_scale);
                cnt++;
            }

            if (value.decimal_scale != 0) {
                if (cnt > 0) write_stream(&c->out, ',');
                json_write_string(&c->out, "DecimalScale");
                write_stream(&c->out, ':');
                json_write_long(&c->out, value.decimal_scale);
                cnt++;
            }

            if (implicit_pointer) {
                if (cnt > 0) write_stream(&c->out, ',');
                json_write_string(&c->out, "ImplicitPointer");
                write_stream(&c->out, ':');
                json_write_boolean(&c->out, 1);
                cnt++;
            }
            else {
                if (value.reg != NULL) {
                    int reg_frame = value.loc->ctx == ctx ? frame : STACK_NO_FRAME;
                    if (cnt > 0) write_stream(&c->out, ',');
                    json_write_string(&c->out, "Register");
                    write_stream(&c->out, ':');
                    json_write_string(&c->out, register2id(value.loc->ctx, reg_frame, value.reg));
                    cnt++;
                }

                if (value.remote) {
                    if (cnt > 0) write_stream(&c->out, ',');
                    json_write_string(&c->out, "Address");
                    write_stream(&c->out, ':');
                    json_write_uint64(&c->out, value.address);
                    cnt++;
                }

                if (value.loc != NULL && value.loc->pieces_cnt > 0 && value.reg == NULL) {
                    unsigned i;
                    if (cnt > 0) write_stream(&c->out, ',');
                    json_write_string(&c->out, "Pieces");
                    write_stream(&c->out, ':');
                    write_stream(&c->out, '[');
                    for (i = 0; i < value.loc->pieces_cnt; i++) {
                        LocationPiece * piece = value.loc->pieces + i;
                        if (i > 0) write_stream(&c->out, ',');
                        write_stream(&c->out, '{');
                        if (piece->size) {
                            json_write_string(&c->out, "Size");
                            write_stream(&c->out, ':');
                            json_write_ulong(&c->out, piece->size);
                        }
                        else {
                            json_write_string(&c->out, "BitSize");
                            write_stream(&c->out, ':');
                            json_write_ulong(&c->out, piece->bit_size);
                        }
                        if (piece->bit_offs) {
                            write_stream(&c->out, ',');
                            json_write_string(&c->out, "BitOffs");
                            write_stream(&c->out, ':');
                            json_write_ulong(&c->out, piece->bit_offs);
                        }
                        if (!piece->optimized_away) {
                            write_stream(&c->out, ',');
                            if (piece->reg) {
                                Context * reg_ctx = value.loc->ctx;
                                int reg_frame = get_info_frame(value.loc->ctx, value.loc->stack_frame);
                                json_write_string(&c->out, "Register");
                                write_stream(&c->out, ':');
                                json_write_string(&c->out, register2id(reg_ctx, reg_frame, piece->reg));
                            }
                            else if (piece->value) {
                                json_write_string(&c->out, "Value");
                                write_stream(&c->out, ':');
                                json_write_binary(&c->out, piece->value, piece->size);
                            }
                            else {
                                json_write_string(&c->out, "Address");
                                write_stream(&c->out, ':');
                                json_write_uint64(&c->out, piece->addr);
                            }
                        }
                        write_stream(&c->out, '}');
                    }
                    write_stream(&c->out, ']');
                    cnt++;
                }

                if (value.big_endian) {
                    if (cnt > 0) write_stream(&c->out, ',');
                    json_write_string(&c->out, "BigEndian");
                    write_stream(&c->out, ':');
                    json_write_boolean(&c->out, 1);
                    cnt++;
                }
            }

            write_stream(&c->out, '}');
            write_stream(&c->out, 0);
        }
        write_stream(&c->out, MARKER_EOM);
    }

    run_ctrl_unlock();
    command_done(c);
}

static void command_evaluate(char * token, Channel * c) {
    CommandArgs args;

    json_read_string(&c->inp, args.id, sizeof(args.id));
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    run_ctrl_lock();
    strlcpy(args.token, token, sizeof(args.token));
    command_start(command_evaluate_cache_client, c, &args, sizeof(args));
}

static void command_assign_cache_client(void * x) {
    CommandAssignArgs * args = (CommandAssignArgs *)x;
    Channel * c = cache_channel();
    Context * ctx = NULL;
    int frame = STACK_NO_FRAME;
    Expression * e = NULL;
    Value value;
    int err = 0;

    memset(&value, 0, sizeof(value));
    if (expression_context_id(args->id, &ctx, &frame, &e) < 0) err = errno;
    if (!err) {
        Context * mem = context_get_group(ctx, CONTEXT_GROUP_PROCESS);
        if ((mem->mem_access & MEM_ACCESS_RD_STOP) != 0 || (ctx->reg_access & REG_ACCESS_RD_STOP) != 0) {
            check_all_stopped(ctx);
        }
        else if ((mem->mem_access & MEM_ACCESS_WR_STOP) != 0 || (ctx->reg_access & REG_ACCESS_WR_STOP) != 0) {
            check_all_stopped(ctx);
        }
        expression_context = ctx;
        expression_frame = frame;
        expression_addr = e->addr;
        if (evaluate_script(MODE_NORMAL, e->script, 0, &value) < 0) err = errno;
    }
    if (!err) {
        if (value.remote) {
            if ((ctx->mem_access & MEM_ACCESS_WR_RUNNING) == 0) {
                if (!is_all_stopped(ctx)) err = set_errno(errno, "Cannot write memory if not stopped");
            }
            if (!err && context_write_mem(ctx, value.address, args->value_buf, args->value_size) < 0) err = errno;
#if SERVICE_Memory
            if (!err) send_event_memory_changed(ctx, value.address, args->value_size);
#endif
        }
        else if (value.loc != NULL && value.loc->pieces_cnt > 0) {
            Trap trap;
            if (set_trap(&trap)) {
                write_location_pieces(value.loc->ctx, value.loc->stack_frame,
                    value.loc->pieces, value.loc->pieces_cnt,
                    value.big_endian, args->value_buf, args->value_size);
#if SERVICE_Registers || SERVICE_Memory
                {
                    unsigned i;
                    for (i = 0; i < value.loc->pieces_cnt; i++) {
                        LocationPiece * piece = value.loc->pieces + i;
                        assert(piece->optimized_away == 0);
#if SERVICE_Registers
                        if (piece->reg != NULL) {
                            send_event_register_changed(register2id(value.loc->ctx,
                                get_info_frame(value.loc->ctx, value.loc->stack_frame), piece->reg));
                        }
#endif
#if SERVICE_Memory
                        if (piece->reg == NULL && piece->value == NULL) {
                            unsigned piece_size = piece->size ? piece->size : (piece->bit_offs + piece->bit_size + 7) / 8;
                            send_event_memory_changed(value.loc->ctx, piece->addr, piece_size);
                        }
#endif
                    }
                }
#endif
                clear_trap(&trap);
            }
            else {
                err = trap.error;
            }
        }
        else {
            err = ERR_INV_EXPRESSION;
        }
    }

    cache_exit();

    if (!is_channel_closed(c)) {
        write_stringz(&c->out, "R");
        write_stringz(&c->out, args->token);
        write_errno(&c->out, err);
        write_stream(&c->out, MARKER_EOM);
    }

    loc_free(args->value_buf);
    run_ctrl_unlock();
    command_done(c);
}

static void command_assign(char * token, Channel * c) {
    CommandAssignArgs args;

    json_read_string(&c->inp, args.id, sizeof(args.id));
    json_test_char(&c->inp, MARKER_EOA);
    args.value_buf = json_read_alloc_binary(&c->inp, &args.value_size);
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    run_ctrl_lock();
    strlcpy(args.token, token, sizeof(args.token));
    command_start(command_assign_cache_client, c, &args, sizeof(args));
}

static void command_dispose_cache_client(void * x) {
    CommandAssignArgs * args = (CommandAssignArgs *)x;
    Channel * c = cache_channel();
    Expression * e;
    int err = 0;

    e = find_expression(args->id);

    cache_exit();

    if (e != NULL) {
        list_remove(&e->link_all);
        list_remove(&e->link_id);
        loc_free(e->script);
        loc_free(e);
    }
    else {
        err = ERR_INV_CONTEXT;
    }

    if (!is_channel_closed(c)) {
        write_stringz(&c->out, "R");
        write_stringz(&c->out, args->token);
        write_errno(&c->out, err);
        write_stream(&c->out, MARKER_EOM);
    }

    command_done(c);
}

static void command_dispose(char * token, Channel * c) {
    CommandArgs args;

    json_read_string(&c->inp, args.id, sizeof(args.id));
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    strlcpy(args.token, token, sizeof(args.token));
    command_start(command_dispose_cache_client, c, &args, sizeof(args));
}

static void on_channel_close(Channel * c) {
    LINK * l = expressions.next;
    while (l != &expressions) {
        Expression * e = link_all2exp(l);
        l = l->next;
        if (e->channel == c) {
            list_remove(&e->link_all);
            list_remove(&e->link_id);
            loc_free(e->script);
            loc_free(e);
        }
    }
}

#endif  /* SERVICE_Expressions */

void add_identifier_callback(ExpressionIdentifierCallBack * callback) {
    if (id_callback_cnt >= id_callback_max) {
        id_callback_max += 8;
        id_callbacks = (ExpressionIdentifierCallBack **)loc_realloc(
            id_callbacks, id_callback_max * sizeof(ExpressionIdentifierCallBack *));
    }
    id_callbacks[id_callback_cnt++] = callback;
}

#if SERVICE_Expressions

#if ENABLE_FuncCallInjection
static void context_intercepted(Context * ctx, void * args) {
    LINK * l = func_call_state.next;
    while (l != &func_call_state) {
        FuncCallState * state = link_all2fc(l);
        l = l->next;
        if (state->ctx == ctx && !state->intercepted) {
            state->intercepted = 1;
            if (!state->finished && state->error == NULL) {
                state->error = get_error_report(set_errno(ERR_OTHER,
                    "Intercepted while executing injected function call"));
            }
            cache_notify_later(&state->cache);
        }
    }
}
#endif

void ini_expressions_service(Protocol * proto) {
    static int init = 0;
    if (init == 0) {
        unsigned i;
#if ENABLE_FuncCallInjection
        static RunControlEventListener rc_listener = { context_intercepted, NULL };
        add_run_control_event_listener(&rc_listener, NULL);
#endif
        for (i = 0; i < ID2EXP_HASH_SIZE; i++) list_init(id2exp + i);
        channel_extension_offset = channel_extension(sizeof(ChannelExtensionExpr));
        add_channel_close_listener(on_channel_close);
        big_endian = big_endian_host();
        init = 1;
    }
    add_command_handler(proto, EXPRESSIONS, "getContext", command_get_context);
    add_command_handler(proto, EXPRESSIONS, "getChildren", command_get_children);
    add_command_handler(proto, EXPRESSIONS, "create", command_create);
    add_command_handler(proto, EXPRESSIONS, "createInScope", command_create_in_scope);
    add_command_handler(proto, EXPRESSIONS, "evaluate", command_evaluate);
    add_command_handler(proto, EXPRESSIONS, "assign", command_assign);
    add_command_handler(proto, EXPRESSIONS, "dispose", command_dispose);
}

#endif  /* if SERVICE_Expressions */
#endif  /* if ENABLE_Expressions */
