/*
 * Copyright (c) 2019 Linux Foundation
 * Author: Richard Purdie
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * int
 * statx(int dirfd, const char *pathname, int flags, unsigned int mask, struct statx *statxbuf) {
 *	int rc = -1;
 */
	pseudo_msg_t *msg;
	PSEUDO_STATBUF buf;
	int save_errno;

	rc = real_statx(dirfd, pathname, flags, mask, statxbuf);
	save_errno = errno;
	if (rc == -1) {
		return rc;
	}

	buf.st_uid = statxbuf->stx_uid;
	buf.st_gid = statxbuf->stx_gid;
	buf.st_dev = makedev(statxbuf->stx_dev_major, statxbuf->stx_dev_minor);
	buf.st_ino = statxbuf->stx_ino;
	buf.st_mode = statxbuf->stx_mode;
	buf.st_rdev = makedev(statxbuf->stx_rdev_major, statxbuf->stx_rdev_minor);
	buf.st_nlink = statxbuf->stx_nlink;
	msg = pseudo_client_op(OP_STAT, 0, -1, dirfd, pathname, &buf);
	if (msg && msg->result == RESULT_SUCCEED) {
		pseudo_debug(PDBGF_FILE, "statx(path %s), flags %o, stat rc %d, stat uid %o\n", pathname, flags, rc, statxbuf->stx_uid);
		statxbuf->stx_uid = msg->uid;
		statxbuf->stx_gid = msg->gid;
		statxbuf->stx_mode = msg->mode;
		statxbuf->stx_rdev_major = major(msg->rdev);
		statxbuf->stx_rdev_minor = minor(msg->rdev);
	} else {
		pseudo_debug(PDBGF_FILE, "statx(path %s) failed, flags %o, stat rc %d, stat uid %o\n", pathname, flags, rc, statxbuf->stx_uid);
	}
	errno = save_errno;
/*	return rc;
 * }
 */
