/*************************************************************/
/* Small utility to identify hardware watchdog               */
/* 							     */
/* Idea and most of the implementation by		     */
/* Corey Minyard <minyard@acm.org>			     */
/*                                                           */
/* The rest was written by me, Michael Meskes                */
/* meskes@debian.org                                         */
/*                                                           */
/*************************************************************/
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <stdio.h>
#include <linux/watchdog.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "extern.h"

static void usage(char *progname)
{
	fprintf(stderr, "%s version %d.%d, usage:\n", progname, MAJOR_VERSION, MINOR_VERSION);
	fprintf(stderr, "%s [options]\n", progname);
	fprintf(stderr, "options:\n");
	fprintf(stderr, "  -c | --config-file <file>  specify location of config file\n");
	fprintf(stderr, "  -v | --verbose             verbose messages\n");
	exit(1);
}

int main(int argc, char *const argv[])
{
	char *configfile = CONFIG_FILENAME;
	int c, rv = 0;
	struct watchdog_info ident;
	char *opts = "c:v";
	struct option long_options[] = {
		{"config-file", required_argument, NULL, 'c'},
		{"verbose", no_argument, NULL, 'v'},
		{NULL, 0, NULL, 0}
	};
	int watchdog = -1;
	char *progname = basename(argv[0]);

	open_logging(progname, MSG_TO_STDERR);

	/* check for the one option we understand */
	while ((c = getopt_long(argc, argv, opts, long_options, NULL)) != EOF) {
		switch (c) {
		case 'c':
			configfile = optarg;
			break;
		case 'v':
			verbose++;
			break;
		default:
			usage(progname);
		}
	}

	read_config(configfile);

	/* this program has no other function than identifying the hardware behind
	 * this device i.e. if there is no device given we better punt */
	if (devname == NULL) {
		printf("No watchdog hardware configured in \"%s\"\n", configfile);
		exit(1);
	}

	/* open the device */
	watchdog = open(devname, O_WRONLY);
	if (watchdog == -1) {
		log_message(LOG_ERR, "cannot open %s (errno = %d = '%s')", devname, errno, strerror(errno));
		exit(1);
	}

	/* Print watchdog identity */
	if (ioctl(watchdog, WDIOC_GETSUPPORT, &ident) < 0) {
		log_message(LOG_ERR, "cannot get watchdog identity (errno = %d = '%s')", errno, strerror(errno));
		rv = 1;
	} else {
		ident.identity[sizeof(ident.identity) - 1] = '\0';	/* Be sure */
		printf("%s\n", ident.identity);
	}

	if (verbose) {
		/* If called with timeout <= 0 then query device. */
		int timeout = 0;
		if (ioctl(watchdog, WDIOC_GETTIMEOUT, &timeout) < 0) {
			int err = errno;
			log_message(LOG_ERR, "cannot get timeout (errno = %d = '%s')", err, strerror(err));
		} else {
			printf("watchdog was set to %d seconds\n", timeout);
		}
	}

	if (write(watchdog, "V", 1) < 0) {
		log_message(LOG_ERR, "write watchdog device gave error %d = '%s'!", errno, strerror(errno));
		rv = 1;
	}

	if (close(watchdog) == -1) {
		log_message(LOG_ALERT, "cannot close watchdog (errno = %d = '%s')", errno, strerror(errno));
		rv = 1;
	}

	close_logging();
	exit(rv);
}
