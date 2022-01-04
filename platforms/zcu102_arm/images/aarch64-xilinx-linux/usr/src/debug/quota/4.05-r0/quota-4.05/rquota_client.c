/*
 * QUOTA    An implementation of the diskquota system for the LINUX
 *          operating system. QUOTA is implemented using the BSD systemcall
 *          interface as the means of communication with the user level.
 *          Should work for all filesystems because of integration into the
 *          VFS layer of the operating system.
 *          This is based on the Melbourne quota system wich uses both user and
 *          group quota files.
 *
 *          This part does the rpc-communication with the rquotad.
 *
 * Author:  Marco van Wieringen <mvw@planets.elm.net>
 *
 *          This program is free software; you can redistribute it and/or
 *          modify it under the terms of the GNU General Public License
 *          as published by the Free Software Foundation; either version
 *          2 of the License, or (at your option) any later version.
 */

#include "config.h"

#if defined(RPC)
#include <rpc/rpc.h>
#endif
#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <stdint.h>

#include "mntopt.h"
#if defined(RPC)
#include "rquota.h"
#endif
#include "common.h"
#include "quotaio.h"
#include "quotasys.h"

#if defined(RPC)

/* Convert network format of quotas to utils one */
static inline void clinet2utildqblk(struct util_dqblk *u, struct rquota *n)
{
	time_t now;
	
	/* Copy the quota */
	u->dqb_bhardlimit = toqb(((qsize_t)n->rq_bhardlimit) * n->rq_bsize);
	u->dqb_bsoftlimit = toqb(((qsize_t)n->rq_bsoftlimit) * n->rq_bsize);
	u->dqb_ihardlimit = n->rq_fhardlimit;
	u->dqb_isoftlimit = n->rq_fsoftlimit;
	u->dqb_curinodes = n->rq_curfiles;
	u->dqb_curspace = ((qsize_t)n->rq_curblocks) * n->rq_bsize;
	time(&now);
	if (n->rq_btimeleft)
		u->dqb_btime = (int32_t)n->rq_btimeleft + now;
	else
		u->dqb_btime = 0;
	if (n->rq_ftimeleft)
		u->dqb_itime = (int32_t)n->rq_ftimeleft + now;
	else
		u->dqb_itime = 0;
}

/* Convert utils format of quotas to network one */
static inline void cliutil2netdqblk(struct sq_dqblk *n, struct util_dqblk *u)
{
	time_t now;

	time(&now);
	n->rq_bhardlimit = u->dqb_bhardlimit;
	n->rq_bsoftlimit = u->dqb_bsoftlimit;
	n->rq_fhardlimit = u->dqb_ihardlimit;
	n->rq_fsoftlimit = u->dqb_isoftlimit;
	n->rq_curblocks = toqb(u->dqb_curspace);
	n->rq_curfiles = u->dqb_curinodes;
	if (u->dqb_btime)
		n->rq_btimeleft = difftime2net(u->dqb_btime, now);
	else
		n->rq_btimeleft = 0;
	if (u->dqb_itime)
		n->rq_ftimeleft = difftime2net(u->dqb_itime, now);
	else
		n->rq_ftimeleft = 0;
}

/* Write appropriate error message */
static int rquota_err(int stat)
{
	switch (stat) {
		case -1:
			return -ECONNREFUSED;
		case 0:
			return -ENOSYS;
		case Q_NOQUOTA:
			return -ENOENT;
		case Q_OK:
			return 0;
		case Q_EPERM:
			return -EPERM;
		default:
			return -EINVAL;
	}
}

static int split_nfs_mount(char *devname, char **host, char **path)
{
	char *pathname;

	/* NFS server name contained in brackets? */
	if (*devname == '[') {
		*host = devname + 1;
		pathname = strchr(devname, ']');
		if (!pathname || pathname[1] != ':')
			return 0;
		/* Autofs? */
		if (pathname[2] == '(')
			return 0;
		*pathname = 0;
		*path = pathname + 2;
		return 1;
	}
	*host = devname;
	pathname = strchr(devname, ':');
	if (!pathname)
		return 0;
	/* Autofs? */
	if (pathname[1] == '(')
		return 0;
	*pathname = 0;
	*path = pathname + 1;
	return 1;
}

/*
 * Collect the requested quota information from a remote host.
 */
int rpc_rquota_get(struct dquot *dquot)
{
	CLIENT *clnt;
	getquota_rslt *result;
	union {
		getquota_args arg;
		ext_getquota_args ext_arg;
	} args;
	char *fsname_tmp, *host, *pathname;
	struct timeval timeout = { 2, 0 };
	int rquotaprog_not_registered = 0;
	int ret;

	/*
	 * Initialize with NULL.
	 */
	memset(&dquot->dq_dqb, 0, sizeof(dquot->dq_dqb));

	/*
	 * Convert host:pathname to seperate host and pathname.
	 */
	fsname_tmp = (char *)smalloc(strlen(dquot->dq_h->qh_quotadev) + 1);
	strcpy(fsname_tmp, dquot->dq_h->qh_quotadev);
	if (!split_nfs_mount(fsname_tmp, &host, &pathname)) {
		free(fsname_tmp);
		return -ENOENT;
	}

	/* For NFSv4, we send the filesystem path without initial /. Server prepends proper
	 * NFS pseudoroot automatically and uses this for detection of NFSv4 mounts. */
	if ((dquot->dq_h->qh_io_flags & IOFL_NFS_MIXED_PATHS) &&
	    !strcmp(dquot->dq_h->qh_fstype, MNTTYPE_NFS4)) {
		while (*pathname == '/')
			pathname++;
	}

	/*
	 * First try EXT_RQUOTAPROG (Extended (LINUX) RPC quota program)
	 */
	args.ext_arg.gqa_pathp = pathname;
	args.ext_arg.gqa_id = dquot->dq_id;
	args.ext_arg.gqa_type = dquot->dq_h->qh_type;

	/*
	 * Create a RPC client.
	 */
	if ((clnt = clnt_create(host, RQUOTAPROG, EXT_RQUOTAVERS, "udp")) != NULL) {
		/*
		 * Initialize unix authentication
		 */
		clnt->cl_auth = authunix_create_default();

		/*
		 * Setup protocol timeout.
		 */
		clnt_control(clnt, CLSET_TIMEOUT, (caddr_t) & timeout);

		/*
		 * Do RPC call and check result.
		 */
		result = rquotaproc_getquota_2(&args.ext_arg, clnt);
		if (result != NULL && result->status == Q_OK)
			clinet2utildqblk(&dquot->dq_dqb, &result->getquota_rslt_u.gqr_rquota);

		/*
		 * Destroy unix authentication and RPC client structure.
		 */
		auth_destroy(clnt->cl_auth);
		clnt_destroy(clnt);
	}
	else {
		result = NULL;
		if (rpc_createerr.cf_stat == RPC_PROGNOTREGISTERED)
			rquotaprog_not_registered = 1;
	}

	if (result == NULL || !result->status) {
		if (dquot->dq_h->qh_type == USRQUOTA) {
			/*
			 * Try RQUOTAPROG because server doesn't seem to understand EXT_RQUOTAPROG. (NON-LINUX servers.)
			 */
			args.arg.gqa_pathp = pathname;
			args.arg.gqa_uid = dquot->dq_id;

			/*
			 * Create a RPC client.
			 */
			if ((clnt = clnt_create(host, RQUOTAPROG, RQUOTAVERS, "udp")) != NULL) {
				/*
				 * Initialize unix authentication
				 */
				clnt->cl_auth = authunix_create_default();

				/*
				 * Setup protocol timeout.
				 */
				clnt_control(clnt, CLSET_TIMEOUT, (caddr_t) & timeout);

				/*
				 * Do RPC call and check result.
				 */
				result = rquotaproc_getquota_1(&args.arg, clnt);
				if (result != NULL && result->status == Q_OK)
					clinet2utildqblk(&dquot->dq_dqb,
							 &result->getquota_rslt_u.gqr_rquota);

				/*
				 * Destroy unix authentication and RPC client structure.
				 */
				auth_destroy(clnt->cl_auth);
				clnt_destroy(clnt);
			} else {
				result = NULL;
				if (rpc_createerr.cf_stat == RPC_PROGNOTREGISTERED)
					    rquotaprog_not_registered = 1;
			}
		}
	}
	free(fsname_tmp);
	if (result)
		ret = result->status;
	else if (rquotaprog_not_registered)
		ret = Q_NOQUOTA;
	else
		ret = -1;
	return rquota_err(ret);
}

/*
 * Set the requested quota information on a remote host.
 */
int rpc_rquota_set(int qcmd, struct dquot *dquot)
{
#if defined(RPC_SETQUOTA)
	CLIENT *clnt;
	setquota_rslt *result;
	union {
		setquota_args arg;
		ext_setquota_args ext_arg;
	} args;
	char *fsname_tmp, *host, *pathname;
	struct timeval timeout = { 2, 0 };
	int rquotaprog_not_registered = 0;
	int ret;

	/* RPC limits values to 32b variables. Prevent value wrapping. */
	if (check_dquot_range(dquot) < 0)
		return -ERANGE;

	/*
	 * Convert host:pathname to seperate host and pathname.
	 */
	fsname_tmp = (char *)smalloc(strlen(dquot->dq_h->qh_quotadev) + 1);
	strcpy(fsname_tmp, dquot->dq_h->qh_quotadev);
	if (!split_nfs_mount(fsname_tmp, &host, &pathname)) {
		free(fsname_tmp);
		return -ENOENT;
	}

	/* For NFSv4, we send the filesystem path without initial /. Server prepends proper
	 * NFS pseudoroot automatically and uses this for detection of NFSv4 mounts. */
	if ((dquot->dq_h->qh_io_flags & IOFL_NFS_MIXED_PATHS) &&
	    !strcmp(dquot->dq_h->qh_fstype, MNTTYPE_NFS4)) {
		while (*pathname == '/')
			pathname++;
	}

	/*
	 * First try EXT_RQUOTAPROG (Extended (LINUX) RPC quota program)
	 */
	args.ext_arg.sqa_qcmd = qcmd;
	args.ext_arg.sqa_pathp = pathname;
	args.ext_arg.sqa_id = dquot->dq_id;
	args.ext_arg.sqa_type = dquot->dq_h->qh_type;
	cliutil2netdqblk(&args.ext_arg.sqa_dqblk, &dquot->dq_dqb);

	if ((clnt = clnt_create(host, RQUOTAPROG, EXT_RQUOTAVERS, "udp")) != NULL) {
		/*
		 * Initialize unix authentication
		 */
		clnt->cl_auth = authunix_create_default();

		/*
		 * Setup protocol timeout.
		 */
		clnt_control(clnt, CLSET_TIMEOUT, (caddr_t) & timeout);

		/*
		 * Do RPC call and check result.
		 */
		result = rquotaproc_setquota_2(&args.ext_arg, clnt);
		if (result != NULL && result->status == Q_OK)
			clinet2utildqblk(&dquot->dq_dqb, &result->setquota_rslt_u.sqr_rquota);

		/*
		 * Destroy unix authentication and RPC client structure.
		 */
		auth_destroy(clnt->cl_auth);
		clnt_destroy(clnt);
	}
	else {
		result = NULL;
		if (rpc_createerr.cf_stat == RPC_PROGNOTREGISTERED)
			rquotaprog_not_registered = 1;
	}

	if (result == NULL || !result->status) {
		if (dquot->dq_h->qh_type == USRQUOTA) {
			/*
			 * Try RQUOTAPROG because server doesn't seem to understand EXT_RQUOTAPROG. (NON-LINUX servers.)
			 */
			args.arg.sqa_qcmd = qcmd;
			args.arg.sqa_pathp = pathname;
			args.arg.sqa_id = dquot->dq_id;
			cliutil2netdqblk(&args.arg.sqa_dqblk, &dquot->dq_dqb);

			/*
			 * Create a RPC client.
			 */
			if ((clnt = clnt_create(host, RQUOTAPROG, RQUOTAVERS, "udp")) != NULL) {
				/*
				 * Initialize unix authentication
				 */
				clnt->cl_auth = authunix_create_default();

				/*
				 * Setup protocol timeout.
				 */
				clnt_control(clnt, CLSET_TIMEOUT, (caddr_t) & timeout);

				/*
				 * Do RPC call and check result.
				 */
				result = rquotaproc_setquota_1(&args.arg, clnt);
				if (result != NULL && result->status == Q_OK)
					clinet2utildqblk(&dquot->dq_dqb,
							 &result->setquota_rslt_u.sqr_rquota);

				/*
				 * Destroy unix authentication and RPC client structure.
				 */
				auth_destroy(clnt->cl_auth);
				clnt_destroy(clnt);
			} else {
				result = NULL;
				if (rpc_createerr.cf_stat == RPC_PROGNOTREGISTERED)
					rquotaprog_not_registered = 1;
			}
		}
	}
	free(fsname_tmp);
	if (result)
		ret = result->status;
	else if (rquotaprog_not_registered)
		ret = Q_NOQUOTA;
	else
		ret = -1;
	return rquota_err(ret);
#endif
	return -1;
}
#endif
