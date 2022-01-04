/*
 * Check decoding of fanotify_init syscall.
 *
 * Copyright (c) 2016 Eugene Syromyatnikov <evgsyr@gmail.com>
 * Copyright (c) 2016-2019 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "tests.h"

#include "scno.h"

#if defined __NR_fanotify_init

# include <limits.h>
# include <stdio.h>
# include <unistd.h>

/* Performs fanotify_init call via the syscall interface. */
static void
do_call(kernel_ulong_t flags, const char *flags_str,
	kernel_ulong_t event_f_flags, const char *event_f_flags_str)
{
	long rc;

	rc = syscall(__NR_fanotify_init, flags, event_f_flags);

	printf("fanotify_init(%s, %s) = %s\n",
	       flags_str, event_f_flags_str, sprintrc(rc));
}

struct strval {
	kernel_ulong_t val;
	const char *str;
};


int
main(void)
{
	static const struct strval flags[] = {
		{ F8ILL_KULONG_MASK, "FAN_CLASS_NOTIF" },
		{ (kernel_ulong_t) 0xffffffff0000000cULL,
			"0xc /* FAN_CLASS_??? */" },
		{ (kernel_ulong_t) 0xdec0deddefacec04ULL,
			"FAN_CLASS_CONTENT|0xefacec00 /* FAN_??? */" },
		{ (kernel_ulong_t) 0xffffffffffffffffULL,
			"0xc /* FAN_CLASS_??? */|FAN_CLOEXEC|FAN_NONBLOCK|"
			"FAN_UNLIMITED_QUEUE|FAN_UNLIMITED_MARKS|"
			"FAN_ENABLE_AUDIT|FAN_REPORT_TID|FAN_REPORT_FID|"
			"0xfffffc80" },
	};
	static const struct strval event_f_flags[] = {
		{ F8ILL_KULONG_MASK, "O_RDONLY" },
		{ (kernel_ulong_t) 0xdeadbeef80000001ULL,
			"O_WRONLY|0x80000000" }
	};

	unsigned int i;
	unsigned int j;


	for (i = 0; i < ARRAY_SIZE(flags); i++)
		for (j = 0; j < ARRAY_SIZE(event_f_flags); j++)
			do_call(flags[i].val, flags[i].str,
				event_f_flags[j].val, event_f_flags[j].str);

	puts("+++ exited with 0 +++");
	return 0;
}

#else

SKIP_MAIN_UNDEFINED("__NR_fanotify_init")

#endif
