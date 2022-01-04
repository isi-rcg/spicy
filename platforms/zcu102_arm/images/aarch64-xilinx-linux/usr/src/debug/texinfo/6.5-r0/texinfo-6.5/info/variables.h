/* variables.h -- Description of user visible variables in Info.
   $Id: variables.h 7666 2017-02-04 00:52:09Z gavin $

   Copyright 1993, 1997, 2004, 2007, 2011, 2013, 2014, 2015,
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

#ifndef INFO_VARIABLES_H
#define INFO_VARIABLES_H

#include "window.h"
#include "info-utils.h"

/* A variable (in the Info sense) is an integer value with a user-visible
   name.  You may supply an array of strings to complete over when the
   variable is set; in that case, the variable is set to the index of the
   string that the user chose.  If you supply a null list, the user can
   set the variable to a numeric value. */

/* Structure describing a user visible variable. */
typedef struct {
  char *name;                   /* Polite name. */
  char *doc;                    /* Documentation string. */
  void *value;                  /* Address of value. */
  char **choices;               /* Array of strings or NULL if numeric only. */
  int where_set;                /* Where this variable was set. */
} VARIABLE_ALIST;

/* Values for VARIABLE_ALIST.where_set, in order of increasing priority. */
#define SET_BY_DEFAULT 0
#define SET_IN_CONFIG_FILE 1
#define SET_ON_COMMAND_LINE 2
#define SET_IN_SESSION 4

VARIABLE_ALIST *variable_by_name (char *name);

/* Make an array of REFERENCE which actually contains the names of the
   variables available in Info. */
REFERENCE **make_variable_completions_array (void);

/* Set the value of an info variable. */
void set_variable (WINDOW *window, int count);
int set_variable_to_value (VARIABLE_ALIST *var, char *value, int where);

void describe_variable (WINDOW *window, int count);

/* The list of user-visible variables. */
extern int auto_footnotes_p;
extern int auto_tiling_p;
extern int terminal_use_visible_bell_p;
extern int info_error_rings_bell_p;
extern int gc_compressed_files;
extern int show_index_match;
extern int info_scroll_behaviour;
extern int window_scroll_step;
extern int cursor_movement_scrolls_p;
extern int ISO_Latin_p;
extern int scroll_last_node;
extern int min_search_length;
extern int search_skip_screen_p;
extern int infopath_no_defaults_p;
extern int preprocess_nodes_p;
extern int key_time;
extern int mouse_protocol;
extern int follow_strategy;
extern int nodeline_print;

typedef struct {
    unsigned long mask;
    unsigned long value;
} RENDITION;

extern RENDITION ref_rendition;
extern RENDITION hl_ref_rendition;
extern RENDITION match_rendition;


#endif /* not INFO_VARIABLES_H */
