/* Test program for changing data in one section (but not others) with gaps.
   Copyright (C) 2017 Red Hat, Inc.
   This file is part of elfutils.

   This file is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   elfutils is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */


#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include ELFUTILS_HEADER(elf)
#include <gelf.h>


/* Index of last string added.  Returned by add_string ().  */
static size_t stridx = 0;

/* Adds a string and returns the offset in the section.  */
static size_t
add_strtab_entry (Elf_Scn *strtab, const char *str)
{
  size_t lastidx = stridx;
  size_t size = strlen (str) + 1;

  Elf_Data *data = elf_newdata (strtab);
  if (data == NULL)
    {
      printf ("cannot create data SHSTRTAB section: %s\n", elf_errmsg (-1));
      exit (1);
    }

  data->d_buf = (char *) str; /* Discards const, but we will not change. */
  data->d_type = ELF_T_BYTE;
  data->d_size = size;
  data->d_align = 1;
  data->d_version = EV_CURRENT;

  stridx += size;
  printf ("add_string: '%s', stridx: %zd, lastidx: %zd\n",
	  str, stridx, lastidx);
  return lastidx;
}

static Elf_Scn *
create_strtab (Elf *elf)
{
  // Create strtab section.
  Elf_Scn *scn = elf_newscn (elf);
  if (scn == NULL)
    {
      printf ("cannot create strings section: %s\n", elf_errmsg (-1));
      exit (1);
    }

  // Add an empty string to the table as NUL entry for section zero.
  add_strtab_entry (scn, "");

  GElf_Shdr shdr_mem;
  GElf_Shdr *shdr = gelf_getshdr (scn, &shdr_mem);
  if (shdr == NULL)
    {
      printf ("cannot get header for new strtab section: %s\n",
	      elf_errmsg (-1));
      exit (1);
    }

  shdr->sh_type = SHT_STRTAB;
  shdr->sh_flags = 0;
  shdr->sh_addr = 0;
  shdr->sh_link = SHN_UNDEF;
  shdr->sh_info = SHN_UNDEF;
  shdr->sh_addralign = 1;
  shdr->sh_entsize = 0;
  shdr->sh_name = add_strtab_entry (scn, ".strtab");

  // We have to store the section strtab index in the ELF header.
  // So sections have actual names.
  GElf_Ehdr ehdr_mem;
  GElf_Ehdr *ehdr = gelf_getehdr (elf, &ehdr_mem);
  if (ehdr == NULL)
    {
      printf ("cannot get ELF header: %s\n", elf_errmsg (-1));
      exit (1);
    }

  int ndx = elf_ndxscn (scn);
  ehdr->e_shstrndx = ndx;

  if (gelf_update_ehdr (elf, ehdr) == 0)
    {
      printf ("cannot update ELF header: %s\n", elf_errmsg (-1));
      exit (1);
    }

  // Finished strtab section, update the header.
  if (gelf_update_shdr (scn, shdr) == 0)
    {
      printf ("cannot update STRTAB section header: %s\n", elf_errmsg (-1));
      exit (1);
    }

  return scn;
}

static char sec_data[] = { 1, 2, 3, 4, 5 };
static char new_data[] = { 5, 4, 3, 2, 1 };

static void
add_data_section (Elf *elf, Elf_Scn *strtab, const char *sname)
{
  printf ("Add data section %s\n", sname);
  Elf_Scn *scn = elf_newscn (elf);
  if (scn == NULL)
    {
      printf ("cannot create strings section: %s\n", elf_errmsg (-1));
      exit (1);
    }

  GElf_Shdr shdr_mem;
  GElf_Shdr *shdr = gelf_getshdr (scn, &shdr_mem);
  if (shdr == NULL)
    {
      printf ("cannot get header for new %s section: %s\n",
	      sname, elf_errmsg (-1));
      exit (1);
    }

  shdr->sh_type = SHT_PROGBITS;
  shdr->sh_flags = 0;
  shdr->sh_addr = 0;
  shdr->sh_link = SHN_UNDEF;
  shdr->sh_info = SHN_UNDEF;
  shdr->sh_addralign = 128;  // Large alignment to force gap between sections.
  shdr->sh_entsize = 1;
  shdr->sh_name = add_strtab_entry (strtab, sname);

  if (gelf_update_shdr (scn, shdr) == 0)
    {
      printf ("cannot update %s section header: %s\n", sname, elf_errmsg (-1));
      exit (1);
    }

  /* Add some data, but less than alignment. */
  Elf_Data *data = elf_newdata (scn);
  if (data == NULL)
    {
      printf ("cannot update %s section header: %s\n", sname, elf_errmsg (-1));
      exit (1);
    }
  data->d_buf = sec_data;
  data->d_size = 5;
}

static void
check_data (const char *sname, Elf_Data *data, char *buf)
{
  printf ("check data %s [", sname);
  for (int i = 0; i < 5; i++)
    printf ("%d%s", buf[i], i < 4 ? "," : "");
  printf ("]\n");
  if (data == NULL || data->d_buf == NULL)
    {
      printf ("No data in section %s\n", sname);
      exit (1);
    }

  if (data->d_size != 5 || memcmp (data->d_buf, buf, 5) != 0)
    {
      printf ("Wrong data in section %s [", sname);
      for (size_t i = 0; i < data->d_size; i++)
	printf ("%d%s", ((char *)data->d_buf)[i],
		i < data->d_size - 1 ? "," : "");
      printf ("]\n");
      exit(1);
    }
}

static void
check_elf (const char *fname, int class, int use_mmap)
{
  printf ("\nfname: %s\n", fname);
  stridx = 0; // Reset strtab strings index

  int fd = open (fname, O_RDWR | O_CREAT | O_TRUNC, 0666);
  if (fd == -1)
    {
      printf ("cannot open `%s': %s\n", fname, strerror (errno));
      exit (1);
    }

  Elf *elf = elf_begin (fd, use_mmap ? ELF_C_WRITE_MMAP : ELF_C_WRITE, NULL);
  if (elf == NULL)
    {
      printf ("cannot create ELF descriptor: %s\n", elf_errmsg (-1));
      exit (1);
    }

  // Create an ELF header.
  if (gelf_newehdr (elf, class) == 0)
    {
      printf ("cannot create ELF header: %s\n", elf_errmsg (-1));
      exit (1);
    }

  GElf_Ehdr ehdr_mem;
  GElf_Ehdr *ehdr = gelf_getehdr (elf, &ehdr_mem);
  if (ehdr == NULL)
    {
      printf ("cannot get ELF header: %s\n", elf_errmsg (-1));
      exit (1);
    }

  // Initialize header.
  ehdr->e_ident[EI_DATA] = class == ELFCLASS64 ? ELFDATA2LSB : ELFDATA2MSB;
  ehdr->e_ident[EI_OSABI] = ELFOSABI_GNU;
  ehdr->e_type = ET_NONE;
  ehdr->e_machine = EM_X86_64;
  ehdr->e_version = EV_CURRENT;

  if (gelf_update_ehdr (elf, ehdr) == 0)
    {
      printf ("cannot update ELF header: %s\n", elf_errmsg (-1));
      exit (1);
    }

  Elf_Scn *strtab = create_strtab (elf);
  add_data_section (elf, strtab, ".data1");
  add_data_section (elf, strtab, ".data2");
  add_data_section (elf, strtab, ".data3");
  add_data_section (elf, strtab, ".data4");

  // Write everything to disk.
  if (elf_update (elf, ELF_C_WRITE) < 0)
    {
      printf ("failure in elf_update(WRITE): %s\n", elf_errmsg (-1));
      exit (1);
    }

  if (elf_end (elf) != 0)
    {
      printf ("failure in elf_end: %s\n", elf_errmsg (-1));
      exit (1);
    }

  close (fd);

  /* Reread the ELF from disk now.  */
  printf ("Rereading %s\n", fname);
  fd = open (fname, O_RDWR, 0666);
  if (fd == -1)
    {
      printf ("cannot (re)open `%s': %s\n", fname, strerror (errno));
      exit (1);
    }

  elf = elf_begin (fd, use_mmap ? ELF_C_RDWR_MMAP : ELF_C_RDWR, NULL);
  if (elf == NULL)
    {
      printf ("cannot create ELF descriptor read-only: %s\n", elf_errmsg (-1));
      exit (1);
    }

  /* We are going to change some data (in-place), but want the layout
     to stay exactly the same. */
  elf_flagelf (elf, ELF_C_SET, ELF_F_LAYOUT);

  size_t shdrstrndx;
  if (elf_getshdrstrndx (elf, &shdrstrndx) != 0)
    {
      printf ("cannot get shdr str ndx\n");
      exit (1);
    }
  printf ("shdrstrndx: %zd\n", shdrstrndx);

  // Get third data section and change it.
  Elf_Scn *checkscn = NULL;
  Elf_Scn *scn = elf_nextscn (elf, NULL);
  while (scn != NULL)
    {
      GElf_Shdr shdr_mem;
      GElf_Shdr *shdr = gelf_getshdr (scn, &shdr_mem);
      if (shdr == NULL)
	{
	  printf ("cannot get header for section: %s\n", elf_errmsg (-1));
	  exit (1);
	}
      const char *sname = elf_strptr (elf, shdrstrndx, shdr->sh_name);
      if (sname != NULL && strcmp (".data3", sname) == 0)
	checkscn = scn;

      // Get all data, but don't really use it
      // (this triggered the original bug).
      Elf_Data *data = elf_getdata (scn, NULL);
      if (data != NULL && data->d_buf != NULL && data->d_size == 0)
	{
	  printf ("Bad data...n");
	  exit (1);
	}
      scn = elf_nextscn (elf, scn);
    }

  if (checkscn == NULL)
    {
      printf ("ELF file doesn't have a .data3 section\n");
      exit (1);
    }

  Elf_Data *data = elf_getdata (checkscn, NULL);
  check_data (".data3", data, sec_data);
  memcpy (data->d_buf, new_data, 5);
  elf_flagdata (data, ELF_C_SET, ELF_F_DIRTY);

  // Write everything to disk.
  if (elf_update (elf, ELF_C_WRITE) < 0)
    {
      printf ("failure in elf_update(WRITE): %s\n", elf_errmsg (-1));
      exit (1);
    }

  if (elf_end (elf) != 0)
    {
      printf ("failure in elf_end: %s\n", elf_errmsg (-1));
      exit (1);
    }

  close (fd);

  // And read it in one last time.
  printf ("Rereading %s again\n", fname);
  fd = open (fname, O_RDONLY, 0666);
  if (fd == -1)
    {
      printf ("cannot open `%s' read-only: %s\n", fname, strerror (errno));
      exit (1);
    }

  elf = elf_begin (fd, use_mmap ? ELF_C_READ_MMAP : ELF_C_READ, NULL);
  if (elf == NULL)
    {
      printf ("cannot create ELF descriptor read-only: %s\n", elf_errmsg (-1));
      exit (1);
    }

  // Get all .data sections and check them.
  Elf_Scn *scn1 = NULL;
  Elf_Scn *scn2 = NULL;
  Elf_Scn *scn3 = NULL;
  Elf_Scn *scn4 = NULL;
  scn = elf_nextscn (elf, NULL);
  while (scn != NULL)
    {
      GElf_Shdr shdr_mem;
      GElf_Shdr *shdr = gelf_getshdr (scn, &shdr_mem);
      if (shdr == NULL)
	{
	  printf ("cannot get header for section: %s\n", elf_errmsg (-1));
	  exit (1);
	}
      const char *sname = elf_strptr (elf, shdrstrndx, shdr->sh_name);
      if (sname != NULL && strcmp (".data1", sname) == 0)
	scn1 = scn;
      else if (sname != NULL && strcmp (".data2", sname) == 0)
	scn2 = scn;
      else if (sname != NULL && strcmp (".data3", sname) == 0)
	scn3 = scn;
      else if (sname != NULL && strcmp (".data4", sname) == 0)
	scn4 = scn;
      scn = elf_nextscn (elf, scn);
    }

  if (scn1 == NULL)
    {
      printf ("ELF file doesn't have a .data1 section\n");
      exit (1);
    }
  data = elf_getdata (scn1, NULL);
  check_data (".data1", data, sec_data);

  if (scn2 == NULL)
    {
      printf ("ELF file doesn't have a .data2 section\n");
      exit (1);
    }
  data = elf_getdata (scn2, NULL);
  check_data (".data2", data, sec_data);

  if (scn3 == NULL)
    {
      printf ("ELF file doesn't have a .data3 section\n");
      exit (1);
    }
  data = elf_getdata (scn3, NULL);
  check_data (".data3", data, new_data);

  if (scn4 == NULL)
    {
      printf ("ELF file doesn't have a .data4 section\n");
      exit (1);
    }
  data = elf_getdata (scn4, NULL);
  check_data (".data4", data, sec_data);

  if (elf_end (elf) != 0)
    {
      printf ("failure in elf_end: %s\n", elf_errmsg (-1));
      exit (1);
    }

  close (fd);

  unlink (fname);
}

int
main (int argc __attribute__ ((unused)),
      char *argv[] __attribute__ ((unused)))
{
  elf_version (EV_CURRENT);

  elf_fill (0xA);

  check_elf ("fill.elf.32", ELFCLASS32, 0);
  check_elf ("fill.elf.32.mmap", ELFCLASS32, 1);
  check_elf ("fill.elf.64", ELFCLASS64, 0);
  check_elf ("fill.elf.64.mmap", ELFCLASS64, 1);

  return 0;
}
