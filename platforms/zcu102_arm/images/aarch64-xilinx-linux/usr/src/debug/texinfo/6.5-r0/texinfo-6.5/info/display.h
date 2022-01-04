/* display.h -- How the display in Info is done.
   $Id: display.h 6586 2015-08-29 17:01:54Z gavin $

   Copyright 1993, 1997, 2004, 2007, 2008, 2011, 2012, 2013, 2014, 2015
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

#ifndef INFO_DISPLAY_H
#define INFO_DISPLAY_H

#include "info-utils.h"
#include "terminal.h"

typedef struct {
  char *text;		/* Text of the line as it appears. */
  int textlen;		/* Printable Length of TEXT. */
  int inverse;		/* Screen line is either in inverse video, or
                           'text' does not represent what is on the screen. */
} DISPLAY_LINE;

/* An array of display lines which tell us what is currently visible on
   the display.  */
extern DISPLAY_LINE **the_display;

/* Non-zero means do no output. */
extern int display_inhibited;

/* Non-zero if we didn't completely redisplay a window. */
extern int display_was_interrupted_p;

/* Initialize THE_DISPLAY to WIDTH and HEIGHT, with nothing in it. */
void display_initialize_display (int width, int height);

/* Clear all of the lines in DISPLAY making the screen blank. */
void display_clear_display (DISPLAY_LINE **display);

/* Update the windows on the display. */
void display_update_display (void);

/* Display WIN on THE_DISPLAY.  Unlike display_update_display (), this
   function only does one window. */
void display_update_one_window (WINDOW *win);

/* Move the screen cursor to directly over the current character in WINDOW. */
void display_cursor_at_point (WINDOW *window);

/* Scroll the region of the_display starting at START, ending at END, and
   moving the lines AMOUNT lines.  If AMOUNT is less than zero, the lines
   are moved up in the screen, otherwise down.  Actually, it is possible
   for no scrolling to take place in the case that the terminal doesn't
   support it.  This doesn't matter to us. */
void display_scroll_display (int start, int end, int amount);

/* Try to scroll lines in WINDOW.  OLD_PAGETOP is the pagetop of WINDOW before
   having had its line starts recalculated.  OLD_STARTS is the list of line
   starts that used to appear in this window.  OLD_COUNT is the number of lines
   that appear in the OLD_STARTS array. */
void display_scroll_line_starts (WINDOW *window, int old_pagetop,
    long *old_starts, int old_count);

#endif /* not INFO_DISPLAY_H */
