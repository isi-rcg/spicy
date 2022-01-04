/* search.h -- Structure used to search large bodies of text, with bounds.
   $Id: search.h 7714 2017-04-11 06:40:52Z gavin $

   Copyright 1993, 1997, 1998, 2002, 2004, 2007, 2009, 2011, 2013, 2014,
   2016, 2017 Free Software Foundation, Inc.

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

/* The search functions take two arguments:

     1) a string to search for, and

     2) a pointer to a SEARCH_BINDING which contains the buffer, start,
        and end of the search.

   They return a long, which is the offset from the start of the buffer
   at which the match was found.  An offset of -1 indicates failure. */

#ifndef INFO_SEARCH_H
#define INFO_SEARCH_H

#include "window.h"

typedef struct {
  char *buffer;                 /* The buffer of text to search. */
  long start;                   /* Offset of the start of the search. */
  long end;                     /* Offset of the end of the searh. */
  int flags;                    /* Flags controlling the type of search. */
} SEARCH_BINDING;

#define S_FoldCase      0x01    /* Set means fold case in searches. */
#define S_SkipDest      0x02    /* Set means return pointing after the dest. */

enum search_result
  {
    search_success,             
    search_not_found,
    search_invalid
  };

enum search_result search_forward (char *string,
                                 SEARCH_BINDING *binding, long *poff);
enum search_result search_backward (char *input_string,
                                    SEARCH_BINDING *binding,
                                    long *poff);
enum search_result search (char *string, SEARCH_BINDING *binding,
                           long *poff);
enum search_result regexp_search (char *regexp,
               int is_literal, int is_insensitive,
               char *buffer, size_t buflen,
               MATCH_STATE *match_state);
int looking_at (char *string, SEARCH_BINDING *binding);
int looking_at_line (char *string, char *pointer);

/* Note that STRING_IN_LINE () always returns the offset of the 1st character
   after the string. */
int string_in_line (char *string, char *line);

/* Function names that start with "skip" are passed a string, and return
   an offset from the start of that string.  Function names that start
   with "find" are passed a SEARCH_BINDING, and return an absolute position
   marker of the item being searched for.  "Find" functions return a value
   of -1 if the item being looked for couldn't be found. */
int skip_whitespace (char *string);
int skip_non_whitespace (char *string);
int skip_whitespace_and_newlines (char *string);
int skip_node_separator (char *body);

long find_node_separator (SEARCH_BINDING *binding);
long find_file_section (SEARCH_BINDING *binding, char *label);
long find_node_in_binding (char *nodename, SEARCH_BINDING *binding);

regmatch_t match_by_index (MATCH_STATE *state, int index);
enum search_result match_in_match_list (MATCH_STATE *state,
                     long start, long end, int dir, int *match_index);

void free_matches (MATCH_STATE *state);
int matches_ready (MATCH_STATE *state);
int at_end_of_matches (MATCH_STATE *state, int index);
void decide_if_in_match (long off, int *in_match, MATCH_STATE *matches,
                         size_t *match_index);

#endif /* not INFO_SEARCH_H */
