/*
 * globbing.c: interface to the POSIX glob routines
 *  
 * Copyright (C) 1995 Graeme W. Wilford. (Wilf.)
 * Copyright (C) 2001, 2002, 2003, 2006, 2007, 2008 Colin Watson.
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
 * Mon Mar 13 20:27:36 GMT 1995  Wilf. (G.Wilford@ee.surrey.ac.uk) 
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <glob.h>
#include <sys/types.h>
#include <dirent.h>

#include "fnmatch.h"
#include "gl_array_list.h"
#include "gl_hash_map.h"
#include "gl_xlist.h"
#include "gl_xmap.h"
#include "regex.h"
#include "xvasprintf.h"

#include "manconfig.h"

#include "error.h"
#include "glcontainers.h"
#include "cleanup.h"
#include "xregcomp.h"

#include "globbing.h"

const char *extension;
static const char *mandir_layout = MANDIR_LAYOUT;

static char *make_pattern (const char *name, const char *sec, int opts)
{
	char *pattern;

	if (opts & LFF_REGEX) {
		if (extension) {
			char *esc_ext = escape_shell (extension);
			pattern = xasprintf ("%s\\..*%s.*", name, esc_ext);
			free (esc_ext);
		} else {
			char *esc_sec = escape_shell (sec);
			pattern = xasprintf ("%s\\.%s.*", name, esc_sec);
			free (esc_sec);
		}
	} else {
		if (extension)
			pattern = xasprintf ("%s.*%s*", name, extension);
		else
			pattern = xasprintf ("%s.%s*", name, sec);
	}

	return pattern;
}

#define LAYOUT_GNU	1
#define LAYOUT_HPUX	2
#define LAYOUT_IRIX	4
#define LAYOUT_SOLARIS	8
#define LAYOUT_BSD	16

static int parse_layout (const char *layout)
{
	if (!*layout)
		return LAYOUT_GNU | LAYOUT_HPUX | LAYOUT_IRIX |
		       LAYOUT_SOLARIS | LAYOUT_BSD;
	else {
		int flags = 0;

		char *upper_layout = xstrdup (layout);
		char *layoutp;
		for (layoutp = upper_layout; *layoutp; layoutp++)
			*layoutp = CTYPE (toupper, *layoutp);

		if (strstr (upper_layout, "GNU"))
			flags |= LAYOUT_GNU;
		if (strstr (upper_layout, "HPUX"))
			flags |= LAYOUT_HPUX;
		if (strstr (upper_layout, "IRIX"))
			flags |= LAYOUT_IRIX;
		if (strstr (upper_layout, "SOLARIS"))
			flags |= LAYOUT_SOLARIS;
		if (strstr (upper_layout, "BSD"))
			flags |= LAYOUT_BSD;

		free (upper_layout);
		return flags;
	}
}

struct dirent_names {
	char **names;
	size_t names_len, names_max;
};

static void dirent_names_free (const void *value)
{
	struct dirent_names *cache = (struct dirent_names *) value;
	size_t i;

	for (i = 0; i < cache->names_len; ++i)
		free (cache->names[i]);
	free (cache->names);
	free (cache);
}

static gl_map_t dirent_map = NULL;

static int cache_compare (const void *a, const void *b)
{
	const char *left = *(const char **) a;
	const char *right = *(const char **) b;
	return strcasecmp (left, right);
}

static struct dirent_names *update_directory_cache (const char *path)
{
	struct dirent_names *cache;
	DIR *dir;
	struct dirent *entry;

	if (!dirent_map) {
		dirent_map = new_string_map (GL_HASH_MAP, dirent_names_free);
		push_cleanup ((cleanup_fun) gl_map_free, dirent_map, 0);
	}
	cache = (struct dirent_names *) gl_map_get (dirent_map, path);

	/* Check whether we've got this one already. */
	if (cache) {
		debug ("update_directory_cache %s: hit\n", path);
		return cache;
	}

	debug ("update_directory_cache %s: miss\n", path);

	dir = opendir (path);
	if (!dir) {
		debug_error ("can't open directory %s", path);
		return NULL;
	}

	cache = XMALLOC (struct dirent_names);
	cache->names_len = 0;
	cache->names_max = 1024;
	cache->names = XNMALLOC (cache->names_max, char *);

	/* Dump all the entries into cache->names, resizing if necessary. */
	for (entry = readdir (dir); entry; entry = readdir (dir)) {
		if (cache->names_len >= cache->names_max) {
			cache->names_max *= 2;
			cache->names =
				xnrealloc (cache->names, cache->names_max,
					   sizeof (char *));
		}
		cache->names[cache->names_len++] = xstrdup (entry->d_name);
	}

	qsort (cache->names, cache->names_len, sizeof *cache->names,
	       &cache_compare);

	gl_map_put (dirent_map, xstrdup (path), cache);
	closedir (dir);

	return cache;
}

struct pattern_bsearch {
	char *pattern;
	size_t len;
};

static int pattern_compare (const void *a, const void *b)
{
	const struct pattern_bsearch *key = a;
	const char *memb = *(const char **) b;
	return strncasecmp (key->pattern, memb, key->len);
}

static void match_in_directory (const char *path, const char *pattern,
				int opts, gl_list_t matched)
{
	struct dirent_names *cache;
	int flags;
	regex_t preg;
	struct pattern_bsearch pattern_start = { NULL, -1 };
	char **bsearched;
	size_t i;

	cache = update_directory_cache (path);
	if (!cache) {
		debug ("directory cache update failed\n");
		return;
	}

	debug ("globbing pattern in %s: %s\n", path, pattern);

	if (opts & LFF_REGEX)
		flags = REG_EXTENDED | REG_NOSUB |
			((opts & LFF_MATCHCASE) ? 0 : REG_ICASE);
	else
		flags = (opts & LFF_MATCHCASE) ? 0 : FNM_CASEFOLD;

	if (opts & LFF_REGEX) {
		xregcomp (&preg, pattern, flags);
		bsearched = cache->names;
	} else {
		pattern_start.pattern = xstrndup (pattern,
						  strcspn (pattern, "?*{}\\"));
		pattern_start.len = strlen (pattern_start.pattern);
		bsearched = bsearch (&pattern_start, cache->names,
				     cache->names_len, sizeof *cache->names,
				     &pattern_compare);
		if (!bsearched) {
			free (pattern_start.pattern);
			return;
		}
		while (bsearched > cache->names &&
		       !strncasecmp (pattern_start.pattern, *(bsearched - 1),
				     pattern_start.len))
			--bsearched;
	}

	for (i = bsearched - cache->names; i < cache->names_len; ++i) {
		if (opts & LFF_REGEX) {
			if (regexec (&preg, cache->names[i], 0, NULL, 0) != 0)
				continue;
		} else {
			if (strncasecmp (pattern_start.pattern,
					 cache->names[i], pattern_start.len))
				break;

			if (fnmatch (pattern, cache->names[i], flags) != 0)
				continue;
		}

		debug ("matched: %s/%s\n", path, cache->names[i]);

		gl_list_add_last (matched,
				  xasprintf ("%s/%s", path, cache->names[i]));
	}

	if (opts & LFF_REGEX)
		regfree (&preg);
	else
		free (pattern_start.pattern);
}

gl_list_t look_for_file (const char *hier, const char *sec,
			 const char *unesc_name, int cat, int opts)
{
	gl_list_t matched;
	char *pattern, *path = NULL;
	static int layout = -1;
	char *name;

	matched = new_string_list (GL_ARRAY_LIST, false);

	/* This routine only does a minimum amount of matching. It does not
	   find cat files in the alternate cat directory. */

	if (layout == -1) {
		layout = parse_layout (mandir_layout);
		debug ("Layout is %s (%d)\n", mandir_layout, layout);
	}

	if (opts & (LFF_REGEX | LFF_WILDCARD))
		name = xstrdup (unesc_name);
	else
		name = escape_shell (unesc_name);

	/* allow lookups like "3x foo" to match "../man3/foo.3x" */

	if (layout & LAYOUT_GNU) {
		gl_list_t dirs;
		const char *dir;

		dirs = new_string_list (GL_ARRAY_LIST, false);
		pattern = xasprintf ("%s\t*", cat ? "cat" : "man");
		*strrchr (pattern, '\t') = *sec;
		match_in_directory (hier, pattern, LFF_MATCHCASE, dirs);
		free (pattern);

		pattern = make_pattern (name, sec, opts);
		GL_LIST_FOREACH_START (dirs, dir) {
			if (path)
				*path = '\0';
			match_in_directory (dir, pattern, opts, matched);
		} GL_LIST_FOREACH_END (dirs);
		free (pattern);
		gl_list_free (dirs);
	}

	/* Try HPUX style compressed man pages */
	if ((layout & LAYOUT_HPUX) && gl_list_size (matched) == 0) {
		if (path)
			*path = '\0';
		path = appendstr (path, hier, cat ? "/cat" : "/man",
				  sec, ".Z", (void *) 0);
		pattern = make_pattern (name, sec, opts);

		match_in_directory (path, pattern, opts, matched);
		free (pattern);
	}

	/* Try man pages without the section extension --- IRIX man pages */
	if ((layout & LAYOUT_IRIX) && gl_list_size (matched) == 0) {
		if (path)
			*path = '\0';
		path = appendstr (path, hier, cat ? "/cat" : "/man", sec,
				  (void *) 0);
		if (opts & LFF_REGEX)
			pattern = xasprintf ("%s\\..*", name);
		else
			pattern = xasprintf ("%s.*", name);

		match_in_directory (path, pattern, opts, matched);
		free (pattern);
	}

	/* Try Solaris style man page directories */
	if ((layout & LAYOUT_SOLARIS) && gl_list_size (matched) == 0) {
		if (path)
			*path = '\0';
		/* TODO: This needs to be man/sec*, not just man/sec. */
		path = appendstr (path, hier, cat ? "/cat" : "/man", sec,
				  (void *) 0);
		pattern = make_pattern (name, sec, opts);

		match_in_directory (path, pattern, opts, matched);
		free (pattern);
	}

	/* BSD cat pages take the extension .0 */
	if ((layout & LAYOUT_BSD) && gl_list_size (matched) == 0) {
		if (path)
			*path = '\0';
		if (cat) {
			path = appendstr (path, hier, "/cat", sec, (void *) 0);
			if (opts & LFF_REGEX)
				pattern = xasprintf ("%s\\.0.*", name);
			else
				pattern = xasprintf ("%s.0*", name);
		} else {
			path = appendstr (path, hier, "/man", sec, (void *) 0);
			pattern = make_pattern (name, sec, opts);
		}
		match_in_directory (path, pattern, opts, matched);
		free (pattern);
	}

	free (name);
	free (path);

	return matched;
}

gl_list_t expand_path (const char *path)
{
	int res = 0;
	gl_list_t result;
	glob_t globbuf;

	result = new_string_list (GL_ARRAY_LIST, false);

	res = glob (path, GLOB_NOCHECK, NULL, &globbuf);
	/* if glob failed, return the given path */
	if (res != 0)
		gl_list_add_last (result, xstrdup (path));
	else {
		size_t i;
		for (i = 0; i < globbuf.gl_pathc; ++i)
			gl_list_add_last (result,
					  xstrdup (globbuf.gl_pathv[i]));
	}

	globfree (&globbuf);

	return result;
}
