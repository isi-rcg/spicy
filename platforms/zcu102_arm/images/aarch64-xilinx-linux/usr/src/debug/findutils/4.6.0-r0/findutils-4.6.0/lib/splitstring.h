/* splitstring.h -- split a const string into fields.
   Copyright (C) 2011 Free Software Foundation, Inc.

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
/*
 * Written by James Youngman.
 */

/* Split a string into fields.   The string is never modified.
 *
 * A false return value indicates that there are no more fields.
 * Otherwise the next field is at the poisition indicated by *POS and
 * has length *LEN.
 *
 * Set FIRST to true only on the first call for any given value of s.
 * *POS and *LEN do not need to be initialized in this case.
 * On subsequent calls, these values should be left at the values
 * set by the last call.
 *
 * Any character in SEPARATORS is taken to be a field separator.
 * Consecutive field separators are taken to indicate the presence of
 * an empty field.
 */
#include <stdbool.h>
#include <stddef.h>

bool splitstring(const char *s, const char *separators,
		 bool first, size_t *pos, size_t *len);
