/* Function return value location for Linux/mips ABI.
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

#include <string.h>
#include <assert.h>
#include <dwarf.h>
#include <elf.h>

#include "../libebl/libeblP.h"
#include "../libdw/libdwP.h"

#define BACKEND mips_
#include "libebl_CPU.h"

/* The ABI of the file.  Also see EF_MIPS_ABI2 above. */
#define EF_MIPS_ABI		0x0000F000

/* The original o32 abi. */
#define E_MIPS_ABI_O32          0x00001000

/* O32 extended to work on 64 bit architectures */
#define E_MIPS_ABI_O64          0x00002000

/* EABI in 32 bit mode */
#define E_MIPS_ABI_EABI32       0x00003000

/* EABI in 64 bit mode */
#define E_MIPS_ABI_EABI64       0x00004000

/* All the possible MIPS ABIs. */
enum mips_abi
  {
    MIPS_ABI_UNKNOWN = 0,
    MIPS_ABI_N32,
    MIPS_ABI_O32,
    MIPS_ABI_N64,
    MIPS_ABI_O64,
    MIPS_ABI_EABI32,
    MIPS_ABI_EABI64,
    MIPS_ABI_LAST
  };

/* Find the mips ABI of the current file */
enum mips_abi find_mips_abi(Elf *elf)
{
  GElf_Ehdr ehdr_mem;
  GElf_Ehdr *ehdr = gelf_getehdr (elf, &ehdr_mem);

  if (ehdr == NULL)
    return MIPS_ABI_LAST;

  GElf_Word elf_flags = ehdr->e_flags;

  /* Check elf_flags to see if it specifies the ABI being used.  */
  switch ((elf_flags & EF_MIPS_ABI))
    {
    case E_MIPS_ABI_O32:
      return MIPS_ABI_O32;
    case E_MIPS_ABI_O64:
      return MIPS_ABI_O64;
    case E_MIPS_ABI_EABI32:
      return MIPS_ABI_EABI32;
    case E_MIPS_ABI_EABI64:
      return MIPS_ABI_EABI64;
    default:
      if ((elf_flags & EF_MIPS_ABI2))
	return MIPS_ABI_N32;
      else if ((ehdr->e_ident[EI_CLASS] == ELFCLASS64))
	return MIPS_ABI_N64;
    }

  /* GCC creates a pseudo-section whose name describes the ABI.  */
  size_t shstrndx;
  if (elf_getshdrstrndx (elf, &shstrndx) < 0)
    return MIPS_ABI_LAST;

  const char *name;
  Elf_Scn *scn = NULL;
  while ((scn = elf_nextscn (elf, scn)) != NULL)
    {
      GElf_Shdr shdr_mem;
      GElf_Shdr *shdr = gelf_getshdr (scn, &shdr_mem);
      if (shdr == NULL)
        return MIPS_ABI_LAST;

      name = elf_strptr (elf, shstrndx, shdr->sh_name) ?: "";
      if (strncmp (name, ".mdebug.", 8) != 0)
        continue;

      if (strcmp (name, ".mdebug.abi32") == 0)
        return MIPS_ABI_O32;
      else if (strcmp (name, ".mdebug.abiN32") == 0)
        return MIPS_ABI_N32;
      else if (strcmp (name, ".mdebug.abi64") == 0)
        return MIPS_ABI_N64;
      else if (strcmp (name, ".mdebug.abiO64") == 0)
        return MIPS_ABI_O64;
      else if (strcmp (name, ".mdebug.eabi32") == 0)
        return MIPS_ABI_EABI32;
      else if (strcmp (name, ".mdebug.eabi64") == 0)
        return MIPS_ABI_EABI64;
      else
        return MIPS_ABI_UNKNOWN;
    }

  return MIPS_ABI_UNKNOWN;
}

unsigned int
mips_abi_regsize (enum mips_abi abi)
{
  switch (abi)
    {
    case MIPS_ABI_EABI32:
    case MIPS_ABI_O32:
      return 4;
    case MIPS_ABI_N32:
    case MIPS_ABI_N64:
    case MIPS_ABI_O64:
    case MIPS_ABI_EABI64:
      return 8;
    case MIPS_ABI_UNKNOWN:
    case MIPS_ABI_LAST:
    default:
      return 0;
    }
}


/* $v0 or pair $v0, $v1 */
static const Dwarf_Op loc_intreg_o32[] =
  {
    { .atom = DW_OP_reg2 }, { .atom = DW_OP_piece, .number = 4 },
    { .atom = DW_OP_reg3 }, { .atom = DW_OP_piece, .number = 4 },
  };

static const Dwarf_Op loc_intreg[] =
  {
    { .atom = DW_OP_reg2 }, { .atom = DW_OP_piece, .number = 8 },
    { .atom = DW_OP_reg3 }, { .atom = DW_OP_piece, .number = 8 },
  };
#define nloc_intreg	1
#define nloc_intregpair	4

/* $f0 (float), or pair $f0, $f1 (double).
 * f2/f3 are used for COMPLEX (= 2 doubles) returns in Fortran */
static const Dwarf_Op loc_fpreg_o32[] =
  {
    { .atom = DW_OP_regx, .number = 32 }, { .atom = DW_OP_piece, .number = 4 },
    { .atom = DW_OP_regx, .number = 33 }, { .atom = DW_OP_piece, .number = 4 },
    { .atom = DW_OP_regx, .number = 34 }, { .atom = DW_OP_piece, .number = 4 },
    { .atom = DW_OP_regx, .number = 35 }, { .atom = DW_OP_piece, .number = 4 },
  };

/* $f0, or pair $f0, $f2.  */
static const Dwarf_Op loc_fpreg[] =
  {
    { .atom = DW_OP_regx, .number = 32 }, { .atom = DW_OP_piece, .number = 8 },
    { .atom = DW_OP_regx, .number = 34 }, { .atom = DW_OP_piece, .number = 8 },
  };
#define nloc_fpreg  1
#define nloc_fpregpair 4
#define nloc_fpregquad 8

/* The return value is a structure and is actually stored in stack space
   passed in a hidden argument by the caller.  But, the compiler
   helpfully returns the address of that space in $v0.  */
static const Dwarf_Op loc_aggregate[] =
  {
    { .atom = DW_OP_breg2, .number = 0 }
  };
#define nloc_aggregate 1

/* Test if a struct member is a float */
static int is_float_child(Dwarf_Die *childdie)
{
  /* Test if this is actually a struct member */
  if (dwarf_tag(childdie) != DW_TAG_member)
    return 0;

  /* Get type of member */
  Dwarf_Attribute attr_mem;
  Dwarf_Die child_type_mem;
  Dwarf_Die *child_typedie =
    dwarf_formref_die(dwarf_attr_integrate(childdie,
                                           DW_AT_type,
                                           &attr_mem), &child_type_mem);

  if (dwarf_tag(child_typedie) != DW_TAG_base_type)
    return 0;

  /* Get base subtype */
  Dwarf_Word encoding;
  if (dwarf_formudata (dwarf_attr_integrate (child_typedie,
                                             DW_AT_encoding,
                                             &attr_mem), &encoding) != 0)
    return 0;

  return encoding == DW_ATE_float;
}

/* Returns the number of fpregs which can be returned in the given struct */
static int get_struct_fpregs(Dwarf_Die *structtypedie)
{
  Dwarf_Die child_mem;
  int fpregs = 0;

  /* Get first structure member */
  if (dwarf_child(structtypedie, &child_mem) != 0)
    return 0;

  do
    {
      /* Ensure this register is a float */
      if (!is_float_child(&child_mem))
        return 0;

      fpregs++;
    }
  while (dwarf_siblingof (&child_mem, &child_mem) == 0);

  return fpregs;
}

int
mips_return_value_location (Dwarf_Die *functypedie, const Dwarf_Op **locp)
{
  /* First find the ABI used by the elf object */
  enum mips_abi abi = find_mips_abi(functypedie->cu->dbg->elf);

  /* Something went seriously wrong while trying to figure out the ABI */
  if (abi == MIPS_ABI_LAST)
    return -1;

  /* We couldn't identify the ABI, but the file seems valid */
  if (abi == MIPS_ABI_UNKNOWN)
    return -2;

  /* Can't handle EABI variants */
  if ((abi == MIPS_ABI_EABI32) || (abi == MIPS_ABI_EABI64))
    return -2;

  unsigned int regsize = mips_abi_regsize (abi);
  if (!regsize)
    return -2;

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

  Dwarf_Word size;
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
	if (dwarf_formudata (dwarf_attr_integrate (typedie, DW_AT_byte_size,
					 &attr_mem), &size) != 0)
	  {
	    if (tag == DW_TAG_pointer_type || tag == DW_TAG_ptr_to_member_type)
	      size = regsize;
	    else
	      return -1;
	  }
	if (tag == DW_TAG_base_type)
	  {
	    Dwarf_Word encoding;
	    if (dwarf_formudata (dwarf_attr_integrate (typedie, DW_AT_encoding,
					     &attr_mem), &encoding) != 0)
	      return -1;

#define ABI_LOC(loc, regsize) ((regsize) == 4 ? (loc ## _o32) : (loc))

	    if (encoding == DW_ATE_float)
	      {
		*locp = ABI_LOC(loc_fpreg, regsize);
		if (size <= regsize)
		    return nloc_fpreg;

		if (size <= 2*regsize)
                  return nloc_fpregpair;

		if (size <= 4*regsize && abi == MIPS_ABI_O32)
                  return nloc_fpregquad;

		goto large;
	      }
	  }
	*locp = ABI_LOC(loc_intreg, regsize);
	if (size <= regsize)
	  return nloc_intreg;
	if (size <= 2*regsize)
	  return nloc_intregpair;

	/* Else pass in memory. Shouldn't happen though (at least with gcc) */
	goto large;

    case DW_TAG_structure_type:
    case DW_TAG_class_type:
    case DW_TAG_union_type:
      /* Handle special cases for structures <= 128 bytes in newer ABIs */
      if (abi == MIPS_ABI_EABI32 || abi == MIPS_ABI_EABI64 ||
          abi == MIPS_ABI_N32 || abi == MIPS_ABI_N64)
        {
          if (dwarf_aggregate_size (typedie, &size) == 0 && size <= 16)
            {
              /*
               * Special case in N64 / N32 -
               * structures containing only floats are returned in fp regs.
               * Everything else is returned in integer regs.
               */
              if (tag != DW_TAG_union_type &&
                  (abi == MIPS_ABI_N32 || abi == MIPS_ABI_N64))
                {
                  int num_fpregs = get_struct_fpregs(typedie);
                  if (num_fpregs == 1 || num_fpregs == 2)
                    {
                      *locp = loc_fpreg;
                      if (num_fpregs == 1)
                        return nloc_fpreg;
                      else
                        return nloc_fpregpair;
                    }
                }

              *locp = loc_intreg;
              if (size <= 8)
                return nloc_intreg;
              else
                return nloc_intregpair;
            }
        }

      /* Fallthrough to handle large types */

    case DW_TAG_array_type:
    large:
      /* Return large structures in memory */
      *locp = loc_aggregate;
      return nloc_aggregate;
    }

  /* XXX We don't have a good way to return specific errors from ebl calls.
     This value means we do not understand the type, but it is well-formed
     DWARF and might be valid.  */
  return -2;
}
