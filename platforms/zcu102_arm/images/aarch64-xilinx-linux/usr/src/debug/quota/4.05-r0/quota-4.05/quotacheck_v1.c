/*
 *
 *	Checking routines for old VFS quota format
 *
 */

#include "config.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "pot.h"
#include "common.h"
#include "quotaio.h"
#include "quotaio_v1.h"
#include "quotacheck.h"

/* Load all other dquot structures */
static void load_dquots(char *filename, int fd, int type)
{
	struct v1_disk_dqblk ddqblk;
	struct util_dqblk *udq;
	struct dquot *dquot;
	int err;
	qid_t id = 0;

	lseek(fd, 0, SEEK_SET);
	while ((err = read(fd, &ddqblk, sizeof(ddqblk)))) {
		if (err < 0)
			die(1, _("Cannot read entry for id %u from quotafile %s: %s\n"), (uint) id,
			    filename, strerror(errno));
		if (err != sizeof(ddqblk)) {
			errstr(_("Entry for id %u is truncated.\n"),
				(uint) id);
			break;
		}
		if (ddqblk.dqb_bhardlimit == 0
			&& ddqblk.dqb_bsoftlimit == 0
			&& ddqblk.dqb_ihardlimit == 0
			&& ddqblk.dqb_isoftlimit == 0) {
			id++;
			continue;
		}
		dquot = add_dquot(id, type);
		udq = &dquot->dq_dqb;
		udq->dqb_bhardlimit = ddqblk.dqb_bhardlimit;
		udq->dqb_bsoftlimit = ddqblk.dqb_bsoftlimit;
		udq->dqb_ihardlimit = ddqblk.dqb_ihardlimit;
		udq->dqb_isoftlimit = ddqblk.dqb_isoftlimit;
		udq->dqb_btime = ddqblk.dqb_btime;
		udq->dqb_itime = ddqblk.dqb_itime;
		id++;
	}
}

/* Load first structure - get grace times */
static int check_info(char *filename, int fd, int type)
{
	struct v1_disk_dqblk ddqblk;
	int err;

	debug(FL_DEBUG, _("Loading first quota entry with grace times.\n"));
	lseek(fd, 0, SEEK_SET);
	err = read(fd, &ddqblk, sizeof(ddqblk));
	if (err < 0)
		die(1, _("Cannot read first entry from quotafile %s: %s\n"), filename,
		    strerror(errno));
	if (err != sizeof(ddqblk)) {
		errstr(
			_("WARNING - Quotafile %s was probably truncated. Cannot save quota settings...\n"),
			filename);
		return -1;
	}
	old_info[type].dqi_bgrace = ddqblk.dqb_btime;
	old_info[type].dqi_igrace = ddqblk.dqb_itime;
	debug(FL_DEBUG, _("First entry loaded.\n"));
	return 0;
}

int v1_buffer_file(char *filename, int fd, int type)
{
	old_info[type].dqi_bgrace = MAX_DQ_TIME;
	old_info[type].dqi_igrace = MAX_IQ_TIME;
	if (flags & FL_NEWFILE)
		return 0;
	if (check_info(filename, fd, type) < 0)
		return 0;
	load_dquots(filename, fd, type);
	return 0;
}
