/* The table of wrapper functions to populate */

/* This file is generated and should not be modified.  See the makewrappers
 * script if you want to modify this. */
typedef struct {
	char *name;		/* the name */
	int (**real)(void);	/* the underlying syscall */
	int (*wrapper)(void);	/* the wrapper from guts/name.c */
	char *version;		/* the version, if we know and care */
} pseudo_function;

static pseudo_function pseudo_functions[] = {
	{ /* int __fxstat(int ver, int fd, struct stat *buf); */
		"__fxstat",
		(int (**)(void)) &real___fxstat,
		(int (*)(void)) wrap___fxstat,
		NULL
	},
	{ /* int __fxstat64(int ver, int fd, struct stat64 *buf); */
		"__fxstat64",
		(int (**)(void)) &real___fxstat64,
		(int (*)(void)) wrap___fxstat64,
		NULL
	},
	{ /* int __fxstatat(int ver, int dirfd, const char *path, struct stat *buf, int flags); */
		"__fxstatat",
		(int (**)(void)) &real___fxstatat,
		(int (*)(void)) wrap___fxstatat,
		NULL
	},
	{ /* int __fxstatat64(int ver, int dirfd, const char *path, struct stat64 *buf, int flags); */
		"__fxstatat64",
		(int (**)(void)) &real___fxstatat64,
		(int (*)(void)) wrap___fxstatat64,
		NULL
	},
	{ /* int __lxstat(int ver, const char *path, struct stat *buf); */
		"__lxstat",
		(int (**)(void)) &real___lxstat,
		(int (*)(void)) wrap___lxstat,
		NULL
	},
	{ /* int __lxstat64(int ver, const char *path, struct stat64 *buf); */
		"__lxstat64",
		(int (**)(void)) &real___lxstat64,
		(int (*)(void)) wrap___lxstat64,
		NULL
	},
	{ /* int __openat64_2(int dirfd, const char *path, int flags); */
		"__openat64_2",
		(int (**)(void)) &real___openat64_2,
		(int (*)(void)) wrap___openat64_2,
		NULL
	},
	{ /* int __openat_2(int dirfd, const char *path, int flags); */
		"__openat_2",
		(int (**)(void)) &real___openat_2,
		(int (*)(void)) wrap___openat_2,
		NULL
	},
	{ /* int __xmknod(int ver, const char *path, mode_t mode, dev_t *dev); */
		"__xmknod",
		(int (**)(void)) &real___xmknod,
		(int (*)(void)) wrap___xmknod,
		NULL
	},
	{ /* int __xmknodat(int ver, int dirfd, const char *path, mode_t mode, dev_t *dev); */
		"__xmknodat",
		(int (**)(void)) &real___xmknodat,
		(int (*)(void)) wrap___xmknodat,
		NULL
	},
	{ /* int __xstat(int ver, const char *path, struct stat *buf); */
		"__xstat",
		(int (**)(void)) &real___xstat,
		(int (*)(void)) wrap___xstat,
		NULL
	},
	{ /* int __xstat64(int ver, const char *path, struct stat64 *buf); */
		"__xstat64",
		(int (**)(void)) &real___xstat64,
		(int (*)(void)) wrap___xstat64,
		NULL
	},
	{ /* int access(const char *path, int mode); */
		"access",
		(int (**)(void)) &real_access,
		(int (*)(void)) wrap_access,
		NULL
	},
	{ /* int acct(const char *path); */
		"acct",
		(int (**)(void)) &real_acct,
		(int (*)(void)) wrap_acct,
		NULL
	},
	{ /* int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen); */
		"bind",
		(int (**)(void)) &real_bind,
		(int (*)(void)) wrap_bind,
		NULL
	},
	{ /* char *canonicalize_file_name(const char *filename); */
		"canonicalize_file_name",
		(int (**)(void)) &real_canonicalize_file_name,
		(int (*)(void)) wrap_canonicalize_file_name,
		NULL
	},
	{ /* int capset(cap_user_header_t hdrp, const cap_user_data_t datap); */
		"capset",
		(int (**)(void)) &real_capset,
		(int (*)(void)) wrap_capset,
		NULL
	},
	{ /* int chdir(const char *path); */
		"chdir",
		(int (**)(void)) &real_chdir,
		(int (*)(void)) wrap_chdir,
		NULL
	},
	{ /* int chmod(const char *path, mode_t mode); */
		"chmod",
		(int (**)(void)) &real_chmod,
		(int (*)(void)) wrap_chmod,
		NULL
	},
	{ /* int chown(const char *path, uid_t owner, gid_t group); */
		"chown",
		(int (**)(void)) &real_chown,
		(int (*)(void)) wrap_chown,
		NULL
	},
	{ /* int chroot(const char *path); */
		"chroot",
		(int (**)(void)) &real_chroot,
		(int (*)(void)) wrap_chroot,
		NULL
	},
	{ /* int clone(int (*fn)(void *), void *child_stack, int flags, void *arg, va_list ap); */
		"clone",
		(int (**)(void)) &real_clone,
		(int (*)(void)) wrap_clone,
		NULL
	},
	{ /* int close(int fd); */
		"close",
		(int (**)(void)) &real_close,
		(int (*)(void)) wrap_close,
		NULL
	},
	{ /* int closedir(DIR *dirp); */
		"closedir",
		(int (**)(void)) &real_closedir,
		(int (*)(void)) wrap_closedir,
		NULL
	},
	{ /* int creat(const char *path, mode_t mode); */
		"creat",
		(int (**)(void)) &real_creat,
		(int (*)(void)) wrap_creat,
		NULL
	},
	{ /* int creat64(const char *path, mode_t mode); */
		"creat64",
		(int (**)(void)) &real_creat64,
		(int (*)(void)) wrap_creat64,
		NULL
	},
	{ /* int dup(int fd); */
		"dup",
		(int (**)(void)) &real_dup,
		(int (*)(void)) wrap_dup,
		NULL
	},
	{ /* int dup2(int oldfd, int newfd); */
		"dup2",
		(int (**)(void)) &real_dup2,
		(int (*)(void)) wrap_dup2,
		NULL
	},
	{ /* int eaccess(const char *path, int mode); */
		"eaccess",
		(int (**)(void)) &real_eaccess,
		(int (*)(void)) wrap_eaccess,
		NULL
	},
	{ /* void endgrent(void); */
		"endgrent",
		(int (**)(void)) &real_endgrent,
		(int (*)(void)) wrap_endgrent,
		NULL
	},
	{ /* void endpwent(void); */
		"endpwent",
		(int (**)(void)) &real_endpwent,
		(int (*)(void)) wrap_endpwent,
		NULL
	},
	{ /* int euidaccess(const char *path, int mode); */
		"euidaccess",
		(int (**)(void)) &real_euidaccess,
		(int (*)(void)) wrap_euidaccess,
		NULL
	},
	{ /* int execl(const char *file, const char *arg, va_list ap); */
		"execl",
		(int (**)(void)) &real_execl,
		(int (*)(void)) wrap_execl,
		NULL
	},
	{ /* int execle(const char *file, const char *arg, va_list ap); */
		"execle",
		(int (**)(void)) &real_execle,
		(int (*)(void)) wrap_execle,
		NULL
	},
	{ /* int execlp(const char *file, const char *arg, va_list ap); */
		"execlp",
		(int (**)(void)) &real_execlp,
		(int (*)(void)) wrap_execlp,
		NULL
	},
	{ /* int execv(const char *file, char *const *argv); */
		"execv",
		(int (**)(void)) &real_execv,
		(int (*)(void)) wrap_execv,
		NULL
	},
	{ /* int execve(const char *file, char *const *argv, char *const *envp); */
		"execve",
		(int (**)(void)) &real_execve,
		(int (*)(void)) wrap_execve,
		NULL
	},
	{ /* int execvp(const char *file, char *const *argv); */
		"execvp",
		(int (**)(void)) &real_execvp,
		(int (*)(void)) wrap_execvp,
		NULL
	},
	{ /* int fchdir(int dirfd); */
		"fchdir",
		(int (**)(void)) &real_fchdir,
		(int (*)(void)) wrap_fchdir,
		NULL
	},
	{ /* int fchmod(int fd, mode_t mode); */
		"fchmod",
		(int (**)(void)) &real_fchmod,
		(int (*)(void)) wrap_fchmod,
		NULL
	},
	{ /* int fchmodat(int dirfd, const char *path, mode_t mode, int flags); */
		"fchmodat",
		(int (**)(void)) &real_fchmodat,
		(int (*)(void)) wrap_fchmodat,
		NULL
	},
	{ /* int fchown(int fd, uid_t owner, gid_t group); */
		"fchown",
		(int (**)(void)) &real_fchown,
		(int (*)(void)) wrap_fchown,
		NULL
	},
	{ /* int fchownat(int dirfd, const char *path, uid_t owner, gid_t group, int flags); */
		"fchownat",
		(int (**)(void)) &real_fchownat,
		(int (*)(void)) wrap_fchownat,
		NULL
	},
	{ /* int fclose(FILE *fp); */
		"fclose",
		(int (**)(void)) &real_fclose,
		(int (*)(void)) wrap_fclose,
		NULL
	},
	{ /* int fcntl(int fd, int cmd, ... { struct flock *lock }); */
		"fcntl",
		(int (**)(void)) &real_fcntl,
		(int (*)(void)) wrap_fcntl,
		NULL
	},
	{ /* int fdatasync(int fd); */
		"fdatasync",
		(int (**)(void)) &real_fdatasync,
		(int (*)(void)) wrap_fdatasync,
		NULL
	},
	{ /* ssize_t fgetxattr(int filedes, const char *name, void *value, size_t size); */
		"fgetxattr",
		(int (**)(void)) &real_fgetxattr,
		(int (*)(void)) wrap_fgetxattr,
		NULL
	},
	{ /* ssize_t flistxattr(int filedes, char *list, size_t size); */
		"flistxattr",
		(int (**)(void)) &real_flistxattr,
		(int (*)(void)) wrap_flistxattr,
		NULL
	},
	{ /* FILE *fopen(const char *path, const char *mode); */
		"fopen",
		(int (**)(void)) &real_fopen,
		(int (*)(void)) wrap_fopen,
		NULL
	},
	{ /* FILE *fopen64(const char *path, const char *mode); */
		"fopen64",
		(int (**)(void)) &real_fopen64,
		(int (*)(void)) wrap_fopen64,
		NULL
	},
	{ /* int fork(void); */
		"fork",
		(int (**)(void)) &real_fork,
		(int (*)(void)) wrap_fork,
		NULL
	},
	{ /* int fremovexattr(int filedes, const char *name); */
		"fremovexattr",
		(int (**)(void)) &real_fremovexattr,
		(int (*)(void)) wrap_fremovexattr,
		NULL
	},
	{ /* FILE *freopen(const char *path, const char *mode, FILE *stream); */
		"freopen",
		(int (**)(void)) &real_freopen,
		(int (*)(void)) wrap_freopen,
		NULL
	},
	{ /* FILE *freopen64(const char *path, const char *mode, FILE *stream); */
		"freopen64",
		(int (**)(void)) &real_freopen64,
		(int (*)(void)) wrap_freopen64,
		NULL
	},
	{ /* int fsetxattr(int filedes, const char *name, const void *value, size_t size, int xflags); */
		"fsetxattr",
		(int (**)(void)) &real_fsetxattr,
		(int (*)(void)) wrap_fsetxattr,
		NULL
	},
	{ /* int fstat(int fd, struct stat *buf); */
		"fstat",
		(int (**)(void)) &real_fstat,
		(int (*)(void)) wrap_fstat,
		NULL
	},
	{ /* int fstat64(int fd, struct stat64 *buf); */
		"fstat64",
		(int (**)(void)) &real_fstat64,
		(int (*)(void)) wrap_fstat64,
		NULL
	},
	{ /* int fsync(int fd); */
		"fsync",
		(int (**)(void)) &real_fsync,
		(int (*)(void)) wrap_fsync,
		NULL
	},
	{ /* FTS *fts_open(char * const *path_argv, int options, int (*compar)(const FTSENT **, const FTSENT **)); */
		"fts_open",
		(int (**)(void)) &real_fts_open,
		(int (*)(void)) wrap_fts_open,
		NULL
	},
	{ /* int ftw(const char *path, int (*fn)(const char *, const struct stat *, int), int nopenfd); */
		"ftw",
		(int (**)(void)) &real_ftw,
		(int (*)(void)) wrap_ftw,
		NULL
	},
	{ /* int ftw64(const char *path, int (*fn)(const char *, const struct stat64 *, int), int nopenfd); */
		"ftw64",
		(int (**)(void)) &real_ftw64,
		(int (*)(void)) wrap_ftw64,
		NULL
	},
	{ /* char *get_current_dir_name(void); */
		"get_current_dir_name",
		(int (**)(void)) &real_get_current_dir_name,
		(int (*)(void)) wrap_get_current_dir_name,
		NULL
	},
	{ /* char *getcwd(char *buf, size_t size); */
		"getcwd",
		(int (**)(void)) &real_getcwd,
		(int (*)(void)) wrap_getcwd,
		NULL
	},
	{ /* gid_t getegid(void); */
		"getegid",
		(int (**)(void)) &real_getegid,
		(int (*)(void)) wrap_getegid,
		NULL
	},
	{ /* uid_t geteuid(void); */
		"geteuid",
		(int (**)(void)) &real_geteuid,
		(int (*)(void)) wrap_geteuid,
		NULL
	},
	{ /* gid_t getgid(void); */
		"getgid",
		(int (**)(void)) &real_getgid,
		(int (*)(void)) wrap_getgid,
		NULL
	},
	{ /* struct group *getgrent(void); */
		"getgrent",
		(int (**)(void)) &real_getgrent,
		(int (*)(void)) wrap_getgrent,
		NULL
	},
	{ /* int getgrent_r(struct group *gbuf, char *buf, size_t buflen, struct group **gbufp); */
		"getgrent_r",
		(int (**)(void)) &real_getgrent_r,
		(int (*)(void)) wrap_getgrent_r,
		NULL
	},
	{ /* struct group *getgrgid(gid_t gid); */
		"getgrgid",
		(int (**)(void)) &real_getgrgid,
		(int (*)(void)) wrap_getgrgid,
		NULL
	},
	{ /* int getgrgid_r(gid_t gid, struct group *gbuf, char *buf, size_t buflen, struct group **gbufp); */
		"getgrgid_r",
		(int (**)(void)) &real_getgrgid_r,
		(int (*)(void)) wrap_getgrgid_r,
		NULL
	},
	{ /* struct group *getgrnam(const char *name); */
		"getgrnam",
		(int (**)(void)) &real_getgrnam,
		(int (*)(void)) wrap_getgrnam,
		NULL
	},
	{ /* int getgrnam_r(const char *name, struct group *gbuf, char *buf, size_t buflen, struct group **gbufp); */
		"getgrnam_r",
		(int (**)(void)) &real_getgrnam_r,
		(int (*)(void)) wrap_getgrnam_r,
		NULL
	},
	{ /* int getgrouplist(const char *user, gid_t group, gid_t *groups, int *ngroups); */
		"getgrouplist",
		(int (**)(void)) &real_getgrouplist,
		(int (*)(void)) wrap_getgrouplist,
		NULL
	},
	{ /* int getgroups(int size, gid_t *list); */
		"getgroups",
		(int (**)(void)) &real_getgroups,
		(int (*)(void)) wrap_getgroups,
		NULL
	},
	{ /* int getpw(uid_t uid, char *buf); */
		"getpw",
		(int (**)(void)) &real_getpw,
		(int (*)(void)) wrap_getpw,
		NULL
	},
	{ /* struct passwd *getpwent(void); */
		"getpwent",
		(int (**)(void)) &real_getpwent,
		(int (*)(void)) wrap_getpwent,
		NULL
	},
	{ /* int getpwent_r(struct passwd *pwbuf, char *buf, size_t buflen, struct passwd **pwbufp); */
		"getpwent_r",
		(int (**)(void)) &real_getpwent_r,
		(int (*)(void)) wrap_getpwent_r,
		NULL
	},
	{ /* struct passwd *getpwnam(const char *name); */
		"getpwnam",
		(int (**)(void)) &real_getpwnam,
		(int (*)(void)) wrap_getpwnam,
		NULL
	},
	{ /* int getpwnam_r(const char *name, struct passwd *pwbuf, char *buf, size_t buflen, struct passwd **pwbufp); */
		"getpwnam_r",
		(int (**)(void)) &real_getpwnam_r,
		(int (*)(void)) wrap_getpwnam_r,
		NULL
	},
	{ /* struct passwd *getpwuid(uid_t uid); */
		"getpwuid",
		(int (**)(void)) &real_getpwuid,
		(int (*)(void)) wrap_getpwuid,
		NULL
	},
	{ /* int getpwuid_r(uid_t uid, struct passwd *pwbuf, char *buf, size_t buflen, struct passwd **pwbufp); */
		"getpwuid_r",
		(int (**)(void)) &real_getpwuid_r,
		(int (*)(void)) wrap_getpwuid_r,
		NULL
	},
	{ /* int getresgid(gid_t *rgid, gid_t *egid, gid_t *sgid); */
		"getresgid",
		(int (**)(void)) &real_getresgid,
		(int (*)(void)) wrap_getresgid,
		NULL
	},
	{ /* int getresuid(uid_t *ruid, uid_t *euid, uid_t *suid); */
		"getresuid",
		(int (**)(void)) &real_getresuid,
		(int (*)(void)) wrap_getresuid,
		NULL
	},
	{ /* uid_t getuid(void); */
		"getuid",
		(int (**)(void)) &real_getuid,
		(int (*)(void)) wrap_getuid,
		NULL
	},
	{ /* char *getwd(char *buf); */
		"getwd",
		(int (**)(void)) &real_getwd,
		(int (*)(void)) wrap_getwd,
		NULL
	},
	{ /* ssize_t getxattr(const char *path, const char *name, void *value, size_t size); */
		"getxattr",
		(int (**)(void)) &real_getxattr,
		(int (*)(void)) wrap_getxattr,
		NULL
	},
	{ /* int glob(const char *pattern, int flags, int (*errfunc)(const char *, int), glob_t *pglob); */
		"glob",
		(int (**)(void)) &real_glob,
		(int (*)(void)) wrap_glob,
		NULL
	},
	{ /* int glob64(const char *pattern, int flags, int (*errfunc)(const char *, int), glob64_t *pglob); */
		"glob64",
		(int (**)(void)) &real_glob64,
		(int (*)(void)) wrap_glob64,
		NULL
	},
	{ /* int lchown(const char *path, uid_t owner, gid_t group); */
		"lchown",
		(int (**)(void)) &real_lchown,
		(int (*)(void)) wrap_lchown,
		NULL
	},
	{ /* int lckpwdf(void); */
		"lckpwdf",
		(int (**)(void)) &real_lckpwdf,
		(int (*)(void)) wrap_lckpwdf,
		NULL
	},
	{ /* ssize_t lgetxattr(const char *path, const char *name, void *value, size_t size); */
		"lgetxattr",
		(int (**)(void)) &real_lgetxattr,
		(int (*)(void)) wrap_lgetxattr,
		NULL
	},
	{ /* int link(const char *oldname, const char *newname); */
		"link",
		(int (**)(void)) &real_link,
		(int (*)(void)) wrap_link,
		NULL
	},
	{ /* int linkat(int olddirfd, const char *oldname, int newdirfd, const char *newname, int flags); */
		"linkat",
		(int (**)(void)) &real_linkat,
		(int (*)(void)) wrap_linkat,
		NULL
	},
	{ /* ssize_t listxattr(const char *path, char *list, size_t size); */
		"listxattr",
		(int (**)(void)) &real_listxattr,
		(int (*)(void)) wrap_listxattr,
		NULL
	},
	{ /* ssize_t llistxattr(const char *path, char *list, size_t size); */
		"llistxattr",
		(int (**)(void)) &real_llistxattr,
		(int (*)(void)) wrap_llistxattr,
		NULL
	},
	{ /* int lremovexattr(const char *path, const char *name); */
		"lremovexattr",
		(int (**)(void)) &real_lremovexattr,
		(int (*)(void)) wrap_lremovexattr,
		NULL
	},
	{ /* int lsetxattr(const char *path, const char *name, const void *value, size_t size, int xflags); */
		"lsetxattr",
		(int (**)(void)) &real_lsetxattr,
		(int (*)(void)) wrap_lsetxattr,
		NULL
	},
	{ /* int lstat(const char *path, struct stat *buf); */
		"lstat",
		(int (**)(void)) &real_lstat,
		(int (*)(void)) wrap_lstat,
		NULL
	},
	{ /* int lstat64(const char *path, struct stat64 *buf); */
		"lstat64",
		(int (**)(void)) &real_lstat64,
		(int (*)(void)) wrap_lstat64,
		NULL
	},
	{ /* int lutimes(const char *path, const struct timeval *tv); */
		"lutimes",
		(int (**)(void)) &real_lutimes,
		(int (*)(void)) wrap_lutimes,
		NULL
	},
	{ /* int mkdir(const char *path, mode_t mode); */
		"mkdir",
		(int (**)(void)) &real_mkdir,
		(int (*)(void)) wrap_mkdir,
		NULL
	},
	{ /* int mkdirat(int dirfd, const char *path, mode_t mode); */
		"mkdirat",
		(int (**)(void)) &real_mkdirat,
		(int (*)(void)) wrap_mkdirat,
		NULL
	},
	{ /* char *mkdtemp(char *template); */
		"mkdtemp",
		(int (**)(void)) &real_mkdtemp,
		(int (*)(void)) wrap_mkdtemp,
		NULL
	},
	{ /* int mkfifo(const char *path, mode_t mode); */
		"mkfifo",
		(int (**)(void)) &real_mkfifo,
		(int (*)(void)) wrap_mkfifo,
		NULL
	},
	{ /* int mkfifoat(int dirfd, const char *path, mode_t mode); */
		"mkfifoat",
		(int (**)(void)) &real_mkfifoat,
		(int (*)(void)) wrap_mkfifoat,
		NULL
	},
	{ /* int mknod(const char *path, mode_t mode, dev_t dev); */
		"mknod",
		(int (**)(void)) &real_mknod,
		(int (*)(void)) wrap_mknod,
		NULL
	},
	{ /* int mknodat(int dirfd, const char *path, mode_t mode, dev_t dev); */
		"mknodat",
		(int (**)(void)) &real_mknodat,
		(int (*)(void)) wrap_mknodat,
		NULL
	},
	{ /* int mkostemp(char *template, int oflags); */
		"mkostemp",
		(int (**)(void)) &real_mkostemp,
		(int (*)(void)) wrap_mkostemp,
		NULL
	},
	{ /* int mkostemps(char *template, int suffixlen, int oflags); */
		"mkostemps",
		(int (**)(void)) &real_mkostemps,
		(int (*)(void)) wrap_mkostemps,
		NULL
	},
	{ /* int mkstemp(char *template); */
		"mkstemp",
		(int (**)(void)) &real_mkstemp,
		(int (*)(void)) wrap_mkstemp,
		NULL
	},
	{ /* int mkstemp64(char *template); */
		"mkstemp64",
		(int (**)(void)) &real_mkstemp64,
		(int (*)(void)) wrap_mkstemp64,
		NULL
	},
	{ /* int mkstemps(char *template, int suffixlen); */
		"mkstemps",
		(int (**)(void)) &real_mkstemps,
		(int (*)(void)) wrap_mkstemps,
		NULL
	},
	{ /* char *mktemp(char *template); */
		"mktemp",
		(int (**)(void)) &real_mktemp,
		(int (*)(void)) wrap_mktemp,
		NULL
	},
	{ /* int msync(void *addr, size_t length, int flags); */
		"msync",
		(int (**)(void)) &real_msync,
		(int (*)(void)) wrap_msync,
		NULL
	},
	{ /* int nftw(const char *path, int (*fn)(const char *, const struct stat *, int, struct FTW *), int nopenfd, int flag); */
		"nftw",
		(int (**)(void)) &real_nftw,
		(int (*)(void)) wrap_nftw,
		NULL
	},
	{ /* int nftw64(const char *path, int (*fn)(const char *, const struct stat64 *, int, struct FTW *), int nopenfd, int flag); */
		"nftw64",
		(int (**)(void)) &real_nftw64,
		(int (*)(void)) wrap_nftw64,
		NULL
	},
	{ /* int open(const char *path, int flags, ... { mode_t mode }); */
		"open",
		(int (**)(void)) &real_open,
		(int (*)(void)) wrap_open,
		NULL
	},
	{ /* int open64(const char *path, int flags, ... { mode_t mode }); */
		"open64",
		(int (**)(void)) &real_open64,
		(int (*)(void)) wrap_open64,
		NULL
	},
	{ /* int openat(int dirfd, const char *path, int flags, ... { mode_t mode }); */
		"openat",
		(int (**)(void)) &real_openat,
		(int (*)(void)) wrap_openat,
		NULL
	},
	{ /* int openat64(int dirfd, const char *path, int flags, ... { mode_t mode }); */
		"openat64",
		(int (**)(void)) &real_openat64,
		(int (*)(void)) wrap_openat64,
		NULL
	},
	{ /* DIR *opendir(const char *path); */
		"opendir",
		(int (**)(void)) &real_opendir,
		(int (*)(void)) wrap_opendir,
		NULL
	},
	{ /* long pathconf(const char *path, int name); */
		"pathconf",
		(int (**)(void)) &real_pathconf,
		(int (*)(void)) wrap_pathconf,
		NULL
	},
	{ /* FILE *popen(const char *command, const char *mode); */
		"popen",
		(int (**)(void)) &real_popen,
		(int (*)(void)) wrap_popen,
		NULL
	},
	{ /* ssize_t readlink(const char *path, char *buf, size_t bufsiz); */
		"readlink",
		(int (**)(void)) &real_readlink,
		(int (*)(void)) wrap_readlink,
		NULL
	},
	{ /* ssize_t readlinkat(int dirfd, const char *path, char *buf, size_t bufsiz); */
		"readlinkat",
		(int (**)(void)) &real_readlinkat,
		(int (*)(void)) wrap_readlinkat,
		NULL
	},
	{ /* char *realpath(const char *name, char *resolved_name); */
		"realpath",
		(int (**)(void)) &real_realpath,
		(int (*)(void)) wrap_realpath,
		"GLIBC_2.3"
	},
	{ /* int remove(const char *path); */
		"remove",
		(int (**)(void)) &real_remove,
		(int (*)(void)) wrap_remove,
		NULL
	},
	{ /* int removexattr(const char *path, const char *name); */
		"removexattr",
		(int (**)(void)) &real_removexattr,
		(int (*)(void)) wrap_removexattr,
		NULL
	},
	{ /* int rename(const char *oldpath, const char *newpath); */
		"rename",
		(int (**)(void)) &real_rename,
		(int (*)(void)) wrap_rename,
		NULL
	},
	{ /* int renameat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath); */
		"renameat",
		(int (**)(void)) &real_renameat,
		(int (*)(void)) wrap_renameat,
		NULL
	},
	{ /* int renameat2(int olddirfd, const char *oldpath, int newdirfd, const char *newpath, unsigned int flags); */
		"renameat2",
		(int (**)(void)) &real_renameat2,
		(int (*)(void)) wrap_renameat2,
		NULL
	},
	{ /* int rmdir(const char *path); */
		"rmdir",
		(int (**)(void)) &real_rmdir,
		(int (*)(void)) wrap_rmdir,
		NULL
	},
	{ /* int scandir(const char *path, struct dirent ***namelist, int (*filter)(const struct dirent *), int (*compar)()); */
		"scandir",
		(int (**)(void)) &real_scandir,
		(int (*)(void)) wrap_scandir,
		NULL
	},
	{ /* int scandir64(const char *path, struct dirent64 ***namelist, int (*filter)(const struct dirent64 *), int (*compar)()); */
		"scandir64",
		(int (**)(void)) &real_scandir64,
		(int (*)(void)) wrap_scandir64,
		NULL
	},
	{ /* int setegid(gid_t egid); */
		"setegid",
		(int (**)(void)) &real_setegid,
		(int (*)(void)) wrap_setegid,
		NULL
	},
	{ /* int seteuid(uid_t euid); */
		"seteuid",
		(int (**)(void)) &real_seteuid,
		(int (*)(void)) wrap_seteuid,
		NULL
	},
	{ /* int setfsgid(gid_t fsgid); */
		"setfsgid",
		(int (**)(void)) &real_setfsgid,
		(int (*)(void)) wrap_setfsgid,
		NULL
	},
	{ /* int setfsuid(uid_t fsuid); */
		"setfsuid",
		(int (**)(void)) &real_setfsuid,
		(int (*)(void)) wrap_setfsuid,
		NULL
	},
	{ /* int setgid(gid_t gid); */
		"setgid",
		(int (**)(void)) &real_setgid,
		(int (*)(void)) wrap_setgid,
		NULL
	},
	{ /* void setgrent(void); */
		"setgrent",
		(int (**)(void)) &real_setgrent,
		(int (*)(void)) wrap_setgrent,
		NULL
	},
	{ /* int setgroups(size_t size, const gid_t *list); */
		"setgroups",
		(int (**)(void)) &real_setgroups,
		(int (*)(void)) wrap_setgroups,
		NULL
	},
	{ /* void setpwent(void); */
		"setpwent",
		(int (**)(void)) &real_setpwent,
		(int (*)(void)) wrap_setpwent,
		NULL
	},
	{ /* int setregid(gid_t rgid, gid_t egid); */
		"setregid",
		(int (**)(void)) &real_setregid,
		(int (*)(void)) wrap_setregid,
		NULL
	},
	{ /* int setresgid(gid_t rgid, gid_t egid, gid_t sgid); */
		"setresgid",
		(int (**)(void)) &real_setresgid,
		(int (*)(void)) wrap_setresgid,
		NULL
	},
	{ /* int setresuid(uid_t ruid, uid_t euid, uid_t suid); */
		"setresuid",
		(int (**)(void)) &real_setresuid,
		(int (*)(void)) wrap_setresuid,
		NULL
	},
	{ /* int setreuid(uid_t ruid, uid_t euid); */
		"setreuid",
		(int (**)(void)) &real_setreuid,
		(int (*)(void)) wrap_setreuid,
		NULL
	},
	{ /* int setuid(uid_t uid); */
		"setuid",
		(int (**)(void)) &real_setuid,
		(int (*)(void)) wrap_setuid,
		NULL
	},
	{ /* int setxattr(const char *path, const char *name, const void *value, size_t size, int xflags); */
		"setxattr",
		(int (**)(void)) &real_setxattr,
		(int (*)(void)) wrap_setxattr,
		NULL
	},
	{ /* int stat(const char *path, struct stat *buf); */
		"stat",
		(int (**)(void)) &real_stat,
		(int (*)(void)) wrap_stat,
		NULL
	},
	{ /* int stat64(const char *path, struct stat64 *buf); */
		"stat64",
		(int (**)(void)) &real_stat64,
		(int (*)(void)) wrap_stat64,
		NULL
	},
	{ /* int statvfs(const char *path, struct statvfs *buf); */
		"statvfs",
		(int (**)(void)) &real_statvfs,
		(int (*)(void)) wrap_statvfs,
		NULL
	},
	{ /* int statx(int dirfd, const char *pathname, int flags, unsigned int mask, struct statx *statxbuf); */
		"statx",
		(int (**)(void)) &real_statx,
		(int (*)(void)) wrap_statx,
		NULL
	},
	{ /* int symlink(const char *oldname, const char *newpath); */
		"symlink",
		(int (**)(void)) &real_symlink,
		(int (*)(void)) wrap_symlink,
		NULL
	},
	{ /* int symlinkat(const char *oldname, int dirfd, const char *newpath); */
		"symlinkat",
		(int (**)(void)) &real_symlinkat,
		(int (*)(void)) wrap_symlinkat,
		NULL
	},
	{ /* void sync(void); */
		"sync",
		(int (**)(void)) &real_sync,
		(int (*)(void)) wrap_sync,
		NULL
	},
	{ /* int sync_file_range(int fd, off64_t offset, off64_t nbytes, unsigned int flags); */
		"sync_file_range",
		(int (**)(void)) &real_sync_file_range,
		(int (*)(void)) wrap_sync_file_range,
		NULL
	},
	{ /* int syncfs(int fd); */
		"syncfs",
		(int (**)(void)) &real_syncfs,
		(int (*)(void)) wrap_syncfs,
		NULL
	},
	{ /* long syscall(long nr, va_list ap); */
		"syscall",
		(int (**)(void)) &real_syscall,
		(int (*)(void)) wrap_syscall,
		NULL
	},
	{ /* int system(const char *command); */
		"system",
		(int (**)(void)) &real_system,
		(int (*)(void)) wrap_system,
		NULL
	},
	{ /* char *tempnam(const char *template, const char *pfx); */
		"tempnam",
		(int (**)(void)) &real_tempnam,
		(int (*)(void)) wrap_tempnam,
		NULL
	},
	{ /* char *tmpnam(char *s); */
		"tmpnam",
		(int (**)(void)) &real_tmpnam,
		(int (*)(void)) wrap_tmpnam,
		NULL
	},
	{ /* int truncate(const char *path, off_t length); */
		"truncate",
		(int (**)(void)) &real_truncate,
		(int (*)(void)) wrap_truncate,
		NULL
	},
	{ /* int truncate64(const char *path, off64_t length); */
		"truncate64",
		(int (**)(void)) &real_truncate64,
		(int (*)(void)) wrap_truncate64,
		NULL
	},
	{ /* int ulckpwdf(void); */
		"ulckpwdf",
		(int (**)(void)) &real_ulckpwdf,
		(int (*)(void)) wrap_ulckpwdf,
		NULL
	},
	{ /* mode_t umask(mode_t mask); */
		"umask",
		(int (**)(void)) &real_umask,
		(int (*)(void)) wrap_umask,
		NULL
	},
	{ /* int unlink(const char *path); */
		"unlink",
		(int (**)(void)) &real_unlink,
		(int (*)(void)) wrap_unlink,
		NULL
	},
	{ /* int unlinkat(int dirfd, const char *path, int rflags); */
		"unlinkat",
		(int (**)(void)) &real_unlinkat,
		(int (*)(void)) wrap_unlinkat,
		NULL
	},
	{ /* int utime(const char *path, const struct utimbuf *buf); */
		"utime",
		(int (**)(void)) &real_utime,
		(int (*)(void)) wrap_utime,
		NULL
	},
	{ /* int utimes(const char *path, const struct timeval *times); */
		"utimes",
		(int (**)(void)) &real_utimes,
		(int (*)(void)) wrap_utimes,
		NULL
	},
	{ NULL, NULL, NULL, NULL },
};
