/*
 *	Headerfile for XFS quota format
 */

#ifndef GUARD_DQBLK_XFS_H
#define GUARD_DQBLK_XFS_H

#include "quotaio_xfs.h"

#define Q_XFS_QUOTAON	Q_XQUOTAON
#define Q_XFS_QUOTAOFF	Q_XQUOTAOFF
#define Q_XFS_GETQUOTA	Q_XGETQUOTA
#define Q_XFS_SETQLIM	Q_XSETQLIM
#define Q_XFS_GETQSTAT	Q_XGETQSTAT
#define Q_XFS_QUOTARM	Q_XQUOTARM

#define xfs_mem_dqinfo	fs_quota_stat
#define xfs_kern_dqblk	fs_disk_quota

struct quotafile_ops;		/* Will be defined later in quotaio.h */

/* Operations above this format */
extern struct quotafile_ops quotafile_ops_xfs;

#endif
