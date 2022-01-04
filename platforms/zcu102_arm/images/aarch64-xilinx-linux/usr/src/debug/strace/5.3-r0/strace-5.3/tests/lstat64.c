/*
 * Copyright (c) 2015-2016 Dmitry V. Levin <ldv@altlinux.org>
 * Copyright (c) 2015-2019 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "tests.h"
#include "scno.h"

#ifdef __NR_lstat64

# define TEST_SYSCALL_NR __NR_lstat64
# define TEST_SYSCALL_STR "lstat64"
# define STRUCT_STAT struct stat64
# define STRUCT_STAT_STR "struct stat64"
# define STRUCT_STAT_IS_STAT64 1
# include "lstatx.c"

#else

SKIP_MAIN_UNDEFINED("__NR_lstat64")

#endif
