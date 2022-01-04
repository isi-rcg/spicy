/*
 * pseudo_client.h, shared declarations for client
 *
 * Copyright (c) 2008-2010 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */
extern pseudo_msg_t *pseudo_client_op(pseudo_op_t op, int access, int fd, int dirfd, const char *path, const PSEUDO_STATBUF *buf, ...);
#if PSEUDO_STATBUF_64
#define base_lstat real_lstat64
#define base_fstat real_fstat64
#define base_stat real_stat64
#define base_fstatat(dirfd, path, buf, flags) real___fxstatat64(_STAT_VER, dirfd, path, buf, flags)
#else
#define base_lstat real_lstat
#define base_fstat real_fstat
#define base_stat real_stat
#define base_fstatat(dirfd, path, buf, flags) real___fxstatat(_STAT_VER, dirfd, path, buf, flags)
#endif
extern void pseudo_antimagic(void);
extern void pseudo_magic(void);
extern void pseudo_client_touchuid(void);
extern void pseudo_client_touchgid(void);
extern char *pseudo_client_fdpath(int fd);
extern int pseudo_client_shutdown(int);
extern int pseudo_fd(int fd, int how);
#define MOVE_FD	0
#define COPY_FD	1
#define PSEUDO_MIN_FD	20
extern uid_t pseudo_euid;
extern uid_t pseudo_fuid;
extern uid_t pseudo_suid;
extern uid_t pseudo_ruid;
extern gid_t pseudo_egid;
extern gid_t pseudo_sgid;
extern gid_t pseudo_rgid;
extern gid_t pseudo_fgid;
extern int pseudo_prefix_dir_fd;
extern int pseudo_localstate_dir_fd;
extern FILE *pseudo_pwd_open(void);
extern FILE *pseudo_grp_open(void);
extern void pseudo_pwd_close(void);
extern void pseudo_grp_close(void);
extern int pseudo_pwd_lck_open(void);
extern int pseudo_pwd_lck_close(void);
extern FILE *pseudo_pwd;
extern FILE *pseudo_grp;

/* pseudo_wrappers will try to initialize these */
extern int (*pseudo_real_lstat)(const char *path, PSEUDO_STATBUF *buf);
extern int (*pseudo_real_unsetenv)(const char *);
extern char * (*pseudo_real_getenv)(const char *);
extern int (*pseudo_real_setenv)(const char *, const char *, int);
extern int (*pseudo_real_fork)(void);
extern int (*pseudo_real_execv)(const char *, char * const *);

/* support related to chroot/getcwd/etc. */
extern int pseudo_client_getcwd(void);
extern int pseudo_client_chroot(const char *);
extern char *pseudo_root_path(const char *, int, int, const char *, int);
extern const char *pseudo_exec_path(const char *filename, int);
#define PSEUDO_ROOT_PATH(x, y, z) pseudo_root_path(__func__, __LINE__, (x), (y), (z));
extern char *pseudo_cwd;
extern size_t pseudo_cwd_len;
extern char *pseudo_cwd_rel;
extern char *pseudo_chroot;
extern char *pseudo_passwd;
extern size_t pseudo_chroot_len;
extern int pseudo_nosymlinkexp;

extern int pseudo_umask;

/* Root can read and write files, and enter directories which have no
 * read, write, or execute permissions.  (But can't execute files without
 * execute permissions!)
 *
 * A non-root user can't.
 *
 * When doing anything which actually writes to the filesystem, we add in
 * the user read/write/execute bits.  When storing to the database, though,
 * we mask out any such bits which weren't in the original mode.
 */
#define PSEUDO_FS_MODE(mode, isdir) (((mode) | S_IRUSR | S_IWUSR | ((isdir) ? S_IXUSR : 0)) & ~(S_IWGRP | S_IWOTH))
#define PSEUDO_DB_MODE(fs_mode, user_mode) (((fs_mode) & ~0722) | ((user_mode & 0722)))

