/* 
 * Copyright (c) 2010 Wind River Systems; see
 * guts/COPYRIGHT for information.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * static char *
 * wrap_tempnam(const char *template, const char *pfx) {
 *	char * rc = NULL;
 */
	/* let gcc know we ignored these on purpose */
	(void) template;
	(void) pfx;
	pseudo_diag("tempnam() is so ludicrously insecure as to defy implementation.");
	errno = ENOMEM;
	rc = NULL;

/*	return rc;
 * }
 */
