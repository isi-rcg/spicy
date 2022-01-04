/*
  Copyright (C) 2000, 2002, 2003  Andreas Gruenbacher <agruen@suse.de>

  This program is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 2.1 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __LIBOBJ_H
#define __LIBOBJ_H

#include <stdlib.h>

#include "config.h"

/* Mark library internal functions as hidden */
#if defined(HAVE_VISIBILITY_ATTRIBUTE)
# define hidden __attribute__((visibility("hidden")))
#else
# define hidden /* hidden */
#endif

/* Ugly pointer manipulation */

#ifdef LIBACL_DEBUG
#  define ext2int(T, ext_p) \
	((T##_obj *)__ext2int_and_check(ext_p, T##_MAGIC, #T))
#else
#  define ext2int(T, ext_p) \
	((T##_obj *)__ext2int_and_check(ext_p, T##_MAGIC))
#endif

#define int2ext(int_p) \
	((int_p) ? &(int_p)->i : NULL)
#define new_var_obj_p(T, sz) \
	((T##_obj *)__new_var_obj_p(T##_MAGIC, sizeof(T##_obj) + sz))
#define realloc_var_obj_p(T, p, sz) \
	((T##_obj *)realloc(p, sizeof(T##_obj) + sz))
#define new_obj_p(T) \
	new_var_obj_p(T, 0)
#define new_obj_p_here(T, p) \
	__new_obj_p_here(T##_MAGIC, p)
#define check_obj_p(T, obj_p) \
	((T##_obj *)__check_obj_p((obj_prefix *)(obj_p), T##_MAGIC))
#define free_obj_p(obj_p) \
	(__free_obj_p((obj_prefix *)(obj_p)))


/* prefix for all objects */
/* [Note: p_magic is a long rather than int so that this structure */
/* does not become padded by the compiler on 64-bit architectures] */

typedef struct {
	unsigned long		p_magic:16;
	unsigned long		p_flags:16;
} obj_prefix;

#define pmagic o_prefix.p_magic
#define pflags o_prefix.p_flags

/* magic object values */
#define acl_MAGIC		(0x712C)
#define acl_entry_MAGIC		(0x9D6B)
#define acl_permset_MAGIC	(0x1ED5)
#define qualifier_MAGIC		(0x1C27)
#define string_MAGIC		(0xD5F2)
#define cap_MAGIC		(0x6CA8)

/* object flags */
#define OBJ_MALLOC_FLAG		1

/* object types */
struct string_obj_tag;
typedef struct string_obj_tag string_obj;

/* string object */
struct __string_ext {
	char			s_str[0];
};
struct string_obj_tag {
	obj_prefix		o_prefix;
	struct __string_ext	i;
};

#define sstr i.s_str

/* object creation, destruction, conversion and validation */
void *__new_var_obj_p(int magic, size_t size) hidden;
void __new_obj_p_here(int magic, void *here) hidden;
void __free_obj_p(obj_prefix *obj_p) hidden;
obj_prefix *__check_obj_p(obj_prefix *obj_p, int magic) hidden;
#ifdef LIBACL_DEBUG
obj_prefix *__ext2int_and_check(void *ext_p, int magic,
				const char *typename) hidden;
#else
obj_prefix *__ext2int_and_check(void *ext_p, int magic) hidden;
#endif

#endif /* __LIBOBJ_H */
