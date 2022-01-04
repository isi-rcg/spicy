/* 
 * Copyright (c) 2010 Wind River Systems; see
 * guts/COPYRIGHT for information.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * static int
 * wrap_euidaccess(const char *path, int mode) {
 *	int rc = -1;
 */

	rc = wrap_access(path, mode);

/*	return rc;
 * }
 */
