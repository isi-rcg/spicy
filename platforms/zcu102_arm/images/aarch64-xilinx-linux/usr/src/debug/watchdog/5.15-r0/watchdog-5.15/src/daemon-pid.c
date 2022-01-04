/* > daemon-pid.c
 *
 * Write the PID of the running daemon to its file.
 *
 * BUGS / TO DO: The checking then writing is not atomic, so there is a small
 * chance of two processes thinking they have got exclusive use of the PID file,
 * however, that is a small probability in practice.
 *
 * (c) 2013 Paul S. Crawford (psc@sat.dundee.ac.uk) under GPL v2 license
 * based on existing code.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>	/* For kill() */
#include <sys/types.h>
#include <sys/wait.h> /* For waitpid() */
#include <unistd.h>

#include "extern.h"

/* Set when write_pid_file() is called. */
pid_t daemon_pid = 0;

/* Used to keep a note of the file we used. */
static char *saved_fname = NULL;

/*
 * Check to see if the PID file exists and is in-use by an active process.
 */

static int check_pid_file(const char *fname)
{
	int pid = 0;
	FILE *fp = fopen(fname, "r");

	if (fp != NULL) {
		/* File exists, read its contents. */
		if (fscanf(fp, " %d", &pid) != 1) {
			pid = 0;
		}

		fclose(fp);

		if (pid != 0 && pid != (int)daemon_pid) {
			/* PID already written, but is it another active process? */
			if (kill(pid, 0) == 0) {
				log_message(LOG_WARNING, "PID file %s already used by PID=%d", fname, pid);
				return -1;
			}
		}
	}

	return 0;
}

/*
 * Code to find the current process' PID and write it to the PID file. This should be
 * called just AFTER you daemonize the process (so it gets the correct PID) and BEFORE you
 * lock the process memory (as that function needs the PID as well).
 *
 * Return value is zero if OK, or -1 for error (null name, PID file in use, or can't write).
 */

int write_pid_file(const char *fname)
{
	FILE *fp;
	int rv = 0;
	daemon_pid = getpid();

	if (fname == NULL || check_pid_file(fname))
		return -1;

	/* Remove any previous file we used, and free its name. */
	remove_pid_file();

	fp = fopen(fname, "w");
	if (fp != NULL) {
		fprintf(fp, "%d\n", daemon_pid);
		fclose(fp);
		saved_fname = xstrdup(fname);
	} else {
		rv = -1;
		log_message(LOG_ERR, "cannot open PID file %s (%s)", fname, strerror(errno));
	}

	return rv;
}

/*
 * Remove the PID file we previously wrote our PID value to. Can be called anywhere,
 * and multiple times, but really should be limited to when the daemon is exiting, and
 * preferably just before syslog is disconnected so any errors can be logged.
 */

int remove_pid_file(void)
{
	int rv = 0;

	if (saved_fname != NULL) {
		if (unlink(saved_fname) < 0) {
			log_message(LOG_ERR, "cannot remove PID file %s (%s)", saved_fname, strerror(errno));
			rv = -1;
		}
		free(saved_fname);
	}

	saved_fname = NULL;
	return rv;
}

/*
 * Function to perform the double-fork daemonization with the same sort of
 * calling and behaviour as BSD' daemon() function.
 */
int wd_daemon(int nochdir, int noclose)
{
	pid_t child_pid;
	/* Become a daemon process: */
	/* make sure we're on the root partition */
	if (!nochdir) {
		if (chdir("/") < 0) {
			return (-1);
		}
	}

	/* flush any stdout/stderr message before we fork */
	fflush(stdout);
	fflush(stderr);

	/* fork to go into the background */
	if ((child_pid = fork()) < 0) {
		return (-1);
	} else if (child_pid > 0) {
		/* fork was okay          */
		/* wait for child to exit */
		if (waitpid(child_pid, NULL, 0) != child_pid) {
			_exit(1);
		}
		/* and exit myself */
		_exit(0);
	}
	/* and fork again to make sure we inherit all rights from init */
	if ((child_pid = fork()) < 0) {
		_exit(1);
	} else if (child_pid > 0) {
		/* fork was OK, give child time to write PID file. */
		usleep(10000);
		_exit(0);
	}
	/* now we're free */

	/* Okay, we're a daemon     */
	/* but we're still attached to the tty */
	/* create our own session */
	if (setsid() == -1)
		return (-1);

	/* As daemon we don't normally do any console IO */
	if (!noclose) {
		const char *null_name = "/dev/null";

		if (freopen(null_name, "r", stdin) == NULL) {
			return (-1);
		}

		if (freopen(null_name, "w", stdout) == NULL) {
			return (-1);
		}

		if (freopen(null_name, "w", stderr) == NULL) {
			return (-1);
		}
	}

	return 0;
}
