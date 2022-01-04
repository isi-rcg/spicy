/*
 * Copyright (C) 2012-2018 Red Hat, Inc.
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

#include <assert.h>
#include "hy-goal-private.hpp"
#include "hy-iutil-private.hpp"
#include "hy-query-private.hpp"
#include "hy-selector.h"
#include "sack/packageset.hpp"
#include "sack/query.hpp"
#include "repo/DependencySplitter.hpp"
#include "repo/solvable/Dependency.hpp"
#include "repo/solvable/DependencyContainer.hpp"
#include "goal/Goal.hpp"

Id
query_get_index_item(HyQuery query, int index)
{
    return query->getIndexItem(index);
}

void
hy_query_apply(HyQuery q)
{
    q->apply();
}

HyQuery
hy_query_create(DnfSack *sack)
{
    return new libdnf::Query(sack);
}

HyQuery
hy_query_create_flags(DnfSack *sack, int flags)
{
    return new libdnf::Query(sack, flags);
}

HyQuery
hy_query_from_nevra(HyNevra nevra, DnfSack *sack, bool icase)
{
    HyQuery query = hy_query_create(sack);
    hy_add_filter_nevra_object(query, nevra, icase);
    return query;
}

void
hy_query_free(HyQuery q)
{
    delete q;
}

void
hy_query_clear(HyQuery q)
{
    q->clear();
}

HyQuery
hy_query_clone(HyQuery q)
{
    return new libdnf::Query(*q);
}

int
hy_query_filter(HyQuery q, int keyname, int cmp_type, const char *match)
{
    return q->addFilter(keyname, cmp_type, match);
}

int
hy_query_filter_empty(HyQuery q)
{
    return q->addFilter(HY_PKG_EMPTY, HY_EQ, 1);
}

int
hy_query_filter_in(HyQuery q, int keyname, int cmp_type,
                   const char **matches)
{
    return q->addFilter(keyname, cmp_type, matches);
}

int
hy_query_filter_num(HyQuery q, int keyname, int cmp_type, int match)
{
    return q->addFilter(keyname, cmp_type, match);
}

int
hy_query_filter_num_in(HyQuery q, int keyname, int cmp_type, int nmatches, const int *matches)
{
    return q->addFilter(keyname, cmp_type, nmatches, matches);
}

int
hy_query_filter_package_in(HyQuery q, int keyname, int cmp_type,
                           DnfPackageSet *pset)
{
    return q->addFilter(keyname, cmp_type, pset);
}

int
hy_query_filter_reldep(HyQuery q, int keyname, DnfReldep *reldep)
{
    return q->addFilter(keyname, reldep);
}

int
hy_query_filter_reldep_in(HyQuery q, int keyname, DnfReldepList *reldeplist)
{
    return q->addFilter( keyname, reldeplist);
}

int
hy_query_filter_provides(HyQuery q, int cmp_type, const char *name, const char *evr)
{
    libdnf::Dependency reldep(q->getSack(), name, evr, cmp_type);
    return q->addFilter(HY_PKG_PROVIDES, &reldep);
}

int
hy_query_filter_provides_in(HyQuery q, char **reldep_strs)
{
    libdnf::DependencyContainer reldeplist(q->getSack());
    for (int i = 0; reldep_strs[i] != NULL; ++i) {
        if (!reldeplist.addReldep(reldep_strs[i])) {
            return DNF_ERROR_BAD_QUERY;
        }
    }
    q->addFilter(HY_PKG_PROVIDES, &reldeplist);
    return 0;
}

/**
 * Narrows to only those installed packages for which there is a downgrading package.
 */
void
hy_query_filter_downgradable(HyQuery q, int val)
{
    q->addFilter(HY_PKG_DOWNGRADABLE, HY_EQ, val);
}

/**
 * Narrows to only packages downgrading installed packages.
 */
void
hy_query_filter_downgrades(HyQuery q, int val)
{
    q->addFilter(HY_PKG_DOWNGRADES, HY_EQ, val);
}

/**
 * Narrows to only those installed packages for which there is an updating package.
 */
void
hy_query_filter_upgradable(HyQuery q, int val)
{
    q->addFilter(HY_PKG_UPGRADABLE, HY_EQ, val);
}

/**
 * Narrows to only packages updating installed packages.
 */
void
hy_query_filter_upgrades(HyQuery q, int val)
{
    q->addFilter(HY_PKG_UPGRADES, HY_EQ, val);
}

/**
 * Narrows to only the highest version of a package per arch.
 */
void
hy_query_filter_latest_per_arch(HyQuery q, int val)
{
    q->addFilter(HY_PKG_LATEST_PER_ARCH, HY_EQ, val);
}

/**
 * Narrows to only the highest version of a package.
 */
void
hy_query_filter_latest(HyQuery q, int val)
{
    q->addFilter(HY_PKG_LATEST, HY_EQ, val);
}

GPtrArray *
hy_query_run(HyQuery q)
{
    return q->run();
}

DnfPackageSet *
hy_query_run_set(HyQuery q)
{
    return new libdnf::PackageSet(*q->runSet());
}

/**
 * hy_query_union:
 * @q:     a #HyQuery instance
 * @other: other #HyQuery instance
 *
 * Unites query with other query (aka logical or).
 *
 * Returns: Nothing.
 *
 * Since: 0.7.0
 */
void
hy_query_union(HyQuery q, HyQuery other)
{
    q->queryUnion(*other);
}

/**
 * hy_query_intersection:
 * @q:     a #HyQuery instance
 * @other: other #HyQuery instance
 *
 * Intersects query with other query (aka logical and).
 *
 * Returns: Nothing.
 *
 * Since: 0.7.0
 */
void
hy_query_intersection(HyQuery q, HyQuery other)
{
    q->queryIntersection(*other);
}

/**
 * hy_query_difference:
 * @q:     a #HyQuery instance
 * @other: other #HyQuery instance
 *
 * Computes difference between query and other query (aka q and not other).
 *
 * Returns: Nothing.
 *
 * Since: 0.7.0
 */
void
hy_query_difference(HyQuery q, HyQuery other)
{
    q->queryDifference(*other);
}

bool
hy_query_is_empty(HyQuery query)
{
    return query->empty();
}

bool
hy_query_is_applied(const HyQuery query)
{
    return query->getApplied();
}

const Map *
hy_query_get_result(const HyQuery query)
{
    return query->getResult();
}

DnfSack *
hy_query_get_sack(HyQuery query)
{
    return query->getSack();
}

void
hy_add_filter_nevra_object(HyQuery query, HyNevra nevra, bool icase)
{
    query->addFilter(nevra, icase);
}

void
hy_add_filter_extras(HyQuery query)
{
    query->filterExtras();
}

void
hy_filter_recent(HyQuery query, const long unsigned int recent_limit)
{
    query->filterRecent(recent_limit);
}

void
hy_filter_duplicated(HyQuery query)
{
    query->filterDuplicated();
}

int
hy_filter_unneeded(HyQuery query, const libdnf::Swdb & swdb, bool debug_solver)
{
    return query->filterUnneeded(swdb, debug_solver);
}

HySelector
hy_query_to_selector(HyQuery query)
{
    HySelector selector = hy_selector_create(query->getSack());
    DnfPackageSet *pset = hy_query_run_set(query);
    hy_selector_pkg_set(selector, pset);
    delete pset;
    return selector;
}
