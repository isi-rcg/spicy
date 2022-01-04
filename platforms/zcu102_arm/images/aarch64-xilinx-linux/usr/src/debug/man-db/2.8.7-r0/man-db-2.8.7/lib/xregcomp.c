/*
 * xregcomp.c: regcomp with error checking
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

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include "regex.h"

#include "gettext.h"
#include <locale.h>
#define _(String) gettext (String)

#include "manconfig.h"

#include "error.h"
#include "xregcomp.h"

void xregcomp (regex_t *preg, const char *regex, int cflags)
{
	int err = regcomp (preg, regex, cflags);
	if (err) {
		size_t errstrsize;
		char *errstr;
		errstrsize = regerror (err, preg, NULL, 0);
		errstr = xmalloc (errstrsize);
		regerror (err, preg, errstr, errstrsize);
		error (FATAL, 0, _("fatal: regex `%s': %s"), regex, errstr);
	}
}
