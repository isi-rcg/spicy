/* 
 * Copyright (c) 2008-2010 Wind River Systems; see
 * guts/COPYRIGHT for information.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * static int
 * wrap_unlink(const char *path) {
 *	int rc = -1;
 */

	rc = wrap_unlinkat(AT_FDCWD, path, 0);

/*	return rc;
 * }
 */
