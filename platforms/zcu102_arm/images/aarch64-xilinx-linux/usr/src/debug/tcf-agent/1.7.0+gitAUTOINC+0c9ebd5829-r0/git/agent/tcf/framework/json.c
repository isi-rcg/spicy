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
 *     Michael Sills-Lavoie(École Polytechnique de Montréal)  - ZeroCopy support
 *              *                         *                 - json_splice_binary
 *******************************************************************************/

/*
 * This module provides support for JSON - a computer data interchange format.
 * It is a text-based, human-readable format for representing simple data structures and
 * associative arrays (called objects). The JSON format is specified in RFC 4627 by Douglas Crockford.
 * JSON is TCF preffered marshaling format.
 */

#if defined(__GNUC__) && !defined(_GNU_SOURCE)
/* pread() need _GNU_SOURCE */
#  define _GNU_SOURCE
#endif

#include <tcf/config.h>
#ifdef ENABLE_STREAM_MACROS
#undef ENABLE_STREAM_MACROS
#endif
#define ENABLE_STREAM_MACROS 1

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <tcf/framework/events.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/exceptions.h>
#include <tcf/framework/base64.h>
#include <tcf/framework/json.h>

#include <math.h>
#if defined(isfinite)
#  define is_nan_or_infinity(x) !isfinite(x)
#elif defined(_MSC_VER)
#  include <float.h>
#  define is_nan_or_infinity(x) !_finite(x)
#elif defined(HUGE_VAL)
#  define is_nan_or_infinity(x) (!(-HUGE_VAL < (x) && (x) < HUGE_VAL))
#else
#  define is_nan_or_infinity(x) ((x) != (x))
#endif

#define ENCODING_BINARY     0
#define ENCODING_BASE64     1

#define read_no_whitespace(ch, inp)  do { int _c_ = read_stream(inp); \
    while (_c_ > 0 && isspace(_c_)) _c_ = read_stream(inp); (ch) = _c_; } while (0)

#define skip_whitespace(inp)  do { int _c_ = peek_stream(inp); \
    while (_c_ > 0 && isspace(_c_)) { read_stream(inp); _c_ = peek_stream(inp); } } while (0)

static char * tmp_buf = NULL;
static size_t tmp_buf_pos = 0;
static size_t tmp_buf_size = 0;
static unsigned tmp_buf_timer = 0;

static uint8_t char_escaping[256] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static void tmp_buf_event(void * args) {
    if (--tmp_buf_timer == 0) {
        loc_free(tmp_buf);
        tmp_buf_size = 0;
        tmp_buf_pos = 0;
        tmp_buf = NULL;
    }
    else {
        post_event_with_delay(tmp_buf_event, NULL, 1000000);
    }
}

static void realloc_tmp_buf(void) {
    if (tmp_buf == NULL) {
        tmp_buf_size = 0x1000 - MEM_HEAP_LINK_SIZE;
        tmp_buf = (char *)loc_alloc(tmp_buf_size);
    }
    else {
        tmp_buf_size = (tmp_buf_size << 1) + MEM_HEAP_LINK_SIZE;
        tmp_buf = (char *)loc_realloc(tmp_buf, tmp_buf_size);
        if (tmp_buf_timer == 0) post_event_with_delay(tmp_buf_event, NULL, 1000000);
        tmp_buf_timer = 10;
    }
}

#define tmp_buf_add(ch) { if (tmp_buf_pos >= tmp_buf_size) realloc_tmp_buf(); tmp_buf[tmp_buf_pos++] = (char)(ch); }

void json_write_ulong(OutputStream * out, unsigned long n) {
    if (n >= 10) {
        json_write_ulong(out, n / 10);
        n = n % 10;
    }
    write_stream(out, (unsigned)n + '0');
}

void json_write_long(OutputStream * out, long n) {
    unsigned long u = (unsigned long)n;
    if (n < 0) {
        write_stream(out, '-');
        u = ~u + 1;
    }
    json_write_ulong(out, u);
}

void json_write_uint64(OutputStream * out, uint64_t n) {
    if (n >= 10) {
        json_write_uint64(out, n / 10);
        n = n % 10;
    }
    write_stream(out, (int)n + '0');
}

void json_write_int64(OutputStream * out, int64_t n) {
    uint64_t u = (uint64_t)n;
    if (n < 0) {
        write_stream(out, '-');
        u = ~u + 1;
    }
    json_write_uint64(out, u);
}

void json_write_double(OutputStream * out, double n) {
    if (is_nan_or_infinity(n)) write_string(out, "null");
    else write_string(out, double_to_str(n));
}

void json_write_boolean(OutputStream * out, int b) {
    if (b) write_string(out, "true");
    else write_string(out, "false");
}

static int hex_digit(unsigned n) {
    n &= 0xf;
    if (n < 10) return '0' + n;
    return 'A' + (n - 10);
}

static void write_escape_seq(OutputStream * out, char ch) {
    unsigned n = ch & 0xff;
    switch (n) {
    case '"':
    case '\\':
        write_stream(out, '\\');
        write_stream(out, n);
        break;
    case '\b':
        write_stream(out, '\\');
        write_stream(out, 'b');
        break;
    case '\f':
        write_stream(out, '\\');
        write_stream(out, 'f');
        break;
    case '\n':
        write_stream(out, '\\');
        write_stream(out, 'n');
        break;
    case '\r':
        write_stream(out, '\\');
        write_stream(out, 'r');
        break;
    case '\t':
        write_stream(out, '\\');
        write_stream(out, 't');
        break;
    default:
        write_stream(out, '\\');
        write_stream(out, 'u');
        write_stream(out, '0');
        write_stream(out, '0');
        write_stream(out, hex_digit(n >> 4));
        write_stream(out, hex_digit(n));
        break;
    }
}

void json_write_char(OutputStream * out, char ch) {
    if (char_escaping[(unsigned char)ch]) {
        write_escape_seq(out, ch);
    }
    else {
        write_stream(out, ch & 0xff);
    }
}

void json_write_string(OutputStream * out, const char * str) {
    if (str == NULL) {
        write_string(out, "null");
    }
    else {
        write_stream(out, '"');
        for (;;) {
            const char * ptr = str;
            while (!char_escaping[(unsigned char)*str]) str++;
            if (ptr < str) {
                unsigned n = str - ptr;
                if (out->cur + n <= out->end) {
                    memcpy(out->cur, ptr, n);
                    out->cur += n;
                }
                else {
                    out->write_block(out, ptr, n);
                }
            }
            if (*str == 0) break;
            write_escape_seq(out, *str++);
        }
        write_stream(out, '"');
    }
}

void json_write_string_len(OutputStream * out, const char * str, size_t len) {
    if (str == NULL) {
        write_string(out, "null");
    }
    else {
        const char * end = str + len;
        write_stream(out, '"');
        while (str < end) {
            const char * ptr = str;
            while (str < end && !char_escaping[(unsigned char)*str]) str++;
            if (ptr < str) {
                unsigned n = str - ptr;
                if (out->cur + n <= out->end) {
                    memcpy(out->cur, ptr, n);
                    out->cur += n;
                }
                else {
                    out->write_block(out, ptr, n);
                }
            }
            if (str == end) break;
            write_escape_seq(out, *str++);
        }
        write_stream(out, '"');
    }
}

static const char * char2str(int ch, char * buf) {
    if (ch == MARKER_EOS) return "<eos>";
    if (ch == MARKER_EOM) return "<eom>";
    if (ch == MARKER_EOA) return "<eoa>";
    if (ch >= ' ' && ch < 127) sprintf(buf, "'%c'", ch);
    else sprintf(buf, "'\\%03o'", ch);
    return buf;
}

static void check_char(int ch, int exp) {
    char s0[16], s1[16];
    if (exp == ch) return;
    str_fmt_exception(ERR_JSON_SYNTAX, "Expected %s, got %s",
        char2str(exp, s0), char2str(ch, s1));
}

static int read_hex_digit(InputStream * inp) {
    int res = 0;
    int ch = read_stream(inp);
    if (ch >= '0' && ch <= '9') res = ch - '0';
    else if (ch >= 'A' && ch <= 'F') res = ch - 'A' + 10;
    else if (ch >= 'a' && ch <= 'f') res = ch - 'a' + 10;
    else exception(ERR_JSON_SYNTAX);
    return res;
}

static int read_hex_char(InputStream * inp) {
    int n = read_hex_digit(inp) << 12;
    n |= read_hex_digit(inp) << 8;
    n |= read_hex_digit(inp) << 4;
    n |= read_hex_digit(inp);
    return n;
}

static unsigned read_esc_char(InputStream * inp, char * utf8) {
    int ch = read_stream(inp);
    switch (ch) {
    case '"': break;
    case '\\': break;
    case '/': break;
    case 'b': ch = '\b'; break;
    case 'f': ch = '\f'; break;
    case 'n': ch = '\n'; break;
    case 'r': ch = '\r'; break;
    case 't': ch = '\t'; break;
    case 'u': ch = read_hex_char(inp); break;
    default: exception(ERR_JSON_SYNTAX); break;
    }
    /* 'ch' can be wide character - convert it to UTF-8 sequence */
    if (ch < 0x80) {
        utf8[0] = (char)ch;
        return 1;
    }
    if (ch < 0x800) {
        utf8[0] = (char)((ch >> 6) | 0xc0);
        utf8[1] = (char)((ch & 0x3f) | 0x80);
        return 2;
    }
    utf8[0] = (char)((ch >> 12) | 0xe0);
    utf8[1] = (char)(((ch >> 6) & 0x3f) | 0x80);
    utf8[2] = (char)((ch & 0x3f) | 0x80);
    return 3;
}

int json_read_string(InputStream * inp, char * str, size_t size) {
    int ch;
    unsigned i = 0;
    read_no_whitespace(ch, inp);
    if (ch == 'n') {
        json_test_char(inp, 'u');
        json_test_char(inp, 'l');
        json_test_char(inp, 'l');
        str[0] = 0;
        return -1;
    }
    if (ch != '"') exception(ERR_PROTOCOL);
    for (;;) {
        ch = read_stream(inp);
        if (ch < 0) exception(ERR_JSON_SYNTAX);
        if (ch == '"') break;
        if (ch == '\\') {
            char utf8[4];
            unsigned l = read_esc_char(inp, utf8);
            unsigned n;
            for (n = 0; n < l; n++, i++) {
                if (i < size - 1) str[i] = utf8[n];
            }
        }
        else {
            if (i < size - 1) str[i] = (char)ch;
            i++;
        }
    }
    if (i < size) str[i] = 0;
    else str[size - 1] = 0;
    return i;
}

char * json_read_alloc_string(InputStream * inp) {
    int ch;
    char * str;
    read_no_whitespace(ch, inp);
    if (ch == 'n') {
        json_test_char(inp, 'u');
        json_test_char(inp, 'l');
        json_test_char(inp, 'l');
        return NULL;
    }
    tmp_buf_pos = 0;
    if (ch != '"') exception(ERR_PROTOCOL);
    for (;;) {
        ch = read_stream(inp);
        if (ch < 0) exception(ERR_JSON_SYNTAX);
        if (ch == '"') break;
        if (ch == '\\') {
            char utf8[4];
            unsigned l = read_esc_char(inp, utf8);
            unsigned n;
            for (n = 0; n < l; n++) tmp_buf_add(utf8[n]);
        }
        else {
            tmp_buf_add(ch);
        }
    }
    tmp_buf_add(0);
    str = (char *)loc_alloc(tmp_buf_pos);
    memcpy(str, tmp_buf, tmp_buf_pos);
    return str;
}

int json_read_boolean(InputStream * inp) {
    int ch;
    read_no_whitespace(ch, inp);
    if (ch == 't') {
        json_test_char(inp, 'r');
        json_test_char(inp, 'u');
        json_test_char(inp, 'e');
        return 1;
    }
    if (ch == 'f') {
        json_test_char(inp, 'a');
        json_test_char(inp, 'l');
        json_test_char(inp, 's');
        json_test_char(inp, 'e');
    }
    else {
        exception(ERR_JSON_SYNTAX);
    }
    return 0;
}

long json_read_long(InputStream * inp) {
    long res;
    int neg = 0;
    int ch;
    read_no_whitespace(ch, inp);
    if (ch == '-') {
        neg = 1;
        ch = read_stream(inp);
    }
    if (ch < '0' || ch > '9') exception(ERR_JSON_SYNTAX);
    res = ch - '0';
    for (;;) {
        ch = peek_stream(inp);
        if (ch < '0' || ch > '9') break;
        read_stream(inp);
        res = res * 10 + (ch - '0');
    }
    if (neg) return -res;
    return res;
}

unsigned long json_read_ulong(InputStream * inp) {
    unsigned long res;
    int neg = 0;
    int ch;
    read_no_whitespace(ch, inp);
    if (ch == '-') {
        neg = 1;
        ch = read_stream(inp);
    }
    if (ch < '0' || ch > '9') exception(ERR_JSON_SYNTAX);
    res = ch - '0';
    for (;;) {
        ch = peek_stream(inp);
        if (ch < '0' || ch > '9') break;
        read_stream(inp);
        res = res * 10 + (ch - '0');
    }
    if (neg) return ~res + 1;
    return res;
}

int64_t json_read_int64(InputStream * inp) {
    int64_t res;
    int neg = 0;
    int ch;
    read_no_whitespace(ch, inp);
    if (ch == '-') {
        neg = 1;
        ch = read_stream(inp);
    }
    if (ch < '0' || ch > '9') exception(ERR_JSON_SYNTAX);
    res = ch - '0';
    for (;;) {
        ch = peek_stream(inp);
        if (ch < '0' || ch > '9') break;
        read_stream(inp);
        res = res * 10 + (ch - '0');
    }
    if (neg) return -res;
    return res;
}

uint64_t json_read_uint64(InputStream * inp) {
    uint64_t res;
    int neg = 0;
    int ch;
    read_no_whitespace(ch, inp);
    if (ch == '-') {
        neg = 1;
        ch = read_stream(inp);
    }
    if (ch < '0' || ch > '9') exception(ERR_JSON_SYNTAX);
    res = ch - '0';
    for (;;) {
        ch = peek_stream(inp);
        if (ch < '0' || ch > '9') break;
        read_stream(inp);
        res = res * 10 + (ch - '0');
    }
    if (neg) return ~res + 1;
    return res;
}

double json_read_double(InputStream * inp) {
    char buf[256];
    int pos = 0;
    double n;
    char * end = buf;

    skip_whitespace(inp);
    for (;;) {
        int ch = peek_stream(inp);
        switch (ch) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '-':
        case '+':
        case 'e':
        case 'E':
        case '.':
            if (pos >= (int)sizeof(buf) - 1) exception(ERR_BUFFER_OVERFLOW);
            buf[pos++] = (char)read_stream(inp);
            continue;
        }
        break;
    }
    if (pos == 0) exception(ERR_JSON_SYNTAX);
    buf[pos] = 0;
    n = str_to_double(buf, &end);
    if (*end != 0) exception(ERR_JSON_SYNTAX);
    return n;
}

int json_read_struct(InputStream * inp, JsonStructCallBack * call_back, void * arg) {
    int ch;
    read_no_whitespace(ch, inp);
    if (ch == 'n') {
        json_test_char(inp, 'u');
        json_test_char(inp, 'l');
        json_test_char(inp, 'l');
        return 0;
    }
    if (ch != '{') {
        exception(ERR_PROTOCOL);
    }
    else if (json_peek(inp) == '}') {
        read_stream(inp);
    }
    else {
        for (;;) {
            char nm[256];
            json_read_string(inp, nm, sizeof(nm));
            json_test_char(inp, ':');
            call_back(inp, nm, arg);
            read_no_whitespace(ch, inp);
            if (ch == ',') continue;
            check_char(ch, '}');
            break;
        }
    }
    return 1;
}

char ** json_read_alloc_string_array(InputStream * inp, int * cnt) {
    int ch;
    read_no_whitespace(ch, inp);
    if (ch == 'n') {
        json_test_char(inp, 'u');
        json_test_char(inp, 'l');
        json_test_char(inp, 'l');
        if (cnt) *cnt = 0;
    }
    else if (ch != '[') {
        exception(ERR_PROTOCOL);
    }
    else {
        static size_t * len_buf = NULL;
        static unsigned len_buf_size = 0;
        unsigned len_pos = 0;

        unsigned i;
        size_t j;
        char * str;
        char ** arr;

        tmp_buf_pos = 0;

        if (json_peek(inp) == ']') {
            read_stream(inp);
        }
        else {
            for (;;) {
                size_t len = 0;
                if (len_pos >= len_buf_size) {
                    len_buf_size = len_buf_size == 0 ? 0x100 : len_buf_size * 2;
                    len_buf = (size_t *)loc_realloc(len_buf, len_buf_size * sizeof(size_t));
                }
                read_no_whitespace(ch, inp);
                if (ch == 'n') {
                    json_test_char(inp, 'u');
                    json_test_char(inp, 'l');
                    json_test_char(inp, 'l');
                }
                else {
                    size_t buf_pos0 = tmp_buf_pos;
                    if (ch != '"') exception(ERR_PROTOCOL);
                    for (;;) {
                        ch = read_stream(inp);
                        if (ch < 0) exception(ERR_JSON_SYNTAX);
                        if (ch == '"') break;
                        if (ch == '\\') {
                            char utf8[4];
                            unsigned l = read_esc_char(inp, utf8);
                            unsigned n;
                            for (n = 0; n < l; n++) tmp_buf_add(utf8[n]);
                        }
                        else {
                            tmp_buf_add(ch);
                        }
                    }
                    len = tmp_buf_pos - buf_pos0;
                }
                tmp_buf_add(0);
                len_buf[len_pos++] = len;
                read_no_whitespace(ch, inp);
                if (ch == ',') continue;
                check_char(ch, ']');
                break;
            }
        }
        tmp_buf_add(0);
        arr = (char **)loc_alloc((len_pos + 1) * sizeof(char *) + tmp_buf_pos);
        str = (char *)(arr + len_pos + 1);
        memcpy(str, tmp_buf, tmp_buf_pos);
        j = 0;
        for (i = 0; i < len_pos; i++) {
            arr[i] = str + j;
            j += len_buf[i] + 1;
        }
        arr[len_pos] = NULL;
        if (cnt) *cnt = len_pos;
        return arr;
    }
    return NULL;
}

/*
* json_read_array - generic read array function
*
* This function will call the call_back with inp and arg as
*       arguments for each element of the list.
* Return 0 if null, 1 otherwise
*/
int json_read_array(InputStream * inp, JsonArrayCallBack * call_back, void * arg) {
    int ch;
    read_no_whitespace(ch, inp);
    if (ch == 'n') {
        json_test_char(inp, 'u');
        json_test_char(inp, 'l');
        json_test_char(inp, 'l');
        return 0;
    }
    if (ch != '[') {
        exception(ERR_PROTOCOL);
    }
    else if (json_peek(inp) == ']') {
        read_stream(inp);
    }
    else {
        for (;;) {
            call_back(inp, arg);
            read_no_whitespace(ch, inp);
            if (ch == ',') continue;
            check_char(ch, ']');
            break;
        }
    }
    return 1;
}

void json_read_binary_start(JsonReadBinaryState * state, InputStream * inp) {
    int ch;
    state->inp = inp;
    state->rem = 0;
    state->size_start = 0;
    state->size_done = 0;
    read_no_whitespace(ch, inp);
    if (ch == '(') {
        state->encoding = ENCODING_BINARY;
        state->size_start = json_read_ulong(inp);
        json_test_char(inp, ')');
    }
    else if (ch == '"') {
        state->encoding = ENCODING_BASE64;
    }
    else if (ch == 'n') {
        json_test_char(inp, 'u');
        json_test_char(inp, 'l');
        json_test_char(inp, 'l');
        state->encoding = ENCODING_BINARY;
    }
    else {
        exception(ERR_JSON_SYNTAX);
    }
}

size_t json_read_binary_data(JsonReadBinaryState * state, void * buf, size_t len) {
    size_t res = 0;
    uint8_t * ptr = (uint8_t *)buf;
    InputStream * inp = state->inp;

    if (state->encoding == ENCODING_BINARY) {
        if (len > (size_t)(state->size_start - state->size_done)) len = state->size_start - state->size_done;
        while (res < len) {
            if (inp->cur < inp->end) {
                size_t n = inp->end - inp->cur;
                if (n > len - res) n = len - res;
                memcpy(ptr + res, inp->cur, n);
                inp->cur += n;
                res += n;
            }
            else {
                ptr[res++] = (uint8_t)inp->read(inp);
            }
        }
    }
    else {
        while (len > 0) {
            assert(state->rem <= sizeof(state->buf));
            if (state->rem > 0) {
                unsigned i = 0;
                while (i < state->rem && i < len) *ptr++ = state->buf[i++];
                len -= i;
                res += i;
                if (i < state->rem) {
                    unsigned j = 0;
                    if (state->rem > sizeof(state->buf)) exception(ERR_JSON_SYNTAX);
                    while (i < state->rem) state->buf[j++] = state->buf[i++];
                    state->rem = j;
                    break;
                }
                state->rem = 0;
            }
            if (len >= 3) {
                size_t i = read_base64(inp, (char *)ptr, len);
                if (i == 0) break;
                ptr += i;
                len -= i;
                res += i;
            }
            else {
                state->rem = read_base64(inp, state->buf, 3);
                if (state->rem == 0) break;
            }
        }
    }
    state->size_done += res;
    return res;
}

void json_read_binary_end(JsonReadBinaryState * state) {
    if (state->rem != 0) exception(ERR_JSON_SYNTAX);
    if (state->encoding == ENCODING_BINARY) {
        assert(state->size_start == state->size_done);
    }
    else {
        json_test_char(state->inp, '"');
    }
}

char * json_read_alloc_binary(InputStream * inp, size_t * size) {
    char * data = NULL;
    *size = 0;
    if (json_peek(inp) == 'n') {
        read_stream(inp);
        json_test_char(inp, 'u');
        json_test_char(inp, 'l');
        json_test_char(inp, 'l');
    }
    else {
        JsonReadBinaryState state;

        json_read_binary_start(&state, inp);

        tmp_buf_pos = 0;
        for (;;) {
            size_t rd;
            if (tmp_buf_pos >= tmp_buf_size) realloc_tmp_buf();
            rd = json_read_binary_data(&state, tmp_buf + tmp_buf_pos, tmp_buf_size - tmp_buf_pos);
            if (rd == 0) break;
            tmp_buf_pos += rd;
        }

        assert(state.size_start <= 0 || tmp_buf_pos == state.size_start);

        json_read_binary_end(&state);
        data = (char *)loc_alloc(tmp_buf_pos);
        memcpy(data, tmp_buf, tmp_buf_pos);
        *size = tmp_buf_pos;
    }
    return data;
}

void json_write_binary_start(JsonWriteBinaryState * state, OutputStream * out, size_t size) {
    state->out = out;
    state->rem = 0;
    state->encoding = out->supports_zero_copy && size > 0 ? ENCODING_BINARY : ENCODING_BASE64;
    state->size_start = size;
    state->size_done = 0;
    if (state->encoding == ENCODING_BINARY) {
        write_stream(state->out, '(');
        json_write_ulong(out, size);
        write_stream(state->out, ')');
    }
    else {
        write_stream(state->out, '"');
    }
}

void json_write_binary_data(JsonWriteBinaryState * state, const void * data, size_t len) {
    if (len <= 0) return;
    if (state->encoding == (int)ENCODING_BINARY) {
        write_block_stream(state->out, (const char *)data, len);
    }
    else {
        const uint8_t * ptr = (uint8_t *)data;
        size_t rem = state->rem;

        if (rem > 0) {
            while (rem < 3 && len > 0) {
                state->buf[rem++] = *ptr++;
                len--;
            }
            assert(rem <= 3);
            if (rem >= 3) {
                write_base64(state->out, state->buf, rem);
                rem = 0;
            }
        }
        if (len > 0) {
            assert(rem == 0);
            rem = len % 3;
            len -= rem;
            write_base64(state->out, (char *)ptr, len);
            if (rem > 0) memcpy(state->buf, ptr + len, rem);
        }
        state->rem = rem;
    }
    state->size_done += len;
}

void json_write_binary_end(JsonWriteBinaryState * state) {
    if (state->encoding == ENCODING_BINARY) {
        assert(state->size_start == state->size_done);
    }
    else {
        size_t rem = state->rem;
        if (rem > 0) write_base64(state->out, state->buf, rem);
        write_stream(state->out, '"');
    }
}

void json_write_binary(OutputStream * out, const void * data, size_t size) {
    if (data == NULL) {
        write_string(out, "null");
    }
    else {
        JsonWriteBinaryState state;
        json_write_binary_start(&state, out, size);
        json_write_binary_data(&state, data, size);
        json_write_binary_end(&state);
    }
}

void json_splice_binary(OutputStream * out, int fd, size_t size) {
    json_splice_binary_offset(out, fd, size, NULL);
}

void json_splice_binary_offset(OutputStream * out, int fd, size_t size, int64_t * offset) {
    if (out->supports_zero_copy && size > 0) {
        write_stream(out, '(');
        json_write_ulong(out, size);
        write_stream(out, ')');
        while (size > 0) {
            ssize_t rd = splice_block_stream(out, fd, size, offset);
            if (rd < 0) exception(errno);
            if (rd == 0) exception(ERR_EOF);
            size -= rd;
        }
    }
    else {
        char buffer[0x1000];
        JsonWriteBinaryState state;
        json_write_binary_start(&state, out, size);

        while (size > 0) {
            ssize_t rd;
            if (offset != NULL) {
                rd = pread(fd, buffer, size < sizeof(buffer) ? size : sizeof(buffer), (off_t)*offset);
                if (rd > 0) *offset += rd;
            }
            else {
                rd = read(fd, buffer, size < sizeof(buffer) ? size : sizeof(buffer));
            }
            if (rd < 0) exception(errno);
            if (rd == 0) exception(ERR_EOF);
            json_write_binary_data(&state, buffer, rd);
            size -= rd;
        }
        json_write_binary_end(&state);
    }
}

static int skip_char(InputStream * inp) {
    int ch = read_stream(inp);
    if (ch < 0) exception(ERR_JSON_SYNTAX);
    tmp_buf_add(ch);
    return ch;
}

static void skip_object(InputStream * inp) {
    int ch;
    skip_whitespace(inp);
    ch = skip_char(inp);
    switch (ch) {
    case 'n':
        check_char(skip_char(inp), 'u');
        check_char(skip_char(inp), 'l');
        check_char(skip_char(inp), 'l');
        return;
    case 'f':
        check_char(skip_char(inp), 'a');
        check_char(skip_char(inp), 'l');
        check_char(skip_char(inp), 's');
        check_char(skip_char(inp), 'e');
        return;
    case 't':
        check_char(skip_char(inp), 'r');
        check_char(skip_char(inp), 'u');
        check_char(skip_char(inp), 'e');
        return;
    case '"':
        for (;;) {
            ch = read_stream(inp);
            if (ch < 0) exception(ERR_JSON_SYNTAX);
            tmp_buf_add(ch);
            if (ch == '"') break;
            if (ch == '\\') skip_char(inp);
        }
        return;
    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        for (;;) {
            ch = peek_stream(inp);
            if ((ch < '0' || ch > '9') && ch != '.'
                    && ch != 'e' && ch != 'E' && ch != '-' && ch != '+') break;
            skip_char(inp);
        }
        return;
    case '[':
        if (json_peek(inp) == ']') {
            skip_char(inp);
        }
        else {
            for (;;) {
                skip_object(inp);
                skip_whitespace(inp);
                ch = skip_char(inp);
                if (ch == ',') continue;
                check_char(ch, ']');
                break;
            }
        }
        return;
    case '{':
        if (json_peek(inp) == '}') {
            skip_char(inp);
        }
        else {
            for (;;) {
                skip_object(inp);
                skip_whitespace(inp);
                check_char(skip_char(inp), ':');
                skip_object(inp);
                skip_whitespace(inp);
                ch = skip_char(inp);
                if (ch == ',') continue;
                check_char(ch, '}');
                break;
            }
        }
        return;
    case '(':
        tmp_buf_pos--;
        {
            size_t i;
            size_t size = 0;
            char * cbf = NULL;
            ByteArrayOutputStream buf;
            OutputStream * out = create_byte_array_output_stream(&buf);
            for (;;) {
                ch = read_stream(inp);
                if (ch < '0' || ch > '9') break;
                size = size * 10 + (ch - '0');
            }
            check_char(ch, ')');
            /* Binary data cannot be stored in tmp_buf and needs to be converted to BASE64 string */
            cbf = (char *)tmp_alloc(size);
            for (i = 0; i < size; i++) cbf[i] = (char)read_stream(inp);
            write_stream(out, '"');
            write_base64(out, cbf, size);
            write_stream(out, '"');
            get_byte_array_output_stream_data(&buf, &cbf, &size);
            while (tmp_buf_pos + size > tmp_buf_size) realloc_tmp_buf();
            memcpy(tmp_buf + tmp_buf_pos, cbf, size);
            tmp_buf_pos += size;
            loc_free(cbf);
        }
        return;
    default:
        exception(ERR_JSON_SYNTAX);
    }
}

char * json_read_object(InputStream * inp) {
    char * str;
    tmp_buf_pos = 0;
    skip_object(inp);
    tmp_buf_add(0);
    str = (char *)loc_alloc(tmp_buf_pos);
    memcpy(str, tmp_buf, tmp_buf_pos);
    return str;
}

void json_skip_object(InputStream * inp) {
    tmp_buf_pos = 0;
    skip_object(inp);
}

void json_test_char(InputStream * inp, int x) {
    switch (x) {
    case ':':
    case ',':
    case '{':
    case '}':
    case '[':
    case ']':
    case MARKER_EOA:
        skip_whitespace(inp);
        break;
    }
    check_char(read_stream(inp), x);
}

int json_peek(InputStream * inp) {
    int ch = peek_stream(inp);
    while (ch > 0 && isspace(ch)) {
        read_stream(inp);
        ch = peek_stream(inp);
    }
    return ch;
}

static void read_errno_param(InputStream * inp, void * x) {
    ErrorReport * err = (ErrorReport *)x;
    if (err->param_cnt >= err->param_max) {
        err->param_max += 4;
        err->params = (char **)loc_realloc(err->params, err->param_max * sizeof(char *));
    }
    err->params[err->param_cnt++] = json_read_object(inp);
}

int read_error_object(InputStream * inp) {
    int no = 0;
    ErrorReport * err = NULL;
    int ch;
    read_no_whitespace(ch, inp);
    if (ch == 'n') {
        json_test_char(inp, 'u');
        json_test_char(inp, 'l');
        json_test_char(inp, 'l');
        return 0;
    }
    check_char(ch, '{');
    if (json_peek(inp) == '}') {
        read_stream(inp);
    }
    else {
        for (;;) {
            char name[256];
            json_read_string(inp, name, sizeof(name));
            json_test_char(inp, ':');
            if (err == NULL) err = create_error_report();
            if (strcmp(name, "Code") == 0) {
                err->code = (int)json_read_long(inp);
            }
            else if (strcmp(name, "Time") == 0) {
                err->time_stamp = json_read_uint64(inp);
            }
            else if (strcmp(name, "Format") == 0) {
                err->format = json_read_alloc_string(inp);
            }
            else if (strcmp(name, "Params") == 0) {
                json_read_array(inp, read_errno_param, err);
            }
            else {
                ErrorReportItem * i = (ErrorReportItem *)loc_alloc_zero(sizeof(ErrorReportItem));
                i->name = loc_strdup(name);
                i->value = json_read_object(inp);
                i->next = err->props;
                err->props = i;
            }
            read_no_whitespace(ch, inp);
            if (ch == ',') continue;
            check_char(ch, '}');
            break;
        }
    }
    if (err == NULL) return 0;
    if (err->code != 0) no = set_error_report_errno(err);
    release_error_report(err);
    return no;
}

int read_errno(InputStream * inp) {
    int no = 0;
    if (json_peek(inp) != MARKER_EOA)
        no = read_error_object(inp);
    json_test_char(inp, MARKER_EOA);
    return no;
}

static void write_error_props(OutputStream * out, ErrorReport * rep) {
    ErrorReportItem * i = rep->props;

    if (rep->time_stamp != 0) {
        write_stream(out, ',');
        json_write_string(out, "Time");
        write_stream(out, ':');
        json_write_uint64(out, rep->time_stamp);
    }

    if (rep->format != NULL) {
        write_stream(out, ',');
        json_write_string(out, "Format");
        write_stream(out, ':');
        json_write_string(out, rep->format);
    }

    if (rep->param_cnt > 0) {
        int n;
        write_stream(out, ',');
        json_write_string(out, "Params");
        write_stream(out, ':');
        write_stream(out, '[');
        for (n = 0; n < rep->param_cnt; n++) {
            if (n > 0) write_stream(out, ',');
            write_string(out, rep->params[n]);
        }
        write_stream(out, ']');
    }

    while (i != NULL) {
        write_stream(out, ',');
        json_write_string(out, i->name);
        write_stream(out, ':');
        write_string(out, i->value);
        i = i->next;
    }
}

void write_error_object(OutputStream * out, int err) {
    ErrorReport * rep = get_error_report(err);
    if (rep == NULL) {
        write_string(out, "null");
    }
    else {
        write_stream(out, '{');

        json_write_string(out, "Code");
        write_stream(out, ':');
        json_write_long(out, rep->code);

        write_error_props(out, rep);
        release_error_report(rep);

        write_stream(out, '}');
    }
}

void write_errno(OutputStream * out, int err) {
    if (err != 0) write_error_object(out, err);
    write_stream(out, 0);
}

void write_service_error(OutputStream * out, int err, const char * service_name, int service_error) {
    ErrorReport * rep = get_error_report(err);
    if (rep != NULL) {
        write_stream(out, '{');

        json_write_string(out, "Service");
        write_stream(out, ':');
        json_write_string(out, service_name);

        write_stream(out, ',');

        json_write_string(out, "Code");
        write_stream(out, ':');
        json_write_long(out, service_error);

        write_error_props(out, rep);
        release_error_report(rep);

        write_stream(out, '}');
    }
    write_stream(out, 0);
}
