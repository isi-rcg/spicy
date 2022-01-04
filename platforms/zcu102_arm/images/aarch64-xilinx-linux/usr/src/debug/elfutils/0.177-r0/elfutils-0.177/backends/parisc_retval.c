/* Function return value location for Linux/PA-RISC ABI.
   Copyright (C) 2005 Red Hat, Inc.
   This file is part of Red Hat elfutils.

   Red Hat elfutils is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by the
   Free Software Foundation; version 2 of the License.

   Red Hat elfutils is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with Red Hat elfutils; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301 USA.

   Red Hat elfutils is an included package of the Open Invention Network.
   An included package of the Open Invention Network is a package for which
   Open Invention Network licensees cross-license their patents.  No patent
   license is granted, either expressly or impliedly, by designation as an
   included package.  Should you wish to participate in the Open Invention
   Network licensing program, please visit www.openinventionnetwork.com
   <http://www.openinventionnetwork.com>.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <assert.h>
#include <dwarf.h>

#define BACKEND parisc_
#include "libebl_CPU.h"
#include "libebl_parisc.h"

/* %r28, or pair %r28, %r29.  */
static const Dwarf_Op loc_intreg32[] =
  {
    { .atom = DW_OP_reg28 }, { .atom = DW_OP_piece, .number = 4 },
    { .atom = DW_OP_reg29 }, { .atom = DW_OP_piece, .number = 4 },
  };

static const Dwarf_Op loc_intreg[] =
  {
    { .atom = DW_OP_reg28 }, { .atom = DW_OP_piece, .number = 8 },
    { .atom = DW_OP_reg29 }, { .atom = DW_OP_piece, .number = 8 },
  };
#define nloc_intreg	1
#define nloc_intregpair	4

/* %fr4L, or pair %fr4L, %fr4R on pa-32 */
static const Dwarf_Op loc_fpreg32[] =
  {
    { .atom = DW_OP_regx, .number = 72 }, { .atom = DW_OP_piece, .number = 4 },
    { .atom = DW_OP_regx, .number = 73 }, { .atom = DW_OP_piece, .number = 4 },
  };
#define nloc_fpreg32 2
#define nloc_fpregpair32 4

/* $fr4 */
static const Dwarf_Op loc_fpreg[] =
  {
    { .atom = DW_OP_regx, .number = 72 },
  };
#define nloc_fpreg  1

#if 0
/* The return value is a structure and is actually stored in stack space
   passed in a hidden argument by the caller. Address of the location is stored
   in %r28 before function call, but it may be changed by function. */
static const Dwarf_Op loc_aggregate[] =
  {
    { .atom = DW_OP_breg28 },
  };
#define nloc_aggregate 1
#endif

static int
parisc_return_value_location_ (Dwarf_Die *functypedie, const Dwarf_Op **locp, int pa64)
{
  Dwarf_Word regsize = pa64 ? 8 : 4;

  /* Start with the function's type, and get the DW_AT_type attribute,
     which is the type of the return value.  */

  Dwarf_Attribute attr_mem;
  Dwarf_Attribute *attr = dwarf_attr_integrate (functypedie, DW_AT_type, &attr_mem);
  if (attr == NULL)
    /* The function has no return value, like a `void' function in C.  */
    return 0;

  Dwarf_Die die_mem;
  Dwarf_Die *typedie = dwarf_formref_die (attr, &die_mem);
  int tag = dwarf_tag (typedie);

  /* Follow typedefs and qualifiers to get to the actual type.  */
  while (tag == DW_TAG_typedef
	 || tag == DW_TAG_const_type || tag == DW_TAG_volatile_type
	 || tag == DW_TAG_restrict_type)
    {
      attr = dwarf_attr_integrate (typedie, DW_AT_type, &attr_mem);
      typedie = dwarf_formref_die (attr, &die_mem);
      tag = dwarf_tag (typedie);
    }

  switch (tag)
    {
    case -1:
      return -1;

    case DW_TAG_subrange_type:
      if (! dwarf_hasattr_integrate (typedie, DW_AT_byte_size))
	{
	  attr = dwarf_attr_integrate (typedie, DW_AT_type, &attr_mem);
	  typedie = dwarf_formref_die (attr, &die_mem);
	  tag = dwarf_tag (typedie);
	}
      /* Fall through.  */

    case DW_TAG_base_type:
    case DW_TAG_enumeration_type:
    case DW_TAG_pointer_type:
    case DW_TAG_ptr_to_member_type:
      {
        Dwarf_Word size;
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

	    if (encoding == DW_ATE_float)
	      {
		if (pa64) {
		  *locp = loc_fpreg;
		  if (size <= 8)
		      return nloc_fpreg;
		}
		else {
		  *locp = loc_fpreg32;
		  if (size <= 4)
		    return nloc_fpreg32;
		  else if (size <= 8)
		    return nloc_fpregpair32;
		}
		goto aggregate;
	      }
	  }
	if (pa64)
	  *locp = loc_intreg;
	else
	  *locp = loc_intreg32;
	if (size <= regsize)
	  return nloc_intreg;
	if (size <= 2 * regsize)
	  return nloc_intregpair;

	/* Else fall through.  */
      }

    case DW_TAG_structure_type:
    case DW_TAG_class_type:
    case DW_TAG_union_type:
    case DW_TAG_array_type:
    aggregate: {
        Dwarf_Word size;
	if (dwarf_aggregate_size (typedie, &size) != 0)
	  return -1;
	if (pa64)
          *locp = loc_intreg;
	else
	  *locp = loc_intreg32;
        if (size <= regsize)
	  return nloc_intreg;
        if (size <= 2 * regsize)
	  return nloc_intregpair;
#if 0
	/* there should be some way to know this location... But I do not see it. */
        *locp = loc_aggregate;
        return nloc_aggregate;
#endif
	/* fall through.  */
      }
    }

  /* XXX We don't have a good way to return specific errors from ebl calls.
     This value means we do not understand the type, but it is well-formed
     DWARF and might be valid.  */
  return -2;
}

int
parisc_return_value_location_32 (Dwarf_Die *functypedie, const Dwarf_Op **locp)
{
  return parisc_return_value_location_ (functypedie, locp, 0);
}

int
parisc_return_value_location_64 (Dwarf_Die *functypedie, const Dwarf_Op **locp)
{
  return parisc_return_value_location_ (functypedie, locp, 1);
}

