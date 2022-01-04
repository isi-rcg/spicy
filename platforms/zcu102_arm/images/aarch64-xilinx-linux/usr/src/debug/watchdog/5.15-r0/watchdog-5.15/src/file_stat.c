#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <time.h>
#include <sys/stat.h>

#include "extern.h"
#include "watch_err.h"

int check_file_stat(struct list *file)
{
	struct stat buf;

	if (file == NULL) {
		return (ENOERR);
	}

	/* in filemode stat file */
	if (stat(file->name, &buf) == -1) {
		int err = errno;
		log_message(LOG_ERR, "cannot stat %s (errno = %d = '%s')", file->name, err, strerror(err));
		return (err);
	} else if (file->parameter.file.mtime != 0) {
		int twait = (int)(time(NULL) - buf.st_mtime);

		if (twait > file->parameter.file.mtime) {
			/* file wasn't changed often enough */
			log_message(LOG_ERR, "file %s was not changed in %d seconds (more than %d)", file->name, twait, file->parameter.file.mtime);
			return (ENOCHANGE);
		}
		/* do verbose logging */
		if (verbose && logtick && ticker == 1) {
			char text[25];
			/* Remove the trailing '\n' of the ctime() formatted string. */
			strncpy(text, ctime(&buf.st_mtime), sizeof(text)-1);
			text[sizeof(text)-1] = 0;
			log_message(LOG_DEBUG, "file %s was last changed at %s (%ds ago)", file->name, text, twait);
		}
	} else {
		/* do verbose logging */
		if (verbose && logtick && ticker == 1) {
			log_message(LOG_DEBUG, "file %s status OK", file->name);
		}
	}
	return (ENOERR);
}
