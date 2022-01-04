/* -*- mode: C; mode: fold -*- */
/*
Copyright (C) 2013-2017,2018 John E. Davis, Manfred Hanke

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

#define _BSD_SOURCE 1		       /* to get strtoll */
#define _DEFAULT_SOURCE 1
#include "config.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <slang.h>

SLANG_MODULE(json);

#define JSON_MODULE_VERSION_NUMBER 300
static char* json_module_version_string = "pre-0.3.0";

/*{{{ JSON grammar based upon json.org & ietf.org/rfc/rfc4627.txt
 *
 * object:
 *   { }
 *   { members }
 * members:
 *   pair
 *   pair , members
 * pair:
 *   string : value
 *
 * array:
 *   [ ]
 *   [ elements ]
 * elements:
 *   value
 *   value , elements
 *
 * value:
 *   string
 *   number
 *   object
 *   array
 *   true
 *   false
 *   null
 *
 * Since a pair consists of a (arbitrary string) keyword and a value,
 * a JSON object maps onto a structure (Struct_Type) in S-Lang.
 *
 * Since a JSON array is a heterogenous collection of elements,
 * these map onto a list (List_Type) in S-Lang.
 *
 * Since S-Lang has no separate boolean type,
 * true|false are represented as 1|0 of UChar_Type.
 */

#define BEGIN_ARRAY	 '['
#define BEGIN_OBJECT	 '{'
#define END_ARRAY	 ']'
#define END_OBJECT	 '}'
#define VALUE_SEPARATOR	 ','
#define NAME_SEPARATOR	 ':'
#define STRING_DELIMITER '"'
#define ESCAPE_CHARACTER '\\'

/*}}}*/

static int Json_Parse_Error = -1;
static int Json_Invalid_Json_Error = -1;

#define DESCRIBE_CHAR_FMT "'%c' = 0x%02X"
#define DESCRIBE_CHAR(ch) ch, (unsigned int)(unsigned char)ch

static int Max_Recursion_Depth = 100;
typedef struct
{
   char *ptr;	/* points into input string */
   int depth;
}
Parse_Type;

static void skip_white (Parse_Type *p) /*{{{*/
{
   unsigned char *s = (unsigned char *)p->ptr;

   while ((*s == ' ') || (*s == '\t') || (*s == '\n') || (*s == '\r'))
     s++;

   p->ptr = (char *)s;
}
/*}}}*/

static int looking_at (Parse_Type *p, char ch) /*{{{*/
{
   return *p->ptr == ch;
}
/*}}}*/

static int skip_char (Parse_Type *p, char ch) /*{{{*/
{
   if (! looking_at (p, ch))
     return 0;

   p->ptr++;
   return 1;
}
/*}}}*/

static int parse_hex_digit (char ch) /*{{{*/
{
   if ('0' <= ch && ch <= '9')  return      ch - '0';
   if ('A' <= ch && ch <= 'F')  return 10 + ch - 'A';
   if ('a' <= ch && ch <= 'f')  return 10 + ch - 'a';
   else return -1;
}
/*}}}*/

static char *parse_4_hex_digits (char *s, unsigned int *new_string_len, char *new_string, int *is_binary_stringp) /*{{{*/
{
   int d1, d2, d3, d4;
   SLwchar_Type wchar;
#define BUFLEN 6
   SLuchar_Type buf[BUFLEN], *u;

   if (   (-1 == (d1 = parse_hex_digit (s[0])))
       || (-1 == (d2 = parse_hex_digit (s[1])))
       || (-1 == (d3 = parse_hex_digit (s[2])))
       || (-1 == (d4 = parse_hex_digit (s[3]))))
     {
	SLang_verror (Json_Parse_Error, "Illegal Unicode escape sequence in JSON string: \\u%c%c%c%c", s[0], s[1], s[2], s[3]);	 /* may contain '\000' */
	return NULL;
     }

   wchar = (d1 << 12) + (d2 << 8) + (d3 << 4) + d4;
   if (is_binary_stringp != NULL)
     *is_binary_stringp = (wchar == 0);

   u = new_string ? (SLuchar_Type*)new_string : buf;
   *new_string_len += SLutf8_encode (wchar, u, BUFLEN) - u;
#undef BUFLEN

   return s+4;
}
/*}}}*/

static int parse_string_length_and_move_ptr (Parse_Type *p, unsigned int *lenp, int *is_binary_stringp) /*{{{*/
{
   unsigned int new_string_len = 0;
   char *s = p->ptr;

   *lenp = 0; *is_binary_stringp = 0;

   while (1)
     {
	char ch = *s++;

	/* STRING_DELIMITER = 34, SPACE = 32 */
	if ((unsigned char)ch <= STRING_DELIMITER)
	  {
	     if (ch == STRING_DELIMITER)
	       break;

	     if (ch == 0)
	       {
		  SLang_verror (Json_Parse_Error, "Unexpected end of input seen while parsing a JSON string");
		  return -1;
	       }
	     if (ch < 32)
	       {
		  SLang_verror (Json_Parse_Error, "Control character 0x%02X in JSON string must be escaped", (unsigned int)ch);
		  return -1;
	       }
	     /* drop */
	  }

	if (ch == ESCAPE_CHARACTER)
	  {
	     ch = *s++;
	     switch (ch)
	       {
		case STRING_DELIMITER:
		case ESCAPE_CHARACTER:
		case '/':
		case 'b': case 'f': case 'n': case 'r': case 't':
		  new_string_len++;
		  break;
		case 'u':
		    {
		       int isbin;
		       if (NULL == (s = parse_4_hex_digits (s, &new_string_len, NULL, &isbin)))
			 return -1;
		       *is_binary_stringp |= isbin;
		       break;
		    }
		default:
		  SLang_verror (Json_Parse_Error, "Illegal escaped character " DESCRIBE_CHAR_FMT " in JSON string", DESCRIBE_CHAR(ch));
		  return -1;
	       }
	  }
	else
	  new_string_len++;
     }

   p->ptr = s;
   *lenp = new_string_len;
   return 0;
}
/*}}}*/

/* try to use string_buffer
 * if there is enough space and the string is not a binary string.
 */
static char *parse_string (Parse_Type *p,
			   char *string_buffer, unsigned int buflen,
			   unsigned int *bstring_lenp) /*{{{*/
{
   char *s, *new_string;
   unsigned int new_string_pos, new_string_len;
   int is_binary_string = 0;

   s = p->ptr;

   if (-1 == parse_string_length_and_move_ptr (p, &new_string_len, &is_binary_string))
     return NULL;

   new_string = string_buffer;
   if (((new_string_len >= buflen) || is_binary_string)
       && (NULL == (new_string = SLmalloc (new_string_len + 1))))
     return NULL;

   new_string_pos = 0;

   while (new_string_pos < new_string_len)
     {
	char ch = *s++;

	if ((ch == STRING_DELIMITER) || ((unsigned char)ch < 32))
	  goto return_application_error;

	if (ch != ESCAPE_CHARACTER)
	  {
	     new_string[new_string_pos++] = ch;
	     continue;
	  }

	ch = *s++;
	switch (ch)
	  {
	   case STRING_DELIMITER:
	   case ESCAPE_CHARACTER:
	   case '/':
	     new_string[new_string_pos++] = ch; break;
	   case 'b':
	     new_string[new_string_pos++] = '\b'; break;
	   case 'f':
	     new_string[new_string_pos++] = '\f'; break;
	   case 'n':
	     new_string[new_string_pos++] = '\n'; break;
	   case 'r':
	     new_string[new_string_pos++] = '\r'; break;
	   case 't':
	     new_string[new_string_pos++] = '\t'; break;
	   case 'u':
	     if (NULL != (s = parse_4_hex_digits (s, &new_string_pos, new_string + new_string_pos, NULL)))
	       break;  /* else drop */
	   default:
	     goto return_application_error;
	  }
     }

   if (bstring_lenp != NULL)
     *bstring_lenp = (is_binary_string ? new_string_len : 0);

   new_string[new_string_pos] = 0;
   return new_string;

return_application_error:
   /* Since any JSon_Parse_Error should already have been recognized
    * (by parse_string_length_and_move_ptr), something must be wrong here.
    */
   SLang_verror (SL_Application_Error, "JSON string being parsed appears to be changing");
   if (new_string != string_buffer) SLfree (new_string);
   return NULL;
}
/*}}}*/

static int parse_and_push_string (Parse_Type *p) /*{{{*/
{
   unsigned int bstring_len;
   char *s;
   char buf[512];

   if (NULL == (s = parse_string (p, buf, sizeof (buf), &bstring_len)))
     return -1;

   if (bstring_len)
     {
	SLang_BString_Type *bstr;
	int status;

	/* NOTE: parse_string will not use buf for a binary string */
	if (NULL == (bstr = SLbstring_create_malloced ((unsigned char *)s, bstring_len, 1)))
	  return -1;
	/* s now belongs to bstr */

	status = SLang_push_bstring (bstr);
	SLbstring_free (bstr);
	return status;
     }

   if (s != buf)
     return SLang_push_malloced_string (s);   /* frees s upon return */

   return SLang_push_string (buf);
}
/*}}}*/

static int parse_and_push_number (Parse_Type *p) /*{{{*/
{
   char *s = p->ptr, ch;
   int is_int = 1, result;

   if (*s == '-')
     s++;
   while ('0' <= *s && *s <= '9')
     s++;
   if (*s == '.')
     {
	is_int = 0;
	s++;
	while ('0' <= *s && *s <= '9')
	  s++;
     }
   if (*s == 'e' || *s == 'E')
     {
	is_int = 0;
	s++;
	if (*s == '+' || *s == '-')
	  s++;
	while ('0' <= *s && *s <= '9')
	  s++;
     }

   ch = *s;
   *s = 0;
   errno = 0;
   result = is_int ?
#ifdef HAVE_LONG_LONG
	    SLang_push_long_long (strtoll (p->ptr, NULL, 10))
#else
	    SLang_push_long (strtol (p->ptr, NULL, 10))
#endif
	  : SLang_push_double (strtod (p->ptr, NULL));
   if (errno == ERANGE)
     {
	SLang_verror (Json_Parse_Error,
		      is_int
			? "Integer value is too large (%s)"
			: "Numeric value is too large (%s)",
		      p->ptr);
     }

   *s = ch;
   p->ptr = s;
   return result;
}
/*}}}*/

static int parse_and_push_literal (Parse_Type *p) /*{{{*/
{
   char *s = p->ptr;

   if (0 == strncmp (s, "true", 4))
     {
	p->ptr += 4;
	return SLang_push_uchar (1);
     }

   if (0 == strncmp (s, "false", 5))
     {
	p->ptr += 5;
	return SLang_push_uchar (0);
     }

   if (0 == strncmp (s, "null", 4))
     {
	p->ptr += 4;
	return SLang_push_null ();
     }

   SLang_verror (Json_Parse_Error, "Unexpected character " DESCRIBE_CHAR_FMT " seen while parsing a JSON value", DESCRIBE_CHAR(*s));
   return -1;
}
/*}}}*/

static int parse_and_push_object_as_struct (Parse_Type *, int);
#if 0
static int parse_and_push_object_as_assoc (Parse_Type *, int);
#endif
static int parse_and_push_array (Parse_Type *, int);
static int parse_and_push_value (Parse_Type *p, int only_toplevel_values) /*{{{*/
{
   int ret;

   skip_white (p);

   if (! only_toplevel_values)
     {
	if (skip_char (p, STRING_DELIMITER))
	  return parse_and_push_string (p);
	switch (*p->ptr)
	  {
	   case '-':
	   case '0': case '1': case '2': case '3': case '4':
	   case '5': case '6': case '7': case '8': case '9':
	     return parse_and_push_number (p);
	   case 'f':
	   case 't':
	   case 'n':
	     return parse_and_push_literal (p);
	  }
     }
   if (p->depth > Max_Recursion_Depth)
     {
	SLang_verror (Json_Parse_Error, "json text exceeds maximum nesting level of %d", Max_Recursion_Depth);
	return -1;
     }

   if (skip_char (p, BEGIN_OBJECT))
     {
	p->depth++;
	ret = parse_and_push_object_as_struct (p, only_toplevel_values);
	p->depth--;
	return ret;
     }

   if (skip_char (p, BEGIN_ARRAY))
     {
	p->depth++;
	ret = parse_and_push_array (p, only_toplevel_values);
	p->depth--;
	return ret;
     }

   SLang_verror (Json_Parse_Error, (only_toplevel_values
				    ? "Unexpected character " DESCRIBE_CHAR_FMT " seen while parsing JSON data (must be an object or an array)"
				    : "Unexpected character " DESCRIBE_CHAR_FMT " seen while parsing a JSON value"
				   ), DESCRIBE_CHAR(*p->ptr));
   return -1;
}
/*}}}*/

#if 0
static int parse_and_push_object_as_assoc (Parse_Type *p, int toplevel) /*{{{*/
{
   SLang_Assoc_Array_Type *assoc;
   char buf[512];

   if (NULL == (assoc = SLang_create_assoc (SLANG_ANY_TYPE, 0)))
     return -1;

   skip_white (p);
   if (! looking_at (p, END_OBJECT)) do
     {
	char *keyword;
	char *str;

	skip_white (p);
	if (! skip_char (p, STRING_DELIMITER))
	  {
	     SLang_verror (Json_Parse_Error, "Expected a string while parsing a JSON object, found " DESCRIBE_CHAR_FMT, DESCRIBE_CHAR(*p->ptr));
	     goto return_error;
	  }

	str = parse_string (p, buf, sizeof (buf), NULL);  /* ignoring binary strings */
	if (str == NULL)
	  goto return_error;

	keyword = SLang_create_slstring (str);
	if (str != buf)
	  SLfree (str);

	if (keyword == NULL)
	  goto return_error;

	skip_white (p);
	if (! skip_char (p, NAME_SEPARATOR))
	  {
	     SLang_verror (Json_Parse_Error, "Expected a '%c' while parsing a JSON object, found " DESCRIBE_CHAR_FMT,
			   NAME_SEPARATOR, DESCRIBE_CHAR(*p->ptr));
	     SLang_free_slstring (keyword);
	     goto return_error;
	  }

	if ((-1 == parse_and_push_value (p, 0))
	    || (-1 == SLang_assoc_put (assoc, keyword)))
	  {
	     SLang_free_slstring (keyword);
	     goto return_error;
	  }

	SLang_free_slstring (keyword);
	skip_white (p);
     }
   while (skip_char (p, VALUE_SEPARATOR));

   if (skip_char (p, END_OBJECT))
     {
	skip_white (p);
	if (! toplevel || looking_at (p, 0))
	  return SLang_push_assoc (assoc, 1);

	SLang_verror (Json_Parse_Error, "Expected end of input after parsing JSON object, found " DESCRIBE_CHAR_FMT, DESCRIBE_CHAR(*p->ptr));
     }
   else
     {
	if (looking_at (p, 0))
	  SLang_verror (Json_Parse_Error, "Unexpected end of input seen while parsing a JSON object");
	else
	  SLang_verror (Json_Parse_Error, "Expected '%c' or '%c' while parsing a JSON object, found " DESCRIBE_CHAR_FMT,
			VALUE_SEPARATOR, END_OBJECT, DESCRIBE_CHAR(*p->ptr));
     }

return_error:
   SLang_free_assoc (assoc);
   return -1;
}
/*}}}*/
#endif

static void free_string_array (char **sp, unsigned int n)
{
   if (sp == NULL)
     return;

   while (n > 0)
     {
	n--;
	SLang_free_slstring (sp[n]);
     }
   SLfree ((char *)sp);
}

/* This has table implementation does not copy the strings */
#define HASH_TABLE_SIZE 601
typedef struct String_Hash_Elem_Type
{
   SLstr_Type *string;		       /* not copied! */
   unsigned int val;
   struct String_Hash_Elem_Type *next;
}
String_Hash_Elem_Type;

typedef struct
{
   String_Hash_Elem_Type hash_table[HASH_TABLE_SIZE];
   unsigned int num_strings;
   unsigned int num_collisions;
}
String_Hash_Type;

static String_Hash_Type *create_string_hash (void)
{
   String_Hash_Type *h;
   if (NULL == (h = (String_Hash_Type *)SLmalloc(sizeof(String_Hash_Type))))
     return NULL;
   memset ((char *)h, 0, sizeof(String_Hash_Type));
   return h;
}

/* returns -1 upon failure, 0 if string added, 1 if already there */
static int add_string_to_hash (String_Hash_Type *h, char *s, unsigned int val, unsigned int *valp)
{
   SLstr_Hash_Type hash;
   String_Hash_Elem_Type *e, *e1;

   hash = SLcompute_string_hash (s);
   e = &h->hash_table[hash % HASH_TABLE_SIZE];
   if (e->string == NULL)
     {
	e->string = s;
	*valp = e->val = val;
	h->num_strings++;
	return 0;
     }

   while (1)
     {
	if (e->string == s)
	  {
	     *valp = e->val;
	     return 1;
	  }
	if (e->next == NULL)
	  break;
	e = e->next;
     }

   e1 = (String_Hash_Elem_Type *)SLmalloc (sizeof (String_Hash_Elem_Type));
   if (e1 == NULL)
     return -1;

   e1->string = s;
   *valp = e1->val = val;
   e1->next = NULL;
   e->next = e1;
   h->num_strings++;
   h->num_collisions++;
   return 0;
}

static void free_string_hash (String_Hash_Type *h)
{
   String_Hash_Elem_Type *e, *emax;
   unsigned int num_collisions;

   if (h == NULL)
     return;

   e = h->hash_table;
   emax = e + HASH_TABLE_SIZE;
   num_collisions = h->num_collisions;
   while (num_collisions && (e < emax))
     {
	String_Hash_Elem_Type *e1;
	if (e->next == NULL)
	  {
	     e++;
	     continue;
	  }
	e1 = e->next;
	while (e1 != NULL)
	  {
	     String_Hash_Elem_Type *e2 = e1->next;
	     SLfree ((char *)e1);
	     num_collisions--;
	     e1 = e2;
	  }
	e++;
     }
   SLfree ((char *)h);
}

static int parse_and_push_object_as_struct (Parse_Type *p, int toplevel) /*{{{*/
{
   char buf[512];
   unsigned int num_fields, max_fields;
   char **fields;
   String_Hash_Type *h = NULL;

   max_fields = 16;
   num_fields = 0;
   if ((NULL == (fields = (char **)SLmalloc (max_fields * sizeof (char *))))
       || (NULL == (h = create_string_hash ())))
     goto return_error;

   skip_white (p);
   if (! looking_at (p, END_OBJECT)) do
     {
	char *keyword;
	char *str;
	int status;
	unsigned int idx;

	skip_white (p);
	if (! skip_char (p, STRING_DELIMITER))
	  {
	     SLang_verror (Json_Parse_Error, "Expected a string while parsing a JSON object, found " DESCRIBE_CHAR_FMT, DESCRIBE_CHAR(*p->ptr));
	     goto return_error;
	  }

	str = parse_string (p, buf, sizeof (buf), NULL);  /* ignoring binary strings */
	if (str == NULL)
	  goto return_error;

	keyword = SLang_create_slstring (str);
	if (str != buf)
	  SLfree (str);

	if (keyword == NULL)
	  goto return_error;

	if (-1 == (status = add_string_to_hash (h, keyword, num_fields, &idx)))
	  goto return_error;

	if (status == 0)
	  {
	     if (num_fields == max_fields)
	       {
		  char **new_fields;
		  unsigned int new_max_fields = max_fields + 32;

		  if (NULL == (new_fields = (char **) SLrealloc ((char *)fields, new_max_fields*sizeof(char *))))
		    {
		       SLang_free_slstring (keyword);
		       goto return_error;
		    }
		  fields = new_fields;
		  max_fields = new_max_fields;
	       }
	     fields[num_fields++] = keyword;
	  }

	skip_white (p);
	if (! skip_char (p, NAME_SEPARATOR))
	  {
	     SLang_verror (Json_Parse_Error, "Expected a '%c' while parsing a JSON object, found " DESCRIBE_CHAR_FMT,
			   NAME_SEPARATOR, DESCRIBE_CHAR(*p->ptr));
	     goto return_error;
	  }


	if (-1 == parse_and_push_value (p, 0))
	  goto return_error;

	if (status == 1)
	  {
	     /* keyword already exists -- update value */
	     if ((-1 == SLstack_exch (0, num_fields - idx))
		 || (-1 == SLdo_pop ()))
	       goto return_error;

	  }
	skip_white (p);
     }
   while (skip_char (p, VALUE_SEPARATOR));

   if (skip_char (p, END_OBJECT))
     {
	skip_white (p);
	if (! toplevel || looking_at (p, 0))
	  {
	     SLang_Struct_Type *s;

	     if (NULL == (s = SLang_create_struct (fields, num_fields)))
	       goto return_error;

	     if ((-1 == SLang_pop_struct_fields (s, num_fields))
		 || (-1 == SLang_push_struct (s)))
	       {
		  SLang_free_struct (s);
		  goto return_error;
	       }
	     SLang_free_struct (s);
	     free_string_hash (h);
	     free_string_array (fields, num_fields);
	     return 0;
	  }

	SLang_verror (Json_Parse_Error, "Expected end of input after parsing JSON object, found " DESCRIBE_CHAR_FMT, DESCRIBE_CHAR(*p->ptr));
     }
   else
     {
	if (looking_at (p, 0))
	  SLang_verror (Json_Parse_Error, "Unexpected end of input seen while parsing a JSON object");
	else
	  SLang_verror (Json_Parse_Error, "Expected '%c' or '%c' while parsing a JSON object, found " DESCRIBE_CHAR_FMT,
			VALUE_SEPARATOR, END_OBJECT, DESCRIBE_CHAR(*p->ptr));
     }

return_error:
   free_string_array (fields, num_fields);
   free_string_hash (h);
   return -1;
}
/*}}}*/

static int parse_and_push_array (Parse_Type *p, int toplevel) /*{{{*/
{
   SLang_List_Type *list = SLang_create_list (8);   /* let's start with 8 elements */

   if (list == NULL)
     return -1;

   skip_white (p);
   if (! looking_at (p, END_ARRAY)) do
     {
	if ((-1 == parse_and_push_value (p, 0))
	    || (-1 == SLang_list_append (list, -1)))
	  goto return_error;
	skip_white (p);
     }
   while (skip_char (p, VALUE_SEPARATOR));

   if (skip_char (p, END_ARRAY))
     {
	skip_white (p);
	if (! toplevel || looking_at (p, 0))
	  return SLang_push_list (list, 1);

	SLang_verror (Json_Parse_Error, "Expected end of input after parsing JSON array, found " DESCRIBE_CHAR_FMT, DESCRIBE_CHAR(*p->ptr));
     }
   else
     {
	if (looking_at (p, 0))
	  SLang_verror (Json_Parse_Error, "Unexpected end of input seen while parsing a JSON array");
	else
	  SLang_verror (Json_Parse_Error, "Expected '%c' or '%c' while parsing a JSON array, found " DESCRIBE_CHAR_FMT,
			VALUE_SEPARATOR, END_ARRAY, DESCRIBE_CHAR(*p->ptr));
     }

return_error:
   SLang_free_list (list);
   return -1;
}
/*}}}*/

static void parse_start (char *input_string) /*{{{*/
{
   Parse_Type pbuf, *p = &pbuf;
   memset ((char *)p, 0, sizeof (Parse_Type));
   p->ptr = input_string;

   if ((NULL == input_string)
       || (0 == *input_string))
     SLang_verror (Json_Parse_Error, "Unexpected empty input string");
   else
     parse_and_push_value (p, 1);
}
/*}}}*/

static void json_decode (void) /*{{{*/
{
   char *buffer;

   if ((SLang_Num_Function_Args != 1)
       || (-1 == SLpop_string (&buffer)))
     {
	SLang_verror (SL_Usage_Error, "Usage: json_decode (String_Type json_text)");
	return;
     }
   parse_start (buffer);
   SLfree (buffer);
}
/*}}}*/

/*{{{ json_generate_string implementation and support functions */

static unsigned int Len_Map[128] = /*{{{*/
{
   6,6,6,6,6,6,6,6,
   2,2,2,6,2,2,6,6,
   6,6,6,6,6,6,6,6,
   6,6,6,6,6,6,6,6,
   1,1,2,1,1,1,1,1,
   1,1,1,1,1,1,1,1,
   1,1,1,1,1,1,1,1,
   1,1,1,1,1,1,1,1,
   1,1,1,1,1,1,1,1,
   1,1,1,1,1,1,1,1,
   1,1,1,1,1,1,1,1,
   1,1,1,1,2,1,1,1,
   1,1,1,1,1,1,1,1,
   1,1,1,1,1,1,1,1,
   1,1,1,1,1,1,1,1,
   1,1,1,1,1,1,1,6
};
/*}}}*/

static char *String_Map[128] = /*{{{*/
{
    "\\u0000", "\\u0001", "\\u0002", "\\u0003", "\\u0004", "\\u0005", "\\u0006", "\\u0007",
        "\\b",     "\\t",     "\\n", "\\u000B",     "\\f",     "\\r", "\\u000E", "\\u000F",
    "\\u0010", "\\u0011", "\\u0012", "\\u0013", "\\u0014", "\\u0015", "\\u0016", "\\u0017",
    "\\u0018", "\\u0019", "\\u001A", "\\u001B", "\\u001C", "\\u001D", "\\u001E", "\\u001F",
          " ",       "!",    "\\\"",       "#",       "$",       "%",       "&",       "'",
          "(",       ")",       "*",       "+",       ",",       "-",       ".",       "/",
          "0",       "1",       "2",       "3",       "4",       "5",       "6",       "7",
          "8",       "9",       ":",       ";",       "<",       "=",       ">",       "?",
          "@",       "A",       "B",       "C",       "D",       "E",       "F",       "G",
          "H",       "I",       "J",       "K",       "L",       "M",       "N",       "O",
          "P",       "Q",       "R",       "S",       "T",       "U",       "V",       "W",
          "X",       "Y",       "Z",       "[",    "\\\\",       "]",       "^",       "_",
          "`",       "a",       "b",       "c",       "d",       "e",       "f",       "g",
          "h",       "i",       "j",       "k",       "l",       "m",       "n",       "o",
          "p",       "q",       "r",       "s",       "t",       "u",       "v",       "w",
          "x",       "y",       "z",       "{",       "|",       "}",       "~", "\\u007F"
};
/*}}}*/

/* Adapted from SLutf8.c */
static int is_invalid_or_overlong_utf8 (SLuchar_Type *u, unsigned int len)
{
   unsigned int i;
   unsigned char ch, ch1;

   /* Check for invalid sequences */
   for (i = 1; i < len; i++)
     {
	if ((u[i] & 0xC0) != 0x80)
	  return 1;
     }

   /* Illegal (overlong) sequences */
   /*           1100000x (10xxxxxx) */
   /*           11100000 100xxxxx (10xxxxxx) */
   /*           11110000 1000xxxx (10xxxxxx 10xxxxxx) */
   /*           11111000 10000xxx (10xxxxxx 10xxxxxx 10xxxxxx) */
   /*           11111100 100000xx (10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx) */
   ch = *u;
   if ((ch == 0xC0) || (ch == 0xC1))
     return 1;

   ch1 = u[1];
   if (((ch1 & ch) == 0x80)
       && ((ch == 0xE0)
	   || (ch == 0xF0)
	   || (ch == 0xF8)
	   || (ch == 0xFC)))
     return 1;

   return 0;
}

static SLstrlen_Type compute_multibyte_char_len (char *p, char *pmax) /*{{{*/
{
   SLstrlen_Type len;
   unsigned char ch;

   ch = (unsigned char)*p;
   len = ((ch & 0xE0) == 0xC0) ? 2  /* (ch & 0b11100000) == 0b11000000 */
     : ((ch & 0xF0) == 0xE0) ? 3  /* (ch & 0b11110000) == 0b11100000 */
     : ((ch & 0xF8) == 0xF0) ? 4  /* (ch & 0b11111000) == 0b11110000 */
     : ((ch & 0xFC) == 0xF8) ? 5  /* (ch & 0b11111100) == 0b11111000 */
     :                         6;

   if (p + len > pmax)
     return 1;

   if (is_invalid_or_overlong_utf8 ((SLuchar_Type *)p, len))
     return 1;

   return len;
}

/*}}}*/

static char *alloc_encoded_json_string (char *ptr, char *end_of_input_string, SLstrlen_Type *lenp) /*{{{*/
{
   SLstrlen_Type len = 2;			       /* first '"' and last '"' */

   while (ptr < end_of_input_string)
     {
	unsigned char ch = (unsigned char) *ptr;
	if (ch < 0x80)
	  {
	     len += Len_Map[ch];
	     ptr++;
	     continue;
	  }

	len += 6;		       /* FIXME: Does not handle 0x1UUUU */
	ptr += compute_multibyte_char_len (ptr, end_of_input_string);

	if (ptr > end_of_input_string)
	  {
	     SLang_verror (Json_Invalid_Json_Error, "Invalid UTF-8 at end of string");
	     return NULL;
	  }
     }

   *lenp = len;
   return SLmalloc (len + 1);
}
/*}}}*/

static char *fill_encoded_json_string (char *ptr, char *end_of_input_string,
				       char *dest_ptr) /*{{{*/
{
   *dest_ptr++ = STRING_DELIMITER;

   while (ptr < end_of_input_string)
     {
	unsigned char ch = *ptr;
	unsigned int len;

	if (ch < 0x80)
	  {
	     if (1 == (len = Len_Map[ch]))
	       *dest_ptr++ = ch;
	     else
	       {
		  char *str = String_Map[ch];
		  while (len--)
		    *dest_ptr++ = *str++;
	       }
	     ptr++;
	     continue;
	  }

	/* We cannot use SLutf8_decode, since we need to handle invalid_or_overlong_utf8 or ILLEGAL_UNICODE as well. */
	len = compute_multibyte_char_len (ptr, end_of_input_string);
	if (len == 1)
	  {
	     /* Malformed or overlong */
	     sprintf (dest_ptr, "<%02X>", (unsigned char)*ptr);
	     dest_ptr += 4;
	  }
	else
	  {  /* stolen from slutf8.c : fast_utf8_decode */
	     static unsigned char masks[7] = { 0, 0, 0x1F, 0xF, 0x7, 0x3, 0x1 };
	     SLwchar_Type w = (ch & masks[len]);
	     SLstrlen_Type i;
	     for (i = 1; i < len; i++)
	       w = (w << 6) | (ptr[i] & 0x3F);

	     if (w > 0xFFFF)
	       {
		  /* FIXME: Must be encoded as a pair of UTF-16 surrogates */
		  memcpy (dest_ptr, ptr, len);
		  dest_ptr += len;
	       }
	     else
	       {
		  sprintf (dest_ptr, "\\u%04X", w);
		  dest_ptr += 6;
	       }
	  }
	ptr += len;
     }
   *dest_ptr++ = STRING_DELIMITER;
   *dest_ptr = 0;
   return dest_ptr;
}
/*}}}*/

static void json_encode_string (void) /*{{{*/
{
   SLang_BString_Type *bstring = NULL;
   char *string, *encoded_json_string;
   SLstrlen_Type len, new_len;

   if (SLang_peek_at_stack () == SLANG_BSTRING_TYPE)
     {
	if (-1 == SLang_pop_bstring (&bstring))
	  return;

	string = (char *)SLbstring_get_pointer (bstring, &len);
     }
   else
     {
	if (-1 == SLang_pop_slstring (&string))
	  {
	     SLang_verror (SL_Usage_Error, "usage: _json_generate_string (String_Type json_string)");
	     return;
	  }
	len = strlen (string);
     }

   if ((encoded_json_string = alloc_encoded_json_string (string, string + len, &new_len)) != NULL)
     {
	SLang_BString_Type *b;
	char *enc_end;

	enc_end = fill_encoded_json_string (string, string + len, encoded_json_string);
	new_len = enc_end - encoded_json_string;

	b = SLbstring_create_malloced ((unsigned char *)encoded_json_string, new_len, 1);
	if (b != NULL)
	  {
	     (void) SLang_push_bstring (b);
	     SLbstring_free (b);
	  }
     }

   if (bstring != NULL)
     SLbstring_free (bstring);
   else
     SLang_free_slstring (string);
}
/*}}}*/

/*}}}*/

static SLang_Intrin_Fun_Type Module_Intrinsics [] = /*{{{*/
{
   MAKE_INTRINSIC_0("json_decode", json_decode, SLANG_VOID_TYPE),
   MAKE_INTRINSIC_0("_json_encode_string", json_encode_string, SLANG_VOID_TYPE),
   SLANG_END_INTRIN_FUN_TABLE
};
/*}}}*/

static SLang_Intrin_Var_Type Module_Variables [] = /*{{{*/
{
   MAKE_VARIABLE("_json_module_version_string", &json_module_version_string, SLANG_STRING_TYPE, 1),
   SLANG_END_INTRIN_VAR_TABLE
};
/*}}}*/

static SLang_IConstant_Type Module_Constants [] = /*{{{*/
{
   MAKE_ICONSTANT("_json_module_version", JSON_MODULE_VERSION_NUMBER),
   SLANG_END_ICONST_TABLE
};
/*}}}*/

int init_json_module_ns (char *ns_name) /*{{{*/
{
   SLang_NameSpace_Type *ns = SLns_create_namespace (ns_name);
   if (ns == NULL)
     return -1;

   if ((Json_Parse_Error == -1)
       && (-1 == (Json_Parse_Error = SLerr_new_exception (SL_RunTime_Error, "Json_Parse_Error", "JSON Parse Error"))))
     return -1;

   if ((Json_Invalid_Json_Error == -1)
       && (-1 == (Json_Invalid_Json_Error = SLerr_new_exception (SL_RunTime_Error, "Json_Invalid_Json_Error", "Invalid JSON Error"))))
     return -1;

   if ((-1 == SLns_add_intrin_fun_table (ns, Module_Intrinsics, NULL))
       || (-1 == SLns_add_intrin_var_table (ns, Module_Variables, NULL))
       || (-1 == SLns_add_iconstant_table (ns, Module_Constants, NULL)))
     return -1;

   return 0;
}
/*}}}*/

void deinit_json_module (void) /*{{{*/
{
   /* This function is optional */
}
/*}}}*/
