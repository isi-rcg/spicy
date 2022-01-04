/*
 * decompress.h: interface to decompression abstraction layer
 *
 * Copyright (C) 2007 Colin Watson.
 *
 * This file is part of man-db.
 *
 * man-db is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * man-db is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with man-db; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef MAN_DECOMPRESS_H
#define MAN_DECOMPRESS_H

#include "pipeline.h"

struct decompress;

/* Open a decompressor reading from FILENAME. The caller must start the
 * resulting pipeline.
 */
pipeline *decompress_open (const char *filename);

/* Open a decompressor reading from file descriptor FD. The caller must
 * start the resulting pipeline.
 */
pipeline *decompress_fdopen (int fd);

#endif /* MAN_DECOMPRESS_H */
