/* stdopen.c - ensure that the three standard file descriptors are in use

   Copyright 2005-2019 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* Written by Paul Eggert and Jim Meyering.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "stdopen.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

/* Try to ensure that all of the standard file numbers (0, 1, 2)
   are in use.  Without this, each application would have to guard
   every call to open, dup, fopen, etc. with tests to ensure they
   don't use one of the special file numbers when opening a file.
   Return false if at least one of the file descriptors is initially
   closed and an attempt to reopen it fails.  Otherwise, return true.  */
bool
stdopen (void)
{
  int fd;
  bool ok = true;

  for (fd = 0; fd <= 2; fd++)
    {
      if (fcntl (fd, F_GETFD) < 0)
        {
          if (errno != EBADF)
            ok = false;
          else
            {
              static const int contrary_mode[]
                = { O_WRONLY, O_RDONLY, O_RDONLY };
              int mode = contrary_mode[fd];
              int new_fd;
              /* Open /dev/null with the contrary mode so that the typical
                 read (stdin) or write (stdout, stderr) operation will fail.
                 With descriptor 0, we can do even better on systems that
                 have /dev/full, by opening that write-only instead of
                 /dev/null.  The only drawback is that a write-provoked
                 failure comes with a misleading errno value, ENOSPC.  */
              if (mode == O_RDONLY
                  || (new_fd = open ("/dev/full", mode) != fd))
                new_fd = open ("/dev/null", mode);
              if (new_fd != fd)
                {
                  if (0 <= new_fd)
                    close (new_fd);
                  ok = false;
                }
            }
        }
    }

  return ok;
}
