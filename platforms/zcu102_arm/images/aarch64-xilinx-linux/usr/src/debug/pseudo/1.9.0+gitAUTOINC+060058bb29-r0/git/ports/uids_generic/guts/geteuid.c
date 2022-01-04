/* 
 * Copyright (c) 2008-2010 Wind River Systems; see
 * guts/COPYRIGHT for information.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * static uid_t
 * wrap_geteuid(void) {
 *	uid_t rc = 0;
 */

	rc = pseudo_euid;

/*	return rc;
 * }
 */
