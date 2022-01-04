/*
 * Copyright (c) 2012, 2013 Wind River Systems; see
 * guts/COPYRIGHT for information.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * int linkat(int olddirfd, const char *oldname, int newdirfd, const char *newname, int flags)
 *	int rc = -1;
 */

	int rc2 = 0, rflags, save_errno, tmpfile_fd = -1;
 	pseudo_msg_t *msg;
	const char *oldpath = NULL, *newpath = NULL;
 	PSEUDO_STATBUF buf;

	/* This is gratuitously complicated. On Linux 2.6.18 and later,
	 * flags may contain AT_SYMLINK_FOLLOW, which implies following
	 * symlinks; otherwise, linkat() will *not* follow symlinks. FreeBSD
	 * appears to use the same semantics.
	 *
	 * So on Darwin, always pass AT_SYMLINK_FOLLOW, because the
	 * alternative doesn't work. And never pass AT_SYMLINK_NOFOLLOW
	 * because that's not a valid flag to linkat().
	 *
	 * So we need a flag for path resolution which is AT_SYMLINK_NOFOLLOW
	 * unless AT_SYMLINK_FOLLOW was specified, in which case it's 0.
	 */
	
	rflags = (flags & AT_SYMLINK_FOLLOW) ? 0 : AT_SYMLINK_NOFOLLOW;

#ifdef PSEUDO_NO_REAL_AT_FUNCTIONS
	if (olddirfd != AT_FDCWD || newdirfd != AT_FDCWD) {
		errno = ENOSYS;
		return -1;
	}
#endif
	oldpath = oldname;
	if (pseudo_chroot_len && strncmp(oldpath, pseudo_chroot, pseudo_chroot_len) &&
		oldpath[pseudo_chroot_len] == '/') {
		oldpath += pseudo_chroot_len;
	}

	newpath = pseudo_root_path(__func__, __LINE__, newdirfd, newname, AT_SYMLINK_NOFOLLOW);

	/* weird special case: if you link /proc/self/fd/N, you're supposed
	 * to get a link to fd N. Used in conjunction with opening a directory
	 * with O_TMPFILE in flags, which actually opens an already-deleted
	 * file atomically, so there's no way to access the file *at all*
	 * unless it's linked.
	 */
#ifdef O_TMPFILE
	if (!strncmp(oldpath, "/proc/self/fd/", 14) && (flags & AT_SYMLINK_FOLLOW)) {
#ifdef PSEUDO_NO_REAL_AT_FUNCTIONS
		/* only linkat() lets you do this horrible thing */
		errno = ENOSYS;
		return -1;
#endif
		tmpfile_fd = atoi(oldpath + 14);
		// call actual link
		rc = real_linkat(AT_FDCWD, oldpath, AT_FDCWD, newpath, AT_SYMLINK_FOLLOW);
	} else
#endif
	{
		oldpath = pseudo_root_path(__func__, __LINE__, olddirfd, oldname, rflags);
		rc = real_link(oldpath, newpath);
	}

	save_errno = errno;
	if (rc == -1) {
		return rc;
	}

	/* if we got this far, the link succeeded, and oldpath and newpath
	 * are the newly-allocated canonical paths. If OS, filesystem, or
	 * the flags value prevent hard linking to symlinks, the resolved
	 * path should be the target's path anyway, so lstat is safe here.
	 */
	/* find the target: */
	if (tmpfile_fd != -1) {
		rc2 = base_fstat(tmpfile_fd, &buf);
	} else {
		rc2 = base_lstat(oldpath, &buf);
	}
	if (rc2 == -1) {
		pseudo_diag("Fatal: Tried to stat '%s' after linking it, but failed: %s.\n",
			oldpath, strerror(errno));
		errno = ENOENT;
		return rc2;
	}
	if (tmpfile_fd != -1) {
		/* if tmpfile_fd is set, it's *possible* but not *certain* that
		 * we're looking at a file descriptor from an O_TMPFILE open,
		 * which would not be in the database.
		 *
		 * If it's not, we want to treat this like a CREAT operation,
		 * instead of a "link".
		 */
		msg = pseudo_client_op(OP_FSTAT, 0, tmpfile_fd, -1, 0, &buf);
		if (!msg || msg->result != RESULT_SUCCEED) {
			/* create it, mark it as open so we have a path recorded
			 * in the client, and return rc immediately instead
			 * of continuing on to call OP_LINK.
			 */
			pseudo_client_op(OP_CREAT, 0, tmpfile_fd, -1, newpath, &buf);
			pseudo_client_op(OP_OPEN, 0, tmpfile_fd, -1, newpath, &buf);
			errno = save_errno;
			return rc;
		}
	} else {
		msg = pseudo_client_op(OP_STAT, 0, -1, -1, oldpath, &buf);
	}
	if (msg && msg->result == RESULT_SUCCEED) {
		pseudo_stat_msg(&buf, msg);
	}
	/* Long story short: I am pretty sure we still want OP_LINK even
	 * if the thing linked is a symlink.
	 */
	pseudo_client_op(OP_LINK, 0, -1, -1, newpath, &buf);

	errno = save_errno;

/*	return rc;
 * }
 */
