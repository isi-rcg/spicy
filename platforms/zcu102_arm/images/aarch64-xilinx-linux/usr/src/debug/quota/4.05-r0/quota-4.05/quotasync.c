#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>

#include "pot.h"
#include "common.h"
#include "quotasys.h"
#include "quotaio.h"

#define FL_USER 1		/* sync user quotas */
#define FL_GROUP 2		/* sync group quotas */
#define FL_ALL 4		/* sync quotas on all filesystems */
#define FL_PROJECT 8		/* sync group quotas */

static int flags, fmt = -1;
static char **mnt;
static int mntcnt;
char *progname;

static void usage(int status)
{
	printf(_(
"%1$s: Utility for syncing quotas.\n"
"Usage: %1$s [-ugP] mount-point...\n"
"   or: %1$s [-ugP] -a\n"
"   or: %1$s -h | -V\n"
"\n"
		), progname);
	printf(_(
"Options:\n"
"-u, --user     synchronize user quotas\n"
"-g, --group    synchronize group quotas\n"
"-P, --project  synchronize project quotas\n"
"-a, --all      synchronize quotas for all mounted file systems\n"
"-h, --help     display this help message and exit\n"
"-V, --version  display version information and exit\n"
"\n"
		));
	printf(_("Report bugs to <%s>.\n"), PACKAGE_BUGREPORT);
	exit(status);
}

static void parse_options(int argcnt, char **argstr)
{
	int ret;
	struct option long_opts[] = {
		{ "user", 0, NULL, 'u' },
		{ "group", 0, NULL, 'g' },
		{ "project", 0, NULL, 'P' },
		{ "all", 0, NULL, 'a' },
		{ "version", 0, NULL, 'V' },
		{ "help", 0, NULL, 'h' },
		{ NULL, 0, NULL, 0 }
	};

	while ((ret = getopt_long(argcnt, argstr, "ahugPV", long_opts, NULL)) != -1) {
		switch (ret) {
			case '?':
				usage(EXIT_FAILURE);
			case 'h':
				usage(EXIT_SUCCESS);
			case 'V':
				version();
				exit(EXIT_SUCCESS);
			case 'u':
				flags |= FL_USER;
				break;
			case 'g':
				flags |= FL_GROUP;
				break;
			case 'P':
				flags |= FL_PROJECT;
				break;
			case 'a':
				flags |= FL_ALL;
				break;
		}
	}

	if ((flags & FL_ALL && optind != argcnt) ||
	    (!(flags & FL_ALL) && optind == argcnt)) {
		fputs(_("Bad number of arguments.\n"), stderr);
		usage(EXIT_FAILURE);
	}
	if (!(flags & FL_ALL)) {
		mnt = argstr + optind;
		mntcnt = argcnt - optind;
	}
	if (!(flags & (FL_USER | FL_GROUP)))
		flags |= FL_USER;
}

static int sync_one(int type, char *dev)
{
	int qcmd = QCMD(Q_SYNC, type);

	return quotactl(qcmd, dev, 0, NULL);
}

static int syncquotas(int type)
{
	struct quota_handle **handles, *h;
	int i, ret = 0;

	if (flags & FL_ALL) {
		if (sync_one(type, NULL) < 0) {
			errstr(_("%s quota sync failed: %s\n"), _(type2name(type)),
					strerror(errno));
			ret = -1;
		}
		return ret;
	}

	handles = create_handle_list(mntcnt, mnt, type, fmt,
				     IOI_READONLY, MS_LOCALONLY | MS_NO_AUTOFS);

	for (i = 0; handles[i]; i++) {
		h = handles[i];
		if (sync_one(type, h->qh_quotadev)) {
			errstr(_("%s quota sync failed for %s: %s\n"),
				_(type2name(type)), h->qh_quotadev, strerror(errno));
			ret = -1;
		}
	}
	dispose_handle_list(handles);

	return ret;
}

int main(int argc, char **argv)
{
	int ret = EXIT_SUCCESS;

	gettexton();
	progname = basename(argv[0]);

	parse_options(argc, argv);
	init_kernel_interface();

	if (flags & FL_USER)
		if (syncquotas(USRQUOTA))
			ret = EXIT_FAILURE;
	if (flags & FL_GROUP)
		if (syncquotas(GRPQUOTA))
			ret = EXIT_FAILURE;
	if (flags & FL_PROJECT)
		if (syncquotas(PRJQUOTA))
			ret = EXIT_FAILURE;
	return ret;
}
