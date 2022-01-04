/* > load.c
 *
 * Code for checking the system load averages.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "extern.h"
#include "watch_err.h"

static int load_fd = -1;
static const char load_name[] = "/proc/loadavg";

/* ============================================================================ */

int open_loadcheck(void)
{
	int rv = -1;

	close_loadcheck();

	if (maxload1 || maxload5 || maxload15) {
		/* open the load average file */
		load_fd = open(load_name, O_RDONLY);
		if (load_fd == -1) {
			log_message(LOG_ERR, "cannot open %s (errno = %d = '%s')", load_name, errno, strerror(errno));
		} else {
			rv = 0;
		}
	}

	return rv;
}

/* ============================================================================ */

int check_load(void)
{
	int avg1, avg5, avg15;
	char buf[40], *ptr;
	int n;

	/* is the load average file open? */
	if (load_fd == -1)
		return (ENOERR);

	/* position pointer at start of file */
	if (lseek(load_fd, 0, SEEK_SET) < 0) {
		int err = errno;
		log_message(LOG_ERR, "lseek %s gave errno = %d = '%s'", load_name, err, strerror(err));
		return (err);
	}

	/* read the line (there is only one) */
	if ((n = read(load_fd, buf, sizeof(buf)-1)) < 0) {
		int err = errno;
		log_message(LOG_ERR, "read %s gave errno = %d = '%s'", load_name, err, strerror(err));
		return (err);
	}
	/* Force string to be nul-terminated. */
	buf[n] = 0;
	/* we only care about integer values */
	avg1 = atoi(buf);

	/* if we have incorrect data we might not be able to find */
	/* the blanks we're looking for */
	ptr = strchr(buf, ' ');
	if (ptr != NULL) {
		avg5 = atoi(ptr);
		ptr = strchr(ptr + 1, ' ');
	}

	if (ptr != NULL) {
		avg15 = atoi(ptr);
	} else {
		log_message(LOG_ERR, "%s does not contain any data (read = %s)", load_name, buf);
		return (ENOLOAD);
	}

	if (verbose && logtick && ticker == 1)
		log_message(LOG_DEBUG, "current load is %d %d %d", avg1, avg5, avg15);

	if ((maxload1  > 0 && avg1  > maxload1) ||
		(maxload5  > 0 && avg5  > maxload5) ||
		(maxload15 > 0 && avg15 > maxload15)) {

		log_message(LOG_ERR, "loadavg %d %d %d is higher than the given threshold %d %d %d!",
							avg1, avg5, avg15,
							maxload1, maxload5, maxload15);

		return (EMAXLOAD);
	}

	return (ENOERR);
}

/* ============================================================================ */

int close_loadcheck(void)
{
	int rv = -1;

	if (load_fd != -1 && close(load_fd) == -1) {
		log_message(LOG_ALERT, "cannot close %s (errno = %d)", load_name, errno);
	} else {
		rv = 0;
	}

	load_fd = -1;
	return rv;
}
