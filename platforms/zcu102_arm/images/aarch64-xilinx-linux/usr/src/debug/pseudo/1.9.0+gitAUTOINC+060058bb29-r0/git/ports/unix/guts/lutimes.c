/* 
 * Copyright (c) 2010 Wind River Systems; see
 * guts/COPYRIGHT for information.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * static int
 * wrap_lutimes(const char *path, const struct timeval *tv) {
 *	int rc = -1;
 */

	rc = real_lutimes(path, tv);

/*	return rc;
 * }
 */
