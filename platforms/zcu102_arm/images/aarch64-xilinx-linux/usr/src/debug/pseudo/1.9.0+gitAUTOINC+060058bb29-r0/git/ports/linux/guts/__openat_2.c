/* 
 * Copyright (c) 2008-2010 Wind River Systems; see
 * guts/COPYRIGHT for information.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * static int
 * wrap___openat_2(int dirfd, const char *path, int flags) {
 *	int rc = -1;
 */

	rc = wrap_openat(dirfd, path, flags, 0);

/*	return rc;
 * }
 */
