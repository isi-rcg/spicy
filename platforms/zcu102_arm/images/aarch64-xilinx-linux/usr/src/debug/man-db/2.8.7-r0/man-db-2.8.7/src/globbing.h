/*
 * globbing.h: Headers for glob routines
 *
 * Copyright (C) 2001, 2002, 2007, 2008 Colin Watson.
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

#include "gl_list.h"

enum look_for_file_opts {
	LFF_MATCHCASE = 1,
	LFF_REGEX = 2,
	LFF_WILDCARD = 4
};

/* globbing.c */
extern gl_list_t look_for_file (const char *hier, const char *sec,
				const char *unesc_name, int cat, int opts);

/* Expand path with wildcards into list of all existing directories. */
extern gl_list_t expand_path (const char *path);
