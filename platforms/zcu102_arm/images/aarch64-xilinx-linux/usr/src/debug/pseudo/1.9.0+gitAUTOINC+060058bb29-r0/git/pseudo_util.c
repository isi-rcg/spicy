/*
 * pseudo_util.c, miscellaneous utility functions
 *
 * Copyright (c) 2008-2013 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */
/* we need access to RTLD_NEXT for a horrible workaround */
#define _GNU_SOURCE

#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <regex.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <limits.h>

/* see the comments below about (*real_regcomp)() */
#include <dlfcn.h>

#include "pseudo.h"
#include "pseudo_ipc.h"
#include "pseudo_db.h"

struct pseudo_variables {
	char *key;
	size_t key_len;
	char *value;
};

/* The order below is not arbitrary, but based on an assumption
 * of how often things will be used.
 */
static struct pseudo_variables pseudo_env[] = {
	{ "PSEUDO_PREFIX", 13, NULL },
	{ "PSEUDO_BINDIR", 13, NULL },
	{ "PSEUDO_LIBDIR", 13, NULL },
	{ "PSEUDO_LOCALSTATEDIR", 20, NULL },
	{ "PSEUDO_PASSWD", 13, NULL },
	{ "PSEUDO_CHROOT", 13, NULL },
	{ "PSEUDO_UIDS", 11, NULL },
	{ "PSEUDO_GIDS", 11, NULL },
	{ "PSEUDO_OPTS", 11, NULL },
	{ "PSEUDO_DEBUG", 12, NULL },
	{ "PSEUDO_DEBUG_FILE", 17, NULL },
	{ "PSEUDO_TAG", 10, NULL },
	{ "PSEUDO_ENOSYS_ABORT", 19, NULL },
	{ "PSEUDO_NOSYMLINKEXP", 19, NULL },
	{ "PSEUDO_DISABLED", 15, NULL },
	{ "PSEUDO_UNLOAD", 13, NULL },
	{ "PSEUDO_ALLOW_FSYNC", 18, NULL },
#ifdef PSEUDO_PROFILING
	{ "PSEUDO_PROFILE_PATH", 19, NULL },
#endif
	{ "PSEUDO_EVLOG", 12, NULL },
	{ "PSEUDO_EVLOG_FILE", 17, NULL },
	{ NULL, 0, NULL } /* Magic terminator */
};

typedef struct {
	struct timeval stamp;
	int len;
	char *data;
} pseudo_evlog_entry;

/* so bash overrides getenv/unsetenv/etcetera, preventing them from
 * actually modifying environ, so we have pseudo_wrappers try to dlsym
 * the right values. This could fail, in which case we'd get null
 * pointers, and we'll just call whatever the linker gives us and
 * hope for the best.
 */
#define SETENV(x, y, z) (pseudo_real_setenv ? pseudo_real_setenv : setenv)(x, y, z)
#define GETENV(x) (pseudo_real_getenv ? pseudo_real_getenv : getenv)(x)
#define UNSETENV(x) (pseudo_real_unsetenv ? pseudo_real_unsetenv : unsetenv)(x)

#define PSEUDO_EVLOG_ENTRIES 250
#define PSEUDO_EVLOG_LENGTH 256
static pseudo_evlog_entry event_log[PSEUDO_EVLOG_ENTRIES];
static char *pseudo_evlog_buffer;
static int pseudo_evlog_next_entry = 0;
static void pseudo_evlog_set(char *);
static void pseudo_evlog_flags_finalize(void);
static unsigned long pseudo_debug_flags_in(char *);

/* -1 - init hasn't been run yet
 * 0 - init has been run
 * 1 - init is running
 *
 * There are cases where the constructor is run AFTER the
 * program starts playing with things, so we need to do our
 * best to handle that case.
 */
static int pseudo_util_initted = -1;  /* Not yet run */

/* bypass wrapper logic on path computations */
int (*pseudo_real_lstat)(const char *path, PSEUDO_STATBUF *buf) = NULL;
/* bash workaround */
int (*pseudo_real_unsetenv)(const char *) = unsetenv;
char * (*pseudo_real_getenv)(const char *) = getenv;
int (*pseudo_real_setenv)(const char *, const char *, int) = setenv;

#if 0
static void
dump_env(char **envp) {
	size_t i = 0;
	for (i = 0; envp[i]; i++) {
		pseudo_debug(PDBGF_ENV, "dump_envp: [%d]%s\n", (int) i, envp[i]);
	}

	for (i = 0; pseudo_env[i].key; i++) {
		pseudo_debug(PDBGF_ENV, "dump_envp: {%d}%s=%s\n", (int) i, pseudo_env[i].key, pseudo_env[i].value);
	}

	pseudo_debug(PDBGF_ENV, "dump_envp: _in_init %d\n", pseudo_util_initted);
}
#endif

int
pseudo_has_unload(char * const *envp) {
	static const char unload[] = "PSEUDO_UNLOAD";
	static size_t unload_len = sizeof(unload) - 1;
	size_t i = 0;

	/* Is it in the caller environment? */
	if (NULL != GETENV(unload))
		return 1;

	/* Is it in the environment cache? */
	if (pseudo_util_initted == -1)
		pseudo_init_util();
	while (pseudo_env[i].key && strcmp(pseudo_env[i].key, unload))
		++i;
	if (pseudo_env[i].key && pseudo_env[i].value)
		return 1;

	/* Is it in the operational environment? */
	while (envp && *envp) {
		if ((!strncmp(*envp, unload, unload_len)) && ('=' == (*envp)[unload_len]))
			return 1;
		++envp;
	}
	return 0;
}

/* Caller must free memory! */
char *
pseudo_get_value(const char *key) {
	size_t i = 0;
	char * value;

	if (pseudo_util_initted == -1)
		pseudo_init_util();

	for (i = 0; pseudo_env[i].key && memcmp(pseudo_env[i].key, key, pseudo_env[i].key_len + 1); i++)
		;

	/* Check if the environment has it and we don't ...
	 * if so, something went wrong... so we'll attempt to recover
	 */
	if (pseudo_env[i].key && !pseudo_env[i].value && GETENV(pseudo_env[i].key))
		pseudo_init_util();

	if (pseudo_env[i].value)
		value = strdup(pseudo_env[i].value);
	else
		value = NULL;

	if (!pseudo_env[i].key) 
		pseudo_diag("Unknown variable %s.\n", key);

	return value;
}

/* We make a copy, so the original values should be freed. */
int
pseudo_set_value(const char *key, const char *value) {
	int rc = 0;
	size_t i = 0;

	if (pseudo_util_initted == -1)
		pseudo_init_util();

	for (i = 0; pseudo_env[i].key && memcmp(pseudo_env[i].key, key, pseudo_env[i].key_len + 1); i++)
		;

	if (pseudo_env[i].key) {
		if (pseudo_env[i].value)
			free(pseudo_env[i].value);
		if (value) {
			char *new = strdup(value);
			if (new)
				pseudo_env[i].value = new;
			else
				pseudo_diag("warning: failed to save new value (%s) for key %s\n",
					value, key);
		} else
			pseudo_env[i].value = NULL;
	} else {
		if (!pseudo_util_initted) pseudo_diag("Unknown variable %s.\n", key);
		rc = -EINVAL;
	}

	return rc;
}

void
pseudo_init_util(void) {
	size_t i = 0;
	char * env;

	pseudo_util_initted = 1;

	for (i = 0; pseudo_env[i].key; i++) {
		if (GETENV(pseudo_env[i].key))
			pseudo_set_value(pseudo_env[i].key, GETENV(pseudo_env[i].key));
	}

	pseudo_util_initted = 0;

	/* Somewhere we have to set the debug level.. */
	env = pseudo_get_value("PSEUDO_DEBUG");
	if (env) {
		int i;
		int level = atoi(env);
		if (level > 0) {
			for (i = 0; i < level; ++i) {
				pseudo_debug_verbose();
			}
		} else {
			pseudo_debug_set(env);
		}
		pseudo_debug_flags_finalize();
	}
	free(env);
	env = pseudo_get_value("PSEUDO_EVLOG");
	if (env) {
		pseudo_evlog_set(env);
		pseudo_evlog_flags_finalize();
	}
	free(env);
}

unsigned long pseudo_util_debug_flags = 0;
unsigned long pseudo_util_evlog_flags = 0;
int pseudo_util_debug_fd = 2;
int pseudo_util_evlog_fd = 2;
static int debugged_newline = 1;
static char pid_text[32];
static size_t pid_len;
static int pseudo_append_element(char *newpath, char *root, size_t allocated, char **pcurrent, const char *element, size_t elen, PSEUDO_STATBUF *buf, int leave_this);
static int pseudo_append_elements(char *newpath, char *root, size_t allocated, char **current, const char *elements, size_t elen, int leave_last, PSEUDO_STATBUF *sbuf);
extern char **environ;
static ssize_t pseudo_max_pathlen = -1;
static ssize_t pseudo_sys_max_pathlen = -1;

/* in our installed system, we usually use a name of the form
 * libpseudoCHECKSUM.so, where CHECKSUM is an md5 checksum of the host
 * libc.so -- this forces rebuilds of the library when the C library
 * changes.  The problem is that the pseudo binary may be
 * a prebuilt, in which case it doesn't know about CHECKSUM, so it
 * has to determine whether a given PRELINK_LIBRARIES contains libpseudo.so
 * or libpseudoCHECKSUM.so, without prior knowledge... Fancy!
 * 
 * We search for anything matching libpseudo*.so, where * is any
 * sequence of non-spaces (including an empty string), with either
 * the beginning of the string or a space in front of it, and either
 * the end of the string or a space after it.
 */
static char *libpseudo_name = "libpseudo.so";
/* this used to look for a "libpseudo*.so", but it turns out you can
 * specify a path even on Linux.
 */
static char *libpseudo_pattern = "(^|=| )[^ ]*libpseudo[^ ]*\\.so($| )";
static regex_t libpseudo_regex;
static int libpseudo_regex_compiled = 0;

/* Okay, so, there's a funny story behind this.  On one of the systems
 * we need to run on, /usr/bin/find happens to provide its own
 * definitions of regcomp and regexec which are INCOMPATIBLE with the
 * ones in the C library, and not only that, but which have buggy and/or
 * incompatible semantics, such that they trash elements of the pmatch
 * array.  So we do our best to call the "real" regcomp/regexec in the
 * C library.  If we can't find them, we just do our best and hope that
 * no one called us from a program with incompatible variants.
 *
 */
#if PSEUDO_PORT_LINUX
static int (*real_regcomp)(regex_t *__restrict __preg, const char *__restrict __pattern, int __cflags);
static int (*real_regexec)(const regex_t *__restrict __preg, const char *__restrict __string, size_t __nmatch, regmatch_t __pmatch[__restrict_arr], int __eflags);
#else
#define real_regcomp regcomp
#define real_regexec regexec
#endif /* PSEUDO_PORT_LINUX */

static int
libpseudo_regex_init(void) {
	int rc;

	if (libpseudo_regex_compiled)
		return 0;
#if PSEUDO_PORT_LINUX
	real_regcomp = dlsym(RTLD_NEXT, "regcomp");
	if (!real_regcomp)
		real_regcomp = regcomp;
	real_regexec = dlsym(RTLD_NEXT, "regexec");
	if (!real_regexec)
		real_regexec = regexec;
#endif
	rc = (*real_regcomp)(&libpseudo_regex, libpseudo_pattern, REG_EXTENDED);
	if (rc == 0)
		libpseudo_regex_compiled = 1;
	return rc;
}

/* given a space-or-colon-separated list of files, ala PRELINK_LIBRARIES,
 # return that list without any variants of libpseudo*.so.
 */
static char *
without_libpseudo(char *list) {
	regmatch_t pmatch[1];
	int counter = 0;
	int skip_start = 0;

	if (libpseudo_regex_init())
		return NULL;

	if (list[0] == '=' || list[0] == PSEUDO_LINKPATH_SEPARATOR[0])
		skip_start = 1;

	if ((*real_regexec)(&libpseudo_regex, list, 1, pmatch, 0)) {
		return list;
	}
	list = strdup(list);
	while (!(*real_regexec)(&libpseudo_regex, list, 1, pmatch, 0)) {
		char *start = list + pmatch[0].rm_so;
		char *end = list + pmatch[0].rm_eo;
		/* don't copy over the space or = */
		start += skip_start;
		memmove(start, end, strlen(end) + 1);
		++counter;
		if (counter > 5) {
			pseudo_diag("Found way too many libpseudo.so in environment, giving up.\n");
			return list;
		}
	}
	return list;
}

static char *
with_libpseudo(char *list, char *libdir_path) {
	regmatch_t pmatch[1];

	if (libpseudo_regex_init())
		return NULL;
	if ((*real_regexec)(&libpseudo_regex, list, 1, pmatch, 0)) {
		size_t len;
#if PSEUDO_PORT_DARWIN
		/* <%s:%s/%s\0> */
		len = strlen(list) + 1 + strlen(libdir_path) + 1 + strlen(libpseudo_name) + 1;
#else
		/* suppress warning */
		(void) libdir_path;
		/* <%s %s\0> */
		len = strlen(list) + 1 + strlen(libpseudo_name) + 1;
#endif
		char *new = malloc(len);
		if (new) {
			/* insert space only if there were previous bits */
			/* on Darwin, we have to provide the full path to
			 * libpseudo
			 */
#if PSEUDO_PORT_DARWIN
			snprintf(new, len, "%s%s%s/%s", list,
				*list ? PSEUDO_LINKPATH_SEPARATOR : "",
				libdir_path ? libdir_path : "",
				libpseudo_name);
#else
			snprintf(new, len, "%s%s%s", list,
				*list ? PSEUDO_LINKPATH_SEPARATOR : "",
				libpseudo_name);
#endif
		}
		return new;
	} else {
		return strdup(list);
	}
}

char *pseudo_version = PSEUDO_VERSION;

/* going away soon */
static int max_debug_level = 0;

void
pseudo_debug_terse(void) {
	char s[2] = { pseudo_debug_type_symbolic(max_debug_level) };

	if (max_debug_level > 0) {
		--max_debug_level;
		pseudo_debug_clear(s);
	}
}

void
pseudo_debug_verbose(void) {
	char s[2] = { pseudo_debug_type_symbolic(max_debug_level + 1) };

	if (s[0]) {
		pseudo_debug_set(s);
		++max_debug_level;
	}
}

void
pseudo_debug_set(char *s) {
	pseudo_util_debug_flags = pseudo_debug_flags_in(s);
}

static void
pseudo_evlog_set(char *s) {
	pseudo_util_evlog_flags = pseudo_debug_flags_in(s);
}

/* This exists because we don't want to allocate a bunch of strings
 * and free them immediately if you have several flags set.
 */
static void
pseudo_flags_finalize(unsigned long flags, char *value) {
	char buf[PDBG_MAX + 1] = "", *s = buf;
	for (int i = 0; i < PDBG_MAX; ++i) {
		if (flags & (1 << i)) {
			*s++ = pseudo_debug_type_symbolic(i);
		}
	}
	pseudo_set_value(value, buf);
}

void
pseudo_debug_flags_finalize(void) {
	pseudo_flags_finalize(pseudo_util_debug_flags, "PSEUDO_DEBUG");
}

void
pseudo_evlog_flags_finalize(void) {
	pseudo_flags_finalize(pseudo_util_evlog_flags, "PSEUDO_EVLOG");
}

static unsigned long
pseudo_debug_flags_in(char *s) {
	unsigned long flags = 0;
	if (!s)
		return flags;
	for (; *s; ++s) {
		int id = pseudo_debug_type_symbolic_id(*s);
		if (id > 0) {
			flags |= (1 << id);
		}
	}
	return flags;
}

void
pseudo_debug_clear(char *s) {
	if (!s)
		return;
	for (; *s; ++s) {
		int id = pseudo_debug_type_symbolic_id(*s);
		if (id > 0) {
			pseudo_util_debug_flags &= ~(1 << id);
		}
	}
}

int
pseudo_diag(char *fmt, ...) {
	va_list ap;
	char debuff[8192];
	int len;
	/* gcc on Ubuntu 8.10 requires that you examine the return from
	 * write(), and won't let you cast it to void.  Of course, if you
	 * can't print error messages, there's nothing to do.
	 */
	int wrote = 0;

	va_start(ap, fmt);
	len = vsnprintf(debuff, 8192, fmt, ap);
	va_end(ap);

	if (len > 8192)
		len = 8192;

	if (debugged_newline && (pseudo_util_debug_flags & PDBGF_PID)) {
		wrote += write(pseudo_util_debug_fd, pid_text, pid_len);
	}
	debugged_newline = (debuff[len - 1] == '\n');

	wrote += write(pseudo_util_debug_fd, debuff, len);
	return wrote;
}

void
pseudo_evlog_dump(void) {
	char scratch[256], firstdate[64], lastdate[64];
	time_t first = 0, last = 0;
	int len;
	int entries = 0;
	struct tm first_tm, last_tm;
	int wrote; /* ignoring write errors because there's nothing we can do */

	for (int i = 0; i < PSEUDO_EVLOG_ENTRIES; ++i) {
		pseudo_evlog_entry *e = &event_log[i];
		if (!e->data || e->len < 0 || e->stamp.tv_sec == 0)
			continue;
		++entries;
		if (!first || e->stamp.tv_sec < first)
			first = e->stamp.tv_sec;
		if (!last || e->stamp.tv_sec > last)
			last = e->stamp.tv_sec;
	}
	localtime_r(&first, &first_tm);
	localtime_r(&last, &last_tm);
	strftime(firstdate, 64, "%Y-%M-%D %H:%M:%S", &first_tm);
	strftime(lastdate, 64, "%Y-%M-%D %H:%M:%S", &last_tm);

	len = snprintf(scratch, 256, "event log for pid %d [%d entries]\n",
		getpid(), entries);
	if (len > 256)
		len = 256;
	wrote = write(pseudo_util_evlog_fd, scratch, len);
	len = snprintf(scratch, 256, "  first entry %s\n", firstdate);
	wrote = write(pseudo_util_evlog_fd, scratch, len);
	len = snprintf(scratch, 256, "  last entry %s\n", lastdate);
	wrote = write(pseudo_util_evlog_fd, scratch, len);

	for (int i = 0; i < PSEUDO_EVLOG_ENTRIES; ++i) {
		int entry = (pseudo_evlog_next_entry + i) % PSEUDO_EVLOG_ENTRIES;
		pseudo_evlog_entry *ev = &event_log[entry];
		if (!ev->data || ev->len <= 0)
			continue;
		localtime_r(&ev->stamp.tv_sec, &first_tm);
		len = strftime(firstdate, 64, "%H:%M:%S", &first_tm);
		if (len) {
			len = snprintf(scratch, 256, "%s.%03d: ", firstdate,
				(int) (ev->stamp.tv_usec / 1000));
			wrote = write(pseudo_util_evlog_fd, scratch, len);
		} else {
			wrote = write(pseudo_util_evlog_fd, "no timestamp: ", 14);
		}
		wrote = write(pseudo_util_evlog_fd, ev->data, ev->len);
	}
	(void) wrote;
}

int
pseudo_evlog_internal(char *fmt, ...) {
	va_list ap;
	pseudo_evlog_entry *ev = &event_log[pseudo_evlog_next_entry++];

	pseudo_evlog_next_entry %= PSEUDO_EVLOG_ENTRIES;

	if (!ev->data) {
		pseudo_evlog_buffer = malloc(PSEUDO_EVLOG_ENTRIES * PSEUDO_EVLOG_LENGTH);
		if (pseudo_evlog_buffer) {
			for (int i = 0; i < PSEUDO_EVLOG_ENTRIES; ++i) {
				event_log[i].data = pseudo_evlog_buffer + (PSEUDO_EVLOG_LENGTH * i);
			}
		} else {
			pseudo_diag("fatal: can't allocate event log storage.\n");
		}
	}

	va_start(ap, fmt);
	ev->len = vsnprintf(ev->data, PSEUDO_EVLOG_LENGTH, fmt, ap);
	va_end(ap);
	if (ev->len > PSEUDO_EVLOG_LENGTH) {
		strcpy(ev->data + PSEUDO_EVLOG_LENGTH - 5, "...\n");
		ev->len = PSEUDO_EVLOG_LENGTH - 1;
	}
	gettimeofday(&ev->stamp, NULL);

	return ev->len;
}

/* store pid in text form for prepending to messages */
void
pseudo_new_pid() {
#if PSEUDO_PORT_LINUX
	extern char *program_invocation_short_name; /* glibcism */
#else
	char *program_invocation_short_name = "unknown";
#endif
	int pid = getpid();
	pid_len = snprintf(pid_text, 32, "%d: ", pid);
	pseudo_debug(PDBGF_PID, "new pid: %d [%s]\n", pid, program_invocation_short_name);
}

/* helper function for pseudo_fix_path
 * adds "element" to "newpath" at location current, if it can, then
 * checks whether this now points to a symlink.  If it does, expand
 * the symlink, appending each element in turn the same way.
 */
static int
pseudo_append_element(char *newpath, char *root, size_t allocated, char **pcurrent, const char *element, size_t elen, PSEUDO_STATBUF *buf, int leave_this) {
	static int link_recursion = 0;
	size_t curlen;
	int is_dir = S_ISDIR(buf->st_mode);
	char *current;
	if (!newpath ||
	    !pcurrent || !*pcurrent ||
	    !root || !element) {
		pseudo_diag("pseudo_append_element: invalid args.\n");
		return -1;
	}
	current = *pcurrent;
	pseudo_debug(PDBGF_PATH | PDBGF_VERBOSE, "pae: '%s', + '%.*s', is_dir %d\n",
		newpath, (int) elen, element, is_dir);
	/* the special cases here to skip empty paths, or ./.., should apply
	 * only to directories; plain files, nodes, etcetera should just get
	 * bogus paths.
	 */
	if (is_dir) {
		/* sanity-check:  ignore // or /./ */
		if (elen == 0 || (elen == 1 && *element == '.')) {
			return 0;
		}
		/* backtrack for .. */
		if (elen == 2 && element[0] == '.' && element[1] == '.') {
			/* if newpath is empty, do nothing. */
			if (current <= root)
				return 0;
			/* now find the previous slash */
			while (current > root && *current != '/') {
				--current;
			}
			/* either we're at root, or we're at a slash.
			 * either way, nul that out, leaving us with a
			 * possibly-empty path which is not slash-terminated.
			 */
			*current = '\0';
			*pcurrent = current;
			return 1;
		}
	}
	curlen = current - newpath;
	/* current length, plus / <element> / \0 */
	/* => curlen + elen + 3 */
	if (curlen + elen + 3 > allocated) {
		pseudo_diag("pseudo_append_element: path too long (wanted %lu bytes).\n", (unsigned long) curlen + elen + 3);
		return -1;
	}
	/* append a slash */
	*current++ = '/';
	memcpy(current, element, elen);
	current += elen;
	/* nul-terminate, and we now point to the nul after the element just added. */
	*current = '\0';
	/* if we are not on the last element of a path and supposed to leave
	 * it alone (for SYMLINK_NOFOLLOW type cases), and we are not trying to
	 * go further under a regular file, we want to know whether this is a symlink.
	 * either way, we want to update buf to show the correct state of the file.
	 */
	if (!pseudo_real_lstat || (pseudo_real_lstat(newpath, buf) == -1)) {
		// if we can't stat it, zero mode so we don't think it's
		// known to be a link or a regular file.
		buf->st_mode = 0;
	}
	/* it is intentional that this uses the "stale" is_dir for the file we
	 * were appending to. we don't want to actually try to do this when
	 * we're appending names to a regular file.
	 */
	if (!leave_this && is_dir) {
		int is_link = S_ISLNK(buf->st_mode);
		if (link_recursion >= PSEUDO_MAX_LINK_RECURSION && is_link) {
			pseudo_diag("link recursion too deep, not expanding path '%s'.\n", newpath);
			is_link = 0;
		}
		if (is_link) {
			char linkbuf[pseudo_path_max() + 1];
			ssize_t linklen;
			int retval;

			linklen = readlink(newpath, linkbuf, pseudo_path_max());
			if (linklen == -1) {
				pseudo_diag("uh-oh!  '%s' seems to be a symlink, but I can't read it.  Ignoring.", newpath);
				return 0;
			}
			/* null-terminate buffer */
			linkbuf[linklen] = '\0';
			/* absolute symlink means start over! */
			if (*linkbuf == '/') {
				current = newpath;
			} else {
				/* point back at the end of the previous path... */
				current -= (elen + 1);
			}
			/* null terminate at the new pointer */
			*current = '\0';
			*pcurrent = current;
			/* we know that we're now pointing either at a directory we
			 * already decided was safe to go into, or root. either way,
			 * the parent item mode should reflect it being a directory.
			 * we don't need to call stat for that.
			 */
			buf->st_mode = S_IFDIR;
			/* append all the elements in series */
			++link_recursion;
			retval = pseudo_append_elements(newpath, root, allocated, pcurrent, linkbuf, linklen, 0, buf);
			--link_recursion;
			return retval;
		}
	}
	/* we used to always append a slash here. now we don't; append_elements
	 * handles slashes, so just update the pointer.
	 */
	*pcurrent = current;
	return 1;
}

static int
pseudo_append_elements(char *newpath, char *root, size_t allocated, char **current, const char *path, size_t elen, int leave_last, PSEUDO_STATBUF *sbuf) {
	int retval = 1;
	/* a shareable buffer so we can cache stat results while walking the path */
	PSEUDO_STATBUF buf;
	buf.st_mode = 0;
	const char * start = path;
	if (!newpath || !root ||
	    !current || !*current ||
	    !path) {
		pseudo_diag("pseudo_append_elements: invalid arguments.");
		return -1;
	}
	if (!sbuf) {
		/* we will use this buffer to hold "the current state of newpath".
		 * append_element will update that whenever it appends an element,
		 * and any calls back here from there will pass in the same buffer.
		 * if we didn't get one, we start using this local one, which will
		 * then get shared by anything we call.
		 */
		sbuf = &buf;
		if (*current > root) {
			if (!pseudo_real_lstat || (pseudo_real_lstat(newpath, sbuf) == -1)) {
				sbuf->st_mode = 0;
			}
		} else {
			/* Don't call lstat on an empty path, or at all when we
			 * know that "root" is always directory-like.
			 */
			sbuf->st_mode = S_IFDIR;
		}
	}
	pseudo_debug(PDBGF_PATH | PDBGF_VERBOSE, "paes: newpath %s, element list <%.*s>\n",
		newpath, (int) elen, path);
	while (path < (start + elen) && *path) {
		size_t this_elen;
		int leave_this = 0;
		char *next = strchr(path, '/');
		if (!next) {
			next = strchr(path, '\0');
			leave_this = leave_last;
		}
		this_elen = next - path;
		/* for a directory, we skip the append for empty path or ".";
		 * regular files get it appended so they can fail properly
		 * later for being invalid paths.
		 */
		pseudo_debug(PDBGF_PATH | PDBGF_VERBOSE, "element to add: '%.*s'\n",
			(int) this_elen, path);
		if (pseudo_append_element(newpath, root, allocated, current, path, this_elen, sbuf, leave_this) == -1) {
			retval = -1;
			break;
		}
		pseudo_debug(PDBGF_FILE | PDBGF_VERBOSE, "paes: append_element gave us '%s', current '%s'\n",
			newpath, *current);
		/* and now move past the separator */
		path += this_elen + 1;
	}
	return retval;
}

/* don't do so many allocations */
#define PATHBUFS 16
static char *pathbufs[PATHBUFS] = { 0 };
static int pathbuf = 0;

/* Canonicalize path.  "base", if present, is an already-canonicalized
 * path of baselen characters, presumed not to end in a /.  path is
 * the new path to be canonicalized.  The tricky part is that path may
 * contain symlinks, which must be resolved.
 * if "path" starts with a /, then it is an absolute path, and
 * we ignore base.
 */
char *
pseudo_fix_path(const char *base, const char *path, size_t rootlen, size_t baselen, size_t *lenp, int leave_last) {
	size_t newpathlen, pathlen;
	char *newpath;
	char *current;
	char *effective_root;
	int trailing_slash = 0;
	
	if (!path) {
		pseudo_diag("can't fix empty path.\n");
		return 0;
	}
	newpathlen = pseudo_path_max();
	if (!pathbufs[pathbuf]) {
		pathbufs[pathbuf] = malloc(newpathlen);
	}
	newpath = pathbufs[pathbuf];
	pathbuf = (pathbuf + 1) % PATHBUFS;
	pathlen = strlen(path);
	/* a trailing slash has special meaning, but processing
	 * trailing slashes is expensive.
	 */
	while (pathlen > 0 && path[pathlen - 1] == '/') {
		trailing_slash = 1;
		--pathlen;
	}
	/* allow a bit of slush.  overallocating a bit won't
	 * hurt.  rounding to 256's in the hopes that it makes life
	 * easier for the library.
	 */
	if (!newpath) {
		pseudo_diag("allocation failed seeking memory for path (%s).\n", path);
		return 0;
	}
	newpath[0] = '\0';
	current = newpath;
	if (baselen && (path[0] != '/' || rootlen)) {
		memcpy(current, base, baselen);
		current += baselen;
	}
	/* "root" is a pointer to the beginning of the *modifiable*
	 * part of the string; you can't back up over it.
	 */
	effective_root = newpath + rootlen;
	*current = '\0';
	/* at any given point:
	 * path is not slash-terminated
	 * current points to the null byte immediately after the path
	 * path points to the next element of path
	 * newpathlen is the total allocated length of newpath
	 * (current - newpath) is the used length of newpath
	 */
	int save_errno = errno;
	if (pseudo_append_elements(newpath, effective_root, newpathlen, &current, path, pathlen, leave_last, 0) != -1) {
		/* if we are expecting a trailing slash, or the path ended up being completely
		 * empty (meaning it's pointing at either effective_root or the beginning of
		 * the path), we need a slash here.
		 */
		if ((current == effective_root) || trailing_slash) {
			if ((current - newpath) < (int) newpathlen) {
				*current++ = '/';
				*current = '\0';
			}
		}
		pseudo_debug(PDBGF_PATH, "%s + %s => <%s>\n",
			base ? base : "<nil>",
			path ? path : "<nil>",
			newpath ? newpath : "<nil>");
		if (lenp) {
			*lenp = current - newpath;
		}
		errno = save_errno;
		return newpath;
	} else {
		errno = save_errno;
		return 0;
	}
}

/* remove the libpseudo stuff from the environment (leaving other preloads
 * alone).
 * There's an implicit memory leak here, but this is called only right
 * before an exec(), or at most once in a given run.
 *
 * we don't try to fix the library path.
 */
void pseudo_dropenv() {
	char *ld_preload = GETENV(PRELINK_LIBRARIES);

	if (ld_preload) {
		ld_preload = without_libpseudo(ld_preload);
		if (!ld_preload) {
			pseudo_diag("fatal: can't allocate new %s variable.\n", PRELINK_LIBRARIES);
		}
		if (ld_preload && strlen(ld_preload)) {
			SETENV(PRELINK_LIBRARIES, ld_preload, 1);
		} else {
			SETENV(PRELINK_LIBRARIES, "", 1);
		}
	}
}

char **
pseudo_dropenvp(char * const *envp) {
	char **new_envp;
	int i, j;

	for (i = 0; envp[i]; ++i) ;

	new_envp = malloc((i + 1) * sizeof(*new_envp));
	if (!new_envp) {
		pseudo_diag("fatal: can't allocate new environment.\n");
		return NULL;
	}

	j = 0;
	for (i = 0; envp[i]; ++i) {
		if (STARTSWITH(envp[i], PRELINK_LIBRARIES "=")) {
			char *new_val = without_libpseudo(envp[i]);
			if (!new_val) {
				pseudo_diag("fatal: can't allocate new environment variable.\n");
				return 0;
			} else {
				/* don't keep an empty value; if the whole string is
				 * PRELINK_LIRBARIES=, we just drop it. */
				if (strcmp(new_val, PRELINK_LIBRARIES "=")) {
					new_envp[j++] = new_val;
				}
			}
		} else {
			new_envp[j++] = envp[i];
		}
	}
	new_envp[j++] = NULL;
	return new_envp;
}

/* add pseudo stuff to the environment.
 */
void
pseudo_setupenv() {
	size_t i = 0;

	pseudo_debug(PDBGF_CLIENT, "setting up pseudo environment.\n");

	/* Make sure everything has been evaluated */
	free(pseudo_get_prefix(NULL));
	free(pseudo_get_bindir());
	free(pseudo_get_libdir());
	free(pseudo_get_localstatedir());

	while (pseudo_env[i].key) {
		if (pseudo_env[i].value) {
			SETENV(pseudo_env[i].key, pseudo_env[i].value, 0);
			pseudo_debug(PDBGF_ENV | PDBGF_VERBOSE, "pseudo_env: %s => %s\n",
				pseudo_env[i].key, pseudo_env[i].value);
		}
		i++;
	}

	const char *ld_library_path = GETENV(PRELINK_PATH);
	char *libdir_path = pseudo_libdir_path(NULL);
	if (!ld_library_path) {
		size_t len = strlen(libdir_path) + 1 + (strlen(libdir_path) + 2) + 1;
		char *newenv = malloc(len);
		if (!newenv) {
			pseudo_diag("fatal: can't allocate new %s variable.\n", PRELINK_PATH);
		}
		snprintf(newenv, len, "%s:%s64", libdir_path, libdir_path);
		SETENV(PRELINK_PATH, newenv, 1);
	} else if (!strstr(ld_library_path, libdir_path)) {
		size_t len = strlen(ld_library_path) + 1 + strlen(libdir_path) + 1 + (strlen(libdir_path) + 2) + 1;
		char *newenv = malloc(len);
		if (!newenv) {
			pseudo_diag("fatal: can't allocate new %s variable.\n", PRELINK_PATH);
		}
		snprintf(newenv, len, "%s:%s:%s64", ld_library_path, libdir_path, libdir_path);
		SETENV(PRELINK_PATH, newenv, 1);
	} else {
		/* nothing to do, ld_library_path exists and contains
		 * our preferred path */
	}

	char *ld_preload = GETENV(PRELINK_LIBRARIES);
	if (ld_preload) {
		ld_preload = with_libpseudo(ld_preload, libdir_path);
		if (!ld_preload) {
			pseudo_diag("fatal: can't allocate new %s variable.\n", PRELINK_LIBRARIES);
		}
		SETENV(PRELINK_LIBRARIES, ld_preload, 1);
		free(ld_preload);
	} else {
		ld_preload = with_libpseudo("", libdir_path);
		if (!ld_preload) {
			pseudo_diag("fatal: can't allocate new %s variable.\n", PRELINK_LIBRARIES);
		}
		SETENV(PRELINK_LIBRARIES, ld_preload, 1);
		free(ld_preload);
	}

	/* we kept libdir path until now because with_libpseudo might
	 * need it
	 */
	free(libdir_path);


#if PSEUDO_PORT_DARWIN
	char *force_flat = GETENV("DYLD_FORCE_FLAT_NAMESPACE");
	if (!force_flat) {
		SETENV("DYLD_FORCE_FLAT_NAMESPACE", "1", 1);
	}
#endif
}

/* add pseudo stuff to the environment.
 * We can't just use setenv(), because one use case is that we're trying
 * to modify the environment of a process about to be forked through
 * execve().
 */
char **
pseudo_setupenvp(char * const *envp) {
	char **new_envp;

	size_t i, j, k;
	size_t env_count = 0;

	size_t size_pseudoenv = 0;

	char *ld_preload = NULL, *ld_library_path = NULL;

	pseudo_debug(PDBGF_ENV, "setting up envp environment.\n");

	/* Make sure everything has been evaluated */
	free(pseudo_get_prefix(NULL));
	free(pseudo_get_bindir());
	free(pseudo_get_libdir());
	free(pseudo_get_localstatedir());

	for (i = 0; envp[i]; ++i) {
		if (STARTSWITH(envp[i], PRELINK_LIBRARIES "=")) {
			ld_preload = envp[i];
		}
		if (STARTSWITH(envp[i], PRELINK_PATH "=")) {
			ld_library_path = envp[i];
		}
		++env_count;
	}

	for (i = 0; pseudo_env[i].key; i++) {
		size_pseudoenv++;
	}

	env_count += size_pseudoenv; /* We're going to over allocate */

	j = 0;
	new_envp = malloc((env_count + 1) * sizeof(*new_envp));
	if (!new_envp) {
		pseudo_diag("fatal: can't allocate new environment.\n");
		return NULL;
	}	

	char *libdir_path = pseudo_libdir_path(NULL);
	if (!ld_library_path) {
		size_t len = strlen(PRELINK_PATH "=") + strlen(libdir_path) + 1 + (strlen(libdir_path) + 2) + 1;
		char *newenv = malloc(len);
		if (!newenv) {
			pseudo_diag("fatal: can't allocate new %s variable.\n", PRELINK_PATH);
		}
		snprintf(newenv, len, PRELINK_PATH "=%s:%s64", libdir_path, libdir_path);
		new_envp[j++] = newenv;
	} else if (!strstr(ld_library_path, libdir_path)) {
		size_t len = strlen(ld_library_path) + 1 + strlen(libdir_path) + 1 + (strlen(libdir_path) + 2) + 1;
		char *newenv = malloc(len);
		if (!newenv) {
			pseudo_diag("fatal: can't allocate new %s variable.\n", PRELINK_PATH);
		}
		snprintf(newenv, len, "%s:%s:%s64", ld_library_path, libdir_path, libdir_path);
		new_envp[j++] = newenv;
	} else {
		/* keep old value */
		new_envp[j++] = ld_library_path;
	}

	if (ld_preload) {
		ld_preload = with_libpseudo(ld_preload, libdir_path);
		if (!ld_preload) {
			pseudo_diag("fatal: can't allocate new %s variable.\n", PRELINK_LIBRARIES);
		}
		new_envp[j++] = ld_preload;
	} else {
		ld_preload = with_libpseudo("", libdir_path);
		size_t len = strlen(PRELINK_LIBRARIES "=") + strlen(ld_preload) + 1;
		char *newenv = malloc(len);
		snprintf(newenv, len, PRELINK_LIBRARIES "=%s", ld_preload);
		new_envp[j++] = newenv;
		free(ld_preload);
	}

	free(libdir_path);

	for (i = 0; envp[i]; ++i) {
		if (STARTSWITH(envp[i], PRELINK_LIBRARIES "=")) continue;
		if (STARTSWITH(envp[i], PRELINK_PATH "=")) continue;
		new_envp[j++] = envp[i];
	}

	for (i = 0; pseudo_env[i].key; i++) {
		int found = 0;
		for (k = 0; k < j; k++) {
			if (!strncmp(pseudo_env[i].key,new_envp[k],strlen(pseudo_env[i].key))) {
				found = 1;
				break;
			}
		}
		if (!found && pseudo_env[i].key && pseudo_env[i].value) {
			size_t len = strlen(pseudo_env[i].key) + 1 + strlen(pseudo_env[i].value) + 1;
			char *newenv = malloc(len);
			if (!newenv) {
				pseudo_diag("fatal: can't allocate new variable.\n");
			}
			snprintf(newenv, len, "%s=%s", pseudo_env[i].key, pseudo_env[i].value);
			new_envp[j++] = newenv;
		}
	}
	new_envp[j++] = NULL;
	return new_envp;
}

/* Append the file value to the prefix value. */
char *
pseudo_append_path(const char * prefix, size_t prefix_len, char *file) {
	char *path;

	if (!file) {
		return strdup(prefix);
	} else {
		size_t len = prefix_len + strlen(file) + 2;
		path = malloc(len);
		if (path) {
			char *endptr;
			int rc;

			rc = snprintf(path, len, "%s", prefix);
			/* this certainly SHOULD be impossible */
			if ((size_t) rc >= len)
				rc = len - 1;
			endptr = path + rc;
			/* strip extra slashes.
			 * This probably has no real effect, but I don't like
			 * seeing "//" in paths.
			 */
			while ((endptr > path) && (endptr[-1] == '/'))
				--endptr;
			snprintf(endptr, len - (endptr - path), "/%s", file);
		}
		return path;
	}
}


/* get the full path to a file under $PSEUDO_PREFIX.  Other ways of
 * setting the prefix all set it in the environment.
 */
char *
pseudo_prefix_path(char *file) {
	char * rc;
	char * prefix = pseudo_get_prefix(NULL);

	if (!prefix) {
		pseudo_diag("You must set the PSEUDO_PREFIX environment variable to run pseudo.\n");
		exit(1);
	}

	rc = pseudo_append_path(prefix, strlen(prefix), file);	
	free(prefix);

	return rc;
}

/* get the full path to a file under $PSEUDO_BINDIR. */
char *
pseudo_bindir_path(char *file) {
	char * rc;
	char * bindir = pseudo_get_bindir();

	if (!bindir) {
		pseudo_diag("You must set the PSEUDO_BINDIR environment variable to run pseudo.\n");
		exit(1);
	}

	rc = pseudo_append_path(bindir, strlen(bindir), file);	
	free(bindir);

	return rc;
}

/* get the full path to a file under $PSEUDO_LIBDIR. */
char *
pseudo_libdir_path(char *file) {
	char * rc;
	char * libdir = pseudo_get_libdir();

	if (!libdir) {
		pseudo_diag("You must set the PSEUDO_LIBDIR environment variable to run pseudo.\n");
		exit(1);
	}

	rc = pseudo_append_path(libdir, strlen(libdir), file);	
	free(libdir);

	return rc;
}

/* get the full path to a file under $PSEUDO_LOCALSTATEDIR. */
char *
pseudo_localstatedir_path(char *file) {
	char * rc;
	char * localstatedir = pseudo_get_localstatedir();

	if (!localstatedir) {
		pseudo_diag("You must set the PSEUDO_LOCALSTATEDIR environment variable to run pseudo.\n");
		exit(1);
	}

	rc = pseudo_append_path(localstatedir, strlen(localstatedir), file);	
	free(localstatedir);

	return rc;
}

char *
pseudo_get_prefix(char *pathname) {
	char *s = pseudo_get_value("PSEUDO_PREFIX");

	/* Generate the PSEUDO_PREFIX if necessary, and possible... */
	if (!s && pathname) {
		char mypath[pseudo_path_max()];
		char *dir;
		char *tmp_path;

		if (pathname[0] == '/') {
			snprintf(mypath, pseudo_path_max(), "%s", pathname);
			s = mypath + strlen(mypath);
		} else {
			if (!getcwd(mypath, pseudo_path_max())) {
				mypath[0] = '\0';
			}
			s = mypath + strlen(mypath);
			s += snprintf(s, pseudo_path_max() - (s - mypath), "/%s",
				pathname);
		}
		tmp_path = pseudo_fix_path(NULL, mypath, 0, 0, 0, AT_SYMLINK_NOFOLLOW);
		/* point s to the end of the fixed path */
		if ((int) strlen(tmp_path) >= pseudo_path_max()) {
			pseudo_diag("Can't expand path '%s' -- expansion exceeds %d.\n",
				mypath, (int) pseudo_path_max());
		} else {
			s = mypath + snprintf(mypath, pseudo_path_max(), "%s", tmp_path);
		}

		while (s > (mypath + 1) && *s != '/')
			--s;
		*s = '\0';
		dir = s - 1;
		while (dir > mypath && *dir != '/') {
			--dir;
		}
		/* strip bin directory, if any */
		if (!strncmp(dir, "/bin", 4)) {
			*dir = '\0';
		}
		/* degenerate case: /bin/pseudo should yield a pseudo_prefix "/" */
		if (*mypath == '\0') {
			strcpy(mypath, "/");
		}

		pseudo_diag("Warning: PSEUDO_PREFIX unset, defaulting to %s.\n",
			mypath);
		pseudo_set_value("PSEUDO_PREFIX", mypath);
		s = pseudo_get_value("PSEUDO_PREFIX");
	}
	return s;
}

char *
pseudo_get_bindir(void) {
	char *s = pseudo_get_value("PSEUDO_BINDIR");
	if (!s) {
		char *pseudo_bindir = pseudo_prefix_path(PSEUDO_BINDIR);
		if (pseudo_bindir) {
			pseudo_set_value("PSEUDO_BINDIR", pseudo_bindir);
			s = pseudo_bindir;
		}
	}
	return s;
}

char *
pseudo_get_libdir(void) {
	char *s = pseudo_get_value("PSEUDO_LIBDIR");
	if (!s) {
		char *pseudo_libdir;
		pseudo_libdir = pseudo_prefix_path(PSEUDO_LIBDIR);
		if (pseudo_libdir) {
			pseudo_set_value("PSEUDO_LIBDIR", pseudo_libdir);
			s = pseudo_libdir;
		}
	}
#if PSEUDO_PORT_DARWIN
	/* on Darwin, we need lib64, because dyld won't search */
#else
	/* If we somehow got lib64 in there, clean it down to just lib... */
	if (s) {
		size_t len = strlen(s);
		if (s[len-2] == '6' && s[len-1] == '4') {
			s[len-2] = '\0';
			pseudo_set_value("PSEUDO_LIBDIR", s);
		}
	}
#endif

	return s;
}

char *
pseudo_get_localstatedir() {
	char *s = pseudo_get_value("PSEUDO_LOCALSTATEDIR");
	if (!s) {
		char *pseudo_localstatedir = pseudo_prefix_path(PSEUDO_LOCALSTATEDIR);
		if (pseudo_localstatedir) {
			pseudo_set_value("PSEUDO_LOCALSTATEDIR", pseudo_localstatedir);
			s = pseudo_localstatedir;
		}
	}
	return s;
}

/* these functions define the sizes pseudo will try to use
 * when trying to allocate space, or guess how much space
 * other people will have allocated; see the GNU man page
 * for realpath(3) for an explanation of why the sys_path_max
 * functions exists, approximately -- it's there to be a size
 * that I'm pretty sure the user will have allocated if they
 * provided a buffer to that defective function.
 */
/* I'm pretty sure this will be larger than real PATH_MAX */
#define REALLY_BIG_PATH 16384
/* A likely common value for PATH_MAX */
#define SORTA_BIG_PATH 4096
ssize_t
pseudo_path_max(void) {
	if (pseudo_max_pathlen == -1) {
		long l = pathconf("/", _PC_PATH_MAX);
		if (l < 0) {
			if (_POSIX_PATH_MAX > 0) {
				pseudo_max_pathlen = _POSIX_PATH_MAX;
			} else {
				pseudo_max_pathlen = REALLY_BIG_PATH;
			}
		} else {
			if (l <= REALLY_BIG_PATH) {
				pseudo_max_pathlen = l;
			} else {
				pseudo_max_pathlen = REALLY_BIG_PATH;
			}
		}
	}
	return pseudo_max_pathlen;
}

ssize_t
pseudo_sys_path_max(void) {
	if (pseudo_sys_max_pathlen == -1) {
		long l = pathconf("/", _PC_PATH_MAX);
		if (l < 0) {
			if (_POSIX_PATH_MAX > 0) {
				pseudo_sys_max_pathlen = _POSIX_PATH_MAX;
			} else {
				pseudo_sys_max_pathlen = SORTA_BIG_PATH;
			}
		} else {
			if (l <= SORTA_BIG_PATH) {
				pseudo_sys_max_pathlen = l;
			} else {
				pseudo_sys_max_pathlen = SORTA_BIG_PATH;
			}
		}
	}
	return pseudo_sys_max_pathlen;
}

/* complicated because in theory you can have modes like * 'ab+'
 * which is the same as 'a+' in POSIX.  The first letter really does have
 * to be one of r, w, a, though.
 */
int
pseudo_access_fopen(const char *mode) {
	int access = 0;
	for (; *mode; ++mode) {
		switch (*mode) {
		case 'a':
			access |= (PSA_APPEND | PSA_WRITE);
			break;
		case 'r':
			access |= PSA_READ;
			break;
		case 'w':
			access |= PSA_WRITE;
			break;
		case 'x':
			/* special case -- note that this conflicts with a
			 * rarely-used glibc extension
			 */
			access |= PSA_EXEC;
			break;
		case 'b':			/* binary mode */
			break;
		case 'c': case 'e': case 'm':	/* glibc extensions */
			break;
		case '+':
			/* one of these will already be set, presumably */
			access |= (PSA_READ | PSA_WRITE);
			break;
		default:
			access = -1;
			break;
		}
	}
	return access;
}

/* find a passwd/group file to use
 * uses in order:
 * - PSEUDO_CHROOT/etc/<file> (only if CHROOT is set)
 * - PSEUDO_PASSWD/etc/<file>
 * - /etc/<file>
 */

#if PSEUDO_PORT_DARWIN
/* on Darwin, you can't just use /etc/passwd for system lookups,
 * you have to use the real library calls because they know about
 * Directory Services.  So...
 *
 * We make up fake fds and FILE * objects that can't possibly be
 * valid.
 */
int pseudo_host_etc_passwd_fd = -3;
int pseudo_host_etc_group_fd = -4;
static FILE pseudo_fake_passwd_file;
static FILE pseudo_fake_group_file;
FILE *pseudo_host_etc_passwd_file = &pseudo_fake_passwd_file;
FILE *pseudo_host_etc_group_file = &pseudo_fake_group_file;

#endif

int
pseudo_etc_file(const char *file, char *realname, int flags, const char **search_dirs, int dircount) {
	char filename[pseudo_path_max()];
	int rc = -1;

	if (!file) {
		pseudo_debug(PDBGF_CHROOT, "pseudo_etc_file: needs argument, usually passwd/group\n");
		errno = ENOENT;
		return -1;
	}
	int i;
	if (!search_dirs || dircount == 0) {
		pseudo_debug(PDBGF_CHROOT, "pseudo_etc_file: no search dirs.\n");
		errno = ENOENT;
		return -1;
	}
	for (i = 0; i < dircount; ++i) {
		const char *s = search_dirs[i];
		/* we used to pass in some paths as NULL when unset,
		 * so we skipped those. Now NULL entries don't get
		 * put in, so the only NULL should be the sentinel
		 * value, and this should never get hit.
		 *
		 * "should" is not comforting to me.
		 */
		if (!s)
			break;
#if PSEUDO_PORT_DARWIN
		/* special magic: empty string implies our emulation
		 * of the passwd/group files.
		 */
		if (!*s) {
			if (!strcmp("passwd", file)) {
				pseudo_debug(PDBGF_CHROOT, "Darwin hackery: pseudo_etc_passwd returning magic passwd fd\n");
				return pseudo_host_etc_passwd_fd;
			} else if (!strcmp("group", file)) {
				pseudo_debug(PDBGF_CHROOT, "Darwin hackery: pseudo_etc_passwd returning magic group fd\n");
				return pseudo_host_etc_group_fd;
			}
		}
#endif
		snprintf(filename, pseudo_path_max(), "%s/etc/%s",
			s, file);
		rc = open(filename, flags, 0600);
		if (rc >= 0) {
			if (realname)
				strcpy(realname, filename);
			pseudo_debug(PDBGF_CHROOT, "pseudo_etc_file: using '%s' for '%s'.\n",
				filename, file);
			return rc;
		} else {
			pseudo_debug(PDBGF_CHROOT | PDBGF_VERBOSE, "didn't find <%s>\n",
				filename);
		}
	}
	return rc;
}

/* set up a log file */
static int
pseudo_logfile(char *filename, char *defname, int prefer_fd) {
	char *pseudo_path;
	char *s;
#if PSEUDO_PORT_LINUX
	extern char *program_invocation_short_name; /* glibcism */
#else
	char *program_invocation_short_name = "unknown";
#endif
	int fd;

	if (!filename) {
		if (!defname) {
			pseudo_debug(PDBGF_INVOKE, "no special log file requested, using stderr.\n");
			return -1;
		}
		pseudo_path = pseudo_localstatedir_path(defname);
		if (!pseudo_path) {
			pseudo_diag("can't get path for prefix/%s\n", PSEUDO_LOGFILE);
			return -1;
		}
	} else {
		char *pid = NULL, *prog = NULL;
		size_t len;
		for (s = filename; *s; ++s) {
			if (s[0] == '%') {
				switch (s[1]) {
				case '%': /* skip the %% */
					++s;
					break;
				case 'd':
					if (pid) {
						pseudo_diag("found second %%d in PSEUDO_DEBUG_FILE, ignoring.\n");
						return -1;
					} else {
						pid = s;
					}
					break;
				case 's':
					if (prog) {
						pseudo_diag("found second %%s in PSEUDO_DEBUG_FILE, ignoring.\n");
						return -1;
					} else {
						prog = s;
					}
					break;
				default:
					if (isprint(s[1])) {
						pseudo_diag("found unknown format character '%c' in PSEUDO_DEBUG_FILE, ignoring.\n",
							s[1]);
					} else {
						pseudo_diag("found unknown format character '\\x%02x' in PSEUDO_DEBUG_FILE, ignoring.\n",
							(unsigned char) s[1]);
					}
					return -1;
					break;
				}
			}
		}
		len = strlen(filename) + 1;
		if (pid)
			len += 8;
		if (prog)
			len += strlen(program_invocation_short_name);
		pseudo_path = malloc(len);
		if (!pseudo_path) {
			pseudo_diag("can't allocate space for debug file name.\n");
			return -1;
		}
		if (pid && prog) {
			if (pid < prog) {
				snprintf(pseudo_path, len, filename, getpid(), program_invocation_short_name);
			} else {
				snprintf(pseudo_path, len, filename, program_invocation_short_name, getpid());
			}
		} else if (pid) {
			snprintf(pseudo_path, len, filename, getpid());
		} else if (prog) {
			snprintf(pseudo_path, len, filename, program_invocation_short_name);
		} else {
			strcpy(pseudo_path, filename);
		}
		free(filename);
	}	
	fd = open(pseudo_path, O_WRONLY | O_APPEND | O_CREAT, 0644);
	if (fd == -1) {
		pseudo_diag("help: can't open log file %s: %s\n", pseudo_path, strerror(errno));
	} else {
		/* try to force fd to prefer_fd.  We do this because glibc's malloc
		 * debug unconditionally writes to fd 2, and we don't want
		 * a client process ending op on fd 2, or server debugging
		 * becomes a nightmare. So, server sets prefer_fd to 2. Client
		 * leaves it at -1.
		 */
		if (prefer_fd >= 0 && fd != prefer_fd) {
			int newfd;
			close(prefer_fd);
			newfd = dup2(fd, prefer_fd);
			if (newfd != -1) {
				fd = newfd;
			}
		}
		pseudo_util_debug_fd = fd;
	}
	free(pseudo_path);
	return fd;
}

int
pseudo_debug_logfile(char *defname, int prefer_fd) {
	char *filename = pseudo_get_value("PSEUDO_DEBUG_FILE");
	int fd;

	fd = pseudo_logfile(filename, defname, prefer_fd);
	if (fd > -1) {
		pseudo_diag("debug_logfile: fd %d\n", fd);
		pseudo_util_debug_fd = fd;
		return 0;
	}
	return 1;
}

int
pseudo_evlog_logfile(char *defname, int prefer_fd) {
	char *filename = pseudo_get_value("PSEUDO_EVLOG_FILE");
	int fd;

	fd = pseudo_logfile(filename, defname, prefer_fd);
	if (fd > -1) {
		pseudo_util_evlog_fd = fd;
		return 0;
	}
	return 1;
}

void
pseudo_stat32_from64(struct stat *buf32, const struct stat64 *buf) {
	buf32->st_dev = buf->st_dev;
	buf32->st_ino = buf->st_ino;
	buf32->st_mode = buf->st_mode;
	buf32->st_nlink = buf->st_nlink;
	buf32->st_uid = buf->st_uid;
	buf32->st_gid = buf->st_gid;
	buf32->st_rdev = buf->st_rdev;
	buf32->st_size = buf->st_size;
	buf32->st_blksize = buf->st_blksize;
	buf32->st_blocks = buf->st_blocks;
	buf32->st_atime = buf->st_atime;
	buf32->st_mtime = buf->st_mtime;
	buf32->st_ctime = buf->st_ctime;
}

void
pseudo_stat64_from32(struct stat64 *buf64, const struct stat *buf) {
	buf64->st_dev = buf->st_dev;
	buf64->st_ino = buf->st_ino;
	buf64->st_mode = buf->st_mode;
	buf64->st_nlink = buf->st_nlink;
	buf64->st_uid = buf->st_uid;
	buf64->st_gid = buf->st_gid;
	buf64->st_rdev = buf->st_rdev;
	buf64->st_size = buf->st_size;
	buf64->st_blksize = buf->st_blksize;
	buf64->st_blocks = buf->st_blocks;
	buf64->st_atime = buf->st_atime;
	buf64->st_mtime = buf->st_mtime;
	buf64->st_ctime = buf->st_ctime;
}

/* pretty-dump some data.
 * expects to be called using pseudo_debug_call() so it doesn't have
 * to do debug checks.
 */
void
pseudo_dump_data(char *name, const void *v, size_t len) {
	char hexbuf[128];
	char asciibuf[32];
	const unsigned char *base = v;
	const unsigned char *data = base;
	int remaining = len;
	pseudo_diag("%s at %p [%d byte%s]:\n",
		name ? name : "data", v, (int) len, len == 1 ? "" : "s");
	while (remaining > 0) {
		char *hexptr = hexbuf;
		char *asciiptr = asciibuf;
		for (int i = 0; i < 16 && i < remaining; ++i) {
			hexptr += snprintf(hexptr, 4, "%02x ", data[i]);
			if (isprint(data[i])) {
				*asciiptr++ = data[i];
			} else {
				*asciiptr++ = '.';
			}
			if (i % 4 == 3) {
				*hexptr++ = ' ';
			}
		}
		*hexptr = '\0';
		*asciiptr = '\0';
		pseudo_diag("0x%06x %-50.50s '%.16s'\n",
			(int) (data - base),
			hexbuf, asciibuf);
		remaining = remaining - 16;
		data = data + 16;
	}
}
