/* sharefile.c -- open files just once.
   Copyright (C) 2008, 2009, 2010, 2011 Free Software Foundation, Inc.

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

/* config.h always comes first. */
#include <config.h>

/* system headers. */
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

/* gnulib headers. */
#include "cloexec.h"
#include "hash.h"
#include "stdio-safer.h"

/* find headers. */
#include "sharefile.h"
#include "defs.h"


enum
  {
    DefaultHashTableSize = 11
  };

struct sharefile
{
  char *mode;
  Hash_table *table;
};


/*
 * We cannot use the name to determine that two strings represent the
 * same file, since that test would be fooled by symbolic links.
 * Instead we use the device and inode number.
 *
 * However, we remember the name of each file that we opened.  This
 * allows us to issue a fatal error message when (flushing and)
 * closing a file fails.
 */
struct SharefileEntry
{
  dev_t device;
  ino_t inode;
  char *name; /* not the only name for this file; error messages only */
  FILE *fp;
};


static bool
entry_comparator (const void *av, const void *bv)
{
  const struct SharefileEntry *a=av, *b=bv;
  return (a->inode == b->inode) && (a->device == b->device);
}

static void
entry_free (void *pv)
{
  struct SharefileEntry *p = pv;
  if (p->fp)
    {
      if (0 != fclose (p->fp))
	fatal_nontarget_file_error (errno, p->name);
    }
  free (p->name);
  free (p);
}

static size_t
entry_hashfunc (const void *pv, size_t buckets)
{
  const struct SharefileEntry *p = pv;
  return (p->device ^ p->inode) % buckets;
}



sharefile_handle
sharefile_init (const char *mode)
{
  struct Hash_tuning;

  struct sharefile *p = malloc (sizeof (struct sharefile));
  if (p)
    {
      p->mode = strdup (mode);
      if (p->mode)
	{
	  p->table = hash_initialize (DefaultHashTableSize, NULL,
				      entry_hashfunc,
				      entry_comparator,
				      entry_free);
	  if (p->table)
	    {
	      return p;
	    }
	  else
	    {
	      free (p->mode);
	      free (p);
	    }
	}
      else
	{
	  free (p);
	}
    }
  return NULL;
}

void
sharefile_destroy (sharefile_handle pv)
{
  struct sharefile *p = pv;
  free (p->mode);
  hash_free (p->table);
}


FILE *
sharefile_fopen (sharefile_handle h, const char *filename)
{
  struct sharefile *p = h;
  struct SharefileEntry *new_entry;

  new_entry = malloc (sizeof (struct SharefileEntry));
  if (!new_entry)
    return NULL;

  new_entry->name = strdup (filename);
  if (NULL == new_entry->name)
    {
      free (new_entry);
      return NULL;
    }

  if (NULL == (new_entry->fp = fopen_safer (filename, p->mode)))
    {
      free (new_entry);
      return NULL;
    }
  else
    {
      struct stat st;
      const int fd = fileno (new_entry->fp);
      assert (fd >= 0);

      set_cloexec_flag (fd, true);
      if (fstat (fd, &st) < 0)
        {
	  entry_free (new_entry);
          return NULL;
        }
      else
        {
	  void *existing;

          new_entry->device = st.st_dev;
          new_entry->inode = st.st_ino;

          existing = hash_lookup (p->table, new_entry);
          if (existing)	    /* We have previously opened that file. */
	    {
	      entry_free (new_entry); /* don't need new_entry. */
	      return ((const struct SharefileEntry*)existing)->fp;
	    }
          else /* We didn't open it already */
	    {
	      if (hash_insert (p->table, new_entry))
		{
		  return new_entry->fp;
		}
	      else			/* failed to insert in hashtable. */
		{
		  const int save_errno = errno;
		  entry_free (new_entry);
		  errno = save_errno;
		  return NULL;
		}
	    }
        }
    }
}
