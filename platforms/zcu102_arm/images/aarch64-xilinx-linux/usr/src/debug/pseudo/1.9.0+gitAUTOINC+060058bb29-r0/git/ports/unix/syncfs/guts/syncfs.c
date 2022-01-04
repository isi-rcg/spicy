/*
 * Copyright (c) 2013 Wind River Systems; see
 * guts/COPYRIGHT for information.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * int syncfs(int fd)
 *	int rc = -1;
 */

	rc = real_syncfs(fd);

/*	return rc;
 * }
 */
