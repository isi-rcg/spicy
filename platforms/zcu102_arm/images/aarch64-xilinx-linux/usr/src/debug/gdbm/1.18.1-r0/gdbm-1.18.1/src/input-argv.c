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
#include <stdlib.h>

struct instream_argv
{
  struct instream base;  /* Base structure */
  int argc;              /* Number of arguments */
  char **argv;           /* Vector of arguments */
  int idx;               /* Index of the current argument */
  char *cur;             /* Current position in argv[idx] */
  int delim;             /* True if cur points to a delimiter */
  int quote;             /* True if the argument must be quoted */
};

static ssize_t
instream_argv_read (instream_t istr, char *buf, size_t size)
{
  size_t total = 0;
  struct instream_argv *i = (struct instream_argv*)istr;
  char const specials[] = " \"\t\n[]{},=";
  char const escapable[] = "\\\"";
  
  while (total < size)
    {
      if (*i->cur == 0)
	{
	  if (i->quote)
	    {
	      buf[total++] = '"';
	      i->quote = 0;
	      continue;
	    }
	  
	  if (i->idx == i->argc)
	    {
	      if (!i->delim)
		{
		  i->cur = "\n";
		  i->delim = 1;
		}
	      else
		break;
	    }
	  else if (!i->delim)
	    {
	      i->cur = " ";
	      i->delim = 1;
	    }
	  else
	    {
	      size_t len;
	      i->cur = i->argv[i->idx++];
	      i->delim = 0;
	      len = strlen (i->cur);
	      if (len > 1 && i->cur[0] == '"' && i->cur[len-1] == '"')
		i->quote = 0;
	      else if (i->cur[strcspn (i->cur, specials)])
		{
		  buf[total++] = '"';
		  i->quote = 1;
		  continue;
		}
	      else
		i->quote = 0;
	    }
	}
      
      if (strchr (escapable, *i->cur))
	{
	  if (total + 2 > size)
	    break;
	  buf[total++] = '\\';
	  i->cur++;
	}
      buf[total++] = *i->cur++;
    }
  return total;
}

static void
instream_argv_close (instream_t istr)
{
  struct instream_argv *i = (struct instream_argv *)istr;
  free (i);
}

static int
instream_argv_eq (instream_t a, instream_t b)
{
  return 0;
}

instream_t
instream_argv_create (int argc, char **argv)
{
  struct instream_argv *istr;

  istr = emalloc (sizeof *istr);
  istr->base.in_name = "argv";
  istr->base.in_inter = 0;
  istr->base.in_read = instream_argv_read;
  istr->base.in_close = instream_argv_close;
  istr->base.in_eq = instream_argv_eq;
  
  istr->argc = argc;
  istr->argv = argv;
  istr->idx = 0;
  istr->cur = "";
  istr->delim = 1;
  istr->quote = 0;
  
  return (instream_t) istr;
}


