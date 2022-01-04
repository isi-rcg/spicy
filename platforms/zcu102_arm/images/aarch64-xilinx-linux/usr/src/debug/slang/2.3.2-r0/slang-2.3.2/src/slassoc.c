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

/* #define SL_APP_WANTS_FOREACH */
#include "slang.h"
#include "_slang.h"

#define USE_NEW_ANYTYPE_CODE 1

#define MAP_HASH_TO_INDEX(hash, table_len) \
   (unsigned int)((hash)&(table_len-1))

/* Must be a power of 2 */
#define MIN_TABLE_SIZE 512

static SLFUTURE_CONST char *Deleted_Key = "*deleted*";

typedef struct _pSLAssoc_Array_Element_Type
{
   SLFUTURE_CONST char *key;                   /* slstring */
   SLstr_Hash_Type hash;
   SLang_Object_Type value;
}
_pSLAssoc_Array_Element_Type;

typedef struct _pSLang_Assoc_Array_Type
{
   _pSLAssoc_Array_Element_Type *elements;
   unsigned int table_len;
   unsigned int num_occupied;	       /* includes deletions */
   unsigned int num_deleted;
   unsigned int resize_num;	       /* resize when num_occupied hits this number */
   SLang_Object_Type default_value;
#define HAS_DEFAULT_VALUE	1
   unsigned int flags;
   SLtype type;
#if SLANG_OPTIMIZE_FOR_SPEED
   int is_scalar_type;
#endif
   int ref_count;
}
_pSLang_Assoc_Array_Type;

static int HASH_AGAIN (SLCONST char *str, SLstr_Hash_Type hash, unsigned int table_len)
{
   int h;
   (void) table_len; (void) str;
   h = (int)(hash % 311);	       /* 311 should be smaller than MIN_TABLE_SIZE */
   if (0 == (h & 1))
     h++;
   return h;
}

static _pSLAssoc_Array_Element_Type *
find_element (SLang_Assoc_Array_Type *a, char *str, SLstr_Hash_Type hash)
{
   int i, c;
   _pSLAssoc_Array_Element_Type *e, *elements;
   int table_len;

   table_len = a->table_len;
   i = MAP_HASH_TO_INDEX(hash, table_len);
   e = a->elements + i;
   if (e->key == str)
     return e;

   if (e->key == NULL)
     return NULL;

   c = HASH_AGAIN(str, hash, table_len);
   elements = a->elements;

   while (1)
     {
	i -= c;
	if (i < 0)
	  i += table_len;

	e = elements + i;
	if (e->key == str)
	  return e;
	if (e->key == NULL)
	  return NULL;
     }
}

static _pSLAssoc_Array_Element_Type *
find_empty_element (_pSLAssoc_Array_Element_Type *elements, unsigned int table_len,
                    SLCONST char *str, SLstr_Hash_Type hash)
{
   int i, c;
   _pSLAssoc_Array_Element_Type *e;

   i = MAP_HASH_TO_INDEX(hash, table_len);
   e = elements + i;
   if ((e->key == NULL) || (e->key == Deleted_Key))
     return e;

   c = HASH_AGAIN(str, hash, table_len);
   while (1)
     {
	i -= c;
	if (i < 0)
	  i += table_len;

	e = elements + i;
	if ((e->key == NULL) || (e->key == Deleted_Key))
	  return e;
     }
}

static int resize_table (SLang_Assoc_Array_Type *a)
{
   int num_occupied, new_table_len;
   _pSLAssoc_Array_Element_Type *old_es;
   _pSLAssoc_Array_Element_Type *new_es;

   num_occupied = a->num_occupied - a->num_deleted;

   if (num_occupied == 0)
     num_occupied = (MIN_TABLE_SIZE >> 1);

   new_table_len = a->table_len;
   if (new_table_len < MIN_TABLE_SIZE)
     new_table_len = MIN_TABLE_SIZE;

   /* In practice, num_occupied*2 will not overflow because we would be
    * out of memory if would be num_occupied objects stored.
    */
   num_occupied *= 2;
   while (num_occupied > new_table_len)
     {
	new_table_len *= 2;
	if (new_table_len < 0)
	  {
	     SLang_set_error (SL_Malloc_Error);
	     return -1;
	  }
     }

   new_es = (_pSLAssoc_Array_Element_Type *)SLcalloc (new_table_len, sizeof (_pSLAssoc_Array_Element_Type));
   if (new_es == NULL)
     return -1;
   if (NULL != (old_es = a->elements))
     {
	_pSLAssoc_Array_Element_Type *old_e, *old_emax;

	old_e = old_es;
	old_emax = old_e + a->table_len;
	while (old_e < old_emax)
	  {
	     _pSLAssoc_Array_Element_Type *new_e;
	     SLCONST char *key = old_e->key;

	     if ((key == NULL) || (key == Deleted_Key))
	       {
		  old_e++;
		  continue;
	       }

	     /* Cannot fail */
	     new_e = find_empty_element (new_es, new_table_len, key, old_e->hash);
	     *new_e = *old_e;
	     old_e++;
	  }
	SLfree ((char *)old_es);
     }
   a->elements = new_es;
   a->table_len = new_table_len;
   a->num_occupied -= a->num_deleted;
   a->num_deleted = 0;
   a->resize_num = 13*(new_table_len>>4);

   return 0;
}

static void delete_assoc_array (SLang_Assoc_Array_Type *a)
{
   _pSLAssoc_Array_Element_Type *e;
#if SLANG_OPTIMIZE_FOR_SPEED
   int is_scalar_type;
#endif

   if (a == NULL) return;

#if SLANG_OPTIMIZE_FOR_SPEED
   is_scalar_type = a->is_scalar_type;
#endif

   e = a->elements;
   if (e != NULL)
     {
	_pSLAssoc_Array_Element_Type *emax = e + a->table_len;
	while (e < emax)
	  {
	     if ((e->key != NULL) && (e->key != Deleted_Key))
	       {
		  _pSLfree_hashed_string ((char *)e->key, strlen (e->key), e->hash);
#if SLANG_OPTIMIZE_FOR_SPEED
		  if ((is_scalar_type == 0) && (e->value.o_data_type != SLANG_INT_TYPE))
#endif
		    SLang_free_object (&e->value);
	       }
	     e++;
	  }
	SLfree ((char *) a->elements);
     }
   if (a->flags & HAS_DEFAULT_VALUE)
     SLang_free_object (&a->default_value);

   SLfree ((char *) a);
}

static void free_assoc (SLang_Assoc_Array_Type *assoc)
{
   if (assoc == NULL)
     return;

   if (assoc->ref_count > 1)
     {
	assoc->ref_count--;
	return;
     }
   delete_assoc_array (assoc);
}

static SLang_Assoc_Array_Type *alloc_assoc_array (SLtype type, int has_default_value)
{
   SLang_Assoc_Array_Type *a;

   a = (SLang_Assoc_Array_Type *)SLmalloc (sizeof (SLang_Assoc_Array_Type));
   if (a == NULL)
     {
	if (has_default_value)
	  SLdo_pop_n (1);
	return NULL;
     }

   memset ((char *) a, 0, sizeof (SLang_Assoc_Array_Type));
   a->type = type;
#if SLANG_OPTIMIZE_FOR_SPEED
   a->is_scalar_type = (SLANG_CLASS_TYPE_SCALAR == _pSLang_get_class_type (type));
#endif

   if (has_default_value)
     {
	if (
#if USE_NEW_ANYTYPE_CODE
	    ((type != SLANG_ANY_TYPE) && (-1 == SLclass_typecast (type, 1, 0)))
#else
	    (-1 == SLclass_typecast (type, 1, 0))
#endif
	    || (-1 == SLang_pop (&a->default_value)))
	  {
	     SLfree ((char *) a);
	     return NULL;
	  }

	a->flags |= HAS_DEFAULT_VALUE;
     }
   if (-1 == resize_table (a))
     {
	delete_assoc_array (a);
	return NULL;
     }

   a->ref_count = 1;
   return a;
}

static _pSLAssoc_Array_Element_Type *store_object (SLang_Assoc_Array_Type *a, _pSLAssoc_Array_Element_Type *e, SLstr_Type *s, SLstr_Hash_Type hash, SLang_Object_Type *obj)
{
   if ((e != NULL)
       || (NULL != (e = find_element (a, s, hash))))
     {
#if SLANG_OPTIMIZE_FOR_SPEED
	if ((a->is_scalar_type == 0) && (e->value.o_data_type != SLANG_INT_TYPE))
#endif
	  SLang_free_object (&e->value);
     }
   else
     {
	if ((a->num_occupied == a->resize_num)
	    && (-1 == resize_table (a)))
	  return NULL;

	if (NULL == (e = find_empty_element (a->elements, a->table_len, s, hash)))
	  return NULL;
	if (e->key == Deleted_Key)
	  a->num_deleted--;
	else
	  a->num_occupied++;

	if (NULL == (e->key = _pSLstring_dup_hashed_string (s, hash)))
	  return NULL;

	e->hash = hash;
     }
   e->value = *obj;
   return e;
}

static int pop_assoc (SLang_Assoc_Array_Type **assoc)
{
   if (-1 == SLclass_pop_ptr_obj (SLANG_ASSOC_TYPE, (VOID_STAR *) assoc))
     {
        *assoc = NULL;
        return -1;
     }
   return 0;
}

static int pop_index (unsigned int num_indices,
                      SLang_Assoc_Array_Type **ap,
                      SLstr_Type **strp, SLstr_Hash_Type *hashp)
{

   if (-1 == pop_assoc (ap))
     {
	*strp = NULL;
	return -1;
     }

   if ((num_indices != 1)
       || (-1 == SLang_pop_slstring (strp)))
     {
	_pSLang_verror (SL_NOT_IMPLEMENTED,
		      "Assoc_Type arrays require a single string index");
	free_assoc (*ap);
	*ap = NULL;
	*strp = NULL;
	return -1;
     }

   *hashp = _pSLstring_get_hash (*strp);

   return 0;
}

static int push_assoc_element (SLang_Assoc_Array_Type *a, SLstr_Type *str, SLstr_Hash_Type hash)
{
   _pSLAssoc_Array_Element_Type *e = find_element (a, str, hash);
   SLang_Object_Type *obj;

   if (e == NULL)
     {
	if (a->flags & HAS_DEFAULT_VALUE)
	  obj = &a->default_value;
	else
	  {
	     _pSLang_verror (SL_INTRINSIC_ERROR,
			   "No such element in Assoc Array: %s", str);
	     return -1;
	  }
     }
   else
     obj = &e->value;

#if SLANG_OPTIMIZE_FOR_SPEED
   if (a->is_scalar_type)
     return SLang_push (obj);
   else
#endif
     return _pSLpush_slang_obj (obj);
}

int _pSLassoc_aget (SLtype type, unsigned int num_indices)
{
   SLstr_Hash_Type hash;
   SLstr_Type *str;
   SLang_Assoc_Array_Type *a;
   int ret;

   (void) type;

   if (-1 == pop_index (num_indices, &a, &str, &hash))
     return -1;

   ret = push_assoc_element (a, str, hash);

   _pSLang_free_slstring (str);
   free_assoc (a);
   return ret;
}

_INLINE_
static _pSLAssoc_Array_Element_Type *
assoc_aput (SLang_Assoc_Array_Type *a, _pSLAssoc_Array_Element_Type *e,
	    SLstr_Type *str, SLstr_Hash_Type hash)
{
   SLang_Object_Type obj;

   if (-1 == SLang_pop (&obj))
     return NULL;

   if ((obj.o_data_type != a->type)
#if USE_NEW_ANYTYPE_CODE
       && (a->type != SLANG_ANY_TYPE)
#endif
      )
     {
	(void) SLang_push (&obj);
	if ((-1 == SLclass_typecast (a->type, 1, 0))
	    || (-1 == SLang_pop (&obj)))
	  return NULL;
     }

   if (NULL == (e = store_object (a, e, str, hash, &obj)))
     SLang_free_object (&obj);

   return e;
}

int _pSLassoc_aput (SLtype type, unsigned int num_indices)
{
   SLstr_Type *str;
   SLang_Assoc_Array_Type *a;
   int ret;
   SLstr_Hash_Type hash;

   (void) type;

   if (-1 == pop_index (num_indices, &a, &str, &hash))
     return -1;

   if (NULL == assoc_aput (a, NULL, str, hash))
     ret = -1;
   else
     ret = 0;

   _pSLang_free_slstring (str);
   free_assoc (a);

   return ret;
}

int _pSLassoc_inc_value (unsigned int num_indices, int inc)
{
   SLstr_Hash_Type hash;
   SLstr_Type *str;
   _pSLAssoc_Array_Element_Type *e;
   SLang_Assoc_Array_Type *a;
   SLang_Object_Type *objp;
   SLang_Object_Type inc_obj;
   int ret;

   if (-1 == pop_index (num_indices, &a, &str, &hash))
     return -1;

   e = find_element (a, str, hash);

   ret = -1;

   if (e == NULL)
     {
	if (a->flags & HAS_DEFAULT_VALUE)
	  {
	     if (-1 == _pSLpush_slang_obj (&a->default_value))
	       goto free_and_return;
	  }
	else
	  {
	     _pSLang_verror (SL_INTRINSIC_ERROR,
			   "No such element in Assoc Array: %s", str);
	     goto free_and_return;
	  }

	if (NULL == (e = assoc_aput (a, e, str, hash)))
	  goto free_and_return;
     }

   objp = &e->value;

   if (objp->o_data_type == SLANG_INT_TYPE)
     {
	ret = 0;
	objp->v.int_val += inc;
	goto free_and_return;
     }

   inc_obj.o_data_type = SLANG_INT_TYPE;
   inc_obj.v.int_val = inc;

   if ((-1 == _pSLang_do_binary_ab (SLANG_PLUS, objp, &inc_obj))
       || (NULL == assoc_aput (a, e, str, hash)))
     goto free_and_return;

   ret = 0;
   /* drop */

free_and_return:

   _pSLang_free_slstring (str);
   free_assoc (a);
   return ret;
}

static int assoc_anew (SLtype type, unsigned int num_dims)
{
   SLang_Assoc_Array_Type *a;
   int has_default_value;

   has_default_value = 0;
   switch (num_dims)
     {
      case 0:
	type = SLANG_ANY_TYPE;
	break;
      case 2:
	if (-1 == SLreverse_stack (2))
	  return -1;
	has_default_value = 1;
	/* drop */
      case 1:
	if (0 == SLang_pop_datatype (&type))
	  break;
	num_dims--;
	/* drop */
      default:
	SLdo_pop_n (num_dims);
	_pSLang_verror (SL_SYNTAX_ERROR, "Usage: Assoc_Type [DataType_Type]");
	return -1;
     }

   a = alloc_assoc_array (type, has_default_value);
   if (a == NULL)
     return -1;

   return SLang_push_assoc (a, 1);
}

static void assoc_get_keys (SLang_Assoc_Array_Type *a)
{
   SLang_Array_Type *at;
   SLindex_Type i, num;
   char **data;
   _pSLAssoc_Array_Element_Type *e, *emax;

   /* Note: If support for threads is added, then we need to modify this
    * algorithm to prevent another thread from modifying the array.
    * However, that should be handled in inner_interp.
    */
   num = a->num_occupied - a->num_deleted;

   if (NULL == (at = SLang_create_array (SLANG_STRING_TYPE, 0, NULL, &num, 1)))
     return;

   data = (char **)at->data;

   e = a->elements;
   emax = e + a->table_len;

   i = 0;
   while (e < emax)
     {
	if ((e->key != NULL) && (e->key != Deleted_Key))
	  {
	     /* Next cannot fail because it is an slstring */
	     data [i] = _pSLstring_dup_hashed_string (e->key, e->hash);
	     i++;
	  }
	e++;
     }
   (void) SLang_push_array (at, 1);
}

static int
transfer_element (SLang_Class_Type *cl, VOID_STAR dest_data,
		  SLang_Object_Type *obj)
{
   VOID_STAR src_data;

#if USE_NEW_ANYTYPE_CODE
   if (cl->cl_data_type == SLANG_ANY_TYPE)
     {
	SLang_Any_Type *any;

	if ((-1 == _pSLpush_slang_obj (obj))
	    || (-1 == SLang_pop_anytype (&any)))
	  return -1;

	*(SLang_Any_Type **)dest_data = any;
	return 0;
     }
#endif
   /* Optimize for scalar */
   if (cl->cl_class_type == SLANG_CLASS_TYPE_SCALAR)
     {
	memcpy ((char *) dest_data, (char *)&obj->v, cl->cl_sizeof_type);
	return 0;
     }

   src_data = _pSLclass_get_ptr_to_value (cl, obj);

   if (-1 == (*cl->cl_acopy) (cl->cl_data_type, src_data, dest_data))
     return -1;

   return 0;
}

static void assoc_get_values (SLang_Assoc_Array_Type *a)
{
   SLang_Array_Type *at;
   SLindex_Type num;
   char *dest_data;
   SLtype type;
   SLang_Class_Type *cl;
   unsigned int sizeof_type;
   _pSLAssoc_Array_Element_Type *e, *emax;

   /* Note: If support for threads is added, then we need to modify this
    * algorithm to prevent another thread from modifying the array.
    * However, that should be handled in inner_interp.
    */
   num = a->num_occupied - a->num_deleted;
   type = a->type;

   cl = _pSLclass_get_class (type);
   sizeof_type = cl->cl_sizeof_type;

   if (NULL == (at = SLang_create_array (type, 0, NULL, &num, 1)))
     return;

   dest_data = (char *)at->data;

   e = a->elements;
   emax = e + a->table_len;

   while (e < emax)
     {
	if ((e->key != NULL) && (e->key != Deleted_Key))
	  {
	     if (-1 == transfer_element (cl, (VOID_STAR) dest_data, &e->value))
	       {
		  SLang_free_array (at);
		  return;
	       }
	     dest_data += sizeof_type;
	  }
	e++;
     }
   (void) SLang_push_array (at, 1);
}

static int assoc_key_exists (SLang_Assoc_Array_Type *a, char *key)
{
   return (NULL != find_element (a, key, SLcompute_string_hash (key)));
}

static void assoc_delete_key (SLang_Assoc_Array_Type *a, char *key)
{
   _pSLAssoc_Array_Element_Type *e;

   e = find_element (a, key, _pSLstring_get_hash (key));
   if (e == NULL)
     return;

   _pSLang_free_slstring ((char *) e->key);
   SLang_free_object (&e->value);
   e->key = Deleted_Key;
   a->num_deleted++;
}

#define A SLANG_ASSOC_TYPE
#define S SLANG_STRING_TYPE
static SLang_Intrin_Fun_Type Assoc_Table [] =
{
   MAKE_INTRINSIC_1("assoc_get_keys", assoc_get_keys, SLANG_VOID_TYPE, A),
   MAKE_INTRINSIC_1("assoc_get_values", assoc_get_values, SLANG_VOID_TYPE, A),
   MAKE_INTRINSIC_2("assoc_key_exists", assoc_key_exists, SLANG_INT_TYPE, A, S),
   MAKE_INTRINSIC_2("assoc_delete_key", assoc_delete_key, SLANG_VOID_TYPE, A, S),

   SLANG_END_INTRIN_FUN_TABLE
};
#undef A
#undef S

static int assoc_length (SLtype type, VOID_STAR v, SLuindex_Type *len)
{
   SLang_Assoc_Array_Type *a;

   (void) type;
   a = *(SLang_Assoc_Array_Type **) v;
   *len = a->num_occupied - a->num_deleted;
   return 0;
}

struct _pSLang_Foreach_Context_Type
{
   SLang_Assoc_Array_Type *a;
   unsigned int next_hash_index;
#define CTX_WRITE_KEYS		1
#define CTX_WRITE_VALUES	2
   unsigned char flags;
#if SLANG_OPTIMIZE_FOR_SPEED
   int is_scalar;
#endif
};

static SLang_Foreach_Context_Type *
cl_foreach_open (SLtype type, unsigned int num)
{
   SLang_Foreach_Context_Type *c;
   SLang_Assoc_Array_Type *assoc;
   unsigned char flags;

   (void) type;

   if (-1 == pop_assoc (&assoc))
     return NULL;

   flags = 0;

   while (num--)
     {
	char *s;

	if (-1 == SLang_pop_slstring (&s))
	  {
	     free_assoc (assoc);
	     return NULL;
	  }

	if (0 == strcmp (s, "keys"))
	  flags |= CTX_WRITE_KEYS;
	else if (0 == strcmp (s, "values"))
	  flags |= CTX_WRITE_VALUES;
	else
	  {
	     _pSLang_verror (SL_NOT_IMPLEMENTED,
			   "using '%s' not supported by SLassoc_Type",
			   s);
	     _pSLang_free_slstring (s);
	     free_assoc (assoc);
	     return NULL;
	  }

	_pSLang_free_slstring (s);
     }

   if (NULL == (c = (SLang_Foreach_Context_Type *) SLmalloc (sizeof (SLang_Foreach_Context_Type))))
     {
	free_assoc (assoc);
	return NULL;
     }

   memset ((char *) c, 0, sizeof (SLang_Foreach_Context_Type));

   c->a = assoc;

   if (flags == 0) flags = CTX_WRITE_VALUES|CTX_WRITE_KEYS;

   c->flags = flags;
#if SLANG_OPTIMIZE_FOR_SPEED
   c->is_scalar = (SLANG_CLASS_TYPE_SCALAR == _pSLang_get_class_type (c->a->type));
#endif
   return c;
}

static void cl_foreach_close (SLtype type, SLang_Foreach_Context_Type *c)
{
   (void) type;
   if (c == NULL) return;
   free_assoc (c->a);
   SLfree ((char *) c);
}

static int cl_foreach (SLtype type, SLang_Foreach_Context_Type *c)
{
   SLang_Assoc_Array_Type *a;
   _pSLAssoc_Array_Element_Type *e, *emax;

   (void) type;

   if (c == NULL)
     return -1;

   a = c->a;

   e = a->elements + c->next_hash_index;
   emax = a->elements + a->table_len;
   while (1)
     {
	if (e == emax)
	  return 0;

	if ((e->key != NULL) && (e->key != Deleted_Key))
	  break;

	e++;
     }

   c->next_hash_index = (e - a->elements) + 1;

   if ((c->flags & CTX_WRITE_KEYS)
       && (-1 == SLang_push_string (e->key)))
     return -1;

   if (c->flags & CTX_WRITE_VALUES)
     {
#if SLANG_OPTIMIZE_FOR_SPEED
	if (c->is_scalar)
	  {
	     if (-1 == SLang_push (&e->value))
	       return -1;
	  }
	else
#endif
	  if (-1 == _pSLpush_slang_obj (&e->value))
	    return -1;
     }

   /* keep going */
   return 1;
}

static void assoc_destroy (SLtype type, VOID_STAR ptr)
{
   (void) type;
   free_assoc (*(SLang_Assoc_Array_Type **) ptr);
}

static int assoc_push (SLtype type, VOID_STAR ptr)
{
   (void) type;
   return SLang_push_assoc (*(SLang_Assoc_Array_Type **) ptr, 0);
}


int SLang_init_slassoc (void)
{
   SLang_Class_Type *cl;

   if (SLclass_is_class_defined (SLANG_ASSOC_TYPE))
     return 0;

   if (NULL == (cl = SLclass_allocate_class ("Assoc_Type")))
     return -1;

   (void) SLclass_set_destroy_function (cl, assoc_destroy);
   (void) SLclass_set_push_function (cl, assoc_push);

   (void) SLclass_set_aput_function (cl, _pSLassoc_aput);
   (void) SLclass_set_aget_function (cl, _pSLassoc_aget);
   (void) SLclass_set_anew_function (cl, assoc_anew);
   cl->cl_length = assoc_length;
   cl->cl_foreach_open = cl_foreach_open;
   cl->cl_foreach_close = cl_foreach_close;
   cl->cl_foreach = cl_foreach;

   cl->is_container = 1;
   if (-1 == SLclass_register_class (cl, SLANG_ASSOC_TYPE, sizeof (SLang_Assoc_Array_Type), SLANG_CLASS_TYPE_PTR))
     return -1;

   if (-1 == SLadd_intrin_fun_table (Assoc_Table, "__SLASSOC__"))
     return -1;

   return 0;
}

SLang_Assoc_Array_Type *SLang_create_assoc (SLtype type, int has_default_value)
{
   if (type == SLANG_VOID_TYPE) type = SLANG_ANY_TYPE;
   return alloc_assoc_array (type, has_default_value);
}

int SLang_assoc_put (SLang_Assoc_Array_Type *assoc, SLstr_Type *key)
{
   SLstr_Hash_Type hash = _pSLstring_get_hash (key);

   if (NULL == assoc_aput (assoc, NULL, key, hash))
     return -1;

   return 0;
}

int SLang_assoc_get (SLang_Assoc_Array_Type *assoc, SLstr_Type *key, SLtype *typep)
{
   int type;
   SLstr_Hash_Type hash = _pSLstring_get_hash (key);

   if ((-1 == push_assoc_element (assoc, key, hash))
       || (-1 == (type = SLang_peek_at_stack ())))
     return -1;

   if (typep != NULL)
     *typep = (SLtype) type;

   return 0;
}

int SLang_push_assoc (SLang_Assoc_Array_Type *assoc, int free_flag)
{
   if (assoc == NULL)
     return SLang_push_null ();

   /* SLclass_push_ptr_obj does not do memory management */
   if (-1 == SLclass_push_ptr_obj (SLANG_ASSOC_TYPE, (VOID_STAR)assoc))
     {
	if (free_flag) free_assoc (assoc);
	return -1;
     }

   if (free_flag == 0)
     assoc->ref_count++;

   return 0;
}

int SLang_pop_assoc (SLang_Assoc_Array_Type **assoc)
{
   return SLclass_pop_ptr_obj (SLANG_ASSOC_TYPE, (VOID_STAR *) assoc);
}

void SLang_free_assoc (SLang_Assoc_Array_Type *assoc)
{
   free_assoc (assoc);
}

int SLang_assoc_key_exists (SLang_Assoc_Array_Type *a, SLstr_Type *key)
{
   return assoc_key_exists(a, key);
}

