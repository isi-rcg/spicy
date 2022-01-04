/* Initialization of PA-RISC specific backend library.
   Copyright (C) 2002, 2005, 2006 Red Hat, Inc.
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

#define BACKEND		parisc_
#define RELOC_PREFIX	R_PARISC_
#include "libebl_CPU.h"
#include "libebl_parisc.h"

/* This defines the common reloc hooks based on parisc_reloc.def.  */
#include "common-reloc.c"


const char *
parisc_init (Elf *elf __attribute__ ((unused)),
     GElf_Half machine __attribute__ ((unused)),
     Ebl *eh,
     size_t ehlen)
{
  int pa64 = 0;

  /* Check whether the Elf_BH object has a sufficent size.  */
  if (ehlen < sizeof (Ebl))
    return NULL;

  if (elf) {
    GElf_Ehdr ehdr_mem;
    GElf_Ehdr *ehdr = gelf_getehdr (elf, &ehdr_mem);
    if (ehdr && (ehdr->e_flags & EF_PARISC_WIDE))
      pa64 = 1;
  }
  /* We handle it.  */
  parisc_init_reloc (eh);
  HOOK (eh, reloc_simple_type);
  HOOK (eh, machine_flag_check);
  HOOK (eh, symbol_type_name);
  HOOK (eh, segment_type_name);
  HOOK (eh, section_type_name);
  HOOK (eh, register_info);
  if (pa64)
    eh->return_value_location = parisc_return_value_location_64;
  else
    eh->return_value_location = parisc_return_value_location_32;

  return MODVERSION;
}
