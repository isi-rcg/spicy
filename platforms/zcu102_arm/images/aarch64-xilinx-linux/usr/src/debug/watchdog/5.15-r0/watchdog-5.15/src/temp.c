#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "extern.h"
#include "watch_err.h"

static int temp_fd = -1;

static int templevel1;
static int templevel2;
static int templevel3;

static int read_temp_sensor(const char *name, int *val);

/* ================================================================= */

int open_tempcheck(struct list *tlist)
{
	int rv = -1;
	struct list *act;

	close_tempcheck();

	if (tlist != NULL) {
		/* Use temp_fd as in-use flag. */
		temp_fd = 0;

		/*
		 * Clear flags and set/compute warning and max thresholds. Make
		 * sure that each level is distinct and properly ordered so that
		 * we have templevel1 < templevel2 < templevel3 < maxtemp
		 */
		for (act = tlist; act != NULL; act = act->next) {
			int itmp = 0;
			act->parameter.temp.have1 = FALSE;
			act->parameter.temp.have2 = FALSE;
			act->parameter.temp.have3 = FALSE;
			/* Check the sensors is usable when initialising. */
			if (read_temp_sensor(act->name, &itmp) == ENOERR) {
				act->parameter.temp.in_use = TRUE;
			} else {
				act->parameter.temp.in_use = FALSE;
				log_message(LOG_WARNING, "Disabling temperature check for %s", act->name);
			}
		}

		templevel3 = (maxtemp * 98) / 100;
		if (templevel3 >= maxtemp) {
			templevel3 = maxtemp - 1;
		}

		templevel2 = (maxtemp * 95) / 100;
		if (templevel2 >= templevel3) {
			templevel2 = templevel3 - 1;
		}

		templevel1 = (maxtemp * 90) / 100;
		if (templevel1 >= templevel2) {
			templevel1 = templevel2 - 1;
		}
	}

	return rv;
}

/*
 * Code to read the ASCII "files" presented by the lm-sensors package with paths such as:
 *
 * -r--r--r-- 1 root root 4096 2013-03-09 09:27 /sys/class/hwmon/hwmon0/device/temp1_input
 * -r--r--r-- 1 root root 4096 2013-03-09 09:01 /sys/class/hwmon/hwmon0/device/temp2_input
 * -r--r--r-- 1 root root 4096 2013-03-09 09:27 /sys/class/hwmon/hwmon0/device/temp3_input
 *
 * Location varies with hardware devices, and you may find two sensors as hwmon0 & hwmon1, etcv
 * but in my case the above paths are really sym-links to the hardware driver, such as:
 *
 * -r--r--r-- 1 root root 4096 2013-03-09 09:27 /sys/devices/platform/w83627ehf.656/temp1_input
 * -r--r--r-- 1 root root 4096 2013-03-09 09:01 /sys/devices/platform/w83627ehf.656/temp2_input
 * -r--r--r-- 1 root root 4096 2013-03-09 09:27 /sys/devices/platform/w83627ehf.656/temp3_input
 *
 * They have the temperature in C x 1000 but resolution may only be 0.5C or 1C. Typical result is:
 *
 * > cat /sys/class/hwmon/hwmon0/device/temp1_input
 * 36000
 *
 * For 36.0C so we read and print as fraction, but truncate so only the whole deg C is used
 * for the watchdog tests below.
 */

static int read_temp_sensor(const char *name, int *val)
{
	float temp;
	char buf[128];
	char *p;
	int err;
	FILE *fp;

	fp = fopen(name, "r");
	if (fp == NULL) {
		err = errno;
		log_message(LOG_ERR, "failed to open %s (%s)", name, strerror(err));
		return err;
	}

	buf[0] = 0; /* to be safe... */
	p = fgets(buf, sizeof(buf)-1, fp);
	err = errno;
	fclose(fp);

	if (p == NULL) {
		log_message(LOG_ERR, "failed to read %s (%s)", name, strerror(err));
		return err;
	}

	/* New style sensors read in milli-Celsius, convert to deg C as float. */
	temp = 1.0e-3F * atof(buf);

	if (verbose && logtick && ticker == 1)
		log_message(LOG_DEBUG, "current temperature is %.3f for %s", temp, name);

	/* convert to integer of whole deg C, small addition to make sure matches integer version. */
	*val = (int)(1.0e-5F + temp);

	return ENOERR;
}

/* ================================================================= */

int check_temp(struct list *act)
{
	int temperature = 0;
	int err;

	/* is the temperature device open? */
	if (temp_fd == -1 || act == NULL || act->parameter.temp.in_use == FALSE)
		return (ENOERR);

	err = read_temp_sensor(act->name, &temperature);
	if (err != ENOERR) {
		return (err);
	}

	/* Print out warnings as we cross the 90/95/98 percent thresholds. */
	if (temperature > templevel3) {
		if (!act->parameter.temp.have3) {
			/* once we reach level3, issue a warning once. */
			log_message(LOG_WARNING, "temperature increases above %d (%s)", templevel3, act->name);
			act->parameter.temp.have1 = act->parameter.temp.have2 = act->parameter.temp.have3 = TRUE;
		}
	} else if (temperature > templevel2) {
		if (!act->parameter.temp.have2) {
			log_message(LOG_WARNING, "temperature increases above %d (%s)", templevel2, act->name);
			act->parameter.temp.have1 = act->parameter.temp.have2 = TRUE;
		}
		act->parameter.temp.have3 = FALSE;
	} else if (temperature > templevel1) {
		if (!act->parameter.temp.have1) {
			log_message(LOG_WARNING, "temperature increases above %d (%s)", templevel1, act->name);
			act->parameter.temp.have1 = TRUE;
		}
		act->parameter.temp.have2 = act->parameter.temp.have3 = FALSE;
	} else {
		/* Below all thresholds, report clear only if previously set. */
		if (act->parameter.temp.have1 || act->parameter.temp.have2 || act->parameter.temp.have3) {
			log_message(LOG_INFO, "temperature now OK again for %s", act->name);
		}
		act->parameter.temp.have1 = act->parameter.temp.have2 = act->parameter.temp.have3 = FALSE;
	}

	if (temperature >= maxtemp) {
		log_message(LOG_ERR, "it is too hot inside (temperature = %d >= %d for %s)", temperature, maxtemp, act->name);
		return (ETOOHOT);
	}
	return (ENOERR);
}

/* ================================================================= */

int close_tempcheck(void)
{
	int rv = -1;

	if (temp_fd != -1) {
		rv = 0;
	}

	temp_fd = -1;
	return rv;
}
