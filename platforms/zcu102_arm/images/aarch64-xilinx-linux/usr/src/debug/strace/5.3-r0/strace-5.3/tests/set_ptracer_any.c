/*
 * Copyright (c) 2013-2015 Dmitry V. Levin <ldv@altlinux.org>
 * Copyright (c) 2013-2018 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "tests.h"
#include <stdio.h>
#include <unistd.h>
#ifdef HAVE_PRCTL
# include <sys/prctl.h>
#endif

int main(int argc, char **argv)
{
	if (argc < 2)
		return 99;
#ifdef HAVE_PRCTL
	/* Turn off restrictions on tracing if applicable.  If the command
	 * aren't available on this system, that's OK too.  */
# ifndef PR_SET_PTRACER
#  define PR_SET_PTRACER 0x59616d61
# endif
# ifndef PR_SET_PTRACER_ANY
#  define PR_SET_PTRACER_ANY -1UL
# endif
	(void) prctl(PR_SET_PTRACER, PR_SET_PTRACER_ANY, 0, 0, 0);
#endif
	if (write(1, "\n", 1) != 1) {
		perror("write");
		return 99;
	}
	(void) execvp(argv[1], argv + 1);
	perror(argv[1]);
	return 99;
}
