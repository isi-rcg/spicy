/*
 *
 *	Headerfile of quota interactions with system - filenames, fstab...
 *
 */

#ifndef GUARD_QUOTASYS_H
#define GUARD_QUOTASYS_H

#include <sys/types.h>
#include <inttypes.h>
#include "mntopt.h"
#include "quota.h"
#include "common.h"

#define MAXNAMELEN 64		/* Maximal length of user/group name */
#define MAXTIMELEN 40		/* Maximal length of time string */
#define MAXNUMLEN 32		/* Maximal length of number */

/* Flags for formatting time */
#define TF_ROUND 0x1		/* Should be printed time rounded? */

/* Flags for IO initialization */
#define IOI_READONLY	0x1	/* Only readonly access */
#define IOI_INITSCAN	0x2	/* Prepare handle for scanning dquots */
#define IOI_NFS_MIXED_PATHS	0x4	/* Trim leading / from NFSv4 mountpoints */

/* Interface versions */
#define IFACE_VFSOLD 1
#define IFACE_VFSV0 2
#define IFACE_GENERIC 3

/* Path to export table of NFS daemon */
#define NFSD_XTAB_PATH "/var/lib/nfs/etab"

/* Supported kernel interface */
extern int kernel_iface;

struct mount_entry {
	char *me_type;		/* Type of filesystem for given entry */
	char *me_opts;		/* Options of filesystem */
	dev_t me_dev;		/* Device filesystem is mounted on */
	ino_t me_ino;		/* Inode number of root of filesystem */
	const char *me_devname;	/* Name of device (after pass through get_device_name()) */
	const char *me__dir;	/* One mountpoint of a filesystem (strdup()ed) */
	const char *me_dir;	/* Current mountpoint of a filesystem to process */
	int me_qfmt[MAXQUOTAS];	/* Detected quota formats */
};

struct fs_project {
	qid_t pr_id;
	char *pr_name;
};

/*
 *	Exported functions
 */
/* Check whether type is one of the NFS filesystems */
int nfs_fstype(char *);
/* Quota file is treated as metadata? */
int meta_qf_fstype(char *type);

/* Convert quota type to written form */
char *type2name(int);

/* Rewind /etc/projid to the beginning */
void setprent(void);

/* Close /etc/projid file */
void endprent(void);

/* Get next entry in /etc/projid */
struct fs_project *getprent(void);

/* Convert username to uid */
uid_t user2uid(char *, int flag, int *err);

/* Convert groupname to gid */
gid_t group2gid(char *, int flag, int *err);

/* Convert project name to project id */
qid_t project2pid(char *name, int flag, int *err);

/* Convert user/groupname to id */
int name2id(char *name, int qtype, int flag, int *err);

/* Convert uid to username */
int uid2user(uid_t, char *);

/* Convert gid to groupname */
int gid2group(gid_t, char *);

/* Convert project id to name */
int pid2project(qid_t, char *);

/* Convert id to user/group name */
int id2name(int id, int qtype, char *buf);

/* Possible default passwd handling */
#define PASSWD_FILES 0
#define PASSWD_DB 1
/* Parse /etc/nsswitch.conf and return type of default passwd handling */
int passwd_handling(void);

/* Convert quota format name to number */
int name2fmt(char *str);

/* Convert quota format number to name */
char *fmt2name(int fmt);

/* Convert output format name to number */
int name2ofmt(char *str);

/* Convert output format number to name */
char *ofmt2name(int fmt);

/* Convert utility to kernel format numbers */
int util2kernfmt(int fmt);

/* Convert time difference between given time and current time to printable form */
void difftime2str(time_t, char *);

/* Round difference of two time_t values into int32_t */
int32_t difftime2net(time_t later, time_t sooner);

/* Convert time to printable form */
void time2str(time_t, char *, int);

/* Convert number and units to time in seconds */
int str2timeunits(time_t, char *, time_t *);

/* Convert number in quota blocks to short printable form */
void space2str(qsize_t, char *, enum s2s_unit);

/* Convert block number with unit from string to quota blocks */
const char *str2space(const char *string, qsize_t *space);

/* Convert number to short printable form */
void number2str(long long, char *, enum s2s_unit);

/* Convert inode number with unit from string to quota inodes. */
const char *str2number(const char *string, qsize_t *inodes);

/* Return pointer to given mount option in mount option string */
char *str_hasmntopt(const char *optstring, const char *opt);

/* Check to see if particular quota type is configured */
static inline int me_hasquota(struct mount_entry *mnt, int type)
{
	return mnt->me_qfmt[type] >= 0;
}

/* Flags for get_qf_name() */
#define NF_EXIST  1	/* Check whether file exists */
#define NF_FORMAT 2	/* Check whether file is in proper format */
/* Get quotafile name for given entry */
int get_qf_name(struct mount_entry *mnt, int type, int fmt, int flags, char **filename);

/* Detect newest quota format with existing file */
int detect_quota_files(struct mount_entry *mnt, int type, int fmt);

/* Create NULL-terminated list of handles for quotafiles for given mountpoints */
struct quota_handle **create_handle_list(int count, char **mntpoints, int type, int fmt,
					 int ioflags, int mntflags);
/* Dispose given list of handles */
int dispose_handle_list(struct quota_handle **hlist);

/* Check whether given device name matches quota handle device */
int devcmp_handle(const char *dev, struct quota_handle *h);

/* Check whether two quota handles have same device */
int devcmp_handles(struct quota_handle *a, struct quota_handle *b);

/* Check kernel supported quotafile format */
void init_kernel_interface(void);

/*
 * Check whether is quota turned on on given device for given type. This
 * works for XFS for all kernels and for other filesystems since kernel 4.1.
 */
int kern_quota_state_xfs(const char *dev, int type);

/* Check whether is quota turned on on given device for given type */
int kern_quota_on(struct mount_entry *mnt, int type, int fmt);

/* Return whether kernel is able to handle given format */
int kern_qfmt_supp(int fmt);

/* Flags for init_mounts_scan() */
#define MS_NO_MNTPOINT 0x01	/* Specified directory needn't be mountpoint */
#define MS_NO_AUTOFS 0x02	/* Ignore autofs mountpoints */
#define MS_QUIET 0x04		/* Be quiet with error reporting */
#define MS_LOCALONLY 0x08	/* Ignore nfs mountpoints */
#define MS_XFS_DISABLED 0x10	/* Return also XFS mountpoints with quota disabled */
#define MS_NFS_ALL 0x20		/* Don't filter NFS mountpoints on the same device */

/* Initialize mountpoints scan */
int init_mounts_scan(int dcnt, char **dirs, int flags);

/* Return next mountpoint for scan */
struct mount_entry *get_next_mount(void);

/* Free all structures associated with mountpoints scan */
void end_mounts_scan(void);

/* Quota output formats */
#define QOF_ERROR	-1
#define QOF_DEFAULT	0
#define QOF_CSV		1
#define QOF_XML		2

#endif /* GUARD_QUOTASYS_H */
