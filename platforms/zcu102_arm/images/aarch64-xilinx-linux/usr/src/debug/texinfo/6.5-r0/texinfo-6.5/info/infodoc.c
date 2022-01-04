/* infodoc.c -- functions which build documentation nodes.
   $Id: infodoc.c 7801 2017-05-20 13:33:45Z gavin $

   Copyright 1993, 1997, 1998, 1999, 2001, 2002, 2003, 2004, 2006,
   2007, 2008, 2011, 2013, 2014, 2015, 2016, 2017 Free Software Foundation, 
   Inc.

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
#include "info-utils.h"
#include "filesys.h"
#include "session.h"
#include "doc.h"
#include "funs.h"

/* The name of the node used in the help window. */
static char *info_help_nodename = "*Info Help*";

/* A node containing printed key bindings and their documentation. */
static NODE *internal_info_help_node = NULL;

/* The (more or less) static text which appears in the internal info
   help node.  The actual key bindings are inserted.  Keep the
   underlines (****, etc.) in the same N_ call as  the text lines they
   refer to, so translations can make the number of *'s or -'s match.  */
static char *info_internal_help_text[] = {
  N_("Basic Info command keys\n"),
  "\n",
  N_("\\%-10[quit-help]  Close this help window.\n"),
  N_("\\%-10[quit]  Quit Info altogether.\n"),
  N_("\\%-10[get-info-help-node]  Invoke the Info tutorial.\n"),
  "\n",
  N_("\\%-10[prev-line]  Move up one line.\n"),
  N_("\\%-10[next-line]  Move down one line.\n"),
  N_("\\%-10[scroll-backward]  Scroll backward one screenful.\n"),
  N_("\\%-10[scroll-forward]  Scroll forward one screenful.\n"),
  N_("\\%-10[beginning-of-node]  Go to the beginning of this node.\n"),
  N_("\\%-10[end-of-node]  Go to the end of this node.\n"),
  "\n",
  N_("\\%-10[move-to-next-xref]  Skip to the next hypertext link.\n"),
  N_("\\%-10[select-reference-this-line]  Follow the hypertext link under the cursor.\n"),
  N_("\\%-10[history-node]  Go back to the last node seen in this window.\n"),
  "\n",
  N_("\\%-10[global-prev-node]  Go to the previous node in the document.\n"),
  N_("\\%-10[global-next-node]  Go to the next node in the document.\n"),
  N_("\\%-10[prev-node]  Go to the previous node on this level.\n"),
  N_("\\%-10[next-node]  Go to the next node on this level.\n"),
  N_("\\%-10[up-node]  Go up one level.\n"),
  N_("\\%-10[top-node]  Go to the top node of this document.\n"),
  N_("\\%-10[dir-node]  Go to the main 'directory' node.\n"),
  "\n",
  N_("1...9       Pick the first...ninth item in this node's menu.\n"),
  N_("\\%-10[last-menu-item]  Pick the last item in this node's menu.\n"),
  N_("\\%-10[menu-item]  Pick a menu item specified by name.\n"),
  N_("\\%-10[xref-item]  Follow a cross reference specified by name.\n"),
  N_("\\%-10[goto-node]  Go to a node specified by name.\n"),
  "\n",
  N_("\\%-10[search]  Search forward for a specified string.\n"),
  N_("\\%-10[search-previous]  Search for previous occurrence.\n"),
  N_("\\%-10[search-next]  Search for next occurrence.\n"),
  N_("\\%-10[index-search]  Search for a specified string in the index, and\n\
              select the node referenced by the first entry found.\n"),
  N_("\\%-10[virtual-index]  Synthesize menu of matching index entries.\n"),
  "\n",
  N_("\\%-10[abort-key]  Cancel the current operation.\n"),
  "\n",
  NULL
};

static char *where_is_internal (Keymap map, InfoCommand *cmd);

static void
dump_map_to_text_buffer (struct text_buffer *tb, int *prefix,
                         int prefix_len, Keymap map)
{
  register int i;
  int *new_prefix = xmalloc ((prefix_len + 2) * sizeof (int));

  memcpy (new_prefix, prefix, prefix_len * sizeof (int));
  new_prefix[prefix_len + 1] = 0;

  for (i = 0; i < KEYMAP_SIZE; i++)
    {
      if (i == 128)
        i = 256;
      if (i == 128 + KEYMAP_META_BASE)
        i = 256 + KEYMAP_META_BASE;

      new_prefix[prefix_len] = i;
      if (map[i].type == ISKMAP)
        {
          dump_map_to_text_buffer (tb, new_prefix, prefix_len + 1,
                                   map[i].value.keymap);
        }
      else if (map[i].value.function)
        {
          long start_of_line = tb->off;
          register int last;
          char *doc, *name;

          /* Hide some key mappings. */
          if (map[i].value.function
              && (map[i].value.function->func == info_do_lowercase_version))
            continue;

          doc = function_documentation (map[i].value.function);
          name = function_name (map[i].value.function);

          if (!*doc)
            continue;

          /* Find out if there is a series of identical functions, as in
             add-digit-to-numeric-arg. */
          for (last = i + 1; last < KEYMAP_SIZE; last++)
            if ((map[last].type != ISFUNC) ||
                (map[last].value.function != map[i].value.function))
              break;

          if (last - 1 != i)
            {
              text_buffer_printf (tb, "%s .. ", pretty_keyseq (new_prefix));
              new_prefix[prefix_len] = last - 1;
              text_buffer_printf (tb, "%s", pretty_keyseq (new_prefix));
              i = last - 1;
            }
          else
            text_buffer_printf (tb, "%s", pretty_keyseq (new_prefix));

          while (tb->off - start_of_line < 8)
            text_buffer_printf (tb, " ");

          /* Print the name of the function, and some padding before the
             documentation string is printed. */
          {
            int length_so_far;
            int desired_doc_start = 40;

            text_buffer_printf (tb, "(%s)", name);
            length_so_far = tb->off - start_of_line;

            if ((desired_doc_start + strlen (doc))
                >= (unsigned int) the_screen->width)
              text_buffer_printf (tb, "\n     ");
            else
              {
                while (length_so_far < desired_doc_start)
                  {
                    text_buffer_printf (tb, " ");
                    length_so_far++;
                  }
              }
          }
          text_buffer_printf (tb, "%s\n", doc);
        }
    }
  free (new_prefix);
}

/* How to create internal_info_help_node.  HELP_IS_ONLY_WINDOW_P says
   whether we're going to end up in a second (or more) window of our
   own, or whether there's only one window and we're going to usurp it.
   This determines how to quit the help window.  Maybe we should just
   make q do the right thing in both cases.  */

static void
create_internal_info_help_node (int help_is_only_window_p)
{
  register int i;
  NODE *node;
  char *exec_keys;

  int printed_one_mx = 0;
  struct text_buffer msg;
  char *infopath_str = infopath_string ();

  text_buffer_init (&msg);

  for (i = 0; info_internal_help_text[i]; i++)
    text_buffer_printf (&msg, replace_in_documentation
                        (_(info_internal_help_text[i]),
                         help_is_only_window_p), NULL, NULL, NULL);

  text_buffer_printf (&msg, "---------------------\n");
  text_buffer_printf (&msg, _("This is GNU Info version %s.  "), VERSION);
  text_buffer_printf (&msg, _("The current search path is:\n"));
  text_buffer_printf (&msg, "%s\n", infopath_str);
  text_buffer_printf (&msg, "---------------------\n\n");
  free (infopath_str);

  text_buffer_printf (&msg, _("Commands available in Info windows:\n\n"));
  dump_map_to_text_buffer (&msg, 0, 0, info_keymap);
  text_buffer_printf (&msg, "---------------------\n\n");
  text_buffer_printf (&msg, _("Commands available in the echo area:\n\n"));
  dump_map_to_text_buffer (&msg, 0, 0, echo_area_keymap);

  /* Get a list of commands which have no keystroke equivs. */
  exec_keys = where_is (info_keymap, InfoCmd(info_execute_command));
  if (exec_keys)
    exec_keys = xstrdup (exec_keys);
  for (i = 0; function_doc_array[i].func; i++)
    {
      InfoCommand *cmd = &function_doc_array[i];

      if (cmd->func != info_do_lowercase_version
          && !where_is_internal (info_keymap, cmd)
          && !where_is_internal (echo_area_keymap, cmd))
        {
          if (!printed_one_mx)
            {
              text_buffer_printf (&msg, "---------------------\n\n");
              if (exec_keys && exec_keys[0])
                text_buffer_printf (&msg,
                    _("The following commands can only be invoked via "
                      "%s:\n\n"),
                    exec_keys);
              else
                text_buffer_printf (&msg,
                   _("The following commands cannot be invoked at all:\n\n"));
              printed_one_mx = 1;
            }

          text_buffer_printf (&msg,
             "%s %s\n     %s\n",
             exec_keys,
             function_doc_array[i].func_name,
             replace_in_documentation (strlen (function_doc_array[i].doc)
               ? _(function_doc_array[i].doc) : "", 0)
            );

        }
    }

  free (exec_keys);

  node = text_buffer_to_node (&msg);

  internal_info_help_node = node;

  name_internal_node (internal_info_help_node, xstrdup (info_help_nodename));
}

/* Return a window which is the window showing help in this Info. */

/* If the eligible window's height is >= this, split it to make the help
   window.  Otherwise display the help window in the current window.  */
#define HELP_SPLIT_SIZE 24

static WINDOW *
info_find_or_create_help_window (void)
{
  int help_is_only_window_p;
  WINDOW *eligible = NULL;
  WINDOW *help_window = get_internal_info_window (info_help_nodename);

  /* Close help window if in it already. */
  if (help_window && help_window == active_window)
    {
      info_delete_window_internal (help_window);
      return NULL;
    }

  /* If we couldn't find the help window, then make it. */
  if (!help_window)
    {
      WINDOW *window;
      int max = 0;

      for (window = windows; window; window = window->next)
        {
          if (window->height > max)
            {
              max = window->height;
              eligible = window;
            }
        }

      if (!eligible)
        {
          info_error ("%s", msg_cant_make_help);
          return NULL;
        }
    }

  /* Make sure that we have a node containing the help text.  The
     argument is false if help will be the only window (so l must be used
     to quit help), true if help will be one of several visible windows
     (so CTRL-x 0 must be used to quit help).  */
  help_is_only_window_p = ((help_window && !windows->next)
        || (!help_window && eligible->height < HELP_SPLIT_SIZE));
  create_internal_info_help_node (help_is_only_window_p);

  /* Either use the existing window to display the help node, or create
     a new window if there was no existing help window. */
  if (!help_window)
    { /* Split the largest window into 2 windows, and show the help text
         in that window. */
      if (eligible->height >= HELP_SPLIT_SIZE)
        {
          active_window = eligible;
          help_window = window_make_window ();
          info_set_node_of_window (help_window, internal_info_help_node);
        }
      else
        {
          info_set_node_of_window (active_window, internal_info_help_node);
          help_window = active_window;
        }
    }
  else
    { /* Case where help node always gets regenerated, and we have an
         existing window in which to place the node. */
      if (active_window != help_window)
        {
          active_window = help_window;
        }
      info_set_node_of_window (active_window, internal_info_help_node);
    }
  return help_window;
}

/* Create or move to the help window. */
DECLARE_INFO_COMMAND (info_get_help_window, _("Display help message"))
{
  WINDOW *help_window;

  help_window = info_find_or_create_help_window ();
  if (help_window)
    {
      active_window = help_window;
    }
}

/* Show the Info help node.  This means that the "info" file is installed
   where it can easily be found on your system. */
DECLARE_INFO_COMMAND (info_get_info_help_node, _("Visit Info node '(info)Help'"))
{
  NODE *node;
  char *nodename;

  /* If there is a window on the screen showing the node "(info)Help" or
     the node "(info)Help-Small-Screen", simply select that window. */
  {
    WINDOW *win;

    for (win = windows; win; win = win->next)
      {
        if (win->node && win->node->fullpath
            && !mbscasecmp ("info",
                            filename_non_directory (win->node->fullpath))
            && (!strcmp (win->node->nodename, "Help")
                || !strcmp (win->node->nodename, "Help-Small-Screen")))
          {
            active_window = win;
            return;
          }
      }
  }

  /* If there is more than one window on the screen, check if the user typed 
     "H" for help message before typing "h" for tutorial.  If so, close help 
     message so the tutorial will not be in a small window. */
  if (windows->next)
    {
      WINDOW *help_window = get_internal_info_window (info_help_nodename);
      if (help_window && help_window == active_window)
        {
          info_delete_window_internal (help_window);
        }
    }

  /* If the current window is small, show the small screen help. */
  if (active_window->height < 24)
    nodename = "Help-Small-Screen";
  else
    nodename = "Help";

  /* Try to get the info file for Info. */
  node = info_get_node ("info", nodename);

  /* info.info is distributed with Emacs, not Texinfo, so fall back to 
     info-stnd.info if it isn't there. */
  if (!node)
    node = info_get_node ("info-stnd", "Top");

  if (!node)
    {
      if (info_recent_file_error)
        info_error ("%s", info_recent_file_error);
      else
        info_error (msg_cant_file_node, "info", nodename);
        
      return;
    }

  info_set_node_of_window (active_window, node);
}

/* **************************************************************** */
/*                                                                  */
/*                   Groveling Info Keymaps and Docs                */
/*                                                                  */
/* **************************************************************** */

/* Return the documentation associated with the Info command FUNCTION. */
char *
function_documentation (InfoCommand *cmd)
{
  char *doc;

  doc = cmd->doc;

  return replace_in_documentation ((strlen (doc) == 0) ? doc : _(doc), 0);
}

/* Return the user-visible name of the function associated with the
   Info command FUNCTION. */
char *
function_name (InfoCommand *cmd)
{
  return cmd->func_name;
}

/* Return a pointer to the info command for function NAME. */
InfoCommand *
named_function (char *name)
{
  register int i;

  for (i = 0; function_doc_array[i].func; i++)
    if (strcmp (function_doc_array[i].func_name, name) == 0)
      break;

  if (!function_doc_array[i].func)
    return 0;
  else
    return &function_doc_array[i];
}

DECLARE_INFO_COMMAND (describe_key, _("Print documentation for KEY"))
{
  int keys[50];
  int keystroke;
  int *k = keys;
  Keymap map = info_keymap;

  *k = '\0';

  for (;;)
    {
      message_in_echo_area (_("Describe key: %s"), pretty_keyseq (keys));
      keystroke = get_input_key ();
      unmessage_in_echo_area ();

      /* Add the KEYSTROKE to our list. */
      *k++ = keystroke;
      *k = '\0';

      if (map[keystroke].value.function == NULL)
        {
          message_in_echo_area (_("%s is undefined"), pretty_keyseq (keys));
          return;
        }
      else if (map[keystroke].type == ISKMAP)
        {
          map = map[keystroke].value.keymap;
          continue;
        }
      else
        {
          char *keyname, *message, *fundoc, *funname = "";

          /* If the key is bound to do-lowercase-version, but its
             lower-case variant is undefined, say that this key is
             also undefined.  This is especially important for unbound
             edit keys that emit an escape sequence: it's terribly
             confusing to see a message "Home (do-lowercase-version)"
             or some such when Home is unbound.  */
          if (map[keystroke].value.function
              && map[keystroke].value.function->func
                 == info_do_lowercase_version)
            {
              int lowerkey;

              if (keystroke >= KEYMAP_META_BASE)
                {
                  lowerkey = keystroke;
                  lowerkey -= KEYMAP_META_BASE;
                  lowerkey = tolower (lowerkey);
                  lowerkey += KEYMAP_META_BASE;
                }
              else
                lowerkey = tolower (keystroke);

              if (map[lowerkey].value.function == NULL)
                {
                  message_in_echo_area (_("%s is undefined"),
					pretty_keyseq (keys));
                  return;
                }
            }

          keyname = pretty_keyseq (keys);

          funname = function_name (map[keystroke].value.function);

          fundoc = function_documentation (map[keystroke].value.function);

          message = xmalloc
            (10 + strlen (keyname) + strlen (fundoc) + strlen (funname));

          sprintf (message, "%s (%s): %s.", keyname, funname, fundoc);

          window_message_in_echo_area ("%s", message);
          free (message);
          break;
        }
    }
}

/* Return the pretty-printable name of a single key. */
char *
pretty_keyname (int key)
{
  static char rep_buffer[30];
  char *rep;

  if (key >= KEYMAP_META_BASE)
    {
      char temp[20];

      rep = pretty_keyname (key - KEYMAP_META_BASE);

      sprintf (temp, "M-%s", rep);
      strcpy (rep_buffer, temp);
      rep = rep_buffer;
    }
  else if (Control_p (key))
    {
      switch (key)
        {
        case '\n': rep = "LFD"; break;
        case '\t': rep = "TAB"; break;
        case '\r': rep = "RET"; break;
        case ESC:  rep = "ESC"; break;

        default:
          sprintf (rep_buffer, "C-%c", UnControl (key));
          rep = rep_buffer;
        }
    }
  else if (key >= 256)
    switch (key)
      {
      case KEY_RIGHT_ARROW:
        rep = "Right"; break;
      case KEY_LEFT_ARROW:
        rep = "Left"; break;
      case KEY_UP_ARROW:
        rep = "Up"; break;
      case KEY_DOWN_ARROW:
        rep = "Down"; break;
      case KEY_PAGE_UP:
        rep = "PgUp"; break;
      case KEY_PAGE_DOWN:
        rep = "PgDn"; break;
      case KEY_HOME:
        rep = "Home"; break;
      case KEY_END:
        rep = "End"; break;
      case KEY_DELETE:
        rep = "DEL"; break;
      case KEY_INSERT:
        rep = "INS"; break;
      case KEY_BACK_TAB:
        rep = "BackTab"; break;
      case KEY_MOUSE:
        rep = "(mouse event)"; break;
      default:
        rep = "(unknown key)"; break; /* This shouldn't be displayed. */
      }
  else
    {
      switch (key)
        {
        case ' ': rep = "SPC"; break;
        case DEL: rep = "DEL"; break;
        default:
          rep_buffer[0] = key;
          rep_buffer[1] = '\0';
          rep = rep_buffer;
        }
    }
  return rep;
}

/* Return the pretty printable string which represents KEYSEQ.  Return
   value should not be freed by caller. */
char *
pretty_keyseq (int *keyseq)
{
  static struct text_buffer rep = { 0 };

  if (!text_buffer_base (&rep))
    text_buffer_init (&rep);
  else
    text_buffer_reset (&rep);

  if (!*keyseq)
    return "";

  while (1)
    {
      text_buffer_printf (&rep, "%s", pretty_keyname (keyseq[0]));
      keyseq++;

      if (!*keyseq)
        break;

      text_buffer_add_char (&rep, ' ');
    }
  return text_buffer_base (&rep);
}

/* Replace the names of functions with the key that invokes them.
   Return value should not be freed by caller. */
char *
replace_in_documentation (const char *string, int help_is_only_window_p)
{
  register int i, start;
  static struct text_buffer txtresult = {0};

  text_buffer_free (&txtresult);
  text_buffer_init (&txtresult);
  text_buffer_alloc (&txtresult, strlen (string));

  start = 0;

  /* Skip to the beginning of a replaceable function. */
  for (i = start; string[i]; i++)
    {
      int j = i + 1;

      /* Is this the start of a replaceable function name? */
      if (string[i] == '\\')
        {
          char *fmt = NULL;

          if(string[j] == '%')
            {
              if (string[++j] == '-')
                j++;
              if (isdigit(string[j]))
                {
                  while (isdigit(string[j]))
                    j++;
                  if (string[j] == '.' && isdigit(string[j + 1]))
                    {
                      j += 1;
                      while (isdigit(string[j]))
                        j++;
                    }
                  fmt = xmalloc (j - i + 2);
                  strncpy (fmt, string + i + 1, j - i);
                  fmt[j - i - 1] = 's';
                  fmt[j - i] = '\0';
                }
              else
                j = i + 1;
            }
          if (string[j] == '[')
            {
              char *rep_name, *fun_name, *rep;
              InfoCommand *command;
              unsigned replen;

              /* Copy in the old text. */
              text_buffer_add_string (&txtresult, string + start, i - start);
              start = j + 1;

              /* Move to the end of the function name. */
              for (i = start; string[i] && (string[i] != ']'); i++);

              rep_name = xmalloc (1 + i - start);
              strncpy (rep_name, string + start, i - start);
              rep_name[i - start] = '\0';

              start = i;
              if (string[start] == ']')
                start++;

              fun_name = rep_name;
              if (strcmp (rep_name, "quit-help") == 0)
                {
                  /* Special case for help window.  If we have only one window 
                     (because the window size was too small to split it), we 
                     have to quit help by going back one node in the history 
                     list, not deleting the window.  */

                  fun_name = help_is_only_window_p ? "history-node"
                                                   : "get-help-window";
                }

              /* Find a key which invokes this function in the info_keymap. */
              command = named_function (fun_name);
              free (rep_name);

              /* If the internal documentation string fails, there is a
                 serious problem with the associated command's documentation.
                 We croak so that it can be fixed immediately. */
              if (!command)
                abort ();

              rep = where_is (info_keymap, command);
              if (!rep)
                rep = "N/A";
              replen = strlen (rep);

              if (fmt)
                text_buffer_printf (&txtresult, fmt, rep);
              else
                text_buffer_add_string (&txtresult, rep, replen);
            }

          free (fmt);
        }
    }
  text_buffer_add_string (&txtresult,
                          string + start, strlen (string + start) + 1);
  return text_buffer_base (&txtresult);
}

/* Return a string of characters which could be typed from the keymap
   MAP to invoke FUNCTION. */
static char *where_is_rep = NULL;
static int where_is_rep_index = 0;
static int where_is_rep_size = 0;

char *
where_is (Keymap map, InfoCommand *cmd)
{
  char *rep;

  if (!where_is_rep_size)
    where_is_rep = xmalloc (where_is_rep_size = 100);
  where_is_rep_index = 0;

  rep = where_is_internal (map, cmd);

  /* If it couldn't be found, return "M-x Foo" (or equivalent). */
  if (!rep)
    {
      char *name;

      name = function_name (cmd);
      if (!name)
        return NULL; /* no such function */

      rep = where_is_internal (map, InfoCmd(info_execute_command));
      if (!rep)
        return ""; /* function exists but can't be got to by user */

      sprintf (where_is_rep, "%s %s", rep, name);

      rep = where_is_rep;
    }
  return rep;
}

/* Return the printed rep of the keystrokes that invoke FUNCTION,
   as found in MAP, or NULL. */
static char *
where_is_internal (Keymap map, InfoCommand *cmd)
{
  register FUNCTION_KEYSEQ *k;

  for (k = cmd->keys; k; k = k->next)
    if (k->map == map)
      return pretty_keyseq (k->keyseq);

  return NULL;
}

DECLARE_INFO_COMMAND (info_where_is,
   _("Show what to type to execute a given command"))
{
  char *command_name;

  command_name = read_function_name (_("Where is command: "), window);

  if (!command_name)
    {
      info_abort_key (active_window, count);
      return;
    }

  if (*command_name)
    {
      InfoCommand *command;

      command = named_function (command_name);

      if (command)
        {
          char *location;

          location = where_is (info_keymap, command);

          if (!location || !location[0])
            {
              info_error (_("'%s' is not on any keys"), command_name);
            }
          else
            {
              if (strstr (location, function_name (command)))
                window_message_in_echo_area
                  (_("%s can only be invoked via %s"),
                   command_name, location);
              else
                window_message_in_echo_area
                  (_("%s can be invoked via %s"),
                   command_name, location);
            }
        }
      else
        info_error (_("There is no function named '%s'"), command_name);
    }

  free (command_name);
}
