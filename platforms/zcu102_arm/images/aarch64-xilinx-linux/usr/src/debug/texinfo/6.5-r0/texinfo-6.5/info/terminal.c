/* terminal.c -- how to handle the physical terminal for Info.
   $Id: terminal.c 7766 2017-04-30 15:01:01Z eliz $

   Copyright 1988, 1989, 1990, 1991, 1992, 1993, 1996, 1997, 1998,
   1999, 2001, 2002, 2004, 2007, 2008, 2012, 2013, 2014, 2015
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

#include "info.h"
#include "terminal.h"
#include "termdep.h"
#include "doc.h"
#include "variables.h"

#include <sys/types.h>
#include <signal.h>

/* The Unix termcap interface code. */
/* With MinGW, if the user has ncurses installed, including termcap.h
   or ncurses/termcap.h will cause the Info binary depend on the ncurses
   DLL, just because BC and PC are declared there, although they are
   never used in the MinGW build.  Avoid that useless dependency.  */
#if defined (HAVE_NCURSES_TERMCAP_H) && !defined (__MINGW32__)
#include <ncurses/termcap.h>
#elif defined (HAVE_TERMCAP_H) && !defined (__MINGW32__)
#include <termcap.h>
#else  /* (!HAVE_NCURSES_TERMCAP_H || __MINGW32__) && !HAVE_TERMCAP_H */
/* On Solaris2, sys/types.h #includes sys/reg.h, which #defines PC.
   Unfortunately, PC is a global variable used by the termcap library. */
#undef PC

/* Termcap requires these variables, whether we access them or not. */
char *BC, *UP;
char PC;      /* Pad character */
short ospeed; /* Terminal output baud rate */
extern int tgetnum (), tgetflag (), tgetent ();
extern char *tgetstr (), *tgoto ();
extern int tputs ();
#endif /* not HAVE_NCURSES_TERMCAP_H */

/* Function "hooks".  If you make one of these point to a function, that
   function is called when appropriate instead of its namesake.  Your
   function is called with exactly the same arguments that were passed
   to the namesake function. */
VFunction *terminal_begin_inverse_hook = NULL;
VFunction *terminal_end_inverse_hook = NULL;
VFunction *terminal_begin_standout_hook = NULL;
VFunction *terminal_end_standout_hook = NULL;
VFunction *terminal_begin_underline_hook = NULL;
VFunction *terminal_end_underline_hook = NULL;
VFunction *terminal_begin_bold_hook = NULL;
VFunction *terminal_begin_blink_hook = NULL;
VFunction *terminal_end_all_modes_hook = NULL;
VFunction *terminal_default_colour_hook = NULL;
VFunction *terminal_set_colour_hook = NULL;
VFunction *terminal_set_bgcolour_hook = NULL;
VFunction *terminal_prep_terminal_hook = NULL;
VFunction *terminal_unprep_terminal_hook = NULL;
VFunction *terminal_up_line_hook = NULL;
VFunction *terminal_down_line_hook = NULL;
VFunction *terminal_clear_screen_hook = NULL;
VFunction *terminal_clear_to_eol_hook = NULL;
VFunction *terminal_get_screen_size_hook = NULL;
VFunction *terminal_goto_xy_hook = NULL;
VFunction *terminal_initialize_terminal_hook = NULL;
VFunction *terminal_new_terminal_hook = NULL;
VFunction *terminal_put_text_hook = NULL;
VFunction *terminal_ring_bell_hook = NULL;
VFunction *terminal_write_chars_hook = NULL;
VFunction *terminal_scroll_terminal_hook = NULL;

/* User variable 'mouse'.  Values can be MP_* constants in terminal.h. */
int mouse_protocol = MP_NONE;

/* **************************************************************** */
/*                                                                  */
/*                      Terminal and Termcap                        */
/*                                                                  */
/* **************************************************************** */

/* A buffer which holds onto the current terminal description, and a pointer
   used to float within it.  And the name of the terminal.  */
static char *term_buffer = NULL;
static char *term_string_buffer = NULL;
static char *term_name;

/* Some strings to control terminal actions.  These are output by tputs (). */
static char *term_goto, *term_clreol, *term_cr, *term_clrpag;
static char *term_begin_use, *term_end_use;
static char *term_AL, *term_DL, *term_al, *term_dl;

static char *term_cs; /* Set scrolling region. */
static char *term_SF, *term_SR; /* Scroll forward and in reverse. */

static char *term_keypad_on, *term_keypad_off;

/* How to go up a line. */
static char *term_up;

/* How to go down a line. */
static char *term_dn;

/* An audible bell, if the terminal can be made to make noise. */
static char *audible_bell;

/* A visible bell, if the terminal can be made to flash the screen. */
static char *visible_bell;

/* The string to turn on inverse mode, if this term has one. */
static char *term_invbeg;

/* The string to turn off inverse mode, if this term has one. */
static char *term_invend;

/* String introducing a mouse event. */
static char *term_Km;

/* Strings entering and leaving standout mode. */
char *term_so, *term_se;

/* Strings entering and leaving underline mode. */
char *term_us, *term_ue;

/* Set foreground and background colours (terminfo setaf and setab) */
char *term_AF, *term_AB;

/* Restore original colours, both foreground and background.
   ("original pair") */
char *term_op;

/* Turn on bold mode. */
char *term_md;

/* Turn on blink mode. */
char *term_mb;

/* Exit all attribute modes. */
char *term_me;


/* Although I can't find any documentation that says this is supposed to
   return its argument, all the code I've looked at (termutils, less)
   does so, so fine.  */
static int
output_character_function (int c)
{
  putc (c, stdout);
  return c;
}

/* Macro to send STRING to the terminal. */
#define send_to_terminal(string) \
  do { \
    if (string) \
      tputs (string, 1, output_character_function); \
     } while (0)

/* Tell the terminal that we will be doing cursor addressable motion.  */
static void
terminal_begin_using_terminal (void)
{
  RETSIGTYPE (*sigsave) (int signum);

  /* Turn on mouse reporting.  This is "normal tracking mode" supported by
     xterm.  The presence of the Km capability may not be a reliable way to
     tell whether this mode exists, but sending the following sequence is
     probably harmless if it doesn't.  */
  if (mouse_protocol == MP_NORMAL_TRACKING
      && term_Km && !strcmp (term_Km, "\033[M"))
    send_to_terminal ("\033[?1000h");
  else
    term_Km = 0;

  if (term_keypad_on)
      send_to_terminal (term_keypad_on);
  
  if (!term_begin_use || !*term_begin_use)
    return;

#ifdef SIGWINCH
  sigsave = signal (SIGWINCH, SIG_IGN); 
#endif

  send_to_terminal (term_begin_use);
  fflush (stdout);
  if (STREQ (term_name, "sun-cmd"))
    /* Without this fflush and sleep, running info in a shelltool or
       cmdtool (TERM=sun-cmd) with scrollbars loses -- the scrollbars are
       not restored properly.
       From: strube@physik3.gwdg.de (Hans Werner Strube).  */
    sleep (1);

#ifdef SIGWINCH
  signal (SIGWINCH, sigsave);
#endif
}

/* Tell the terminal that we will not be doing any more cursor
   addressable motion. */
static void
terminal_end_using_terminal (void)
{
  RETSIGTYPE (*sigsave) (int signum);

  /* Turn off mouse reporting ("normal tracking mode"). */
  if (term_Km)
    send_to_terminal ("\033[?1000l");

  if (term_keypad_off)
      send_to_terminal (term_keypad_off);
  
  if (!term_end_use || !*term_end_use)
    return;

#ifdef SIGWINCH
  sigsave = signal (SIGWINCH, SIG_IGN);
#endif

  send_to_terminal (term_end_use);
  fflush (stdout);
  if (STREQ (term_name, "sun-cmd"))
    /* See comments at other sleep.  */
    sleep (1);

#ifdef SIGWINCH
  signal (SIGWINCH, sigsave);
#endif
}

/* **************************************************************** */
/*                                                                  */
/*                   Necessary Terminal Functions                   */
/*                                                                  */
/* **************************************************************** */

/* The functions and variables on this page implement the user visible
   portion of the terminal interface. */

/* The width and height of the terminal. */
int screenwidth, screenheight;

/* Non-zero means this terminal can't really do anything. */
int terminal_is_dumb_p = 0;

/* Non-zero means that this terminal can produce a visible bell. */
int terminal_has_visible_bell_p = 0;

/* Non-zero means to use that visible bell if at all possible. */
int terminal_use_visible_bell_p = 0;

/* Non-zero means that the terminal can do scrolling. */
int terminal_can_scroll = 0;

/* Non-zero means that the terminal scroll within a restricted region
   of lines. */
int terminal_can_scroll_region = 0;

/* The key sequences output by the arrow keys, if this terminal has any. */
char *term_ku = NULL;
char *term_kd = NULL;
char *term_kr = NULL;
char *term_kl = NULL;
char *term_kP = NULL;   /* page-up */
char *term_kN = NULL;   /* page-down */
char *term_kh = NULL;	/* home */
char *term_ke = NULL;	/* end */
char *term_kD = NULL;	/* delete */
char *term_ki = NULL;	/* ins */
char *term_kB = NULL;	/* back tab */

/* Move the cursor to the terminal location of X and Y. */
void
terminal_goto_xy (int x, int y)
{
  if (terminal_goto_xy_hook)
    (*terminal_goto_xy_hook) (x, y);
  else
    {
      if (term_goto)
        tputs (tgoto (term_goto, x, y), 1, output_character_function);
    }
}

/* Print STRING to the terminal at the current position. */
void
terminal_put_text (char *string)
{
  if (terminal_put_text_hook)
    (*terminal_put_text_hook) (string);
  else
    {
      printf ("%s", string);
    }
}

/* Print NCHARS from STRING to the terminal at the current position. */
void
terminal_write_chars (char *string, int nchars)
{
  if (terminal_write_chars_hook)
    (*terminal_write_chars_hook) (string, nchars);
  else
    {
      if (nchars)
        fwrite (string, 1, nchars, stdout);
    }
}

/* Clear from the current position of the cursor to the end of the line. */
void
terminal_clear_to_eol (void)
{
  if (terminal_clear_to_eol_hook)
    (*terminal_clear_to_eol_hook) ();
  else
    {
      send_to_terminal (term_clreol);
    }
}

/* Clear the entire terminal screen. */
void
terminal_clear_screen (void)
{
  if (terminal_clear_screen_hook)
    (*terminal_clear_screen_hook) ();
  else
    {
      send_to_terminal (term_clrpag);
    }
}

/* Move the cursor up one line. */
void
terminal_up_line (void)
{
  if (terminal_up_line_hook)
    (*terminal_up_line_hook) ();
  else
    {
      send_to_terminal (term_up);
    }
}

/* Move the cursor down one line. */
void
terminal_down_line (void)
{
  if (terminal_down_line_hook)
    (*terminal_down_line_hook) ();
  else
    {
      send_to_terminal (term_dn);
    }
}

/* Turn on reverse video if possible. */
void
terminal_begin_inverse (void)
{
  if (terminal_begin_inverse_hook)
    (*terminal_begin_inverse_hook) ();
  else
    {
      send_to_terminal (term_invbeg);
    }
}

/* Turn off reverse video if possible. */
void
terminal_end_inverse (void)
{
  if (terminal_end_inverse_hook)
    (*terminal_end_inverse_hook) ();
  else
    {
      send_to_terminal (term_invend);
    }
}

/* Turn on "standout mode" if possible.  Likely the same
   as reverse video. */
void
terminal_begin_standout (void)
{
  if (terminal_begin_standout_hook)
    (*terminal_begin_standout_hook) ();
  else
    {
      send_to_terminal (term_so);
    }
}

/* Turn off "standout mode" if possible. */
void
terminal_end_standout (void)
{
  if (terminal_end_standout_hook)
    (*terminal_end_standout_hook) ();
  else
    {
      send_to_terminal (term_se);
    }
}

void
terminal_begin_underline (void)
{
  if (terminal_begin_underline_hook)
    (*terminal_begin_underline_hook) ();
  else
    {
      send_to_terminal (term_us);
    }
}

void
terminal_end_underline (void)
{
  if (terminal_end_underline_hook)
    (*terminal_end_underline_hook) ();
  else
    {
      send_to_terminal (term_ue);
    }
}

void
terminal_begin_bold (void)
{
  if (terminal_begin_bold_hook)
    (*terminal_begin_bold_hook) ();
  else
    {
      send_to_terminal (term_md);
    }
}

void
terminal_begin_blink (void)
{
  if (terminal_begin_blink_hook)
    (*terminal_begin_blink_hook) ();
  else
    {
      send_to_terminal (term_mb);
    }
}

void
terminal_end_all_modes (void)
{
  if (terminal_end_all_modes_hook)
    (*terminal_end_all_modes_hook) ();
  else
    {
      send_to_terminal (term_me);
    }
}

/* Ring the terminal bell.  The bell is run visibly if it both has one and
   terminal_use_visible_bell_p is non-zero. */
void
terminal_ring_bell (void)
{
  if (terminal_ring_bell_hook)
    (*terminal_ring_bell_hook) ();
  else
    {
      if (terminal_has_visible_bell_p && terminal_use_visible_bell_p)
        send_to_terminal (visible_bell);
      else
        send_to_terminal (audible_bell);
    }
}

/* At the line START, delete COUNT lines from the terminal display. */
static void
terminal_delete_lines (int start, int count)
{
  int lines;

  /* Normalize arguments. */
  if (start < 0)
    start = 0;

  lines = screenheight - start;
  terminal_goto_xy (0, start);
  if (term_DL)
    tputs (tgoto (term_DL, 0, count), lines, output_character_function);
  else
    {
      while (count--)
        tputs (term_dl, lines, output_character_function);
    }

  fflush (stdout);
}

/* At the line START, insert COUNT lines in the terminal display. */
static void
terminal_insert_lines (int start, int count)
{
  int lines;

  /* Normalize arguments. */
  if (start < 0)
    start = 0;

  lines = screenheight - start;
  terminal_goto_xy (0, start);

  if (term_AL)
    tputs (tgoto (term_AL, 0, count), lines, output_character_function);
  else
    {
      while (count--)
        tputs (term_al, lines, output_character_function);
    }

  fflush (stdout);
}

void
terminal_scroll_region (int start, int end, int amount)
{
  /* Any scrolling at all? */
  if (amount == 0)
    return;

  if (terminal_scroll_terminal_hook)
    {
      (*terminal_scroll_terminal_hook) (start, end, amount);
      return;
    }

  if (terminal_can_scroll_region)
    {
      /* Set scrolling region. */
      tputs (tgoto (term_cs, end - 1, start), 0, output_character_function);

      /* Scroll. */
      if (amount > 0)
        tputs (tgoto (term_SR, 0, amount), 0, output_character_function);
      else
        tputs (tgoto (term_SF, 0, -amount), 0, output_character_function);

      /* Reset scrolling region. */
      tputs (tgoto (term_cs, screenheight - 1, 0), 0, output_character_function);
      return;
    }
}

/* Scroll an area of the terminal, starting with the region from START
   to END, AMOUNT lines.  If AMOUNT is negative, the lines are scrolled
   towards the top of the screen, else they are scrolled towards the
   bottom of the screen. */
void
terminal_scroll_terminal (int start, int end, int amount)
{
  if (!terminal_can_scroll)
    return;

  /* Any scrolling at all? */
  if (amount == 0)
    return;

  if (terminal_scroll_terminal_hook)
    (*terminal_scroll_terminal_hook) (start, end, amount);
  else if (amount > 0)
    {
      /* If we are scrolling down, delete AMOUNT lines at END.  Then insert
         AMOUNT lines at START. */
      terminal_delete_lines (end, amount);
      terminal_insert_lines (start, amount);
    }
  else
    {
      /* If we are scrolling up, delete AMOUNT lines before START.  This
         actually does the upwards scroll.  Then, insert AMOUNT lines
         after the already scrolled region (i.e., END - AMOUNT). */
      int abs_amount = -amount;
      terminal_delete_lines (start - abs_amount, abs_amount);
      terminal_insert_lines (end - abs_amount, abs_amount);
    }
}

/* Revert to the default foreground and background colours. */
static void
terminal_default_colour (void)
{
  if (terminal_default_colour_hook)
    (*terminal_default_colour_hook) ();
  else
    tputs (term_op, 0, output_character_function);
}

static void
terminal_set_colour (int colour)
{
  if (terminal_set_colour_hook)
    (*terminal_set_colour_hook) (colour);
  else
    tputs (tgoto (term_AF, 0, colour), 0, output_character_function);
}

static void
terminal_set_bgcolour (int colour)
{
  if (terminal_set_bgcolour_hook)
    (*terminal_set_bgcolour_hook) (colour);
  else
    tputs (tgoto (term_AB, 0, colour), 0, output_character_function);
}

/* Information about what styles like colour, underlining, boldface are
   currently output for text on the screen.  All zero represents the default
   rendition. */
static unsigned long terminal_rendition;

/* Modes for which there aren't termcap entries for turning them off. */
#define COMBINED_MODES (BOLD_MASK | BLINK_MASK)

void
terminal_switch_rendition (unsigned long new)
{
  unsigned long old = terminal_rendition;

  if ((old & new & COMBINED_MODES) != (old & COMBINED_MODES))
    {
      /* Some modes we can't turn off by themselves, so if we need to turn
         one of them off, turn back on all the ones that should be on 
         afterwards. */
      terminal_end_all_modes ();
      old = 0;
    }
  else if (!(new & COLOUR_MASK) && (old & COLOUR_MASK)
           || !(new & BGCOLOUR_MASK) && (old & BGCOLOUR_MASK))
    {
      terminal_default_colour ();
      old &= ~(COLOUR_MASK|BGCOLOUR_MASK);
    }

  if ((new & COLOUR_MASK) != (old & COLOUR_MASK))
    {
      if ((new & COLOUR_MASK) >= 8)
        {
          terminal_set_colour ((new & COLOUR_MASK) - 8);
        }
      /* Colour values from 1 to 7 don't do anything right now. */
    }
  if ((new & BGCOLOUR_MASK) != (old & BGCOLOUR_MASK))
    {
      /* Switch colour. */
      if ((new & BGCOLOUR_MASK) >> 9 >= 8)
        {
          terminal_set_bgcolour (((new & BGCOLOUR_MASK) >> 9) - 8);
        }
      /* Colour values from 1 to 7 don't do anything right now. */
    }

  if ((new & UNDERLINE_MASK) != (old & UNDERLINE_MASK))
    {
      if ((new & UNDERLINE_MASK))
        terminal_begin_underline ();
      else
        terminal_end_underline ();
    }
  if ((new & STANDOUT_MASK) != (old & STANDOUT_MASK))
    {
      if ((new & STANDOUT_MASK))
        terminal_begin_standout ();
      else
        terminal_end_standout ();
    }
  if ((new & BOLD_MASK) != (old & BOLD_MASK))
    {
      if ((new & BOLD_MASK))
        terminal_begin_bold ();
    }
  if ((new & BLINK_MASK) != (old & BLINK_MASK))
    {
      if ((new & BLINK_MASK))
        terminal_begin_blink ();
    }
  terminal_rendition = new;
}


/* Re-initialize the terminal considering that the TERM/TERMCAP variable
   has changed. */
void
terminal_new_terminal (char *terminal_name)
{
  if (terminal_new_terminal_hook)
    (*terminal_new_terminal_hook) (terminal_name);
  else
    {
      terminal_initialize_terminal (terminal_name);
    }
}

/* Saved values of the LINES and COLUMNS environmental variables. */
static char *env_lines, *env_columns;

/* Set the global variables SCREENWIDTH and SCREENHEIGHT. */
void
terminal_get_screen_size (void)
{
  if (terminal_get_screen_size_hook)
    (*terminal_get_screen_size_hook) ();
  else
    {
      screenwidth = screenheight = 0;

#if defined (TIOCGWINSZ)
      {
        struct winsize window_size;

        if (ioctl (fileno (stdout), TIOCGWINSZ, &window_size) == 0)
          {
            screenwidth = (int) window_size.ws_col;
            screenheight = (int) window_size.ws_row;
          }
      }
#endif                          /* TIOCGWINSZ */

      /* Environment variable COLUMNS overrides setting of "co". */
      if (screenwidth <= 0)
        {
          if (env_columns)
            screenwidth = atoi (env_columns);

          if (screenwidth <= 0)
            screenwidth = tgetnum ("co");
        }

      /* Environment variable LINES overrides setting of "li". */
      if (screenheight <= 0)
        {
          if (env_lines)
            screenheight = atoi (env_lines);

          if (screenheight <= 0)
            screenheight = tgetnum ("li");
        }

      /* If all else fails, default to 80x24 terminal. */
      if (screenwidth <= 0)
        screenwidth = 80;

      if (screenheight <= 0)
        screenheight = 24;
    }
}

/* Root of structure representing a mapping from sequences of bytes to named
   keys. */
BYTEMAP_ENTRY *byte_seq_to_key;

static void
add_seq_to_byte_map (int key_id, char *seq)
{
  BYTEMAP_ENTRY *b = byte_seq_to_key;

  /* Must consider bytes as unsigned because we use them as array indices. */
  unsigned char *c = (unsigned char *) seq;
  for (; *c; c++)
    {
      if (c[1] == '\0') /* Last character. */
        {
          b[*c].type = BYTEMAP_KEY;
          b[*c].key = key_id;
        }
      else
        {
          b[*c].type = BYTEMAP_MAP;
          b[*c].key = 0;
          if (!b[*c].next)
            b[*c].next = xzalloc (256 * sizeof (BYTEMAP_ENTRY));
          b = b[*c].next;
        }
    }
}

/* When non-zero, various display and input functions handle extended
   character sets such as ISO Latin or UTF-8 correctly. */
int ISO_Latin_p = 1;

/* Initialize byte map read in get_input_key. */
static void
initialize_byte_map (void)
{
  int i;

  static struct special_keys {
      int key_id;
      char **byte_seq;
  } keys[] = {
      KEY_RIGHT_ARROW, &term_kr,
      KEY_LEFT_ARROW, &term_kl,
      KEY_UP_ARROW, &term_ku,
      KEY_DOWN_ARROW, &term_kd,
      KEY_PAGE_UP, &term_kP,
      KEY_PAGE_DOWN, &term_kN,
      KEY_HOME, &term_kh,
      KEY_END, &term_ke,
      KEY_DELETE, &term_kD,
      KEY_INSERT, &term_ki,
      KEY_BACK_TAB, &term_kB
  };

  /* Recognize arrow key sequences with both of the usual prefixes in case they 
     are missing in the termcap entry. */
  static struct special_keys2 {
      int key_id;
      char *byte_seq;
  } keys2[] = {
      KEY_RIGHT_ARROW, "\033[C",
      KEY_RIGHT_ARROW, "\033OC",
      KEY_LEFT_ARROW, "\033[D",
      KEY_LEFT_ARROW, "\033OD",
      KEY_UP_ARROW, "\033[A",
      KEY_UP_ARROW, "\033OA",
      KEY_DOWN_ARROW, "\033[B",
      KEY_DOWN_ARROW, "\033OB"
  };

  byte_seq_to_key = xmalloc (256 * sizeof (BYTEMAP_ENTRY));

  /* Make each byte represent itself by default. */
  for (i = 0; i < 128; i++)
    {
      byte_seq_to_key[i].type = BYTEMAP_KEY;
      byte_seq_to_key[i].key = i;
      byte_seq_to_key[i].next = 0;
    }

  /* Use 'ISO-Latin' variable to decide whether bytes with the 8th bit set 
     represent the Meta key being pressed.  Maybe we should have another 
     variable to enable 8-bit input.  If 'ISO-Latin' is set this allows input 
     of non-ASCII characters in the echo area. */
  if (!ISO_Latin_p)
    for (i = 128; i < 256; i++)
      {
        byte_seq_to_key[i].type = BYTEMAP_KEY;
        byte_seq_to_key[i].key = (i - 128) + KEYMAP_META_BASE;
        byte_seq_to_key[i].next = 0;
      }

  /* Hard-code octal 177 = delete.  Either 177 or the term_kD sequence will
     result in a delete key being registered. */
  byte_seq_to_key['\177'].type = BYTEMAP_KEY;
  byte_seq_to_key['\177'].key = KEY_DELETE;
  byte_seq_to_key['\177'].next = 0;

  /* For each special key, record its byte sequence. */
  for (i = 0; i < sizeof (keys) / sizeof (*keys); i++)
    {
      if (!*keys[i].byte_seq)
        continue; /* No byte sequence known for this key. */

      add_seq_to_byte_map (keys[i].key_id, *keys[i].byte_seq);
    }

  /* Hard-coded byte sequences. */
  for (i = 0; i < sizeof (keys2) / sizeof (*keys2); i++)
    {
      add_seq_to_byte_map (keys2[i].key_id, keys2[i].byte_seq);
    }

  if (term_Km)
    add_seq_to_byte_map (KEY_MOUSE, term_Km);

  /* Special case for ESC: Can introduce special key sequences, represent the
     Meta key being pressed, or be a key on its own. */
  byte_seq_to_key['\033'].type = BYTEMAP_ESC;
}

/* Initialize the terminal which is known as TERMINAL_NAME.  If this
   terminal doesn't have cursor addressability, `terminal_is_dumb_p'
   becomes nonzero.  The variables SCREENHEIGHT and SCREENWIDTH are set
   to the dimensions that this terminal actually has.  Get and save various
   termcap strings. */
void
terminal_initialize_terminal (char *terminal_name)
{
  char *buffer;

  terminal_is_dumb_p = 0;

  if (terminal_initialize_terminal_hook)
    {
      (*terminal_initialize_terminal_hook) (terminal_name);
      initialize_byte_map ();
      return;
    }

  term_name = terminal_name ? terminal_name : getenv ("TERM");
  if (!term_name)
    term_name = "dumb";

  env_lines = getenv ("LINES");
  env_columns = getenv ("COLUMNS");
  /* We save LINES and COLUMNS before the call to tgetent below, because
     on some openSUSE systems, including openSUSE 12.3, the call to tgetent 
     changes the values returned by getenv for these. */

  if (!term_string_buffer)
    term_string_buffer = xmalloc (2048);

  if (!term_buffer)
    term_buffer = xmalloc (2048);

  buffer = term_string_buffer;

  term_clrpag = term_cr = term_clreol = NULL;

  /* HP-UX 11.x returns 0 for OK --jeff.hull@state.co.us.  */
  if (tgetent (term_buffer, term_name) < 0)
    {
      terminal_is_dumb_p = 1;
      screenwidth = 80;
      screenheight = 24;
      term_cr = "\r";
      term_up = term_dn = audible_bell = visible_bell = NULL;
      term_ku = term_kd = term_kl = term_kr = NULL;
      term_kP = term_kN = NULL;
      term_kh = term_ke = NULL;
      term_kD = NULL;
      return;
    }

  BC = tgetstr ("pc", &buffer);
  PC = BC ? *BC : 0;

#if defined (HAVE_TERMIOS_H)
  {
    struct termios ti;
    if (tcgetattr (fileno(stdout), &ti) != -1)
      ospeed = cfgetospeed (&ti);
    else
      ospeed = B9600;
  }
#else
# if defined (TIOCGETP)
  {
    struct sgttyb sg;

    if (ioctl (fileno (stdout), TIOCGETP, &sg) != -1)
      ospeed = sg.sg_ospeed;
    else
      ospeed = B9600;
  }
# else
#ifndef __MINGW32__
  ospeed = B9600;
#endif
# endif /* !TIOCGETP */
#endif

  term_cr = tgetstr ("cr", &buffer);
  term_clreol = tgetstr ("ce", &buffer);
  term_clrpag = tgetstr ("cl", &buffer);
  term_goto = tgetstr ("cm", &buffer);

  /* Find out about this terminal's scrolling capability. */
  term_AL = tgetstr ("AL", &buffer);
  term_DL = tgetstr ("DL", &buffer);
  term_al = tgetstr ("al", &buffer);
  term_dl = tgetstr ("dl", &buffer);

  term_cs = tgetstr ("cs", &buffer);
  term_SF = tgetstr ("SF", &buffer);
  term_SR = tgetstr ("SR", &buffer);

  terminal_can_scroll = ((term_AL || term_al) && (term_DL || term_dl));
  terminal_can_scroll_region = term_cs && term_SF && term_SR;

  term_invbeg = tgetstr ("mr", &buffer);
  if (term_invbeg)
    term_invend = tgetstr ("me", &buffer);
  else
    term_invend = NULL;

  term_so = tgetstr ("so", &buffer);
  if (term_so)
    term_se = tgetstr ("se", &buffer);
  else
    term_se = NULL;

  term_us = tgetstr ("us", &buffer);
  if (term_us)
    term_ue = tgetstr ("ue", &buffer);
  else
    term_ue = NULL;

  term_AF = tgetstr ("AF", &buffer);
  if (term_AF)
    term_AB = tgetstr ("AB", &buffer);
  else
    term_AB = NULL;

  term_op = tgetstr ("op", &buffer);

  term_md = tgetstr ("md", &buffer);
  term_mb = tgetstr ("mb", &buffer);

  term_me = tgetstr ("me", &buffer);
  if (!term_me)
    term_md = 0; /* Don't use modes if we can't turn them off. */

  if (!term_cr)
    term_cr =  "\r";

  terminal_get_screen_size ();

  term_up = tgetstr ("up", &buffer);
  term_dn = tgetstr ("dn", &buffer);
  visible_bell = tgetstr ("vb", &buffer);
  terminal_has_visible_bell_p = (visible_bell != NULL);
  audible_bell = tgetstr ("bl", &buffer);
  if (!audible_bell)
    audible_bell = "\007";
  term_begin_use = tgetstr ("ti", &buffer);
  term_end_use = tgetstr ("te", &buffer);

  term_keypad_on = tgetstr ("ks", &buffer);
  term_keypad_off = tgetstr ("ke", &buffer);

  /* Attempt to find the arrow keys.  */
  term_ku = tgetstr ("ku", &buffer);
  term_kd = tgetstr ("kd", &buffer);
  term_kr = tgetstr ("kr", &buffer);
  term_kl = tgetstr ("kl", &buffer);

  term_kP = tgetstr ("kP", &buffer);
  term_kN = tgetstr ("kN", &buffer);

  term_kh = tgetstr ("kh", &buffer);
  term_ke = tgetstr ("@7", &buffer);
  term_ki = tgetstr ("kI", &buffer);
  term_kD = tgetstr ("kD", &buffer);

  term_kB = tgetstr ("kB", &buffer);

  /* String introducing a mouse event. */
  term_Km = tgetstr ("Km", &buffer);

  initialize_byte_map ();

  /* If this terminal is not cursor addressable, then it is really dumb. */
  if (!term_goto)
    terminal_is_dumb_p = 1;
}

/* How to read characters from the terminal.  */

#if defined (HAVE_TERMIOS_H)
struct termios original_termios, ttybuff;
#else
#  if defined (HAVE_TERMIO_H)
/* A buffer containing the terminal mode flags upon entry to info. */
struct termio original_termio, ttybuff;
#  else /* !HAVE_TERMIO_H */
/* Buffers containing the terminal mode flags upon entry to info. */
int original_tty_flags = 0;
int original_lmode;
#ifndef __MINGW32__
struct sgttyb ttybuff;
#endif

#    if defined(TIOCGETC) && defined(M_XENIX)
/* SCO 3.2v5.0.2 defines but does not support TIOCGETC.  Gak.  Maybe
   better fix would be to use Posix termios in preference.  --gildea,
   1jul99.  */
#      undef TIOCGETC
#    endif

#    if defined (TIOCGETC)
/* A buffer containing the terminal interrupt characters upon entry
   to Info. */
struct tchars original_tchars;
#    endif

#    if defined (TIOCGLTC)
/* A buffer containing the local terminal mode characters upon entry
   to Info. */
struct ltchars original_ltchars;
#    endif
#  endif /* !HAVE_TERMIO_H */
#endif /* !HAVE_TERMIOS_H */

/* Prepare to start using the terminal to read characters singly.  Return
   0 if terminal is too dumb to run Info interactively. */
int
terminal_prep_terminal (void)
{
  int tty;

  if (terminal_is_dumb_p)
    return 0;

  if (terminal_prep_terminal_hook)
    {
      (*terminal_prep_terminal_hook) ();
      return 1;
    }

  terminal_begin_using_terminal ();

  tty = fileno (stdin);

#if defined (HAVE_TERMIOS_H)
  tcgetattr (tty, &original_termios);
  tcgetattr (tty, &ttybuff);
#else
#  if defined (HAVE_TERMIO_H)
  ioctl (tty, TCGETA, &original_termio);
  ioctl (tty, TCGETA, &ttybuff);
#  endif
#endif

#if defined (HAVE_TERMIOS_H) || defined (HAVE_TERMIO_H)
  ttybuff.c_iflag &= (~ISTRIP & ~INLCR & ~IGNCR & ~ICRNL & ~IXON);
/* These output flags are not part of POSIX, so only use them if they
   are defined.  */
#ifdef ONLCR
  ttybuff.c_oflag &= ~ONLCR ;
#endif
#ifdef OCRNL
  ttybuff.c_oflag &= ~OCRNL;
#endif
  ttybuff.c_lflag &= (~ICANON & ~ECHO);

  ttybuff.c_cc[VMIN] = 1;
  ttybuff.c_cc[VTIME] = 0;

  if (ttybuff.c_cc[VINTR] == '\177')
    ttybuff.c_cc[VINTR] = -1;

  if (ttybuff.c_cc[VQUIT] == '\177')
    ttybuff.c_cc[VQUIT] = -1;

#ifdef VLNEXT
  if (ttybuff.c_cc[VLNEXT] == '\026')
    ttybuff.c_cc[VLNEXT] = -1;
#endif /* VLNEXT */
#endif /* TERMIOS or TERMIO */

/* cf. emacs/src/sysdep.c for being sure output is on. */
#if defined (HAVE_TERMIOS_H)
  /* linux kernel 2.2.x needs a TCOFF followed by a TCOON to turn output
     back on if the user presses ^S at the very beginning; just a TCOON
     doesn't work.  --Kevin Ryde <user42@zip.com.au>, 16jun2000.  */
  tcsetattr (tty, TCSANOW, &ttybuff);
#  ifdef TCOON
  tcflow (tty, TCOOFF);
  tcflow (tty, TCOON);
#  endif
#else
#  if defined (HAVE_TERMIO_H)
  ioctl (tty, TCSETA, &ttybuff);
#    ifdef TCXONC
  ioctl (tty, TCXONC, 1);
#    endif
#  endif
#endif

#if !defined (HAVE_TERMIOS_H) && !defined (HAVE_TERMIO_H) && !defined(__MINGW32__)
  ioctl (tty, TIOCGETP, &ttybuff);

  if (!original_tty_flags)
    original_tty_flags = ttybuff.sg_flags;

  /* Make this terminal pass 8 bits around while we are using it. */
#  if defined (PASS8)
  ttybuff.sg_flags |= PASS8;
#  endif /* PASS8 */

#  if defined (TIOCLGET) && defined (LPASS8)
  {
    int flags;
    ioctl (tty, TIOCLGET, &flags);
    original_lmode = flags;
    flags |= LPASS8;
    ioctl (tty, TIOCLSET, &flags);
  }
#  endif /* TIOCLGET && LPASS8 */

#  if defined (TIOCGETC)
  {
    struct tchars temp;

    ioctl (tty, TIOCGETC, &original_tchars);
    temp = original_tchars;

    /* C-s and C-q. */
    temp.t_startc = temp.t_stopc = -1;

    /* Often set to C-d. */
    temp.t_eofc = -1;

    /* If the a quit or interrupt character conflicts with one of our
       commands, then make it go away. */
    if (temp.t_intrc == '\177')
      temp.t_intrc = -1;

    if (temp.t_quitc == '\177')
      temp.t_quitc = -1;

    ioctl (tty, TIOCSETC, &temp);
  }
#  endif /* TIOCGETC */

#  if defined (TIOCGLTC)
  {
    struct ltchars temp;

    ioctl (tty, TIOCGLTC, &original_ltchars);
    temp = original_ltchars;

    /* Make the interrupt keys go away.  Just enough to make people happy. */
    temp.t_lnextc = -1;         /* C-v. */
    temp.t_dsuspc = -1;         /* C-y. */
    temp.t_flushc = -1;         /* C-o. */
    ioctl (tty, TIOCSLTC, &temp);
  }
#  endif /* TIOCGLTC */

# ifndef __MINGW32__
  ttybuff.sg_flags &= ~ECHO;
  ttybuff.sg_flags |= CBREAK;
  ioctl (tty, TIOCSETN, &ttybuff);
# endif
#endif /* !HAVE_TERMIOS_H && !HAVE_TERMIO_H */
  return 1;
}

/* Restore the tty settings back to what they were before we started using
   this terminal. */
void
terminal_unprep_terminal (void)
{
  int tty;

  if (terminal_unprep_terminal_hook)
    {
      (*terminal_unprep_terminal_hook) ();
      return;
    }

  tty = fileno (stdin);

#if defined (HAVE_TERMIOS_H)
  tcsetattr (tty, TCSANOW, &original_termios);
#else
#  if defined (HAVE_TERMIO_H)
  ioctl (tty, TCSETA, &original_termio);
#  else /* !HAVE_TERMIO_H */
#   ifndef __MINGW32__
  ioctl (tty, TIOCGETP, &ttybuff);
  ttybuff.sg_flags = original_tty_flags;
  ioctl (tty, TIOCSETN, &ttybuff);
#   endif

#  if defined (TIOCGETC)
  ioctl (tty, TIOCSETC, &original_tchars);
#  endif /* TIOCGETC */

#  if defined (TIOCGLTC)
  ioctl (tty, TIOCSLTC, &original_ltchars);
#  endif /* TIOCGLTC */

#  if defined (TIOCLGET) && defined (LPASS8)
  ioctl (tty, TIOCLSET, &original_lmode);
#  endif /* TIOCLGET && LPASS8 */

#  endif /* !HAVE_TERMIO_H */
#endif /* !HAVE_TERMIOS_H */
  terminal_end_using_terminal ();
}

#if defined(__MSDOS__) || defined(__MINGW32__)
# include "pcterm.c"
#endif
