/*
 *	Implementation of new quotafile format
 *
 *	Jan Kara <jack@suse.cz> - sponsored by SuSE CR
 */

#include "config.h"

#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <endian.h>

#include "pot.h"
#include "common.h"
#include "quotaio_v2.h"
#include "dqblk_v2.h"
#include "quotaio.h"
#include "quotasys.h"
#include "quotaio_generic.h"

typedef char *dqbuf_t;

static int v2_check_file(int fd, int type, int fmt);
static int v2_init_io(struct quota_handle *h);
static int v2_new_io(struct quota_handle *h);
static int v2_write_info(struct quota_handle *h);
static struct dquot *v2_read_dquot(struct quota_handle *h, qid_t id);
static int v2_commit_dquot(struct dquot *dquot, int flags);
static int v2_scan_dquots(struct quota_handle *h, int (*process_dquot) (struct dquot *dquot, char *dqname));
static int v2_report(struct quota_handle *h, int verbose);

struct quotafile_ops quotafile_ops_2 = {
check_file:	v2_check_file,
init_io:	v2_init_io,
new_io:		v2_new_io,
write_info:	v2_write_info,
read_dquot:	v2_read_dquot,
commit_dquot:	v2_commit_dquot,
scan_dquots:	v2_scan_dquots,
report:	v2_report
};

#define getdqbuf() smalloc(V2_DQBLKSIZE)
#define freedqbuf(buf) free(buf)

/*
 *	Copy dquot from disk to memory
 */
static void v2r0_disk2memdqblk(struct dquot *dquot, void *dp)
{
	struct util_dqblk *m = &dquot->dq_dqb;
	struct v2r0_disk_dqblk *d = dp, empty;

	dquot->dq_id = le32toh(d->dqb_id);
	m->dqb_ihardlimit = le32toh(d->dqb_ihardlimit);
	m->dqb_isoftlimit = le32toh(d->dqb_isoftlimit);
	m->dqb_bhardlimit = le32toh(d->dqb_bhardlimit);
	m->dqb_bsoftlimit = le32toh(d->dqb_bsoftlimit);
	m->dqb_curinodes = le32toh(d->dqb_curinodes);
	m->dqb_curspace = le64toh(d->dqb_curspace);
	m->dqb_itime = le64toh(d->dqb_itime);
	m->dqb_btime = le64toh(d->dqb_btime);

	memset(&empty, 0, sizeof(struct v2r0_disk_dqblk));
	empty.dqb_itime = htole64(1);
	if (!memcmp(&empty, dp, sizeof(struct v2r0_disk_dqblk)))
		m->dqb_itime = 0;
}

/*
 *	Copy dquot from memory to disk
 */
static void v2r0_mem2diskdqblk(void *dp, struct dquot *dquot)
{
	struct util_dqblk *m = &dquot->dq_dqb;
	struct v2r0_disk_dqblk *d = dp;
	struct qtree_mem_dqinfo *info = &dquot->dq_h->qh_info.u.v2_mdqi.dqi_qtree;

	d->dqb_ihardlimit = htole32(m->dqb_ihardlimit);
	d->dqb_isoftlimit = htole32(m->dqb_isoftlimit);
	d->dqb_bhardlimit = htole32(m->dqb_bhardlimit);
	d->dqb_bsoftlimit = htole32(m->dqb_bsoftlimit);
	d->dqb_curinodes = htole32(m->dqb_curinodes);
	d->dqb_curspace = htole64(m->dqb_curspace);
	d->dqb_itime = htole64(m->dqb_itime);
	d->dqb_btime = htole64(m->dqb_btime);
	d->dqb_id = htole32(dquot->dq_id);
	if (qtree_entry_unused(info, dp))
		d->dqb_itime = htole64(1);
}

static int v2r0_is_id(void *dp, struct dquot *dquot)
{
	struct v2r0_disk_dqblk *d = dp;
	struct qtree_mem_dqinfo *info = &dquot->dq_h->qh_info.u.v2_mdqi.dqi_qtree;

	if (qtree_entry_unused(info, dp))
		return 0;
	return le32toh(d->dqb_id) == dquot->dq_id;
}

/*
 *	Copy dquot from disk to memory
 */
static void v2r1_disk2memdqblk(struct dquot *dquot, void *dp)
{
	struct util_dqblk *m = &dquot->dq_dqb;
	struct v2r1_disk_dqblk *d = dp, empty;

	dquot->dq_id = le32toh(d->dqb_id);
	m->dqb_ihardlimit = le64toh(d->dqb_ihardlimit);
	m->dqb_isoftlimit = le64toh(d->dqb_isoftlimit);
	m->dqb_bhardlimit = le64toh(d->dqb_bhardlimit);
	m->dqb_bsoftlimit = le64toh(d->dqb_bsoftlimit);
	m->dqb_curinodes = le64toh(d->dqb_curinodes);
	m->dqb_curspace = le64toh(d->dqb_curspace);
	m->dqb_itime = le64toh(d->dqb_itime);
	m->dqb_btime = le64toh(d->dqb_btime);

	memset(&empty, 0, sizeof(struct v2r1_disk_dqblk));
	empty.dqb_itime = htole64(1);
	if (!memcmp(&empty, dp, sizeof(struct v2r1_disk_dqblk)))
		m->dqb_itime = 0;
}

/*
 *	Copy dquot from memory to disk
 */
static void v2r1_mem2diskdqblk(void *dp, struct dquot *dquot)
{
	struct util_dqblk *m = &dquot->dq_dqb;
	struct v2r1_disk_dqblk *d = dp;

	d->dqb_ihardlimit = htole64(m->dqb_ihardlimit);
	d->dqb_isoftlimit = htole64(m->dqb_isoftlimit);
	d->dqb_bhardlimit = htole64(m->dqb_bhardlimit);
	d->dqb_bsoftlimit = htole64(m->dqb_bsoftlimit);
	d->dqb_curinodes = htole64(m->dqb_curinodes);
	d->dqb_curspace = htole64(m->dqb_curspace);
	d->dqb_itime = htole64(m->dqb_itime);
	d->dqb_btime = htole64(m->dqb_btime);
	d->dqb_id = htole32(dquot->dq_id);
	d->dqb_pad = 0;     /* Initialize because of qtree_entry_unused() scan */
	if (qtree_entry_unused(&dquot->dq_h->qh_info.u.v2_mdqi.dqi_qtree, dp))
		d->dqb_itime = htole64(1);
}

static int v2r1_is_id(void *dp, struct dquot *dquot)
{
	struct v2r1_disk_dqblk *d = dp;
	struct qtree_mem_dqinfo *info = &dquot->dq_h->qh_info.u.v2_mdqi.dqi_qtree;

	if (qtree_entry_unused(info, dp))
		return 0;
	return le32toh(d->dqb_id) == dquot->dq_id;
}

static struct qtree_fmt_operations v2r0_fmt_ops = {
	.mem2disk_dqblk = v2r0_mem2diskdqblk,
	.disk2mem_dqblk = v2r0_disk2memdqblk,
	.is_id = v2r0_is_id,
};

static struct qtree_fmt_operations v2r1_fmt_ops = {
	.mem2disk_dqblk = v2r1_mem2diskdqblk,
	.disk2mem_dqblk = v2r1_disk2memdqblk,
	.is_id = v2r1_is_id,
};

/*
 *	Copy dqinfo from disk to memory
 */
static inline void v2_disk2memdqinfo(struct util_dqinfo *m, struct v2_disk_dqinfo *d)
{
	m->dqi_bgrace = le32toh(d->dqi_bgrace);
	m->dqi_igrace = le32toh(d->dqi_igrace);
	m->u.v2_mdqi.dqi_flags = le32toh(d->dqi_flags) & V2_DQF_MASK;
	m->u.v2_mdqi.dqi_qtree.dqi_blocks = le32toh(d->dqi_blocks);
	m->u.v2_mdqi.dqi_qtree.dqi_free_blk = le32toh(d->dqi_free_blk);
	m->u.v2_mdqi.dqi_qtree.dqi_free_entry = le32toh(d->dqi_free_entry);
}

/*
 *	Copy dqinfo from memory to disk
 */
static inline void v2_mem2diskdqinfo(struct v2_disk_dqinfo *d, struct util_dqinfo *m)
{
	d->dqi_bgrace = htole32(m->dqi_bgrace);
	d->dqi_igrace = htole32(m->dqi_igrace);
	d->dqi_flags = htole32(m->u.v2_mdqi.dqi_flags & V2_DQF_MASK);
	d->dqi_blocks = htole32(m->u.v2_mdqi.dqi_qtree.dqi_blocks);
	d->dqi_free_blk = htole32(m->u.v2_mdqi.dqi_qtree.dqi_free_blk);
	d->dqi_free_entry = htole32(m->u.v2_mdqi.dqi_qtree.dqi_free_entry);
}

/* Convert kernel quotablock format to utility one */
static inline void v2_kern2utildqblk(struct util_dqblk *u, struct v2_kern_dqblk *k)
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
static inline void v2_util2kerndqblk(struct v2_kern_dqblk *k, struct util_dqblk *u)
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

static int v2_read_header(int fd, struct v2_disk_dqheader *h)
{
	lseek(fd, 0, SEEK_SET);
	if (read(fd, h, sizeof(struct v2_disk_dqheader)) != sizeof(struct v2_disk_dqheader))
		return 0;
	return 1;
}

/*
 *	Check whether given quota file is in our format
 */
static int v2_check_file(int fd, int type, int fmt)
{
	struct v2_disk_dqheader h;
	int file_magics[] = INITQMAGICS;
	int known_versions[] = INIT_V2_VERSIONS;
	int version;

	if (!v2_read_header(fd, &h))
		return 0;
	if (fmt == QF_VFSV0)
		version = 0;
	else if (fmt == QF_VFSV1)
		version = 1;
	else
		return 0;

	if (le32toh(h.dqh_magic) != file_magics[type]) {
		if (be32toh(h.dqh_magic) == file_magics[type])
			die(3, _("Your quota file is stored in wrong endianity. Please use convertquota(8) to convert it.\n"));
		return 0;
	}
	if (le32toh(h.dqh_version) > known_versions[type])
		return 0;
	if (version != le32toh(h.dqh_version))
		return 0;
	return 1;
}

/*
 *	Open quotafile
 */
static int v2_init_io(struct quota_handle *h)
{
	if (QIO_ENABLED(h)) {
		if (kernel_iface == IFACE_GENERIC) {
			if (vfs_get_info(h) < 0)
				return -1;
		}
		else {
			struct v2_kern_dqinfo kdqinfo;

			if (quotactl(QCMD(Q_V2_GETINFO, h->qh_type), h->qh_quotadev, 0, (void *)&kdqinfo) < 0) {
				/* Temporary check just before fix gets to kernel */
				if (errno == EPERM)	/* Don't have permission to get information? */
					return 0;
				return -1;
			}
			h->qh_info.dqi_bgrace = kdqinfo.dqi_bgrace;
			h->qh_info.dqi_igrace = kdqinfo.dqi_igrace;
			h->qh_info.u.v2_mdqi.dqi_flags = kdqinfo.dqi_flags;
			h->qh_info.u.v2_mdqi.dqi_qtree.dqi_blocks = kdqinfo.dqi_blocks;
			h->qh_info.u.v2_mdqi.dqi_qtree.dqi_free_blk = kdqinfo.dqi_free_blk;
			h->qh_info.u.v2_mdqi.dqi_qtree.dqi_free_entry = kdqinfo.dqi_free_entry;
		}
	}
	if (h->qh_fd != -1) {
		struct v2_disk_dqinfo ddqinfo;
		struct v2_disk_dqheader header;

		if (!v2_read_header(h->qh_fd, &header))
			return -1;

		lseek(h->qh_fd, V2_DQINFOOFF, SEEK_SET);
		if (read(h->qh_fd, &ddqinfo, sizeof(ddqinfo)) != sizeof(ddqinfo))
			return -1;
		/* Convert everything */
		if (!QIO_ENABLED(h))
			v2_disk2memdqinfo(&h->qh_info, &ddqinfo);
		else	/* We need just the number of blocks */
			h->qh_info.u.v2_mdqi.dqi_qtree.dqi_blocks = le32toh(ddqinfo.dqi_blocks);

		if (le32toh(header.dqh_version) == 0) {
			h->qh_info.u.v2_mdqi.dqi_qtree.dqi_entry_size = sizeof(struct v2r0_disk_dqblk);
			h->qh_info.u.v2_mdqi.dqi_qtree.dqi_ops = &v2r0_fmt_ops;
			h->qh_info.dqi_max_b_limit = ~(uint32_t)0;
			h->qh_info.dqi_max_i_limit = ~(uint32_t)0;
			h->qh_info.dqi_max_b_usage = ~(uint64_t)0;
			h->qh_info.dqi_max_i_usage = ~(uint32_t)0;
		} else {
			h->qh_info.u.v2_mdqi.dqi_qtree.dqi_entry_size = sizeof(struct v2r1_disk_dqblk);
			h->qh_info.u.v2_mdqi.dqi_qtree.dqi_ops = &v2r1_fmt_ops;
			h->qh_info.dqi_max_b_limit = ~(uint64_t)0;
			h->qh_info.dqi_max_i_limit = ~(uint64_t)0;
			h->qh_info.dqi_max_b_usage = ~(uint64_t)0;
			h->qh_info.dqi_max_i_usage = ~(uint64_t)0;
		}
	} else {
		/* We don't have the file open -> we don't need quota tree operations */
		h->qh_info.u.v2_mdqi.dqi_qtree.dqi_ops = NULL;
	}
	return 0;
}

/*
 *	Initialize new quotafile
 */
static int v2_new_io(struct quota_handle *h)
{
	int file_magics[] = INITQMAGICS;
	struct v2_disk_dqheader ddqheader;
	struct v2_disk_dqinfo ddqinfo;
	int version;

	if (h->qh_fmt == QF_VFSV0)
		version = 0;
	else if (h->qh_fmt == QF_VFSV1)
		version = 1;
	else
		return -1;

	/* Write basic quota header */
	ddqheader.dqh_magic = htole32(file_magics[h->qh_type]);
	ddqheader.dqh_version = htole32(version);
	lseek(h->qh_fd, 0, SEEK_SET);
	if (write(h->qh_fd, &ddqheader, sizeof(ddqheader)) != sizeof(ddqheader))
		return -1;
	/* Write information about quotafile */
	h->qh_info.dqi_bgrace = MAX_DQ_TIME;
	h->qh_info.dqi_igrace = MAX_IQ_TIME;
	h->qh_info.u.v2_mdqi.dqi_flags = 0;
	h->qh_info.u.v2_mdqi.dqi_qtree.dqi_blocks = QT_TREEOFF + 1;
	h->qh_info.u.v2_mdqi.dqi_qtree.dqi_free_blk = 0;
	h->qh_info.u.v2_mdqi.dqi_qtree.dqi_free_entry = 0;
	if (version == 0) {
		h->qh_info.u.v2_mdqi.dqi_qtree.dqi_entry_size = sizeof(struct v2r0_disk_dqblk);
		h->qh_info.u.v2_mdqi.dqi_qtree.dqi_ops = &v2r0_fmt_ops;
		h->qh_info.dqi_max_b_limit = ~(uint32_t)0;
		h->qh_info.dqi_max_i_limit = ~(uint32_t)0;
		h->qh_info.dqi_max_b_usage = ~(uint64_t)0;
		h->qh_info.dqi_max_i_usage = ~(uint32_t)0;
	} else if (version == 1) {
		h->qh_info.u.v2_mdqi.dqi_qtree.dqi_entry_size = sizeof(struct v2r1_disk_dqblk);
		h->qh_info.u.v2_mdqi.dqi_qtree.dqi_ops = &v2r1_fmt_ops;
		h->qh_info.dqi_max_b_limit = ~(uint64_t)0;
		h->qh_info.dqi_max_i_limit = ~(uint64_t)0;
		h->qh_info.dqi_max_b_usage = ~(uint64_t)0;
		h->qh_info.dqi_max_i_usage = ~(uint64_t)0;
	}
	v2_mem2diskdqinfo(&ddqinfo, &h->qh_info);
	lseek(h->qh_fd, V2_DQINFOOFF, SEEK_SET);
	if (write(h->qh_fd, &ddqinfo, sizeof(ddqinfo)) != sizeof(ddqinfo))
		return -1;
	return 0;
}

/*
 *	Write information (grace times to file)
 */
static int v2_write_info(struct quota_handle *h)
{
	if (QIO_RO(h)) {
		errstr(_("Trying to write info to readonly quotafile on %s\n"), h->qh_quotadev);
		errno = EPERM;
		return -1;
	}
	if (QIO_ENABLED(h)) {
		if (kernel_iface == IFACE_GENERIC) {
			if (vfs_set_info(h, IIF_BGRACE | IIF_IGRACE))
				return -1;
		}
		else {
			struct v2_kern_dqinfo kdqinfo;

			kdqinfo.dqi_bgrace = h->qh_info.dqi_bgrace;
			kdqinfo.dqi_igrace = h->qh_info.dqi_igrace;
			kdqinfo.dqi_flags = h->qh_info.u.v2_mdqi.dqi_flags;
			kdqinfo.dqi_blocks = h->qh_info.u.v2_mdqi.dqi_qtree.dqi_blocks;
			kdqinfo.dqi_free_blk = h->qh_info.u.v2_mdqi.dqi_qtree.dqi_free_blk;
			kdqinfo.dqi_free_entry = h->qh_info.u.v2_mdqi.dqi_qtree.dqi_free_entry;
			if (quotactl(QCMD(Q_V2_SETGRACE, h->qh_type), h->qh_quotadev, 0, (void *)&kdqinfo) < 0 ||
			    quotactl(QCMD(Q_V2_SETFLAGS, h->qh_type), h->qh_quotadev, 0, (void *)&kdqinfo) < 0)
					return -1;
		}
	}
	else {
		struct v2_disk_dqinfo ddqinfo;

		v2_mem2diskdqinfo(&ddqinfo, &h->qh_info);
		lseek(h->qh_fd, V2_DQINFOOFF, SEEK_SET);
		if (write(h->qh_fd, &ddqinfo, sizeof(ddqinfo)) != sizeof(ddqinfo))
			return -1;
	}
	return 0;
}

/*
 *  Read dquot (either from disk or from kernel)
 *  User can use errno to detect errstr when NULL is returned
 */
static struct dquot *v2_read_dquot(struct quota_handle *h, qid_t id)
{
	if (QIO_ENABLED(h)) {
		struct dquot *dquot = get_empty_dquot();

		dquot->dq_id = id;
		dquot->dq_h = h;
		dquot->dq_dqb.u.v2_mdqb.dqb_off = 0;
		memset(&dquot->dq_dqb, 0, sizeof(struct util_dqblk));
		if (kernel_iface == IFACE_GENERIC) {
			if (vfs_get_dquot(dquot) < 0) {
				free(dquot);
				return NULL;
			}
		}
		else {
			struct v2_kern_dqblk kdqblk;

			if (quotactl(QCMD(Q_V2_GETQUOTA, h->qh_type), h->qh_quotadev, id, (void *)&kdqblk) < 0) {
				free(dquot);
				return NULL;
			}
			v2_kern2utildqblk(&dquot->dq_dqb, &kdqblk);
		}
		return dquot;
	}
	return qtree_read_dquot(h, id);
}

/* 
 *  Commit changes of dquot to disk - it might also mean deleting it when quota became fake one and user has no blocks...
 *  User can process use 'errno' to detect errstr
 */
static int v2_commit_dquot(struct dquot *dquot, int flags)
{
	struct util_dqblk *b = &dquot->dq_dqb;

	if (QIO_RO(dquot->dq_h)) {
		errstr(_("Trying to write quota to readonly quotafile on %s\n"), dquot->dq_h->qh_quotadev);
		errno = EPERM;
		return -1;
	}
	if (QIO_ENABLED(dquot->dq_h)) {
		if (kernel_iface == IFACE_GENERIC) {
			if (vfs_set_dquot(dquot, flags) < 0)
				return -1;
		}
		else {
			struct v2_kern_dqblk kdqblk;
			int cmd;

			if (flags == COMMIT_USAGE)
				cmd = Q_V2_SETUSE;
			else if (flags == COMMIT_LIMITS)
				cmd = Q_V2_SETQLIM;
			else if (flags & COMMIT_TIMES) {
				errno = EINVAL;
				return -1;
			}
			else
				cmd = Q_V2_SETQUOTA;
			v2_util2kerndqblk(&kdqblk, &dquot->dq_dqb);
			if (quotactl(QCMD(cmd, dquot->dq_h->qh_type), dquot->dq_h->qh_quotadev,
			     dquot->dq_id, (void *)&kdqblk) < 0)
				return -1;
		}
		return 0;
	}
	if (!b->dqb_curspace && !b->dqb_curinodes && !b->dqb_bsoftlimit && !b->dqb_isoftlimit
	    && !b->dqb_bhardlimit && !b->dqb_ihardlimit)
		qtree_delete_dquot(dquot);
	else {
		if (check_dquot_range(dquot) < 0) {
			errno = ERANGE;
			return -1;
		}
		qtree_write_dquot(dquot);
	}
	return 0;
}

static int v2_scan_dquots(struct quota_handle *h, int (*process_dquot) (struct dquot *, char *))
{
	return qtree_scan_dquots(h, process_dquot);
}

/* Report information about quotafile */
static int v2_report(struct quota_handle *h, int verbose)
{
	if (verbose) {
		struct v2_mem_dqinfo *info = &h->qh_info.u.v2_mdqi;

		printf(_("Statistics:\nTotal blocks: %u\nData blocks: %u\nEntries: %u\nUsed average: %f\n"),
			 info->dqi_qtree.dqi_blocks, info->dqi_data_blocks, info->dqi_used_entries,
			 ((float)info->dqi_used_entries) / info->dqi_data_blocks);
	}
	return 0;
}
