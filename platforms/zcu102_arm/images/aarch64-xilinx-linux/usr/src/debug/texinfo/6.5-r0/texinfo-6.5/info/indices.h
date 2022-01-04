/* indices.h -- Functions defined in indices.c.
   $Id: indices.h 7773 2017-05-13 13:38:44Z gavin $

   Copyright 1993, 1997, 2004, 2007, 2013, 2014, 2015, 2016, 2017
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

   Originally written by Brian Fox. */

#ifndef INFO_INDICES_H
#define INFO_INDICES_H

/* User-visible variable controls the output of info-index-next. */
extern int show_index_match;

/* For every menu item in DIR, search the indices of that file for STRING. */
REFERENCE **apropos_in_all_indices (char *search_string, int inform);

/* User visible functions declared in indices.c. */
void info_index_search (WINDOW *window, int count);
void info_index_apropos (WINDOW *window, int count);
REFERENCE *next_index_match (FILE_BUFFER *fb, char *string,
                             int offset, int dir,
                             int *found_offset, int *match_offset);
void report_index_match (int i, int match_offset);
REFERENCE *look_in_indices (FILE_BUFFER *fb, char *string, int sloppy);
NODE *create_virtual_index (FILE_BUFFER *file_buffer, char *index_search);

#define APROPOS_NONE \
   N_("No available info files have '%s' in their indices")

#endif /* not INFO_INDICES_H */
