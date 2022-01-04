/* regextype.h -- Decode the name of a regular expression syntax.

   Copyright 2005, 2010, 2011, 2015 Free Software Foundation, Inc.

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
/* Written by James Youngman <jay@gnu.org>.
 */

/* Translate a regular expression type name into an option mask.
 * This could convert "grep" into RE_SYNTAX_GREP, for example.
 * Return -1 if there is no match.
 */
int get_regex_type(const char *s);

enum {
  CONTEXT_FINDUTILS = 1u,
  CONTEXT_GENERIC   = 2u,
  CONTEXT_ALL = CONTEXT_GENERIC|CONTEXT_FINDUTILS,
};



/* Returns the regex type name corresponding to index IX.
 * Indexes start at 0.  Returns NULL if IX is too large.
 */
const char * get_regex_type_name(unsigned int ix);


/* Returns the option mask name corresponding to regular expresion
 * type index IX.  Indexes start at 0.  Behaviour is undefined if IX
 * has a value which would cause get_regex_type_name to return NULL.
 */
int get_regex_type_flags(unsigned int ix);

/* If regular expression type IX (which is a regular expression type index) has
 * one or more synonyms, return the index of one of them.  Otherwise, return -1.
 */
int get_regex_type_synonym(unsigned int ix);

/* Returns one of CONTEXT_FINDUTILS, CONTEXT_GENERIC or CONTEXT_ALL.
 * This identifies whether this regular expression type index is relevant for,
 * respectively, findutils, general callers, or all callers.
 */
unsigned int get_regex_type_context(unsigned int ix);
