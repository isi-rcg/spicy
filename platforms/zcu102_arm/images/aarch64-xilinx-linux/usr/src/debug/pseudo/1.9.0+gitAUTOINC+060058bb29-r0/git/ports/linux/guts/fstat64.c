/*
 * Copyright (c) 2012 Wind River Systems; see
 * guts/COPYRIGHT for information.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * int fstat64(int fd, struct stat *buf)
 *	int rc = -1;
 */

	rc = wrap___fxstat64(_STAT_VER, fd, buf);

/*	return rc;
 * }
 */
