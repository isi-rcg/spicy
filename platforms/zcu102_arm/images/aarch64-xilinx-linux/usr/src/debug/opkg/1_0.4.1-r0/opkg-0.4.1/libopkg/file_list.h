/* vi: set expandtab sw=4 sts=4: */
/* file_list.h - the opkg package management system

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

#ifndef FILE_LIST_H
#define FILE_LIST_H

#include "void_list.h"
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char *path;
    mode_t mode;
    char *link_target;
} file_info_t;

typedef struct void_list_elt file_list_elt_t;

typedef struct void_list file_list_t;

static inline int file_list_empty(file_list_t *list)
{
    return void_list_empty((void_list_t *)list);
}

file_list_t *file_list_alloc(void);
void file_list_init(file_list_t *list);
void file_list_deinit(file_list_t *list);

file_info_t *file_list_append(file_list_t *list, char *name, mode_t mode, char *link_target);
void file_list_remove_elt(file_list_t * list, const char *path);

file_list_elt_t *file_list_first(file_list_t *list);
file_list_elt_t *file_list_next(file_list_t *list, file_list_elt_t *node);

void file_list_purge(file_list_t * list);

#ifdef __cplusplus
}
#endif

#endif                          /* FILE_LIST_H */
