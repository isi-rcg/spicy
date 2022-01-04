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
#include <limits.h>
#include <slang.h>

#include "_slint.h"

#define SHA1_BUFSIZE	64
#define SHA1_DIGEST_LEN	20
#define CHKSUM_TYPE_PRIVATE_FIELDS \
   _pSLuint32_Type h[5]; \
   _pSLuint32_Type num_bits[2];		       /* 64 bit representation */ \
   unsigned int num_buffered; \
   unsigned char buf[SHA1_BUFSIZE];

#include "chksum.h"

/* 8 bit bytes assumed in what follows.  The algorithm is based upon
 * <http://www.itl.nist.gov/fipspubs/fip180-1.htm>
 *
 * The algorithm there resembles that of RFC1321, which defines the
 * MD5 checksum.  The notes that follow have been adapted from my
 * md5sum code.
 */

/*
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

/* Padding is performed as follows: a single "1" bit is appended to the
 *  message, and then "0" bits are appended so that the length in bits
 *  of the padded message becomes congruent to 448, modulo 512. In
 *  all, at least one bit and at most 512 bits are appended.
 *
 * A 64-bit representation of b (the length of the message before the
 * padding bits were added) is appended to the result of the previous
 * step. In the unlikely event that b is greater than 2^64, then only
 * the low-order 64 bits of b are used. (These bits are appended as two
 * 32-bit words and appended low-order word first in accordance with the
 * previous conventions.)
 *
 * At this point the resulting message (after padding with bits and with
 * b) has a length that is an exact multiple of 512 bits. Equivalently,
 * this message has a length that is an exact multiple of 16 (32-bit)
 * words. Let M[0 ... N-1] denote the words of the resulting message,
 * where N is a multiple of 16.
 */
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
 * The message digest is computed using the final padded message. The
 * computation uses two buffers, each consisting of five 32-bit
 * words, and a sequence of eighty 32-bit words. The words of the
 * first 5-word buffer are labeled A,B,C,D,E. The words of the second
 * 5-word buffer are labeled H0, H1, H2, H3, H4. The words of the
 * 80-word sequence are labeled W0, W1,..., W79. A single word buffer
 * TEMP is also employed.
 */

/* Initialization
 *
 * A four-word buffer (A,B,C,D) is used to compute the message digest.
 * Here each of A, B, C, D is a 32-bit register. These registers are
 * initialized to the following values in hexadecimal.
 */

static _pSLuint32_Type overflow_add (_pSLuint32_Type a, _pSLuint32_Type b, _pSLuint32_Type *c)
{
   _pSLuint32_Type b1 = UINT_MAX - b;
   if (a <= b1)
     {
	*c = 0;
	return a+b;
     }
   *c = 1;
   return (a - b1) - 1;
}

static int update_num_bits (SLChksum_Type *sha1, unsigned int dnum_bits)
{
   _pSLuint32_Type lo, hi, c, d;

   d = (_pSLuint32_Type)dnum_bits << 3;
   hi = sha1->num_bits[0];
   lo = sha1->num_bits[1];

   lo = overflow_add (lo, d, &c);
   if (c)
     {
	hi = overflow_add (hi, c, &c);
	if (c)
	  return -1;
     }
   hi = overflow_add (hi, dnum_bits >> 29, &c);
   if (c)
     return -1;

   sha1->num_bits[0] = hi;
   sha1->num_bits[1] = lo;
   return 0;
}

#define F_00_19(B,C,D) (((B) & (C)) | (~(B) & (D)))
#define F_20_39(B,C,D) ((B)^(C)^(D))
#define F_40_59(B,C,D) (((B)&(C)) | ((B)&(D)) | ((C)&(D)))
#define F_60_79(B,C,D) ((B)^(C)^(D))
#define K_00_19 0x5A827999
#define K_20_39 0x6ED9EBA1
#define K_40_59 0x8F1BBCDC
#define K_60_79 0xCA62C1D6
#define CSHIFT(n,X) (((X) << (n)) | ((X) >> (32-(n))))
#define MAKE_WORD(b) \
   ((((_pSLuint32_Type)(b[0]))<<24) | (((_pSLuint32_Type)(b[1]))<<16) \
     | (((_pSLuint32_Type)(b[2]))<<8) | ((_pSLuint32_Type)(b[3])))

static int sha1_process_block (SLChksum_Type *sha1, unsigned char *buf)
{
   _pSLuint32_Type a, b, c, d, e;
   _pSLuint32_Type w[80];
   unsigned int t;

   for (t = 0; t < 16; t++)
     {
	w[t] = MAKE_WORD(buf);
	buf += 4;
     }
   for (t = 16; t < 80; t++)
     {
	_pSLuint32_Type x = w[t-3] ^ w[t-8] ^ w[t-14] ^ w[t-16];
	w[t] = CSHIFT(1, x);
     }

   a = sha1->h[0]; b = sha1->h[1]; c = sha1->h[2]; d = sha1->h[3]; e = sha1->h[4];

   for (t = 0; t < 20; t++)
     {
	_pSLuint32_Type tmp;
	tmp = CSHIFT(5,a) + F_00_19(b,c,d) + e + w[t] + K_00_19;
	e = d; d = c; c = CSHIFT(30,b); b = a; a = tmp;
     }
   for (t = 20; t < 40; t++)
     {
	_pSLuint32_Type tmp;
	tmp = CSHIFT(5,a) + F_20_39(b,c,d) + e + w[t] + K_20_39;
	e = d; d = c; c = CSHIFT(30,b); b = a; a = tmp;
     }
   for (t = 40; t < 60; t++)
     {
	_pSLuint32_Type tmp;
	tmp = CSHIFT(5,a) + F_40_59(b,c,d) + e + w[t] + K_40_59;
	e = d; d = c; c = CSHIFT(30,b); b = a; a = tmp;
     }
   for (t = 60; t < 80; t++)
     {
	_pSLuint32_Type tmp;
	tmp = CSHIFT(5,a) + F_60_79(b,c,d) + e + w[t] + K_60_79;
	e = d; d = c; c = CSHIFT(30,b); b = a; a = tmp;
     }

   sha1->h[0] += a; sha1->h[1] += b; sha1->h[2] += c; sha1->h[3] += d; sha1->h[4] += e;

   return 0;
}

static int sha1_accumulate (SLChksum_Type *sha1, unsigned char *buf, unsigned int buflen)
{
   unsigned int num_buffered;
   unsigned char *bufmax;

   if ((sha1 == NULL) || (buf == NULL))
     return -1;

   update_num_bits (sha1, buflen);

   num_buffered = sha1->num_buffered;

   if (num_buffered)
     {
	unsigned int dlen = SHA1_BUFSIZE - sha1->num_buffered;

	if (buflen < dlen)
	  dlen = buflen;

	memcpy (sha1->buf+num_buffered, buf, dlen);
	num_buffered += dlen;
	buflen -= dlen;
	buf += dlen;

	if (num_buffered < SHA1_BUFSIZE)
	  {
	     sha1->num_buffered = num_buffered;
	     return 0;
	  }

	sha1_process_block (sha1, sha1->buf);
	num_buffered = 0;
     }

   num_buffered = buflen % SHA1_BUFSIZE;
   bufmax = buf + (buflen - num_buffered);
   while (buf < bufmax)
     {
	sha1_process_block (sha1, buf);
	buf += SHA1_BUFSIZE;
     }

   if (num_buffered)
     memcpy (sha1->buf, bufmax, num_buffered);

   sha1->num_buffered = num_buffered;

   return 0;
}

static void uint32_to_uchar (_pSLuint32_Type *u, unsigned int num, unsigned char *buf)
{
   unsigned int i;

   for (i = 0; i < num; i++)
     {
	_pSLuint32_Type x = u[i];
	buf[3] = (unsigned char) (x & 0xFF);
	buf[2] = (unsigned char) ((x>>8) & 0xFF);
	buf[1] = (unsigned char) ((x>>16) & 0xFF);
	buf[0] = (unsigned char) ((x>>24) & 0xFF);
	buf += 4;
     }
}

static int sha1_close (SLChksum_Type *sha1, unsigned char *digest)
{
   unsigned char num_bits_buf[8];

   if (sha1 == NULL)
     return -1;

   if (digest != NULL)
     {
	/* Handle num bits before padding */
	uint32_to_uchar (sha1->num_bits, 2, num_bits_buf);

	/* Add pad and num_bits bytes */
	(void) sha1_accumulate (sha1, Pad_Bytes, compute_pad_length (sha1->num_buffered));
	(void) sha1_accumulate (sha1, num_bits_buf, 8);
	uint32_to_uchar (sha1->h, 5, digest);
     }

   SLfree ((char *)sha1);
   return 0;
}

SLChksum_Type *_pSLchksum_sha1_new (char *name)
{
   SLChksum_Type *sha1;

   (void) name;
   sha1 = (SLChksum_Type *)SLmalloc (sizeof (SLChksum_Type));
   if (sha1 == NULL)
     return NULL;
   memset ((char *)sha1, 0, sizeof (SLChksum_Type));

   sha1->accumulate = sha1_accumulate;
   sha1->close = sha1_close;
   sha1->digest_len = SHA1_DIGEST_LEN;

   sha1->h[0] = 0x67452301;
   sha1->h[1] = 0xEFCDAB89;
   sha1->h[2] = 0x98BADCFE;
   sha1->h[3] = 0x10325476;
   sha1->h[4] = 0xC3D2E1F0;

   return sha1;
}

#if 0
int main (int argc, char **argv)
{
   SLChksum_Type *sha1;
   unsigned int i;
   unsigned char buf[1024];
   unsigned int len;
   unsigned char digest[SHA1_DIGEST_LEN];
   char *s;

   if (NULL == (sha1 = sha1_open ()))
     return 1;
#if 0
   while (0 != (len = fread (buf, 1, sizeof (buf), stdin)))
     (void) sha1_accumulate (sha1, buf, len);
#else
   for (i = 0; i < 1000000; i++)
     {
	s = "a";
	(void) sha1_accumulate (sha1, s, strlen(s));
     }
#endif
   sha1_close (sha1, digest);

   for (i = 0; i < 20; i++)
     fprintf (stdout, "%02x", digest[i]);

   fputs ("\n", stdout);
   return 0;
}
#endif
