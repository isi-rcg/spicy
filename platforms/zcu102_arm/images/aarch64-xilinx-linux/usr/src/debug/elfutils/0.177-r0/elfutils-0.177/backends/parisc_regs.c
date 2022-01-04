/* Register names and numbers for PA-RISC DWARF.
   Copyright (C) 2005, 2006 Red Hat, Inc.
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
#include <dwarf.h>

#define BACKEND parisc_
#include "libebl_CPU.h"

ssize_t
parisc_register_info (Ebl *ebl, int regno, char *name, size_t namelen,
		     const char **prefix, const char **setname,
		     int *bits, int *type)
{
  int pa64 = 0;

  if (ebl->elf) {
    GElf_Ehdr ehdr_mem;
    GElf_Ehdr *ehdr = gelf_getehdr (ebl->elf, &ehdr_mem);
    if (ehdr->e_flags & EF_PARISC_WIDE)
      pa64 = 1;
  }

  int nregs = pa64 ? 127 : 128;

  if (name == NULL)
    return nregs;

  if (regno < 0 || regno >= nregs || namelen < 6)
    return -1;

  *prefix = "%";

  if (regno < 32)
  {
    *setname = "integer";
    *type = DW_ATE_signed;
    if (pa64)
    {
	*bits = 64;
    }
    else
    {
    	*bits = 32;
    }
  }
  else if (regno == 32)
  {
    *setname = "special";
    if (pa64)
    {
	*bits = 6;
    }
    else
    {
    	*bits = 5;
    }
    *type = DW_ATE_unsigned;
  }
  else
  {
    *setname = "FPU";
    *type = DW_ATE_float;
    if (pa64)
    {
	*bits = 64;
    }
    else
    {
    	*bits = 32;
    }
  }

  if (regno < 33) {
    switch (regno)
      {
      case 0 ... 9:
        name[0] = 'r';
        name[1] = regno + '0';
        namelen = 2;
        break;
      case 10 ... 31:
        name[0] = 'r';
        name[1] = regno / 10 + '0';
        name[2] = regno % 10 + '0';
        namelen = 3;
        break;
      case 32:
	*prefix = NULL;
	name[0] = 'S';
	name[1] = 'A';
	name[2] = 'R';
	namelen = 3;
	break;
      }
  }
  else {
    if (pa64 && ((regno - 72) % 2)) {
      *setname = NULL;
      return 0;
    }

    switch (regno)
      {
      case 72 + 0 ... 72 + 11:
        name[0] = 'f';
	name[1] = 'r';
        name[2] = (regno + 8 - 72) / 2 + '0';
        namelen = 3;
        if ((regno + 8 - 72) % 2) {
	  name[3] = 'R';
	  namelen++;
        }
        break;
      case 72 + 12 ... 72 + 55:
        name[0] = 'f';
	name[1] = 'r';
        name[2] = (regno + 8 - 72) / 2 / 10 + '0';
        name[3] = (regno + 8 - 72) / 2 % 10 + '0';
        namelen = 4;
        if ((regno + 8 - 72) % 2) {
	  name[4] = 'R';
	  namelen++;
        }
        break;
      default:
        *setname = NULL;
        return 0;
      }
  }
  name[namelen++] = '\0';
  return namelen;
}
