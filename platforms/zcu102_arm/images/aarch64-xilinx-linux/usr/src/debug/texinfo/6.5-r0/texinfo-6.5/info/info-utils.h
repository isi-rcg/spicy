/* info-utils.h -- Exported functions and variables from info-utils.c.
   $Id: info-utils.h 7013 2016-02-13 21:19:19Z gavin $   

   Copyright 1993, 1996, 1998, 2002, 2003, 2004, 2007, 2011, 2012, 2013,
   2014, 2015, 2016 Free Software Foundation, Inc.

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

#ifndef INFO_UTILS_H
#define INFO_UTILS_H

#include "nodes.h"
#include "window.h"
#include "search.h"

#if HAVE_ICONV
# include <iconv.h>
#endif

/* Variable which holds the most recent filename parsed as a result of
   calling info_parse_xxx (). */
extern char *info_parsed_filename;

/* Variable which holds the most recent nodename parsed as a result of
   calling info_parse_xxx (). */
extern char *info_parsed_nodename;

/* Parse the filename and nodename out of STRING. */ 
void info_parse_node (char *string);

long read_quoted_string (char *start, char *terminator, int lines,
                         char **output);

void scan_node_contents (NODE *node, FILE_BUFFER *fb, TAG **tag_ptr);

/* Get the menu entry associated with LABEL in NODE.  Return a
   pointer to the reference if found, or NULL.  If SLOPPY, accept
   initial substrings and check insensitively to case. */
REFERENCE *info_get_menu_entry_by_label (NODE *node, char *label,
                                                int sloppy);

/* A utility function for concatenating REFERENCE **.  Returns a new
   REFERENCE ** which is the concatenation of REF1 and REF2.  The REF1
   and REF2 arrays are freed, but their contents are not. */
REFERENCE **info_concatenate_references (REFERENCE **ref1, REFERENCE **ref2);

/* Copy an existing reference into new memory.  */
REFERENCE *info_copy_reference (REFERENCE *src);

/* Copy a list of existing references into new memory.  */
REFERENCE **info_copy_references (REFERENCE **ref1);

/* Free the data associated with a single REF */
void info_reference_free (REFERENCE *ref);

/* Free the data associated with REFERENCES. */
void info_free_references (REFERENCE **references);

/* Create new REFERENCE structure. */
REFERENCE *info_new_reference (char *filename, char *nodename);

/* Search for sequences of whitespace or newlines in STRING, replacing
   all such sequences with just a single space.  Remove whitespace from
   start and end of string. */
void canonicalize_whitespace (char *string);

/* Used with multibyte iterator mbi_iterator_t. */
#define ITER_SETBYTES(iter,n) ((iter).cur.bytes = n)
#define ITER_LIMIT(iter) ((iter).limit - (iter).cur.ptr)

int ansi_escape (mbi_iterator_t iter, size_t *plen);

/* Return a pointer to a string which is the printed representation
   of CHARACTER if it were printed at HPOS. */
char *printed_representation (mbi_iterator_t *iter,
                                     int *delim, size_t pl_chars,
                                     size_t *pchars, size_t *pbytes);

FILE_BUFFER *file_buffer_of_window (WINDOW *window);

char *node_printed_rep (NODE *node);

/* Return a pointer to the part of PATHNAME that simply defines the file. */
char *filename_non_directory (char *pathname);

/* Return non-zero if NODE is one especially created by Info. */
int internal_info_node_p (NODE *node);

/* Make NODE appear to be one especially created by Info, and give it NAME. */
void name_internal_node (NODE *node, char *name);

/* Return the window displaying NAME, the name of an internally created
   Info window. */
WINDOW *get_internal_info_window (char *name);

struct text_buffer
{
  char *base;
  size_t size;
  size_t off;
};

#define MIN_TEXT_BUF_ALLOC 512

void text_buffer_init (struct text_buffer *buf);
void text_buffer_free (struct text_buffer *buf);
void text_buffer_alloc (struct text_buffer *buf, size_t len);
size_t text_buffer_vprintf (struct text_buffer *buf, const char *format,
			    va_list ap);
size_t text_buffer_space_left (struct text_buffer *buf);
#if HAVE_ICONV
size_t text_buffer_iconv (struct text_buffer *buf, iconv_t iconv_state,
                          ICONV_CONST char **inbuf, size_t *inbytesleft);
#endif
size_t text_buffer_add_string (struct text_buffer *buf, const char *str,
			       size_t len);
size_t text_buffer_fill (struct text_buffer *buf, int c, size_t len);
void text_buffer_add_char (struct text_buffer *buf, int c);
size_t text_buffer_printf (struct text_buffer *buf, const char *format, ...);
#define text_buffer_reset(buf) ((buf)->off = 0)
#define text_buffer_base(buf) ((buf)->base)
#define text_buffer_off(buf) ((buf)->off)

struct info_namelist_entry;
int info_namelist_add (struct info_namelist_entry **ptop, const char *name);
void info_namelist_free (struct info_namelist_entry *top);

#if defined(__MSDOS__) || defined(__MINGW32__)
int fncmp (const char *fn1, const char *fn2);
#else
# define fncmp(s,t) strcmp(s,t)
#endif

#endif /* not INFO_UTILS_H */
