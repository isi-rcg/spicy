/*
 *	State changes for the XFS Quota Manager.
 *	Copyright (c) 2001 Silicon Graphics, Inc.
 */

#include "config.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "quotaon.h"
#include "dqblk_xfs.h"

#define QOFF	1
#define ACCT	2
#define ENFD	3

/*
 *	Ensure we don't attempt to go into a dodgey state.
 */

static int xfs_state_check(int qcmd, int type, int flags, const char *dev, int roothack, int xopts)
{
	struct xfs_mem_dqinfo info;
	int state;
	char *acctstr = "";

	/* we never want to operate via -a in XFS quota */
	if (flags & STATEFLAG_ALL)
		return 0;	/* noop */

	if (quotactl(QCMD(Q_XFS_GETQSTAT, 0), dev, 0, (void *)&info) < 0) {
		errstr(_("quotactl() on %s: %s\n"), dev, strerror(errno));
		return -1;
	}

	/* establish current state before any transition */
	state = QOFF;
	if (type == USRQUOTA) {
		if (info.qs_flags & XFS_QUOTA_UDQ_ACCT)
			state = ACCT;
		if (info.qs_flags & XFS_QUOTA_UDQ_ENFD)
			state = ENFD;
	}
	else if (type == GRPQUOTA) {
		if (info.qs_flags & XFS_QUOTA_GDQ_ACCT)
			state = ACCT;
		if (info.qs_flags & XFS_QUOTA_GDQ_ENFD)
			state = ENFD;
	}
	else if (type == PRJQUOTA) {
		if (info.qs_flags & XFS_QUOTA_PDQ_ACCT)
			state = ACCT;
		if (info.qs_flags & XFS_QUOTA_PDQ_ENFD)
			state = ENFD;
	}

	switch (state) {
	  case QOFF:
		  switch (qcmd) {
		    case Q_XFS_QUOTARM:
			    return 1;
		    case Q_XFS_QUOTAON:
			    if (roothack) {
				    pinfo(_("Enabling %s quota on root filesystem"
					    " (reboot to take effect)\n"), _(type2name(type)));
				    return 1;
			    }
			    errstr(_("Enable XFS %s quota accounting during mount\n"),
				    _(type2name(type)));
			    return -1;
		    case Q_XFS_QUOTAOFF:
			    return 0;	/* noop */
		  }
		  break;
	  case ACCT:
		  switch (qcmd) {
		    case Q_XFS_QUOTARM:
			    errstr(_("Cannot delete %s quota on %s - "
					      "switch quota accounting off first\n"),
				    _(type2name(type)), dev);
			    return -1;
		    case Q_XFS_QUOTAON:
			    if (roothack) {
				    pinfo(_("Enabling %s quota on root filesystem"
					    " (reboot to take effect)\n"), _(type2name(type)));
				    return 1;
			    }
			    if (xopts & XFS_QUOTA_UDQ_ENFD ||
				xopts & XFS_QUOTA_GDQ_ENFD ||
				xopts & XFS_QUOTA_PDQ_ENFD) {
				    pinfo(_("Enabling %s quota enforcement on %s\n"), _(type2name(type)), dev);
				    return 1;
			    }
			    errstr(_("Already accounting %s quota on %s\n"),
					_(type2name(type)), dev);
			    return -1;
		    case Q_XFS_QUOTAOFF:
			    if (xopts & XFS_QUOTA_UDQ_ACCT ||
				xopts & XFS_QUOTA_GDQ_ACCT ||
				xopts & XFS_QUOTA_PDQ_ACCT) {
				    pinfo(_("Disabling %s quota accounting on %s\n"),
					   _(type2name(type)), dev);
			    	    return 1;
			    }
			    errstr(_("Quota enforcement already disabled for %s on %s\n"),
					_(type2name(type)), dev);
			    return -1;
		  }
		  break;

	  case ENFD:
		  switch (qcmd) {
		    case Q_XFS_QUOTARM:
			    errstr(_("Cannot delete %s quota on %s - "
				      "switch quota enforcement and accounting off first\n"),
				    _(type2name(type)), dev);
			    return -1;
		    case Q_XFS_QUOTAON:
			    errstr(_("Enforcing %s quota already on %s\n"),
				    _(type2name(type)), dev);
			    return -1;
		    case Q_XFS_QUOTAOFF:
			    if (xopts == XFS_QUOTA_UDQ_ACCT ||
				xopts == XFS_QUOTA_GDQ_ACCT ||
				xopts == XFS_QUOTA_PDQ_ACCT) {
				    errstr(_("Cannot switch off %s quota "
					"accounting on %s when enforcement is on\n"),
					_(type2name(type)), dev);
				    return -1;
			    }
			    if (xopts & XFS_QUOTA_UDQ_ACCT ||
				xopts & XFS_QUOTA_GDQ_ACCT ||
				xopts & XFS_QUOTA_PDQ_ACCT)
		    		    acctstr = _("and accounting ");
			    pinfo(_("Disabling %s quota enforcement %son %s\n"),
				  _(type2name(type)), acctstr, dev);
			    return 1;
		  }
		  break;
	}
	errstr(_("Unexpected XFS quota state sought on %s\n"), dev);
	return -1;
}

static int xfs_onoff(const char *dev, int type, int flags, int roothack, int xopts)
{
	int qoff, qcmd, check;

	qoff = (flags & STATEFLAG_OFF);
	qcmd = qoff ? Q_XFS_QUOTAOFF : Q_XFS_QUOTAON;
	check = xfs_state_check(qcmd, type, flags, dev, roothack, xopts);
	if (check != 1)
		return (check < 0);

	if (quotactl(QCMD(qcmd, type), dev, 0, (void *)&xopts) < 0) {
		errstr(_("quotactl on %s: %s\n"), dev, strerror(errno));
		return 1;
	}
	if (qoff)
		pinfo(_("%s: %s quotas turned off\n"), dev, _(type2name(type)));
	else
		pinfo(_("%s: %s quotas turned on\n"), dev, _(type2name(type)));
	return 0;
}

static int xfs_delete(const char *dev, int type, int flags, int roothack, int xopts)
{
	int qcmd, check;

	qcmd = Q_XFS_QUOTARM;
	check = xfs_state_check(qcmd, type, flags, dev, roothack, xopts);
	if (check != 1)
		return (check < 0);

	if (quotactl(QCMD(qcmd, type), dev, 0, (void *)&xopts) < 0) {
		errstr(_("Failed to delete quota: %s\n"),
			strerror(errno));
		return 1;
	}

	pinfo(_("%s: deleted %s quota blocks\n"), dev, _(type2name(type)));
	return 0;
}

/*
 *	Change state for given filesystem - on/off, acct/enfd, & delete.
 *	Must consider existing state and also whether or not this is the
 *	root filesystem.
 *	We are passed in the new requested state through "type" & "xarg".
 */
int xfs_newstate(struct mount_entry *mnt, int type, char *xarg, int flags)
{
	int err = 1;
	int xopts = 0;
	int roothack = 0;

#ifdef XFS_ROOTHACK
	/*
	 * Old XFS filesystems (up to XFS 1.2 / Linux 2.5.47) had a
	 * hack to allow enabling quota on the root filesystem without
	 * having to specify it at mount time.
	 */
	if ((strcmp(mnt->me_dir, "/") == 0)) {
		struct xfs_mem_dqinfo info;
		u_int16_t sbflags = 0;

		if (!quotactl(QCMD(Q_XFS_GETQSTAT, type), mnt->me_devname, 0, (void *)&info))
			sbflags = (info.qs_flags & 0xff00) >> 8;

		if ((type == USRQUOTA && (sbflags & XFS_QUOTA_UDQ_ACCT)) &&
		    (type == GRPQUOTA && (sbflags & XFS_QUOTA_GDQ_ACCT)))
			roothack = 1;
	}
#endif /* XFS_ROOTHACK */

	if (xarg == NULL || strcmp(xarg, "enforce") == 0) {	/* only enfd on/off */
		if (type == USRQUOTA)
			xopts |= XFS_QUOTA_UDQ_ENFD;
		else if (type == GRPQUOTA)
			xopts |= XFS_QUOTA_GDQ_ENFD;
		else if (type == PRJQUOTA)
			xopts |= XFS_QUOTA_PDQ_ENFD;
		err = xfs_onoff(mnt->me_devname, type, flags, roothack, xopts);
	}
	else if (strcmp(xarg, "account") == 0) {
		if (type == USRQUOTA)
			xopts |= XFS_QUOTA_UDQ_ACCT;
		else if (type == GRPQUOTA)
			xopts |= XFS_QUOTA_GDQ_ACCT;
		else if (type == PRJQUOTA)
			xopts |= XFS_QUOTA_PDQ_ACCT;
		err = xfs_onoff(mnt->me_devname, type, flags, roothack, xopts);
	}
	else if (strcmp(xarg, "delete") == 0) {
		if (type == USRQUOTA)
			xopts |= XFS_USER_QUOTA;
		else if (type == GRPQUOTA)
			xopts |= XFS_GROUP_QUOTA;
		else if (type == PRJQUOTA)
			xopts |= XFS_PROJ_QUOTA;
		err = xfs_delete(mnt->me_devname, type, flags, roothack, xopts);
	}
	else
		die(1, _("Invalid argument \"%s\"\n"), xarg);
	return err;
}
