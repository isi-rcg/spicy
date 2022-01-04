/* wrapper functions. generated automatically. */

/* IF YOU ARE SEEING COMPILER ERRORS IN THIS FILE:
 * If you are seeing a whole lot of errors, make sure you aren't actually
 * trying to compile pseudo_wrapfuncs.c directly.  This file is #included
 * from pseudo_wrappers.c, which has a lot of needed include files and
 * static declarations.
 */

/* This file is generated and should not be modified.  See the makewrappers
 * script if you want to modify this. */

static int (*real___fxstat)(int ver, int fd, struct stat *buf) = NULL;



int
__fxstat(int ver, int fd, struct stat *buf) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real___fxstat) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("__fxstat");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real___fxstat)(ver, fd, buf);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: __fxstat\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "__fxstat - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "__fxstat failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "__fxstat calling real syscall.\n");
		rc = (*real___fxstat)(ver, fd, buf);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap___fxstat(ver, fd, buf);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "__fxstat - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: __fxstat returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: __fxstat returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap___fxstat(int ver, int fd, struct stat *buf) {
	int rc = -1;
	
	

#include "ports/linux/guts/__fxstat.c"

	return rc;
}



static int (*real___fxstat64)(int ver, int fd, struct stat64 *buf) = NULL;



int
__fxstat64(int ver, int fd, struct stat64 *buf) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real___fxstat64) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("__fxstat64");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real___fxstat64)(ver, fd, buf);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: __fxstat64\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "__fxstat64 - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "__fxstat64 failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "__fxstat64 calling real syscall.\n");
		rc = (*real___fxstat64)(ver, fd, buf);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap___fxstat64(ver, fd, buf);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "__fxstat64 - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: __fxstat64 returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: __fxstat64 returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap___fxstat64(int ver, int fd, struct stat64 *buf) {
	int rc = -1;
	
	

#include "ports/linux/guts/__fxstat64.c"

	return rc;
}



static int (*real___fxstatat)(int ver, int dirfd, const char *path, struct stat *buf, int flags) = NULL;



int
__fxstatat(int ver, int dirfd, const char *path, struct stat *buf, int flags) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real___fxstatat) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("__fxstatat");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real___fxstatat)(ver, dirfd, path, buf, flags);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: __fxstatat\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "__fxstatat - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "__fxstatat failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "__fxstatat calling real syscall.\n");
		rc = (*real___fxstatat)(ver, dirfd, path, buf, flags);
	} else {
		path = pseudo_root_path(__func__, __LINE__, dirfd, path, (flags & AT_SYMLINK_NOFOLLOW));
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap___fxstatat(ver, dirfd, path, buf, flags);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "__fxstatat - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: __fxstatat returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: __fxstatat returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap___fxstatat(int ver, int dirfd, const char *path, struct stat *buf, int flags) {
	int rc = -1;
	
	

#include "ports/linux/guts/__fxstatat.c"

	return rc;
}



static int (*real___fxstatat64)(int ver, int dirfd, const char *path, struct stat64 *buf, int flags) = NULL;



int
__fxstatat64(int ver, int dirfd, const char *path, struct stat64 *buf, int flags) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real___fxstatat64) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("__fxstatat64");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real___fxstatat64)(ver, dirfd, path, buf, flags);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: __fxstatat64\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "__fxstatat64 - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "__fxstatat64 failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "__fxstatat64 calling real syscall.\n");
		rc = (*real___fxstatat64)(ver, dirfd, path, buf, flags);
	} else {
		path = pseudo_root_path(__func__, __LINE__, dirfd, path, (flags & AT_SYMLINK_NOFOLLOW));
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap___fxstatat64(ver, dirfd, path, buf, flags);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "__fxstatat64 - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: __fxstatat64 returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: __fxstatat64 returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap___fxstatat64(int ver, int dirfd, const char *path, struct stat64 *buf, int flags) {
	int rc = -1;
	
	

#include "ports/linux/guts/__fxstatat64.c"

	return rc;
}



static int (*real___lxstat)(int ver, const char *path, struct stat *buf) = NULL;



int
__lxstat(int ver, const char *path, struct stat *buf) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real___lxstat) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("__lxstat");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real___lxstat)(ver, path, buf);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: __lxstat\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "__lxstat - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "__lxstat failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "__lxstat calling real syscall.\n");
		rc = (*real___lxstat)(ver, path, buf);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, AT_SYMLINK_NOFOLLOW);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap___lxstat(ver, path, buf);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "__lxstat - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: __lxstat returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: __lxstat returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap___lxstat(int ver, const char *path, struct stat *buf) {
	int rc = -1;
	
	

#include "ports/linux/guts/__lxstat.c"

	return rc;
}



static int (*real___lxstat64)(int ver, const char *path, struct stat64 *buf) = NULL;



int
__lxstat64(int ver, const char *path, struct stat64 *buf) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real___lxstat64) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("__lxstat64");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real___lxstat64)(ver, path, buf);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: __lxstat64\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "__lxstat64 - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "__lxstat64 failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "__lxstat64 calling real syscall.\n");
		rc = (*real___lxstat64)(ver, path, buf);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, AT_SYMLINK_NOFOLLOW);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap___lxstat64(ver, path, buf);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "__lxstat64 - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: __lxstat64 returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: __lxstat64 returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap___lxstat64(int ver, const char *path, struct stat64 *buf) {
	int rc = -1;
	
	

#include "ports/linux/guts/__lxstat64.c"

	return rc;
}



static int (*real___openat64_2)(int dirfd, const char *path, int flags) = NULL;



int
__openat64_2(int dirfd, const char *path, int flags) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real___openat64_2) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("__openat64_2");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real___openat64_2)(dirfd, path, flags);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: __openat64_2\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "__openat64_2 - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "__openat64_2 failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "__openat64_2 calling real syscall.\n");
		rc = (*real___openat64_2)(dirfd, path, flags);
	} else {
		path = pseudo_root_path(__func__, __LINE__, dirfd, path, flags&O_NOFOLLOW);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap___openat64_2(dirfd, path, flags);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "__openat64_2 - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: __openat64_2 returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: __openat64_2 returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap___openat64_2(int dirfd, const char *path, int flags) {
	int rc = -1;
	
	

#include "ports/linux/guts/__openat64_2.c"

	return rc;
}



static int (*real___openat_2)(int dirfd, const char *path, int flags) = NULL;



int
__openat_2(int dirfd, const char *path, int flags) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real___openat_2) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("__openat_2");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real___openat_2)(dirfd, path, flags);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: __openat_2\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "__openat_2 - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "__openat_2 failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "__openat_2 calling real syscall.\n");
		rc = (*real___openat_2)(dirfd, path, flags);
	} else {
		path = pseudo_root_path(__func__, __LINE__, dirfd, path, flags&O_NOFOLLOW);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap___openat_2(dirfd, path, flags);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "__openat_2 - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: __openat_2 returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: __openat_2 returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap___openat_2(int dirfd, const char *path, int flags) {
	int rc = -1;
	
	

#include "ports/linux/guts/__openat_2.c"

	return rc;
}



static int (*real___xmknod)(int ver, const char *path, mode_t mode, dev_t *dev) = NULL;



int
__xmknod(int ver, const char *path, mode_t mode, dev_t *dev) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real___xmknod) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("__xmknod");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real___xmknod)(ver, path, mode, dev);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: __xmknod\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "__xmknod - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "__xmknod failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "__xmknod calling real syscall.\n");
		rc = (*real___xmknod)(ver, path, mode, dev);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, AT_SYMLINK_NOFOLLOW);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap___xmknod(ver, path, mode, dev);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "__xmknod - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: __xmknod returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: __xmknod returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap___xmknod(int ver, const char *path, mode_t mode, dev_t *dev) {
	int rc = -1;
	
	

#include "ports/linux/guts/__xmknod.c"

	return rc;
}



static int (*real___xmknodat)(int ver, int dirfd, const char *path, mode_t mode, dev_t *dev) = NULL;



int
__xmknodat(int ver, int dirfd, const char *path, mode_t mode, dev_t *dev) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real___xmknodat) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("__xmknodat");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real___xmknodat)(ver, dirfd, path, mode, dev);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: __xmknodat\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "__xmknodat - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "__xmknodat failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "__xmknodat calling real syscall.\n");
		rc = (*real___xmknodat)(ver, dirfd, path, mode, dev);
	} else {
		path = pseudo_root_path(__func__, __LINE__, dirfd, path, AT_SYMLINK_NOFOLLOW);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap___xmknodat(ver, dirfd, path, mode, dev);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "__xmknodat - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: __xmknodat returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: __xmknodat returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap___xmknodat(int ver, int dirfd, const char *path, mode_t mode, dev_t *dev) {
	int rc = -1;
	
	

#include "ports/linux/guts/__xmknodat.c"

	return rc;
}



static int (*real___xstat)(int ver, const char *path, struct stat *buf) = NULL;



int
__xstat(int ver, const char *path, struct stat *buf) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real___xstat) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("__xstat");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real___xstat)(ver, path, buf);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: __xstat\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "__xstat - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "__xstat failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "__xstat calling real syscall.\n");
		rc = (*real___xstat)(ver, path, buf);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, 0);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap___xstat(ver, path, buf);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "__xstat - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: __xstat returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: __xstat returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap___xstat(int ver, const char *path, struct stat *buf) {
	int rc = -1;
	
	

#include "ports/linux/guts/__xstat.c"

	return rc;
}



static int (*real___xstat64)(int ver, const char *path, struct stat64 *buf) = NULL;



int
__xstat64(int ver, const char *path, struct stat64 *buf) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real___xstat64) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("__xstat64");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real___xstat64)(ver, path, buf);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: __xstat64\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "__xstat64 - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "__xstat64 failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "__xstat64 calling real syscall.\n");
		rc = (*real___xstat64)(ver, path, buf);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, 0);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap___xstat64(ver, path, buf);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "__xstat64 - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: __xstat64 returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: __xstat64 returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap___xstat64(int ver, const char *path, struct stat64 *buf) {
	int rc = -1;
	
	

#include "ports/linux/guts/__xstat64.c"

	return rc;
}



static int (*real_access)(const char *path, int mode) = NULL;



int
access(const char *path, int mode) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_access) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("access");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_access)(path, mode);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: access\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "access - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "access failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "access calling real syscall.\n");
		rc = (*real_access)(path, mode);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, 0);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_access(path, mode);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "access - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: access returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: access returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_access(const char *path, int mode) {
	int rc = -1;
	
	

#include "ports/unix/guts/access.c"

	return rc;
}



static int (*real_acct)(const char *path) = NULL;



int
acct(const char *path) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_acct) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("acct");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_acct)(path);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: acct\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "acct - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "acct failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "acct calling real syscall.\n");
		rc = (*real_acct)(path);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, 0);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_acct(path);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "acct - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: acct returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: acct returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_acct(const char *path) {
	int rc = -1;
	
	

#include "ports/unix/guts/acct.c"

	return rc;
}



static int (*real_bind)(int sockfd, const struct sockaddr *addr, socklen_t addrlen) = NULL;



int
bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_bind) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("bind");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_bind)(sockfd, addr, addrlen);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: bind\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "bind - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "bind failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "bind calling real syscall.\n");
		rc = (*real_bind)(sockfd, addr, addrlen);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_bind(sockfd, addr, addrlen);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "bind - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: bind returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: bind returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
	int rc = -1;
	
	

#include "ports/unix/guts/bind.c"

	return rc;
}



static char * (*real_canonicalize_file_name)(const char *filename) = NULL;



char *
canonicalize_file_name(const char *filename) {
	sigset_t saved;
	
	char *rc = NULL;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_canonicalize_file_name) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("canonicalize_file_name");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_canonicalize_file_name)(filename);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: canonicalize_file_name\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "canonicalize_file_name - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "canonicalize_file_name failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return NULL;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "canonicalize_file_name calling real syscall.\n");
		rc = (*real_canonicalize_file_name)(filename);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_canonicalize_file_name(filename);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "canonicalize_file_name - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: canonicalize_file_name returns %s (errno: %s)\n", rc ? rc : "<nil>", strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: canonicalize_file_name returns %s (errno: %d)\n", rc ? rc : "<nil>", save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static char *
wrap_canonicalize_file_name(const char *filename) {
	char *rc = NULL;
	
	

#include "ports/linux/guts/canonicalize_file_name.c"

	return rc;
}



static int (*real_capset)(cap_user_header_t hdrp, const cap_user_data_t datap) = pseudo_capset;



int
capset(cap_user_header_t hdrp, const cap_user_data_t datap) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_capset) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("capset");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_capset)(hdrp, datap);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: capset\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "capset - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "capset failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "capset calling real syscall.\n");
		rc = (*real_capset)(hdrp, datap);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_capset(hdrp, datap);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "capset - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: capset returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: capset returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_capset(cap_user_header_t hdrp, const cap_user_data_t datap) {
	int rc = -1;
	
	

#include "ports/linux/guts/capset.c"

	return rc;
}



static int (*real_chdir)(const char *path) = NULL;



int
chdir(const char *path) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_chdir) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("chdir");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_chdir)(path);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: chdir\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "chdir - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "chdir failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "chdir calling real syscall.\n");
		rc = (*real_chdir)(path);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, 0);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_chdir(path);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "chdir - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: chdir returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: chdir returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_chdir(const char *path) {
	int rc = -1;
	
	

#include "ports/unix/guts/chdir.c"

	return rc;
}



static int (*real_chmod)(const char *path, mode_t mode) = NULL;



int
chmod(const char *path, mode_t mode) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_chmod) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("chmod");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_chmod)(path, mode);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: chmod\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "chmod - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "chmod failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "chmod calling real syscall.\n");
		rc = (*real_chmod)(path, mode);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, 0);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_chmod(path, mode);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "chmod - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: chmod returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: chmod returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_chmod(const char *path, mode_t mode) {
	int rc = -1;
	
	

#include "ports/unix/guts/chmod.c"

	return rc;
}



static int (*real_chown)(const char *path, uid_t owner, gid_t group) = NULL;



int
chown(const char *path, uid_t owner, gid_t group) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_chown) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("chown");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_chown)(path, owner, group);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: chown\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "chown - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "chown failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "chown calling real syscall.\n");
		rc = (*real_chown)(path, owner, group);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, 0);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_chown(path, owner, group);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "chown - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: chown returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: chown returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_chown(const char *path, uid_t owner, gid_t group) {
	int rc = -1;
	
	

#include "ports/unix/guts/chown.c"

	return rc;
}



static int (*real_chroot)(const char *path) = NULL;



int
chroot(const char *path) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_chroot) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("chroot");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_chroot)(path);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: chroot\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "chroot - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "chroot failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "chroot calling real syscall.\n");
		rc = (*real_chroot)(path);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, 0);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_chroot(path);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "chroot - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: chroot returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: chroot returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_chroot(const char *path) {
	int rc = -1;
	
	

#include "ports/unix/guts/chroot.c"

	return rc;
}



static int (*real_clone)(int (*fn)(void *), void *child_stack, int flags, void *arg, ...) = NULL;

/* Hand-written wrapper for this function. */
#if 0


int
clone(int (*fn)(void *), void *child_stack, int flags, void *arg, ...) {
	sigset_t saved;
	va_list ap;

	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_clone) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("clone");
		PROFILE_DONE;
		return rc;
	}

	va_start(ap, arg);


	if (pseudo_disabled) {
		rc = (*real_clone)(fn, child_stack, flags, arg, ap);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: clone\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "clone - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "clone failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "clone calling real syscall.\n");
		rc = (*real_clone)(fn, child_stack, flags, arg, ap);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_clone(fn, child_stack, flags, arg, ap);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "clone - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: clone returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: clone returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_clone(int (*fn)(void *), void *child_stack, int flags, void *arg, va_list ap) {
	int rc = -1;
	
	

#include "ports/linux/newclone/guts/clone.c"

	return rc;
}

/* Hand-written wrapper for this function. */
#endif


static int (*real_close)(int fd) = NULL;



int
close(int fd) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_close) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("close");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_close)(fd);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: close\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "close - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "close failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "close calling real syscall.\n");
		rc = (*real_close)(fd);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_close(fd);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "close - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: close returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: close returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_close(int fd) {
	int rc = -1;
	
	

#include "ports/unix/guts/close.c"

	return rc;
}



static int (*real_closedir)(DIR *dirp) = NULL;



int
closedir(DIR *dirp) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_closedir) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("closedir");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_closedir)(dirp);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: closedir\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "closedir - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "closedir failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "closedir calling real syscall.\n");
		rc = (*real_closedir)(dirp);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_closedir(dirp);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "closedir - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: closedir returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: closedir returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_closedir(DIR *dirp) {
	int rc = -1;
	
	

#include "ports/unix/guts/closedir.c"

	return rc;
}



static int (*real_creat)(const char *path, mode_t mode) = NULL;



int
creat(const char *path, mode_t mode) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_creat) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("creat");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_creat)(path, mode);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: creat\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "creat - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "creat failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "creat calling real syscall.\n");
		rc = (*real_creat)(path, mode);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, 0);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_creat(path, mode);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "creat - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: creat returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: creat returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_creat(const char *path, mode_t mode) {
	int rc = -1;
	
	

#include "ports/unix/guts/creat.c"

	return rc;
}



static int (*real_creat64)(const char *path, mode_t mode) = NULL;



int
creat64(const char *path, mode_t mode) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_creat64) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("creat64");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_creat64)(path, mode);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: creat64\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "creat64 - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "creat64 failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "creat64 calling real syscall.\n");
		rc = (*real_creat64)(path, mode);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, 0);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_creat64(path, mode);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "creat64 - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: creat64 returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: creat64 returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_creat64(const char *path, mode_t mode) {
	int rc = -1;
	
	

#include "ports/linux/guts/creat64.c"

	return rc;
}



static int (*real_dup)(int fd) = NULL;



int
dup(int fd) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_dup) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("dup");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_dup)(fd);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: dup\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "dup - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "dup failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "dup calling real syscall.\n");
		rc = (*real_dup)(fd);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_dup(fd);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "dup - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: dup returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: dup returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_dup(int fd) {
	int rc = -1;
	
	

#include "ports/unix/guts/dup.c"

	return rc;
}



static int (*real_dup2)(int oldfd, int newfd) = NULL;



int
dup2(int oldfd, int newfd) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_dup2) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("dup2");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_dup2)(oldfd, newfd);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: dup2\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "dup2 - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "dup2 failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "dup2 calling real syscall.\n");
		rc = (*real_dup2)(oldfd, newfd);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_dup2(oldfd, newfd);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "dup2 - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: dup2 returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: dup2 returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_dup2(int oldfd, int newfd) {
	int rc = -1;
	
	

#include "ports/unix/guts/dup2.c"

	return rc;
}



static int (*real_eaccess)(const char *path, int mode) = NULL;



int
eaccess(const char *path, int mode) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_eaccess) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("eaccess");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_eaccess)(path, mode);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: eaccess\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "eaccess - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "eaccess failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "eaccess calling real syscall.\n");
		rc = (*real_eaccess)(path, mode);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, 0);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_eaccess(path, mode);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "eaccess - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: eaccess returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: eaccess returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_eaccess(const char *path, int mode) {
	int rc = -1;
	
	

#include "ports/linux/guts/eaccess.c"

	return rc;
}



static void (*real_endgrent)(void) = NULL;



void
endgrent(void) {
	sigset_t saved;
	
	
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_endgrent) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("endgrent");
		PROFILE_DONE;
		return;
	}

	

	if (pseudo_disabled) {
		(void) (*real_endgrent)();
		
		PROFILE_DONE;
		return;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: endgrent\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "endgrent - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "endgrent failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "endgrent calling real syscall.\n");
		(void) (*real_endgrent)();
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		(void) wrap_endgrent();
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "endgrent - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: endgrent returns void%s (errno: %s)\n", "", strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: endgrent returns void%s (errno: %d)\n", "", save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return;
}

static void
wrap_endgrent(void) {
	
	
	

#include "ports/uids_generic/guts/endgrent.c"

	return;
}



static void (*real_endpwent)(void) = NULL;



void
endpwent(void) {
	sigset_t saved;
	
	
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_endpwent) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("endpwent");
		PROFILE_DONE;
		return;
	}

	

	if (pseudo_disabled) {
		(void) (*real_endpwent)();
		
		PROFILE_DONE;
		return;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: endpwent\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "endpwent - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "endpwent failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "endpwent calling real syscall.\n");
		(void) (*real_endpwent)();
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		(void) wrap_endpwent();
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "endpwent - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: endpwent returns void%s (errno: %s)\n", "", strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: endpwent returns void%s (errno: %d)\n", "", save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return;
}

static void
wrap_endpwent(void) {
	
	
	

#include "ports/uids_generic/guts/endpwent.c"

	return;
}



static int (*real_euidaccess)(const char *path, int mode) = NULL;



int
euidaccess(const char *path, int mode) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_euidaccess) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("euidaccess");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_euidaccess)(path, mode);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: euidaccess\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "euidaccess - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "euidaccess failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "euidaccess calling real syscall.\n");
		rc = (*real_euidaccess)(path, mode);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, 0);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_euidaccess(path, mode);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "euidaccess - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: euidaccess returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: euidaccess returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_euidaccess(const char *path, int mode) {
	int rc = -1;
	
	

#include "ports/linux/guts/euidaccess.c"

	return rc;
}



static int (*real_execl)(const char *file, const char *arg, ...) = NULL;

/* Hand-written wrapper for this function. */
#if 0


int
execl(const char *file, const char *arg, ...) {
	sigset_t saved;
	va_list ap;

	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_execl) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("execl");
		PROFILE_DONE;
		return rc;
	}

	va_start(ap, arg);


	if (pseudo_disabled) {
		rc = (*real_execl)(file, arg, ap);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: execl\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "execl - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "execl failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "execl calling real syscall.\n");
		rc = (*real_execl)(file, arg, ap);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_execl(file, arg, ap);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "execl - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: execl returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: execl returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_execl(const char *file, const char *arg, va_list ap) {
	int rc = -1;
	
	

#include "ports/common/guts/execl.c"

	return rc;
}

/* Hand-written wrapper for this function. */
#endif


static int (*real_execle)(const char *file, const char *arg, ...) = NULL;

/* Hand-written wrapper for this function. */
#if 0


int
execle(const char *file, const char *arg, ...) {
	sigset_t saved;
	va_list ap;

	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_execle) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("execle");
		PROFILE_DONE;
		return rc;
	}

	va_start(ap, arg);


	if (pseudo_disabled) {
		rc = (*real_execle)(file, arg, ap);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: execle\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "execle - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "execle failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "execle calling real syscall.\n");
		rc = (*real_execle)(file, arg, ap);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_execle(file, arg, ap);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "execle - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: execle returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: execle returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_execle(const char *file, const char *arg, va_list ap) {
	int rc = -1;
	
	

#include "ports/common/guts/execle.c"

	return rc;
}

/* Hand-written wrapper for this function. */
#endif


static int (*real_execlp)(const char *file, const char *arg, ...) = NULL;

/* Hand-written wrapper for this function. */
#if 0


int
execlp(const char *file, const char *arg, ...) {
	sigset_t saved;
	va_list ap;

	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_execlp) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("execlp");
		PROFILE_DONE;
		return rc;
	}

	va_start(ap, arg);


	if (pseudo_disabled) {
		rc = (*real_execlp)(file, arg, ap);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: execlp\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "execlp - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "execlp failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "execlp calling real syscall.\n");
		rc = (*real_execlp)(file, arg, ap);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_execlp(file, arg, ap);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "execlp - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: execlp returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: execlp returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_execlp(const char *file, const char *arg, va_list ap) {
	int rc = -1;
	
	

#include "ports/common/guts/execlp.c"

	return rc;
}

/* Hand-written wrapper for this function. */
#endif


static int (*real_execv)(const char *file, char *const *argv) = NULL;

/* Hand-written wrapper for this function. */
#if 0


int
execv(const char *file, char *const *argv) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_execv) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("execv");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_execv)(file, argv);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: execv\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "execv - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "execv failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "execv calling real syscall.\n");
		rc = (*real_execv)(file, argv);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_execv(file, argv);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "execv - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: execv returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: execv returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_execv(const char *file, char *const *argv) {
	int rc = -1;
	
	

#include "ports/common/guts/execv.c"

	return rc;
}

/* Hand-written wrapper for this function. */
#endif


static int (*real_execve)(const char *file, char *const *argv, char *const *envp) = NULL;

/* Hand-written wrapper for this function. */
#if 0


int
execve(const char *file, char *const *argv, char *const *envp) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_execve) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("execve");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_execve)(file, argv, envp);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: execve\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "execve - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "execve failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "execve calling real syscall.\n");
		rc = (*real_execve)(file, argv, envp);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_execve(file, argv, envp);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "execve - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: execve returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: execve returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_execve(const char *file, char *const *argv, char *const *envp) {
	int rc = -1;
	
	

#include "ports/common/guts/execve.c"

	return rc;
}

/* Hand-written wrapper for this function. */
#endif


static int (*real_execvp)(const char *file, char *const *argv) = NULL;

/* Hand-written wrapper for this function. */
#if 0


int
execvp(const char *file, char *const *argv) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_execvp) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("execvp");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_execvp)(file, argv);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: execvp\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "execvp - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "execvp failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "execvp calling real syscall.\n");
		rc = (*real_execvp)(file, argv);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_execvp(file, argv);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "execvp - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: execvp returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: execvp returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_execvp(const char *file, char *const *argv) {
	int rc = -1;
	
	

#include "ports/common/guts/execvp.c"

	return rc;
}

/* Hand-written wrapper for this function. */
#endif


static int (*real_fchdir)(int dirfd) = NULL;



int
fchdir(int dirfd) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_fchdir) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("fchdir");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_fchdir)(dirfd);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: fchdir\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "fchdir - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "fchdir failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "fchdir calling real syscall.\n");
		rc = (*real_fchdir)(dirfd);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_fchdir(dirfd);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "fchdir - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: fchdir returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: fchdir returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_fchdir(int dirfd) {
	int rc = -1;
	
	

#include "ports/unix/guts/fchdir.c"

	return rc;
}



static int (*real_fchmod)(int fd, mode_t mode) = NULL;



int
fchmod(int fd, mode_t mode) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_fchmod) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("fchmod");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_fchmod)(fd, mode);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: fchmod\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "fchmod - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "fchmod failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "fchmod calling real syscall.\n");
		rc = (*real_fchmod)(fd, mode);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_fchmod(fd, mode);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "fchmod - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: fchmod returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: fchmod returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_fchmod(int fd, mode_t mode) {
	int rc = -1;
	
	

#include "ports/unix/guts/fchmod.c"

	return rc;
}



static int (*real_fchmodat)(int dirfd, const char *path, mode_t mode, int flags) = NULL;



int
fchmodat(int dirfd, const char *path, mode_t mode, int flags) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_fchmodat) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("fchmodat");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_fchmodat)(dirfd, path, mode, flags);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: fchmodat\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "fchmodat - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "fchmodat failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "fchmodat calling real syscall.\n");
		rc = (*real_fchmodat)(dirfd, path, mode, flags);
	} else {
		path = pseudo_root_path(__func__, __LINE__, dirfd, path, (flags & AT_SYMLINK_NOFOLLOW));
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_fchmodat(dirfd, path, mode, flags);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "fchmodat - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: fchmodat returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: fchmodat returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_fchmodat(int dirfd, const char *path, mode_t mode, int flags) {
	int rc = -1;
	
	

#include "ports/unix/guts/fchmodat.c"

	return rc;
}



static int (*real_fchown)(int fd, uid_t owner, gid_t group) = NULL;



int
fchown(int fd, uid_t owner, gid_t group) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_fchown) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("fchown");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_fchown)(fd, owner, group);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: fchown\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "fchown - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "fchown failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "fchown calling real syscall.\n");
		rc = (*real_fchown)(fd, owner, group);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_fchown(fd, owner, group);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "fchown - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: fchown returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: fchown returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_fchown(int fd, uid_t owner, gid_t group) {
	int rc = -1;
	
	

#include "ports/unix/guts/fchown.c"

	return rc;
}



static int (*real_fchownat)(int dirfd, const char *path, uid_t owner, gid_t group, int flags) = NULL;



int
fchownat(int dirfd, const char *path, uid_t owner, gid_t group, int flags) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_fchownat) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("fchownat");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_fchownat)(dirfd, path, owner, group, flags);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: fchownat\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "fchownat - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "fchownat failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "fchownat calling real syscall.\n");
		rc = (*real_fchownat)(dirfd, path, owner, group, flags);
	} else {
		path = pseudo_root_path(__func__, __LINE__, dirfd, path, (flags & AT_SYMLINK_NOFOLLOW));
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_fchownat(dirfd, path, owner, group, flags);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "fchownat - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: fchownat returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: fchownat returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_fchownat(int dirfd, const char *path, uid_t owner, gid_t group, int flags) {
	int rc = -1;
	
	

#include "ports/unix/guts/fchownat.c"

	return rc;
}



static int (*real_fclose)(FILE *fp) = NULL;



int
fclose(FILE *fp) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_fclose) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("fclose");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_fclose)(fp);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: fclose\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "fclose - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "fclose failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "fclose calling real syscall.\n");
		rc = (*real_fclose)(fp);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_fclose(fp);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "fclose - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: fclose returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: fclose returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_fclose(FILE *fp) {
	int rc = -1;
	
	

#include "ports/unix/guts/fclose.c"

	return rc;
}



static int (*real_fcntl)(int fd, int cmd, ... /* struct flock *lock */) = NULL;



int
fcntl(int fd, int cmd, ... /* struct flock *lock */) {
	sigset_t saved;
	va_list ap;
	struct flock *lock;

	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_fcntl) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("fcntl");
		PROFILE_DONE;
		return rc;
	}

	va_start(ap, cmd);
	lock = va_arg(ap, struct flock *);
	va_end(ap);


	if (pseudo_disabled) {
		rc = (*real_fcntl)(fd, cmd, lock);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: fcntl\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "fcntl - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "fcntl failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "fcntl calling real syscall.\n");
		rc = (*real_fcntl)(fd, cmd, lock);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_fcntl(fd, cmd, lock);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "fcntl - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: fcntl returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: fcntl returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_fcntl(int fd, int cmd, ... /* struct flock *lock */) {
	int rc = -1;
	va_list ap;
	struct flock *lock;

	va_start(ap, cmd);
	lock = va_arg(ap, struct flock *);
	va_end(ap);


#include "ports/linux/guts/fcntl.c"

	return rc;
}



static int (*real_fdatasync)(int fd) = NULL;



int
fdatasync(int fd) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;

/* This function is not called if pseudo is configured --enable-force-async */
#ifdef PSEUDO_FORCE_ASYNC
	if (!pseudo_allow_fsync) {
		PROFILE_DONE;
		return 0;
	}
#endif


	if (!pseudo_check_wrappers() || !real_fdatasync) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("fdatasync");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_fdatasync)(fd);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: fdatasync\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "fdatasync - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "fdatasync failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "fdatasync calling real syscall.\n");
		rc = (*real_fdatasync)(fd);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_fdatasync(fd);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "fdatasync - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: fdatasync returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: fdatasync returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_fdatasync(int fd) {
	int rc = -1;
	
	

#include "ports/unix/guts/fdatasync.c"

	return rc;
}



static ssize_t (*real_fgetxattr)(int filedes, const char *name, void *value, size_t size) = NULL;



ssize_t
fgetxattr(int filedes, const char *name, void *value, size_t size) {
	sigset_t saved;
	
	ssize_t rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_fgetxattr) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("fgetxattr");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_fgetxattr)(filedes, name, value, size);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: fgetxattr\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "fgetxattr - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "fgetxattr failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "fgetxattr calling real syscall.\n");
		rc = (*real_fgetxattr)(filedes, name, value, size);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_fgetxattr(filedes, name, value, size);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "fgetxattr - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: fgetxattr returns %ld (errno: %s)\n", (long) rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: fgetxattr returns %ld (errno: %d)\n", (long) rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static ssize_t
wrap_fgetxattr(int filedes, const char *name, void *value, size_t size) {
	ssize_t rc = -1;
	
	

#include "ports/linux/xattr/guts/fgetxattr.c"

	return rc;
}



static ssize_t (*real_flistxattr)(int filedes, char *list, size_t size) = NULL;



ssize_t
flistxattr(int filedes, char *list, size_t size) {
	sigset_t saved;
	
	ssize_t rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_flistxattr) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("flistxattr");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_flistxattr)(filedes, list, size);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: flistxattr\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "flistxattr - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "flistxattr failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "flistxattr calling real syscall.\n");
		rc = (*real_flistxattr)(filedes, list, size);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_flistxattr(filedes, list, size);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "flistxattr - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: flistxattr returns %ld (errno: %s)\n", (long) rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: flistxattr returns %ld (errno: %d)\n", (long) rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static ssize_t
wrap_flistxattr(int filedes, char *list, size_t size) {
	ssize_t rc = -1;
	
	

#include "ports/linux/xattr/guts/flistxattr.c"

	return rc;
}



static FILE * (*real_fopen)(const char *path, const char *mode) = NULL;



FILE *
fopen(const char *path, const char *mode) {
	sigset_t saved;
	
	FILE *rc = NULL;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_fopen) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("fopen");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_fopen)(path, mode);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: fopen\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "fopen - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "fopen failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return NULL;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "fopen calling real syscall.\n");
		rc = (*real_fopen)(path, mode);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, 0);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_fopen(path, mode);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "fopen - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: fopen returns %p (errno: %s)\n", (void *) rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: fopen returns %p (errno: %d)\n", (void *) rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static FILE *
wrap_fopen(const char *path, const char *mode) {
	FILE *rc = NULL;
	
	

#include "ports/unix/guts/fopen.c"

	return rc;
}



static FILE * (*real_fopen64)(const char *path, const char *mode) = NULL;



FILE *
fopen64(const char *path, const char *mode) {
	sigset_t saved;
	
	FILE *rc = NULL;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_fopen64) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("fopen64");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_fopen64)(path, mode);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: fopen64\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "fopen64 - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "fopen64 failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return NULL;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "fopen64 calling real syscall.\n");
		rc = (*real_fopen64)(path, mode);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, 0);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_fopen64(path, mode);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "fopen64 - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: fopen64 returns %p (errno: %s)\n", (void *) rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: fopen64 returns %p (errno: %d)\n", (void *) rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static FILE *
wrap_fopen64(const char *path, const char *mode) {
	FILE *rc = NULL;
	
	

#include "ports/linux/guts/fopen64.c"

	return rc;
}



static int (*real_fork)(void) = NULL;

/* Hand-written wrapper for this function. */
#if 0


int
fork(void) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_fork) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("fork");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_fork)();
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: fork\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "fork - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "fork failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "fork calling real syscall.\n");
		rc = (*real_fork)();
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_fork();
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "fork - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: fork returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: fork returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_fork(void) {
	int rc = -1;
	
	

#include "ports/common/guts/fork.c"

	return rc;
}

/* Hand-written wrapper for this function. */
#endif


static int (*real_fremovexattr)(int filedes, const char *name) = NULL;



int
fremovexattr(int filedes, const char *name) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_fremovexattr) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("fremovexattr");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_fremovexattr)(filedes, name);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: fremovexattr\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "fremovexattr - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "fremovexattr failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "fremovexattr calling real syscall.\n");
		rc = (*real_fremovexattr)(filedes, name);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_fremovexattr(filedes, name);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "fremovexattr - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: fremovexattr returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: fremovexattr returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_fremovexattr(int filedes, const char *name) {
	int rc = -1;
	
	

#include "ports/linux/xattr/guts/fremovexattr.c"

	return rc;
}



static FILE * (*real_freopen)(const char *path, const char *mode, FILE *stream) = NULL;



FILE *
freopen(const char *path, const char *mode, FILE *stream) {
	sigset_t saved;
	
	FILE *rc = NULL;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_freopen) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("freopen");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_freopen)(path, mode, stream);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: freopen\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "freopen - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "freopen failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return NULL;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "freopen calling real syscall.\n");
		rc = (*real_freopen)(path, mode, stream);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, 0);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_freopen(path, mode, stream);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "freopen - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: freopen returns %p (errno: %s)\n", (void *) rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: freopen returns %p (errno: %d)\n", (void *) rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static FILE *
wrap_freopen(const char *path, const char *mode, FILE *stream) {
	FILE *rc = NULL;
	
	

#include "ports/unix/guts/freopen.c"

	return rc;
}



static FILE * (*real_freopen64)(const char *path, const char *mode, FILE *stream) = NULL;



FILE *
freopen64(const char *path, const char *mode, FILE *stream) {
	sigset_t saved;
	
	FILE *rc = NULL;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_freopen64) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("freopen64");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_freopen64)(path, mode, stream);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: freopen64\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "freopen64 - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "freopen64 failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return NULL;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "freopen64 calling real syscall.\n");
		rc = (*real_freopen64)(path, mode, stream);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, 0);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_freopen64(path, mode, stream);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "freopen64 - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: freopen64 returns %p (errno: %s)\n", (void *) rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: freopen64 returns %p (errno: %d)\n", (void *) rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static FILE *
wrap_freopen64(const char *path, const char *mode, FILE *stream) {
	FILE *rc = NULL;
	
	

#include "ports/linux/guts/freopen64.c"

	return rc;
}



static int (*real_fsetxattr)(int filedes, const char *name, const void *value, size_t size, int xflags) = NULL;



int
fsetxattr(int filedes, const char *name, const void *value, size_t size, int xflags) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_fsetxattr) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("fsetxattr");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_fsetxattr)(filedes, name, value, size, xflags);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: fsetxattr\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "fsetxattr - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "fsetxattr failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "fsetxattr calling real syscall.\n");
		rc = (*real_fsetxattr)(filedes, name, value, size, xflags);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_fsetxattr(filedes, name, value, size, xflags);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "fsetxattr - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: fsetxattr returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: fsetxattr returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_fsetxattr(int filedes, const char *name, const void *value, size_t size, int xflags) {
	int rc = -1;
	
	

#include "ports/linux/xattr/guts/fsetxattr.c"

	return rc;
}



static int (*real_fstat)(int fd, struct stat *buf) = pseudo_fstat;



int
fstat(int fd, struct stat *buf) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_fstat) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("fstat");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_fstat)(fd, buf);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: fstat\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "fstat - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "fstat failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "fstat calling real syscall.\n");
		rc = (*real_fstat)(fd, buf);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_fstat(fd, buf);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "fstat - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: fstat returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: fstat returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_fstat(int fd, struct stat *buf) {
	int rc = -1;
	
	

#include "ports/linux/guts/fstat.c"

	return rc;
}



static int (*real_fstat64)(int fd, struct stat64 *buf) = pseudo_fstat64;



int
fstat64(int fd, struct stat64 *buf) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_fstat64) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("fstat64");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_fstat64)(fd, buf);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: fstat64\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "fstat64 - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "fstat64 failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "fstat64 calling real syscall.\n");
		rc = (*real_fstat64)(fd, buf);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_fstat64(fd, buf);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "fstat64 - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: fstat64 returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: fstat64 returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_fstat64(int fd, struct stat64 *buf) {
	int rc = -1;
	
	

#include "ports/linux/guts/fstat64.c"

	return rc;
}



static int (*real_fsync)(int fd) = NULL;



int
fsync(int fd) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;

/* This function is not called if pseudo is configured --enable-force-async */
#ifdef PSEUDO_FORCE_ASYNC
	if (!pseudo_allow_fsync) {
		PROFILE_DONE;
		return 0;
	}
#endif


	if (!pseudo_check_wrappers() || !real_fsync) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("fsync");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_fsync)(fd);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: fsync\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "fsync - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "fsync failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "fsync calling real syscall.\n");
		rc = (*real_fsync)(fd);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_fsync(fd);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "fsync - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: fsync returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: fsync returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_fsync(int fd) {
	int rc = -1;
	
	

#include "ports/unix/guts/fsync.c"

	return rc;
}



static FTS * (*real_fts_open)(char * const *path_argv, int options, int (*compar)(const FTSENT **, const FTSENT **)) = NULL;



FTS *
fts_open(char * const *path_argv, int options, int (*compar)(const FTSENT **, const FTSENT **)) {
	sigset_t saved;
	
	FTS *rc = NULL;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_fts_open) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("fts_open");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_fts_open)(path_argv, options, compar);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: fts_open\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "fts_open - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "fts_open failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return NULL;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "fts_open calling real syscall.\n");
		rc = (*real_fts_open)(path_argv, options, compar);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_fts_open(path_argv, options, compar);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "fts_open - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: fts_open returns %p (errno: %s)\n", (void *) rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: fts_open returns %p (errno: %d)\n", (void *) rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static FTS *
wrap_fts_open(char * const *path_argv, int options, int (*compar)(const FTSENT **, const FTSENT **)) {
	FTS *rc = NULL;
	
	

#include "ports/unix/guts/fts_open.c"

	return rc;
}



static int (*real_ftw)(const char *path, int (*fn)(const char *, const struct stat *, int), int nopenfd) = NULL;



int
ftw(const char *path, int (*fn)(const char *, const struct stat *, int), int nopenfd) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_ftw) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("ftw");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_ftw)(path, fn, nopenfd);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: ftw\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "ftw - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "ftw failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "ftw calling real syscall.\n");
		rc = (*real_ftw)(path, fn, nopenfd);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, 0);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_ftw(path, fn, nopenfd);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "ftw - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: ftw returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: ftw returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_ftw(const char *path, int (*fn)(const char *, const struct stat *, int), int nopenfd) {
	int rc = -1;
	
	

#include "ports/unix/guts/ftw.c"

	return rc;
}



static int (*real_ftw64)(const char *path, int (*fn)(const char *, const struct stat64 *, int), int nopenfd) = NULL;



int
ftw64(const char *path, int (*fn)(const char *, const struct stat64 *, int), int nopenfd) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_ftw64) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("ftw64");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_ftw64)(path, fn, nopenfd);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: ftw64\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "ftw64 - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "ftw64 failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "ftw64 calling real syscall.\n");
		rc = (*real_ftw64)(path, fn, nopenfd);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, 0);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_ftw64(path, fn, nopenfd);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "ftw64 - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: ftw64 returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: ftw64 returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_ftw64(const char *path, int (*fn)(const char *, const struct stat64 *, int), int nopenfd) {
	int rc = -1;
	
	

#include "ports/linux/guts/ftw64.c"

	return rc;
}



static char * (*real_get_current_dir_name)(void) = NULL;



char *
get_current_dir_name(void) {
	sigset_t saved;
	
	char *rc = NULL;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_get_current_dir_name) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("get_current_dir_name");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_get_current_dir_name)();
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: get_current_dir_name\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "get_current_dir_name - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "get_current_dir_name failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return NULL;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "get_current_dir_name calling real syscall.\n");
		rc = (*real_get_current_dir_name)();
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_get_current_dir_name();
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "get_current_dir_name - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: get_current_dir_name returns %s (errno: %s)\n", rc ? rc : "<nil>", strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: get_current_dir_name returns %s (errno: %d)\n", rc ? rc : "<nil>", save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static char *
wrap_get_current_dir_name(void) {
	char *rc = NULL;
	
	

#include "ports/linux/guts/get_current_dir_name.c"

	return rc;
}



static char * (*real_getcwd)(char *buf, size_t size) = NULL;



char *
getcwd(char *buf, size_t size) {
	sigset_t saved;
	
	char *rc = NULL;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_getcwd) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("getcwd");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_getcwd)(buf, size);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: getcwd\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getcwd - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "getcwd failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return NULL;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "getcwd calling real syscall.\n");
		rc = (*real_getcwd)(buf, size);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_getcwd(buf, size);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getcwd - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getcwd returns %s (errno: %s)\n", rc ? rc : "<nil>", strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getcwd returns %s (errno: %d)\n", rc ? rc : "<nil>", save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static char *
wrap_getcwd(char *buf, size_t size) {
	char *rc = NULL;
	
	

#include "ports/unix/guts/getcwd.c"

	return rc;
}



static gid_t (*real_getegid)(void) = NULL;



gid_t
getegid(void) {
	sigset_t saved;
	
	gid_t rc = 0;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_getegid) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("getegid");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_getegid)();
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: getegid\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getegid - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "getegid failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return 0;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "getegid calling real syscall.\n");
		rc = (*real_getegid)();
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_getegid();
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getegid - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getegid returns %ld (errno: %s)\n",  (long) rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getegid returns %ld (errno: %d)\n",  (long) rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static gid_t
wrap_getegid(void) {
	gid_t rc = 0;
	
	

#include "ports/uids_generic/guts/getegid.c"

	return rc;
}



static uid_t (*real_geteuid)(void) = NULL;



uid_t
geteuid(void) {
	sigset_t saved;
	
	uid_t rc = 0;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_geteuid) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("geteuid");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_geteuid)();
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: geteuid\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "geteuid - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "geteuid failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return 0;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "geteuid calling real syscall.\n");
		rc = (*real_geteuid)();
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_geteuid();
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "geteuid - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: geteuid returns %ld (errno: %s)\n",  (long) rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: geteuid returns %ld (errno: %d)\n",  (long) rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static uid_t
wrap_geteuid(void) {
	uid_t rc = 0;
	
	

#include "ports/uids_generic/guts/geteuid.c"

	return rc;
}



static gid_t (*real_getgid)(void) = NULL;



gid_t
getgid(void) {
	sigset_t saved;
	
	gid_t rc = 0;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_getgid) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("getgid");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_getgid)();
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: getgid\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getgid - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "getgid failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return 0;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "getgid calling real syscall.\n");
		rc = (*real_getgid)();
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_getgid();
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getgid - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getgid returns %ld (errno: %s)\n",  (long) rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getgid returns %ld (errno: %d)\n",  (long) rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static gid_t
wrap_getgid(void) {
	gid_t rc = 0;
	
	

#include "ports/uids_generic/guts/getgid.c"

	return rc;
}



static struct group * (*real_getgrent)(void) = NULL;



struct group *
getgrent(void) {
	sigset_t saved;
	
	struct group *rc = NULL;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_getgrent) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("getgrent");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_getgrent)();
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: getgrent\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getgrent - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "getgrent failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return NULL;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "getgrent calling real syscall.\n");
		rc = (*real_getgrent)();
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_getgrent();
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getgrent - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getgrent returns %p (errno: %s)\n", (void *) rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getgrent returns %p (errno: %d)\n", (void *) rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static struct group *
wrap_getgrent(void) {
	struct group *rc = NULL;
	
	

#include "ports/uids_generic/guts/getgrent.c"

	return rc;
}



static int (*real_getgrent_r)(struct group *gbuf, char *buf, size_t buflen, struct group **gbufp) = NULL;



int
getgrent_r(struct group *gbuf, char *buf, size_t buflen, struct group **gbufp) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_getgrent_r) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("getgrent_r");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_getgrent_r)(gbuf, buf, buflen, gbufp);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: getgrent_r\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getgrent_r - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "getgrent_r failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "getgrent_r calling real syscall.\n");
		rc = (*real_getgrent_r)(gbuf, buf, buflen, gbufp);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_getgrent_r(gbuf, buf, buflen, gbufp);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getgrent_r - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getgrent_r returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getgrent_r returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_getgrent_r(struct group *gbuf, char *buf, size_t buflen, struct group **gbufp) {
	int rc = -1;
	
	

#include "ports/linux/guts/getgrent_r.c"

	return rc;
}



static struct group * (*real_getgrgid)(gid_t gid) = NULL;



struct group *
getgrgid(gid_t gid) {
	sigset_t saved;
	
	struct group *rc = NULL;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_getgrgid) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("getgrgid");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_getgrgid)(gid);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: getgrgid\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getgrgid - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "getgrgid failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return NULL;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "getgrgid calling real syscall.\n");
		rc = (*real_getgrgid)(gid);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_getgrgid(gid);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getgrgid - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getgrgid returns %p (errno: %s)\n", (void *) rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getgrgid returns %p (errno: %d)\n", (void *) rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static struct group *
wrap_getgrgid(gid_t gid) {
	struct group *rc = NULL;
	
	

#include "ports/uids_generic/guts/getgrgid.c"

	return rc;
}



static int (*real_getgrgid_r)(gid_t gid, struct group *gbuf, char *buf, size_t buflen, struct group **gbufp) = NULL;



int
getgrgid_r(gid_t gid, struct group *gbuf, char *buf, size_t buflen, struct group **gbufp) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_getgrgid_r) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("getgrgid_r");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_getgrgid_r)(gid, gbuf, buf, buflen, gbufp);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: getgrgid_r\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getgrgid_r - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "getgrgid_r failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "getgrgid_r calling real syscall.\n");
		rc = (*real_getgrgid_r)(gid, gbuf, buf, buflen, gbufp);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_getgrgid_r(gid, gbuf, buf, buflen, gbufp);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getgrgid_r - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getgrgid_r returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getgrgid_r returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_getgrgid_r(gid_t gid, struct group *gbuf, char *buf, size_t buflen, struct group **gbufp) {
	int rc = -1;
	
	

#include "ports/uids_generic/guts/getgrgid_r.c"

	return rc;
}



static struct group * (*real_getgrnam)(const char *name) = NULL;



struct group *
getgrnam(const char *name) {
	sigset_t saved;
	
	struct group *rc = NULL;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_getgrnam) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("getgrnam");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_getgrnam)(name);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: getgrnam\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getgrnam - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "getgrnam failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return NULL;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "getgrnam calling real syscall.\n");
		rc = (*real_getgrnam)(name);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_getgrnam(name);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getgrnam - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getgrnam returns %p (errno: %s)\n", (void *) rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getgrnam returns %p (errno: %d)\n", (void *) rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static struct group *
wrap_getgrnam(const char *name) {
	struct group *rc = NULL;
	
	

#include "ports/uids_generic/guts/getgrnam.c"

	return rc;
}



static int (*real_getgrnam_r)(const char *name, struct group *gbuf, char *buf, size_t buflen, struct group **gbufp) = NULL;



int
getgrnam_r(const char *name, struct group *gbuf, char *buf, size_t buflen, struct group **gbufp) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_getgrnam_r) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("getgrnam_r");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_getgrnam_r)(name, gbuf, buf, buflen, gbufp);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: getgrnam_r\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getgrnam_r - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "getgrnam_r failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "getgrnam_r calling real syscall.\n");
		rc = (*real_getgrnam_r)(name, gbuf, buf, buflen, gbufp);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_getgrnam_r(name, gbuf, buf, buflen, gbufp);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getgrnam_r - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getgrnam_r returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getgrnam_r returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_getgrnam_r(const char *name, struct group *gbuf, char *buf, size_t buflen, struct group **gbufp) {
	int rc = -1;
	
	

#include "ports/uids_generic/guts/getgrnam_r.c"

	return rc;
}



static int (*real_getgrouplist)(const char *user, gid_t group, gid_t *groups, int *ngroups) = NULL;



int
getgrouplist(const char *user, gid_t group, gid_t *groups, int *ngroups) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_getgrouplist) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("getgrouplist");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_getgrouplist)(user, group, groups, ngroups);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: getgrouplist\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getgrouplist - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "getgrouplist failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "getgrouplist calling real syscall.\n");
		rc = (*real_getgrouplist)(user, group, groups, ngroups);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_getgrouplist(user, group, groups, ngroups);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getgrouplist - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getgrouplist returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getgrouplist returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_getgrouplist(const char *user, gid_t group, gid_t *groups, int *ngroups) {
	int rc = -1;
	
	

#include "ports/linux/guts/getgrouplist.c"

	return rc;
}



static int (*real_getgroups)(int size, gid_t *list) = NULL;



int
getgroups(int size, gid_t *list) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_getgroups) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("getgroups");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_getgroups)(size, list);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: getgroups\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getgroups - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "getgroups failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "getgroups calling real syscall.\n");
		rc = (*real_getgroups)(size, list);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_getgroups(size, list);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getgroups - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getgroups returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getgroups returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_getgroups(int size, gid_t *list) {
	int rc = -1;
	
	

#include "ports/linux/guts/getgroups.c"

	return rc;
}



static int (*real_getpw)(uid_t uid, char *buf) = NULL;



int
getpw(uid_t uid, char *buf) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_getpw) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("getpw");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_getpw)(uid, buf);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: getpw\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getpw - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "getpw failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "getpw calling real syscall.\n");
		rc = (*real_getpw)(uid, buf);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_getpw(uid, buf);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getpw - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getpw returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getpw returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_getpw(uid_t uid, char *buf) {
	int rc = -1;
	
	

#include "ports/linux/guts/getpw.c"

	return rc;
}



static struct passwd * (*real_getpwent)(void) = NULL;



struct passwd *
getpwent(void) {
	sigset_t saved;
	
	struct passwd *rc = NULL;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_getpwent) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("getpwent");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_getpwent)();
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: getpwent\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getpwent - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "getpwent failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return NULL;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "getpwent calling real syscall.\n");
		rc = (*real_getpwent)();
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_getpwent();
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getpwent - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getpwent returns %p (errno: %s)\n", (void *) rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getpwent returns %p (errno: %d)\n", (void *) rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static struct passwd *
wrap_getpwent(void) {
	struct passwd *rc = NULL;
	
	

#include "ports/uids_generic/guts/getpwent.c"

	return rc;
}



static int (*real_getpwent_r)(struct passwd *pwbuf, char *buf, size_t buflen, struct passwd **pwbufp) = NULL;



int
getpwent_r(struct passwd *pwbuf, char *buf, size_t buflen, struct passwd **pwbufp) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_getpwent_r) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("getpwent_r");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_getpwent_r)(pwbuf, buf, buflen, pwbufp);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: getpwent_r\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getpwent_r - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "getpwent_r failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "getpwent_r calling real syscall.\n");
		rc = (*real_getpwent_r)(pwbuf, buf, buflen, pwbufp);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_getpwent_r(pwbuf, buf, buflen, pwbufp);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getpwent_r - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getpwent_r returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getpwent_r returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_getpwent_r(struct passwd *pwbuf, char *buf, size_t buflen, struct passwd **pwbufp) {
	int rc = -1;
	
	

#include "ports/linux/guts/getpwent_r.c"

	return rc;
}



static struct passwd * (*real_getpwnam)(const char *name) = NULL;



struct passwd *
getpwnam(const char *name) {
	sigset_t saved;
	
	struct passwd *rc = NULL;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_getpwnam) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("getpwnam");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_getpwnam)(name);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: getpwnam\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getpwnam - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "getpwnam failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return NULL;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "getpwnam calling real syscall.\n");
		rc = (*real_getpwnam)(name);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_getpwnam(name);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getpwnam - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getpwnam returns %p (errno: %s)\n", (void *) rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getpwnam returns %p (errno: %d)\n", (void *) rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static struct passwd *
wrap_getpwnam(const char *name) {
	struct passwd *rc = NULL;
	
	

#include "ports/uids_generic/guts/getpwnam.c"

	return rc;
}



static int (*real_getpwnam_r)(const char *name, struct passwd *pwbuf, char *buf, size_t buflen, struct passwd **pwbufp) = NULL;



int
getpwnam_r(const char *name, struct passwd *pwbuf, char *buf, size_t buflen, struct passwd **pwbufp) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_getpwnam_r) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("getpwnam_r");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_getpwnam_r)(name, pwbuf, buf, buflen, pwbufp);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: getpwnam_r\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getpwnam_r - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "getpwnam_r failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "getpwnam_r calling real syscall.\n");
		rc = (*real_getpwnam_r)(name, pwbuf, buf, buflen, pwbufp);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_getpwnam_r(name, pwbuf, buf, buflen, pwbufp);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getpwnam_r - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getpwnam_r returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getpwnam_r returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_getpwnam_r(const char *name, struct passwd *pwbuf, char *buf, size_t buflen, struct passwd **pwbufp) {
	int rc = -1;
	
	

#include "ports/uids_generic/guts/getpwnam_r.c"

	return rc;
}



static struct passwd * (*real_getpwuid)(uid_t uid) = NULL;



struct passwd *
getpwuid(uid_t uid) {
	sigset_t saved;
	
	struct passwd *rc = NULL;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_getpwuid) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("getpwuid");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_getpwuid)(uid);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: getpwuid\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getpwuid - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "getpwuid failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return NULL;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "getpwuid calling real syscall.\n");
		rc = (*real_getpwuid)(uid);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_getpwuid(uid);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getpwuid - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getpwuid returns %p (errno: %s)\n", (void *) rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getpwuid returns %p (errno: %d)\n", (void *) rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static struct passwd *
wrap_getpwuid(uid_t uid) {
	struct passwd *rc = NULL;
	
	

#include "ports/uids_generic/guts/getpwuid.c"

	return rc;
}



static int (*real_getpwuid_r)(uid_t uid, struct passwd *pwbuf, char *buf, size_t buflen, struct passwd **pwbufp) = NULL;



int
getpwuid_r(uid_t uid, struct passwd *pwbuf, char *buf, size_t buflen, struct passwd **pwbufp) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_getpwuid_r) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("getpwuid_r");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_getpwuid_r)(uid, pwbuf, buf, buflen, pwbufp);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: getpwuid_r\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getpwuid_r - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "getpwuid_r failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "getpwuid_r calling real syscall.\n");
		rc = (*real_getpwuid_r)(uid, pwbuf, buf, buflen, pwbufp);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_getpwuid_r(uid, pwbuf, buf, buflen, pwbufp);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getpwuid_r - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getpwuid_r returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getpwuid_r returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_getpwuid_r(uid_t uid, struct passwd *pwbuf, char *buf, size_t buflen, struct passwd **pwbufp) {
	int rc = -1;
	
	

#include "ports/uids_generic/guts/getpwuid_r.c"

	return rc;
}



static int (*real_getresgid)(gid_t *rgid, gid_t *egid, gid_t *sgid) = NULL;



int
getresgid(gid_t *rgid, gid_t *egid, gid_t *sgid) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_getresgid) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("getresgid");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_getresgid)(rgid, egid, sgid);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: getresgid\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getresgid - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "getresgid failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "getresgid calling real syscall.\n");
		rc = (*real_getresgid)(rgid, egid, sgid);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_getresgid(rgid, egid, sgid);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getresgid - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getresgid returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getresgid returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_getresgid(gid_t *rgid, gid_t *egid, gid_t *sgid) {
	int rc = -1;
	
	

#include "ports/linux/guts/getresgid.c"

	return rc;
}



static int (*real_getresuid)(uid_t *ruid, uid_t *euid, uid_t *suid) = NULL;



int
getresuid(uid_t *ruid, uid_t *euid, uid_t *suid) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_getresuid) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("getresuid");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_getresuid)(ruid, euid, suid);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: getresuid\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getresuid - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "getresuid failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "getresuid calling real syscall.\n");
		rc = (*real_getresuid)(ruid, euid, suid);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_getresuid(ruid, euid, suid);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getresuid - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getresuid returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getresuid returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_getresuid(uid_t *ruid, uid_t *euid, uid_t *suid) {
	int rc = -1;
	
	

#include "ports/linux/guts/getresuid.c"

	return rc;
}



static uid_t (*real_getuid)(void) = NULL;



uid_t
getuid(void) {
	sigset_t saved;
	
	uid_t rc = 0;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_getuid) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("getuid");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_getuid)();
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: getuid\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getuid - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "getuid failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return 0;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "getuid calling real syscall.\n");
		rc = (*real_getuid)();
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_getuid();
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getuid - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getuid returns %ld (errno: %s)\n",  (long) rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getuid returns %ld (errno: %d)\n",  (long) rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static uid_t
wrap_getuid(void) {
	uid_t rc = 0;
	
	

#include "ports/uids_generic/guts/getuid.c"

	return rc;
}



static char * (*real_getwd)(char *buf) = NULL;



char *
getwd(char *buf) {
	sigset_t saved;
	
	char *rc = NULL;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_getwd) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("getwd");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_getwd)(buf);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: getwd\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getwd - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "getwd failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return NULL;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "getwd calling real syscall.\n");
		rc = (*real_getwd)(buf);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_getwd(buf);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getwd - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getwd returns %s (errno: %s)\n", rc ? rc : "<nil>", strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getwd returns %s (errno: %d)\n", rc ? rc : "<nil>", save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static char *
wrap_getwd(char *buf) {
	char *rc = NULL;
	
	

#include "ports/unix/guts/getwd.c"

	return rc;
}



static ssize_t (*real_getxattr)(const char *path, const char *name, void *value, size_t size) = NULL;



ssize_t
getxattr(const char *path, const char *name, void *value, size_t size) {
	sigset_t saved;
	
	ssize_t rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_getxattr) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("getxattr");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_getxattr)(path, name, value, size);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: getxattr\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getxattr - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "getxattr failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "getxattr calling real syscall.\n");
		rc = (*real_getxattr)(path, name, value, size);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, 0);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_getxattr(path, name, value, size);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "getxattr - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getxattr returns %ld (errno: %s)\n", (long) rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: getxattr returns %ld (errno: %d)\n", (long) rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static ssize_t
wrap_getxattr(const char *path, const char *name, void *value, size_t size) {
	ssize_t rc = -1;
	
	

#include "ports/linux/xattr/guts/getxattr.c"

	return rc;
}



static int (*real_glob)(const char *pattern, int flags, int (*errfunc)(const char *, int), glob_t *pglob) = NULL;



int
glob(const char *pattern, int flags, int (*errfunc)(const char *, int), glob_t *pglob) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_glob) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("glob");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_glob)(pattern, flags, errfunc, pglob);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: glob\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "glob - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "glob failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "glob calling real syscall.\n");
		rc = (*real_glob)(pattern, flags, errfunc, pglob);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_glob(pattern, flags, errfunc, pglob);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "glob - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: glob returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: glob returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_glob(const char *pattern, int flags, int (*errfunc)(const char *, int), glob_t *pglob) {
	int rc = -1;
	
	

#include "ports/unix/guts/glob.c"

	return rc;
}



static int (*real_glob64)(const char *pattern, int flags, int (*errfunc)(const char *, int), glob64_t *pglob) = NULL;



int
glob64(const char *pattern, int flags, int (*errfunc)(const char *, int), glob64_t *pglob) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_glob64) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("glob64");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_glob64)(pattern, flags, errfunc, pglob);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: glob64\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "glob64 - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "glob64 failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "glob64 calling real syscall.\n");
		rc = (*real_glob64)(pattern, flags, errfunc, pglob);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_glob64(pattern, flags, errfunc, pglob);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "glob64 - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: glob64 returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: glob64 returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_glob64(const char *pattern, int flags, int (*errfunc)(const char *, int), glob64_t *pglob) {
	int rc = -1;
	
	

#include "ports/linux/guts/glob64.c"

	return rc;
}



static int (*real_lchown)(const char *path, uid_t owner, gid_t group) = NULL;



int
lchown(const char *path, uid_t owner, gid_t group) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_lchown) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("lchown");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_lchown)(path, owner, group);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: lchown\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "lchown - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "lchown failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "lchown calling real syscall.\n");
		rc = (*real_lchown)(path, owner, group);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, AT_SYMLINK_NOFOLLOW);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_lchown(path, owner, group);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "lchown - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: lchown returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: lchown returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_lchown(const char *path, uid_t owner, gid_t group) {
	int rc = -1;
	
	

#include "ports/linux/guts/lchown.c"

	return rc;
}



static int (*real_lckpwdf)(void) = NULL;



int
lckpwdf(void) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_lckpwdf) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("lckpwdf");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_lckpwdf)();
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: lckpwdf\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "lckpwdf - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "lckpwdf failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "lckpwdf calling real syscall.\n");
		rc = (*real_lckpwdf)();
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_lckpwdf();
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "lckpwdf - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: lckpwdf returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: lckpwdf returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_lckpwdf(void) {
	int rc = -1;
	
	

#include "ports/linux/guts/lckpwdf.c"

	return rc;
}



static ssize_t (*real_lgetxattr)(const char *path, const char *name, void *value, size_t size) = NULL;



ssize_t
lgetxattr(const char *path, const char *name, void *value, size_t size) {
	sigset_t saved;
	
	ssize_t rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_lgetxattr) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("lgetxattr");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_lgetxattr)(path, name, value, size);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: lgetxattr\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "lgetxattr - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "lgetxattr failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "lgetxattr calling real syscall.\n");
		rc = (*real_lgetxattr)(path, name, value, size);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, AT_SYMLINK_NOFOLLOW);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_lgetxattr(path, name, value, size);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "lgetxattr - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: lgetxattr returns %ld (errno: %s)\n", (long) rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: lgetxattr returns %ld (errno: %d)\n", (long) rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static ssize_t
wrap_lgetxattr(const char *path, const char *name, void *value, size_t size) {
	ssize_t rc = -1;
	
	

#include "ports/linux/xattr/guts/lgetxattr.c"

	return rc;
}



static int (*real_link)(const char *oldname, const char *newname) = NULL;



int
link(const char *oldname, const char *newname) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_link) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("link");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_link)(oldname, newname);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: link\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "link - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "link failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "link calling real syscall.\n");
		rc = (*real_link)(oldname, newname);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_link(oldname, newname);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "link - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: link returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: link returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_link(const char *oldname, const char *newname) {
	int rc = -1;
	
	

#include "ports/unix/guts/link.c"

	return rc;
}



static int (*real_linkat)(int olddirfd, const char *oldname, int newdirfd, const char *newname, int flags) = NULL;



int
linkat(int olddirfd, const char *oldname, int newdirfd, const char *newname, int flags) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_linkat) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("linkat");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_linkat)(olddirfd, oldname, newdirfd, newname, flags);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: linkat\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "linkat - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "linkat failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "linkat calling real syscall.\n");
		rc = (*real_linkat)(olddirfd, oldname, newdirfd, newname, flags);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_linkat(olddirfd, oldname, newdirfd, newname, flags);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "linkat - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: linkat returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: linkat returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_linkat(int olddirfd, const char *oldname, int newdirfd, const char *newname, int flags) {
	int rc = -1;
	
	

#include "ports/unix/guts/linkat.c"

	return rc;
}



static ssize_t (*real_listxattr)(const char *path, char *list, size_t size) = NULL;



ssize_t
listxattr(const char *path, char *list, size_t size) {
	sigset_t saved;
	
	ssize_t rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_listxattr) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("listxattr");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_listxattr)(path, list, size);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: listxattr\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "listxattr - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "listxattr failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "listxattr calling real syscall.\n");
		rc = (*real_listxattr)(path, list, size);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, 0);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_listxattr(path, list, size);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "listxattr - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: listxattr returns %ld (errno: %s)\n", (long) rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: listxattr returns %ld (errno: %d)\n", (long) rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static ssize_t
wrap_listxattr(const char *path, char *list, size_t size) {
	ssize_t rc = -1;
	
	

#include "ports/linux/xattr/guts/listxattr.c"

	return rc;
}



static ssize_t (*real_llistxattr)(const char *path, char *list, size_t size) = NULL;



ssize_t
llistxattr(const char *path, char *list, size_t size) {
	sigset_t saved;
	
	ssize_t rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_llistxattr) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("llistxattr");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_llistxattr)(path, list, size);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: llistxattr\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "llistxattr - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "llistxattr failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "llistxattr calling real syscall.\n");
		rc = (*real_llistxattr)(path, list, size);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, AT_SYMLINK_NOFOLLOW);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_llistxattr(path, list, size);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "llistxattr - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: llistxattr returns %ld (errno: %s)\n", (long) rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: llistxattr returns %ld (errno: %d)\n", (long) rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static ssize_t
wrap_llistxattr(const char *path, char *list, size_t size) {
	ssize_t rc = -1;
	
	

#include "ports/linux/xattr/guts/llistxattr.c"

	return rc;
}



static int (*real_lremovexattr)(const char *path, const char *name) = NULL;



int
lremovexattr(const char *path, const char *name) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_lremovexattr) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("lremovexattr");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_lremovexattr)(path, name);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: lremovexattr\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "lremovexattr - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "lremovexattr failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "lremovexattr calling real syscall.\n");
		rc = (*real_lremovexattr)(path, name);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, AT_SYMLINK_NOFOLLOW);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_lremovexattr(path, name);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "lremovexattr - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: lremovexattr returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: lremovexattr returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_lremovexattr(const char *path, const char *name) {
	int rc = -1;
	
	

#include "ports/linux/xattr/guts/lremovexattr.c"

	return rc;
}



static int (*real_lsetxattr)(const char *path, const char *name, const void *value, size_t size, int xflags) = NULL;



int
lsetxattr(const char *path, const char *name, const void *value, size_t size, int xflags) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_lsetxattr) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("lsetxattr");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_lsetxattr)(path, name, value, size, xflags);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: lsetxattr\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "lsetxattr - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "lsetxattr failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "lsetxattr calling real syscall.\n");
		rc = (*real_lsetxattr)(path, name, value, size, xflags);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, AT_SYMLINK_NOFOLLOW);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_lsetxattr(path, name, value, size, xflags);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "lsetxattr - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: lsetxattr returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: lsetxattr returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_lsetxattr(const char *path, const char *name, const void *value, size_t size, int xflags) {
	int rc = -1;
	
	

#include "ports/linux/xattr/guts/lsetxattr.c"

	return rc;
}



static int (*real_lstat)(const char *path, struct stat *buf) = pseudo_lstat;



int
lstat(const char *path, struct stat *buf) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_lstat) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("lstat");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_lstat)(path, buf);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: lstat\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "lstat - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "lstat failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "lstat calling real syscall.\n");
		rc = (*real_lstat)(path, buf);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, AT_SYMLINK_NOFOLLOW);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_lstat(path, buf);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "lstat - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: lstat returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: lstat returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_lstat(const char *path, struct stat *buf) {
	int rc = -1;
	
	

#include "ports/linux/guts/lstat.c"

	return rc;
}



static int (*real_lstat64)(const char *path, struct stat64 *buf) = pseudo_lstat64;



int
lstat64(const char *path, struct stat64 *buf) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_lstat64) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("lstat64");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_lstat64)(path, buf);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: lstat64\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "lstat64 - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "lstat64 failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "lstat64 calling real syscall.\n");
		rc = (*real_lstat64)(path, buf);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, AT_SYMLINK_NOFOLLOW);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_lstat64(path, buf);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "lstat64 - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: lstat64 returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: lstat64 returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_lstat64(const char *path, struct stat64 *buf) {
	int rc = -1;
	
	

#include "ports/linux/guts/lstat64.c"

	return rc;
}



static int (*real_lutimes)(const char *path, const struct timeval *tv) = NULL;



int
lutimes(const char *path, const struct timeval *tv) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_lutimes) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("lutimes");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_lutimes)(path, tv);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: lutimes\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "lutimes - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "lutimes failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "lutimes calling real syscall.\n");
		rc = (*real_lutimes)(path, tv);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, AT_SYMLINK_NOFOLLOW);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_lutimes(path, tv);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "lutimes - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: lutimes returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: lutimes returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_lutimes(const char *path, const struct timeval *tv) {
	int rc = -1;
	
	

#include "ports/unix/guts/lutimes.c"

	return rc;
}



static int (*real_mkdir)(const char *path, mode_t mode) = NULL;



int
mkdir(const char *path, mode_t mode) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_mkdir) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("mkdir");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_mkdir)(path, mode);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: mkdir\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "mkdir - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "mkdir failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "mkdir calling real syscall.\n");
		rc = (*real_mkdir)(path, mode);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, AT_SYMLINK_NOFOLLOW);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_mkdir(path, mode);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "mkdir - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: mkdir returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: mkdir returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_mkdir(const char *path, mode_t mode) {
	int rc = -1;
	
	

#include "ports/unix/guts/mkdir.c"

	return rc;
}



static int (*real_mkdirat)(int dirfd, const char *path, mode_t mode) = NULL;



int
mkdirat(int dirfd, const char *path, mode_t mode) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_mkdirat) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("mkdirat");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_mkdirat)(dirfd, path, mode);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: mkdirat\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "mkdirat - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "mkdirat failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "mkdirat calling real syscall.\n");
		rc = (*real_mkdirat)(dirfd, path, mode);
	} else {
		path = pseudo_root_path(__func__, __LINE__, dirfd, path, AT_SYMLINK_NOFOLLOW);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_mkdirat(dirfd, path, mode);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "mkdirat - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: mkdirat returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: mkdirat returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_mkdirat(int dirfd, const char *path, mode_t mode) {
	int rc = -1;
	
	

#include "ports/unix/guts/mkdirat.c"

	return rc;
}



static char * (*real_mkdtemp)(char *template) = NULL;



char *
mkdtemp(char *template) {
	sigset_t saved;
	
	char *rc = NULL;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_mkdtemp) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("mkdtemp");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_mkdtemp)(template);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: mkdtemp\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "mkdtemp - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "mkdtemp failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return NULL;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "mkdtemp calling real syscall.\n");
		rc = (*real_mkdtemp)(template);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_mkdtemp(template);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "mkdtemp - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: mkdtemp returns %s (errno: %s)\n", rc ? rc : "<nil>", strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: mkdtemp returns %s (errno: %d)\n", rc ? rc : "<nil>", save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static char *
wrap_mkdtemp(char *template) {
	char *rc = NULL;
	
	

#include "ports/unix/guts/mkdtemp.c"

	return rc;
}



static int (*real_mkfifo)(const char *path, mode_t mode) = NULL;



int
mkfifo(const char *path, mode_t mode) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_mkfifo) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("mkfifo");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_mkfifo)(path, mode);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: mkfifo\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "mkfifo - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "mkfifo failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "mkfifo calling real syscall.\n");
		rc = (*real_mkfifo)(path, mode);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, AT_SYMLINK_NOFOLLOW);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_mkfifo(path, mode);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "mkfifo - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: mkfifo returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: mkfifo returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_mkfifo(const char *path, mode_t mode) {
	int rc = -1;
	
	

#include "ports/unix/guts/mkfifo.c"

	return rc;
}



static int (*real_mkfifoat)(int dirfd, const char *path, mode_t mode) = NULL;



int
mkfifoat(int dirfd, const char *path, mode_t mode) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_mkfifoat) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("mkfifoat");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_mkfifoat)(dirfd, path, mode);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: mkfifoat\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "mkfifoat - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "mkfifoat failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "mkfifoat calling real syscall.\n");
		rc = (*real_mkfifoat)(dirfd, path, mode);
	} else {
		path = pseudo_root_path(__func__, __LINE__, dirfd, path, AT_SYMLINK_NOFOLLOW);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_mkfifoat(dirfd, path, mode);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "mkfifoat - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: mkfifoat returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: mkfifoat returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_mkfifoat(int dirfd, const char *path, mode_t mode) {
	int rc = -1;
	
	

#include "ports/unix/guts/mkfifoat.c"

	return rc;
}



static int (*real_mknod)(const char *path, mode_t mode, dev_t dev) = pseudo_mknod;



int
mknod(const char *path, mode_t mode, dev_t dev) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_mknod) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("mknod");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_mknod)(path, mode, dev);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: mknod\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "mknod - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "mknod failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "mknod calling real syscall.\n");
		rc = (*real_mknod)(path, mode, dev);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, 0);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_mknod(path, mode, dev);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "mknod - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: mknod returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: mknod returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_mknod(const char *path, mode_t mode, dev_t dev) {
	int rc = -1;
	
	

#include "ports/linux/guts/mknod.c"

	return rc;
}



static int (*real_mknodat)(int dirfd, const char *path, mode_t mode, dev_t dev) = pseudo_mknodat;



int
mknodat(int dirfd, const char *path, mode_t mode, dev_t dev) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_mknodat) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("mknodat");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_mknodat)(dirfd, path, mode, dev);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: mknodat\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "mknodat - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "mknodat failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "mknodat calling real syscall.\n");
		rc = (*real_mknodat)(dirfd, path, mode, dev);
	} else {
		path = pseudo_root_path(__func__, __LINE__, dirfd, path, 0);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_mknodat(dirfd, path, mode, dev);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "mknodat - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: mknodat returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: mknodat returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_mknodat(int dirfd, const char *path, mode_t mode, dev_t dev) {
	int rc = -1;
	
	

#include "ports/linux/guts/mknodat.c"

	return rc;
}



static int (*real_mkostemp)(char *template, int oflags) = NULL;



int
mkostemp(char *template, int oflags) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_mkostemp) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("mkostemp");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_mkostemp)(template, oflags);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: mkostemp\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "mkostemp - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "mkostemp failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "mkostemp calling real syscall.\n");
		rc = (*real_mkostemp)(template, oflags);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_mkostemp(template, oflags);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "mkostemp - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: mkostemp returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: mkostemp returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_mkostemp(char *template, int oflags) {
	int rc = -1;
	
	

#include "ports/unix/guts/mkostemp.c"

	return rc;
}



static int (*real_mkostemps)(char *template, int suffixlen, int oflags) = NULL;



int
mkostemps(char *template, int suffixlen, int oflags) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_mkostemps) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("mkostemps");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_mkostemps)(template, suffixlen, oflags);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: mkostemps\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "mkostemps - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "mkostemps failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "mkostemps calling real syscall.\n");
		rc = (*real_mkostemps)(template, suffixlen, oflags);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_mkostemps(template, suffixlen, oflags);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "mkostemps - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: mkostemps returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: mkostemps returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_mkostemps(char *template, int suffixlen, int oflags) {
	int rc = -1;
	
	

#include "ports/unix/guts/mkostemps.c"

	return rc;
}



static int (*real_mkstemp)(char *template) = NULL;



int
mkstemp(char *template) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_mkstemp) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("mkstemp");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_mkstemp)(template);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: mkstemp\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "mkstemp - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "mkstemp failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "mkstemp calling real syscall.\n");
		rc = (*real_mkstemp)(template);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_mkstemp(template);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "mkstemp - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: mkstemp returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: mkstemp returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_mkstemp(char *template) {
	int rc = -1;
	
	

#include "ports/unix/guts/mkstemp.c"

	return rc;
}



static int (*real_mkstemp64)(char *template) = NULL;



int
mkstemp64(char *template) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_mkstemp64) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("mkstemp64");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_mkstemp64)(template);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: mkstemp64\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "mkstemp64 - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "mkstemp64 failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "mkstemp64 calling real syscall.\n");
		rc = (*real_mkstemp64)(template);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_mkstemp64(template);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "mkstemp64 - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: mkstemp64 returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: mkstemp64 returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_mkstemp64(char *template) {
	int rc = -1;
	
	

#include "ports/linux/guts/mkstemp64.c"

	return rc;
}



static int (*real_mkstemps)(char *template, int suffixlen) = NULL;



int
mkstemps(char *template, int suffixlen) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_mkstemps) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("mkstemps");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_mkstemps)(template, suffixlen);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: mkstemps\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "mkstemps - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "mkstemps failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "mkstemps calling real syscall.\n");
		rc = (*real_mkstemps)(template, suffixlen);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_mkstemps(template, suffixlen);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "mkstemps - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: mkstemps returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: mkstemps returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_mkstemps(char *template, int suffixlen) {
	int rc = -1;
	
	

#include "ports/unix/guts/mkstemps.c"

	return rc;
}



static char * (*real_mktemp)(char *template) = NULL;



char *
mktemp(char *template) {
	sigset_t saved;
	
	char *rc = NULL;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_mktemp) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("mktemp");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_mktemp)(template);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: mktemp\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "mktemp - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "mktemp failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return NULL;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "mktemp calling real syscall.\n");
		rc = (*real_mktemp)(template);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_mktemp(template);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "mktemp - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: mktemp returns %s (errno: %s)\n", rc ? rc : "<nil>", strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: mktemp returns %s (errno: %d)\n", rc ? rc : "<nil>", save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static char *
wrap_mktemp(char *template) {
	char *rc = NULL;
	
	

#include "ports/unix/guts/mktemp.c"

	return rc;
}



static int (*real_msync)(void *addr, size_t length, int flags) = NULL;



int
msync(void *addr, size_t length, int flags) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;

/* This function is not called if pseudo is configured --enable-force-async */
#ifdef PSEUDO_FORCE_ASYNC
	if (!pseudo_allow_fsync) {
		PROFILE_DONE;
		return 0;
	}
#endif


	if (!pseudo_check_wrappers() || !real_msync) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("msync");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_msync)(addr, length, flags);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: msync\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "msync - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "msync failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "msync calling real syscall.\n");
		rc = (*real_msync)(addr, length, flags);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_msync(addr, length, flags);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "msync - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: msync returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: msync returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_msync(void *addr, size_t length, int flags) {
	int rc = -1;
	
	

#include "ports/unix/guts/msync.c"

	return rc;
}



static int (*real_nftw)(const char *path, int (*fn)(const char *, const struct stat *, int, struct FTW *), int nopenfd, int flag) = NULL;



int
nftw(const char *path, int (*fn)(const char *, const struct stat *, int, struct FTW *), int nopenfd, int flag) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_nftw) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("nftw");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_nftw)(path, fn, nopenfd, flag);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: nftw\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "nftw - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "nftw failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "nftw calling real syscall.\n");
		rc = (*real_nftw)(path, fn, nopenfd, flag);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, 0);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_nftw(path, fn, nopenfd, flag);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "nftw - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: nftw returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: nftw returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_nftw(const char *path, int (*fn)(const char *, const struct stat *, int, struct FTW *), int nopenfd, int flag) {
	int rc = -1;
	
	

#include "ports/unix/guts/nftw.c"

	return rc;
}



static int (*real_nftw64)(const char *path, int (*fn)(const char *, const struct stat64 *, int, struct FTW *), int nopenfd, int flag) = NULL;



int
nftw64(const char *path, int (*fn)(const char *, const struct stat64 *, int, struct FTW *), int nopenfd, int flag) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_nftw64) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("nftw64");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_nftw64)(path, fn, nopenfd, flag);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: nftw64\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "nftw64 - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "nftw64 failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "nftw64 calling real syscall.\n");
		rc = (*real_nftw64)(path, fn, nopenfd, flag);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, 0);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_nftw64(path, fn, nopenfd, flag);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "nftw64 - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: nftw64 returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: nftw64 returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_nftw64(const char *path, int (*fn)(const char *, const struct stat64 *, int, struct FTW *), int nopenfd, int flag) {
	int rc = -1;
	
	

#include "ports/linux/guts/nftw64.c"

	return rc;
}



static int (*real_open)(const char *path, int flags, ... /* mode_t mode */) = NULL;



int
open(const char *path, int flags, ... /* mode_t mode */) {
	sigset_t saved;
	va_list ap;
	mode_t mode;

	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_open) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("open");
		PROFILE_DONE;
		return rc;
	}

	va_start(ap, flags);
	mode = va_arg(ap, mode_t);
	va_end(ap);


	if (pseudo_disabled) {
		rc = (*real_open)(path, flags, mode);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: open\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "open - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "open failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "open calling real syscall.\n");
		rc = (*real_open)(path, flags, mode);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, flags&O_NOFOLLOW);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_open(path, flags, mode);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "open - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: open returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: open returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_open(const char *path, int flags, ... /* mode_t mode */) {
	int rc = -1;
	va_list ap;
	mode_t mode;

	va_start(ap, flags);
	mode = va_arg(ap, mode_t);
	va_end(ap);


#include "ports/linux/guts/open.c"

	return rc;
}



static int (*real_open64)(const char *path, int flags, ... /* mode_t mode */) = NULL;



int
open64(const char *path, int flags, ... /* mode_t mode */) {
	sigset_t saved;
	va_list ap;
	mode_t mode;

	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_open64) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("open64");
		PROFILE_DONE;
		return rc;
	}

	va_start(ap, flags);
	mode = va_arg(ap, mode_t);
	va_end(ap);


	if (pseudo_disabled) {
		rc = (*real_open64)(path, flags, mode);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: open64\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "open64 - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "open64 failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "open64 calling real syscall.\n");
		rc = (*real_open64)(path, flags, mode);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, flags&O_NOFOLLOW);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_open64(path, flags, mode);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "open64 - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: open64 returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: open64 returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_open64(const char *path, int flags, ... /* mode_t mode */) {
	int rc = -1;
	va_list ap;
	mode_t mode;

	va_start(ap, flags);
	mode = va_arg(ap, mode_t);
	va_end(ap);


#include "ports/linux/guts/open64.c"

	return rc;
}



static int (*real_openat)(int dirfd, const char *path, int flags, ... /* mode_t mode */) = NULL;



int
openat(int dirfd, const char *path, int flags, ... /* mode_t mode */) {
	sigset_t saved;
	va_list ap;
	mode_t mode;

	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_openat) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("openat");
		PROFILE_DONE;
		return rc;
	}

	va_start(ap, flags);
	mode = va_arg(ap, mode_t);
	va_end(ap);


	if (pseudo_disabled) {
		rc = (*real_openat)(dirfd, path, flags, mode);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: openat\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "openat - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "openat failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "openat calling real syscall.\n");
		rc = (*real_openat)(dirfd, path, flags, mode);
	} else {
		path = pseudo_root_path(__func__, __LINE__, dirfd, path, flags&O_NOFOLLOW);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_openat(dirfd, path, flags, mode);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "openat - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: openat returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: openat returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_openat(int dirfd, const char *path, int flags, ... /* mode_t mode */) {
	int rc = -1;
	va_list ap;
	mode_t mode;

	va_start(ap, flags);
	mode = va_arg(ap, mode_t);
	va_end(ap);


#include "ports/linux/guts/openat.c"

	return rc;
}



static int (*real_openat64)(int dirfd, const char *path, int flags, ... /* mode_t mode */) = NULL;



int
openat64(int dirfd, const char *path, int flags, ... /* mode_t mode */) {
	sigset_t saved;
	va_list ap;
	mode_t mode;

	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_openat64) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("openat64");
		PROFILE_DONE;
		return rc;
	}

	va_start(ap, flags);
	mode = va_arg(ap, mode_t);
	va_end(ap);


	if (pseudo_disabled) {
		rc = (*real_openat64)(dirfd, path, flags, mode);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: openat64\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "openat64 - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "openat64 failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "openat64 calling real syscall.\n");
		rc = (*real_openat64)(dirfd, path, flags, mode);
	} else {
		path = pseudo_root_path(__func__, __LINE__, dirfd, path, flags&O_NOFOLLOW);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_openat64(dirfd, path, flags, mode);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "openat64 - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: openat64 returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: openat64 returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_openat64(int dirfd, const char *path, int flags, ... /* mode_t mode */) {
	int rc = -1;
	va_list ap;
	mode_t mode;

	va_start(ap, flags);
	mode = va_arg(ap, mode_t);
	va_end(ap);


#include "ports/linux/guts/openat64.c"

	return rc;
}



static DIR * (*real_opendir)(const char *path) = NULL;



DIR *
opendir(const char *path) {
	sigset_t saved;
	
	DIR *rc = NULL;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_opendir) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("opendir");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_opendir)(path);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: opendir\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "opendir - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "opendir failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return NULL;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "opendir calling real syscall.\n");
		rc = (*real_opendir)(path);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, 0);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_opendir(path);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "opendir - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: opendir returns %p (errno: %s)\n", (void *) rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: opendir returns %p (errno: %d)\n", (void *) rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static DIR *
wrap_opendir(const char *path) {
	DIR *rc = NULL;
	
	

#include "ports/unix/guts/opendir.c"

	return rc;
}



static long (*real_pathconf)(const char *path, int name) = NULL;



long
pathconf(const char *path, int name) {
	sigset_t saved;
	
	long rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_pathconf) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("pathconf");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_pathconf)(path, name);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: pathconf\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "pathconf - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "pathconf failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "pathconf calling real syscall.\n");
		rc = (*real_pathconf)(path, name);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, 0);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_pathconf(path, name);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "pathconf - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: pathconf returns %ld (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: pathconf returns %ld (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static long
wrap_pathconf(const char *path, int name) {
	long rc = -1;
	
	

#include "ports/unix/guts/pathconf.c"

	return rc;
}



static FILE * (*real_popen)(const char *command, const char *mode) = NULL;

/* Hand-written wrapper for this function. */
#if 0


FILE *
popen(const char *command, const char *mode) {
	sigset_t saved;
	
	FILE *rc = NULL;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_popen) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("popen");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_popen)(command, mode);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: popen\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "popen - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "popen failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return NULL;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "popen calling real syscall.\n");
		rc = (*real_popen)(command, mode);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_popen(command, mode);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "popen - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: popen returns %p (errno: %s)\n", (void *) rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: popen returns %p (errno: %d)\n", (void *) rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static FILE *
wrap_popen(const char *command, const char *mode) {
	FILE *rc = NULL;
	
	

#include "ports/unix/guts/popen.c"

	return rc;
}

/* Hand-written wrapper for this function. */
#endif


static ssize_t (*real_readlink)(const char *path, char *buf, size_t bufsiz) = NULL;



ssize_t
readlink(const char *path, char *buf, size_t bufsiz) {
	sigset_t saved;
	
	ssize_t rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_readlink) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("readlink");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_readlink)(path, buf, bufsiz);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: readlink\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "readlink - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "readlink failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "readlink calling real syscall.\n");
		rc = (*real_readlink)(path, buf, bufsiz);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, AT_SYMLINK_NOFOLLOW);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_readlink(path, buf, bufsiz);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "readlink - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: readlink returns %ld (errno: %s)\n", (long) rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: readlink returns %ld (errno: %d)\n", (long) rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static ssize_t
wrap_readlink(const char *path, char *buf, size_t bufsiz) {
	ssize_t rc = -1;
	
	

#include "ports/unix/guts/readlink.c"

	return rc;
}



static ssize_t (*real_readlinkat)(int dirfd, const char *path, char *buf, size_t bufsiz) = NULL;



ssize_t
readlinkat(int dirfd, const char *path, char *buf, size_t bufsiz) {
	sigset_t saved;
	
	ssize_t rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_readlinkat) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("readlinkat");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_readlinkat)(dirfd, path, buf, bufsiz);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: readlinkat\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "readlinkat - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "readlinkat failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "readlinkat calling real syscall.\n");
		rc = (*real_readlinkat)(dirfd, path, buf, bufsiz);
	} else {
		path = pseudo_root_path(__func__, __LINE__, dirfd, path, AT_SYMLINK_NOFOLLOW);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_readlinkat(dirfd, path, buf, bufsiz);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "readlinkat - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: readlinkat returns %ld (errno: %s)\n", (long) rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: readlinkat returns %ld (errno: %d)\n", (long) rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static ssize_t
wrap_readlinkat(int dirfd, const char *path, char *buf, size_t bufsiz) {
	ssize_t rc = -1;
	
	

#include "ports/unix/guts/readlinkat.c"

	return rc;
}



static char * (*real_realpath)(const char *name, char *resolved_name) = NULL;



char *
realpath(const char *name, char *resolved_name) {
	sigset_t saved;
	
	char *rc = NULL;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_realpath) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("realpath");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_realpath)(name, resolved_name);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: realpath\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "realpath - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "realpath failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return NULL;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "realpath calling real syscall.\n");
		rc = (*real_realpath)(name, resolved_name);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_realpath(name, resolved_name);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "realpath - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: realpath returns %s (errno: %s)\n", rc ? rc : "<nil>", strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: realpath returns %s (errno: %d)\n", rc ? rc : "<nil>", save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static char *
wrap_realpath(const char *name, char *resolved_name) {
	char *rc = NULL;
	
	

#include "ports/unix/guts/realpath.c"

	return rc;
}



static int (*real_remove)(const char *path) = NULL;



int
remove(const char *path) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_remove) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("remove");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_remove)(path);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: remove\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "remove - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "remove failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "remove calling real syscall.\n");
		rc = (*real_remove)(path);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, AT_SYMLINK_NOFOLLOW);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_remove(path);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "remove - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: remove returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: remove returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_remove(const char *path) {
	int rc = -1;
	
	

#include "ports/unix/guts/remove.c"

	return rc;
}



static int (*real_removexattr)(const char *path, const char *name) = NULL;



int
removexattr(const char *path, const char *name) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_removexattr) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("removexattr");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_removexattr)(path, name);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: removexattr\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "removexattr - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "removexattr failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "removexattr calling real syscall.\n");
		rc = (*real_removexattr)(path, name);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, 0);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_removexattr(path, name);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "removexattr - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: removexattr returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: removexattr returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_removexattr(const char *path, const char *name) {
	int rc = -1;
	
	

#include "ports/linux/xattr/guts/removexattr.c"

	return rc;
}



static int (*real_rename)(const char *oldpath, const char *newpath) = NULL;



int
rename(const char *oldpath, const char *newpath) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_rename) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("rename");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_rename)(oldpath, newpath);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: rename\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "rename - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "rename failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "rename calling real syscall.\n");
		rc = (*real_rename)(oldpath, newpath);
	} else {
		oldpath = pseudo_root_path(__func__, __LINE__, AT_FDCWD, oldpath, AT_SYMLINK_NOFOLLOW);
		newpath = pseudo_root_path(__func__, __LINE__, AT_FDCWD, newpath, AT_SYMLINK_NOFOLLOW);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_rename(oldpath, newpath);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "rename - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: rename returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: rename returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_rename(const char *oldpath, const char *newpath) {
	int rc = -1;
	
	

#include "ports/unix/guts/rename.c"

	return rc;
}



static int (*real_renameat)(int olddirfd, const char *oldpath, int newdirfd, const char *newpath) = NULL;



int
renameat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_renameat) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("renameat");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_renameat)(olddirfd, oldpath, newdirfd, newpath);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: renameat\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "renameat - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "renameat failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "renameat calling real syscall.\n");
		rc = (*real_renameat)(olddirfd, oldpath, newdirfd, newpath);
	} else {
		oldpath = pseudo_root_path(__func__, __LINE__, olddirfd, oldpath, AT_SYMLINK_NOFOLLOW);
		newpath = pseudo_root_path(__func__, __LINE__, newdirfd, newpath, AT_SYMLINK_NOFOLLOW);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_renameat(olddirfd, oldpath, newdirfd, newpath);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "renameat - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: renameat returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: renameat returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_renameat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath) {
	int rc = -1;
	
	

#include "ports/unix/guts/renameat.c"

	return rc;
}



static int (*real_renameat2)(int olddirfd, const char *oldpath, int newdirfd, const char *newpath, unsigned int flags) = NULL;



int
renameat2(int olddirfd, const char *oldpath, int newdirfd, const char *newpath, unsigned int flags) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_renameat2) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("renameat2");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_renameat2)(olddirfd, oldpath, newdirfd, newpath, flags);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: renameat2\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "renameat2 - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "renameat2 failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "renameat2 calling real syscall.\n");
		rc = (*real_renameat2)(olddirfd, oldpath, newdirfd, newpath, flags);
	} else {
		oldpath = pseudo_root_path(__func__, __LINE__, olddirfd, oldpath, AT_SYMLINK_NOFOLLOW);
		newpath = pseudo_root_path(__func__, __LINE__, newdirfd, newpath, AT_SYMLINK_NOFOLLOW);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_renameat2(olddirfd, oldpath, newdirfd, newpath, flags);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "renameat2 - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: renameat2 returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: renameat2 returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_renameat2(int olddirfd, const char *oldpath, int newdirfd, const char *newpath, unsigned int flags) {
	int rc = -1;
	
	

#include "ports/linux/guts/renameat2.c"

	return rc;
}



static int (*real_rmdir)(const char *path) = NULL;



int
rmdir(const char *path) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_rmdir) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("rmdir");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_rmdir)(path);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: rmdir\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "rmdir - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "rmdir failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "rmdir calling real syscall.\n");
		rc = (*real_rmdir)(path);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, AT_SYMLINK_NOFOLLOW);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_rmdir(path);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "rmdir - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: rmdir returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: rmdir returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_rmdir(const char *path) {
	int rc = -1;
	
	

#include "ports/unix/guts/rmdir.c"

	return rc;
}



static int (*real_scandir)(const char *path, struct dirent ***namelist, int (*filter)(const struct dirent *), int (*compar)()) = NULL;



int
scandir(const char *path, struct dirent ***namelist, int (*filter)(const struct dirent *), int (*compar)()) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_scandir) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("scandir");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_scandir)(path, namelist, filter, compar);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: scandir\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "scandir - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "scandir failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "scandir calling real syscall.\n");
		rc = (*real_scandir)(path, namelist, filter, compar);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, 0);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_scandir(path, namelist, filter, compar);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "scandir - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: scandir returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: scandir returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_scandir(const char *path, struct dirent ***namelist, int (*filter)(const struct dirent *), int (*compar)()) {
	int rc = -1;
	
	

#include "ports/linux/guts/scandir.c"

	return rc;
}



static int (*real_scandir64)(const char *path, struct dirent64 ***namelist, int (*filter)(const struct dirent64 *), int (*compar)()) = NULL;



int
scandir64(const char *path, struct dirent64 ***namelist, int (*filter)(const struct dirent64 *), int (*compar)()) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_scandir64) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("scandir64");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_scandir64)(path, namelist, filter, compar);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: scandir64\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "scandir64 - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "scandir64 failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "scandir64 calling real syscall.\n");
		rc = (*real_scandir64)(path, namelist, filter, compar);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, 0);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_scandir64(path, namelist, filter, compar);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "scandir64 - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: scandir64 returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: scandir64 returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_scandir64(const char *path, struct dirent64 ***namelist, int (*filter)(const struct dirent64 *), int (*compar)()) {
	int rc = -1;
	
	

#include "ports/linux/guts/scandir64.c"

	return rc;
}



static int (*real_setegid)(gid_t egid) = NULL;



int
setegid(gid_t egid) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_setegid) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("setegid");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_setegid)(egid);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: setegid\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "setegid - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "setegid failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "setegid calling real syscall.\n");
		rc = (*real_setegid)(egid);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_setegid(egid);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "setegid - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: setegid returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: setegid returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_setegid(gid_t egid) {
	int rc = -1;
	
	

#include "ports/uids_generic/guts/setegid.c"

	return rc;
}



static int (*real_seteuid)(uid_t euid) = NULL;



int
seteuid(uid_t euid) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_seteuid) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("seteuid");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_seteuid)(euid);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: seteuid\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "seteuid - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "seteuid failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "seteuid calling real syscall.\n");
		rc = (*real_seteuid)(euid);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_seteuid(euid);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "seteuid - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: seteuid returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: seteuid returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_seteuid(uid_t euid) {
	int rc = -1;
	
	

#include "ports/uids_generic/guts/seteuid.c"

	return rc;
}



static int (*real_setfsgid)(gid_t fsgid) = NULL;



int
setfsgid(gid_t fsgid) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_setfsgid) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("setfsgid");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_setfsgid)(fsgid);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: setfsgid\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "setfsgid - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "setfsgid failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "setfsgid calling real syscall.\n");
		rc = (*real_setfsgid)(fsgid);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_setfsgid(fsgid);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "setfsgid - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: setfsgid returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: setfsgid returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_setfsgid(gid_t fsgid) {
	int rc = -1;
	
	

#include "ports/linux/guts/setfsgid.c"

	return rc;
}



static int (*real_setfsuid)(uid_t fsuid) = NULL;



int
setfsuid(uid_t fsuid) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_setfsuid) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("setfsuid");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_setfsuid)(fsuid);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: setfsuid\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "setfsuid - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "setfsuid failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "setfsuid calling real syscall.\n");
		rc = (*real_setfsuid)(fsuid);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_setfsuid(fsuid);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "setfsuid - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: setfsuid returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: setfsuid returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_setfsuid(uid_t fsuid) {
	int rc = -1;
	
	

#include "ports/linux/guts/setfsuid.c"

	return rc;
}



static int (*real_setgid)(gid_t gid) = NULL;



int
setgid(gid_t gid) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_setgid) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("setgid");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_setgid)(gid);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: setgid\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "setgid - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "setgid failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "setgid calling real syscall.\n");
		rc = (*real_setgid)(gid);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_setgid(gid);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "setgid - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: setgid returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: setgid returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_setgid(gid_t gid) {
	int rc = -1;
	
	

#include "ports/uids_generic/guts/setgid.c"

	return rc;
}



static void (*real_setgrent)(void) = NULL;



void
setgrent(void) {
	sigset_t saved;
	
	
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_setgrent) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("setgrent");
		PROFILE_DONE;
		return;
	}

	

	if (pseudo_disabled) {
		(void) (*real_setgrent)();
		
		PROFILE_DONE;
		return;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: setgrent\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "setgrent - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "setgrent failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "setgrent calling real syscall.\n");
		(void) (*real_setgrent)();
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		(void) wrap_setgrent();
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "setgrent - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: setgrent returns void%s (errno: %s)\n", "", strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: setgrent returns void%s (errno: %d)\n", "", save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return;
}

static void
wrap_setgrent(void) {
	
	
	

#include "ports/uids_generic/guts/setgrent.c"

	return;
}



static int (*real_setgroups)(size_t size, const gid_t *list) = NULL;



int
setgroups(size_t size, const gid_t *list) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_setgroups) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("setgroups");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_setgroups)(size, list);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: setgroups\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "setgroups - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "setgroups failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "setgroups calling real syscall.\n");
		rc = (*real_setgroups)(size, list);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_setgroups(size, list);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "setgroups - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: setgroups returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: setgroups returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_setgroups(size_t size, const gid_t *list) {
	int rc = -1;
	
	

#include "ports/linux/guts/setgroups.c"

	return rc;
}



static void (*real_setpwent)(void) = NULL;



void
setpwent(void) {
	sigset_t saved;
	
	
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_setpwent) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("setpwent");
		PROFILE_DONE;
		return;
	}

	

	if (pseudo_disabled) {
		(void) (*real_setpwent)();
		
		PROFILE_DONE;
		return;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: setpwent\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "setpwent - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "setpwent failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "setpwent calling real syscall.\n");
		(void) (*real_setpwent)();
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		(void) wrap_setpwent();
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "setpwent - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: setpwent returns void%s (errno: %s)\n", "", strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: setpwent returns void%s (errno: %d)\n", "", save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return;
}

static void
wrap_setpwent(void) {
	
	
	

#include "ports/uids_generic/guts/setpwent.c"

	return;
}



static int (*real_setregid)(gid_t rgid, gid_t egid) = NULL;



int
setregid(gid_t rgid, gid_t egid) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_setregid) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("setregid");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_setregid)(rgid, egid);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: setregid\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "setregid - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "setregid failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "setregid calling real syscall.\n");
		rc = (*real_setregid)(rgid, egid);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_setregid(rgid, egid);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "setregid - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: setregid returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: setregid returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_setregid(gid_t rgid, gid_t egid) {
	int rc = -1;
	
	

#include "ports/uids_generic/guts/setregid.c"

	return rc;
}



static int (*real_setresgid)(gid_t rgid, gid_t egid, gid_t sgid) = NULL;



int
setresgid(gid_t rgid, gid_t egid, gid_t sgid) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_setresgid) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("setresgid");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_setresgid)(rgid, egid, sgid);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: setresgid\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "setresgid - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "setresgid failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "setresgid calling real syscall.\n");
		rc = (*real_setresgid)(rgid, egid, sgid);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_setresgid(rgid, egid, sgid);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "setresgid - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: setresgid returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: setresgid returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_setresgid(gid_t rgid, gid_t egid, gid_t sgid) {
	int rc = -1;
	
	

#include "ports/linux/guts/setresgid.c"

	return rc;
}



static int (*real_setresuid)(uid_t ruid, uid_t euid, uid_t suid) = NULL;



int
setresuid(uid_t ruid, uid_t euid, uid_t suid) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_setresuid) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("setresuid");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_setresuid)(ruid, euid, suid);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: setresuid\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "setresuid - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "setresuid failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "setresuid calling real syscall.\n");
		rc = (*real_setresuid)(ruid, euid, suid);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_setresuid(ruid, euid, suid);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "setresuid - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: setresuid returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: setresuid returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_setresuid(uid_t ruid, uid_t euid, uid_t suid) {
	int rc = -1;
	
	

#include "ports/linux/guts/setresuid.c"

	return rc;
}



static int (*real_setreuid)(uid_t ruid, uid_t euid) = NULL;



int
setreuid(uid_t ruid, uid_t euid) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_setreuid) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("setreuid");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_setreuid)(ruid, euid);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: setreuid\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "setreuid - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "setreuid failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "setreuid calling real syscall.\n");
		rc = (*real_setreuid)(ruid, euid);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_setreuid(ruid, euid);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "setreuid - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: setreuid returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: setreuid returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_setreuid(uid_t ruid, uid_t euid) {
	int rc = -1;
	
	

#include "ports/uids_generic/guts/setreuid.c"

	return rc;
}



static int (*real_setuid)(uid_t uid) = NULL;



int
setuid(uid_t uid) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_setuid) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("setuid");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_setuid)(uid);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: setuid\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "setuid - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "setuid failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "setuid calling real syscall.\n");
		rc = (*real_setuid)(uid);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_setuid(uid);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "setuid - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: setuid returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: setuid returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_setuid(uid_t uid) {
	int rc = -1;
	
	

#include "ports/uids_generic/guts/setuid.c"

	return rc;
}



static int (*real_setxattr)(const char *path, const char *name, const void *value, size_t size, int xflags) = NULL;



int
setxattr(const char *path, const char *name, const void *value, size_t size, int xflags) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_setxattr) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("setxattr");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_setxattr)(path, name, value, size, xflags);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: setxattr\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "setxattr - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "setxattr failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "setxattr calling real syscall.\n");
		rc = (*real_setxattr)(path, name, value, size, xflags);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, 0);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_setxattr(path, name, value, size, xflags);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "setxattr - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: setxattr returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: setxattr returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_setxattr(const char *path, const char *name, const void *value, size_t size, int xflags) {
	int rc = -1;
	
	

#include "ports/linux/xattr/guts/setxattr.c"

	return rc;
}



static int (*real_stat)(const char *path, struct stat *buf) = pseudo_stat;



int
stat(const char *path, struct stat *buf) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_stat) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("stat");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_stat)(path, buf);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: stat\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "stat - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "stat failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "stat calling real syscall.\n");
		rc = (*real_stat)(path, buf);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, 0);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_stat(path, buf);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "stat - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: stat returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: stat returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_stat(const char *path, struct stat *buf) {
	int rc = -1;
	
	

#include "ports/linux/guts/stat.c"

	return rc;
}



static int (*real_stat64)(const char *path, struct stat64 *buf) = pseudo_stat64;



int
stat64(const char *path, struct stat64 *buf) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_stat64) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("stat64");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_stat64)(path, buf);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: stat64\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "stat64 - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "stat64 failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "stat64 calling real syscall.\n");
		rc = (*real_stat64)(path, buf);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, 0);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_stat64(path, buf);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "stat64 - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: stat64 returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: stat64 returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_stat64(const char *path, struct stat64 *buf) {
	int rc = -1;
	
	

#include "ports/linux/guts/stat64.c"

	return rc;
}



static int (*real_statvfs)(const char *path, struct statvfs *buf) = NULL;



int
statvfs(const char *path, struct statvfs *buf) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_statvfs) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("statvfs");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_statvfs)(path, buf);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: statvfs\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "statvfs - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "statvfs failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "statvfs calling real syscall.\n");
		rc = (*real_statvfs)(path, buf);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, 0);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_statvfs(path, buf);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "statvfs - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: statvfs returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: statvfs returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_statvfs(const char *path, struct statvfs *buf) {
	int rc = -1;
	
	

#include "ports/linux/statvfs/guts/statvfs.c"

	return rc;
}



static int (*real_statx)(int dirfd, const char *pathname, int flags, unsigned int mask, struct statx *statxbuf) = NULL;



int
statx(int dirfd, const char *pathname, int flags, unsigned int mask, struct statx *statxbuf) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_statx) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("statx");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_statx)(dirfd, pathname, flags, mask, statxbuf);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: statx\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "statx - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "statx failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "statx calling real syscall.\n");
		rc = (*real_statx)(dirfd, pathname, flags, mask, statxbuf);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_statx(dirfd, pathname, flags, mask, statxbuf);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "statx - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: statx returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: statx returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_statx(int dirfd, const char *pathname, int flags, unsigned int mask, struct statx *statxbuf) {
	int rc = -1;
	
	

#include "ports/linux/statx/guts/statx.c"

	return rc;
}



static int (*real_symlink)(const char *oldname, const char *newpath) = NULL;



int
symlink(const char *oldname, const char *newpath) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_symlink) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("symlink");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_symlink)(oldname, newpath);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: symlink\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "symlink - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "symlink failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "symlink calling real syscall.\n");
		rc = (*real_symlink)(oldname, newpath);
	} else {
		newpath = pseudo_root_path(__func__, __LINE__, AT_FDCWD, newpath, AT_SYMLINK_NOFOLLOW);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_symlink(oldname, newpath);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "symlink - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: symlink returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: symlink returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_symlink(const char *oldname, const char *newpath) {
	int rc = -1;
	
	

#include "ports/unix/guts/symlink.c"

	return rc;
}



static int (*real_symlinkat)(const char *oldname, int dirfd, const char *newpath) = NULL;



int
symlinkat(const char *oldname, int dirfd, const char *newpath) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_symlinkat) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("symlinkat");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_symlinkat)(oldname, dirfd, newpath);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: symlinkat\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "symlinkat - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "symlinkat failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "symlinkat calling real syscall.\n");
		rc = (*real_symlinkat)(oldname, dirfd, newpath);
	} else {
		newpath = pseudo_root_path(__func__, __LINE__, dirfd, newpath, AT_SYMLINK_NOFOLLOW);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_symlinkat(oldname, dirfd, newpath);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "symlinkat - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: symlinkat returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: symlinkat returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_symlinkat(const char *oldname, int dirfd, const char *newpath) {
	int rc = -1;
	
	

#include "ports/unix/guts/symlinkat.c"

	return rc;
}



static void (*real_sync)(void) = NULL;



void
sync(void) {
	sigset_t saved;
	
	
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_sync) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("sync");
		PROFILE_DONE;
		return;
	}

	

	if (pseudo_disabled) {
		(void) (*real_sync)();
		
		PROFILE_DONE;
		return;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: sync\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "sync - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "sync failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "sync calling real syscall.\n");
		(void) (*real_sync)();
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		(void) wrap_sync();
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "sync - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: sync returns void%s (errno: %s)\n", "", strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: sync returns void%s (errno: %d)\n", "", save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return;
}

static void
wrap_sync(void) {
	
	
	

#include "ports/unix/guts/sync.c"

	return;
}



static int (*real_sync_file_range)(int fd, off64_t offset, off64_t nbytes, unsigned int flags) = NULL;



int
sync_file_range(int fd, off64_t offset, off64_t nbytes, unsigned int flags) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;

/* This function is not called if pseudo is configured --enable-force-async */
#ifdef PSEUDO_FORCE_ASYNC
	if (!pseudo_allow_fsync) {
		PROFILE_DONE;
		return 0;
	}
#endif


	if (!pseudo_check_wrappers() || !real_sync_file_range) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("sync_file_range");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_sync_file_range)(fd, offset, nbytes, flags);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: sync_file_range\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "sync_file_range - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "sync_file_range failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "sync_file_range calling real syscall.\n");
		rc = (*real_sync_file_range)(fd, offset, nbytes, flags);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_sync_file_range(fd, offset, nbytes, flags);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "sync_file_range - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: sync_file_range returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: sync_file_range returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_sync_file_range(int fd, off64_t offset, off64_t nbytes, unsigned int flags) {
	int rc = -1;
	
	

#include "ports/unix/guts/sync_file_range.c"

	return rc;
}



static int (*real_syncfs)(int fd) = NULL;



int
syncfs(int fd) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;

/* This function is not called if pseudo is configured --enable-force-async */
#ifdef PSEUDO_FORCE_ASYNC
	if (!pseudo_allow_fsync) {
		PROFILE_DONE;
		return 0;
	}
#endif


	if (!pseudo_check_wrappers() || !real_syncfs) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("syncfs");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_syncfs)(fd);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: syncfs\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "syncfs - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "syncfs failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "syncfs calling real syscall.\n");
		rc = (*real_syncfs)(fd);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_syncfs(fd);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "syncfs - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: syncfs returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: syncfs returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_syncfs(int fd) {
	int rc = -1;
	
	

#include "ports/unix/syncfs/guts/syncfs.c"

	return rc;
}



static long (*real_syscall)(long nr, ...) = NULL;

/* Hand-written wrapper for this function. */
#if 0


long
syscall(long nr, ...) {
	sigset_t saved;
	va_list ap;

	long rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_syscall) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("syscall");
		PROFILE_DONE;
		return rc;
	}

	va_start(ap, nr);


	if (pseudo_disabled) {
		rc = (*real_syscall)(nr, ap);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: syscall\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "syscall - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "syscall failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "syscall calling real syscall.\n");
		rc = (*real_syscall)(nr, ap);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_syscall(nr, ap);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "syscall - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: syscall returns %ld (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: syscall returns %ld (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static long
wrap_syscall(long nr, va_list ap) {
	long rc = -1;
	
	

#include "ports/linux/guts/syscall.c"

	return rc;
}

/* Hand-written wrapper for this function. */
#endif


static int (*real_system)(const char *command) = NULL;



int
system(const char *command) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_system) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("system");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_system)(command);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: system\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "system - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "system failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "system calling real syscall.\n");
		rc = (*real_system)(command);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_system(command);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "system - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: system returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: system returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_system(const char *command) {
	int rc = -1;
	
	

#include "ports/unix/guts/system.c"

	return rc;
}



static char * (*real_tempnam)(const char *template, const char *pfx) = NULL;



char *
tempnam(const char *template, const char *pfx) {
	sigset_t saved;
	
	char *rc = NULL;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_tempnam) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("tempnam");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_tempnam)(template, pfx);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: tempnam\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "tempnam - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "tempnam failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return NULL;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "tempnam calling real syscall.\n");
		rc = (*real_tempnam)(template, pfx);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_tempnam(template, pfx);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "tempnam - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: tempnam returns %s (errno: %s)\n", rc ? rc : "<nil>", strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: tempnam returns %s (errno: %d)\n", rc ? rc : "<nil>", save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static char *
wrap_tempnam(const char *template, const char *pfx) {
	char *rc = NULL;
	
	

#include "ports/unix/guts/tempnam.c"

	return rc;
}



static char * (*real_tmpnam)(char *s) = NULL;



char *
tmpnam(char *s) {
	sigset_t saved;
	
	char *rc = NULL;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_tmpnam) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("tmpnam");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_tmpnam)(s);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: tmpnam\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "tmpnam - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "tmpnam failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return NULL;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "tmpnam calling real syscall.\n");
		rc = (*real_tmpnam)(s);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_tmpnam(s);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "tmpnam - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: tmpnam returns %s (errno: %s)\n", rc ? rc : "<nil>", strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: tmpnam returns %s (errno: %d)\n", rc ? rc : "<nil>", save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static char *
wrap_tmpnam(char *s) {
	char *rc = NULL;
	
	

#include "ports/unix/guts/tmpnam.c"

	return rc;
}



static int (*real_truncate)(const char *path, off_t length) = NULL;



int
truncate(const char *path, off_t length) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_truncate) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("truncate");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_truncate)(path, length);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: truncate\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "truncate - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "truncate failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "truncate calling real syscall.\n");
		rc = (*real_truncate)(path, length);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, 0);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_truncate(path, length);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "truncate - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: truncate returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: truncate returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_truncate(const char *path, off_t length) {
	int rc = -1;
	
	

#include "ports/unix/guts/truncate.c"

	return rc;
}



static int (*real_truncate64)(const char *path, off64_t length) = NULL;



int
truncate64(const char *path, off64_t length) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_truncate64) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("truncate64");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_truncate64)(path, length);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: truncate64\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "truncate64 - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "truncate64 failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "truncate64 calling real syscall.\n");
		rc = (*real_truncate64)(path, length);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, 0);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_truncate64(path, length);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "truncate64 - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: truncate64 returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: truncate64 returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_truncate64(const char *path, off64_t length) {
	int rc = -1;
	
	

#include "ports/linux/guts/truncate64.c"

	return rc;
}



static int (*real_ulckpwdf)(void) = NULL;



int
ulckpwdf(void) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_ulckpwdf) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("ulckpwdf");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_ulckpwdf)();
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: ulckpwdf\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "ulckpwdf - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "ulckpwdf failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "ulckpwdf calling real syscall.\n");
		rc = (*real_ulckpwdf)();
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_ulckpwdf();
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "ulckpwdf - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: ulckpwdf returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: ulckpwdf returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_ulckpwdf(void) {
	int rc = -1;
	
	

#include "ports/linux/guts/ulckpwdf.c"

	return rc;
}



static mode_t (*real_umask)(mode_t mask) = NULL;



mode_t
umask(mode_t mask) {
	sigset_t saved;
	
	mode_t rc = 0;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_umask) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("umask");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_umask)(mask);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: umask\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "umask - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "umask failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return 0;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "umask calling real syscall.\n");
		rc = (*real_umask)(mask);
	} else {
		
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_umask(mask);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "umask - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: umask returns 0%lo (errno: %s)\n", (long) rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: umask returns 0%lo (errno: %d)\n", (long) rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static mode_t
wrap_umask(mode_t mask) {
	mode_t rc = 0;
	
	

#include "ports/unix/guts/umask.c"

	return rc;
}



static int (*real_unlink)(const char *path) = NULL;



int
unlink(const char *path) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_unlink) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("unlink");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_unlink)(path);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: unlink\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "unlink - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "unlink failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "unlink calling real syscall.\n");
		rc = (*real_unlink)(path);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, AT_SYMLINK_NOFOLLOW);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_unlink(path);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "unlink - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: unlink returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: unlink returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_unlink(const char *path) {
	int rc = -1;
	
	

#include "ports/unix/guts/unlink.c"

	return rc;
}



static int (*real_unlinkat)(int dirfd, const char *path, int rflags) = NULL;



int
unlinkat(int dirfd, const char *path, int rflags) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_unlinkat) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("unlinkat");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_unlinkat)(dirfd, path, rflags);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: unlinkat\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "unlinkat - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "unlinkat failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "unlinkat calling real syscall.\n");
		rc = (*real_unlinkat)(dirfd, path, rflags);
	} else {
		path = pseudo_root_path(__func__, __LINE__, dirfd, path, AT_SYMLINK_NOFOLLOW);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_unlinkat(dirfd, path, rflags);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "unlinkat - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: unlinkat returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: unlinkat returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_unlinkat(int dirfd, const char *path, int rflags) {
	int rc = -1;
	
	

#include "ports/unix/guts/unlinkat.c"

	return rc;
}



static int (*real_utime)(const char *path, const struct utimbuf *buf) = NULL;



int
utime(const char *path, const struct utimbuf *buf) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_utime) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("utime");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_utime)(path, buf);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: utime\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "utime - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "utime failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "utime calling real syscall.\n");
		rc = (*real_utime)(path, buf);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, 0);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_utime(path, buf);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "utime - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: utime returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: utime returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_utime(const char *path, const struct utimbuf *buf) {
	int rc = -1;
	
	

#include "ports/unix/guts/utime.c"

	return rc;
}



static int (*real_utimes)(const char *path, const struct timeval *times) = NULL;



int
utimes(const char *path, const struct timeval *times) {
	sigset_t saved;
	
	int rc = -1;
	PROFILE_START;



	if (!pseudo_check_wrappers() || !real_utimes) {
		/* rc was initialized to the "failure" value */
		pseudo_enosys("utimes");
		PROFILE_DONE;
		return rc;
	}

	

	if (pseudo_disabled) {
		rc = (*real_utimes)(path, times);
		
		PROFILE_DONE;
		return rc;
	}

	pseudo_debug(PDBGF_WRAPPER, "wrapper called: utimes\n");
	pseudo_sigblock(&saved);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "utimes - signals blocked, obtaining lock\n");
	if (pseudo_getlock()) {
		errno = EBUSY;
		sigprocmask(SIG_SETMASK, &saved, NULL);
		pseudo_debug(PDBGF_WRAPPER, "utimes failed to get lock, giving EBUSY.\n");
		PROFILE_DONE;
		return -1;
	}

	int save_errno;
	if (antimagic > 0) {
		/* call the real syscall */
		pseudo_debug(PDBGF_SYSCALL, "utimes calling real syscall.\n");
		rc = (*real_utimes)(path, times);
	} else {
		path = pseudo_root_path(__func__, __LINE__, AT_FDCWD, path, 0);
		/* exec*() use this to restore the sig mask */
		pseudo_saved_sigmask = saved;
		rc = wrap_utimes(path, times);
	}
	
	save_errno = errno;
	pseudo_droplock();
	sigprocmask(SIG_SETMASK, &saved, NULL);
	pseudo_debug(PDBGF_WRAPPER | PDBGF_VERBOSE, "utimes - yielded lock, restored signals\n");
#if 0
/* This can cause hangs on some recentish systems which use locale
 * stuff for strerror...
 */
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: utimes returns %d (errno: %s)\n", rc, strerror(save_errno));
#endif
	pseudo_debug(PDBGF_WRAPPER, "wrapper completed: utimes returns %d (errno: %d)\n", rc, save_errno);
	errno = save_errno;
	PROFILE_DONE;
	return rc;
}

static int
wrap_utimes(const char *path, const struct timeval *times) {
	int rc = -1;
	
	

#include "ports/unix/guts/utimes.c"

	return rc;
}


