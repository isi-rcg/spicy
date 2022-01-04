/*
 * check_mandirs.h: Interface to updating database caches
 *
 * Copyright (C) 2001, 2002 Colin Watson.
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
 */

#include "db_storage.h"

/* check_mandirs.c */
extern void test_manfile (MYDBM_FILE dbf, const char *file, const char *path);
extern void chown_if_possible (const char *path);
extern int create_db (const char *database,
		      const char *manpath, const char *catpath);
extern int update_db (const char *database,
		      const char *manpath, const char *catpath);
extern void purge_pointers (MYDBM_FILE dbf, const char *name);
extern int purge_missing (const char *database,
			  const char *manpath, const char *catpath,
			  int will_run_mandb);
