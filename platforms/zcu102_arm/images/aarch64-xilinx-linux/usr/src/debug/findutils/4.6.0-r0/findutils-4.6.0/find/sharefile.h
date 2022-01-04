/* sharefile.h -- open files just once.
   Copyright (C) 2008, 2009, 2010, 2011 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef INC_SHAREFILE_H
#define INC_SHAREFILE_H 1

#include <stdlib.h>
#include <stdio.h>

typedef void * sharefile_handle;

sharefile_handle sharefile_init(const char *mode);
FILE *sharefile_fopen(sharefile_handle, const char *filename);
void sharefile_destroy(sharefile_handle);

#endif
