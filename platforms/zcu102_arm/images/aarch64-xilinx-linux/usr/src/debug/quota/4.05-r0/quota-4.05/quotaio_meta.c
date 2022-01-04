/*
 *	Implementation of handling of quotafiles which are hidden
 *
 *	Jan Kara <jack@suse.cz>
 */

#include "config.h"

#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/types.h>

#include "pot.h"
#include "common.h"
#include "quotasys.h"
#include "quotaio_generic.h"

static int meta_init_io(struct quota_handle *h)
{
	if (!QIO_ENABLED(h)) {
		errstr(_("Metadata init_io called when kernel support is not enabled.\n"));
		return -1;
	}
	if (kernel_iface != IFACE_GENERIC) {
		errstr(_("Metadata init_io called when kernel does not support generic quota interface!\n"));
		return -1;
	}
	return vfs_get_info(h);
}

static int meta_write_info(struct quota_handle *h)
{
	return vfs_set_info(h, IIF_BGRACE | IIF_IGRACE);
}

static struct dquot *meta_read_dquot(struct quota_handle *h, qid_t id)
{
	struct dquot *dquot = get_empty_dquot();

	dquot->dq_id = id;
	dquot->dq_h = h;
	memset(&dquot->dq_dqb, 0, sizeof(struct util_dqblk));
	if (vfs_get_dquot(dquot) < 0) {
		free(dquot);
		return NULL;
	}
	return dquot;
}

static int meta_commit_dquot(struct dquot *dquot, int flags)
{
	return vfs_set_dquot(dquot, flags);
}

static int meta_scan_dquots(struct quota_handle *h, int (*process_dquot)(struct dquot *dquot, char *dqname))
{
	struct if_nextdqblk kdqblk;
	int ret;

	ret = quotactl(QCMD(Q_GETNEXTQUOTA, h->qh_type), h->qh_quotadev, 0,
		       (void *)&kdqblk);
	/*
	 * Fall back to scanning using passwd if Q_GETNEXTQUOTA is not
	 * supported
	 */
	if (ret < 0 && (errno == ENOSYS || errno == EINVAL))
		return generic_scan_dquots(h, process_dquot, vfs_get_dquot);
	return vfs_scan_dquots(h, process_dquot);
}

struct quotafile_ops quotafile_ops_meta = {
init_io:	meta_init_io,
write_info:	meta_write_info,
read_dquot:	meta_read_dquot,
commit_dquot:	meta_commit_dquot,
scan_dquots:	meta_scan_dquots,
};
