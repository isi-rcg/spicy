/* PA-RISC specific symbolic name handling.
   Copyright (C) 2002, 2005 Red Hat, Inc.
   This file is part of Red Hat elfutils.
   Written by Ulrich Drepper <drepper@redhat.com>, 2002.

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

#include <elf.h>
#include <stddef.h>

#define BACKEND		parisc_
#include "libebl_CPU.h"

const char *
parisc_segment_type_name (int segment, char *buf __attribute__ ((unused)),
			size_t len __attribute__ ((unused)))
{
  switch (segment)
    {
    case PT_PARISC_ARCHEXT:
      return "PARISC_ARCHEXT";
    case PT_PARISC_UNWIND:
      return "PARISC_UNWIND";
    default:
      break;
    }
  return NULL;
}

/* Return symbolic representation of symbol type.  */
const char *
parisc_symbol_type_name(int symbol, char *buf __attribute__ ((unused)),
    size_t len __attribute__ ((unused)))
{
	if (symbol == STT_PARISC_MILLICODE)
	  return "PARISC_MILLI";
	return NULL;
}

/* Return symbolic representation of section type.  */
const char *
parisc_section_type_name (int type,
			char *buf __attribute__ ((unused)),
			size_t len __attribute__ ((unused)))
{
  switch (type)
    {
    case SHT_PARISC_EXT:
      return "PARISC_EXT";
    case SHT_PARISC_UNWIND:
      return "PARISC_UNWIND";
    case SHT_PARISC_DOC:
      return "PARISC_DOC";
    }

  return NULL;
}

/* Check whether machine flags are valid.  */
bool
parisc_machine_flag_check (GElf_Word flags)
{
  if (flags &~ (EF_PARISC_TRAPNIL | EF_PARISC_EXT | EF_PARISC_LSB |
	EF_PARISC_WIDE | EF_PARISC_NO_KABP |
	EF_PARISC_LAZYSWAP | EF_PARISC_ARCH))
    return 0;

  GElf_Word arch = flags & EF_PARISC_ARCH;

  return ((arch == EFA_PARISC_1_0) || (arch == EFA_PARISC_1_1) ||
      (arch == EFA_PARISC_2_0));
}

/* Check for the simple reloc types.  */
Elf_Type
parisc_reloc_simple_type (Ebl *ebl __attribute__ ((unused)), int type,
                          int *addsub __attribute__ ((unused)))
{
  switch (type)
    {
    case R_PARISC_DIR64:
    case R_PARISC_SECREL64:
      return ELF_T_XWORD;
    case R_PARISC_DIR32:
    case R_PARISC_SECREL32:
      return ELF_T_WORD;
    default:
      return ELF_T_NUM;
    }
}
