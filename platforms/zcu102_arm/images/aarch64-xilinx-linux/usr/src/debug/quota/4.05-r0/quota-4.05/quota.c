/*
 * Basic utility for reporting quotas
 */

#include "config.h"

/*
 * Disk quota reporting program.
 */
#include <sys/types.h>
#include <sys/param.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <ctype.h>
#ifdef RPC
#include <rpc/rpc.h>
#include "rquota.h"
#endif

#include "quota.h"
#include "quotaops.h"
#include "quotasys.h"
#include "pot.h"
#include "common.h"

#define FL_QUIET 1
#define FL_VERBOSE 2
#define FL_USER 4
#define FL_GROUP 8
#define FL_LOCALONLY 32
#define FL_QUIETREFUSE 64
#define FL_NOAUTOFS 128
#define FL_NOWRAP 256
#define FL_FSLIST 512
#define FL_NUMNAMES 1024
#define FL_NFSALL 2048
#define FL_RAWGRACE 4096
#define FL_NO_MIXED_PATHS 8192
#define FL_SHOW_MNTPOINT 16384
#define FL_SHOW_DEVICE 32768
#define FL_PROJECT 65536

static int flags, fmt = -1;
static enum s2s_unit spaceunit = S2S_NONE, inodeunit = S2S_NONE;
char *progname;

static void usage(void)
{
	errstr( "%s%s%s%s%s",
		_("Usage: quota [-guPqvswim] [-l | [-Q | -A]] [-F quotaformat]\n"),
		_("\tquota [-qvswim] [-l | [-Q | -A]] [-F quotaformat] -u username ...\n"),
		_("\tquota [-qvswim] [-l | [-Q | -A]] [-F quotaformat] -g groupname ...\n"),
		_("\tquota [-qvswugPQm] [-F quotaformat] -f filesystem ...\n"),
		_("\n\
-u, --user                    display quota for user\n\
-g, --group                   display quota for group\n\
-P, --project                 display quota for project\n\
-q, --quiet                   print more terse message\n\
-v, --verbose                 print more verbose message\n\
-s, --human-readable[=units]  display numbers in human friendly units (MB, GB,\n\
                              ...). Units can be also specified explicitely by\n\
                              an optional argument in format [kgt],[kgt] where\n\
                              the first character specifies space units and the\n\
                              second character specifies inode units\n\
    --always-resolve          always try to translate name to id, even if it is\n\
			      composed of only digits\n\
-w, --no-wrap                 do not wrap long lines\n\
-p, --raw-grace               print grace time in seconds since epoch\n\
-l, --local-only              do not query NFS filesystems\n\
-Q, --quiet-refuse            do not print error message when NFS server does\n\
                              not respond\n\
-i, --no-autofs               do not query autofs mountpoints\n\
-F, --format=formatname       display quota of a specific format\n\
-f, --filesystem-list         display quota information only for given\n\
                              filesystems\n\
-A, --all-nfs                 display quota for all NFS mountpoints\n\
-m, --no-mixed-pathnames      trim leading slashes from NFSv4 mountpoints\n\
    --show-mntpoint           show mount point of the file system in output\n\
    --hide-device             do not show file system device in output\n\
-h, --help                    display this help message and exit\n\
-V, --version                 display version information and exit\n\n"));
	fprintf(stderr, _("Bugs to: %s\n"), PACKAGE_BUGREPORT);
	exit(1);
}

static void heading(int type, qid_t id, char *name, char *tag)
{
	char *spacehdr;

	if (spaceunit != S2S_NONE)
		spacehdr = _("space");
	else
		spacehdr = _("blocks");

	printf(_("Disk quotas for %s %s (%cid %u): %s\n"), _(type2name(type)),
	       name, *type2name(type), (uint) id, tag);
	if (!(flags & FL_QUIET) && !tag[0]) {
		printf("%15s%8s %7s%8s%8s%8s %7s%8s%8s\n", _("Filesystem"),
		       spacehdr, _("quota"), _("limit"), _("grace"),
		       _("files"), _("quota"), _("limit"), _("grace"));
	}
}

static void print_fs_location(struct dquot *q)
{
	struct quota_handle *h = q->dq_h;

	if (flags & FL_QUIET) {
		if (flags & FL_SHOW_DEVICE)
			printf(" %s", h->qh_quotadev);
		if (flags & FL_SHOW_MNTPOINT)
			printf(" %s", h->qh_dir);
		putchar('\n');
	} else {
		int wrap = 0;

		if (flags & FL_SHOW_DEVICE && flags & FL_SHOW_MNTPOINT &&
		    !(flags & FL_NOWRAP))
			wrap = 1;
		else if (flags & FL_SHOW_DEVICE && strlen(h->qh_quotadev) > 15 &&
		    !(flags & FL_NOWRAP))
			wrap = 1;
		else if (flags & FL_SHOW_MNTPOINT && strlen(h->qh_dir) > 15 &&
		    !(flags & FL_NOWRAP))
			wrap = 1;
		
		if (flags & FL_SHOW_DEVICE) {
			if (wrap || flags & FL_SHOW_MNTPOINT)
				printf("%s", h->qh_quotadev);
			else
				printf("%15s", h->qh_quotadev);
		}
		if (flags & FL_SHOW_MNTPOINT) {
			if (flags & FL_SHOW_DEVICE)
				putchar(' ');
			if (wrap || flags & FL_SHOW_DEVICE)
				printf("%s", h->qh_dir);
			else
				printf("%15s", h->qh_dir);
		}
		if (wrap)
			printf("\n%15s", "");
	}
}

static int showquotas(int type, qid_t id, int mntcnt, char **mnt)
{
	struct dquot *qlist, *q;
	char *msgi, *msgb;
	char timebuf[MAXTIMELEN];
	char name[MAXNAMELEN];
	struct quota_handle **handles;
	int lines = 0, bover, iover, over, unlimited;
	time_t now;

	time(&now);
	id2name(id, type, name);
	handles = create_handle_list(mntcnt, mnt, type, fmt,
		IOI_READONLY | ((flags & FL_NO_MIXED_PATHS) ? 0 : IOI_NFS_MIXED_PATHS),
		((flags & FL_NOAUTOFS) ? MS_NO_AUTOFS : 0)
		| ((flags & FL_LOCALONLY) ? MS_LOCALONLY : 0)
		| ((flags & FL_NFSALL) ? MS_NFS_ALL : 0));
	qlist = getprivs(id, handles, !!(flags & FL_QUIETREFUSE));
	if (!qlist) {
		over = 1;
		goto out_handles;
	}
	over = 0;
	unlimited = 1;
	for (q = qlist; q; q = q->dq_next) {
		bover = iover = 0;
		if (!q->dq_dqb.dqb_isoftlimit && !q->dq_dqb.dqb_ihardlimit
		    && !q->dq_dqb.dqb_bsoftlimit && !q->dq_dqb.dqb_bhardlimit) {
			if (!(flags & FL_VERBOSE))
				continue;
		} else {
			unlimited = 0;
		}
		msgi = NULL;
		if (q->dq_dqb.dqb_ihardlimit && q->dq_dqb.dqb_curinodes >= q->dq_dqb.dqb_ihardlimit) {
			msgi = _("File limit reached on");
			iover = 1;
		}
		else if (q->dq_dqb.dqb_isoftlimit
			 && q->dq_dqb.dqb_curinodes > q->dq_dqb.dqb_isoftlimit) {
			if (q->dq_dqb.dqb_itime > now) {
				msgi = _("In file grace period on");
				iover = 2;
			}
			else {
				msgi = _("Over file quota on");
				iover = 3;
			}
		}
		msgb = NULL;
		if (q->dq_dqb.dqb_bhardlimit && toqb(q->dq_dqb.dqb_curspace) >= q->dq_dqb.dqb_bhardlimit) {
				msgb = _("Block limit reached on");
				bover = 1;
		}
		else if (q->dq_dqb.dqb_bsoftlimit
			 && toqb(q->dq_dqb.dqb_curspace) > q->dq_dqb.dqb_bsoftlimit) {
			if (q->dq_dqb.dqb_btime > now) {
				msgb = _("In block grace period on");
				bover = 2;
			}
			else {
				msgb = _("Over block quota on");
				bover = 3;
			}
		}
		over |= bover | iover;
		if (flags & FL_QUIET) {
			if ((msgi || msgb) && !lines++)
				heading(type, id, name, "");
			if (msgi) {
				printf("\t%s", msgi);
				print_fs_location(q);
			}
			if (msgb) {
				printf("\t%s", msgb);
				print_fs_location(q);
			}
			continue;
		}
		if ((flags & FL_VERBOSE) || q->dq_dqb.dqb_curspace || q->dq_dqb.dqb_curinodes) {
			char numbuf[3][MAXNUMLEN];

			if (!lines++)
				heading(type, id, name, "");
			print_fs_location(q);
			if (!(flags & FL_RAWGRACE)) {
				if (bover)
					difftime2str(q->dq_dqb.dqb_btime, timebuf);
				else
					timebuf[0] = 0;
			}
			else {
				if (bover)
					sprintf(timebuf, "%llu", (long long unsigned int)q->dq_dqb.dqb_btime);
				else
					strcpy(timebuf, "0");
			}
			space2str(toqb(q->dq_dqb.dqb_curspace), numbuf[0], spaceunit);
			space2str(q->dq_dqb.dqb_bsoftlimit, numbuf[1], spaceunit);
			space2str(q->dq_dqb.dqb_bhardlimit, numbuf[2], spaceunit);
			printf(" %7s%c %6s %7s %7s", numbuf[0], bover ? '*' : ' ', numbuf[1],
			       numbuf[2], timebuf);

			if (!(flags & FL_RAWGRACE)) {
				if (iover)
					difftime2str(q->dq_dqb.dqb_itime, timebuf);
				else
					timebuf[0] = 0;
			}
			else {
				if (iover)
					sprintf(timebuf, "%llu", (long long unsigned int)q->dq_dqb.dqb_itime);
				else
					strcpy(timebuf, "0");
			}
			number2str(q->dq_dqb.dqb_curinodes, numbuf[0], inodeunit);
			number2str(q->dq_dqb.dqb_isoftlimit, numbuf[1], inodeunit);
			number2str(q->dq_dqb.dqb_ihardlimit, numbuf[2], inodeunit);
			printf(" %7s%c %6s %7s %7s\n", numbuf[0], iover ? '*' : ' ', numbuf[1],
			       numbuf[2], timebuf);
			continue;
		}
	}
	if (!(flags & FL_QUIET) && !lines && qlist)
		heading(type, id, name, unlimited ? _("none") : _("no limited resources used"));
	freeprivs(qlist);
out_handles:
	dispose_handle_list(handles);
	return over > 0 ? 1 : 0;
}

int main(int argc, char **argv)
{
	int ngroups;
	gid_t gidset[NGROUPS_MAX], *gidsetp;
	int i, ret, type = 0;
	struct option long_opts[] = {
		{ "help", 0, NULL, 'h' },
		{ "version", 0, NULL, 'V' },
		{ "user", 0, NULL, 'u' },
		{ "group", 0, NULL, 'g' },
		{ "project", 0, NULL, 'P' },
		{ "quiet", 0, NULL, 'q' },
		{ "verbose", 0, NULL, 'v' },
		{ "human-readable", 2, NULL, 's' },
		{ "always-resolve", 0, NULL, 256 },
		{ "raw-grace", 0, NULL, 'p' },
		{ "local-only", 0, NULL, 'l' },
		{ "no-autofs", 0, NULL, 'i' },
		{ "quiet-refuse", 0, NULL, 'Q' },
		{ "format", 1, NULL, 'F' },
		{ "no-wrap", 0, NULL, 'w' },
		{ "filesystem-list", 0, NULL, 'f' },
		{ "all-nfs", 0, NULL, 'A' },
		{ "no-mixed-pathnames", 0, NULL, 'm' },
		{ "show-mntpoint", 0, NULL, 257 },
		{ "hide-device", 0, NULL, 258 },
		{ NULL, 0, NULL, 0 }
	};

	gettexton();
	progname = basename(argv[0]);

	flags |= FL_SHOW_DEVICE;
	while ((ret = getopt_long(argc, argv, "hguqvs::PVliQF:wfApm", long_opts, NULL)) != -1) {
		switch (ret) {
		  case 'g':
			  flags |= FL_GROUP;
			  break;
		  case 'u':
			  flags |= FL_USER;
			  break;
		  case 'P':
			  flags |= FL_PROJECT;
			  break;
		  case 'q':
			  flags |= FL_QUIET;
			  break;
		  case 'v':
			  flags |= FL_VERBOSE;
			  break;
		  case 'F':
			  if ((fmt = name2fmt(optarg)) == QF_ERROR)	/* Error? */
				  exit(1);
			  break;
		  case 's':
			  inodeunit = spaceunit = S2S_AUTO;
			  if (optarg) {
				if (unitopt2unit(optarg, &spaceunit, &inodeunit) < 0)
					die(1, _("Bad output format units for human readable output: %s\n"), optarg);
			  }
			  break;
		  case 'p':
			  flags |= FL_RAWGRACE;
			  break;
		  case 256:
			  flags |= FL_NUMNAMES;
			  break;
		  case 'l':
			  flags |= FL_LOCALONLY;
			  break;
		  case 'Q':
			  flags |= FL_QUIETREFUSE;
			  break;
		  case 'i':
			  flags |= FL_NOAUTOFS;
			  break;
		  case 'w':
			  flags |= FL_NOWRAP;
			  break;
		  case 'f':
			  flags |= FL_FSLIST;
			  break;
		  case 'A':
			  flags |= FL_NFSALL;
			  break;
		  case 'm':
			  flags |= FL_NO_MIXED_PATHS;
			  break;
		  case 257:
			  flags |= FL_SHOW_MNTPOINT;
			  break;
		  case 258:
			  flags &= ~FL_SHOW_DEVICE;
			  break;
		  case 'V':
			  version();
			  exit(0);
		  case 'h':
		  default:
			  usage();
		}
	}
	argc -= optind;
	argv += optind;

	if (!(flags & FL_USER) && !(flags & FL_GROUP) && !(flags & FL_PROJECT))
		flags |= FL_USER;
	if (flags & FL_FSLIST && flags & (FL_LOCALONLY | FL_NOAUTOFS))
		errstr(_("Warning: Ignoring -%c when filesystem list specified.\n"), flags & FL_LOCALONLY ? 'l' : 'i');

	init_kernel_interface();

	ret = 0;
	if (argc == 0 || flags & FL_FSLIST) {
		if (flags & FL_FSLIST && argc == 0)
			die(1, _("No filesystem specified.\n"));
		if (flags & FL_USER)
			ret |= showquotas(USRQUOTA, getuid(), argc, argv);
		if (flags & FL_GROUP) {
			ngroups = sysconf(_SC_NGROUPS_MAX);
			if (ngroups > NGROUPS_MAX) {
				gidsetp = malloc(ngroups * sizeof(gid_t));
				if (!gidsetp)
					die(1, _("Gid set allocation (%d): %s\n"), ngroups, strerror(errno));
			} else {
				gidsetp = &gidset[0];
			}
			ngroups = getgroups(ngroups, gidsetp);
			if (ngroups < 0)
				die(1, _("getgroups(): %s\n"), strerror(errno));
			for (i = 0; i < ngroups; i++)
				ret |= showquotas(GRPQUOTA, gidsetp[i], argc, argv);
		}
		if (flags & FL_PROJECT)
			die(1, _("Project reports not supported without project name\n"));
		exit(ret);
	}

	if (flags & FL_USER)
		type++;
	if (flags & FL_GROUP)
		type++;
	if (flags & FL_PROJECT)
		type++;
	if (type > 1)
		usage();

	if (flags & FL_USER)
		for (; argc > 0; argc--, argv++)
			ret |= showquotas(USRQUOTA, user2uid(*argv, !!(flags & FL_NUMNAMES), NULL), 0, NULL);
	else if (flags & FL_GROUP)
		for (; argc > 0; argc--, argv++)
			ret |= showquotas(GRPQUOTA, group2gid(*argv, !!(flags & FL_NUMNAMES), NULL), 0, NULL);
	else if (flags & FL_PROJECT)
		for (; argc > 0; argc--, argv++)
			ret |= showquotas(PRJQUOTA, project2pid(*argv, !!(flags & FL_NUMNAMES), NULL), 0, NULL);
	return ret;
}
