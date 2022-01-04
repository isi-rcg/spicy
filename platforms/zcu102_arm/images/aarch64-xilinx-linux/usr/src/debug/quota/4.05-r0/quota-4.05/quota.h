#ifndef GUARD_QUOTA_H
#define GUARD_QUOTA_H

#include <sys/cdefs.h>
#include <sys/types.h>
#include <stdint.h>

typedef u_int32_t qid_t;	/* Type in which we store ids in memory */
typedef int64_t qsize_t;	/* Type in which we store size limitations */
#define QSIZE_MAX INT64_MAX /* Maximum value storable in qsize_t */

#define MAXQUOTAS 3
#define USRQUOTA  0		/* element used for user quotas */
#define GRPQUOTA  1		/* element used for group quotas */
#define PRJQUOTA  2		/* element used for project quotas */

/*
 * Definitions for the default names of the quotas files.
 * N_ is gettext magic for static strings
 */
#define N_(x) (x)
#define INITQFNAMES { \
	N_("user"),    /* USRQUOTA */ \
	N_("group"),   /* GRPQUOTA */ \
	N_("project"), /* PRJQUOTA */ \
	N_("undefined"), \
}

/*
 * Definitions of magics and versions of current quota files
 */
#define INITQMAGICS {\
	0xd9c01f11,	/* USRQUOTA */\
	0xd9c01927	/* GRPQUOTA */\
}

/* Size of blocks in which are counted size limits in generic utility parts */
#define QUOTABLOCK_BITS 10
#define QUOTABLOCK_SIZE (1 << QUOTABLOCK_BITS)

/* Conversion routines from and to quota blocks */
#define qb2kb(x) ((x) << (QUOTABLOCK_BITS-10))
#define kb2qb(x) ((x) >> (QUOTABLOCK_BITS-10))
#define toqb(x) (((x) + QUOTABLOCK_SIZE - 1) >> QUOTABLOCK_BITS)

/*
 * Command definitions for the 'quotactl' system call.
 * The commands are broken into a main command defined below
 * and a subcommand that is used to convey the type of
 * quota that is being manipulated (see above).
 */
#define SUBCMDMASK  0x00ff
#define SUBCMDSHIFT 8
#define QCMD(cmd, type)  (((cmd) << SUBCMDSHIFT) | ((type) & SUBCMDMASK))

#define Q_6_5_QUOTAON  0x0100	/* enable quotas */
#define Q_6_5_QUOTAOFF 0x0200	/* disable quotas */
#define Q_6_5_SYNC     0x0600	/* sync disk copy of a filesystems quotas */

#define Q_SYNC     0x800001	/* sync disk copy of a filesystems quotas */
#define Q_QUOTAON  0x800002	/* turn quotas on */
#define Q_QUOTAOFF 0x800003	/* turn quotas off */
#define Q_GETFMT   0x800004	/* get quota format used on given filesystem */
#define Q_GETINFO  0x800005	/* get information about quota files */
#define Q_SETINFO  0x800006	/* set information about quota files */
#define Q_GETQUOTA 0x800007	/* get user quota structure */
#define Q_SETQUOTA 0x800008	/* set user quota structure */
#define Q_GETNEXTQUOTA 0x800009	/* get disk limits and usage >= ID */

/*
 * Quota structure used for communication with userspace via quotactl
 * Following flags are used to specify which fields are valid
 */
#define QIF_BLIMITS	1
#define QIF_SPACE	2
#define QIF_ILIMITS	4
#define QIF_INODES	8
#define QIF_BTIME	16
#define QIF_ITIME	32
#define QIF_LIMITS	(QIF_BLIMITS | QIF_ILIMITS)
#define QIF_USAGE	(QIF_SPACE | QIF_INODES)
#define QIF_TIMES	(QIF_BTIME | QIF_ITIME)
#define QIF_ALL		(QIF_LIMITS | QIF_USAGE | QIF_TIMES)

struct if_dqblk {
	u_int64_t dqb_bhardlimit;
	u_int64_t dqb_bsoftlimit;
	u_int64_t dqb_curspace;
	u_int64_t dqb_ihardlimit;
	u_int64_t dqb_isoftlimit;
	u_int64_t dqb_curinodes;
	u_int64_t dqb_btime;
	u_int64_t dqb_itime;
	u_int32_t dqb_valid;
};

struct if_nextdqblk {
	u_int64_t dqb_bhardlimit;
	u_int64_t dqb_bsoftlimit;
	u_int64_t dqb_curspace;
	u_int64_t dqb_ihardlimit;
	u_int64_t dqb_isoftlimit;
	u_int64_t dqb_curinodes;
	u_int64_t dqb_btime;
	u_int64_t dqb_itime;
	u_int32_t dqb_valid;
	u_int32_t dqb_id;
};

/*
 * Structure used for setting quota information about file via quotactl
 * Following flags are used to specify which fields are valid
 */
#define IIF_BGRACE	1
#define IIF_IGRACE	2
#define IIF_FLAGS	4
#define IIF_ALL		(IIF_BGRACE | IIF_IGRACE | IIF_FLAGS)

#define DQF_SYS_FILE	0x10000		/* Quota stored in a system file */

struct if_dqinfo {
	u_int64_t dqi_bgrace;
	u_int64_t dqi_igrace;
	u_int32_t dqi_flags;
	u_int32_t dqi_valid;
};

/*
 * Definitions for quota netlink interface
 */
#define QUOTA_NL_NOWARN 0
#define QUOTA_NL_IHARDWARN 1            /* Inode hardlimit reached */
#define QUOTA_NL_ISOFTLONGWARN 2        /* Inode grace time expired */
#define QUOTA_NL_ISOFTWARN 3            /* Inode softlimit reached */
#define QUOTA_NL_BHARDWARN 4            /* Block hardlimit reached */
#define QUOTA_NL_BSOFTLONGWARN 5        /* Block grace time expired */
#define QUOTA_NL_BSOFTWARN 6            /* Block softlimit reached */
#define QUOTA_NL_IHARDBELOW 7		/* Usage got below inode hardlimit */
#define QUOTA_NL_ISOFTBELOW 8		/* Usage got below inode softlimit */
#define QUOTA_NL_BHARDBELOW 9		/* Usage got below block hardlimit */
#define QUOTA_NL_BSOFTBELOW 10		/* Usage got below block softlimit */

enum {
	QUOTA_NL_C_UNSPEC,
	QUOTA_NL_C_WARNING,
	ENUM_QUOTA_NL_C_MAX,
};
#define QUOTA_NL_C_MAX (ENUM_QUOTA_NL_C_MAX - 1)

enum {
	QUOTA_NL_A_UNSPEC,
	QUOTA_NL_A_QTYPE,
	QUOTA_NL_A_EXCESS_ID,
	QUOTA_NL_A_WARNING,
	QUOTA_NL_A_DEV_MAJOR,
	QUOTA_NL_A_DEV_MINOR,
	QUOTA_NL_A_CAUSED_ID,
	ENUM_QUOTA_NL_A_MAX,
};
#define QUOTA_NL_A_MAX (ENUM_QUOTA_NL_A_MAX - 1)

/* Quota format identifiers */
#define QFMT_VFS_OLD 1
#define QFMT_VFS_V0  2
#define QFMT_OCFS2   3
#define QFMT_VFS_V1  4

/* Flags supported by kernel */
#define V1_DQF_RSQUASH 1

/* Ioctl for getting quota size */
#include <sys/ioctl.h>
#ifndef FIOQSIZE
	#if defined(__alpha__) || defined(__powerpc__) || defined(__sh__) || defined(__sparc__) || defined(__sparc64__)
		#define FIOQSIZE _IOR('f', 128, loff_t)
	#elif defined(__arm__) || defined(__mc68000__) || defined(__s390__)
		#define FIOQSIZE 0x545E
        #elif defined(__i386__) || defined(__i486__) || defined(__i586__) || defined(__ia64__) || defined(__parisc__) || defined(__cris__) || defined(__hppa__) || defined(__x86_64__)
		#define FIOQSIZE 0x5460
	#elif defined(__mips__) || defined(__mips64__)
		#define FIOQSIZE 0x6667
	#endif
#endif

long quotactl (int, const char *, qid_t, caddr_t);

#endif /* _QUOTA_ */
