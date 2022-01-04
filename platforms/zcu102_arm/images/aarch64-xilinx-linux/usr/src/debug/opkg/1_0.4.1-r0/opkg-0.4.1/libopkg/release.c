/* vi: set expandtab sw=4 sts=4: */
/* release.c - the opkg package management system

   Copyright (C) 2010,2011 Javier Palacios

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

#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "opkg_message.h"
#include "release.h"
#include "opkg_utils.h"
#include "xfuncs.h"
#include "opkg_archive.h"

#include "opkg_download.h"
#include "sprintf_alloc.h"

#include "release_parse.h"

#include "parse_util.h"
#include "file_util.h"

static void release_init(release_t * release)
{
    release->name = NULL;
    release->datestring = NULL;
    release->architectures = NULL;
    release->architectures_count = 0;
    release->components = NULL;
    release->components_count = 0;
    release->complist = NULL;
    release->complist_count = 0;
}

static const char *item_in_list(const char *comp, char **complist, unsigned int count)
{
    unsigned int i;

    if (!complist)
        return comp;

    for (i = 0; i < count; i++) {
        if (strcmp(comp, complist[i]) == 0)
            return complist[i];
    }

    return NULL;
}

static int release_arch_supported(release_t * release)
{
    nv_pair_list_elt_t *l;

    list_for_each_entry(l, &opkg_config->arch_list.head, node) {
        nv_pair_t *nv = (nv_pair_t *) l->data;
        const char *arch = item_in_list(nv->name, release->architectures,
                release->architectures_count);
        if (arch) {
            opkg_msg(DEBUG, "Arch %s (priority %s) supported for dist %s.\n",
                     nv->name, nv->value, release->name);
            return 1;
        }
    }

    return 0;
}

static int release_get_size(release_t * release, const char *pathname)
{
    const cksum_t *cksum;

    if (release->md5sums) {
        cksum = cksum_list_find(release->md5sums, pathname);
        return cksum->size;
    }
#ifdef HAVE_SHA256
    if (release->sha256sums) {
        cksum = cksum_list_find(release->sha256sums, pathname);
        return cksum->size;
    }
#endif

    return -1;
}

static const char *release_get_md5(release_t * release, const char *pathname)
{
    const cksum_t *cksum;

    if (release->md5sums) {
        cksum = cksum_list_find(release->md5sums, pathname);
        return cksum->value;
    }

    return '\0';
}

#ifdef HAVE_SHA256
static const char *release_get_sha256(release_t * release, const char *pathname)
{
    const cksum_t *cksum;

    if (release->sha256sums) {
        cksum = cksum_list_find(release->sha256sums, pathname);
        return cksum->value;
    }

    return '\0';
}
#endif

int release_verify_file(release_t * release, const char *file_name,
                        const char *pathname)
{
    struct stat f_info;
    char *f_md5 = NULL;
    const char *md5 = release_get_md5(release, pathname);
#ifdef HAVE_SHA256
    char *f_sha256 = NULL;
    const char *sha256 = release_get_sha256(release, pathname);
#endif
    int ret = 0;
    int r;

    r = stat(file_name, &f_info);
    if ((r != 0) || (f_info.st_size != release_get_size(release, pathname))) {
        opkg_msg(ERROR, "Size verification failed for %s - %s.\n",
                 release->name, pathname);
        ret = 1;
    } else {

        f_md5 = file_md5sum_alloc(file_name);
#ifdef HAVE_SHA256
        f_sha256 = file_sha256sum_alloc(file_name);
#endif

        if (md5 && strcmp(md5, f_md5)) {
            opkg_msg(ERROR, "MD5 verification failed for %s - %s.\n",
                     release->name, pathname);
            ret = 1;
#ifdef HAVE_SHA256
        } else if (sha256 && strcmp(sha256, f_sha256)) {
            opkg_msg(ERROR, "SHA256 verification failed for %s - %s.\n",
                     release->name, pathname);
            ret = 1;
#endif
        }

    }

    free(f_md5);
#ifdef HAVE_SHA256
    free(f_sha256);
#endif

    return ret;
}

release_t *release_new(void)
{
    release_t *release;

    release = xcalloc(1, sizeof(release_t));
    release_init(release);

    return release;
}

void release_deinit(release_t * release)
{
    unsigned int i;

    free(release->name);
    free(release->datestring);

    for (i = 0; i < release->architectures_count; i++) {
        free(release->architectures[i]);
    }
    free(release->architectures);

    for (i = 0; i < release->components_count; i++) {
        free(release->components[i]);
    }
    free(release->components);

    for (i = 0; i < release->complist_count; i++) {
        free(release->complist[i]);
    }
    free(release->complist);

}

int release_init_from_file(release_t * release, const char *filename)
{
    int err = 0;
    FILE *release_file = NULL;
    char *bp = NULL;

    if (opkg_config->compress_list_files) {
        struct opkg_ar *ar;
        size_t size;

        ar = ar_open_compressed_file(filename);
        if (!ar)
            return -1;

        FILE *mfp = open_memstream(&bp, &size);

        if (ar_copy_to_stream(ar, mfp) < 0) {
            opkg_perror(ERROR, "Failed to open %s", filename);
            err = -1;
            goto cleanup;
        }
        fclose(mfp);

        release_file = fmemopen(bp, size, "r");
        if (release_file == NULL) {
            opkg_perror(ERROR, "Failed to open memory buffer: %s\n", strerror(errno));
            err = -1;
            goto cleanup;
        }
    } else {
        release_file = fopen(filename, "r");
        if (release_file == NULL) {
            opkg_perror(ERROR, "Failed to open %s", filename);
            return -1;
        }
    }

    err = release_parse_from_stream(release, release_file);
    if (!err) {
        if (!release_arch_supported(release)) {
            opkg_msg(ERROR, "No valid architecture found on Release file.\n");
            err = -1;
        }
    }

cleanup:
    fclose(release_file);
    free(bp);
    return err;
}

int release_comps_supported(release_t * release, const char *complist)
{
    int ret = 1;
    unsigned int i;

    if (complist) {
        release->complist = parse_list(complist, &release->complist_count,
                ' ', 1);
        for (i = 0; i < release->complist_count; i++) {
            const char *arch = item_in_list (release->complist[i],
                    release->components,
                    release->components_count);
            if (!arch) {
                opkg_msg(ERROR, "Component %s not supported for dist %s.\n",
                         release->complist[i], release->name);
                ret = 0;
            }
        }
    }

    return ret;
}

const char **release_comps(release_t * release, unsigned int *count)
{
    char **comps = release->complist;

    if (!comps) {
        comps = release->components;
        *count = release->components_count;
    } else {
        *count = release->complist_count;
    }

    return (const char **)comps;
}

int release_download(release_t * release, pkg_src_t * dist, char *lists_dir,
                     char *tmpdir)
{
    int ret = 0;
    unsigned int ncomp;
    const char **comps = release_comps(release, &ncomp);
    nv_pair_list_elt_t *l;
    unsigned int i;

    for (i = 0; i < ncomp; i++) {
        int err = 0;
        char *prefix;

        sprintf_alloc(&prefix, "%s/dists/%s/%s/binary", dist->value, dist->name,
                      comps[i]);

        list_for_each_entry(l, &opkg_config->arch_list.head, node) {
            char *url;
            char *list_file_name;
            char *subpath = NULL;

            nv_pair_t *nv = (nv_pair_t *) l->data;

            sprintf_alloc(&list_file_name, "%s/%s-%s-%s", lists_dir, dist->name,
                          comps[i], nv->name);

            sprintf_alloc(&subpath, "%s/binary-%s/%s", comps[i], nv->name,
                          dist->gzip ? "Packages.gz" : "Packages");

            if (dist->gzip) {
                char *cache_location;
                sprintf_alloc(&url, "%s-%s/Packages.gz", prefix, nv->name);
                cache_location = opkg_download_cache(url, NULL, NULL);
                if (cache_location) {
                    err = release_verify_file(release, cache_location, subpath);
                    if (err) {
                        unlink(list_file_name);
                    } else {
                        if (opkg_config->compress_list_files) {
                            strcat(list_file_name, ".gz");
                            err = file_copy(cache_location, list_file_name);
                        } else {
                            err = file_decompress(cache_location, list_file_name);
                        }
                        if (err) {
                            opkg_msg(ERROR, "Couldn't %s %s",
                                    (opkg_config->compress_list_files) ? "copy" : "decompress", url);
                        }
                    }
                }
                free(url);
                free(cache_location);
            }

            if (!dist->gzip || err) {
                sprintf_alloc(&url, "%s-%s/Packages", prefix, nv->name);
                err = opkg_download(url, list_file_name, NULL, NULL);
                if (!err) {
                    if (opkg_config->compress_list_files)
                        err = file_gz_compress(list_file_name);
                    err = release_verify_file(release, list_file_name, subpath);
                    if (err)
                        unlink(list_file_name);
                }
                free(url);
            }

            free(list_file_name);
        }

        if (err)
            ret = 1;

        free(prefix);
    }

    return ret;
}
