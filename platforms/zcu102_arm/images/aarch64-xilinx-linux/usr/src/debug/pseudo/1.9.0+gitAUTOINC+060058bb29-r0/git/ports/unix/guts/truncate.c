/* 
 * Copyright (c) 2010 Wind River Systems; see
 * guts/COPYRIGHT for information.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * static int
 * wrap_truncate(const char *path, off_t length) {
 *	int rc = -1;
 */

	rc = real_truncate(path, length);

/*	return rc;
 * }
 */
