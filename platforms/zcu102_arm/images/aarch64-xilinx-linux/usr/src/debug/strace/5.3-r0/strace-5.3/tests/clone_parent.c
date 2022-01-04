/*
 * Check handling of CLONE_PARENT'ed processes.
 *
 * Copyright (c) 2017-2018 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "tests.h"

#include <errno.h>
#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

static int
child(void *const arg)
{
	return 42;
}

#define child_stack_size	(get_page_size() / 2)

#ifdef IA64
extern int __clone2(int (*)(void *), void *, size_t, int, void *, ...);
# define clone(fn, child_stack, flags, arg)	\
		__clone2(fn, child_stack, child_stack_size, flags, arg)
#endif

int
main(void)
{
	const pid_t pid = clone(child, tail_alloc(child_stack_size),
				CLONE_PARENT | SIGCHLD, 0);
	if (pid < 0)
		perror_msg_and_fail("clone");

	int status;
	if (wait(&status) >= 0)
		error_msg_and_fail("unexpected return code from wait");

	while (!kill(pid, 0))
		;
	if (errno != ESRCH)
		perror_msg_and_fail("kill");

	FILE *const fp = fdopen(3, "a");
	if (!fp)
		perror_msg_and_fail("fdopen");
	if (fprintf(fp, "%s: Exit of unknown pid %d ignored\n",
		    getenv("STRACE_EXE") ?: "strace", pid) < 0)
		perror_msg_and_fail("fprintf");

	puts("+++ exited with 0 +++");
	return 0;
}
