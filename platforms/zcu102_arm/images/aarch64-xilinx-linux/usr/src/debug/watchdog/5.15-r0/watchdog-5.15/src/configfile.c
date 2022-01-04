/* > configfile.c
 *
 * Code based on old watchdog.c function to read settings and to get the
 * test binary(s) (if any). Reads the configuration file on a line-by-line
 * basis and parses it for "parameter = value" sort of entries.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <sys/stat.h>

#include "extern.h"
#include "watch_err.h"
#include "read-conf.h"

static void add_test_binaries(const char *path);

#define ADMIN			"admin"
#define CHANGE			"change"
#define DEVICE			"watchdog-device"
#define DEVICE_TIMEOUT		"watchdog-timeout"
#define	FILENAME		"file"
#define INTERFACE		"interface"
#define INTERVAL		"interval"
#define LOGTICK			"logtick"
#define MAXLOAD1		"max-load-1"
#define MAXLOAD5		"max-load-5"
#define MAXLOAD15		"max-load-15"
#define MAXTEMP			"max-temperature"
#define MINMEM			"min-memory"
#define ALLOCMEM		"allocatable-memory"
#define SERVERPIDFILE		"pidfile"
#define PING			"ping"
#define PINGCOUNT		"ping-count"
#define PRIORITY		"priority"
#define REALTIME		"realtime"
#define REPAIRBIN		"repair-binary"
#define REPAIRTIMEOUT		"repair-timeout"
#define SOFTBOOT		"softboot-option"
#define TEMP			"temperature-sensor"
#define TEMPPOWEROFF   		"temp-power-off"
#define TESTBIN			"test-binary"
#define TESTTIMEOUT		"test-timeout"
#define HEARTBEAT		"heartbeat-file"
#define HBSTAMPS		"heartbeat-stamps"
#define LOGDIR			"log-dir"
#define TESTDIR			"test-directory"
#define SIGTERM_DELAY	"sigterm-delay"
#define RETRYTIMEOUT	"retry-timeout"
#define REPAIRMAX		"repair-maximum"
#define VERBOSE			"verbose"

#ifndef TESTBIN_PATH
#define TESTBIN_PATH	NULL
#endif
static char *test_dir = TESTBIN_PATH;

/* Global configuration variables */

int tint = 1;
int logtick = 1;
int ticker = 1;
int schedprio = 1;
int maxload1 = 0;
int maxload5 = 0;
int maxload15 = 0;
int minpages = 0;
int minalloc = 0;
int maxtemp = 90;
int pingcount = 3;
int temp_poweroff = TRUE;
int sigterm_delay = 5;	/* Seconds from first SIGTERM to sending SIGKILL during shutdown. */
int repair_max = 1; /* Number of repair attempts without success. */

char *devname = NULL;
char *admin = "root";

int test_timeout = TIMER_MARGIN;   /* test-binary time out value. */
int repair_timeout = TIMER_MARGIN; /* repair-binary time out value. */
int dev_timeout = TIMER_MARGIN;    /* Watchdog hardware time-out. */
int retry_timeout = TIMER_MARGIN;  /* Retry on non-critical errors. */

char *logdir = "/var/log/watchdog";

char *heartbeat = NULL;
int hbstamps = 300;

int realtime = FALSE;

/* Self-repairing binaries list */
struct list *tr_bin_list = NULL;
struct list *file_list = NULL;
struct list *target_list = NULL;
struct list *pidfile_list = NULL;
struct list *iface_list = NULL;
struct list *temp_list = NULL;

char *repair_bin = NULL;

/* Command line options also used globally. */
int softboot = FALSE;
int verbose = FALSE;

/* Simple table for yes/no enumerated options. */
static const read_list_t Yes_No_list[] = {
READ_LIST_ADD("no", 0)
READ_LIST_ADD("yes", 1)
READ_LIST_END()
};

/* Use the macros below to simplify the parsing function. For now we don't use the
 * integer range checking (0=0 so not checked), and assume all strings can be blank and
 * enumerated choices are Yes/No, but in future we could add such settings to the #define'd
 * list of names above.
 */

#define READ_INT(name, iv)		read_int_func(		 arg, val, name, 0, 0, iv)
#define READ_STRING(name, str)	read_string_func(	 arg, val, name, Read_allow_blank, str)
#define READ_ENUM(name, iv)		read_enumerated_func(arg, val, name, Yes_No_list, iv)
#define READ_LIST(name, list)	read_list_func(		 arg, val, name, 0, list)

/*
 * Open the configuration file, read & parse it, and set the global configuration variables to those values.
 */

void read_config(char *configfile)
{
	FILE *wc;
	char *line = NULL, *arg=NULL, *val=NULL;
	size_t n = 0;
	int linecount = 0;

	maxload5 = maxload15 = 0;

	if ((wc = fopen(configfile, "r")) == NULL) {
		fatal_error(EX_SYSERR, "Can't open config file \"%s\" (%s)", configfile, strerror(errno));
	}

	while (getline(&line, &n, wc) != -1) {
		int itmp = 0;
		linecount++;

		/* find first non-white space character and check for blank/commented lines. */
		arg = str_start(line);
		if (arg[0] == 0 || arg[0] == '#') {
			continue;
		}

		/* find the '=' for the "arg = val" parsing. */
		val = strchr(arg, '=');
		if (val == NULL) {
			log_message(LOG_WARNING, "Warning: no '=' assignment at line %d of config file", linecount);
			continue;
		}

		/* split at found '=' and move to next non-white-space character. */
		*val = '\0';
		val = str_start(val+1);

		/* remove trailing white-space characters for easier parsing. */
		trim_white(val);
		trim_white(arg);

		/* Search for a match. Note that the read_*_func() calls deal with a zero-length 'val' as needed. */
		if (READ_LIST(FILENAME, &file_list) == 0) {
		} else if (READ_INT(CHANGE, &itmp) == 0) {
			struct list *ptr;
			if (!file_list) {	/* no file entered yet */
				log_message(LOG_WARNING,
					"Warning: file change interval, but no file (yet) at line %d of config file", linecount);
			} else {
				for (ptr = file_list; ptr->next != NULL; ptr = ptr->next) {
					/* loop to find end of list. */
				}

				if (ptr->parameter.file.mtime != 0)
					log_message(LOG_WARNING,
						"Warning: duplicate change interval at line %d of config file (ignoring previous)", linecount);

				ptr->parameter.file.mtime = itmp;
			}
		} else if (READ_LIST(SERVERPIDFILE, &pidfile_list) == 0) {
		} else if (READ_INT(PINGCOUNT, &pingcount) == 0) {
		} else if (READ_LIST(PING, &target_list) == 0) {
		} else if (READ_LIST(INTERFACE, &iface_list) == 0) {
		} else if (READ_ENUM(REALTIME, &realtime) == 0) {
		} else if (READ_INT(PRIORITY, &schedprio) == 0) {
		} else if (READ_STRING(REPAIRBIN, &repair_bin) == 0) {
		} else if (READ_INT(REPAIRTIMEOUT, &repair_timeout) == 0) {
		} else if (READ_LIST(TESTBIN, &tr_bin_list) == 0) {
		} else if (READ_INT(TESTTIMEOUT, &test_timeout) == 0) {
		} else if (READ_STRING(HEARTBEAT, &heartbeat) == 0) {
		} else if (READ_INT(HBSTAMPS, &hbstamps) == 0) {
		} else if (READ_STRING(ADMIN, &admin) == 0) {
		} else if (READ_INT(INTERVAL, &tint) == 0) {
		} else if (READ_INT(LOGTICK, &logtick) == 0) {
			ticker = logtick;
		} else if (READ_STRING(DEVICE, &devname) == 0) {
		} else if (READ_INT(DEVICE_TIMEOUT, &dev_timeout) == 0) {
		} else if (READ_LIST(TEMP, &temp_list) == 0) {
		} else if (READ_INT(MAXTEMP, &maxtemp) == 0) {
		} else if (READ_INT(MAXLOAD1, &maxload1) == 0) {
		} else if (READ_INT(MAXLOAD5, &maxload5) == 0) {
		} else if (READ_INT(MAXLOAD15, &maxload15) == 0) {
		} else if (READ_INT(MINMEM, &minpages) == 0) {
		} else if (READ_INT(ALLOCMEM, &minalloc) == 0) {
		} else if (READ_STRING(LOGDIR, &logdir) == 0) {
		} else if (READ_STRING(TESTDIR, &test_dir) == 0) {
		} else if (READ_ENUM(SOFTBOOT, &softboot) == 0) {
		} else if (READ_ENUM(TEMPPOWEROFF, &temp_poweroff) == 0) {
		} else if (READ_INT(SIGTERM_DELAY, &sigterm_delay) == 0) {
		} else if (READ_INT(RETRYTIMEOUT, &retry_timeout) == 0) {
		} else if (READ_INT(REPAIRMAX, &repair_max) == 0) {
		} else if (READ_ENUM(VERBOSE, &verbose) == 0) {
		} else {
			log_message(LOG_WARNING, "Ignoring invalid option at line %d of config file: %s=%s", linecount, arg, val);
		}
	}

	if (line)
		free(line);

	if (fclose(wc) != 0) {
		fatal_error(EX_SYSERR, "Error closing file \"%s\" (%s)", configfile, strerror(errno));
	}

	add_test_binaries(test_dir);

	if (tint <= 0) {
		fatal_error(EX_SYSERR, "Parameters %s = %d in file \"%s\" must be > 0", INTERVAL, tint, configfile);
	}

	/* compute 5 & 15 minute averages if not given. */
	if (maxload1 && !maxload5)
		maxload5 = maxload1 * 3 / 4;

	if (maxload1 && !maxload15)
		maxload15 = maxload1 / 2;

}

static void add_test_binaries(const char *path)
{
	DIR *d;
	struct dirent dentry;
	struct dirent *rdret;
	struct stat sb;
	int ret;
	char fname[PATH_MAX];

	if (!path)
		return;

	ret = stat(path, &sb);
	if (ret < 0)
		return;

	if (!S_ISDIR(sb.st_mode))
		return;

	d = opendir(path);
	if (!d)
		return;

	do {
		ret = readdir_r(d, &dentry, &rdret);
		if (ret)
			break;
		if (rdret == NULL)
			break;

		ret = snprintf(fname, sizeof(fname), "%s/%s", path, dentry.d_name);
		if (ret >= sizeof(fname))
			continue;
		ret = stat(fname, &sb);
		if (ret < 0)
			continue;
		if (!S_ISREG(sb.st_mode))
			continue;

		/* Skip any hidden files - a bit suspicious. */
		if(dentry.d_name[0] == '.') {
			log_message(LOG_WARNING, "skipping hidden file %s", fname);
			continue;
		}

		if (!(sb.st_mode & S_IXUSR))
			continue;
		if (!(sb.st_mode & S_IRUSR))
			continue;

		if (verbose)
			log_message(LOG_DEBUG, "adding %s to list of auto-repair binaries", fname);

		add_list(&tr_bin_list, fname, 1);
	} while (1);

	closedir(d);
}
