/*
 * Copyright (C) 2014 Red Hat, Inc.
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

#ifndef __DNF_ADVISORYPKG_H
#define __DNF_ADVISORYPKG_H

#include <solv/pool.h>
#include <glib-object.h>

#ifdef __cplusplus
namespace libdnf {
    struct AdvisoryPkg;
}
typedef struct libdnf::AdvisoryPkg DnfAdvisoryPkg;
#else
typedef struct AdvisoryPkg DnfAdvisoryPkg;
#endif

#ifdef __cplusplus
extern "C" {
#endif

void dnf_advisorypkg_free (DnfAdvisoryPkg *advisorypkg);
const char      *dnf_advisorypkg_get_name         (DnfAdvisoryPkg *advisorypkg);
const char      *dnf_advisorypkg_get_evr          (DnfAdvisoryPkg *advisorypkg);
const char      *dnf_advisorypkg_get_arch         (DnfAdvisoryPkg *advisorypkg);
const char      *dnf_advisorypkg_get_filename     (DnfAdvisoryPkg *advisorypkg);

int              dnf_advisorypkg_compare          (DnfAdvisoryPkg *left,
                                                   DnfAdvisoryPkg *right);
gboolean         dnf_advisorypkg_compare_solvable (DnfAdvisoryPkg *advisorypkg,
                                                   Pool           *pool,
                                                   Solvable       *s);
#ifdef __cplusplus
}
#endif

#endif /* __DNF_ADVISORYPKG_H */
