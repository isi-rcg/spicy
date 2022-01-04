/*
 * Copyright (c) 2016-2018 Dmitry V. Levin <ldv@altlinux.org>
 * All rights reserved.
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef STRACE_STATFS_H
# define STRACE_STATFS_H

struct strace_statfs {
	unsigned long long f_type;
	unsigned long long f_bsize;
	unsigned long long f_blocks;
	unsigned long long f_bfree;
	unsigned long long f_bavail;
	unsigned long long f_files;
	unsigned long long f_ffree;
	unsigned long f_fsid[2];
	unsigned long long f_namelen;
	unsigned long long f_frsize;
	unsigned long long f_flags;
};

#endif /* !STRACE_STATFS_H */
