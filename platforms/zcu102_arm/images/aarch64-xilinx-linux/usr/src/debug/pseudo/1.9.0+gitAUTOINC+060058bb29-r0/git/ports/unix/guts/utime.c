/* 
 * Copyright (c) 2010 Wind River Systems; see
 * guts/COPYRIGHT for information.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * static int
 * wrap_utime(const char *path, const struct utimbuf *buf) {
 *	int rc = -1;
 */
	rc = real_utime(path, buf);

/*	return rc;
 * }
 */
