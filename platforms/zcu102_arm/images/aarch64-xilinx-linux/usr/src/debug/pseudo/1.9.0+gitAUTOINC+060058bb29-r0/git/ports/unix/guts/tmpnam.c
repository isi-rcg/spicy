/* 
 * Copyright (c) 2010 Wind River Systems; see
 * guts/COPYRIGHT for information.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * static char *
 * wrap_tmpnam(char *s) {
 *	char * rc = NULL;
 */

	/* let gcc know we're ignoring this */
	(void) s;
	pseudo_diag("tmpnam() is so ludicrously insecure as to defy implementation.");
	errno = ENOMEM;
	rc = NULL;

/*	return rc;
 * }
 */
