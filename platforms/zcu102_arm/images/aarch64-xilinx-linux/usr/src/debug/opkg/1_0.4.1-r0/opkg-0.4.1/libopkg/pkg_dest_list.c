/* vi: set expandtab sw=4 sts=4: */
/* pkg_dest_list.c - the opkg package management system

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
#include <stdlib.h>

#include "pkg_dest.h"
#include "void_list.h"
#include "pkg_dest_list.h"
#include "xfuncs.h"

void pkg_dest_list_elt_init(pkg_dest_list_elt_t * elt, pkg_dest_t * data)
{
    void_list_elt_init((void_list_elt_t *) elt, data);
}

void pkg_dest_list_elt_deinit(pkg_dest_list_elt_t * elt)
{
    void_list_elt_deinit((void_list_elt_t *) elt);
}

void pkg_dest_list_init(pkg_dest_list_t * list)
{
    void_list_init((void_list_t *) list);
}

void pkg_dest_list_deinit(pkg_dest_list_t * list)
{
    pkg_dest_list_elt_t *iter, *n;
    pkg_dest_t *pkg_dest;

    list_for_each_entry_safe(iter, n, &list->head, node) {
        pkg_dest = (pkg_dest_t *) iter->data;
        pkg_dest_deinit(pkg_dest);

        /* malloced in pkg_dest_list_append */
        free(pkg_dest);
        iter->data = NULL;
    }
    void_list_deinit((void_list_t *) list);
}

pkg_dest_t *pkg_dest_list_append(pkg_dest_list_t * list, const char *name,
                                 const char *root_dir)
{
    pkg_dest_t *pkg_dest;

    /* freed in pkg_dest_list_deinit */
    pkg_dest = xcalloc(1, sizeof(pkg_dest_t));
    pkg_dest_init(pkg_dest, name, root_dir);
    void_list_append((void_list_t *) list, pkg_dest);

    return pkg_dest;
}
