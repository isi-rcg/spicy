/*
 * sundries.h
 * Support function prototypes.  Functions are in sundries.c.
 */

#include <sys/types.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <stdarg.h>
#include <stdlib.h>
#if !defined(bool_t) && !defined(__GLIBC__)
#include <rpc/types.h>
#endif

extern int mount_mount_quiet;
extern int mount_verbose;
extern int sloppy;

#define streq(s, t)	(strcmp ((s), (t)) == 0)

/* String list data structure.  */
typedef struct string_list {
	char *hd;
	struct string_list *tl;
} *string_list;

#define car(p) ((p) -> hd)
#define cdr(p) ((p) -> tl)

string_list cons(char *a, const string_list);

/* Functions in sundries.c that are used in mount.c and umount.c  */
void block_signals(int how);
char *canonicalize(const char *path);
char *realpath(const char *path, char *resolved_path);
void error(const char *fmt, ...);
int matching_type(const char *type, string_list types);
string_list parse_list(char *strings);
char *xstrconcat2(const char *, const char *);
char *xstrconcat3(const char *, const char *, const char *);
char *xstrconcat4(const char *, const char *, const char *, const char *);

/* Here is some serious cruft.  */
#ifdef __GNUC__
#if defined(__GNUC_MINOR__) && __GNUC__ == 2 && __GNUC_MINOR__ >= 5
void die(int errcode, const char *fmt, ...) __attribute__ ((noreturn));
#else				/* GNUC < 2.5 */
void die(int errcode, const char *fmt, ...);
#endif				/* GNUC < 2.5 */
#else				/* !__GNUC__ */
void die(int errcode, const char *fmt, ...);
#endif				/* !__GNUC__ */

#ifdef HAVE_NFS
int nfsmount(const char *spec, const char *node, int *flags, char **orig_opts, char **opt_args, int running_bg);
#endif

#include "logmessage.h" /* Has EX_USAGE and other exit values. */
#include "xmalloc.h"	/* Has xmalloc(), xstrdup() & xstrndup() */
