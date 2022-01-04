/* splitstring.c -- split a const string into fields.
   Copyright (C) 2011 Free Software Foundation, Inc.

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
/*
 * Written by James Youngman.
 */
/* config.h must be included first. */
#include <config.h>

/* system headers. */
#include <stdbool.h>
#include <string.h>

/* gnulib headers would go here. */

/* find headers. */
#include "splitstring.h"

static size_t
field_length (const char *str, const char *separators)
{
  /* if there are no separators, the whole input is one field. */
  if (*separators)
    {
      const char *end = strpbrk (str, separators);
      if (end)
	return end - str;
    }
  return strlen (str);
}


bool
splitstring(const char *s, const char *separators, bool first,
	    size_t *pos, size_t *len)
{
  if (first)
    {
      *pos = 0u;
      *len = 0u;
    }
  else
    {
      *pos += *len;		/* advance to the next field. */
      if (s[*pos])
	++*pos;			/* skip the separator */
      else
	return false;		/* we reached the end. */
    }
  *len = field_length (&s[*pos], separators);
  return true;
}
