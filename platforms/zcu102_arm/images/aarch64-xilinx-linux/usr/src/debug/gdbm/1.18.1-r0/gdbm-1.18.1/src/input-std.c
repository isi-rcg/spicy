/* This file is part of GDBM, the GNU data base manager.
   Copyright (C) 2016-2018 Free Software Foundation, Inc.

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

static ssize_t
instream_stdin_read (instream_t istr, char *buf, size_t size)
{
  if (istr->in_inter)
    print_prompt_at_bol ();
  if (fgets (buf, size, stdin) == NULL)
    return 0;
  return strlen (buf);
}

static void
instream_stdin_close (instream_t istr)
{
  free (istr);
}

static int
instream_stdin_eq (instream_t a, instream_t b)
{
  return 0;
}

instream_t
instream_stdin_create (void)
{
  struct instream *istr;

  istr = emalloc (sizeof *istr);
  istr->in_name = "stdin";
  istr->in_inter = isatty (fileno (stdin));
  istr->in_read = instream_stdin_read;
  istr->in_close = instream_stdin_close;
  istr->in_eq = instream_stdin_eq;

  return istr;
}

void
input_init (void)
{
  /* nothing */
}

void
input_done (void)
{
  /* nothing */
}

