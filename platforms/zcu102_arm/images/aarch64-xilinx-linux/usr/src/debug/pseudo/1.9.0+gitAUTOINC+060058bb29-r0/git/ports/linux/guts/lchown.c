/* 
 * Copyright (c) 2008-2010 Wind River Systems; see
 * guts/COPYRIGHT for information.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * static int
 * wrap_lchown(const char *path, uid_t owner, gid_t group) {
 */

	rc = wrap_fchownat(AT_FDCWD, path, owner, group, AT_SYMLINK_NOFOLLOW);

/*	return rc;
 * }
 */
