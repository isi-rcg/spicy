/*
 * Execute an executable with zero argc and specified anvironment.
 *
 * Copyright (c) 2017-2018 Dmitry V. Levin <ldv@altlinux.org>
 * All rights reserved.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "tests.h"
#include <stdlib.h>
#include <unistd.h>

int
main(const int ac, char **const av)
{
	if (ac < 2)
		error_msg_and_fail("missing operand");
	const char *const path = av[1];
	av[1] = 0;
	execve(path, av + 1, av + 2);
	perror_msg_and_fail("execve: %s", path);
}
