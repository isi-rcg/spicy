/*
 * Copyright (c) 2015-2016 Dmitry V. Levin <ldv@altlinux.org>
 * Copyright (c) 2015-2019 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "tests.h"
#include "scno.h"

#if defined __NR_get_robust_list && defined __NR_set_robust_list

# include <stdio.h>
# include <unistd.h>

static const char *
sprintaddr(void *addr)
{
	static char buf[sizeof(addr) * 2 + sizeof("0x")];

	if (!addr)
		return "NULL";
	else
		snprintf(buf, sizeof(buf), "%p", addr);

	return buf;
}

int
main(void)
{
	const pid_t pid = getpid();
	const long long_pid = (unsigned long) (0xdeadbeef00000000LL | pid);
	TAIL_ALLOC_OBJECT_CONST_PTR(void *, p_head);
	TAIL_ALLOC_OBJECT_CONST_PTR(size_t, p_len);

	if (syscall(__NR_get_robust_list, long_pid, p_head, p_len))
		perror_msg_and_skip("get_robust_list");
	printf("get_robust_list(%d, [%s], [%lu]) = 0\n",
	       (int) pid, sprintaddr(*p_head), (unsigned long) *p_len);

	void *head = tail_alloc(*p_len);
	if (syscall(__NR_set_robust_list, head, *p_len))
		perror_msg_and_skip("set_robust_list");
	printf("set_robust_list(%p, %lu) = 0\n",
	       head, (unsigned long) *p_len);

	if (syscall(__NR_get_robust_list, long_pid, p_head, p_len))
		perror_msg_and_skip("get_robust_list");
	printf("get_robust_list(%d, [%s], [%lu]) = 0\n",
	       (int) pid, sprintaddr(*p_head), (unsigned long) *p_len);

	puts("+++ exited with 0 +++");
	return 0;
}

#else

SKIP_MAIN_UNDEFINED("__NR_get_robust_list && __NR_set_robust_list")

#endif
