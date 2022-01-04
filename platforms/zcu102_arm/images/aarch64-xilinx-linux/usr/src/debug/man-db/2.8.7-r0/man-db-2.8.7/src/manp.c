/*
 * manp.c: Manpath calculations
 *
 * Copyright (C) 1990, 1991 John W. Eaton.
 * Copyright (C) 1994, 1995 Graeme W. Wilford. (Wilf.)
 * Copyright (C) 2001, 2002, 2003, 2004, 2006, 2007, 2008, 2009, 2010, 2011,
 *               2012 Colin Watson.
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
 * unpack_locale_bits is derived from _nl_explode_name in libintl:
 * Copyright (C) 1995-1998, 2000-2001, 2003, 2005 Free Software Foundation,
 * Inc.
 * Contributed by Ulrich Drepper <drepper@gnu.ai.mit.edu>, 1995.
 * This was originally LGPL v2 or later, but I (Colin Watson) hereby
 * exercise my option under section 3 of LGPL v2 to distribute it under the
 * GPL v2 or later as above.
 *
 * Wed May  4 15:44:47 BST 1994 Wilf. (G.Wilford@ee.surrey.ac.uk): changes
 * to get_dirlist() and manpath().
 *
 * This whole code segment is unfriendly and could do with a complete 
 * overhaul.
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <errno.h>
#include <dirent.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "canonicalize.h"
#include "gl_array_list.h"
#include "gl_linkedhash_list.h"
#include "gl_xlist.h"
#include "xgetcwd.h"
#include "xvasprintf.h"

#include "gettext.h"
#define _(String) gettext (String)

#include "manconfig.h"

#include "error.h"
#include "cleanup.h"
#include "glcontainers.h"
#include "security.h"

#include "manp.h"
#include "globbing.h"

enum config_flag {
	MANDATORY,
	MANPATH_MAP,
	MANDB_MAP,
	MANDB_MAP_USER,
	DEFINE,
	DEFINE_USER,
	SECTION,
	SECTION_USER
};

struct config_item {
	char *key;
	char *cont;
	enum config_flag flag;
};

static gl_list_t config;

char *user_config_file = NULL;
bool disable_cache;
int min_cat_width = 80, max_cat_width = 80, cat_width = 0;

static void add_man_subdirs (gl_list_t list, const char *p);
static char *fsstnd (const char *path);
static char *def_path (enum config_flag flag);
static void add_dir_to_list (gl_list_t list, const char *dir);
static void add_dir_to_path_list (gl_list_t list, const char *p);


static void config_item_free (const void *elt)
{
	/* gl_list declares the argument as const, but there doesn't seem to
	 * be a good reason for this.
	 */
	struct config_item *item = (struct config_item *) elt;
	free (item->key);
	free (item->cont);
	free (item);
}

static void add_config (const char *key, const char *cont,
			enum config_flag flag)
{
	struct config_item *item = XMALLOC (struct config_item);
	item->key = xstrdup (key);
	item->cont = xstrdup (cont);
	item->flag = flag;
	gl_list_add_last (config, item);
}

static const char *get_config (const char *key, enum config_flag flag)
{
	const struct config_item *item;
	char *cont = NULL;

	GL_LIST_FOREACH_START (config, item)
		if (flag == item->flag && STREQ (key, item->key)) {
			cont = item->cont;
			break;
		}
	GL_LIST_FOREACH_END (config);

	return cont;
}

/* Must not return DEFINEs set in ~/.manpath. This is used to fetch
 * definitions used in raised-privilege code; if in doubt, be conservative!
 *
 * If not setuid, this is identical to get_def_user.
 */
const char *get_def (const char *thing, const char *def)
{
	const char *config_def;

	if (!running_setuid ())
		return get_def_user (thing, def);

	config_def = get_config (thing, DEFINE);
	return config_def ? config_def : def;
}

const char *get_def_user (const char *thing, const char *def)
{
	const char *config_def = get_config (thing, DEFINE_USER);
	if (!config_def)
		config_def = get_config (thing, DEFINE);
	return config_def ? config_def : def;
}

static const char *describe_flag (enum config_flag flag)
{
	switch (flag) {
		case MANDATORY:
			return "MANDATORY";
		case MANPATH_MAP:
			return "MANPATH_MAP";
		case MANDB_MAP:
			return "MANDB_MAP";
		case MANDB_MAP_USER:
			return "MANDB_MAP_USER";
		case DEFINE:
			return "DEFINE";
		case DEFINE_USER:
			return "DEFINE_USER";
		case SECTION:
			return "SECTION";
		case SECTION_USER:
			return "SECTION_USER";
		default:
			error (FATAL, 0, "impossible config_flag value %u",
			       flag);
			abort (); /* error should have exited */
	}
}

static void print_list (void)
{
	const struct config_item *item;

	GL_LIST_FOREACH_START (config, item)
		debug ("`%s'\t`%s'\t`%s'\n",
		       item->key, item->cont, describe_flag (item->flag));
	GL_LIST_FOREACH_END (config);
}

static void add_sections (char *sections, int user)
{
	char *section_list = xstrdup (sections);
	char *sect;

	for (sect = strtok (section_list, " "); sect;
	     sect = strtok (NULL, " ")) {
		add_config (sect, "", user ? SECTION_USER : SECTION);
		debug ("Added section `%s'.\n", sect);
	}
	free (section_list);
}

gl_list_t get_sections (void)
{
	const struct config_item *item;
	int length_user = 0, length = 0;
	gl_list_t sections;
	enum config_flag flag;

	GL_LIST_FOREACH_START (config, item) {
		if (item->flag == SECTION_USER)
			length_user++;
		else if (item->flag == SECTION)
			length++;
	} GL_LIST_FOREACH_END (config);
	sections = new_string_list (GL_ARRAY_LIST, true);
	if (length_user)
		flag = SECTION_USER;
	else
		flag = SECTION;
	GL_LIST_FOREACH_START (config, item)
		if (item->flag == flag)
			gl_list_add_last (sections, xstrdup (item->key));
	GL_LIST_FOREACH_END (config);
	return sections;
}

static void add_def (const char *thing, const char *config_def, int user)
{
	add_config (thing, config_def, user ? DEFINE_USER : DEFINE);

	debug ("Defined `%s' as `%s'.\n", thing, config_def);
}

static void add_manpath_map (const char *path, const char *mandir)
{
	if (!path || !mandir)
		return;

	add_config (path, mandir, MANPATH_MAP);

	debug ("Path `%s' mapped to mandir `%s'.\n", path, mandir);
}

static void add_mandb_map (const char *mandir, const char *catdir, int user)
{
	char *tmpcatdir;

	if (!mandir)
		return;

	if (STREQ (catdir, "FSSTND"))
		tmpcatdir = fsstnd (mandir);
	else
		tmpcatdir = xstrdup (catdir);

	if (!tmpcatdir)
		return;

	add_config (mandir, tmpcatdir, user ? MANDB_MAP_USER : MANDB_MAP);

	debug ("%s mandir `%s', catdir `%s'.\n",
	       user ? "User" : "Global", mandir, tmpcatdir);

	free (tmpcatdir);
}

static void add_mandatory (const char *mandir)
{
	if (!mandir)
		return;

	add_config (mandir, "", MANDATORY);

	debug ("Mandatory mandir `%s'.\n", mandir);
}

/* accept (NULL or oldpath) and new path component. return new path */
static char *pathappend (char *oldpath, const char *appendage)
{
	assert ((!oldpath || *oldpath) && appendage);
	/* Remove duplicates */
	if (oldpath) {
		char *oldpathtok = xstrdup (oldpath), *tok;
		char *app_dedup = xstrdup (appendage);
		char *oldpathtok_ptr = oldpathtok;
		for (tok = strsep (&oldpathtok_ptr, ":"); tok;
		     tok = strsep (&oldpathtok_ptr, ":")) {
			char *search;
			if (!*tok)	    /* ignore empty fields */
				continue;
			search = strstr (app_dedup, tok);
			while (search) {
				char *terminator = search + strlen (tok);
				if (!*terminator) {
					/* End of the string, so chop here. */
					*search = 0;
					while (search > app_dedup &&
					       *--search == ':')
						*search = 0;
					break;
				} else if (*terminator == ':') {
					char *newapp;
					*search = 0;
					newapp = xasprintf ("%s%s", app_dedup,
							    terminator + 1);
					free (app_dedup);
					app_dedup = newapp;
				}
				search = strstr (terminator, tok);
			}
		}
		free (oldpathtok);
		if (!STREQ (appendage, app_dedup))
			debug ("%s:%s reduced to %s%s%s\n",
			       oldpath, appendage,
			       oldpath, *app_dedup ? ":" : "", app_dedup);
		if (*app_dedup)
			oldpath = appendstr (oldpath, ":", app_dedup,
					     (void *) 0);
		free (app_dedup);
		return oldpath;
	} else
		return xstrdup (appendage);
}

static void gripe_reading_mp_config (const char *file)
{
	error (FAIL, 0,
	       _("can't make sense of the manpath configuration file %s"),
	       file);
}

static void gripe_stat_file (const char *file)
{
	debug_error (_("warning: %s"), file);
}

static void gripe_not_directory (const char *dir)
{
	if (!quiet)
		error (0, 0, _("warning: %s isn't a directory"), dir);
}

/* accept a manpath list, separated with ':', return the associated 
   catpath list */
char *cat_manpath (char *manp)
{
	char *catp = NULL;
	const char *path, *catdir;

	for (path = strsep (&manp, ":"); path; path = strsep (&manp, ":")) {
		catdir = get_config (path, MANDB_MAP_USER);
		if (!catdir)
			catdir = get_config (path, MANDB_MAP);
		catp = catdir ? pathappend (catp, catdir) 
			      : pathappend (catp, path);
	}

	return catp;
}		

/* Unpack a glibc-style locale into its component parts.
 *
 * This function was inspired by _nl_explode_name in libintl; I've rewritten
 * it here with extensive modifications in order not to require libintl or
 * glibc internals, because this API is more convenient for man-db, and to
 * be consistent with surrounding style. I also dropped the normalised
 * codeset handling, which we don't need here.
 */
void unpack_locale_bits (const char *locale, struct locale_bits *bits)
{
	const char *p, *start;

	bits->language = NULL;
	bits->territory = NULL;
	bits->codeset = NULL;
	bits->modifier = NULL;

	/* Now we determine the single parts of the locale name. First look
	 * for the language. Termination symbols are '_', '.', and '@'.
	 */
	p = locale;
	while (*p && *p != '_' && *p != '.' && *p != '@')
		++p;
	if (p == locale) {
		/* This does not make sense: language has to be specified.
		 * Use this entry as it is without exploding. Perhaps it is
		 * an alias.
		 */
		bits->language = xstrdup (locale);
		goto out;
	}
	bits->language = xstrndup (locale, p - locale);

	if (*p == '_') {
		/* Next is the territory. */
		start = ++p;
		while (*p && *p != '.' && *p != '@')
			++p;
		bits->territory = xstrndup (start, p - start);
	}

	if (*p == '.') {
		/* Next is the codeset. */
		start = ++p;
		while (*p && *p != '@')
			++p;
		bits->codeset = xstrndup (start, p - start);
	}

	if (*p == '@')
		/* Next is the modifier. */
		bits->modifier = xstrdup (++p);

out:
	if (!bits->territory)
		bits->territory = xstrdup ("");
	if (!bits->codeset)
		bits->codeset = xstrdup ("");
	if (!bits->modifier)
		bits->modifier = xstrdup ("");
}

/* Free the contents of a locale_bits structure populated by
 * unpack_locale_bits. Does not free the pointer argument.
 */
void free_locale_bits (struct locale_bits *bits)
{
	free (bits->language);
	free (bits->territory);
	free (bits->codeset);
	free (bits->modifier);
}


static char *get_nls_manpath (const char *manpathlist, const char *locale)
{
	struct locale_bits lbits;
	char *manpath = NULL;
	char *manpathlist_copy, *path, *manpathlist_ptr;

	unpack_locale_bits (locale, &lbits);
	if (STREQ (lbits.language, "C") || STREQ (lbits.language, "POSIX")) {
		free_locale_bits (&lbits);
		return xstrdup (manpathlist);
	}

	manpathlist_copy = xstrdup (manpathlist);
	manpathlist_ptr = manpathlist_copy;
	for (path = strsep (&manpathlist_ptr, ":"); path;
	     path = strsep (&manpathlist_ptr, ":")) {
		DIR *mandir = opendir (path);
		struct dirent *mandirent;

		if (!mandir)
			continue;

		while ((mandirent = readdir (mandir)) != NULL) {
			const char *name;
			struct locale_bits mbits;
			char *fullpath;

			name = mandirent->d_name;
			if (STREQ (name, ".") || STREQ (name, ".."))
				continue;
			if (STRNEQ (name, "man", 3))
				continue;
			fullpath = xasprintf ("%s/%s", path, name);
			if (is_directory (fullpath) != 1) {
				free (fullpath);
				continue;
			}

			unpack_locale_bits (name, &mbits);
			if (STREQ (lbits.language, mbits.language) &&
			    (!*mbits.territory ||
			     STREQ (lbits.territory, mbits.territory)) &&
			    (!*mbits.modifier ||
			     STREQ (lbits.modifier, mbits.modifier)))
				manpath = pathappend (manpath, fullpath);
			free_locale_bits (&mbits);
			free (fullpath);
		}

		if (STREQ (lbits.language, "en"))
			/* For English, we look in the subdirectories as
			 * above just in case there's something like
			 * en_GB.UTF-8, but it's more probable that English
			 * manual pages reside at the top level.
			 */
			manpath = pathappend (manpath, path);

		closedir (mandir);
	}
	free (manpathlist_copy);

	free_locale_bits (&lbits);
	return manpath;
}

char *add_nls_manpaths (const char *manpathlist, const char *locales)
{
	char *manpath = NULL;
	char *locales_copy, *tok, *locales_ptr;
	char *locale_manpath;

	debug ("add_nls_manpaths(): processing %s\n", manpathlist);

	if (locales == NULL || *locales == '\0')
		return xstrdup (manpathlist);

	/* For each locale, we iterate over the manpath and find appropriate
	 * locale directories for each item. We then concatenate the results
	 * for all locales. In other words, LANGUAGE=fr:de and
	 * manpath=/usr/share/man:/usr/local/share/man could result in
	 * something like this list:
	 *
	 *   /usr/share/man/fr
	 *   /usr/local/share/man/fr
	 *   /usr/share/man/de
	 *   /usr/local/share/man/de
	 *   /usr/share/man
	 *   /usr/local/share/man
	 *
	 * This assumes that it's more important to have documentation in
	 * the preferred language than to have documentation for the correct
	 * object (in the case where there are different versions of a
	 * program in different hierarchies, for example). It is not
	 * entirely obvious that this is the right assumption, but on the
	 * other hand the other choice is not entirely obvious either. We
	 * tie-break on "we've always done it this way", and people can use
	 * 'man -a' or whatever in the occasional case where we get it
	 * wrong.
	 *
	 * We go to no special effort to de-duplicate directories here.
	 * create_pathlist will sort it out later; note that it preserves
	 * order in that it keeps the first of any duplicate set in its
	 * original position.
	 */

	locales_copy = xstrdup (locales);
	locales_ptr = locales_copy;
	for (tok = strsep (&locales_ptr, ":"); tok;
	     tok = strsep (&locales_ptr, ":")) {
		if (!*tok)	/* ignore empty fields */
			continue;
		debug ("checking for locale %s\n", tok);

		locale_manpath = get_nls_manpath (manpathlist, tok);
		if (locale_manpath) {
			if (manpath)
				manpath = appendstr (manpath, ":",
						     locale_manpath,
						     (void *) 0);
			else
				manpath = xstrdup (locale_manpath);
			free (locale_manpath);
		}
	}
	free (locales_copy);

	/* Always try untranslated pages as a last resort. */
	locale_manpath = get_nls_manpath (manpathlist, "C");
	if (locale_manpath) {
		if (manpath)
			manpath = appendstr (manpath, ":",
					     locale_manpath, (void *) 0);
		else
			manpath = xstrdup (locale_manpath);
		free (locale_manpath);
	}

	return manpath;
}

static char *add_system_manpath (const char *systems, const char *manpathlist)
{
	char *one_system;
	char *manpath = NULL;
	char *tmpsystems;

	if (!systems)
		systems = getenv ("SYSTEM");

	if (!systems || !*systems)
		return xstrdup (manpathlist);

	/* Avoid breaking the environment. */
	tmpsystems = xstrdup (systems);

	/* For each systems component */

	for (one_system = strtok (tmpsystems, ",:"); one_system;
	     one_system = strtok (NULL, ",:")) {

		/* For each manpathlist component */

		if (!STREQ (one_system, "man")) {
			const char *next, *path;
			char *newdir = NULL;
			for (path = manpathlist; path; path = next) {
				int status;
				char *element;

				next = strchr (path, ':');
				if (next) {
					element = xstrndup (path, next - path);
					++next;
				} else
					element = xstrdup (path);
				newdir = appendstr (newdir, element, "/",
						    one_system, (void *) 0);
				free (element);

				status = is_directory (newdir);

				if (status == 0)
					gripe_not_directory (newdir);
				else if (status == 1) {
					debug ("adding %s to manpathlist\n",
					       newdir);
					manpath = pathappend (manpath, newdir);
				} else
					debug_error ("can't stat %s", newdir);
				/* reset newdir */
				*newdir = '\0';
			}
			free (newdir);
		} else
			manpath = pathappend (manpath, manpathlist);
	}
	free (tmpsystems);

	/*
	 * Thu, 21 Nov 1996 22:24:19 +0200 fpolacco@debian.org
	 * bug#5534 (man fails if env var SYSTEM is defined)
	 * with error [man: internal manpath equates to NULL]
	 * the reason: is_directory (newdir); returns -1
	 */
	if (!manpath) {
		debug ("add_system_manpath(): "
		       "internal manpath equates to NULL\n");
		return xstrdup (manpathlist);
	}
	return manpath;
}

/*
 * Always add system and locale directories to pathlist.
 * If the environment variable MANPATH is set, return it.
 * If the environment variable PATH is set and has a nonzero length,
 * try to determine the corresponding manpath, otherwise, return the
 * default manpath.
 *
 * The man_db.config file is used to map system wide /bin directories
 * to top level man page directories.
 *
 * For directories which are in the user's path but not in the
 * man_db.config file, see if there is a subdirectory `man' or `MAN'.
 * If so, add that directory to the path.  Example:  user has
 * $HOME/bin in his path and the directory $HOME/bin/man exists -- the
 * directory $HOME/bin/man will be added to the manpath.
 */
static char *guess_manpath (const char *systems)
{
	const char *path = getenv ("PATH");
	char *manpathlist, *manpath;

	if (path == NULL || getenv ("MAN_TEST_DISABLE_PATH")) {
		/* Things aren't going to work well, but hey... */
		if (path == NULL && !quiet)
			error (0, 0, _("warning: $PATH not set"));

		manpathlist = def_path (MANDATORY);
	} else {
		if (strlen (path) == 0) {
			/* Things aren't going to work well here either... */
			if (!quiet)
				error (0, 0, _("warning: empty $PATH"));
			
			return add_system_manpath (systems,
						   def_path (MANDATORY));
		}

		manpathlist = get_manpath_from_path (path, 1);
	}
	manpath = add_system_manpath (systems, manpathlist);
	free (manpathlist);
	return manpath;
}

char *get_manpath (const char *systems)
{
	char *manpathlist;

	/* need to read config file even if MANPATH set, for mandb(8) */
	read_config_file (false);

	manpathlist = getenv ("MANPATH");
	if (manpathlist && *manpathlist) {
		char *system1, *system2, *guessed;
		char *pos;
		/* This must be it. */
		if (manpathlist[0] == ':') {
			if (!quiet)
				error (0, 0,
				       _("warning: $MANPATH set, "
					 "prepending %s"),
				       CONFIG_FILE);
			system1 = add_system_manpath (systems, manpathlist);
			guessed = guess_manpath (systems);
			manpathlist = xasprintf ("%s%s", guessed, system1);
			free (guessed);
			free (system1);
		} else if (manpathlist[strlen (manpathlist) - 1] == ':') {
			if (!quiet)
				error (0, 0,
				       _("warning: $MANPATH set, "
					 "appending %s"),
				       CONFIG_FILE);
			system1 = add_system_manpath (systems, manpathlist);
			guessed = guess_manpath (systems);
			manpathlist = xasprintf ("%s%s", system1, guessed);
			free (guessed);
			free (system1);
		} else if ((pos = strstr (manpathlist,"::"))) {
			*(pos++) = '\0';
			if (!quiet)
				error (0, 0,
				       _("warning: $MANPATH set, "
					 "inserting %s"),
				       CONFIG_FILE);
			system1 = add_system_manpath (systems, manpathlist);
			guessed = guess_manpath (systems);
			system2 = add_system_manpath (systems, pos);
			manpathlist = xasprintf ("%s:%s%s", system1, guessed,
						 system2);
			free (system2);
			free (guessed);
			free (system1);
		} else {
			if (!quiet)
				error (0, 0,
				       _("warning: $MANPATH set, ignoring %s"),
				       CONFIG_FILE);
			manpathlist = add_system_manpath (systems,
							  manpathlist);
		}
	} else
		manpathlist = guess_manpath (systems);

	return manpathlist;
}

/* Parse the manpath.config file, extracting appropriate information. */
static void add_to_dirlist (FILE *config_file, int user)
{
	char *bp;
	char *buf = NULL;
	size_t n = 0;
	char key[512], cont[512];
	int val;
	int c;

	while (getline (&buf, &n, config_file) >= 0) {
		bp = buf;

		while (CTYPE (isspace, *bp))
			bp++;

		/* TODO: would like a (limited) replacement for sscanf()
		 * here that allocates its own memory. At that point check
		 * everything that sprintf()s manpath et al!
		 */
		if (*bp == '#' || *bp == '\0')
			goto next;
		else if (strncmp (bp, "NOCACHE", 7) == 0)
			disable_cache = true;
		else if (strncmp (bp, "NO", 2) == 0)
			goto next;	/* match any word starting with NO */
		else if (sscanf (bp, "MANBIN %*s") == 1)
			goto next;
		else if (sscanf (bp, "MANDATORY_MANPATH %511s", key) == 1)
			add_mandatory (key);	
		else if (sscanf (bp, "MANPATH_MAP %511s %511s",
			 key, cont) == 2) 
			add_manpath_map (key, cont);
		else if ((c = sscanf (bp, "MANDB_MAP %511s %511s",
				      key, cont)) > 0) 
			add_mandb_map (key, c == 2 ? cont : key, user);
		else if ((c = sscanf (bp, "DEFINE %511s %511[^\n]",
				      key, cont)) > 0)
			add_def (key, c == 2 ? cont : "", user);
		else if (sscanf (bp, "SECTION %511[^\n]", cont) == 1)
			add_sections (cont, user);
		else if (sscanf (bp, "SECTIONS %511[^\n]", cont) == 1)
			/* Since I keep getting it wrong ... */
			add_sections (cont, user);
		else if (sscanf (bp, "MINCATWIDTH %d", &val) == 1)
			min_cat_width = val;
		else if (sscanf (bp, "MAXCATWIDTH %d", &val) == 1)
			max_cat_width = val;
		else if (sscanf (bp, "CATWIDTH %d", &val) == 1)
			cat_width = val;
	 	else {
			error (0, 0, _("can't parse directory list `%s'"), bp);
			gripe_reading_mp_config (CONFIG_FILE);
		}

next:
		free (buf);
		buf = NULL;
	}

	free (buf);
}

static void free_config_file (void *unused _GL_UNUSED)
{
	gl_list_free (config);
}

void read_config_file (bool optional)
{
	static int done = 0;
	char *dotmanpath = NULL;
	FILE *config_file;

	if (done)
		return;

	config = gl_list_create_empty (GL_ARRAY_LIST, NULL, NULL,
				       config_item_free, true);
	push_cleanup (free_config_file, NULL, 0);

	if (user_config_file)
		dotmanpath = xstrdup (user_config_file);
	else {
		char *home = getenv ("HOME");
		if (home)
			dotmanpath = xasprintf ("%s/.manpath", home);
	}
	if (dotmanpath) {
		config_file = fopen (dotmanpath, "r");
		if (config_file != NULL) {
			debug ("From the config file %s:\n\n", dotmanpath);
			add_to_dirlist (config_file, 1);
			fclose (config_file);
		}
		free (dotmanpath);
	}

	if (getenv ("MAN_TEST_DISABLE_SYSTEM_CONFIG") == NULL) {
		config_file = fopen (CONFIG_FILE, "r");
		if (config_file == NULL) {
			if (optional)
				debug ("can't open %s; continuing anyway\n",
				       CONFIG_FILE);
			else
				error (FAIL, 0,
				       _("can't open the manpath "
					 "configuration file %s"),
				       CONFIG_FILE);
		} else {
			debug ("From the config file %s:\n\n", CONFIG_FILE);

			add_to_dirlist (config_file, 0);
			fclose (config_file);
		}
	}

	print_list ();

	done = 1;
}


/*
 * Construct the default manpath.  This picks up mandatory manpaths
 * only.
 */
static char *def_path (enum config_flag flag)
{
	char *manpath = NULL;
	const struct config_item *item;

	GL_LIST_FOREACH_START (config, item)
		if (item->flag == flag) {
			gl_list_t expanded_dirs;
			const char *expanded_dir;

			expanded_dirs = expand_path (item->key);
			GL_LIST_FOREACH_START (expanded_dirs, expanded_dir) {
				int status = is_directory (expanded_dir);

				if (status < 0)
					gripe_stat_file (expanded_dir);
				else if (status == 0 && !quiet)
					error (0, 0,
					       _("warning: mandatory "
						 "directory %s doesn't exist"),
					       expanded_dir);
				else if (status == 1)
					manpath = pathappend (manpath,
							      expanded_dir);
			} GL_LIST_FOREACH_END (expanded_dirs);
			gl_list_free (expanded_dirs);
		}
	GL_LIST_FOREACH_END (config);

	/* If we have complete config file failure... */
	if (!manpath)
		return xstrdup ("/usr/man");

	return manpath;
}

/*
 * If specified with configure, append OVERRIDE_DIR to dir param and add it
 * to list.
 */
static void insert_override_dir (gl_list_t list, const char *dir)
{
	char *override_dir = NULL;

	if (!strlen (OVERRIDE_DIR))
		return;

	if ((override_dir = xasprintf ("%s/%s", dir, OVERRIDE_DIR))) {
		add_dir_to_list (list, override_dir);
		free (override_dir);
	}
}

/*
 * For each directory in the user's path, see if it is one of the
 * directories listed in the man_db.config file.  If so, and it is
 * not already in the manpath, add it.  If the directory is not listed
 * in the man_db.config file, see if there is a subdirectory `../man' or
 * `man', or, for FHS-compliance, `../share/man' or `share/man'.  If so,
 * and it is not already in the manpath, add it.
 * Example:  user has $HOME/bin in his path and the directory
 * $HOME/man exists -- the directory $HOME/man will be added
 * to the manpath.
 */
char *get_manpath_from_path (const char *path, int mandatory)
{
	gl_list_t tmplist;
	const struct config_item *config_item;
	int len;
	char *tmppath;
	char *p;
	char *end;
	char *manpathlist;
	char *item;

	tmplist = new_string_list (GL_LINKEDHASH_LIST, false);
	tmppath = xstrdup (path);

	for (end = p = tmppath; end; p = end + 1) {
		bool manpath_map_found = false;

		end = strchr (p, ':');
		if (end)
			*end = '\0';

		/* don't do this for current dir ("." or empty entry in PATH) */
		if (*p == '\0' || strcmp (p, ".") == 0)
			continue;

		debug ("\npath directory %s ", p);

		/* If the directory we're working on has MANPATH_MAP entries
		 * in the config file, add them to the list.
		 */
		GL_LIST_FOREACH_START (config, config_item) {
			if (MANPATH_MAP != config_item->flag ||
			    !STREQ (p, config_item->key))
				continue;
			if (!manpath_map_found)
				debug ("is in the config file\n");
			manpath_map_found = true;
			insert_override_dir (tmplist, config_item->cont);
			add_dir_to_list (tmplist, config_item->cont);
		} GL_LIST_FOREACH_END (config);

		 /* The directory we're working on isn't in the config file.  
		    See if it has ../man, man, ../share/man, or share/man
		    subdirectories.  If so, and they haven't been added to
		    the list, do. */

		if (!manpath_map_found) {
			debug ("is not in the config file\n");
			add_man_subdirs (tmplist, p);
		}
	}

	free (tmppath);

	if (mandatory) {
		debug ("\nadding mandatory man directories\n\n");

		GL_LIST_FOREACH_START (config, config_item) {
			if (config_item->flag == MANDATORY) {
				insert_override_dir (tmplist,
						     config_item->key);
				add_dir_to_list (tmplist, config_item->key);
			}
		} GL_LIST_FOREACH_END (config);
	}

	len = 0;
	GL_LIST_FOREACH_START (tmplist, item)
		len += strlen (item) + 1;
	GL_LIST_FOREACH_END (tmplist);

	if (!len)
		/* No path elements in configuration file or with
		 * appropriate subdirectories.
		 */
		return xstrdup ("");

	manpathlist = xmalloc (len);
	*manpathlist = '\0';

	p = manpathlist;
	GL_LIST_FOREACH_START (tmplist, item) {
		len = strlen (item);
		memcpy (p, item, len);
		p += len;
		*p++ = ':';
	} GL_LIST_FOREACH_END (tmplist);

	p[-1] = '\0';

	gl_list_free (tmplist);

	return manpathlist;
}

/* Add a directory to the manpath list if it isn't already there. */
static void add_expanded_dir_to_list (gl_list_t list, const char *dir)
{
	int status;

	if (gl_list_search (list, dir)) {
		debug ("%s is already in the manpath\n", dir);
		return;
	}

	/* Not found -- add it. */

	status = is_directory (dir);

	if (status < 0)
		gripe_stat_file (dir);
	else if (status == 0)
		gripe_not_directory (dir);
	else if (status == 1) {
		debug ("adding %s to manpath\n", dir);
		gl_list_add_last (list, xstrdup (dir));
	}
}

/*
 * Add a directory to the manpath list if it isn't already there, expanding
 * wildcards.
 */
static void add_dir_to_list (gl_list_t list, const char *dir)
{
	gl_list_t expanded_dirs;
	const char *expanded_dir;

	expanded_dirs = expand_path (dir);
	GL_LIST_FOREACH_START (expanded_dirs, expanded_dir)
		add_expanded_dir_to_list (list, expanded_dir);
	GL_LIST_FOREACH_END (expanded_dirs);
	gl_list_free (expanded_dirs);
}

/* path does not exist in config file: check to see if path/../man,
   path/man, path/../share/man, or path/share/man exist, and add them to the
   list if they do. */
static void add_man_subdirs (gl_list_t list, const char *path)
{
	char *newpath;
	int found = 0;

	/* don't assume anything about path, especially that it ends in 
	   "bin" or even has a '/' in it! */
	   
	char *subdir = strrchr (path, '/');
	if (subdir) {
		newpath = xasprintf ("%.*s/man", (int) (subdir - path), path);
		if (is_directory (newpath) == 1) {
			insert_override_dir (list, newpath);
			add_dir_to_list (list, newpath);
			found = 1;
		}
		free (newpath);
	}

	newpath = xasprintf ("%s/man", path);
	if (is_directory (newpath) == 1) {
		insert_override_dir (list, newpath);
		add_dir_to_list (list, newpath);
		found = 1;
	}
	free (newpath);

	if (subdir) {
		newpath = xasprintf ("%.*s/share/man",
				     (int) (subdir - path), path);
		if (is_directory (newpath) == 1) {
			insert_override_dir (list, newpath);
			add_dir_to_list (list, newpath);
			found = 1;
		}
		free (newpath);
	}

	newpath = xasprintf ("%s/share/man", path);
	if (is_directory (newpath) == 1) {
		insert_override_dir (list, newpath);
		add_dir_to_list (list, newpath);
		found = 1;
	}
	free (newpath);

	if (!found)
		debug ("and doesn't have ../man, man, ../share/man, or "
		       "share/man subdirectories\n");
}

static void add_dir_to_path_list (gl_list_t list, const char *p)
{
	gl_list_t expanded_dirs;
	char *expanded_dir;

	expanded_dirs = expand_path (p);
	GL_LIST_FOREACH_START (expanded_dirs, expanded_dir) {
		int status = is_directory (expanded_dir);

		if (status < 0)
			gripe_stat_file (expanded_dir);
		else if (status == 0)
			gripe_not_directory (expanded_dir);
		else {
			char *path;

			/* deal with relative paths */
			if (*expanded_dir != '/') {
				char *cwd = xgetcwd ();
				if (!cwd)
					error (FATAL, errno,
							_("can't determine current directory"));
				path = appendstr (cwd, "/", expanded_dir,
						  (void *) 0);
			} else
				path = xstrdup (expanded_dir);

			debug ("adding %s to manpathlist\n", path);
			gl_list_add_last (list, path);
		}
	} GL_LIST_FOREACH_END (expanded_dirs);
	gl_list_free (expanded_dirs);
}

gl_list_t create_pathlist (const char *manp)
{
	gl_list_t list;
	gl_list_iterator_t iter;
	gl_list_node_t node;
	const char *p, *end;

	/* Expand the manpath into a list for easier handling. */

	list = new_string_list (GL_LINKEDHASH_LIST, true);
	for (p = manp;; p = end + 1) {
		end = strchr (p, ':');
		if (end) {
			char *element = xstrndup (p, end - p);
			add_dir_to_path_list (list, element);
			free (element);
		} else {
			add_dir_to_path_list (list, p);
			break;
		}
	}

	/* Eliminate duplicates due to symlinks. */
	iter = gl_list_iterator (list);
	while (gl_list_iterator_next (&iter, (const void **) &p, &node)) {
		char *target;
		gl_list_iterator_t dupcheck_iter;
		const char *dupcheck;
		gl_list_node_t dupcheck_node;

		/* After resolving all symlinks, is the target also in the
		 * manpath?
		 */
		target = canonicalize_file_name (p);
		if (!target)
			continue;
		/* Only check up to the current list position, to keep item
		 * order stable across deduplication.
		 */
		dupcheck_iter = gl_list_iterator (list);
		while (gl_list_iterator_next (&dupcheck_iter,
					      (const void **) &dupcheck,
					      &dupcheck_node) &&
		       dupcheck_node != node) {
			char *dupcheck_target = canonicalize_file_name
				(dupcheck);
			if (!dupcheck_target)
				continue;
			if (!STREQ (target, dupcheck_target)) {
				free (dupcheck_target);
				continue;
			}
			free (dupcheck_target);
			debug ("Removing duplicate manpath entry %s -> %s\n",
			       p, dupcheck);
			gl_list_remove_node (list, node);
			break;
		}
		gl_list_iterator_free (&dupcheck_iter);
		free (target);
	}
	gl_list_iterator_free (&iter);

	if (debug_level) {
		bool first = true;

		debug ("final search path = ");
		GL_LIST_FOREACH_START (list, p) {
			if (first) {
				debug ("%s", p);
				first = false;
			} else
				debug (":%s", p);
		} GL_LIST_FOREACH_END (list);
		debug ("\n");
	}

	return list;
}

void free_pathlist (gl_list_t list)
{
	gl_list_free (list);
}

/* Routine to get list of named system and user manpaths (in reverse order). */
char *get_mandb_manpath (void)
{
	char *manpath = NULL;
	const struct config_item *item;

	GL_LIST_FOREACH_START (config, item)
		if (item->flag == MANDB_MAP || item->flag == MANDB_MAP_USER)
			manpath = pathappend (manpath, item->key);
	GL_LIST_FOREACH_END (config);

	return manpath;
}

/* Take manpath or manfile path as the first argument, and the type of
 * catpaths we want as the other (system catpaths, user catpaths, or both).
 * Return catdir mapping or NULL if it isn't a global/user mandir (as
 * appropriate).
 *
 * This routine would seem to work correctly for nls subdirs and would 
 * specify the (correct) consistent catpath even if not defined in the 
 * config file.
 *
 * Do not return user catpaths when cattype == 0! This is used to decide
 * whether to drop privileges. When cattype != 0 it's OK to return global
 * catpaths.
 */
char *get_catpath (const char *name, int cattype)
{
	const struct config_item *item;
	char *ret = NULL;

	GL_LIST_FOREACH_START (config, item)
		if (((cattype & SYSTEM_CAT) && item->flag == MANDB_MAP) ||
		    ((cattype & USER_CAT)   && item->flag == MANDB_MAP_USER)) {
			size_t manlen = strlen (item->key);
			if (STRNEQ (name, item->key, manlen)) {
				const char *suffix;
				char *infix;
				char *catpath = xstrdup (item->cont);

				/* For NLS subdirectories (e.g.
				 * /usr/share/man/de -> /var/cache/man/de),
				 * we need to find the second-last slash, as
				 * long as this strictly follows the key.
				 */
				suffix = strrchr (name, '/');
				if (!suffix) {
					ret = appendstr (catpath,
							 name + manlen,
							 (void *) 0);
					break;
				}

				while (suffix > name + manlen)
					if (*--suffix == '/')
						break;
				if (suffix < name + manlen)
					suffix = name + manlen;
				if (*suffix == '/')
					++suffix;
				infix = xstrndup (name + manlen,
						  suffix - (name + manlen));
				catpath = appendstr (catpath, infix,
						     (void *) 0);
				free (infix);
				if (STRNEQ (suffix, "man", 3)) {
					suffix += 3;
					catpath = appendstr (catpath, "cat",
							     (void *) 0);
				}
				catpath = appendstr (catpath, suffix,
						     (void *) 0);
			  	ret = catpath;
				break;
			}
		}
	GL_LIST_FOREACH_END (config);

	return ret;
}

/* Check to see if the supplied man directory is a system-wide mandir.
 * Obviously, user directories must not be included here.
 */
bool is_global_mandir (const char *dir)
{
	const struct config_item *item;
	bool ret = false;

	GL_LIST_FOREACH_START (config, item)
		if (item->flag == MANDB_MAP &&
		    STRNEQ (dir, item->key, strlen (item->key))) {
		    	ret = true;
			break;
		}
	GL_LIST_FOREACH_END (config);

	return ret;
}

/* Accept a manpath (not a full pathname to a file) and return an FSSTND 
   equivalent catpath */
static char *fsstnd (const char *path)
{
	char *manpath;
	char *catpath;
	char *element;
	
	if (strncmp (path, MAN_ROOT, sizeof MAN_ROOT - 1) != 0) {
		if (!quiet)
			error (0, 0, _("warning: %s does not begin with %s"),
			       path, MAN_ROOT);
		return xstrdup (path);
	}
	/* get rid of initial "/usr" */
	path += sizeof MAN_ROOT - 1;
	manpath = xstrdup (path);
	catpath = xmalloc (strlen (path) + sizeof CAT_ROOT - 3);

	/* start with CAT_ROOT */ 
	(void) strcpy (catpath, CAT_ROOT);

	/* split up path into elements and deal with accordingly */
	for (element = strtok (manpath, "/"); element;
	     element = strtok (NULL, "/")) {
		if (strncmp (element, "man", 3) == 0) {
			if (*(element + 3)) { 
				*element = 'c';
				*(element + 2) = 't';
			} else
				continue;
		} 
		(void) strcat (catpath, "/");
		(void) strcat (catpath, element);
	}
	free (manpath);
	return catpath;
}
