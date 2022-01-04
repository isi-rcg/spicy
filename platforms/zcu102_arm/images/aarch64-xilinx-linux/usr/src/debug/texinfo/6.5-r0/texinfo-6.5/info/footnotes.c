/* footnotes.c -- Some functions for manipulating footnotes.
   $Id: footnotes.c 5949 2014-12-03 17:53:45Z gavin $

   Copyright 1993, 1997, 1998, 1999, 2002, 2004, 2007, 2008, 2011, 2013,
   2014 Free Software Foundation, Inc.

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
#include "info-utils.h"
#include "footnotes.h"

/* Nonzero means attempt to show footnotes when displaying a new window. */
int auto_footnotes_p = 0;

static char *footnote_nodename = "*Footnotes*";

/* Find the window currently showing footnotes. */
static WINDOW *
find_footnotes_window (void)
{
  WINDOW *win;

  /* Try to find an existing window first. */
  for (win = windows; win; win = win->next)
    if (internal_info_node_p (win->node) &&
        (strcmp (win->node->nodename, footnote_nodename) == 0))
      break;

  return win;
}

/* Manufacture a node containing the footnotes of this node, and
   return the manufactured node.  If NODE has no footnotes, return a 
   NULL pointer. */
NODE *
make_footnotes_node (NODE *node)
{
  NODE *fn_node, *footnotes_node = NULL, *result = NULL;
  long fn_start = -1;
  char *fnptr;

  /* Make the initial assumption that the footnotes appear as simple
     text within this windows node. */
  fn_node = node;

  /* See if this node contains the magic footnote label. */
    {
      char saved = node->contents[node->nodelen];
      node->contents[node->nodelen] = '\0';
      fnptr = strstr (node->contents, FOOTNOTE_LABEL);
      node->contents[node->nodelen] = saved;
    }
  if (fnptr)
    fn_start = fnptr - node->contents;

  /* If it doesn't, check to see if it has an associated footnotes node. */
  if (!fnptr)
    {
      REFERENCE **refs;

      refs = node->references;

      if (refs)
        {
          register int i;
          char *refname;
          int reflen = strlen ("-Footnotes") + strlen (node->nodename);

          refname = xmalloc (reflen + 1);

          strcpy (refname, node->nodename);
          strcat (refname, "-Footnotes");

          for (i = 0; refs[i]; i++)
            if (refs[i]->type == REFERENCE_XREF
                && (refs[i]->nodename != NULL)
                /* Support both the older "foo-Footnotes" and the new
                   style "foo-Footnote-NN" references.  */
                && (strcmp (refs[i]->nodename, refname) == 0 ||
                 (strncmp (refs[i]->nodename, refname, reflen - 1) == 0 &&
                  refs[i]->nodename[reflen - 1] == '-' &&
                  isdigit (refs[i]->nodename[reflen]))))
              {
                footnotes_node = info_get_node (node->fullpath, refname);
                if (footnotes_node)
                  {
                    fn_node = footnotes_node;
                    fn_start = 0;
                  }
                break;
              }

          free (refname);
        }
    }

  /* If we never found the start of a footnotes area, quit now. */
  if (fn_start == -1)
    return NULL;

  /* Make the new node. */
  result = info_create_node ();

  /* Get the size of the footnotes appearing within this node. */
  {
    char *header;
    long text_start = fn_start;

    asprintf (&header,
              "*** Footnotes appearing in the node '%s' ***\n",
              node->nodename);

    /* Move the start of the displayed text to right after the first line.
       This effectively skips either "---- footno...", or "File: foo...". */
    while (text_start < fn_node->nodelen)
      if (fn_node->contents[text_start++] == '\n')
        break;
  
    result->nodelen = strlen (header) + fn_node->nodelen - text_start;

    /* Set the contents of this node. */
    result->contents = xmalloc (1 + result->nodelen);
    sprintf (result->contents, "%s", header);
    memcpy (result->contents + strlen (header),
            fn_node->contents + text_start, fn_node->nodelen - text_start);
    result->contents[strlen (header) + fn_node->nodelen - text_start] = '\0';

   /* Copy and adjust references that appear in footnotes section. */
    {
      REFERENCE **ref = fn_node->references;

      for (; *ref; ref++)
        {
          if ((*ref)->start > text_start)
            break;
        }

      result->references = info_copy_references (ref);

      for (ref = result->references; *ref; ref++)
        {
          (*ref)->start -= text_start - strlen (header);
          (*ref)->end -= text_start - strlen (header);
        }
    }

    result->nodename = xstrdup (footnote_nodename);
    result->flags |= N_IsInternal | N_WasRewritten;

    /* Needed in case the user follows a reference in the footnotes window. */
    result->fullpath = fn_node->fullpath;
    result->subfile = fn_node->subfile;

    free (header);
  }

  free_history_node (footnotes_node);
  return result;
}

/* Create or delete the footnotes window depending on whether footnotes
   exist in WINDOW's node or not.  Returns FN_FOUND if footnotes were found
   and displayed.  Returns FN_UNFOUND if there were no footnotes found
   in WINDOW's node.  Returns FN_UNABLE if there were footnotes, but the
   window to show them couldn't be made. */
int
info_get_or_remove_footnotes (WINDOW *window)
{
  WINDOW *fn_win;
  NODE *new_footnotes = 0;

  fn_win = find_footnotes_window ();

  /* If we are in the footnotes window, change nothing. */
  if (fn_win == window)
    return FN_FOUND;

  /* Don't display footnotes for the "*" node (entire contents of file) or
     for nodes without a name like completion windows. */
  if (window->node->nodename && strcmp ("*", window->node->nodename))
    /* Try to find footnotes for this window's node. */
    new_footnotes = make_footnotes_node (window->node);

  if (!new_footnotes)
    {
      /* If there was a window showing footnotes, and there are no footnotes
         for the current window, delete the old footnote window. */
      if (fn_win && windows->next)
        info_delete_window_internal (fn_win);
      return FN_UNFOUND;
    }

  /* If there is no window around showing footnotes, try
     to make a new window. */
  if (!fn_win)
    {
      WINDOW *old_active;
      WINDOW *last, *win;

      /* Always make this window be the last one appearing in the list.  Find
         the last window in the chain. */
      for (win = windows, last = windows; win; last = win, win = win->next);

      /* Try to split this window, and make the split window the one to
         contain the footnotes. */
      old_active = active_window;
      active_window = last;
      fn_win = window_make_window ();
      active_window = old_active;

      /* If we are hacking automatic footnotes, and there are footnotes
         but we couldn't display them, print a message to that effect. */
      if (!fn_win)
        {
          if (auto_footnotes_p)
            info_error (_("Footnotes could not be displayed"));
          return FN_UNABLE;
        }
    }

  /* Note that info_set_node_of_window calls this function
     (info_get_or_remove_footnotes), but we do not recurse indefinitely
     because we check if we are in the footnote window above. */
  info_set_node_of_window (fn_win, new_footnotes);
  fn_win->flags |= W_TempWindow;

  /* Make the height be the number of lines appearing in the footnotes. */
  if (new_footnotes)
    window_change_window_height (fn_win, fn_win->line_count - fn_win->height);

  return FN_FOUND;
}

/* Show the footnotes associated with this node in another window. */
DECLARE_INFO_COMMAND (info_show_footnotes,
   _("Show the footnotes associated with this node in another window"))
{
  /* A negative argument means just make the window go away. */
  if (count < 0)
    {
      WINDOW *fn_win = find_footnotes_window ();

      /* If there is an old footnotes window, and it isn't the only window
         on the screen, delete it. */
      if (fn_win && windows->next)
        info_delete_window_internal (fn_win);
    }
  else
    {
      int result;

      result = info_get_or_remove_footnotes (window);

      switch (result)
        {
        case FN_UNFOUND:
          info_error ("%s", msg_no_foot_node);
          break;

        case FN_UNABLE:
          info_error ("%s", msg_win_too_small);
          break;
        }
    }
}
