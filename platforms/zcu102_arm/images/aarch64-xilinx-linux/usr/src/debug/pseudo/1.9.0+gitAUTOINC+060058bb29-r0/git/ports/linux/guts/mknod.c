/*
 * Copyright (c) 2016 Wind River Systems; see
 * guts/COPYRIGHT for information.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * int mknod(const char *path, mode_t mode, dev_t dev)
 *	int rc = -1;
 */

	rc = wrap___xmknod(_MKNOD_VER, path, mode, &dev);

/*	return rc;
 * }
 */
