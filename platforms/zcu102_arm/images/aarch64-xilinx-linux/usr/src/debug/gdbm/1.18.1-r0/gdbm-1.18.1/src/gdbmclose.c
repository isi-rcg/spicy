/* gdbmclose.c - Close a previously opened dbm file. */

/* This file is part of GDBM, the GNU data base manager.
   Copyright (C) 1990-1991, 1993, 2007, 2011, 2013, 2017-2018 Free
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

/* Close the dbm file and free all memory associated with the file DBF.
   Before freeing members of DBF, check and make sure that they were
   allocated.  */

int
gdbm_close (GDBM_FILE dbf)
{
  int index;	/* For freeing the bucket cache. */
  int syserrno;
  
  gdbm_set_errno (dbf, GDBM_NO_ERROR, FALSE);

  if (dbf->desc != -1)
    {
      /* Make sure the database is all on disk. */
      if (dbf->read_write != GDBM_READER)
	gdbm_file_sync (dbf);

      /* Close the file and free all malloced memory. */
#if HAVE_MMAP
      _gdbm_mapped_unmap (dbf);
#endif
      if (dbf->file_locking)
	_gdbm_unlock_file (dbf);

      if (close (dbf->desc))
	GDBM_SET_ERRNO (dbf, GDBM_FILE_CLOSE_ERROR, FALSE);
    }

  syserrno = gdbm_last_syserr (dbf);
  
  gdbm_clear_error (dbf);
  
  free (dbf->name);
  free (dbf->dir);

  if (dbf->bucket_cache != NULL)
    {
      for (index = 0; index < dbf->cache_size; index++)
	{
	  free (dbf->bucket_cache[index].ca_bucket);
	  free (dbf->bucket_cache[index].ca_data.dptr);
	}
      free (dbf->bucket_cache);
    }
  free (dbf->header);
  free (dbf);
  if (gdbm_errno)
    {
      errno = syserrno;
      return -1;
    }
  return 0;
}
