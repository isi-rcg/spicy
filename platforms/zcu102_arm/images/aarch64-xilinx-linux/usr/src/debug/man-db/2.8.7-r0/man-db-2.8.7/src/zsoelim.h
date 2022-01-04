/*
 * zsoelim.h: interface to eliminating .so includes within *roff source
 *  
 * Copyright (C) 2008 Colin Watson.
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

int zsoelim_open_file (const char *filename, gl_list_t manpathlist,
		       const char *parent_path);
void zsoelim_parse_file (gl_list_t manpathlist, const char *parent_path);

struct zsoelim_stdin_data;

void zsoelim_stdin (void *data);
struct zsoelim_stdin_data *zsoelim_stdin_data_new (const char *path,
						   gl_list_t manpathlist);
void zsoelim_stdin_data_free (void *data);
