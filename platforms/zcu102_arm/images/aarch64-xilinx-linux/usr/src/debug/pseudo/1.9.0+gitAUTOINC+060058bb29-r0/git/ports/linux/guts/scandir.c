/* 
 * Copyright (c) 2010 Wind River Systems; see
 * guts/COPYRIGHT for information.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * static int
 * wrap_scandir(const char *path, struct dirent ***namelist, int (*filter)(const struct dirent *), int (*compar)(const void *, const void *)) {
 *	int rc = -1;
 */

	rc = real_scandir(path, namelist, filter, compar);

/*	return rc;
 * }
 */
