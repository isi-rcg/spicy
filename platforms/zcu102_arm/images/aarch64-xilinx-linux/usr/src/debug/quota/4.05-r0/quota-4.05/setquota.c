/*
 *
 *	Set disk quota from command line 
 *
 *	Jan Kara <jack@suse.cz> - sponsored by SuSE CR
 */

#include "config.h"

#if defined(RPC)
#include <rpc/rpc.h>
#endif
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <time.h>
#include <ctype.h>
#include <stdlib.h>

#if defined(RPC)
#include "rquota.h"
#include "rquota_client.h"
#endif
#include "pot.h"
#include "quotaops.h"
#include "common.h"
#include "quotasys.h"

#define FL_USER 1
#define FL_GROUP 2
#define FL_RPC 4
#define FL_ALL 8
#define FL_PROTO 16
#define FL_GRACE 32
#define FL_INDIVIDUAL_GRACE 64
#define FL_BATCH 128
#define FL_NUMNAMES 256
#define FL_NO_MIXED_PATHS 512
#define FL_CONTINUE_BATCH 1024
#define FL_PROJECT 2048

static int flags, fmt = -1;
static char **mnt;
char *progname;
static int mntcnt;
static qid_t protoid, id;
static struct util_dqblk toset;

/* Print usage information */
static void usage(void)
{
#if defined(RPC_SETQUOTA)
	char *ropt = "[-rm] ";
#else
	char *ropt = "";
#endif
	errstr(_("Usage:\n\
  setquota [-u|-g|-P] %1$s[-F quotaformat] <user|group|project>\n\
\t<block-softlimit> <block-hardlimit> <inode-softlimit> <inode-hardlimit> -a|<filesystem>...\n\
  setquota [-u|-g|-P] %1$s[-F quotaformat] <-p protouser|protogroup|protoproject> <user|group|project> -a|<filesystem>...\n\
  setquota [-u|-g|-P] %1$s[-F quotaformat] -b [-c] -a|<filesystem>...\n\
  setquota [-u|-g|-P] [-F quotaformat] -t <blockgrace> <inodegrace> -a|<filesystem>...\n\
  setquota [-u|-g|-P] [-F quotaformat] <user|group|project> -T <blockgrace> <inodegrace> -a|<filesystem>...\n\n\
-u, --user                 set limits for user\n\
-g, --group                set limits for group\n\
-P, --project              set limits for project\n\
-a, --all                  set limits for all filesystems\n\
    --always-resolve       always try to resolve name, even if is\n\
                           composed only of digits\n\
-F, --format=formatname    operate on specific quota format\n\
-p, --prototype=protoname  copy limits from user/group/project\n\
-b, --batch                read limits from standard input\n\
-c, --continue-batch       continue in input processing in case of an error\n"), ropt);
#if defined(RPC_SETQUOTA)
	fputs(_("-r, --remote               set remote quota (via RPC)\n\
-m, --no-mixed-pathnames      trim leading slashes from NFSv4 mountpoints\n"), stderr);
#endif
	fputs(_("-t, --edit-period          edit grace period\n\
-T, --edit-times           edit grace times for user/group/project\n\
-h, --help                 display this help text and exit\n\
-V, --version              display version information and exit\n\n"), stderr);
	fprintf(stderr, _("Bugs to: %s\n"), PACKAGE_BUGREPORT);
	exit(1);
}

/* Convert string to number - print errstr message in case of failure */
static qsize_t parse_unum(char *str, char *msg)
{
	char *errch;
	qsize_t ret = strtoull(str, &errch, 0);

	if (*errch) {
		errstr(_("%s: %s\n"), msg, str);
		usage();
	}
	return ret;
}

/* Convert block size to number - print errstr message in case of failure */
static qsize_t parse_blocksize(const char *str, const char *msg)
{
	qsize_t ret;
	const char *error = str2space(str, &ret);

	if (error) {
		errstr(_("%s: %s: %s\n"), msg, str, error);
		usage();
	}
	return ret;
}

/* Convert inode count to number - print errstr message in case of failure */
static qsize_t parse_inodecount(const char *str, const char *msg)
{
	qsize_t ret;
	const char *error = str2number(str, &ret);

	if (error) {
		errstr(_("%s: %s: %s\n"), msg, str, error);
		usage();
	}
	return ret;
}

/* Convert our flags to quota type */
static inline int flag2type(int flags)
{
	if (flags & FL_USER)
		return USRQUOTA;
	if (flags & FL_GROUP)
		return GRPQUOTA;
	if (flags & FL_PROJECT)
		return PRJQUOTA;
	return -1;
}

/* Parse options of setquota */
static void parse_options(int argcnt, char **argstr)
{
	int ret, otherargs;
	char *protoname = NULL;
	int type = 0;
#ifdef RPC_SETQUOTA
	char *opts = "ghp:uPrmVF:taTbc";
#else
	char *opts = "ghp:uPVF:taTbc";
#endif
	struct option long_opts[] = {
		{ "user", 0, NULL, 'u' },
		{ "group", 0, NULL, 'g' },
		{ "project", 0, NULL, 'P' },
		{ "prototype", 1, NULL, 'p' },
#ifdef RPC_SETQUOTA
		{ "remote", 0, NULL, 'r' },
		{ "no-mixed-pathnames", 0, NULL, 'm' },
#endif
		{ "all", 0, NULL, 'a' },
		{ "always-resolve", 0, NULL, 256},
		{ "edit-period", 0, NULL, 't' },
		{ "edit-times", 0, NULL, 'T' },
		{ "batch", 0, NULL, 'b' },
		{ "continue", 0, NULL, 'c' },
		{ "format", 1, NULL, 'F' },
		{ "version", 0, NULL, 'V' },
		{ "help", 0, NULL, 'h' },
		{ NULL, 0, NULL, 0 }
	};

	while ((ret = getopt_long(argcnt, argstr, opts, long_opts, NULL)) != -1) {
		switch (ret) {
		  case '?':
		  case 'h':
			  usage();
		  case 'g':
			  flags |= FL_GROUP;
			  break;
		  case 'u':
			  flags |= FL_USER;
			  break;
		  case 'P':
			  flags |= FL_PROJECT;
			  break;
		  case 'p':
			  flags |= FL_PROTO;
			  protoname = optarg;
			  break;
		  case 'r':
			  flags |= FL_RPC;
			  break;
		  case 'm':
			  flags |= FL_NO_MIXED_PATHS;
			  break;
		  case 'a':
			  flags |= FL_ALL;
			  break;
		  case 256:
			  flags |= FL_NUMNAMES;
			  break;
		  case 't':
			  flags |= FL_GRACE;
			  break;
		  case 'b':
			  flags |= FL_BATCH;
			  break;
		  case 'c':
			  flags |= FL_CONTINUE_BATCH;
			  break;
		  case 'T':
			  flags |= FL_INDIVIDUAL_GRACE;
			  break;
		  case 'F':
			  if ((fmt = name2fmt(optarg)) == QF_ERROR)
				  exit(1);
			  break;
		  case 'V':
			  version();
			  exit(0);
		}
	}
	if (flags & FL_USER)
		type++;
	if (flags & FL_GROUP)
		type++;
	if (flags & FL_PROJECT)
		type++;
	if (type > 1) {
		errstr(_("Group/user/project quotas cannot be used together.\n"));
		usage();
	}
	if (flags & FL_PROTO && flags & FL_GRACE) {
		errstr(_("Prototype user has no sense when editing grace times.\n"));
		usage();
	}
	if (flags & FL_INDIVIDUAL_GRACE && flags & FL_GRACE) {
		errstr(_("Cannot set both individual and global grace time.\n"));
		usage();
	}
	if (flags & FL_BATCH && flags & (FL_GRACE | FL_INDIVIDUAL_GRACE)) {
		errstr(_("Batch mode cannot be used for setting grace times.\n"));
		usage();
	}
	if (flags & FL_BATCH && flags & FL_PROTO) {
		errstr(_("Batch mode and prototype user cannot be used together.\n"));
		usage();
	}
	if (flags & FL_RPC && (flags & (FL_GRACE | FL_INDIVIDUAL_GRACE))) {
		errstr(_("Cannot set grace times over RPC protocol.\n"));
		usage();
	}
	if (flags & FL_GRACE)
		otherargs = 2;
	else if (flags & FL_INDIVIDUAL_GRACE)
		otherargs = 3;
	else if (flags & FL_BATCH)
		otherargs = 0;
	else {
		otherargs = 1;
		if (!(flags & FL_PROTO))
			otherargs += 4;
	}
	if (optind + otherargs > argcnt) {
		errstr(_("Bad number of arguments.\n"));
		usage();
	}
	if (!(flags & (FL_USER | FL_GROUP | FL_PROJECT)))
		flags |= FL_USER;
	if (!(flags & (FL_GRACE | FL_BATCH))) {
		id = name2id(argstr[optind++], flag2type(flags), !!(flags & FL_NUMNAMES), NULL);
		if (!(flags & (FL_GRACE | FL_INDIVIDUAL_GRACE | FL_PROTO))) {
			toset.dqb_bsoftlimit = parse_blocksize(argstr[optind++], _("Bad block softlimit"));
			toset.dqb_bhardlimit = parse_blocksize(argstr[optind++], _("Bad block hardlimit"));
			toset.dqb_isoftlimit = parse_inodecount(argstr[optind++], _("Bad inode softlimit"));
			toset.dqb_ihardlimit = parse_inodecount(argstr[optind++], _("Bad inode hardlimit"));
		}
		else if (flags & FL_PROTO)
			protoid = name2id(protoname, flag2type(flags), !!(flags & FL_NUMNAMES), NULL);
	}
	if (flags & FL_GRACE) {
		toset.dqb_btime = parse_unum(argstr[optind++], _("Bad block grace time"));
		toset.dqb_itime = parse_unum(argstr[optind++], _("Bad inode grace time"));
	}
	else if (flags & FL_INDIVIDUAL_GRACE) {
		time_t now;

		time(&now);
		if (!strcmp(argstr[optind], _("unset"))) {
			toset.dqb_btime = 0;
			optind++;
		}
		else
			toset.dqb_btime = now + parse_unum(argstr[optind++], _("Bad block grace time"));
		if (!strcmp(argstr[optind], _("unset"))) {
			toset.dqb_itime = 0;
			optind++;
		}
		else
			toset.dqb_itime = now + parse_unum(argstr[optind++], _("Bad inode grace time"));
	}
	if (!(flags & FL_ALL)) {
		mntcnt = argcnt - optind;
		mnt = argstr + optind;
		if (!mntcnt) {
			errstr(_("Mountpoint not specified.\n"));
			usage();
		}
	}
}

/* Set user limits */
static int setlimits(struct quota_handle **handles)
{
	struct dquot *q, *protoq, *protoprivs = NULL, *curprivs;
	int ret = 0;

	curprivs = getprivs(id, handles, 0);
	if (flags & FL_PROTO) {
		protoprivs = getprivs(protoid, handles, 0);
		for (q = curprivs, protoq = protoprivs; q && protoq; q = q->dq_next, protoq = protoq->dq_next) {
			q->dq_dqb.dqb_bsoftlimit = protoq->dq_dqb.dqb_bsoftlimit;
			q->dq_dqb.dqb_bhardlimit = protoq->dq_dqb.dqb_bhardlimit;
			q->dq_dqb.dqb_isoftlimit = protoq->dq_dqb.dqb_isoftlimit;
			q->dq_dqb.dqb_ihardlimit = protoq->dq_dqb.dqb_ihardlimit;
			update_grace_times(q);
		}
		freeprivs(protoprivs);
	}
	else {
		for (q = curprivs; q; q = q->dq_next) {
			q->dq_dqb.dqb_bsoftlimit = toset.dqb_bsoftlimit;
			q->dq_dqb.dqb_bhardlimit = toset.dqb_bhardlimit;
			q->dq_dqb.dqb_isoftlimit = toset.dqb_isoftlimit;
			q->dq_dqb.dqb_ihardlimit = toset.dqb_ihardlimit;
			update_grace_times(q);
		}
	}
	if (putprivs(curprivs, COMMIT_LIMITS) == -1)
		ret = -1;
	freeprivs(curprivs);
	return ret;
}

#define MAXLINELEN 65536

/* Read & parse one batch entry */
static int read_entry(qid_t *id, qsize_t *isoftlimit, qsize_t *ihardlimit, qsize_t *bsoftlimit, qsize_t *bhardlimit)
{
	static int line = 0;
	char name[MAXNAMELEN+1];
	char linebuf[MAXLINELEN], *chptr;
	char is[MAXNAMELEN+1], ih[MAXNAMELEN+1];
	char bs[MAXNAMELEN+1], bh[MAXNAMELEN+1];
	const char *error;
	int ret;

	while (1) {
		line++;
		if (!fgets(linebuf, sizeof(linebuf), stdin))
			return -1;
		if (linebuf[strlen(linebuf)-1] != '\n')
			die(1, _("Line %d too long.\n"), line);
		/* Comment? */
		if (linebuf[0] == '#')
			continue;
		/* Blank line? */
		chptr = linebuf;
		while (isblank(*chptr))
			chptr++;
		if (*chptr == '\n')
			continue;
		ret = sscanf(chptr, "%s %s %s %s %s", name, bs, bh, is, ih);
		if (ret != 5) {
			errstr(_("Cannot parse input line %d.\n"), line);
			if (!(flags & FL_CONTINUE_BATCH))
				die(1, _("Exitting.\n"));
			errstr(_("Skipping line.\n"));
			continue;
		}
		*id = name2id(name, flag2type(flags), !!(flags & FL_NUMNAMES), &ret);
		if (ret) {
			errstr(_("Unable to resolve name '%s' on line %d.\n"), name, line);
			if (!(flags & FL_CONTINUE_BATCH))
				die(1, _("Exitting.\n"));
			errstr(_("Skipping line.\n"));
			continue;
		}
		error = str2space(bs, bsoftlimit);
		if (error) {
			errstr(_("Unable to parse block soft limit '%s' "
				    "on line %d: %s\n"), bs, line, error);
			if (!(flags & FL_CONTINUE_BATCH))
				die(1, _("Exitting.\n"));
			errstr(_("Skipping line.\n"));
			continue;
		}
		error = str2space(bh, bhardlimit);
		if (error) {
			errstr(_("Unable to parse block hard limit '%s' "
				    "on line %d: %s\n"), bh, line, error);
			if (!(flags & FL_CONTINUE_BATCH))
				die(1, _("Exitting.\n"));
			errstr(_("Skipping line.\n"));
			continue;
		}
		error = str2number(is, isoftlimit);
		if (error) {
			errstr(_("Unable to parse inode soft limit '%s' "
				    "on line %d: %s\n"), is, line, error);
			if (!(flags & FL_CONTINUE_BATCH))
				die(1, _("Exitting.\n"));
			errstr(_("Skipping line.\n"));
			continue;
		}
		error = str2number(ih, ihardlimit);
		if (error) {
			errstr(_("Unable to parse inode hard limit '%s' "
				    "on line %d: %s\n"), ih, line, error);
			if (!(flags & FL_CONTINUE_BATCH))
				die(1, _("Exitting.\n"));
			errstr(_("Skipping line.\n"));
			continue;
		}
		break;
	}
	return 0;
}

/* Set user limits in batch mode */
static int batch_setlimits(struct quota_handle **handles)
{
	struct dquot *curprivs, *q;
	qsize_t bhardlimit, bsoftlimit, ihardlimit, isoftlimit;
	qid_t id;
	int ret = 0;

	while (!read_entry(&id, &isoftlimit, &ihardlimit, &bsoftlimit, &bhardlimit)) {
		curprivs = getprivs(id, handles, 0);
		for (q = curprivs; q; q = q->dq_next) {
			q->dq_dqb.dqb_bsoftlimit = bsoftlimit;
			q->dq_dqb.dqb_bhardlimit = bhardlimit;
			q->dq_dqb.dqb_isoftlimit = isoftlimit;
			q->dq_dqb.dqb_ihardlimit = ihardlimit;
			update_grace_times(q);
		}
		if (putprivs(curprivs, COMMIT_LIMITS) == -1)
			ret = -1;
		freeprivs(curprivs);
	}
	return ret;
}

/* Set grace times */
static int setgraces(struct quota_handle **handles)
{
	int i, ret = 0;

	for (i = 0; handles[i]; i++) {
		if (!handles[i]->qh_ops->write_info) {
			errstr(_("Setting grace period on %s is not supported.\n"), handles[i]->qh_quotadev);
			ret = -1;
			continue;
		}
		handles[i]->qh_info.dqi_bgrace = toset.dqb_btime;
		handles[i]->qh_info.dqi_igrace = toset.dqb_itime;
		mark_quotafile_info_dirty(handles[i]);
	}
	return ret;
}

/* Set grace times for individual user */
static int setindivgraces(struct quota_handle **handles)
{
	int ret = 0;
	struct dquot *q, *curprivs;

	curprivs = getprivs(id, handles, 0);
	for (q = curprivs; q; q = q->dq_next) {
		if (q->dq_dqb.dqb_bsoftlimit && toqb(q->dq_dqb.dqb_curspace) > q->dq_dqb.dqb_bsoftlimit)
			q->dq_dqb.dqb_btime = toset.dqb_btime;
		else
			errstr(_("Not setting block grace time on %s because softlimit is not exceeded.\n"), q->dq_h->qh_quotadev);
		if (q->dq_dqb.dqb_isoftlimit && q->dq_dqb.dqb_curinodes > q->dq_dqb.dqb_isoftlimit)
			q->dq_dqb.dqb_itime = toset.dqb_itime;
		else
			errstr(_("Not setting inode grace time on %s because softlimit is not exceeded.\n"), q->dq_h->qh_quotadev);
	}
	if (putprivs(curprivs, COMMIT_TIMES) == -1) {
		int type;

		if (flags & FL_USER)
			type = USRQUOTA;
		else if (flags & FL_GROUP)
			type = GRPQUOTA;
		else
			type = PRJQUOTA;
		errstr(_("cannot write times for %s. Maybe kernel does not support such operation?\n"), _(type2name(type)));
		ret = -1;
	}
	freeprivs(curprivs);
	return ret;
}

int main(int argc, char **argv)
{
	struct quota_handle **handles;
	int ret;

	gettexton();
	progname = basename(argv[0]);

	parse_options(argc, argv);
	init_kernel_interface();

	if (flags & FL_ALL)
		handles = create_handle_list(0, NULL, flag2type(flags), fmt,
			(flags & FL_NO_MIXED_PATHS) ? 0 : IOI_NFS_MIXED_PATHS,
			(flags & FL_RPC) ? 0 : MS_LOCALONLY);
	else
		handles = create_handle_list(mntcnt, mnt, flag2type(flags), fmt,
			(flags & FL_NO_MIXED_PATHS) ? 0 : IOI_NFS_MIXED_PATHS,
			(flags & FL_RPC) ? 0 : MS_LOCALONLY);

	if (flags & FL_GRACE)
		ret = setgraces(handles);
	else if (flags & FL_INDIVIDUAL_GRACE)
		ret = setindivgraces(handles);
	else if (flags & FL_BATCH)
		ret = batch_setlimits(handles);
	else
		ret = setlimits(handles);

	if (dispose_handle_list(handles) == -1)
		ret = -1;

	return ret ? 1 : 0;
}
