/* 
 * Copyright (c) 2015 Wind River Systems; see
 * guts/COPYRIGHT for information.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * static int
 * wrap_mkfifoat(int dirfd, const char *path, mode_t mode) {
 *	int rc = -1;
 */

 	pseudo_msg_t *msg;
	PSEUDO_STATBUF buf;
        int save_errno = errno;

	/* mask out mode bits appropriately */
	mode = mode & ~pseudo_umask;

#ifdef PSEUDO_NO_REAL_AT_FUNCTIONS
	if (dirfd != AT_FDCWD) {
		errno = ENOSYS;
		return -1;
	}
	rc = base_stat(path, &buf);
#else
	rc = base_fstatat(dirfd, path, &buf, AT_SYMLINK_NOFOLLOW);
#endif
	if (rc != -1) {
		/* if we can stat the file, you can't mkfifo it */
		errno = EEXIST;
		return -1;
	}
#ifdef PSEUDO_NO_REAL_AT_FUNCTIONS
	rc = real_mkfifo(path, PSEUDO_FS_MODE(mode, 0));
	if (rc == -1) {
		return -1;
	}
	save_errno = errno;
	rc = base_stat(path, &buf);
	real_chmod(path, PSEUDO_FS_MODE(mode, 0));
#else
	rc = real_mkfifoat(dirfd, path, PSEUDO_FS_MODE(mode, 0));
	if (rc == -1) {
		return -1;
	}
	save_errno = errno;
	rc = base_fstatat(dirfd, path, &buf, AT_SYMLINK_NOFOLLOW);
	real_fchmodat(dirfd, path, PSEUDO_FS_MODE(mode, 0), 0);
#endif
	/* if the stat failed, we are going to give up and nuke
	 * any file we may have created, and hope for the best.
	 */
	if (rc == 0) {
		buf.st_mode = PSEUDO_DB_MODE(buf.st_mode, mode);
		/* mkfifo/mknod are the same op, in that they create a file
		 * with a non-file type.
		 */
		msg = pseudo_client_op(OP_MKNOD, 0, -1, dirfd, path, &buf);
		if (msg && msg->result != RESULT_SUCCEED) {
			errno = EPERM;
			rc = -1;
		} else {
			/* just pretend we worked */
			errno = save_errno;
			rc = 0;
		}
	}
	if (rc == -1) {
                save_errno = errno;
#ifdef PSEUDO_NO_REAL_AT_FUNCTIONS
		real_unlink(path);
#else
		real_unlinkat(dirfd, path, AT_SYMLINK_NOFOLLOW);
#endif
		errno = save_errno;
	}

/*	return rc;
 * }
 */
