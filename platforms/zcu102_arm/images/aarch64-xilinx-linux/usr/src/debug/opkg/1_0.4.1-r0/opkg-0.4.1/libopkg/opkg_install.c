/* vi: set expandtab sw=4 sts=4: */
/* opkg_install.c - the opkg package management system

   Carl D. Worth

   Copyright (C) 2001 University of Southern California

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
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>

#include "pkg.h"
#include "pkg_hash.h"
#include "pkg_extract.h"

#include "opkg_configure.h"
#include "opkg_download.h"
#include "opkg_remove.h"
#include "opkg_verify.h"

#include "opkg_utils.h"
#include "opkg_message.h"
#include "opkg_cmd.h"
#include "opkg_conf.h"

#include "sprintf_alloc.h"
#include "file_util.h"
#include "xsystem.h"
#include "xfuncs.h"

static int update_file_ownership(pkg_t * new_pkg, pkg_t * old_pkg)
{
    file_list_t *new_list, *old_list;
    file_list_elt_t *iter, *niter;

    new_list = pkg_get_installed_files(new_pkg);
    if (new_list == NULL)
        return -1;

    for (iter = file_list_first(new_list), niter = file_list_next(new_list, iter);
            iter; iter = niter, niter = file_list_next(new_list, niter)) {
        file_info_t *new_file = (file_info_t *)iter->data;
        pkg_t *owner = file_hash_get_file_owner(new_file->path);
        pkg_t *obs = hash_table_get(&opkg_config->obs_file_hash, new_file->path);

        opkg_msg(DEBUG2, "%s: new_pkg=%s wants file %s, from owner=%s\n",
                 __func__, new_pkg->name, new_file->path,
                 owner ? owner->name : "<NULL>");

        if (!owner || (owner == old_pkg) || obs)
            file_hash_set_file_owner(new_file->path, new_pkg);
    }

    if (old_pkg) {
        old_list = pkg_get_installed_files(old_pkg);
        if (old_list == NULL) {
            pkg_free_installed_files(new_pkg);
            return -1;
        }

        for (iter = file_list_first(old_list), niter = file_list_next(old_list, iter);
                iter; iter = niter, niter = file_list_next(old_list, niter)) {
            file_info_t *old_file = (file_info_t *)iter->data;
            pkg_t *owner = file_hash_get_file_owner(old_file->path);
            if (!owner || (owner == old_pkg)) {
                /* obsolete */
                hash_table_insert(&opkg_config->obs_file_hash, old_file->path,
                                  old_pkg);
            }
        }
        pkg_free_installed_files(old_pkg);
    }
    pkg_free_installed_files(new_pkg);
    return 0;
}

static int verify_pkg_installable(pkg_t * pkg)
{
    unsigned long kbs_available, pkg_size_kbs;
    char *root_dir = NULL;
    struct stat s;

    if (opkg_config->force_space || pkg->installed_size == 0)
        return 0;

    if (pkg->dest) {
        int have_overlay_root = !strcmp(pkg->dest->name, "root")
                && opkg_config->overlay_root
                && !stat(opkg_config->overlay_root, &s)
                && (s.st_mode & S_IFDIR);
        if (have_overlay_root)
            root_dir = opkg_config->overlay_root;
        else
            root_dir = pkg->dest->root_dir;
    }

    if (!root_dir)
        root_dir = opkg_config->default_dest->root_dir;

    kbs_available = get_available_kbytes(root_dir);

    pkg_size_kbs = (pkg->installed_size + 1023) / 1024;

    if (pkg_size_kbs >= kbs_available) {
        opkg_msg(ERROR,
                 "Only have %ldkb available on filesystem %s, "
                 "pkg %s needs %ld\n", kbs_available, root_dir, pkg->name,
                 pkg_size_kbs);
        return -1;
    }

    return 0;
}

static int unpack_pkg_control_files(pkg_t * pkg)
{
    int err;
    char *conffiles_file_name;
    char *root_dir;
    FILE *conffiles_file;

    sprintf_alloc(&pkg->tmp_unpack_dir, "%s/%s-XXXXXX", opkg_config->tmp_dir,
                  pkg->name);

    pkg->tmp_unpack_dir = mkdtemp(pkg->tmp_unpack_dir);
    if (pkg->tmp_unpack_dir == NULL) {
        opkg_perror(ERROR, "Failed to create temporary directory '%s'",
                    pkg->tmp_unpack_dir);
        return -1;
    }

    err = pkg_extract_control_files_to_dir(pkg, pkg->tmp_unpack_dir);
    if (err) {
        return err;
    }

    /* XXX: CLEANUP: There might be a cleaner place to read in the
     * conffiles. Seems like I should be able to get everything to go
     * through pkg_init_from_file. If so, maybe it would make sense to
     * move all of unpack_pkg_control_files to that function. */

    /* Don't need to re-read conffiles if we already have it */
    if (!nv_pair_list_empty(&pkg->conffiles)) {
        return 0;
    }

    sprintf_alloc(&conffiles_file_name, "%s/conffiles", pkg->tmp_unpack_dir);
    if (!file_exists(conffiles_file_name)) {
        free(conffiles_file_name);
        return 0;
    }

    conffiles_file = fopen(conffiles_file_name, "r");
    if (conffiles_file == NULL) {
        opkg_perror(ERROR, "Failed to open %s", conffiles_file_name);
        free(conffiles_file_name);
        return -1;
    }
    free(conffiles_file_name);

    while (1) {
        char *cf_name;
        char *cf_name_in_dest;
        int i;

        cf_name = file_read_line_alloc(conffiles_file);
        if (cf_name == NULL)
            break;
        if (cf_name[0] == '\0')
            continue;
	for (i = strlen(cf_name) - 1;
	     (i >=0) && (cf_name[i] == ' ' || cf_name[i] == '\t');
	     i--)
	  {
	     cf_name[i] = '\0';
	  }

        /* Prepend dest->root_dir to conffile name.
         * Take pains to avoid multiple slashes. */
        root_dir = pkg->dest->root_dir;
        if (opkg_config->offline_root)
            /* skip the offline_root prefix */
            root_dir = pkg->dest->root_dir + strlen(opkg_config->offline_root);
        sprintf_alloc(&cf_name_in_dest, "%s%s", root_dir,
                      cf_name[0] == '/' ? (cf_name + 1) : cf_name);

        /* Can't get an md5sum now, (file isn't extracted yet).
         * We'll wait until resolve_conffiles */
        conffile_list_append(&pkg->conffiles, cf_name_in_dest, NULL);

        free(cf_name);
        free(cf_name_in_dest);
    }

    fclose(conffiles_file);

    return 0;
}

static int prerm_upgrade_old_pkg(pkg_t * pkg, pkg_t * old_pkg)
{
    int err;
    char *script_args;
    char *new_version;

    if (!old_pkg || !pkg)
        return 0;

    new_version = pkg_version_str_alloc(pkg);

    sprintf_alloc(&script_args, "upgrade %s", new_version);
    free(new_version);
    err = pkg_run_script(old_pkg, "prerm", script_args);
    free(script_args);
    if (err != 0) {
        opkg_msg(ERROR, "prerm script for package \"%s\" failed\n",
                 old_pkg->name);
        return -1;
    }
    return 0;
}

static int prerm_upgrade_old_pkg_unwind(pkg_t * pkg, pkg_t * old_pkg)
{
    /* DPKG_INCOMPATIBILITY:
     * dpkg does some things here that we don't do yet. Do we care?
     * If prerm_upgrade_old_pkg fails, attempt:
     * new-prerm failed-upgrade old-version
     * Error unwind:
     * old-postinst abort-upgrade new-version
     */
    return 0;
}

static int preinst_configure(pkg_t * pkg, pkg_t * old_pkg)
{
    int err;
    char *preinst_args;

    if (old_pkg) {
        char *old_version = pkg_version_str_alloc(old_pkg);
        sprintf_alloc(&preinst_args, "upgrade %s", old_version);
        free(old_version);
    } else if (pkg->state_status == SS_CONFIG_FILES) {
        char *pkg_version = pkg_version_str_alloc(pkg);
        sprintf_alloc(&preinst_args, "install %s", pkg_version);
        free(pkg_version);
    } else {
        preinst_args = xstrdup("install");
    }

    err = pkg_run_script(pkg, "preinst", preinst_args);
    if (err) {
        opkg_msg(ERROR, "Aborting installation of %s.\n", pkg->name);
        free(preinst_args);
        return -1;
    }

    free(preinst_args);

    return 0;
}

static int preinst_configure_unwind(pkg_t * pkg, pkg_t * old_pkg)
{
    /* DPKG_INCOMPATIBILITY:
     * dpkg does the following error unwind, should we?
     * pkg->postrm abort-upgrade old-version
     * OR pkg->postrm abort-install old-version
     * OR pkg->postrm abort-install
     */
    return 0;
}

static char *backup_filename_alloc(const char *file_name)
{
    char *backup;

    sprintf_alloc(&backup, "%s%s", file_name, OPKG_BACKUP_SUFFIX);

    return backup;
}

static int backup_make_backup(const char *file_name)
{
    int err;
    char *backup;

    backup = backup_filename_alloc(file_name);
    err = file_copy(file_name, backup);
    if (err)
        opkg_msg(ERROR, "Failed to copy %s to %s\n", file_name, backup);

    free(backup);

    return err;
}

static int backup_exists_for(const char *file_name)
{
    int ret;
    char *backup;

    backup = backup_filename_alloc(file_name);

    ret = file_exists(backup);

    free(backup);

    return ret;
}

static int backup_remove(const char *file_name)
{
    char *backup;

    backup = backup_filename_alloc(file_name);
    unlink(backup);
    free(backup);

    return 0;
}

static int backup_modified_conffiles(pkg_t * pkg, pkg_t * old_pkg)
{
    int err;
    conffile_list_elt_t *iter;
    conffile_t *cf;

    if (opkg_config->noaction)
        return 0;

    /* Backup all modified conffiles */
    if (old_pkg) {
        for (iter = nv_pair_list_first(&old_pkg->conffiles); iter;
                iter = nv_pair_list_next(&old_pkg->conffiles, iter)) {
            char *cf_name;

            cf = iter->data;
            cf_name = root_filename_alloc(cf->name);

            /* Don't worry if the conffile is just plain gone */
            if (file_exists(cf_name) && conffile_has_been_modified(cf)) {
                err = backup_make_backup(cf_name);
                if (err) {
                    return err;
                }
            }
            free(cf_name);
        }
    }

    /* Backup all conffiles that were not conffiles in old_pkg */
    for (iter = nv_pair_list_first(&pkg->conffiles); iter;
            iter = nv_pair_list_next(&pkg->conffiles, iter)) {
        char *cf_name;
        cf = (conffile_t *) iter->data;
        cf_name = root_filename_alloc(cf->name);
        /* Ignore if this was a conffile in old_pkg as well */
        if (pkg_get_conffile(old_pkg, cf->name)) {
            free(cf_name);
            continue;
        }

        if (file_exists(cf_name) && (!backup_exists_for(cf_name))) {
            err = backup_make_backup(cf_name);
            if (err) {
                return err;
            }
        }
        free(cf_name);
    }

    return 0;
}

static int backup_modified_conffiles_unwind(pkg_t * pkg, pkg_t * old_pkg)
{
    conffile_list_elt_t *iter;

    if (old_pkg) {
        for (iter = nv_pair_list_first(&old_pkg->conffiles); iter;
                iter = nv_pair_list_next(&old_pkg->conffiles, iter)) {
            backup_remove(((nv_pair_t *) iter->data)->name);
        }
    }

    for (iter = nv_pair_list_first(&pkg->conffiles); iter;
            iter = nv_pair_list_next(&pkg->conffiles, iter)) {
        backup_remove(((nv_pair_t *) iter->data)->name);
    }

    return 0;
}

static int check_data_file_clashes(pkg_t * pkg, pkg_t * old_pkg)
{
    /* DPKG_INCOMPATIBILITY:
     * opkg takes a slightly different approach than dpkg at this
     * point.  dpkg installs each file in the new package while
     * creating a backup for any file that is replaced, (so that it
     * can unwind if necessary).  To avoid complexity and redundant
     * storage, opkg doesn't do any installation until later, (at the
     * point at which dpkg removes the backups.
     *
     * But, we do have to check for data file clashes, since after
     * installing a package with a file clash, removing either of the
     * packages involved in the clash has the potential to break the
     * other package.
     */
    file_list_t *files_list;
    file_list_elt_t *iter, *niter;
    file_info_t *file_info;
    char *filename;
    int clashes = 0;

    files_list = pkg_get_installed_files(pkg);
    if (files_list == NULL)
        return -1;

    for (iter = file_list_first(files_list), niter = file_list_next(files_list, iter);
            iter; iter = niter, niter = file_list_next(files_list, iter)) {
        file_info = (file_info_t *)iter->data;
        filename = file_info->path;
        if (file_exists(filename)) {
            pkg_t *owner;
            pkg_t *obs;
            int existing_is_dir = file_is_dir(filename);

            /* OK if both the existing file and new file are directories. */
            if (existing_is_dir && S_ISDIR(file_info->mode)) {
                continue;
            } else if (existing_is_dir || S_ISDIR(file_info->mode)) {
                /* Can't mix directory and non-directory.  For normal files,
                 * it would be OK if the package being replaced owns the
                 * directory, but directories may be owned by multiple packages.
                 */
                opkg_msg(ERROR,
                         "Package %s wants to install %s %s\n"
                         "\tBut that path is currently a %s\n",
                         pkg->name, existing_is_dir ? "file" : "directory",
                         filename, existing_is_dir ? "directory" : "file");
                clashes++;
                continue;
            }

            /* OK if both the existing and new are a symlink and point to
             * the same directory */
            if (S_ISLNK(file_info->mode) && file_is_symlink(filename)) {
                char *link_target;
                int r, target_is_same_directory = 0;
                struct stat target_stat;

                link_target = file_readlink_alloc(filename);
                r = strcmp(link_target, file_info->link_target);
                free(link_target);

                if (r == 0) {
                    /* Ensure the target is a directory, not a file.
                     * NOTE: This requires the directory to exist -- if this
                     * is a broken symlink, it will be treated as a file and
                     * be reported as a conflict. */
                    link_target = realpath(filename, NULL);
                    if (link_target && xlstat(link_target, &target_stat) == 0)
                        target_is_same_directory = S_ISDIR(target_stat.st_mode);
                    free(link_target);
                }

                if (target_is_same_directory)
                    continue;
            }

            if (backup_exists_for(filename)) {
                continue;
            }

            /* Pre-existing files are OK if force-overwrite was asserted. */
            if (opkg_config->force_overwrite) {
                /* but we need to change who owns this file */
                file_hash_set_file_owner(filename, pkg);
                continue;
            }

            owner = file_hash_get_file_owner(filename);

            if (!owner && opkg_config->overwrite_no_owner)
                continue;

            /* Pre-existing files are OK if owned by the pkg being upgraded. */
            if (owner && old_pkg) {
                if (strcmp(owner->name, old_pkg->name) == 0) {
                    continue;
                }
            }

            /* Pre-existing files are OK if owned by a package replaced by new pkg. */
            if (owner) {
                opkg_msg(DEBUG2, "Checking replaces for %s in package %s\n",
                         filename, owner->name);
                if (pkg_replaces(pkg, owner)) {
                    continue;
                }
                /* If the file that would be installed is owned by the same
                 * package, ( as per a reinstall or similar ) then it's ok to
                 * overwrite.
                 */
                if (strcmp(owner->name, pkg->name) == 0) {
                    opkg_msg(INFO,
                             "Replacing pre-existing file %s"
                             " owned by package %s\n", filename, owner->name);
                    continue;
                }
            }

            /* Pre-existing files are OK if they are obsolete */
            obs = hash_table_get(&opkg_config->obs_file_hash, filename);
            if (obs) {
                opkg_msg(INFO,
                         "Pre-exiting file %s is obsolete." " obs_pkg=%s\n",
                         filename, obs->name);
                continue;
            }

            /* We have found a clash. */
            opkg_msg(ERROR,
                     "Package %s wants to install file %s\n"
                     "\tBut that file is already provided by package ",
                     pkg->name, filename);
            if (owner) {
                opkg_message(ERROR, "%s\n", owner->name);
            } else {
                opkg_message(ERROR,
                             "<no package>\n"
                             "Please move this file out of the way and try again.\n");
            }
            clashes++;
        }
    }
    pkg_free_installed_files(pkg);

    return clashes;
}

/*
 * XXX: This function sucks, as does the below comment.
 */
static int check_data_file_clashes_change(pkg_t * pkg, pkg_t * old_pkg)
{
    /* Basically that's the worst hack I could do to be able to change ownership
     * of file list, but, being that we have no way to unwind the mods, due to
     * structure of hash table, probably is the quickest hack too, whishing it
     * would not slow-up thing too much.  What we do here is change the
     * ownership of file in hash if a replace ( or similar events happens ) Only
     * the action that are needed to change name should be considered.
     *
     * @@@ To change after 1.0 release.
     */
    file_list_t *files_list;
    file_list_elt_t *iter, *niter;
    file_info_t *file_info;
    char *filename;

    files_list = pkg_get_installed_files(pkg);
    if (files_list == NULL)
        return -1;

    for (iter = file_list_first(files_list), niter = file_list_next(files_list, iter);
            iter; iter = niter, niter = file_list_next(files_list, niter)) {
        file_info = (file_info_t *)iter->data;
        filename = file_info->path;
        if (file_exists(filename) && (!file_is_dir(filename))) {
            pkg_t *owner;

            owner = file_hash_get_file_owner(filename);

            if (opkg_config->force_overwrite) {
                /* but we need to change who owns this file */
                file_hash_set_file_owner(filename, pkg);
                continue;
            }

            /* Pre-existing files are OK if owned by a package replaced by new
             * pkg. */
            if (owner) {
                if (pkg_replaces(pkg, owner)) {
                    /* It's now time to change the owner of that file.
                     * It has been "replaced" from the new "Replaces", then I
                     * need to inform lists file about that.
                     */
                    opkg_msg(INFO,
                             "Replacing pre-existing file %s "
                             "owned by package %s\n", filename, owner->name);
                    file_hash_set_file_owner(filename, pkg);
                    continue;
                }
            }

        }
    }
    pkg_free_installed_files(pkg);

    return 0;
}

static int check_data_file_clashes_unwind(pkg_t * pkg, pkg_t * old_pkg)
{
    /* Nothing to do since check_data_file_clashes doesn't change state */
    return 0;
}

static int postrm_upgrade_old_pkg(pkg_t * pkg, pkg_t * old_pkg)
{
    int err;
    char *script_args;
    char *new_version;

    if (!old_pkg || !pkg)
        return 0;

    new_version = pkg_version_str_alloc(pkg);

    sprintf_alloc(&script_args, "upgrade %s", new_version);
    free(new_version);
    err = pkg_run_script(old_pkg, "postrm", script_args);
    free(script_args);
    if (err != 0) {
        opkg_msg(ERROR, "postrm script for package \"%s\" failed\n",
                 old_pkg->name);
        return -1;
    }
    return 0;
}

static int postrm_upgrade_old_pkg_unwind(pkg_t * pkg, pkg_t * old_pkg)
{
    /* DPKG_INCOMPATIBILITY:
     * dpkg does some things here that we don't do yet. Do we care?
     * If postrm_upgrade_old_pkg fails, attempt:
     * new-postrm failed-upgrade old-version
     * Error unwind:
     * old-preinst abort-upgrade new-version
     */
    return 0;
}

static int remove_obsolesced_files(pkg_t * pkg, pkg_t * old_pkg)
{
    int err = 0;
    file_list_t *old_files;
    file_list_elt_t *of;
    file_list_t *new_files;
    file_list_elt_t *nf;
    hash_table_t new_files_table;

    old_files = pkg_get_installed_files(old_pkg);
    if (old_files == NULL)
        return -1;

    new_files = pkg_get_installed_files(pkg);
    if (new_files == NULL) {
        pkg_free_installed_files(old_pkg);
        return -1;
    }

    new_files_table.entries = NULL;
    hash_table_init("new_files", &new_files_table, 20);
    for (nf = file_list_first(new_files); nf; nf = file_list_next(new_files, nf)) {
        file_info_t *info = (file_info_t *)nf->data;
        if (info && info->path)
            hash_table_insert(&new_files_table, info->path, info->path);
    }

    for (of = file_list_first(old_files); of; of = file_list_next(old_files, of)) {
        pkg_t *owner;
        file_info_t *old = (file_info_t *)of->data;
        char *new;
        new = (char *)hash_table_get(&new_files_table, old->path);
        if (new)
            continue;

        if (file_is_dir(old->path)) {
            continue;
        }
        owner = file_hash_get_file_owner(old->path);
        if (owner != old_pkg) {
            /* in case obsolete file no longer belongs to old_pkg */
            continue;
        }

        /* old file is obsolete */
        opkg_msg(NOTICE, "Removing obsolete file %s.\n", old->path);
        if (!opkg_config->noaction) {
            err = unlink(old->path);
            if (err) {
                opkg_perror(ERROR, "unlinking %s failed", old->path);
            }
        }
    }

    hash_table_deinit(&new_files_table);
    pkg_free_installed_files(old_pkg);
    pkg_free_installed_files(pkg);

    return err;
}

static int install_maintainer_scripts(pkg_t * pkg, pkg_t * old_pkg)
{
    int ret;
    char *prefix;

    sprintf_alloc(&prefix, "%s.", pkg->name);
    ret = pkg_extract_control_files_to_dir_with_prefix(pkg, pkg->dest->info_dir,
                                                       prefix);
    free(prefix);
    return ret;
}

static int remove_disappeared(pkg_t * pkg)
{
    /* DPKG_INCOMPATIBILITY:
     * This is a fairly sophisticated dpkg operation. Shall we
     * skip it? */

    /* Any packages all of whose files have been overwritten during the
     * installation, and which aren't required for dependencies, are
     * considered to have been removed. For each such package
     * 1. disappearer's-postrm disappear overwriter overwriter-version
     * 2. The package's maintainer scripts are removed
     * 3. It is noted in the status database as being in a sane state,
     * namely not installed (any conffiles it may have are ignored,
     * rather than being removed by dpkg). Note that disappearing
     * packages do not have their prerm called, because dpkg doesn't
     * know in advance that the package is going to vanish.
     */
    return 0;
}

static int install_data_files(pkg_t * pkg)
{
    int err;

    /* opkg takes a slightly different approach to data file backups
     * than dpkg. Rather than removing backups at this point, we
     * actually do the data file installation now. See comments in
     * check_data_file_clashes() for more details. */

    opkg_msg(INFO, "Extracting data files to %s.\n", pkg->dest->root_dir);
    err = pkg_extract_data_files_to_dir(pkg, pkg->dest->root_dir);
    if (err) {
        return err;
    }

    opkg_msg(DEBUG, "Calling pkg_write_filelist.\n");
    err = pkg_write_filelist(pkg);
    if (err)
        return err;

    /* XXX: FEATURE: opkg should identify any files which existed
     * before installation and which were overwritten, (see
     * check_data_file_clashes()). What it must do is remove any such
     * files from the filelist of the old package which provided the
     * file. Otherwise, if the old package were removed at some point
     * it would break the new package. Removing the new package will
     * also break the old one, but this cannot be helped since the old
     * package's file has already been deleted. This is the importance
     * of check_data_file_clashes(), and only allowing opkg to install
     * a clashing package with a user force. */

    return 0;
}

static int resolve_conffiles(pkg_t * pkg)
{
    conffile_list_elt_t *iter;
    conffile_t *cf;
    char *cf_backup;
    char *md5sum;

    if (opkg_config->noaction)
        return 0;

    for (iter = nv_pair_list_first(&pkg->conffiles); iter;
            iter = nv_pair_list_next(&pkg->conffiles, iter)) {
        char *root_filename;
        cf = (conffile_t *) iter->data;
        root_filename = root_filename_alloc(cf->name);

        /* Might need to initialize the md5sum for each conffile */
        if (cf->value == NULL) {
            cf->value = file_md5sum_alloc(root_filename);
        }

        if (!file_exists(root_filename)) {
            free(root_filename);
            continue;
        }

        cf_backup = backup_filename_alloc(root_filename);

        if (file_exists(cf_backup)) {
            /* Let's compute md5 to test if files are changed */
            md5sum = file_md5sum_alloc(cf_backup);
            if (md5sum && cf->value && strcmp(cf->value, md5sum) != 0) {
                if (opkg_config->force_maintainer) {
                    opkg_msg(NOTICE,
                             "Conffile %s using maintainer's setting.\n",
                             cf_backup);
                } else if (opkg_config->ignore_maintainer) {
                    opkg_msg(NOTICE,
                             "Conffile %s ignoring maintainer's changes.\n",
                             root_filename);
                    rename(cf_backup, root_filename);
                } else {
                    char *new_conffile;
                    sprintf_alloc(&new_conffile, "%s-opkg", root_filename);
                    opkg_msg(NOTICE,
                             "Existing conffile %s "
                             "is different from the conffile in the new package.\n"
                             " The new conffile will be placed at %s.\n",
                             root_filename, new_conffile);
                    rename(root_filename, new_conffile);
                    rename(cf_backup, root_filename);
                    free(new_conffile);
                }
            }
            unlink(cf_backup);
            if (md5sum)
                free(md5sum);
        }

        free(cf_backup);
        free(root_filename);
    }

    return 0;
}

/**
 *  @brief Really install a pkg_t
 */
int opkg_install_pkg(pkg_t * pkg)
{
    int err = 0;
    pkg_t *old_pkg = NULL;
    abstract_pkg_t *ab_pkg = NULL;
    int old_state_flag;
    sigset_t newset, oldset;

    opkg_msg(DEBUG2, "Calling pkg_arch_supported.\n");

    if (!pkg_arch_supported(pkg)) {
        opkg_msg(ERROR,
                 "INTERNAL ERROR: architecture %s for pkg %s is unsupported.\n",
                 pkg->architecture, pkg->name);
        return -1;
    }

    if (pkg->dest == NULL) {
        pkg->dest = opkg_config->default_dest;
    }

    old_pkg = pkg_hash_fetch_installed_by_name(pkg->name);

    pkg->state_want = SW_INSTALL;
    if (old_pkg) {
        old_pkg->state_want = SW_DEINSTALL;
        /* needed for check_data_file_clashes of dependencies */
    }

    err = verify_pkg_installable(pkg);
    if (err)
        return -1;

    /* check that the repository is valid */
    if (opkg_config->check_signature && pkg->src && !(pkg->src->options->disable_sig_check)) {
        /* pkg_src_verify prints an error message so we don't have to. */
        err = pkg_src_verify(pkg->src);
        if (err)
            return err;
    }

    if (opkg_config->noaction)
        return 0;

    if (pkg->local_filename == NULL && !opkg_config->download_first) {
        err = opkg_download_pkg(pkg);
        if (err) {
            opkg_msg(ERROR,
                     "Failed to download %s. "
                     "Perhaps you need to run 'opkg update'?\n", pkg->name);
            return -1;
        }
    }

    if (opkg_config->download_only)
        return 0;

    if (pkg->tmp_unpack_dir == NULL) {
        err = unpack_pkg_control_files(pkg);
        if (err == -1) {
            opkg_msg(ERROR, "Failed to unpack control files from %s.\n",
                     pkg->local_filename);
            return -1;
        }
    }

    err = update_file_ownership(pkg, old_pkg);
    if (err)
        return -1;

    /* this next section we do with SIGINT blocked to prevent inconsistency
     * between opkg database and filesystem */

    sigemptyset(&newset);
    sigaddset(&newset, SIGINT);
    sigprocmask(SIG_BLOCK, &newset, &oldset);

    opkg_state_changed++;
    pkg->state_flag |= SF_FILELIST_CHANGED;

    err = prerm_upgrade_old_pkg(pkg, old_pkg);
    if (err)
        goto UNWIND_PRERM_UPGRADE_OLD_PKG;
    err = preinst_configure(pkg, old_pkg);
    if (err)
        goto UNWIND_PREINST_CONFIGURE;

    err = backup_modified_conffiles(pkg, old_pkg);
    if (err)
        goto UNWIND_BACKUP_MODIFIED_CONFFILES;

    err = check_data_file_clashes(pkg, old_pkg);
    if (err)
        goto UNWIND_CHECK_DATA_FILE_CLASHES;

    err = postrm_upgrade_old_pkg(pkg, old_pkg);
    if (err)
        goto UNWIND_POSTRM_UPGRADE_OLD_PKG;

    if (opkg_config->noaction)
        return 0;

    /* point of no return: no unwinding after this */
    if (old_pkg) {
        old_pkg->state_want = SW_DEINSTALL;

        if (old_pkg->state_flag & SF_NOPRUNE) {
            opkg_msg(INFO,
                     "Not removing obsolesced files because "
                     "package %s marked noprune.\n", old_pkg->name);
        } else {
            opkg_msg(INFO, "Removing obsolesced files for %s\n", old_pkg->name);
            err = remove_obsolesced_files(pkg, old_pkg);
            if (err) {
                opkg_msg(ERROR,
                         "Failed to determine "
                         "obsolete files from previously " "installed %s\n",
                         old_pkg->name);
            }
        }

        /* removing files from old package, to avoid ghost files */
        remove_data_files_and_list(old_pkg);
        remove_maintainer_scripts(old_pkg);

        /* maintain the "Auto-Installed: yes" flag */
        pkg->auto_installed = old_pkg->auto_installed;
    }

    opkg_msg(INFO, "Installing maintainer scripts.\n");
    err = install_maintainer_scripts(pkg, old_pkg);
    if (err) {
        opkg_msg(ERROR,
                 "Failed to extract maintainer scripts for %s."
                 " Package debris may remain!\n", pkg->name);
        goto pkg_is_hosed;
    }

    /* the following just returns 0 */
    remove_disappeared(pkg);

    opkg_msg(INFO, "Installing data files for %s.\n", pkg->name);

    err = install_data_files(pkg);
    if (err) {
        opkg_msg(ERROR,
                 "Failed to extract data files for %s. "
                 "Package debris may remain!\n", pkg->name);
        goto pkg_is_hosed;
    }

    err = check_data_file_clashes_change(pkg, old_pkg);
    if (err) {
        opkg_msg(ERROR,
                 "check_data_file_clashes_change() failed for "
                 "for files belonging to %s.\n", pkg->name);
    }

    opkg_msg(INFO, "Resolving conf files for %s\n", pkg->name);
    resolve_conffiles(pkg);

    pkg->state_status = SS_UNPACKED;
    old_state_flag = pkg->state_flag;
    pkg->state_flag &= ~SF_PREFER;
    opkg_msg(DEBUG, "pkg=%s old_state_flag=%x state_flag=%x\n", pkg->name,
             old_state_flag, pkg->state_flag);

    if (old_pkg)
        old_pkg->state_status = SS_NOT_INSTALLED;

    time(&pkg->installed_time);

    ab_pkg = pkg->parent;
    if (ab_pkg)
        ab_pkg->state_status = pkg->state_status;

    sigprocmask(SIG_UNBLOCK, &newset, &oldset);
    return 0;

 UNWIND_POSTRM_UPGRADE_OLD_PKG:
    postrm_upgrade_old_pkg_unwind(pkg, old_pkg);
 UNWIND_CHECK_DATA_FILE_CLASHES:
    check_data_file_clashes_unwind(pkg, old_pkg);
 UNWIND_BACKUP_MODIFIED_CONFFILES:
    backup_modified_conffiles_unwind(pkg, old_pkg);
 UNWIND_PREINST_CONFIGURE:
    preinst_configure_unwind(pkg, old_pkg);
 UNWIND_PRERM_UPGRADE_OLD_PKG:
    prerm_upgrade_old_pkg_unwind(pkg, old_pkg);
 pkg_is_hosed:
    /* Set the package flags to something consistent which indicates a
     * failed install.
     */
    pkg->state_flag = SF_REINSTREQ;
    pkg->state_status = SS_HALF_INSTALLED;

    if (old_pkg)
        old_pkg->state_status = SS_NOT_INSTALLED;

    /* Print some advice for the user. */
    opkg_msg(NOTICE, "To remove package debris, try `opkg remove %s`.\n",
             pkg->name);
    opkg_msg(NOTICE, "To re-attempt the install, try `opkg install %s`.\n",
             pkg->name);

    sigprocmask(SIG_UNBLOCK, &newset, &oldset);
    return -1;
}
