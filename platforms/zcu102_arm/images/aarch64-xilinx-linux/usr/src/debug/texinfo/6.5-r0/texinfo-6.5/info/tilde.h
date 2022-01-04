/* tilde.h: tilde expansion.
   $Id: tilde.h 7670 2017-02-05 13:00:17Z gavin $

   Copyright 1988, 1989, 1990, 1991, 1992, 1993, 2004, 2007, 2013, 2017
   Free Software Foundation, Inc.

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

   Originally written by Brian Fox (for bash). */

#ifndef TILDE_H
#define TILDE_H

#include "info.h"

/* Do the work of tilde expansion on FILENAME.  FILENAME starts with a
   tilde. */
extern char *tilde_expand_word (const char *filename);

#endif /* not TILDE_H */
