/*
 * Copyright (c) 2015-2018 Dmitry V. Levin <ldv@altlinux.org>
 * Copyright (c) 2015-2019 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "tests.h"
#include "scno.h"

#ifdef __NR_lseek

# include <stdio.h>
# include <unistd.h>

int
main(void)
{
	const kernel_ulong_t offset = (kernel_ulong_t) 0xfacefeeddeadbeefULL;

	if (sizeof(offset) > sizeof(long)) {
		/*
		 * Cannot use syscall because it takes long arguments.
		 * Let's call lseek with hope it will invoke lseek syscall.
		 */
		long long rc = lseek(-1, offset, SEEK_SET);
		printf("lseek(-1, %lld, SEEK_SET) = %lld %s (%m)\n",
		       (long long) offset, rc, errno2name());
	} else {
		long rc = syscall(__NR_lseek, -1L, offset, SEEK_SET);
		printf("lseek(-1, %ld, SEEK_SET) = %ld %s (%m)\n",
		       (long) offset, rc, errno2name());
	}

	puts("+++ exited with 0 +++");
	return 0;
}

#else

SKIP_MAIN_UNDEFINED("__NR_lseek")

#endif
