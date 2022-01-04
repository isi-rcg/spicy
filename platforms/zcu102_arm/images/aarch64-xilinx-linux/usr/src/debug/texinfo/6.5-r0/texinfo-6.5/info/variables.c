/* variables.c -- how to manipulate user visible variables in Info.
   $Id: variables.c 7769 2017-05-03 19:47:57Z gavin $

   Copyright 1993, 1997, 2001, 2002, 2004, 2007, 2008, 2011, 2013,
   2014, 2015, 2017 Free Software Foundation, Inc.

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

#include "info.h"
#include "session.h"
#include "echo-area.h"
#include "variables.h"
#include "terminal.h"
#include "display.h"

static VARIABLE_ALIST *read_variable_name (char *prompt, WINDOW *window);

/* **************************************************************** */
/*                                                                  */
/*                  User Visible Variables in Info                  */
/*                                                                  */
/* **************************************************************** */

/* Choices used by the completer when reading a zero/non-zero value for
   a variable. */
static char *on_off_choices[] = { "Off", "On", NULL };

static char *mouse_choices[] = { "Off", "normal-tracking", NULL };

static char *follow_strategy_choices[] = { "remain", "path", NULL };

static char *nodeline_choices[] = { "no", "print", "pointers", NULL };

/* Choices used by the completer when reading a value for the user-visible
   variable "scroll-behaviour". */
static char *info_scroll_choices[] = { "Continuous", "Next Only",
    "Page Only", NULL };

/* Choices for the scroll-last-node variable */
static char *scroll_last_node_choices[] = { "Stop", "Top", NULL };

/* Choices for, and indicator of, renditions. */
static char *rendition_choices[] = { "black", "red", "green", "yellow", "blue",
    "magenta", "cyan", "white", "nocolour", "bgblack", "bgred", "bggreen",
    "bgyellow", "bgblue", "bgmagenta", "bgcyan", "bgwhite", "nobgcolour",
    "underline", "nounderline", "standout", "nostandout", "bold", "regular",
    "blink", "noblink", NULL };

/* Address of this indicates the 'highlight-searches' variable. */
static int *highlight_searches;

/* Note that the 'where_set' field of each element in the array is
   not given and defaults to 0. */
VARIABLE_ALIST info_variables[] = {
  { "automatic-footnotes",
      N_("When \"On\", footnotes appear and disappear automatically"),
      &auto_footnotes_p, (char **)on_off_choices },

  { "automatic-tiling",
      N_("When \"On\", creating or deleting a window resizes other windows"),
      &auto_tiling_p, (char **)on_off_choices },

  { "visible-bell",
      N_("When \"On\", flash the screen instead of ringing the bell"),
      &terminal_use_visible_bell_p, (char **)on_off_choices },

  { "errors-ring-bell",
      N_("When \"On\", errors cause the bell to ring"),
      &info_error_rings_bell_p, (char **)on_off_choices },

  { "gc-compressed-files",
      N_("When \"On\", Info garbage collects files which had to be uncompressed"),
      &gc_compressed_files, (char **)on_off_choices },
  { "show-index-match",
      N_("When \"On\", the portion of the matched search string is highlighted"),
      &show_index_match, (char **)on_off_choices },

  { "scroll-behaviour",
      N_("Controls what happens when scrolling is requested at the end of a node"),
      &info_scroll_behaviour, (char **)info_scroll_choices },

  /* Alternate spelling */
  { "scroll-behavior",
      N_("Same as scroll-behaviour"),
      &info_scroll_behaviour, (char **)info_scroll_choices },

  { "scroll-step",
      N_("The number lines to scroll when the cursor moves out of the window"),
      &window_scroll_step, NULL },

  { "cursor-movement-scrolls",
    N_("Controls whether scroll-behavior affects cursor movement commands"),
    &cursor_movement_scrolls_p, (char **)on_off_choices },
  
  { "ISO-Latin",
      N_("When \"On\", Info accepts and displays ISO Latin characters"),
      &ISO_Latin_p, (char **)on_off_choices },

  { "scroll-last-node",
    N_("What to do when a scrolling command is issued at the end of the "
       "last node"),
    &scroll_last_node, (char**)scroll_last_node_choices },
  
  { "min-search-length",
    N_("Minimal length of a search string"),
    &min_search_length, NULL },

  { "search-skip-screen",
      N_("Skip current window when searching"),
    &search_skip_screen_p, (char **)on_off_choices },

  { "infopath-no-defaults",
      N_("Exclude default directories from file search path"),
    &infopath_no_defaults_p, (char **)on_off_choices },

  { "hide-note-references",
      N_("Hide some Info file syntax in the text of nodes"),
    &preprocess_nodes_p, (char **)on_off_choices },

  { "key-time",
      N_("Length of time in milliseconds to wait for the next byte in a sequence indicating that a key has been pressed"),
    &key_time, NULL },

  { "mouse",
      N_("Method to use to track mouse events"),
    &mouse_protocol, (char **)mouse_choices },

  { "follow-strategy",
      N_("How to follow a cross-reference"),
    &follow_strategy, (char **)follow_strategy_choices },

  { "highlight-searches",
      N_("Highlight search matches"),
    &highlight_searches, (char **)on_off_choices },

  { "link-style",
      N_("Styles for links"),
    &ref_rendition, (char **)rendition_choices },

  { "active-link-style",
      N_("Styles for active links"),
    &hl_ref_rendition, (char **)rendition_choices },

  { "match-style",
      N_("Styles for search matches"),
    &match_rendition, (char **)rendition_choices },

  { "nodeline",
      N_("How to print the information line at the start of a node"),
    &nodeline_print, (char **)nodeline_choices },

  { NULL }
};

static char *
rendition_to_string (RENDITION *rendition)
{
  static char string[8 /* nocolour */ + 1 /* comma */
                     + 10 /* nobgcolour */ + 1
                     + 11 /* nounderline */ + 1
                     + 10 /* nostandout */ + 1
                     + 7 /* regular */ + 1
                     + 7 /* noblink */ + 1];
  unsigned long style;
  static const char* fg[] = { "black", "red", "green", "yellow", "blue",
      "magenta", "cyan", "white" };
  static const char* bg[] = { "bgblack", "bgred", "bggreen", "bgyellow",
      "bgblue", "bgmagenta", "bgcyan", "bgwhite" };

  *string = '\0';

  if (rendition->mask & BLINK_MASK)
    strcat (string, rendition->value & BLINK_MASK ? "blink" : "noblink");
  if (rendition->mask & BOLD_MASK)
    {
      if (*string != '\0')
        strcat (string, ",");
      strcat (string, rendition->value & BOLD_MASK ? "bold" : "nobold");
    }
  if (rendition->mask & STANDOUT_MASK)
    {
      if (*string != '\0')
        strcat (string, ",");
      strcat (string, rendition->value & STANDOUT_MASK ? "standout" : "nostandout");
    }
  if (rendition->mask & UNDERLINE_MASK)
    {
      if (*string != '\0')
        strcat (string, ",");
      strcat (string, rendition->value & UNDERLINE_MASK ? "underline" : "nounderline");
    }
  if (rendition->mask & COLOUR_MASK)
    {
      if (*string != '\0')
        strcat (string, ",");
      style = rendition->value & COLOUR_MASK;
      strcat (string, style >= 8 ? fg[style - 8] : "nocolour");
    }
  if (rendition->mask & BGCOLOUR_MASK)
    {
      if (*string != '\0')
        strcat (string, ",");
      style = (rendition->value & BGCOLOUR_MASK) >> 9;
      strcat (string, style >= 8 ? bg[style - 8] : "nobgcolour");
    }

  return string;
}

DECLARE_INFO_COMMAND (describe_variable, _("Explain the use of a variable"))
{
  VARIABLE_ALIST *var;
  char *description;

  /* Get the variable's name. */
  var = read_variable_name (_("Describe variable: "), window);
  if (!var)
    return;

  if (var->choices)
    asprintf (&description, "%s (%s): %s.",
             var->name,
             var->value == &highlight_searches
             ? on_off_choices[match_rendition.mask != 0]
             : var->choices == (char **) &rendition_choices
             ? rendition_to_string (var->value)
             : var->choices[*(int *)var->value], _(var->doc));
  else
    asprintf (&description, "%s (%d): %s.",
             var->name, *(int *)var->value, _(var->doc));

  window_message_in_echo_area ("%s", description);
  free (description);
}

DECLARE_INFO_COMMAND (set_variable, _("Set the value of an Info variable"))
{
  VARIABLE_ALIST *var;
  char *line;
  char prompt[100];

  /* Get the variable's name and value. */
  var = read_variable_name (_("Set variable: "), window);

  if (!var)
    return;

  /* Read a new value for this variable. */

  if (!var->choices)
    {
      int potential_value;

      if (info_explicit_arg || count != 1)
        potential_value = count;
      else
        potential_value = *(int *)(var->value);

      sprintf (prompt, _("Set %s to value (%d): "),
               var->name, potential_value);
      line = info_read_in_echo_area (prompt);

      /* User aborted? */
      if (!line)
        return;

      /* If the user specified a value, get that, otherwise, we are done. */
      canonicalize_whitespace (line);

      set_variable_to_value (var, line, SET_IN_SESSION);

      free (line);
    }
  else
    {
      register int i;
      REFERENCE **array = NULL;
      size_t array_index = 0;
      size_t array_slots = 0;

      for (i = 0; var->choices[i]; i++)
        {
          REFERENCE *entry;

          entry = xmalloc (sizeof (REFERENCE));
          entry->label = xstrdup (var->choices[i]);
          entry->nodename = NULL;
          entry->filename = NULL;

          add_pointer_to_array (entry, array_index, array, array_slots, 10);
        }

      sprintf (prompt, _("Set %s to value (%s): "),
               var->name,
               var->value == &highlight_searches
               ? on_off_choices[match_rendition.mask != 0]
               : var->choices == (char **) &rendition_choices
               ? rendition_to_string (var->value)
               : var->choices[*(int *)(var->value)]);

      /* Ask the completer to read a variable value for us. */
      if (var->choices == (char **) &rendition_choices)
        line = info_read_maybe_completing (prompt, array);
      else
        line = info_read_completing_in_echo_area (prompt, array);

      info_free_references (array);

      /* User aborted? */
      if (!line)
        {
          info_abort_key (active_window, 0);
          return;
        }

      /* User accepted default choice?  If so, no change. */
      if (!*line)
        {
          free (line);
          return;
        }

      set_variable_to_value (var, line, SET_IN_SESSION);
    }
}

VARIABLE_ALIST *
variable_by_name (char *name)
{
  int i;

  /* Find the variable in our list of variables. */
  for (i = 0; info_variables[i].name; i++)
    if (strcmp (info_variables[i].name, name) == 0)
      break;

  if (!info_variables[i].name)
    return NULL;
  else
    return &info_variables[i];
}

/* Read the name of an Info variable in the echo area and return the
   address of a VARIABLE_ALIST member.  A return value of NULL indicates
   that no variable could be read. */
static VARIABLE_ALIST *
read_variable_name (char *prompt, WINDOW *window)
{
  char *line;
  REFERENCE **variables;

  /* Get the completion array of variable names. */
  variables = make_variable_completions_array ();

  /* Ask the completer to read a variable for us. */
  line = info_read_completing_in_echo_area (prompt, variables);

  info_free_references (variables);

  /* User aborted? */
  if (!line)
    {
      info_abort_key (active_window, 0);
      return NULL;
    }

  /* User accepted "default"?  (There is none.) */
  if (!*line)
    {
      free (line);
      return NULL;
    }

  return variable_by_name (line);
}

/* Make an array of REFERENCE which actually contains the names of the
   variables available in Info. */
REFERENCE **
make_variable_completions_array (void)
{
  register int i;
  REFERENCE **array = NULL;
  size_t array_index = 0, array_slots = 0;

  for (i = 0; info_variables[i].name; i++)
    {
      REFERENCE *entry;

      entry = xmalloc (sizeof (REFERENCE));
      entry->label = xstrdup (info_variables[i].name);
      entry->nodename = NULL;
      entry->filename = NULL;

      add_pointer_to_array (entry, array_index, array, array_slots, 200);
    }

  return array;
}

/* VALUE is a string that is the value of the variable specified
   by the user.  Update our internal data structure VAR using this
   information. */
int
set_variable_to_value (VARIABLE_ALIST *var, char *value, int where)
{
  /* If variable was set elsewhere with a higher priority, don't do
     anything, but don't indicate an error. */
  if (var->where_set > where)
    return 1;

  if (var->choices)
    {
      register int j;

      /* "highlight-searches=On" is equivalent to
         "match-rendition=standout". */
      if (var->value == &highlight_searches)
        {
          if (strcmp (on_off_choices[0], value) == 0)
            {
              match_rendition.mask = 0;
              match_rendition.value = 0;
            }
          else
            {
              match_rendition.mask = STANDOUT_MASK;
              match_rendition.value = STANDOUT_MASK;
            }
        }
      else if (var->choices != (char **) &rendition_choices)
        {
          /* Find the choice in our list of choices. */
          for (j = 0; var->choices[j]; j++)
            if (strcmp (var->choices[j], value) == 0)
              {
                *(int *)var->value = j;
                var->where_set = where;
                return 1;
              }
        }
      else
        {
          static struct {
              unsigned long mask;
              unsigned long value;
              char *name;
          } styles[] = {
              COLOUR_MASK, COLOUR_BLACK,   "black",
              COLOUR_MASK, COLOUR_RED,     "red",
              COLOUR_MASK, COLOUR_GREEN,   "green",
              COLOUR_MASK, COLOUR_YELLOW,  "yellow",
              COLOUR_MASK, COLOUR_BLUE,    "blue",
              COLOUR_MASK, COLOUR_MAGENTA, "magenta",
              COLOUR_MASK, COLOUR_CYAN,    "cyan",
              COLOUR_MASK, COLOUR_WHITE,   "white",
              COLOUR_MASK, 0,           "nocolour",
              COLOUR_MASK, 0,           "nocolor",
              BGCOLOUR_MASK, BGCOLOUR_BLACK,   "bgblack",
              BGCOLOUR_MASK, BGCOLOUR_RED,     "bgred",
              BGCOLOUR_MASK, BGCOLOUR_GREEN,   "bggreen",
              BGCOLOUR_MASK, BGCOLOUR_YELLOW,  "bgyellow",
              BGCOLOUR_MASK, BGCOLOUR_BLUE,    "bgblue",
              BGCOLOUR_MASK, BGCOLOUR_MAGENTA, "bgmagenta",
              BGCOLOUR_MASK, BGCOLOUR_CYAN,    "bgcyan",
              BGCOLOUR_MASK, BGCOLOUR_WHITE,   "bgwhite",
              BGCOLOUR_MASK, 0,           "nobgcolour",
              BGCOLOUR_MASK, 0,           "nobgcolor",
              UNDERLINE_MASK, UNDERLINE_MASK, "underline",
              UNDERLINE_MASK, 0,              "nounderline",
              STANDOUT_MASK, STANDOUT_MASK, "standout",
              STANDOUT_MASK, 0,             "nostandout",
              BOLD_MASK, BOLD_MASK,         "bold",
              BOLD_MASK, 0,                 "regular",
              BOLD_MASK, 0,                 "nobold",
              BLINK_MASK, BLINK_MASK,       "blink",
              BLINK_MASK, 0,                "noblink",
          };
          int i;
          char *component;
          unsigned long rendition_mask = 0;
          unsigned long rendition_value = 0;

          component = strtok (value, ",");
          while (component)
            {
              for (i = 0; (styles[i].name); i++)
                {
                  if (!strcmp (styles[i].name, component))
                    break;
                }
              if (styles[i].name)
                {
                  rendition_mask |= styles[i].mask;
                  rendition_value &= ~styles[i].mask;
                  rendition_value |= styles[i].value;
                }
              /* If not found, silently ignore, in case more options are
                 added in the future. */

              component = strtok (0, ",");
            }

          /* Now all the specified styles are recorded in rendition_value. */
          ((RENDITION *)var->value)->mask = rendition_mask;
          ((RENDITION *)var->value)->value = rendition_value;
        }
      return 1;
    }
  else
    {
      char *p;
      long n = strtol (value, &p, 10);
      if (*p == 0 && INT_MIN <= n && n <= INT_MAX)
	{
          *(int *)var->value = n;
	  return 1;
	}
    }

  return 0;
}

