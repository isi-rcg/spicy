/* 
 * Copyright (c) 2008-2010 Wind River Systems; see
 * guts/COPYRIGHT for information.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * int
 * wrap___fxstat(int ver, int fd, struct stat *buf) {
 *	int rc = -1;
 */

	struct stat64 buf64;
	/* populate buffer with complete data */
	real___fxstat(ver, fd, buf);
	/* obtain fake data */
	rc = wrap___fxstat64(ver, fd, &buf64);
	/* overwrite */
	pseudo_stat32_from64(buf, &buf64);

/*	return rc;
 * }
 */
