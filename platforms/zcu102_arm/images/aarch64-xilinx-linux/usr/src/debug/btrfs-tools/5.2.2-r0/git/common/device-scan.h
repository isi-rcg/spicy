#ifndef __DEVICE_SCAN_H__
#define __DEVICE_SCAN_H__

#include "kerncompat.h"
#include "ioctl.h"

#define BTRFS_SCAN_MOUNTED	(1ULL << 0)
#define BTRFS_SCAN_LBLKID	(1ULL << 1)

#define BTRFS_UPDATE_KERNEL	1

#define BTRFS_ARG_UNKNOWN	0
#define BTRFS_ARG_MNTPOINT	1
#define BTRFS_ARG_UUID		2
#define BTRFS_ARG_BLKDEV	3
#define BTRFS_ARG_REG		4

#define SEEN_FSID_HASH_SIZE 256

struct btrfs_root;
struct btrfs_trans_handle;
struct seen_fsid;
struct DIR;

struct seen_fsid {
	u8 fsid[BTRFS_FSID_SIZE];
	struct seen_fsid *next;
	DIR *dirstream;
	int fd;
};

int btrfs_scan_devices(void);
int btrfs_register_one_device(const char *fname);
int btrfs_register_all_devices(void);
int btrfs_add_to_fsid(struct btrfs_trans_handle *trans,
		      struct btrfs_root *root, int fd, const char *path,
		      u64 device_total_bytes, u32 io_width, u32 io_align,
		      u32 sectorsize);
int btrfs_device_already_in_root(struct btrfs_root *root, int fd,
				 int super_offset);
int is_seen_fsid(u8 *fsid, struct seen_fsid *seen_fsid_hash[]);
int add_seen_fsid(u8 *fsid, struct seen_fsid *seen_fsid_hash[],
		int fd, DIR *dirstream);
void free_seen_fsid(struct seen_fsid *seen_fsid_hash[]);

#endif
