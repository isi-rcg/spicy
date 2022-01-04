#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <linux/limits.h>

#include "extern.h"
#include "watch_err.h"

#define TEST_RUNNING	0
#define TEST_COMPLETED	1
#define TEST_BLANK		2

struct process {
	char proc_name[PATH_MAX];
	pid_t pid;
	time_t time;
	int ecode;
	int is_done;
	struct process *next;
};

static struct process *process_head = NULL;

/*
 * Add a process to the list. We index by PID primarily to act on child exit
 * values, but check the process name when attempting to start a new child.
 */

static int add_process(const char *name, pid_t pid)
{
	struct process *node = (struct process *)malloc(sizeof(struct process));

	if (node == NULL) {
		log_message(LOG_ALERT, "out of memory adding test binary");
		free_process();
		return (ENOMEM);
	}

	snprintf(node->proc_name, sizeof(node->proc_name), "%s", name);
	node->pid = pid;
	node->time = time(NULL);
	node->ecode = 0;
	node->is_done = FALSE;
	node->next = process_head;
	process_head = node;

	return (ENOERR);
}

/*
 * Free the whole chain. Used on out-of-memory case to hopefully to have enough
 * heap left to create the process kill-list for an orderly shut-down.
 */

void free_process(void)
{
	struct process *last, *current;
	current = process_head;

	while (current != NULL) {
		last = current;
		current = current->next;
		free(last);
	}

	process_head = NULL;
}

/*
 * Remove a finished process from the list, indexed by PID.
 */

static void remove_process(pid_t pid)
{
	struct process *last, *current;
	last = NULL;
	current = process_head;
	while (current != NULL && current->pid != pid) {
		last = current;
		current = current->next;
	}
	if (current != NULL) {
		if (last == NULL)
			process_head = current->next;
		else
			last->next = current->next;
		free(current);
	}
}

/*
 * When a child process has changed state, update the list to record
 * the exit status (or kill signal event).
 */

static void update_process(pid_t pid, int result)
{
	struct process *current;
	current = process_head;
	while (current != NULL && current->pid != pid) {
		current = current->next;
	}

	if (current != NULL) {
		/* Found a PID match in while() loop, but has something already reported? */
		if (current->is_done == FALSE) {
			if (WIFEXITED(result)) {
				/* Child exited normally, report the exit code.
				 * Log this if non-zero (i.e. error) or always when verbose.
				 */
				int ecode = WEXITSTATUS(result);
				if (ecode || verbose) {
					log_message(LOG_DEBUG, "test binary %s returned %d = '%s'", current->proc_name, ecode, wd_strerror(ecode));
				}
				current->ecode = ecode;
				current->is_done = TRUE;
			} else if (WIFSIGNALED(result)) {
				/* Child was terminated by a signal. We don't care what signal did
				 * it, so always report it simple as "process killed". When we kill
				 * on time-out, we have already set the 'is_done' flag so don't see this.
				 */
				int sig = WTERMSIG(result);
				log_message(LOG_ERR, "test binary %s was killed by uncaught signal %d", current->proc_name, sig);
				current->ecode = ECHKILL;
				current->is_done = TRUE;
			}
		}
	}
}

/*
 * Look for any child process having changed state. This call also removes
 * them, so it stops programs such as 'top' reporting zombie processes.
 */

static void gather_children(void)
{
	int ret, err;
	int result = 0;

	do {
		ret = waitpid(-1, &result, WNOHANG);
		err = errno;

		/* check result: */
		/* ret < 0                      => error */
		/* ret == 0                     => no more child returned, however we may already have caught the actual child */
		/* WIFEXITED(result) == 0       => child did not exit normally but was killed by signal which was not caught */
		/* WEXITSTATUS(result) != 0     => child returned an error code */

		if (ret > 0) {
			update_process(ret, result);
		} else if (ret < 0 && err != ECHILD) {
			log_message(LOG_ERR, "error getting child process %d = '%s'", err, strerror(err));
		}
	} while (ret > 0);
}

/* See if any test processes have exceeded the timeout */
static int check_timeouts(int timeout)
{
	struct process *current;
	time_t now = time(NULL);

	current = process_head;
	while (current != NULL) {
		if (current->is_done == FALSE && (int)(now - current->time) > timeout) {
			/* Process has timed-out, kill it and report this. */
			kill(current->pid, SIGKILL);
			current->is_done = TRUE;
			current->ecode = ETOOLONG;
			log_message(LOG_ERR, "test-binary %s exceeded time limit %d", current->proc_name, timeout);
		}
		current = current->next;
	}
	return (ENOERR);
}

/*
 * Report on any past child processes. Return values are:
 *
 * 0 = TEST_RUNNING   = child of this name still running.
 * 1 = TEST_COMPLETED = child has stopped, can use result.
 * 2 = TEST_BLANK     = nothing in list, safe to run test program.
 *
 * So if zero returned, then don't try another child instance but
 * return ENOERR until we get an answer.
 *
 * In both other cases (1 or 2) you can start another child but maybe
 * not such a wise thing to do if there is an error condition.
 */

static int check_processes(const char *name, int *ecode)
{
	struct process *current;

	current = process_head;
	while (current != NULL) {
		if (!strcmp(current->proc_name, name)) {
			/* Process still in list, but is it finished or not? */
			if (current->is_done == FALSE) {
				/* Still running. */
				return (TEST_RUNNING);
			} else {
				/* Process has terminated (or we killed it on time-out), so return
				 * any error code and remove from list. We must return at this point,
				 * or the loop will access freed memory for 'current->next' below.
				 */
				*ecode = current->ecode;
				remove_process(current->pid);
				return (TEST_COMPLETED);
			}
		}
		current = current->next;
	}
	/* No match. */
	return (TEST_BLANK);
}

/*
 * execute test binary
 *
 * This has no intentional delay, so basically starts the child process asynchronously and
 * the next call with the same 'tbinary' name will return any error results, or start
 * another (if last one finished normally). While waiting (or no new run) the return
 * value is EDONTKNOW to make the job of the retry-timer workable.
 *
 * A time-out of zero will disable the time-out checking, but in that case a blocked child
 * will simply persist indefinitely and no error will be found.
 */
int check_bin(char *tbinary, int timeout, int version)
{
	pid_t child_pid;
	int ecode = EDONTKNOW;

	/* Call this before test on 'tbinary' so ANY early returns can be
	 * gathered (less zombie process reported that way).
	 */
	gather_children();

	if (timeout > 0)
		check_timeouts(timeout);

	if (tbinary == NULL)
		return ENOERR;

	if (check_processes(tbinary, &ecode) == TEST_RUNNING) {
		/* The process 'tbinary' is still running. */
		return EDONTKNOW;
	}

	child_pid = fork();
	if (!child_pid) {

		/* Don't want the stdout and stderr of our test program
		 * to cause trouble, so make them go to their respective files */
		strcpy(filename_buf, logdir);
		strcat(filename_buf, "/test-bin.stdout");
		if (!freopen(filename_buf, "a+", stdout))
			exit(errno);
		strcpy(filename_buf, logdir);
		strcat(filename_buf, "/test-bin.stderr");
		if (!freopen(filename_buf, "a+", stderr))
			exit(errno);

		/* now start binary */
		if (version == 0) {
			execl(tbinary, tbinary, NULL);
		} else {
			execl(tbinary, tbinary, "test", NULL);
		}

		/* execl should only return in case of an error */
		/* so we return that error */
		exit(errno);
	} else if (child_pid < 0) {	/* fork failed */
		int err = errno;
		log_message(LOG_ERR, "process fork failed with error = %d = '%s'", err, strerror(err));
		return (EREBOOT);
	} else {
		/* fork was okay, add child to process list */
		int err = add_process(tbinary, child_pid);
		/* if that failed, report it instead of exit code. */
		if (err)
			ecode = err;
	}

	return ecode;
}
