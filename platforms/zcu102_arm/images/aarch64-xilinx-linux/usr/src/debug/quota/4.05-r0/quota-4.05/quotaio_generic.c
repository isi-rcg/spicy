/*
 *	Implementation of communication with kernel generic interface
 *
 *	Jan Kara <jack@suse.cz> - sponsored by SuSE CR
 */

#include "config.h"

#include <errno.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <stdlib.h>
#include <sys/types.h>

#include "pot.h"
#include "common.h"
#include "quotaio.h"
#include "quota.h"
#include "quotasys.h"

/* Convert kernel quotablock format to utility one */
static inline void generic_kern2utildqblk(struct util_dqblk *u, struct if_dqblk *k)
{
	u->dqb_ihardlimit = k->dqb_ihardlimit;
	u->dqb_isoftlimit = k->dqb_isoftlimit;
	u->dqb_bhardlimit = k->dqb_bhardlimit;
	u->dqb_bsoftlimit = k->dqb_bsoftlimit;
	u->dqb_curinodes = k->dqb_curinodes;
	u->dqb_curspace = k->dqb_curspace;
	u->dqb_itime = k->dqb_itime;
	u->dqb_btime = k->dqb_btime;
}

/* Convert utility quotablock format to kernel one */
static inline void generic_util2kerndqblk(struct if_dqblk *k, struct util_dqblk *u)
{
	k->dqb_ihardlimit = u->dqb_ihardlimit;
	k->dqb_isoftlimit = u->dqb_isoftlimit;
	k->dqb_bhardlimit = u->dqb_bhardlimit;
	k->dqb_bsoftlimit = u->dqb_bsoftlimit;
	k->dqb_curinodes = u->dqb_curinodes;
	k->dqb_curspace = u->dqb_curspace;
	k->dqb_itime = u->dqb_itime;
	k->dqb_btime = u->dqb_btime;
}

/* Get info from kernel to handle */
int vfs_get_info(struct quota_handle *h)
{
	struct if_dqinfo kinfo;

	if (quotactl(QCMD(Q_GETINFO, h->qh_type), h->qh_quotadev, 0, (void *)&kinfo) < 0) {
		errstr(_("Cannot get info for %s quota file from kernel on %s: %s\n"), type2name(h->qh_type), h->qh_quotadev, strerror(errno));
		return -1;
	}
	h->qh_info.dqi_bgrace = kinfo.dqi_bgrace;
	h->qh_info.dqi_igrace = kinfo.dqi_igrace;
	return 0;
}

/* Set info in kernel from handle */
int vfs_set_info(struct quota_handle *h, int flags)
{
	struct if_dqinfo kinfo;

	kinfo.dqi_bgrace = h->qh_info.dqi_bgrace;
	kinfo.dqi_igrace = h->qh_info.dqi_igrace;
	kinfo.dqi_valid = flags;

	if (quotactl(QCMD(Q_SETINFO, h->qh_type), h->qh_quotadev, 0, (void *)&kinfo) < 0) {
		errstr(_("Cannot set info for %s quota file from kernel on %s: %s\n"), type2name(h->qh_type), h->qh_quotadev, strerror(errno));
		return -1;
	}
	return 0;
}

/* Get dquot from kernel */
int vfs_get_dquot(struct dquot *dquot)
{
	struct if_dqblk kdqblk;

	if (quotactl(QCMD(Q_GETQUOTA, dquot->dq_h->qh_type), dquot->dq_h->qh_quotadev, dquot->dq_id, (void *)&kdqblk) < 0) {
		errstr(_("Cannot get quota for %s %d from kernel on %s: %s\n"), type2name(dquot->dq_h->qh_type), dquot->dq_id, dquot->dq_h->qh_quotadev, strerror(errno));
		return -1;
	}
	generic_kern2utildqblk(&dquot->dq_dqb, &kdqblk);
	return 0;
}

/* Set dquot in kernel */
int vfs_set_dquot(struct dquot *dquot, int flags)
{
	struct if_dqblk kdqblk;

	generic_util2kerndqblk(&kdqblk, &dquot->dq_dqb);
	kdqblk.dqb_valid = flags;
	if (quotactl(QCMD(Q_SETQUOTA, dquot->dq_h->qh_type), dquot->dq_h->qh_quotadev, dquot->dq_id, (void *)&kdqblk) < 0) {
		errstr(_("Cannot set quota for %s %d from kernel on %s: %s\n"), type2name(dquot->dq_h->qh_type), dquot->dq_id, dquot->dq_h->qh_quotadev, strerror(errno));
		return -1;
	}
	return 0;
}

static int scan_one_dquot(struct dquot *dquot, int (*get_dquot)(struct dquot *))
{
	int ret;
	struct util_dqblk *dqb = &dquot->dq_dqb;

	memset(dqb, 0, sizeof(struct util_dqblk));
	ret = get_dquot(dquot);
	if (ret < 0)
		return ret;
	if (!dqb->dqb_bhardlimit && !dqb->dqb_bsoftlimit && !dqb->dqb_ihardlimit && !dqb->dqb_isoftlimit && !dqb->dqb_curinodes && !dqb->dqb_curspace)
		return 1;
	return 0;
}

/* Generic quota scanning using passwd... */
int generic_scan_dquots(struct quota_handle *h,
			int (*process_dquot)(struct dquot *dquot, char *dqname),
			int (*get_dquot)(struct dquot *dquot))
{
	struct dquot *dquot = get_empty_dquot();
	int ret = 0;

	dquot->dq_h = h;
	if (h->qh_type == USRQUOTA) {
		struct passwd *usr;

		setpwent();
		while ((usr = getpwent()) != NULL) {
			dquot->dq_id = usr->pw_uid;
			ret = scan_one_dquot(dquot, get_dquot);
			if (ret < 0)
				break;
			if (ret > 0)
				continue;
			ret = process_dquot(dquot, usr->pw_name);
			if (ret < 0)
				break;
		}
		endpwent();
	} else if (h->qh_type == GRPQUOTA) {
		struct group *grp;

		setgrent();
		while ((grp = getgrent()) != NULL) {
			dquot->dq_id = grp->gr_gid;
			ret = scan_one_dquot(dquot, get_dquot);
			if (ret < 0)
				break;
			if (ret > 0)
				continue;
			ret = process_dquot(dquot, grp->gr_name);
			if (ret < 0)
				break;
		}
		endgrent();
	} else if (h->qh_type == PRJQUOTA) {
		struct fs_project *prj;

		setprent();
		while ((prj = getprent()) != NULL) {
			dquot->dq_id = prj->pr_id;
			ret = scan_one_dquot(dquot, get_dquot);
			if (ret < 0)
				break;
			if (ret > 0)
				continue;
			ret = process_dquot(dquot, prj->pr_name);
			if (ret < 0)
				break;
		}
		endprent();
	}
	free(dquot);
	return ret;
}

int vfs_scan_dquots(struct quota_handle *h,
		    int (*process_dquot)(struct dquot *dquot, char *dqname))
{
	struct dquot *dquot = get_empty_dquot();
	qid_t id = 0;
	struct if_nextdqblk kdqblk;
	int ret;

	dquot->dq_h = h;
	while (1) {
		ret = quotactl(QCMD(Q_GETNEXTQUOTA, h->qh_type),
			       h->qh_quotadev, id, (void *)&kdqblk);
		if (ret < 0)
			break;

		/*
		 * This is a slight hack but we know struct if_dqblk is a
		 * subset of struct if_nextdqblk
		 */
		generic_kern2utildqblk(&dquot->dq_dqb,
				       (struct if_dqblk *)&kdqblk);
		dquot->dq_id = kdqblk.dqb_id;
		ret = process_dquot(dquot, NULL);
		if (ret < 0)
			break;
		id = kdqblk.dqb_id + 1;
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
