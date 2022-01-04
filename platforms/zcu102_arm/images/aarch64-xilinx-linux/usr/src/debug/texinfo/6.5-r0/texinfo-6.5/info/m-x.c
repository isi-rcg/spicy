/* m-x.c -- Meta-x minibuffer reader.
   $Id: m-x.c 7778 2017-05-14 10:54:47Z gavin $

   Copyright 1993, 1997, 1998, 2001, 2002, 2004, 2007, 2008, 2011, 2013,
   2014, 2015, 2016, 2017 Free Software Foundation, Inc.

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
#include "display.h"
#include "session.h"
#include "echo-area.h"
#include "funs.h"

/* **************************************************************** */
/*                                                                  */
/*                     Reading Named Commands                       */
/*                                                                  */
/* **************************************************************** */

/* Read the name of an Info function in the echo area and return the
   name.  A return value of NULL indicates that no function name could
   be read. */
char *
read_function_name (char *prompt, WINDOW *window)
{
  register int i;
  char *line;
  REFERENCE **array = NULL;
  size_t array_index = 0, array_slots = 0;

  /* Make an array of REFERENCE which actually contains the names of
     the functions available in Info. */
  for (i = 0; function_doc_array[i].func; i++)
    {
      REFERENCE *entry;

      entry = xmalloc (sizeof (REFERENCE));
      entry->label = xstrdup (function_doc_array[i].func_name);
      entry->nodename = NULL;
      entry->filename = NULL;

      add_pointer_to_array (entry, array_index, array, array_slots, 200);
    }

  line = info_read_completing_in_echo_area (prompt, array);
  info_free_references (array);

  return line;
}

DECLARE_INFO_COMMAND (describe_command,
   _("Read the name of an Info command and describe it"))
{
  char *line;

  line = read_function_name (_("Describe command: "), window);

  if (!line)
    {
      info_abort_key (active_window, count);
      return;
    }

  /* Describe the function named in "LINE". */
  if (*line)
    {
      InfoCommand *cmd = named_function (line);

      if (!cmd)
        return;

      window_message_in_echo_area ("%s: %s.",
                                   line, function_documentation (cmd));
    }
  free (line);
}

DECLARE_INFO_COMMAND (info_execute_command,
   _("Read a command name in the echo area and execute it"))
{
  char *line;
  char *keys;
  char *prompt;

  keys = where_is (info_keymap, InfoCmd(info_execute_command));
  /* If the where_is () function thinks that this command doesn't exist,
     there's something very wrong!  */
  if (!keys)
    abort();

  if (info_explicit_arg || count != 1)
    asprintf (&prompt, "%d %s ", count, keys);
  else
    asprintf (&prompt, "%s ", keys);

  /* Ask the completer to read a reference for us. */
  line = read_function_name (prompt, window);
  free (prompt);

  /* User aborted? */
  if (!line)
    {
      info_abort_key (active_window, count);
      return;
    }

  /* User accepted "default"?  (There is none.) */
  if (!*line)
    {
      free (line);
      return;
    }

  /* User wants to execute a named command.  Do it. */
  {
    InfoCommand *command;

    if ((active_window != the_echo_area) &&
        (strncmp (line, "echo-area-", 10) == 0))
      {
        free (line);
        info_error (_("Cannot execute an 'echo-area' command here"));
        return;
      }

    command = named_function (line);
    free (line);

    if (command && command->func)
      (*command->func) (active_window, count, 0);
  }
}

/* Okay, now that we have M-x, let the user set the screen height. */
DECLARE_INFO_COMMAND (set_screen_height,
  _("Set the height of the displayed window"))
{
  int new_height, old_height = screenheight;

  if (info_explicit_arg || count != 1)
    new_height = count;
  else
    {
      char prompt[80];
      char *line;

      new_height = screenheight;

      sprintf (prompt, _("Set screen height to (%d): "), new_height);

      line = info_read_in_echo_area (prompt);

      /* If the user aborted, do that now. */
      if (!line)
        {
          info_abort_key (active_window, count);
          return;
        }

      /* Find out what the new height is supposed to be. */
      if (*line)
        new_height = atoi (line);

      free (line);
    }

  terminal_clear_screen ();
  display_clear_display (the_display);
  screenheight = new_height;
#ifdef SET_SCREEN_SIZE_HELPER
  SET_SCREEN_SIZE_HELPER;
#endif
  if (screenheight == old_height)
    {
      /* Display dimensions didn't actually change, so
	 window_new_screen_size won't do anything, but we've
	 already cleared the display above.  Undo the damage.  */
      window_mark_chain (windows, W_UpdateWindow);
      display_update_display ();
    }
  else
    {
      display_initialize_display (screenwidth, screenheight);
      window_new_screen_size (screenwidth, screenheight);
    }
}
