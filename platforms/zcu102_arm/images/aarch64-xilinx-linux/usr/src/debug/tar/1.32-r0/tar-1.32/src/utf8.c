/* Charset handling for GNU tar.

   Copyright 2004-2019 Free Software Foundation, Inc.

   This file is part of GNU tar.

   GNU tar is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   GNU tar is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <system.h>
#include <quotearg.h>
#include <localcharset.h>
#include "common.h"
#ifdef HAVE_ICONV_H
# include <iconv.h>
#endif

#ifndef ICONV_CONST
# define ICONV_CONST
#endif

#ifndef HAVE_ICONV

# undef iconv_open
# define iconv_open(tocode, fromcode) ((iconv_t) -1)

# undef iconv
# define iconv(cd, inbuf, inbytesleft, outbuf, outbytesleft) (errno = ENOSYS, (size_t) -1)

# undef iconv_close
# define iconv_close(cd) 0

# undef iconv_t
# define iconv_t int

#endif




static iconv_t conv_desc[2] = { (iconv_t) -1, (iconv_t) -1 };

static iconv_t
utf8_init (bool to_utf)
{
  if (conv_desc[(int) to_utf] == (iconv_t) -1)
    {
      if (to_utf)
	conv_desc[(int) to_utf] = iconv_open ("UTF-8", locale_charset ());
      else
	conv_desc[(int) to_utf] = iconv_open (locale_charset (), "UTF-8");
    }
  return conv_desc[(int) to_utf];
}

bool
utf8_convert (bool to_utf, char const *input, char **output)
{
  char ICONV_CONST *ib;
  char *ob, *ret;
  size_t inlen;
  size_t outlen;
  iconv_t cd = utf8_init (to_utf);

  if (cd == 0)
    {
      *output = xstrdup (input);
      return true;
    }
  else if (cd == (iconv_t)-1)
    return false;

  inlen = strlen (input) + 1;
  outlen = inlen * MB_LEN_MAX + 1;
  ob = ret = xmalloc (outlen);
  ib = (char ICONV_CONST *) input;
  /* According to POSIX, "if iconv() encounters a character in the input
     buffer that is valid, but for which an identical character does not
     exist in the target codeset, iconv() shall perform an
     implementation-defined conversion on this character." It will "update
     the variables pointed to by the arguments to reflect the extent of the
     conversion and return the number of non-identical conversions performed".
     On error, it returns -1. 
     In other words, non-zero return always indicates failure, either because
     the input was not fully converted, or because it was converted in a
     non-reversible way.
   */
  if (iconv (cd, &ib, &inlen, &ob, &outlen) != 0)
    {
      free (ret);
      return false;
    }
  *ob = 0;
  *output = ret;
  return true;
}


bool
string_ascii_p (char const *p)
{
  for (; *p; p++)
    if (*p & ~0x7f)
      return false;
  return true;
}
