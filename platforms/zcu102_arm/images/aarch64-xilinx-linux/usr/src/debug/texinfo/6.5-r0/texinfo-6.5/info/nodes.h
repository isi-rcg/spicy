/* nodes.h -- How we represent nodes internally.
   $Id: nodes.h 7794 2017-05-18 20:59:43Z gavin $

   Copyright 1993, 1997, 1998, 2002, 2004, 2007, 2011, 2012, 2013,
   2014, 2015 Free Software Foundation, Inc.

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

#ifndef NODES_H
#define NODES_H

#include "info.h"

/* Structure which describes a node reference, such as a menu entry or
   cross reference. */
typedef struct {
  char *label;          /* User Label. */
  char *filename;       /* File where this node can be found. */
  char *nodename;       /* Name of the node. */
  int start, end;       /* Offsets within the containing node of LABEL. */
  long line_number;     /* Specific line number a menu item points to.  */
  int type;             /* Whether reference is a xref or a menu item */
} REFERENCE;

/* Possible values of REFERENCE.type */
#define REFERENCE_XREF 0
#define REFERENCE_MENU_ITEM 1

typedef struct {
  char *fullpath;               /* Non-null is the logical file name. */
  char *subfile;                /* File containing node for split files. */
  char *nodename;               /* The name of this node. */
  char *contents;               /* Characters appearing in this node. */
  long nodelen;                 /* The length of the CONTENTS member. */
  long display_pos;             /* Where to display at, if nonzero.  */
  long body_start;              /* Offset of the actual node body */
  int flags;                    /* See immediately below. */
  REFERENCE **references;       /* Cross-references or menu items in node.
                                   Null-terminated.  references == 0 implies 
                                   uninitialized, not empty */
  char *up, *prev, *next;       /* Names of nearby nodes. */
  int active_menu;              /* Used for subnodes search. */
} NODE;

/* Values for NODE.flags or FILE_BUFFER.flags. */
#define N_HasTagsTable 0x01     /* This node was found through a tags table. */
#define N_TagsIndirect 0x02     /* The tags table was an indirect one. */
#define N_UpdateTags   0x04     /* The tags table is out of date. */
#define N_IsCompressed 0x08     /* The file is compressed on disk. */
#define N_IsInternal   0x10     /* This node was made by Info. */
#define N_CannotGC     0x20     /* File buffer cannot be gc'ed. */
#define N_IsManPage    0x40     /* This node is a manpage. */
#define N_WasRewritten 0x100    /* NODE->contents can be passed to free(). */ 
#define N_IsIndex      0x200    /* An index node. */
#define N_IsDir        0x400    /* A dir node. */
#define N_Subfile      0x800    /* File buffer is a subfile of a split file. */
#define N_Gone         0x1000   /* File is no more. */
#define N_Simple       0x2000   /* Data about cross-references is missing. */
#define N_SeenBySearch 0x4000   /* Node has already been seen in a search. */

/* String constants. */
#define INFO_FILE_LABEL                 "File:"
#define INFO_REF_LABEL                  "Ref:"
#define INFO_NODE_LABEL                 "Node:"
#define INFO_PREV_LABEL                 "Prev:"
#define INFO_ALTPREV_LABEL              "Previous:"
#define INFO_NEXT_LABEL                 "Next:"
#define INFO_UP_LABEL                   "Up:"
#define INFO_MENU_LABEL                 "\n* Menu:"
#define INFO_MENU_ENTRY_LABEL           "\n* "
#define INFO_XREF_LABEL                 "*Note"
#define TAGS_TABLE_END_LABEL            "End Tag Table"
#define TAGS_TABLE_BEG_LABEL            "Tag Table:"
#define INDIRECT_TABLE_LABEL            "Indirect:"
#define TAGS_TABLE_IS_INDIRECT_LABEL    "(Indirect)"
#define LOCAL_VARIABLES_LABEL           "Local Variables"
#define CHARACTER_ENCODING_LABEL        "coding:"

/* Character constants. */
#define INFO_COOKIE '\037'
#define INFO_FF     '\014'
#define INFO_TAGSEP '\177'

/* For each logical file that we have loaded, we keep a list of
   the names of the nodes that are found in that file.  A pointer to
   a node in an info file is called a "tag".  For split files, the
   tag pointer is "indirect"; that is, the pointer also contains the
   name of the split file where the node can be found.  For non-split
   files, the filename member simply contains the name of the
   current file. */
typedef struct {
  char *filename;               /* The file where this node can be found. */
  char *nodename;               /* The node pointed to by this tag. */
  long nodestart;               /* The value read from the tag table. */
  long nodestart_adjusted;      /* Where the node or anchor actually is. */
  int flags;                    /* Same as NODE.flags. */
  NODE cache;                   /* Saved information about pointed-to node. */
} TAG;

/* The following structure is used to remember information about the contents
   of Info files that we have loaded at least once before.  The FINFO member
   is present so that we can reload the file if it has been modified since
   last being loaded.  All of the arrays appearing within this structure
   are NULL terminated. */
typedef struct {
  char *filename;               /* The filename used to find this file. */
  char *fullpath;               /* The full pathname of this info file. */
  struct stat finfo;            /* Information about this file. */
  char *contents;               /* The contents of this particular file. */
  size_t filesize;              /* The number of bytes this file expands to. */
  char **subfiles;              /* If non-null, the list of subfiles. */
  TAG **tags;                   /* If non-null, the tags table. */
  size_t tags_slots;            /* Number of slots allocated for TAGS. */
  int flags;                    /* Various flags.  Mimics of N_* flags. */
  char *encoding;               /* Name of character encoding of file. */
} FILE_BUFFER;

/* Array of FILE_BUFFER * which represents the currently loaded info files. */
extern FILE_BUFFER **info_loaded_files;
extern size_t info_loaded_files_index;
extern size_t info_loaded_files_slots;

/* Locate the file named by FILENAME, and return the information structure
   describing this file.  The file may appear in our list of loaded files
   already, or it may not.  If it does not already appear, find the file,
   and add it to the list of loaded files.  If the file cannot be found,
   return a NULL FILE_BUFFER *. */
FILE_BUFFER *info_find_file (char *filename);

FILE_BUFFER *check_loaded_file (char *filename);

FILE_BUFFER *info_find_subfile (char *filename);

TAG *info_create_tag (void);

/* Return a pointer to a new NODE structure. */
NODE *info_create_node (void);

/* Return a pointer to a NODE structure for the Info node (FILENAME)NODENAME.
   FILENAME can be passed as NULL, in which case the filename of "dir" is used.
   NODENAME can be passed as NULL, in which case the nodename of "Top" is used.
   
   If the node cannot be found, return a NULL pointer. */
NODE *info_get_node (char *filename, char *nodename);

NODE *info_get_node_with_defaults (char *filename, char *nodename,
                                          NODE *defaults);

NODE *info_node_of_tag (FILE_BUFFER *fb, TAG **tag_ptr);
NODE *info_node_of_tag_fast (FILE_BUFFER *fb, TAG **tag_ptr);

/* Return a pointer to a NODE structure for the Info node NODENAME in
   FILE_BUFFER.  NODENAME can be passed as NULL, in which case the
   nodename of "Top" is used.  If the node cannot be found, return a
   NULL pointer. */
NODE *info_get_node_of_file_buffer (FILE_BUFFER *file_buffer,
                                           char *nodename);

/* Grovel FILE_BUFFER->contents finding tags and nodes, and filling in the
   various slots.  This can also be used to rebuild a tag or node table. */
void build_tags_and_nodes (FILE_BUFFER *file_buffer);

void free_history_node (NODE *n);

/* When non-zero, this is a string describing the most recent file error. */
extern char *info_recent_file_error;

/* Create a new, empty file buffer. */
FILE_BUFFER *make_file_buffer (void);

/* Non-zero means don't try to be smart when searching for nodes.  */
extern int strict_node_location_p;


/* Found in dir.c */
NODE *get_dir_node (void);
REFERENCE *lookup_dir_entry (char *label, int sloppy);
REFERENCE *dir_entry_of_infodir (char *label, char *searchdir);

#endif /* not NODES_H */
