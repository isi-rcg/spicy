/*
 * Copyright (c) 2015-2019 Dmitry V. Levin <ldv@altlinux.org>
 * All rights reserved.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "tests.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include "scno.h"

#if defined __NR_umount2 && (!defined __NR_umount || __NR_umount2 != __NR_umount)
# define TEST_SYSCALL_NR __NR_umount2
# define TEST_SYSCALL_STR "umount2"
#else
# define TEST_SYSCALL_NR __NR_umount
# define TEST_SYSCALL_STR "umount"
#endif

int
main(void)
{
	static const char sample[] = "umount2.sample";
	if (mkdir(sample, 0700))
		perror_msg_and_fail("mkdir: %s", sample);
	(void) syscall(TEST_SYSCALL_NR, sample, 31);
	printf("%s(\"%s\", MNT_FORCE|MNT_DETACH|MNT_EXPIRE|UMOUNT_NOFOLLOW|0x10)"
	       " = -1 EINVAL (%m)\n", TEST_SYSCALL_STR, sample);
	if (rmdir(sample))
		perror_msg_and_fail("rmdir: %s", sample);
	puts("+++ exited with 0 +++");
	return 0;
}
