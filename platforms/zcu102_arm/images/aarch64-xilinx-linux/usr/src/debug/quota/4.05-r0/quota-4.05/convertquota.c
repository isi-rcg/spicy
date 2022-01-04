/*
 *
 *	Utility for converting quota file from old to new format
 *
 *	Sponsored by SuSE CR
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <getopt.h>

#include <endian.h>

#include "pot.h"
#include "common.h"
#include "quotaio.h"
#include "quotasys.h"
#include "quota.h"
#include "bylabel.h"
#include "quotaio_v2.h"
#include "dqblk_v2.h"

#define ACT_FORMAT 1		/* Convert format from old to new */
#define ACT_ENDIAN 2		/* Convert endianity */

static char *mntpoint;
char *progname;
static int ucv, gcv;
static struct quota_handle *qn;	/* Handle of new file */
static int action;			/* Action to be performed */
static int infmt, outfmt;

static void usage(void)
{
	errstr(_("Utility for converting quota files.\nUsage:\n\t%s [options] mountpoint\n\n\
-u, --user                          convert user quota file\n\
-g, --group                         convert group quota file\n\
-e, --convert-endian                convert quota file to correct endianity\n\
-f, --convert-format oldfmt,newfmt  convert from old to VFSv0 quota format\n\
-h, --help                          show this help text and exit\n\
-V, --version                       output version information and exit\n\n"), progname);
	errstr(_("Bugs to %s\n"), PACKAGE_BUGREPORT);
	exit(1);
}

static inline unsigned int min(unsigned a, unsigned b)
{
	if (a < b)
		return a;
	return b;
}

#define MAX_FMTNAME_LEN 32

static void parse_options(int argcnt, char **argstr)
{
	int ret;
	struct option long_opts[] = {
		{ "help", 0, NULL, 'h'},
		{ "version", 0, NULL, 'V'},
		{ "user", 0, NULL, 'u'},
		{ "group", 0, NULL, 'g'},
		{ "convert-endian", 0, NULL, 'e'},
		{ "convert-format", 1, NULL, 'f'},
		{ NULL, 0, NULL, 0}
	};
	char *comma;
	char fmtbuf[MAX_FMTNAME_LEN];

	while ((ret = getopt_long(argcnt, argstr, "Vugef:h", long_opts, NULL)) != -1) {
		switch (ret) {
			case '?':
			case 'h':
				usage();
			case 'V':
				version();
				exit(0);
			case 'u':
				ucv = 1;
				break;
			case 'g':
				gcv = 1;
				break;
			case 'e':
				action = ACT_ENDIAN;
				break;
			case 'f':
				action = ACT_FORMAT;
				comma = strchr(optarg, ',');
				if (!comma) {
					errstr(_("You have to specify source and target format of conversion.\n"));
					usage();
				}
				sstrncpy(fmtbuf, optarg, min(comma - optarg + 1, MAX_FMTNAME_LEN));
				infmt = name2fmt(fmtbuf);
				if (infmt == QF_ERROR)
					usage();
				outfmt = name2fmt(comma + 1);
				if (outfmt == QF_ERROR)
					usage();
				break;
		}
	}

	if (optind + 1 != argcnt) {
		errstr(_("Bad number of arguments.\n"));
		usage();
	}

	if (!(ucv | gcv))
		ucv = 1;
	if (!action) {
		errstr(_("You have to specify action to perform.\n"));
		usage();
	}

	mntpoint = argstr[optind];
}

/*
 *	Implementation of endian conversion
 */

typedef char *dqbuf_t;

#define set_bit(bmp, ind) ((bmp)[(ind) >> 3] |= (1 << ((ind) & 7)))
#define get_bit(bmp, ind) ((bmp)[(ind) >> 3] & (1 << ((ind) & 7)))

#define getdqbuf() smalloc(QT_BLKSIZE)
#define freedqbuf(buf) free(buf)

static inline void endian_disk2memdqblk(struct util_dqblk *m, struct v2r0_disk_dqblk *d)
{
	m->dqb_ihardlimit = be32toh(d->dqb_ihardlimit);
	m->dqb_isoftlimit = be32toh(d->dqb_isoftlimit);
	m->dqb_bhardlimit = be32toh(d->dqb_bhardlimit);
	m->dqb_bsoftlimit = be32toh(d->dqb_bsoftlimit);
	m->dqb_curinodes = be32toh(d->dqb_curinodes);
	m->dqb_curspace = be64toh(d->dqb_curspace);
	m->dqb_itime = be64toh(d->dqb_itime);
	m->dqb_btime = be64toh(d->dqb_btime);
}

/* Is given dquot empty? */
static int endian_empty_dquot(struct v2r0_disk_dqblk *d)
{
	static struct v2r0_disk_dqblk fakedquot;

	return !memcmp(d, &fakedquot, sizeof(fakedquot));
}

/* Read given block */
static void read_blk(int fd, uint blk, dqbuf_t buf)
{
	int err;

	lseek(fd, blk << QT_BLKSIZE_BITS, SEEK_SET);
	err = read(fd, buf, QT_BLKSIZE);
	if (err < 0)
		die(2, _("Cannot read block %u: %s\n"), blk, strerror(errno));
	else if (err != QT_BLKSIZE)
		memset(buf + err, 0, QT_BLKSIZE - err);
}

static void endian_report_block(int fd, uint blk, char *bitmap)
{
	dqbuf_t buf = getdqbuf();
	struct qt_disk_dqdbheader *dh;
	struct v2r0_disk_dqblk *ddata;
	struct dquot dquot;
	struct qtree_mem_dqinfo *info = &qn->qh_info.u.v2_mdqi.dqi_qtree;
	int i;

	set_bit(bitmap, blk);
	read_blk(fd, blk, buf);
	dh = (struct qt_disk_dqdbheader *)buf;
	ddata = (struct v2r0_disk_dqblk *)(dh + 1);
	for (i = 0; i < qtree_dqstr_in_blk(info); i++)
		if (!endian_empty_dquot(ddata + i)) {
			memset(&dquot, 0, sizeof(dquot));
			dquot.dq_h = qn;
			endian_disk2memdqblk(&dquot.dq_dqb, ddata + i);
			dquot.dq_id = be32toh(ddata[i].dqb_id);
			if (qn->qh_ops->commit_dquot(&dquot, COMMIT_ALL) < 0)
				errstr(_("Cannot commit dquot for id %u: %s\n"),
					(uint)dquot.dq_id, strerror(errno));
		}
	freedqbuf(buf);
}

static void endian_report_tree(int fd, uint blk, int depth, char *bitmap)
{
	int i;
	dqbuf_t buf = getdqbuf();
	u_int32_t *ref = (u_int32_t *) buf;

	read_blk(fd, blk, buf);
	if (depth == QT_TREEDEPTH - 1) {
		for (i = 0; i < QT_BLKSIZE >> 2; i++) {
			blk = be32toh(ref[i]);
			if (blk && !get_bit(bitmap, blk))
				endian_report_block(fd, blk, bitmap);
		}
	}
	else {
		for (i = 0; i < QT_BLKSIZE >> 2; i++)
			if ((blk = be32toh(ref[i])))
				endian_report_tree(fd, blk, depth + 1, bitmap);
	}
	freedqbuf(buf);
}

static int endian_scan_structures(int fd, int type)
{
	char *bitmap;
	loff_t blocks = (lseek(fd, 0, SEEK_END) + QT_BLKSIZE - 1) >> QT_BLKSIZE_BITS;

	bitmap = smalloc((blocks + 7) >> 3);
	memset(bitmap, 0, (blocks + 7) >> 3);
	endian_report_tree(fd, QT_TREEOFF, 0, bitmap);
	free(bitmap);
	return 0;
}

static int endian_check_header(int fd, int type)
{
	struct v2_disk_dqheader head;
	u_int32_t file_magics[] = INITQMAGICS;
	u_int32_t known_versions[] = INIT_V2_VERSIONS;

	lseek(fd, 0, SEEK_SET);
	if (read(fd, &head, sizeof(head)) != sizeof(head)) {
		errstr(_("Cannot read header of old quotafile.\n"));
		return -1;
	}
	if (be32toh(head.dqh_magic) != file_magics[type] || be32toh(head.dqh_version) > known_versions[type]) {
		errstr(_("Bad file magic or version (probably not quotafile with bad endianity).\n"));
		return -1;
	}
	return 0;
}

static int endian_load_info(int fd, int type)
{
	struct v2_disk_dqinfo dinfo;

	if (read(fd, &dinfo, sizeof(dinfo)) != sizeof(dinfo)) {
		errstr(_("Cannot read information about old quotafile.\n"));
		return -1;
	}
	qn->qh_info.u.v2_mdqi.dqi_flags = be32toh(dinfo.dqi_flags);
	qn->qh_info.dqi_bgrace = be32toh(dinfo.dqi_bgrace);
	qn->qh_info.dqi_igrace = be32toh(dinfo.dqi_igrace);
	return 0;
}

/*
 *	End of endian conversion
 */

static int convert_dquot(struct dquot *dquot, char *name)
{
	struct dquot newdquot;

	memset(&newdquot, 0, sizeof(newdquot));
	newdquot.dq_id = dquot->dq_id;
	newdquot.dq_h = qn;
	newdquot.dq_dqb.dqb_ihardlimit = dquot->dq_dqb.dqb_ihardlimit;
	newdquot.dq_dqb.dqb_isoftlimit = dquot->dq_dqb.dqb_isoftlimit;
	newdquot.dq_dqb.dqb_curinodes = dquot->dq_dqb.dqb_curinodes;
	newdquot.dq_dqb.dqb_bhardlimit = dquot->dq_dqb.dqb_bhardlimit;
	newdquot.dq_dqb.dqb_bsoftlimit = dquot->dq_dqb.dqb_bsoftlimit;
	newdquot.dq_dqb.dqb_curspace = dquot->dq_dqb.dqb_curspace;
	newdquot.dq_dqb.dqb_btime = dquot->dq_dqb.dqb_btime;
	newdquot.dq_dqb.dqb_itime = dquot->dq_dqb.dqb_itime;
	if (qn->qh_ops->commit_dquot(&newdquot, COMMIT_ALL) < 0) {
		errstr(_("Cannot commit dquot for id %u: %s\n"),
			(uint)dquot->dq_id, strerror(errno));
		return -1;
	}
	return 0;
}

static int rename_file(int type, int fmt, struct mount_entry *mnt)
{
	char *qfname, namebuf[PATH_MAX];
	int ret = 0;

	if (get_qf_name(mnt, type, fmt, 0, &qfname) < 0) {
		errstr(_("Cannot get name of new quotafile.\n"));
		return -1;
	}
	strcpy(namebuf, qfname);
	sstrncat(namebuf, ".new", sizeof(namebuf));
	if (rename(namebuf, qfname) < 0) {
		errstr(_("Cannot rename new quotafile %s to name %s: %s\n"),
			namebuf, qfname, strerror(errno));
		ret = -1;
	}
	free(qfname);
	return ret;
}

static int convert_format(int type, struct mount_entry *mnt)
{
	struct quota_handle *qo;
	int ret = 0;
	
	if (!(qo = init_io(mnt, type, infmt, IOI_INITSCAN))) {
		errstr(_("Cannot open old format file for %ss on %s\n"),
			_(type2name(type)), mnt->me_dir);
		return -1;
	}
	if (!(qn = new_io(mnt, type, outfmt))) {
		errstr(_("Cannot create file for %ss for new format on %s: %s\n"),
			_(type2name(type)), mnt->me_dir, strerror(errno));
		end_io(qo);
		return -1;
	}
	if (qo->qh_ops->scan_dquots(qo, convert_dquot) >= 0)	/* Conversion succeeded? */
		ret = rename_file(type, outfmt, mnt);
	else
		ret = -1;
	end_io(qo);
	end_io(qn);
	return ret;
}

static int convert_endian(int type, struct mount_entry *mnt)
{
	int ret = 0;
	int ofd;
	char *qfname;

	if (get_qf_name(mnt, type, QF_VFSV0, NF_EXIST, &qfname) < 0)
		return -1;
	if ((ofd = open(qfname, O_RDONLY)) < 0) {
		errstr(_("Cannot open old quota file on %s: %s\n"), mnt->me_dir, strerror(errno));
		free(qfname);
		return -1;
	}
	free(qfname);
	if (endian_check_header(ofd, type) < 0) {
		close(ofd);
		return -1;
	}
	if (!(qn = new_io(mnt, type, QF_VFSV0))) {
		errstr(_("Cannot create file for %ss for new format on %s: %s\n"),
			type2name(type), mnt->me_dir, strerror(errno));
		close(ofd);
		return -1;
	}
	if (endian_load_info(ofd, type) < 0) {
		end_io(qn);
		close(ofd);
		return -1;
	}
	ret = endian_scan_structures(ofd, type);
	end_io(qn);
	close(ofd);
	if (ret < 0)
		return ret;
	
	return rename_file(type, QF_VFSV0, mnt);
}

static int convert_file(int type, struct mount_entry *mnt)
{
	switch (action) {
		case ACT_FORMAT:
			return convert_format(type, mnt);
		case ACT_ENDIAN:
			return convert_endian(type, mnt);
	}
	errstr(_("Unknown action should be performed.\n"));
	return -1;
}

int main(int argc, char **argv)
{
	struct mount_entry *mnt;
	int ret = 0;
	
	gettexton();
	progname = basename(argv[0]);

	parse_options(argc, argv);
	init_kernel_interface();
	if (init_mounts_scan(1, &mntpoint, 0) < 0)
		return 1;
	if (!(mnt = get_next_mount())) {
		end_mounts_scan();
		return 1;
	}
	if (ucv)
		ret |= convert_file(USRQUOTA, mnt);
	if (gcv)
		ret |= convert_file(GRPQUOTA, mnt);
	end_mounts_scan();

	if (ret)
		return 1;
	return 0;
}
