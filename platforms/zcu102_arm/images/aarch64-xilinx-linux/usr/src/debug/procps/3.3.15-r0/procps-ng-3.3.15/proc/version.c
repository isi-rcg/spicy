/*
 * libprocps - Library to read proc filesystem
 *
 * Copyright (C) 1995 Martin Schulze <joey@infodrom.north.de>
 * Copyright (C) 1996 Charles Blake <cblake@bbn.com>
 * Copyright (C) 2003 Albert Cahalan
 * Copyright (C) 2015 Craig Small <csmall@enc.com.au>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include <errno.h>
#include <stdio.h>
#include "procps-private.h"
#include "version.h"

#define PROCFS_OSRELEASE "/proc/sys/kernel/osrelease"

/*
 * procps_linux_version
 *
 * Return the current running Linux version release as shown in
 * the procps filesystem.
 *
 * There are three ways you can get OS release:
 *  1) /proc/sys/kernel/osrelease - returns correct version of procfs
 *  2) /proc/version - returns version of kernel e.g. BSD this is wrong
 *  3) uname and uts.release - same as /proc/version field #3
 *
 * Returns: version as an integer
 * Negative value means an error
 */
PROCPS_EXPORT int procps_linux_version(void)
{
    FILE *fp;
    char buf[256];
    unsigned int x = 0, y = 0, z = 0;
    int version_string_depth;

    if ((fp = fopen(PROCFS_OSRELEASE, "r")) == NULL)
	return -errno;
    if (fgets(buf, 256, fp) == NULL) {
	fclose(fp);
	return -EIO;
    }
    fclose(fp);
    version_string_depth = sscanf(buf, "%u.%u.%u", &x, &y, &z);
    if ((version_string_depth < 2) ||		 /* Non-standard for all known kernels */
       ((version_string_depth < 3) && (x < 3))) /* Non-standard for 2.x.x kernels */
	return -ERANGE;
    return LINUX_VERSION(x,y,z);
}
