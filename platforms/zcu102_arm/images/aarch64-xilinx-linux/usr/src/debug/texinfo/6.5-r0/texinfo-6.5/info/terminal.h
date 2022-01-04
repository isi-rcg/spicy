/* terminal.h -- The external interface to terminal I/O.
   $Id: terminal.h 6914 2016-01-02 17:36:03Z gavin $

   Copyright 1993, 1996, 1997, 2001, 2002, 2004, 2007, 2013, 2014, 2015
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

   Originally uWritten by Brian Fox. */

#if !defined (TERMINAL_H)
#define TERMINAL_H

#include "info.h"

/* For almost every function externally visible from terminal.c, there is
   a corresponding "hook" function which can be bound in order to replace
   the functionality of the one found in terminal.c.  This is how we go
   about implemented X window display. */

/* The width and height of the terminal. */
extern int screenwidth, screenheight;

/* Non-zero means this terminal can't really do anything. */
extern int terminal_is_dumb_p;

/* Non-zero means that this terminal can produce a visible bell. */
extern int terminal_has_visible_bell_p;

/* Non-zero means to use that visible bell if at all possible. */
extern int terminal_use_visible_bell_p;

/* Non-zero means that this terminal can scroll lines up and down. */
extern int terminal_can_scroll;

/* Non-zero means that this terminal can scroll within a restricted region. */
extern int terminal_can_scroll_region;

/* Initialize the terminal which is known as TERMINAL_NAME.  If this terminal
   doesn't have cursor addressability, TERMINAL_IS_DUMB_P becomes non-zero.
   The variables SCREENHEIGHT and SCREENWIDTH are set to the dimensions that
   this terminal actually has. */
extern void terminal_initialize_terminal (char *terminal_name);
extern VFunction *terminal_initialize_terminal_hook;

/* Return the current screen width and height in the variables
   SCREENWIDTH and SCREENHEIGHT. */
extern void terminal_get_screen_size (void);
extern VFunction *terminal_get_screen_size_hook;

/* Save and restore tty settings. */
extern int terminal_prep_terminal (void);
extern void terminal_unprep_terminal (void);

extern VFunction *terminal_prep_terminal_hook;
extern VFunction *terminal_unprep_terminal_hook;

/* Re-initialize the terminal to TERMINAL_NAME. */
extern void terminal_new_terminal (char *terminal_name);
extern VFunction *terminal_new_terminal_hook;

/* Move the cursor to the terminal location of X and Y. */
extern void terminal_goto_xy (int x, int y);
extern VFunction *terminal_goto_xy_hook;

/* Print STRING to the terminal at the current position. */
extern void terminal_put_text (char *string);
extern VFunction *terminal_put_text_hook;

/* Print NCHARS from STRING to the terminal at the current position. */
extern void terminal_write_chars (char *string, int nchars);
extern VFunction *terminal_write_chars_hook;

/* Clear from the current position of the cursor to the end of the line. */
extern void terminal_clear_to_eol (void);
extern VFunction *terminal_clear_to_eol_hook;

/* Clear the entire terminal screen. */
extern void terminal_clear_screen (void);
extern VFunction *terminal_clear_screen_hook;

/* Move the cursor up one line. */
extern void terminal_up_line (void);
extern VFunction *terminal_up_line_hook;

/* Move the cursor down one line. */
extern void terminal_down_line (void);
extern VFunction *terminal_down_line_hook;

/* Turn on reverse video if possible. */
extern void terminal_begin_inverse (void);
extern VFunction *terminal_begin_inverse_hook;

/* Turn off reverse video if possible. */
extern void terminal_end_inverse (void);
extern VFunction *terminal_end_inverse_hook;

/* Turn on standout mode if possible. */
extern void terminal_begin_standout (void);
extern VFunction *terminal_begin_standout_hook;

/* Turn off standout mode if possible. */
extern void terminal_end_standout (void);
extern VFunction *terminal_end_standout_hook;

/* Turn on and off underline mode if possible. */
void terminal_begin_underline (void);
extern VFunction *terminal_begin_underline_hook;
void terminal_end_underline (void);
extern VFunction *terminal_end_underline_hook;

/* Scroll an area of the terminal, starting with the region from START
   to END, AMOUNT lines.  If AMOUNT is negative, the lines are scrolled
   towards the top of the screen, else they are scrolled towards the
   bottom of the screen. */
extern void terminal_scroll_terminal (int start, int end, int amount);
extern VFunction *terminal_scroll_terminal_hook;

extern void terminal_scroll_region (int start, int end, int amount);

/* Ring the terminal bell.  The bell is run visibly if it both has one and
   terminal_use_visible_bell_p is non-zero. */
extern void terminal_ring_bell (void);
extern VFunction *terminal_ring_bell_hook;

/* The key sequences output by special keys, if this terminal has any. */
extern char *term_ku, *term_kd, *term_kr, *term_kl;
extern char *term_kP, *term_kN;
extern char *term_ke, *term_kh;
extern char *term_kD, *term_ki;
extern char *term_kB;

extern char *term_so, *term_se;

#define MP_NONE 0
#define MP_NORMAL_TRACKING 1
extern int mouse_protocol;

#define COLOUR_MASK             000000000017
#define COLOUR_BLACK    (8 + 0)
#define COLOUR_RED      (8 + 1)
#define COLOUR_GREEN    (8 + 2)
#define COLOUR_YELLOW   (8 + 3)
#define COLOUR_BLUE     (8 + 4)
#define COLOUR_MAGENTA  (8 + 5)
#define COLOUR_CYAN     (8 + 6)
#define COLOUR_WHITE    (8 + 7)
#define UNDERLINE_MASK          000000000020
#define STANDOUT_MASK           000000000040
#define BOLD_MASK               000000000100
#define ZERO1_MASK              000000000200
#define BLINK_MASK              000000000400
#define BGCOLOUR_MASK           000000017000
#define BGCOLOUR_BLACK    ((8 + 0) << 9)
#define BGCOLOUR_RED      ((8 + 1) << 9)
#define BGCOLOUR_GREEN    ((8 + 2) << 9)
#define BGCOLOUR_YELLOW   ((8 + 3) << 9)
#define BGCOLOUR_BLUE     ((8 + 4) << 9)
#define BGCOLOUR_MAGENTA  ((8 + 5) << 9)
#define BGCOLOUR_CYAN     ((8 + 6) << 9)
#define BGCOLOUR_WHITE    ((8 + 7) << 9)
#define ZERO2_MASK              000000100000
#define ZERO3_MASK              000040000000
#define ZERO4_MASK              020000000000

/* ZEROi_MASK are always zero bits. */

void terminal_switch_rendition (unsigned long desired_rendition);


#endif /* !TERMINAL_H */
