/* vi: set expandtab sw=4 sts=4: */
/* file_util.h - convenience routines for common file operations

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

#ifndef FILE_UTIL_H
#define FILE_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

struct stat;

int xlstat(const char *file_name, struct stat *st);
int file_exists(const char *file_name);
int file_is_dir(const char *file_name);
int file_is_symlink(const char *file_name);
char *file_readlink_alloc(const char *file_name);
char *file_read_line_alloc(FILE * file);
int file_link(const char *src, const char *dest);
int file_copy(const char *src, const char *dest);
int file_mkdir_hier(const char *path, long mode);
char *file_md5sum_alloc(const char *file_name);
char *file_sha256sum_alloc(const char *file_name);
int rm_r(const char *path);
int file_decompress(const char *in, const char *out);
int file_gz_compress(const char *filename);

/* Buffer size used for extracting files from archives. */
#define EXTRACT_BUFFER_LEN 0x8000

#ifdef __cplusplus
}
#endif
#endif                          /* FILE_UTIL_H */
