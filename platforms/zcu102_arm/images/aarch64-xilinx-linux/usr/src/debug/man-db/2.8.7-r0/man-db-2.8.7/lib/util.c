/*
 * util.c
 *
 * Copyright (C) 1990, 1991 John W. Eaton.
 * Copyright (C) 1994, 1995 Graeme W. Wilford. (Wilf.)
 * Copyright (C) 2001, 2002, 2004, 2007, 2008, 2010 Colin Watson.
 *
 * This file is part of man-db.
 *
 * man-db is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * man-db is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with man-db; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * John W. Eaton
 * jwe@che.utexas.edu
 * Department of Chemical Engineering
 * The University of Texas at Austin
 * Austin, Texas  78712
 *
 * Wed May  4 15:44:47 BST 1994 Wilf. (G.Wilford@ee.surrey.ac.uk): slight
 * changes to all routines, mainly cosmetic.
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <locale.h>

#include "stat-time.h"
#include "timespec.h"
#include "xvasprintf.h"

#include "gettext.h"

#include "manconfig.h"

#include "error.h"
#include "pipeline.h"

/*
 * Does file a have a different timestamp to file b?
 *
 * case:
 *
 *   a is man_page, b is cat_page
 *
 *   a and b have different times  returns  1/3  (ret & 1) == 1
 *   a and b have same times       returns  0/2  (!(ret & 1)) == 1
 *   a is zero in length           returns  + 2 (for Wilf. and his stray cats)
 *   b is zero in length           returns  + 4
 *   stat on a fails               returns   -1
 *   stat on b fails               returns   -2
 *   stat on a and b fails         returns   -3
 */
int is_changed (const char *fa, const char *fb)
{
	struct stat fa_sb;
	struct stat fb_sb;
	int fa_stat;
	int fb_stat;
	int status = 0;

	debug ("is_changed: a=%s, b=%s", fa, fb);

	fa_stat = stat (fa, &fa_sb);
	if (fa_stat != 0)
		status = 1;

	fb_stat = stat (fb, &fb_sb);
	if (fb_stat != 0)
		status |= 2;

	if (status != 0) {
		debug (" (%d)\n", -status);
		return -status;
	}

	if (fa_sb.st_size == 0)
		status |= 2;

	if (fb_sb.st_size == 0)
		status |= 4;

	status |= (timespec_cmp (get_stat_mtime (&fa_sb),
				 get_stat_mtime (&fb_sb)) != 0);

	debug (" (%d)\n", status);
	return status;
}

/*
 * Is path a directory?
 */
int is_directory (const char *path)
{
	struct stat sb;
	int status;

	status = stat (path, &sb);

	if (status != 0)
		return status;

	return ((sb.st_mode & S_IFDIR) != 0);
}

/* Escape dangerous metacharacters before dumping into a shell command. */
char *escape_shell (const char *unesc)
{
	char *esc, *escp;
	const char *unescp;

	if (!unesc)
		return NULL;

	escp = esc = xmalloc (strlen (unesc) * 2 + 1);
	for (unescp = unesc; *unescp; unescp++)
		if ((*unescp >= '0' && *unescp <= '9') ||
		    (*unescp >= 'A' && *unescp <= 'Z') ||
		    (*unescp >= 'a' && *unescp <= 'z') ||
		    strchr (",-./:@_", *unescp))
			*escp++ = *unescp;
		else {
			*escp++ = '\\';
			*escp++ = *unescp;
		}
	*escp = 0;
	return esc;
}

/* Remove a directory and all files in it.  Only recurse beyond that if
 * RECURSE is set.
 */
int remove_directory (const char *directory, int recurse)
{
	DIR *handle = opendir (directory);
	struct dirent *entry;

	if (!handle)
		return -1;
	while ((entry = readdir (handle)) != NULL) {
		struct stat st;
		char *path;

		if (STREQ (entry->d_name, ".") || STREQ (entry->d_name, ".."))
			continue;
		path = xasprintf ("%s/%s", directory, entry->d_name);
		if (stat (path, &st) == -1) {
			free (path);
			closedir (handle);
			return -1;
		}
		if (recurse && S_ISDIR (st.st_mode)) {
			if (remove_directory (path, recurse) == -1) {
				free (path);
				closedir (handle);
				return -1;
			}
		} else if (S_ISREG (st.st_mode)) {
			if (unlink (path) == -1) {
				free (path);
				closedir (handle);
				return -1;
			}
		}
		free (path);
	}
	closedir (handle);

	if (rmdir (directory) == -1)
		return -1;
	return 0;
}

/* Returns an allocated copy of s, with leading and trailing spaces
 * removed.
 */
char * _GL_ATTRIBUTE_MALLOC trim_spaces (const char *s)
{
	int length;
	while (*s == ' ')
		++s;
	length = strlen (s);
	while (length && s[length - 1] == ' ')
		--length;
	return xstrndup (s, length);
}

char *lang_dir (const char *filename)
{
	char *ld;	/* the lang dir: point to static data */
	const char *fm;	/* the first "/man/" dir */
	const char *sm;	/* the second "/man?/" dir */

	ld = xstrdup ("");
	if (!filename) 
		return ld;

	/* Check whether filename is in a man page hierarchy. */
	if (STRNEQ (filename, "man/", 4))
		fm = filename;
	else {
		fm = strstr (filename, "/man/");
		if (fm)
			++fm;
	}
	if (!fm)
		return ld;
	sm = strstr (fm + 2, "/man");
	if (!sm)
		return ld;
	if (sm[5] != '/')
		return ld;
	if (!strchr ("123456789lno", sm[4]))
		return ld;

	/* If there's no lang dir element, it's an English man page. */
	if (sm == fm + 3) {
		free (ld);
		return xstrdup ("C");
	}

	/* found a lang dir */
	fm += 4;
	sm = strchr (fm, '/');
	if (!sm)
		return ld;
	free (ld);
	ld = xstrndup (fm, sm - fm);
	debug ("found lang dir element %s\n", ld);
	return ld;
}

void init_locale (void)
{
	char *locale = setlocale (LC_ALL, "");
	if (!locale &&
	    !getenv ("MAN_NO_LOCALE_WARNING") &&
	    !getenv ("DPKG_RUNNING_VERSION"))
		/* Obviously can't translate this. */
		error (0, 0, "can't set the locale; make sure $LC_* and $LANG "
			     "are correct");
	setenv ("MAN_NO_LOCALE_WARNING", "1", 1);
#ifdef ENABLE_NLS
	bindtextdomain (PACKAGE, LOCALEDIR);
	bindtextdomain (PACKAGE "-gnulib", LOCALEDIR);
	textdomain (PACKAGE);
#endif /* ENABLE_NLS */
}
