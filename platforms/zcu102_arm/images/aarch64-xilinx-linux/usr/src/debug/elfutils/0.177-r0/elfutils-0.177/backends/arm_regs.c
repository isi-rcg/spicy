/* Register names and numbers for ARM DWARF.
   Copyright (C) 2009 Red Hat, Inc.
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

#include <string.h>
#include <stdio.h>
#include <dwarf.h>

#define BACKEND arm_
#include "libebl_CPU.h"

ssize_t
arm_register_info (Ebl *ebl __attribute__ ((unused)),
		   int regno, char *name, size_t namelen,
		   const char **prefix, const char **setname,
		   int *bits, int *type)
{
  if (name == NULL)
    return 320;

  if (regno < 0 || regno > 320 || namelen < 5)
    return -1;

  *prefix = "";
  *bits = 32;
  *type = DW_ATE_signed;
  *setname = "integer";

  switch (regno)
    {
    case 0 ... 9:
      name[0] = 'r';
      name[1] = regno + '0';
      namelen = 2;
      break;

    case 10 ... 12:
      name[0] = 'r';
      name[1] = '1';
      name[2] = regno % 10 + '0';
      namelen = 3;
      break;

    case 13 ... 15:
      *type = DW_ATE_address;
      name[0] = "slp"[regno - 13];
      name[1] = "prc"[regno - 13];
      namelen = 2;
      break;

    case 16 + 0 ... 16 + 7:
      /* AADWARF says that there are no registers in that range,
       * but gcc maps FPA registers here
       */
      regno += 96 - 16;
      FALLTHROUGH;
    case 96 + 0 ... 96 + 7:
      *setname = "FPA";
      *type = DW_ATE_float;
      *bits = 96;
      name[0] = 'f';
      name[1] = regno - 96 + '0';
      namelen = 2;
      break;

    case 64 + 0 ... 64 + 9:
      *setname = "VFP";
      *bits = 32;
      *type = DW_ATE_float;
      name[0] = 's';
      name[1] = regno - 64 + '0';
      namelen = 2;
      break;

    case 64 + 10 ... 64 + 31:
      *setname = "VFP";
      *bits = 32;
      *type = DW_ATE_float;
      name[0] = 's';
      name[1] = (regno - 64) / 10 + '0';
      name[2] = (regno - 64) % 10 + '0';
      namelen = 3;
      break;

    case 104 + 0 ... 104 + 7:
      /* XXX TODO:
       * This can be either intel wireless MMX general purpose/control
       * registers or xscale accumulator, which have different usage.
       * We only have the intel wireless MMX here now.
       * The name needs to be changed for the xscale accumulator too. */
      *setname = "MMX";
      *type = DW_ATE_unsigned;
      *bits = 32;
      memcpy(name, "wcgr", 4);
      name[4] = regno - 104 + '0';
      namelen = 5;
      break;

    case 112 + 0 ... 112 + 9:
      *setname = "MMX";
      *type = DW_ATE_unsigned;
      *bits = 64;
      name[0] = 'w';
      name[1] = 'r';
      name[2] = regno - 112 + '0';
      namelen = 3;
      break;

    case 112 + 10 ... 112 + 15:
      *setname = "MMX";
      *type = DW_ATE_unsigned;
      *bits = 64;
      name[0] = 'w';
      name[1] = 'r';
      name[2] = '1';
      name[3] = regno - 112 - 10 + '0';
      namelen = 4;
      break;

    case 128:
      *setname = "state";
      *type = DW_ATE_unsigned;
      return stpcpy (name, "spsr") + 1 - name;

    case 129:
      *setname = "state";
      *type = DW_ATE_unsigned;
      return stpcpy(name, "spsr_fiq") + 1 - name;

    case 130:
      *setname = "state";
      *type = DW_ATE_unsigned;
      return stpcpy(name, "spsr_irq") + 1 - name;

    case 131:
      *setname = "state";
      *type = DW_ATE_unsigned;
      return stpcpy(name, "spsr_abt") + 1 - name;

    case 132:
      *setname = "state";
      *type = DW_ATE_unsigned;
      return stpcpy(name, "spsr_und") + 1 - name;

    case 133:
      *setname = "state";
      *type = DW_ATE_unsigned;
      return stpcpy(name, "spsr_svc") + 1 - name;

    case 144 ... 150:
      *setname = "integer";
      *type = DW_ATE_signed;
      *bits = 32;
      return sprintf(name, "r%d_usr", regno - 144 + 8) + 1;

    case 151 ... 157:
      *setname = "integer";
      *type = DW_ATE_signed;
      *bits = 32;
      return sprintf(name, "r%d_fiq", regno - 151 + 8) + 1;

    case 158 ... 159:
      *setname = "integer";
      *type = DW_ATE_signed;
      *bits = 32;
      return sprintf(name, "r%d_irq", regno - 158 + 13) + 1;

    case 160 ... 161:
      *setname = "integer";
      *type = DW_ATE_signed;
      *bits = 32;
      return sprintf(name, "r%d_abt", regno - 160 + 13) + 1;

    case 162 ... 163:
      *setname = "integer";
      *type = DW_ATE_signed;
      *bits = 32;
      return sprintf(name, "r%d_und", regno - 162 + 13) + 1;

    case 164 ... 165:
      *setname = "integer";
      *type = DW_ATE_signed;
      *bits = 32;
      return sprintf(name, "r%d_svc", regno - 164 + 13) + 1;

    case 192 ... 199:
     *setname = "MMX";
      *bits = 32;
      *type = DW_ATE_unsigned;
      name[0] = 'w';
      name[1] = 'c';
      name[2] = regno - 192 + '0';
      namelen = 3;
      break;

    case 256 + 0 ... 256 + 9:
      /* XXX TODO: Neon also uses those registers and can contain
       * both float and integers */
      *setname = "VFP";
      *type = DW_ATE_float;
      *bits = 64;
      name[0] = 'd';
      name[1] = regno - 256 + '0';
      namelen = 2;
      break;

    case 256 + 10 ... 256 + 31:
      *setname = "VFP";
      *type = DW_ATE_float;
      *bits = 64;
      name[0] = 'd';
      name[1] = (regno - 256) / 10 + '0';
      name[2] = (regno - 256) % 10 + '0';
      namelen = 3;
      break;

    default:
      *setname = NULL;
      return 0;
    }

  name[namelen++] = '\0';
  return namelen;
}
