/*
 *	Headerfile for old quotafile format
 */

#ifndef GUARD_QUOTAIO_V1_H
#define GUARD_QUOTAIO_V1_H

#include <sys/types.h>

#define V1_DQBLK_SIZE_BITS 10
#define V1_DQBLK_SIZE (1 << V1_DQBLK_SIZE_BITS)	/* Size of one quota block in bytes in old format */

#define V1_DQOFF(id) ((loff_t) ((id) * sizeof(struct v1_disk_dqblk)))

/* Structure of quota on disk */
struct v1_disk_dqblk {
	u_int32_t dqb_bhardlimit;	/* absolute limit on disk blks alloc */
	u_int32_t dqb_bsoftlimit;	/* preferred limit on disk blks */
	u_int32_t dqb_curblocks;	/* current block count */
	u_int32_t dqb_ihardlimit;	/* maximum # allocated inodes */
	u_int32_t dqb_isoftlimit;	/* preferred limit on inodes */
	u_int32_t dqb_curinodes;	/* current # allocated inodes */
	time_t dqb_btime;	/* time limit for excessive disk use */
	time_t dqb_itime;	/* time limit for excessive files */
} __attribute__ ((packed));

/* Structure used for communication with kernel */
struct v1_kern_dqblk {
	u_int32_t dqb_bhardlimit;	/* absolute limit on disk blks alloc */
	u_int32_t dqb_bsoftlimit;	/* preferred limit on disk blks */
	u_int32_t dqb_curblocks;	/* current block count */
	u_int32_t dqb_ihardlimit;	/* maximum # allocated inodes */
	u_int32_t dqb_isoftlimit;	/* preferred inode limit */
	u_int32_t dqb_curinodes;	/* current # allocated inodes */
	time_t dqb_btime;	/* time limit for excessive disk use */
	time_t dqb_itime;	/* time limit for excessive files */
};

struct v1_dqstats {
	u_int32_t lookups;
	u_int32_t drops;
	u_int32_t reads;
	u_int32_t writes;
	u_int32_t cache_hits;
	u_int32_t allocated_dquots;
	u_int32_t free_dquots;
	u_int32_t syncs;
};                                                                               
#endif
