/* This file is part of GDBM, the GNU data base manager.
   Copyright (C) 2018 Free Software Foundation, Inc.

   GDBM is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GDBM is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GDBM. If not, see <http://www.gnu.org/licenses/>.    */

#include "gdbmtool.h"

struct instream_file
{
  struct instream base;   /* Base structure */
  FILE *fp;               /* Opened file */
  dev_t dev;              /* Device number */
  ino_t ino;              /* Inode number */
};

static ssize_t
instream_file_read (instream_t istr, char *buf, size_t size)
{
  struct instream_file *file = (struct instream_file *)istr;
  return fread (buf, 1, size, file->fp);
}

static void
instream_file_close (instream_t istr)
{
  struct instream_file *file = (struct instream_file *)istr;
  fclose (file->fp);
  free (file->base.in_name);
  free (file);
}

static int
instream_file_eq (instream_t a, instream_t b)
{
  struct instream_file *file_a = (struct instream_file *)a;
  struct instream_file *file_b = (struct instream_file *)b;
  return file_a->dev == file_b->dev && file_a->ino == file_b->ino;
}

instream_t
instream_file_create (char const *name)
{
  struct instream_file *istr;
  struct stat st;
  FILE *fp;

  if (stat (name, &st))
    {
      terror (_("cannot open `%s': %s"), name, strerror (errno));
      return NULL;
    }
  else if (!S_ISREG (st.st_mode))
    {
      terror (_("%s is not a regular file"), name);
      return NULL;
    }

  fp = fopen (name, "r");
  if (!fp)
    {
      terror (_("cannot open %s for reading: %s"), name,
	      strerror (errno));
      return NULL;
    }
  
  istr = emalloc (sizeof *istr);
  istr->base.in_name = estrdup (name);
  istr->base.in_inter = 0;
  istr->base.in_read = instream_file_read;
  istr->base.in_close = instream_file_close;
  istr->base.in_eq = instream_file_eq;
  istr->fp = fp;
  istr->dev = st.st_dev;
  istr->ino = st.st_ino;

  return (instream_t) istr;
}
