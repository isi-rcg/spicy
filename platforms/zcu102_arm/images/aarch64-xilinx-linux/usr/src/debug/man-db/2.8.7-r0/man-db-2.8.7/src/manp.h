/*
 * manp.h: Interface to manpath calculations
 *
 * Copyright (C) 1990, 1991 John W. Eaton.
 * Copyright (C) 1994, 1995 Graeme W. Wilford. (Wilf.)
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

#include <stdbool.h>

#include "gl_list.h"

struct locale_bits {
	char *language;
	char *territory;
	char *codeset;
	char *modifier;
};

/* manp.c */
extern char *cat_manpath (char *manp);
extern void unpack_locale_bits (const char *locale, struct locale_bits *bits);
extern void free_locale_bits (struct locale_bits *bits);
extern char *add_nls_manpaths (const char *manpathlist, const char *locales);
extern char *get_manpath (const char *systems);
extern char *get_manpath_from_path (const char *path, int mandatory);
extern gl_list_t create_pathlist (const char *manp);
extern void free_pathlist (gl_list_t list);
extern char *get_mandb_manpath (void);
extern char *get_catpath (const char *name, int cattype);
extern bool is_global_mandir (const char *dir);
extern void read_config_file (bool optional);
extern const char *get_def (const char *thing, const char *def);
extern const char *get_def_user (const char *thing, const char *def);
extern gl_list_t get_sections (void);
