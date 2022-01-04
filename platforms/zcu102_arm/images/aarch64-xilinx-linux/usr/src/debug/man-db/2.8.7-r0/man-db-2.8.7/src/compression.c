/*
 * compression.c: code to find decompressor / compression extension
 *  
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
 *
 * Sat Aug 20 15:01:02 BST 1994  Wilf. (G.Wilford@ee.surrey.ac.uk) 
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "xvasprintf.h"

#include "manconfig.h"
#ifdef COMP_SRC /* must come after manconfig.h */

#include "error.h"
#include "pipeline.h"

/* Take filename as arg, return structure containing decompressor 
   and extension, or NULL if no comp extension found. 
   If want_stem, set comp->stem to the filename without extension, which
   the caller should free.

   eg.
   	filename = /usr/man/man1/foo.1.gz 

	comp->prog = "/usr/bin/gzip -dc";
   	comp->ext = "gz";
   	comp->stem = "/usr/man/man1/foo.1";
 */
struct compression *comp_info (const char *filename, int want_stem)
{
	const char *ext;
	static struct compression hpux_comp = {GUNZIP " -S \"\"", "", NULL};

	ext = strrchr (filename, '.');

	if (ext) {
		struct compression *comp;
		for (comp = comp_list; comp->ext; comp++) {
			if (strcmp (comp->ext, ext + 1) == 0) {
				if (want_stem)
					comp->stem = xstrndup (filename,
							       ext - filename);
				else
					comp->stem = NULL;
				return comp;
			}
		}
	}

	ext = strstr (filename, ".Z/");
	if (ext) {
		if (want_stem)
			hpux_comp.stem = xstrndup (filename, ext - filename);
		else
			hpux_comp.stem = NULL;
		return &hpux_comp;
	}

	return NULL;
}

/* take filename w/o comp ext. as arg, return comp->stem as a relative
   compressed file or NULL if none found */
struct compression *comp_file (const char *filename)
{
	size_t len;
	char *compfile;
	struct compression *comp;

	compfile = xasprintf ("%s.", filename);
	len = strlen (compfile);
	
	for (comp = comp_list; comp->ext; comp++) {
		struct stat buf;
		
		compfile = appendstr (compfile, comp->ext, (void *) 0);

		if (stat (compfile, &buf) == 0) {
			comp->stem = compfile;
			return comp;
		}

		*(compfile + len) = '\0';
	}
	free (compfile);
	return NULL;
}

#endif /* COMP_SRC */
