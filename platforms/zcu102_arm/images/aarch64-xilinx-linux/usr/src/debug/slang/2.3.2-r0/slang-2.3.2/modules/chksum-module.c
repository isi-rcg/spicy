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
#include <stdio.h>
#include <string.h>
#include <slang.h>

#include "chksum.h"

SLANG_MODULE(chksum);

static int Chksum_Type_Id = 0;

typedef struct
{
   SLFUTURE_CONST char *name;
   SLChksum_Type *(*create)(char *);
}
Chksum_Def_Type;

typedef struct
{
   char *name;
   unsigned int numrefs;
   SLChksum_Type *c;
}
Chksum_Object_Type;

static Chksum_Def_Type Chksum_Table[] =
{
   {"md5", _pSLchksum_md5_new},
   {"sha1", _pSLchksum_sha1_new},
   {NULL, NULL}
};

static Chksum_Def_Type *lookup_chksum (char *name)
{
   Chksum_Def_Type *t = Chksum_Table;

   while (t->name != NULL)
     {
	if (0 == strcmp (t->name, name))
	  return t;
	t++;
     }

   SLang_verror (SL_RunTime_Error, "Unsupported/Unknown checksum method `%s'", name);
   return NULL;
}

static int chksum_push (Chksum_Object_Type *obj)
{
   obj->numrefs++;

   if (0 == SLclass_push_ptr_obj (Chksum_Type_Id, (VOID_STAR)obj))
     return 0;

   obj->numrefs--;
   return -1;
}

static void chksum_free (Chksum_Object_Type *obj)
{
   if (obj == NULL)
     return;
   if (obj->numrefs > 1)
     {
	obj->numrefs--;
	return;
     }
   if (obj->c != NULL)
     (void) obj->c->close (obj->c, NULL);
   SLfree ((char *)obj);
}

static void chksum_new (char *name)
{
   Chksum_Def_Type *t;
   Chksum_Object_Type *obj;

   t = lookup_chksum (name);
   if (t == NULL)
     return;

   obj = (Chksum_Object_Type *)SLmalloc (sizeof (Chksum_Object_Type));
   if (obj == NULL)
     return;
   memset ((char *)obj, 0, sizeof(SLChksum_Type));

   obj->numrefs = 1;
   if (NULL == (obj->c = t->create (name)))
     {
	SLfree ((char *)obj);
	return;
     }

   (void) chksum_push (obj);
   chksum_free (obj);
}

/* s is assumed to be at least 2*len+1 bytes. */
static void hexify_string (unsigned char *s, unsigned int len)
{
   unsigned char *s0, *s1;

   s0 = s + len;
   s1 = s0 + len;
   *s1-- = 0;
   while (s0 > s)
     {
	unsigned char ch;
	unsigned char buf[3];

	s0--;
	ch = *s0;
	sprintf ((char *)buf, "%02x", ch);
	*s1-- = buf[1];
	*s1-- = buf[0];
     }
}

static void chksum_close (Chksum_Object_Type *obj)
{
   unsigned char *digest;
   unsigned int digest_len;
   SLChksum_Type *c;

   if (NULL == (c = obj->c))
     {
	(void) SLang_push_null ();
	return;
     }

   digest_len = c->digest_len;
   if (NULL == (digest = (unsigned char *)SLmalloc(2*digest_len+1)))
     return;

   if (-1 == c->close (c, digest))
     {
	SLfree ((char *)digest);
	return;
     }
   obj->c = NULL;

   hexify_string (digest, digest_len);

   (void) SLang_push_malloced_string ((char *)digest);
}

static void chksum_accumulate (Chksum_Object_Type *obj, SLang_BString_Type *b)
{
   SLChksum_Type *c;
   SLstrlen_Type len;
   unsigned char *s;

   if (NULL == (c = obj->c))
     {
	SLang_verror (SL_InvalidParm_Error, "Checksum object is invalid");
	return;
     }
   if (NULL == (s = SLbstring_get_pointer (b, &len)))
     return;

   (void) c->accumulate (c, s, len);
}

#define DUMMY_CHKSUM_TYPE ((unsigned int)-1)
static SLang_Intrin_Fun_Type Intrinsics [] =
{
   MAKE_INTRINSIC_1 ("_chksum_new", chksum_new, SLANG_VOID_TYPE, SLANG_STRING_TYPE),
   MAKE_INTRINSIC_2 ("_chksum_accumulate", chksum_accumulate, SLANG_VOID_TYPE, DUMMY_CHKSUM_TYPE, SLANG_BSTRING_TYPE),
   MAKE_INTRINSIC_1 ("_chksum_close", chksum_close, SLANG_VOID_TYPE, DUMMY_CHKSUM_TYPE),
   SLANG_END_INTRIN_FUN_TABLE
};

static void destroy_chksum_type (SLtype type, VOID_STAR ptr)
{
   (void) type;
   chksum_free (*(Chksum_Object_Type **)ptr);
}

static int push_chksum_type (SLtype type, VOID_STAR ptr)
{
   (void) type;
   return chksum_push (*(Chksum_Object_Type **)ptr);
}

static int register_chksum_type (void)
{
   SLang_Class_Type *cl;

   if (Chksum_Type_Id != 0)
     return 0;

   if (NULL == (cl = SLclass_allocate_class ("Chksum_Type")))
     return -1;

   if (-1 == SLclass_set_destroy_function (cl, destroy_chksum_type))
     return -1;

   if (-1 == SLclass_set_push_function (cl, push_chksum_type))
     return -1;

   /* By registering as SLANG_VOID_TYPE, slang will dynamically allocate a
    * type.
    */
   if (-1 == SLclass_register_class (cl, SLANG_VOID_TYPE, sizeof (Chksum_Object_Type *), SLANG_CLASS_TYPE_PTR))
     return -1;

   Chksum_Type_Id = SLclass_get_class_id (cl);

   if (-1 == SLclass_patch_intrin_fun_table1 (Intrinsics, DUMMY_CHKSUM_TYPE, Chksum_Type_Id))
     return -1;

   return 0;
}

int init_chksum_module_ns (char *ns_name)
{
   SLang_NameSpace_Type *ns;

   ns = SLns_create_namespace (ns_name);
   if (ns == NULL)
     return -1;

   if (-1 == register_chksum_type ())
     return -1;

   if (-1 == SLns_add_intrin_fun_table (ns, Intrinsics, NULL))
     return -1;

   return 0;
}

void deinit_chksum_module (void)
{
}
