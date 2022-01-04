/*
 *
 *	Header file for communication with kernel generic interface
 *
 */

#ifndef GUARD_QUOTAIO_GENERIC_H
#define GUARD_QUOTAIO_GENERIC_H

#include "quotaio.h"

/* Get info from kernel to handle */
int vfs_get_info(struct quota_handle *h);

/* Set info in kernel from handle */
int vfs_set_info(struct quota_handle *h, int flags);

/* Get dquot from kernel */
int vfs_get_dquot(struct dquot *dquot);

/* Set dquot in kernel */
int vfs_set_dquot(struct dquot *dquot, int flags);

/* Generic routine for scanning dquots when quota format does not have
 * better way */
int generic_scan_dquots(struct quota_handle *h,
			int (*process_dquot)(struct dquot *dquot, char *dqname),
			int (*get_dquot)(struct dquot *dquot));

/* Scan all dquots using kernel quotactl to get existing ids */
int vfs_scan_dquots(struct quota_handle *h,
		    int (*process_dquot)(struct dquot *dquot, char *dqname));

#endif
