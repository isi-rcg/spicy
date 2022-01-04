/*

# Spca561decoder (C) 2005 Andrzej Szombierski [qq@kuku.eu.org]

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; either version 2.1 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA  02110-1335  USA

# Note this code was originally licensed under the GNU GPL instead of the
# GNU LGPL, its license has been changed with permission, see the permission
# mail at the end of this file.

 */

/*
 *	Decoder for compressed spca561 images
 *	It was developed for "Labtec WebCam Elch 2(SPCA561A)" (046d:0929)
 *	but it might work with other spca561 cameras
 */
#include <string.h>
#include "libv4lconvert-priv.h"

/*fixme: not reentrant */
static unsigned int bit_bucket;
static const unsigned char *input_ptr;

static inline void refill(int *bitfill)
{
	if (*bitfill < 8) {
		bit_bucket = (bit_bucket << 8) | *(input_ptr++);
		*bitfill += 8;
	}
}

static inline int nbits(int *bitfill, int n)
{
	bit_bucket = (bit_bucket << 8) | *(input_ptr++);
	*bitfill -= n;
	return (bit_bucket >> (*bitfill & 0xff)) & ((1 << n) - 1);
}

static inline int _nbits(int *bitfill, int n)
{
	*bitfill -= n;
	return (bit_bucket >> (*bitfill & 0xff)) & ((1 << n) - 1);
}

static int fun_A(int *bitfill)
{
	int ret;
	static int tab[] = {
		12, 13, 14, 15, 16, 17, 18, 19, -12, -13, -14, -15,
		-16, -17, -18, -19, -19
	};

	ret = tab[nbits(bitfill, 4)];

	refill(bitfill);
	return ret;
}

static int fun_B(int *bitfill)
{
	static int tab1[] = {
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 31, 31,
		31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,
		16, 17,
		18,
		19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30
	};
	static int tab[] = {
		4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, -5,
		-6, -7, -8, -9, -10, -11, -12, -13, -14, -15, -16, -17,
		-18, -19
	};
	unsigned int tmp;

	tmp = nbits(bitfill, 7) - 68;
	refill(bitfill);
	if (tmp > 47)
		return 0xff;
	return tab[tab1[tmp]];
}

static int fun_C(int *bitfill, int gkw)
{
	static int tab1[] = {
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 23, 23, 23, 23, 23, 23,
		23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
		12, 13,
		14,
		15, 16, 17, 18, 19, 20, 21, 22
	};
	static int tab[] = {
		8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, -9, -10, -11,
		-12, -13, -14, -15, -16, -17, -18, -19
	};
	unsigned int tmp;

	if (gkw == 0xfe) {
		if (nbits(bitfill, 1) == 0)
			return 7;
		else
			return -8;
	}

	if (gkw != 0xff)
		return 0xff;

	tmp = nbits(bitfill, 7) - 72;
	if (tmp > 43)
		return 0xff;

	refill(bitfill);
	return tab[tab1[tmp]];
}

static int fun_D(int *bitfill, int gkw)
{
	if (gkw == 0xfd) {
		if (nbits(bitfill, 1) == 0)
			return 12;
		return -13;
	}

	if (gkw == 0xfc) {
		if (nbits(bitfill, 1) == 0)
			return 13;
		return -14;
	}

	if (gkw == 0xfe) {
		switch (nbits(bitfill, 2)) {
		case 0:
			return 14;
		case 1:
			return -15;
		case 2:
			return 15;
		case 3:
			return -16;
		}
	}

	if (gkw == 0xff) {
		switch (nbits(bitfill, 3)) {
		case 4:
			return 16;
		case 5:
			return -17;
		case 6:
			return 17;
		case 7:
			return -18;
		case 2:
			return _nbits(bitfill, 1) ? 0xed : 0x12;
		case 3:
			(*bitfill)--;
			return 18;
		}
		return 0xff;
	}
	return gkw;
}

static int fun_E(int cur_byte, int *bitfill)
{
	static int tab0[] = { 0, -1, 1, -2, 2, -3, 3, -4 };
	static int tab1[] = { 4, -5, 5, -6, 6, -7, 7, -8 };
	static int tab2[] = { 8, -9, 9, -10, 10, -11, 11, -12 };
	static int tab3[] = { 12, -13, 13, -14, 14, -15, 15, -16 };
	static int tab4[] = { 16, -17, 17, -18, 18, -19, 19, -19 };

	if ((cur_byte & 0xf0) >= 0x80) {
		*bitfill -= 4;
		return tab0[(cur_byte >> 4) & 7];
	}
	if ((cur_byte & 0xc0) == 0x40) {
		*bitfill -= 5;
		return tab1[(cur_byte >> 3) & 7];

	}
	if ((cur_byte & 0xe0) == 0x20) {
		*bitfill -= 6;
		return tab2[(cur_byte >> 2) & 7];

	}
	if ((cur_byte & 0xf0) == 0x10) {
		*bitfill -= 7;
		return tab3[(cur_byte >> 1) & 7];

	}
	if ((cur_byte & 0xf8) == 8) {
		*bitfill -= 8;
		return tab4[cur_byte & 7];
	}
	return 0xff;
}

static int fun_F(int cur_byte, int *bitfill)
{
	*bitfill -= 5;
	switch (cur_byte & 0xf8) {
	case 0x80:
		return 0;
	case 0x88:
		return -1;
	case 0x90:
		return 1;
	case 0x98:
		return -2;
	case 0xa0:
		return 2;
	case 0xa8:
		return -3;
	case 0xb0:
		return 3;
	case 0xb8:
		return -4;
	case 0xc0:
		return 4;
	case 0xc8:
		return -5;
	case 0xd0:
		return 5;
	case 0xd8:
		return -6;
	case 0xe0:
		return 6;
	case 0xe8:
		return -7;
	case 0xf0:
		return 7;
	case 0xf8:
		return -8;
	}

	*bitfill -= 1;
	switch (cur_byte & 0xfc) {
	case 0x40:
		return 8;
	case 0x44:
		return -9;
	case 0x48:
		return 9;
	case 0x4c:
		return -10;
	case 0x50:
		return 10;
	case 0x54:
		return -11;
	case 0x58:
		return 11;
	case 0x5c:
		return -12;
	case 0x60:
		return 12;
	case 0x64:
		return -13;
	case 0x68:
		return 13;
	case 0x6c:
		return -14;
	case 0x70:
		return 14;
	case 0x74:
		return -15;
	case 0x78:
		return 15;
	case 0x7c:
		return -16;
	}

	*bitfill -= 1;
	switch (cur_byte & 0xfe) {
	case 0x20:
		return 16;
	case 0x22:
		return -17;
	case 0x24:
		return 17;
	case 0x26:
		return -18;
	case 0x28:
		return 18;
	case 0x2a:
		return -19;
	case 0x2c:
		return 19;
	}

	*bitfill += 7;
	return 0xff;
}

static int internal_spca561_decode(int width, int height,
		const unsigned char *inbuf,
		unsigned char *outbuf)
{
	/* buffers */
	static int accum[8 * 8 * 8];
	static int i_hits[8 * 8 * 8];

	static const int nbits_A[] = {
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1,
		1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1,
		1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1,
		1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1,
		1, 1, 1, 1, 1,
		8, 8, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 0, 7, 7,
		7, 7,
		7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
		3, 3,
		3, 3, 3, 3, 3,
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		5, 5,
		5, 5, 5, 5, 5,
		5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3, 3, 3, 3, 3,
		3, 3,
		3, 3, 3, 3, 3,
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	};
	static const int tab_A[] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0, 11, -11, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9,
		10, 10,
		255, 254, -4,
		-4, -5, -5, -6, -6, -7, -7, -8, -8, -9, -9, -10, -10, -1,
		-1, -1,
		-1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1,
		-1, -1,
		-1, -1, -1, -1, -1, -1, -1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3,
		3, 3, 3,
		3, 3, 3,
		-2, -2, -2, -2, -2, -2, -2, -2, -3, -3, -3, -3, -3, -3, -3,
		-3, 1,
		1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1,
		1, 1, 1, 1,
		1
	};

	static const int nbits_B[] = {
		0, 8, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4,
		4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3,
		3, 3,
		3, 3, 3, 3, 3,
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2,
		2, 2,
		2, 2, 2, 2, 2,
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
		2, 2,
		2, 2, 2, 2, 2,
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
		2, 2,
		2, 2, 2, 2, 2,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1,
		1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1,
		1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1,
		1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1,
		1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	};
	static const int tab_B[] = {
		0xff, -4, 3, 3, -3, -3, -3, -3, 2, 2, 2, 2, 2, 2, 2, 2, -2,
		-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
		1, 1,
		1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1,
		1, 1, 1, 1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1,
		-1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1,
		-1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1,
		-1, -1,
		-1, -1, -1, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0,
	};

	static const int nbits_C[] = {
		0, 0, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5,
		5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4,
		4, 4,
		4, 4, 4, 4, 4,
		4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3,
		3, 3,
		3, 3, 3, 3, 3,
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
		3, 3,
		3, 3, 3, 3, 3,
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
		3, 3,
		3, 3, 3, 3, 3,
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
		2, 2,
		2, 2, 2, 2, 2,
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
		2, 2,
		2, 2, 2, 2, 2,
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
		2, 2,
		2, 2, 2, 2, 2,
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
		2, 2,
		2, 2, 2, 2, 2,
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	};
	static const int tab_C[] = {
		0xff, 0xfe, 6, -7, 5, 5, -6, -6, 4, 4, 4, 4, -5, -5, -5, -5,
		3, 3, 3, 3, 3, 3, 3, 3, -4, -4, -4, -4, -4, -4, -4, -4, 2,
		2, 2, 2,
		2, 2, 2,
		2, 2, 2, 2, 2, 2, 2, 2, 2, -3, -3, -3, -3, -3, -3, -3, -3,
		-3, -3,
		-3, -3, -3,
		-3, -3, -3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1,
		1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, -2, -2, -2, -2, -2, -2, -2,
		-2, -2,
		-2, -2, -2,
		-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
		-2, -2,
		-2, -2,
		-2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1,
		-1, -1,
		-1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1,
		-1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1,
		-1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1,
	};

	static const int nbits_D[] = {
		0, 0, 0, 0, 8, 8, 8, 8, 7, 7, 7, 7, 7, 7, 7, 7, 6, 6, 6, 6,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5,
		5, 5,
		5, 5, 5, 5, 5,
		5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4,
		4, 4,
		4, 4, 4, 4, 4,
		4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
		4, 4,
		4, 4, 4, 4, 4,
		4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
		4, 4,
		4, 4, 4, 4, 4,
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
		3, 3,
		3, 3, 3, 3, 3,
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
		3, 3,
		3, 3, 3, 3, 3,
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
		3, 3,
		3, 3, 3, 3, 3,
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
		3, 3,
		3, 3, 3, 3, 3,
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3
	};
	static const int tab_D[] = {
		0xff, 0xfe, 0xfd, 0xfc, 10, -11, 11, -12, 8, 8, -9, -9, 9, 9,
		-10, -10, 6, 6, 6, 6, -7, -7, -7, -7, 7, 7, 7, 7, -8, -8,
		-8, -8,
		4, 4, 4, 4,
		4, 4, 4, 4, -5, -5, -5, -5, -5, -5, -5, -5, 5, 5, 5, 5, 5,
		5, 5, 5,
		-6, -6,
		-6, -6, -6, -6, -6, -6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
		2, 2,
		2, 2, -3,
		-3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3,
		3, 3,
		3, 3, 3, 3,
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, -4, -4, -4, -4, -4, -4, -4,
		-4, -4,
		-4, -4, -4,
		-4, -4, -4, -4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1,
		-1, -1,
		-1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1,
		-1, -1,
		-1, -1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1,
		1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, -2, -2, -2, -2, -2, -2, -2, -2,
		-2, -2,
		-2, -2, -2,
		-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
		-2, -2,
		-2, -2
	};

	/* a_curve[19 + i] = ... [-19..19] => [-160..160] */
	static const int a_curve[] = {
		-160, -144, -128, -112, -98, -88, -80, -72, -64, -56, -48,
		-40, -32, -24, -18, -12, -8, -5, -2, 0, 2, 5, 8, 12, 18,
		24, 32,
		40, 48, 56, 64,
		72, 80, 88, 98, 112, 128, 144, 160
	};
	/* clamp0_255[256 + i] = min(max(i,255),0) */
	static const unsigned char clamp0_255[] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 2,
		3, 4, 5, 6, 7, 8, 9, 10,
		11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
		26, 27,
		28, 29, 30, 31, 32, 33,
		34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
		49, 50,
		51, 52, 53, 54, 55, 56,
		57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71,
		72, 73,
		74, 75, 76, 77, 78, 79,
		80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94,
		95, 96,
		97, 98, 99, 100, 101,
		102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113,
		114,
		115, 116, 117, 118, 119,
		120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131,
		132,
		133, 134, 135, 136, 137,
		138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149,
		150,
		151, 152, 153, 154, 155,
		156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167,
		168,
		169, 170, 171, 172, 173,
		174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185,
		186,
		187, 188, 189, 190, 191,
		192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203,
		204,
		205, 206, 207, 208, 209,
		210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221,
		222,
		223, 224, 225, 226, 227,
		228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
		240,
		241, 242, 243, 244, 245,
		246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 255, 255,
		255,
		255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255,
		255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255,
		255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255,
		255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255,
		255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255,
		255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255,
		255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255,
		255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255,
		255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255,
		255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255,
		255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255,
		255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255,
		255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255,
		255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255,
		255
	};
	/* abs_clamp15[19 + i] = min(abs(i), 15) */
	static const int abs_clamp15[] = {
		15, 15, 15, 15, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3,
		2, 1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
		15, 15,
		15
	};
	/* diff_encoding[256 + i] = ... */
	static const int diff_encoding[] = {
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7,
		7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7,
		7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7,
		7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7,
		7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7,
		7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7,
		7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7,
		7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 5,
		5, 5,
		5, 5, 5, 5, 5, 3, 3,
		3, 3, 1, 1, 0, 2, 2, 4, 4, 4, 4, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		6, 6,
		6, 6, 6, 6, 6, 6, 6,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		6, 6,
		6, 6, 6, 6, 6, 6, 6,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		6, 6,
		6, 6, 6, 6, 6, 6, 6,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		6, 6,
		6, 6, 6, 6, 6, 6, 6,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		6, 6,
		6, 6, 6, 6, 6, 6, 6,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		6, 6,
		6, 6, 6, 6, 6, 6, 6,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		6, 6,
		6, 6, 6, 6, 6, 6, 6,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		6, 6,
		6, 6, 6, 6, 6, 6, 6,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		6, 6,
		6, 6, 6, 6, 6, 6
	};

	int block;
	int bitfill = 0;
	int xwidth = width + 6;
	int off_up_right = 2 - 2 * xwidth;
	int off_up_left = -2 - 2 * xwidth;
	int pixel_U = 0, saved_pixel_UR = 0;
	int pixel_x = 0, pixel_y = 2;
	unsigned char *output_ptr = outbuf;

	memset(i_hits, 0, sizeof(i_hits));
	memset(accum, 0, sizeof(accum));

	memcpy(outbuf + xwidth * 2 + 3, inbuf + 0x14, width);
	memcpy(outbuf + xwidth * 3 + 3, inbuf + 0x14 + width, width);

	input_ptr = inbuf + 0x14 + width * 2;
	output_ptr = outbuf + (xwidth) * 4 + 3;

	bit_bucket = 0;

	for (block = 0; block < ((height - 2) * width) / 32; ++block) {
		int b_it, var_7 = 0;
		int cur_byte;

		refill(&bitfill);

		cur_byte = (bit_bucket >> (bitfill & 7)) & 0xff;

		if ((cur_byte & 0x80) == 0) {
			var_7 = 0;
			bitfill--;
		} else if ((cur_byte & 0xC0) == 0x80) {
			var_7 = 1;
			bitfill -= 2;
		} else if ((cur_byte & 0xc0) == 0xc0) {
			var_7 = 2;
			bitfill -= 2;
		}

		for (b_it = 0; b_it < 32; b_it++) {
			int index;
			int pixel_L, pixel_UR, pixel_UL;
			int multiplier;
			int dL, dC, dR;
			int gkw;	/* God knows what */

			refill(&bitfill);
			cur_byte = bit_bucket >> (bitfill & 7) & 0xff;

			pixel_L = output_ptr[-2];
			pixel_UR = output_ptr[off_up_right];
			pixel_UL = output_ptr[off_up_left];

			dL = diff_encoding[0x100 + pixel_UL - pixel_L];
			dC = diff_encoding[0x100 + pixel_U - pixel_UL];
			dR = diff_encoding[0x100 + pixel_UR - pixel_U];

			if (pixel_x < 2) {
				pixel_L = pixel_UL = pixel_U =
					output_ptr[-xwidth * 2];
				pixel_UR = output_ptr[off_up_right];
				dL = dC = 0;
				dR = diff_encoding[0x100 + pixel_UR -
					pixel_U];
			} else if (pixel_x > width - 3)
				dR = 0;

			multiplier = 4;
			index = dR + dC * 8 + dL * 64;

			if (pixel_L + pixel_U * 2 <= 144
					&& (pixel_y & 1) == 0
					&& (b_it & 3) == 0 && (dR < 5) && (dC < 5)
					&& (dL < 5)) {
				multiplier = 1;
			} else if (pixel_L <= 48
					&& dL <= 4 && dC <= 4 && dL >= 1
					&& dC >= 1) {
				multiplier = 2;
			} else if (var_7 == 1) {
				multiplier = 2;
			} else if (dC + dL >= 11 || var_7 == 2) {
				multiplier = 8;
			}

			if (i_hits[index] < 7) {
				bitfill -= nbits_A[cur_byte];
				gkw = tab_A[cur_byte];
				if (gkw == 0xfe)
					gkw = fun_A(&bitfill);
			} else if (i_hits[index] >= accum[index]) {
				bitfill -= nbits_B[cur_byte];
				gkw = tab_B[cur_byte];
				if (cur_byte == 0)
					gkw = fun_B(&bitfill);
			} else if (i_hits[index] * 2 >= accum[index]) {
				bitfill -= nbits_C[cur_byte];
				gkw = tab_C[cur_byte];
				if (cur_byte < 2)
					gkw = fun_C(&bitfill, gkw);
			} else if (i_hits[index] * 4 >= accum[index]) {
				bitfill -= nbits_D[cur_byte];
				gkw = tab_D[cur_byte];
				if (cur_byte < 4)
					gkw = fun_D(&bitfill, gkw);
			} else if (i_hits[index] * 8 >= accum[index]) {
				gkw = fun_E(cur_byte, &bitfill);
			} else {
				gkw = fun_F(cur_byte, &bitfill);
			}

			if (gkw == 0xff)
				return -3;

			{
				int tmp1, tmp2;

				tmp1 =
					(pixel_U + pixel_L) * 3 - pixel_UL * 2;
				tmp1 += (tmp1 < 0) ? 3 : 0;
				tmp2 = a_curve[19 + gkw] * multiplier;
				tmp2 += (tmp2 < 0) ? 1 : 0;

				*(output_ptr++) =
					clamp0_255[0x100 + (tmp1 >> 2) -
					(tmp2 >> 1)];
			}
			pixel_U = saved_pixel_UR;
			saved_pixel_UR = pixel_UR;

			if (++pixel_x == width) {
				output_ptr += 6;
				pixel_x = 0;
				pixel_y++;
			}

			accum[index] += abs_clamp15[19 + gkw];

			if (i_hits[index]++ == 15) {
				i_hits[index] = 8;
				accum[index] /= 2;
			}
		}
	}
	return 0;
}

/* FIXME, change internal_spca561_decode not to need the extra border
   around its dest buffer */
void v4lconvert_decode_spca561(const unsigned char *inbuf,
		unsigned char *outbuf, int width, int height)
{
	int i;
	static unsigned char tmpbuf[650 * 490];

	if (internal_spca561_decode(width, height, inbuf, tmpbuf) != 0)
		return;
	for (i = 0; i < height; i++)
		memcpy(outbuf + i * width,
				tmpbuf + (i + 2) * (width + 6) + 3, width);
}

/*************** License Change Permission Notice ***************

  Return-Path: <qq@kuku.eu.org>
Received: from koko.hhs.nl ([145.52.2.16] verified)
by hhs.nl (CommuniGate Pro SMTP 4.3.6)
with ESMTP id 88574071 for j.w.r.degoede@hhs.nl; Mon, 16 Jun 2008 16:36:24 +0200
Received: from exim (helo=koko)
by koko.hhs.nl with local-smtp (Exim 4.62)
(envelope-from <qq@kuku.eu.org>)
id 1K8Fom-0002iJ-3K
for j.w.r.degoede@hhs.nl; Mon, 16 Jun 2008 16:36:24 +0200
Received: from [192.87.102.74] (port=41377 helo=filter6-ams.mf.surf.net)
by koko.hhs.nl with esmtp (Exim 4.62)
(envelope-from <qq@kuku.eu.org>)
id 1K8Fol-0002iC-Qo
for j.w.r.degoede@hhs.nl; Mon, 16 Jun 2008 16:36:23 +0200
Received: from kuku.eu.org (pa90.wielkopole.sdi.tpnet.pl [217.99.123.90])
by filter6-ams.mf.surf.net (8.13.8/8.13.8/Debian-3) with ESMTP id m5GEa55r001787
for <j.w.r.degoede@hhs.nl>; Mon, 16 Jun 2008 16:36:06 +0200
Received: (qmail 2243 invoked by uid 500); 16 Jun 2008 14:29:37 -0000
Date: Mon, 16 Jun 2008 16:29:37 +0200 (CEST)
From: Andrzej Szombierski <qq@kuku.eu.org>
To: Hans de Goede <j.w.r.degoede@hhs.nl>
Subject: Re: spca561 decoder license question
In-Reply-To: <485673B6.4050003@hhs.nl>
Message-ID: <Pine.LNX.4.44L.0806161614560.7665-100000@kuku.eu.org>
References: <485673B6.4050003@hhs.nl>
MIME-Version: 1.0
Content-Type: TEXT/PLAIN; charset=iso-8859-2
Content-Transfer-Encoding: QUOTED-PRINTABLE
X-Canit-CHI2: 0.00
X-Bayes-Prob: 0.0001 (Score 0, tokens from: @@RPTN)
X-Spam-Score: 2.00 (**) [Tag at 6.00] RBL(uceprotect-blacklist.surfnet.nl,2.0)
X-CanItPRO-Stream: hhs:j.w.r.degoede@hhs.nl (inherits from hhs:default,base:default)
X-Canit-Stats-ID: 85673281 - 37e52c8b07bc
X-Scanned-By: CanIt (www . roaringpenguin . com) on 192.87.102.74
X-Anti-Virus: Kaspersky Anti-Virus for MailServers 5.5.2/RELEASE, bases: 16062008 #776409, status: clean

On Mon, 16 Jun 2008, Hans de Goede wrote:

> Hi,
>=20
> I don't know if you're still subscribed to the spca devel mailing list, s=
o let=20
> me start with a short intro.
>
> I'm a Linux enthusiast / developer currently helping Jean-Fran=E7ois Moin=
e with=20
> porting gspca to video4linux2 and cleaning up the code to get it ready fo=
r=20
> mainline kernel inclusion.
>=20
> As part of this process the decompression code for all supported cams mus=
t be=20
> moved to userspace, as doing in kernel decompression is considered unwant=
ed by=20
> the mainline people (I agree) as it should be done in userspace.
>

Sounds reasonable.
=20
> As such I'm working on a library which does decompression of custom cam f=
ormats=20
> in userspace.
>

Nice. I hope that the library won't be limited to spca-supported webcams,=
=20
and as an application developer I would be able to just request RGB data=20
from any /dev/video*, right?

> I do not want to license this library as GPL (as the current spca code is=
		), as=20
> it should be usable by as much software as possible. Instead I want to li=
cense=20
> it under the LGPL version 2.1 or later.

Also sounds reasonable.

>=20
> So my question us my I have your permission to relicense your spca561=20
> decompression code under the LGPL?
>=20

Yes, of course.=20

> Thanks & Regards,
	>=20
	> Hans
	>=20
	>

	--=20
	:: Andrzej Szombierski :: qq@kuku.eu.org :: http://kuku.eu.org ::
	:: anszom@bezkitu.com  :: radio bez kitu :: http://bezkitu.com ::

	*/
