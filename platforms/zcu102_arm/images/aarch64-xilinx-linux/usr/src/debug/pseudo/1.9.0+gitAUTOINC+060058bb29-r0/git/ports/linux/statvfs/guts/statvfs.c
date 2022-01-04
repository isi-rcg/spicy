/*
 * Copyright (c) 2018 Wind River Systems; see
 * guts/COPYRIGHT for information.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * int statvfs(const char *path, struct statvfs *buf)
 *	int rc = -1;
 */

	rc = real_statvfs(path, buf);

/*	return rc;
 * }
 */
