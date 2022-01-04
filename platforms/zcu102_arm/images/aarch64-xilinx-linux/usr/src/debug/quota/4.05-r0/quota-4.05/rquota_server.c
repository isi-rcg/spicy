/*
 * QUOTA    An implementation of the diskquota system for the LINUX
 *          operating system. QUOTA is implemented using the BSD systemcall
 *          interface as the means of communication with the user level.
 *          Should work for all filesystems because of integration into the
 *          VFS layer of the operating system.
 *          This is based on the Melbourne quota system wich uses both user and
 *          group quota files.
 *
 *          This part does the lookup of the info.
 *
 * Author:  Marco van Wieringen <mvw@planets.elm.net>
 *
 *          This program is free software; you can redistribute it and/or
 *          modify it under the terms of the GNU General Public License
 *          as published by the Free Software Foundation; either version
 *          2 of the License, or (at your option) any later version.
 */

#include "config.h"

#include <rpc/rpc.h>
#include <arpa/inet.h>
#include <paths.h>
#include <stdio.h>
#include <syslog.h>
#include <time.h>
#include <stdint.h>

#include "mntopt.h"
#include "quotaops.h"
#include "bylabel.h"
#include "rquota.h"
#include "quotaio.h"
#include "quotasys.h"
#include "dqblk_rpc.h"
#include "common.h"

#define STDIN_FILENO	0

#define TYPE_EXTENDED	0x01
#define ACTIVE		0x02

#define FACILITY	LOG_LOCAL7

#ifndef MAXPATHNAMELEN
#define MAXPATHNAMELEN BUFSIZ
#endif

#define NETTYPE AF_INET

/* Options from rquota_svc.c */
#define FL_AUTOFS 4
extern int flags;

extern char nfs_pseudoroot[PATH_MAX];

/*
 * Global unix authentication credentials.
 */
extern struct authunix_parms *unix_cred;

int in_group(gid_t * gids, uint32_t len, gid_t gid)
{
	gid_t *gidsp = gids + len;

	while (gidsp > gids)
		if (*(--gids) == gid)
			return 1;

	return 0;
}

static inline void servnet2utildqblk(struct util_dqblk *u, sq_dqblk * n)
{
	time_t now;

	time(&now);
	u->dqb_bhardlimit = n->rq_bhardlimit;
	u->dqb_bsoftlimit = n->rq_bsoftlimit;
	u->dqb_ihardlimit = n->rq_fhardlimit;
	u->dqb_isoftlimit = n->rq_fsoftlimit;
	u->dqb_curspace = ((qsize_t)n->rq_curblocks) << RPC_DQBLK_SIZE_BITS;
	u->dqb_curinodes = n->rq_curfiles;
	if (n->rq_btimeleft)
		u->dqb_btime = (int32_t)n->rq_btimeleft + now;
	else
		u->dqb_btime = 0;
	if (n->rq_ftimeleft)
		u->dqb_itime = (int32_t)n->rq_ftimeleft + now;
	else
		u->dqb_itime = 0;
}

/* XDR transports 32b variables exactly. Find smallest needed shift to fit
 * 64b variable into into 32 bits and to preserve precision as high as
 * possible. */
static int find_block_shift(qsize_t hard, qsize_t soft, qsize_t cur)
{
	int shift;
	qsize_t value = hard;

	if (value < soft)
		value = soft;
	if (value < cur)
		value = cur;
	value >>= 32;
	for (shift = QUOTABLOCK_BITS; value; shift++)
		value >>= 1;

	return shift;
}

static inline void servutil2netdqblk(struct rquota *n, struct util_dqblk *u)
{
	time_t now;
	int shift;

	shift = find_block_shift(u->dqb_bhardlimit, u->dqb_bsoftlimit,
		toqb(u->dqb_curspace));
	n->rq_bsize = 1 << shift;
	n->rq_bhardlimit = u->dqb_bhardlimit >> (shift - QUOTABLOCK_BITS);
	n->rq_bsoftlimit = u->dqb_bsoftlimit >> (shift - QUOTABLOCK_BITS);
	n->rq_fhardlimit = u->dqb_ihardlimit;
	n->rq_fsoftlimit = u->dqb_isoftlimit;
	n->rq_curblocks = toqb(u->dqb_curspace) >> (shift - QUOTABLOCK_BITS);
	n->rq_curfiles = u->dqb_curinodes;

	time(&now);
	if (u->dqb_btime)
		n->rq_btimeleft = difftime2net(u->dqb_btime, now);
	else
		n->rq_btimeleft = 0;
	if (u->dqb_itime)
		n->rq_ftimeleft = difftime2net(u->dqb_itime, now);
	else
		n->rq_ftimeleft = 0;
}

setquota_rslt *setquotainfo(int lflags, caddr_t * argp, struct svc_req *rqstp)
{
	static setquota_rslt result;

#if defined(RPC_SETQUOTA)
	union {
		setquota_args *args;
		ext_setquota_args *ext_args;
	} arguments;
	struct util_dqblk dqblk;
	struct dquot *dquot;
	struct mount_entry *mnt;
	char pathname[PATH_MAX] = {0};
	char *pathp = pathname;
	int id, qcmd, type;
	struct quota_handle *handles[2] = { NULL, NULL };

	/*
	 * First check authentication.
	 */
	if (lflags & TYPE_EXTENDED) {
		arguments.ext_args = (ext_setquota_args *) argp;

		id = arguments.ext_args->sqa_id;
		if (unix_cred->aup_uid != 0) {
			result.status = Q_EPERM;
			return (&result);
		}

		qcmd = arguments.ext_args->sqa_qcmd;
		type = arguments.ext_args->sqa_type;
		if (arguments.ext_args->sqa_pathp[0] != '/')
			sstrncpy(pathname, nfs_pseudoroot, PATH_MAX);
		sstrncat(pathname, arguments.ext_args->sqa_pathp, PATH_MAX);
		servnet2utildqblk(&dqblk, &arguments.ext_args->sqa_dqblk);
	}
	else {
		arguments.args = (setquota_args *) argp;

		id = arguments.args->sqa_id;
		if (unix_cred->aup_uid != 0) {
			result.status = Q_EPERM;
			return (&result);
		}

		qcmd = arguments.args->sqa_qcmd;
		type = USRQUOTA;
		if (arguments.args->sqa_pathp[0] != '/')
			sstrncpy(pathname, nfs_pseudoroot, PATH_MAX);
		sstrncat(pathname, arguments.args->sqa_pathp, PATH_MAX);
		servnet2utildqblk(&dqblk, &arguments.args->sqa_dqblk);
	}

	result.status = Q_NOQUOTA;
	result.setquota_rslt_u.sqr_rquota.rq_bsize = RPC_DQBLK_SIZE;

	if (init_mounts_scan(1, &pathp, MS_QUIET | MS_NO_MNTPOINT | MS_NFS_ALL | ((flags & FL_AUTOFS) ? 0 : MS_NO_AUTOFS)) < 0)
		goto out;
	if (!(mnt = get_next_mount())) {
		end_mounts_scan();
		goto out;
	}
	if (!(handles[0] = init_io(mnt, type, -1, 0))) {
		end_mounts_scan();
		goto out;
	}
	end_mounts_scan();
	if (!(dquot = handles[0]->qh_ops->read_dquot(handles[0], id)))
		goto out;
	if (qcmd == QCMD(Q_RPC_SETQLIM, type) || qcmd == QCMD(Q_RPC_SETQUOTA, type)) {
		dquot->dq_dqb.dqb_bsoftlimit = dqblk.dqb_bsoftlimit;
		dquot->dq_dqb.dqb_bhardlimit = dqblk.dqb_bhardlimit;
		dquot->dq_dqb.dqb_isoftlimit = dqblk.dqb_isoftlimit;
		dquot->dq_dqb.dqb_ihardlimit = dqblk.dqb_ihardlimit;
		dquot->dq_dqb.dqb_btime = dqblk.dqb_btime;
		dquot->dq_dqb.dqb_itime = dqblk.dqb_itime;
	}
	if (qcmd == QCMD(Q_RPC_SETUSE, type) || qcmd == QCMD(Q_RPC_SETQUOTA, type)) {
		dquot->dq_dqb.dqb_curspace = dqblk.dqb_curspace;
		dquot->dq_dqb.dqb_curinodes = dqblk.dqb_curinodes;
	}
	if (handles[0]->qh_ops->commit_dquot(dquot, COMMIT_LIMITS) == -1) {
		free(dquot);
		goto out;
	}
	free(dquot);
	result.status = Q_OK;
out:
	dispose_handle_list(handles);
#else
	result.status = Q_EPERM;
#endif
	return (&result);
}

getquota_rslt *getquotainfo(int lflags, caddr_t * argp, struct svc_req * rqstp)
{
	static getquota_rslt result;
	union {
		getquota_args *args;
		ext_getquota_args *ext_args;
	} arguments;
	struct dquot *dquot = NULL;
	struct mount_entry *mnt;
	char pathname[PATH_MAX] = {0};
	char *pathp = pathname;
	int id, type;
	struct quota_handle *handles[2] = { NULL, NULL };

	/*
	 * First check authentication.
	 */
	if (lflags & TYPE_EXTENDED) {
		arguments.ext_args = (ext_getquota_args *) argp;
		id = arguments.ext_args->gqa_id;
		type = arguments.ext_args->gqa_type;
		if (arguments.ext_args->gqa_pathp[0] != '/')
			sstrncpy(pathname, nfs_pseudoroot, PATH_MAX);
		sstrncat(pathname, arguments.ext_args->gqa_pathp, PATH_MAX);

		if (type == USRQUOTA && unix_cred->aup_uid && unix_cred->aup_uid != id) {
			result.status = Q_EPERM;
			return (&result);
		}

		if (type == GRPQUOTA && unix_cred->aup_uid && unix_cred->aup_gid != id &&
		    !in_group((gid_t *) unix_cred->aup_gids, unix_cred->aup_len, id)) {
			result.status = Q_EPERM;
			return (&result);
		}
	}
	else {
		arguments.args = (getquota_args *) argp;
		id = arguments.args->gqa_uid;
		type = USRQUOTA;
		if (arguments.ext_args->gqa_pathp[0] != '/')
			sstrncpy(pathname, nfs_pseudoroot, PATH_MAX);
		sstrncat(pathname, arguments.args->gqa_pathp, PATH_MAX);

		if (unix_cred->aup_uid && unix_cred->aup_uid != id) {
			result.status = Q_EPERM;
			return (&result);
		}
	}

	result.status = Q_NOQUOTA;

	if (init_mounts_scan(1, &pathp, MS_QUIET | MS_NO_MNTPOINT | MS_NFS_ALL | ((flags & FL_AUTOFS) ? 0 : MS_NO_AUTOFS)) < 0)
		goto out;
	if (!(mnt = get_next_mount())) {
		end_mounts_scan();
		goto out;
	}
	if (!(handles[0] = init_io(mnt, type, -1, IOI_READONLY))) {
		end_mounts_scan();
		goto out;
	}
	end_mounts_scan();
	if (!(lflags & ACTIVE) || QIO_ENABLED(handles[0]))
		dquot = handles[0]->qh_ops->read_dquot(handles[0], id);
	if (dquot) {
		result.status = Q_OK;
		result.getquota_rslt_u.gqr_rquota.rq_active =
			QIO_ENABLED(handles[0]) ? TRUE : FALSE;
		servutil2netdqblk(&result.getquota_rslt_u.gqr_rquota, &dquot->dq_dqb);
		free(dquot);
	}
out:
	dispose_handle_list(handles);
	return (&result);
}

/*
 * Map RPC-entrypoints to local function names.
 */
getquota_rslt *rquotaproc_getquota_1_svc(getquota_args * argp, struct svc_req * rqstp)
{
	return (getquotainfo(0, (caddr_t *) argp, rqstp));
}

getquota_rslt *rquotaproc_getactivequota_1_svc(getquota_args * argp, struct svc_req * rqstp)
{
	return (getquotainfo(ACTIVE, (caddr_t *) argp, rqstp));
}

getquota_rslt *rquotaproc_getquota_2_svc(ext_getquota_args * argp, struct svc_req * rqstp)
{
	return (getquotainfo(TYPE_EXTENDED, (caddr_t *) argp, rqstp));
}

getquota_rslt *rquotaproc_getactivequota_2_svc(ext_getquota_args * argp, struct svc_req * rqstp)
{
	return (getquotainfo(TYPE_EXTENDED | ACTIVE, (caddr_t *) argp, rqstp));
}

setquota_rslt *rquotaproc_setquota_1_svc(setquota_args * argp, struct svc_req * rqstp)
{
	return (setquotainfo(0, (caddr_t *) argp, rqstp));
}

setquota_rslt *rquotaproc_setactivequota_1_svc(setquota_args * argp, struct svc_req * rqstp)
{
	return (setquotainfo(ACTIVE, (caddr_t *) argp, rqstp));
}

setquota_rslt *rquotaproc_setquota_2_svc(ext_setquota_args * argp, struct svc_req * rqstp)
{
	return (setquotainfo(TYPE_EXTENDED, (caddr_t *) argp, rqstp));
}

setquota_rslt *rquotaproc_setactivequota_2_svc(ext_setquota_args * argp, struct svc_req * rqstp)
{
	return (setquotainfo(TYPE_EXTENDED | ACTIVE, (caddr_t *) argp, rqstp));
}
