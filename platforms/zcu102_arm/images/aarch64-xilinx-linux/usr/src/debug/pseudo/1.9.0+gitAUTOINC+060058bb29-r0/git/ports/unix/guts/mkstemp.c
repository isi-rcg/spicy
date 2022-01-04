/* 
 * Copyright (c) 2008-2010, 2012 Wind River Systems; see
 * guts/COPYRIGHT for information.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * static int
 * wrap_mkstemp(char *template) {
 *	int rc = -1;
 */
	/* mkstemp() is just like mkostemps() with no flags and no suffix */
	rc = wrap_mkostemps(template, 0, 0);

/*	return rc;
 * }
 */
