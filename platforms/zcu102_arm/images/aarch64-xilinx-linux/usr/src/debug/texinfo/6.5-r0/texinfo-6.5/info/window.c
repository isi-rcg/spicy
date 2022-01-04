/* window.c -- windows in Info.
   $Id: window.c 7804 2017-05-20 15:51:33Z gavin $

   Copyright 1993, 1997, 1998, 2001, 2002, 2003, 2004, 2007, 2008,
   2011, 2012, 2013, 2014, 2015, 2016, 2017 Free Software Foundation, Inc.

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

   Originally written by Brian Fox.  */

#include "info.h"
#include "session.h"
#include "display.h"
#include "info-utils.h"
#include "doc.h"
#include "tag.h"
#include "variables.h"

/* The window which describes the screen. */
WINDOW *the_screen = NULL;

/* The window which describes the echo area. */
WINDOW *the_echo_area = NULL;

/* The list of windows in Info. */
WINDOW *windows = NULL;

/* Pointer to the active window in WINDOW_LIST. */
WINDOW *active_window = NULL;

/* The size of the echo area in Info.  It never changes, irregardless of the
   size of the screen. */
#define ECHO_AREA_HEIGHT 1

/* Show malformed multibyte sequences */
int show_malformed_multibyte_p = 0;

/* Initalize the window system by creating THE_SCREEN and THE_ECHO_AREA.
   Create the first window ever.
   You pass the dimensions of the total screen size. */
void
window_initialize_windows (int width, int height)
{
  the_screen = xzalloc (sizeof (WINDOW));
  the_echo_area = xzalloc (sizeof (WINDOW));
  windows = xzalloc (sizeof (WINDOW));
  active_window = windows;

  /* The active and echo_area windows are visible.
     The echo_area is permanent.
     The screen is permanent. */
  active_window->flags = W_WindowVisible;
  the_echo_area->flags = W_WindowIsPerm | W_InhibitMode | W_WindowVisible;
  the_screen->flags    = W_WindowIsPerm;

  /* The height of the echo area never changes.  It is statically set right
     here, and it must be at least 1 line for display.  The size of the
     initial window cannot be the same size as the screen, since the screen
     includes the echo area.  So, we make the height of the initial window
     equal to the screen's displayable region minus the height of the echo
     area. */
  the_echo_area->height = ECHO_AREA_HEIGHT;
  active_window->height = the_screen->height - 1 - the_echo_area->height;
  window_new_screen_size (width, height);
}

/* Given that the size of the screen has changed to WIDTH and HEIGHT
   from whatever it was before (found in the_screen->height, ->width),
   change the size (and possibly location) of each window in the screen.
   If a window would become too small, call the function DELETER on it,
   after deleting the window from our chain of windows.  If DELETER is NULL,
   nothing extra is done.  The last window can never be deleted, but it can
   become invisible. */
void
window_new_screen_size (int width, int height)
{
  register WINDOW *win, *first_win;
  int delta_height, delta_each, delta_leftover;
  int numwins;

  /* If no change, do nothing. */
  if (width == the_screen->width && height == the_screen->height)
    return;

  /* The screen has changed height and width. */
  delta_height = height - the_screen->height;
  the_screen->height = height;
  the_screen->width = width;

  /* Set the start of the echo area. */
  the_echo_area->first_row = height - the_echo_area->height;
  the_echo_area->width = width;

  /* Count number of windows. */
  numwins = 0;
  for (win = windows; win; win = win->next)
    numwins++;

  if (numwins == 0)
    return; /* There is nothing to do. */

  /* Divide the change in height among the available windows. */
  delta_each = delta_height / numwins;
  delta_leftover = delta_height - (delta_each * numwins);

  /* See if some windows will need to be deleted.  This is the case if
     the screen is getting smaller, and the available space divided by
     the number of windows is less than WINDOW_MIN_SIZE.  In that case,
     delete some windows and try again until there is either enough
     space to divy up among the windows, or until there is only one
     window left. */
  while (height - 1 <= WINDOW_MIN_SIZE * numwins)
    {
      /* If only one window left, give up. */
      if (!windows->next)
        {
          /* Keep track of the height so that when the screen gets bigger
             again, it can be resized properly.  The -2 is for the window
             information bar and the echo area. */
          windows->height = height - 2;
          windows->width = width;
          free (windows->modeline);
          windows->modeline = xmalloc (1 + width);
          return;
        }

      /* If we have some temporary windows, delete one of them. */
      for (win = windows; win; win = win->next)
        if (win->flags & W_TempWindow)
          break;

      /* Otherwise, delete the first window, and try again. */
      if (!win)
        win = windows;

      forget_window_and_nodes (win);
      window_delete_window (win);
      numwins--;
    }

  /* Alternate which window we start resizing at, to resize all
     windows evenly. */
    {
      int first_win_num = the_screen->height % numwins;
      int i;
      first_win = windows;
      for (i = 0; i < first_win_num; i++)
        first_win = first_win->next;
    }

  /* Change the height of each window in the chain by delta_each.  Change
     the height of the last window in the chain by delta_each and by the
     leftover amount of change.  Change the width of each window to be
     WIDTH. */
  win = first_win;
  do
    {
      if ((win->width != width) && ((win->flags & W_InhibitMode) == 0))
        {
          win->width = width;
          free (win->modeline);
          win->modeline = xmalloc (1 + width);
        }

      /* Don't resize a window to be smaller than one line. */
      if (win->height + delta_each >= 1)
        win->height += delta_each;
      else
        delta_leftover += delta_each;

      /* Try to use up the extra space. */
      if (delta_leftover != 0 && win->height + delta_leftover >= 1)
        {
          win->height += delta_leftover;
          delta_leftover = 0;
        }
      /* Go to next window, wrapping round to the start. */
      win = win->next;
      if (!win)
        win = windows;
    }
  while (win != first_win);

  for (win = windows; win; win = win->next)
    {
      /* If this is not the first window in the chain, set the
         first row of it by adding one to the location of the
         previous window's modeline. */
      if (win->prev)
        win->first_row = (win->prev->first_row + win->prev->height) + 1;

      if (win->node)
        {
          free (win->line_starts);
          free (win->log_line_no);
          calculate_line_starts (win);
        }

      win->flags |= W_UpdateWindow;
    }

  /* If the screen got smaller, check over the windows just shrunk to
     keep them within bounds.  Some of the windows may have gotten smaller
     than WINDOW_MIN_HEIGHT in which case some of the other windows are
     larger than the available display space in the screen.  Because of our
     intial test above, we know that there is enough space for all of the
     windows. */
  if ((delta_each < 0) && ((windows->height != 0) && windows->next))
    {
      int avail;

      avail = the_screen->height - (numwins + the_echo_area->height);
      win = windows;

      while (win)
        {
          if ((win->height < WINDOW_MIN_HEIGHT) ||
              (win->height > avail))
            {
              WINDOW *lastwin = NULL;

              /* Split the space among the available windows. */
              delta_each = avail / numwins;
              delta_leftover = avail - (delta_each * numwins);

              for (win = windows; win; win = win->next)
                {
                  lastwin = win;
                  if (win->prev)
                    win->first_row =
                      (win->prev->first_row + win->prev->height) + 1;
                  win->height = delta_each;
                }

              /* Give the leftover space (if any) to the last window. */
              lastwin->height += delta_leftover;
              break;
            }
          else
            win = win->next;
        }
    }

  /* Make sure point is in displayed part of active window. */
  window_adjust_pagetop (active_window);

  /* One more loop.  If any heights or widths have become negative,
     set them to zero.  This can apparently happen with resizing down to
     very small sizes.  Sadly, it is not apparent to me where in the
     above calculations it goes wrong.  */
  for (win = windows; win; win = win->next)
    {
      if (win->height < 0)
        win->height = 0;

      if (win->width < 0)
        win->width = 0;
    }
}

/* Make a new window by splitting an existing one. If the window could
   not be made return a null pointer.  The active window is not changed .*/
WINDOW *
window_make_window (void)
{
  WINDOW *window;

  /* If there isn't enough room to make another window, return now. */
  if ((active_window->height / 2) < WINDOW_MIN_SIZE)
    return NULL;

  /* Make and initialize the new window.
     The fudging about with -1 and +1 is because the following window in the
     chain cannot start at window->height, since that is where the modeline
     for the previous window is displayed.  The inverse adjustment is made
     in window_delete_window (). */
  window = xzalloc (sizeof (WINDOW));
  window->width = the_screen->width;
  window->height = (active_window->height / 2) - 1;
  window->first_row = active_window->first_row +
    (active_window->height - window->height);
  window->goal_column = -1;
  memset (&window->line_map, 0, sizeof (window->line_map));
  window->modeline = xmalloc (1 + window->width);
  window->line_starts = NULL;
  window->flags = W_UpdateWindow | W_WindowVisible;

  /* Adjust the height of the old active window. */
  active_window->height -= (window->height + 1);
  active_window->flags |= W_UpdateWindow;

  window_make_modeline (active_window);

  /* This window is just after the active one.  Which window is active is
     not changed. */
  window->prev = active_window;
  window->next = active_window->next;
  active_window->next = window;
  if (window->next)
    window->next->prev = window;
  return window;
}

/* These useful macros make it possible to read the code in
   window_change_window_height (). */
#define grow_me_shrinking_next(me, next, diff) \
  do { \
    me->height += diff; \
    next->height -= diff; \
    next->first_row += diff; \
  } while (0)

#define grow_me_shrinking_prev(me, prev, diff) \
  do { \
    me->height += diff; \
    prev->height -= diff; \
    me->first_row -=diff; \
  } while (0)

#define shrink_me_growing_next(me, next, diff) \
  do { \
    me->height -= diff; \
    next->height += diff; \
    next->first_row -= diff; \
  } while (0)

#define shrink_me_growing_prev(me, prev, diff) \
  do { \
    me->height -= diff; \
    prev->height += diff; \
    me->first_row += diff; \
  } while (0)

/* Change the height of WINDOW by AMOUNT.  This also automagically adjusts
   the previous and next windows in the chain.  If there is only one user
   window, then no change takes place. */
void
window_change_window_height (WINDOW *window, int amount)
{
  register WINDOW *win, *prev, *next;

  /* If there is only one window, or if the amount of change is zero,
     return immediately. */
  if (!windows->next || amount == 0)
    return;

  /* Find this window in our chain. */
  for (win = windows; win; win = win->next)
    if (win == window)
      break;

  /* If the window is isolated (i.e., doesn't appear in our window list,
     then quit now. */
  if (!win)
    return;

  /* Change the height of this window by AMOUNT, if that is possible.
     It can be impossible if there isn't enough available room on the
     screen, or if the resultant window would be too small. */

    prev = window->prev;
    next = window->next;

  /* WINDOW decreasing in size? */
  if (amount < 0)
    {
      int abs_amount = -amount; /* It is easier to deal with this way. */

      /* If the resultant window would be too small, stop here. */
      if ((window->height - abs_amount) < WINDOW_MIN_HEIGHT)
        return;

      /* If we have two neighboring windows, choose the smaller one to get
         larger. */
      if (next && prev)
        {
          if (prev->height < next->height)
            shrink_me_growing_prev (window, prev, abs_amount);
          else
            shrink_me_growing_next (window, next, abs_amount);
        }
      else if (next)
        shrink_me_growing_next (window, next, abs_amount);
      else
        shrink_me_growing_prev (window, prev, abs_amount);
    }

  /* WINDOW increasing in size? */
  if (amount > 0)
    {
      int total_avail, next_avail = 0, prev_avail = 0;

      if (next)
        next_avail = next->height - WINDOW_MIN_SIZE;

      if (prev)
        prev_avail = prev->height - WINDOW_MIN_SIZE;

      total_avail = next_avail + prev_avail;

      /* If there isn't enough space available to grow this window, give up. */
      if (amount > total_avail)
        return;

      /* If there aren't two neighboring windows, or if one of the neighbors
         is larger than the other one by at least AMOUNT, grow that one. */
      if (next_avail - amount >= prev_avail)
        grow_me_shrinking_next (window, next, amount);
      else if (prev_avail - amount >= next_avail)
        grow_me_shrinking_prev (window, prev, amount);
      else
        {
          int change;

          /* This window has two neighbors.  They both must be shrunk in to
             make enough space for WINDOW to grow.  Make them both the same
             size. */
          if (prev_avail > next_avail)
            {
              change = prev_avail - next_avail;
              grow_me_shrinking_prev (window, prev, change);
              amount -= change;
            }
          else
            {
              change = next_avail - prev_avail;
              grow_me_shrinking_next (window, next, change);
              amount -= change;
            }

          /* Both neighbors are the same size.  Split the difference in
             AMOUNT between them. */
          while (amount)
            {
              window->height++;
              amount--;

              /* Odd numbers grow next, even grow prev. */
              if (amount & 1)
                {
                  prev->height--;
                  window->first_row--;
                }
              else
                {
                  next->height--;
                  next->first_row++;
                }
            }
        }
    }
  if (prev)
    prev->flags |= W_UpdateWindow;

  if (next)
    next->flags |= W_UpdateWindow;

  window->flags |= W_UpdateWindow;
}

/* Tile all of the windows currently displayed in the global variable
   WINDOWS.  If argument STYLE is TILE_INTERNALS, tile windows displaying
   internal nodes as well, otherwise do not change the height of such
   windows. */
void
window_tile_windows (int style)
{
  WINDOW *win, *last_adjusted;
  int numwins, avail, per_win_height, leftover;
  int do_internals;

  numwins = avail = 0;
  do_internals = (style == TILE_INTERNALS);

  for (win = windows; win; win = win->next)
    if (do_internals || !win->node ||
        (win->node->flags & N_IsInternal) == 0)
      {
        avail += win->height;
        numwins++;
      }

  if (numwins <= 1 || !the_screen->height)
    return;

  /* Find the size for each window.  Divide the size of the usable portion
     of the screen by the number of windows. */
  per_win_height = avail / numwins;
  leftover = avail - (per_win_height * numwins);

  last_adjusted = NULL;
  for (win = windows; win; win = win->next)
    {
      if (do_internals || !win->node ||
          (win->node->flags & N_IsInternal) == 0)
        {
          last_adjusted = win;
          win->height = per_win_height;
        }
    }

  if (last_adjusted)
    last_adjusted->height += leftover;

  /* Readjust the first_row of every window in the chain. */
  for (win = windows; win; win = win->next)
    {
      if (win->prev)
        win->first_row = win->prev->first_row + win->prev->height + 1;

      window_adjust_pagetop (win);
      win->flags |= W_UpdateWindow;
    }
}

/* Toggle the state of line wrapping in WINDOW.  This can do a bit of fancy
   redisplay. */
void
window_toggle_wrap (WINDOW *window)
{
  if (window->flags & W_NoWrap)
    window->flags &= ~W_NoWrap;
  else
    window->flags |= W_NoWrap;

  if (window != the_echo_area)
    {
      long *old_starts;
      long *old_xlat;
      int old_lines, old_pagetop;

      old_starts = window->line_starts;
      old_xlat = window->log_line_no;
      old_lines = window->line_count;
      old_pagetop = window->pagetop;

      calculate_line_starts (window);

      /* Make sure that point appears within this window. */
      window_adjust_pagetop (window);

      /* If the pagetop hasn't changed maybe we can do some scrolling now
         to speed up the display.  Many of the line starts will be the same,
         so scrolling here is a very good optimization.*/
      if (old_pagetop == window->pagetop)
        display_scroll_line_starts (window, old_pagetop,
                                    old_starts, old_lines);
      free (old_starts);
      free (old_xlat);
    }
  window->flags |= W_UpdateWindow;
}

/* Set WINDOW to display NODE. */
void
window_set_node_of_window (WINDOW *window, NODE *node)
{
  window->node = node;
  window->pagetop = 0;
  window->point = 0;

  free (window->line_starts);
  free (window->log_line_no);
  calculate_line_starts (window);
  window_compute_line_map (window);

  /* Clear displayed search matches if any. */
  free_matches (&window->matches);

  window->flags |= W_UpdateWindow;
  if (node)
    {
      /* The display_pos member is nonzero if we're displaying an anchor.  */
      window->point = node ? node->display_pos : 0;
      window_adjust_pagetop (window);
    }
  window_make_modeline (window);
}

/* Delete WINDOW from the list of known windows.  If this window was the
   active window, make the next window in the chain be the active window.
   If the active window is the next or previous window, choose that window
   as the recipient of the extra space.  Otherwise, prefer the next window.
   Be aware that info_delete_window_internal (in session.c) should be called
   instead if you need to remove the window from the info_windows list. */
void
window_delete_window (WINDOW *window)
{
  WINDOW *next, *prev, *window_to_fix;

  next = window->next;
  prev = window->prev;

  /* You cannot delete the only window or a permanent window. */
  if ((!next && !prev) || (window->flags & W_WindowIsPerm))
    return;

  if (next)
    next->prev = prev;

  if (!prev)
    windows = next;
  else
    prev->next = next;

  free (window->line_starts);
  free (window->log_line_no);
  free (window->line_map.map);
  free (window->modeline);
  free_matches (&window->matches);
  free (window->search_string);

  if (window == active_window)
    {
      WINDOW *new_active = 0;

      /* If there isn't a next window, then there must be a previous one,
         since we cannot delete the last window.  If there is a next window,
         prefer to use that as the active window.  Try to find an important
         window to select, e.g. not a footnotes window. */
      if (next)
        {
          new_active = next;
          while ((new_active->flags & W_TempWindow) && new_active->next)
            new_active = new_active->next;
        }

      if ((!new_active || new_active->flags & W_TempWindow) && prev)
        {
          new_active = prev;
          while ((new_active->flags & W_TempWindow) && new_active->prev)
            new_active = new_active->prev;
        }
      active_window = new_active;
    }

  if (next && active_window == next)
    window_to_fix = next;
  else if (prev && active_window == prev)
    window_to_fix = prev;
  else if (next)
    window_to_fix = next;
  else if (prev)
    window_to_fix = prev;
  else
    window_to_fix = windows;
    
  if (window_to_fix->first_row > window->first_row)
    {
      int diff;

      /* Try to adjust the visible part of the node so that as little
         text as possible has to move. */
      diff = window_to_fix->first_row - window->first_row;
      window_to_fix->first_row = window->first_row;

      window_to_fix->pagetop -= diff;
      if (window_to_fix->pagetop < 0)
        window_to_fix->pagetop = 0;
    }

  /* The `+ 1' is to offset the difference between the first_row locations.
     See the code in window_make_window (). */
  window_to_fix->height += window->height + 1;
  window_to_fix->flags |= W_UpdateWindow;

  free (window);
}

/* For every window in CHAIN, set the flags member to have FLAG set. */
void
window_mark_chain (WINDOW *chain, int flag)
{
  register WINDOW *win;

  for (win = chain; win; win = win->next)
    win->flags |= flag;
}

/* For every window in CHAIN, clear the flags member of FLAG. */
void
window_unmark_chain (WINDOW *chain, int flag)
{
  register WINDOW *win;

  for (win = chain; win; win = win->next)
    win->flags &= ~flag;
}

/* Return the number of first physical line corresponding to the logical
   line LN.

   A logical line can occupy one or more physical lines of output.  It
   occupies more than one physical line if its width is greater than the
   window width and the flag W_NoWrap is not set for that window.
 */
long
window_log_to_phys_line (WINDOW *window, long ln)
{
  size_t i;
  
  if (ln > window->line_count)
    return 0;
  for (i = ln; i < window->line_count && window->log_line_no[i] < ln; i++)
    ;
  return i;
}

/* Change the pagetop of WINDOW to DESIRED_TOP, perhaps scrolling the screen
   to do so.  WINDOW->pagetop should be the currently displayed pagetop. */
void
set_window_pagetop (WINDOW *window, int desired_top)
{
  int point_line, old_pagetop;

  if (desired_top < 0)
    desired_top = 0;
  else if (desired_top > window->line_count)
    desired_top = window->line_count - 1;

  if (window->pagetop == desired_top)
    return;

  old_pagetop = window->pagetop;
  window->pagetop = desired_top;

  /* Make sure that point appears in this window. */
  point_line = window_line_of_point (window);
  if (point_line < window->pagetop)
    {
      window->point = window->line_starts[window->pagetop];
      window->goal_column = 0;
    }
  else if (point_line >= window->pagetop + window->height)
    {
      long bottom = window->pagetop + window->height - 1;
      window->point = window->line_starts[bottom];
      window->goal_column = 0;
    }

  window->flags |= W_UpdateWindow;

  /* Find out which direction to scroll, and scroll the window in that
     direction.  Do this only if there would be a savings in redisplay
     time.  This is true if the amount to scroll is less than the height
     of the window, and if the number of lines scrolled would be greater
     than 10 % of the window's height.

     To prevent status line blinking when keeping up or down key,
     scrolling is disabled if the amount to scroll is 1. */
  if (old_pagetop < desired_top)
    {
      int start, end, amount;

      amount = desired_top - old_pagetop;

      if (amount == 1 ||
          (amount >= window->height) ||
          (((window->height - amount) * 10) < window->height))
        return;

      start = window->first_row;
      end = window->height + window->first_row;

      display_scroll_display (start, end, -amount);
    }
  else
    {
      int start, end, amount;

      amount = old_pagetop - desired_top;

      if (amount == 1 ||
          (amount >= window->height) ||
          (((window->height - amount) * 10) < window->height))
        return;

      start = window->first_row;
      end = window->first_row + window->height;
      display_scroll_display (start, end, amount);
    }
}

/* Adjust the pagetop of WINDOW such that the cursor point will be visible. */
void
window_adjust_pagetop (WINDOW *window)
{
  register int line;

  if (!window->node)
    return;

  line = window_line_of_point (window);

  /* If this line appears in the current displayable page, do nothing.
     Otherwise, adjust the top of the page to make this line visible. */
  if (line < window->pagetop
      || line - window->pagetop > window->height - 1)
    {
      int new_pagetop = line - ((window->height - 1) / 2);

      if (new_pagetop < 0)
        new_pagetop = 0;
      set_window_pagetop (window, new_pagetop);
    }
}

/* Return the index of the line containing point. */
int
window_line_of_point (WINDOW *window)
{
  register int i, start = 0;

  if (!window->line_starts)
    calculate_line_starts (window);

  /* Check if point is past the pagetop for this window, and if so, start 
     searching forward from there. */
  if (window->pagetop > -1 && window->pagetop < window->line_count
      && window->line_starts[window->pagetop] <= window->point)
    start = window->pagetop;

  for (i = start; i < window->line_count; i++)
    {
      if (window->line_starts[i] > window->point)
        break;
    }

  if (i > 0)
    return i - 1;
  else
    return 0; /* Shouldn't happen */
}

/* Get and return the printed column offset of the cursor in this window. */
int
window_get_cursor_column (WINDOW *window)
{
  return window_point_to_column (window, window->point, &window->point);
}

/* Create a modeline for WINDOW, and store it in window->modeline. */
void
window_make_modeline (WINDOW *window)
{
  register int i;
  char *modeline;
  char location_indicator[4];
  int lines_remaining;

  /* Only make modelines for those windows which have one. */
  if (window->flags & W_InhibitMode)
    return;

  /* Find the number of lines actually displayed in this window. */
  lines_remaining = window->line_count - window->pagetop;

  if (window->pagetop == 0)
    {
      if (lines_remaining <= window->height)
        strcpy (location_indicator, "All");
      else
        strcpy (location_indicator, "Top");
    }
  else
    {
      if (lines_remaining <= window->height)
        strcpy (location_indicator, "Bot");
      else
        {
          float pt, lc;
          int percentage;

          pt = (float)window->pagetop;
          lc = (float)(window->line_count - window->height);

          percentage = 100 * (pt / lc);

          sprintf (location_indicator, "%2d%%", percentage);
        }
    }

  /* Calculate the maximum size of the information to stick in MODELINE. */
  {
    int modeline_len = 0;
    char *nodename = "*no node*";
    NODE *node = window->node;
    char *name;
    int dot;

    if (node && node->nodename)
      nodename = node->nodename;

    name = filename_non_directory (node->fullpath);

    /* 10 for the decimal representation of the number of lines in this
       node, and the remainder of the text that can appear in the line. */
    modeline_len += 10 + strlen (_("-----Info: (), lines ----, "));
    modeline_len += 3; /* strlen (location_indicator) */
    modeline_len += strlen (name);
    if (nodename)
      modeline_len += strlen (nodename);
    if (modeline_len < window->width)
      modeline_len = window->width;

    modeline = xcalloc (1, 1 + modeline_len);

    sprintf (modeline + strlen (modeline), "-----Info: ");

    /* Omit any extension like ".info.gz" from file name. */
    dot = strcspn (name, ".");

    if (name && strcmp ("", name))
      {
        sprintf (modeline + strlen (modeline), "(");
        strncpy (modeline + strlen (modeline), name, dot);
        sprintf (modeline + strlen (modeline), ")");
      }
    sprintf (modeline + strlen (modeline),
             "%s, %ld lines --%s",
             nodename, window->line_count, location_indicator);

    i = strlen (modeline);

    if (i >= window->width)
      modeline[window->width] = '\0';
    else
      {
        while (i < window->width)
          modeline[i++] = '-';
        modeline[i] = '\0';
      }

    strcpy (window->modeline, modeline);
    free (modeline);
  }
}

/* Make WINDOW start displaying at PERCENT percentage of its node. */
void
window_goto_percentage (WINDOW *window, int percent)
{
  int desired_line;

  if (!percent)
    desired_line = 0;
  else
    desired_line =
      (int) ((float)window->line_count * ((float)percent / 100.0));

  window->pagetop = desired_line;
  window->point =
    window->line_starts[window->pagetop];
  window->flags |= W_UpdateWindow;
  window_make_modeline (window);
}


/* A place to buffer echo area messages. */
static NODE *echo_area_node = NULL;

/* Make the node of the_echo_area be an empty one. */
void
free_echo_area (void)
{
  if (echo_area_node)
    {
      free (echo_area_node->contents);
      free (echo_area_node);
    }

  echo_area_node = NULL;
  window_set_node_of_window (the_echo_area, echo_area_node);
}
  
/* Clear the echo area, removing any message that is already present.
   The echo area is cleared immediately. */
void
window_clear_echo_area (void)
{
  free_echo_area ();
  display_update_one_window (the_echo_area);
}

void
vwindow_message_in_echo_area (const char *format, va_list ap)
{
  free_echo_area ();
  echo_area_node = build_message_node (format, ap);
  window_set_node_of_window (the_echo_area, echo_area_node);
  display_update_one_window (the_echo_area);
}

/* Make a message appear in the echo area, built from FORMAT, ARG1 and ARG2.
   The arguments are treated similar to printf () arguments, but not all of
   printf () hair is present.  The message appears immediately.  If there was
   already a message appearing in the echo area, it is removed. */
void
window_message_in_echo_area (const char *format, ...)
{
  va_list ap;
  
  va_start (ap, format);
  vwindow_message_in_echo_area (format, ap);
  va_end (ap);
}

/* Place a temporary message in the echo area built from FORMAT, ARG1
   and ARG2.  The message appears immediately, but does not destroy
   any existing message.  A future call to unmessage_in_echo_area ()
   restores the old contents. */
static NODE **old_echo_area_nodes = NULL;
static size_t old_echo_area_nodes_index = 0;
static size_t old_echo_area_nodes_slots = 0;

void
message_in_echo_area (const char *format, ...)
{
  va_list ap;
  
  if (echo_area_node)
    {
      add_pointer_to_array (echo_area_node, old_echo_area_nodes_index,
                            old_echo_area_nodes, old_echo_area_nodes_slots,
                            4);
    }
  echo_area_node = NULL;
  va_start (ap, format);
  vwindow_message_in_echo_area (format, ap);
  va_end (ap);
}

void
unmessage_in_echo_area (void)
{
  free_echo_area ();

  if (old_echo_area_nodes_index)
    echo_area_node = old_echo_area_nodes[--old_echo_area_nodes_index];

  window_set_node_of_window (the_echo_area, echo_area_node);
  display_update_one_window (the_echo_area);
}


/* Build a new node which has FORMAT printed with ARG1 and ARG2 as the
   contents. */
NODE *
build_message_node (const char *format, va_list ap)
{
  struct text_buffer msg;

  text_buffer_init (&msg);
  text_buffer_vprintf (&msg, format, ap);

  return text_buffer_to_node (&msg);
}

NODE *
format_message_node (const char *format, ...)
{
  NODE *node;
  va_list ap;
  
  va_start (ap, format);
  node = build_message_node (format, ap);
  va_end (ap);
  return node;
}

NODE *
text_buffer_to_node (struct text_buffer *tb)
{
  NODE *node;

  node = info_create_node ();

  /* Make sure that this buffer ends with a newline. */
  text_buffer_add_char (tb, '\n');
  node->nodelen = text_buffer_off (tb);
  text_buffer_add_char (tb, '\0');

  node->contents = text_buffer_base (tb);
  node->flags |= N_IsInternal;
  return node;
}

/* Used by calculate_line_starts to record line starts in the
   win->LINE_COUNT and win->LOG_LINE_NO arrays. */
static void
collect_line_starts (WINDOW *win, long ll_num, long pl_start)
{
  add_element_to_array (pl_start, win->line_count,
                        win->line_starts, win->line_slots, 2);

  /* We cannot do add_element_to_array for this, as this would lead
     to incrementing cp->win->line_count twice. */
  win->log_line_no = xrealloc (win->log_line_no,
                               win->line_slots * sizeof (long));
  win->log_line_no[win->line_count - 1] = ll_num;
}

#define NO_NODELINE 0
#define PRINT_NODELINE 1
#define NODELINE_POINTERS_ONLY 2
int nodeline_print = 2;

/* Calculate a list of line starts for the node belonging to WINDOW.  The
   line starts are offsets within WINDOW->node->contents.

   Note that this function must agree with what display_update_one_window
   in display.c does. */
void
calculate_line_starts (WINDOW *win)
{
  long pl_chars = 0;     /* Number of characters in line so far. */
  long pl_start;         /* Offset of start of current physical line. */
  long ll_num = 0;       /* Number of logical lines */
  mbi_iterator_t iter;

  /* Width of character carried over from one physical line to the next.  */
  size_t carried_over_chars = 0;

  win->line_starts = NULL;
  win->log_line_no = NULL;
  win->line_count = 0;
  win->line_slots = 0;

  if (!win->node)
    return;

  pl_start = 0;
  if (nodeline_print != PRINT_NODELINE
      && !memcmp (win->node->contents, "File:", strlen ("File:")))
    {
      char *s = strchr (win->node->contents, '\n');
      if (s && nodeline_print == NO_NODELINE)
        {
          pl_start = s - win->node->contents + 1;
        }
      else if (s && nodeline_print == NODELINE_POINTERS_ONLY)
        {
          char *s2;
          char saved = *s;
          *s = '\0';
          s2 = strstr (win->node->contents, "Next: ");
          if (!s2)
            s2 = strstr (win->node->contents, "Prev: ");
          if (!s2)
            s2 = strstr (win->node->contents, "Up: ");
          if (s2)
            pl_start = s2 - win->node->contents;
          *s = saved;
        }
    }

  for (mbi_init (iter,
                 win->node->contents + pl_start,
                 win->node->nodelen - pl_start);
       mbi_avail (iter);
       mbi_advance (iter))
    {
      size_t pchars = 0; /* Screen columns for this character. */
      size_t pbytes = 0; /* Not used. */
      int delim = 0;

      /* Set pchars. */
      (void) printed_representation (&iter, &delim, pl_chars,
                                     &pchars, &pbytes);

      /* If this character can be printed without passing the width of
         the line, then include it in the line. */
      if (!delim && pl_chars + pchars < win->width)
        {
          pl_chars += pchars;
          continue;
        }

      /* If this character cannot be printed in this line, we have
         found the end of this line as it would appear on the screen. */

      carried_over_chars = delim ? 0 : pchars;

      collect_line_starts (win, ll_num, pl_start);

      if (delim == '\r' || delim == '\n')
        ++ll_num;

      /* Start a new physical line at next character, unless a character
         was carried over, in which case start there. */
      pl_start = mbi_cur_ptr (iter) - win->node->contents;
      if (carried_over_chars == 0)
        pl_start += mb_len (mbi_cur (iter));
      pl_chars = 0;

      /* If there is a character carried over, count it now.  Expected to be 
         "short", i.e. a representation like "^A". */
      if (carried_over_chars != 0)
        {
          pl_chars = carried_over_chars;
    
          /* If this window has chosen not to wrap lines, skip to the end
             of the logical line in the buffer, and start a new line here. */
          if (win->flags & W_NoWrap)
            {
              for (; mbi_avail (iter); mbi_advance (iter))
                if (mb_len (mbi_cur (iter)) == 1
                    && *mbi_cur_ptr (iter) == '\n')
                  break;

              pl_chars = 0;
              pl_start = mbi_cur_ptr (iter) + mb_len (mbi_cur (iter))
                         - win->node->contents;
            }
        }
    }

  if (pl_chars)
    collect_line_starts (win, ll_num++, pl_start);

  /* Have one line start at the end of the node. */
  collect_line_starts (win, ll_num, mbi_cur_ptr (iter) - win->node->contents);
  win->line_count--;

  /* Finally, initialize the line map for the current line. */
  window_line_map_init (win);
}


static void
line_map_init (LINE_MAP *map, NODE *node, int line)
{
  map->node = node;
  map->nline = line;
  map->used = 0;
}

static void
line_map_add (LINE_MAP *map, long pos)
{
  if (map->used == map->size)
    {
      if (map->size == 0)				       
	map->size = 80; /* Initial allocation */	       
      map->map = x2nrealloc (map->map,
			     &map->size,
			     sizeof (map->map[0]));
    }

  map->map[map->used++] = pos;
}

/* Initialize (clear) WIN's line map. */
void
window_line_map_init (WINDOW *win)
{
  win->line_map.used = 0;
}

/* Compute the line map for the current line in WIN. */
void
window_compute_line_map (WINDOW *win)
{
  int line = window_line_of_point (win);
  mbi_iterator_t iter;
  int delim = 0;
  char *endp;
  const char *cur_ptr;

  if (win->line_map.node == win->node && win->line_map.nline == line
      && win->line_map.used)
    return;
  line_map_init (&win->line_map, win->node, line);
  if (!win->node)
    return;

  if (line + 1 < win->line_count)
    endp = win->node->contents + win->line_starts[line + 1];
  else
    endp = win->node->contents + win->node->nodelen;
  
  for (mbi_init (iter,
		 win->node->contents + win->line_starts[line], 
		 win->node->nodelen - win->line_starts[line]);
       !delim && mbi_avail (iter);
       mbi_advance (iter))
    {
      size_t pchars, pbytes;
      cur_ptr = mbi_cur_ptr (iter);

      if (cur_ptr >= endp)
	break;
      
      /* Set pchars */
      (void) printed_representation (&iter, &delim, win->line_map.used,
                                     &pchars, &pbytes);

      while (pchars--)
        line_map_add (&win->line_map, cur_ptr - win->node->contents);
    }
}

/* Translate the value of POINT into a column number.  If NP is given
   store there the value of point corresponding to the beginning of a
   multibyte character in this column.  If the character at POINT spans 
   multiple columns (e.g. a tab), return the leftmost column it occupies. */
int
window_point_to_column (WINDOW *win, long point, long *np)
{
  int i;
  
  window_compute_line_map (win);
  if (!win->line_map.map || point < win->line_map.map[0])
    return 0;
  for (i = 0; i < win->line_map.used; i++)
    if (win->line_map.map[i] >= point)
      break;
  if (np)
    *np = win->line_map.map[i];
  return i;
}
      
