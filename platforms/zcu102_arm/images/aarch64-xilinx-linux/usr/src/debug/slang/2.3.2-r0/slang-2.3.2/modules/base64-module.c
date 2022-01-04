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
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <slang.h>

SLANG_MODULE(base64);

static int Base64_Type_Id = 0;
typedef struct _Base64_Type Base64_Type;
struct _Base64_Type
{
   int type;
#define B64_TYPE_ENCODER	1
#define B64_TYPE_DECODER	2
   SLang_Name_Type *callback;
   SLang_Any_Type *callback_data;
   unsigned char *buffer;		       /* malloced buffer_size + overhead */
#define B64_ENCODE_BUFFER_SIZE 76    /* multiple of 4 */
#define B64_DECODE_BUFFER_SIZE 512
   unsigned int buffer_size;
   unsigned int num_buffered;
   unsigned char smallbuf[4];
   unsigned int smallbuf_len;
#define B64_CLOSED	0x1
#define B64_INVALID	0x2
   int flags;
};

static int check_b64_type (Base64_Type *b64, int type, int err)
{
   if (type && (b64->type != type))
     {
	if (err)
	  SLang_verror (SL_InvalidParm_Error, "Expected a base64 %s type",
			(type == B64_TYPE_ENCODER) ? "encoder" : "decoder");
	return -1;
     }

   if (b64->flags & (B64_INVALID|B64_CLOSED))
     {
	if (err)
	  SLang_verror (SL_InvalidParm_Error, "Base64 encoder is invalid or closed");
	return -1;
     }
   return 0;
}

static void b64_partial_free (Base64_Type *b64)
{
   if (b64->callback_data != NULL) SLang_free_anytype (b64->callback_data);
   b64->callback_data = NULL;
   if (b64->callback != NULL) SLang_free_function (b64->callback);
   b64->callback = NULL;
   if (b64->buffer != NULL) SLfree ((char *)b64->buffer);
   b64->buffer = NULL;
   b64->flags |= B64_INVALID;
}

static int create_b64_buffer (Base64_Type *b64)
{
   b64->num_buffered = 0;
   if (NULL == (b64->buffer = (unsigned char *)SLmalloc (b64->buffer_size + 1)))
     return -1;
   return 0;
}

static int execute_callback (Base64_Type *b64)
{
   SLang_BString_Type *b;

   if (NULL == (b = SLbstring_create_malloced (b64->buffer, b64->num_buffered, 0)))
     return -1;

   if (-1 == create_b64_buffer (b64))
     {
	SLbstring_free (b);
	return -1;
     }

   if ((-1 == SLang_start_arg_list ())
       || (-1 == SLang_push_anytype (b64->callback_data))
       || (-1 == SLang_push_bstring (b))
       || (-1 == SLang_end_arg_list ())
       || (-1 == SLexecute_function (b64->callback)))
     {
	b64->flags |= B64_INVALID;
	SLbstring_free (b);
	return -1;
     }
   SLbstring_free (b);
   return 0;
}

static void free_b64_type (Base64_Type *b64)
{
   if (b64 == NULL)
     return;
   b64_partial_free (b64);
   SLfree ((char *)b64);
}

/* rfc1521:
 *
 * The encoding process represents 24-bit groups of input bits as output
 *  strings of 4 encoded characters. Proceeding from left to right, a
 *  24-bit input group is formed by concatenating 3 8-bit input groups.
 *  These 24 bits are then treated as 4 concatenated 6-bit groups, each
 *  of which is translated into a single digit in the base64 alphabet.
 *  When encoding a bit stream via the base64 encoding, the bit stream
 *  must be presumed to be ordered with the most-significant-bit first.
 */

static char Base64_Bit_Mapping[64] =
{
   'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
   'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
   'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
   'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/',
};

/*  The output stream (encoded bytes) must be represented in lines of no
 *  more than 76 characters each.  All line breaks or other characters
 *  not found in Table 1 must be ignored by decoding software.  In base64
 *  data, characters other than those in Table 1, line breaks, and other
 *  white space probably indicate a transmission error, about which a
 *  warning message or even a message rejection might be appropriate
 *  under some circumstances.
 */

static int b64_encode_triplet (Base64_Type *b64, unsigned char *str)
{
   unsigned char *encode_buf;
   unsigned char ch0, ch1, ch2;

   encode_buf = b64->buffer + b64->num_buffered;

   ch0 = str[0];
   ch1 = str[1];
   ch2 = str[2];
   encode_buf[0] = Base64_Bit_Mapping[ch0>>2];
   encode_buf[1] = Base64_Bit_Mapping[((ch0&0x3)<<4) | (ch1>>4)];
   encode_buf[2] = Base64_Bit_Mapping[((ch1&0xF)<<2) | (ch2>>6)];
   encode_buf[3] = Base64_Bit_Mapping[ch2&0x3F];
   b64->num_buffered += 4;
   if (b64->num_buffered < b64->buffer_size)
     return 0;
   encode_buf[4] = 0;
   return execute_callback (b64);
}

static int b64_encode_accumulate (Base64_Type *b64, unsigned char *line, unsigned int len)
{
   unsigned char *linemax;
   unsigned int i;

   linemax = line + len;

   i = b64->smallbuf_len;
   if (i && (i < 3))
     {
	if (line < linemax)
	  b64->smallbuf[i++] = *line++;
	if ((i < 3) && (line < linemax))
	  b64->smallbuf[i++] = *line++;

	if (i < 3)
	  {
	     b64->smallbuf_len = i;
	     return 0;
	  }
	if (-1 == b64_encode_triplet (b64, b64->smallbuf))
	  return -1;
	b64->smallbuf_len = 0;
     }

   while (line + 3 <= linemax)
     {
	if (-1 == b64_encode_triplet (b64, line))
	  return -1;
	line += 3;
     }

   i = 0;
   while (line < linemax)
     b64->smallbuf[i++] = *line++;
   b64->smallbuf_len = i;
   return 0;
}

static void b64_encoder_accumulate_intrin (Base64_Type *b64, SLang_BString_Type *bstr)
{
   unsigned char *data;
   SLstrlen_Type len;

   if (-1 == check_b64_type (b64, B64_TYPE_ENCODER, 1))
     return;

   if (NULL == (data = SLbstring_get_pointer (bstr, &len)))
     return;

   (void) b64_encode_accumulate (b64, data, len);
}

static void b64_encoder_close_intrin (Base64_Type *b64)
{
   if (-1 == check_b64_type (b64, B64_TYPE_ENCODER, 0))
     goto close_encoder;

   /* Handle the padding */
   if (b64->smallbuf_len)
     {
	unsigned char *encode_buf = b64->buffer + b64->num_buffered;
	unsigned char ch0;

	ch0 = b64->smallbuf[0];
	encode_buf[0] = Base64_Bit_Mapping[ch0>>2];
	if (b64->smallbuf_len > 1)
	  {
	     unsigned char ch1 = b64->smallbuf[1];
	     encode_buf[1] = Base64_Bit_Mapping[((ch0&0x3)<<4) | (ch1>>4)];
	     encode_buf[2] = Base64_Bit_Mapping[((ch1&0xF)<<2)];
	  }
	else
	  {
	     encode_buf[1] = Base64_Bit_Mapping[((ch0&0x3)<<4)];
	     encode_buf[2] = '=';
	  }
	encode_buf[3] = '=';
	b64->num_buffered += 4;
	b64->smallbuf_len = 0;
	if (b64->num_buffered >= b64->buffer_size)
	  (void) execute_callback (b64);
     }

   if (b64->num_buffered)
     (void) execute_callback (b64);

close_encoder:
   b64_partial_free (b64);
   b64->flags |= B64_CLOSED;
}

static unsigned char Base64_Decode_Map [256] =
{
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255, 62,255,255,255, 63,
   52, 53, 54, 55, 56, 57, 58, 59, 60, 61,255,255,255,255,255,255,
   255,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
   15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,255,255,255,255,255,
   255, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
   41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255
};

static int b64_decode_quartet (Base64_Type *b64, unsigned char *str4)
{
   unsigned char b0, b1, b2, b3;
   unsigned char bytes_buf[3], *bytes;
   unsigned int bad;
   unsigned int n;

   if (0xFF == (b0 = Base64_Decode_Map[str4[0]]))
     {
	bad = 0;
	goto return_error;
     }
   if (0xFF == (b1 = Base64_Decode_Map[str4[1]]))
     {
	bad = 1;
	goto return_error;
     }

   n = 3;

   b2 = Base64_Decode_Map[str4[2]];
   b3 = Base64_Decode_Map[str4[3]];
   if ((b2 == 0xFF) || (b3 == 0xFF))
     {
	if (b2 == 0xFF)
	  {
	     if ('=' != str4[2])
	       {
		  bad = 2;
		  goto return_error;
	       }
	     n = 1;
	  }
	else n = 2;

	if (str4[3] != '=')
	  {
	     SLang_verror (SL_Data_Error, "Illegally padded base64 sequence seen");
	     return -1;
	  }
     }

   if (b64->num_buffered + n < b64->buffer_size)
     bytes = b64->buffer + b64->num_buffered;
   else
     bytes = bytes_buf;

   bytes[0] = (b0 << 2) | (b1>>4);
   if (n > 1)
     {
	bytes[1] = (b1 << 4) | (b2 >> 2);
	if (n > 2)
	  bytes[2] = (b2 << 6) | b3;
     }

   if (bytes != bytes_buf)
     {
	b64->num_buffered += n;
	return 0;
     }

   while (n && (b64->num_buffered < b64->buffer_size))
     {
	b64->buffer[b64->num_buffered++] = *bytes++;
	n--;
     }
   if (b64->num_buffered == b64->buffer_size)
     {
	if (-1 == execute_callback (b64))
	  return -1;
     }
   while (n)
     {
	b64->buffer[b64->num_buffered++] = *bytes++;
	n--;
     }
   return 0;

return_error:
   SLang_verror (SL_Data_Error, "Invalid character (0x%X) found in base64-encoded stream", str4[bad]);
   return -1;
}

static void b64_decoder_accumulate_intrin (Base64_Type *b64, SLFUTURE_CONST char *str)
{
   unsigned int i;
   unsigned char ch;
   unsigned char *buf4;
   if (-1 == check_b64_type (b64, B64_TYPE_DECODER, 1))
     return;

#define NEXT_CHAR \
   while (isspace ((unsigned char)*str)) str++; ch = *str++

   NEXT_CHAR;
   if (ch == 0)
     return;

   i = b64->smallbuf_len;
   buf4 = b64->smallbuf;
   if (i && (i < 4))
     {
	buf4[i++] = ch;
	NEXT_CHAR;

	if ((i < 4) && (ch != 0))
	  {
	     buf4[i++] = ch;
	     NEXT_CHAR;
	  }
	if ((i < 4) && (ch != 0))
	  {
	     buf4[i++] = ch;
	     NEXT_CHAR;
	  }
	if (i < 4)
	  {
	     b64->smallbuf_len = i;
	     return;
	  }
	if (-1 == b64_decode_quartet (b64, buf4))
	  return;

	b64->smallbuf_len = 0;
     }

   while (1)
     {
	if (ch == 0)
	  {
	     i = 0;
	     break;
	  }
	buf4[0] = ch;
	NEXT_CHAR;
	if (ch == 0)
	  {
	     i = 1;
	     break;
	  }
	buf4[1] = ch;
	NEXT_CHAR;
	if (ch == 0)
	  {
	     i = 2;
	     break;
	  }
	buf4[2] = ch;
	NEXT_CHAR;
	if (ch == 0)
	  {
	     i = 3;
	     break;
	  }
	buf4[3] = ch;
	if (-1 == b64_decode_quartet (b64, buf4))
	  return;
	NEXT_CHAR;
     }

   b64->smallbuf_len = i;
}

static void b64_decoder_close_intrin (Base64_Type *b64)
{
   SLFUTURE_CONST char *pad = "====";

   if (-1 == check_b64_type (b64, B64_TYPE_DECODER, 0))
     goto close_decoder;

   /* silently add pad characters if necessary */
   if (b64->smallbuf_len)
     (void) b64_decoder_accumulate_intrin (b64, pad + b64->smallbuf_len);

   if (b64->num_buffered)
     (void) execute_callback (b64);

close_decoder:
   b64_partial_free (b64);
   b64->flags |= B64_CLOSED;
}

static void new_b64_type (int type)
{
   Base64_Type *b64;
   SLang_MMT_Type *mmt;

   if (NULL == (b64 = (Base64_Type *)SLmalloc(sizeof(Base64_Type))))
     return;
   memset ((char *)b64, 0, sizeof(Base64_Type));

   b64->type = type;
   if (type == B64_TYPE_ENCODER)
     b64->buffer_size = B64_ENCODE_BUFFER_SIZE;
   else
     b64->buffer_size = B64_DECODE_BUFFER_SIZE;

   if (-1 == create_b64_buffer(b64))
     {
	SLfree ((char *)b64);
	return;
     }

   if ((-1 == SLang_pop_anytype (&b64->callback_data))
	|| (NULL == (b64->callback = SLang_pop_function ()))
	|| (NULL == (mmt = SLang_create_mmt (Base64_Type_Id, (VOID_STAR)b64))))
     {
	free_b64_type (b64);
	return;
     }

   if (-1 == SLang_push_mmt (mmt))
     SLang_free_mmt (mmt);
}

static void new_b64_encoder_intrin (void)
{
   new_b64_type (B64_TYPE_ENCODER);
}

static void new_b64_decoder_intrin (void)
{
   new_b64_type (B64_TYPE_DECODER);
}

#define DUMMY_B64_TYPE ((SLtype)-1)
static SLang_Intrin_Fun_Type Module_Intrinsics [] =
{
   MAKE_INTRINSIC_0("_base64_encoder_new", new_b64_encoder_intrin, SLANG_VOID_TYPE),
   MAKE_INTRINSIC_2("_base64_encoder_accumulate", b64_encoder_accumulate_intrin, SLANG_VOID_TYPE, DUMMY_B64_TYPE, SLANG_BSTRING_TYPE),
   MAKE_INTRINSIC_1("_base64_encoder_close", b64_encoder_close_intrin, SLANG_VOID_TYPE, DUMMY_B64_TYPE),
   MAKE_INTRINSIC_0("_base64_decoder_new", new_b64_decoder_intrin, SLANG_VOID_TYPE),
   MAKE_INTRINSIC_2("_base64_decoder_accumulate", b64_decoder_accumulate_intrin, SLANG_VOID_TYPE, DUMMY_B64_TYPE, SLANG_STRING_TYPE),
   MAKE_INTRINSIC_1("_base64_decoder_close", b64_decoder_close_intrin, SLANG_VOID_TYPE, DUMMY_B64_TYPE),
   SLANG_END_INTRIN_FUN_TABLE
};

static void destroy_b64 (SLtype type, VOID_STAR f)
{
   (void) type;
   free_b64_type ((Base64_Type *)f);
}

static int register_b64_type (void)
{
   SLang_Class_Type *cl;

   if (Base64_Type_Id != 0)
     return 0;

   if (NULL == (cl = SLclass_allocate_class ("Base64_Type")))
     return -1;

   if (-1 == SLclass_set_destroy_function (cl, destroy_b64))
     return -1;

   /* By registering as SLANG_VOID_TYPE, slang will dynamically allocate a
    * type.
    */
   if (-1 == SLclass_register_class (cl, SLANG_VOID_TYPE, sizeof (Base64_Type), SLANG_CLASS_TYPE_MMT))
     return -1;

   Base64_Type_Id = SLclass_get_class_id (cl);
   if (-1 == SLclass_patch_intrin_fun_table1 (Module_Intrinsics, DUMMY_B64_TYPE, Base64_Type_Id))
     return -1;

   return 0;
}

int init_base64_module_ns (char *ns_name)
{
   SLang_NameSpace_Type *ns = SLns_create_namespace (ns_name);
   if (ns == NULL)
     return -1;

   if (-1 == register_b64_type ())
     return -1;

   if (-1 == SLns_add_intrin_fun_table (ns, Module_Intrinsics, NULL))
     return -1;

   return 0;
}

/* This function is optional */
void deinit_base64_module (void)
{
}
