/*
 *	Implementation of XFS quota manager.
 *      Copyright (c) 2001 Silicon Graphics, Inc.
 */

#include "config.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "pot.h"
#include "common.h"
#include "bylabel.h"
#include "quotaio.h"
#include "quotasys.h"
#include "dqblk_xfs.h"
#include "quotaio_generic.h"

#define XFS_USRQUOTA(h)	((h)->qh_type == USRQUOTA && \
			(h)->qh_info.u.xfs_mdqi.qs_flags & XFS_QUOTA_UDQ_ACCT)
#define XFS_GRPQUOTA(h)	((h)->qh_type == GRPQUOTA && \
			(h)->qh_info.u.xfs_mdqi.qs_flags & XFS_QUOTA_GDQ_ACCT)
#define XFS_PRJQUOTA(h)	((h)->qh_type == PRJQUOTA && \
			(h)->qh_info.u.xfs_mdqi.qs_flags & XFS_QUOTA_PDQ_ACCT)

static int xfs_init_io(struct quota_handle *h);
static int xfs_write_info(struct quota_handle *h);
static struct dquot *xfs_read_dquot(struct quota_handle *h, qid_t id);
static int xfs_commit_dquot(struct dquot *dquot, int flags);
static int xfs_scan_dquots(struct quota_handle *h, int (*process_dquot) (struct dquot *dquot, char *dqname));
static int xfs_report(struct quota_handle *h, int verbose);

struct quotafile_ops quotafile_ops_xfs = {
init_io:	xfs_init_io,
write_info:	xfs_write_info,
read_dquot:	xfs_read_dquot,
commit_dquot:	xfs_commit_dquot,
scan_dquots:	xfs_scan_dquots,
report:		xfs_report
};

/*
 *	Convert XFS kernel quota format to utility format
 */
static inline void xfs_kern2utildqblk(struct util_dqblk *u, struct xfs_kern_dqblk * k)
{
	u->dqb_ihardlimit = k->d_ino_hardlimit;
	u->dqb_isoftlimit = k->d_ino_softlimit;
	u->dqb_bhardlimit = k->d_blk_hardlimit >> 1;
	u->dqb_bsoftlimit = k->d_blk_softlimit >> 1;
	u->dqb_curinodes = k->d_icount;
	u->dqb_curspace = ((qsize_t)k->d_bcount) << 9;
	u->dqb_itime = k->d_itimer;
	u->dqb_btime = k->d_btimer;
}

/*
 *	Convert utility quota format to XFS kernel format
 */
static inline void xfs_util2kerndqblk(struct xfs_kern_dqblk *k, struct util_dqblk *u)
{
	memset(k, 0, sizeof(struct xfs_kern_dqblk));
	k->d_ino_hardlimit = u->dqb_ihardlimit;
	k->d_ino_softlimit = u->dqb_isoftlimit;
	k->d_blk_hardlimit = u->dqb_bhardlimit << 1;
	k->d_blk_softlimit = u->dqb_bsoftlimit << 1;
	k->d_icount = u->dqb_curinodes;
	k->d_bcount = u->dqb_curspace >> 9;
	k->d_itimer = u->dqb_itime;
	k->d_btimer = u->dqb_btime;
}

/*
 *	Initialize quota information
 */
static int xfs_init_io(struct quota_handle *h)
{
	struct xfs_mem_dqinfo info;
	int qcmd;

	qcmd = QCMD(Q_XFS_GETQSTAT, 0);
	memset(&info, 0, sizeof(struct xfs_mem_dqinfo));
	if (quotactl(qcmd, h->qh_quotadev, 0, (void *)&info) < 0)
		return -1;
	h->qh_info.dqi_bgrace = info.qs_btimelimit;
	h->qh_info.dqi_igrace = info.qs_itimelimit;
	h->qh_info.u.xfs_mdqi = info;
	return 0;
}

/*
 *	Write information (grace times)
 */
static int xfs_write_info(struct quota_handle *h)
{
	struct xfs_kern_dqblk xdqblk;
	int qcmd;

	if (!XFS_USRQUOTA(h) && !XFS_GRPQUOTA(h) && !XFS_PRJQUOTA(h))
		return 0;

	memset(&xdqblk, 0, sizeof(struct xfs_kern_dqblk));

	xdqblk.d_btimer = h->qh_info.dqi_bgrace;
	xdqblk.d_itimer = h->qh_info.dqi_igrace;
	xdqblk.d_fieldmask |= FS_DQ_TIMER_MASK;
	qcmd = QCMD(Q_XFS_SETQLIM, h->qh_type);
	if (quotactl(qcmd, h->qh_quotadev, 0, (void *)&xdqblk) < 0)
		return -1;
	return 0;
}

/*
 *	Read a dqblk struct from the quota manager
 */
static struct dquot *xfs_read_dquot(struct quota_handle *h, qid_t id)
{
	struct dquot *dquot = get_empty_dquot();
	struct xfs_kern_dqblk xdqblk;
	int qcmd;

	dquot->dq_id = id;
	dquot->dq_h = h;

	if (!XFS_USRQUOTA(h) && !XFS_GRPQUOTA(h) && !XFS_PRJQUOTA(h))
		return dquot;

	qcmd = QCMD(Q_XFS_GETQUOTA, h->qh_type);
	if (quotactl(qcmd, h->qh_quotadev, id, (void *)&xdqblk) < 0) {
		;
	}
	else {
		xfs_kern2utildqblk(&dquot->dq_dqb, &xdqblk);
	}
	return dquot;
}

/*
 *	Write a dqblk struct to the XFS quota manager
 */
static int xfs_commit_dquot(struct dquot *dquot, int flags)
{
	struct quota_handle *h = dquot->dq_h;
	struct xfs_kern_dqblk xdqblk;
	qid_t id = dquot->dq_id;
	int qcmd;

	if (!XFS_USRQUOTA(h) && !XFS_GRPQUOTA(h) && !XFS_PRJQUOTA(h))
		return 0;

	xfs_util2kerndqblk(&xdqblk, &dquot->dq_dqb);
	if (XFS_USRQUOTA(h))
		xdqblk.d_flags |= XFS_USER_QUOTA;
	else if (XFS_GRPQUOTA(h))
		xdqblk.d_flags |= XFS_GROUP_QUOTA;
	else if (XFS_PRJQUOTA(h))
		xdqblk.d_flags |= XFS_PROJ_QUOTA;
	xdqblk.d_id = id;
	if (strcmp(h->qh_fstype, MNTTYPE_GFS2) == 0) {
		if (flags & COMMIT_LIMITS) /* warn/limit */
			xdqblk.d_fieldmask |= FS_DQ_BSOFT | FS_DQ_BHARD;
		if (flags & COMMIT_USAGE) /* block usage */
			xdqblk.d_fieldmask |= FS_DQ_BCOUNT;
	} else {
		xdqblk.d_fieldmask |= FS_DQ_LIMIT_MASK;
	}

	qcmd = QCMD(Q_XFS_SETQLIM, h->qh_type);
	if (quotactl(qcmd, h->qh_quotadev, id, (void *)&xdqblk) < 0) {
		;
	}
	else {
		return 0;
	}
	return -1;
}

/*
 *	xfs_scan_dquots helper - processes a single dquot
 */
static int xfs_get_dquot(struct dquot *dq)
{
	struct xfs_kern_dqblk d;
	int qcmd = QCMD(Q_XFS_GETQUOTA, dq->dq_h->qh_type);
	int ret;

	memset(&d, 0, sizeof(d));
	ret = quotactl(qcmd, dq->dq_h->qh_quotadev, dq->dq_id, (void *)&d);
	if (ret < 0) {
		if (errno == ENOENT)
			return 0;
		return -1;
	}
	xfs_kern2utildqblk(&dq->dq_dqb, &d);
	return 0;
}

static int xfs_kernel_scan_dquots(struct quota_handle *h,
		int (*process_dquot)(struct dquot *dquot, char *dqname))
{
	struct dquot *dquot = get_empty_dquot();
	qid_t id = 0;
	struct xfs_kern_dqblk xdqblk;
	int ret;

	dquot->dq_h = h;
	while (1) {
		ret = quotactl(QCMD(Q_XGETNEXTQUOTA, h->qh_type),
			       h->qh_quotadev, id, (void *)&xdqblk);
		if (ret < 0)
			break;

		xfs_kern2utildqblk(&dquot->dq_dqb, &xdqblk);
		dquot->dq_id = xdqblk.d_id;
		ret = process_dquot(dquot, NULL);
		if (ret < 0)
			break;
		id = xdqblk.d_id + 1;
		/* id -1 is invalid and the last one... */
		if (id == -1) {
			errno = ENOENT;
			break;
		}
	}
	free(dquot);

	if (errno == ENOENT)
		return 0;
	return ret;
}

/*
 *	Scan all known dquots and call callback on each
 */
static int xfs_scan_dquots(struct quota_handle *h, int (*process_dquot) (struct dquot *dquot, char *dqname))
{
	int ret;
	struct xfs_kern_dqblk xdqblk;

	ret = quotactl(QCMD(Q_XGETNEXTQUOTA, h->qh_type), h->qh_quotadev, 0,
		       (void *)&xdqblk);
	if (ret < 0 && (errno == ENOSYS || errno == EINVAL)) {
		if (!XFS_USRQUOTA(h) && !XFS_GRPQUOTA(h) && !XFS_PRJQUOTA(h))
			return 0;
		return generic_scan_dquots(h, process_dquot, xfs_get_dquot);
	}
	return xfs_kernel_scan_dquots(h, process_dquot);
}

/*
 *	Report information about XFS quota on given filesystem
 */
static int xfs_report(struct quota_handle *h, int verbose)
{
	u_int16_t sbflags;
	struct xfs_mem_dqinfo *info = &h->qh_info.u.xfs_mdqi;

	if (!verbose)
		return 0;

	/* quotaon/off flags */
	printf(_("*** Status for %s quotas on device %s\n"), type2name(h->qh_type), h->qh_quotadev);

#define XQM_ON(flag) ((info->qs_flags & (flag)) ? _("ON") : _("OFF"))
	if (h->qh_type == USRQUOTA) {
		printf(_("Accounting: %s; Enforcement: %s\n"),
		       XQM_ON(XFS_QUOTA_UDQ_ACCT), XQM_ON(XFS_QUOTA_UDQ_ENFD));
	}
	else if (h->qh_type == GRPQUOTA) {
		printf(_("Accounting: %s; Enforcement: %s\n"),
		       XQM_ON(XFS_QUOTA_GDQ_ACCT), XQM_ON(XFS_QUOTA_GDQ_ENFD));
	}
	else if (h->qh_type == PRJQUOTA) {
		printf(_("Accounting: %s; Enforcement: %s\n"),
		       XQM_ON(XFS_QUOTA_PDQ_ACCT), XQM_ON(XFS_QUOTA_PDQ_ENFD));
	}
#undef XQM_ON

	/*
	 * If this is the root file system, it is possible that quotas are
	 * on ondisk, but not incore. Those flags will be in the HI 8 bits.
	 */
#define XQM_ONDISK(flag) ((sbflags & (flag)) ? _("ON") : _("OFF"))
	if ((sbflags = (info->qs_flags & 0xff00) >> 8) != 0) {
		if (h->qh_type == USRQUOTA) {
			printf(_("Accounting [ondisk]: %s; Enforcement [ondisk]: %s\n"),
			       XQM_ONDISK(XFS_QUOTA_UDQ_ACCT), XQM_ONDISK(XFS_QUOTA_UDQ_ENFD));
		}
		else if (h->qh_type == GRPQUOTA) {
			printf(_("Accounting [ondisk]: %s; Enforcement [ondisk]: %s\n"),
			       XQM_ONDISK(XFS_QUOTA_GDQ_ACCT), XQM_ONDISK(XFS_QUOTA_GDQ_ENFD));
		}
		else if (h->qh_type == PRJQUOTA) {
			printf(_("Accounting [ondisk]: %s; Enforcement [ondisk]: %s\n"),
			       XQM_ONDISK(XFS_QUOTA_PDQ_ACCT), XQM_ONDISK(XFS_QUOTA_PDQ_ENFD));
		}
#undef XQM_ONDISK
	}

	/* user and group quota file status information */
	if (h->qh_type == USRQUOTA) {
		if (info->qs_uquota.qfs_ino == -1 || info->qs_uquota.qfs_ino == 0)
			printf(_("Inode: none\n"));
		else
			printf(_("Inode: #%llu (%llu blocks, %u extents)\n"),
			       (unsigned long long)info->qs_uquota.qfs_ino,
			       (unsigned long long)info->qs_uquota.qfs_nblks,
			       info->qs_uquota.qfs_nextents);
	}
	else if (h->qh_type == GRPQUOTA) {
		if (info->qs_gquota.qfs_ino == -1)
			printf(_("Inode: none\n"));
		else
			printf(_("Inode: #%llu (%llu blocks, %u extents)\n"),
			       (unsigned long long)info->qs_gquota.qfs_ino,
			       (unsigned long long)info->qs_gquota.qfs_nblks,
			       info->qs_gquota.qfs_nextents);
	}
	else if (h->qh_type == PRJQUOTA) {
		/*
		 * FIXME: Older XFS use group files for project quotas, newer
		 * have dedicated file and we should detect that.
		 */
		if (info->qs_gquota.qfs_ino == -1)
			printf(_("Inode: none\n"));
		else
			printf(_("Inode: #%llu (%llu blocks, %u extents)\n"),
			       (unsigned long long)info->qs_gquota.qfs_ino,
			       (unsigned long long)info->qs_gquota.qfs_nblks,
			       info->qs_gquota.qfs_nextents);
	}
	return 0;
}
