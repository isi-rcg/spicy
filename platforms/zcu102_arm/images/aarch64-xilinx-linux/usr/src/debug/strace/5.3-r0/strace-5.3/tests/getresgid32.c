/*
 * Copyright (c) 2016-2019 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "tests.h"
#include "scno.h"

#ifdef __NR_getresgid32

# define SYSCALL_NR	__NR_getresgid32
# define SYSCALL_NAME	"getresgid32"
# define UGID_TYPE	int
# include "getresugid.c"

#else

SKIP_MAIN_UNDEFINED("__NR_getresgid32")

#endif
