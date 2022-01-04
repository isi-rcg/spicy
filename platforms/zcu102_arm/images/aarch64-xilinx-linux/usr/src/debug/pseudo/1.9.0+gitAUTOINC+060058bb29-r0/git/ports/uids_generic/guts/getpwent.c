/* 
 * Copyright (c) 2010 Wind River Systems; see
 * guts/COPYRIGHT for information.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * static struct passwd *
 * wrap_getpwent(void) {
 *	struct passwd * rc = NULL;
 */
	static struct passwd pwd;
	static char pwbuf[PSEUDO_PWD_MAX];
	int r_rc;

	r_rc = wrap_getpwent_r(&pwd, pwbuf, PSEUDO_PWD_MAX, &rc);
	/* different error return conventions */
	if (r_rc != 0) {
		errno = r_rc;
	}

/*	return rc;
 * }
 */
