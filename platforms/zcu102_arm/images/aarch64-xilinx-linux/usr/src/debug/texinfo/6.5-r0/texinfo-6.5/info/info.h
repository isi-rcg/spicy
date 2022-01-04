/* info.h -- Header file which includes all of the other headers.
   $Id: info.h 5998 2014-12-27 22:23:46Z gavin $

   Copyright 1993, 1997, 1998, 1999, 2001, 2002, 2003, 2004, 2007, 2011,
   2013, 2014 Free Software Foundation, Inc.

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

#ifndef INFO_H
#define INFO_H

/* System dependencies.  */
#include "system.h"

/* Some of our other include files use these.  */
typedef int Function ();
typedef void VFunction ();
typedef char *CFunction ();

#include "string.h"
#include "mbiter.h"
#include "mbchar.h"

extern char *program_name;

#if !defined (whitespace)
#  define whitespace(c) ((c == ' ') || (c == '\t'))
#endif /* !whitespace */

#if !defined (whitespace_or_newline)
#  define whitespace_or_newline(c) (whitespace (c) \
                                    || (c == '\n') || (c == '\r'))
#endif /* !whitespace_or_newline */

/* Add ELT to the list of elements found in ARRAY.  SLOTS is the number
   of slots that have already been allocated.  IDX is the index into the
   array where ELT should be added.  MINSLOTS is the number of slots to
   start the array with in case it is empty. */
#define add_pointer_to_array(elt, idx, array, slots, minslots)	\
  do									\
    {									\
       if (idx + 2 >= slots)						\
	 {								\
	   if (slots == 0)						\
	     slots = minslots;						\
	   array = x2nrealloc (array, &slots, sizeof(array[0]));	\
	 }								\
       array[idx++] = elt;						\
       array[idx] = 0; /* null pointer for pointer types */       	\
    }									\
  while (0)

#define add_element_to_array add_pointer_to_array

/* All commands that can be invoked from within info_session () receive
   arguments in the same way.  This simple define declares the header
   of a function named NAME, with associated documentation DOC.  The
   documentation string is groveled out of the source files by the
   utility program `makedoc', which is also responsible for making
   the documentation/function-pointer maps. */
#define DECLARE_INFO_COMMAND(name, doc) \
void name (WINDOW *window, int count)


/* For handling errors.  If you initialize the window system, you should
   also set info_windows_initialized_p to non-zero.  It is used by the
   info_error () function to determine how to format and output errors. */
extern int info_windows_initialized_p;

/* Non-zero means default keybindings are loosely modeled on vi(1).  */
extern int vi_keys_p;

/* Non-zero means don't remove ANSI escape sequences from man pages.  */
extern int raw_escapes_p;

/* Error message defines. */
extern const char *msg_cant_find_node;
extern const char *msg_cant_file_node;
extern const char *msg_cant_find_window;
extern const char *msg_cant_find_point;
extern const char *msg_cant_kill_last;
extern const char *msg_no_menu_node;
extern const char *msg_no_foot_node;
extern const char *msg_no_xref_node;
extern const char *msg_no_pointer;
extern const char *msg_unknown_command;
extern const char *msg_term_too_dumb;
extern const char *msg_at_node_bottom;
extern const char *msg_at_node_top;
extern const char *msg_one_window;
extern const char *msg_win_too_small;
extern const char *msg_cant_make_help;


/* In infopath.c, but also used in man.c. */

/* Given a string containing units of information separated by colons,
   return the next one pointed to by IDX, or NULL if there are no more.
   Advance IDX to the character after the colon. */
char *extract_colon_unit (char *string, int *idx);

#endif /* !INFO_H */
