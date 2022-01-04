/*
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */
/* the unix port wants to know that real_stat() and
 * friends exist.  So they do. And because the Linux
 * port really uses stat64 for those...
 */
int
pseudo_stat(const char *path, struct stat *buf) {
	return real___xstat(_STAT_VER, path, buf);
}

int
pseudo_lstat(const char *path, struct stat *buf) {
	return real___lxstat(_STAT_VER, path, buf);
}

int
pseudo_fstat(int fd, struct stat *buf) {
	return real___fxstat(_STAT_VER, fd, buf);
}

int
pseudo_stat64(const char *path, struct stat64 *buf) {
	return real___xstat64(_STAT_VER, path, buf);
}

int
pseudo_lstat64(const char *path, struct stat64 *buf) {
	return real___lxstat64(_STAT_VER, path, buf);
}

int
pseudo_fstat64(int fd, struct stat64 *buf) {
	return real___fxstat64(_STAT_VER, fd, buf);
}

/* similar thing happens with mknod */
int
pseudo_mknod(const char *path, mode_t mode, dev_t dev) {
	return real___xmknod(_MKNOD_VER, path, mode, &dev);
}

int
pseudo_mknodat(int dirfd, const char *path, mode_t mode, dev_t dev) {
	return real___xmknodat(_MKNOD_VER, dirfd, path, mode, &dev);
}

int pseudo_capset(cap_user_header_t hdrp, const cap_user_data_t datap) {
	(void)hdrp;
	(void)datap;

	return 0;
}

long
syscall(long number, ...) {
	long rc = -1;

	if (!pseudo_check_wrappers() || !real_syscall) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("syscall");
		return rc;
	}

#ifdef SYS_renameat2
	/* concerns exist about trying to parse arguments because syscall(2)
	 * specifies strange ABI behaviors. If we can get better clarity on
	 * that, it could make sense to redirect to wrap_renameat2().
	 */
	if (number == SYS_renameat2) {
		errno = ENOSYS;
		return -1;
	}
#else
	(void) number;
#endif

	/* gcc magic to attempt to just pass these args to syscall. we have to
	 * guess about the number of args; the docs discuss calling conventions
	 * up to 7, so let's try that?
	 */
	void *res = __builtin_apply((void (*)()) real_syscall, __builtin_apply_args(), sizeof(long) * 7);
	__builtin_return(res);
}

/* unused.
 */
static long wrap_syscall(long nr, va_list ap) {
	(void) nr;
	(void) ap;
	return -1;
}
