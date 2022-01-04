/* 
 * Copyright (c) 2008-2010 Wind River Systems; see
 * guts/COPYRIGHT for information.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * static char *
 * wrap_canonicalize_file_name(const char *filename) {
 *	char * rc = NULL;
 */

	rc = wrap_realpath(filename, NULL);

/*	return rc;
 * }
 */
