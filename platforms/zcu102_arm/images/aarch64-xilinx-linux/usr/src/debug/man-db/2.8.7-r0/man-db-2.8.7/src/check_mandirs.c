/*
 * check_mandirs.c: used to auto-update the database caches
 *
 * Copyright (C) 1994, 1995 Graeme W. Wilford. (Wilf.)
 * Copyright (C) 2001, 2002, 2003, 2004, 2007, 2008, 2009, 2010, 2011
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
 * Mon May  2 17:36:33 BST 1994  Wilf. (G.Wilford@ee.surrey.ac.uk)
 *
 * CJW: Many changes to whatis parsing. Added database purging.
 * See ChangeLog for details.
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>
#include <ctype.h>
#include <dirent.h>
#include <unistd.h>

#include "dirname.h"
#include "gl_array_list.h"
#include "gl_hash_map.h"
#include "gl_xlist.h"
#include "gl_xmap.h"
#include "stat-time.h"
#include "timespec.h"
#include "xvasprintf.h"

#include "gettext.h"
#define _(String) gettext (String)

#include "manconfig.h"

#include "error.h"
#include "glcontainers.h"
#include "orderfiles.h"
#include "security.h"

#include "mydbm.h"
#include "db_storage.h"

#include "descriptions.h"
#include "filenames.h"
#include "globbing.h"
#include "manp.h"
#include "ult_src.h"
#include "check_mandirs.h"

bool opt_test;		/* don't update db */
int pages;
bool force_rescan = false;

gl_map_t whatis_map = NULL;

struct whatis {
	char *whatis;
	gl_list_t trace;
};

static void whatis_free (const void *value)
{
	struct whatis *whatis = (struct whatis *) value;

	free (whatis->whatis);
	gl_list_free (whatis->trace);
	free (whatis);
}

static void gripe_multi_extensions (const char *path, const char *sec, 
				    const char *name, const char *ext)
{
	if (quiet < 2)
		error (0, 0,
		       _("warning: %s/man%s/%s.%s*: competing extensions"),
		       path, sec, name, ext);
}

/* Test whether an errno value is EAGAIN or (on systems where it differs)
 * EWOULDBLOCK.  This is a separate function mainly in order to be able to
 * control GCC diagnostics in one place.
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wlogical-op"
static inline bool is_eagain (int err)
{
	return err == EAGAIN || err == EWOULDBLOCK;
}
#pragma GCC diagnostic pop

static void gripe_rwopen_failed (const char *database)
{
	if (errno == EACCES || errno == EROFS)
		debug ("database %s is read-only\n", database);
	else if (is_eagain (errno))
		debug ("database %s is locked by another process\n", database);
	else {
#ifdef MAN_DB_UPDATES
		if (!quiet)
#endif /* MAN_DB_UPDATES */
			error (0, errno, _("can't update index cache %s"),
			       database);
	}
}

/* Take absolute filename and path (for ult_src) and do sanity checks on
 * file. Also check that file is non-zero in length and is not already in
 * the db. If not, find its ult_src() and see if we have the whatis cached,
 * otherwise cache it in case we trace another manpage back to it. Next,
 * store it in the db along with any references found in the whatis.
 */
void test_manfile (MYDBM_FILE dbf, const char *file, const char *path)
{
	char *manpage_base;
	const char *ult;
	struct lexgrog lg;
	char *manpage;
	struct mandata info, *exists;
	struct stat buf;
	size_t len;
	gl_list_t ult_trace = NULL;
	const struct whatis *whatis;

	memset (&lg, 0, sizeof (struct lexgrog));
	memset (&info, 0, sizeof (struct mandata));

	manpage = filename_info (file, &info, NULL);
	if (!manpage)
		return;
	manpage_base = manpage + strlen (manpage) + 1;

	len  = strlen (manpage) + 1;		/* skip over directory name */
	len += strlen (manpage + len) + 1;	/* skip over base name */
	len += strlen (manpage + len);		/* skip over section ext */

	/* to get mtime info */
	(void) lstat (file, &buf);
	info.mtime = get_stat_mtime (&buf);

	/* check that our file actually contains some data */
	if (buf.st_size == 0) {
		/* man-db pre 2.3 place holder ? */
		free (manpage);
		return;
	}

	/* See if we already have it, before going any further. This will
	 * save both an ult_src() and a find_name(), amongst other wastes of
	 * time.
	 */
	exists = dblookup_exact (dbf, manpage_base, info.ext, true);

	/* Ensure we really have the actual page. Gzip keeps the mtime the
	 * same when it compresses, so we have to compare compression
	 * extensions as well.
	 */
	if (exists) {
		if (strcmp (exists->comp, info.comp ? info.comp : "-") == 0) {
			if (timespec_cmp (exists->mtime, info.mtime) == 0 &&
			    exists->id < WHATIS_MAN) {
				free_mandata_struct (exists);
				free (manpage);
				return;
			}
		} else {
			char *abs_filename;

			/* see if the cached file actually exists. It's 
			   evident at this point that we have multiple 
			   comp extensions */
			abs_filename = make_filename (path, NULL,
						      exists, "man");
			if (!abs_filename) {
				if (!opt_test)
					dbdelete (dbf, manpage_base, exists);
			} else {
				gripe_multi_extensions (path, exists->sec,
							manpage_base,
							exists->ext);
				free (abs_filename);
				free_mandata_struct (exists);
				free (manpage);
				return;
			}
		}
		free_mandata_struct (exists);
	}

	/* Check if it happens to be a symlink/hardlink to something already
	 * in our cache. This just does some extra checks to avoid scanning
	 * links quite so many times.
	 */
	{
		/* Avoid too much noise in debug output */
		bool save_debug = debug_level;
		debug_level = false;
		ult = ult_src (file, path, &buf, SOFT_LINK | HARD_LINK, NULL);
		debug_level = save_debug;
	}

	if (!ult) {
		/* already warned about this, don't do so again */
		debug ("test_manfile(): bad link %s\n", file);
		free (manpage);
		return;
	}

	if (!whatis_map)
		whatis_map = new_string_map (GL_HASH_MAP, whatis_free);

	whatis = gl_map_get (whatis_map, ult);
	if (!whatis) {
		if (!STRNEQ (ult, file, len))
			debug ("\ntest_manfile(): link not in cache:\n"
			       " source = %s\n"
			       " target = %s\n", file, ult);
		/* Trace the file to its ultimate source, otherwise we'll be
		 * looking for whatis info in files containing only '.so
		 * manx/foo.x', which will give us an unobtainable whatis
		 * for the entry. */
		ult_trace = new_string_list (GL_ARRAY_LIST, true);
		ult = ult_src (file, path, &buf,
			       SO_LINK | SOFT_LINK | HARD_LINK, ult_trace);
	}

	if (!ult) {
		if (quiet < 2)
			error (0, 0,
			       _("warning: %s: bad symlink or ROFF `.so' request"),
			       file);
		free (manpage);
		return;
	}

	pages++;			/* pages seen so far */

	if (strncmp (ult, file, len) == 0)
		info.id = ULT_MAN;	/* ultimate source file */
	else
		info.id = SO_MAN;	/* .so, sym or hard linked file */

	/* Ok, here goes: Use a hash tree to store the ult_srcs with
	 * their whatis. Anytime after, check the hash tree, if it's there, 
	 * use it. This saves us a find_name() which is a real hog.
	 *
	 * Use the full path in ult as the hash key so we don't have to
	 * clear the hash between calls.
	 */

	if (whatis)
		lg.whatis = whatis->whatis ? xstrdup (whatis->whatis) : NULL;
	else {
		/* Cache miss; go and get the whatis info in its raw state. */
		char *file_base = base_name (file);
		struct whatis *new_whatis;

		lg.type = MANPAGE;
		drop_effective_privs ();
		find_name (ult, file_base, &lg, NULL);
		free (file_base);
		regain_effective_privs ();

		new_whatis = XMALLOC (struct whatis);
		new_whatis->whatis = lg.whatis ? xstrdup (lg.whatis) : NULL;
		/* We filled out ult_trace above. */
		new_whatis->trace = ult_trace;
		gl_map_put (whatis_map, xstrdup (ult), new_whatis);
		whatis = new_whatis;
	}

	debug ("\"%s\"\n", lg.whatis);

	/* split up the raw whatis data and store references */
	info.pointer = NULL;	/* direct page, so far */
	info.filter = lg.filters;
	if (lg.whatis) {
		gl_list_t descs = parse_descriptions (manpage_base, lg.whatis);
		if (!opt_test)
			store_descriptions (dbf, descs, &info, path,
					    manpage_base, whatis->trace);
		gl_list_free (descs);
	} else if (quiet < 2) {
		(void) stat (ult, &buf);
		if (buf.st_size == 0)
			error (0, 0, _("warning: %s: ignoring empty file"),
			       ult);
		else
			error (0, 0,
			       _("warning: %s: whatis parse for %s(%s) failed"),
			       ult, manpage_base, info.ext);
	}

	free (manpage);
	free (lg.whatis);
}

static void add_dir_entries (MYDBM_FILE dbf, const char *path, char *infile)
{
	char *manpage;
	int len;
	struct dirent *newdir;
	DIR *dir;
	gl_list_t names;
	const char *name;

	manpage = xasprintf ("%s/%s/", path, infile);
	len = strlen (manpage);

	/*
	 * All filename entries in this dir should either be valid manpages
	 * or . files (such as current, parent dir).
	 */

	dir = opendir (infile);
	if (!dir) {
		error (0, errno, _("can't search directory %s"), manpage);
		free (manpage);
                return;
        }

	names = new_string_list (GL_ARRAY_LIST, false);

        /* strlen(newdir->d_name) could be replaced by newdir->d_reclen */

	while ((newdir = readdir (dir)) != NULL) {
		if (*newdir->d_name == '.' &&
		    strlen (newdir->d_name) < (size_t) 3)
			continue;
		gl_list_add_last (names, xstrdup (newdir->d_name));
	}
	closedir (dir);

	order_files (infile, &names);

	GL_LIST_FOREACH_START (names, name) {
		manpage = appendstr (manpage, name, (void *) 0);
		test_manfile (dbf, manpage, path);
		*(manpage + len) = '\0';
	} GL_LIST_FOREACH_END (names);

	gl_list_free (names);
	free (manpage);
}

#ifdef MAN_OWNER
extern uid_t uid;			/* current effective user id */
extern gid_t gid;			/* current effective group id */

/* Fix a path's ownership if possible and necessary. */
void chown_if_possible (const char *path)
{
	struct stat st;
	struct passwd *man_owner = get_man_owner ();

	if (lstat (path, &st) != 0)
		return;

	if ((uid == 0 ||
	     (uid == man_owner->pw_uid && st.st_uid == man_owner->pw_uid &&
	      gid == man_owner->pw_gid)) &&
	    (st.st_uid != man_owner->pw_uid ||
	     st.st_gid != man_owner->pw_gid)) {
		debug ("fixing ownership of %s\n", path);
		if (lchown (path, man_owner->pw_uid, man_owner->pw_gid) < 0)
			error (FATAL, 0, _("can't chown %s"), path);
	}
}
#else /* !MAN_OWNER */
void chown_if_possible (const char *path _GL_UNUSED)
{
}
#endif /* MAN_OWNER */

/* create the catman hierarchy if it doesn't exist */
static void mkcatdirs (const char *mandir, const char *catdir)
{
	char *manname, *catname;

	if (catdir) {
		int oldmask = umask (022);
		/* first the base catdir */
		if (is_directory (catdir) != 1) {
			regain_effective_privs ();
			if (mkdir (catdir, 0755) < 0) {
				if (!quiet)
					error (0, 0,
					       _("warning: cannot create catdir %s"),
					       catdir);
				debug ("warning: cannot create catdir %s\n",
				       catdir);
			} else
				debug ("created base catdir %s\n", catdir);
			chown_if_possible (catdir);
			drop_effective_privs ();
		}
		/* then the hierarchy */
		catname = xasprintf ("%s/cat1", catdir);
		manname = xasprintf ("%s/man1", mandir);
		if (is_directory (catdir) == 1) {
			int j;
			regain_effective_privs ();
			debug ("creating catdir hierarchy %s	", catdir);
			for (j = 1; j <= 9; j++) {
				catname[strlen (catname) - 1] = '0' + j;
				manname[strlen (manname) - 1] = '0' + j;
				if ((is_directory (manname) == 1)
				 && (is_directory (catname) != 1)) {
					if (mkdir (catname, 0755) < 0) {
						if (!quiet)
							error (0, 0, _("warning: cannot create catdir %s"), catname);
						debug ("warning: cannot create catdir %s\n", catname);
					} else
						debug (" cat%d", j);
					chown_if_possible (catname);
				}
			}
			debug ("\n");
			drop_effective_privs ();
		}
		free (catname);
		free (manname);
		umask (oldmask);
	}
}

/* We used to install cat directories with the setgid bit set, but this
 * wasn't very useful and introduces the ability to escalate privileges to
 * that group:
 *   https://www.halfdog.net/Security/2015/SetgidDirectoryPrivilegeEscalation/
 */
static void fix_permissions (const char *dir)
{
	struct stat st;

	if (stat (dir, &st) == 0) {
		if ((st.st_mode & S_ISGID) != 0) {
			int status;

			debug ("removing setgid bit from %s\n", dir);
			status = chmod (dir, st.st_mode & ~S_ISGID);
			if (status)
				error (0, errno, _("can't chmod %s"), dir);
		}

		chown_if_possible (dir);
	}
}

static void fix_permissions_tree (const char *catdir)
{
	if (is_directory (catdir) == 1) {
		char *catname;
		int i;

		fix_permissions (catdir);
		catname = xasprintf ("%s/cat1", catdir);
		for (i = 1; i <= 9; ++i) {
			catname[strlen (catname) - 1] = '0' + i;
			fix_permissions (catname);
		}
		free (catname);
	}
}

/*
 * accepts the raw man dir tree eg. "/usr/man" and the time stored in the db
 * any dirs of the tree that have been modified (ie added to) will then be
 * scanned for new files, which are then added to the db.
 */
static int testmandirs (const char *database,
			const char *path, const char *catpath,
			struct timespec last, int create)
{
	DIR *dir;
	struct dirent *mandir;
	int amount = 0;
	int created = 0;

	debug ("Testing %s for new files\n", path);

	if (catpath)
		fix_permissions_tree (catpath);

	dir = opendir (path);
	if (!dir) {
		error (0, errno, _("can't search directory %s"), path);
		return 0;
	}

	if (chdir (path) != 0) {
		error (0, errno, _("can't change to directory %s"), path);
		closedir (dir);
		return 0;
	}

	while( (mandir = readdir (dir)) ) {
		struct stat stbuf;
		struct timespec mtime;
		MYDBM_FILE dbf;

		if (strncmp (mandir->d_name, "man", 3) != 0)
			continue;

		debug ("Examining %s\n", mandir->d_name);

		if (stat (mandir->d_name, &stbuf) != 0)	/* stat failed */
			continue;
		if (!S_ISDIR(stbuf.st_mode))		/* not a directory */
			continue;
		mtime = get_stat_mtime (&stbuf);
		if (last.tv_sec && timespec_cmp (mtime, last) <= 0) {
			/* scanned already */
			debug ("%s modified %ld.%09ld, "
			       "db modified %ld.%09ld\n",
			       mandir->d_name,
			       (long) mtime.tv_sec, (long) mtime.tv_nsec,
			       (long) last.tv_sec, (long) last.tv_nsec);
			continue;
		}

		debug ("\tsubdirectory %s has been 'modified'\n",
		       mandir->d_name);

		if (create && !created) {
			/* We seem to have something to do, so create the
			 * database now.
			 */
			mkcatdirs (path, catpath);

			/* Open the db in CTRW mode to store the $ver$ ID */

			dbf = MYDBM_CTRWOPEN (database);
			if (dbf == NULL) {
				if (errno == EACCES || errno == EROFS) {
					debug ("database %s is read-only\n",
					       database);
					closedir (dir);
					return 0;
				} else {
					error (0, errno,
					       _("can't create index cache %s"),
					       database);
					closedir (dir);
					return -errno;
				}
			}

			dbver_wr (dbf);

			created = 1;
		} else
			dbf = MYDBM_RWOPEN(database);

		if (!dbf) {
			gripe_rwopen_failed (database);
			closedir (dir);
			return 0;
		}

		if (!quiet) {
			int tty = isatty (STDERR_FILENO);

			if (tty)
				fprintf (stderr, "\r");
			fprintf (stderr,
				 _("Updating index cache for path "
				   "`%s/%s'. Wait..."), path, mandir->d_name);
			if (!tty)
				fprintf (stderr, "\n");
		}
		add_dir_entries (dbf, path, mandir->d_name);
		MYDBM_CLOSE (dbf);
		amount++;
	}
	closedir (dir);

	return amount;
}

/* update the modification timestamp of `database' */
static void update_db_time (const char *database)
{
	MYDBM_FILE dbf;
	struct timespec now;

	/* Open the db in RW to update its mtime */
	/* we know that this should succeed because we just updated the db! */
	dbf = MYDBM_RWOPEN (database);
	if (dbf == NULL) {
		if (is_eagain (errno))
			/* Another mandb process is probably running.  With
			 * any luck it will update the mtime ...
			 */
			debug ("database %s is locked by another process\n",
			       database);
		else {
#ifdef MAN_DB_UPDATES
			if (!quiet)
#endif /* MAN_DB_UPDATES */
				error (0, errno,
				       _("can't update index cache %s"),
				       database);
		}
		return;
	}
	now.tv_sec = 0;
	now.tv_nsec = UTIME_NOW;
	MYDBM_SET_TIME (dbf, now);

	MYDBM_CLOSE (dbf);
}

/* routine to prepare/create the db prior to calling testmandirs() */
int create_db (const char *database, const char *manpath, const char *catpath)
{
	struct timespec time_zero;
	int amount;

	debug ("create_db(%s): %s\n", manpath, database);

	time_zero.tv_sec = 0;
	time_zero.tv_nsec = 0;
	amount = testmandirs (database, manpath, catpath, time_zero, 1);

	if (amount) {
		update_db_time (database);
		if (!quiet)
			fputs (_("done.\n"), stderr);
	}

	return amount;
}

/* Make sure an existing database is essentially sane. */
static bool sanity_check_db (MYDBM_FILE dbf)
{
	datum key;

	if (dbver_rd (dbf))
		return false;

	key = MYDBM_FIRSTKEY (dbf);
	while (MYDBM_DPTR (key) != NULL) {
		datum content, nextkey;

		content = MYDBM_FETCH (dbf, key);
		if (!MYDBM_DPTR (content)) {
			debug ("warning: %s has a key with no content (%s); "
			       "rebuilding\n", dbf->name, MYDBM_DPTR (key));
			MYDBM_FREE_DPTR (key);
			return false;
		}
		MYDBM_FREE_DPTR (content);
		nextkey = MYDBM_NEXTKEY (dbf, key);
		MYDBM_FREE_DPTR (key);
		key = nextkey;
	}

	return true;
}

/* routine to update the db, ensure that it is consistent with the 
   filesystem */
int update_db (const char *database, const char *manpath, const char *catpath)
{
	MYDBM_FILE dbf;
	struct timespec mtime;
	int new;

	dbf = MYDBM_RDOPEN (database);
	if (dbf && !sanity_check_db (dbf)) {
		MYDBM_CLOSE (dbf);
		dbf = NULL;
	}
	if (!dbf) {
		debug ("failed to open %s O_RDONLY\n", database);
		return EOF;
	}
	mtime = MYDBM_GET_TIME (dbf);
	MYDBM_CLOSE (dbf);

	debug ("update_db(): %ld.%09ld\n",
	       (long) mtime.tv_sec, (long) mtime.tv_nsec);
	new = testmandirs (database, manpath, catpath, mtime, 0);

	if (new) {
		update_db_time (database);
		if (!quiet)
			fputs (_("done.\n"), stderr);
	}

	return new;
}

/* Purge any entries pointing to name. This currently assumes that pointers
 * are always shallow, which may not be a good assumption yet; it should be
 * close, though.
 */
void purge_pointers (MYDBM_FILE dbf, const char *name)
{
	datum key = MYDBM_FIRSTKEY (dbf);

	debug ("Purging pointers to vanished page \"%s\"\n", name);

	while (MYDBM_DPTR (key) != NULL) {
		datum content, nextkey;
		struct mandata entry;
		char *nicekey, *tab;

		/* Ignore db identifier keys. */
		if (*MYDBM_DPTR (key) == '$')
			goto pointers_next;

		content = MYDBM_FETCH (dbf, key);
		if (!MYDBM_DPTR (content))
			return;

		/* Get just the name. */
		nicekey = xstrdup (MYDBM_DPTR (key));
		tab = strchr (nicekey, '\t');
		if (tab)
			*tab = '\0';

		if (*MYDBM_DPTR (content) == '\t')
			goto pointers_contentnext;

		split_content (dbf, MYDBM_DPTR (content), &entry);
		if (entry.id != SO_MAN && entry.id != WHATIS_MAN)
			goto pointers_contentnext;

		if (STREQ (entry.pointer, name)) {
			if (!opt_test)
				dbdelete (dbf, nicekey, &entry);
			else
				debug ("%s(%s): pointer vanished, "
				       "would delete\n", nicekey, entry.ext);
		}

pointers_contentnext:
		free (nicekey);
		MYDBM_FREE_DPTR (content);
pointers_next:
		nextkey = MYDBM_NEXTKEY (dbf, key);
		MYDBM_FREE_DPTR (key);
		key = nextkey;
	}
}

/* Count the number of exact extension matches returned from look_for_file()
 * (which may return inexact extension matches in some cases). It may turn
 * out that this is better handled in look_for_file() itself.
 */
static int count_glob_matches (const char *name, const char *ext,
			       gl_list_t source, struct timespec db_mtime)
{
	const char *walk;
	int count = 0;

	GL_LIST_FOREACH_START (source, walk) {
		struct mandata info;
		struct stat statbuf;
		char *buf;

		memset (&info, 0, sizeof (struct mandata));

		if (stat (walk, &statbuf) == -1) {
			debug ("count_glob_matches: excluding %s "
			       "because stat failed\n", walk);
			continue;
		}
		if (db_mtime.tv_sec != (time_t) -1 &&
		    timespec_cmp (get_stat_mtime (&statbuf), db_mtime) <= 0) {
			debug ("count_glob_matches: excluding %s, "
			       "no newer than database\n", walk);
			continue;
		}

		buf = filename_info (walk, &info, name);
		if (buf) {
			if (STREQ (ext, info.ext))
				++count;
			free (info.name);
			free (buf);
		}
	} GL_LIST_FOREACH_END (source);

	return count;
}

/* Decide whether to purge a reference to a "normal" (ULT_MAN or SO_MAN)
 * page.
 */
static int purge_normal (MYDBM_FILE dbf, const char *name,
			 struct mandata *info, gl_list_t found)
{
	struct timespec t;

	/* TODO: On some systems, the cat page extension differs from the
	 * man page extension, so this may be too strict.
	 */
	t.tv_sec = -1;
	t.tv_nsec = -1;
	if (count_glob_matches (name, info->ext, found, t))
		return 0;

	if (!opt_test)
		dbdelete (dbf, name, info);
	else
		debug ("%s(%s): missing page, would delete\n",
		       name, info->ext);

	return 1;
}

/* Decide whether to purge a reference to a WHATIS_MAN or WHATIS_CAT page. */
static int purge_whatis (MYDBM_FILE dbf, const char *path, int cat,
			 const char *name, struct mandata *info,
			 gl_list_t found, struct timespec db_mtime)
{
	/* TODO: On some systems, the cat page extension differs from the
	 * man page extension, so this may be too strict.
	 */
	if (count_glob_matches (name, info->ext, found, db_mtime)) {
		/* If the page exists and didn't beforehand, then presumably
		 * we're about to rescan, which will replace the WHATIS_MAN
		 * entry with something better. However, there have been
		 * bugs that created false WHATIS_MAN entries, so force the
		 * rescan just to be sure; since in the absence of a bug we
		 * would rescan anyway, this isn't a problem.
		 */
		if (!force_rescan)
			debug ("%s(%s): whatis replaced by real page; "
			       "forcing a rescan just in case\n",
			       name, info->ext);
		force_rescan = true;
		return 0;
	} else if (STREQ (info->pointer, "-")) {
		/* This is broken; a WHATIS_MAN should never have an empty
		 * pointer field. This might have happened due to the first
		 * name in a page being different from what the file name
		 * says; that's fixed now, so delete and force a rescan.
		 */
		if (!opt_test)
			dbdelete (dbf, name, info);
		else
			debug ("%s(%s): whatis with empty pointer, "
			       "would delete\n", name, info->ext);

		if (!force_rescan)
			debug ("%s(%s): whatis had empty pointer; "
			       "forcing a rescan just in case\n",
			       name, info->ext);
		force_rescan = true;
		return 1;
	} else {
		/* Does the real page still exist? */
		gl_list_t real_found;
		bool save_debug = debug_level;
		struct timespec t;
		int count;

		debug_level = false;
		real_found = look_for_file (path, info->ext,
					    info->pointer, cat, LFF_MATCHCASE);
		debug_level = save_debug;

		t.tv_sec = -1;
		t.tv_nsec = -1;
		count = count_glob_matches (info->pointer, info->ext,
					    real_found, t);
		gl_list_free (real_found);
		if (count)
			return 0;

		if (!opt_test)
			dbdelete (dbf, name, info);
		else
			debug ("%s(%s): whatis target was deleted, "
			       "would delete\n", name, info->ext);
		return 1;
	}
}

/* Check that multi keys are correctly constructed. */
static int check_multi_key (const char *name, const char *content)
{
	const char *walk, *next;

	if (!*content)
		return 0;

	for (walk = content; walk && *walk; walk = next) {
		/* The name in the multi key should only differ from the
		 * name of the key itself in its case, if at all.
		 */
		int valid = 1;
		++walk; /* skip over initial tab */
		next = strchr (walk, '\t');
		if (next) {
			if (strncasecmp (name, walk, next - walk))
				valid = 0;
		} else {
			if (strcasecmp (name, walk))
				valid = 0;
		}
		if (!valid) {
			debug ("%s: broken multi key \"%s\", "
			       "forcing a rescan\n", name, content);
			force_rescan = true;
			return 1;
		}

		/* If the name was valid, skip over the extension and
		 * continue the scan.
		 */
		walk = next;
		next = walk ? strchr (walk + 1, '\t') : NULL;
	}

	return 0;
}

/* Go through the database and purge references to man pages that no longer
 * exist.
 */
int purge_missing (const char *database,
		   const char *manpath, const char *catpath,
		   int will_run_mandb)
{
#ifdef NDBM
	char *dirfile;
#endif
	struct stat st;
	int db_exists;
	MYDBM_FILE dbf;
	datum key;
	int count = 0;
	struct timespec db_mtime;

#ifdef NDBM
	dirfile = xasprintf ("%s.dir", database);
	db_exists = stat (dirfile, &st) == 0;
	free (dirfile);
#else
	db_exists = stat (database, &st) == 0;
#endif
	if (!db_exists)
		/* nothing to purge */
		return 0;

	if (!quiet)
		printf (_("Purging old database entries in %s...\n"), manpath);

	dbf = MYDBM_RWOPEN (database);
	if (!dbf) {
		gripe_rwopen_failed (database);
		return 0;
	}
	if (!sanity_check_db (dbf)) {
		MYDBM_CLOSE (dbf);
		dbf = NULL;
		return 0;
	}
	db_mtime = MYDBM_GET_TIME (dbf);

	key = MYDBM_FIRSTKEY (dbf);

	while (MYDBM_DPTR (key) != NULL) {
		datum content, nextkey;
		struct mandata entry;
		char *nicekey, *tab;
		bool save_debug;
		gl_list_t found;

		/* Ignore db identifier keys. */
		if (*MYDBM_DPTR (key) == '$') {
			nextkey = MYDBM_NEXTKEY (dbf, key);
			MYDBM_FREE_DPTR (key);
			key = nextkey;
			continue;
		}

		content = MYDBM_FETCH (dbf, key);
		if (!MYDBM_DPTR (content)) {
			nextkey = MYDBM_NEXTKEY (dbf, key);
			MYDBM_FREE_DPTR (key);
			key = nextkey;
			continue;
		}

		/* Get just the name. */
		nicekey = xstrdup (MYDBM_DPTR (key));
		tab = strchr (nicekey, '\t');
		if (tab)
			*tab = '\0';

		/* Deal with multi keys. */
		if (*MYDBM_DPTR (content) == '\t') {
			if (check_multi_key (nicekey, MYDBM_DPTR (content)))
				MYDBM_DELETE (dbf, key);
			free (nicekey);
			MYDBM_FREE_DPTR (content);
			nextkey = MYDBM_NEXTKEY (dbf, key);
			MYDBM_FREE_DPTR (key);
			key = nextkey;
			continue;
		}

		split_content (dbf, MYDBM_DPTR (content), &entry);

		save_debug = debug_level;
		debug_level = false;	/* look_for_file() is quite noisy */
		if (entry.id <= WHATIS_MAN)
			found = look_for_file (manpath, entry.ext,
					       entry.name ? entry.name
							  : nicekey,
					       0, LFF_MATCHCASE);
		else
			found = look_for_file (catpath, entry.ext,
					       entry.name ? entry.name
							  : nicekey,
					       1, LFF_MATCHCASE);
		debug_level = save_debug;

		/* Now actually decide whether to purge, depending on the
		 * type of entry.
		 */
		if (entry.id == ULT_MAN || entry.id == SO_MAN ||
		    entry.id == STRAY_CAT)
			count += purge_normal (dbf, nicekey, &entry, found);
		else if (entry.id == WHATIS_MAN)
			count += purge_whatis (dbf, manpath, 0, nicekey,
					       &entry, found, db_mtime);
		else	/* entry.id == WHATIS_CAT */
			count += purge_whatis (dbf, catpath, 1, nicekey,
					       &entry, found, db_mtime);

		gl_list_free (found);
		free (nicekey);

		free_mandata_elements (&entry);
		nextkey = MYDBM_NEXTKEY (dbf, key);
		MYDBM_FREE_DPTR (key);
		key = nextkey;
	}

	MYDBM_REORG (dbf);
	if (will_run_mandb)
		/* Reset mtime to avoid confusing mandb into not running.
		 * TODO: It would be better to avoid this by only opening
		 * the database once between here and mandb.
		 */
		MYDBM_SET_TIME (dbf, db_mtime);
	MYDBM_CLOSE (dbf);
	return count;
}
