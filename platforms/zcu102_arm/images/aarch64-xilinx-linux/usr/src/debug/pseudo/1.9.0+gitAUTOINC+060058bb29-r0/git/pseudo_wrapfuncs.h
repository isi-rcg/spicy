/* wrapper functions. generated automatically. */

/* This file is generated and should not be modified.  See the makewrappers
 * script if you want to modify this. */
/* int __fxstat(int ver, int fd, struct stat *buf) */
static int wrap___fxstat(int ver, int fd, struct stat *buf);
static int (*real___fxstat)(int ver, int fd, struct stat *buf);

/* int __fxstat64(int ver, int fd, struct stat64 *buf) */
static int wrap___fxstat64(int ver, int fd, struct stat64 *buf);
static int (*real___fxstat64)(int ver, int fd, struct stat64 *buf);

/* int __fxstatat(int ver, int dirfd, const char *path, struct stat *buf, int flags) */
static int wrap___fxstatat(int ver, int dirfd, const char *path, struct stat *buf, int flags);
static int (*real___fxstatat)(int ver, int dirfd, const char *path, struct stat *buf, int flags);

/* int __fxstatat64(int ver, int dirfd, const char *path, struct stat64 *buf, int flags) */
static int wrap___fxstatat64(int ver, int dirfd, const char *path, struct stat64 *buf, int flags);
static int (*real___fxstatat64)(int ver, int dirfd, const char *path, struct stat64 *buf, int flags);

/* int __lxstat(int ver, const char *path, struct stat *buf) */
static int wrap___lxstat(int ver, const char *path, struct stat *buf);
static int (*real___lxstat)(int ver, const char *path, struct stat *buf);

/* int __lxstat64(int ver, const char *path, struct stat64 *buf) */
static int wrap___lxstat64(int ver, const char *path, struct stat64 *buf);
static int (*real___lxstat64)(int ver, const char *path, struct stat64 *buf);

/* int __openat64_2(int dirfd, const char *path, int flags) */
static int wrap___openat64_2(int dirfd, const char *path, int flags);
static int (*real___openat64_2)(int dirfd, const char *path, int flags);

/* int __openat_2(int dirfd, const char *path, int flags) */
static int wrap___openat_2(int dirfd, const char *path, int flags);
static int (*real___openat_2)(int dirfd, const char *path, int flags);

/* int __xmknod(int ver, const char *path, mode_t mode, dev_t *dev) */
static int wrap___xmknod(int ver, const char *path, mode_t mode, dev_t *dev);
static int (*real___xmknod)(int ver, const char *path, mode_t mode, dev_t *dev);

/* int __xmknodat(int ver, int dirfd, const char *path, mode_t mode, dev_t *dev) */
static int wrap___xmknodat(int ver, int dirfd, const char *path, mode_t mode, dev_t *dev);
static int (*real___xmknodat)(int ver, int dirfd, const char *path, mode_t mode, dev_t *dev);

/* int __xstat(int ver, const char *path, struct stat *buf) */
static int wrap___xstat(int ver, const char *path, struct stat *buf);
static int (*real___xstat)(int ver, const char *path, struct stat *buf);

/* int __xstat64(int ver, const char *path, struct stat64 *buf) */
static int wrap___xstat64(int ver, const char *path, struct stat64 *buf);
static int (*real___xstat64)(int ver, const char *path, struct stat64 *buf);

/* int access(const char *path, int mode) */
static int wrap_access(const char *path, int mode);
static int (*real_access)(const char *path, int mode);

/* int acct(const char *path) */
static int wrap_acct(const char *path);
static int (*real_acct)(const char *path);

/* int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) */
static int wrap_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
static int (*real_bind)(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

/* char *canonicalize_file_name(const char *filename) */
static char * wrap_canonicalize_file_name(const char *filename);
static char * (*real_canonicalize_file_name)(const char *filename);

/* int capset(cap_user_header_t hdrp, const cap_user_data_t datap) */
static int wrap_capset(cap_user_header_t hdrp, const cap_user_data_t datap);
static int (*real_capset)(cap_user_header_t hdrp, const cap_user_data_t datap);
int pseudo_capset(cap_user_header_t hdrp, const cap_user_data_t datap);
/* int chdir(const char *path) */
static int wrap_chdir(const char *path);
static int (*real_chdir)(const char *path);

/* int chmod(const char *path, mode_t mode) */
static int wrap_chmod(const char *path, mode_t mode);
static int (*real_chmod)(const char *path, mode_t mode);

/* int chown(const char *path, uid_t owner, gid_t group) */
static int wrap_chown(const char *path, uid_t owner, gid_t group);
static int (*real_chown)(const char *path, uid_t owner, gid_t group);

/* int chroot(const char *path) */
static int wrap_chroot(const char *path);
static int (*real_chroot)(const char *path);

/* int clone(int (*fn)(void *), void *child_stack, int flags, void *arg, va_list ap) */
static int wrap_clone(int (*fn)(void *), void *child_stack, int flags, void *arg, va_list ap);
static int (*real_clone)(int (*fn)(void *), void *child_stack, int flags, void *arg, ...);

/* int close(int fd) */
static int wrap_close(int fd);
static int (*real_close)(int fd);

/* int closedir(DIR *dirp) */
static int wrap_closedir(DIR *dirp);
static int (*real_closedir)(DIR *dirp);

/* int creat(const char *path, mode_t mode) */
static int wrap_creat(const char *path, mode_t mode);
static int (*real_creat)(const char *path, mode_t mode);

/* int creat64(const char *path, mode_t mode) */
static int wrap_creat64(const char *path, mode_t mode);
static int (*real_creat64)(const char *path, mode_t mode);

/* int dup(int fd) */
static int wrap_dup(int fd);
static int (*real_dup)(int fd);

/* int dup2(int oldfd, int newfd) */
static int wrap_dup2(int oldfd, int newfd);
static int (*real_dup2)(int oldfd, int newfd);

/* int eaccess(const char *path, int mode) */
static int wrap_eaccess(const char *path, int mode);
static int (*real_eaccess)(const char *path, int mode);

/* void endgrent(void) */
static void wrap_endgrent(void);
static void (*real_endgrent)(void);

/* void endpwent(void) */
static void wrap_endpwent(void);
static void (*real_endpwent)(void);

/* int euidaccess(const char *path, int mode) */
static int wrap_euidaccess(const char *path, int mode);
static int (*real_euidaccess)(const char *path, int mode);

/* int execl(const char *file, const char *arg, va_list ap) */
static int wrap_execl(const char *file, const char *arg, va_list ap);
static int (*real_execl)(const char *file, const char *arg, ...);

/* int execle(const char *file, const char *arg, va_list ap) */
static int wrap_execle(const char *file, const char *arg, va_list ap);
static int (*real_execle)(const char *file, const char *arg, ...);

/* int execlp(const char *file, const char *arg, va_list ap) */
static int wrap_execlp(const char *file, const char *arg, va_list ap);
static int (*real_execlp)(const char *file, const char *arg, ...);

/* int execv(const char *file, char *const *argv) */
static int wrap_execv(const char *file, char *const *argv);
static int (*real_execv)(const char *file, char *const *argv);

/* int execve(const char *file, char *const *argv, char *const *envp) */
static int wrap_execve(const char *file, char *const *argv, char *const *envp);
static int (*real_execve)(const char *file, char *const *argv, char *const *envp);

/* int execvp(const char *file, char *const *argv) */
static int wrap_execvp(const char *file, char *const *argv);
static int (*real_execvp)(const char *file, char *const *argv);

/* int fchdir(int dirfd) */
static int wrap_fchdir(int dirfd);
static int (*real_fchdir)(int dirfd);

/* int fchmod(int fd, mode_t mode) */
static int wrap_fchmod(int fd, mode_t mode);
static int (*real_fchmod)(int fd, mode_t mode);

/* int fchmodat(int dirfd, const char *path, mode_t mode, int flags) */
static int wrap_fchmodat(int dirfd, const char *path, mode_t mode, int flags);
static int (*real_fchmodat)(int dirfd, const char *path, mode_t mode, int flags);

/* int fchown(int fd, uid_t owner, gid_t group) */
static int wrap_fchown(int fd, uid_t owner, gid_t group);
static int (*real_fchown)(int fd, uid_t owner, gid_t group);

/* int fchownat(int dirfd, const char *path, uid_t owner, gid_t group, int flags) */
static int wrap_fchownat(int dirfd, const char *path, uid_t owner, gid_t group, int flags);
static int (*real_fchownat)(int dirfd, const char *path, uid_t owner, gid_t group, int flags);

/* int fclose(FILE *fp) */
static int wrap_fclose(FILE *fp);
static int (*real_fclose)(FILE *fp);

/* int fcntl(int fd, int cmd, ... { struct flock *lock }) */
static int wrap_fcntl(int fd, int cmd, ... /* struct flock *lock */);
static int (*real_fcntl)(int fd, int cmd, ... /* struct flock *lock */);

/* int fdatasync(int fd) */
static int wrap_fdatasync(int fd);
static int (*real_fdatasync)(int fd);

/* ssize_t fgetxattr(int filedes, const char *name, void *value, size_t size) */
static ssize_t wrap_fgetxattr(int filedes, const char *name, void *value, size_t size);
static ssize_t (*real_fgetxattr)(int filedes, const char *name, void *value, size_t size);

/* ssize_t flistxattr(int filedes, char *list, size_t size) */
static ssize_t wrap_flistxattr(int filedes, char *list, size_t size);
static ssize_t (*real_flistxattr)(int filedes, char *list, size_t size);

/* FILE *fopen(const char *path, const char *mode) */
static FILE * wrap_fopen(const char *path, const char *mode);
static FILE * (*real_fopen)(const char *path, const char *mode);

/* FILE *fopen64(const char *path, const char *mode) */
static FILE * wrap_fopen64(const char *path, const char *mode);
static FILE * (*real_fopen64)(const char *path, const char *mode);

/* int fork(void) */
static int wrap_fork(void);
static int (*real_fork)(void);

/* int fremovexattr(int filedes, const char *name) */
static int wrap_fremovexattr(int filedes, const char *name);
static int (*real_fremovexattr)(int filedes, const char *name);

/* FILE *freopen(const char *path, const char *mode, FILE *stream) */
static FILE * wrap_freopen(const char *path, const char *mode, FILE *stream);
static FILE * (*real_freopen)(const char *path, const char *mode, FILE *stream);

/* FILE *freopen64(const char *path, const char *mode, FILE *stream) */
static FILE * wrap_freopen64(const char *path, const char *mode, FILE *stream);
static FILE * (*real_freopen64)(const char *path, const char *mode, FILE *stream);

/* int fsetxattr(int filedes, const char *name, const void *value, size_t size, int xflags) */
static int wrap_fsetxattr(int filedes, const char *name, const void *value, size_t size, int xflags);
static int (*real_fsetxattr)(int filedes, const char *name, const void *value, size_t size, int xflags);

/* int fstat(int fd, struct stat *buf) */
static int wrap_fstat(int fd, struct stat *buf);
static int (*real_fstat)(int fd, struct stat *buf);
int pseudo_fstat(int fd, struct stat *buf);
/* int fstat64(int fd, struct stat64 *buf) */
static int wrap_fstat64(int fd, struct stat64 *buf);
static int (*real_fstat64)(int fd, struct stat64 *buf);
int pseudo_fstat64(int fd, struct stat64 *buf);
/* int fsync(int fd) */
static int wrap_fsync(int fd);
static int (*real_fsync)(int fd);

/* FTS *fts_open(char * const *path_argv, int options, int (*compar)(const FTSENT **, const FTSENT **)) */
static FTS * wrap_fts_open(char * const *path_argv, int options, int (*compar)(const FTSENT **, const FTSENT **));
static FTS * (*real_fts_open)(char * const *path_argv, int options, int (*compar)(const FTSENT **, const FTSENT **));

/* int ftw(const char *path, int (*fn)(const char *, const struct stat *, int), int nopenfd) */
static int wrap_ftw(const char *path, int (*fn)(const char *, const struct stat *, int), int nopenfd);
static int (*real_ftw)(const char *path, int (*fn)(const char *, const struct stat *, int), int nopenfd);

/* int ftw64(const char *path, int (*fn)(const char *, const struct stat64 *, int), int nopenfd) */
static int wrap_ftw64(const char *path, int (*fn)(const char *, const struct stat64 *, int), int nopenfd);
static int (*real_ftw64)(const char *path, int (*fn)(const char *, const struct stat64 *, int), int nopenfd);

/* char *get_current_dir_name(void) */
static char * wrap_get_current_dir_name(void);
static char * (*real_get_current_dir_name)(void);

/* char *getcwd(char *buf, size_t size) */
static char * wrap_getcwd(char *buf, size_t size);
static char * (*real_getcwd)(char *buf, size_t size);

/* gid_t getegid(void) */
static gid_t wrap_getegid(void);
static gid_t (*real_getegid)(void);

/* uid_t geteuid(void) */
static uid_t wrap_geteuid(void);
static uid_t (*real_geteuid)(void);

/* gid_t getgid(void) */
static gid_t wrap_getgid(void);
static gid_t (*real_getgid)(void);

/* struct group *getgrent(void) */
static struct group * wrap_getgrent(void);
static struct group * (*real_getgrent)(void);

/* int getgrent_r(struct group *gbuf, char *buf, size_t buflen, struct group **gbufp) */
static int wrap_getgrent_r(struct group *gbuf, char *buf, size_t buflen, struct group **gbufp);
static int (*real_getgrent_r)(struct group *gbuf, char *buf, size_t buflen, struct group **gbufp);

/* struct group *getgrgid(gid_t gid) */
static struct group * wrap_getgrgid(gid_t gid);
static struct group * (*real_getgrgid)(gid_t gid);

/* int getgrgid_r(gid_t gid, struct group *gbuf, char *buf, size_t buflen, struct group **gbufp) */
static int wrap_getgrgid_r(gid_t gid, struct group *gbuf, char *buf, size_t buflen, struct group **gbufp);
static int (*real_getgrgid_r)(gid_t gid, struct group *gbuf, char *buf, size_t buflen, struct group **gbufp);

/* struct group *getgrnam(const char *name) */
static struct group * wrap_getgrnam(const char *name);
static struct group * (*real_getgrnam)(const char *name);

/* int getgrnam_r(const char *name, struct group *gbuf, char *buf, size_t buflen, struct group **gbufp) */
static int wrap_getgrnam_r(const char *name, struct group *gbuf, char *buf, size_t buflen, struct group **gbufp);
static int (*real_getgrnam_r)(const char *name, struct group *gbuf, char *buf, size_t buflen, struct group **gbufp);

/* int getgrouplist(const char *user, gid_t group, gid_t *groups, int *ngroups) */
static int wrap_getgrouplist(const char *user, gid_t group, gid_t *groups, int *ngroups);
static int (*real_getgrouplist)(const char *user, gid_t group, gid_t *groups, int *ngroups);

/* int getgroups(int size, gid_t *list) */
static int wrap_getgroups(int size, gid_t *list);
static int (*real_getgroups)(int size, gid_t *list);

/* int getpw(uid_t uid, char *buf) */
static int wrap_getpw(uid_t uid, char *buf);
static int (*real_getpw)(uid_t uid, char *buf);

/* struct passwd *getpwent(void) */
static struct passwd * wrap_getpwent(void);
static struct passwd * (*real_getpwent)(void);

/* int getpwent_r(struct passwd *pwbuf, char *buf, size_t buflen, struct passwd **pwbufp) */
static int wrap_getpwent_r(struct passwd *pwbuf, char *buf, size_t buflen, struct passwd **pwbufp);
static int (*real_getpwent_r)(struct passwd *pwbuf, char *buf, size_t buflen, struct passwd **pwbufp);

/* struct passwd *getpwnam(const char *name) */
static struct passwd * wrap_getpwnam(const char *name);
static struct passwd * (*real_getpwnam)(const char *name);

/* int getpwnam_r(const char *name, struct passwd *pwbuf, char *buf, size_t buflen, struct passwd **pwbufp) */
static int wrap_getpwnam_r(const char *name, struct passwd *pwbuf, char *buf, size_t buflen, struct passwd **pwbufp);
static int (*real_getpwnam_r)(const char *name, struct passwd *pwbuf, char *buf, size_t buflen, struct passwd **pwbufp);

/* struct passwd *getpwuid(uid_t uid) */
static struct passwd * wrap_getpwuid(uid_t uid);
static struct passwd * (*real_getpwuid)(uid_t uid);

/* int getpwuid_r(uid_t uid, struct passwd *pwbuf, char *buf, size_t buflen, struct passwd **pwbufp) */
static int wrap_getpwuid_r(uid_t uid, struct passwd *pwbuf, char *buf, size_t buflen, struct passwd **pwbufp);
static int (*real_getpwuid_r)(uid_t uid, struct passwd *pwbuf, char *buf, size_t buflen, struct passwd **pwbufp);

/* int getresgid(gid_t *rgid, gid_t *egid, gid_t *sgid) */
static int wrap_getresgid(gid_t *rgid, gid_t *egid, gid_t *sgid);
static int (*real_getresgid)(gid_t *rgid, gid_t *egid, gid_t *sgid);

/* int getresuid(uid_t *ruid, uid_t *euid, uid_t *suid) */
static int wrap_getresuid(uid_t *ruid, uid_t *euid, uid_t *suid);
static int (*real_getresuid)(uid_t *ruid, uid_t *euid, uid_t *suid);

/* uid_t getuid(void) */
static uid_t wrap_getuid(void);
static uid_t (*real_getuid)(void);

/* char *getwd(char *buf) */
static char * wrap_getwd(char *buf);
static char * (*real_getwd)(char *buf);

/* ssize_t getxattr(const char *path, const char *name, void *value, size_t size) */
static ssize_t wrap_getxattr(const char *path, const char *name, void *value, size_t size);
static ssize_t (*real_getxattr)(const char *path, const char *name, void *value, size_t size);

/* int glob(const char *pattern, int flags, int (*errfunc)(const char *, int), glob_t *pglob) */
static int wrap_glob(const char *pattern, int flags, int (*errfunc)(const char *, int), glob_t *pglob);
static int (*real_glob)(const char *pattern, int flags, int (*errfunc)(const char *, int), glob_t *pglob);

/* int glob64(const char *pattern, int flags, int (*errfunc)(const char *, int), glob64_t *pglob) */
static int wrap_glob64(const char *pattern, int flags, int (*errfunc)(const char *, int), glob64_t *pglob);
static int (*real_glob64)(const char *pattern, int flags, int (*errfunc)(const char *, int), glob64_t *pglob);

/* int lchown(const char *path, uid_t owner, gid_t group) */
static int wrap_lchown(const char *path, uid_t owner, gid_t group);
static int (*real_lchown)(const char *path, uid_t owner, gid_t group);

/* int lckpwdf(void) */
static int wrap_lckpwdf(void);
static int (*real_lckpwdf)(void);

/* ssize_t lgetxattr(const char *path, const char *name, void *value, size_t size) */
static ssize_t wrap_lgetxattr(const char *path, const char *name, void *value, size_t size);
static ssize_t (*real_lgetxattr)(const char *path, const char *name, void *value, size_t size);

/* int link(const char *oldname, const char *newname) */
static int wrap_link(const char *oldname, const char *newname);
static int (*real_link)(const char *oldname, const char *newname);

/* int linkat(int olddirfd, const char *oldname, int newdirfd, const char *newname, int flags) */
static int wrap_linkat(int olddirfd, const char *oldname, int newdirfd, const char *newname, int flags);
static int (*real_linkat)(int olddirfd, const char *oldname, int newdirfd, const char *newname, int flags);

/* ssize_t listxattr(const char *path, char *list, size_t size) */
static ssize_t wrap_listxattr(const char *path, char *list, size_t size);
static ssize_t (*real_listxattr)(const char *path, char *list, size_t size);

/* ssize_t llistxattr(const char *path, char *list, size_t size) */
static ssize_t wrap_llistxattr(const char *path, char *list, size_t size);
static ssize_t (*real_llistxattr)(const char *path, char *list, size_t size);

/* int lremovexattr(const char *path, const char *name) */
static int wrap_lremovexattr(const char *path, const char *name);
static int (*real_lremovexattr)(const char *path, const char *name);

/* int lsetxattr(const char *path, const char *name, const void *value, size_t size, int xflags) */
static int wrap_lsetxattr(const char *path, const char *name, const void *value, size_t size, int xflags);
static int (*real_lsetxattr)(const char *path, const char *name, const void *value, size_t size, int xflags);

/* int lstat(const char *path, struct stat *buf) */
static int wrap_lstat(const char *path, struct stat *buf);
static int (*real_lstat)(const char *path, struct stat *buf);
int pseudo_lstat(const char *path, struct stat *buf);
/* int lstat64(const char *path, struct stat64 *buf) */
static int wrap_lstat64(const char *path, struct stat64 *buf);
static int (*real_lstat64)(const char *path, struct stat64 *buf);
int pseudo_lstat64(const char *path, struct stat64 *buf);
/* int lutimes(const char *path, const struct timeval *tv) */
static int wrap_lutimes(const char *path, const struct timeval *tv);
static int (*real_lutimes)(const char *path, const struct timeval *tv);

/* int mkdir(const char *path, mode_t mode) */
static int wrap_mkdir(const char *path, mode_t mode);
static int (*real_mkdir)(const char *path, mode_t mode);

/* int mkdirat(int dirfd, const char *path, mode_t mode) */
static int wrap_mkdirat(int dirfd, const char *path, mode_t mode);
static int (*real_mkdirat)(int dirfd, const char *path, mode_t mode);

/* char *mkdtemp(char *template) */
static char * wrap_mkdtemp(char *template);
static char * (*real_mkdtemp)(char *template);

/* int mkfifo(const char *path, mode_t mode) */
static int wrap_mkfifo(const char *path, mode_t mode);
static int (*real_mkfifo)(const char *path, mode_t mode);

/* int mkfifoat(int dirfd, const char *path, mode_t mode) */
static int wrap_mkfifoat(int dirfd, const char *path, mode_t mode);
static int (*real_mkfifoat)(int dirfd, const char *path, mode_t mode);

/* int mknod(const char *path, mode_t mode, dev_t dev) */
static int wrap_mknod(const char *path, mode_t mode, dev_t dev);
static int (*real_mknod)(const char *path, mode_t mode, dev_t dev);
int pseudo_mknod(const char *path, mode_t mode, dev_t dev);
/* int mknodat(int dirfd, const char *path, mode_t mode, dev_t dev) */
static int wrap_mknodat(int dirfd, const char *path, mode_t mode, dev_t dev);
static int (*real_mknodat)(int dirfd, const char *path, mode_t mode, dev_t dev);
int pseudo_mknodat(int dirfd, const char *path, mode_t mode, dev_t dev);
/* int mkostemp(char *template, int oflags) */
static int wrap_mkostemp(char *template, int oflags);
static int (*real_mkostemp)(char *template, int oflags);

/* int mkostemps(char *template, int suffixlen, int oflags) */
static int wrap_mkostemps(char *template, int suffixlen, int oflags);
static int (*real_mkostemps)(char *template, int suffixlen, int oflags);

/* int mkstemp(char *template) */
static int wrap_mkstemp(char *template);
static int (*real_mkstemp)(char *template);

/* int mkstemp64(char *template) */
static int wrap_mkstemp64(char *template);
static int (*real_mkstemp64)(char *template);

/* int mkstemps(char *template, int suffixlen) */
static int wrap_mkstemps(char *template, int suffixlen);
static int (*real_mkstemps)(char *template, int suffixlen);

/* char *mktemp(char *template) */
static char * wrap_mktemp(char *template);
static char * (*real_mktemp)(char *template);

/* int msync(void *addr, size_t length, int flags) */
static int wrap_msync(void *addr, size_t length, int flags);
static int (*real_msync)(void *addr, size_t length, int flags);

/* int nftw(const char *path, int (*fn)(const char *, const struct stat *, int, struct FTW *), int nopenfd, int flag) */
static int wrap_nftw(const char *path, int (*fn)(const char *, const struct stat *, int, struct FTW *), int nopenfd, int flag);
static int (*real_nftw)(const char *path, int (*fn)(const char *, const struct stat *, int, struct FTW *), int nopenfd, int flag);

/* int nftw64(const char *path, int (*fn)(const char *, const struct stat64 *, int, struct FTW *), int nopenfd, int flag) */
static int wrap_nftw64(const char *path, int (*fn)(const char *, const struct stat64 *, int, struct FTW *), int nopenfd, int flag);
static int (*real_nftw64)(const char *path, int (*fn)(const char *, const struct stat64 *, int, struct FTW *), int nopenfd, int flag);

/* int open(const char *path, int flags, ... { mode_t mode }) */
static int wrap_open(const char *path, int flags, ... /* mode_t mode */);
static int (*real_open)(const char *path, int flags, ... /* mode_t mode */);

/* int open64(const char *path, int flags, ... { mode_t mode }) */
static int wrap_open64(const char *path, int flags, ... /* mode_t mode */);
static int (*real_open64)(const char *path, int flags, ... /* mode_t mode */);

/* int openat(int dirfd, const char *path, int flags, ... { mode_t mode }) */
static int wrap_openat(int dirfd, const char *path, int flags, ... /* mode_t mode */);
static int (*real_openat)(int dirfd, const char *path, int flags, ... /* mode_t mode */);

/* int openat64(int dirfd, const char *path, int flags, ... { mode_t mode }) */
static int wrap_openat64(int dirfd, const char *path, int flags, ... /* mode_t mode */);
static int (*real_openat64)(int dirfd, const char *path, int flags, ... /* mode_t mode */);

/* DIR *opendir(const char *path) */
static DIR * wrap_opendir(const char *path);
static DIR * (*real_opendir)(const char *path);

/* long pathconf(const char *path, int name) */
static long wrap_pathconf(const char *path, int name);
static long (*real_pathconf)(const char *path, int name);

/* FILE *popen(const char *command, const char *mode) */
static FILE * wrap_popen(const char *command, const char *mode);
static FILE * (*real_popen)(const char *command, const char *mode);

/* ssize_t readlink(const char *path, char *buf, size_t bufsiz) */
static ssize_t wrap_readlink(const char *path, char *buf, size_t bufsiz);
static ssize_t (*real_readlink)(const char *path, char *buf, size_t bufsiz);

/* ssize_t readlinkat(int dirfd, const char *path, char *buf, size_t bufsiz) */
static ssize_t wrap_readlinkat(int dirfd, const char *path, char *buf, size_t bufsiz);
static ssize_t (*real_readlinkat)(int dirfd, const char *path, char *buf, size_t bufsiz);

/* char *realpath(const char *name, char *resolved_name) */
static char * wrap_realpath(const char *name, char *resolved_name);
static char * (*real_realpath)(const char *name, char *resolved_name);

/* int remove(const char *path) */
static int wrap_remove(const char *path);
static int (*real_remove)(const char *path);

/* int removexattr(const char *path, const char *name) */
static int wrap_removexattr(const char *path, const char *name);
static int (*real_removexattr)(const char *path, const char *name);

/* int rename(const char *oldpath, const char *newpath) */
static int wrap_rename(const char *oldpath, const char *newpath);
static int (*real_rename)(const char *oldpath, const char *newpath);

/* int renameat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath) */
static int wrap_renameat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath);
static int (*real_renameat)(int olddirfd, const char *oldpath, int newdirfd, const char *newpath);

/* int renameat2(int olddirfd, const char *oldpath, int newdirfd, const char *newpath, unsigned int flags) */
static int wrap_renameat2(int olddirfd, const char *oldpath, int newdirfd, const char *newpath, unsigned int flags);
static int (*real_renameat2)(int olddirfd, const char *oldpath, int newdirfd, const char *newpath, unsigned int flags);

/* int rmdir(const char *path) */
static int wrap_rmdir(const char *path);
static int (*real_rmdir)(const char *path);

/* int scandir(const char *path, struct dirent ***namelist, int (*filter)(const struct dirent *), int (*compar)()) */
static int wrap_scandir(const char *path, struct dirent ***namelist, int (*filter)(const struct dirent *), int (*compar)());
static int (*real_scandir)(const char *path, struct dirent ***namelist, int (*filter)(const struct dirent *), int (*compar)());

/* int scandir64(const char *path, struct dirent64 ***namelist, int (*filter)(const struct dirent64 *), int (*compar)()) */
static int wrap_scandir64(const char *path, struct dirent64 ***namelist, int (*filter)(const struct dirent64 *), int (*compar)());
static int (*real_scandir64)(const char *path, struct dirent64 ***namelist, int (*filter)(const struct dirent64 *), int (*compar)());

/* int setegid(gid_t egid) */
static int wrap_setegid(gid_t egid);
static int (*real_setegid)(gid_t egid);

/* int seteuid(uid_t euid) */
static int wrap_seteuid(uid_t euid);
static int (*real_seteuid)(uid_t euid);

/* int setfsgid(gid_t fsgid) */
static int wrap_setfsgid(gid_t fsgid);
static int (*real_setfsgid)(gid_t fsgid);

/* int setfsuid(uid_t fsuid) */
static int wrap_setfsuid(uid_t fsuid);
static int (*real_setfsuid)(uid_t fsuid);

/* int setgid(gid_t gid) */
static int wrap_setgid(gid_t gid);
static int (*real_setgid)(gid_t gid);

/* void setgrent(void) */
static void wrap_setgrent(void);
static void (*real_setgrent)(void);

/* int setgroups(size_t size, const gid_t *list) */
static int wrap_setgroups(size_t size, const gid_t *list);
static int (*real_setgroups)(size_t size, const gid_t *list);

/* void setpwent(void) */
static void wrap_setpwent(void);
static void (*real_setpwent)(void);

/* int setregid(gid_t rgid, gid_t egid) */
static int wrap_setregid(gid_t rgid, gid_t egid);
static int (*real_setregid)(gid_t rgid, gid_t egid);

/* int setresgid(gid_t rgid, gid_t egid, gid_t sgid) */
static int wrap_setresgid(gid_t rgid, gid_t egid, gid_t sgid);
static int (*real_setresgid)(gid_t rgid, gid_t egid, gid_t sgid);

/* int setresuid(uid_t ruid, uid_t euid, uid_t suid) */
static int wrap_setresuid(uid_t ruid, uid_t euid, uid_t suid);
static int (*real_setresuid)(uid_t ruid, uid_t euid, uid_t suid);

/* int setreuid(uid_t ruid, uid_t euid) */
static int wrap_setreuid(uid_t ruid, uid_t euid);
static int (*real_setreuid)(uid_t ruid, uid_t euid);

/* int setuid(uid_t uid) */
static int wrap_setuid(uid_t uid);
static int (*real_setuid)(uid_t uid);

/* int setxattr(const char *path, const char *name, const void *value, size_t size, int xflags) */
static int wrap_setxattr(const char *path, const char *name, const void *value, size_t size, int xflags);
static int (*real_setxattr)(const char *path, const char *name, const void *value, size_t size, int xflags);

/* int stat(const char *path, struct stat *buf) */
static int wrap_stat(const char *path, struct stat *buf);
static int (*real_stat)(const char *path, struct stat *buf);
int pseudo_stat(const char *path, struct stat *buf);
/* int stat64(const char *path, struct stat64 *buf) */
static int wrap_stat64(const char *path, struct stat64 *buf);
static int (*real_stat64)(const char *path, struct stat64 *buf);
int pseudo_stat64(const char *path, struct stat64 *buf);
/* int statvfs(const char *path, struct statvfs *buf) */
static int wrap_statvfs(const char *path, struct statvfs *buf);
static int (*real_statvfs)(const char *path, struct statvfs *buf);

/* int statx(int dirfd, const char *pathname, int flags, unsigned int mask, struct statx *statxbuf) */
static int wrap_statx(int dirfd, const char *pathname, int flags, unsigned int mask, struct statx *statxbuf);
static int (*real_statx)(int dirfd, const char *pathname, int flags, unsigned int mask, struct statx *statxbuf);

/* int symlink(const char *oldname, const char *newpath) */
static int wrap_symlink(const char *oldname, const char *newpath);
static int (*real_symlink)(const char *oldname, const char *newpath);

/* int symlinkat(const char *oldname, int dirfd, const char *newpath) */
static int wrap_symlinkat(const char *oldname, int dirfd, const char *newpath);
static int (*real_symlinkat)(const char *oldname, int dirfd, const char *newpath);

/* void sync(void) */
static void wrap_sync(void);
static void (*real_sync)(void);

/* int sync_file_range(int fd, off64_t offset, off64_t nbytes, unsigned int flags) */
static int wrap_sync_file_range(int fd, off64_t offset, off64_t nbytes, unsigned int flags);
static int (*real_sync_file_range)(int fd, off64_t offset, off64_t nbytes, unsigned int flags);

/* int syncfs(int fd) */
static int wrap_syncfs(int fd);
static int (*real_syncfs)(int fd);

/* long syscall(long nr, va_list ap) */
static long wrap_syscall(long nr, va_list ap);
static long (*real_syscall)(long nr, ...);

/* int system(const char *command) */
static int wrap_system(const char *command);
static int (*real_system)(const char *command);

/* char *tempnam(const char *template, const char *pfx) */
static char * wrap_tempnam(const char *template, const char *pfx);
static char * (*real_tempnam)(const char *template, const char *pfx);

/* char *tmpnam(char *s) */
static char * wrap_tmpnam(char *s);
static char * (*real_tmpnam)(char *s);

/* int truncate(const char *path, off_t length) */
static int wrap_truncate(const char *path, off_t length);
static int (*real_truncate)(const char *path, off_t length);

/* int truncate64(const char *path, off64_t length) */
static int wrap_truncate64(const char *path, off64_t length);
static int (*real_truncate64)(const char *path, off64_t length);

/* int ulckpwdf(void) */
static int wrap_ulckpwdf(void);
static int (*real_ulckpwdf)(void);

/* mode_t umask(mode_t mask) */
static mode_t wrap_umask(mode_t mask);
static mode_t (*real_umask)(mode_t mask);

/* int unlink(const char *path) */
static int wrap_unlink(const char *path);
static int (*real_unlink)(const char *path);

/* int unlinkat(int dirfd, const char *path, int rflags) */
static int wrap_unlinkat(int dirfd, const char *path, int rflags);
static int (*real_unlinkat)(int dirfd, const char *path, int rflags);

/* int utime(const char *path, const struct utimbuf *buf) */
static int wrap_utime(const char *path, const struct utimbuf *buf);
static int (*real_utime)(const char *path, const struct utimbuf *buf);

/* int utimes(const char *path, const struct timeval *times) */
static int wrap_utimes(const char *path, const struct timeval *times);
static int (*real_utimes)(const char *path, const struct timeval *times);

