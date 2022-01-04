/*
 * Copyright (C) 2012-2013 Red Hat, Inc.
 * Copyright (C) 2015 Richard Hughes <richard@hughsie.com>
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

#ifndef __HY_PACKAGE_INTERNAL_H
#define __HY_PACKAGE_INTERNAL_H

#include <memory>
#include <vector>

#include "hy-package.h"
#include "dnf-sack.h"
#include "sack/changelog.hpp"

Pool        *dnf_package_get_pool       (DnfPackage *pkg);
DnfSack     *dnf_package_get_sack       (DnfPackage *pkg);
std::vector<libdnf::Changelog>   dnf_package_get_changelogs (DnfPackage *pkg);

#endif // __HY_PACKAGE_INTERNAL_H
