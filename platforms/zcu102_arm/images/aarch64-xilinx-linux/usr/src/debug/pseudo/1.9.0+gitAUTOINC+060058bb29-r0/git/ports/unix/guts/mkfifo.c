/* 
 * Copyright (c) 2008-2010 Wind River Systems; see
 * guts/COPYRIGHT for information.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * static int
 * wrap_mkfifo(const char *path, mode_t mode) {
 *	int rc = -1;
 */

	rc = wrap_mkfifoat(AT_FDCWD, path, mode);

/*	return rc;
 * }
 */
