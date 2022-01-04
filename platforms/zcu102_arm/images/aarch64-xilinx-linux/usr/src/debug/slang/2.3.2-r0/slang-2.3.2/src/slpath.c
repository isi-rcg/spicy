/* Pathname and filename functions */
/*
Copyright (C) 2004-2017,2018 John E. Davis

This file is part of the S-Lang Library.

The S-Lang Library is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

The S-Lang Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
USA.
*/

#include "slinclud.h"

#ifdef HAVE_IO_H
# include <io.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <time.h>

#include <errno.h>
#include <string.h>

#include "slang.h"
#include "_slang.h"

/* In this file, all file names are assumed to be specified in the Unix
 * format, or in the native format.
 *
 * Aboout VMS:
 * VMS pathnames are a mess.  In general, they look like
 * node::device:[dir.dir]file.ext;version
 * and I do not know of a well-defined Unix representation for them.  So,
 * I am going to punt and encourage users to stick to the native
 * representation.
 */

#if defined(IBMPC_SYSTEM)
# define PATH_SEP		'\\'
# define DRIVE_SPECIFIER	':'
# define SEARCH_PATH_DELIMITER	';'
# define THIS_DIR_STRING	"."
# define PARENT_DIR_STRING      ".."
#else
# if defined(VMS)
#  define PATH_SEP		']'
#  define DRIVE_SPECIFIER	':'
#  define SEARCH_PATH_DELIMITER	' '
#  define THIS_DIR_STRING	"[]"   /* Is this correct?? */
#  define PARENT_DIR_STRING     "[-]" /* ??????????????? */
# else
#  define PATH_SEP		'/'
#  define UNIX_PATHNAMES_OK
#  define SEARCH_PATH_DELIMITER	':'
#  define THIS_DIR_STRING	"."
#  define PARENT_DIR_STRING      ".."
# endif
#endif

#ifdef UNIX_PATHNAMES_OK
# define IS_PATH_SEP(x) ((x) == PATH_SEP)
#else
# define IS_PATH_SEP(x)	(((x) == PATH_SEP) || ((x) == '/'))
#endif

#define TEST_VMS_ON_UNIX	0

static char Path_Delimiter = SEARCH_PATH_DELIMITER;

/* On Unix, this has no effect.
 *   C:zzz ==> returns zzz (DOS/Windows, VMS)
 *   //drive/zzz ==> returns zzz (DOS/Windows)
 */
static SLFUTURE_CONST char *skip_drive (SLFUTURE_CONST char *file)
{
#ifdef DRIVE_SPECIFIER
   SLFUTURE_CONST char *f;
   /* look for A:/foo form */
   f = file + strlen (file);
   while (f != file)
     {
	f--;
	if (*f == DRIVE_SPECIFIER)
	  return f+1;
     }
#endif
#ifdef IBMPC_SYSTEM
   /* windows //netdrive/dir form */
   if (IS_PATH_SEP(file[0]) && IS_PATH_SEP(file[1]))
     {
	file += 2;
	while (*file && (0 == IS_PATH_SEP(*file)))
	  file++;
	return file;
     }
#endif
   return file;
}

/* If file is /a/b/c/basename, this function returns a pointer to basename */
SLFUTURE_CONST char *SLpath_basename (SLFUTURE_CONST char *drivefile)
{
   SLFUTURE_CONST char *b, *file;

   if (drivefile == NULL) return NULL;

   file = skip_drive (drivefile);
   b = file + strlen (file);

   while (b != file)
     {
	b--;
	if (IS_PATH_SEP(*b))
	  return b + 1;
     }

   return b;
}

/* Returns a malloced string */
char *SLpath_pathname_sans_extname (SLFUTURE_CONST char *drivefile)
{
   char *b;
   char *file;

   drivefile = SLmake_string (drivefile);
   if (drivefile == NULL)
     return NULL;

   file = (char *)skip_drive (drivefile);
   b = file + strlen (file);

   while (b != file)
     {
	b--;
	if (IS_PATH_SEP(*b))
	  break;

	if (*b == '.')
	  {
	     *b = 0;
	     break;
	  }
     }

   return drivefile;
}

/* If path looks like: A/B/C/D/whatever, it returns A/B/C/D as a malloced
 * string.
 */
char *SLpath_dirname (SLFUTURE_CONST char *drivefile)
{
   SLCONST char *b;
   const char *file;
   char *dir, *drivedir;
   size_t len;

   if (drivefile == NULL) return NULL;
   file = skip_drive (drivefile);

   b = file + strlen (file);

   while (b != file)
     {
	b--;
	if (0 == IS_PATH_SEP(*b))
	  continue;

#ifdef VMS
	b++;		       /* make sure final ] is included */
#else
	/* collapse multiple slashes */
	while ((b != file) && IS_PATH_SEP(*(b-1)))
	  b--;

	if (b == file) b++;
#endif
	break;
     }

   /* now b should point to the character after the slash:
    *    file="zzz/xxxx"
    *       b------^
    */
   if (b == file)
     {
	/* pathological cases -- what is the parent?  For simplicity
	 * simply return the current directory.
	 */
	len = file - drivefile;
	if (NULL == (dir = SLmalloc (len + 1 + strlen(THIS_DIR_STRING))))
	  return NULL;
	strncpy (dir, drivefile, len);
	strcpy (dir + len, THIS_DIR_STRING);
	return dir;
     }

   if (NULL == (drivedir = SLmake_nstring (drivefile, b - drivefile)))
     return NULL;

   dir = drivedir + (file - drivefile);
   len = b - file;		       /* len is from start of file on drive */

#ifndef VMS
   /* handle special cases
    *    /foo/.   --> /foo
    *    /.       --> /
    *    /foo/..  --> /
    * C:/.
    */
   while ((len > 1) && (dir[len-1] == '.'))
     {
	if (IS_PATH_SEP(dir[len-2]))
	  {
	     len--;		       /* lose "." */
	     while ((len > 1) && IS_PATH_SEP(dir[len-1]))
	       len--;		       /* lose "/" */
	     dir[len] = 0;
	     continue;
	  }
	if ((len > 2) && (dir[len-2] == '.') && IS_PATH_SEP(dir[len-3]))
	  {
	     len -= 2;		       /* lose ".." */
	     if (len > 1)
	       {
		  len--;		       /* lose "/" */
		  dir[len] = 0;
		  b = SLpath_basename (dir);   /* will not fail: zzz/xxx --> zzz/x */
		  len = b - dir;
		  while ((len > 1) && IS_PATH_SEP(dir[len-1]))
		    len--;
	       }
	     dir[len] = 0;
	     continue;
	  }

	break;
     }
#endif				       /* not VMS */
   return drivedir;
}

/* Note: VMS filenames also contain version numbers.  The caller will have
 * to deal with that.
 *
 * The extension includes the '.'.  If no extension is present, "" is returned.
 */
char *SLpath_extname (SLFUTURE_CONST char *file)
{
   char *b;

   if (NULL == (file = SLpath_basename (file)))
     return NULL;

   b = (char *) file + strlen (file);
   while (b != file)
     {
	b--;
	if (*b == '.')
	  return b;
     }

   if (*b == '.')
     return b;

   /* Do not return a literal "" */
   return (char *) file + strlen (file);
}

#ifdef IBMPC_SYSTEM
static void convert_slashes (char *f)
{
   while (*f)
     {
	if (*f == '/') *f = PATH_SEP;
	f++;
     }
}
#endif

int SLpath_is_absolute_path (SLFUTURE_CONST char *name)
{
   if (name == NULL)
     return -1;

#ifdef UNIX_PATHNAMES_OK
   return (*name == '/');
#else
   if (IS_PATH_SEP (*name))
     return 1;

   /* If it contains a drive specifier, regard it as absolute */
   if (name != skip_drive (name))
     return 1;

   return 0;
#endif
}

#if TEST_VMS_ON_UNIX || defined(VMS)
static char *vms_fixup_filename (char *file)
{
   char *bracket, *slash, *last_slash;
   /* Convert slashes in filenames such as:
    *  1.  drive:[dir.dir]a/b.c   --> drive[dir.dir.a]b.c
    *  2.  drive:/a/b/c.d         --> drive:[a.b]c.d
    *  3.  drive:a/b/c.d          --> drive:[a.b]c.d
    *  4.  drive:/a.c             --> drive:a.c  ???
    *  5.  drive:[dir.dir]/a/b    --> drive:a/b
    * Since the input pointer is malloced, and the length is preserved for the
    * first 2 cases, the conversion may be performed in-place.
    *
    * FIXME: case 5 is not handled
    */
   /* First case */
   bracket = strchr (file,']');
   if (bracket != NULL)
     {
	last_slash = NULL;
	slash = file;
	while (NULL != (slash = strchr (slash, '/')))
	  {
	     last_slash = slash;
	     *slash++ = '.';
	  }
	if (last_slash != NULL)
	  {
	     *bracket = '.';
	     *last_slash = ']';
	  }
	return file;
     }
   /* Second case */
   slash = strchr (file, '/');
   if (slash == NULL)
     return file;
   if ((slash != file) && (*(slash-1) != ':'))
     {
	/* Case 3:  drive:a/b/c.d          --> drive:[a.b]c.d */
	/* Convert it to case 2:  drive:/a/b/c.d */
	char *drive;
	unsigned int len;
	char *new_file = SLmalloc (strlen (file) + 2);

	if (new_file == NULL)
	  {
	     SLfree (file);
	     return NULL;
	  }
	drive = strchr (file, ':');
	if (drive == NULL)
	  len = 0;
	else
	  {
	     len = 1 + (drive-file);
	     strncpy (new_file, file, len);
	  }
	slash = new_file + len;
	*slash = '/';
	strcpy (slash + 1, file + len);
	SLfree (file);
	file = new_file;
     }

   /* Case 2: drive:/a/b/c.d         --> drive:[a.b]c.d */
   *slash++ = '[';
   last_slash = NULL;
   while (NULL != (slash = strchr (slash, '/')))
     {
	last_slash = slash;
	*slash++ = '.';
     }
   if (last_slash != NULL)
     {
	*last_slash = ']';
	return file;
     }

   /* case 4: drive:/a.c  -->  drive:a.c */
   slash = strchr (file, '[');	       /* non-NULL */
   while (*slash)
     {
	*slash = *(slash+1);
	slash++;
     }
   return file;
}
#endif

/* This returns a MALLOCED string */
char *SLpath_dircat (SLFUTURE_CONST char *dir, SLFUTURE_CONST char *name)
{
   unsigned int len, dirlen;
   char *file;
#ifndef VMS
   int requires_fixup;
#endif

   if (name == NULL)
     name = "";
   if ((dir == NULL)
#if !TEST_VMS_ON_UNIX
       || (SLpath_is_absolute_path (name))
#endif
       )
     dir = "";

   /* Both VMS and MSDOS have default directories associated with each drive.
    * That is, the meaning of something like C:X depends upon more than just
    * the syntax of the string.  Since this concept has more power under VMS
    * it will be honored here.  However, I am going to treat C:X as C:\X
    * under MSDOS.
    *
    * Note!!!
    * VMS has problems of its own regarding path names, so I am simply
    * going to strcat.  Hopefully the VMS RTL is smart enough to deal with
    * the result.
    */
   dirlen = strlen (dir);
#ifndef VMS
# if TEST_VMS_ON_UNIX
   requires_fixup = 0;
# else
   requires_fixup = (dirlen && (0 == IS_PATH_SEP(dir[dirlen - 1])));
# endif
#endif

   len = dirlen + strlen (name) + 2;
   if (NULL == (file = (char *)SLmalloc (len)))
     return NULL;

   strcpy (file, dir);

#ifndef VMS
   if (requires_fixup)
     file[dirlen++] = PATH_SEP;
#endif

   strcpy (file + dirlen, name);

#if defined(IBMPC_SYSTEM)
   convert_slashes (file);
#endif

#if TEST_VMS_ON_UNIX || defined (VMS)
   return vms_fixup_filename (file);
#else
   return file;
#endif
}

int SLpath_file_exists (SLFUTURE_CONST char *file)
{
   struct stat st;
   int m;

#if defined(__os2__) && !defined(S_IFMT)
/* IBM VA3 doesn't declare S_IFMT */
# define	S_IFMT	(S_IFDIR | S_IFCHR | S_IFREG)
#endif

#ifdef _S_IFDIR
# ifndef S_IFDIR
#  define S_IFDIR _S_IFDIR
# endif
#endif

#ifndef S_ISDIR
# ifdef S_IFDIR
#  define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
# else
#  define S_ISDIR(m) 0
# endif
#endif

   if (file == NULL)
     return -1;

   if (stat(file, &st) < 0) return 0;
   m = st.st_mode;

   if (S_ISDIR(m)) return (2);
   return 1;
}

/* By relatively-absolute, I mean paths of the form ./foo,
 * and ../foo/bar.  But not foo/bar.
 */
static int is_relatively_absolute (SLFUTURE_CONST char *file)
{
   if (file == NULL)
     return -1;
   if (SLpath_is_absolute_path (file))
     return 1;

#if defined(VMS)
   return (*file == '[');
#else
   if (*file == '.') file++;
   if (*file == '.') file++;
   return ((*file == PATH_SEP)
# if defined(IBMPC_SYSTEM)
	   || (*file == '/')
# endif
	   );
#endif
}

char *SLpath_find_file_in_path (SLFUTURE_CONST char *path, SLFUTURE_CONST char *name)
{
   unsigned int max_path_len;
   unsigned int this_path_len;
   char *file, *dir;
   SLCONST char *p;
   unsigned int nth;

   if ((path == NULL) || (*path == 0)
       || (name == NULL) || (*name == 0))
     return NULL;

   if (is_relatively_absolute (name))
     {
	if (0 == SLpath_file_exists (name))
	  return NULL;
	return SLmake_string (name);
     }

   /* Allow "." to mean the current directory on all systems */
   if ((path[0] == '.') && (path[1] == 0))
     {
	if (0 == SLpath_file_exists (name))
	  return NULL;
	return SLpath_dircat (THIS_DIR_STRING, name);
     }

   max_path_len = 0;
   this_path_len = 0;
   p = path;
   while (*p != 0)
     {
	if (*p++ == Path_Delimiter)
	  {
	     if (this_path_len > max_path_len) max_path_len = this_path_len;
	     this_path_len = 0;
	  }
	else this_path_len++;
     }
   if (this_path_len > max_path_len) max_path_len = this_path_len;
   max_path_len++;

   if (NULL == (dir = (char *)SLmalloc (max_path_len)))
     return NULL;

   nth = 0;
   while (-1 != SLextract_list_element ((char *) path, nth, Path_Delimiter,
					dir, max_path_len))
     {
	nth++;
	if (*dir == 0)
	  continue;

	if (NULL == (file = SLpath_dircat (dir, name)))
	  {
	     SLfree (dir);
	     return NULL;
	  }

	if (1 == SLpath_file_exists (file))
	  {
	     SLfree (dir);
	     return file;
	  }

	SLfree (file);
     }

   SLfree (dir);
   return NULL;
}

int SLpath_get_delimiter (void)
{
   return Path_Delimiter;
}

int SLpath_set_delimiter (int d)
{
   char ch = (char) d;
   if (ch == 0)
     return -1;

   Path_Delimiter = ch;
   return 0;
}


char *SLpath_getcwd (void)
{
   char cwd[4096];
   char *p;
   size_t len;

#ifndef HAVE_GETCWD
   p = getwd (cwd);
#else
# if defined (__EMX__)
   p = _getcwd2(cwd, sizeof(cwd));	       /* includes drive specifier */
# else
   p = getcwd(cwd, sizeof(cwd));	       /* djggp includes drive specifier */
# endif
#endif

   if (p == NULL)
     return NULL;

#ifdef IBMPC_SYSTEM
   convert_slashes (cwd);
#endif

   len = strlen (cwd);

   p = (char *) SLmalloc (len+2);      /* \0 + trailing / */
   if (p == NULL)
     {
#ifdef ENOMEM
	errno = ENOMEM;
#endif
	return NULL;
     }

   strcpy (p, cwd);

#ifndef VMS
   if (len && (p[len-1] != PATH_SEP))
     {
	p[len++] = PATH_SEP;
	p[len] = 0;
     }
#endif

   return p;
}
