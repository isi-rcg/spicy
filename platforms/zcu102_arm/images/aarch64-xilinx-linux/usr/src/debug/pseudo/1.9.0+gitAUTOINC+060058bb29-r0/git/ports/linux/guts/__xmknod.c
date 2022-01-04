/* 
 * Copyright (c) 2008-2010 Wind River Systems; see
 * guts/COPYRIGHT for information.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * static int
 * wrap___xmknod(int ver, const char *path, mode_t mode, dev_t *dev) {
 *	int rc = -1;
 */

	rc = wrap___xmknodat(ver, AT_FDCWD, path, mode, dev);

/*	return rc;
 * }
 */
