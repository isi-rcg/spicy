/* This file is part of GDBM test suite.
   Copyright (C) 2018 Free Software Foundation, Inc.

   GDBM is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   GDBM is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GDBM. If not, see <http://www.gnu.org/licenses/>.
*/
#include "autoconf.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include "gdbm.h"

int
main (int argc, char **argv)
{
  GDBM_FILE dbf;
  char dbname[] = "junk.gdbm";
  int rc;
  
  assert (argc == 1);
  
  dbf = gdbm_open (dbname, 0, GDBM_NEWDB, 0600, NULL);
  if (!dbf)
    {
      fprintf (stderr, "gdbm_open: %s; %s\n", gdbm_strerror (gdbm_errno),
	       strerror (errno));
      return 1;
    }
  if (close (gdbm_fdesc (dbf)))
    {
      perror ("close");
      return 77;
    }

  if (gdbm_close (dbf))
    {
      fprintf (stderr, "gdbm_close: %s; %s\n", gdbm_strerror (gdbm_errno),
	       strerror (errno));
      return 1;
    }
      
  return 0;
}
