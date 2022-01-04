/* vi: set expandtab sw=4 sts=4: */
/* opkg_solver_libsolv.c - handle package dependency solving with
   calls to libsolv.

   Copyright (C) 2015 National Instruments Corp.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2, or (at
   your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.
*/

#include <stdlib.h>

#include <solv/pool.h>
#include <solv/poolarch.h>
#include <solv/queue.h>
#include <solv/repo.h>
#include <solv/solver.h>
#include <solv/solverdebug.h>

#include "opkg_install.h"
#include "opkg_download.h"
#include "opkg_remove.h"
#include "opkg_message.h"
#include "opkg_utils.h"
#include "pkg_vec.h"
#include "pkg_hash.h"
#include "xfuncs.h"
#include "sprintf_alloc.h"

#define INITIAL_ARCH_LIST_SIZE 4

/* Priority values (the high priority 99 was taken from libsolv's
   examples/solv.c) These values are aribtrary values such that
   1 < PRIORITY_PREFERRED < PRIORITY_MARKED_FOR_INSTALL so that
   packages marked to be installed have higher priority than packages
   marked as preferred which have higher priority ofer other pakcages */
#define PRIORITY_PREFERRED 90
#define PRIORITY_MARKED_FOR_INSTALL 99

struct libsolv_solver {
    Solver *solver;
    Queue solver_jobs;
    Pool *pool;
    Repo *repo_installed;
    Repo *repo_available;
    Repo *repo_preferred;
    Repo *repo_to_install;
};
typedef struct libsolv_solver libsolv_solver_t;

struct arch_data {
    char *arch;
    int priority;
};
typedef struct arch_data arch_data_t;

enum job_action {
    JOB_NOOP,
    JOB_INSTALL,
    JOB_REMOVE,
    JOB_UPGRADE,
    JOB_DISTUPGRADE
};
typedef enum job_action job_action_t;

static libsolv_solver_t *libsolv_solver_new(void);
static void libsolv_solver_free(libsolv_solver_t *libsolv_solver);
static void libsolv_solver_add_job(libsolv_solver_t *libsolv_solver,
                                   job_action_t action, const char *pkg_name,
                                   const char *pkg_version,
                                   version_constraint_t constraint);
static int libsolv_solver_solve(libsolv_solver_t *libsolv_solver);

typedef int (*libsolv_solver_action_func_t)(libsolv_solver_t *libsolv_solver);
static int libsolv_solver_execute_transaction(libsolv_solver_t *libsolv_solver);
static int libsolv_solver_print_transaction(libsolv_solver_t *libsolv_solver);

int opkg_solver_install(int num_pkgs, char **pkg_names)
{
    int i, err;
    char *name, *version;
    version_constraint_t constraint;
    Dataiterator di;

    libsolv_solver_t *solver = libsolv_solver_new();
    if (solver == NULL)
        return -1;

    if (num_pkgs == 0) {
        opkg_msg(ERROR, "No packages specified for install!\n");
        err = -1;
        goto CLEANUP;
    }

    for (i = 0; i < num_pkgs; i++) {
        strip_pkg_name_and_version(pkg_names[i], &name, &version, &constraint);

        dataiterator_init(&di, solver->pool, solver->repo_available, 0,
                          SOLVABLE_NAME | SOLVABLE_PROVIDES, name, SEARCH_GLOB);
        while (dataiterator_step(&di)) {
            libsolv_solver_add_job(solver, JOB_INSTALL, di.kv.str, version, constraint);
            dataiterator_skip_solvable(&di);
        }

        dataiterator_free(&di);
        free(name);
        free(version);
    }

    err = libsolv_solver_solve(solver);
    if (err)
        goto CLEANUP;

    err = libsolv_solver_execute_transaction(solver);

CLEANUP:
    libsolv_solver_free(solver);
    return err;
}

int opkg_solver_remove(int num_pkgs, char **pkg_names)
{
    int i, err;
    Dataiterator di;

    libsolv_solver_t *solver = libsolv_solver_new();
    if (solver == NULL)
        return -1;

    if (num_pkgs == 0) {
        opkg_msg(ERROR, "No packages specified for removal!\n");
        err = -1;
        goto CLEANUP;
    }

    for (i = 0; i < num_pkgs; i++){
        dataiterator_init(&di, solver->pool, solver->repo_installed, 0, 0, pkg_names[i], SEARCH_GLOB);
        while (dataiterator_step(&di)) {
            libsolv_solver_add_job(solver, JOB_REMOVE, di.kv.str, NULL, NONE);
            dataiterator_skip_solvable(&di);
        }
        dataiterator_free(&di);
    }

    err = libsolv_solver_solve(solver);
    if (err)
        goto CLEANUP;

    err = libsolv_solver_execute_transaction(solver);

CLEANUP:
    libsolv_solver_free(solver);
    return err;
}

static int opkg_solver_do_upgrade(int num_pkgs, char **pkg_names, libsolv_solver_action_func_t libsolv_solver_action)
{
    int i, err;
    Dataiterator di;

    libsolv_solver_t *solver = libsolv_solver_new();
    if (solver == NULL)
        return -1;

    if (num_pkgs == 0) {
        libsolv_solver_add_job(solver, JOB_UPGRADE, 0, NULL, NONE);
    } else {
        for (i = 0; i < num_pkgs; i++) {
            dataiterator_init(&di, solver->pool, solver->repo_installed, 0, 0, pkg_names[i], SEARCH_GLOB);
            while (dataiterator_step(&di)) {
                libsolv_solver_add_job(solver, JOB_UPGRADE, di.kv.str, NULL, NONE);
                dataiterator_skip_solvable(&di);
            }
            dataiterator_free(&di);
        }
    }

    err = libsolv_solver_solve(solver);
    if (err)
        goto CLEANUP;

    err = libsolv_solver_action(solver);

CLEANUP:
    libsolv_solver_free(solver);
    return err;
}

int opkg_solver_upgrade(int num_pkgs, char **pkg_names)
{
    return opkg_solver_do_upgrade(num_pkgs, pkg_names, libsolv_solver_execute_transaction);
}

int opkg_solver_list_upgradable(int num_pkgs, char **pkg_names)
{
    return opkg_solver_do_upgrade(num_pkgs, pkg_names, libsolv_solver_print_transaction);
}

int opkg_solver_distupgrade(int num_pkgs, char **pkg_names)
{
    int i, err;

    libsolv_solver_t *solver = libsolv_solver_new();
    if (solver == NULL)
        return -1;

    if (num_pkgs == 0) {
        libsolv_solver_add_job(solver, JOB_DISTUPGRADE, 0, NULL, NONE);
    } else {
        for (i = 0; i < num_pkgs; i++)
            libsolv_solver_add_job(solver, JOB_DISTUPGRADE, pkg_names[i], NULL, NONE);
    }

    err = libsolv_solver_solve(solver);
    if (err)
        goto CLEANUP;

    err = libsolv_solver_execute_transaction(solver);

CLEANUP:
    libsolv_solver_free(solver);
    return err;
}

static int compare_arch_priorities(const void *p1, const void *p2)
{
    int priority1 = ((arch_data_t *)p1)->priority;
    int priority2 = ((arch_data_t *)p2)->priority;

    if (priority1 < priority2)
        return -1;
    else if (priority1 == priority2)
        return 0;
    else
        return 1;
}

static void libsolv_solver_set_arch_policy(libsolv_solver_t *libsolv_solver)
{
    int arch_list_size = INITIAL_ARCH_LIST_SIZE;
    int arch_count = 0;
    char *arch_policy;

    arch_data_t *archs = xcalloc(arch_list_size, sizeof(arch_data_t));

    nv_pair_list_elt_t *arch_info;

    list_for_each_entry(arch_info, &opkg_config->arch_list.head, node) {
        if (arch_count >= arch_list_size) {
            arch_list_size *= 2;
            archs = xrealloc(archs, arch_list_size * sizeof(arch_data_t));
        }

        archs[arch_count].arch = ((nv_pair_t *)(arch_info->data))->name;
        archs[arch_count].priority = atoi(((nv_pair_t *)
                                           (arch_info->data))->value);
        arch_count++;
    }

    /* if there are no architectures, set the policy to accept packages with
       architectures noarch and all */
    if (!arch_count) {
        arch_policy = "all=noarch";
        goto SET_ARCH_POLICY_AND_EXIT;
    }

    qsort(archs, arch_count, sizeof(arch_data_t), compare_arch_priorities);

    int j = 0;
    arch_policy = archs[j].arch;
    int prev_priority = archs[j].priority;

    /* libsolv's arch policy is a string of archs seperated by : < or =
       depending on priorities.
       A=B means arch A and arch B have equal priority.
       A:B means A is better than B and if a package with A is available,
           packages with arch B are not considered
       A>B means A is better than B, but a package of arch B may be installed
           if its version is better than a package of arch A

       TODO: this should not be necessary as ':' should work in both cases if
       SOLVER_FLAG_NO_INFARCHCHECK works as documented and does not check whether
       packages are of inferior architectures see libsolv issue #98 */
    char *better_arch_delimiter = opkg_config->prefer_arch_to_version ? ":" : ">";

    for (j = 1; j < arch_count; j++) {
        int priority = archs[j].priority;
        char *arch_delimiter = (prev_priority == priority) ?
                           "=" : better_arch_delimiter;
        arch_policy = pool_tmpjoin(libsolv_solver->pool, archs[j].arch,
                                   arch_delimiter, arch_policy);
        prev_priority = archs[j].priority;
    }

 SET_ARCH_POLICY_AND_EXIT:
    free(archs);
    opkg_msg(DEBUG2, "libsolv arch policy: %s\n", arch_policy);
    pool_setarchpolicy(libsolv_solver->pool, arch_policy);
}

/* Convert an opkg constraint to libsolv flags */
static int constraint_to_solv_flags(version_constraint_t constraint)
{
    int flags = 0;

    switch (constraint) {
    case EARLIER:
        flags = REL_LT;
        break;
    case EARLIER_EQUAL:
        flags = REL_LT | REL_EQ;
        break;
    case EQUAL:
        flags = REL_EQ;
        break;
    case LATER:
        flags = REL_GT;
        break;
    case LATER_EQUAL:
        flags = REL_GT | REL_EQ;
        break;
    default:
        break;
    }

    return flags;
}

/* This transforms a compound depend [e.g. A (=3.0)|C (>=2.0) ]
   into an id usable by libsolv. */
static Id dep2id(Pool *pool, compound_depend_t *dep)
{
    Id nameId = 0;
    Id versionId = 0;
    Id flagId = 0;
    Id previousId = 0;
    Id dependId = 0;

    int i;

    for (i = 0; i < dep->possibility_count; ++i) {
        depend_t *depend = dep->possibilities[i];

        nameId = pool_str2id(pool, depend->pkg->name, 1);
        if (depend->version) {
            versionId = pool_str2id(pool, depend->version, 1);
            flagId = constraint_to_solv_flags(depend->constraint);
            dependId = pool_rel2id(pool, nameId, versionId, flagId, 1);
        } else {
            dependId = nameId;
        }

        if (previousId)
            dependId = pool_rel2id(pool, dependId, previousId, REL_OR, 1);

        previousId = dependId;
    }
    return dependId;
}

static void pkg2solvable(pkg_t *pkg, Solvable *solvable_out)
{
    if (!solvable_out) {
        opkg_msg(ERROR, "Solvable undefined!\n");
        return;
    }

    int i;
    Id dependId;
    Repo *repo = solvable_out->repo;
    Pool *pool = repo->pool;

    char *version = pkg_version_str_alloc(pkg);
    Id versionId = pool_str2id(pool, version, 1);
    free(version);

    /* set the solvable version */
    solvable_out->evr = versionId;

    /* set the solvable name */
    Id nameId = pool_str2id(pool, pkg->name, 1);
    solvable_out->name = nameId;

    /* set the architecture of the solvable */
    solvable_out->arch = pool_str2id(pool, pkg->architecture, 1);

    /* set the solvable's dependencies (depends, recommends, suggests) */
    if (pkg->depends && !opkg_config->nodeps) {
        unsigned int deps_count = pkg->depends_count + pkg->pre_depends_count
            + pkg->recommends_count + pkg->suggests_count;

        for (i = 0; i < deps_count; ++i) {
            compound_depend_t dep = pkg->depends[i];
            dependId = dep2id(pool, &dep);
            switch (dep.type) {
            /* pre-depends is not supported by opkg. However, the
               field is parsed in pkg_parse. This will handle the
               pre-depends case if a package control happens to have it*/
            case PREDEPEND:
                opkg_msg(DEBUG2, "%s pre-depends %s\n", pkg->name,
                         pool_dep2str(pool, dependId));
                solvable_out->requires = repo_addid_dep(repo,
                         solvable_out->requires, dependId,
                         SOLVABLE_PREREQMARKER);
            case DEPEND:
                opkg_msg(DEBUG2, "%s depends %s\n", pkg->name,
                         pool_dep2str(pool, dependId));
                solvable_out->requires = repo_addid_dep(repo,
                         solvable_out->requires, dependId,
                         -SOLVABLE_PREREQMARKER);
                break;
            case RECOMMEND:
                opkg_msg(DEBUG2, "%s recommends %s\n", pkg->name,
                         pool_dep2str(pool, dependId));
                solvable_out->recommends = repo_addid_dep(repo,
                         solvable_out->recommends, dependId, 0);
                break;
            case SUGGEST:
                opkg_msg(DEBUG2, "%s suggests %s\n", pkg->name,
                         pool_dep2str(pool, dependId));
                solvable_out->suggests = repo_addid_dep(repo,
                         solvable_out->suggests, dependId, 0);
                break;
            default:
                break;
            }
        }
    }

    /* set solvable conflicts */
    if (pkg->conflicts) {
        for (i = 0; i < pkg->conflicts_count; i++) {
            compound_depend_t dep = pkg->conflicts[i];

            dependId = dep2id(pool, &dep);
            opkg_msg(DEBUG2, "%s conflicts %s\n", pkg->name,
                     pool_dep2str(pool, dependId));
            solvable_out->conflicts = repo_addid_dep(repo,
                     solvable_out->conflicts, dependId, 0);
        }
    }

    /* set solvable provides */
    if (pkg->provides_count) {
        for (i = 0; i < pkg->provides_count; i++) {
            abstract_pkg_t *provide = pkg->provides[i];

            /* we will handle the case where a package provides itself
               separately */
            if (strcmp(pkg->name, provide->name) != 0) {
                opkg_msg(DEBUG2, "%s provides %s\n", pkg->name, provide->name);
                Id provideId = pool_str2id(pool, provide->name, 1);
                solvable_out->provides = repo_addid_dep(repo,
                    solvable_out->provides, provideId, 0);
            }
        }
    }
    /* every package provides itself in its own version */
    solvable_out->provides = repo_addid_dep(repo, solvable_out->provides,
        pool_rel2id(pool, nameId, versionId, REL_EQ, 1),
        0);

    /* set solvable obsoletes. Obsoletes is libsolv's equivalent of Replaces*/
    if (pkg->replaces_count) {
        for (i = 0; i < pkg->replaces_count; i++) {
            compound_depend_t dep = pkg->replaces[i];

            Id replacesId = dep2id(pool, &dep);
            opkg_msg(DEBUG2, "%s replaces %s\n", pkg->name,
                     pool_dep2str(pool, replacesId));
            solvable_out->obsoletes = repo_addid_dep(repo,
                solvable_out->obsoletes, replacesId, 0);
        }
    }
}

static void populate_installed_repo(libsolv_solver_t *libsolv_solver)
{
    int i;
    Id what;

    pkg_vec_t *installed_pkgs = pkg_vec_alloc();

    pkg_hash_fetch_all_installed(installed_pkgs, INSTALLED_HALF_INSTALLED);

    for (i = 0; i < installed_pkgs->len; i++) {
        pkg_t *pkg = installed_pkgs->pkgs[i];

        char *version = pkg_version_str_alloc(pkg);

        opkg_message(DEBUG2, "Installed package: %s - %s\n",
                     pkg->name, version);

        free(version);

        /* add a new solvable to the installed packages repo */
        Id solvable_id = repo_add_solvable(libsolv_solver->repo_installed);
        Solvable *solvable = pool_id2solvable(libsolv_solver->pool,
                                              solvable_id);

        /* set solvable attributes */
        pkg2solvable(pkg, solvable);

        /* if the package is in ignore-recommends-list, disfavor installation */
        if (str_list_contains(&opkg_config->ignore_recommends_list, pkg->name)) {
            opkg_message(NOTICE, "Disfavor package: %s\n",
                         pkg->name);
            what = pool_str2id(libsolv_solver->pool, pkg->name, 1);
            queue_push2(&libsolv_solver->solver_jobs, SOLVER_SOLVABLE_NAME
                        | SOLVER_DISFAVOR, what);
        }

        /* if the package is not autoinstalled, mark it as user installed */
        if (!pkg->auto_installed)
            queue_push2(&libsolv_solver->solver_jobs, SOLVER_SOLVABLE
                        | SOLVER_USERINSTALLED, solvable_id);
        else
            queue_push2(&libsolv_solver->solver_jobs, SOLVER_SOLVABLE
                        | SOLVER_ALLOWUNINSTALL, solvable_id);

        /* if the package is held, mark it as locked */
        if (pkg->state_flag & SF_HOLD)
            queue_push2(&libsolv_solver->solver_jobs, SOLVER_SOLVABLE
                        | SOLVER_LOCK, solvable_id);

        /* if the --force-depends option is specified, make dependencies weak */
        if (opkg_config->force_depends)
            queue_push2(&libsolv_solver->solver_jobs, SOLVER_SOLVABLE
                        | SOLVER_WEAKENDEPS, solvable_id);

        if (pkg->essential && !opkg_config->force_removal_of_essential_packages) {
            Id essential_pkg_id = pool_str2id(libsolv_solver->pool, pkg->name, 1);
            queue_push2(&libsolv_solver->solver_jobs, SOLVER_SOLVABLE_NAME
                        | SOLVER_INSTALL | SOLVER_ESSENTIAL, essential_pkg_id);
        }
    }

    pkg_vec_free(installed_pkgs);
}

static void populate_available_repos(libsolv_solver_t *libsolv_solver)
{
    int i;
    Solvable *solvable;
    Id solvable_id, what;

    pkg_vec_t *available_pkgs = pkg_vec_alloc();

    pkg_hash_fetch_available(available_pkgs);

    for (i = 0; i < available_pkgs->len; i++) {
        pkg_t *pkg = available_pkgs->pkgs[i];

        /* if the package is marked as excluded, skip it */
        if (str_list_contains(&opkg_config->exclude_list, pkg->name))
            continue;

        /* if the package is installed or unpacked, skip it */
        if (pkg->state_status == SS_INSTALLED ||
            pkg->state_status == SS_UNPACKED ||
            pkg->state_status == SS_HALF_INSTALLED)
            continue;

        /* if the package is marked as held, skip it */
        if (pkg->state_flag & SF_HOLD)
            continue;

        char *version = pkg_version_str_alloc(pkg);

        /* if the package is to be installed, create a solvable in
           repo_to_install and create a job to install it */
        if (pkg->state_want == SW_INSTALL) {
            opkg_message(DEBUG2, "Package marked for install: %s - %s\n",
                         pkg->name, version);
            /* If a package has state_want == SW_INSTALL and pkg->auto_installed == 1
             * it signals that it was meant to be installed as a dependency, but the
             * installation failed, probably because the Package index was out of date.
             * Create a non-version constrained solvable, since other package versions
             * may satisfy the dependency
             *
             * Bugzilla #8351
             */
            if (pkg->auto_installed == 1) {
                solvable_id = pool_str2id(libsolv_solver->pool, pkg->name, 1);
                queue_push2(&libsolv_solver->solver_jobs, SOLVER_SOLVABLE_PROVIDES
                            | SOLVER_INSTALL, solvable_id);
                free(version);
                continue;
            } else {
                solvable_id = repo_add_solvable(libsolv_solver->repo_to_install);
                queue_push2(&libsolv_solver->solver_jobs, SOLVER_SOLVABLE
                            | SOLVER_INSTALL | SOLVER_SETEVR, solvable_id);
            }
        }
        /* if the package is preferred, create a solvable in repo_preferred */
        else if (pkg->state_flag & SF_PREFER) {
            opkg_message(DEBUG2, "Preferred package: %s - %s\n",
                         pkg->name, version);
            solvable_id = repo_add_solvable(libsolv_solver->repo_preferred);
        }
        /* otherwise, create a solvable in repo_available */
        else {
            opkg_message(DEBUG2, "Available package: %s - %s\n",
                         pkg->name, version);
            solvable_id = repo_add_solvable(libsolv_solver->repo_available);
        }

        free(version);

        /* set solvable attributes using the package */
        solvable = pool_id2solvable(libsolv_solver->pool, solvable_id);
        pkg2solvable(pkg, solvable);

        /* if the package is in ignore-recommends-list, disfavor installation */
        if (str_list_contains(&opkg_config->ignore_recommends_list, pkg->name)) {
            opkg_message(NOTICE, "Disfavor package: %s\n",
                         pkg->name);
            what = pool_str2id(libsolv_solver->pool, pkg->name, 1);
            queue_push2(&libsolv_solver->solver_jobs, SOLVER_SOLVABLE_NAME
                        | SOLVER_DISFAVOR, what);
        }

        /* if the --force-depends option is specified make dependencies weak */
        if (opkg_config->force_depends)
            queue_push2(&libsolv_solver->solver_jobs, SOLVER_SOLVABLE
                        | SOLVER_WEAKENDEPS, solvable_id);
    }

    pkg_vec_free(available_pkgs);
}

static void printsolution_callback(struct _Pool *pool, void *data, int type, const char *str)
{
    opkg_message(ERROR, "%s\n", str);
}

static int libsolv_solver_init(libsolv_solver_t *libsolv_solver)
{
    /* initialize the solver job queue */
    queue_init(&libsolv_solver->solver_jobs);

    /* create the solvable pool and repos */
    libsolv_solver->pool = pool_create();
    libsolv_solver->repo_installed = repo_create(libsolv_solver->pool,
                                                 "@System");
    libsolv_solver->repo_available = repo_create(libsolv_solver->pool,
                                                 "@Available");
    libsolv_solver->repo_preferred = repo_create(libsolv_solver->pool,
                                                 "@Preferred");
    libsolv_solver->repo_to_install = repo_create(libsolv_solver->pool,
                                                  "@To_Install");

    /* set the repo priorities */
    libsolv_solver->repo_preferred->priority = PRIORITY_PREFERRED;
    libsolv_solver->repo_to_install->priority = PRIORITY_MARKED_FOR_INSTALL;

    /* set the architecture policy for the solver pool */
    libsolv_solver_set_arch_policy(libsolv_solver);

    /* set libsolv pool flags to match provides behavior of opkg.
       Obsoletes is libsolv's equivalent of replaces */
    pool_set_flag(libsolv_solver->pool, POOL_FLAG_OBSOLETEUSESPROVIDES, 1);
    pool_set_flag(libsolv_solver->pool,
                  POOL_FLAG_IMPLICITOBSOLETEUSESPROVIDES, 1);

    /* Use version matching that most closely matches debian */
    if (pool_setdisttype(libsolv_solver->pool, DISTTYPE_DEB) == -1) {
        opkg_message(ERROR, "libsolv not built with Debian or Multi semantics\n");
        return -1;
    }

    /* read in repo of installed packages */
    populate_installed_repo(libsolv_solver);

    /* set the installed repo */
    pool_set_installed(libsolv_solver->pool, libsolv_solver->repo_installed);

    /* read in available packages */
    populate_available_repos(libsolv_solver);

    /* create index of what each package provides */
    pool_createwhatprovides(libsolv_solver->pool);

    /* create the solver with the solver pool */
    libsolv_solver->solver = solver_create(libsolv_solver->pool);

    /* allow upgrades of installed packages during install */
    solver_set_flag(libsolv_solver->solver,
                    SOLVER_FLAG_INSTALL_ALSO_UPDATES, 1);

    /* set libsolv solver flags to match behavoir of opkg options */
    if (opkg_config->force_removal_of_dependent_packages)
        solver_set_flag(libsolv_solver->solver,
                        SOLVER_FLAG_ALLOW_UNINSTALL, 1);
    if (opkg_config->force_downgrade)
        solver_set_flag(libsolv_solver->solver,
                        SOLVER_FLAG_ALLOW_DOWNGRADE, 1);
    if (opkg_config->no_install_recommends) {
        solver_set_flag(libsolv_solver->solver,
                        SOLVER_FLAG_IGNORE_RECOMMENDED, 1);
    } else {
         solver_set_flag(libsolv_solver->solver,
                        SOLVER_FLAG_STRONG_RECOMMENDS, 1);
    }
    if (!opkg_config->prefer_arch_to_version) {
        solver_set_flag(libsolv_solver->solver,
                        SOLVER_FLAG_ALLOW_ARCHCHANGE, 1);
        solver_set_flag(libsolv_solver->solver,
                        SOLVER_FLAG_NO_INFARCHCHECK, 1);
    }

    /* Set callback to log solutions to error queue during solver_printsolution */
    pool_setdebugcallback(libsolv_solver->pool, printsolution_callback, NULL);

    return 0;
}

static libsolv_solver_t *libsolv_solver_new(void)
{
    libsolv_solver_t *libsolv_solver;
    int err;

    libsolv_solver = xcalloc(1, sizeof(libsolv_solver_t));
    err = libsolv_solver_init(libsolv_solver);
    if (err) {
        opkg_message(ERROR, "Could not initialize libsolv solver\n");
        libsolv_solver_free(libsolv_solver);
        return NULL;
    }

    return libsolv_solver;
}

static void libsolv_solver_add_job(libsolv_solver_t *libsolv_solver,
                                   job_action_t action, const char *pkg_name,
                                   const char *pkg_version,
                                   version_constraint_t constraint)
{
    Id what = 0;
    Id how = 0;

    if (pkg_version) {
        what = pool_rel2id(libsolv_solver->pool,
                           pool_str2id(libsolv_solver->pool, pkg_name, 1),
                           pool_str2id(libsolv_solver->pool, pkg_version, 1),
                           constraint_to_solv_flags(constraint), 1);
    } else {
        what = pool_str2id(libsolv_solver->pool, pkg_name, 1);
    }

    switch (action) {
    case JOB_INSTALL:
        how = SOLVER_INSTALL | SOLVER_SOLVABLE_PROVIDES;
        break;
    case JOB_REMOVE:
        /* Only remove packages by real package name -- matches internalsolv */
        how = SOLVER_ERASE | SOLVER_SOLVABLE_NAME;
        break;
    case JOB_UPGRADE:
        if (pkg_name && strcmp(pkg_name, "") != 0) {
            how = SOLVER_UPDATE | SOLVER_SOLVABLE_PROVIDES | SOLVER_TARGETED;
        } else {
            how = SOLVER_UPDATE | SOLVER_SOLVABLE_REPO;
            what = libsolv_solver->pool->installed->repoid;
        }
        break;
    case JOB_DISTUPGRADE:
        if (pkg_name && strcmp(pkg_name, "") != 0) {
            how = SOLVER_DISTUPGRADE | SOLVER_SOLVABLE_PROVIDES | SOLVER_TARGETED;
        } else {
            how = SOLVER_DISTUPGRADE | SOLVER_SOLVABLE_ALL;
            queue_push2(&libsolv_solver->solver_jobs, SOLVER_DROP_ORPHANED | SOLVER_SOLVABLE_ALL, 0);
        }
        break;
    case JOB_NOOP:
    default:
        break;
    }

    if (opkg_config->autoremove)
        how |= SOLVER_CLEANDEPS;

    queue_push2(&libsolv_solver->solver_jobs, how, what);

    /* Given two packages, one which provides 'name' and one which is
       actually named 'name', prefer the latter. Regression for issue9533 */
    if ((how & SOLVER_SELECTMASK) == SOLVER_SOLVABLE_PROVIDES) {
        queue_push2(&libsolv_solver->solver_jobs,
                    SOLVER_FAVOR | SOLVER_SOLVABLE_NAME, what);
    }
}

static int libsolv_solver_solve(libsolv_solver_t *libsolv_solver)
{
    int problem_count = solver_solve(libsolv_solver->solver,
                                     &libsolv_solver->solver_jobs);

    /* print out all problems and recommended solutions */
    if (problem_count) {
        opkg_message(ERROR, "Solver encountered %d problem(s):\n", problem_count);

        int problem;
        /* problems start at 1, not 0 */
        for (problem = 1; problem <= problem_count; problem++) {
            opkg_message(ERROR, "Problem %d/%d:\n", problem, problem_count);
            opkg_message(ERROR, "  - %s\n",
                         solver_problem2str(libsolv_solver->solver, problem));
            opkg_message(ERROR, "\n");

            int solution_count = solver_solution_count(libsolv_solver->solver,
                                                       problem);
            int solution;

            /* solutions also start from 1 */
            for (solution = 1; solution <= solution_count; solution++) {
                opkg_message(ERROR, "Solution %d:\n", solution);
                solver_printsolution(libsolv_solver->solver, problem, solution);
                opkg_message(NOTICE, "\n");
            }
        }
    }

    return problem_count;
}

static void libsolv_solver_free(libsolv_solver_t *libsolv_solver)
{
    if (libsolv_solver->solver)
        solver_free(libsolv_solver->solver);
    queue_free(&libsolv_solver->solver_jobs);
    pool_free(libsolv_solver->pool);
    free(libsolv_solver);
}

static int requires_download(Id typeId)
{
    return typeId == SOLVER_TRANSACTION_INSTALL || typeId == SOLVER_TRANSACTION_UPGRADE ||
                     typeId == SOLVER_TRANSACTION_DOWNGRADE || typeId == SOLVER_TRANSACTION_REINSTALL ||
                     typeId == SOLVER_TRANSACTION_MULTIINSTALL;
}

static int libsolv_solver_transaction_preamble(libsolv_solver_t *libsolv_solver, pkg_vec_t *pkgs, Transaction *transaction, int no_action)
{
    pkg_t *pkg;
    int i;

    /* order the transaction so dependencies are handled first */
    transaction_order(transaction, 0);

    for (i = 0; i < transaction->steps.count; i++) {
        Id stepId = transaction->steps.elements[i];
        Solvable *solvable = pool_id2solvable(libsolv_solver->pool, stepId);
        Id typeId = transaction_type(transaction, stepId,
                SOLVER_TRANSACTION_SHOW_ACTIVE |
                SOLVER_TRANSACTION_CHANGE_IS_REINSTALL |
                SOLVER_TRANSACTION_SHOW_OBSOLETES);

        const char *pkg_name = pool_id2str(libsolv_solver->pool, solvable->name);
        const char *evr = pool_id2str(libsolv_solver->pool, solvable->evr);
        const char *arch = pool_id2str(libsolv_solver->pool, solvable->arch);

        pkg = pkg_hash_fetch_by_name_version_arch(pkg_name, evr, arch);
        pkg_vec_insert(pkgs, pkg);

        if (!no_action && pkg->local_filename == NULL &&
            opkg_config->download_first && requires_download(typeId)) {
            if (opkg_download_pkg(pkg)) {
                opkg_msg(ERROR,
                         "Failed to download %s. "
                         "Perhaps you need to run 'opkg update'?\n", pkg->name);
                return -1;
            }
        }
    }

    return 0;
}

static int libsolv_solver_execute_transaction(libsolv_solver_t *libsolv_solver)
{
    int i, ret = 0, err = 0;
    Transaction *transaction;
    pkg_vec_t *pkgs;

    transaction = solver_create_transaction(libsolv_solver->solver);
    pkgs = pkg_vec_alloc();

    if (!transaction->steps.count) {
        opkg_message(NOTICE, "No packages installed or removed.\n");
    } else {
        pkg_t *pkg;

        if (libsolv_solver_transaction_preamble(libsolv_solver, pkgs, transaction, 0)) {
            err = -1;
            goto CLEANUP;
        }

        for (i = 0; i < transaction->steps.count; i++) {
            Id stepId = transaction->steps.elements[i];
            Id typeId = transaction_type(transaction, stepId,
                    SOLVER_TRANSACTION_SHOW_ACTIVE |
                    SOLVER_TRANSACTION_CHANGE_IS_REINSTALL |
                    SOLVER_TRANSACTION_SHOW_OBSOLETES);

            pkg = pkgs->pkgs[i];
            pkg_t *old, *obs = NULL;

            Id decision_rule;

            switch (typeId) {
            case SOLVER_TRANSACTION_ERASE:
                ret = opkg_remove_pkg(pkg);
                if (ret) {
                    err = -1;
                    goto CLEANUP;
                }
                break;
            case SOLVER_TRANSACTION_OBSOLETES:
                /* Replaces operations are expressed in two steps: the first one is a SOLVER_TRANSACTION_OBSOLETES, with the name of
                 * the replacer package. The second one is a SOLVER_TRANSACTION_IGNORE, with the name of the replacee */
                obs = pkgs->pkgs[i + 1];
                ret = opkg_remove_pkg(obs);
                if (ret) {
                    err = -1;
                    goto CLEANUP;
                }
            case SOLVER_TRANSACTION_DOWNGRADE:
            case SOLVER_TRANSACTION_REINSTALL:
            case SOLVER_TRANSACTION_INSTALL:
            case SOLVER_TRANSACTION_MULTIINSTALL:
                 solver_describe_decision(libsolv_solver->solver, stepId,
                                          &decision_rule);

                /* If a package is not explicitly installed by a job,
                mark it as autoinstalled. */
                if (solver_ruleclass(libsolv_solver->solver, decision_rule)
                                    != SOLVER_RULE_JOB)
                    pkg->auto_installed = 1;

                if (pkg->dest == NULL)
                    pkg->dest = opkg_config->default_dest;

                if (!opkg_config->download_only) {
                    opkg_message(NOTICE, "Installing %s (%s) on %s\n",
                                 pkg->name, pkg->version, pkg->dest->name);
                }
                ret = opkg_install_pkg(pkg);
                if (ret) {
                    err = -1;
                    goto CLEANUP;
                }
                break;
            case SOLVER_TRANSACTION_UPGRADE:
                old = pkg_hash_fetch_installed_by_name(pkg->name);
                /* if an old version was found set the new package's
                   autoinstalled status to that of the old package. */
                if (old) {
                    pkg->auto_installed = old->auto_installed;
                    pkg->dest = old->dest;
                    if (pkg->dest == NULL)
                        pkg->dest = opkg_config->default_dest;

                    if (!opkg_config->download_only) {
                        char *old_version = pkg_version_str_alloc(old);

                        opkg_message(NOTICE, "Upgrading %s from %s to %s on %s\n",
                                     pkg->name, old_version, pkg->version, pkg->dest->name);
                        free(old_version);
                    }
                } else {
                    if (pkg->dest == NULL)
                        pkg->dest = opkg_config->default_dest;
                    if (!opkg_config->download_only) {
                        opkg_message(NOTICE, "Upgrading %s to %s on %s\n",
                                     pkg->name, pkg->version, pkg->dest->name);
                    }
                }

                ret = opkg_install_pkg(pkg);
                if (ret) {
                    err = -1;
                    goto CLEANUP;
                }
                break;
            case SOLVER_TRANSACTION_IGNORE:
            default:
                break;
            }
        }
    }

CLEANUP:
    pkg_vec_free(pkgs);
    transaction_free(transaction);
    return err;
}

static int libsolv_solver_print_transaction(libsolv_solver_t *libsolv_solver)
{
    int i;
    Transaction *transaction;
    pkg_vec_t *pkgs;

    transaction = solver_create_transaction(libsolv_solver->solver);
    pkgs = pkg_vec_alloc();

    if (transaction->steps.count) {
        pkg_t *pkg;

        if (libsolv_solver_transaction_preamble(libsolv_solver, pkgs, transaction, 1))
            goto CLEANUP;

        for (i = 0; i < transaction->steps.count; i++) {
            Id stepId = transaction->steps.elements[i];
            Id typeId = transaction_type(transaction, stepId,
                    SOLVER_TRANSACTION_SHOW_ACTIVE |
                    SOLVER_TRANSACTION_CHANGE_IS_REINSTALL);

            pkg = pkgs->pkgs[i];
            pkg_t *old = 0;
            char *old_v, *new_v;

            switch (typeId) {
            case SOLVER_TRANSACTION_UPGRADE:
                old = pkg_hash_fetch_installed_by_name(pkg->name);
                new_v = pkg_version_str_alloc(pkg);
                old_v = pkg_version_str_alloc(old);
                printf("%s - %s - %s\n", pkg->name, old_v, new_v);
                free(new_v);
                free(old_v);
                break;
            default:
                break;
            }
        }
    }

CLEANUP:
    pkg_vec_free(pkgs);
    transaction_free(transaction);
    return 0;
}

char *opkg_solver_version_alloc(void)
{
    char *version;

    sprintf_alloc(&version, "libsolv %s", solv_version);
    return version;
}
