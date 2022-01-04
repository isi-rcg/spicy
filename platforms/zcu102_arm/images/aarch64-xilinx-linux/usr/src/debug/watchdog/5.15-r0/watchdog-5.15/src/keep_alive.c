/* > keep_alive.c
 *
 * Code here from old keep_alive.c and taken from watchdog.c & shutdown.c to group
 * it together. This has the code to open, refresh, and safely close the watchdog device.
 *
 * While the watchdog daemon can still function without such hardware support, it is
 * MUCH less effective as a result, as it can't deal with kernel faults or very difficult
 * reboot conditions.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define _GNU_SOURCE	/* For O_CLOEXEC on older systems. */

#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>			/* for ioctl() */
#include <linux/watchdog.h>		/* for 'struct watchdog_info' */

#include "extern.h"
#include "watch_err.h"

static int watchdog_fd = -1;
static int timeout_used = TIMER_MARGIN;
static int Refresh_using_ioctl = FALSE;

/*
 * Open the watchdog timer (if name non-NULL) and set the time-out value (if non-zero).
 */

int open_watchdog(char *name, int timeout)
{
	struct watchdog_info ident;
	int rv = 0;
	memset(&ident, 0, sizeof(ident));

	close_watchdog();

	if (name != NULL) {
		watchdog_fd = open(name, O_WRONLY | O_CLOEXEC);
		if (watchdog_fd == -1) {
			int err = errno;
			log_message(LOG_ERR, "cannot open %s (errno = %d = '%s')", name, err, strerror(err));
			rv = -1;
			/* do not exit here per default */
			/* we can use watchdog even if there is no watchdog device */
		} else {
			set_watchdog_timeout(timeout);

			/* Also log watchdog identity */
			if (ioctl(watchdog_fd, WDIOC_GETSUPPORT, &ident) < 0) {
				int err = errno;
				log_message(LOG_ERR, "cannot get watchdog identity (errno = %d = '%s')", err, strerror(err));
			} else {
				ident.identity[sizeof(ident.identity) - 1] = '\0';	/* Be sure */
				log_message(LOG_INFO, "hardware watchdog identity: %s", ident.identity);
			}
		}
	}

	/* The IT8728 on Gigabyte motherboard (and similar) would trip due to the normal
	 * refresh in the device driver failing to reset the timer for no obvious reason
	 * (though the normal operation used the Consumer IR sender to refresh via an
	 * interrupt - also a non-obvious method!) so this work-around simply sets the
	 * time-out every refresh operation.
	 *
	 * See https://bugs.launchpad.net/ubuntu/+source/linux/+bug/932381
	 *
	 */
	Refresh_using_ioctl = FALSE;
	if (strcmp("IT87 WDT", (char *)ident.identity) == 0) {
		Refresh_using_ioctl = TRUE;
		log_message(LOG_INFO, "Running IT87 module fix-up");
	}

	return rv;
}

/*
 * In particular the iTCO_wdt driver has a lower limit of about 3 or 5 seconds
 * (and depending on version, upper of 31, 76 or 614). Since we also use this
 * to speed the reboot process in fault case, we might want to do something
 * about the lower limit failure.
 */

static int try_other_times(const int timeout)
{
	static const int try_values[] = {3, 5, 10};
	static const int num_try = sizeof(try_values) / sizeof(int);
	int ii;

	for (ii = 0; ii < num_try; ii++) {
		int tmp = try_values[ii];
		if (tmp > timeout) {
			if (ioctl(watchdog_fd, WDIOC_SETTIMEOUT, &tmp) >= 0) {
				log_message(LOG_ERR, "trying watchdog time-out success for %d", tmp);
				return 0;
			}
		}
	}

	return -1;
}

/*
 * Once opened, call this to query or change the watchdog timer value.
 */

int set_watchdog_timeout(int timeout)
{
	int rv = -1;

	if (watchdog_fd != -1) {
		if (timeout > 0) {
			timeout_used = timeout;
			/* Set the watchdog hard-stop timeout; default = unset (use driver default) */
			if (ioctl(watchdog_fd, WDIOC_SETTIMEOUT, &timeout) < 0) {
				int err = errno;
				log_message(LOG_ERR, "cannot set timeout %d (errno = %d = '%s')", timeout, err, strerror(err));
				/*
				 * We can get here for several reasons: driver in error, time-out too big
				 * or time-out too small. Try other values just in case.
				 */
				 try_other_times(timeout);
			} else {
				if(timeout <= tint * 2) {
					log_message(LOG_WARNING,
						"Warning: watchdog now set to %d seconds, should be more than double interval = %d",
						timeout, tint);
				} else {
					log_message(LOG_INFO, "watchdog now set to %d seconds", timeout);
				}
				rv = 0;
			}
		} else {
			timeout = 0;
			/* If called with timeout <= 0 then query device. */
			if (ioctl(watchdog_fd, WDIOC_GETTIMEOUT, &timeout) < 0) {
				int err = errno;
				log_message(LOG_ERR, "cannot get timeout (errno = %d = '%s')", err, strerror(err));
			} else {
				log_message(LOG_INFO, "watchdog was set to %d seconds", timeout);
				rv = 0;
			}
		}
	}

	return rv;
}

/* write to the watchdog device */
int keep_alive(void)
{
	int err = ENOERR;

	if (watchdog_fd == -1)
		return (ENOERR);

	if (Refresh_using_ioctl) {
		int timeout = timeout_used;
		if (ioctl(watchdog_fd, WDIOC_SETTIMEOUT, &timeout) < 0) {
			err = errno;
			log_message(LOG_ERR, "set watchdog timeout gave error %d = '%s'!", err, strerror(err));
		}
	} else {
		if (write(watchdog_fd, "\0", 1) < 0) {
			/* Normal use of the watchdog driver - any write refreshes it */
			err = errno;
			log_message(LOG_ERR, "write watchdog device gave error %d = '%s'!", err, strerror(err));
		}
	}

	/* MJ 20/2/2001 write a heartbeat to a file outside the syslog, because:
	   - there is no guarantee the system logger is up and running
	   - easier and quicker to parse checkpoint information */
	write_heartbeat();

	return (err);
}

/*
 * Provide read-only access to the watchdog file handle.
 */

int get_watchdog_fd(void)
{
	return watchdog_fd;
}

/*
 * Close the watchdog device, this normally stops the hardware timer to prevent a
 * spontaneous reboot, but not if the kernel is compiled with the
 * CONFIG_WATCHDOG_NOWAYOUT option enabled!
 */

int close_watchdog(void)
{
	int rv = 0;

	if (watchdog_fd != -1) {
		if (write(watchdog_fd, "V", 1) < 0) {
			int err = errno;
			log_message(LOG_ERR, "write watchdog device gave error %d = '%s'!", err, strerror(err));
			rv = -1;
		}

		if (close(watchdog_fd) == -1) {
			int err = errno;
			log_message(LOG_ALERT, "cannot close watchdog (errno = %d = '%s')", err, strerror(err));
			rv = -1;
		}
	}

	watchdog_fd = -1;

	return rv;
}

/* A version of sleep() that keeps the watchdog timer alive. */
void safe_sleep(int sec)
{
	int i;

	keep_alive();
	for (i=0; i<sec; i++) {
		sleep(1);
		keep_alive();
	}
}
