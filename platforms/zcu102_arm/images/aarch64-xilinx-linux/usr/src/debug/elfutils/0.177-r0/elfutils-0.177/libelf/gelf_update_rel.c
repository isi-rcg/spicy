/* Update REL relocation information at given index.
   Copyright (C) 2000, 2001, 2002, 2005, 2009, 2014 Red Hat, Inc.
   This file is part of elfutils.
   Written by Ulrich Drepper <drepper@redhat.com>, 2000.

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

#include <gelf.h>
#include <stdlib.h>

#include "libelfP.h"

#define EF_MIPS_ABI	0x0000F000

int
gelf_update_rel (Elf_Data *dst, int ndx, GElf_Rel *src)
{
  Elf_Data_Scn *data_scn = (Elf_Data_Scn *) dst;
  Elf_Scn *scn;
  int result = 0;

  if (dst == NULL)
    return 0;

  if (unlikely (data_scn->d.d_type != ELF_T_REL))
    {
      /* The type of the data better should match.  */
      __libelf_seterrno (ELF_E_DATA_MISMATCH);
      return 0;
    }

  scn = data_scn->s;
  rwlock_wrlock (scn->elf->lock);

  if (scn->elf->class == ELFCLASS32)
    {
      Elf32_Rel *rel;

      /* There is the possibility that the values in the input are
	 too large.  */
      if (unlikely (src->r_offset > 0xffffffffull)
	  || unlikely (GELF_R_SYM (src->r_info) > 0xffffff)
	  || unlikely (GELF_R_TYPE (src->r_info) > 0xff))
	{
	  __libelf_seterrno (ELF_E_INVALID_DATA);
	  goto out;
	}

      /* Check whether we have to resize the data buffer.  */
      if (INVALID_NDX (ndx, Elf32_Rel, &data_scn->d))
	{
	  __libelf_seterrno (ELF_E_INVALID_INDEX);
	  goto out;
	}

      rel = &((Elf32_Rel *) data_scn->d.d_buf)[ndx];

      rel->r_offset = src->r_offset;
      rel->r_info = ELF32_R_INFO (GELF_R_SYM (src->r_info),
				  GELF_R_TYPE (src->r_info));
    }
  else
    {
      GElf_Ehdr hdr;
      GElf_Rel value = *src;

      /* Check whether we have to resize the data buffer.  */
      if (INVALID_NDX (ndx, Elf64_Rel, &data_scn->d))
	{
	  __libelf_seterrno (ELF_E_INVALID_INDEX);
	  goto out;
	}

      if (gelf_getehdr(scn->elf, &hdr) != NULL &&
          hdr.e_ident[EI_DATA] == ELFDATA2LSB &&
          hdr.e_machine == EM_MIPS &&
          (hdr.e_flags & EF_MIPS_ABI) == 0)
        {
          /* Undo the MIPSEL N64 hack from gelf_getrel */
          GElf_Xword r_info = value.r_info;
          value.r_info = (r_info >> 32) |
                         ((r_info << 8) &  0x000000FF00000000) |
                         ((r_info << 24) & 0x0000FF0000000000) |
                         ((r_info << 40) & 0x00FF000000000000) |
                         ((r_info << 56) & 0xFF00000000000000);
        }

      ((Elf64_Rel *) data_scn->d.d_buf)[ndx] = value;
    }

  result = 1;

  /* Mark the section as modified.  */
  scn->flags |= ELF_F_DIRTY;

 out:
  rwlock_unlock (scn->elf->lock);

  return result;
}
