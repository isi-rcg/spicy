/*
 * db_gdbm.c: low level gdbm interface routines for man.
 *
 * Copyright (C) 1994, 1995 Graeme W. Wilford. (Wilf.)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Mon Aug  8 20:35:30 BST 1994  Wilf. (G.Wilford@ee.surrey.ac.uk)
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#ifdef GDBM

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "stat-time.h"
#include "timespec.h"

#include "manconfig.h"

#include "cleanup.h"

#include "db_xdbm.h"
#include "mydbm.h"

/* setjmp/longjmp handling to defend against _gdbm_fatal exiting under our
 * feet.  Not thread-safe, but there is no plan for man-db to ever use
 * threads.
 */
static jmp_buf open_env;
static int opening;

/* Mimic _gdbm_fatal's error output, but handle errors during open more
 * gracefully than exiting.
 */
static void trap_error (const char *val)
{
	if (opening) {
		debug ("gdbm error: %s\n", val);
		longjmp (open_env, 1);
	} else
		fprintf (stderr, "gdbm fatal: %s\n", val);
}

man_gdbm_wrapper man_gdbm_open_wrapper (const char *name, int flags)
{
	man_gdbm_wrapper wrap;
	GDBM_FILE file;
	datum key, content;

	opening = 1;
	if (setjmp (open_env))
		return NULL;
	file = gdbm_open ((char *) name, BLK_SIZE, flags, DBMODE, trap_error);
	if (!file)
		return NULL;

	wrap = xmalloc (sizeof *wrap);
	wrap->name = xstrdup (name);
	wrap->file = file;

	if ((flags & ~GDBM_FAST) != GDBM_NEWDB) {
		/* While the setjmp/longjmp guard is in effect, make sure we
		 * can read from the database at all.
		 */
		memset (&key, 0, sizeof key);
		MYDBM_SET (key, xstrdup (VER_KEY));
		content = MYDBM_FETCH (wrap, key);
		MYDBM_FREE_DPTR (key);
		MYDBM_FREE_DPTR (content);
	}

	opening = 0;

	return wrap;
}

static datum unsorted_firstkey (man_gdbm_wrapper wrap)
{
	return gdbm_firstkey (wrap->file);
}

static datum unsorted_nextkey (man_gdbm_wrapper wrap, datum key)
{
	return gdbm_nextkey (wrap->file, key);
}

datum man_gdbm_firstkey (man_gdbm_wrapper wrap)
{
	return man_xdbm_firstkey (wrap, unsorted_firstkey, unsorted_nextkey);
}

datum man_gdbm_nextkey (man_gdbm_wrapper wrap, datum key)
{
	return man_xdbm_nextkey (wrap, key);
}

struct timespec man_gdbm_get_time (man_gdbm_wrapper wrap)
{
	struct stat st;

	if (fstat (gdbm_fdesc (wrap->file), &st) < 0) {
		struct timespec t;
		t.tv_sec = -1;
		t.tv_nsec = -1;
		return t;
	}
	return get_stat_mtime (&st);
}

void man_gdbm_set_time (man_gdbm_wrapper wrap, const struct timespec time)
{
	struct timespec times[2];

	times[0] = time;
	times[1] = time;
	futimens (gdbm_fdesc (wrap->file), times);
}

static void raw_close (man_gdbm_wrapper wrap)
{
	gdbm_close (wrap->file);
}

void man_gdbm_close (man_gdbm_wrapper wrap)
{
	man_xdbm_close (wrap, raw_close);
}

#ifndef HAVE_GDBM_EXISTS

int gdbm_exists (GDBM_FILE file, datum key)
{
	char *memory;

	memory = MYDBM_DPTR (gdbm_fetch (file, key));
	if (memory) {
		free (memory);
		return 1;
	}

	return 0;
}

#endif /* !HAVE_GDBM_EXISTS */

#endif /* GDBM */
