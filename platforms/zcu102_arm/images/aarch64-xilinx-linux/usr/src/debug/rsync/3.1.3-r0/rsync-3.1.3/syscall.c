/*
 * Syscall wrappers to ensure that nothing gets done in dry_run mode
 * and to handle system peculiarities.
 *
 * Copyright (C) 1998 Andrew Tridgell
 * Copyright (C) 2002 Martin Pool
 * Copyright (C) 2003-2018 Wayne Davison
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, visit the http://fsf.org website.
 */

#include "rsync.h"

#if !defined MKNOD_CREATES_SOCKETS && defined HAVE_SYS_UN_H
#include <sys/un.h>
#endif
#ifdef HAVE_SYS_ATTR_H
#include <sys/attr.h>
#endif

#if defined HAVE_SYS_FALLOCATE && !defined HAVE_FALLOCATE
#include <sys/syscall.h>
#endif

extern int dry_run;
extern int am_root;
extern int am_sender;
extern int read_only;
extern int list_only;
extern int inplace;
extern int preallocate_files;
extern int preserve_perms;
extern int preserve_executability;

#ifndef S_BLKSIZE
# if defined hpux || defined __hpux__ || defined __hpux
#  define S_BLKSIZE 1024
# elif defined _AIX && defined _I386
#  define S_BLKSIZE 4096
# else
#  define S_BLKSIZE 512
# endif
#endif

#define RETURN_ERROR_IF(x,e) \
	do { \
		if (x) { \
			errno = (e); \
			return -1; \
		} \
	} while (0)

#define RETURN_ERROR_IF_RO_OR_LO RETURN_ERROR_IF(read_only || list_only, EROFS)

int do_unlink(const char *fname)
{
	if (dry_run) return 0;
	RETURN_ERROR_IF_RO_OR_LO;
	return unlink(fname);
}

#ifdef SUPPORT_LINKS
int do_symlink(const char *lnk, const char *fname)
{
	if (dry_run) return 0;
	RETURN_ERROR_IF_RO_OR_LO;

#if defined NO_SYMLINK_XATTRS || defined NO_SYMLINK_USER_XATTRS
	/* For --fake-super, we create a normal file with mode 0600
	 * and write the lnk into it. */
	if (am_root < 0) {
		int ok, len = strlen(lnk);
		int fd = open(fname, O_WRONLY|O_CREAT|O_TRUNC, S_IWUSR|S_IRUSR);
		if (fd < 0)
			return -1;
		ok = write(fd, lnk, len) == len;
		if (close(fd) < 0)
			ok = 0;
		return ok ? 0 : -1;
	}
#endif

	return symlink(lnk, fname);
}

#if defined NO_SYMLINK_XATTRS || defined NO_SYMLINK_USER_XATTRS
ssize_t do_readlink(const char *path, char *buf, size_t bufsiz)
{
	/* For --fake-super, we read the link from the file. */
	if (am_root < 0) {
		int fd = do_open_nofollow(path, O_RDONLY);
		if (fd >= 0) {
			int len = read(fd, buf, bufsiz);
			close(fd);
			return len;
		}
		if (errno != ELOOP)
			return -1;
		/* A real symlink needs to be turned into a fake one on the receiving
		 * side, so tell the generator that the link has no length. */
		if (!am_sender)
			return 0;
		/* Otherwise fall through and let the sender report the real length. */
	}

	return readlink(path, buf, bufsiz);
}
#endif
#endif

#ifdef HAVE_LINK
int do_link(const char *fname1, const char *fname2)
{
	if (dry_run) return 0;
	RETURN_ERROR_IF_RO_OR_LO;
	return link(fname1, fname2);
}
#endif

int do_lchown(const char *path, uid_t owner, gid_t group)
{
	if (dry_run) return 0;
	RETURN_ERROR_IF_RO_OR_LO;
#ifndef HAVE_LCHOWN
#define lchown chown
#endif
	return lchown(path, owner, group);
}

int do_mknod(const char *pathname, mode_t mode, dev_t dev)
{
	if (dry_run) return 0;
	RETURN_ERROR_IF_RO_OR_LO;

	/* For --fake-super, we create a normal file with mode 0600. */
	if (am_root < 0) {
		int fd = open(pathname, O_WRONLY|O_CREAT|O_TRUNC, S_IWUSR|S_IRUSR);
		if (fd < 0 || close(fd) < 0)
			return -1;
		return 0;
	}

#if !defined MKNOD_CREATES_FIFOS && defined HAVE_MKFIFO
	if (S_ISFIFO(mode))
		return mkfifo(pathname, mode);
#endif
#if !defined MKNOD_CREATES_SOCKETS && defined HAVE_SYS_UN_H
	if (S_ISSOCK(mode)) {
		int sock;
		struct sockaddr_un saddr;
		unsigned int len = strlcpy(saddr.sun_path, pathname, sizeof saddr.sun_path);
		if (len >= sizeof saddr.sun_path) {
			errno = ENAMETOOLONG;
			return -1;
		}
#ifdef HAVE_SOCKADDR_UN_LEN
		saddr.sun_len = len + 1;
#endif
		saddr.sun_family = AF_UNIX;

		if ((sock = socket(PF_UNIX, SOCK_STREAM, 0)) < 0
		    || (unlink(pathname) < 0 && errno != ENOENT)
		    || (bind(sock, (struct sockaddr*)&saddr, sizeof saddr)) < 0)
			return -1;
		close(sock);
#ifdef HAVE_CHMOD
		return do_chmod(pathname, mode);
#else
		return 0;
#endif
	}
#endif
#ifdef HAVE_MKNOD
	return mknod(pathname, mode, dev);
#else
	return -1;
#endif
}

int do_rmdir(const char *pathname)
{
	if (dry_run) return 0;
	RETURN_ERROR_IF_RO_OR_LO;
	return rmdir(pathname);
}

int do_open(const char *pathname, int flags, mode_t mode)
{
	if (flags != O_RDONLY) {
		RETURN_ERROR_IF(dry_run, 0);
		RETURN_ERROR_IF_RO_OR_LO;
	}

	return open(pathname, flags | O_BINARY, mode);
}

#ifdef HAVE_CHMOD
int do_chmod(const char *path, mode_t mode)
{
	int code;
	if (dry_run) return 0;
	RETURN_ERROR_IF_RO_OR_LO;
#ifdef HAVE_LCHMOD
	code = lchmod(path, mode & CHMOD_BITS);
#else
	if (S_ISLNK(mode)) {
# if defined HAVE_SETATTRLIST
		struct attrlist attrList;
		uint32_t m = mode & CHMOD_BITS; /* manpage is wrong: not mode_t! */

		memset(&attrList, 0, sizeof attrList);
		attrList.bitmapcount = ATTR_BIT_MAP_COUNT;
		attrList.commonattr = ATTR_CMN_ACCESSMASK;
		code = setattrlist(path, &attrList, &m, sizeof m, FSOPT_NOFOLLOW);
# else
		code = 1;
# endif
	} else
		code = chmod(path, mode & CHMOD_BITS); /* DISCOURAGED FUNCTION */
#endif /* !HAVE_LCHMOD */
	if (code != 0 && (preserve_perms || preserve_executability))
		return code;
	return 0;
}
#endif

int do_rename(const char *fname1, const char *fname2)
{
	if (dry_run) return 0;
	RETURN_ERROR_IF_RO_OR_LO;
	return rename(fname1, fname2);
}

#ifdef HAVE_FTRUNCATE
int do_ftruncate(int fd, OFF_T size)
{
	int ret;

	if (dry_run) return 0;
	RETURN_ERROR_IF_RO_OR_LO;

	do {
		ret = ftruncate(fd, size);
	} while (ret < 0 && errno == EINTR);

	return ret;
}
#endif

void trim_trailing_slashes(char *name)
{
	int l;
	/* Some BSD systems cannot make a directory if the name
	 * contains a trailing slash.
	 * <http://www.opensource.apple.com/bugs/X/BSD%20Kernel/2734739.html> */

	/* Don't change empty string; and also we can't improve on
	 * "/" */

	l = strlen(name);
	while (l > 1) {
		if (name[--l] != '/')
			break;
		name[l] = '\0';
	}
}

int do_mkdir(char *fname, mode_t mode)
{
	if (dry_run) return 0;
	RETURN_ERROR_IF_RO_OR_LO;
	trim_trailing_slashes(fname);
	return mkdir(fname, mode);
}

/* like mkstemp but forces permissions */
int do_mkstemp(char *template, mode_t perms)
{
	RETURN_ERROR_IF(dry_run, 0);
	RETURN_ERROR_IF(read_only, EROFS);
	perms |= S_IWUSR;

#if defined HAVE_SECURE_MKSTEMP && defined HAVE_FCHMOD && (!defined HAVE_OPEN64 || defined HAVE_MKSTEMP64)
	{
		int fd = mkstemp(template);
		if (fd == -1)
			return -1;
		if (fchmod(fd, perms) != 0 && preserve_perms) {
			int errno_save = errno;
			close(fd);
			unlink(template);
			errno = errno_save;
			return -1;
		}
#if defined HAVE_SETMODE && O_BINARY
		setmode(fd, O_BINARY);
#endif
		return fd;
	}
#else
	if (!mktemp(template))
		return -1;
	return do_open(template, O_RDWR|O_EXCL|O_CREAT, perms);
#endif
}

int do_stat(const char *fname, STRUCT_STAT *st)
{
#ifdef USE_STAT64_FUNCS
	return stat64(fname, st);
#else
	return stat(fname, st);
#endif
}

int do_lstat(const char *fname, STRUCT_STAT *st)
{
#ifdef SUPPORT_LINKS
# ifdef USE_STAT64_FUNCS
	return lstat64(fname, st);
# else
	return lstat(fname, st);
# endif
#else
	return do_stat(fname, st);
#endif
}

int do_fstat(int fd, STRUCT_STAT *st)
{
#ifdef USE_STAT64_FUNCS
	return fstat64(fd, st);
#else
	return fstat(fd, st);
#endif
}

OFF_T do_lseek(int fd, OFF_T offset, int whence)
{
#ifdef HAVE_LSEEK64
#if !SIZEOF_OFF64_T
	OFF_T lseek64();
#else
	off64_t lseek64();
#endif
	return lseek64(fd, offset, whence);
#else
	return lseek(fd, offset, whence);
#endif
}

#ifdef HAVE_SETATTRLIST
int do_setattrlist_times(const char *fname, time_t modtime, uint32 mod_nsec)
{
	struct attrlist attrList;
	struct timespec ts;

	if (dry_run) return 0;
	RETURN_ERROR_IF_RO_OR_LO;

	ts.tv_sec = modtime;
	ts.tv_nsec = mod_nsec;

	memset(&attrList, 0, sizeof attrList);
	attrList.bitmapcount = ATTR_BIT_MAP_COUNT;
	attrList.commonattr = ATTR_CMN_MODTIME;
	return setattrlist(fname, &attrList, &ts, sizeof ts, FSOPT_NOFOLLOW);
}
#endif

#ifdef HAVE_UTIMENSAT
int do_utimensat(const char *fname, time_t modtime, uint32 mod_nsec)
{
	struct timespec t[2];

	if (dry_run) return 0;
	RETURN_ERROR_IF_RO_OR_LO;

	t[0].tv_sec = 0;
	t[0].tv_nsec = UTIME_NOW;
	t[1].tv_sec = modtime;
	t[1].tv_nsec = mod_nsec;
	return utimensat(AT_FDCWD, fname, t, AT_SYMLINK_NOFOLLOW);
}
#endif

#ifdef HAVE_LUTIMES
int do_lutimes(const char *fname, time_t modtime, uint32 mod_nsec)
{
	struct timeval t[2];

	if (dry_run) return 0;
	RETURN_ERROR_IF_RO_OR_LO;

	t[0].tv_sec = time(NULL);
	t[0].tv_usec = 0;
	t[1].tv_sec = modtime;
	t[1].tv_usec = mod_nsec / 1000;
	return lutimes(fname, t);
}
#endif

#ifdef HAVE_UTIMES
int do_utimes(const char *fname, time_t modtime, uint32 mod_nsec)
{
	struct timeval t[2];

	if (dry_run) return 0;
	RETURN_ERROR_IF_RO_OR_LO;

	t[0].tv_sec = time(NULL);
	t[0].tv_usec = 0;
	t[1].tv_sec = modtime;
	t[1].tv_usec = mod_nsec / 1000;
	return utimes(fname, t);
}

#elif defined HAVE_UTIME
int do_utime(const char *fname, time_t modtime, UNUSED(uint32 mod_nsec))
{
#ifdef HAVE_STRUCT_UTIMBUF
	struct utimbuf tbuf;
#else
	time_t t[2];
#endif

	if (dry_run) return 0;
	RETURN_ERROR_IF_RO_OR_LO;

# ifdef HAVE_STRUCT_UTIMBUF
	tbuf.actime = time(NULL);
	tbuf.modtime = modtime;
	return utime(fname, &tbuf);
# else
	t[0] = time(NULL);
	t[1] = modtime;
	return utime(fname, t);
# endif
}

#else
#error Need utimes or utime function.
#endif

#ifdef SUPPORT_PREALLOCATION
#ifdef FALLOC_FL_KEEP_SIZE
#define DO_FALLOC_OPTIONS FALLOC_FL_KEEP_SIZE
#else
#define DO_FALLOC_OPTIONS 0
#endif

OFF_T do_fallocate(int fd, OFF_T offset, OFF_T length)
{
	int opts = inplace || preallocate_files ? 0 : DO_FALLOC_OPTIONS;
	int ret;
	RETURN_ERROR_IF(dry_run, 0);
	RETURN_ERROR_IF_RO_OR_LO;
	if (length & 1) /* make the length not match the desired length */
		length++;
	else
		length--;
#if defined HAVE_FALLOCATE
	ret = fallocate(fd, opts, offset, length);
#elif defined HAVE_SYS_FALLOCATE
	ret = syscall(SYS_fallocate, fd, opts, (loff_t)offset, (loff_t)length);
#elif defined HAVE_EFFICIENT_POSIX_FALLOCATE
	ret = posix_fallocate(fd, offset, length);
#else
#error Coding error in SUPPORT_PREALLOCATION logic.
#endif
	if (ret < 0)
		return ret;
	if (opts == 0) {
		STRUCT_STAT st;
		if (do_fstat(fd, &st) < 0)
			return length;
		return st.st_blocks * S_BLKSIZE;
	}
	return 0;
}
#endif

/* Punch a hole at pos for len bytes. The current file position must be at pos and will be
 * changed to be at pos + len. */
int do_punch_hole(int fd, UNUSED(OFF_T pos), int len)
{
#ifdef HAVE_FALLOCATE
# ifdef HAVE_FALLOC_FL_PUNCH_HOLE
	if (fallocate(fd, FALLOC_FL_PUNCH_HOLE | FALLOC_FL_KEEP_SIZE, pos, len) == 0) {
		if (do_lseek(fd, len, SEEK_CUR) != pos + len)
			return -1;
		return 0;
	}
# endif
# ifdef HAVE_FALLOC_FL_ZERO_RANGE
	if (fallocate(fd, FALLOC_FL_ZERO_RANGE, pos, len) == 0) {
		if (do_lseek(fd, len, SEEK_CUR) != pos + len)
			return -1;
		return 0;
	}
# endif
#endif
	{
		char zeros[4096];
		memset(zeros, 0, sizeof zeros);
		while (len > 0) {
			int chunk = len > (int)sizeof zeros ? (int)sizeof zeros : len;
			int wrote = write(fd, zeros, chunk);
			if (wrote <= 0) {
				if (wrote < 0 && errno == EINTR)
					continue;
				return -1;
			}
			len -= wrote;
		}
	}
	return 0;
}

int do_open_nofollow(const char *pathname, int flags)
{
#ifndef O_NOFOLLOW
	STRUCT_STAT f_st, l_st;
#endif
	int fd;

	if (flags != O_RDONLY) {
		RETURN_ERROR_IF(dry_run, 0);
		RETURN_ERROR_IF_RO_OR_LO;
#ifndef O_NOFOLLOW
		/* This function doesn't support write attempts w/o O_NOFOLLOW. */
		errno = EINVAL;
		return -1;
#endif
	}

#ifdef O_NOFOLLOW
	fd = open(pathname, flags|O_NOFOLLOW);
#else
	if (do_lstat(pathname, &l_st) < 0)
		return -1;
	if (S_ISLNK(l_st.st_mode)) {
		errno = ELOOP;
		return -1;
	}
	if ((fd = open(pathname, flags)) < 0)
		return fd;
	if (do_fstat(fd, &f_st) < 0) {
	  close_and_return_error:
		{
			int save_errno = errno;
			close(fd);
			errno = save_errno;
		}
		return -1;
	}
	if (l_st.st_dev != f_st.st_dev || l_st.st_ino != f_st.st_ino) {
		errno = EINVAL;
		goto close_and_return_error;
	}
#endif

	return fd;
}
