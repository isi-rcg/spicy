/*
 *	Headerfile for rpc quotafile format
 */

#ifndef GUARD_DQBLK_RPC_H
#define GUARD_DQBLK_RPC_H

/* Values used for communication through network */
#define Q_RPC_GETQUOTA	0x0300	/* get limits and usage */
#define Q_RPC_SETQUOTA	0x0400	/* set limits and usage */
#define Q_RPC_SETUSE	0x0500	/* set usage */
#define Q_RPC_SETQLIM	0x0700	/* set limits */

#define RPC_DQBLK_SIZE_BITS 10
#define RPC_DQBLK_SIZE (1 << RPC_DQBLK_SIZE_BITS)

/* Operations above this format */
extern struct quotafile_ops quotafile_ops_rpc;

#endif
