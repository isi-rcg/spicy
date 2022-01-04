/* vi: set expandtab sw=4 sts=4: */
/* file_list.c - the opkg package management system

   Copyright (C) 2017 Michael Hansen

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
#include <string.h>

#include "file_list.h"
#include "xfuncs.h"

static int file_info_init(file_info_t *info, char *path, mode_t mode, char *link_target)
{
    info->path = xstrdup(path);
    info->mode = mode;
    info->link_target = xstrdup(link_target);

    return 0;
}

static void file_info_deinit(file_info_t *info)
{
    free(info->path);
    info->path = NULL;

    free(info->link_target);
    info->link_target = NULL;
}

file_list_t *file_list_alloc()
{
    file_list_t *list = xcalloc(1, sizeof(file_list_t));
    file_list_init(list);
    return list;
}

void file_list_init(file_list_t *list)
{
    void_list_init((void_list_t *)list);
}

void file_list_deinit(file_list_t *list)
{
    file_list_elt_t *iter, *n;
    file_info_t *info;

    list_for_each_entry_safe(iter, n, &list->head, node) {
        info = (file_info_t *) iter->data;
        file_info_deinit(info);

        /* malloced in file_list_append */
        free(info);
        iter->data = NULL;
    }
    void_list_deinit((void_list_t *)list);
}

file_info_t *file_list_append(file_list_t *list, char *name, mode_t mode, char *link_target)
{
    /* freed in file_list_deinit */
    file_info_t *info = xcalloc(1, sizeof(file_info_t));
    file_info_init(info, name, mode, link_target);

    void_list_append((void_list_t *)list, info);

    return info;
}

static int file_list_cmp(const void *left, const void *right)
{
    file_info_t *left_pkg = (file_info_t *)left;
    return strcmp(left_pkg->path, (const char *)right);
}

void file_list_remove_elt(file_list_t *list, const char *path)
{
    char *str = void_list_remove_elt((void_list_t *)list,
                                     (void *)path,
                                     file_list_cmp);
    if (str)
        free(str);
}

file_list_elt_t *file_list_first(file_list_t *list)
{
    return (file_list_elt_t *)void_list_first((void_list_t *)list);
}

file_list_elt_t *file_list_next(file_list_t *list, file_list_elt_t * node)
{
    return (file_list_elt_t *)void_list_next((void_list_t *)list,
                                             (void_list_elt_t *)node);
}

void file_list_purge(file_list_t * list)
{
    file_list_deinit(list);
    free(list);
}
