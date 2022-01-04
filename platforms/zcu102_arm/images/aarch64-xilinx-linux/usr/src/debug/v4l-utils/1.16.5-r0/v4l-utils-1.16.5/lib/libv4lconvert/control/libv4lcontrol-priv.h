/*
#             (C) 2008-2009 Elmar Kleijn <elmar_kleijn@hotmail.com>
#             (C) 2008-2009 Sjoerd Piepenbrink <need4weed@gmail.com>
#             (C) 2008-2009 Radjnies Bhansingh <radjnies@gmail.com>
#             (C) 2008-2009 Hans de Goede <hdegoede@redhat.com>

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; either version 2.1 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA  02110-1335  USA
 */

#ifndef __LIBV4LCONTROL_PRIV_H
#define __LIBV4LCONTROL_PRIV_H

#include "libv4l-plugin.h"

#define V4LCONTROL_SHM_SIZE 4096

#define V4LCONTROL_SUPPORTS_NEXT_CTRL 0x01
#define V4LCONTROL_MEMORY_IS_MALLOCED 0x02

struct v4lcontrol_flags_info;

struct v4lcontrol_data {
	int fd;                   /* Device fd */
	int bandwidth;            /* Connection bandwidth (0 = unknown) */
	int flags;                /* Flags for this device */
	int priv_flags;           /* Internal use only flags */
	int controls;             /* Which controls to use for this device */
	unsigned int *shm_values; /* shared memory control value store */
	unsigned int old_values[V4LCONTROL_COUNT]; /* for controls_changed() */
	const struct v4lcontrol_flags_info *flags_info;
	void *dev_ops_priv;
	const struct libv4l_dev_ops *dev_ops;
};

struct v4lcontrol_flags_info {
	unsigned short vendor_id;
	unsigned short product_id;
	unsigned short product_mask;
	const char *dmi_board_vendor;
	const char *dmi_board_name;
	/* We could also use the USB manufacturer and product strings some devices have
	   const char *manufacturer;
	   const char *product; */
	int flags;
	int default_gamma;
	/* Some seldom used dmi strings (for notebooks with bogus info in the board
	   entries, but usefull info elsewhere). We keep this at the end as to not
	   polute the initalizers for the normal case. */
	/* System (product) vendor / name */
	const char *dmi_system_vendor;
	const char *dmi_system_name;
	/* Board and System versions */
	const char *dmi_board_version;
	const char *dmi_system_version;
};

struct v4lcontrol_usb_id {
	unsigned short vendor_id;
	unsigned short product_id;
};

struct v4lcontrol_upside_down_table {
	const char **board_vendor;
	const char **board_name;
	const struct v4lcontrol_usb_id *camera_id;
};

#endif
