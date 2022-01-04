/* 
 * Copyright (c) 2010 Wind River Systems; see
 * guts/COPYRIGHT for information.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * static struct group *
 * wrap_getgrent(void) {
 *	struct group * rc = NULL;
 */
	static struct group grp;
	static size_t grbuflen = PSEUDO_PWD_MAX;
	static char *grbuf = NULL;
	int r_rc = ERANGE;

	do {
		char *new_grbuf = grbuf;

		if (r_rc == ERANGE)
			new_grbuf = realloc(grbuf, grbuflen);

		if (!new_grbuf) {
			r_rc = ENOMEM;
			break;
		}

		grbuf = new_grbuf;

		r_rc = wrap_getgrent_r(&grp, grbuf, grbuflen, &rc);

		if (r_rc == ERANGE)
			grbuflen = grbuflen << 1;

	} while (r_rc == ERANGE);

	/* different error return conventions */
	if (r_rc != 0) {
		errno = r_rc;
	}

/*	return rc;
 * }
 */
