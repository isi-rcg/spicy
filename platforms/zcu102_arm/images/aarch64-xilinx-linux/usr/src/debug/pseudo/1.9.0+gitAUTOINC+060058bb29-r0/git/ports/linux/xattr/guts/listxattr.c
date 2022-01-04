/*
 * Copyright (c) 2014 Wind River Systems; see
 * guts/COPYRIGHT for information.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * ssize_t listxattr(const char *path, char *list, size_t size)
 *	ssize_t rc = -1;
 */
	rc = shared_listxattr(path, -1, list, size);

/*	return rc;
 * }
 */
