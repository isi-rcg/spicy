/*
 * cleanup.h -- simple dynamic cleanup function management
 * Copyright (C) 1995 Markus Armbruster.
 * Copyright (C) 2007 Colin Watson.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; see the file COPYING.LIB.  If not,
 * write to the Free Software Foundation, Inc., 51 Franklin St, Fifth
 * Floor, Boston, MA  02110-1301  USA.
 */

#ifndef _CLEANUP_H
#define _CLEANUP_H

typedef void (*cleanup_fun) (void *);

extern void do_cleanups_sigsafe (int);
extern void do_cleanups (void);
extern int push_cleanup (cleanup_fun, void *, int);
extern void pop_cleanup (cleanup_fun, void *);
extern void pop_all_cleanups (void);

#endif /* _CLEANUP_H */
