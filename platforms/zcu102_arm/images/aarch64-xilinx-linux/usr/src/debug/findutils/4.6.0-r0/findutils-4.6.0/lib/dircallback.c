/* listfile.c -- run a function in a specific directory
   Copyright (C) 2007, 2008, 2009, 2010, 2011 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* This file was written by James Youngman, based on gnulib'c at-func.c.
 */

/* config.h must be included first. */
#include <config.h>

/* system headers. */
#include <errno.h>
#include <locale.h>
#include <stdarg.h>
#include <sys/stat.h>

/* gnulib headers. */
#include "fcntl--.h"
#include "openat.h"
#include "save-cwd.h"

/* find headers. */
#include "dircallback.h"

int
run_in_dir (const struct saved_cwd *there,
	    int (*callback)(void*), void *usercontext)
{
  int err = -1;
  int saved_errno = 0;
  struct saved_cwd here;
  if (0 == save_cwd (&here))
    {
      if (0 == restore_cwd (there))
	{
	  err = callback(usercontext);
	  saved_errno = (err < 0 ? errno : 0);
	}
      else
	{
	  openat_restore_fail (errno);
	}

      if (restore_cwd (&here) != 0)
	openat_restore_fail (errno);

      free_cwd (&here);
    }
  else
    {
      openat_save_fail (errno);
    }
  if (saved_errno)
    errno = saved_errno;
  return err;
}


int
run_in_dirfd (int dir_fd, int (*callback)(void*), void *usercontext)
{
  if (dir_fd == AT_FDCWD)
    {
      return (*callback)(usercontext);
    }
  else
    {
      struct saved_cwd saved_cwd;
      int saved_errno;
      int err;

      if (save_cwd (&saved_cwd) != 0)
	openat_save_fail (errno);

      if (fchdir (dir_fd) != 0)
	{
	  saved_errno = errno;
	  free_cwd (&saved_cwd);
	  errno = saved_errno;
	  return -1;
	}

      err = (*callback)(usercontext);
      saved_errno = (err < 0 ? errno : 0);

      if (restore_cwd (&saved_cwd) != 0)
	openat_restore_fail (errno);

      free_cwd (&saved_cwd);

      if (saved_errno)
	errno = saved_errno;
      return err;
    }
}
