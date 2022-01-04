/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2014-2015 Richard Hughes <richard@hughsie.com>
 *
 * Licensed under the GNU Lesser General Public License Version 2.1
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 */

/**
 * SECTION:dnf-version
 * @short_description: Preprocessor macros for the libdnf version
 * @include: libdnf.h
 * @stability: Stable
 *
 * These functions are used in client code to conditionalize compilation
 * depending on the version of libdnf headers installed.
 */

#ifndef __DNF_VERSION_H
#define __DNF_VERSION_H

/* compile time version
 */
#define LIBDNF_MAJOR_VERSION				(0)
#define LIBDNF_MINOR_VERSION				(28)
#define LIBDNF_MICRO_VERSION				(1)

/* check whether a As version equal to or greater than
 * major.minor.micro.
 */
#define DNF_CHECK_VERSION(major,minor,micro)    \
    (LIBDNF_MAJOR_VERSION > (major) || \
     (LIBDNF_MAJOR_VERSION == (major) && LIBDNF_MINOR_VERSION > (minor)) || \
     (LIBDNF_MAJOR_VERSION == (major) && LIBDNF_MINOR_VERSION == (minor) && \
      LIBDNF_MICRO_VERSION >= (micro)))

#endif /* __DNF_VERSION_H */
