/*
 * Utility for turning quotas on and off and reporting their state.
 */

#include "config.h"

/*
 * Turn quota on/off for a filesystem.
 */
#include <stdio.h>
#include <errno.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "quotaon.h"
#include "quota.h"
#include "quotasys.h"

#define FL_USER 1
#define FL_GROUP 2
#define FL_VERBOSE 4
#define FL_ALL 8
#define FL_STAT 16
#define FL_OFF 32
#define FL_PROJECT 64

static int flags, fmt = -1;
char *progname;
static char **mntpoints;
static int mntcnt;
static char *xarg = NULL;

static void usage(void)
{
	errstr(_("Usage:\n\t%s [-guPvp] [-F quotaformat] [-x state] -a\n\
\t%s [-guPvp] [-F quotaformat] [-x state] filesys ...\n\n\
-a, --all                %s\n\
-f, --off                turn quotas off\n\
-u, --user               operate on user quotas\n\
-g, --group              operate on group quotas\n\
-P, --project            operate on project quotas\n\
-p, --print-state        print whether quotas are on or off\n\
-x, --xfs-command=cmd    perform XFS quota command\n\
-F, --format=formatname  operate on specific quota format\n\
-v, --verbose            print more messages\n\
-h, --help               display this help text and exit\n\
-V, --version            display version information and exit\n"),
 progname, progname,
 strcmp(progname, "quotaon") ? _("turn quotas off for all filesystems") :
			       _("turn quotas on for all filesystems"));
	exit(1);
}

static void parse_options(int argcnt, char **argstr)
{
	int c;
	struct option long_opts[] = {
		{ "all", 0, NULL, 'a' },
		{ "off", 0, NULL, 'f' },
		{ "verbose", 0, NULL, 'v' },
		{ "user", 0, NULL, 'u' },
		{ "group", 0, NULL, 'g' },
		{ "project", 0, NULL, 'P' },
		{ "print-state", 0, NULL, 'p' },
		{ "xfs-command", 1, NULL, 'x' },
		{ "format", 1, NULL, 'F' },
		{ "version", 0, NULL, 'V' },
		{ "help", 0, NULL, 'h' },
		{ NULL, 0, NULL, 0 }
	};

	while ((c = getopt_long(argcnt, argstr, "afvugpPx:VF:h", long_opts, NULL)) != -1) {
		switch (c) {
		  case 'a':
			  flags |= FL_ALL;
			  break;
		  case 'f':
			  flags |= FL_OFF;
			  break;
		  case 'g':
			  flags |= FL_GROUP;
			  break;
		  case 'u':
			  flags |= FL_USER;
			  break;
		  case 'P':
			  flags |= FL_PROJECT;
			  break;
		  case 'v':
			  flags |= FL_VERBOSE;
			  break;
		  case 'x':
			  xarg = optarg;
			  break;
		  case 'p':
			  flags |= FL_STAT;
			  break;
		  case 'F':
			  if ((fmt = name2fmt(optarg)) == QF_ERROR)
				exit(1);
			  break;
		  case 'V':
			  version();
			  exit(0);
		  case 'h':
		  default:
			  usage();
		}
	}
	if ((flags & FL_ALL && optind != argcnt) || (!(flags & FL_ALL) && optind == argcnt)) {
		fputs(_("Bad number of arguments.\n"), stderr);
		usage();
	}
	if (fmt == QF_RPC) {
		fputs(_("Cannot turn on/off quotas via RPC.\n"), stderr);
		exit(1);
	}
	if (!(flags & (FL_USER | FL_GROUP | FL_PROJECT)))
		flags |= FL_USER | FL_GROUP | FL_PROJECT;
	if (!(flags & FL_ALL)) {
		mntpoints = argstr + optind;
		mntcnt = argcnt - optind;
	}
}

int pinfo(char *fmt, ...)
{
	va_list arg;
	int ret;

	if (!(flags & FL_VERBOSE))
		return 0;
	va_start(arg, fmt);
	ret = vprintf(fmt, arg);
	va_end(arg);
	return ret;
}

/*
 *	Enable/disable rsquash on given filesystem
 */
static int quotarsquashonoff(const char *quotadev, int type, int flags)
{
#if defined(MNTOPT_RSQUASH)
	int ret;

	if (kernel_iface == IFACE_GENERIC) {
		int qcmd = QCMD(Q_SETINFO, type);
		struct if_dqinfo info;

		info.dqi_flags = V1_DQF_RSQUASH;
		info.dqi_valid = IIF_FLAGS;
		ret = quotactl(qcmd, quotadev, 0, (void *)&info);
	}
	else {
		int mode = (flags & STATEFLAG_OFF) ? 0 : 1;
		int qcmd = QCMD(Q_V1_RSQUASH, type);

		ret = quotactl(qcmd, quotadev, 0, (void *)&mode);
	}
	if (ret < 0) {
		errstr(_("set root_squash on %s: %s\n"), quotadev, strerror(errno));
		return 1;
	}
	if (flags & STATEFLAG_OFF)
		pinfo(_("%s: %s root_squash turned off\n"), quotadev, type2name(type));
	else if (flags & STATEFLAG_ON)
		pinfo(_("%s: %s root_squash turned on\n"), quotadev, type2name(type));
#endif
	return 0;
}

/*
 *	Enable/disable VFS quota on given filesystem
 */
static int quotaonoff(const char *quotadev, const char *quotadir, char *quotafile, int type, int fmt, int flags)
{
	int qcmd, kqf;

	if (flags & STATEFLAG_OFF) {
		if (kernel_iface == IFACE_GENERIC)
			qcmd = QCMD(Q_QUOTAOFF, type);
		else
			qcmd = QCMD(Q_6_5_QUOTAOFF, type);
		if (quotactl(qcmd, quotadev, 0, NULL) < 0) {
			errstr(_("quotactl on %s [%s]: %s\n"), quotadev, quotadir, strerror(errno));
			return 1;
		}
		pinfo(_("%s [%s]: %s quotas turned off\n"), quotadev, quotadir, _(type2name(type)));
		return 0;
	}
	if (kernel_iface == IFACE_GENERIC) {
		qcmd = QCMD(Q_QUOTAON, type);
 		kqf = util2kernfmt(fmt);
	}
	else {
		qcmd = QCMD(Q_6_5_QUOTAON, type);
		kqf = 0;
	}
	if (quotactl(qcmd, quotadev, kqf, (void *)quotafile) < 0) {
		if (errno == ENOENT)
			errstr(_("cannot find %s on %s [%s]\n"), quotafile, quotadev, quotadir);
		else
			errstr(_("using %s on %s [%s]: %s\n"), quotafile, quotadev, quotadir, strerror(errno));
		if (errno == EINVAL)
			errstr(_("Maybe create new quota files with quotacheck(8)?\n"));
		else if (errno == ESRCH)
			errstr(_("Quota format not supported in kernel.\n"));
		return 1;
	}
	pinfo(_("%s [%s]: %s quotas turned on\n"), quotadev, quotadir, _(type2name(type)));
	return 0;
}

/*
 *	Enable/disable quota/rootsquash on given filesystem (version 1)
 */
static int v1_newstate(struct mount_entry *mnt, int type, char *file, int flags, int fmt)
{
	int errs = 0;

	if ((flags & STATEFLAG_OFF) && str_hasmntopt(mnt->me_opts, MNTOPT_RSQUASH))
		errs += quotarsquashonoff(mnt->me_devname, type, flags);
	errs += quotaonoff(mnt->me_devname, mnt->me_dir, file, type, QF_VFSOLD, flags);
	if ((flags & STATEFLAG_ON) && str_hasmntopt(mnt->me_opts, MNTOPT_RSQUASH))
		errs += quotarsquashonoff(mnt->me_devname, type, flags);
	return errs;
}

/*
 *	Enable/disable quota on given filesystem (generic VFS quota)
 */
static int v2_newstate(struct mount_entry *mnt, int type, char *file, int flags, int fmt)
{
	return quotaonoff(mnt->me_devname, mnt->me_dir, file, type, fmt, flags);
}

/*
 *	For both VFS quota formats, need to pass in the quota file;
 *	for XFS quota manager, pass on the -x command line option.
 */
static int newstate(struct mount_entry *mnt, int type, char *extra)
{
	int sflags, ret = 0;

	sflags = flags & FL_OFF ? STATEFLAG_OFF : STATEFLAG_ON;
	if (flags & FL_ALL)
		sflags |= STATEFLAG_ALL;

	if (!strcmp(mnt->me_type, MNTTYPE_GFS2)) {
		errstr(_("Cannot change state of GFS2 quota.\n"));
		return 1;
	} else if (!strcmp(mnt->me_type, MNTTYPE_XFS) ||
		   !strcmp(mnt->me_type, MNTTYPE_EXFS)) {	/* XFS filesystem has special handling... */
		if (!kern_qfmt_supp(QF_XFS)) {
			errstr(_("Cannot change state of XFS quota. It's not compiled in kernel.\n"));
			return 1;
		}
		ret = xfs_newstate(mnt, type, extra, sflags);
	}
	else if (mnt->me_qfmt[type] == QF_META) {
		/* Must be non-empty because empty path is always invalid. */
		ret = v2_newstate(mnt, type, ".", sflags, QF_VFSV0);
	}
	else {
		int usefmt;

		if (!me_hasquota(mnt, type))
			return 0;
		if (fmt == -1) {
			if (get_qf_name(mnt, type, QF_VFSV0,
					NF_FORMAT, &extra) >= 0)
				usefmt = QF_VFSV0;
			else if (get_qf_name(mnt, type, QF_VFSV1,
					NF_FORMAT, &extra) >= 0)
				usefmt = QF_VFSV1;
			else if (get_qf_name(mnt, type, QF_VFSOLD,
					NF_FORMAT, &extra) >= 0)
				usefmt = QF_VFSOLD;
			else {
				errstr(_("Cannot find quota file on %s [%s] to turn quotas on/off.\n"), mnt->me_dir, mnt->me_devname);
				return 1;
			}
		} else {
			if (get_qf_name(mnt, type, fmt, NF_FORMAT, &extra) < 0) {
				errstr(_("Quota file on %s [%s] does not exist or has wrong format.\n"), mnt->me_dir, mnt->me_devname);
				return 1;
			}
			usefmt = fmt;
		}
		if (is_tree_qfmt(usefmt))
			ret = v2_newstate(mnt, type, extra, sflags, usefmt);
		else
			ret = v1_newstate(mnt, type, extra, sflags, QF_VFSOLD);
		free(extra);
	}
	return ret;
}

/* Print state of quota (on/off) */
static int print_state(struct mount_entry *mnt, int type)
{
	int on = 0;
	char *state;

	if (kern_qfmt_supp(QF_XFS)) {
		on = kern_quota_state_xfs(mnt->me_devname, type);
		if (!strcmp(mnt->me_type, MNTTYPE_XFS) ||
		    !strcmp(mnt->me_type, MNTTYPE_GFS2) || on >= 0 ||
		    !strcmp(mnt->me_type, MNTTYPE_EXFS)) {
			if (on < 0)
				on = 0;
			if (!(flags & FL_VERBOSE))
				goto print_state;
			if (on == 0)
				state = _("off");
			else if (on == 1)
				state = _("on (accounting)");
			else
				state = _("on (enforced)");
			goto print;
		}
	}
	if (kernel_iface == IFACE_GENERIC)
		on = kern_quota_on(mnt, type, -1) != -1;
	else if (kern_qfmt_supp(QF_VFSV0))
		on = kern_quota_on(mnt, type, QF_VFSV0) != -1;
	else if (kern_qfmt_supp(QF_VFSOLD))
		on = kern_quota_on(mnt, type, QF_VFSOLD) != -1;

print_state:
	state = on ? _("on") : _("off");
print:
	printf(_("%s quota on %s (%s) is %s\n"), _(type2name(type)),
		mnt->me_dir, mnt->me_devname, state);
	
	return on > 0;
}

int main(int argc, char **argv)
{
	struct mount_entry *mnt;
	int errs = 0;

	gettexton();

	progname = basename(argv[0]);
	if (strcmp(progname, "quotaoff") == 0)
		flags |= FL_OFF;
	else if (strcmp(progname, "quotaon") != 0)
		die(1, _("Name must be quotaon or quotaoff not %s\n"), progname);

	parse_options(argc, argv);

	init_kernel_interface();
	if (fmt != -1 && !kern_qfmt_supp(fmt))
		die(1, _("Required format %s not supported by kernel.\n"), fmt2name(fmt));
	else if (!kern_qfmt_supp(-1))
		errstr(_("Warning: No quota format detected in the kernel.\n"));

	if (init_mounts_scan(mntcnt, mntpoints, MS_XFS_DISABLED | MS_LOCALONLY) < 0)
		return 1;
	while ((mnt = get_next_mount())) {
		if (nfs_fstype(mnt->me_type)) {
			if (!(flags & FL_ALL))
				errstr(_("%s: Quota cannot be turned on on NFS filesystem\n"), mnt->me_devname);
			continue;
		}

		if (!(flags & FL_STAT)) {
			if (flags & FL_GROUP)
				errs += newstate(mnt, GRPQUOTA, xarg);
			if (flags & FL_USER)
				errs += newstate(mnt, USRQUOTA, xarg);
			if (flags & FL_PROJECT)
				errs += newstate(mnt, PRJQUOTA, xarg);
		}
		else {
			if (flags & FL_GROUP)
				errs += print_state(mnt, GRPQUOTA);
			if (flags & FL_USER)
				errs += print_state(mnt, USRQUOTA);
			if (flags & FL_PROJECT)
				errs += print_state(mnt, PRJQUOTA);
		}
	}
	end_mounts_scan();

	return errs;
}

