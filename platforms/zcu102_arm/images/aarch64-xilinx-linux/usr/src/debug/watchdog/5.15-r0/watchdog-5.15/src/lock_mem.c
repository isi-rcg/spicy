/* > lock_mem.c
 *
 * Common taken from watchdog.c & wd_keepalive.c that locks the process memory, and
 * from shutdown.c etc that unlocks it again for a tidy exit.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/oom.h>		/* For OOM_SCORE_ADJ_MIN and OOM_DISABLE */
#include <sys/mman.h>
#include <sched.h>

#include "extern.h"

#if defined(_POSIX_MEMLOCK)
static int mlocked = FALSE;
#endif /* _POSIX_MEMLOCK */

/*
 * Function to lock the process and attempt to disable the Out-Of-Memory killer
 * that Linux uses so the daemon is not kicked out unexpectedly. Calling arguments
 * are:
 *		do_lock	:	Set to TRUE if you want to process locked.
 *		priority:	Set to the real-time priority level you want.
 *		pid:		This should be the current process' PID. Either from a call
 *					to getpid() just for this, or the value already found.
 */

void lock_our_memory(int do_lock, int priority, pid_t pid)
{
	int oom_adjusted = 0;
#if defined( OOM_SCORE_ADJ_MIN) || defined( OOM_DISABLE )
	FILE *fp = NULL;
	struct stat s;
	char buf[256];
#endif

#if defined(_POSIX_MEMLOCK)
	if (do_lock == TRUE) {
		unlock_our_memory();
		/* lock all actual and future pages into memory */
		if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0) {
			log_message(LOG_ERR, "cannot lock realtime memory (errno = %d = '%s')", errno, strerror(errno));
		} else {
			struct sched_param sp;
			memset(&sp, 0, sizeof(sp));
			/* now set the scheduler */
			sp.sched_priority = priority;
			if (sched_setscheduler(0, SCHED_RR, &sp) != 0) {
				log_message(LOG_ERR, "cannot set scheduler (errno = %d = '%s')", errno, strerror(errno));
			} else
				mlocked = TRUE;
		}
	}
#endif /* _POSIX_MEMLOCK */

	/* tell oom killer to not kill this process */
	if (pid > 0) {
#ifdef OOM_SCORE_ADJ_MIN
		snprintf(buf, sizeof(buf), "/proc/%d/oom_score_adj", (int)pid);
		if (!oom_adjusted) {	/* Or do both ? */
			if (!stat(buf, &s)) {
				fp = fopen(buf, "w");
				if (fp) {
					fprintf(fp, "%d\n", OOM_SCORE_ADJ_MIN);
					fclose(fp);
					oom_adjusted = 1;
				}
			}
		}
#endif /* OOM_SCORE_ADJ_MIN */

#ifdef OOM_DISABLE
		snprintf(buf, sizeof(buf), "/proc/%d/oom_adj", (int)pid);
		if (!oom_adjusted) {	/* Or do both ? */
			if (!stat(buf, &s)) {
				fp = fopen(buf, "w");
				if (fp) {
					fprintf(fp, "%d\n", OOM_DISABLE);
					fclose(fp);
					oom_adjusted = 1;
				}
			}
		}
#endif /* OOM_DISABLE */

		if (!oom_adjusted) {
			log_message(LOG_WARNING, "unable to disable oom handling!");
		}
	}

}

/*
 * Release the lock on our memory (if used).
 */

void unlock_our_memory(void)
{
#if defined(_POSIX_MEMLOCK)
	if (mlocked == TRUE) {
		/* unlock all locked pages */
		if (munlockall() != 0) {
			log_message(LOG_ERR, "cannot unlock realtime memory (errno = %d = '%s')", errno, strerror(errno));
		}
		mlocked = FALSE;
	}
#endif /* _POSIX_MEMLOCK */
}
