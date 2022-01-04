/*
 * Copyright (c) 2019 Dmitry V. Levin <ldv@altlinux.org>
 * All rights reserved.
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "defs.h"
#ifdef HAVE_LINUX_MOUNT_H
# include <linux/mount.h>
#endif
#include "xlat/fsconfig_cmds.h"

SYS_FUNC(fsconfig)
{
	const int fs_fd = tcp->u_arg[0];
	const unsigned int cmd = tcp->u_arg[1];
	const kernel_ulong_t key = tcp->u_arg[2];
	const kernel_ulong_t value = tcp->u_arg[3];
	const int aux = tcp->u_arg[4];

	printfd(tcp, fs_fd);
	tprints(", ");

	printxval(fsconfig_cmds, cmd, "FSCONFIG_???");
	tprints(", ");

	switch (cmd) {
		case FSCONFIG_SET_FLAG:
		case FSCONFIG_SET_STRING:
		case FSCONFIG_SET_BINARY:
		case FSCONFIG_SET_PATH:
		case FSCONFIG_SET_PATH_EMPTY:
		case FSCONFIG_SET_FD:
			printstr_ex(tcp, key, 256, QUOTE_0_TERMINATED);
			break;
		case FSCONFIG_CMD_CREATE:
		case FSCONFIG_CMD_RECONFIGURE:
		default:
			printaddr(key);
			break;
	}
	tprints(", ");

	switch (cmd) {
		case FSCONFIG_SET_STRING:
			printstr_ex(tcp, value, 256, QUOTE_0_TERMINATED);
			break;
		case FSCONFIG_SET_PATH:
		case FSCONFIG_SET_PATH_EMPTY:
			printpath(tcp, value);
			break;
		case FSCONFIG_SET_BINARY:
			if (aux >= 0 && aux <= 1024 * 1024) {
				printstr_ex(tcp, value, aux, QUOTE_FORCE_HEX);
				break;
			}
			ATTRIBUTE_FALLTHROUGH;
		case FSCONFIG_SET_FLAG:
		case FSCONFIG_SET_FD:
		case FSCONFIG_CMD_CREATE:
		case FSCONFIG_CMD_RECONFIGURE:
		default:
			printaddr(value);
			break;
	}
	tprints(", ");

	switch (cmd) {
		case FSCONFIG_SET_PATH:
		case FSCONFIG_SET_PATH_EMPTY:
			print_dirfd(tcp, aux);
			break;
		case FSCONFIG_SET_FD:
			printfd(tcp, aux);
			break;
		case FSCONFIG_SET_FLAG:
		case FSCONFIG_SET_STRING:
		case FSCONFIG_SET_BINARY:
		case FSCONFIG_CMD_CREATE:
		case FSCONFIG_CMD_RECONFIGURE:
		default:
			tprintf("%d", aux);
			break;
	}

	return RVAL_DECODED;
}
