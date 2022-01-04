/*
 * db_xdbm.c: interface to common code for gdbm and ndbm backends
 *
 * Copyright (C) 2019 Colin Watson.
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
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef MAN_XDBM_H
#define MAN_XDBM_H

#if defined(GDBM) || defined(NDBM)

#include "mydbm.h"

typedef datum (*man_xdbm_unsorted_firstkey) (MYDBM_FILE dbf);
typedef datum (*man_xdbm_unsorted_nextkey) (MYDBM_FILE dbf, datum key);
typedef void (*man_xdbm_raw_close) (MYDBM_FILE dbf);

datum man_xdbm_firstkey (MYDBM_FILE dbf,
			 man_xdbm_unsorted_firstkey firstkey,
			 man_xdbm_unsorted_nextkey nextkey);
datum man_xdbm_nextkey (MYDBM_FILE dbf, datum key);
void man_xdbm_close (MYDBM_FILE dbf, man_xdbm_raw_close raw_close);

#endif /* GDBM || NDBM */

#endif /* MAN_XDBM_H */
