/* 
 * Copyright (c) 2008-2010 Wind River Systems; see
 * guts/COPYRIGHT for information.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * static int
 * wrap_creat(const char *path, mode_t mode) {
 *	int rc = -1;
 */

	rc = wrap_open(path, O_CREAT|O_WRONLY|O_TRUNC, mode);

/*	return rc;
 * }
 */
