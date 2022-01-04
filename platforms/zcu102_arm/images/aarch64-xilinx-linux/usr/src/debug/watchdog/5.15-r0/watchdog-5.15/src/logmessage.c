/* > logmessage.c
 *
 * Code for creating messages and sending them to stderr and/or to syslog.
 * Also has fatal_error() function to the same then exit.
 *
 * NOTE: We can't use malloc() here as one reason for a call could be the
 * out-of-memory condition, so we use a modest stack-based buffer for the
 * string "printing" before dumping it to the terminal and/or syslog.
 *
 * (c) 2013 Paul S. Crawford (psc@sat.dundee.ac.uk) licensed under GPL v2
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>

#include "logmessage.h"

#define MAX_MESSAGE		2048
#define MAX_PROG_NAME	256

static int output_message(int level, char *buf);

static int using_syslog = 0;
static int using_terminal = 0;
static char progname[MAX_PROG_NAME];

static int  err_count = 0;
static int  err_level = LOG_DEBUG;
static char err_buf[MAX_MESSAGE];

/*
 * Prepare for message printing.
 *
 * On the 1st call to this should include the program's name (e.g. argv[])
 * but after that you can use NULL.
 *
 * The integer flags enable/disable output to either the terminal (via 'stderr')
 * or to syslog (assuming it is compiled as such, otherwise terminal as well).
 */

int open_logging(const char *name, int flags)
{
	int rv = 0;

	if (name != NULL)
		strncpy(progname, name, sizeof(progname) - 1);

	err_count = 0;
	using_terminal = (flags & MSG_TO_STDERR);

	if (flags & MSG_TO_SYSLOG) {
		if (!using_syslog) {
#if USE_SYSLOG
			openlog(progname, LOG_PID, LOG_DAEMON);
#endif /*USE_SYSLOG */
			using_syslog = 1;	/* Future messages to syslog. */
		}
	} else {
		close_logging();
	}

	return rv;
}

/*
 * Output a message with a given priority level. Used internally for both
 * log_message() and fatal_error() calls. When using syslog we can output
 * twice, but without syslog either mode is directed to the terminal once.
 */

static int output_message(int level, char *buf)
{
	FILE *fp = stderr;
	int rv = 0;

#if USE_SYSLOG
	if (using_syslog && err_count == 0) {
		syslog(level, "%s", buf);
	} else {
		/* In 'suspend' mode, copy message so can output on 'resume'. */
		err_count++;
		err_level = level;
		strncpy(err_buf, buf, sizeof(err_buf)-1);
	}

	if (using_terminal) {
#else
	if (using_terminal || using_syslog) {
#endif	/* !USE_SYSLOG */
		rv = fprintf(fp, "%s: %s\n", progname, buf);
		if(rv < 0 || fflush(fp)) {
			/* Error writing out to terminal - don't bother trying again. */
			using_terminal = 0;
#if USE_SYSLOG && 0
			syslog(LOG_WARNING, "failed writing message terminal (rv=%d, errno='%s')", rv, strerror(errno));
#endif /* USE_SYSLOG */
		}
	}

	return rv;
}

/*
 * Log a message to syslog and/or the terminal.
 *
 * NOTE: Unlike syslog you can't use '%m' formatting for error codes, instead use
 * the string '%s' and give it strerror(errno) as an argument.
 */

int log_message(int level, const char *fmt, ...)
{
	int rv = 0;
	char buf[MAX_MESSAGE];
	va_list args;

	memset(buf, 0, sizeof(buf));

	va_start(args, fmt);
#if _BSD_SOURCE || _XOPEN_SOURCE >= 500 || _ISOC99_SOURCE || _POSIX_C_SOURCE >= 200112L
	vsnprintf(buf, sizeof(buf) - 1, fmt, args);
#else
	vsprintf(buf, fmt, args);
#endif
	va_end(args);

	rv = output_message(level, buf);

	return rv;
}

/*
 * Function to log a message then exit program with a given error code.
 */

void fatal_error(int exitcode, const char *fmt, ...)
{
	char buf[MAX_MESSAGE];
	va_list args;

	memset(buf, 0, sizeof(buf));

	va_start(args, fmt);
#if _BSD_SOURCE || _XOPEN_SOURCE >= 500 || _ISOC99_SOURCE || _POSIX_C_SOURCE >= 200112L
	vsnprintf(buf, sizeof(buf) - 1, fmt, args);
#else
	vsprintf(buf, fmt, args);
#endif
	va_end(args);

	output_message(LOG_ERR, buf);
	close_logging();

#if defined(DEBUG)
	/*
	 * This should trigger a core dump when in debug mode, allowing trace-back
	 * to find out why we were leaving unexpectedly.
	 */
	assert(!buf);
#endif /*DEBUG*/
	exit(exitcode);
}

/*
 * Stop any logging via syslog.
 *
 * Return is 0 is stopped, or -1 if not in use.
 *
 * Note you don't need to use this, it is also possible to call something
 * like open_logging(NULL, MSG_TO_STDERR) to close syslog.
 */

int close_logging(void)
{
	int rv = -1;

	/* Log the closing message */
	if (using_syslog) {
#if USE_SYSLOG
		closelog();
#endif /* USE_SYSLOG */
		using_syslog = 0;
		rv = 0;
	}

	return rv;
}

/*
 * Calling this function will stop any syslog output, and resume_logging() will start
 * it again. Unlike the usual options, this is not closing the syslog connection and it
 * will output of the last (if any) message generated during the suspended period on
 * resumption (again, only if syslog is already in use).
 */

int suspend_logging(void)
{
	if (err_count == 0) {
		err_count++; /* Start at 1 */
	}
	return 0;
}

/*
 * Allow syslog output again (if it was in use) and send the last message during the
 * suspended period (if any).
 */

int resume_logging(void)
{
	int rv = 0;
	if (err_count && using_syslog) {
		/* We start at 1 so remove that. */
		err_count--;
		if (err_count == 1) {
			/* Exactly one message sent, output as if nothing happened. */
			syslog(err_level, "%s", err_buf);
		} else if (err_count > 1) {
			/* More than one needed, but we only buffer one, so report loss. */
			syslog(LOG_WARNING, "had %d messages in log suspend, last: %s", err_count, err_buf);
			rv = -1;
		}
	}
	err_count = 0;
	return rv;
}
