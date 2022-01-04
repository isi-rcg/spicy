/* gdbmexp.c - Export a GDBM database. */

/* This file is part of GDBM, the GNU data base manager.
   Copyright (C) 2007, 2011, 2013, 2016-2018 Free Software Foundation,
   Inc.

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
# include "autoconf.h"
# include <arpa/inet.h>

#ifdef GDBM_EXPORT_18
# define GDBM_SET_ERRNO(dbf, ec, fatal) gdbm_errno = ec
# define GDBM_ASSERT_CONSISTENCY(dbf, val)
#else
# include "gdbmdefs.h"
# include "gdbm.h"
#endif

int
gdbm_export_to_file (GDBM_FILE dbf, FILE *fp)
{
  unsigned long size;
  datum key, nextkey, data;
  const char *header1 = "!\r\n! GDBM FLAT FILE DUMP -- THIS IS NOT A TEXT FILE\r\n! ";
  const char *header2 = "\r\n!\r\n";
  int count = 0;

  /* Return immediately if the database needs recovery */	
  GDBM_ASSERT_CONSISTENCY (dbf, -1);
  
  /* Write out the text header. */
  if (fwrite (header1, strlen (header1), 1, fp) != 1)
    goto write_fail;
  if (fwrite (gdbm_version, strlen (gdbm_version), 1, fp) != 1)
    goto write_fail;
  if (fwrite (header2, strlen (header2), 1, fp) != 1)
    goto write_fail;

  /* For each item in the database, write out a record to the file. */
  key = gdbm_firstkey (dbf);

  while (key.dptr != NULL)
    {
      data = gdbm_fetch (dbf, key);
      if (data.dptr == NULL)
	{
	  if (gdbm_errno != GDBM_NO_ERROR)
	    return -1;
	}
      else
 	{
	  /* Add the data to the new file. */
	  size = htonl (key.dsize);
	  if (fwrite (&size, sizeof (size), 1, fp) != 1)
	    goto write_fail;
	  if (fwrite (key.dptr, key.dsize, 1, fp) != 1)
	    goto write_fail;

	  size = htonl (data.dsize);
	  if (fwrite (&size, sizeof (size), 1, fp) != 1)
	    goto write_fail;
	  if (fwrite (data.dptr, data.dsize, 1, fp) != 1)
	    goto write_fail;
 	}
      
      nextkey = gdbm_nextkey (dbf, key);
      free (key.dptr);
      free (data.dptr);
      key = nextkey;
      
      count++;
    }
  if (gdbm_last_errno (dbf) == GDBM_ITEM_NOT_FOUND)
    {
      gdbm_clear_error (dbf);
      gdbm_errno = GDBM_NO_ERROR;
    }
  else
    return -1;
  
  return count;
  
 write_fail:
  
  GDBM_SET_ERRNO (NULL, GDBM_FILE_WRITE_ERROR, FALSE);
  return -1;
}

int
gdbm_export (GDBM_FILE dbf, const char *exportfile, int flags, int mode)
{
  int nfd, rc;
  FILE *fp;
  
  /* Only support GDBM_WCREAT or GDBM_NEWDB */
  switch (flags)
    {
    case GDBM_WRCREAT:
      nfd = open (exportfile, O_WRONLY | O_CREAT | O_EXCL, mode);
      if (nfd == -1)
	{
	  GDBM_SET_ERRNO (NULL, GDBM_FILE_OPEN_ERROR, FALSE);
	  return -1;
	}
      break;
    case GDBM_NEWDB:
      nfd = open (exportfile, O_WRONLY | O_CREAT | O_TRUNC, mode);
      if (nfd == -1)
	{
	  GDBM_SET_ERRNO (NULL, GDBM_FILE_OPEN_ERROR, FALSE);
	  return -1;
	}
      break;
    default:
#ifdef GDBM_BAD_OPEN_FLAGS
      GDBM_SET_ERRNO (NULL, GDBM_BAD_OPEN_FLAGS, FALSE);
#else
      GDBM_SET_ERRNO (NULL, GDBM_FILE_OPEN_ERROR, FALSE);
#endif
      return -1;
  }

  fp = fdopen (nfd, "w");
  if (!fp)
    {
      close (nfd);
      GDBM_SET_ERRNO (NULL, GDBM_FILE_OPEN_ERROR, FALSE);
      return -1;
    }
	
  rc = gdbm_export_to_file (dbf, fp);
  fclose (fp);
  return rc;
}
