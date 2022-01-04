/* 
 * Copyright (c) 2008-2010, 2013 Wind River Systems; see
 * guts/COPYRIGHT for information.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * static int
 * wrap_openat(int dirfd, const char *path, int flags, ...mode_t mode) {
 *	int rc = -1;
 */
	struct stat64 buf;
	int overly_magic_nonblocking = 0;
	int existed = 1;
	int save_errno;
	sigset_t local_saved_sigmask;

	/* mask out mode bits appropriately */
	mode = mode & ~pseudo_umask;

#ifdef PSEUDO_NO_REAL_AT_FUNCTIONS
	if (dirfd != AT_FDCWD) {
		errno = ENOSYS;
		return -1;
	}
#endif

#ifdef PSEUDO_FORCE_ASYNC
        /* Yes, I'm aware that every Linux system I've seen has
         * DSYNC and RSYNC being the same value as SYNC.
         */

        flags &= ~(O_SYNC
#ifdef O_DIRECT
                | O_DIRECT
#endif
#ifdef O_DSYNC
                | O_DSYNC
#endif
#ifdef O_RSYNC
                | O_RSYNC
#endif
        );
#endif

#ifdef O_TMPFILE
	/* don't handle O_CREAT the same way if O_TMPFILE exists
	 * and is set.
	 */
	if ((flags & O_TMPFILE) == O_TMPFILE) {
		existed = 0;
	} else
#endif
	/* if a creation has been requested, check whether file exists */
	/* note "else" in #ifdef O_TMPFILE above */
	if (flags & O_CREAT) {
		save_errno = errno;
#ifdef PSEUDO_NO_REAL_AT_FUNCTIONS
		if (flags & O_NOFOLLOW) {
			rc = real___lxstat64(_STAT_VER, path, &buf);
		} else {
			rc = real___xstat64(_STAT_VER, path, &buf);
		}
#else
		rc = real___fxstatat64(_STAT_VER, dirfd, path, &buf, (flags & O_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0);
#endif
		existed = (rc != -1);
		if (!existed)
			pseudo_debug(PDBGF_FILE, "openat_creat: %s -> 0%o\n", path, mode);
		errno = save_errno;
	}

	/* if a pipe is opened without O_NONBLOCK, for only reading or
	 * only writing, it can block forever. We need to do extra magic
	 * in that case...
	 */
	if (!(flags & O_NONBLOCK) && ((flags & (O_WRONLY | O_RDONLY | O_RDWR)) != O_RDWR)) {
		save_errno = errno;
#ifdef PSEUDO_NO_REAL_AT_FUNCTIONS
		if (flags & O_NOFOLLOW) {
			rc = real___lxstat64(_STAT_VER, path, &buf);
		} else {
			rc = real___xstat64(_STAT_VER, path, &buf);
		}
#else
		rc = real___fxstatat64(_STAT_VER, dirfd, path, &buf, (flags & O_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0);
#endif
		if (rc != -1 && S_ISFIFO(buf.st_mode)) {
			overly_magic_nonblocking = 1;
		}
	}

	/* this is a horrible special case and i do not know whether it will work */
	if (overly_magic_nonblocking) {
		pseudo_droplock();
		sigprocmask(SIG_SETMASK, &pseudo_saved_sigmask, &local_saved_sigmask);
	}
	/* because we are not actually root, secretly mask in 0600 to the
	 * underlying mode.  The ", 0" is because the only time mode matters
	 * is if a file is going to be created, in which case it's
	 * not a directory.
	 */
#ifdef PSEUDO_NO_REAL_AT_FUNCTIONS
	rc = real_open(path, flags, PSEUDO_FS_MODE(mode, 0));
#else
	rc = real_openat(dirfd, path, flags, PSEUDO_FS_MODE(mode, 0));
#endif
	if (overly_magic_nonblocking) {
		save_errno = errno;
		sigprocmask(SIG_SETMASK, &local_saved_sigmask, NULL);
		/* well this is a problem. we can't NOT proceed; we may have
		 * already opened the file! we can't even return up the call
		 * stack to stuff that's going to try to drop the lock.
		 */
		if (pseudo_getlock()) {
			pseudo_diag("PANIC: after opening a readonly/writeonly FIFO (path '%s', fd %d, errno %d, saved errno %d), could not regain lock. unrecoverable. sorry. bye.\n",
				path, rc, errno, save_errno);
			abort();
		}
		errno = save_errno;
	}

	if (rc != -1) {
		save_errno = errno;
		int stat_rc;
#ifdef O_TMPFILE
		/* in O_TMPFILE case, nothing gets put in the
		 * database, because there's no directory entries for
		 * the file yet.
		 */
		if ((flags & O_TMPFILE) == O_TMPFILE) {
			real_fchmod(rc, PSEUDO_FS_MODE(mode, 0));
			errno = save_errno;
			return rc;
		}
#endif
#ifdef PSEUDO_NO_REAL_AT_FUNCTIONS
		if (flags & O_NOFOLLOW) {
			stat_rc = real___lxstat64(_STAT_VER, path, &buf);
		} else {
			stat_rc = real___xstat64(_STAT_VER, path, &buf);
		}
#else
		stat_rc = real___fxstatat64(_STAT_VER, dirfd, path, &buf, (flags & O_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0);
#endif

		pseudo_debug(PDBGF_FILE, "openat(path %s), flags %o, stat rc %d, stat mode %o\n",
			path, flags, stat_rc, buf.st_mode);
		if (stat_rc != -1) {
			buf.st_mode = PSEUDO_DB_MODE(buf.st_mode, mode);
			if (!existed) {
				real_fchmod(rc, PSEUDO_FS_MODE(mode, 0));
				// file has no path, but has been created
				pseudo_client_op(OP_CREAT, 0, -1, dirfd, path, &buf);
			}
				pseudo_client_op(OP_OPEN, PSEUDO_ACCESS(flags), rc, dirfd, path, &buf);
		} else {
			pseudo_debug(PDBGF_FILE, "openat (fd %d, path %d/%s, flags %d) succeeded, but stat failed (%s).\n",
				rc, dirfd, path, flags, strerror(errno));
			pseudo_client_op(OP_OPEN, PSEUDO_ACCESS(flags), rc, dirfd, path, 0);
		}
		errno = save_errno;
	}

/*	return rc;
 * }
 */
