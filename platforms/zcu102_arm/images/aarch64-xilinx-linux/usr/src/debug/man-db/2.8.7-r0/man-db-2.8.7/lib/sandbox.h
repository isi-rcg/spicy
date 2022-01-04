/*
 * sandbox.h: Interface to process sandboxing
 *  
 * Copyright (C) 2017 Colin Watson.
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

#ifndef MAN_SANDBOX_H
#define MAN_SANDBOX_H

struct man_sandbox;
typedef struct man_sandbox man_sandbox;

extern man_sandbox *sandbox_init (void);

/* These functions take a man_sandbox * argument, but have more generic
 * types suitable for use with pipecmd_pre_exec.
 */
extern void sandbox_load (void *data);
extern void sandbox_load_permissive (void *data);
extern void sandbox_free (void *data);

#endif /* MAN_SANDBOX_H */
