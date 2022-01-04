/* 
 * Copyright (c) 2010 Wind River Systems; see
 * guts/COPYRIGHT for information.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * static struct group *
 * wrap_getgrnam(const char *name) {
 *	struct group * rc = NULL;
 */

	static struct group grp;
	static size_t grbufsz = PSEUDO_PWD_MAX;
	static char *grbuf = NULL;
	int r_rc = ERANGE;

	do {
		char *new_grbuf = grbuf;

		if (r_rc != 0)
			new_grbuf = realloc(grbuf, grbufsz);

		if (!new_grbuf) {
			r_rc = ENOMEM;
			break;
		}

		grbuf = new_grbuf;

		r_rc = wrap_getgrnam_r(name, &grp, grbuf, grbufsz, &rc);

		if (r_rc == ERANGE)
			grbufsz = grbufsz << 1;
	} while (r_rc == ERANGE);

	/* different error return conventions */
	if (r_rc != 0) {
		errno = r_rc;
	}


/*	return rc;
 * }
 */
