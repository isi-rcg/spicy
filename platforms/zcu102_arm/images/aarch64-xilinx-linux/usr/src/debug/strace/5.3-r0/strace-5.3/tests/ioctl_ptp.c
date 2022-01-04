/*
 * Check decoding of PTP_* commands of ioctl syscall.
 *
 * Copyright (c) 2018 Harsha Sharma <harshasharmaiitr@gmail.com>
 * Copyright (c) 2018 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "tests.h"

#ifdef HAVE_STRUCT_PTP_SYS_OFFSET

# include <errno.h>
# include <fcntl.h>
# include <inttypes.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/ioctl.h>
# include <linux/ptp_clock.h>

# include "xlat.h"
# include "xlat/ptp_flags_options.h"

static void
test_no_device(void)
{
	TAIL_ALLOC_OBJECT_CONST_PTR(struct ptp_clock_caps, caps);
	fill_memory(caps, sizeof(*caps));

	TAIL_ALLOC_OBJECT_CONST_PTR(struct ptp_sys_offset, sysoff);
	fill_memory(sysoff, sizeof(*sysoff));

	TAIL_ALLOC_OBJECT_CONST_PTR(struct ptp_extts_request, extts);
	fill_memory(extts, sizeof(*extts));

	TAIL_ALLOC_OBJECT_CONST_PTR(struct ptp_perout_request, perout);
	fill_memory(perout, sizeof(*perout));

	int saved_errno;

	/* PTP_CLOCK_GETCAPS */
	ioctl(-1, PTP_CLOCK_GETCAPS, NULL);
	printf("ioctl(-1, PTP_CLOCK_GETCAPS, NULL) = -1 EBADF (%m)\n");
	ioctl(-1, PTP_CLOCK_GETCAPS, caps);
	printf("ioctl(-1, PTP_CLOCK_GETCAPS, %p) = -1 EBADF (%m)\n", caps);

	/* PTP_SYS_OFFSET */
	ioctl(-1, PTP_SYS_OFFSET, NULL);
	printf("ioctl(-1, PTP_SYS_OFFSET, NULL) = -1 EBADF (%m)\n");
	ioctl(-1, PTP_SYS_OFFSET, sysoff);
	printf("ioctl(-1, PTP_SYS_OFFSET, {n_samples=%u}) = -1 EBADF (%m)\n",
	       sysoff->n_samples);

	/* PTP_ENABLE_PPS */
	ioctl(-1, PTP_ENABLE_PPS, 0);
	printf("ioctl(-1, PTP_ENABLE_PPS, 0) = -1 EBADF (%m)\n");
	ioctl(-1, PTP_ENABLE_PPS, 1);
	printf("ioctl(-1, PTP_ENABLE_PPS, 1) = -1 EBADF (%m)\n");

	/* PTP_EXTTS_REQUEST */
	ioctl(-1, PTP_EXTTS_REQUEST, NULL);
	printf("ioctl(-1, PTP_EXTTS_REQUEST, NULL) = -1 EBADF (%m)\n");
	ioctl(-1, PTP_EXTTS_REQUEST, extts);
	saved_errno = errno;
	printf("ioctl(-1, PTP_EXTTS_REQUEST, {index=%d, flags=", extts->index);
	printflags(ptp_flags_options, extts->flags, "PTP_???");
	errno = saved_errno;
	printf("}) = -1 EBADF (%m)\n");

	/* PTP_PEROUT_REQUEST */
	ioctl(-1, PTP_PEROUT_REQUEST, NULL);
	printf("ioctl(-1, PTP_PEROUT_REQUEST, NULL) = -1 EBADF (%m)\n");
	ioctl(-1, PTP_PEROUT_REQUEST, perout);
	saved_errno = errno;
	printf("ioctl(-1, PTP_PEROUT_REQUEST, {start={sec=%" PRId64
	       ", nsec=%" PRIu32 "}, period={sec=%" PRId64 ", nsec=%" PRIu32 "}"
	       ", index=%d, flags=",
	       (int64_t) perout->start.sec, perout->start.nsec,
	       (int64_t)perout->period.sec, perout->period.nsec, perout->index);
	printflags(ptp_flags_options, perout->flags, "PTP_???");
	errno = saved_errno;
	printf("}) = -1 EBADF (%m)\n");

	/* unrecognized */
	ioctl(-1, _IOC(_IOC_READ, PTP_CLK_MAGIC, 0xff, 0xfe), 0);
	printf("ioctl(-1, _IOC(_IOC_READ, %#x, 0xff, 0xfe), 0)"
	       " = -1 EBADF (%m)\n", PTP_CLK_MAGIC);

	const unsigned long arg = (unsigned long) 0xfacefeeddeadbeefULL;
	ioctl(-1, _IOC(_IOC_WRITE, PTP_CLK_MAGIC, 0xfd, 0xfc), arg);
	printf("ioctl(-1, _IOC(_IOC_WRITE, %#x, 0xfd, 0xfc), %#lx)"
	       " = -1 EBADF (%m)\n", PTP_CLK_MAGIC, arg);
}

int
main(void)
{
	test_no_device();

	puts("+++ exited with 0 +++");
	return 0;
}

#else

SKIP_MAIN_UNDEFINED("HAVE_STRUCT_PTP_SYS_OFFSET")

#endif /* HAVE_STRUCT_PTP_SYS_OFFSET */
