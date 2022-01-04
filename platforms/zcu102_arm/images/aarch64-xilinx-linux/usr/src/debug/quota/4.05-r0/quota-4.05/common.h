/*
 *
 *	Various things common for all utilities
 *
 */

#ifndef GUARD_COMMON_H
#define GUARD_COMMON_H

#include <time.h>

#ifndef __attribute__
# if !defined __GNUC__ || __GNUC__ < 2 || (__GNUC__ == 2 && __GNUC_MINOR__ < 8) || __STRICT_ANSI__
#  define __attribute__(x)
# endif
#endif

/* Name of current program for error reporting */
extern char *progname;

/* Finish programs being */
void __attribute ((noreturn)) die(int, char *, ...) __attribute__ ((__format__ (__printf__, 2, 3)));

/* Print an error */
void errstr(char *, ...) __attribute__ ((__format__ (__printf__, 1, 2)));

/* If use_syslog is called, all error reports using errstr() and die() are
 * written to syslog instead of stderr */
void use_syslog();

/* malloc() with error check */
void *smalloc(size_t);

/* realloc() with error check */
void *srealloc(void *, size_t);

/* Safe strncpy - always finishes string */
void sstrncpy(char *, const char *, size_t);

/* Safe strncat - always finishes string */
void sstrncat(char *, const char *, size_t);

/* Safe version of strdup() */
char *sstrdup(const char *s);

/* Print version string */
void version(void);

/* Desired output unit */
enum s2s_unit {
	S2S_NONE = 0,
	S2S_KB,
	S2S_MB,
	S2S_GB,
	S2S_TB,
	S2S_AUTO,
	S2S_INVALID
};

/* Compare two times */
int timespec_cmp(struct timespec *a, struct timespec *b);

/* Convert command line option to desired output unit */
int unitopt2unit(char *opt, enum s2s_unit *space_unit, enum s2s_unit *inode_unit);

#endif /* GUARD_COMMON_H */
