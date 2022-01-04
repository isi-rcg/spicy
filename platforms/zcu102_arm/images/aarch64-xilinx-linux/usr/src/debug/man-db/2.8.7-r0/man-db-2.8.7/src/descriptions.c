/*
 * descriptions.c: manipulate man page descriptions
 *
 * Copyright (C) 2002, 2003, 2006, 2007, 2008, 2009, 2010, 2011 Colin Watson.
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

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include <string.h>
#include <stdlib.h>

#include "gl_array_list.h"
#include "gl_xlist.h"

#include "manconfig.h"
#include "descriptions.h"

/* Free a page description. */
static void page_description_free (const void *value)
{
	struct page_description *desc = (struct page_description *) value;

	free (desc->name);
	free (desc->whatis);
	free (desc);
}

/* Parse the description in a whatis line returned by find_name() into a
 * list of names and whatis descriptions.
 */
gl_list_t parse_descriptions (const char *base, const char *whatis)
{
	const char *sep, *nextsep;
	gl_list_t descs;
	int seen_base = 0;

	descs = gl_list_create_empty (GL_ARRAY_LIST, NULL, NULL,
				      page_description_free, true);

	if (!whatis)
		return descs;

	sep = whatis;

	while (sep) {
		char *record;
		size_t length;
		const char *dash;
		char *names;
		const char *token;

		/* Use a while loop so that we skip over things like the
		 * result of double line breaks.
		 */
		while (*sep == 0x11 || *sep == ' ')
			++sep;
		nextsep = strchr (sep, 0x11);

		/* Get this record as a null-terminated string. */
		if (nextsep)
			length = (size_t) (nextsep - sep);
		else
			length = strlen (sep);
		if (length == 0)
			break;

		record = xstrndup (sep, length);
		debug ("record = '%s'\n", record);

		/* Split the record into name and whatis description. */
		dash = strstr (record, " - ");
		if (dash)
			names = xstrndup (record, dash - record);
		else if (!gl_list_size (descs))
			/* Some pages have a NAME section with just the page
			 * name and no whatis.  We might as well include
			 * this.
			 */
			names = xstrdup (record);
		else
			/* Once at least one record has been seen, further
			 * cases where there is no whatis usually amount to
			 * garbage following the useful records, and can
			 * cause problems due to false WHATIS_MAN entries in
			 * the database.  On the whole it seems best to
			 * ignore these.
			 */
			goto next;

		for (token = strtok (names, ","); token;
		     token = strtok (NULL, ",")) {
			char *name = trim_spaces (token);
			struct page_description *desc;

			/* Skip name tokens containing whitespace. They are
			 * almost never useful as manual page names.
			 */
			if (strpbrk (name, " \t") != NULL) {
				free (name);
				continue;
			}

			/* Allocate new description node. */
			desc = xmalloc (sizeof *desc);
			desc->name   = name; /* steal memory */
			desc->whatis = dash ? trim_spaces (dash + 3) : NULL;
			gl_list_add_last (descs, desc);

			if (base && STREQ (base, desc->name))
				seen_base = 1;
		}

		free (names);
next:
		free (record);

		sep = nextsep;
	}

	/* If it isn't there already, add the base name onto the returned
	 * list.
	 */
	if (base && !seen_base) {
		struct page_description *desc = xmalloc (sizeof *desc);

		desc->name = xstrdup (base);
		desc->whatis = NULL;
		if (gl_list_size (descs)) {
			const struct page_description *first =
				gl_list_get_at (descs, 0);
			if (first->whatis)
				desc->whatis = xstrdup (first->whatis);
		}
		gl_list_add_last (descs, desc);
	}

	return descs;
}
