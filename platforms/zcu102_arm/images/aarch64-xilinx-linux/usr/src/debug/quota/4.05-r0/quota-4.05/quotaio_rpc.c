/*
 *	quotaio_rpc.c - quota IO operations for RPC (just wrappers for RPC calls)
 */

#include "config.h"

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>

#include "common.h"
#include "quotaio.h"
#include "dqblk_rpc.h"
#include "rquota_client.h"
#include "pot.h"

static int rpc_init_io(struct quota_handle *h);
static struct dquot *rpc_read_dquot(struct quota_handle *h, qid_t id);
static int rpc_commit_dquot(struct dquot *dquot, int flags);

struct quotafile_ops quotafile_ops_rpc = {
init_io:	rpc_init_io,
read_dquot:	rpc_read_dquot,
commit_dquot:	rpc_commit_dquot
};

/*
 * Define maximal values RPC client can transmit to server.
 */
static int rpc_init_io(struct quota_handle *h)
{
#ifdef RPC
	h->qh_info.dqi_max_b_limit = ~(uint32_t)0;
	h->qh_info.dqi_max_i_limit = ~(uint32_t)0;
	h->qh_info.dqi_max_b_usage = ((uint64_t)(~(uint32_t)0))
							 << QUOTABLOCK_BITS;
	h->qh_info.dqi_max_i_usage = ~(uint32_t)0;
	return 0;
#else
	return -1;
#endif
}

/*
 *	Read a dqblk struct from RPC server - just wrapper function.
 */
static struct dquot *rpc_read_dquot(struct quota_handle *h, qid_t id)
{
#ifdef RPC
	struct dquot *dquot = get_empty_dquot();
	int ret;

	dquot->dq_id = id;
	dquot->dq_h = h;
	if ((ret = rpc_rquota_get(dquot)) < 0) {
		errno = -ret;
		free(dquot);
		return NULL;
	}
	return dquot;
#else
	errno = ENOTSUP;
	return NULL;
#endif
}

/*
 *	Write a dqblk struct to RPC server - just wrapper function.
 */
static int rpc_commit_dquot(struct dquot *dquot, int flags)
{
#ifdef RPC
	int ret;

	if (QIO_RO(dquot->dq_h)) {
		errstr(_("Trying to write quota to readonly quotafile on %s\n"), dquot->dq_h->qh_quotadev);
		errno = EPERM;
		return -1;
	}
	if ((ret = rpc_rquota_set(QCMD(Q_RPC_SETQUOTA, dquot->dq_h->qh_type), dquot)) < 0) {
		errno = -ret;
		return -1;
	}
	return 0;
#else
	errno = ENOTSUP;
	return -1;
#endif
}
