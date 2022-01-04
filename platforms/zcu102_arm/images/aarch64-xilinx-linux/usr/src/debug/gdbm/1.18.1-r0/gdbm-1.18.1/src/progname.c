/* This file is part of GDBM, the GNU data base manager.
   Copyright (C) 2011, 2013, 2017-2018 Free Software Foundation, Inc.

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

# include "autoconf.h"
# include "gdbm.h"
# include "gdbmapp.h"
# include <string.h>

const char *progname;

void
set_progname (const char *arg)
{
  const char *p = strrchr (arg, '/');
  if (p)
    ++p;
  else
    p = arg;
  if (strncmp (p, "lt-", 3) == 0)
    p += 3;
  progname = p;
}
