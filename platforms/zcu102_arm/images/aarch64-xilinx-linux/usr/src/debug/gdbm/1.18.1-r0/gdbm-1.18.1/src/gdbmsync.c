/* gdbmsync.c - Sync the disk with the in memory state. */

/* This file is part of GDBM, the GNU data base manager.
   Copyright (C) 1990-1991, 1993, 2007, 2011, 2013, 2016-2018 Free
   Software Foundation, Inc.

   GDBM is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GDBM is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GDBM. If not, see <http://www.gnu.org/licenses/>.   */

/* Include system configuration before all else. */
#include "autoconf.h"

#include "gdbmdefs.h"

/* Make sure the database is all on disk. */

int
gdbm_sync (GDBM_FILE dbf)
{
  /* Return immediately if the database needs recovery */	
  GDBM_ASSERT_CONSISTENCY (dbf, -1);

  /* Initialize the gdbm_errno variable. */
  gdbm_set_errno (dbf, GDBM_NO_ERROR, FALSE);

  /* Do the sync on the file. */
  return gdbm_file_sync (dbf);
}
