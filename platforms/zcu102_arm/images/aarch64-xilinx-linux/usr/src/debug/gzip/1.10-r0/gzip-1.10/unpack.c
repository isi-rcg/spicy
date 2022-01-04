/* unpack.c -- decompress files in pack format.

   Copyright (C) 1997, 1999, 2006, 2009-2018 Free Software Foundation, Inc.
   Copyright (C) 1992-1993 Jean-loup Gailly

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

#include <config.h>
#include "tailor.h"
#include "gzip.h"

#define MIN(a,b) ((a) <= (b) ? (a) : (b))
/* The arguments must not have side effects. */

#define MAX_BITLEN 25
/* Maximum length of Huffman codes. (Minor modifications to the code
 * would be needed to support 32 bits codes, but pack never generates
 * more than 24 bits anyway.)
 */

#define LITERALS 256
/* Number of literals, excluding the End of Block (EOB) code */

#define MAX_PEEK 12
/* Maximum number of 'peek' bits used to optimize traversal of the
 * Huffman tree.
 */

local ulg orig_len;       /* original uncompressed length */
local int max_len;        /* maximum bit length of Huffman codes */

local uch literal[LITERALS];
/* The literal bytes present in the Huffman tree. The EOB code is not
 * represented.
 */

local int lit_base[MAX_BITLEN+1];
/* All literals of a given bit length are contiguous in literal[] and
 * have contiguous codes. literal[code+lit_base[len]] is the literal
 * for a code of len bits.
 */

local int leaves [MAX_BITLEN+1]; /* Number of leaves for each bit length */
local int parents[MAX_BITLEN+1]; /* Number of parents for each bit length */

local int peek_bits; /* Number of peek bits currently used */

/* local uch prefix_len[1 << MAX_PEEK]; */
#define prefix_len outbuf
/* For each bit pattern b of peek_bits bits, prefix_len[b] is the length
 * of the Huffman code starting with a prefix of b (upper bits), or 0
 * if all codes of prefix b have more than peek_bits bits. It is not
 * necessary to have a huge table (large MAX_PEEK) because most of the
 * codes encountered in the input stream are short codes (by construction).
 * So for most codes a single lookup will be necessary.
 */
#if (1<<MAX_PEEK) > OUTBUFSIZ
    error cannot overlay prefix_len and outbuf
#endif

local ulg bitbuf;
/* Bits are added on the low part of bitbuf and read from the high part. */

local int valid;                  /* number of valid bits in bitbuf */
/* all bits above the last valid bit are always zero */

/* Read an input byte, reporting an error at EOF.  */
static unsigned char
read_byte (void)
{
  int b = get_byte ();
  if (b < 0)
    gzip_error ("invalid compressed data -- unexpected end of file");
  return b;
}

/* Set code to the next 'bits' input bits without skipping them. code
 * must be the name of a simple variable and bits must not have side effects.
 * IN assertions: bits <= 25 (so that we still have room for an extra byte
 * when valid is only 24), and mask = (1<<bits)-1.
 */
#define look_bits(code,bits,mask) \
{ \
  while (valid < (bits)) bitbuf = (bitbuf<<8) | read_byte(), valid += 8; \
  code = (bitbuf >> (valid-(bits))) & (mask); \
}

/* Skip the given number of bits (after having peeked at them): */
#define skip_bits(bits)  (valid -= (bits))

#define clear_bitbuf() (valid = 0, bitbuf = 0)

/* Local functions */

local void read_tree  (void);
local void build_tree (void);

/* ===========================================================================
 * Read the Huffman tree.
 */
local void read_tree()
{
    int len;  /* bit length */
    int base; /* base offset for a sequence of leaves */
    int n;
    int max_leaves = 1;

    /* Read the original input size, MSB first */
    orig_len = 0;
    for (n = 1; n <= 4; n++)
      orig_len = (orig_len << 8) | read_byte ();

    /* Read the maximum bit length of Huffman codes.  */
    max_len = read_byte ();
    if (! (0 < max_len && max_len <= MAX_BITLEN))
      gzip_error ("invalid compressed data -- "
                  "Huffman code bit length out of range");

    /* Get the number of leaves at each bit length */
    n = 0;
    for (len = 1; len <= max_len; len++) {
        leaves[len] = read_byte ();
        if (max_leaves - (len == max_len) < leaves[len])
          gzip_error ("too many leaves in Huffman tree");
        max_leaves = (max_leaves - leaves[len] + 1) * 2 - 1;
        n += leaves[len];
    }
    if (LITERALS <= n) {
        gzip_error ("too many leaves in Huffman tree");
    }
    Trace((stderr, "orig_len %lu, max_len %d, leaves %d\n",
           orig_len, max_len, n));
    /* There are at least 2 and at most 256 leaves of length max_len.
     * (Pack arbitrarily rejects empty files and files consisting of
     * a single byte even repeated.) To fit the last leaf count in a
     * byte, it is offset by 2. However, the last literal is the EOB
     * code, and is not transmitted explicitly in the tree, so we must
     * adjust here by one only.
     */
    leaves[max_len]++;

    /* Now read the leaves themselves */
    base = 0;
    for (len = 1; len <= max_len; len++) {
        /* Remember where the literals of this length start in literal[] : */
        lit_base[len] = base;
        /* And read the literals: */
        for (n = leaves[len]; n > 0; n--) {
            literal[base++] = read_byte ();
        }
    }
    leaves[max_len]++; /* Now include the EOB code in the Huffman tree */
}

/* ===========================================================================
 * Build the Huffman tree and the prefix table.
 */
local void build_tree()
{
    int nodes = 0; /* number of nodes (parents+leaves) at current bit length */
    int len;       /* current bit length */
    uch *prefixp;  /* pointer in prefix_len */

    for (len = max_len; len >= 1; len--) {
        /* The number of parent nodes at this level is half the total
         * number of nodes at parent level:
         */
        nodes >>= 1;
        parents[len] = nodes;
        /* Update lit_base by the appropriate bias to skip the parent nodes
         * (which are not represented in the literal array):
         */
        lit_base[len] -= nodes;
        /* Restore nodes to be parents+leaves: */
        nodes += leaves[len];
    }
    if ((nodes >> 1) != 1)
      gzip_error ("too few leaves in Huffman tree");

    /* Construct the prefix table, from shortest leaves to longest ones.
     * The shortest code is all ones, so we start at the end of the table.
     */
    peek_bits = MIN(max_len, MAX_PEEK);
    prefixp = &prefix_len[1<<peek_bits];
    for (len = 1; len <= peek_bits; len++) {
        int prefixes = leaves[len] << (peek_bits-len); /* may be 0 */
        while (prefixes--) *--prefixp = (uch)len;
    }
    /* The length of all other codes is unknown: */
    while (prefixp > prefix_len) *--prefixp = 0;
}

/* ===========================================================================
 * Unpack in to out.  This routine does not support the old pack format
 * with magic header \037\037.
 *
 * IN assertions: the buffer inbuf contains already the beginning of
 *   the compressed data, from offsets inptr to insize-1 included.
 *   The magic header has already been checked. The output buffer is cleared.
 */
int unpack(in, out)
    int in, out;            /* input and output file descriptors */
{
    int len;                /* Bit length of current code */
    unsigned eob;           /* End Of Block code */
    register unsigned peek; /* lookahead bits */
    unsigned peek_mask;     /* Mask for peek_bits bits */

    ifd = in;
    ofd = out;

    read_tree();     /* Read the Huffman tree */
    build_tree();    /* Build the prefix table */
    clear_bitbuf();  /* Initialize bit input */
    peek_mask = (1<<peek_bits)-1;

    /* The eob code is the largest code among all leaves of maximal length: */
    eob = leaves[max_len]-1;
    Trace((stderr, "eob %d %x\n", max_len, eob));

    /* Decode the input data: */
    for (;;) {
        /* Since eob is the longest code and not shorter than max_len,
         * we can peek at max_len bits without having the risk of reading
         * beyond the end of file.
         */
        look_bits(peek, peek_bits, peek_mask);
        len = prefix_len[peek];
        if (len > 0) {
            peek >>= peek_bits - len; /* discard the extra bits */
        } else {
            /* Code of more than peek_bits bits, we must traverse the tree */
            ulg mask = peek_mask;
            len = peek_bits;

            /* Loop as long as peek is a parent node.  */
            while (peek < parents[len])
              {
                len++, mask = (mask<<1)+1;
                look_bits(peek, len, mask);
              }
        }
        /* At this point, peek is the next complete code, of len bits */
        if (peek == eob && len == max_len)
          break; /* End of file.  */
        put_ubyte(literal[peek+lit_base[len]]);
        Tracev((stderr,"%02d %04x %c\n", len, peek,
                literal[peek+lit_base[len]]));
        skip_bits(len);
    } /* for (;;) */

    flush_window();
    if (orig_len != (ulg)(bytes_out & 0xffffffff)) {
        gzip_error ("invalid compressed data--length error");
    }
    return OK;
}
