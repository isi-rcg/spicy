/*
 * Copyright (c) 2011 Wind River Systems; see
 * guts/COPYRIGHT for information.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * int stat(const char *path, struct stat *buf)
 *	int rc = -1;
 */

	rc = wrap___fxstatat(_STAT_VER, AT_FDCWD, path, buf, 0);

/*	return rc;
 * }
 */
