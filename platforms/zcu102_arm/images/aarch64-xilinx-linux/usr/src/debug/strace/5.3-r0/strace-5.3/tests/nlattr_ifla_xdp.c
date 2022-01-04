/*
 * Copyright (c) 2017 JingPiao Chen <chenjingpiao@gmail.com>
 * Copyright (c) 2017-2018 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "tests.h"

#include <stdio.h>
#include "test_nlattr.h"
#include <linux/if.h>
#include <linux/if_arp.h>
#ifdef HAVE_LINUX_IF_LINK_H
# include <linux/if_link.h>
#endif
#include <linux/rtnetlink.h>

#if !HAVE_DECL_IFLA_XDP
enum { IFLA_XDP = 43 };
#endif
#ifndef IFLA_XDP_FD
# define IFLA_XDP_FD 1
#endif

#ifndef IFLA_XDP_ATTACHED
# define IFLA_XDP_ATTACHED 2
#endif

#ifndef IFLA_XDP_PROG_ID
# define IFLA_XDP_PROG_ID 4
#endif

#ifndef IFLA_XDP_DRV_PROG_ID
# define IFLA_XDP_DRV_PROG_ID 5
#endif

#ifndef IFLA_XDP_SKB_PROG_ID
# define IFLA_XDP_SKB_PROG_ID 6
#endif

#ifndef IFLA_XDP_HW_PROG_ID
# define IFLA_XDP_HW_PROG_ID 7
#endif

#ifndef XDP_ATTACHED_NONE
# define XDP_ATTACHED_NONE 0
#endif

#ifndef XDP_ATTACHED_MULTI
# define XDP_ATTACHED_MULTI 4
#endif

#define IFLA_ATTR IFLA_XDP
#include "nlattr_ifla.h"

int
main(void)
{
	skip_if_unavailable("/proc/self/fd/");

	const int32_t num = 0xabacdbcd;
	const int fd = create_nl_socket(NETLINK_ROUTE);
	void *nlh0 = midtail_alloc(NLMSG_SPACE(hdrlen),
				   NLA_HDRLEN + sizeof(num));

	static char pattern[4096];
	fill_memory_ex(pattern, sizeof(pattern), 'a', 'z' - 'a' + 1);

	TEST_NESTED_NLATTR_OBJECT(fd, nlh0, hdrlen,
				  init_ifinfomsg, print_ifinfomsg,
				  IFLA_XDP_FD, pattern, num,
				  printf("%d", num));

	static const struct {
		uint8_t val;
		const char *str;
	} attach_types[] = {
		{ ARG_STR(XDP_ATTACHED_NONE) },
		{ ARG_STR(XDP_ATTACHED_MULTI) },
		{ ARG_STR(0x5)  " /* XDP_ATTACHED_??? */" },
		{ ARG_STR(0xfe) " /* XDP_ATTACHED_??? */" },
	};

	for (size_t i = 0; i < ARRAY_SIZE(attach_types); i++) {
		TEST_NESTED_NLATTR_OBJECT(fd, nlh0, hdrlen,
					  init_ifinfomsg, print_ifinfomsg,
					  IFLA_XDP_ATTACHED, pattern,
					  attach_types[i].val,
					  printf("%s", attach_types[i].str));
	}

#ifdef XDP_FLAGS_UPDATE_IF_NOEXIST
	const uint32_t flags = XDP_FLAGS_UPDATE_IF_NOEXIST;
	TEST_NESTED_NLATTR_OBJECT(fd, nlh0, hdrlen,
				  init_ifinfomsg, print_ifinfomsg,
				  IFLA_XDP_FLAGS, pattern, flags,
				  printf("XDP_FLAGS_UPDATE_IF_NOEXIST"));
#endif

	static const struct {
		uint32_t val;
		const char *str;
	} attrs[] = {
		{ ARG_STR(IFLA_XDP_PROG_ID) },
		{ ARG_STR(IFLA_XDP_DRV_PROG_ID) },
		{ ARG_STR(IFLA_XDP_SKB_PROG_ID) },
		{ ARG_STR(IFLA_XDP_HW_PROG_ID) },
	};

	for (size_t i = 0; i < ARRAY_SIZE(attrs); i++) {
		TEST_NESTED_NLATTR_OBJECT_EX_(fd, nlh0, hdrlen,
					      init_ifinfomsg, print_ifinfomsg,
					      attrs[i].val, attrs[i].str,
					      pattern, num,
					      print_quoted_hex, 1,
					      printf("%u", num));
	}

	puts("+++ exited with 0 +++");
	return 0;
}
