/*******************************************************************************
* Copyright (c) 2018 Xilinx, Inc. and others.
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
 * Implements RFC 1951: http://www.ietf.org/rfc/rfc1951.txt
 *
 * RFC 1951 defines a lossless compressed data format that
 * compresses data using a combination of the LZ77 algorithm and Huffman coding.
 *
 * Only decompressor is implemented.
 */

#include <tcf/config.h>

#include <assert.h>
#include <tcf/framework/errors.h>
#include <tcf/framework/exceptions.h>
#include <tcf/framework/compression.h>

#define FAST_LOOKUP_BITS 8
#define MAX_SYMBOLS_CNT 288

typedef struct HuffmanSymbol {
    uint16_t code;
    uint8_t size;
    uint8_t tree;
} HuffmanSymbol;

typedef struct HuffmanTable {
    unsigned syms_cnt;
    uint8_t code_size[MAX_SYMBOLS_CNT];
    HuffmanSymbol lookup[1 << FAST_LOOKUP_BITS];
    HuffmanSymbol tree[MAX_SYMBOLS_CNT * 2];
} HuffmanTable;

static HuffmanTable tables[3];

static uint8_t * inp_buf = NULL;
static size_t inp_size = 0;
static unsigned inp_pos = 0;
static uint32_t inp_bit_buf = 0;
static unsigned inp_bit_cnt = 0;
static uint8_t * out_buf = NULL;
static size_t out_size = 0;
static unsigned out_pos = 0;

static unsigned get_bits(unsigned bit_cnt) {
    unsigned v = 0;
    if (bit_cnt > 0) {
        assert(bit_cnt <= 32);
        while (inp_bit_cnt < bit_cnt) {
            if (inp_pos >= inp_size) exception(ERR_BUFFER_OVERFLOW);
            inp_bit_buf |= (uint32_t)inp_buf[inp_pos++] << inp_bit_cnt;
            inp_bit_cnt += 8;
        }
        v = inp_bit_buf & ((1u << bit_cnt) - 1);
        inp_bit_buf >>= bit_cnt;
        inp_bit_cnt -= bit_cnt;
    }
    return v;
}

static unsigned decode_huffman_symbol(HuffmanTable * t) {
    HuffmanSymbol v;

    while (inp_bit_cnt < 15) {
        if (inp_pos >= inp_size) exception(ERR_BUFFER_OVERFLOW);
        inp_bit_buf |= (uint32_t)inp_buf[inp_pos++] << inp_bit_cnt;
        inp_bit_cnt += 8;
    }

    v = t->lookup[inp_bit_buf & ((1 << FAST_LOOKUP_BITS) - 1)];
    if (v.tree) {
        unsigned b = FAST_LOOKUP_BITS;
        do {
            assert(b <= 14);
            assert(v.code <= MAX_SYMBOLS_CNT * 2 - 2);
            v = t->tree[v.code + ((inp_bit_buf >> b++) & 1)];
        }
        while (v.tree);
        assert(v.size == b);
    }
    assert(v.size <= 15);
    if (v.size == 0) exception(ERR_OTHER);
    inp_bit_buf >>= v.size;
    inp_bit_cnt -= v.size;
    return v.code;
}

static void decode_tables(unsigned type) {
    memset(tables, 0, sizeof(tables));
    if (type == 1) {
        unsigned t;
        for (t = 0; t < 2; t++) {
            unsigned i = 0;
            uint8_t * p = tables[t].code_size;
            switch (t) {
            case 0:
                for (; i <= 143; i++) *p++ = 8;
                for (; i <= 255; i++) *p++ = 9;
                for (; i <= 279; i++) *p++ = 7;
                for (; i <= 287; i++) *p++ = 8;
                tables[t].syms_cnt = MAX_SYMBOLS_CNT;
                break;
            case 1:
                for (; i <= 31; i++) *p++ = 5;
                tables[t].syms_cnt = 32;
                break;
            }
        }
    }
    else {
        unsigned i;
        static const uint8_t zigzag[19] = {
            16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
        };
        tables[0].syms_cnt = get_bits(5) + 257;
        tables[1].syms_cnt = get_bits(5) + 1;
        tables[2].syms_cnt = get_bits(4) + 4;
        for (i = 0; i < tables[2].syms_cnt; i++) {
            tables[2].code_size[zigzag[i]] = (uint8_t)get_bits(3);
        }
        tables[2].syms_cnt = 19;
    }
    for (;;) {
        unsigned i;
        unsigned sym;
        unsigned syms_used = 0;
        unsigned syms_total = 0;
        unsigned syms_next[17];
        unsigned syms_per_size[16];
        HuffmanSymbol hs, hs_next;

        memset(syms_next, 0, sizeof(syms_next));
        memset(syms_per_size, 0, sizeof(syms_per_size));
        for (i = 0; i < tables[type].syms_cnt; i++) syms_per_size[tables[type].code_size[i]]++;
        for (i = 1; i <= 15; i++) {
            syms_used += syms_per_size[i];
            syms_total = (syms_total + syms_per_size[i]) << 1;
            syms_next[i + 1] = syms_total;
        }
        if (syms_total != 0x10000 && syms_used > 1) exception(ERR_OTHER);

        hs_next.tree = 1;
        hs_next.code = 0;
        hs_next.size = 0;
        for (sym = 0; sym < tables[type].syms_cnt; sym++) {
            unsigned code_size = tables[type].code_size[sym];
            unsigned rev_code = 0;
            unsigned cur_code = 0;
            if (code_size == 0) continue;
            assert(code_size <= 15);
            cur_code = syms_next[code_size]++;
            for (i = 0; i < code_size; i++) {
                rev_code <<= 1;
                rev_code = rev_code | (cur_code & 1);
                cur_code >>= 1;
            }
            if (code_size <= FAST_LOOKUP_BITS) {
                hs.tree = 0;
                hs.code = (uint16_t)sym;
                hs.size = (uint8_t)code_size;
                while (rev_code < (1 << FAST_LOOKUP_BITS)) {
                    tables[type].lookup[rev_code] = hs;
                    rev_code += 1 << code_size;
                }
                continue;
            }
            hs = tables[type].lookup[rev_code & ((1 << FAST_LOOKUP_BITS) - 1)];
            assert(hs.size == 0);
            if (hs.tree == 0) {
                hs = tables[type].lookup[rev_code & ((1 << FAST_LOOKUP_BITS) - 1)] = hs_next;
                hs_next.code += 2;
            }
            rev_code >>= FAST_LOOKUP_BITS;
            for (i = code_size; i > FAST_LOOKUP_BITS + 1; i--) {
                hs.code += rev_code & 1;
                if (tables[type].tree[hs.code].tree) {
                    hs = tables[type].tree[hs.code];
                }
                else {
                    hs = tables[type].tree[hs.code] = hs_next;
                    hs_next.code += 2;
                }
                rev_code >>= 1;
            }
            assert(hs_next.code <= MAX_SYMBOLS_CNT * 2);
            hs.code += rev_code & 1;
            tables[type].tree[hs.code].code = (uint16_t)sym;
            tables[type].tree[hs.code].size = (uint8_t)code_size;
        }

        if (type == 2) {
            uint8_t len_codes[MAX_SYMBOLS_CNT + 32 + 137];
            i = 0;
            while (i < tables[0].syms_cnt + tables[1].syms_cnt) {
                unsigned s = 0;
                unsigned d = decode_huffman_symbol(tables + 2);
                if (d < 16) {
                    len_codes[i++] = (uint8_t)d;
                    continue;
                }
                if (d == 16 && i == 0) exception(ERR_OTHER);
                switch (d - 16) {
                case 0: s = get_bits(2) + 3; break;
                case 1: s = get_bits(3) + 3; break;
                case 2: s = get_bits(7) + 11; break;
                }
                if (s == 0) exception(ERR_OTHER);
                memset(len_codes + i, d == 16 ? len_codes[i - 1] : 0, s);
                i += s;
            }
            if (tables[0].syms_cnt + tables[1].syms_cnt != i) exception(ERR_OTHER);
            memcpy(tables[0].code_size, len_codes, tables[0].syms_cnt);
            memcpy(tables[1].code_size, len_codes + tables[0].syms_cnt, tables[1].syms_cnt);
        }
        if (type == 0) break;
        type--;
    }
}

static void decode_data_block() {
    static const unsigned length_base[31] = {
          3,   4,   5,   6,   7,   8,   9,  10,
         11,  13,  15,  17,  19,  23,  27,  31,
         35,  43,  51,  59,  67,  83,  99, 115,
        131, 163, 195, 227, 258,   0,   0
    };
    static const unsigned length_extra[31] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 1, 1, 2, 2, 2, 2,
        3, 3, 3, 3, 4, 4, 4, 4,
        5, 5, 5, 5, 0, 0, 0
    };
    static const unsigned dist_base[32] = {
           1,    2,    3,     4,     5,     7,    9,   13,
          17,   25,   33,    49,    65,    97,  129,  193,
         257,  385,  513,   769,  1025,  1537, 2049, 3073,
        4097, 6145, 8193, 12289, 16385, 24577,    0,    0
    };
    static const unsigned dist_extra[32] = {
         0,  0,  0,  0,  1,  1,  2,  2,
         3,  3,  4,  4,  5,  5,  6,  6,
         7,  7,  8,  8,  9,  9, 10, 10,
        11, 11, 12, 12, 13, 13,  0,  0
    };
    for (;;) {
        unsigned sym = decode_huffman_symbol(tables);
        if (sym < 256) {
            if (out_pos >= out_size) exception(ERR_OTHER);
            out_buf[out_pos++] = (uint8_t)sym;
        }
        else if (sym == 256) {
            return;
        }
        else {
            unsigned len = length_base[sym - 257] + get_bits(length_extra[sym - 257]);
            unsigned sym_ofs = decode_huffman_symbol(tables + 1);
            unsigned ofs = dist_base[sym_ofs] + get_bits(dist_extra[sym_ofs]);

            if (ofs > out_pos) exception(ERR_OTHER);
            if (out_pos + len > out_size) exception(ERR_OTHER);

            while (len > 0) {
                out_buf[out_pos] = out_buf[out_pos - ofs];
                out_pos++;
                len--;
            }
        }
    }
}

unsigned decompress(void * src_buf, size_t src_size, void * dst_buf, size_t dst_size) {
    inp_buf = (uint8_t *)src_buf;
    inp_size = src_size;
    inp_pos = 0;
    inp_bit_buf = 0;
    inp_bit_cnt = 0;
    out_buf = (uint8_t *)dst_buf;
    out_size = dst_size;
    out_pos = 0;
    for (;;) {
        unsigned final = get_bits(1);
        unsigned type = get_bits(2);
        if (type == 0) {
            unsigned len = 0, nlen = 0;
            inp_pos -= inp_bit_cnt >> 3;
            inp_bit_cnt = 0;
            if (inp_pos + 4 > inp_size) exception(ERR_OTHER);
            len |= inp_buf[inp_pos++];
            len |= (unsigned)inp_buf[inp_pos++] << 8;
            nlen |= inp_buf[inp_pos++];
            nlen |= (unsigned)inp_buf[inp_pos++] << 8;
            if ((len ^ nlen) != 0xffff) exception(ERR_OTHER);
            if (inp_pos + len > inp_size) exception(ERR_OTHER);
            if (out_pos + len > out_size) exception(ERR_OTHER);
            memcpy(out_buf + out_pos, inp_buf + inp_pos, len);
            inp_pos += len;
            out_pos += len;
        }
        else if (type == 3) {
            exception(ERR_OTHER);
        }
        else {
            decode_tables(type);
            decode_data_block();
        }
        if (final) break;
    }
    inp_pos -= inp_bit_cnt >> 3;
    inp_bit_cnt = 0;
    return inp_pos;
}
