#ifndef LIBICAL_ICALSS_H
#define LIBICAL_ICALSS_H
#ifndef S_SPLINT_S
#ifdef __cplusplus
extern "C" {
#endif
/*======================================================================
 FILE: icalgauge.h
 CREATOR: eric 23 December 1999
 (C) COPYRIGHT 2000, Eric Busboom <eric@softwarestudio.org>
     http://www.softwarestudio.org
 This library is free software; you can redistribute it and/or modify
 it under the terms of either:
    The LGPL as published by the Free Software Foundation, version
    2.1, available at: http://www.gnu.org/licenses/lgpl-2.1.html
 Or:
    The Mozilla Public License Version 2.0. You may obtain a copy of
    the License at http://www.mozilla.org/MPL/
 The Original Code is eric. The Initial Developer of the Original
 Code is Eric Busboom
======================================================================*/
#ifndef ICALGAUGE_H
#define ICALGAUGE_H
#include "libical_icalss_export.h"
/** @file icalgauge.h
 *  @brief Routines implementing a filter for ical components
 */
typedef struct icalgauge_impl icalgauge;
LIBICAL_ICALSS_EXPORT icalgauge *icalgauge_new_from_sql(const char *sql, int expand);
/**
 * Returns the expand value for the specified icalgauge.
 * If @p gauge is NULL a value of -1 is returned.
 */
LIBICAL_ICALSS_EXPORT int icalgauge_get_expand(icalgauge *gauge);
LIBICAL_ICALSS_EXPORT void icalgauge_free(icalgauge *gauge);
LIBICAL_ICALSS_EXPORT void icalgauge_dump(icalgauge *gauge);
/** @brief Return true if comp matches the gauge.
 *
 * The component must be in
 * cannonical form -- a VCALENDAR with one VEVENT, VTODO or VJOURNAL
 * sub component
 */
LIBICAL_ICALSS_EXPORT int icalgauge_compare(icalgauge *g, icalcomponent *comp);
#endif /* ICALGAUGE_H */
/**
 @file icalset.h
 @author eric 28 November 1999
 Icalset is the "base class" for representations of a collection of
 iCal components. Derived classes (actually delegatees) include:
    icalfileset   Store components in a single file
    icaldirset    Store components in multiple files in a directory
    icalbdbset    Store components in a Berkeley DB File
    icalheapset   Store components on the heap
    icalmysqlset  Store components in a mysql database.
**/
/*
 (C) COPYRIGHT 2000, Eric Busboom <eric@softwarestudio.org>
 This library is free software; you can redistribute it and/or modify
 it under the terms of either:
    The LGPL as published by the Free Software Foundation, version
    2.1, available at: http://www.gnu.org/licenses/lgpl-2.1.html
 Or:
    The Mozilla Public License Version 2.0. You may obtain a copy of
    the License at http://www.mozilla.org/MPL/
 The Original Code is eric. The Initial Developer of the Original
 Code is Eric Busboom
======================================================================*/
#ifndef ICALSET_H
#define ICALSET_H
#include "libical_icalss_export.h"
typedef struct icalset_impl icalset;
typedef enum icalset_kind
{
    ICAL_FILE_SET,
    ICAL_DIR_SET,
    ICAL_BDB_SET
} icalset_kind;
typedef struct icalsetiter
{
    icalcompiter iter;  /* icalcomponent_kind, pvl_elem iter */
    icalgauge *gauge;
    icalrecur_iterator *ritr;   /*the last iterator */
    icalcomponent *last_component;      /*the pending recurring component to be processed  */
    const char *tzid;   /* the calendar's timezone id */
} icalsetiter;
struct icalset_impl
{
    icalset_kind kind;
    size_t size;
    char *dsn;
    icalset *(*init) (icalset *set, const char *dsn, void *options);
    void (*free) (icalset *set);
    const char *(*path) (icalset *set);
    void (*mark) (icalset *set);
    icalerrorenum(*commit) (icalset *set);
    icalerrorenum(*add_component) (icalset *set, icalcomponent *comp);
    icalerrorenum(*remove_component) (icalset *set, icalcomponent *comp);
    int (*count_components) (icalset *set, icalcomponent_kind kind);
    icalerrorenum(*select) (icalset *set, icalgauge *gauge);
    void (*clear) (icalset *set);
    icalcomponent *(*fetch) (icalset *set, icalcomponent_kind kind, const char *uid);
    icalcomponent *(*fetch_match) (icalset *set, icalcomponent *comp);
    int (*has_uid) (icalset *set, const char *uid);
    icalerrorenum(*modify) (icalset *set, icalcomponent *old, icalcomponent *newc);
    icalcomponent *(*get_current_component) (icalset *set);
    icalcomponent *(*get_first_component) (icalset *set);
    icalcomponent *(*get_next_component) (icalset *set);
    icalsetiter(*icalset_begin_component) (icalset *set,
                                           icalcomponent_kind kind, icalgauge *gauge,
                                           const char *tzid);
    icalcomponent *(*icalsetiter_to_next) (icalset *set, icalsetiter *i);
    icalcomponent *(*icalsetiter_to_prior) (icalset *set, icalsetiter *i);
};
/** @brief Register a new derived class */
LIBICAL_ICALSS_EXPORT int icalset_register_class(icalset *set);
/** @brief Generic icalset constructor
 *
 * @param kind     The type of icalset to create
 * @param dsn      Data Source Name - usually a pathname or DB handle
 * @param options  Any implementation specific options
 *
 * @return         A valid icalset reference or NULL if error.
 *
 * This creates any of the icalset types available.
 */
LIBICAL_ICALSS_EXPORT icalset *icalset_new(icalset_kind kind, const char *dsn, void *options);
LIBICAL_ICALSS_EXPORT icalset *icalset_new_file(const char *path);
LIBICAL_ICALSS_EXPORT icalset *icalset_new_file_reader(const char *path);
LIBICAL_ICALSS_EXPORT icalset *icalset_new_file_writer(const char *path);
LIBICAL_ICALSS_EXPORT icalset *icalset_new_dir(const char *path);
LIBICAL_ICALSS_EXPORT void icalset_free(icalset *set);
LIBICAL_ICALSS_EXPORT const char *icalset_path(icalset *set);
/** Mark the cluster as changed, so it will be written to disk when it
    is freed. **/
LIBICAL_ICALSS_EXPORT void icalset_mark(icalset *set);
/** Write changes to disk immediately */
LIBICAL_ICALSS_EXPORT icalerrorenum icalset_commit(icalset *set);
LIBICAL_ICALSS_EXPORT icalerrorenum icalset_add_component(icalset *set, icalcomponent *comp);
LIBICAL_ICALSS_EXPORT icalerrorenum icalset_remove_component(icalset *set, icalcomponent *comp);
LIBICAL_ICALSS_EXPORT int icalset_count_components(icalset *set, icalcomponent_kind kind);
/** Restrict the component returned by icalset_first, _next to those
    that pass the gauge. */
LIBICAL_ICALSS_EXPORT icalerrorenum icalset_select(icalset *set, icalgauge *gauge);
/** Get a component by uid */
LIBICAL_ICALSS_EXPORT icalcomponent *icalset_fetch(icalset *set, const char *uid);
LIBICAL_ICALSS_EXPORT int icalset_has_uid(icalset *set, const char *uid);
LIBICAL_ICALSS_EXPORT icalcomponent *icalset_fetch_match(icalset *set, icalcomponent *c);
/** Modify components according to the MODIFY method of CAP. Works on
   the currently selected components. */
LIBICAL_ICALSS_EXPORT icalerrorenum icalset_modify(icalset *set,
                                                   icalcomponent *oldc, icalcomponent *newc);
/** Iterate through the components. If a guage has been defined, these
   will skip over components that do not pass the gauge */
LIBICAL_ICALSS_EXPORT icalcomponent *icalset_get_current_component(icalset *set);
LIBICAL_ICALSS_EXPORT icalcomponent *icalset_get_first_component(icalset *set);
LIBICAL_ICALSS_EXPORT icalcomponent *icalset_get_next_component(icalset *set);
/** External Iterator with gauge - for thread safety */
LIBICAL_ICALSS_EXPORT extern icalsetiter icalsetiter_null;
LIBICAL_ICALSS_EXPORT icalsetiter icalset_begin_component(icalset *set,
                                                          icalcomponent_kind kind,
                                                          icalgauge *gauge, const char *tzid);
/** Default _next, _prior, _deref for subclasses that use single cluster */
LIBICAL_ICALSS_EXPORT icalcomponent *icalsetiter_next(icalsetiter *i);
LIBICAL_ICALSS_EXPORT icalcomponent *icalsetiter_prior(icalsetiter *i);
LIBICAL_ICALSS_EXPORT icalcomponent *icalsetiter_deref(icalsetiter *i);
/** for subclasses that use multiple clusters that require specialized cluster traversal */
LIBICAL_ICALSS_EXPORT icalcomponent *icalsetiter_to_next(icalset *set, icalsetiter *i);
LIBICAL_ICALSS_EXPORT icalcomponent *icalsetiter_to_prior(icalset *set, icalsetiter *i);
#endif /* !ICALSET_H */
/*======================================================================
 FILE: icalcluster.h
 CREATOR: acampi 13 March 2002
 Copyright (C) 2002 Andrea Campi <a.campi@inet.it>
 This library is free software; you can redistribute it and/or modify
 it under the terms of either:
    The LGPL as published by the Free Software Foundation, version
    2.1, available at: http://www.gnu.org/licenses/lgpl-2.1.html
 Or:
    The Mozilla Public License Version 2.0. You may obtain a copy of
    the License at http://www.mozilla.org/MPL/
======================================================================*/
#ifndef ICALCLUSTER_H
#define ICALCLUSTER_H
#include "libical_icalss_export.h"
typedef struct icalcluster_impl icalcluster;
LIBICAL_ICALSS_EXPORT icalcluster *icalcluster_new(const char *key, icalcomponent *data);
LIBICAL_ICALSS_EXPORT icalcluster *icalcluster_new_clone(const icalcluster *cluster);
LIBICAL_ICALSS_EXPORT void icalcluster_free(icalcluster *cluster);
LIBICAL_ICALSS_EXPORT const char *icalcluster_key(icalcluster *cluster);
LIBICAL_ICALSS_EXPORT int icalcluster_is_changed(icalcluster *cluster);
LIBICAL_ICALSS_EXPORT void icalcluster_mark(icalcluster *cluster);
LIBICAL_ICALSS_EXPORT void icalcluster_commit(icalcluster *cluster);
LIBICAL_ICALSS_EXPORT icalcomponent *icalcluster_get_component(icalcluster *cluster);
LIBICAL_ICALSS_EXPORT int icalcluster_count_components(icalcluster *cluster,
                                                       icalcomponent_kind kind);
LIBICAL_ICALSS_EXPORT icalerrorenum icalcluster_add_component(icalcluster *cluster,
                                                              icalcomponent *child);
LIBICAL_ICALSS_EXPORT icalerrorenum icalcluster_remove_component(icalcluster *cluster,
                                                                 icalcomponent *child);
LIBICAL_ICALSS_EXPORT icalcomponent *icalcluster_get_current_component(icalcluster *cluster);
LIBICAL_ICALSS_EXPORT icalcomponent *icalcluster_get_first_component(icalcluster *cluster);
LIBICAL_ICALSS_EXPORT icalcomponent *icalcluster_get_next_component(icalcluster *cluster);
#endif /* !ICALCLUSTER_H */
/*======================================================================
 FILE: icalfileset.h
 CREATOR: eric 23 December 1999
 (C) COPYRIGHT 2000, Eric Busboom <eric@softwarestudio.org>
 This library is free software; you can redistribute it and/or modify
 it under the terms of either:
    The LGPL as published by the Free Software Foundation, version
    2.1, available at: http://www.gnu.org/licenses/lgpl-2.1.html
 Or:
    The Mozilla Public License Version 2.0. You may obtain a copy of
    the License at http://www.mozilla.org/MPL/
 The Original Code is eric. The Initial Developer of the Original
 Code is Eric Busboom
======================================================================*/
#ifndef ICALFILESET_H
#define ICALFILESET_H
#include "libical_icalss_export.h"
typedef struct icalfileset_impl icalfileset;
LIBICAL_ICALSS_EXPORT icalset *icalfileset_new(const char *path);
LIBICAL_ICALSS_EXPORT icalset *icalfileset_new_reader(const char *path);
LIBICAL_ICALSS_EXPORT icalset *icalfileset_new_writer(const char *path);
LIBICAL_ICALSS_EXPORT icalset *icalfileset_init(icalset *set, const char *dsn, void *options);
LIBICAL_ICALSS_EXPORT icalcluster *icalfileset_produce_icalcluster(const char *path);
LIBICAL_ICALSS_EXPORT void icalfileset_free(icalset *cluster);
LIBICAL_ICALSS_EXPORT const char *icalfileset_path(icalset *cluster);
/* Mark the cluster as changed, so it will be written to disk when it
   is freed. Commit writes to disk immediately. */
LIBICAL_ICALSS_EXPORT void icalfileset_mark(icalset *set);
LIBICAL_ICALSS_EXPORT icalerrorenum icalfileset_commit(icalset *set);
LIBICAL_ICALSS_EXPORT icalerrorenum icalfileset_add_component(icalset *set, icalcomponent *child);
LIBICAL_ICALSS_EXPORT icalerrorenum icalfileset_remove_component(icalset *set,
                                                                 icalcomponent *child);
LIBICAL_ICALSS_EXPORT int icalfileset_count_components(icalset *set, icalcomponent_kind kind);
/**
 * Restrict the component returned by icalfileset_first, _next to those
 * that pass the gauge. _clear removes the gauge
 */
LIBICAL_ICALSS_EXPORT icalerrorenum icalfileset_select(icalset *set, icalgauge *gauge);
/** clear the gauge **/
LIBICAL_ICALSS_EXPORT void icalfileset_clear(icalset *set);
/** Get and search for a component by uid **/
LIBICAL_ICALSS_EXPORT icalcomponent *icalfileset_fetch(icalset *set,
                                                       icalcomponent_kind kind, const char *uid);
LIBICAL_ICALSS_EXPORT int icalfileset_has_uid(icalset *set, const char *uid);
LIBICAL_ICALSS_EXPORT icalcomponent *icalfileset_fetch_match(icalset *set, icalcomponent *c);
/**
 *  Modify components according to the MODIFY method of CAP. Works on the
 *  currently selected components.
 */
LIBICAL_ICALSS_EXPORT icalerrorenum icalfileset_modify(icalset *set,
                                                       icalcomponent *oldcomp,
                                                       icalcomponent *newcomp);
/* Iterate through components. If a gauge has been defined, these
   will skip over components that do not pass the gauge */
LIBICAL_ICALSS_EXPORT icalcomponent *icalfileset_get_current_component(icalset *cluster);
LIBICAL_ICALSS_EXPORT icalcomponent *icalfileset_get_first_component(icalset *cluster);
LIBICAL_ICALSS_EXPORT icalcomponent *icalfileset_get_next_component(icalset *cluster);
/* External iterator for thread safety */
LIBICAL_ICALSS_EXPORT icalsetiter icalfileset_begin_component(icalset *set,
                                                              icalcomponent_kind kind,
                                                              icalgauge *gauge, const char *tzid);
LIBICAL_ICALSS_EXPORT icalcomponent *icalfilesetiter_to_next(icalset *set, icalsetiter *iter);
LIBICAL_ICALSS_EXPORT icalcomponent *icalfileset_form_a_matched_recurrence_component(icalsetiter *
                                                                                     itr);
/** Return a reference to the internal component. You probably should
   not be using this. */
LIBICAL_ICALSS_EXPORT icalcomponent *icalfileset_get_component(icalset *cluster);
/**
 * @brief options for opening an icalfileset.
 *
 * These options should be passed to the icalset_new() function
 */
typedef struct icalfileset_options
{
    int flags;                /**< flags for open() O_RDONLY, etc  */
    int mode;                 /**< file mode */
    int safe_saves;           /**< to lock or not */
    icalcluster *cluster;     /**< use this cluster to initialize data */
} icalfileset_options;
extern icalfileset_options icalfileset_options_default;
#endif /* !ICALFILESET_H */
/*======================================================================
 FILE: icaldirset.h
 CREATOR: eric 28 November 1999
 (C) COPYRIGHT 2000, Eric Busboom <eric@softwarestudio.org>
     http://www.softwarestudio.org
 This library is free software; you can redistribute it and/or modify
 it under the terms of either:
    The LGPL as published by the Free Software Foundation, version
    2.1, available at: http://www.gnu.org/licenses/lgpl-2.1.html
 Or:
    The Mozilla Public License Version 2.0. You may obtain a copy of
    the License at http://www.mozilla.org/MPL/
 The Original Code is eric. The Initial Developer of the Original
 Code is Eric Busboom
======================================================================*/
#ifndef ICALDIRSET_H
#define ICALDIRSET_H
#include "libical_icalss_export.h"
/* icaldirset Routines for storing, fetching, and searching for ical
 * objects in a database */
typedef struct icaldirset_impl icaldirset;
LIBICAL_ICALSS_EXPORT icalset *icaldirset_new(const char *path);
LIBICAL_ICALSS_EXPORT icalset *icaldirset_new_reader(const char *path);
LIBICAL_ICALSS_EXPORT icalset *icaldirset_new_writer(const char *path);
LIBICAL_ICALSS_EXPORT icalset *icaldirset_init(icalset *set, const char *dsn, void *options);
LIBICAL_ICALSS_EXPORT void icaldirset_free(icalset *set);
LIBICAL_ICALSS_EXPORT const char *icaldirset_path(icalset *set);
/* Mark the cluster as changed, so it will be written to disk when it
   is freed. Commit writes to disk immediately*/
LIBICAL_ICALSS_EXPORT void icaldirset_mark(icalset *set);
LIBICAL_ICALSS_EXPORT icalerrorenum icaldirset_commit(icalset *set);
LIBICAL_ICALSS_EXPORT icalerrorenum icaldirset_add_component(icalset *store, icalcomponent *comp);
LIBICAL_ICALSS_EXPORT icalerrorenum icaldirset_remove_component(icalset *store,
                                                                icalcomponent *comp);
LIBICAL_ICALSS_EXPORT int icaldirset_count_components(icalset *store, icalcomponent_kind kind);
/* Restrict the component returned by icaldirset_first, _next to those
   that pass the gauge. _clear removes the gauge. */
LIBICAL_ICALSS_EXPORT icalerrorenum icaldirset_select(icalset *store, icalgauge *gauge);
LIBICAL_ICALSS_EXPORT void icaldirset_clear(icalset *store);
/* Get a component by uid */
LIBICAL_ICALSS_EXPORT icalcomponent *icaldirset_fetch(icalset *store,
                                                      icalcomponent_kind kind, const char *uid);
LIBICAL_ICALSS_EXPORT int icaldirset_has_uid(icalset *store, const char *uid);
LIBICAL_ICALSS_EXPORT icalcomponent *icaldirset_fetch_match(icalset *set, icalcomponent *c);
/* Modify components according to the MODIFY method of CAP. Works on
   the currently selected components. */
LIBICAL_ICALSS_EXPORT icalerrorenum icaldirset_modify(icalset *store,
                                                      icalcomponent *oldc, icalcomponent *newc);
/* Iterate through the components. If a gauge has been defined, these
   will skip over components that do not pass the gauge */
LIBICAL_ICALSS_EXPORT icalcomponent *icaldirset_get_current_component(icalset *store);
LIBICAL_ICALSS_EXPORT icalcomponent *icaldirset_get_first_component(icalset *store);
LIBICAL_ICALSS_EXPORT icalcomponent *icaldirset_get_next_component(icalset *store);
/* External iterator for thread safety */
LIBICAL_ICALSS_EXPORT icalsetiter icaldirset_begin_component(icalset *set,
                                                             icalcomponent_kind kind,
                                                             icalgauge *gauge, const char *tzid);
LIBICAL_ICALSS_EXPORT icalcomponent *icaldirsetiter_to_next(icalset *set, icalsetiter *i);
LIBICAL_ICALSS_EXPORT icalcomponent *icaldirsetiter_to_prior(icalset *set, icalsetiter *i);
typedef struct icaldirset_options
{
    int flags;            /**< flags corresponding to the open() system call O_RDWR, etc. */
} icaldirset_options;
#endif /* !ICALDIRSET_H */
/*======================================================================
 FILE: icalcalendar.h
 CREATOR: eric 23 December 1999
 (C) COPYRIGHT 2000, Eric Busboom <eric@softwarestudio.org>
 This library is free software; you can redistribute it and/or modify
 it under the terms of either:
    The LGPL as published by the Free Software Foundation, version
    2.1, available at: http://www.gnu.org/licenses/lgpl-2.1.html
 Or:
    The Mozilla Public License Version 2.0. You may obtain a copy of
    the License at http://www.mozilla.org/MPL/
 The Original Code is eric. The Initial Developer of the Original
 Code is Eric Busboom
======================================================================*/
#ifndef ICALCALENDAR_H
#define ICALCALENDAR_H
#include "libical_icalss_export.h"
/* icalcalendar
 * Routines for storing calendar data in a file system. The calendar
 * has two icaldirsets, one for incoming components and one for booked
 * components. It also has interfaces to access the free/busy list
 * and a list of calendar properties */
typedef struct icalcalendar_impl icalcalendar;
LIBICAL_ICALSS_EXPORT icalcalendar *icalcalendar_new(const char *dir);
LIBICAL_ICALSS_EXPORT void icalcalendar_free(icalcalendar *calendar);
LIBICAL_ICALSS_EXPORT int icalcalendar_lock(icalcalendar *calendar);
LIBICAL_ICALSS_EXPORT int icalcalendar_unlock(icalcalendar *calendar);
LIBICAL_ICALSS_EXPORT int icalcalendar_islocked(icalcalendar *calendar);
LIBICAL_ICALSS_EXPORT int icalcalendar_ownlock(icalcalendar *calendar);
LIBICAL_ICALSS_EXPORT icalset *icalcalendar_get_booked(icalcalendar *calendar);
LIBICAL_ICALSS_EXPORT icalset *icalcalendar_get_incoming(icalcalendar *calendar);
LIBICAL_ICALSS_EXPORT icalset *icalcalendar_get_properties(icalcalendar *calendar);
LIBICAL_ICALSS_EXPORT icalset *icalcalendar_get_freebusy(icalcalendar *calendar);
#endif /* !ICALCALENDAR_H */
/*======================================================================
 FILE: icalclassify.h
 CREATOR: eric 21 Aug 2000
 (C) COPYRIGHT 2000, Eric Busboom <eric@softwarestudio.org>
     http://www.softwarestudio.org
 This library is free software; you can redistribute it and/or modify
 it under the terms of either:
    The LGPL as published by the Free Software Foundation, version
    2.1, available at: http://www.gnu.org/licenses/lgpl-2.1.html
 Or:
    The Mozilla Public License Version 2.0. You may obtain a copy of
    the License at http://www.mozilla.org/MPL/
 =========================================================================*/
#ifndef ICALCLASSIFY_H
#define ICALCLASSIFY_H
#include "libical_icalss_export.h"
LIBICAL_ICALSS_EXPORT icalproperty_xlicclass icalclassify(icalcomponent *c,
                                                          icalcomponent *match, const char *user);
LIBICAL_ICALSS_EXPORT icalcomponent *icalclassify_find_overlaps(icalset *set,
                                                                icalcomponent *comp);
#endif /* ICALCLASSIFY_H */
/*======================================================================
 FILE: icalspanlist.h
 CREATOR: eric 21 Aug 2000
 (C) COPYRIGHT 2000, Eric Busboom <eric@softwarestudio.org>
     http://www.softwarestudio.org
 This library is free software; you can redistribute it and/or modify
 it under the terms of either:
    The LGPL as published by the Free Software Foundation, version
    2.1, available at: http://www.gnu.org/licenses/lgpl-2.1.html
 Or:
    The Mozilla Public License Version 2.0. You may obtain a copy of
    the License at http://www.mozilla.org/MPL/
=========================================================================*/
#ifndef ICALSPANLIST_H
#define ICALSPANLIST_H
#include "libical_icalss_export.h"
/** @file icalspanlist.h
 *  @brief Code that supports collections of free/busy spans of time
 */
typedef struct icalspanlist_impl icalspanlist;
/** @brief Constructor
 * Make a free list from a set of component. Start and end should be in UTC
 */
LIBICAL_ICALSS_EXPORT icalspanlist *icalspanlist_new(icalset *set,
                                                     struct icaltimetype start,
                                                     struct icaltimetype end);
/** @brief Destructor
 */
LIBICAL_ICALSS_EXPORT void icalspanlist_free(icalspanlist *spl);
/** Get first next free time after time t. all times are in UTC. */
LIBICAL_ICALSS_EXPORT struct icalperiodtype icalspanlist_next_free_time(icalspanlist *sl,
                                                                        struct icaltimetype t);
LIBICAL_ICALSS_EXPORT void icalspanlist_dump(icalspanlist *s);
/** @brief Return a valid VFREEBUSY component for this span */
LIBICAL_ICALSS_EXPORT icalcomponent *icalspanlist_as_vfreebusy(icalspanlist *sl,
                                                               const char *organizer,
                                                               const char *attendee);
/** @brief Return an integer matrix of total events per delta_t timespan */
LIBICAL_ICALSS_EXPORT int *icalspanlist_as_freebusy_matrix(icalspanlist *span, int delta_t);
/** @brief Construct an icalspanlist from a VFREEBUSY component */
LIBICAL_ICALSS_EXPORT icalspanlist *icalspanlist_from_vfreebusy(icalcomponent *comp);
#endif
/*======================================================================
 FILE: icalmessage.h
 CREATOR: eric 07 Nov 2000
 (C) COPYRIGHT 2000, Eric Busboom <eric@softwarestudio.org>
 This library is free software; you can redistribute it and/or modify
 it under the terms of either:
    The LGPL as published by the Free Software Foundation, version
    2.1, available at: http://www.gnu.org/licenses/lgpl-2.1.html
 Or:
    The Mozilla Public License Version 2.0. You may obtain a copy of
    the License at http://www.mozilla.org/MPL/
 =========================================================================*/
#ifndef ICALMESSAGE_H
#define ICALMESSAGE_H
#include "libical_icalss_export.h"
LIBICAL_ICALSS_EXPORT icalcomponent *icalmessage_new_accept_reply(icalcomponent *c,
                                                                  const char *user,
                                                                  const char *msg);
LIBICAL_ICALSS_EXPORT icalcomponent *icalmessage_new_decline_reply(icalcomponent *c,
                                                                   const char *user,
                                                                   const char *msg);
/* New is modified version of old */
LIBICAL_ICALSS_EXPORT icalcomponent *icalmessage_new_counterpropose_reply(icalcomponent *oldc,
                                                                          icalcomponent *newc,
                                                                          const char *user,
                                                                          const char *msg);
LIBICAL_ICALSS_EXPORT icalcomponent *icalmessage_new_delegate_reply(icalcomponent *c,
                                                                    const char *user,
                                                                    const char *delegatee,
                                                                    const char *msg);
LIBICAL_ICALSS_EXPORT icalcomponent *icalmessage_new_error_reply(icalcomponent *c,
                                                                 const char *user,
                                                                 const char *msg,
                                                                 const char *debug,
                                                                 icalrequeststatus rs);
#endif /* ICALMESSAGE_H */

#ifdef __cplusplus
}
#endif
#endif
#endif
