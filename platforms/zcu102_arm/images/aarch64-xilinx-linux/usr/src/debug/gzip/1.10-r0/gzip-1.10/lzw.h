/* lzw.h -- define the lzw functions.

   Copyright (C) 1994-2018 Free Software Foundation, Inc.
   Copyright (C) 1992-1993 Jean-loup Gailly.

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

#ifndef BITS
#  define BITS 16
#endif
#define INIT_BITS 9              /* Initial number of bits per code */

#define	LZW_MAGIC  "\037\235"   /* Magic header for lzw files, 1F 9D */

#define BIT_MASK    0x1f /* Mask for 'number of compression bits' */
/* Mask 0x20 is reserved to mean a fourth header byte, and 0x40 is free.
 * It's a pity that old uncompress does not check bit 0x20. That makes
 * extension of the format actually undesirable because old compress
 * would just crash on the new format instead of giving a meaningful
 * error message. It does check the number of bits, but it's more
 * helpful to say "unsupported format, get a new version" than
 * "can only handle 16 bits".
 */

#define BLOCK_MODE  0x80
/* Block compression: if table is full and compression rate is dropping,
 * clear the dictionary.
 */

#define LZW_RESERVED 0x60 /* reserved bits */

#define	CLEAR  256       /* flush the dictionary */
#define FIRST  (CLEAR+1) /* first free entry */

extern int maxbits;      /* max bits per code for LZW */
extern int block_mode;   /* block compress mode -C compatible with 2.0 */

extern int lzw    (int in, int out);
extern int unlzw  (int in, int out);
