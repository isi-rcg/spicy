/* vi: set expandtab sw=4 sts=4: */
/* pkg_hash.h - the opkg package management system

   Steven M. Ayer

   Copyright (C) 2002 Compaq Computer Corporation

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2, or (at
   your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.
*/

#ifndef PKG_HASH_H
#define PKG_HASH_H

#include "pkg.h"
#include "pkg_src.h"
#include "pkg_dest.h"
#include "hash_table.h"

#ifdef __cplusplus
extern "C" {
#endif

enum fetch_type {
    INSTALLED,
    INSTALLED_HALF_INSTALLED,
    INSTALLED_TOBE_INSTALLED
};
typedef enum fetch_type fetch_type_t;

void pkg_hash_init(void);
void pkg_hash_deinit(void);

void pkg_hash_fetch_available(pkg_vec_t * available);

int pkg_hash_load_feeds(void);
int pkg_hash_load_status_files(void);

void hash_insert_pkg(pkg_t * pkg, int set_status);

abstract_pkg_t *ensure_abstract_pkg_by_name(const char *pkg_name);
void pkg_hash_fetch_all_installed(pkg_vec_t * installed, fetch_type_t constain);
pkg_t *pkg_hash_fetch_by_name_version_arch(const char *pkg_name,
                                           const char *version,
                                           const char *arch);
abstract_pkg_t *abstract_pkg_fetch_by_name(const char *pkg_name);
void abstract_pkgs_fetch_by_glob(const char *pkg_glob, abstract_pkg_vec_t *apkgs);
pkg_t *pkg_hash_fetch_best_installation_candidate(abstract_pkg_t * apkg,
                                                  int (*constraint_fcn)(pkg_t * pkg, void *data),
                                                  void *cdata, int prefer_installed, int quiet);
pkg_t *pkg_hash_fetch_best_installation_candidate_by_name(const char *name);
pkg_t *pkg_hash_fetch_installed_by_name(const char *pkg_name);
pkg_t *pkg_hash_fetch_installed_by_name_dest(const char *pkg_name,
                                             pkg_dest_t * dest);

void file_hash_remove(const char *file_name);
pkg_t *file_hash_get_file_owner(const char *file_name);
void file_hash_set_file_owner(const char *file_name, pkg_t * pkg);

#ifdef __cplusplus
}
#endif
#endif                          /* PKG_HASH_H */
