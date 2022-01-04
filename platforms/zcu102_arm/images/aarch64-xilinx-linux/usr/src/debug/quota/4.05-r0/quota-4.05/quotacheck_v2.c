/*
 *
 *	Checking routines for new VFS quota format
 *
 */

#include "config.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <endian.h>

#include "pot.h"
#include "common.h"
#include "quota.h"
#include "quotaio.h"
#include "quotaio_v2.h"
#include "quotacheck.h"
#include "quota_tree.h"

#define getdqbuf() smalloc(QT_BLKSIZE)
#define freedqbuf(buf) free(buf)

#define SET_BLK(blk) (blkbmp[(blk) >> 3] |= 1 << ((blk) & 7))
#define GET_BLK(blk) (blkbmp[(blk) >> 3] & (1 << ((blk) & 7)))

typedef char *dqbuf_t;

static const int magics[MAXQUOTAS] = INITQMAGICS;	/* Magics we should look for */
static const int known_versions[MAXQUOTAS] = INIT_V2_VERSIONS;	/* Versions we accept */
static char *blkbmp;		/* Bitmap of checked blocks */
static int detected_versions[MAXQUOTAS];

static int check_blkref(uint blk, uint blocks)
{
	if (blk >= blocks)
		return -1;
	if (blk && blk < QT_TREEOFF)
		return -1;
	return 0;
}

/* Load and check basic info about quotas */
static int check_info(char *filename, int fd, int type)
{
	struct v2_disk_dqinfo dinfo;
	uint blocks, dflags, freeblk, freeent;
	off_t filesize;
	int err;

	debug(FL_DEBUG, _("Checking quotafile info...\n"));
	lseek(fd, V2_DQINFOOFF, SEEK_SET);
	err = read(fd, &dinfo, sizeof(struct v2_disk_dqinfo));

	if (err < 0) {
		errstr(_("Cannot read info from quota file %s: %s\n"),
			filename, strerror(errno));
		return -1;
	}
	if (err != sizeof(struct v2_disk_dqinfo)) {
		errstr(_("WARNING - Quota file %s was probably truncated. Cannot save quota settings...\n"),
			filename);
		return -1;
	}

	blocks = le32toh(dinfo.dqi_blocks);
	freeblk = le32toh(dinfo.dqi_free_blk);
	freeent = le32toh(dinfo.dqi_free_entry);
	dflags = le32toh(dinfo.dqi_flags);
	filesize = lseek(fd, 0, SEEK_END);
	if (check_blkref(freeblk, blocks) < 0 || dflags & ~V2_DQF_MASK ||
	    check_blkref(freeent, blocks) < 0 || (filesize + QT_BLKSIZE - 1) >> QT_BLKSIZE_BITS != blocks) {
		errstr(_("WARNING - Quota file info was corrupted.\n"));
		debug(FL_DEBUG, _("Size of file: %lu\nBlocks: %u Free block: %u Block with free entry: %u Flags: %x\n"),
		      (unsigned long)filesize, blocks, freeblk, freeent, dflags);
		old_info[type].dqi_bgrace = MAX_DQ_TIME;
		old_info[type].dqi_igrace = MAX_IQ_TIME;
		old_info[type].u.v2_mdqi.dqi_qtree.dqi_blocks =
			(filesize + QT_BLKSIZE - 1) >> QT_BLKSIZE_BITS;
		old_info[type].u.v2_mdqi.dqi_flags = 0;
		printf(_("Setting grace times and other flags to default values.\nAssuming number of blocks is %u.\n"),
		       old_info[type].u.v2_mdqi.dqi_qtree.dqi_blocks);
	}
	else {
		old_info[type].dqi_bgrace = le32toh(dinfo.dqi_bgrace);
		old_info[type].dqi_igrace = le32toh(dinfo.dqi_igrace);
		old_info[type].u.v2_mdqi.dqi_qtree.dqi_blocks = blocks;
		old_info[type].u.v2_mdqi.dqi_flags = dflags;
	}
	if (detected_versions[type] == 0)
		old_info[type].u.v2_mdqi.dqi_qtree.dqi_entry_size = sizeof(struct v2r0_disk_dqblk);
	else if (detected_versions[type] == 1)
		old_info[type].u.v2_mdqi.dqi_qtree.dqi_entry_size = sizeof(struct v2r1_disk_dqblk);
	/* Won't be needed */
	old_info[type].u.v2_mdqi.dqi_qtree.dqi_free_blk = 0;
	old_info[type].u.v2_mdqi.dqi_qtree.dqi_free_entry = 0;

	debug(FL_DEBUG, _("File info done.\n"));
	return 0;
}

/* Print errstr message */
static void blk_corrupted(int *corrupted, uint * lblk, uint blk, char *fmtstr, ...)
{
	va_list args;

	if (!*corrupted) {
		if (!(flags & (FL_VERBOSE | FL_DEBUG)))
			errstr(_("Corrupted blocks: "));
	}
	if (flags & (FL_VERBOSE | FL_DEBUG)) {
		va_start(args, fmtstr);
		errstr(_("Block %u: "), blk);
		vfprintf(stderr, fmtstr, args);
		fputc('\n', stderr);
		va_end(args);
	}
	else if (*lblk != blk) {
		if (!*corrupted)
			fprintf(stderr, "%u", blk);
		else
			fprintf(stderr, ", %u", blk);
	}
	*corrupted = 1;
	*lblk = blk;
	fflush(stderr);
}

/* Convert dist quota format to utility one - copy just needed fields */
static void v2r0_disk2utildqblk(struct util_dqblk *u, struct v2r0_disk_dqblk *d)
{
	u->dqb_ihardlimit = le32toh(d->dqb_ihardlimit);
	u->dqb_isoftlimit = le32toh(d->dqb_isoftlimit);
	u->dqb_bhardlimit = le32toh(d->dqb_bhardlimit);
	u->dqb_bsoftlimit = le32toh(d->dqb_bsoftlimit);
	u->dqb_itime = le64toh(d->dqb_itime);
	u->dqb_btime = le64toh(d->dqb_btime);
}

/* Convert dist quota format to utility one - copy just needed fields */
static void v2r1_disk2utildqblk(struct util_dqblk *u, struct v2r1_disk_dqblk *d)
{
	u->dqb_ihardlimit = le64toh(d->dqb_ihardlimit);
	u->dqb_isoftlimit = le64toh(d->dqb_isoftlimit);
	u->dqb_bhardlimit = le64toh(d->dqb_bhardlimit);
	u->dqb_bsoftlimit = le64toh(d->dqb_bsoftlimit);
	u->dqb_itime = le64toh(d->dqb_itime);
	u->dqb_btime = le64toh(d->dqb_btime);
}

/* Put one entry info memory */
static int buffer_entry(dqbuf_t buf, uint blk, int *corrupted, uint * lblk, int cnt, int type)
{
	struct util_dqblk *fdq, mdq;
	qid_t id;
	struct dquot *cd;
	struct qtree_mem_dqinfo *info = &old_info[type].u.v2_mdqi.dqi_qtree;
	char *ddq = (char *)buf + sizeof(struct qt_disk_dqdbheader) + cnt * info->dqi_entry_size;

	if (detected_versions[type] == 0) {
		v2r0_disk2utildqblk(&mdq, (struct v2r0_disk_dqblk *)ddq);
		id = le32toh(((struct v2r0_disk_dqblk *)ddq)->dqb_id);
	} else {
		v2r1_disk2utildqblk(&mdq, (struct v2r1_disk_dqblk *)ddq);
		id = le32toh(((struct v2r1_disk_dqblk *)ddq)->dqb_id);
	}

	cd = lookup_dquot(id, type);
	if (cd != NODQUOT) {
		fdq = &cd->dq_dqb;
		if (mdq.dqb_bhardlimit != fdq->dqb_bhardlimit
		    || mdq.dqb_bsoftlimit != fdq->dqb_bsoftlimit
		    || mdq.dqb_ihardlimit != fdq->dqb_ihardlimit
		    || mdq.dqb_isoftlimit != fdq->dqb_isoftlimit) {
			blk_corrupted(corrupted, lblk, blk, _("Duplicated entries."));
			if (flags & FL_GUESSDQ) {
				if (!(flags & (FL_DEBUG | FL_VERBOSE)))
					fputc('\n', stderr);
				errstr(_("Found more structures for ID %u. Using values: BHARD: %lld BSOFT: %lld IHARD: %lld ISOFT: %lld\n"),
					(uint) id, (long long)fdq->dqb_bhardlimit, (long long)fdq->dqb_bsoftlimit,
					(long long)fdq->dqb_ihardlimit, (long long)fdq->dqb_isoftlimit);
				return 0;
			}
			else if (flags & FL_INTERACTIVE) {
				if (!(flags & (FL_DEBUG | FL_VERBOSE)))
					fputc('\n', stderr);
				errstr(_("Found more structures for ID %u. Values: BHARD: %lld/%lld BSOFT: %lld/%lld IHARD: %lld/%lld ISOFT: %lld/%lld\n"),
					(uint) id, (long long)fdq->dqb_bhardlimit, (long long)mdq.dqb_bhardlimit,
					(long long)fdq->dqb_bsoftlimit, (long long)mdq.dqb_bsoftlimit,
					(long long)fdq->dqb_ihardlimit, (long long)mdq.dqb_ihardlimit,
					(long long)fdq->dqb_isoftlimit, (long long)mdq.dqb_isoftlimit);
				if (ask_yn(_("Should I use new values?"), 0)) {
					fdq->dqb_bhardlimit = mdq.dqb_bhardlimit;
					fdq->dqb_bsoftlimit = mdq.dqb_bsoftlimit;
					fdq->dqb_ihardlimit = mdq.dqb_ihardlimit;
					fdq->dqb_isoftlimit = mdq.dqb_isoftlimit;
					fdq->dqb_btime = mdq.dqb_btime;
					fdq->dqb_itime = mdq.dqb_itime;
				}
			}
			else {
				errstr(_("ID %u has more structures. User intervention needed (use -i for interactive mode or -n for automatic answer).\n"),
					(uint) id);
				return -1;
			}
		}
		else if (mdq.dqb_itime != fdq->dqb_itime || mdq.dqb_btime != fdq->dqb_btime) {
			if (fdq->dqb_btime < mdq.dqb_btime)
				fdq->dqb_btime = mdq.dqb_btime;
			if (fdq->dqb_itime < mdq.dqb_itime)
				fdq->dqb_itime = mdq.dqb_itime;
		}
	}
	else {
		cd = add_dquot(id, type);
		fdq = &cd->dq_dqb;
		fdq->dqb_bhardlimit = mdq.dqb_bhardlimit;
		fdq->dqb_bsoftlimit = mdq.dqb_bsoftlimit;
		fdq->dqb_ihardlimit = mdq.dqb_ihardlimit;
		fdq->dqb_isoftlimit = mdq.dqb_isoftlimit;
		/* Add grace times only if there are limits... */
		if (mdq.dqb_bsoftlimit)
			fdq->dqb_btime = mdq.dqb_btime;
		if (mdq.dqb_isoftlimit)
			fdq->dqb_itime = mdq.dqb_itime;
	}
	return 0;
}

static void check_read_blk(int fd, uint blk, dqbuf_t buf)
{
	ssize_t rd;

	lseek(fd, blk << QT_BLKSIZE_BITS, SEEK_SET);
	rd = read(fd, buf, QT_BLKSIZE);
	if (rd < 0)
		die(2, _("Cannot read block %u: %s\n"), blk, strerror(errno));
	if (rd != QT_BLKSIZE) {
		debug(FL_VERBOSE | FL_DEBUG, _("Block %u is truncated.\n"), blk);
		memset(buf + rd, 0, QT_BLKSIZE - rd);
	}
}

static int check_tree_ref(uint blk, uint ref, uint blocks, int check_use, int * corrupted,
			  uint * lblk)
{
	if (check_blkref(ref, blocks) < 0) {
		blk_corrupted(corrupted, lblk, blk, _("Reference to illegal block %u"), ref);
		return -1;
	}
	if (!ref)
		return 0;
	if (!check_use || !GET_BLK(ref))
		return 0;
	blk_corrupted(corrupted, lblk, blk, _("Block %u in tree referenced twice"), ref);
	return -1;
}

/* Check block with structures */
static int check_data_blk(int fd, uint blk, int type, uint blocks, int * corrupted, uint * lblk)
{
	dqbuf_t buf = getdqbuf();
	struct qt_disk_dqdbheader *head = (struct qt_disk_dqdbheader *)buf;
	int i;
	char *dd = (char *)(head + 1);
	struct qtree_mem_dqinfo *info = &old_info[type].u.v2_mdqi.dqi_qtree;

	SET_BLK(blk);
	check_read_blk(fd, blk, buf);
	if (check_blkref(le32toh(head->dqdh_next_free), blocks) < 0)
		blk_corrupted(corrupted, lblk, blk, _("Illegal free block reference to block %u"),
			      le32toh(head->dqdh_next_free));
	if (le16toh(head->dqdh_entries) > qtree_dqstr_in_blk(info))
		blk_corrupted(corrupted, lblk, blk, _("Corrupted number of used entries (%u)"),
			      (uint) le16toh(head->dqdh_entries));
	for (i = 0; i < qtree_dqstr_in_blk(info); i++)
		if (!qtree_entry_unused(info, dd + i * info->dqi_entry_size))
			if (buffer_entry(buf, blk, corrupted, lblk, i, type) < 0) {
				freedqbuf(buf);
				return -1;
			}
	freedqbuf(buf);
	return 0;
}

/* Check one tree block */
static int check_tree_blk(int fd, uint blk, int depth, int type, uint blocks, int * corrupted,
			  uint * lblk)
{
	dqbuf_t buf = getdqbuf();
	u_int32_t *r = (u_int32_t *) buf;
	int i;

	SET_BLK(blk);
	check_read_blk(fd, blk, buf);
	for (i = 0; i < QT_BLKSIZE >> 2; i++)
		if (depth < QT_TREEDEPTH - 1) {
			if (check_tree_ref(blk, le32toh(r[i]), blocks, 1, corrupted, lblk) >= 0 &&
			    le32toh(r[i]))	/* Isn't block OK? */
				if (check_tree_blk(fd, le32toh(r[i]), depth + 1, type, blocks, corrupted, lblk) < 0) {
					freedqbuf(buf);
					return -1;
				}
		}
		else if (check_tree_ref(blk, le32toh(r[i]), blocks, 0, corrupted, lblk) >= 0 && le32toh(r[i]))
			if (!GET_BLK(le32toh(r[i])) && check_data_blk(fd, le32toh(r[i]), type, blocks, corrupted, lblk) < 0) {
				freedqbuf(buf);
				return -1;
			}
	freedqbuf(buf);
	return 0;
}

int v2_detect_version(char *filename, int fd, int type)
{
	struct v2_disk_dqheader head;
	int err;
	int ver;

	lseek(fd, 0, SEEK_SET);
	err = read(fd, &head, sizeof(head));
	if (err < 0 || err != sizeof(head))
		return -1;
	if (le32toh(head.dqh_magic) != magics[type] ||
	    le32toh(head.dqh_version) > known_versions[type]) {
		errstr(_("Quota file %s has corrupted headers. You have to specify quota format on command line.\n"),
			filename);
		return -1;
	}
	ver = le32toh(head.dqh_version);
	if (ver == 0)
		return QF_VFSV0;
	return QF_VFSV1;
}

/* Check basic header */
static int check_header(char *filename, int fd, int type, int version)
{
	int err;
	struct v2_disk_dqheader head;

	debug(FL_DEBUG, _("Checking quotafile headers...\n"));
	lseek(fd, 0, SEEK_SET);
	err = read(fd, &head, sizeof(head));
	if (err < 0)
		die(1, _("Cannot read header from quotafile %s: %s\n"), filename, strerror(errno));
	if (err != sizeof(head)) {
		errstr(_("WARNING -  Quotafile %s was probably truncated. Cannot save quota settings...\n"),
			filename);
		return -1;
	}
	if (le32toh(head.dqh_magic) != magics[type] ||
	    le32toh(head.dqh_version) > known_versions[type]) {
		errstr(_("WARNING - Quota file %s has corrupted headers\n"),
			filename);
		if (!(flags & FL_INTERACTIVE) ||
		    !ask_yn(_("Continue checking assuming format from command line?"), 0))
			return -1;
	}
	if (le32toh(head.dqh_version) != version) {
		errstr(_("Quota file format version %d does not match the one "
			 "specified on command line (%d). Quota file header "
			 "may be corrupted.\n"),
		       le32toh(head.dqh_version), version);
		if (!(flags & FL_INTERACTIVE) ||
		    !ask_yn(_("Continue checking assuming version from command line?"), 1))
			return -1;
		detected_versions[type] = version;
	} else
		detected_versions[type] = le32toh(head.dqh_version);

	debug(FL_DEBUG, _("Headers checked.\n"));
	return 0;
}

/* Load data from file to memory */
int v2_buffer_file(char *filename, int fd, int type, int fmt)
{
	uint blocks, lastblk = 0;
	int corrupted = 0, ret = 0;
	int version;

	if (fmt == QF_VFSV0)
		version = 0;
	else if (fmt == QF_VFSV1)
		version = 1;
	else
		die(3, _("Do not know how to buffer format %d\n"), fmt);

	old_info[type].dqi_bgrace = MAX_DQ_TIME;
	old_info[type].dqi_igrace = MAX_IQ_TIME;
	if (flags & FL_NEWFILE)
		return 0;
	if (check_header(filename, fd, type, version) < 0)
		return -1;
	if (check_info(filename, fd, type) < 0)
		return -1;
	debug(FL_DEBUG, _("Headers of file %s checked. Going to load data...\n"),
	      filename);
	blocks = old_info[type].u.v2_mdqi.dqi_qtree.dqi_blocks;
	blkbmp = xmalloc((blocks + 7) >> 3);
	memset(blkbmp, 0, (blocks + 7) >> 3);
	if (check_tree_ref(0, QT_TREEOFF, blocks, 1, &corrupted, &lastblk) >= 0)
		ret = check_tree_blk(fd, QT_TREEOFF, 0, type, blocks, &corrupted, &lastblk);
	else
		errstr(_("Cannot gather quota data. Tree root node corrupted.\n"));
#ifdef DEBUG_MALLOC
	free_mem += (blocks + 7) >> 3;
#endif
	free(blkbmp);
	if (corrupted) {
		if (!(flags & (FL_VERBOSE | FL_DEBUG)))
			fputc('\n', stderr);
		errstr(_("WARNING - Some data might be changed due to corruption.\n"));
	}
	else
		debug(FL_DEBUG, _("Not found any corrupted blocks. Congratulations.\n"));
	return ret;
}

/* Merge quotafile info from old and new file */
void v2_merge_info(struct util_dqinfo *new, struct util_dqinfo *old)
{
	new->u.v2_mdqi.dqi_flags = old->u.v2_mdqi.dqi_flags;
}
