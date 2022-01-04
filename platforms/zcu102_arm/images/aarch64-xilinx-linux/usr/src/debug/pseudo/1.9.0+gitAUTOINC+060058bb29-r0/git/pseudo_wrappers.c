/*
 * pseudo_wrappers.c, shared code for wrapper functions
 *
 * Copyright (c) 2008-2012 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */
#include <assert.h>
#include <stdlib.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <dlfcn.h>

/* include this to get PSEUDO_PORT_* definitions */
#include "pseudo.h"

/* used for various specific function arguments */
#include <dirent.h>
#include <fts.h>
#include <ftw.h>
#include <glob.h>
#include <grp.h>
#include <pwd.h>
#include <utime.h>
#ifdef PSEUDO_PORT_LINUX_STATVFS
#include <sys/statvfs.h>
#endif

#include "pseudo_wrapfuncs.h"
#include "pseudo_ipc.h"
#include "pseudo_client.h"


/* Types and declarations we need in advance. */
#include "pseudo_wrapper_table.c"

static void pseudo_enosys(const char *);
static int pseudo_check_wrappers(void);
static volatile int antimagic = 0;
static pthread_mutex_t pseudo_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_t pseudo_mutex_holder;
static int pseudo_mutex_recursion = 0;
static int pseudo_getlock(void);
static void pseudo_droplock(void);
static size_t pseudo_dechroot(char *, size_t);
static void pseudo_sigblock(sigset_t *);

extern char *program_invocation_short_name;
static sigset_t pseudo_saved_sigmask;

/* Constructor only exists in libpseudo */
static void _libpseudo_init(void) __attribute__ ((constructor));

static int _libpseudo_initted = 0;

#ifdef PSEUDO_PROFILING
extern struct timeval *pseudo_wrapper_time;
/* profiling shared postamble */
#define PROFILE_START \
	struct timeval tv1, tv2; \
	do { gettimeofday(&tv1, NULL); } while(0)
#define PROFILE_DONE do { \
	gettimeofday(&tv2, NULL); \
	pseudo_wrapper_time->tv_sec += tv2.tv_sec - tv1.tv_sec; \
	pseudo_wrapper_time->tv_usec += tv2.tv_usec - tv1.tv_usec; } while(0)
#else
#define PROFILE_START do {} while(0)
#define PROFILE_DONE do {} while(0)
#endif

/* later, the init code can change these to refer to the real calls and
 * skip the wrappers.
 */
#ifdef PSEUDO_XATTRDB
extern ssize_t (*pseudo_real_lgetxattr)(const char *, const char *, void *, size_t);
extern ssize_t (*pseudo_real_fgetxattr)(int, const char *, void *, size_t);
extern int (*pseudo_real_lsetxattr)(const char *, const char *, const void *, size_t, int);
extern int (*pseudo_real_fsetxattr)(int, const char *, const void *, size_t, int);
#endif

static void libpseudo_atfork_child(void)
{
	pthread_mutex_init(&pseudo_mutex, NULL);
	pseudo_mutex_recursion = 0;
	pseudo_mutex_holder = 0;
}

static void
_libpseudo_init(void) {
	if (!_libpseudo_initted)
		pthread_atfork(NULL, NULL, libpseudo_atfork_child);

	pseudo_getlock();
	pseudo_antimagic();
	_libpseudo_initted = 1;

	pseudo_init_util();
	pseudo_init_wrappers();
	pseudo_init_client();

	pseudo_magic();
	pseudo_droplock();
}

void
pseudo_reinit_libpseudo(void) {
	_libpseudo_init();
}

static void
pseudo_init_one_wrapper(pseudo_function *func) {
	int (*f)(void) = (int (*)(void)) NULL;

	if (*func->real != NULL) {
		/* already initialized */
		return;
	}
	dlerror();

#if PSEUDO_PORT_LINUX
	if (func->version)
		f = dlvsym(RTLD_NEXT, func->name, func->version);
	/* fall through to the general case, if that failed */
	if (!f)
#endif
	f = dlsym(RTLD_NEXT, func->name);
	if (f) {
		*func->real = f;
	}
	/* it turns out that in some cases, we get apparently-harmless
	 * errors if a function is missing, and that printing output
	 * for these seems unhelpful. so we no longer do that.
	 */
}

void
pseudo_init_wrappers(void) {
	int i;
	static int done = 0;

	pseudo_getlock();
	pseudo_antimagic();

	/* We only ever want to run this once, even though we might want to
	 * "re-init" at specific times...
	 */
	if (!done) {
		for (i = 0; pseudo_functions[i].name; ++i) {
			pseudo_init_one_wrapper(&pseudo_functions[i]);
		}
		done = 1;
	}

#ifdef PSEUDO_XATTRDB
	pseudo_real_lgetxattr = real_lgetxattr;
	pseudo_real_fgetxattr = real_fgetxattr;
	pseudo_real_lsetxattr = real_lsetxattr;
	pseudo_real_fsetxattr = real_fsetxattr;
#endif
	pseudo_real_lstat = base_lstat;
	/* bash has its own local copies of these which it uses
	 * instead of ours...
	 */
	pseudo_real_unsetenv = dlsym(RTLD_NEXT, "unsetenv");
	pseudo_real_getenv = dlsym(RTLD_NEXT, "getenv");
	pseudo_real_setenv = dlsym(RTLD_NEXT, "setenv");
	/* and these are used so the client's server spawn can bypass
	 * wrappers.
	 */
	pseudo_real_fork = dlsym(RTLD_NEXT, "fork");
	pseudo_real_execv = dlsym(RTLD_NEXT, "execv");

	/* Once the wrappers are setup, we can now use open... so
	 * setup the logfile, if necessary...
	 */
	pseudo_debug_logfile(NULL, -1);

	pseudo_magic();
	pseudo_droplock();
}

static void
pseudo_sigblock(sigset_t *saved) {
	sigset_t blocked;

	/* these are signals for which the handlers often
	 * invoke operations, such as close(), which are handled
	 * by pseudo and could result in a deadlock.
	 */
	sigemptyset(&blocked);
	sigaddset(&blocked, SIGALRM);	/* every-N-seconds tasks */
	sigaddset(&blocked, SIGCHLD);	/* reaping child processes */
	sigaddset(&blocked, SIGHUP);	/* idiomatically, reloading config */
	sigaddset(&blocked, SIGTERM);	/* shutdown/teardown operations */
	sigaddset(&blocked, SIGUSR1);	/* reopening log files, sometimes */
	sigaddset(&blocked, SIGUSR2);	/* who knows what people do */
	sigprocmask(SIG_BLOCK, &blocked, saved);
}

static int
pseudo_getlock(void) {
	if (pthread_equal(pseudo_mutex_holder, pthread_self())) {
		++pseudo_mutex_recursion;
		return 0;
	} else {
		if (pthread_mutex_lock(&pseudo_mutex) == 0) {
			pseudo_mutex_recursion = 1;
			pseudo_mutex_holder = pthread_self();
			return 0;
		} else {
			return -1;
		}
	}
}

static void
pseudo_droplock(void) {
	if (--pseudo_mutex_recursion == 0) {
		pseudo_mutex_holder = 0;
		pthread_mutex_unlock(&pseudo_mutex);
	}
}

void
pseudo_antimagic() {
	++antimagic;
}

void
pseudo_magic() {
	if (antimagic > 0)
		--antimagic;
}

static void
pseudo_enosys(const char *func) {
	pseudo_diag("pseudo: ENOSYS for '%s'.\n", func ? func : "<nil>");
	char * value = pseudo_get_value("PSEUDO_ENOSYS_ABORT");
	if (value)
		abort();
	free(value);
	errno = ENOSYS;
}

/* de-chroot a string.
 * note that readlink() yields an unterminated buffer, so
 * must pass in the correct length.  Buffers are null-terminated
 * unconditionally if they are modified -- the modification would
 * shorten the string, so there will be space for the NUL, so
 * this is safe even for stuff like readlink().
 */
static size_t
pseudo_dechroot(char *s, size_t len) {
	if (len == (size_t) -1)
		len = strlen(s);
	if (pseudo_chroot_len && len >= pseudo_chroot_len &&
		!memcmp(s, pseudo_chroot, pseudo_chroot_len)) {
		if (s[pseudo_chroot_len] == '/') {
			memmove(s, s + pseudo_chroot_len, len - pseudo_chroot_len);
			len -= pseudo_chroot_len;
			s[len] = '\0';
		} else if (s[pseudo_chroot_len] == '\0') {
			s[0] = '/';
			len = 1;
			s[len] = '\0';
		}
		/* otherwise, it's not really a match... */
	}
	return len;
}

static int
pseudo_check_wrappers(void) {
	if (!_libpseudo_initted)
		pseudo_reinit_libpseudo();

	return _libpseudo_initted;
}		

/* the generated code goes here */
#include "port_wrappers.c"
#include "pseudo_wrapfuncs.c"

