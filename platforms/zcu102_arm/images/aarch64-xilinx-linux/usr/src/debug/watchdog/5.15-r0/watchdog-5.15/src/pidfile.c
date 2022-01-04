#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>

#include "extern.h"
#include "watch_err.h"

int check_pidfile(struct list *file)
{
	int fd = open(file->name, O_RDONLY), pid;
	char buf[20];
	int n;

	if (fd == -1) {
		int err = errno;
		log_message(LOG_ERR, "cannot open %s (errno = %d = '%s')", file->name, err, strerror(err));
		return (err);
	}

	/* position pointer at start of file */
	if (lseek(fd, 0, SEEK_SET) < 0) {
		int err = errno;
		log_message(LOG_ERR, "lseek %s gave errno = %d = '%s'", file->name, err, strerror(err));
		close(fd);
		return (err);
	}

	/* read the line (there is only one) */
	if ((n = read(fd, buf, sizeof(buf)-1)) < 0) {
		int err = errno;
		log_message(LOG_ERR, "read %s gave errno = %d = '%s'", file->name, err, strerror(err));
		close(fd);
		return (err);
	}
	/* Force string to be nul-terminated. */
	buf[n] = 0;

	/* we only care about integer values */
	pid = atoi(buf);

	if (close(fd) == -1) {
		int err = errno;
		log_message(LOG_ERR, "could not close %s, errno = %d = '%s'", file->name, err, strerror(err));
		return (err);
	}

	if (kill(pid, 0) == -1) {
		int err = errno;
		log_message(LOG_ERR, "pinging process %d (%s) gave errno = %d = '%s'", pid, file->name, err, strerror(err));
		return (err);
	}

	/* do verbose logging */
	if (verbose && logtick && ticker == 1)
		log_message(LOG_DEBUG, "was able to ping process %d (%s)", pid, file->name);

	return (ENOERR);
}
