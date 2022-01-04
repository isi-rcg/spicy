/* -*- mode: C; mode: fold -*-
Copyright (C) 2010-2017,2018 John E. Davis

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

#include <stdio.h>
#include <slang.h>
#include <string.h>
#include <pcre.h>

SLANG_MODULE(pcre);

static int PCRE_Type_Id = 0;

typedef struct
{
   pcre *p;
   pcre_extra *extra;
   int *ovector;
   unsigned int ovector_len;	       /* must be a multiple of 3 */
   unsigned int num_matches;	       /* return value of pcre_exec (>= 1)*/
}
PCRE_Type;

static void free_pcre_type (PCRE_Type *pt)
{
   if (pt->ovector != NULL)
     SLfree ((char *) pt->ovector);

   SLfree ((char *) pt);
}

static SLang_MMT_Type *allocate_pcre_type (pcre *p, pcre_extra *extra)
{
   PCRE_Type *pt;
   SLang_MMT_Type *mmt;
   int ovector_len;

   pt = (PCRE_Type *) SLmalloc (sizeof (PCRE_Type));
   if (pt == NULL)
     return NULL;
   memset ((char *) pt, 0, sizeof (PCRE_Type));

   pt->p = p;
   pt->extra = extra;

   if (0 != pcre_fullinfo (p, extra, PCRE_INFO_CAPTURECOUNT, &ovector_len))
     {
	free_pcre_type (pt);
	SLang_verror (SL_INTRINSIC_ERROR, "pcre_fullinfo failed");
	return NULL;
     }

   ovector_len += 1;		       /* allow for pattern matched */
   ovector_len *= 3;		       /* required to be multiple of 3 */
   if (NULL == (pt->ovector = (int *)SLmalloc (ovector_len * sizeof (int))))
     {
	free_pcre_type (pt);
	return NULL;
     }
   pt->ovector_len = ovector_len;

   if (NULL == (mmt = SLang_create_mmt (PCRE_Type_Id, (VOID_STAR) pt)))
     {
	free_pcre_type (pt);
	return NULL;
     }
   return mmt;
}

static int _pcre_compile_1 (char *pattern, int options)
{
   pcre *p;
   pcre_extra *extra;
   SLCONST char *err;
   int erroffset;
   unsigned char *table;
   SLang_MMT_Type *mmt;

   table = NULL;
   p = pcre_compile (pattern, options, &err, &erroffset, table);
   if (NULL == p)
     {
	SLang_verror (SL_Parse_Error, "Error compiling pattern '%s' at offset %d: %s",
		      pattern, erroffset, err);
	return -1;
     }

   extra = pcre_study (p, 0, &err);
   /* apparantly, a NULL return is ok */
   if (err != NULL)
     {
	SLang_verror (SL_INTRINSIC_ERROR, "pcre_study failed: %s", err);
	pcre_free (p);
	return -1;
     }

   if (NULL == (mmt = allocate_pcre_type (p, extra)))
     {
	pcre_free ((char *) p);
	pcre_free ((char *) extra);
	return -1;
     }

   if (-1 == SLang_push_mmt (mmt))
     {
	SLang_free_mmt (mmt);
	return -1;
     }
   return 0;
}

static void _pcre_compile (void)
{
   char *pattern;
   int options = 0;

   switch (SLang_Num_Function_Args)
     {
      case 2:
	if (-1 == SLang_pop_integer (&options))
	  return;
	/* drop */
      case 1:
      default:
	if (-1 == SLang_pop_slstring (&pattern))
	  return;
     }
   (void) _pcre_compile_1 (pattern, options);
   SLang_free_slstring (pattern);
}

/* returns number of matches */
static int _pcre_exec_1 (PCRE_Type *pt, char *str, unsigned int len, int pos, int options)
{
   int rc;

   pt->num_matches = 0;
   if ((unsigned int) pos > len)
     return 0;

   rc = pcre_exec (pt->p, pt->extra, str, len, pos,
		   options, pt->ovector, pt->ovector_len);

   if (rc == PCRE_ERROR_NOMATCH)
     return 0;

   if (rc <= 0)
     {
	SLang_verror (SL_INTRINSIC_ERROR, "pcre_exec returned %d", rc);
	return -1;
     }
   pt->num_matches = (unsigned int) rc;
   return rc;
}

static int _pcre_exec (void)
{
   PCRE_Type *p;
   SLang_MMT_Type *mmt;
   char *str;
   SLang_BString_Type *bstr = NULL;
   SLstrlen_Type len;
   int pos = 0;
   int options = 0;
   int ret = -1;

   switch (SLang_Num_Function_Args)
     {
      case 4:
	if (-1 == SLang_pop_integer (&options))
	  return -1;
	/* drop */
      case 3:
	/* drop */
	if (-1 == SLang_pop_integer (&pos))
	  return -1;
	/* drop */
      default:
	switch (SLang_peek_at_stack())
	  {
	   case SLANG_STRING_TYPE:
	     if (-1 == SLang_pop_slstring (&str))
	       return -1;
	     len = strlen (str);
	     break;

	   case SLANG_BSTRING_TYPE:
	   default:
	     if (-1 == SLang_pop_bstring(&bstr))
	       return -1;
	     str = (char *)SLbstring_get_pointer(bstr, &len);
	     if (str == NULL)
	       {
		  SLbstring_free (bstr);
		  return -1;
	       }
	     break;
	  }
     }

   if (NULL == (mmt = SLang_pop_mmt (PCRE_Type_Id)))
     goto free_and_return;
   p = (PCRE_Type *)SLang_object_from_mmt (mmt);

   ret = _pcre_exec_1 (p, str, len, pos, options);

free_and_return:

   SLang_free_mmt (mmt);
   if (bstr != NULL)
     SLbstring_free (bstr);
   else
     SLang_free_slstring (str);
   return ret;
}

static int get_nth_start_stop (PCRE_Type *pt, unsigned int n,
			       SLstrlen_Type *a, SLstrlen_Type *b)
{
   int start, stop;

   if (n >= pt->num_matches)
     return -1;

   start = pt->ovector[2*n];
   stop = pt->ovector[2*n+1];
   if ((start < 0) || (stop < start))
     return -1;

   *a = (unsigned int) start;
   *b = (unsigned int) stop;
   return 0;
}

static void _pcre_nth_match (PCRE_Type *pt, int *np)
{
   SLuindex_Type start, stop;
   SLang_Array_Type *at;
   SLindex_Type two = 2;
   int *data;

   if (-1 == get_nth_start_stop (pt, (unsigned int) *np, &start, &stop))
     {
	SLang_push_null ();
	return;
     }

   if (NULL == (at = SLang_create_array (SLANG_INT_TYPE, 0, NULL, &two, 1)))
     return;

   data = (int *)at->data;
   data[0] = (int)start;
   data[1] = (int)stop;
   (void) SLang_push_array (at, 1);
}

static void _pcre_nth_substr (PCRE_Type *pt, char *str, int *np)
{
   SLstrlen_Type start, stop;
   SLstrlen_Type len;

   len = strlen (str);

   if ((-1 == get_nth_start_stop (pt, (unsigned int) *np, &start, &stop))
       || (start > len) || (stop > len))
     {
	SLang_push_null ();
	return;
     }

   str = SLang_create_nslstring (str + start, stop - start);
   (void) SLang_push_string (str);
   SLang_free_slstring (str);
}

/* This function converts a slang RE to a pcre expression.  It performs the
 * following transformations:
 *    (     -->   \(
 *    )     -->   \)
 *    #     -->   \#
 *    |     -->   \|
 *    {     -->   \{
 *    }     -->   \}
 *   \<     -->   \b
 *   \>     -->   \b
 *   \C     -->   (?i)
 *   \c     -->   (?-i)
 *   \(     -->   (
 *   \)     -->   )
 *   \{     -->   {
 *   \}     -->   }
 * Anything else?
 */
static char *_slang_to_pcre (char *slpattern)
{
   char *pattern, *p, *s;
   SLstrlen_Type len;
   int in_bracket;
   char ch;

   len = strlen (slpattern);
   pattern = (char *)SLmalloc (3*len + 1);
   if (pattern == NULL)
     return NULL;

   p = pattern;
   s = slpattern;
   in_bracket = 0;
   while ((ch = *s++) != 0)
     {
	switch (ch)
	  {
	   case '{':
	   case '}':
	   case '(':
	   case ')':
	   case '#':
	   case '|':
	     if (0 == in_bracket) *p++ = '\\';
	     *p++ = ch;
	     break;

	   case '[':
	     in_bracket = 1;
	     *p++ = ch;
	     break;

	   case ']':
	     in_bracket = 0;
	     *p++ = ch;
	     break;

	   case '\\':
	     ch = *s++;
	     switch (ch)
	       {
		case 0:
		  s--;
		  break;

		case '<':
		case '>':
		  *p++ = '\\'; *p++ = 'b';
		  break;

		case '(':
		case ')':
		case '{':
		case '}':
		  *p++ = ch;
		  break;

		case 'C':
		  *p++ = '('; *p++ = '?'; *p++ = 'i'; *p++ = ')';
		  break;
		case 'c':
		  *p++ = '('; *p++ = '?'; *p++ = '-'; *p++ = 'i'; *p++ = ')';
		  break;

		default:
		  *p++ = '\\';
		  *p++ = ch;
	       }
	     break;

	   default:
	     *p++ = ch;
	     break;
	  }
     }
   *p = 0;

   s = SLang_create_slstring (pattern);
   SLfree (pattern);
   return s;
}

static void slang_to_pcre (char *pattern)
{
   /* NULL ok in code below */
   pattern = _slang_to_pcre (pattern);
   (void) SLang_push_string (pattern);
   SLang_free_slstring (pattern);
}

static void destroy_pcre (SLtype type, VOID_STAR f)
{
   PCRE_Type *pt;
   (void) type;

   pt = (PCRE_Type *) f;
   if (pt->extra != NULL)
     pcre_free ((char *) pt->extra);
   if (pt->p != NULL)
     pcre_free ((char *) pt->p);
   free_pcre_type (pt);
}

#define DUMMY_PCRE_TYPE ((SLtype)-1)
#define P DUMMY_PCRE_TYPE
#define I SLANG_INT_TYPE
#define V SLANG_VOID_TYPE
#define S SLANG_STRING_TYPE
static SLang_Intrin_Fun_Type PCRE_Intrinsics [] =
{
   MAKE_INTRINSIC_0("pcre_exec", _pcre_exec, I),
   MAKE_INTRINSIC_0("pcre_compile", _pcre_compile, V),
   MAKE_INTRINSIC_2("pcre_nth_match", _pcre_nth_match, V, P, I),
   MAKE_INTRINSIC_3("pcre_nth_substr", _pcre_nth_substr, V, P, S, I),
   MAKE_INTRINSIC_1("slang_to_pcre", slang_to_pcre, V, S),
   SLANG_END_INTRIN_FUN_TABLE
};

static SLang_IConstant_Type PCRE_Consts [] =
{
   /* compile options */
#ifndef PCRE_ANCHORED
# define PCRE_ANCHORED 0
#endif
   MAKE_ICONSTANT("PCRE_ANCHORED", PCRE_ANCHORED),
#ifndef PCRE_AUTO_CALLOUT
# define PCRE_AUTO_CALLOUT 0
#endif
   MAKE_ICONSTANT("PCRE_AUTO_CALLOUT", PCRE_AUTO_CALLOUT),
#ifndef PCRE_BSR_ANYCRLF
# define PCRE_BSR_ANYCRLF 0
#endif
   MAKE_ICONSTANT("PCRE_BSR_ANYCRLF", PCRE_BSR_ANYCRLF),
#ifndef PCRE_BSR_UNICODE
# define PCRE_BSR_UNICODE 0
#endif
   MAKE_ICONSTANT("PCRE_BSR_UNICODE", PCRE_BSR_UNICODE),
#ifndef PCRE_CASELESS
# define PCRE_CASELESS 0
#endif
   MAKE_ICONSTANT("PCRE_CASELESS", PCRE_CASELESS),
#ifndef PCRE_DUPNAMES
# define PCRE_DUPNAMES 0
#endif
   MAKE_ICONSTANT("PCRE_DUPNAMES", PCRE_DUPNAMES),
#ifndef PCRE_DOLLAR_ENDONLY
# define PCRE_DOLLAR_ENDONLY 0
#endif
   MAKE_ICONSTANT("PCRE_DOLLAR_ENDONLY", PCRE_DOLLAR_ENDONLY),
#ifndef PCRE_DOTALL
# define PCRE_DOTALL 0
#endif
   MAKE_ICONSTANT("PCRE_DOTALL", PCRE_DOTALL),
#ifndef PCRE_EXTENDED
# define PCRE_EXTENDED 0
#endif
   MAKE_ICONSTANT("PCRE_EXTENDED", PCRE_EXTENDED),
#ifndef PCRE_EXTRA
# define PCRE_EXTRA 0
#endif
   MAKE_ICONSTANT("PCRE_EXTRA", PCRE_EXTRA),
#ifndef PCRE_FIRSTLINE
# define PCRE_FIRSTLINE 0
#endif
   MAKE_ICONSTANT("PCRE_FIRSTLINE", PCRE_FIRSTLINE),
#ifndef PCRE_JAVASCRIPT_COMPAT
# define PCRE_JAVASCRIPT_COMPAT 0
#endif
   MAKE_ICONSTANT("PCRE_JAVASCRIPT_COMPAT", PCRE_JAVASCRIPT_COMPAT),
#ifndef PCRE_MULTILINE
# define PCRE_MULTILINE 0
#endif
   MAKE_ICONSTANT("PCRE_MULTILINE", PCRE_MULTILINE),
#ifndef PCRE_NEVER_UTF
# define PCRE_NEVER_UTF 0
#endif
   MAKE_ICONSTANT("PCRE_NEVER_UTF", PCRE_NEVER_UTF),
#ifndef PCRE_NEWLINE_ANY
# define PCRE_NEWLINE_ANY 0
#endif
   MAKE_ICONSTANT("PCRE_NEWLINE_ANY", PCRE_NEWLINE_ANY),
#ifndef PCRE_NEWLINE_ANYCRLF
# define PCRE_NEWLINE_ANYCRLF 0
#endif
   MAKE_ICONSTANT("PCRE_NEWLINE_ANYCRLF", PCRE_NEWLINE_ANYCRLF),
#ifndef PCRE_NEWLINE_CR
# define PCRE_NEWLINE_CR 0
#endif
   MAKE_ICONSTANT("PCRE_NEWLINE_CR", PCRE_NEWLINE_CR),
#ifndef PCRE_NEWLINE_CRLF
# define PCRE_NEWLINE_CRLF 0
#endif
   MAKE_ICONSTANT("PCRE_NEWLINE_CRLF", PCRE_NEWLINE_CRLF),
#ifndef PCRE_NEWLINE_LF
# define PCRE_NEWLINE_LF 0
#endif
   MAKE_ICONSTANT("PCRE_NEWLINE_LF", PCRE_NEWLINE_LF),
#ifndef PCRE_NO_START_OPTIMIZE
# define PCRE_NO_START_OPTIMIZE 0
#endif
   MAKE_ICONSTANT("PCRE_NO_START_OPTIMIZE", PCRE_NO_START_OPTIMIZE),
#ifndef PCRE_NOTEMPTY
# define PCRE_NOTEMPTY 0
#endif
   MAKE_ICONSTANT("PCRE_NOTEMPTY", PCRE_NOTEMPTY),
#ifndef PCRE_NO_AUTO_CAPTURE
# define PCRE_NO_AUTO_CAPTURE 0
#endif
   MAKE_ICONSTANT("PCRE_NO_AUTO_CAPTURE", PCRE_NO_AUTO_CAPTURE),
#ifndef PCRE_NO_AUTO_POSSESS
# define PCRE_NO_AUTO_POSSESS 0
#endif
   MAKE_ICONSTANT("PCRE_NO_AUTO_POSSESS", PCRE_NO_AUTO_POSSESS),
#ifndef PCRE_NO_UTF8_CHECK
# define PCRE_NO_UTF8_CHECK 0
#endif
   MAKE_ICONSTANT("PCRE_NO_UTF8_CHECK", PCRE_NO_UTF8_CHECK),
#ifndef PCRE_UCP
# define PCRE_UCP 0
#endif
   MAKE_ICONSTANT("PCRE_UCP", PCRE_UCP),
#ifndef PCRE_UNGREEDY
# define PCRE_UNGREEDY 0
#endif
   MAKE_ICONSTANT("PCRE_UNGREEDY", PCRE_UNGREEDY),
#ifndef PCRE_UTF8
# define PCRE_UTF8 0
#endif
   MAKE_ICONSTANT("PCRE_UTF8", PCRE_UTF8),

   /* exec options */
#ifndef PCRE_NOTBOL
# define PCRE_NOTBOL 0
#endif
   MAKE_ICONSTANT("PCRE_NOTBOL", PCRE_NOTBOL),
#ifndef PCRE_NOTEOL
# define PCRE_NOTEOL 0
#endif
   MAKE_ICONSTANT("PCRE_NOTEOL", PCRE_NOTEOL),
#ifndef PCRE_NOTEMPTY
# define PCRE_NOTEMPTY 0
#endif
   MAKE_ICONSTANT("PCRE_NOTEMPTY", PCRE_NOTEMPTY),
#ifndef PCRE_PARTIAL_SOFT
# define PCRE_PARTIAL_SOFT 0
#endif
   MAKE_ICONSTANT("PCRE_PARTIAL_SOFT", PCRE_PARTIAL_SOFT),
#ifndef PCRE_DFA_SHORTEST
# define PCRE_DFA_SHORTEST 0
#endif
   MAKE_ICONSTANT("PCRE_DFA_SHORTEST", PCRE_DFA_SHORTEST),
#ifndef PCRE_DFA_RESTART
# define PCRE_DFA_RESTART 0
#endif
   MAKE_ICONSTANT("PCRE_DFA_RESTART", PCRE_DFA_RESTART),
#ifndef PCRE_PARTIAL_HARD
# define PCRE_PARTIAL_HARD 0
#endif
   MAKE_ICONSTANT("PCRE_PARTIAL_HARD", PCRE_PARTIAL_HARD),
#ifndef PCRE_NOTEMPTY_ATSTART
# define PCRE_NOTEMPTY_ATSTART 0
#endif
   MAKE_ICONSTANT("PCRE_NOTEMPTY_ATSTART", PCRE_NOTEMPTY_ATSTART),

   SLANG_END_ICONST_TABLE
};

#undef P
#undef I
#undef V
#undef S

static int register_pcre_type (void)
{
   SLang_Class_Type *cl;

   if (PCRE_Type_Id != 0)
     return 0;

   if (NULL == (cl = SLclass_allocate_class ("PCRE_Type")))
     return -1;

   if (-1 == SLclass_set_destroy_function (cl, destroy_pcre))
     return -1;

   /* By registering as SLANG_VOID_TYPE, slang will dynamically allocate a
    * type.
    */
   if (-1 == SLclass_register_class (cl, SLANG_VOID_TYPE, sizeof (PCRE_Type), SLANG_CLASS_TYPE_MMT))
     return -1;

   PCRE_Type_Id = SLclass_get_class_id (cl);
   if (-1 == SLclass_patch_intrin_fun_table1 (PCRE_Intrinsics, DUMMY_PCRE_TYPE, PCRE_Type_Id))
     return -1;

   return 0;
}

static void *do_malloc (size_t n)
{
   return (void *) SLmalloc (n);
}

static void do_free (void *x)
{
   SLfree ((char *) x);
}

int init_pcre_module_ns (char *ns_name)
{
   SLang_NameSpace_Type *ns = SLns_create_namespace (ns_name);
   if (ns == NULL)
     return -1;

   if (-1 == register_pcre_type ())
     return -1;

   pcre_malloc = do_malloc;
   pcre_free = do_free;

   if ((-1 == SLns_add_intrin_fun_table (ns, PCRE_Intrinsics, "__PCRE__"))
       || (-1 == SLns_add_iconstant_table (ns, PCRE_Consts, NULL)))
     return -1;

   return 0;
}

/* This function is optional */
void deinit_pcre_module (void)
{
}

