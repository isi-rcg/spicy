/*
 * Copyright (c) 2019 Peter Seebach/Seebs <seebs@seebs.net>; see
 * guts/COPYRIGHT for information.
 *
 * [Note: copyright added by code generator, may be
 * incorrect. Remove this if you fix it.]
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * int renameat2(int olddirfd, const char *oldpath, int newdirfd, const char *newpath, unsigned int flags)
 *	int rc = -1;
 */

	(void) olddirfd;
	(void) oldpath;
	(void) newdirfd;
	(void) newpath;
	(void) flags;
	/* for now, let's try just failing out hard, and hope things retry with a
	 * different syscall.
	 */
	errno = ENOSYS;
	rc = -1;

/*	return rc;
 * }
 */
