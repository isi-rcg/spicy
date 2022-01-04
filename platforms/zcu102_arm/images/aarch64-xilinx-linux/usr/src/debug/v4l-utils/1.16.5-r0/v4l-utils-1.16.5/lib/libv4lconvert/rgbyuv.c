/*

# RGB <-> YUV conversion routines
#             (C) 2008 Hans de Goede <hdegoede@redhat.com>

# RGB565 conversion routines
#             (C) 2009 Mauro Carvalho Chehab

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

 */

#include <string.h>
#include "libv4lconvert-priv.h"

#define RGB2Y(r, g, b, y) \
	(y) = ((8453 * (r) + 16594 * (g) + 3223 * (b) + 524288) >> 15)

#define RGB2UV(r, g, b, u, v) 	\
	do {			\
		(u) = ((-4878 * (r) - 9578 * (g) + 14456 * (b) + 4210688) >> 15); \
		(v) = ((14456 * (r) - 12105 * (g) - 2351 * (b) + 4210688) >> 15); \
	} while (0)

void v4lconvert_rgb24_to_yuv420(const unsigned char *src, unsigned char *dest,
		const struct v4l2_format *src_fmt, int bgr, int yvu, int bpp)
{
	int x, y;
	unsigned char *udest, *vdest;

	/* Y */
	for (y = 0; y < src_fmt->fmt.pix.height; y++) {
		for (x = 0; x < src_fmt->fmt.pix.width; x++) {
			if (bgr)
				RGB2Y(src[2], src[1], src[0], *dest++);
			else
				RGB2Y(src[0], src[1], src[2], *dest++);
			src += bpp;
		}

		src += src_fmt->fmt.pix.bytesperline - bpp * src_fmt->fmt.pix.width;
	}
	src -= src_fmt->fmt.pix.height * src_fmt->fmt.pix.bytesperline;

	/* U + V */
	if (yvu) {
		vdest = dest;
		udest = dest + src_fmt->fmt.pix.width * src_fmt->fmt.pix.height / 4;
	} else {
		udest = dest;
		vdest = dest + src_fmt->fmt.pix.width * src_fmt->fmt.pix.height / 4;
	}

	for (y = 0; y < src_fmt->fmt.pix.height / 2; y++) {
		for (x = 0; x < src_fmt->fmt.pix.width / 2; x++) {
			int avg_src[3];

			avg_src[0] = (src[0] + src[bpp] + src[src_fmt->fmt.pix.bytesperline] +
					src[src_fmt->fmt.pix.bytesperline + bpp]) / 4;
			avg_src[1] = (src[1] + src[bpp + 1] + src[src_fmt->fmt.pix.bytesperline + 1] +
					src[src_fmt->fmt.pix.bytesperline + bpp + 1]) / 4;
			avg_src[2] = (src[2] + src[bpp + 2] + src[src_fmt->fmt.pix.bytesperline + 2] +
					src[src_fmt->fmt.pix.bytesperline + bpp + 2]) / 4;
			if (bgr)
				RGB2UV(avg_src[2], avg_src[1], avg_src[0], *udest++, *vdest++);
			else
				RGB2UV(avg_src[0], avg_src[1], avg_src[2], *udest++, *vdest++);
			src += 2 * bpp;
		}
		src += 2 * src_fmt->fmt.pix.bytesperline - bpp * src_fmt->fmt.pix.width;
	}
}

#define YUV2R(y, u, v) ({ \
		int r = (y) + ((((v) - 128) * 1436) >> 10); r > 255 ? 255 : r < 0 ? 0 : r; })
#define YUV2G(y, u, v) ({ \
		int g = (y) - ((((u) - 128) * 352 + ((v) - 128) * 731) >> 10); g > 255 ? 255 : g < 0 ? 0 : g; })
#define YUV2B(y, u, v) ({ \
		int b = (y) + ((((u) - 128) * 1814) >> 10); b > 255 ? 255 : b < 0 ? 0 : b; })

#define CLIP(color) (unsigned char)(((color) > 0xFF) ? 0xff : (((color) < 0) ? 0 : (color)))

void v4lconvert_yuv420_to_bgr24(const unsigned char *src, unsigned char *dest,
		int width, int height, int yvu)
{
	int i, j;

	const unsigned char *ysrc = src;
	const unsigned char *usrc, *vsrc;

	if (yvu) {
		vsrc = src + width * height;
		usrc = vsrc + (width * height) / 4;
	} else {
		usrc = src + width * height;
		vsrc = usrc + (width * height) / 4;
	}

	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j += 2) {
#if 1 /* fast slightly less accurate multiplication free code */
			int u1 = (((*usrc - 128) << 7) +  (*usrc - 128)) >> 6;
			int rg = (((*usrc - 128) << 1) +  (*usrc - 128) +
					((*vsrc - 128) << 2) + ((*vsrc - 128) << 1)) >> 3;
			int v1 = (((*vsrc - 128) << 1) +  (*vsrc - 128)) >> 1;

			*dest++ = CLIP(*ysrc + u1);
			*dest++ = CLIP(*ysrc - rg);
			*dest++ = CLIP(*ysrc + v1);
			ysrc++;

			*dest++ = CLIP(*ysrc + u1);
			*dest++ = CLIP(*ysrc - rg);
			*dest++ = CLIP(*ysrc + v1);
#else
			*dest++ = YUV2B(*ysrc, *usrc, *vsrc);
			*dest++ = YUV2G(*ysrc, *usrc, *vsrc);
			*dest++ = YUV2R(*ysrc, *usrc, *vsrc);
			ysrc++;

			*dest++ = YUV2B(*ysrc, *usrc, *vsrc);
			*dest++ = YUV2G(*ysrc, *usrc, *vsrc);
			*dest++ = YUV2R(*ysrc, *usrc, *vsrc);
#endif
			ysrc++;
			usrc++;
			vsrc++;
		}
		/* Rewind u and v for next line */
		if (!(i & 1)) {
			usrc -= width / 2;
			vsrc -= width / 2;
		}
	}
}

void v4lconvert_yuv420_to_rgb24(const unsigned char *src, unsigned char *dest,
		int width, int height, int yvu)
{
	int i, j;

	const unsigned char *ysrc = src;
	const unsigned char *usrc, *vsrc;

	if (yvu) {
		vsrc = src + width * height;
		usrc = vsrc + (width * height) / 4;
	} else {
		usrc = src + width * height;
		vsrc = usrc + (width * height) / 4;
	}

	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j += 2) {
#if 1 /* fast slightly less accurate multiplication free code */
			int u1 = (((*usrc - 128) << 7) +  (*usrc - 128)) >> 6;
			int rg = (((*usrc - 128) << 1) +  (*usrc - 128) +
					((*vsrc - 128) << 2) + ((*vsrc - 128) << 1)) >> 3;
			int v1 = (((*vsrc - 128) << 1) +  (*vsrc - 128)) >> 1;

			*dest++ = CLIP(*ysrc + v1);
			*dest++ = CLIP(*ysrc - rg);
			*dest++ = CLIP(*ysrc + u1);
			ysrc++;

			*dest++ = CLIP(*ysrc + v1);
			*dest++ = CLIP(*ysrc - rg);
			*dest++ = CLIP(*ysrc + u1);
#else
			*dest++ = YUV2R(*ysrc, *usrc, *vsrc);
			*dest++ = YUV2G(*ysrc, *usrc, *vsrc);
			*dest++ = YUV2B(*ysrc, *usrc, *vsrc);
			ysrc++;

			*dest++ = YUV2R(*ysrc, *usrc, *vsrc);
			*dest++ = YUV2G(*ysrc, *usrc, *vsrc);
			*dest++ = YUV2B(*ysrc, *usrc, *vsrc);
#endif
			ysrc++;
			usrc++;
			vsrc++;
		}
		/* Rewind u and v for next line */
		if (!(i&1)) {
			usrc -= width / 2;
			vsrc -= width / 2;
		}
	}
}

void v4lconvert_yuyv_to_bgr24(const unsigned char *src, unsigned char *dest,
		int width, int height, int stride)
{
	int j;

	while (--height >= 0) {
		for (j = 0; j + 1 < width; j += 2) {
			int u = src[1];
			int v = src[3];
			int u1 = (((u - 128) << 7) +  (u - 128)) >> 6;
			int rg = (((u - 128) << 1) +  (u - 128) +
					((v - 128) << 2) + ((v - 128) << 1)) >> 3;
			int v1 = (((v - 128) << 1) +  (v - 128)) >> 1;

			*dest++ = CLIP(src[0] + u1);
			*dest++ = CLIP(src[0] - rg);
			*dest++ = CLIP(src[0] + v1);

			*dest++ = CLIP(src[2] + u1);
			*dest++ = CLIP(src[2] - rg);
			*dest++ = CLIP(src[2] + v1);
			src += 4;
		}
		src += stride - width * 2;
	}
}

void v4lconvert_yuyv_to_rgb24(const unsigned char *src, unsigned char *dest,
		int width, int height, int stride)
{
	int j;

	while (--height >= 0) {
		for (j = 0; j + 1 < width; j += 2) {
			int u = src[1];
			int v = src[3];
			int u1 = (((u - 128) << 7) +  (u - 128)) >> 6;
			int rg = (((u - 128) << 1) +  (u - 128) +
					((v - 128) << 2) + ((v - 128) << 1)) >> 3;
			int v1 = (((v - 128) << 1) +  (v - 128)) >> 1;

			*dest++ = CLIP(src[0] + v1);
			*dest++ = CLIP(src[0] - rg);
			*dest++ = CLIP(src[0] + u1);

			*dest++ = CLIP(src[2] + v1);
			*dest++ = CLIP(src[2] - rg);
			*dest++ = CLIP(src[2] + u1);
			src += 4;
		}
		src += stride - (width * 2);
	}
}

void v4lconvert_yuyv_to_yuv420(const unsigned char *src, unsigned char *dest,
		int width, int height, int stride, int yvu)
{
	int i, j;
	const unsigned char *src1;
	unsigned char *udest, *vdest;

	/* copy the Y values */
	src1 = src;
	for (i = 0; i < height; i++) {
		for (j = 0; j + 1 < width; j += 2) {
			*dest++ = src1[0];
			*dest++ = src1[2];
			src1 += 4;
		}
		src1 += stride - width * 2;
	}

	/* copy the U and V values */
	src++;				/* point to V */
	src1 = src + stride;		/* next line */
	if (yvu) {
		vdest = dest;
		udest = dest + width * height / 4;
	} else {
		udest = dest;
		vdest = dest + width * height / 4;
	}
	for (i = 0; i < height; i += 2) {
		for (j = 0; j + 1 < width; j += 2) {
			*udest++ = ((int) src[0] + src1[0]) / 2;	/* U */
			*vdest++ = ((int) src[2] + src1[2]) / 2;	/* V */
			src += 4;
			src1 += 4;
		}
		src1 += stride - width * 2;
		src = src1;
		src1 += stride;
	}
}

void v4lconvert_nv16_to_yuyv(const unsigned char *src, unsigned char *dest,
		int width, int height)
{
	const unsigned char *y, *cbcr;
	int count = 0;

	y = src;
	cbcr = src + width*height;

	while (count++ < width*height) {
		*dest++ = *y++;
		*dest++ = *cbcr++;
	}
}

void v4lconvert_yvyu_to_bgr24(const unsigned char *src, unsigned char *dest,
		int width, int height, int stride)
{
	int j;

	while (--height >= 0) {
		for (j = 0; j + 1 < width; j += 2) {
			int u = src[3];
			int v = src[1];
			int u1 = (((u - 128) << 7) +  (u - 128)) >> 6;
			int rg = (((u - 128) << 1) +  (u - 128) +
					((v - 128) << 2) + ((v - 128) << 1)) >> 3;
			int v1 = (((v - 128) << 1) +  (v - 128)) >> 1;

			*dest++ = CLIP(src[0] + u1);
			*dest++ = CLIP(src[0] - rg);
			*dest++ = CLIP(src[0] + v1);

			*dest++ = CLIP(src[2] + u1);
			*dest++ = CLIP(src[2] - rg);
			*dest++ = CLIP(src[2] + v1);
			src += 4;
		}
		src += stride - (width * 2);
	}
}

void v4lconvert_yvyu_to_rgb24(const unsigned char *src, unsigned char *dest,
		int width, int height, int stride)
{
	int j;

	while (--height >= 0) {
		for (j = 0; j + 1 < width; j += 2) {
			int u = src[3];
			int v = src[1];
			int u1 = (((u - 128) << 7) +  (u - 128)) >> 6;
			int rg = (((u - 128) << 1) +  (u - 128) +
					((v - 128) << 2) + ((v - 128) << 1)) >> 3;
			int v1 = (((v - 128) << 1) +  (v - 128)) >> 1;

			*dest++ = CLIP(src[0] + v1);
			*dest++ = CLIP(src[0] - rg);
			*dest++ = CLIP(src[0] + u1);

			*dest++ = CLIP(src[2] + v1);
			*dest++ = CLIP(src[2] - rg);
			*dest++ = CLIP(src[2] + u1);
			src += 4;
		}
		src += stride - (width * 2);
	}
}

void v4lconvert_uyvy_to_bgr24(const unsigned char *src, unsigned char *dest,
		int width, int height, int stride)
{
	int j;

	while (--height >= 0) {
		for (j = 0; j + 1 < width; j += 2) {
			int u = src[0];
			int v = src[2];
			int u1 = (((u - 128) << 7) +  (u - 128)) >> 6;
			int rg = (((u - 128) << 1) +  (u - 128) +
					((v - 128) << 2) + ((v - 128) << 1)) >> 3;
			int v1 = (((v - 128) << 1) +  (v - 128)) >> 1;

			*dest++ = CLIP(src[1] + u1);
			*dest++ = CLIP(src[1] - rg);
			*dest++ = CLIP(src[1] + v1);

			*dest++ = CLIP(src[3] + u1);
			*dest++ = CLIP(src[3] - rg);
			*dest++ = CLIP(src[3] + v1);
			src += 4;
		}
		src += stride - width * 2;
	}
}

void v4lconvert_uyvy_to_rgb24(const unsigned char *src, unsigned char *dest,
		int width, int height, int stride)
{
	int j;

	while (--height >= 0) {
		for (j = 0; j + 1 < width; j += 2) {
			int u = src[0];
			int v = src[2];
			int u1 = (((u - 128) << 7) +  (u - 128)) >> 6;
			int rg = (((u - 128) << 1) +  (u - 128) +
					((v - 128) << 2) + ((v - 128) << 1)) >> 3;
			int v1 = (((v - 128) << 1) +  (v - 128)) >> 1;

			*dest++ = CLIP(src[1] + v1);
			*dest++ = CLIP(src[1] - rg);
			*dest++ = CLIP(src[1] + u1);

			*dest++ = CLIP(src[3] + v1);
			*dest++ = CLIP(src[3] - rg);
			*dest++ = CLIP(src[3] + u1);
			src += 4;
		}
		src += stride - width * 2;
	}
}

void v4lconvert_uyvy_to_yuv420(const unsigned char *src, unsigned char *dest,
		int width, int height, int stride, int yvu)
{
	int i, j;
	const unsigned char *src1;
	unsigned char *udest, *vdest;

	/* copy the Y values */
	src1 = src;
	for (i = 0; i < height; i++) {
		for (j = 0; j + 1 < width; j += 2) {
			*dest++ = src1[1];
			*dest++ = src1[3];
			src1 += 4;
		}
		src1 += stride - width * 2;
	}

	/* copy the U and V values */
	src1 = src + stride;		/* next line */
	if (yvu) {
		vdest = dest;
		udest = dest + width * height / 4;
	} else {
		udest = dest;
		vdest = dest + width * height / 4;
	}
	for (i = 0; i < height; i += 2) {
		for (j = 0; j + 1 < width; j += 2) {
			*udest++ = ((int) src[0] + src1[0]) / 2;	/* U */
			*vdest++ = ((int) src[2] + src1[2]) / 2;	/* V */
			src += 4;
			src1 += 4;
		}
		src1 += stride - width * 2;
		src = src1;
		src1 += stride;
	}
}

void v4lconvert_swap_rgb(const unsigned char *src, unsigned char *dst,
		int width, int height)
{
	int i;

	for (i = 0; i < (width * height); i++) {
		unsigned char tmp0, tmp1;
		tmp0 = *src++;
		tmp1 = *src++;
		*dst++ = *src++;
		*dst++ = tmp1;
		*dst++ = tmp0;
	}
}

void v4lconvert_swap_uv(const unsigned char *src, unsigned char *dest,
		const struct v4l2_format *src_fmt)
{
	int y;

	/* Copy Y */
	for (y = 0; y < src_fmt->fmt.pix.height; y++) {
		memcpy(dest, src, src_fmt->fmt.pix.width);
		dest += src_fmt->fmt.pix.width;
		src += src_fmt->fmt.pix.bytesperline;
	}

	/* Copy component 2 */
	src += src_fmt->fmt.pix.height * src_fmt->fmt.pix.bytesperline / 4;
	for (y = 0; y < src_fmt->fmt.pix.height / 2; y++) {
		memcpy(dest, src, src_fmt->fmt.pix.width / 2);
		dest += src_fmt->fmt.pix.width / 2;
		src += src_fmt->fmt.pix.bytesperline / 2;
	}

	/* Copy component 1 */
	src -= src_fmt->fmt.pix.height * src_fmt->fmt.pix.bytesperline / 2;
	for (y = 0; y < src_fmt->fmt.pix.height / 2; y++) {
		memcpy(dest, src, src_fmt->fmt.pix.width / 2);
		dest += src_fmt->fmt.pix.width / 2;
		src += src_fmt->fmt.pix.bytesperline / 2;
	}
}

void v4lconvert_rgb565_to_rgb24(const unsigned char *src, unsigned char *dest,
		int width, int height)
{
	int j;
	while (--height >= 0) {
		for (j = 0; j < width; j++) {
			unsigned short tmp = *(unsigned short *)src;

			/* Original format: rrrrrggg gggbbbbb */
			*dest++ = 0xf8 & (tmp >> 8);
			*dest++ = 0xfc & (tmp >> 3);
			*dest++ = 0xf8 & (tmp << 3);

			src += 2;
		}
	}
}

void v4lconvert_rgb565_to_bgr24(const unsigned char *src, unsigned char *dest,
		int width, int height)
{
	int j;
	while (--height >= 0) {
		for (j = 0; j < width; j++) {
			unsigned short tmp = *(unsigned short *)src;

			/* Original format: rrrrrggg gggbbbbb */
			*dest++ = 0xf8 & (tmp << 3);
			*dest++ = 0xfc & (tmp >> 3);
			*dest++ = 0xf8 & (tmp >> 8);

			src += 2;
		}
	}
}

void v4lconvert_rgb565_to_yuv420(const unsigned char *src, unsigned char *dest,
		const struct v4l2_format *src_fmt, int yvu)
{
	int x, y;
	unsigned short tmp;
	unsigned char *udest, *vdest;
	unsigned r[4], g[4], b[4];
	int avg_src[3];

	/* Y */
	for (y = 0; y < src_fmt->fmt.pix.height; y++) {
		for (x = 0; x < src_fmt->fmt.pix.width; x++) {
			tmp = *(unsigned short *)src;
			r[0] = 0xf8 & (tmp << 3);
			g[0] = 0xfc & (tmp >> 3);
			b[0] = 0xf8 & (tmp >> 8);
			RGB2Y(r[0], g[0], b[0], *dest++);
			src += 2;
		}
		src += src_fmt->fmt.pix.bytesperline - 2 * src_fmt->fmt.pix.width;
	}
	src -= src_fmt->fmt.pix.height * src_fmt->fmt.pix.bytesperline;

	/* U + V */
	if (yvu) {
		vdest = dest;
		udest = dest + src_fmt->fmt.pix.width * src_fmt->fmt.pix.height / 4;
	} else {
		udest = dest;
		vdest = dest + src_fmt->fmt.pix.width * src_fmt->fmt.pix.height / 4;
	}

	for (y = 0; y < src_fmt->fmt.pix.height / 2; y++) {
		for (x = 0; x < src_fmt->fmt.pix.width / 2; x++) {
			tmp = *(unsigned short *)src;
			r[0] = 0xf8 & (tmp << 3);
			g[0] = 0xfc & (tmp >> 3);
			b[0] = 0xf8 & (tmp >> 8);

			tmp = *(((unsigned short *)src) + 1);
			r[1] = 0xf8 & (tmp << 3);
			g[1] = 0xfc & (tmp >> 3);
			b[1] = 0xf8 & (tmp >> 8);

			tmp = *(((unsigned short *)src) + src_fmt->fmt.pix.bytesperline);
			r[2] = 0xf8 & (tmp << 3);
			g[2] = 0xfc & (tmp >> 3);
			b[2] = 0xf8 & (tmp >> 8);

			tmp = *(((unsigned short *)src) + src_fmt->fmt.pix.bytesperline + 1);
			r[3] = 0xf8 & (tmp << 3);
			g[3] = 0xfc & (tmp >> 3);
			b[3] = 0xf8 & (tmp >> 8);

			avg_src[0] = (r[0] + r[1] + r[2] + r[3]) / 4;
			avg_src[1] = (g[0] + g[1] + g[2] + g[3]) / 4;
			avg_src[2] = (b[0] + b[1] + b[2] + b[3]) / 4;
			RGB2UV(avg_src[0], avg_src[1], avg_src[2], *udest++, *vdest++);
			src += 4;
		}
		src += 2 * src_fmt->fmt.pix.bytesperline - 2 * src_fmt->fmt.pix.width;
	}
}

void v4lconvert_y16_to_rgb24(const unsigned char *src, unsigned char *dest,
		int width, int height, int little_endian)
{
	int j;

	if (little_endian)
		src++;

	while (--height >= 0) {
		for (j = 0; j < width; j++) {
			*dest++ = *src;
			*dest++ = *src;
			*dest++ = *src;
			src+=2;
		}
	}
}

void v4lconvert_y16_to_yuv420(const unsigned char *src, unsigned char *dest,
		const struct v4l2_format *src_fmt, int little_endian)
{
	int x, y;

	if (little_endian)
		src++;

	/* Y */
	for (y = 0; y < src_fmt->fmt.pix.height; y++)
		for (x = 0; x < src_fmt->fmt.pix.width; x++){
			*dest++ = *src;
			src+=2;
		}

	/* Clear U/V */
	memset(dest, 0x80, src_fmt->fmt.pix.width * src_fmt->fmt.pix.height / 2);
}

void v4lconvert_grey_to_rgb24(const unsigned char *src, unsigned char *dest,
		int width, int height)
{
	int j;
	while (--height >= 0) {
		for (j = 0; j < width; j++) {
			*dest++ = *src;
			*dest++ = *src;
			*dest++ = *src;
			src++;
		}
	}
}

void v4lconvert_grey_to_yuv420(const unsigned char *src, unsigned char *dest,
		const struct v4l2_format *src_fmt)
{
	int x, y;

	/* Y */
	for (y = 0; y < src_fmt->fmt.pix.height; y++)
		for (x = 0; x < src_fmt->fmt.pix.width; x++)
			*dest++ = *src++;

	/* Clear U/V */
	memset(dest, 0x80, src_fmt->fmt.pix.width * src_fmt->fmt.pix.height / 2);
}

/* Unpack buffer of (vw bit) data into padded 16bit buffer. */
static inline void convert_packed_to_16bit(const uint8_t *raw, uint16_t *unpacked,
					   int vw, int unpacked_len)
{
	int mask = (1 << vw) - 1;
	uint32_t buffer = 0;
	int bitsIn = 0;
	while (unpacked_len--) {
		while (bitsIn < vw) {
			buffer = (buffer << 8) | *(raw++);
			bitsIn += 8;
		}
		bitsIn -= vw;
		*(unpacked++) = (buffer >> bitsIn) & mask;
	}
}

int v4lconvert_y10b_to_rgb24(struct v4lconvert_data *data,
	const unsigned char *src, unsigned char *dest, int width, int height)
{
	unsigned char *unpacked_buffer;

	unpacked_buffer = v4lconvert_alloc_buffer(width * height * 2,
					&data->convert_pixfmt_buf,
					&data->convert_pixfmt_buf_size);
	if (!unpacked_buffer)
		return v4lconvert_oom_error(data);

	convert_packed_to_16bit((uint8_t *)src, (uint16_t *)unpacked_buffer,
				10, width * height);

	int j;
	unsigned short *tmp = (unsigned short *)unpacked_buffer;
	while (--height >= 0) {
		for (j = 0; j < width; j++) {

			/* Only 10 useful bits, so we discard the LSBs */
			*dest++ = (*tmp & 0x3ff) >> 2;
			*dest++ = (*tmp & 0x3ff) >> 2;
			*dest++ = (*tmp & 0x3ff) >> 2;

			/* +1 means two bytes as we are dealing with (unsigned short) */
			tmp += 1;
		}
	}
	return 0;
}

int v4lconvert_y10b_to_yuv420(struct v4lconvert_data *data,
	const unsigned char *src, unsigned char *dest, int width, int height)
{
	unsigned char *unpacked_buffer;

	unpacked_buffer = v4lconvert_alloc_buffer(width * height * 2,
					&data->convert_pixfmt_buf,
					&data->convert_pixfmt_buf_size);
	if (!unpacked_buffer)
		return v4lconvert_oom_error(data);

	convert_packed_to_16bit((uint8_t *)src, (uint16_t *)unpacked_buffer,
				10, width * height);

	int x, y;
	unsigned short *tmp = (unsigned short *)unpacked_buffer;

	/* Y */
	for (y = 0; y < height; y++)
		for (x = 0; x < width; x++) {

			/* Only 10 useful bits, so we discard the LSBs */
			*dest++ = (*tmp & 0x3ff) >> 2;

			/* +1 means two bytes as we are dealing with (unsigned short) */
			tmp += 1;
		}

	/* Clear U/V */
	memset(dest, 0x80, width * height / 2);

	return 0;
}

void v4lconvert_rgb32_to_rgb24(const unsigned char *src, unsigned char *dest,
		int width, int height,int bgr)
{
	int j;
	while (--height >= 0) {
		for (j = 0; j < width; j++) {
			if (bgr){
				*dest++ = src[2];
				*dest++ = src[1];
				*dest++ = src[0];
				src+=4;
			}
			else{
				*dest++ = *src++;
				*dest++ = *src++;
				*dest++ = *src++;
				src+=1;
			}
		}
	}
}

static void hsvtorgb(const unsigned char *hsv, unsigned char *rgb,
		     unsigned char hsv_enc)
{
	/* From http://stackoverflow.com/questions/3018313/ */
	uint8_t region;
	uint8_t remain;
	uint8_t p, q, t;

	if (!hsv[1]) {
		rgb[0] = rgb[1] = rgb[2] = hsv[2];
		return;
	}

	if (hsv_enc == V4L2_HSV_ENC_256) {
		region = hsv[0] / 43;
		remain = (hsv[0] - (region * 43)) * 6;

	} else {
		int aux;

		region = hsv[0] / (180/6);

		/* Remain must be scaled to 0..255 */
		aux = (hsv[0] % (180/6)) * 6 * 256;
		aux /= 180;
		remain = aux;
	}

	p = (hsv[2] * (255 - hsv[1])) >> 8;
	q = (hsv[2] * (255 - ((hsv[1] * remain) >> 8))) >> 8;
	t = (hsv[2] * (255 - ((hsv[1] * (255 - remain)) >> 8))) >> 8;

	switch (region)	{
	case 0:
		rgb[0] = hsv[2]; rgb[1] = t; rgb[2] = p;
		break;
	case 1:
		rgb[0] = q; rgb[1] = hsv[2]; rgb[2] = p;
		break;
	case 2:
		rgb[0] = p; rgb[1] = hsv[2]; rgb[2] = t;
		break;
	case 3:
		rgb[0] = p; rgb[1] = q; rgb[2] = hsv[2];
		break;
	case 4:
		rgb[0] = t; rgb[1] = p; rgb[2] = hsv[2];
		break;
	default:
		rgb[0] = hsv[2]; rgb[1] = p; rgb[2] = q;
		break;
	}

}

void v4lconvert_hsv_to_rgb24(const unsigned char *src, unsigned char *dest,
		int width, int height, int bgr, int Xin, unsigned char hsv_enc){
	int j, k;
	int bppIN = Xin / 8;
	unsigned char rgb[3];

	src += bppIN - 3;

	while (--height >= 0)
		for (j = 0; j < width; j++) {
			hsvtorgb(src, rgb, hsv_enc);
			for (k = 0; k < 3; k++)
				if (bgr && k < 3)
					*dest++ = rgb[2-k];
				else
					*dest++ = rgb[k];
			src += bppIN;
		}
}
