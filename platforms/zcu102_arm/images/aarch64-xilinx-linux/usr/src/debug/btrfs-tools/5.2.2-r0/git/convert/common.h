/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License v2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 021110-1307, USA.
 */

/*
 * Defines and function declarations for users of the mkfs API, no internal
 * defintions.
 */

#ifndef __BTRFS_CONVERT_COMMON_H__
#define __BTRFS_CONVERT_COMMON_H__

#include "kerncompat.h"
#include "common-defs.h"
#include "extent-cache.h"

struct btrfs_mkfs_config;

struct btrfs_convert_context {
	u32 blocksize;
	u64 first_data_block;
	u64 block_count;
	u64 inodes_count;
	u64 free_inodes_count;
	u64 total_bytes;
	char *volume_name;
	const struct btrfs_convert_operations *convert_ops;

	/* The accurate used space of old filesystem */
	struct cache_tree used_space;

	/* Batched ranges which must be covered by data chunks */
	struct cache_tree data_chunks;

	/* Free space which is not covered by data_chunks */
	struct cache_tree free_space;

	void *fs_data;
};

int make_convert_btrfs(int fd, struct btrfs_mkfs_config *cfg,
			      struct btrfs_convert_context *cctx);

#endif
