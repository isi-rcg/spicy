/*
 * Copyright (c) 2014 Wind River Systems; see
 * guts/COPYRIGHT for information.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * int lremovexattr(const char *path, const char *name)
 *	int rc = -1;
 */
	rc = shared_removexattr(path, -1, name);

/*	return rc;
 * }
 */
