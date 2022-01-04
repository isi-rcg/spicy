/*
 * Check decoding of waitpid syscall.
 *
 * Copyright (c) 2016-2018 Dmitry V. Levin <ldv@altlinux.org>
 * Copyright (c) 2016-2019 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "tests.h"
#include "scno.h"

#ifdef __NR_waitpid

# include <stdio.h>
# include <unistd.h>
# include <sys/wait.h>

int
main(void)
{
	unsigned long pid =
		(unsigned long) 0xdefaced00000000ULL | (unsigned) getpid();
	long rc = syscall(__NR_waitpid, pid, 0L, (unsigned long) WNOHANG);
	printf("waitpid(%d, NULL, WNOHANG) = %ld %s (%m)\n",
	       (int) pid, rc, errno2name());

	puts("+++ exited with 0 +++");
	return 0;
}

#else

SKIP_MAIN_UNDEFINED("__NR_waitpid")

#endif
