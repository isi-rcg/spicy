/*
 * straycats.c: find and process stray cat files
 *
 * Copyright (C) 1994, 1995 Graeme W. Wilford. (Wilf.)
 * Copyright (C) 2001, 2002, 2003, 2004, 2006, 2007, 2008, 2010, 2011
 *               Colin Watson.
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
 * Tue May  3 21:24:51 BST 1994 Wilf. (G.Wilford@ee.surrey.ac.uk)
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include "canonicalize.h"
#include "dirname.h"
#include "gl_array_list.h"
#include "gl_xlist.h"

#include "gettext.h"
#define _(String) gettext (String)

#include "manconfig.h"

#include "error.h"
#include "glcontainers.h"
#include "pipeline.h"
#include "decompress.h"
#include "encodings.h"
#include "orderfiles.h"
#include "sandbox.h"
#include "security.h"

#include "mydbm.h"
#include "db_storage.h"

#include "descriptions.h"
#include "manp.h"
#include "manconv_client.h"
#include "ult_src.h"

extern man_sandbox *sandbox;

static char *catdir, *mandir;

static int check_for_stray (MYDBM_FILE dbf)
{
	DIR *cdir;
	struct dirent *catlist;
	gl_list_t names;
	const char *name;
	size_t lenman, lencat;
	int strays = 0;

	cdir = opendir (catdir);
	if (!cdir) {
		error (0, errno, _("can't search directory %s"), catdir);
		return 0;
	}

	names = new_string_list (GL_ARRAY_LIST, false);

	while ((catlist = readdir (cdir)) != NULL) {
		if (*catlist->d_name == '.' && 
		    strlen (catlist->d_name) < (size_t) 3)
			continue;
		gl_list_add_last (names, xstrdup (catlist->d_name));
	}
	closedir (cdir);

	order_files (catdir, &names);

	mandir = appendstr (mandir, "/", (void *) 0);
	catdir = appendstr (catdir, "/", (void *) 0);
	lenman = strlen (mandir);
	lencat = strlen (catdir);

	GL_LIST_FOREACH_START (names, name) {
		struct mandata info;
		char *ext, *section;
		short found;
		struct stat buf;
#ifdef COMP_SRC
		struct compression *comp;
#endif

		memset (&info, 0, sizeof (struct mandata));

		*(mandir + lenman) = *(catdir + lencat) = '\0';
		mandir = appendstr (mandir, name, (void *) 0);
		catdir = appendstr (catdir, name, (void *) 0);

		ext = strrchr (mandir, '.');
		if (!ext) {
			if (quiet < 2)
				error (0, 0,
				       _("warning: %s: "
					 "ignoring bogus filename"),
				       catdir);
			continue;

#if defined(COMP_SRC) || defined(COMP_CAT)

#  if defined(COMP_SRC)
		} else if (comp_info (ext, 0)) {
#  elif defined(COMP_CAT)
		} else if (strcmp (ext + 1, COMPRESS_EXT) == 0) {
#  endif /* COMP_* */
			*ext = '\0';
			info.comp = ext + 1;
#endif /* COMP_SRC || COMP_CAT */

		} else
			info.comp = NULL;

		ext = strrchr (mandir, '.');
		*(mandir + lenman - 1) = '\0';
		section = xstrdup (strrchr (mandir, '/') + 4);
		*(mandir + lenman - 1) = '/';

		/* check for bogosity */
		
		if (!ext || strncmp (ext + 1, section, strlen (section)) != 0) {
			if (quiet < 2)
				error (0, 0,
				       _("warning: %s: "
					 "ignoring bogus filename"),
				       catdir);
			goto next_section;
		}

		/*
		 * now that we've stripped off the cat compression
		 * extension (if it has one), we can try some of ours.
		 */

		debug ("Testing for existence: %s\n", mandir);

		if (stat (mandir, &buf) == 0) 
			found = 1;
#ifdef COMP_SRC 
		else if ((comp = comp_file (mandir))) {
			found = 1;
			free (comp->stem);
		}
#endif
		else 
			found = 0;

		if (!found) {
			pipeline *decomp;
			struct mandata *exists;
			lexgrog lg;
			char *lang, *page_encoding;
			char *mandir_base;
			pipecmd *col_cmd;
			char *col_locale;
			char *fullpath;

			/* we have a straycat. Need to filter it and get
			   its whatis (if necessary)  */

			lg.whatis = 0;
			*(ext++) = '\0';
			info.ext = ext;

			/* see if we already have it, before going any 
			   further */
			mandir_base = base_name (mandir);
			exists = dblookup_exact (dbf, mandir_base, info.ext,
						 true);
			if (exists &&
			    compare_ids (STRAY_CAT, exists->id, 0) >= 0)
				goto next_exists;
			debug ("%s(%s) is not in the db.\n",
			       mandir_base, info.ext);

			/* fill in the missing parts of the structure */
			info.name = NULL;
			info.sec = section;
			info.id = STRAY_CAT;
			info.pointer = NULL;
			info.filter = "-";
			info.mtime.tv_sec = 0;
			info.mtime.tv_nsec = 0;

			drop_effective_privs ();
			decomp = decompress_open (catdir);
			regain_effective_privs ();
			if (!decomp) {
				error (0, errno, _("can't open %s"), catdir);
				goto next_exists;
			}

			lang = lang_dir (mandir);
			page_encoding = get_page_encoding (lang);
			if (page_encoding)
				add_manconv (decomp, page_encoding, "UTF-8");
			free (page_encoding);
			free (lang);

			col_cmd = pipecmd_new_argstr
				(get_def_user ("col", COL));
			pipecmd_arg (col_cmd, "-bx");
			col_locale = find_charset_locale ("UTF-8");
			if (col_locale) {
				pipecmd_setenv (col_cmd, "LC_CTYPE",
						col_locale);
				free (col_locale);
			}
			pipecmd_pre_exec (col_cmd, sandbox_load, sandbox_free,
					  sandbox);
			pipeline_command (decomp, col_cmd);

			fullpath = canonicalize_file_name (catdir);
			if (!fullpath) {
				if (quiet < 2) {
					if (errno == ENOENT)
						error (0, 0, _("warning: %s is a dangling symlink"), fullpath);
					else
						error (0, errno,
						       _("can't resolve %s"),
						       catdir);
				}
			} else {
				char *catdir_base;

				free (fullpath);
				drop_effective_privs ();
				pipeline_start (decomp);
				regain_effective_privs ();

				strays++;

				lg.type = CATPAGE;
				catdir_base = base_name (catdir);
				if (find_name_decompressed (decomp,
							    catdir_base,
							    &lg)) {
					gl_list_t descs;
					strays++;
					descs = parse_descriptions
						(mandir_base, lg.whatis);
					store_descriptions (dbf, descs, &info,
							    NULL, mandir_base,
							    NULL);
					gl_list_free (descs);
				} else if (quiet < 2)
					error (0, 0, _("warning: %s: whatis parse for %s(%s) failed"),
					       catdir, mandir_base, info.sec);
				free (catdir_base);
			}

			free (lg.whatis);
			pipeline_free (decomp);
next_exists:
			free_mandata_struct (exists);
			free (mandir_base);
		}
next_section:
		free (section);
	} GL_LIST_FOREACH_END (names);
	gl_list_free (names);
	return strays;
}

static int open_catdir (MYDBM_FILE dbf)
{
	DIR *cdir;
	struct dirent *catlist;
	size_t catlen, manlen;
	int strays = 0;

	cdir = opendir (catdir);
	if (!cdir) {
		error (0, errno, _("can't search directory %s"), catdir);
		return 0;
	}

	if (!quiet)
		printf (_("Checking for stray cats under %s...\n"), catdir);

	catdir = appendstr (catdir, "/", (void *) 0);
	mandir = appendstr (mandir, "/", (void *) 0);
	catlen = strlen (catdir);
	manlen = strlen (mandir);

	/* should make this case insensitive */
	while ((catlist = readdir (cdir))) {
		char *t1;

		if (strncmp (catlist->d_name, "cat", 3) != 0)
			continue;

		catdir = appendstr (catdir, catlist->d_name, (void *) 0);
		mandir = appendstr (mandir, catlist->d_name, (void *) 0);

		*(t1 = mandir + manlen) = 'm';
		*(t1 + 2) = 'n';

		strays += check_for_stray (dbf);

		*(catdir + catlen) = *(mandir + manlen) = '\0';
	}
	closedir (cdir);
	return strays;
}

int straycats (const char *database, const char *manpath)
{
	MYDBM_FILE dbf;
	char *catpath;
	int strays;

	dbf = MYDBM_RWOPEN (database);
	if (dbf && dbver_rd (dbf)) {
		MYDBM_CLOSE (dbf);
		dbf = NULL;
	}
	if (!dbf) {
		error (0, errno, _("warning: can't update index cache %s"),
		       database);
		return 0;
	}

	catpath = get_catpath (manpath, SYSTEM_CAT | USER_CAT);

	/* look in the usual catpath location */
	mandir = xstrdup (manpath);
	catdir = xstrdup (manpath);
	strays = open_catdir (dbf); 

	/* look in the alternate catpath location if we have one 
	   and it's different from the usual catpath */

	if (catpath)
		debug ("catpath: %s, manpath: %s\n", catpath, manpath);
		
	if (catpath && strcmp (catpath, manpath) != 0) {
		*mandir = *catdir = '\0';
		mandir = appendstr (mandir, manpath, (void *) 0);
		catdir = appendstr (catdir, catpath, (void *) 0);
		strays += open_catdir (dbf);
	}

	free (mandir);
	free (catdir);

	free (catpath);

	MYDBM_CLOSE (dbf);
	return strays;
}
