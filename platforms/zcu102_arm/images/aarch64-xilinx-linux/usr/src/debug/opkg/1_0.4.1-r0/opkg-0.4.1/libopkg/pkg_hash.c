/* vi: set expandtab sw=4 sts=4: */
/* opkg_hash.c - the opkg package management system

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

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <fnmatch.h>

#include "hash_table.h"
#include "release.h"
#include "pkg.h"
#include "opkg_message.h"
#include "opkg_archive.h"
#include "pkg_vec.h"
#include "pkg_hash.h"
#include "parse_util.h"
#include "pkg_parse.h"
#include "opkg_utils.h"
#include "sprintf_alloc.h"
#include "file_util.h"
#include "xfuncs.h"

static void free_pkgs(const char *key, void *entry, void *data)
{
    unsigned int i;
    abstract_pkg_t *ab_pkg;

    /* Each entry in the hash table is an abstract package, which contains
     * a list of packages that provide the abstract package.
     */

    ab_pkg = (abstract_pkg_t *) entry;

    if (ab_pkg->pkgs) {
        for (i = 0; i < ab_pkg->pkgs->len; i++) {
            pkg_deinit(ab_pkg->pkgs->pkgs[i]);
            free(ab_pkg->pkgs->pkgs[i]);
        }
    }

    abstract_pkg_vec_free(ab_pkg->depended_upon_by);
    abstract_pkg_vec_free(ab_pkg->provided_by);
    abstract_pkg_vec_free(ab_pkg->replaced_by);
    pkg_vec_free(ab_pkg->pkgs);
    free(ab_pkg->name);
    free(ab_pkg);
}

static int pkg_hash_add_from_file(const char *file_name, pkg_src_t * src,
                           pkg_dest_t * dest, int is_status_file)
{
    pkg_t *pkg;
    FILE *fp = NULL;
    char *buf = NULL, *bp = NULL;
    const size_t len = 4096;
    int ret = 0;

    if (opkg_config->compress_list_files  && !is_status_file) {
        struct opkg_ar *ar;
        size_t size;

        ar = ar_open_compressed_file(file_name);
        if (!ar)
            return -1;

        FILE *mfp = open_memstream(&bp, &size);

        if (ar_copy_to_stream(ar, mfp) < 0) {
            opkg_perror(ERROR, "Failed to open %s", file_name);
            ret = -1;
            goto cleanup;
        }
        fclose(mfp);

        fp = fmemopen(bp, size, "r");
        if (fp == NULL) {
            opkg_perror(ERROR, "Failed to open memory buffer: %s\n", strerror(errno));
            ret = -1;
            goto cleanup;
        }
    } else {
        fp = fopen(file_name, "r");
        if (fp == NULL) {
            opkg_perror(ERROR, "Failed to open %s", file_name);
            ret = -1;
            goto cleanup;
        }
    }

    /* Remove UTF-8 BOM if present */
    if (!(getc(fp) == 0xEF && getc(fp) == 0xBB && getc(fp) == 0xBF))
        rewind(fp);

    buf = xmalloc(len);

    do {
        pkg = pkg_new();
        pkg->src = src;
        pkg->dest = dest;

        ret = parse_from_stream_nomalloc(pkg_parse_line, pkg, fp, 0, &buf, len);
        if (pkg->name == NULL) {
            /* probably just a blank line */
            ret = 1;
        }
        if (ret) {
            pkg_deinit(pkg);
            free(pkg);
            if (ret == -1)
                break;
            if (ret == 1)
                /* Probably a blank line, continue parsing. */
                ret = 0;
            continue;
        }

        if (!pkg->architecture) {
            char *version_str = pkg_version_str_alloc(pkg);
            opkg_msg(NOTICE,
                     "Package %s version %s has no "
                     "valid architecture, ignoring.\n", pkg->name, version_str);
            free(version_str);
            pkg_deinit(pkg);
            free(pkg);
            continue;
        }
        if (!pkg->arch_priority) {
            char *version_str = pkg_version_str_alloc(pkg);
            opkg_msg(DEBUG,
                     "Package %s version %s is built for architecture %s "
                     "which cannot be installed here, ignoring.\n", pkg->name,
                     version_str, pkg->architecture);
            free(version_str);
            pkg_deinit(pkg);
            free(pkg);
            continue;
        }

        hash_insert_pkg(pkg, is_status_file);

    } while (!feof(fp));

cleanup:
    free(buf);
    if (fp)
        fclose(fp);
    free(bp);

    return ret;
}

static int dist_hash_add_from_file(pkg_src_t * dist)
{
    nv_pair_list_elt_t *l;
    char *list_file, *subname;
    int r;

    list_for_each_entry(l, &opkg_config->arch_list.head, node) {
        nv_pair_t *nv = (nv_pair_t *) l->data;
        sprintf_alloc(&subname, "%s-%s", dist->name, nv->name);
        sprintf_alloc(&list_file, "%s/%s", opkg_config->lists_dir, subname);

        if (file_exists(list_file)) {
            r = pkg_hash_add_from_file(list_file, dist, NULL, 0);
            if (r != 0) {
                free(list_file);
                return -1;
            }
            char *subpath, *distribution, *component;
            distribution = strtok(dist->name, "-");
            component = strtok(NULL, "-");
            sprintf_alloc(&subpath, "dists/%s/%s/binary-%s", distribution, component, nv->name);
            pkg_src_list_append(&opkg_config->pkg_src_list, subname,
                                dist->value, NULL, subpath, 0);
        }

        free(list_file);
    }

    return 0;
}

void pkg_hash_init(void)
{
    hash_table_init("pkg-hash", &opkg_config->pkg_hash,
                    OPKG_CONF_DEFAULT_HASH_LEN);
}

void pkg_hash_deinit(void)
{
    hash_table_foreach(&opkg_config->pkg_hash, free_pkgs, NULL);
    hash_table_deinit(&opkg_config->pkg_hash);
}

/*
 * Load in feed files from the cached "src" and/or "src/gz" locations.
 */
int pkg_hash_load_feeds(void)
{
    pkg_src_list_elt_t *iter;
    pkg_src_t *src, *subdist;
    char *list_file;
    int r;

    opkg_msg(INFO, "\n");

    for (iter = void_list_first(&opkg_config->dist_src_list); iter;
            iter = void_list_next(&opkg_config->dist_src_list, iter)) {

        src = (pkg_src_t *) iter->data;

        sprintf_alloc(&list_file, "%s/%s%s", opkg_config->lists_dir, src->name,
                      opkg_config->compress_list_files ? ".gz" : "");
        if (file_exists(list_file)) {
            unsigned int i;
            release_t *release = release_new();
            r = release_init_from_file(release, list_file);
            if (r != 0) {
                free(list_file);
                return -1;
            }

            unsigned int ncomp;
            const char **comps = release_comps(release, &ncomp);
            subdist = (pkg_src_t *) xmalloc(sizeof(pkg_src_t));
            memcpy(subdist, src, sizeof(pkg_src_t));

            for (i = 0; i < ncomp; i++) {
                subdist->name = NULL;
                sprintf_alloc(&subdist->name, "%s-%s", src->name, comps[i]);
                r = dist_hash_add_from_file(subdist);
                if (r != 0) {
                    free(subdist->name);
                    free(subdist);
                    free(list_file);
                    return -1;
                }
            }
            free(subdist->name);
            free(subdist);
        }
        free(list_file);
    }

    for (iter = void_list_first(&opkg_config->pkg_src_list); iter;
            iter = void_list_next(&opkg_config->pkg_src_list, iter)) {

        src = (pkg_src_t *) iter->data;

        sprintf_alloc(&list_file, "%s/%s%s", opkg_config->lists_dir, src->name,
                      opkg_config->compress_list_files ? ".gz" : "" );

        if (file_exists(list_file)) {
            r = pkg_hash_add_from_file(list_file, src, NULL, 0);
            if (r != 0) {
                free(list_file);
                return -1;
            }
        }
        free(list_file);
    }

    return 0;
}

/*
 * Load in status files from the configured "dest"s.
 */
int pkg_hash_load_status_files(void)
{
    pkg_dest_list_elt_t *iter;
    pkg_dest_t *dest;

    opkg_msg(INFO, "\n");

    for (iter = void_list_first(&opkg_config->pkg_dest_list); iter;
            iter = void_list_next(&opkg_config->pkg_dest_list, iter)) {

        dest = (pkg_dest_t *) iter->data;

        if (file_exists(dest->status_file_name)) {
            int r = pkg_hash_add_from_file(dest->status_file_name, NULL, dest,
                                           1);
            if (r != 0)
                return -1;
        }
    }

    return 0;
}

abstract_pkg_t *abstract_pkg_fetch_by_name(const char *pkg_name)
{
    return (abstract_pkg_t *) hash_table_get(&opkg_config->pkg_hash, pkg_name);
}

void abstract_pkgs_fetch_by_glob(const char *pkg_glob, abstract_pkg_vec_t *apkgs)
{
    unsigned int i;
    hash_table_t *hash = &opkg_config->pkg_hash;

    if (!hash)
        return;

    for (i = 0; i < hash->n_buckets; i++) {
        hash_entry_t *hash_entry = (hash->entries + i);

        do {
            if (hash_entry->key) {
                if (!fnmatch(pkg_glob, hash_entry->key, 0))
                    abstract_pkg_vec_insert(apkgs, hash_entry->data);
            }
        } while ((hash_entry = hash_entry->next));
     }
}

pkg_t *pkg_hash_fetch_best_installation_candidate(abstract_pkg_t * apkg,
                                                  int (*constraint_fcn) (pkg_t * pkg, void *cdata),
                                                  void *cdata, int prefer_installed, int quiet)
{
    unsigned int i, j;
    unsigned int nprovides = 0;
    pkg_vec_t *matching_pkgs;
    abstract_pkg_vec_t *matching_apkgs;
    abstract_pkg_vec_t *provided_apkg_vec;
    abstract_pkg_t **provided_apkgs;
    abstract_pkg_vec_t *providers;
    pkg_t *latest_installed_parent = NULL;
    pkg_t *latest_matching = NULL;
    pkg_t *priorized_matching = NULL;
    pkg_t *held_pkg = NULL;
    pkg_t *prefer_pkg = NULL;
    pkg_t *good_pkg_by_name = NULL;

    if (!apkg || !apkg->provided_by || !apkg->provided_by->len)
        return NULL;

    matching_pkgs = pkg_vec_alloc();
    matching_apkgs = abstract_pkg_vec_alloc();
    providers = abstract_pkg_vec_alloc();

    opkg_msg(DEBUG, "Best installation candidate for %s:\n", apkg->name);

    provided_apkg_vec = apkg->provided_by;
    nprovides = provided_apkg_vec->len;
    provided_apkgs = provided_apkg_vec->pkgs;
    if (nprovides > 1)
        opkg_msg(DEBUG, "apkg=%s nprovides=%d.\n", apkg->name, nprovides);

    /* accumulate all the providers */
    for (i = 0; i < nprovides; i++) {
        abstract_pkg_t *provider_apkg = provided_apkgs[i];

        /* Don't double insert packages. */
        if (abstract_pkg_vec_contains(providers, provider_apkg))
            continue;

        opkg_msg(DEBUG, "Adding %s to providers.\n", provider_apkg->name);
        abstract_pkg_vec_insert(providers, provider_apkg);
    }
    nprovides = providers->len;

    for (i = 0; i < nprovides; i++) {
        abstract_pkg_t *provider_apkg = abstract_pkg_vec_get(providers, i);
        abstract_pkg_t *replacement_apkg = NULL;
        pkg_vec_t *vec;

        if (provider_apkg->replaced_by && provider_apkg->replaced_by->len) {
            replacement_apkg = provider_apkg->replaced_by->pkgs[0];
            if (provider_apkg->replaced_by->len > 1) {
                opkg_msg(NOTICE,
                         "Multiple replacers for %s, "
                         "using first one (%s).\n", provider_apkg->name,
                         replacement_apkg->name);
            }
        }

        if (replacement_apkg)
            opkg_msg(DEBUG, "replacement_apkg=%s for provider_apkg=%s.\n",
                     replacement_apkg->name, provider_apkg->name);

        if (replacement_apkg && (replacement_apkg != provider_apkg)) {
            if (abstract_pkg_vec_contains(providers, replacement_apkg))
                continue;
            else
                abstract_pkg_vec_insert(providers, replacement_apkg);
        }

        vec = provider_apkg->pkgs;
        if (!vec) {
            opkg_msg(DEBUG, "No pkgs for provider_apkg %s.\n",
                     provider_apkg->name);
            continue;
        }

        for (j = 0; j < vec->len; j++) {
            pkg_t *maybe = vec->pkgs[j];

            /* If package is installed and held, add it to the matching list
             * regardless of whether it satisfies our other checks. This
             * ensures that a held package won't be removed due to edge cases
             * like the forced installation of a new package with dependency
             * on a later version of the held package.
             */
            int installed_and_held = (maybe->state_status == SS_INSTALLED
                        || maybe->state_status == SS_UNPACKED)
                    && (maybe->state_flag & SF_HOLD);
            if (installed_and_held)
                goto add_matching_pkg;

            /* Ensure that the package meets the specified constraint. */
            if (constraint_fcn && !constraint_fcn(maybe, cdata)) {
                opkg_msg(DEBUG,
                         "Not selecting %s %s due to unmatched constraint.\n",
                         maybe->name, maybe->version);
                continue;
            }

            /* Ensure that installing this package won't break the
             * dependencies of packages which are already installed, unless
             * force_depends is set.
             */
            if (pkg_breaks_reverse_dep(maybe) && !opkg_config->force_depends) {
                opkg_msg(NOTICE,
                         "Not selecting %s %s as installing it would break "
                         "existing dependencies.\n",
                         maybe->name, maybe->version);
                continue;
            }

            /* now check for supported architecture */
            opkg_msg(DEBUG, "%s arch=%s arch_priority=%d version=%s.\n",
                     maybe->name, maybe->architecture, maybe->arch_priority,
                     maybe->version);
            if (maybe->arch_priority <= 0) {
                opkg_msg(NOTICE,
                         "Not selecting %s %s due to incompatible architecture.\n",
                         maybe->name, maybe->version);
                continue;
            }

 add_matching_pkg:
            /* We make sure not to add the same package twice. Need to search
             * for the reason why they show up twice sometimes. */
            if (!pkg_vec_contains(matching_pkgs, maybe)) {
                abstract_pkg_vec_insert(matching_apkgs, maybe->parent);
                pkg_vec_insert(matching_pkgs, maybe);
            }
        }
    }

    if (matching_pkgs->len < 1) {
        pkg_vec_free(matching_pkgs);
        abstract_pkg_vec_free(matching_apkgs);
        abstract_pkg_vec_free(providers);
        return NULL;
    }

    if (matching_pkgs->len > 1)
        pkg_vec_sort(matching_pkgs, pkg_name_version_and_architecture_compare);
    if (matching_apkgs->len > 1)
        abstract_pkg_vec_sort(matching_apkgs, abstract_pkg_name_compare);

    for (i = 0; i < matching_pkgs->len; i++) {
        pkg_t *matching = matching_pkgs->pkgs[i];
        /* Set good_pkg_by_name if the package name matches the originally
         * requested name (which will be the apkg name).
         */
        if (strcmp(matching->name, apkg->name) == 0) {
            opkg_msg(DEBUG, "Candidate: %s %s.\n", matching->name,
                     matching->version);
            /* It has been provided by hand, so it is what user want */
            if (matching->provided_by_hand == 1) {
                good_pkg_by_name = matching;
                break;
            }
            /* Respect to the arch priorities when given alternatives */
            if (good_pkg_by_name && opkg_config->prefer_arch_to_version) {
                if (matching->arch_priority >= good_pkg_by_name->arch_priority) {
                    good_pkg_by_name = matching;
                    opkg_msg(DEBUG, "%s %s wins by priority.\n", matching->name,
                             matching->version);
                } else
                    opkg_msg(DEBUG, "%s %s wins by priority.\n",
                             good_pkg_by_name->name, good_pkg_by_name->version);
            } else if (good_pkg_by_name && prefer_installed) {
                int is_installed = good_pkg_by_name->state_status == SS_INSTALLED
                                   || good_pkg_by_name->state_status == SS_UNPACKED;
                if (!is_installed)
                    good_pkg_by_name = matching;
            } else
                good_pkg_by_name = matching;
        }
    }

    for (i = 0; i < matching_pkgs->len; i++) {
        pkg_t *matching = matching_pkgs->pkgs[i];
        latest_matching = matching;
        int is_installed = matching->parent->state_status == SS_INSTALLED
                || matching->parent->state_status == SS_UNPACKED;
        if (is_installed)
            latest_installed_parent = matching;
        if (matching->state_flag & SF_HOLD) {
            if (held_pkg)
                opkg_msg(NOTICE,
                         "Multiple packages (%s %s and %s %s) providing"
                         " same name marked HOLD. " "Using latest.\n",
                         held_pkg->name, held_pkg->version, matching->name,
                         matching->version);
            held_pkg = matching;
        }
        if (matching->state_flag & SF_PREFER) {
            if (prefer_pkg)
                opkg_msg(NOTICE,
                         "Multiple packages (%s %s and %s %s) providing"
                         " same name marked PREFER. " "Using latest.\n",
                         prefer_pkg->name, prefer_pkg->version, matching->name,
                         matching->version);
            prefer_pkg = matching;
        }
    }

    int not_found = !good_pkg_by_name && !held_pkg && !latest_installed_parent;
    if (not_found && matching_apkgs->len > 1 && !quiet) {
        int prio = 0;
        for (i = 0; i < matching_pkgs->len; i++) {
            pkg_t *matching = matching_pkgs->pkgs[i];
            if (matching->arch_priority > prio) {
                priorized_matching = matching;
                prio = matching->arch_priority;
                opkg_msg(DEBUG, "Match %s with priority %i.\n", matching->name,
                         prio);
            }
        }
    }

    if (opkg_config->verbosity >= INFO && matching_apkgs->len > 1) {
        opkg_msg(INFO, "%d matching pkgs for apkg=%s:\n", matching_pkgs->len,
                 apkg->name);
        for (i = 0; i < matching_pkgs->len; i++) {
            pkg_t *matching = matching_pkgs->pkgs[i];
            opkg_msg(INFO, "%s %s %s\n", matching->name, matching->version,
                     matching->architecture);
        }
    }

    pkg_vec_free(matching_pkgs);
    abstract_pkg_vec_free(matching_apkgs);
    abstract_pkg_vec_free(providers);

    if (held_pkg) {
        if (prefer_pkg) {
            opkg_msg(NOTICE,
                     "Ignoring preferred package %s %s due to held package %s %s.\n",
                     prefer_pkg->name, prefer_pkg->version, held_pkg->name,
                     held_pkg->version);
        } else {
            opkg_msg(INFO, "Using held package %s %s.\n", held_pkg->name,
                     held_pkg->version);
        }
        return held_pkg;
    }
    if (prefer_pkg) {
        opkg_msg(INFO, "Using prefered package %s %s.\n", prefer_pkg->name,
                 prefer_pkg->version);
        return prefer_pkg;
    }
    if (good_pkg_by_name) {     /* We found a good candidate, we will install it */
        return good_pkg_by_name;
    }
    if (latest_installed_parent) {
        opkg_msg(INFO, "Using latest version of installed package %s.\n",
                 latest_installed_parent->name);
        return latest_installed_parent;
    }
    if (priorized_matching) {
        opkg_msg(INFO, "Using priorized matching %s %s %s.\n",
                 priorized_matching->name, priorized_matching->version,
                 priorized_matching->architecture);
        return priorized_matching;
    }
    if (latest_matching) {
        opkg_msg(INFO, "Using latest matching %s %s %s.\n",
                 latest_matching->name, latest_matching->version,
                 latest_matching->architecture);
        return latest_matching;
    }
    opkg_msg(INFO, "No matching pkg for '%s'.\n", apkg->name);
    return NULL;
}

static pkg_vec_t *pkg_vec_fetch_by_name(const char *pkg_name)
{
    abstract_pkg_t *ab_pkg;

    ab_pkg = abstract_pkg_fetch_by_name(pkg_name);
    if (!ab_pkg)
        return NULL;

    if (ab_pkg->pkgs)
        return ab_pkg->pkgs;

    if (ab_pkg->provided_by) {
        abstract_pkg_t *abpkg = abstract_pkg_vec_get(ab_pkg->provided_by, 0);
        if (abpkg != NULL)
            return abpkg->pkgs;
        else
            return ab_pkg->pkgs;
    }

    return NULL;
}

pkg_t *pkg_hash_fetch_best_installation_candidate_by_name(const char *name)
{
    abstract_pkg_t *apkg = NULL;

    apkg = abstract_pkg_fetch_by_name(name);
    if (!apkg)
        return NULL;

    return pkg_hash_fetch_best_installation_candidate(apkg, NULL, NULL, 0, 0);
}

pkg_t *pkg_hash_fetch_by_name_version_arch(const char *pkg_name,
                                           const char *version,
                                           const char *arch)
{
    pkg_vec_t *vec;
    long i;
    char *version_str = NULL;

    vec = pkg_vec_fetch_by_name(pkg_name);
    if (!vec)
        return NULL;

    // TODO: Packages are traversed in reverse order to get the latest
    // package in case more than one package is of the same version.
    // This is so that libsolv choses the correct package for reinstall
    for (i = vec->len-1; i >= 0; i--) {
        version_str = pkg_version_str_alloc(vec->pkgs[i]);
        int is_match = (strcmp(version_str, version) == 0)
                && (strcmp(vec->pkgs[i]->architecture, arch) == 0);
        if (is_match) {
            free(version_str);
            break;
        }
        free(version_str);
    }

    // TODO: see above
    if (i == -1)
        return NULL;

    return vec->pkgs[i];
}

pkg_t *pkg_hash_fetch_installed_by_name_dest(const char *pkg_name,
                                             pkg_dest_t * dest)
{
    pkg_vec_t *vec;
    unsigned int i;

    vec = pkg_vec_fetch_by_name(pkg_name);
    if (!vec) {
        return NULL;
    }

    for (i = 0; i < vec->len; i++) {
        int is_installed = (vec->pkgs[i]->state_status == SS_INSTALLED
                    || vec->pkgs[i]->state_status == SS_UNPACKED
                    || vec->pkgs[i]->state_status == SS_HALF_INSTALLED)
                && vec->pkgs[i]->dest == dest;
        if (is_installed)
            return vec->pkgs[i];
    }

    return NULL;
}

pkg_t *pkg_hash_fetch_installed_by_name(const char *pkg_name)
{
    pkg_vec_t *vec;
    unsigned int i;

    vec = pkg_vec_fetch_by_name(pkg_name);
    if (!vec) {
        return NULL;
    }

    for (i = 0; i < vec->len; i++) {
        int is_installed = vec->pkgs[i]->state_status == SS_INSTALLED
            || vec->pkgs[i]->state_status == SS_UNPACKED
            || vec->pkgs[i]->state_status == SS_HALF_INSTALLED;
        if (is_installed)
            return vec->pkgs[i];
    }

    return NULL;
}

static void pkg_hash_fetch_available_helper(const char *pkg_name, void *entry,
                                            void *data)
{
    unsigned int j;
    abstract_pkg_t *ab_pkg = (abstract_pkg_t *) entry;
    pkg_vec_t *all = (pkg_vec_t *) data;
    pkg_vec_t *pkg_vec = ab_pkg->pkgs;

    if (!pkg_vec)
        return;

    for (j = 0; j < pkg_vec->len; j++) {
        pkg_t *pkg = pkg_vec->pkgs[j];
        pkg_vec_insert(all, pkg);
    }
}

void pkg_hash_fetch_available(pkg_vec_t * all)
{
    hash_table_foreach(&opkg_config->pkg_hash, pkg_hash_fetch_available_helper,
                       all);
}

static void pkg_hash_fetch_all_installed_helper(const char *pkg_name,
                                                void *entry, void *data)
{
    abstract_pkg_t *ab_pkg = (abstract_pkg_t *) entry;
    pkg_vec_t *all = (pkg_vec_t *) data;
    pkg_vec_t *pkg_vec = ab_pkg->pkgs;
    unsigned int j;

    if (!pkg_vec)
        return;

    for (j = 0; j < pkg_vec->len; j++) {
        pkg_t *pkg = pkg_vec->pkgs[j];
        int is_installed = pkg->state_status == SS_INSTALLED
                || pkg->state_status == SS_UNPACKED;
        if (is_installed)
            pkg_vec_insert(all, pkg);
    }
}

static void pkg_hash_fetch_all_installed_and_tobe_installed_helper(const char *pkg_name,
                                                void *entry, void *data)
{
    abstract_pkg_t *ab_pkg = (abstract_pkg_t *) entry;
    pkg_vec_t *all = (pkg_vec_t *) data;
    pkg_vec_t *pkg_vec = ab_pkg->pkgs;
    unsigned int j;

    if (!pkg_vec)
        return;

    for (j = 0; j < pkg_vec->len; j++) {
        pkg_t *pkg = pkg_vec->pkgs[j];
        int is_installed = pkg->state_status == SS_INSTALLED
                || pkg->state_status == SS_UNPACKED
                || pkg->state_want == SW_INSTALL;
        if (is_installed)
            pkg_vec_insert(all, pkg);
    }
}

static void pkg_hash_fetch_all_half_or_installed_helper(const char *pkg_name,
                                                        void *entry, void *data)
{
    abstract_pkg_t *ab_pkg = (abstract_pkg_t *) entry;
    pkg_vec_t *all = (pkg_vec_t *) data;
    pkg_vec_t *pkg_vec = ab_pkg->pkgs;
    unsigned int j;

    if (!pkg_vec)
        return;

    for (j = 0; j < pkg_vec->len; j++) {
        pkg_t *pkg = pkg_vec->pkgs[j];
        int is_installed = pkg->state_status == SS_INSTALLED
                           || pkg->state_status == SS_UNPACKED
                           || pkg->state_status == SS_HALF_INSTALLED;
        if (is_installed)
            pkg_vec_insert(all, pkg);
    }
}

void pkg_hash_fetch_all_installed(pkg_vec_t * all, fetch_type_t constrain)
{
    void (*pkg_hash_fetch)(const char *key, void *entry, void *data);

    switch (constrain) {
    case INSTALLED_HALF_INSTALLED:
        pkg_hash_fetch = pkg_hash_fetch_all_half_or_installed_helper;
        break;
    case INSTALLED_TOBE_INSTALLED:
        pkg_hash_fetch = pkg_hash_fetch_all_installed_and_tobe_installed_helper;
        break;
    default:
        pkg_hash_fetch = pkg_hash_fetch_all_installed_helper;
        break;
    }

    hash_table_foreach(&opkg_config->pkg_hash, pkg_hash_fetch, all);
}

/*
 * This assumes that the abstract pkg doesn't exist.
 */
static abstract_pkg_t *add_new_abstract_pkg_by_name(const char *pkg_name)
{
    abstract_pkg_t *ab_pkg;

    ab_pkg = abstract_pkg_new();

    ab_pkg->name = xstrdup(pkg_name);
    hash_table_insert(&opkg_config->pkg_hash, pkg_name, ab_pkg);

    return ab_pkg;
}

abstract_pkg_t *ensure_abstract_pkg_by_name(const char *pkg_name)
{
    abstract_pkg_t *ab_pkg;

    ab_pkg = abstract_pkg_fetch_by_name(pkg_name);
    if (!ab_pkg)
        ab_pkg = add_new_abstract_pkg_by_name(pkg_name);

    return ab_pkg;
}

void hash_insert_pkg(pkg_t * pkg, int set_status)
{
    abstract_pkg_t *ab_pkg;

    ab_pkg = ensure_abstract_pkg_by_name(pkg->name);
    if (!ab_pkg->pkgs)
        ab_pkg->pkgs = pkg_vec_alloc();

    if (pkg->state_status == SS_INSTALLED) {
        ab_pkg->state_status = SS_INSTALLED;
    } else if (pkg->state_status == SS_UNPACKED) {
        ab_pkg->state_status = SS_UNPACKED;
    }

    buildDepends(pkg);

    buildProvides(ab_pkg, pkg);

    /* Need to build the conflicts graph before replaces for correct
     * calculation of replaced_by relation.
     */
    buildConflicts(pkg);

    buildReplaces(ab_pkg, pkg);

    buildDependedUponBy(pkg, ab_pkg);

    pkg_vec_insert_merge(ab_pkg->pkgs, pkg, set_status);
    pkg->parent = ab_pkg;
}

static const char *strip_offline_root(const char *file_name)
{
    unsigned int len;

    if (opkg_config->offline_root) {
        len = strlen(opkg_config->offline_root);
        if (strncmp(file_name, opkg_config->offline_root, len) == 0)
            file_name += len;
    }

    return file_name;
}

void file_hash_remove(const char *file_name)
{
    file_name = strip_offline_root(file_name);
    hash_table_remove(&opkg_config->file_hash, file_name);
}

pkg_t *file_hash_get_file_owner(const char *file_name)
{
    file_name = strip_offline_root(file_name);
    return hash_table_get(&opkg_config->file_hash, file_name);
}

void file_hash_set_file_owner(const char *file_name, pkg_t * owning_pkg)
{
    pkg_t *old_owning_pkg;

    file_name = strip_offline_root(file_name);

    old_owning_pkg = hash_table_get(&opkg_config->file_hash, file_name);
    hash_table_insert(&opkg_config->file_hash, file_name, owning_pkg);

    if (old_owning_pkg) {
        if (!old_owning_pkg->installed_files)
            pkg_get_installed_files(old_owning_pkg);
        file_list_remove_elt(old_owning_pkg->installed_files, file_name);

        /* mark this package to have its filelist written */
        old_owning_pkg->state_flag |= SF_FILELIST_CHANGED;
        owning_pkg->state_flag |= SF_FILELIST_CHANGED;
    }
}
