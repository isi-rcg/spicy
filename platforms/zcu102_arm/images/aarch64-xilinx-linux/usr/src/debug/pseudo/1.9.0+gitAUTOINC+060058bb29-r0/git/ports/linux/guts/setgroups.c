/* 
 * Copyright (c) 2010 Wind River Systems; see
 * guts/COPYRIGHT for information.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * static int
 * wrap_setgroups(size_t size, const gid_t *list) {
 *	int rc = -1;
 */

	/* let gcc know we're ignoring these */
	(void) size;
	(void) list;
	/* you always have all group privileges.  we're like magic! */
	rc = 0;

/*	return rc;
 * }
 */
