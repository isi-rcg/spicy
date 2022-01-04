/**********************************************************
 * Copyright:   Appliance Studio Ltd
 *              Michael Meskes
 * License:	GPL
 *
 * Filename:    $Id: wd_keepalive.c,v 1.6 2007/08/17 09:24:54 meskes Exp $
 * Author:      Marcel Jansen, 22 February 2001
 * 		Michael Meskes, since then
 * Purpose:     This program can be run during critical periods
 *              when the normal watchdog shouldn't be run. It will
 *              read from the same configuration file, it will do
 *              no checks but will keep writing to the device
 *
***********************************************************/
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <limits.h>
#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <linux/oom.h>
#include <linux/watchdog.h>
#include <libgen.h>
#include <string.h>
#include <unistd.h>

#include "extern.h"

#define TRUE  1
#define FALSE 0

static void usage(char *progname)
{
	fprintf(stderr, "%s version %d.%d, usage:\n", progname, MAJOR_VERSION, MINOR_VERSION);
	fprintf(stderr, "%s [options]\n", progname);
	fprintf(stderr, "options:\n");
	fprintf(stderr, "  -c | --config-file <file>  specify location of config file\n");
	fprintf(stderr, "  -X | --loop-exit <number>  run a fixed number of loops then exit\n");
	exit(1);
}

/* Dummy function for keep_alive.c use */
int write_heartbeat(void)
{
	return 0;
}

/* close the device and check for error */
static void close_all(void)
{
	close_watchdog();
}

/* on exit we close the device and log that we stop */
void terminate(int ecode)
{
	log_message(LOG_NOTICE, "stopping watchdog keepalive daemon (%d.%d)", MAJOR_VERSION, MINOR_VERSION);
	unlock_our_memory();
	close_all();
	remove_pid_file();
	close_logging();
	usleep(100000);		/* 0.1s to make sure log is written */
	exit(ecode);
}

int main(int argc, char *const argv[])
{
	char *configfile = CONFIG_FILENAME;
	long count = 0L;
	long count_max = 0L;
	int c;
	char *progname;

	/* allow all options watchdog understands too */
	char *opts = "d:i:n:fsvbql:p:t:c:r:m:a:X:";
	struct option long_options[] = {
		{"config-file", required_argument, NULL, 'c'},
		{"force", no_argument, NULL, 'f'},
		{"sync", no_argument, NULL, 's'},
		{"no-action", no_argument, NULL, 'q'},
		{"verbose", no_argument, NULL, 'v'},
		{"softboot", no_argument, NULL, 'b'},
		{"loop-exit", required_argument, NULL, 'X'},
		{NULL, 0, NULL, 0}
	};

	progname = basename(argv[0]);
	open_logging(progname, MSG_TO_STDERR | MSG_TO_SYSLOG);

	/* check for the one option we understand */
	while ((c = getopt_long(argc, argv, opts, long_options, NULL)) != EOF) {
		switch (c) {
		case 'c':
			configfile = optarg;
			break;
		case 'n':
		case 'p':
		case 'a':
		case 'r':
		case 'd':
		case 't':
		case 'l':
		case 'm':
		case 'i':
		case 'f':
		case 's':
		case 'b':
		case 'q':
		case 'v':
			break;
		case 'X':
			count_max = atol(optarg);
			log_message(LOG_WARNING, "NOTE: Using --loop-exit so daemon will exit after %ld time intervals",
				    count_max);
			break;
		default:
			usage(progname);
		}
	}

	read_config(configfile);

	/* this daemon has no other function than writing to this device
	 * i.e. if there is no device given we better punt */
	if (devname == NULL) {
		log_message(LOG_INFO, " no watchdog device configured, aborting");
		exit(0);
	}

	if (wd_daemon(0, 0)) {
		fatal_error(EX_SYSERR, "failed to daemonize (%s)", strerror(errno));
	}

	/* tuck my process id away */
	if (write_pid_file(KA_PIDFILE)) {
		fatal_error(EX_USAGE, "unable to gain lock via PID file");
	}

	/* Log the starting message */
	open_logging(NULL, MSG_TO_SYSLOG);
	log_message(LOG_NOTICE, "starting watchdog keepalive daemon (%d.%d):", MAJOR_VERSION, MINOR_VERSION);
	log_message(LOG_INFO, " int=%d alive=%s realtime=%s", tint, devname, realtime ? "yes" : "no");

	/* open the device */
	if (open_watchdog(devname, dev_timeout) < 0) {
		terminate(1);
	}

	/* set signal term to call sigterm_handler() */
	/* to make sure watchdog device is closed */
	signal(SIGTERM, sigterm_handler);

	lock_our_memory(realtime, schedprio, daemon_pid);

	/* main loop: update after <tint> seconds */
	while (_running) {
		keep_alive();
		/* finally sleep some seconds */
		sleep(tint);

		count++;

		if (count_max > 0 && count >= count_max) {
			log_message(LOG_WARNING, "loop exit on interval counter reached");
			_running = 0;
		}
	}

	terminate(0);
	/* not reached */
	return 0;
}
