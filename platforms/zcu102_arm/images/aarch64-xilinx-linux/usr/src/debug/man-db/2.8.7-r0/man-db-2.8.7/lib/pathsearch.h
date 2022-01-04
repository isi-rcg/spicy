/*
 * pathsearch.h: interface to $PATH-searching functions
 *
 * Copyright (C) 2004 Colin Watson.
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
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA.
 */

#ifndef PATHSEARCH_H
#define PATHSEARCH_H

#include <stdbool.h>

/* Return true if NAME is found as an executable regular file on the $PATH,
 * otherwise false.
 */
bool pathsearch_executable (const char *name);

/* Return true if DIR matches an entry on the $PATH, otherwise false. */
bool directory_on_path (const char *dir);

#endif /* PATHSEARCH_H */
