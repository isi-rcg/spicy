/*
Copyright (C) 2004-2017,2018 John E. Davis

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

#include "slinclud.h"

#include "slang.h"
#include "_slang.h"

struct _pSLang_BString_Type
{
   unsigned int num_refs;
   unsigned int len;
#define BSTRING_EXTRA_BYTES(len)	(32+(len)/10)
   unsigned int malloced_len;
   int ptr_type;
#define IS_BSTRING		0      /* uses bytes field */
#define IS_SLSTRING		1      /* uses ptr */
#define IS_MALLOCED		2      /* uses ptr */
#define IS_NOT_TO_BE_FREED	3      /* uses ptr */
   union
     {
	unsigned char bytes[1];
	unsigned char *ptr;
     }
   v;
};

#define BS_GET_POINTER(b) \
   (((b)->ptr_type != IS_BSTRING) ? (b)->v.ptr : (b)->v.bytes)

static SLang_BString_Type *create_bstring_of_type (char *bytes, size_t len, int type)
{
   SLang_BString_Type *b;
   size_t size;
   size_t malloced_len = len;

   size = sizeof(SLang_BString_Type);
   if (type == IS_BSTRING)
     {
	unsigned int dlen = BSTRING_EXTRA_BYTES(len);
	malloced_len = len + dlen;
	if ((malloced_len < len)
	    || (size + malloced_len < size))
	  {
	     SLang_verror (SL_Malloc_Error, "Unable to create a binary string of the desired size");
	     return NULL;
	  }
	size += malloced_len;
     }

   if (NULL == (b = (SLang_BString_Type *)SLmalloc (size)))
     return NULL;

   b->len = len;
   b->malloced_len = malloced_len;
   b->num_refs = 1;
   b->ptr_type = type;

   switch (type)
     {
      default:
      case IS_BSTRING:
	if (bytes != NULL) memcpy ((char *) b->v.bytes, bytes, len);
	/* Now \0 terminate it because we want to also use it as a C string
	 * whenever possible.  Note that sizeof(SLang_BString_Type) includes
	 * space for 1 character and we allocated len extra bytes.  Thus, it is
	 * ok to add a \0 to the end.
	 */
	b->v.bytes[len] = 0;
	break;

      case IS_SLSTRING:
	if (NULL == (b->v.ptr = (unsigned char *)SLang_create_nslstring (bytes, len)))
	  {
	     SLfree ((char *) b);
	     return NULL;
	  }
	break;

      case IS_MALLOCED:
      case IS_NOT_TO_BE_FREED:
	b->v.ptr = (unsigned char *)bytes;
	bytes [len] = 0;	       /* NULL terminate */
	break;
     }

   return b;
}

SLang_BString_Type *
SLbstring_create (unsigned char *bytes, SLstrlen_Type len)
{
   return create_bstring_of_type ((char *)bytes, len, IS_BSTRING);
}

/* Note that ptr must be len + 1 bytes long for \0 termination */
SLang_BString_Type *
SLbstring_create_malloced (unsigned char *ptr, SLstrlen_Type len, int free_on_error)
{
   SLang_BString_Type *b;

   if (ptr == NULL)
     return NULL;

   if (NULL == (b = create_bstring_of_type ((char *)ptr, len, IS_MALLOCED)))
     {
	if (free_on_error)
	  SLfree ((char *) ptr);
     }
   return b;
}

SLang_BString_Type *SLbstring_create_slstring (SLFUTURE_CONST char *s)
{
   if (s == NULL)
     return NULL;

   return create_bstring_of_type ((char *) s, strlen (s), IS_SLSTRING);
}

SLang_BString_Type *SLbstring_dup (SLang_BString_Type *b)
{
   if (b != NULL)
     b->num_refs += 1;

   return b;
}

unsigned char *SLbstring_get_pointer (SLang_BString_Type *b, SLstrlen_Type *len)
{
   if (b == NULL)
     {
	*len = 0;
	return NULL;
     }
   *len = b->len;
   return BS_GET_POINTER(b);
}

void SLbstring_free (SLang_BString_Type *b)
{
   if (b == NULL)
     return;

   if (b->num_refs > 1)
     {
	b->num_refs -= 1;
	return;
     }

   switch (b->ptr_type)
     {
      case IS_BSTRING:
      case IS_NOT_TO_BE_FREED:
      default:
	break;

      case IS_SLSTRING:
	_pSLang_free_slstring ((char *)b->v.ptr);
	break;

      case IS_MALLOCED:
	SLfree ((char *)b->v.ptr);
	break;
     }

   SLfree ((char *) b);
}

int SLang_pop_bstring (SLang_BString_Type **b)
{
   return SLclass_pop_ptr_obj (SLANG_BSTRING_TYPE, (VOID_STAR *)b);
}

int SLang_push_bstring (SLang_BString_Type *b)
{
   if (b == NULL)
     return SLang_push_null ();

   b->num_refs += 1;

   if (0 == SLclass_push_ptr_obj (SLANG_BSTRING_TYPE, (VOID_STAR)b))
     return 0;

   b->num_refs -= 1;
   return -1;
}

static int
bstring_bstring_bin_op_result (int op, SLtype a, SLtype b,
			       SLtype *c)
{
   (void) a;
   (void) b;
   switch (op)
     {
      default:
	return 0;

      case SLANG_PLUS:
	*c = SLANG_BSTRING_TYPE;
	break;

      case SLANG_GT:
      case SLANG_GE:
      case SLANG_LT:
      case SLANG_LE:
      case SLANG_EQ:
      case SLANG_NE:
	*c = SLANG_CHAR_TYPE;
	break;
     }
   return 1;
}

static int compare_bstrings (SLang_BString_Type *a, SLang_BString_Type *b)
{
   unsigned int len;
   int ret;

   len = a->len;
   if (b->len < len) len = b->len;

   ret = memcmp ((char *)BS_GET_POINTER(a), (char *)BS_GET_POINTER(b), len);
   if (ret != 0)
     return ret;

   if (a->len > b->len)
     return 1;
   if (a->len == b->len)
     return 0;

   return -1;
}

static SLang_BString_Type *
concat_bstrings (SLang_BString_Type *a, SLang_BString_Type *b)
{
   SLuindex_Type len;
   SLang_BString_Type *c;
   char *bytes;

   len = a->len + b->len;

#if SLANG_USE_TMP_OPTIMIZATION
   if ((a->num_refs == 1)	       /* owned by stack */
       && (a->ptr_type == IS_BSTRING)
       && (len < a->malloced_len))
     {
	bytes = (char *)a->v.bytes;
	memcpy (bytes + a->len, (char *)BS_GET_POINTER(b), b->len);
	bytes[len] = 0;		       /* ok to \0 terminate --- see above */
	a->len = len;
	a->num_refs++;
	return a;
     }
#endif

   if (NULL == (c = SLbstring_create (NULL, len)))
     return NULL;

   bytes = (char *)BS_GET_POINTER(c);

   memcpy (bytes, (char *)BS_GET_POINTER(a), a->len);
   memcpy (bytes + a->len, (char *)BS_GET_POINTER(b), b->len);
   bytes[len] = 0;

   return c;
}

static void free_n_bstrings (SLang_BString_Type **a, SLuindex_Type n)
{
   SLuindex_Type i;

   if (a == NULL) return;

   for (i = 0; i < n; i++)
     {
	SLbstring_free (a[i]);
	a[i] = NULL;
     }
}

static int
bstring_bstring_bin_op (int op,
			SLtype a_type, VOID_STAR ap, SLuindex_Type na,
			SLtype b_type, VOID_STAR bp, SLuindex_Type nb,
			VOID_STAR cp)
{
   char *ic;
   SLang_BString_Type **a, **b, **c;
   SLuindex_Type n, n_max;
   unsigned int da, db;

   (void) a_type;
   (void) b_type;

   if (na == 1) da = 0; else da = 1;
   if (nb == 1) db = 0; else db = 1;

   if (na > nb) n_max = na; else n_max = nb;

   a = (SLang_BString_Type **) ap;
   b = (SLang_BString_Type **) bp;
   for (n = 0; n < n_max; n++)
     {
	if ((*a == NULL) || (*b == NULL))
	  {
	     _pSLang_verror (SL_VARIABLE_UNINITIALIZED,
			   "Binary string element[%lu] not initialized for binary operation", (unsigned long)n);
	     return -1;
	  }
	a += da; b += db;
     }

   a = (SLang_BString_Type **) ap;
   b = (SLang_BString_Type **) bp;
   ic = (char *) cp;
   c = NULL;

   switch (op)
     {
       case SLANG_PLUS:
	/* Concat */
	c = (SLang_BString_Type **) cp;
	for (n = 0; n < n_max; n++)
	  {
	     if (NULL == (c[n] = concat_bstrings (*a, *b)))
	       goto return_error;

	     a += da; b += db;
	  }
	break;

      case SLANG_NE:
	for (n = 0; n < n_max; n++)
	  {
	     ic [n] = (0 != compare_bstrings (*a, *b));
	     a += da;
	     b += db;
	  }
	break;
      case SLANG_GT:
	for (n = 0; n < n_max; n++)
	  {
	     ic [n] = (compare_bstrings (*a, *b) > 0);
	     a += da;
	     b += db;
	  }
	break;
      case SLANG_GE:
	for (n = 0; n < n_max; n++)
	  {
	     ic [n] = (compare_bstrings (*a, *b) >= 0);
	     a += da;
	     b += db;
	  }
	break;
      case SLANG_LT:
	for (n = 0; n < n_max; n++)
	  {
	     ic [n] = (compare_bstrings (*a, *b) < 0);
	     a += da;
	     b += db;
	  }
	break;
      case SLANG_LE:
	for (n = 0; n < n_max; n++)
	  {
	     ic [n] = (compare_bstrings (*a, *b) <= 0);
	     a += da;
	     b += db;
	  }
	break;
      case SLANG_EQ:
	for (n = 0; n < n_max; n++)
	  {
	     ic [n] = (compare_bstrings (*a, *b) == 0);
	     a += da;
	     b += db;
	  }
	break;
     }
   return 1;

   return_error:
   if (c != NULL)
     {
	free_n_bstrings (c, n);
	while (n < n_max)
	  {
	     c[n] = NULL;
	     n++;
	  }
     }
   return -1;
}

/* If preserve_ptr, then use a[i] as the bstring data.  See how this function
 * is called by the binary op routines for why.
 */
static SLang_BString_Type **
make_n_bstrings (SLang_BString_Type **b, char **a, SLuindex_Type n, int ptr_type)
{
   SLuindex_Type i;
   int malloc_flag;

   malloc_flag = 0;
   if (b == NULL)
     {
	b = (SLang_BString_Type **) _SLcalloc (n, sizeof (SLang_BString_Type *));
	if (b == NULL)
	  return NULL;
	malloc_flag = 1;
     }

   for (i = 0; i < n; i++)
     {
	char *s = a[i];

	if (s == NULL)
	  {
	     b[i] = NULL;
	     continue;
	  }

	if (NULL == (b[i] = create_bstring_of_type (s, strlen(s), ptr_type)))
	  {
	     free_n_bstrings (b, i);
	     if (malloc_flag) SLfree ((char *) b);
	     return NULL;
	  }
     }

   return b;
}

static int
bstring_string_bin_op (int op,
		       SLtype a_type, VOID_STAR ap, SLuindex_Type na,
		       SLtype b_type, VOID_STAR bp, SLuindex_Type nb,
		       VOID_STAR cp)
{
   SLang_BString_Type **b;
   int ret;

   if (NULL == (b = make_n_bstrings (NULL, (char **)bp, nb, IS_NOT_TO_BE_FREED)))
     return -1;

   b_type = SLANG_BSTRING_TYPE;
   ret = bstring_bstring_bin_op (op,
				 a_type, ap, na,
				 b_type, (VOID_STAR) b, nb,
				 cp);
   free_n_bstrings (b, nb);
   SLfree ((char *) b);
   return ret;
}

static int
string_bstring_bin_op (int op,
		       SLtype a_type, VOID_STAR ap, SLuindex_Type na,
		       SLtype b_type, VOID_STAR bp, SLuindex_Type nb,
		       VOID_STAR cp)
{
   SLang_BString_Type **a;
   int ret;

   if (NULL == (a = make_n_bstrings (NULL, (char **)ap, na, IS_NOT_TO_BE_FREED)))
     return -1;

   a_type = SLANG_BSTRING_TYPE;
   ret = bstring_bstring_bin_op (op,
				 a_type, (VOID_STAR) a, na,
				 b_type, bp, nb,
				 cp);
   free_n_bstrings (a, na);
   SLfree ((char *) a);

   return ret;
}

static void bstring_destroy (SLtype unused, VOID_STAR s)
{
   (void) unused;
   SLbstring_free (*(SLang_BString_Type **) s);
}

static int bstring_push (SLtype unused, VOID_STAR sptr)
{
   (void) unused;

   return SLang_push_bstring (*(SLang_BString_Type **) sptr);
}

static int string_to_bstring (SLtype a_type, VOID_STAR ap, SLuindex_Type na,
			      SLtype b_type, VOID_STAR bp)
{
   char **s;
   SLang_BString_Type **b;

   (void) a_type;
   (void) b_type;

   s = (char **) ap;
   b = (SLang_BString_Type **) bp;

   if (NULL == make_n_bstrings (b, s, na, IS_SLSTRING))
     return -1;

   return 1;
}

static int bstring_to_string (SLtype a_type, VOID_STAR ap, SLuindex_Type na,
			      SLtype b_type, VOID_STAR bp)
{
   char **s;
   SLuindex_Type i;
   SLang_BString_Type **a;

   (void) a_type;
   (void) b_type;

   s = (char **) bp;
   a = (SLang_BString_Type **) ap;

   for (i = 0; i < na; i++)
     {
	SLang_BString_Type *ai = a[i];

	if (ai == NULL)
	  {
	     s[i] = NULL;
	     continue;
	  }

	if (NULL == (s[i] = SLang_create_slstring ((char *)BS_GET_POINTER(ai))))
	  {
	     while (i != 0)
	       {
		  i--;
		  _pSLang_free_slstring (s[i]);
		  s[i] = NULL;
	       }
	     return -1;
	  }
     }

   return 1;
}

static size_t Printable_BString_Size = 256;
static void set_printable_bstring_size (unsigned int *np)
{
   unsigned int n = *np;
   if (n > 0xFFFFU)
     n = 0xFFFFU;
   if (n < 16)
     n = 16;

   Printable_BString_Size = n;
}

static unsigned int get_printable_bstring_size (void)
{
   return (unsigned int) Printable_BString_Size;
}

static char *bstring_string (SLtype type, VOID_STAR v)
{
   SLang_BString_Type *s;
   unsigned char *buf;
   unsigned char *bytes, *bytes_max;
   unsigned char *b, *bmax;

   (void) type;

   s = *(SLang_BString_Type **) v;
   bytes = BS_GET_POINTER(s);
   bytes_max = bytes + s->len;

   buf = (unsigned char *)SLmalloc (Printable_BString_Size);
   if (buf == NULL)
     return NULL;

   b = buf;
   bmax = buf + (Printable_BString_Size - 4);

   while (bytes < bytes_max)
     {
	unsigned char ch = *bytes;

	if ((ch < 32) || (ch >= 127) || (ch == '\\'))
	  {
	     if (b + 4 > bmax)
	       break;

	     sprintf ((char *) b, "\\%03o", ch);
	     b += 4;
	  }
	else
	  {
	     if (b == bmax)
	       break;

	     *b++ = ch;
	  }

	bytes++;
     }

   if (bytes < bytes_max)
     {
	*b++ = '.';
	*b++ = '.';
	*b++ = '.';
     }
   *b = 0;

   /* reduce the size */
   b = (unsigned char *)SLrealloc ((char *)buf, 1+(b-buf));
   if (b != NULL) buf = b;
   return (char *)buf;
}

static unsigned int bstrlen_cmd (SLang_BString_Type *b)
{
   return b->len;
}

static unsigned int count_byte_occurrences (SLang_BString_Type *b, unsigned char *chp)
{
   unsigned char ch = *chp;
   unsigned char *bytes, *bytes_max;
   unsigned int n;

   bytes = BS_GET_POINTER(b);
   bytes_max = bytes + b->len;

   n = 0;
   while (bytes < bytes_max)
     {
	if (*bytes++ == ch)
	  n++;
     }
   return n;
}

/* returns the 1-based byte offset of substring in a string, or 0 */
static int issubbytes_1 (SLang_BString_Type *as, SLang_BString_Type *bs, unsigned int ofs0)
{
   unsigned int lena, lenb;
   unsigned char *a, *b, *amax, *bmax, *astart;
   unsigned char ch0;

   a = BS_GET_POINTER(as);
   b = BS_GET_POINTER(bs);
   lena = as->len;
   lenb = bs->len;

   if (ofs0)
     {
	if (lena < ofs0)
	  return 0;
	lena -= ofs0;
	a += ofs0;
     }

   if ((lenb > lena) || (lenb == 0))
     return 0;

   lena -= lenb;
   amax = a + lena;
   bmax = b + lenb;

   astart = a;
   ch0 = *b++;
   while (a <= amax)
     {
	unsigned char *a0, *b0;

	if (ch0 != *a++)
	  continue;
	a0 = a;
	b0 = b;
	while ((b < bmax) && (*a == *b))
	  {
	     a++;
	     b++;
	  }
	if (b == bmax)
	  return ofs0 + (a0 - astart);

	a = a0;
	b = b0;
     }
   return 0;
}

static SLindex_Type issubbytes (void)
{
   SLang_BString_Type *a, *b;
   SLindex_Type ofs = 0;

   if (SLang_Num_Function_Args == 3)
     {
	if (-1 == SLang_pop_array_index (&ofs))
	  return -1;
	if (ofs <= 0)
	  {
	     SLang_verror (SL_InvalidParm_Error, "1-based offset must be greater than 0");
	     return -1;
	  }
	ofs--;
     }
   if (-1 == SLang_pop_bstring (&b))
     return 0;
   if (0 == SLang_pop_bstring (&a))
     {
	SLuindex_Type uofs = issubbytes_1 (a, b, (SLuindex_Type) ofs);
	/* 1-based upon return */

	ofs = (SLindex_Type) uofs;
	SLbstring_free (a);
     }
   SLbstring_free (b);
   return ofs;
}

static SLang_BString_Type *join_bstrings (SLang_BString_Type **data, SLuindex_Type num, SLang_BString_Type *delim,
					  int tmp_opt_ok)
{
   size_t len;
   SLuindex_Type i;
   unsigned char *delim_ptr = NULL, *bytes = NULL;
   SLstrlen_Type delim_len = 0;
   SLang_BString_Type *bstr;
   unsigned char *ptr;
   SLstrlen_Type dlen;

   (void) tmp_opt_ok;

   if (num == 0)
     return SLbstring_create ((unsigned char *)"", 0);

   if (delim != NULL)
     {
	if (NULL == (delim_ptr = SLbstring_get_pointer (delim, &delim_len)))
	  return NULL;
     }

   len = 0;
   for (i = 0; i < num; i++)
     {
	if (data[i] == NULL)
	  continue;
	if (NULL == SLbstring_get_pointer (data[i], &dlen))
	  return NULL;

	len += dlen;
     }

   len += (num-1) * delim_len;

   if (len != (SLstrlen_Type)len)
     {
	SLang_set_error (SL_Malloc_Error);
	return NULL;
     }

# if SLANG_USE_TMP_OPTIMIZATION
   bstr = data[0];
   if (tmp_opt_ok && (bstr != NULL)
       && (bstr->num_refs == 1)	       /* owned by stack */
       && (bstr->ptr_type == IS_BSTRING)
       && (len < bstr->malloced_len))
     {
	bytes = (unsigned char *)bstr->v.bytes + bstr->len;
	bstr->len = len;
	bstr->num_refs++;
     }
   else
     {
#endif
	if (NULL == (bstr = create_bstring_of_type (NULL, len, IS_BSTRING)))
	  return NULL;

	bytes = bstr->v.bytes;

	if (NULL == (ptr = SLbstring_get_pointer (data[0], &dlen)))
	  goto return_error;

	memcpy (bytes, ptr, dlen);
	bytes += dlen;
#if SLANG_USE_TMP_OPTIMIZATION
     }
#endif

   for (i = 1; i < num; i++)
     {
	if (delim_len)
	  {
	     memcpy (bytes, delim_ptr, delim_len);
	     bytes += delim_len;
	  }
	if (data[i] != NULL)
	  {
	     if (NULL == (ptr = SLbstring_get_pointer (data[i], &dlen)))
	       goto return_error;
	     memcpy (bytes, ptr, dlen);
	     bytes += dlen;
	  }
     }
   *bytes = 0;
   return bstr;

return_error:

   SLbstring_free (bstr);
   return NULL;
}

static void bstrjoin_cmd (void)
{
   SLang_Array_Type *at;
   SLang_BString_Type *delim, *bstr;

   if (SLang_Num_Function_Args == 1)
     delim = NULL;
   else
     {
	if (-1 == SLang_pop_bstring (&delim))
	  return;
     }

   if (-1 == SLang_pop_array_of_type (&at, SLANG_BSTRING_TYPE))
     {
	if (delim != NULL)
	  SLbstring_free (delim);
	return;
     }

   bstr = join_bstrings ((SLang_BString_Type **)at->data, at->num_elements, delim, 0);
   if (bstr != NULL)
     {
	(void) SLang_push_bstring (bstr);
	SLbstring_free (bstr);
     }
   if (delim != NULL) SLbstring_free (delim);
   SLang_free_array (at);
}

static void bstrcat_cmd (void) /*{{{*/
{
   SLang_BString_Type *bstrings_buf[10];
   SLang_BString_Type **bstrings, *bstr;
   int i, nargs;

   nargs = SLang_Num_Function_Args;
   if (nargs <= 0) nargs = 2;

   if (nargs <= 10)
     bstrings = bstrings_buf;
   else if (NULL == (bstrings = (SLang_BString_Type **)_SLcalloc (nargs, sizeof (SLang_BString_Type *))))
     return;

   memset ((char *) bstrings, 0, sizeof (SLang_BString_Type *) * nargs);

   i = nargs;
   while (i != 0)
     {
	i--;

	if (-1 == SLang_pop_bstring (bstrings + i))
	  goto free_and_return;
     }

   bstr = join_bstrings (bstrings, nargs, NULL, 1);
   if (bstr != NULL)
     {
	(void) SLang_push_bstring (bstr);
	SLbstring_free (bstr);
     }
   /* drop */

free_and_return:
   for (i = 0; i < nargs; i++)
     SLbstring_free (bstrings[i]);

   if (bstrings != bstrings_buf)
     SLfree ((char *) bstrings);
}

static SLang_Intrin_Fun_Type BString_Table [] = /*{{{*/
{
   MAKE_INTRINSIC_1("bstrlen",  bstrlen_cmd, SLANG_UINT_TYPE, SLANG_BSTRING_TYPE),
   MAKE_INTRINSIC_0("bstrcat",  bstrcat_cmd, SLANG_VOID_TYPE),
   MAKE_INTRINSIC_0("bstrjoin",  bstrjoin_cmd, SLANG_VOID_TYPE),
   MAKE_INTRINSIC_2("count_byte_occurances", count_byte_occurrences, SLANG_UINT_TYPE, SLANG_BSTRING_TYPE, SLANG_UCHAR_TYPE),
   MAKE_INTRINSIC_2("count_byte_occurrences", count_byte_occurrences, SLANG_UINT_TYPE, SLANG_BSTRING_TYPE, SLANG_UCHAR_TYPE),
   MAKE_INTRINSIC_0("pack", _pSLpack, SLANG_VOID_TYPE),
   MAKE_INTRINSIC_2("unpack", _pSLunpack, SLANG_VOID_TYPE, SLANG_STRING_TYPE, SLANG_BSTRING_TYPE),
   MAKE_INTRINSIC_1("pad_pack_format", _pSLpack_pad_format, SLANG_VOID_TYPE, SLANG_STRING_TYPE),
   MAKE_INTRINSIC_1("sizeof_pack", _pSLpack_compute_size, SLANG_UINT_TYPE, SLANG_STRING_TYPE),
   MAKE_INTRINSIC_0("is_substrbytes", issubbytes, SLANG_ARRAY_INDEX_TYPE),
   MAKE_INTRINSIC_1("set_printable_bstring_size", set_printable_bstring_size, SLANG_VOID_TYPE, SLANG_UINT_TYPE),
   MAKE_INTRINSIC_0("get_printable_bstring_size", get_printable_bstring_size, SLANG_UINT_TYPE),
   SLANG_END_INTRIN_FUN_TABLE
};

struct _pSLang_Foreach_Context_Type
{
   SLang_BString_Type *bstr;
   unsigned char *s, *smax;	       /* pointers into string */
   int using_chars;
};

SLang_Foreach_Context_Type *
_pSLbstring_foreach_open (SLtype type, unsigned int num)
{
   char *u;
   SLang_Foreach_Context_Type *c;
   int using_chars = 0;
   SLang_BString_Type *bstr;
   SLstrlen_Type len;

   (void) type;

   if (-1 == SLang_pop_bstring (&bstr))
     return NULL;

   switch (num)
     {
      case 1:
	if (-1 == SLang_pop_slstring (&u))
	  {
	     SLbstring_free (bstr);
	     return NULL;
	  }
	if (0 == strcmp (u, "chars"))
	  using_chars = 1;
	else if (0 == strcmp (u, "bytes"))
	  using_chars = 0;
	else
	  {
	     _pSLang_verror (SL_InvalidParm_Error, "Expected foreach ([B]String_Type) using (chars|bytes)");
	     SLang_free_slstring (u);
	     SLbstring_free (bstr);
	     return NULL;
	  }
	SLang_free_slstring (u);
   	break;

      case 0:
	using_chars = 0;
	break;
      default:
	_pSLang_verror (SL_NumArgs_Error,
		      "'foreach ([B]String_Type) using' requires single control value (chars|bytes)");
	return NULL;
     }

   /* In UTF-8 mode, chars and bytes are synonymous */
   if (_pSLinterp_UTF8_Mode == 0)
     using_chars = 0;

   c = (SLang_Foreach_Context_Type *)SLmalloc (sizeof (SLang_Foreach_Context_Type));
   if (c == NULL)
     {
	SLbstring_free (bstr);
	return NULL;
     }

   memset ((char *) c, 0, sizeof (SLang_Foreach_Context_Type));

   c->bstr = bstr;
   c->s = SLbstring_get_pointer (bstr, &len);
   c->smax = c->s + len;
   c->using_chars = using_chars;
   return c;
}

void _pSLbstring_foreach_close (SLtype type, SLang_Foreach_Context_Type *c)
{
   (void) type;
   if (c == NULL) return;
   SLbstring_free (c->bstr);
   SLfree ((char *) c);
}

int _pSLbstring_foreach (SLtype type, SLang_Foreach_Context_Type *c)
{
   SLwchar_Type wch;
   unsigned char *s, *s1, *smax;

   (void) type;

   s = c->s;
   smax = c->smax;
   if (s == smax)
     return 0;

   if (c->using_chars == 0)
     {
	unsigned char ch = (unsigned char) *s++;
	c->s = s;

	if (-1 == SLclass_push_char_obj (SLANG_UCHAR_TYPE, ch))
	  return -1;

	return 1;
     }
   s1 = SLutf8_decode (s, smax, &wch, NULL);
   if (s1 == NULL)
     {
	int iwch = (int) *s;
	c->s = s + 1;
	/* Invalid encoded char-- return it as a negative int */
	if (-1 == SLang_push_int (-iwch))
	  return -1;

	return 1;
     }
   c->s = s1;
   if (-1 == SLang_push_wchar (wch))
     return -1;

   return 1;
}

static void bstring_inc_ref (SLtype type, VOID_STAR ptr, int dr)
{
   (void) type;
   (*(SLang_BString_Type **) ptr)->num_refs += dr;
}

int _pSLang_init_bstring (void)
{
   SLang_Class_Type *cl;

   if (NULL == (cl = SLclass_allocate_class ("BString_Type")))
     return -1;
   (void) SLclass_set_destroy_function (cl, bstring_destroy);
   (void) SLclass_set_push_function (cl, bstring_push);
   (void) SLclass_set_string_function (cl, bstring_string);
   cl->cl_inc_ref = bstring_inc_ref;

   if (-1 == SLclass_register_class (cl, SLANG_BSTRING_TYPE, sizeof (char *),
				     SLANG_CLASS_TYPE_PTR))
     return -1;

   if ((-1 == SLclass_add_typecast (SLANG_BSTRING_TYPE, SLANG_STRING_TYPE, bstring_to_string, 1))
       || (-1 == SLclass_add_typecast (SLANG_STRING_TYPE, SLANG_BSTRING_TYPE, string_to_bstring, 1))
       || (-1 == SLclass_add_binary_op (SLANG_STRING_TYPE, SLANG_BSTRING_TYPE, string_bstring_bin_op, bstring_bstring_bin_op_result))
       || (-1 == SLclass_add_binary_op (SLANG_BSTRING_TYPE, SLANG_STRING_TYPE, bstring_string_bin_op, bstring_bstring_bin_op_result))
       || (-1 == SLclass_add_binary_op (SLANG_BSTRING_TYPE, SLANG_BSTRING_TYPE, bstring_bstring_bin_op, bstring_bstring_bin_op_result)))
     return -1;

   cl->cl_foreach_open = _pSLbstring_foreach_open;
   cl->cl_foreach_close = _pSLbstring_foreach_close;
   cl->cl_foreach = _pSLbstring_foreach;

   if (-1 == SLadd_intrin_fun_table (BString_Table, NULL))
     return -1;

   return 0;
}

