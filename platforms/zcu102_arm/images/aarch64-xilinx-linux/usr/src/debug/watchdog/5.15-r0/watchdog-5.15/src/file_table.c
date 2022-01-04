#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "extern.h"
#include "watch_err.h"

int check_file_table(void)
{
	int fd;
	const char fname[] = "/proc/uptime"; /* Any file will do, so long as it ALWAYS exists. */
	int err = ENOERR;

	/* open a file */
	fd = open(fname, O_RDONLY);
	if (fd == -1) {
		err = errno;
		log_message(LOG_ERR, "cannot open %s (errno = %d = '%s')", fname, err, strerror(err));

		if (err == ENFILE) {
			/* we need a reboot if ENFILE is returned (file table overflow) */
			log_message(LOG_ERR, "file table overflow detected!");
			return (EREBOOT);
		}
	} else {
		if (close(fd) < 0) {
			err = errno;
			log_message(LOG_ERR, "close %s gave errno = %d = '%s'", fname, err, strerror(err));
		}
	}

	return (err);
}
