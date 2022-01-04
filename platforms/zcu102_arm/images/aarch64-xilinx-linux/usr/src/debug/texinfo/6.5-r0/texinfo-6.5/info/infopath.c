/* infopath.c -- INFOPATH handling.
   $Id: infopath.c 6177 2015-03-04 12:12:18Z gavin $

   Copyright 1993, 1997, 1998, 2000, 2002, 2003, 2004, 2007, 2008, 2009, 2011,
   2012, 2013, 2014, 2015 Free Software Foundation, Inc.

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

#include "info.h"
#include "info-utils.h"
#include "session.h"
#include "filesys.h"

typedef struct {
  char *name;    /* Path to directory to be searched. */
  dev_t device;  /* Storage device this directory is on. */
  ino_t inode;   /* Inode number, used to detect duplicates. */
} INFO_DIR;

INFO_DIR **infodirs = 0;
size_t infodirs_index = 0;
size_t infodirs_slots = 0;

/* Exclude default file search directories. */
int infopath_no_defaults_p;

static void infopath_add_dir (char *path);
char *extract_colon_unit (char *string, int *idx);

void
infopath_init ()
{
  /* Initialize INFOPATH.
     Highest priority is the environment variable, if set
     Then comes the user's INFODIR from the Makefile.
     The hardwired default settings (filesys.h) are the lowest priority. */
  char *path_from_env = getenv ("INFOPATH");

  if (path_from_env)
    {
      infopath_add (path_from_env);
    }

  if (!infopath_no_defaults_p)
    {
#ifdef INFODIR /* $infodir, set by configure script in Makefile */
      infopath_add (INFODIR);
#ifdef INFODIR2 /* $datadir/info, which could be different. */
      if (!STREQ (INFODIR, INFODIR2))
        infopath_add (INFODIR2);
#endif /* INFODIR2 */
#endif /* INFODIR */
    }

  if (!path_from_env)
    {
      infopath_add (DEFAULT_INFOPATH);
    }
  else
    { 
      /* Only insert default path if there is a trailing : on INFOPATH. */

      unsigned len = strlen (path_from_env);
      if (len && path_from_env[len - 1] == PATH_SEP[0])
	{
	  path_from_env[len - 1] = 0;
	  infopath_add (DEFAULT_INFOPATH);
	}
    }
}

/* Return value to be freed by caller. */
char *
infopath_string ()
{
  struct text_buffer path;
  int dir_idx;
  char *this_dir;

  this_dir = infopath_first (&dir_idx);
  if (!this_dir)
    return "";

  text_buffer_init (&path);

  while (1)
    {
      text_buffer_printf (&path, "%s", this_dir);
      this_dir = infopath_next (&dir_idx);
      if (!this_dir)
        break;
      text_buffer_add_char (&path, ':');
    }
  return text_buffer_base (&path); 
}

/* For each path element PREFIX/DIR in PATH substitute either
   PREFIX/share/info or PREFIX/info if that directory exists. */
static void
build_infopath_from_path (void)
{
  char *path_from_env, *temp_dirname;
  int dirname_index = 0;
  struct stat finfo;

  path_from_env = getenv ("PATH");

  while ((temp_dirname = extract_colon_unit (path_from_env, &dirname_index)))
    {
      unsigned int i, dir = 0;

      /* Find end of DIRNAME/ (but ignore "/") */
      for (i = 0; temp_dirname[i]; i++)
        if (i && IS_SLASH (temp_dirname[i]))
          dir = i + 1;

      /* Discard path elements ending with "/", "/.", or "/.." */
      if (!temp_dirname[dir] || STREQ (temp_dirname + dir, ".") || STREQ (temp_dirname + dir, "."))
        dir = 0;
      
      if (dir)
        {
          temp_dirname = xrealloc (temp_dirname, dir + strlen ("share/info") +1);

          /* first try DIRNAME/share/info */
          strcpy (temp_dirname + dir, "share/info");
          if (stat (temp_dirname, &finfo) != 0 || !S_ISDIR (finfo.st_mode))
            {
              /* then try DIRNAME/info */
              strcpy (temp_dirname + dir, "info");
              if (stat (temp_dirname, &finfo) != 0 || !S_ISDIR (finfo.st_mode))
                dir = 0;
            }
        }

      if (dir)
        infopath_add_dir (temp_dirname);
      else
        free (temp_dirname);
    }
}

/* Add directory at PATH to Info search path.  A reference to PATH is retained,
   or PATH is freed. */
static void
infopath_add_dir (char *path)
{
  struct stat dirinfo;
  INFO_DIR *entry;
  int i;

  if (stat (path, &dirinfo) == -1)
    {
      debug (2, ("inaccessible directory %s not added to INFOPATH", path));
      free (path);
      return; /* Doesn't exist, or not accessible. */
    }

  for (i = 0; i < infodirs_index; i++)
    {
      if (   dirinfo.st_ino == infodirs[i]->inode
          && dirinfo.st_dev == infodirs[i]->device
          /* On MS-Windows, `stat' returns zero as the inode, so we
             use file-name comparison instead for that OS.  */
          && (infodirs[i]->inode != 0 || fncmp (path, infodirs[i]->name) == 0))
        {
          debug (2, ("duplicate directory %s not added to INFOPATH", path));
          free (path);
          return; /* We have it already. */
        }
    }

  debug (2, ("adding %s to INFOPATH", path));
  entry = xmalloc (sizeof (INFO_DIR));
  entry->name = path;
  entry->inode = dirinfo.st_ino;
  entry->device = dirinfo.st_dev;
  add_pointer_to_array (entry, infodirs_index, infodirs, infodirs_slots, 8);
}

/* Add PATH to the list of paths found in INFOPATH.  PATH should be allocated
   on the heap and not referenced by the caller after calling this function.
   If PATH is "PATH", add a sequence of path elements derived from the
   environment variable PATH. */
void
infopath_add (char *path)
{
  int idx = 0;
  char *dirname;

  while (dirname = extract_colon_unit (path, &idx))
    {
      if (!strcmp ("PATH", dirname))
        {
          free (dirname);
          build_infopath_from_path ();
        }
      else
        infopath_add_dir (dirname);
    }
}

/* Used to iterate over INFOPATH.  Return value should not be freed
   by caller. */
char *
infopath_next (int *idx)
{
  INFO_DIR *entry;
 
  if (!infodirs)
   return 0;
  entry = infodirs[(*idx)++];
  if (!entry)
    return 0;
  return entry->name;
}

char *
infopath_first (int *idx)
{
  *idx = 0;
  return infopath_next (idx);
}

/* Given a string containing units of information separated by the
   PATH_SEP character, return the next one after IDX, or NULL if there
   are no more.  Advance IDX to the character after the colon. */
char *
extract_colon_unit (char *string, int *idx)
{
  unsigned int i = (unsigned int) *idx;
  unsigned int start = i;

  if (!string || i >= strlen (string))
    return NULL;

  if (!string[i]) /* end of string */
    return NULL;

  /* Advance to next PATH_SEP.  */
  while (string[i] && string[i] != PATH_SEP[0])
    i++;

  {
    char *value = xmalloc ((i - start) + 1);
    strncpy (value, &string[start], (i - start));
    value[i - start] = 0;

    i++; /* move past PATH_SEP */
    *idx = i;
    return value;
  }
}
