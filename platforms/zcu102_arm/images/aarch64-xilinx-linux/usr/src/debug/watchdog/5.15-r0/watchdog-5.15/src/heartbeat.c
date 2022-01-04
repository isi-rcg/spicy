/* > heartbeat.c
 *
 * Groups together the code for the special heart-beat timestamps file
 * used for debug.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "watch_err.h"
#include "extern.h"

static FILE *hb = NULL;
static int lastts, nrts;
static char *timestamps = NULL;

static void next_value(void)
{
	if (nrts < hbstamps)
		nrts++;
	++lastts;
	lastts = lastts % hbstamps;
}

/*
 * Open the heartbeat file based on the global variable 'heartbeat' and allocate enough
 * memory for a buffer based on the global variable 'hbstamps'.
 */

int open_heartbeat(void)
{
	int rv = 0;

	close_heartbeat();

	if (heartbeat != NULL) {
		hb = ((hb = fopen(heartbeat, "r+")) == NULL) ? fopen(heartbeat, "w+") : hb;
		if (hb == NULL) {
			log_message(LOG_ERR, "cannot open %s (errno = %d = '%s')", heartbeat, errno, strerror(errno));
			rv = -1;
		} else {
			char rbuf[TS_SIZE + 1];

			/* Allocate  memory for keeping the timestamps in */
			nrts = 0;
			lastts = 0;
			timestamps = (char *)xcalloc(hbstamps, TS_SIZE);
			/* read any previous timestamps */
			rewind(hb);
			while (fgets(rbuf, TS_SIZE + 1, hb) != NULL) {
				memcpy(timestamps + (TS_SIZE * lastts), rbuf, TS_SIZE);
				next_value();
			}
			/* Write an indication that the watchdog has started to the heartbeat file */
			/* copy it to the buffer */
			sprintf(rbuf, "%*s\n", TS_SIZE - 1, "--restart--");
			memcpy(timestamps + (lastts * TS_SIZE), rbuf, TS_SIZE);

			/* success */
			next_value();
		}
	}

	return rv;
}

/* write a heartbeat file */
int write_heartbeat(void)
{
	time_t timenow;
	struct tm *tm;
	char tbuf[TS_SIZE + 1];
	char tbufw[TS_SIZE + 1];

	if (hb == NULL)
		return (ENOERR);

	/* MJ 16/2/2001 keep a rolling buffer in a file of writes to the
	   watchdog device, any gaps in this will indicate a reboot */

	timenow = time(NULL);
	if (timenow != -1) {
		tm = gmtime(&timenow);
		/* Get the seconds since seconds since 00:00:00, Jan 1, 1970 */
		strftime(tbuf, TS_SIZE - 1, "%s", tm);
		/* Make it the right width */
		sprintf(tbufw, "%*s\n", TS_SIZE - 1, tbuf);
		/* copy it to the buffer */
		memcpy(timestamps + (lastts * TS_SIZE), tbufw, TS_SIZE);

		/* success */
		next_value();

		/* write the buffer to the file */
		rewind(hb);
		if (nrts == hbstamps) {
			/* write from the logical start of the buffer to the physical end */
			if (fwrite(timestamps + (lastts * TS_SIZE), TS_SIZE, hbstamps - lastts, hb) != (hbstamps - lastts)) {
				int err = errno;
				log_message(LOG_ERR, "write heartbeat file gave error %d = '%s'!", err, strerror(err));
			}
			/* write from the physical start of the buffer to the logical end */
			if (fwrite(timestamps, TS_SIZE, lastts, hb) != lastts) {
				int err = errno;
				log_message(LOG_ERR, "write heartbeat file gave error %d = '%s'!", err, strerror(err));
			}
		} else {
			/* write from the physical start of the buffer to the logical end */
			if (fwrite(timestamps, TS_SIZE, nrts, hb) != nrts) {
				int err = errno;
				log_message(LOG_ERR, "write heartbeat file gave error %d = '%s'!", err, strerror(err));
			}
		}
		fflush(hb);
	}
	return (ENOERR);
}


int close_heartbeat(void)
{
	int rv = 0;

	if (hb != NULL && fclose(hb) == -1) {
		log_message(LOG_ALERT, "cannot close %s (errno = %d)", heartbeat, errno);
		rv = -1;
	}

	hb = NULL;

	if (timestamps != NULL) {
		free(timestamps);
		timestamps = NULL;
	}

	return rv;
}
