/*
 * db_delete.c: dbdelete(), database delete routine.
 *
 * Copyright (C) 1994, 1995 Graeme W. Wilford. (Wilf.)
 * Copyright (C) 2001, 2002 Colin Watson.
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
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "error.h"
#include "gl_list.h"

#include "gettext.h"
#define _(String) gettext (String)

#include "manconfig.h"

#include "glcontainers.h"

#include "mydbm.h"
#include "db_storage.h"

/* Delete an entry for a page.
   Again, 3 possibilities:

   1) page is singular reference, just delete it :)
   2) page has 2+ companions. Delete page and alter multi entry to not
      point to it anymore.
   3) page has 1 companion. Could do as (2), but we'd waste an entry in
      the db. Should delete page, extract friend and reinsert as singular,
      overwriting the old multi entry.
*/

#define NO_ENTRY	1;

int dbdelete (MYDBM_FILE dbf, const char *name, struct mandata *info)
{
	datum key, cont;

	memset (&key, 0, sizeof key);
	memset (&cont, 0, sizeof cont);

	/* get entry for info */

	debug ("Attempting delete of %s(%s) entry.\n", name, info->ext);

	MYDBM_SET (key, name_to_key (name));
	cont = MYDBM_FETCH (dbf, key);

	if (!MYDBM_DPTR (cont)) {			/* 0 entries */
		MYDBM_FREE_DPTR (key);
		return NO_ENTRY;
	} else if (*MYDBM_DPTR (cont) != '\t') {	/* 1 entry */
		MYDBM_DELETE (dbf, key);
		MYDBM_FREE_DPTR (cont);
	} else {					/* 2+ entries */
		gl_list_t refs;
		struct name_ext this_ref, *ref;
		size_t this_index;
		char *multi_content = NULL;
		datum multi_key;

		/* Extract all of the extensions associated with
		   this key */

		refs = list_extensions (MYDBM_DPTR (cont) + 1);

		this_ref.name = name;
		this_ref.ext = info->ext;
		this_index = gl_list_indexof (refs, &this_ref);

		if (this_index == (size_t) -1) {
			gl_list_free (refs);
			MYDBM_FREE_DPTR (cont);
			MYDBM_FREE_DPTR (key);
			return NO_ENTRY;
		}

		multi_key = make_multi_key (name, info->ext);
		if (!MYDBM_EXISTS (dbf, multi_key)) {
			error (0, 0,
			       _( "multi key %s does not exist"),
			       MYDBM_DPTR (multi_key));
			gripe_corrupt_data (dbf);
		}
		MYDBM_DELETE (dbf, multi_key);
		MYDBM_FREE_DPTR (multi_key);
		gl_list_remove_at (refs, this_index);

		/* If all manual pages with this name have been deleted,
		   we'll have to remove the key too. */

		if (!gl_list_size (refs)) {
			gl_list_free (refs);
			MYDBM_FREE_DPTR (cont);
			MYDBM_DELETE (dbf, key);
			MYDBM_FREE_DPTR (key);
			return 0;
		}

		/* create our new multi content */
		GL_LIST_FOREACH_START (refs, ref)
			multi_content = appendstr (multi_content,
						   "\t", ref->name,
						   "\t", ref->ext,
						   (void *) 0);
		GL_LIST_FOREACH_END (refs);

		MYDBM_FREE_DPTR (cont);
		MYDBM_SET (cont, multi_content);
		if (MYDBM_REPLACE (dbf, key, cont))
			gripe_replace_key (dbf, MYDBM_DPTR (key));

		gl_list_free (refs);
	}

	MYDBM_FREE_DPTR (key);
	return 0;
}
