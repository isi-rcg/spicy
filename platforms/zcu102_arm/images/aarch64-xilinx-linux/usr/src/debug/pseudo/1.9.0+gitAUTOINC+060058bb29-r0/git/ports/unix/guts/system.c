/*
 * Copyright (c) 2011, 2012 Wind River Systems; see
 * guts/COPYRIGHT for information.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * int system(const char *command)
 *	int rc = -1;
 */
	if (!command)
		return 1;

	pseudo_setupenv();
	if (pseudo_has_unload(NULL))
		pseudo_dropenv();

	rc = real_system(command);

/*	return rc;
 * }
 */
