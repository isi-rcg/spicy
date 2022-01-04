/*
 * QUOTA    An implementation of the diskquota system for the LINUX operating
 *          system. QUOTA is implemented using the BSD systemcall interface
 *          as the means of communication with the user level. Should work for
 *          all filesystems because of integration into the VFS layer of the
 *          operating system. This is based on the Melbourne quota system wich
 *          uses both user and group quota files.
 * 
 *          Program to query for the internal statistics.
 * 
 * Author:  Marco van Wieringen <mvw@planets.elm.net>
 *
 *          This program is free software; you can redistribute it and/or
 *          modify it under the terms of the GNU General Public License as
 *          published by the Free Software Foundation; either version 2 of
 *          the License, or (at your option) any later version.
 */

#include "config.h"

#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#include "pot.h"
#include "common.h"
#include "quota.h"
#include "quotasys.h"
#include "quotaio.h"
#include "quotaio_v1.h"
#include "dqblk_v1.h"
#include "quotaio_v2.h"
#include "dqblk_v2.h"

char *progname;

static int get_proc_num(char *name)
{
	int ret;
	char namebuf[PATH_MAX] = "/proc/sys/fs/quota/";
	FILE *f;

	sstrncat(namebuf, name, PATH_MAX);
	if (!(f = fopen(namebuf, "r"))) {
		errstr(_("Cannot read stat file %s: %s\n"), namebuf, strerror(errno));
		return -1;
	}
	fscanf(f, "%d", &ret);
	fclose(f);
	return ret;
}

static int get_stats(struct util_dqstats *dqstats)
{
	struct v1_dqstats old_dqstats;
	struct v2_dqstats v0_dqstats;
	int ret = -1;
	struct stat st;

	signal(SIGSEGV, SIG_IGN);	/* Ignore SIGSEGV due to bad quotactl() */
	if (!stat("/proc/sys/fs/quota", &st)) {
		dqstats->version = 6*10000+5*100+1;
		dqstats->lookups = get_proc_num("lookups");
		dqstats->drops = get_proc_num("drops");
		dqstats->reads = get_proc_num("reads");
		dqstats->writes = get_proc_num("writes");
		dqstats->cache_hits = get_proc_num("cache_hits");
		dqstats->allocated_dquots = get_proc_num("allocated_dquots");
		dqstats->free_dquots = get_proc_num("free_dquots");
		dqstats->syncs = get_proc_num("syncs");
	}
	else if (quotactl(QCMD(Q_V1_GETSTATS, 0), NULL, 0, (caddr_t)&old_dqstats) >= 0) {
		/* Structures are currently the same */
		memcpy(dqstats, &old_dqstats, sizeof(old_dqstats));
		dqstats->version = 0;
	}
	else {
		/* Sadly these all are possible to get from kernel :( */
		if (errno != EINVAL && errno != EPERM && errno != EFAULT) {
			errstr(_("Error while getting quota statistics from kernel: %s\n"), strerror(errno));
			goto out;
		}
		if (quotactl(QCMD(Q_V2_GETSTATS, 0), NULL, 0, (caddr_t)&v0_dqstats) < 0) {
			errstr(_("Error while getting old quota statistics from kernel: %s\n"), strerror(errno));
			goto out;
		}
		memcpy(dqstats, &v0_dqstats, sizeof(v0_dqstats));
	}
	ret = 0;
out:
	signal(SIGSEGV, SIG_DFL);
	return ret;
}

static inline int print_stats(struct util_dqstats *dqstats)
{
	if (!dqstats->version)
		printf(_("Kernel quota version: old\n"));
	else
		printf(_("Kernel quota version: %u.%u.%u\n"), dqstats->version/10000, dqstats->version/100%100, dqstats->version%100);
	printf(_("Number of dquot lookups: %ld\n"), (long)dqstats->lookups);
	printf(_("Number of dquot drops: %ld\n"), (long)dqstats->drops);
	printf(_("Number of dquot reads: %ld\n"), (long)dqstats->reads);
	printf(_("Number of dquot writes: %ld\n"), (long)dqstats->writes);
	printf(_("Number of quotafile syncs: %ld\n"), (long)dqstats->syncs);
	printf(_("Number of dquot cache hits: %ld\n"), (long)dqstats->cache_hits);
	printf(_("Number of allocated dquots: %ld\n"), (long)dqstats->allocated_dquots);
	printf(_("Number of free dquots: %ld\n"), (long)dqstats->free_dquots);
	printf(_("Number of in use dquot entries (user/group): %ld\n"),
		(long)(dqstats->allocated_dquots - dqstats->free_dquots));
	return 0;
}

int main(int argc, char **argv)
{
	struct util_dqstats dqstats;

	gettexton();
	progname = basename(argv[0]);

	if (!get_stats(&dqstats))
		print_stats(&dqstats);
	return 0;
}
