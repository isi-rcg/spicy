/*
Copyright (C) 2014-2017,2018 John E. Davis

This file is part of the S-Lang Library.

The S-Lang Library is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

The S-Lang Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
USA.
*/

#include "config.h"
#include <string.h>
#include <slang.h>

#include "_slint.h"

#define MD5_BUFSIZE	64
#define MD5_DIGEST_LEN	16
#define CHKSUM_TYPE_PRIVATE_FIELDS \
   _pSLuint32_Type abcd[4]; \
   _pSLuint32_Type num_bits[2];		       /* 64 bit representation */ \
   unsigned int num_buffered; \
   unsigned char buf[MD5_BUFSIZE];
#include "chksum.h"

/* 8 bit bytes assumed in what follows.  The algorithm is based upon
 * section 3 of RFC 1321.
 */

/*
3.1 Step 1. Append Padding Bits

   The message is "padded" (extended) so that its length (in bits) is
   congruent to 448, modulo 512. That is, the message is extended so
   that it is just 64 bits shy of being a multiple of 512 bits long.
   Padding is always performed, even if the length of the message is
   already congruent to 448, modulo 512.
*/

static unsigned int compute_pad_length (unsigned int len)
{
   unsigned int mod64 = len % 64;
   unsigned int dlen;

   if (mod64 < 56)
     dlen = 56 - mod64;
   else
     dlen = 120 - mod64;

   return dlen;
}

/*    Padding is performed as follows: a single "1" bit is appended to the */
/*    message, and then "0" bits are appended so that the length in bits of */
/*    the padded message becomes congruent to 448, modulo 512. In all, at */
/*    least one bit and at most 512 bits are appended. */
static unsigned char Pad_Bytes[64] =
{
   0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/*
3.2 Step 2. Append Length

   A 64-bit representation of b (the length of the message before the
   padding bits were added) is appended to the result of the previous
   step. In the unlikely event that b is greater than 2^64, then only
   the low-order 64 bits of b are used. (These bits are appended as two
   32-bit words and appended low-order word first in accordance with the
   previous conventions.)

   At this point the resulting message (after padding with bits and with
   b) has a length that is an exact multiple of 512 bits. Equivalently,
   this message has a length that is an exact multiple of 16 (32-bit)
   words. Let M[0 ... N-1] denote the words of the resulting message,
   where N is a multiple of 16.
*/

/*
3.3 Step 3. Initialize MD Buffer

   A four-word buffer (A,B,C,D) is used to compute the message digest.
   Here each of A, B, C, D is a 32-bit register. These registers are
   initialized to the following values in hexadecimal, low-order bytes
   first):

          word A: 01 23 45 67
          word B: 89 ab cd ef
          word C: fe dc ba 98
          word D: 76 54 32 10
*/

static void init_md5_buffer (_pSLuint32_Type buffer[4])
{
   buffer[0] = 0x67452301;
   buffer[1] = 0xEFCDAB89;
   buffer[2] = 0x98BADCFE;
   buffer[3] = 0x10325476;
}

/*
3.4 Step 4. Process Message in 16-Word Blocks

   We first define four auxiliary functions that each take as input
   three 32-bit words and produce as output one 32-bit word.

          F(X,Y,Z) = XY v not(X) Z
          G(X,Y,Z) = XZ v Y not(Z)
          H(X,Y,Z) = X xor Y xor Z
          I(X,Y,Z) = Y xor (X v not(Z))
 */

#define MD5_F_OP(X,Y,Z) (((X)&(Y)) | ((~(X))&(Z)))
#define MD5_G_OP(X,Y,Z) (((X)&(Z)) | ((Y)&(~(Z))))
#define MD5_H_OP(X,Y,Z) ((X)^(Y)^(Z))
#define MD5_I_OP(X,Y,Z) ((Y)^((X)|(~(Z))))

/* The algorithm works on blocks that consist of 16 32bit ints */
static void process_block (_pSLuint32_Type block[16], _pSLuint32_Type abcd[4])
{
   _pSLuint32_Type a = abcd[0], b = abcd[1], c = abcd[2], d = abcd[3];

#define MD5_ROTATE(a,n)   (((a)<<(n)) | ((a)>>(32-(n))))

#define MD5_ROUND1_OP(A,B,C,D,k,s,t) \
   A += MD5_F_OP(B,C,D) + block[k] + t;\
   A = B + MD5_ROTATE(A,s);

#define MD5_ROUND2_OP(A,B,C,D,k,s,t) \
   A += MD5_G_OP(B,C,D) + block[k] + t;\
   A = B + MD5_ROTATE(A,s);

#define MD5_ROUND3_OP(A,B,C,D,k,s,t) \
   A += MD5_H_OP(B,C,D) + block[k] + t;\
   A = B + MD5_ROTATE(A,s);

#define MD5_ROUND4_OP(A,B,C,D,k,s,t) \
   A += MD5_I_OP(B,C,D) + block[k] + t;\
   A = B + MD5_ROTATE(A,s);

   MD5_ROUND1_OP(a,b,c,d,0,7,0xD76AA478);
   MD5_ROUND1_OP(d,a,b,c,1,12,0xE8C7B756);
   MD5_ROUND1_OP(c,d,a,b,2,17,0x242070DB);
   MD5_ROUND1_OP(b,c,d,a,3,22,0xC1BDCEEE);
   MD5_ROUND1_OP(a,b,c,d,4,7,0xF57C0FAF);
   MD5_ROUND1_OP(d,a,b,c,5,12,0x4787C62A);
   MD5_ROUND1_OP(c,d,a,b,6,17,0xA8304613);
   MD5_ROUND1_OP(b,c,d,a,7,22,0xFD469501);
   MD5_ROUND1_OP(a,b,c,d,8,7,0x698098D8);
   MD5_ROUND1_OP(d,a,b,c,9,12,0x8B44F7AF);
   MD5_ROUND1_OP(c,d,a,b,10,17,0xFFFF5BB1);
   MD5_ROUND1_OP(b,c,d,a,11,22,0x895CD7BE);
   MD5_ROUND1_OP(a,b,c,d,12,7,0x6B901122);
   MD5_ROUND1_OP(d,a,b,c,13,12,0xFD987193);
   MD5_ROUND1_OP(c,d,a,b,14,17,0xA679438E);
   MD5_ROUND1_OP(b,c,d,a,15,22,0x49B40821);

   MD5_ROUND2_OP(a,b,c,d,1,5,0xF61E2562);
   MD5_ROUND2_OP(d,a,b,c,6,9,0xC040B340);
   MD5_ROUND2_OP(c,d,a,b,11,14,0x265E5A51);
   MD5_ROUND2_OP(b,c,d,a,0,20,0xE9B6C7AA);
   MD5_ROUND2_OP(a,b,c,d,5,5,0xD62F105D);
   MD5_ROUND2_OP(d,a,b,c,10,9,0x02441453);
   MD5_ROUND2_OP(c,d,a,b,15,14,0xD8A1E681);
   MD5_ROUND2_OP(b,c,d,a,4,20,0xE7D3FBC8);
   MD5_ROUND2_OP(a,b,c,d,9,5,0x21E1CDE6);
   MD5_ROUND2_OP(d,a,b,c,14,9,0xC33707D6);
   MD5_ROUND2_OP(c,d,a,b,3,14,0xF4D50D87);
   MD5_ROUND2_OP(b,c,d,a,8,20,0x455A14ED);
   MD5_ROUND2_OP(a,b,c,d,13,5,0xA9E3E905);
   MD5_ROUND2_OP(d,a,b,c,2,9,0xFCEFA3F8);
   MD5_ROUND2_OP(c,d,a,b,7,14,0x676F02D9);
   MD5_ROUND2_OP(b,c,d,a,12,20,0x8D2A4C8A);

   MD5_ROUND3_OP(a,b,c,d,5,4,0xFFFA3942);
   MD5_ROUND3_OP(d,a,b,c,8,11,0x8771F681);
   MD5_ROUND3_OP(c,d,a,b,11,16,0x6D9D6122);
   MD5_ROUND3_OP(b,c,d,a,14,23,0xFDE5380C);
   MD5_ROUND3_OP(a,b,c,d,1,4,0xA4BEEA44);
   MD5_ROUND3_OP(d,a,b,c,4,11,0x4BDECFA9);
   MD5_ROUND3_OP(c,d,a,b,7,16,0xF6BB4B60);
   MD5_ROUND3_OP(b,c,d,a,10,23,0xBEBFBC70);
   MD5_ROUND3_OP(a,b,c,d,13,4,0x289B7EC6);
   MD5_ROUND3_OP(d,a,b,c,0,11,0xEAA127FA);
   MD5_ROUND3_OP(c,d,a,b,3,16,0xD4EF3085);
   MD5_ROUND3_OP(b,c,d,a,6,23,0x04881D05);
   MD5_ROUND3_OP(a,b,c,d,9,4,0xD9D4D039);
   MD5_ROUND3_OP(d,a,b,c,12,11,0xE6DB99E5);
   MD5_ROUND3_OP(c,d,a,b,15,16,0x1FA27CF8);
   MD5_ROUND3_OP(b,c,d,a,2,23,0xC4AC5665);

   MD5_ROUND4_OP(a,b,c,d,0,6,0xF4292244);
   MD5_ROUND4_OP(d,a,b,c,7,10,0x432AFF97);
   MD5_ROUND4_OP(c,d,a,b,14,15,0xAB9423A7);
   MD5_ROUND4_OP(b,c,d,a,5,21,0xFC93A039);
   MD5_ROUND4_OP(a,b,c,d,12,6,0x655B59C3);
   MD5_ROUND4_OP(d,a,b,c,3,10,0x8F0CCC92);
   MD5_ROUND4_OP(c,d,a,b,10,15,0xFFEFF47D);
   MD5_ROUND4_OP(b,c,d,a,1,21,0x85845DD1);
   MD5_ROUND4_OP(a,b,c,d,8,6,0x6FA87E4F);
   MD5_ROUND4_OP(d,a,b,c,15,10,0xFE2CE6E0);
   MD5_ROUND4_OP(c,d,a,b,6,15,0xA3014314);
   MD5_ROUND4_OP(b,c,d,a,13,21,0x4E0811A1);
   MD5_ROUND4_OP(a,b,c,d,4,6,0xF7537E82);
   MD5_ROUND4_OP(d,a,b,c,11,10,0xBD3AF235);
   MD5_ROUND4_OP(c,d,a,b,2,15,0x2AD7D2BB);
   MD5_ROUND4_OP(b,c,d,a,9,21,0xEB86D391);

   abcd[0] += a;
   abcd[1] += b;
   abcd[2] += c;
   abcd[3] += d;
}

/* Note: Little Endian is used */
static void uint32_to_uchar (_pSLuint32_Type *u, unsigned int num, unsigned char *buf)
{
   unsigned short t = 0xFF;
   unsigned int i;

   if (*(unsigned char *)&t == 0xFF)
     {
	memcpy ((char *)buf, (char *)u, num*4);
	return;
     }

   for (i = 0; i < num; i++)
     {
	_pSLuint32_Type x = u[i];
	buf[0] = (unsigned char) (x & 0xFF);
	buf[1] = (unsigned char) ((x>>8) & 0xFF);
	buf[2] = (unsigned char) ((x>>16) & 0xFF);
	buf[3] = (unsigned char) ((x>>24) & 0xFF);
	buf += 4;
     }
}

static void uchar_to_uint32 (unsigned char *buf, unsigned int len, _pSLuint32_Type *u)
{
   unsigned short x = 0xFF;
   unsigned char *bufmax;

   if (*(unsigned char *)&x == 0xFF)
     {
	memcpy ((char *)u, buf, len);
	return;
     }

   bufmax = buf + len;
   while (buf < bufmax)
     {
	*u++ = (((_pSLuint32_Type)buf[0]) | ((_pSLuint32_Type)buf[1]<<8)
		| ((_pSLuint32_Type)buf[2]<<16) | ((_pSLuint32_Type)buf[3]<<24));
	buf += 4;
     }
}

static void process_64_byte_block (unsigned char *buf, _pSLuint32_Type abcd[4])
{
   _pSLuint32_Type block[16];

   uchar_to_uint32 (buf, 64, block);
   process_block (block, abcd);
}

static void update_num_bits (SLChksum_Type *md5, unsigned int dnum_bits)
{
   _pSLuint32_Type d, lo, hi;

   d = (_pSLuint32_Type)dnum_bits << 3;
   lo = md5->num_bits[0];
   hi = md5->num_bits[1];

   d += lo;
   if (d < lo)
     hi++; /* overflow */
   hi += (_pSLuint32_Type)dnum_bits >> 29;
   lo = d;

   md5->num_bits[0] = lo;
   md5->num_bits[1] = hi;
}

static int md5_accumulate (SLChksum_Type *md5, unsigned char *buf, unsigned int buflen)
{
   unsigned int num_buffered;
   unsigned char *bufmax;

   if ((md5 == NULL) || (buf == NULL))
     return -1;

   update_num_bits (md5, buflen);

   num_buffered = md5->num_buffered;

   if (num_buffered)
     {
	unsigned int dlen = MD5_BUFSIZE - md5->num_buffered;

	if (buflen < dlen)
	  dlen = buflen;

	memcpy (md5->buf+num_buffered, buf, dlen);
	num_buffered += dlen;
	buflen -= dlen;
	buf += dlen;

	if (num_buffered < MD5_BUFSIZE)
	  {
	     md5->num_buffered = num_buffered;
	     return 0;
	  }

	process_64_byte_block (md5->buf, md5->abcd);
	num_buffered = 0;
     }

   num_buffered = buflen % MD5_BUFSIZE;
   bufmax = buf + (buflen - num_buffered);
   while (buf < bufmax)
     {
	process_64_byte_block (buf, md5->abcd);
	buf += MD5_BUFSIZE;
     }

   if (num_buffered)
     memcpy (md5->buf, bufmax, num_buffered);

   md5->num_buffered = num_buffered;

   return 0;
}

static int md5_close (SLChksum_Type *md5, unsigned char *digest)
{
   unsigned char num_bits_buf[8];

   if (md5 == NULL)
     return -1;

   if (digest != NULL)
     {
	/* Handle num bits before padding */
	uint32_to_uchar (md5->num_bits, 2, num_bits_buf);

	/* Add pad and num_bits bytes */
	(void) md5_accumulate (md5, Pad_Bytes, compute_pad_length (md5->num_buffered));
	(void) md5_accumulate (md5, num_bits_buf, 8);
	uint32_to_uchar (md5->abcd, 4, digest);
     }

   SLfree ((char *)md5);
   return 0;
}

SLChksum_Type *_pSLchksum_md5_new (char *name)
{
   SLChksum_Type *md5;
   (void) name;

   if (NULL == (md5 = (SLChksum_Type *) SLmalloc (sizeof(SLChksum_Type))))
     return NULL;
   memset ((char *)md5, 0, sizeof (SLChksum_Type));
   md5->accumulate = md5_accumulate;
   md5->close = md5_close;
   md5->digest_len = MD5_DIGEST_LEN;
   init_md5_buffer (md5->abcd);

   return md5;
}

#if 0
int main ()
{
   SLChksum_Type *md5;
   unsigned int i;
   unsigned char buf[1024];
   unsigned int len;
   unsigned char *digest;

   if (NULL == (md5 = _pSLchksum_md5_new ("md5")))
     return 1;

   while (0 != (len = fread (buf, 1, sizeof (buf), stdin)))
     (void) md5->accumulate (md5, buf, len);

   if (NULL == (digest = (unsigned char *)SLmalloc (md5->digest_len)))
     return 1;

   md5->close (md5, digest);

   for (i = 0; i < 16; i++)
     fprintf (stdout, "%02x", digest[i]);
   fputs ("\n", stdout);
   SLfree ((char *)digest);

   return 0;
}
#endif

