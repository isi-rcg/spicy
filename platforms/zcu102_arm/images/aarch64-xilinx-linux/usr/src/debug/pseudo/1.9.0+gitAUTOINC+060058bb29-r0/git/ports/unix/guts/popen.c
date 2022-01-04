/*
 * Copyright (c) 2012 Wind River Systems; see
 * guts/COPYRIGHT for information.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * FILE *popen(const char *command, const char *mode)
 *	FILE *rc = NULL;
 */
	/* on at least some systems, popen() calls fork and exec
	 * in ways that avoid our usual enforcement of the environment.
	 */
	pseudo_setupenv();
	if (pseudo_has_unload(NULL))
		pseudo_dropenv();

	rc = real_popen(command, mode);

/*	return rc;
 * }
 */
