/*
 * Copyright (c) 2012 Wind River Systems; see
 * guts/COPYRIGHT for information.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * int stat64(const char *path, struct stat *buf)
 *	int rc = -1;
 */

	rc = wrap___fxstatat64(_STAT_VER, AT_FDCWD, path, buf, 0);

/*	return rc;
 * }
 */
