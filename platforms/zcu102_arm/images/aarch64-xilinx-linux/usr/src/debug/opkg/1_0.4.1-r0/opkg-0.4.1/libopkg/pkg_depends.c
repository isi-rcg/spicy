/* vi: set expandtab sw=4 sts=4: */
/* pkg_depends.c - the opkg package management system

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

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "pkg.h"
#include "opkg_conf.h"
#include "opkg_utils.h"
#include "pkg_hash.h"
#include "opkg_message.h"
#include "pkg_parse.h"
#include "hash_table.h"
#include "xfuncs.h"

static int parseDepends(compound_depend_t * compound_depend,
                        const char *depend_str);
static depend_t *depend_init(void);

int version_constraints_satisfied(depend_t * depends, pkg_t * pkg)
{
    pkg_t *temp;
    int comparison;

    if (depends->constraint == NONE)
        return 1;

    temp = pkg_new();

    parse_version(temp, depends->version);

    comparison = pkg_compare_versions(pkg, temp);

    pkg_deinit(temp);
    free(temp);

    if ((depends->constraint == EARLIER) && (comparison < 0))
        return 1;
    else if ((depends->constraint == LATER) && (comparison > 0))
        return 1;
    else if ((depends->constraint == EQUAL) && (comparison == 0))
        return 1;
    else if ((depends->constraint == LATER_EQUAL) && (comparison >= 0))
        return 1;
    else if ((depends->constraint == EARLIER_EQUAL) && (comparison <= 0))
        return 1;

    return 0;
}

int pkg_constraint_satisfied(pkg_t *pkg, void *cdata)
{
    depend_t *depend = (depend_t *) cdata;
    return version_constraints_satisfied(depend, pkg);
}

int pkg_dependence_satisfiable(depend_t * depend)
{
    abstract_pkg_t *apkg = depend->pkg;
    abstract_pkg_vec_t *provider_apkgs = apkg->provided_by;
    int n_providers = provider_apkgs->len;
    abstract_pkg_t **apkgs = provider_apkgs->pkgs;
    pkg_vec_t *pkg_vec;
    int n_pkgs;
    int i;
    int j;

    for (i = 0; i < n_providers; i++) {
        abstract_pkg_t *papkg = apkgs[i];
        pkg_vec = papkg->pkgs;
        if (pkg_vec) {
            n_pkgs = pkg_vec->len;
            for (j = 0; j < n_pkgs; j++) {
                pkg_t *pkg = pkg_vec->pkgs[j];
                if (version_constraints_satisfied(depend, pkg)) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

int is_pkg_in_pkg_vec(pkg_vec_t * vec, pkg_t * pkg)
{
    unsigned int i;
    pkg_t **pkgs = vec->pkgs;

    for (i = 0; i < vec->len; i++) {
        int match = (strcmp(pkg->name, pkgs[i]->name) == 0)
                && (pkg_compare_versions(pkg, pkgs[i]) == 0)
                && (strcmp(pkg->architecture, pkgs[i]->architecture) == 0);
        if (match)
            return 1;
    }
    return 0;
}

/**
 * pkg_replaces returns 1 if pkg->replaces contains one of replacee's provides and 0
 * otherwise.
 */
int pkg_replaces(pkg_t * pkg, pkg_t * replacee)
{
    compound_depend_t *replaces = pkg->replaces;
    int replaces_count = pkg->replaces_count;
    int replacee_provides_count = replacee->provides_count;
    int i, j;
    for (i = 0; i < replaces_count; i++) {
        /* Replaces field doesn't support or'ed conditions */
        abstract_pkg_t *abstract_replacee = replaces[i].possibilities[0]->pkg;
        for (j = 0; j < replacee_provides_count; j++) {
            if (replacee->provides[j] == abstract_replacee)
                return 1;
        }
    }
    return 0;
}

/**
 * pkg_conflicts_abstract returns 1 if pkg->conflicts contains conflictee and 0
 * otherwise.
 */
int pkg_conflicts_abstract(pkg_t * pkg, abstract_pkg_t * conflictee)
{
    compound_depend_t *conflicts = pkg->conflicts;
    int conflicts_count = pkg->conflicts_count;
    int i, j;
    for (i = 0; i < conflicts_count; i++) {
        int possibility_count = conflicts[i].possibility_count;
        struct depend **possibilities = conflicts[i].possibilities;
        for (j = 0; j < possibility_count; j++) {
            if (possibilities[j]->pkg == conflictee) {
                return 1;
            }
        }
    }
    return 0;
}

/**
 * pkg_conflicts returns 1 if pkg->conflicts contains one of
 * conflictee's provides and 0 otherwise.
 */
int pkg_conflicts(pkg_t * pkg, pkg_t * conflictee)
{
    compound_depend_t *conflicts = pkg->conflicts;
    int conflicts_count = pkg->conflicts_count;
    abstract_pkg_t **conflictee_provides = conflictee->provides;
    int conflictee_provides_count = conflictee->provides_count;
    int i, j, k;
    int possibility_count;
    struct depend **possibilities;
    abstract_pkg_t *possibility;

    for (i = 0; i < conflicts_count; i++) {
        possibility_count = conflicts[i].possibility_count;
        possibilities = conflicts[i].possibilities;
        for (j = 0; j < possibility_count; j++) {
            possibility = possibilities[j]->pkg;
            for (k = 0; k < conflictee_provides_count; k++) {
                int is_conflict = (possibility == conflictee_provides[k])
                        && version_constraints_satisfied(possibilities[j],
                                                         conflictee);
                if (is_conflict)
                    return 1;
            }
        }
    }
    return 0;
}

/**
 * pkg_breaks_reverse_dep returns 1 if pkg does not satisfy the version
 * constraints of the packages which depend upon pkg and otherwise returns 0.
 */
int pkg_breaks_reverse_dep(pkg_t * pkg)
{
    /* We consider only the abstract_pkg_t to which pkg belongs (ie. that which
     * shares its name) not the abstract pkgs which it provides as dependence on
     * a virtual package should never involve a version constraint.
     */
    abstract_pkg_t *rev_dep;
    abstract_pkg_t *apkg = pkg->parent;
    unsigned int i = 0;
    unsigned int j, k, m;
    unsigned int n_rev_deps;

    n_rev_deps = apkg->depended_upon_by->len;
    for (i = 0; i < n_rev_deps; i++) {
        rev_dep = apkg->depended_upon_by->pkgs[i];
        unsigned int npkgs = rev_dep->pkgs->len;

        for (j = 0; j < npkgs; j++) {
            pkg_t *cmp_pkg = rev_dep->pkgs->pkgs[j];
            compound_depend_t *cdeps = cmp_pkg->depends;
            unsigned int ncdeps = cmp_pkg->depends_count;

            /* Only check dependencies of a package which either will be
             * installed or will remain installed.
             */
            if (cmp_pkg->state_want != SW_INSTALL)
                continue;

            /* Find the dependence on apkg. */
            for (k = 0; k < ncdeps; k++) {
                depend_t **deps = cdeps[k].possibilities;
                unsigned int ndeps = cdeps[k].possibility_count;

                /* Skip recommends and suggests. */
                if (cdeps[k].type != PREDEPEND && cdeps[k].type != DEPEND)
                    continue;

                for (m = 0; m < ndeps; m++) {
                    /* Is this the dependence on apkg? */
                    if (deps[m]->pkg != apkg)
                        continue;

                    if (!version_constraints_satisfied(deps[m], pkg)) {
                        opkg_msg(DEBUG,
                                 "Installing %s %s would break reverse dependency from %s.\n",
                                 pkg->name, pkg->version, cmp_pkg->name);
                        return 1;
                    }
                }
            }
        }
    }

    return 0;
}

void buildProvides(abstract_pkg_t * ab_pkg, pkg_t * pkg)
{
    unsigned int i;

    /* every pkg provides itself */
    pkg->provides_count++;
    if (!abstract_pkg_vec_contains(ab_pkg->provided_by, ab_pkg))
        abstract_pkg_vec_insert(ab_pkg->provided_by, ab_pkg);
    pkg->provides = xcalloc(pkg->provides_count, sizeof(abstract_pkg_t *));
    pkg->provides[0] = ab_pkg;

    for (i = 1; i < pkg->provides_count; i++) {
        char* provides = trim_xstrdup(pkg->provides_str[i-1]);
        abstract_pkg_t *provided_abpkg = ensure_abstract_pkg_by_name(provides);
        free(pkg->provides_str[i - 1]);
        free(provides);

        pkg->provides[i] = provided_abpkg;

        abstract_pkg_vec_insert(provided_abpkg->provided_by, ab_pkg);
    }
    free(pkg->provides_str);
}

void buildConflicts(pkg_t * pkg)
{
    unsigned int i;
    compound_depend_t *conflicts;

    if (!pkg->conflicts_count)
        return;

    conflicts = pkg->conflicts = xcalloc(pkg->conflicts_count,
            sizeof(compound_depend_t));
    for (i = 0; i < pkg->conflicts_count; i++) {
        parseDepends(conflicts, pkg->conflicts_str[i]);
        conflicts->type = CONFLICTS;
        free(pkg->conflicts_str[i]);
        conflicts++;
    }
    free(pkg->conflicts_str);
}

void buildReplaces(abstract_pkg_t * ab_pkg, pkg_t * pkg)
{
    unsigned int i;
    compound_depend_t *replaces;

    if (!pkg->replaces_count)
        return;

    replaces = pkg->replaces = xcalloc(pkg->replaces_count,
            sizeof(compound_depend_t));

    for (i = 0; i < pkg->replaces_count; i++) {
        parseDepends(replaces, pkg->replaces_str[i]);
        replaces->type = REPLACES;
        free(pkg->replaces_str[i]);

        /* Replaces field doesn't support or'ed conditions */
        abstract_pkg_t *old_abpkg = replaces->possibilities[0]->pkg;

        if (!old_abpkg->replaced_by)
            old_abpkg->replaced_by = abstract_pkg_vec_alloc();
        /* if a package pkg both replaces and conflicts old_abpkg,
         * then add it to the replaced_by vector so that old_abpkg
         * will be upgraded to ab_pkg automatically */
        if (pkg_conflicts_abstract(pkg, old_abpkg)) {
            if (!abstract_pkg_vec_contains(old_abpkg->replaced_by, ab_pkg))
                abstract_pkg_vec_insert(old_abpkg->replaced_by, ab_pkg);
        }
        replaces++;
    }
    free(pkg->replaces_str);
}

void buildDepends(pkg_t * pkg)
{
    unsigned int count;
    unsigned int i;
    compound_depend_t *depends;

    count = pkg->pre_depends_count + pkg->depends_count + pkg->recommends_count
            + pkg->suggests_count;
    if (!count)
        return;

    depends = pkg->depends = xcalloc(count, sizeof(compound_depend_t));

    for (i = 0; i < pkg->pre_depends_count; i++) {
        parseDepends(depends, pkg->pre_depends_str[i]);
        free(pkg->pre_depends_str[i]);
        depends->type = PREDEPEND;
        depends++;
    }
    free(pkg->pre_depends_str);

    for (i = 0; i < pkg->depends_count; i++) {
        parseDepends(depends, pkg->depends_str[i]);
        free(pkg->depends_str[i]);
        depends++;
    }
    free(pkg->depends_str);

    for (i = 0; i < pkg->recommends_count; i++) {
        parseDepends(depends, pkg->recommends_str[i]);
        free(pkg->recommends_str[i]);
        depends->type = RECOMMEND;
        depends++;
    }
    free(pkg->recommends_str);

    for (i = 0; i < pkg->suggests_count; i++) {
        parseDepends(depends, pkg->suggests_str[i]);
        free(pkg->suggests_str[i]);
        depends->type = SUGGEST;
        depends++;
    }
    free(pkg->suggests_str);
}

const char *constraint_to_str(version_constraint_t c)
{
    switch (c) {
    case NONE:
        return "";
    case EARLIER:
        return "<< ";
    case EARLIER_EQUAL:
        return "<= ";
    case EQUAL:
        return "= ";
    case LATER_EQUAL:
        return ">= ";
    case LATER:
        return ">> ";
    }

    return "";
}

version_constraint_t str_to_constraint(const char **str)
{
    if (strncmp(*str, "<<", 2) == 0) {
        *str += 2;
        return EARLIER;
    } else if (strncmp(*str, "<=", 2) == 0) {
        *str += 2;
        return EARLIER_EQUAL;
    } else if (strncmp(*str, ">=", 2) == 0) {
        *str += 2;
        return LATER_EQUAL;
    } else if (strncmp(*str, ">>", 2) == 0) {
        *str += 2;
        return LATER;
    } else if (strncmp(*str, "=", 1) == 0) {
        *str += 1;
        return EQUAL;
    }
    /* should these be here to support deprecated designations; dpkg does */
    else if (strncmp(*str, "<", 1) == 0) {
        *str += 1;
        opkg_msg(NOTICE,
                 "Deprecated version constraint '<' was used with the same "
                 "meaning as '<='. Use '<<' for EARLIER constraint.\n");
        return EARLIER_EQUAL;
    } else if (strncmp(*str, ">", 1) == 0) {
        *str += 1;
        opkg_msg(NOTICE,
                 "Deprecated version constraint '>' was used with the same "
                 "meaning as '>='. Use '>>' for LATER constraint.\n");
        return LATER_EQUAL;
    } else {
        return NONE;
    }
}

void strip_pkg_name_and_version(const char *pkg_name, char **name, char **version,
                                version_constraint_t *constraint)
{
    const char *tmp;

    if (!pkg_name) {
        *version = NULL;
        *name = NULL;
        return;
    }

    tmp = strpbrk(pkg_name, "><=");

    if (tmp) {
        /* Capture and move past the constraint characters */
        const char *ver_tmp = tmp;
        *constraint = str_to_constraint(&ver_tmp);
        *version = xstrdup(ver_tmp);
        *name = xstrndup(pkg_name, tmp - pkg_name);
    } else {
        *version = NULL;
        *constraint = NONE;
        *name = xstrdup(pkg_name);
    }
}

/*
 * Returns a printable string for pkg's dependency at the specified idx. The
 * resultant string must be passed to free() by the caller.
 */
char *pkg_depend_str(pkg_t * pkg, int idx)
{
    int i;
    unsigned int len;
    char *str;
    compound_depend_t *cdep;
    depend_t *dep;

    len = 0;
    cdep = &pkg->depends[idx];

    /* calculate string length */
    for (i = 0; i < cdep->possibility_count; i++) {
        dep = cdep->possibilities[i];

        if (i != 0)
            len += 3;           /* space, pipe, space */

        len += strlen(dep->pkg->name);

        if (dep->version) {
            len += 2;           /* space, left parenthesis */
            len += 3;           /* constraint string (<=, >=, etc), space */
            len += strlen(dep->version);
            len += 1;           /* right parenthesis */
        }
    }

    str = xmalloc(len + 1);     /* +1 for the NULL terminator */
    str[0] = '\0';

    for (i = 0; i < cdep->possibility_count; i++) {
        dep = cdep->possibilities[i];

        if (i != 0)
            strncat(str, " | ", len);

        strncat(str, dep->pkg->name, len);

        if (dep->version) {
            strncat(str, " (", len);
            strncat(str, constraint_to_str(dep->constraint), len);
            strncat(str, dep->version, len);
            strncat(str, ")", len);
        }
    }

    return str;
}

void buildDependedUponBy(pkg_t * pkg, abstract_pkg_t * ab_pkg)
{
    compound_depend_t *depends;
    int count;
    int i, j;
    abstract_pkg_t *ab_depend;

    count = pkg->pre_depends_count + pkg->depends_count + pkg->recommends_count
        + pkg->suggests_count;

    for (i = 0; i < count; i++) {
        depends = &pkg->depends[i];
        int wrong_type = depends->type != PREDEPEND
                && depends->type != DEPEND
                && depends->type != RECOMMEND;
        if (wrong_type)
            continue;
        for (j = 0; j < depends->possibility_count; j++) {
            ab_depend = depends->possibilities[j]->pkg;
            if (!abstract_pkg_vec_contains(ab_depend->depended_upon_by, ab_pkg))
                abstract_pkg_vec_insert(ab_depend->depended_upon_by, ab_pkg);
        }
    }
}

static depend_t *depend_init(void)
{
    depend_t *d = xcalloc(1, sizeof(depend_t));
    d->constraint = NONE;
    d->version = NULL;
    d->pkg = NULL;

    return d;
}

static int parseDepends(compound_depend_t * compound_depend,
                        const char *depend_str)
{
    char *pkg_name, buffer[2048];
    unsigned int num_of_ors = 0;
    unsigned int i;
    const char *src;
    char *dest;
    depend_t **possibilities;

    /* first count the number of ored possibilities for satisfying dependency */
    src = depend_str;
    while (*src)
        if (*src++ == '|')
            num_of_ors++;

    compound_depend->type = DEPEND;

    compound_depend->possibility_count = num_of_ors + 1;
    possibilities = xcalloc((num_of_ors + 1), sizeof(depend_t *));
    compound_depend->possibilities = possibilities;

    src = depend_str;
    for (i = 0; i < num_of_ors + 1; i++) {
        possibilities[i] = depend_init();
        /* gobble up just the name first */
        dest = buffer;
        while (*src && !isspace(*src) && (*src != '(') && (*src != '*')
               && (*src != '|'))
            *dest++ = *src++;
        *dest = '\0';
        pkg_name = trim_xstrdup(buffer);

        /* now look at possible version info */

        /* skip to next chars */
        if (isspace(*src))
            while (*src && isspace(*src))
                src++;

        /* extract constraint and version */
        if (*src == '(') {
            src++;
            possibilities[i]->constraint = str_to_constraint(&src);

            /* now we have any constraint, pass space to version string */
            while (isspace(*src))
                src++;

            /* this would be the version string */
            dest = buffer;
            while (*src && *src != ')')
                *dest++ = *src++;
            *dest = '\0';

            possibilities[i]->version = trim_xstrdup(buffer);
        }
        /* hook up the dependency to its abstract pkg */
        possibilities[i]->pkg = ensure_abstract_pkg_by_name(pkg_name);

        free(pkg_name);

        /* now get past the ) and any possible | chars */
        while (*src && (isspace(*src) || (*src == ')') || (*src == '|')))
            src++;
        if (*src == '*') {
            compound_depend->type = GREEDY_DEPEND;
            src++;
        }
    }

    return 0;
}
