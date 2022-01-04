/* 
 * Copyright (c) 2010 Wind River Systems; see
 * guts/COPYRIGHT for information.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * static int
 * wrap_ftw(const char *path, int (*fn)(const char *, const struct stat *, int), int nopenfd) {
 *	int rc = -1;
 */

	rc = real_ftw(path, fn, nopenfd);

/*	return rc;
 * }
 */
