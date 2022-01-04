/* filesys.h -- external declarations for filesys.c.
   $Id: filesys.h 7013 2016-02-13 21:19:19Z gavin $

   Copyright 1993, 1997, 1998, 2002, 2004, 2005, 2007, 2009, 2012, 2013,
   2014, 2016 Free Software Foundation, Inc.

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

   Originally written by Brian Fox. */

#ifndef INFO_FILESYS_H
#define INFO_FILESYS_H

/* Return a string describing the search path. */
extern char *infopath_string ();

/* Initialize INFOPATH */
void infopath_init (void);

/* Add PATH to the list of paths found in INFOPATH. */
void infopath_add (char *path);

/* Iterate over INFOPATH */
char *infopath_first (int *idx);
char *infopath_next (int *idx);

/* Expand the filename in PARTIAL to make a real name for this operating
   system.  This looks in INFO_PATHS in order to find the correct file.
   If it can't find the file, it returns NULL. */
char *info_find_fullpath (char *partial, struct stat *finfo);

/* Scan the list of directories in PATH looking for FILENAME.  If we find
   one that is a regular file, return it as a new string.  Otherwise, return
   a NULL pointer. */
char *info_file_find_next_in_path (char *filename, int *diridx,
                                   struct stat *finfo);

char *info_add_extension (char *dirname, char *filename, struct stat *finfo);

/* Read the contents of PATHNAME, returning a buffer with the contents of
   that file in it, and returning the size of that buffer in FILESIZE.
   FINFO is a stat struct which has already been filled in by the caller.
   If the file cannot be read, return a NULL pointer. */
char *filesys_read_info_file (char *pathname, size_t *filesize,
    struct stat *finfo, int *is_compressed);

/* A function which returns a pointer to a static buffer containing
   an error message for FILENAME and ERROR_NUM. */
char *filesys_error_string (char *filename, int error_num);

/* The number of the most recent file system error. */
extern int filesys_error_number;

/* Return true if FILENAME is `dir', with a possible compression suffix.  */
int is_dir_name (char *filename);

/* The default value of INFOPATH. */
#if !defined (DEFAULT_INFOPATH)
#  define DEFAULT_INFOPATH "PATH:/usr/local/info:/usr/info:/usr/local/lib/info:/usr/lib/info:/usr/local/gnu/info:/usr/local/gnu/lib/info:/usr/gnu/info:/usr/gnu/lib/info:/opt/gnu/info:/usr/share/info:/usr/share/lib/info:/usr/local/share/info:/usr/local/share/lib/info:/usr/gnu/lib/emacs/info:/usr/local/gnu/lib/emacs/info:/usr/local/lib/emacs/info:/usr/local/emacs/info:."
#endif /* !DEFAULT_INFOPATH */

#if !defined (S_ISREG) && defined (S_IFREG)
#  define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif /* !S_ISREG && S_IFREG */

#if !defined (S_ISDIR) && defined (S_IFDIR)
#  define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif /* !S_ISDIR && S_IFDIR */

#endif /* not INFO_FILESYS_H */
