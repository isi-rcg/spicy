/* doc.h -- Structures associating function pointers with documentation.
   $Id: doc.h 5835 2014-09-24 12:01:56Z gavin $

   Copyright 1993, 2001, 2004, 2007, 2011, 2013, 2014
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

#if !defined (DOC_H)
#define DOC_H

#include "info.h"

/* For each function, we keep track of the first defined key sequence
   which invokes that function, for each different map.  This is so that
   the dynamic documentation generation in infodoc.c (a) doesn't have to
   search through copious KEYMAP_ENTRYs, and, more importantly, (b) the
   user and programmer can choose the preferred key sequence that is
   printed for any given function -- it's just the first one that
   appears in the user's infokey file or the default keymaps in
   infomap.c.

   Each FUNCTION_DOC has a linked list of FUNCTION_KEYSEQ structs
   hanging off it, which are created on startup when the user and/or
   default keymaps are being parsed.  */
typedef struct function_keyseq
{
  struct function_keyseq *next;
  struct keymap_entry *map;
  int *keyseq;
} FUNCTION_KEYSEQ;

/* Structure describing an Info command. */
typedef struct
{
  VFunction *func;        /* Pointer to function implementing command. */
  char *func_name;        /* Name of this command. */
  FUNCTION_KEYSEQ *keys;  /* Key sequences that could invoke this command. */
  char *doc;              /* Documentation string. */
} FUNCTION_DOC;

/* Array of FUNCTION_DOC structures defined in doc.c, generated
   by the makedoc utility. */
extern FUNCTION_DOC function_doc_array[];

typedef FUNCTION_DOC InfoCommand;
#define InfoCmd(fn) (&function_doc_array[A_##fn])

#include "infomap.h" /* for Keymap.  */

extern char *function_name (InfoCommand *cmd);
extern InfoCommand *named_function (char *name);

extern char *function_documentation (InfoCommand *cmd);
extern char *pretty_keyname (int key);
extern char *pretty_keyseq (int *keyseq);
extern char *where_is (Keymap map, InfoCommand *cmd);
extern char *replace_in_documentation (const char *string,
    int help_is_only_window_p);
extern void dump_map_to_message_buffer (char *prefix, Keymap map);

#endif /* !DOC_H */
