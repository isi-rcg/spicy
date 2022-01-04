/*
 * Functions for handling old quota format with sparse quota file
 */

#include "config.h"

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "pot.h"
#include "common.h"
#include "quotaio_v1.h"
#include "dqblk_v1.h"
#include "quotaio.h"
#include "quotasys.h"
#include "quotaio_generic.h"

static int v1_check_file(int fd, int type, int fmt);
static int v1_init_io(struct quota_handle *h);
static int v1_new_io(struct quota_handle *h);
static int v1_write_info(struct quota_handle *h);
static struct dquot *v1_read_dquot(struct quota_handle *h, qid_t id);
static int v1_commit_dquot(struct dquot *dquot, int flags);
static int v1_scan_dquots(struct quota_handle *h, int (*process_dquot) (struct dquot *dquot, char *dqname));

struct quotafile_ops quotafile_ops_1 = {
check_file:	v1_check_file,
init_io:	v1_init_io,
new_io:		v1_new_io,
write_info:	v1_write_info,
read_dquot:	v1_read_dquot,
commit_dquot:	v1_commit_dquot,
scan_dquots:	v1_scan_dquots,
};

/*
 *	Copy dquot from disk to memory
 */
static inline void v1_disk2memdqblk(struct util_dqblk *m, struct v1_disk_dqblk *d)
{
	m->dqb_ihardlimit = d->dqb_ihardlimit;
	m->dqb_isoftlimit = d->dqb_isoftlimit;
	m->dqb_bhardlimit = d->dqb_bhardlimit;
	m->dqb_bsoftlimit = d->dqb_bsoftlimit;
	m->dqb_curinodes = d->dqb_curinodes;
	m->dqb_curspace = ((qsize_t)d->dqb_curblocks) * V1_DQBLK_SIZE;
	m->dqb_itime = d->dqb_itime;
	m->dqb_btime = d->dqb_btime;
}

/*
 *	Copy dquot from memory to disk
 */
static inline void v1_mem2diskdqblk(struct v1_disk_dqblk *d, struct util_dqblk *m)
{
	d->dqb_ihardlimit = m->dqb_ihardlimit;
	d->dqb_isoftlimit = m->dqb_isoftlimit;
	d->dqb_bhardlimit = m->dqb_bhardlimit;
	d->dqb_bsoftlimit = m->dqb_bsoftlimit;
	d->dqb_curinodes = m->dqb_curinodes;
	d->dqb_curblocks = m->dqb_curspace >> V1_DQBLK_SIZE_BITS;
	d->dqb_itime = m->dqb_itime;
	d->dqb_btime = m->dqb_btime;
}

/* Convert kernel quotablock format to utility one */
static inline void v1_kern2utildqblk(struct util_dqblk *u, struct v1_kern_dqblk *k)
{
	u->dqb_ihardlimit = k->dqb_ihardlimit;
	u->dqb_isoftlimit = k->dqb_isoftlimit;
	u->dqb_bhardlimit = k->dqb_bhardlimit;
	u->dqb_bsoftlimit = k->dqb_bsoftlimit;
	u->dqb_curinodes = k->dqb_curinodes;
	u->dqb_curspace = ((qsize_t)k->dqb_curblocks) << V1_DQBLK_SIZE_BITS;
	u->dqb_itime = k->dqb_itime;
	u->dqb_btime = k->dqb_btime;
}

/* Convert utility quotablock format to kernel one */
static inline void v1_util2kerndqblk(struct v1_kern_dqblk *k, struct util_dqblk *u)
{
	k->dqb_ihardlimit = u->dqb_ihardlimit;
	k->dqb_isoftlimit = u->dqb_isoftlimit;
	k->dqb_bhardlimit = u->dqb_bhardlimit;
	k->dqb_bsoftlimit = u->dqb_bsoftlimit;
	k->dqb_curinodes = u->dqb_curinodes;
	k->dqb_curblocks = (u->dqb_curspace + V1_DQBLK_SIZE - 1) >> V1_DQBLK_SIZE_BITS;
	k->dqb_itime = u->dqb_itime;
	k->dqb_btime = u->dqb_btime;
}

/*
 *	Check whether quotafile is in our format
 */
static int v1_check_file(int fd, int type, int fmt)
{
	struct stat st;

	if (fstat(fd, &st) < 0)
		return 0;
	if (!st.st_size || st.st_size % sizeof(struct v1_disk_dqblk))
		return 0;
	return 1;
}

/*
 *	Open quotafile
 */
static int v1_init_io(struct quota_handle *h)
{
	if (QIO_ENABLED(h)) {
		if (kernel_iface == IFACE_GENERIC) {
			if (vfs_get_info(h) < 0)
				return -1;
		}
		else {
			struct v1_kern_dqblk kdqblk;

			if (quotactl(QCMD(Q_V1_GETQUOTA, h->qh_type), h->qh_quotadev, 0, (void *)&kdqblk) < 0) {
				if (errno == EPERM) {	/* We have no permission to get this information? */
					h->qh_info.dqi_bgrace = h->qh_info.dqi_igrace = 0;	/* It hopefully won't be needed */
				}
				else
					return -1;
			}
			else {
				h->qh_info.dqi_bgrace = kdqblk.dqb_btime;
				h->qh_info.dqi_igrace = kdqblk.dqb_itime;
			}
		}
	}
	else {
		struct v1_disk_dqblk ddqblk;

		lseek(h->qh_fd, 0, SEEK_SET);
		if (read(h->qh_fd, &ddqblk, sizeof(ddqblk)) != sizeof(ddqblk))
			return -1;
		h->qh_info.dqi_bgrace = ddqblk.dqb_btime;
		h->qh_info.dqi_igrace = ddqblk.dqb_itime;
	}
	if (!h->qh_info.dqi_bgrace)
		h->qh_info.dqi_bgrace = MAX_DQ_TIME;
	if (!h->qh_info.dqi_igrace)
		h->qh_info.dqi_igrace = MAX_IQ_TIME;
	h->qh_info.dqi_max_b_limit = ~(uint32_t)0;
	h->qh_info.dqi_max_i_limit = ~(uint32_t)0;
	h->qh_info.dqi_max_b_usage = ((uint64_t)(~(uint32_t)0)) << V1_DQBLK_SIZE_BITS;
	h->qh_info.dqi_max_i_usage = ~(uint32_t)0;

	return 0;
}

/*
 *	Initialize new quotafile
 */
static int v1_new_io(struct quota_handle *h)
{
	struct v1_disk_dqblk ddqblk;

	/* Write at least roots dquot with grace times */
	memset(&ddqblk, 0, sizeof(ddqblk));
	ddqblk.dqb_btime = MAX_DQ_TIME;
	ddqblk.dqb_itime = MAX_IQ_TIME;
	h->qh_info.dqi_bgrace = MAX_DQ_TIME;
	h->qh_info.dqi_igrace = MAX_IQ_TIME;
	h->qh_info.dqi_max_b_limit = ~(uint32_t)0;
	h->qh_info.dqi_max_i_limit = ~(uint32_t)0;
	h->qh_info.dqi_max_b_usage = ((uint64_t)(~(uint32_t)0)) << V1_DQBLK_SIZE_BITS;
	h->qh_info.dqi_max_i_usage = ~(uint32_t)0;
	lseek(h->qh_fd, 0, SEEK_SET);
	if (write(h->qh_fd, &ddqblk, sizeof(ddqblk)) != sizeof(ddqblk))
		return -1;
	return 0;
}

/*
 *	Write information (grace times to file)
 */
static int v1_write_info(struct quota_handle *h)
{
	if (QIO_RO(h)) {
		errstr(_("Trying to write info to readonly quotafile on %s.\n"), h->qh_quotadev);
		errno = EPERM;
		return -1;
	}
	if (QIO_ENABLED(h)) {
		if (kernel_iface == IFACE_GENERIC) {
			if (vfs_set_info(h, IIF_BGRACE | IIF_IGRACE) < 0)
				return -1;
		}
		else {
			struct v1_kern_dqblk kdqblk;

			if (quotactl(QCMD(Q_V1_GETQUOTA, h->qh_type), h->qh_quotadev, 0, (void *)&kdqblk) < 0)
				return -1;
			kdqblk.dqb_btime = h->qh_info.dqi_bgrace;
			kdqblk.dqb_itime = h->qh_info.dqi_igrace;
			if (quotactl(QCMD(Q_V1_SETQUOTA, h->qh_type), h->qh_quotadev, 0, (void *)&kdqblk) < 0)
				return -1;
		}
	}
	else {
		struct v1_disk_dqblk ddqblk;

		lseek(h->qh_fd, 0, SEEK_SET);
		if (read(h->qh_fd, &ddqblk, sizeof(ddqblk)) != sizeof(ddqblk))
			return -1;
		ddqblk.dqb_btime = h->qh_info.dqi_bgrace;
		ddqblk.dqb_itime = h->qh_info.dqi_igrace;
		lseek(h->qh_fd, 0, SEEK_SET);
		if (write(h->qh_fd, &ddqblk, sizeof(ddqblk)) != sizeof(ddqblk))
			return -1;
	}
	return 0;
}

/*
 *	Read a dqblk struct from the quotafile.
 *	User can use 'errno' to detect errstr.
 */
static struct dquot *v1_read_dquot(struct quota_handle *h, qid_t id)
{
	struct v1_disk_dqblk ddqblk;
	struct dquot *dquot = get_empty_dquot();

	dquot->dq_id = id;
	dquot->dq_h = h;
	if (QIO_ENABLED(h)) {	/* Does kernel use the file? */
		if (kernel_iface == IFACE_GENERIC) {
			if (vfs_get_dquot(dquot) < 0) {
				free(dquot);
				return NULL;
			}
		}
		else {
			struct v1_kern_dqblk kdqblk;

			if (quotactl(QCMD(Q_V1_GETQUOTA, h->qh_type), h->qh_quotadev, id, (void *)&kdqblk) < 0) {
				free(dquot);
				return NULL;
			}
			v1_kern2utildqblk(&dquot->dq_dqb, &kdqblk);
		}
	}
	else {
		lseek(h->qh_fd, (long)V1_DQOFF(id), SEEK_SET);
		switch (read(h->qh_fd, &ddqblk, sizeof(ddqblk))) {
			case 0:	/* EOF */
				/*
				 * Convert implicit 0 quota (EOF) into an
				 * explicit one (zero'ed dqblk)
				 */
				memset(&dquot->dq_dqb, 0, sizeof(struct util_dqblk));
				break;
			case sizeof(struct v1_disk_dqblk):	/* OK */
				v1_disk2memdqblk(&dquot->dq_dqb, &ddqblk);
				break;
			default:	/* ERROR */
				free(dquot);
				return NULL;
		}
	}
	return dquot;
}

/*
 *	Write a dqblk struct to the quotafile.
 *	User can process use 'errno' to detect errstr
 */
static int v1_commit_dquot(struct dquot *dquot, int flags)
{
	struct v1_disk_dqblk ddqblk;
	struct quota_handle *h = dquot->dq_h;

	if (QIO_RO(h)) {
		errstr(_("Trying to write quota to readonly quotafile on %s\n"), h->qh_quotadev);
		errno = EPERM;
		return -1;
	}
	if (QIO_ENABLED(h)) {	/* Kernel uses same file? */
		if (kernel_iface == IFACE_GENERIC) {
			if (vfs_set_dquot(dquot, flags) < 0)
				return -1;
		}
		else {
			struct v1_kern_dqblk kdqblk;
			int cmd;

			if (flags == COMMIT_USAGE)
				cmd = Q_V1_SETUSE;
			else if (flags == COMMIT_LIMITS)
				cmd = Q_V1_SETQLIM;
			else if (flags & COMMIT_TIMES) {
				errno = EINVAL;
				return -1;
			}
			else
				cmd = Q_V1_SETQUOTA;
			v1_util2kerndqblk(&kdqblk, &dquot->dq_dqb);
			if (quotactl(QCMD(cmd, h->qh_type), h->qh_quotadev, dquot->dq_id,
			     (void *)&kdqblk) < 0)
				return -1;
		}
	}
	else {
		if (check_dquot_range(dquot) < 0) {
			errno = ERANGE;
			return -1;
		}
		v1_mem2diskdqblk(&ddqblk, &dquot->dq_dqb);
		lseek(h->qh_fd, (long)V1_DQOFF(dquot->dq_id), SEEK_SET);
		if (write(h->qh_fd, &ddqblk, sizeof(ddqblk)) != sizeof(ddqblk))
			return -1;
	}
	return 0;
}

/*
 *	Scan all dquots in file and call callback on each
 */
#define SCANBUFSIZE 256

static int v1_scan_dquots(struct quota_handle *h, int (*process_dquot) (struct dquot *, char *))
{
	int rd, scanbufpos = 0, scanbufsize = 0;
	char scanbuf[sizeof(struct v1_disk_dqblk)*SCANBUFSIZE];
	struct v1_disk_dqblk *ddqblk;
	struct dquot *dquot = get_empty_dquot();
	qid_t id = 0;

	memset(dquot, 0, sizeof(*dquot));
	dquot->dq_h = h;
	lseek(h->qh_fd, 0, SEEK_SET);
	for(id = 0; ; id++, scanbufpos++) {
		if (scanbufpos >= scanbufsize) {
			rd = read(h->qh_fd, scanbuf, sizeof(scanbuf));
			if (rd < 0 || rd % sizeof(struct v1_disk_dqblk))
				goto out_err;
			if (!rd)
				break;
			scanbufpos = 0;
			scanbufsize = rd / sizeof(struct v1_disk_dqblk);
		}
		ddqblk = ((struct v1_disk_dqblk *)scanbuf) + scanbufpos;
		if ((ddqblk->dqb_ihardlimit | ddqblk->dqb_isoftlimit |
		     ddqblk->dqb_bhardlimit | ddqblk->dqb_bsoftlimit |
		     ddqblk->dqb_curblocks | ddqblk->dqb_curinodes |
		     ddqblk->dqb_itime | ddqblk->dqb_btime) == 0)
			continue;
		v1_disk2memdqblk(&dquot->dq_dqb, ddqblk);
		dquot->dq_id = id;
		if ((rd = process_dquot(dquot, NULL)) < 0) {
			free(dquot);
			return rd;
		}
	}
	if (!rd) {		/* EOF? */
		free(dquot);
		return 0;
	}
out_err:
	free(dquot);
	return -1;		/* Some read errstr... */
}
