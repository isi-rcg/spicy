/* safe-atoi.c -- checked string-to-int conversion.
   Copyright (C) 2007, 2010, 2011 Free Software Foundation, Inc.

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
/* config.h must be included first. */
#include <config.h>

/* system headers. */
#include <errno.h>
#include <limits.h>
#include <stdlib.h>

/* gnulib headers. */
#include "error.h"
#include "gettext.h"
#include "quotearg.h"

/* find headers. */
#include "safe-atoi.h"

#if ENABLE_NLS
# include <libintl.h>
# define _(Text) gettext (Text)
#else
# define _(Text) Text
#endif
#ifdef gettext_noop
# define N_(String) gettext_noop (String)
#else
/* See locate.c for explanation as to why not use (String) */
# define N_(String) String
#endif


int
safe_atoi (const char *s, enum quoting_style style)
{
  long lval;
  char *end;

  errno = 0;
  lval = strtol (s, &end, 10);
  if ( (LONG_MAX == lval) || (LONG_MIN == lval) )
    {
      /* max/min possible value, or an error. */
      if (errno == ERANGE)
	{
	  /* too big, or too small. */
	  error (EXIT_FAILURE, errno, "%s", s);
	}
      else
	{
	  /* not a valid number */
	  error (EXIT_FAILURE, errno, "%s", s);
	}
      /* Otherwise, we do a range chack against INT_MAX and INT_MIN
       * below.
       */
    }

  if (lval > INT_MAX || lval < INT_MIN)
    {
      /* The number was in range for long, but not int. */
      errno = ERANGE;
      error (EXIT_FAILURE, errno, "%s", s);
    }
  else if (*end)
    {
      error (EXIT_FAILURE, errno, _("Unexpected suffix %s on %s"),
	     quotearg_n_style (0, style, end),
	     quotearg_n_style (1, style, s));
    }
  else if (end == s)
    {
      error (EXIT_FAILURE, errno, _("Expected an integer: %s"),
	     quotearg_n_style (0, style, s));
    }
  return (int)lval;
}
