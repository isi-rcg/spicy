/*
 * Copyright (c) 2014 Wind River Systems; see
 * guts/COPYRIGHT for information.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * ssize_t flistxattr(int filedes, char *list, size_t size)
 *	ssize_t rc = -1;
 */
	rc = shared_listxattr(NULL, filedes, list, size);

/*	return rc;
 * }
 */
