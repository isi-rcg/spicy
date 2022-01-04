/* fstype.c -- determine type of file systems that files are on
   Copyright (C) 1990, 1991, 1992, 1993, 1994, 2000,
                 2004, 2010, 2011 Free Software Foundation, Inc.
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

/* Written by David MacKenzie <djm@gnu.org>.
 *
 * Converted to use gnulib's read_file_system_list()
 * by James Youngman <jay@gnu.org> (which saves a lot
 * of manual hacking of configure.in).
 */

/* config.h must be included first. */
#include <config.h>

/* system headers. */
#include <errno.h>
#include <fcntl.h>
#if HAVE_MNTENT_H
# include <mntent.h>
#endif
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_SYS_MKDEV_H
# include <sys/mkdev.h>
#endif
#ifdef HAVE_SYS_MNTIO_H
# include <sys/mntio.h>
#endif
#if HAVE_SYS_MNTTAB_H
# include <sys/mnttab.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/* gnulib headers. */
#include "dirname.h"
#include "xalloc.h"
#include "xstrtol.h"
#include "mountlist.h"
#include "error.h"
#include "gettext.h"

/* find headers. */
#include "defs.h"
#include "extendbuf.h"

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

static char *file_system_type_uncached (const struct stat *statp, const char *path);


static void
free_file_system_list (struct mount_entry *p)
{
  while (p)
    {
      struct mount_entry *pnext = p->me_next;

      free (p->me_devname);
      free (p->me_mountdir);

      if (p->me_type_malloced)
	free (p->me_type);
      p->me_next = NULL;
      free (p);
      p = pnext;
    }
}




#ifdef AFS
#include <netinet/in.h>
#include <afs/venus.h>
#if __STDC__
/* On SunOS 4, afs/vice.h defines this to rely on a pre-ANSI cpp.  */
#undef _VICEIOCTL
#define _VICEIOCTL(id)  ((unsigned int ) _IOW('V', id, struct ViceIoctl))
#endif
#ifndef _IOW
/* AFS on Solaris 2.3 doesn't get this definition.  */
#include <sys/ioccom.h>
#endif

static int
in_afs (char *path)
{
  static char space[2048];
  struct ViceIoctl vi;

  vi.in_size = 0;
  vi.out_size = sizeof (space);
  vi.out = space;

  if (pioctl (path, VIOC_FILE_CELL_NAME, &vi, 1)
      && (errno == EINVAL || errno == ENOENT))
	return 0;
  return 1;
}
#endif /* AFS */

/* Nonzero if the current file system's type is known.  */
static int fstype_known = 0;

/* Return a static string naming the type of file system that the file PATH,
   described by STATP, is on.
   RELPATH is the file name relative to the current directory.
   Return "unknown" if its file system type is unknown.  */

char *
filesystem_type (const struct stat *statp, const char *path)
{
  static char *current_fstype = NULL;
  static dev_t current_dev;

  if (current_fstype != NULL)
    {
      if (fstype_known && statp->st_dev == current_dev)
	return current_fstype;	/* Cached value.  */
      free (current_fstype);
    }
  current_dev = statp->st_dev;
  current_fstype = file_system_type_uncached (statp, path);
  return current_fstype;
}

static int
set_fstype_devno (struct mount_entry *p)
{
  struct stat stbuf;

  if (p->me_dev == (dev_t)-1)
    {
      set_stat_placeholders (&stbuf);
      if (0 == (options.xstat)(p->me_mountdir, &stbuf))
	{
	  p->me_dev = stbuf.st_dev;
	  return 0;
	}
      else
	{
	  return -1;
	}
    }
  return 0;			/* not needed */
}

static struct mount_entry *
must_read_fs_list (bool need_fs_type)
{
  struct mount_entry *entries = read_file_system_list (need_fs_type);
  if (NULL == entries)
    {
      /* We cannot determine for sure which file we were trying to
       * use because gnulib has abstracted all that stuff away.
       * Hence we cannot issue a specific error message here.
       */
      error (EXIT_FAILURE, 0, _("Cannot read mounted file system list"));
    }
  return entries;
}



/* Return a newly allocated string naming the type of file system that the
   file PATH, described by STATP, is on.
   RELPATH is the file name relative to the current directory.
   Return "unknown" if its file system type is unknown.  */

static char *
file_system_type_uncached (const struct stat *statp, const char *path)
{
  struct mount_entry *entries, *entry, *best;
  char *type;

  (void) path;

#ifdef AFS
  if (in_afs (path))
    {
      fstype_known = 1;
      return xstrdup ("afs");
    }
#endif

  best = NULL;
  entries = must_read_fs_list (true);
  for (type=NULL, entry=entries; entry; entry=entry->me_next)
    {
#ifdef MNTTYPE_IGNORE
      if (!strcmp (entry->me_type, MNTTYPE_IGNORE))
	continue;
#endif
      if (0 == set_fstype_devno (entry))
	{
	  if (entry->me_dev == statp->st_dev)
	    {
	      best = entry;
	      /* Don't exit the loop, because some systems (for
		 example Linux-based systems in which /etc/mtab is a
		 symlink to /proc/mounts) can have duplicate entries
		 in the filesystem list.  This happens most frequently
		 for /.
	      */
	    }
	}
    }
  if (best)
    {
      type = xstrdup (best->me_type);
    }
  free_file_system_list (entries);

  /* Don't cache unknown values. */
  fstype_known = (type != NULL);

  return type ? type : xstrdup (_("unknown"));
}


char *
get_mounted_filesystems (void)
{
  char *result = NULL;
  size_t alloc_size = 0u;
  size_t used = 0u;
  struct mount_entry *entries, *entry;
  void *p;

  entries = must_read_fs_list (false);
  for (entry=entries; entry; entry=entry->me_next)
    {
      size_t len;

#ifdef MNTTYPE_IGNORE
      if (!strcmp (entry->me_type, MNTTYPE_IGNORE))
	continue;
#endif

      len = strlen (entry->me_mountdir) + 1;
      p = extendbuf (result, used+len, &alloc_size);
      if (p)
	{
	  result = p;
	  strcpy (&result[used], entry->me_mountdir);
	  used += len;		/* len already includes one for the \0 */
	}
      else
	{
	  break;
	}
    }

  free_file_system_list (entries);
  return result;
}


dev_t *
get_mounted_devices (size_t *n)
{
  size_t alloc_size = 0u;
  size_t used = 0u;
  struct mount_entry *entries, *entry;
  dev_t *result = NULL;

  /* Use read_file_system_list () rather than must_read_fs_list()
   * because on some system this is always called at startup,
   * and find should only exit fatally if it needs to use the
   * result of this operation.   If we can't get the fs list
   * but we never need the information, there is no need to fail.
   */
  for (entry = entries = read_file_system_list (false);
       entry;
       entry = entry->me_next)
    {
      void *p = extendbuf (result, sizeof(dev_t)*(used+1), &alloc_size);
      if (p)
	{
	  result = p;
	  if (0 == set_fstype_devno (entry))
	    {
	      result[used] = entry->me_dev;
	      ++used;
	    }
	}
      else
	{
	  free (result);
	  result = NULL;
	}
    }
  free_file_system_list (entries);
  if (result)
    {
      *n = used;
    }
  return result;
}
