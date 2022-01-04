/*
 * NUMA node support for <PIDS> & <STAT> interfaces
 * Copyright 2017 by James C. Warmer
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef PROCPS_NUMA_H
#define PROCPS_NUMA_H

#include "procps.h"

EXTERN_C_BEGIN

void numa_init (void);
void numa_uninit (void);

extern int (*numa_max_node) (void);
extern int (*numa_node_of_cpu) (int);

EXTERN_C_END

#endif
