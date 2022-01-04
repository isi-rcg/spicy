/* mbox-util.c - Mail address helper functions
 * Copyright (C) 1998-2010 Free Software Foundation, Inc.
 * Copyright (C) 1998-2015 Werner Koch
 *
 * This file is part of GnuPG.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, see <https://gnu.org/licenses/>.
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

/* NB: This code has been taken from GnuPG.  Please keep it in sync
 * with GnuPG.  */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "mbox-util.h"

/* Lowercase all ASCII characters in STRING.  */
static char *
ascii_strlwr (char *string)
{
  char *p;

  for (p = string; *p; p++ )
    if (!(*p & ~0x7f) && *p >= 'A' && *p <= 'Z')
      *p |= 0x20;

  return string;
}


static int
string_count_chr (const char *string, int c)
{
  int count;

  for (count=0; *string; string++ )
    if ( *string == c )
      count++;
  return count;
}

static int
mem_count_chr (const void *buffer, int c, size_t length)
{
  const char *s = buffer;
  int count;

  for (count=0; length; length--, s++)
    if (*s == c)
      count++;
  return count;
}


/* This is a case-sensitive version of our memistr.  I wonder why no
   standard function memstr exists but I better do not use the name
   memstr to avoid future conflicts.  */
static const char *
my_memstr (const void *buffer, size_t buflen, const char *sub)
{
  const unsigned char *buf = buffer;
  const unsigned char *t = (const unsigned char *)buf;
  const unsigned char *s = (const unsigned char *)sub;
  size_t n = buflen;

  for ( ; n ; t++, n-- )
    {
      if (*t == *s)
        {
          for (buf = t++, buflen = n--, s++; n && *t ==*s; t++, s++, n--)
            ;
          if (!*s)
            return (const char*)buf;
          t = (const unsigned char *)buf;
          s = (const unsigned char *)sub ;
          n = buflen;
	}
    }
  return NULL;
}



static int
string_has_ctrl_or_space (const char *string)
{
  for (; *string; string++ )
    if (!(*string & 0x80) && *string <= 0x20)
      return 1;
  return 0;
}


/* Return true if STRING has two consecutive '.' after an '@'
   sign.  */
static int
has_dotdot_after_at (const char *string)
{
  string = strchr (string, '@');
  if (!string)
    return 0; /* No at-sign.  */
  string++;
  return !!strstr (string, "..");
}


/* Check whether BUFFER has characters not valid in an RFC-822
   address.  LENGTH gives the length of BUFFER.

   To cope with OpenPGP we ignore non-ascii characters so that for
   example umlauts are legal in an email address.  An OpenPGP user ID
   must be utf-8 encoded but there is no strict requirement for
   RFC-822.  Thus to avoid IDNA encoding we put the address verbatim
   as utf-8 into the user ID under the assumption that mail programs
   handle IDNA at a lower level and take OpenPGP user IDs as utf-8.
   Note that we can't do an utf-8 encoding checking here because in
   keygen.c this function is called with the native encoding and
   native to utf-8 encoding is only done later.  */
static int
has_invalid_email_chars (const void *buffer, size_t length)
{
  const unsigned char *s = buffer;
  int at_seen=0;
  const char *valid_chars=
    "01234567890_-.abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

  for ( ; length && *s; length--, s++ )
    {
      if ((*s & 0x80))
        continue; /* We only care about ASCII.  */
      if (*s == '@')
        at_seen=1;
      else if (!at_seen && !(strchr (valid_chars, *s)
                             || strchr ("!#$%&'*+/=?^`{|}~", *s)))
        return 1;
      else if (at_seen && !strchr (valid_chars, *s))
        return 1;
    }
  return 0;
}


/* Same as is_valid_mailbox (see below) but operates on non-nul
   terminated buffer.  */
static int
is_valid_mailbox_mem (const void *name_arg, size_t namelen)
{
  const char *name = name_arg;

  return !( !name
            || !namelen
            || has_invalid_email_chars (name, namelen)
            || mem_count_chr (name, '@', namelen) != 1
            || *name == '@'
            || name[namelen-1] == '@'
            || name[namelen-1] == '.'
            || my_memstr (name, namelen, ".."));
}


/* Check whether NAME represents a valid mailbox according to
   RFC822. Returns true if so. */
int
_gpgme_is_valid_mailbox (const char *name)
{
  return name? is_valid_mailbox_mem (name, strlen (name)) : 0;
}


/* Return the mailbox (local-part@domain) form a standard user id.
   All plain ASCII characters in the result are converted to
   lowercase.  Caller must free the result.  Returns NULL if no valid
   mailbox was found (or we are out of memory). */
char *
_gpgme_mailbox_from_userid (const char *userid)
{
  const char *s, *s_end;
  size_t len;
  char *result = NULL;

  s = strchr (userid, '<');
  if (s)
    {
      /* Seems to be a standard user id.  */
      s++;
      s_end = strchr (s, '>');
      if (s_end && s_end > s)
        {
          len = s_end - s;
          result = malloc (len + 1);
          if (!result)
            return NULL; /* Ooops - out of core.  */
          strncpy (result, s, len);
          result[len] = 0;
          /* Apply some basic checks on the address.  We do not use
             is_valid_mailbox because those checks are too strict.  */
          if (string_count_chr (result, '@') != 1  /* Need exactly one '@.  */
              || *result == '@'           /* local-part missing.  */
              || result[len-1] == '@'     /* domain missing.  */
              || result[len-1] == '.'     /* ends with a dot.  */
              || string_has_ctrl_or_space (result)
              || has_dotdot_after_at (result))
            {
              free (result);
              result = NULL;
              errno = EINVAL;
            }
        }
      else
        errno = EINVAL;
    }
  else if (_gpgme_is_valid_mailbox (userid))
    {
      /* The entire user id is a mailbox.  Return that one.  Note that
         this fallback method has some restrictions on the valid
         syntax of the mailbox.  However, those who want weird
         addresses should know about it and use the regular <...>
         syntax.  */
      result = strdup (userid);
    }
  else
    errno = EINVAL;

  return result? ascii_strlwr (result): NULL;
}


/* /\* Check whether UID is a valid standard user id of the form */
/*      "Heinrich Heine <heinrichh@duesseldorf.de>" */
/*    and return true if this is the case. *\/ */
/* int */
/* is_valid_user_id (const char *uid) */
/* { */
/*   if (!uid || !*uid) */
/*     return 0; */

/*   return 1; */
/* } */


/*
 * Exported public API
 */


/* Return the mail address ("addr-spec" as per RFC-5322) from a string
 * which is assumed to be an user id ("address" in RFC-5322).  All
 * plain ASCII characters (those with bit 7 cleared) in the result
 * are converted to lowercase.  Caller must free the result using
 * gpgme_free.  Returns NULL if no valid address was found (in which
 * case ERRNO is set to EINVAL) or for other errors.  */
char *
gpgme_addrspec_from_uid (const char *uid)
{
  return _gpgme_mailbox_from_userid (uid);
}
