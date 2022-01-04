/* print.h -- declarations for symbols in print.c.
   Copyright (C) 2011-2015 Free Software Foundation, Inc.

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
#include "defs.h"

struct format_val;
struct parser_table;
struct predicate;
struct segment;

struct segment **make_segment (struct segment **segment,
			       char *format, int len,
			       int kind, char format_char,
			       char aux_format_char,
			       struct predicate *pred);
bool
insert_fprintf (struct format_val *vec,
		const struct parser_table *entry,
		char *format);
