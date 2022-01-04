/* librepo - A library providing (libcURL like) API to downloading repository
 * Copyright (C) 2012  Tomas Mlcoch
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef __LR_VERSION_H__
#define __LR_VERSION_H__

#include <glib.h>

G_BEGIN_DECLS

/** \defgroup   version   Library version constatnts and check macros
 *  \addtogroup version
 *  @{
 */

#define LR_VERSION_MAJOR 1  /*!< Major Librepo version */
#define LR_VERSION_MINOR 10  /*!< Minor Librepo version */
#define LR_VERSION_PATCH 5  /*!< Patch Librepo version */
#define LR_VERSION  "1.10.5" /*!< Version string */

/** Macro for version check.
 * @param major     Major version
 * @param minor     Minor version
 * @param patch     Patch version
 * @return          True if current Librepo version is higher or equal
 */
#define LR_VERSION_CHECK(major,minor,patch)    \
    (LR_VERSION_MAJOR > (major) || \
     (LR_VERSION_MAJOR == (major) && LR_VERSION_MINOR > (minor)) || \
     (LR_VERSION_MAJOR == (major) && LR_VERSION_MINOR == (minor) && \
      LR_VERSION_PATCH >= (patch)))

/** @} */

G_END_DECLS

#endif
