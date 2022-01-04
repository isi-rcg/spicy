/** @file
 * @brief usbhid-dump - device
 *
 * Copyright (C) 2010-2011 Nikolai Kondrashov
 *
 * This file is part of usbhid-dump.
 *
 * Usbhid-dump is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Usbhid-dump is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with usbhid-dump; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * @author Nikolai Kondrashov <spbnick@gmail.com>
 *
 * @(#) $Id$
 */

#ifndef __UHD_DEV_H__
#define __UHD_DEV_H__

#include <stdbool.h>
#include "uhd/libusb.h"

#ifdef __cplusplus
extern "C" {
#endif

/** usbhid-dump device */
typedef struct uhd_dev uhd_dev;

struct uhd_dev {
    uhd_dev                *next;       /**< Next device in the list */
    libusb_device_handle   *handle;     /**< Handle */
};

/**
 * Check if a device is valid.
 *
 * @param dev Device to check.
 *
 * @return True if the device is valid, false otherwise.
 */
extern bool uhd_dev_valid(const uhd_dev *dev);

/**
 * Open a device.
 *
 * @param lusb_dev  Libusb device.
 * @param pdev      Location for the opened device pointer.
 *
 * @return Libusb error code.
 */
extern enum libusb_error uhd_dev_open(libusb_device    *lusb_dev,
                                      uhd_dev         **pdev);

/**
 * Close a device.
 *
 * @param dev   The device to close.
 */
extern void uhd_dev_close(uhd_dev *dev);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __UHD_DEV_H__ */
