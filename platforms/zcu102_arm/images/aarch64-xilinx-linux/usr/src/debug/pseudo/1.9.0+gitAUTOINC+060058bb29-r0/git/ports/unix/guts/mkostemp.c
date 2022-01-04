/*
 * Copyright (c) 2018 Wind River Systems; see
 * guts/COPYRIGHT for information.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * int mkostemp(char *template, int oflags)
 *	int rc = -1;
 */

	/* mkostemp = mkostemps() with suffixlen of 0 */
	rc = wrap_mkostemps(template, 0, oflags);

/*	return rc;
 * }
 */
