/* This file is part of GNU paxutils
   Copyright (C) 2005, 2007, 2010 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 3, or (at your option) any later
   version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
   Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

#include <system.h>
#include <hash.h>
#include <paxlib.h>


/* Hash tables of strings.  */

/* Calculate the hash of a string.  */
static size_t
hash_string_hasher (void const *name, size_t n_buckets)
{
  return hash_string (name, n_buckets);
}

/* Compare two strings for equality.  */
static bool
hash_string_compare (void const *name1, void const *name2)
{
  return strcmp (name1, name2) == 0;
}

/* Return zero if TABLE contains a LEN-character long prefix of STRING,
   otherwise, insert a newly allocated copy of this prefix to TABLE and
   return 1.  If RETURN_PREFIX is not NULL, point it to the allocated
   copy. */
static bool
hash_string_insert_prefix (Hash_table **table, char const *string, size_t len,
			   const char **return_prefix)
{
  Hash_table *t = *table;
  char *s;
  char *e;

  if (len)
    {
      s = xmalloc (len + 1);
      memcpy (s, string, len);
      s[len] = 0;
    }
  else
    s = xstrdup (string);

  if (! ((t
	  || (*table = t = hash_initialize (0, 0, hash_string_hasher,
					    hash_string_compare, 0)))
	 && (e = hash_insert (t, s))))
    xalloc_die ();

  if (e == s)
    {
      if (return_prefix)
	*return_prefix = s;
      return 1;
    }
  else
    {
      free (s);
      return 0;
    }
}


static Hash_table *prefix_table[2];

/* Return true if file names of some members in the archive were stripped off
   their leading components. We could have used
        return prefix_table[0] || prefix_table[1]
   but the following seems to be safer: */
bool
removed_prefixes_p (void)
{
  return (prefix_table[0] && hash_get_n_entries (prefix_table[0]) != 0)
         || (prefix_table[1] && hash_get_n_entries (prefix_table[1]) != 0);
}

/* Return a safer suffix of FILE_NAME, or "." if it has no safer
   suffix.  Check for fully specified file names and other atrocities.
   Warn the user if we do not return NAME.  If LINK_TARGET is 1,
   FILE_NAME is the target of a hard link, not a member name.
   If ABSOLUTE_NAMES is 0, strip filesystem prefix from the file name. */

char *
safer_name_suffix (char const *file_name, bool link_target,
		   bool absolute_names)
{
  char const *p;

  if (absolute_names)
    p = file_name;
  else
    {
      /* Skip file system prefixes, leading file name components that contain
	 "..", and leading slashes.  */

      size_t prefix_len = FILE_SYSTEM_PREFIX_LEN (file_name);

      for (p = file_name + prefix_len; *p; )
	{
          if (p[0] == '.' && p[1] == '.' && (ISSLASH (p[2]) || !p[2]))
	    prefix_len = p + 2 - file_name;

	  do
	    {
	      char c = *p++;
	      if (ISSLASH (c))
		break;
	    }
	  while (*p);
	}

      for (p = file_name + prefix_len; ISSLASH (*p); p++)
	continue;
      prefix_len = p - file_name;

      if (prefix_len)
	{
	  const char *prefix;
	  if (hash_string_insert_prefix (&prefix_table[link_target], file_name,
					 prefix_len, &prefix))
	    {
	      static char const *const diagnostic[] =
	      {
		N_("Removing leading `%s' from member names"),
		N_("Removing leading `%s' from hard link targets")
	      };
	      WARN ((0, 0, _(diagnostic[link_target]), prefix));
	    }
	}
    }

  if (! *p)
    {
      if (p == file_name)
	{
	  static char const *const diagnostic[] =
	  {
	    N_("Substituting `.' for empty member name"),
	    N_("Substituting `.' for empty hard link target")
	  };
	  WARN ((0, 0, "%s", _(diagnostic[link_target])));
	}

      p = ".";
    }

  return (char *) p;
}
