/* Function return value location for ARM EABI.
   Copyright (C) 2009-2010, 2014 Red Hat, Inc.
   This file is part of elfutils.

   This file is free software; you can redistribute it and/or modify
   it under the terms of either

     * the GNU Lesser General Public License as published by the Free
       Software Foundation; either version 3 of the License, or (at
       your option) any later version

   or

     * the GNU General Public License as published by the Free
       Software Foundation; either version 2 of the License, or (at
       your option) any later version

   or both in parallel, as here.

   elfutils is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received copies of the GNU General Public License and
   the GNU Lesser General Public License along with this program.  If
   not, see <http://www.gnu.org/licenses/>.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <assert.h>
#include <dwarf.h>

#define BACKEND arm_
#include "libebl_CPU.h"


/* r0, or pair r0, r1, or aggregate up to r0-r3.  */
static const Dwarf_Op loc_intreg[] =
  {
    { .atom = DW_OP_reg0 }, { .atom = DW_OP_piece, .number = 4 },
    { .atom = DW_OP_reg1 }, { .atom = DW_OP_piece, .number = 4 },
    { .atom = DW_OP_reg2 }, { .atom = DW_OP_piece, .number = 4 },
    { .atom = DW_OP_reg3 }, { .atom = DW_OP_piece, .number = 4 },
  };
#define nloc_intreg	1
#define nloc_intregs(n)	(2 * (n))

/* f1  */ /* XXX TODO: f0 can also have number 96 if program was compiled with -mabi=aapcs */
static const Dwarf_Op loc_fpreg[] =
  {
    { .atom = DW_OP_reg16 },
  };
#define nloc_fpreg  1

/* The return value is a structure and is actually stored in stack space
   passed in a hidden argument by the caller.  But, the compiler
   helpfully returns the address of that space in r0.  */
static const Dwarf_Op loc_aggregate[] =
  {
    { .atom = DW_OP_breg0, .number = 0 }
  };
#define nloc_aggregate 1


static int
arm_return_value_location_ (Dwarf_Die *functypedie, const Dwarf_Op **locp,
		            int soft_float)
{
  /* Start with the function's type, and get the DW_AT_type attribute,
     which is the type of the return value.  */
  Dwarf_Die die_mem, *typedie = &die_mem;
  int tag = dwarf_peeled_die_type (functypedie, typedie);
  if (tag <= 0)
    return tag;

  Dwarf_Word size;
  switch (tag)
    {
    case -1:
      return -1;

    case DW_TAG_subrange_type:
      if (! dwarf_hasattr_integrate (typedie, DW_AT_byte_size))
	{
	  Dwarf_Attribute attr_mem, *attr;
	  attr = dwarf_attr_integrate (typedie, DW_AT_type, &attr_mem);
	  typedie = dwarf_formref_die (attr, &die_mem);
	  tag = DWARF_TAG_OR_RETURN (typedie);
	}
      FALLTHROUGH;

    case DW_TAG_base_type:
    case DW_TAG_enumeration_type:
    case DW_TAG_pointer_type:
    case DW_TAG_ptr_to_member_type:
      {
	Dwarf_Attribute attr_mem;
	if (dwarf_formudata (dwarf_attr_integrate (typedie, DW_AT_byte_size,
						   &attr_mem), &size) != 0)
	  {
	    if (tag == DW_TAG_pointer_type || tag == DW_TAG_ptr_to_member_type)
	      size = 4;
	    else
	      return -1;
	  }
	if (tag == DW_TAG_base_type)
	  {
	    Dwarf_Word encoding;
	    if (dwarf_formudata (dwarf_attr_integrate (typedie, DW_AT_encoding,
				 &attr_mem), &encoding) != 0)
	      return -1;

	    if ((encoding == DW_ATE_float) && !soft_float)
	      {
		*locp = loc_fpreg;
		if (size <= 8)
		  return nloc_fpreg;
		goto aggregate;
	      }
	  }
	if (size <= 16)
	  {
	  intreg:
	    *locp = loc_intreg;
	    return size <= 4 ? nloc_intreg : nloc_intregs ((size + 3) / 4);
	  }

      aggregate:
	/* XXX TODO sometimes aggregates are returned in r0 (-mabi=aapcs) */
	*locp = loc_aggregate;
	return nloc_aggregate;
      }

    case DW_TAG_structure_type:
    case DW_TAG_class_type:
    case DW_TAG_union_type:
    case DW_TAG_array_type:
      if (dwarf_aggregate_size (typedie, &size) == 0
	  && size > 0 && size <= 4)
	goto intreg;
      goto aggregate;
    }

  /* XXX We don't have a good way to return specific errors from ebl calls.
     This value means we do not understand the type, but it is well-formed
     DWARF and might be valid.  */
  return -2;
}

/* return location for -mabi=apcs-gnu -msoft-float */
int
arm_return_value_location_soft (Dwarf_Die *functypedie, const Dwarf_Op **locp)
{
   return arm_return_value_location_ (functypedie, locp, 1);
}

/* return location for -mabi=apcs-gnu -mhard-float (current default) */
int
arm_return_value_location_hard (Dwarf_Die *functypedie, const Dwarf_Op **locp)
{
   return arm_return_value_location_ (functypedie, locp, 0);
}

