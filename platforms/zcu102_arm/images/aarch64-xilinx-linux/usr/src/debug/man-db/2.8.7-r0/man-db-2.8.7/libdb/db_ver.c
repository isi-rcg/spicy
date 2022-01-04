/*
 * dbver.c: code to read, write and identify the database version no.
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
 * Mon Aug 18 20:35:30 BST 1994  Wilf. (G.Wilford@ee.surrey.ac.uk)
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <string.h>
#include <stdlib.h>

#include "gettext.h"
#define _(String) gettext (String)

#include "manconfig.h"

#include "error.h"

#include "mydbm.h"

int dbver_rd (MYDBM_FILE dbfile)
{
	datum key, content;

	memset (&key, 0, sizeof key);

	MYDBM_SET (key, xstrdup (VER_KEY));

	content = MYDBM_FETCH (dbfile, key);

	MYDBM_FREE_DPTR (key);

	if (MYDBM_DPTR (content) == NULL) {
		debug (_("warning: %s has no version identifier\n"),
		       dbfile->name);
		return 1;
	} else if (!STREQ (MYDBM_DPTR (content), VER_ID)) {
		debug (_("warning: %s is version %s, expecting %s\n"),
		       dbfile->name, MYDBM_DPTR (content), VER_ID);
		MYDBM_FREE_DPTR (content);
		return 1;
	} else {
		MYDBM_FREE_DPTR (content);
		return 0;
	}
}

void dbver_wr (MYDBM_FILE dbfile)
{
	datum key, content;

	memset (&key, 0, sizeof key);
	memset (&content, 0, sizeof content);

	MYDBM_SET (key, xstrdup (VER_KEY));
	MYDBM_SET (content, xstrdup (VER_ID));

	if (MYDBM_INSERT (dbfile, key, content) != 0)
		error (FATAL, 0,
		       _("fatal: unable to insert version identifier into %s"),
		       dbfile->name);

	MYDBM_FREE_DPTR (key);
	MYDBM_FREE_DPTR (content);
}
