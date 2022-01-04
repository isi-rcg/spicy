#ifndef _LOGMESSAGE_H
#define _LOGMESSAGE_H

/* These include files are for the strerror(errno) replacement for '%m' format option of syslog. */

#include <errno.h>
#include <string.h>

/*
 * We need the LOG_? values used for log_message() so either include <syslog.h>
 * or we include manually define them as an alternative.
 */

#ifdef USE_SYSLOG
#include <syslog.h>
#else
#define	LOG_EMERG	0	/* system is unusable */
#define	LOG_ALERT	1	/* action must be taken immediately */
#define	LOG_CRIT	2	/* critical conditions */
#define	LOG_ERR		3	/* error conditions */
#define	LOG_WARNING	4	/* warning conditions */
#define	LOG_NOTICE	5	/* normal but significant condition */
#define	LOG_INFO	6	/* informational */
#define	LOG_DEBUG	7	/* debug-level messages */
#endif /* !USE_SYSLOG */

/*
 * Define exit status for fatal_error() calls (from sundries.h originally).
 * Bits below are ORed.
 */

#ifndef EX_USAGE
#define EX_USAGE		1	/* incorrect invocation or permission */
#define EX_SYSERR		2	/* out of memory, cannot fork, ... */
#define EX_SOFTWARE		4	/* internal mount bug or wrong version */
#define EX_USER			8	/* user interrupt */
#define EX_FILEIO		16	/* problems writing, locking, ... mtab/fstab */
#define EX_FAIL			32	/* mount failure */
#define EX_SOMEOK		64	/* some mount succeeded */

#define EX_BG			256	/* retry in background (internal only) */
#endif /*EX_USAGE*/

/* Define bit-values for the 'flags' in open_logging() to decide where messages go. */
#define MSG_TO_STDERR 1
#define MSG_TO_SYSLOG 2

/*
 * Enable the printf() format string checking of gcc:
 * http://gcc.gnu.org/onlinedocs/gcc-3.2/gcc/Function-Attributes.html
 */

#ifndef PRINTF_STYLE
#if defined( __GNUC__ )
#define PRINTF_STYLE(Fmt, FirstArg) __attribute__ ((format (printf, Fmt, FirstArg)))
#else
#define PRINTF_STYLE(Fmt, FirstArg)
#endif /* !__GNUC__ */
#endif /* PRINTF_STYLE */

/** logmessage.c **/
int  open_logging(const char *name, int flags);
int  log_message(int level,		const char *fmt, ...) PRINTF_STYLE(2, 3);
void fatal_error(int exitcode,	const char *fmt, ...) PRINTF_STYLE(2, 3);
int  close_logging(void);

int suspend_logging(void);
int resume_logging(void);

#endif /*_LOGMESSAGE_H */
