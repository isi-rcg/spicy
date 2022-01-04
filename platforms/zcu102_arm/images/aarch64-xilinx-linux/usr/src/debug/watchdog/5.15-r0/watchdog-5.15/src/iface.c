#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "extern.h"
#include "watch_err.h"

#define NETDEV_LINE_LEN	128

int check_iface(struct list *dev)
{
	const char fname[] = "/proc/net/dev";
	FILE *file = fopen(fname, "r");

	if (file == NULL) {
		int err = errno;
		log_message(LOG_ERR, "cannot open %s (errno = %d = '%s')", fname, err, strerror(err));
		return (err);
	}

	/* read the file line by line */
	while (!feof(file)) {
		char line[NETDEV_LINE_LEN];
		memset(line, 0, sizeof(line)); /* Just in case. */

		if (fgets(line, NETDEV_LINE_LEN, file) == NULL) {
			if (!ferror(file)) {
				break;
			} else {
				int err = errno;
				log_message(LOG_ERR, "cannot read %s (errno = %d = '%s')", fname, err, strerror(err));
				fclose(file);
				return (err);
			}
		} else {
			int i = 0;

			for (; line[i] == ' ' || line[i] == '\t'; i++) ;
			if (strncmp(line + i, dev->name, strlen(dev->name)) == 0) {
				unsigned long bytes = strtoul(line + i + strlen(dev->name) + 1, NULL, 10);

				/* do verbose logging */
				if (verbose && logtick && ticker == 1)
					log_message(LOG_DEBUG, "device %s received %lu bytes", dev->name, bytes);

				if (dev->parameter.iface.bytes == bytes) {
					fclose(file);
					log_message(LOG_ERR, "device %s did not receive anything since last check", dev->name);
					return (ENETUNREACH);
				} else {
					dev->parameter.iface.bytes = bytes;
				}
			}
		}
	}

	if (fclose(file) != 0) {
		int err = errno;
		log_message(LOG_ERR, "cannot close %s (errno = %d = '%s')", fname, err, strerror(err));
		return (err);
	}

	return (ENOERR);
}
