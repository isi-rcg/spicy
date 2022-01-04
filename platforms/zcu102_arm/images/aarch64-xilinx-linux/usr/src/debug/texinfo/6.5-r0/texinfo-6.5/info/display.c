/* display.c -- How to display Info windows.
   $Id: display.c 7760 2017-04-27 18:06:21Z gavin $

   Copyright 1993, 1997, 2003, 2004, 2006, 2007, 2008, 2012, 2013,
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
#include "tag.h"
#include "signals.h"
#include "variables.h"

static void free_display (DISPLAY_LINE **display);
static DISPLAY_LINE **make_display (int width, int height);

/* An array of display lines which tell us what is currently visible on
   the display.  */
DISPLAY_LINE **the_display = NULL;

/* Non-zero means do no output. */
int display_inhibited = 0;

/* Initialize THE_DISPLAY to WIDTH and HEIGHT, with nothing in it. */
void
display_initialize_display (int width, int height)
{
  free_display (the_display);
  the_display = make_display (width, height);
  display_clear_display (the_display);
}

/* Clear all of the lines in DISPLAY making the screen blank. */
void
display_clear_display (DISPLAY_LINE **display)
{
  register int i;

  signal_block_winch ();
  for (i = 0; display[i]; i++)
    {
      display[i]->text[0] = '\0';
      display[i]->textlen = 0;
      display[i]->inverse = 0;
    }
  signal_unblock_winch ();
}

/* Non-zero if we didn't completely redisplay a window. */
int display_was_interrupted_p = 0;

/* Check each window on the screen, and update it if it needs updating. */
void
display_update_display (void)
{
  register WINDOW *win;

  /* Block window resize signals (SIGWINCH) while accessing the the_display
     object, because the signal handler may reallocate it out from under our
     feet. */
  signal_block_winch ();
  display_was_interrupted_p = 0;

  for (win = windows; win; win = win->next)
    {
      /* Only re-display visible windows which need updating. */
      if ((win->flags & W_WindowVisible) == 0
          || (win->flags & W_UpdateWindow) == 0
          || win->height == 0)
        continue;

      display_update_one_window (win);
      if (display_was_interrupted_p)
        break;
    }

  /* Always update the echo area. */
  display_update_one_window (the_echo_area);
  signal_unblock_winch ();
}

/* Return the screen column of where to write to screen to update line to
   match A, given that B contains the current state of the line.  *PPOS gets
   the offset into the string A to write from. */
static int
find_diff (const char *a, size_t alen, const char *b, size_t blen, int *ppos)
{
  mbi_iterator_t itra, itrb;
  int i;
  int pos = 0;
  int first_escape = -1;
  int escape_pos = -1;
  
  for (i = 0, mbi_init (itra, a, alen), mbi_init (itrb, b, blen);
       mbi_avail (itra) && mbi_avail (itrb);
       i += wcwidth (itra.cur.wc), mbi_advance (itra), mbi_advance (itrb))
    {
      if (mb_cmp (mbi_cur (itra), mbi_cur (itrb)))
        break;

      if (first_escape == -1 && *mbi_cur_ptr (itra) == '\033')
        {
          first_escape = i;
          escape_pos = pos;
        }
      pos += mb_len (mbi_cur (itra));
    }

  if (mbi_avail (itra) || mbi_avail (itrb))
    {
      if (first_escape != -1)
        {
          *ppos = escape_pos;
          return first_escape;
        }
      else
        {
          /* If there was a difference in the line, and there was an escape
             character, return the position of the escape character, as it could
             start a terminal escape sequence. */
          *ppos = pos;
          return i;
        }
    }

  /* Otherwise, no redrawing is required. */
  return -1;
}

/* Update line PL_NUM of the screen to be PRINTED_LINE, which is PL_BYTES long
   and takes up PL_CHARS columns. */
static int
display_update_line (long pl_num, char *printed_line,
                     long pl_bytes, long pl_chars)
{
  DISPLAY_LINE **display = the_display;
  DISPLAY_LINE *entry;

 entry = display[pl_num];

  /* We have the exact line as it should appear on the screen.
     Check to see if this line matches the one already appearing
     on the screen. */

  /* If the window is very small, entry might be NULL. */
  if (entry)
    {
      int i, off;
	      
      /* If the screen line is inversed, or if the entry is marked as
         invalid, then clear the line from the screen first. */
      if (entry->inverse)
	{
	  terminal_goto_xy (0, pl_num);
	  terminal_clear_to_eol ();
	  entry->inverse = 0;
	  entry->text[0] = '\0';
	  entry->textlen = 0;
	}

      i = find_diff (printed_line, pl_bytes,
		     entry->text, strlen (entry->text), &off);

      /* If the lines differed at all, we must do some redrawing. */
      if (i != -1)
	{
	  /* Move to the proper point on the terminal. */
	  terminal_goto_xy (i, pl_num);

	  /* If there is any text to print, print it. */
          terminal_put_text (printed_line + off);
	  
	  /* If the printed text didn't extend all the way to the edge
	     of the screen, and text was appearing between here and the
	     edge of the screen, clear from here to the end of the
	     line. */
	  if ((pl_chars < screenwidth && pl_chars < entry->textlen)
	      || entry->inverse)
	    terminal_clear_to_eol ();
	  
	  fflush (stdout);
	  
	  /* Update the display text buffer. */
	  if (strlen (printed_line) > (unsigned int) screenwidth)
	    /* printed_line[] can include more than screenwidth
	       characters, e.g. if multibyte encoding is used or
	       if we are under -R and there are escape sequences
	       in it.  However, entry->text was allocated (in
	       display_initialize_display) for screenwidth
	       bytes only.  */
	    entry->text = xrealloc (entry->text, strlen (printed_line) + 1);
	  strcpy (entry->text + off, printed_line + off);
	  entry->textlen = pl_chars;
	  
	  /* Lines showing node text are not in inverse.  Only modelines
	     have that distinction. */
	  entry->inverse = 0;
	}
    }

  /* A line has been displayed, and the screen reflects that state.
     If there is typeahead pending, then let that typeahead be read
     now, instead of continuing with the display. */
  if (info_any_buffered_input_p ())
    {
      display_was_interrupted_p = 1;
      return 1;
    }
  return 0;
}


/* Similar to decide_if_in_match, but used for reference highlighting.
   Given an array REFERENCES with regions, starting at *REF_INDEX decide
   if we are inside a region at offset OFF.  The regions are assumed not
   to overlap and to be in order. */
static void
decide_if_in_reference (long off, int *in_ref, REFERENCE **references,
                        int *ref_index)
{
  int i = *ref_index;
  int m = *in_ref;

  for (; (references[i]); i++)
    {
      if (references[i]->start > off)
        break;

      m = 1;

      if (references[i]->end > off)
        break;

      m = 0;
    }

  *ref_index = i;
  *in_ref = m;
}

/* Used when processing a line to be displayed from a node.  DEFAULT is the
   value when the line has no special styles like underlined references or
   highlighted search matches.  Otherwise, a line is processed once with
   COLLECT as the value, and if it differs to what is on the display already,
   it is processed with WRITEOUT and written to the display. */
static int writing_out;
#define DEFAULT 0
#define COLLECT 1
#define WRITEOUT 2 /* Values for writing_out global. */

/* Combine rendition masks that are active, in order of priority,
   then check what's currently active on the display, and output
   the necessary codes to switch.  The list of rendition masks is
   the complete information about what the style should now be.
   RENDITION3 takes priority over RENDITION2, which in turn takes
   priority over RENDITION1. */
static void
wrap_terminal_switch_rendition (struct text_buffer *printed_line,
                                 RENDITION rendition1,
                                 RENDITION rendition2,
                                 RENDITION rendition3)
{
  long int desired_rendition = 0;
  desired_rendition = rendition1.value;
  desired_rendition &= ~rendition2.mask;
  desired_rendition |= rendition2.value;
  desired_rendition &= ~rendition3.mask;
  desired_rendition |= rendition3.value;

  if (writing_out == WRITEOUT)
    terminal_switch_rendition (desired_rendition);
  else
    {
      /* Guarantee that each byte is non-zero, by having at least one
         non-zero bit in it.  See ZERO1_MASK symbol in display.c. */
      desired_rendition = ~desired_rendition;

      /* The text added here is only used internally to see when the
         display has changed, and is not output to the terminal. */
      text_buffer_add_string (printed_line, "\033", 1);
      text_buffer_add_string (printed_line, (char *) &desired_rendition,
                              sizeof (long));
    }
}

/* Set in display_update_node_text if matches or references are to be 
   distinguished with terminal appearance modes. */
static MATCH_STATE *matches;
static REFERENCE **refs;
static size_t match_index;
static int ref_index;

/* Number of screen columns output so far in a line. */
static int pl_chars;

/* Whether we are currently outputting a highlighted reference.  This can be 
   carried over from one line to another. */
static int ref_highlighted;

static int pl_num; /* Number of printed lines done so far. */

RENDITION ref_rendition = {UNDERLINE_MASK, UNDERLINE_MASK};
RENDITION hl_ref_rendition = {UNDERLINE_MASK, 0};
RENDITION match_rendition = {STANDOUT_MASK, STANDOUT_MASK};


/* Process a line from the node in WIN starting at ITER, and advancing ITER
   to the end of the line.  What is done with the line depends on the value
   of WRITING_OUT.
   If the line ends in a newline character, set *DELIM to 1. */
static void
display_process_line (WINDOW *win,
                 mbi_iterator_t *iter_inout,
                 struct text_buffer *tb_printed_line,
                 int *delim)
{
  mbi_iterator_t iter;
  const char *cur_ptr;
  size_t pchars = 0; /* Printed chars */
  size_t pbytes = 0; /* Bytes to output. */
  char *rep;
  int in_match = 0;
  int in_ref = 0, in_ref_proper = 0;
  RENDITION empty = {0, 0};

  int point_in_line;

  if (win->point >= win->line_starts[win->pagetop + pl_num]
      && win->point < win->line_starts[win->pagetop + pl_num + 1])
    point_in_line = 1;
  else
    point_in_line = 0;

  iter = *iter_inout;

  while (1)
    {
      int was_in_ref_proper = in_ref_proper;
      int was_in_match = in_match;

      if (!mbi_avail (iter))
        break;
      cur_ptr = mbi_cur_ptr (iter);

      if (matches && matches_ready (matches)
          && !at_end_of_matches (matches, match_index))
        {
          int was_in_match = in_match;
          decide_if_in_match (cur_ptr - win->node->contents,
                              &in_match, matches, &match_index);

          if (!was_in_match && in_match && writing_out == DEFAULT)
            writing_out = COLLECT;
        }

      if (refs && refs[ref_index])
        {
          int was_in_ref = in_ref;
          decide_if_in_reference (cur_ptr - win->node->contents,
                                  &in_ref, refs, &ref_index);

          if (was_in_ref && !in_ref)
            {
              in_ref_proper = ref_highlighted = 0;
            }
          else if (!was_in_ref && in_ref)
            {
              if (writing_out == DEFAULT)
                writing_out = COLLECT;

              /* Decide if this reference should be highlighted. */
              if (point_in_line && win->point < refs[ref_index]->end)
                {
                  /* The reference in is the part of the line after
                     the cursor, or the reference contains the cursor. */
                  point_in_line = 0;
                  ref_highlighted = 1;
                }
              else if (point_in_line
                       && (!refs[ref_index + 1]
                           || refs[ref_index + 1]->start
                              >= win->line_starts[win->pagetop + pl_num + 1]))
                {
                  /* The reference label is before the cursor in
                     the current line and none occurs after it in
                     the current line. */
                  point_in_line = 0;
                  ref_highlighted = 1;
                }
              else if (win->point >= refs[ref_index]->start
                       && win->point < refs[ref_index]->end)
                {
                  /* The point is in a cross-reference, but not in the 
                     current line. */
                  ref_highlighted = 1;
                }
              else if (win->point >= win->line_starts
                                                  [win->pagetop + pl_num + 1]
                       && win->point < win->line_starts
                                                    [win->pagetop + pl_num + 2]
                       && refs[ref_index]->end
                          >= win->line_starts[win->pagetop + pl_num + 1]
                       && (!refs[ref_index + 1]
                           || refs[ref_index + 1]->start
                              >= win->line_starts[win->pagetop + pl_num + 2]))
                {
                  /* Point is in the next line, not inside this reference,
                     but this reference continues onto the next line and
                     no other reference follows it in the line. */
                  ref_highlighted = 1;
                }
            }
        }

      if (in_ref && !in_ref_proper && !strchr (" \t", *cur_ptr))
        in_ref_proper = 1;

      if (was_in_ref_proper != in_ref_proper || was_in_match != in_match)
        {
          /* Calculate the new rendition for output characters, and call
             the function to switch to it. */
          RENDITION ref = {0, 0};
          RENDITION match = {0, 0};

          if (in_ref_proper)
            ref = ref_highlighted && hl_ref_rendition.mask
                    ? hl_ref_rendition : ref_rendition;
          if (in_match)
            match = match_rendition;
          if (!ref_highlighted)
            {
              wrap_terminal_switch_rendition (tb_printed_line,
                                              ref, match, empty);
            }
          else
            {
              wrap_terminal_switch_rendition (tb_printed_line,
                                              match, ref, empty);
            }
        }

      rep = printed_representation (&iter, delim, pl_chars,
                                    &pchars, &pbytes);

      /* If a newline character has been seen, or we have reached the
         edge of the display.  */
      if (*delim || pl_chars + pchars >= win->width)
        break;

      if (rep)
        {
          if (writing_out != WRITEOUT)
            text_buffer_add_string (tb_printed_line, rep, pbytes);
          else
            terminal_write_chars (rep, pbytes);

          pl_chars += pchars;
        }
      mbi_advance (iter);
    }

  if (writing_out != DEFAULT)
    wrap_terminal_switch_rendition (tb_printed_line, empty, empty, empty);

  *iter_inout = iter;
}

/* Update the part of WIN containing text from a node, i.e. not the blank
   part at the end or a modeline.
   Print each line in the window into our local buffer, and then
   check the contents of that buffer against the display.  If they
   differ, update the display.  Return number of lines printed. */
int
display_update_node_text (WINDOW *win)
{
  static struct text_buffer tb_printed_line;  /* Buffer for a printed line.  */

  mbi_iterator_t iter;  /* Used to iterate through part of node displayed.  */
  mbi_iterator_t bol_iter; /* Keep reference to beginning of each line.  */
  int bol_ref_index = 0, bol_match_index = 0;
  int bol_ref_highlighted;

  int finish;

  matches = 0;
  refs = 0;
  if (match_rendition.mask)
    matches = &win->matches;
  if (ref_rendition.mask || hl_ref_rendition.mask)
    refs = win->node->references;

  pl_num = 0;

  ref_highlighted = 0;

  writing_out = DEFAULT; /* Global variable, declared above. */
  ref_index = match_index = 0;

  mbi_init (iter, win->node->contents + win->line_starts[win->pagetop],
            win->node->nodelen - win->line_starts[win->pagetop]);
  mbi_avail (iter);
  while (1)
    {
      int delim;
      mbi_copy (&bol_iter, &iter);
      bol_ref_index = ref_index;
      bol_match_index = match_index;
      bol_ref_highlighted = ref_highlighted;

      /* Come back here at end of line when write_out == COLLECT */
start_of_line:
      pl_chars = 0;

      text_buffer_reset (&tb_printed_line);

      delim = 0;
      /* Check if we have processed all the lines in the window. */
      if (pl_num == win->height)
        break;

      /* Check if this line of the window is off the screen.  This might 
         happen if the screen was resized very small. */
      if (win->first_row + pl_num >= screenheight)
        break;

      display_process_line (win, &iter, &tb_printed_line, &delim);

      /* End of printed line. */
      text_buffer_add_char (&tb_printed_line, '\0');

      finish = 0;
      /* If there are no highlighted regions in a line, we output the line with
         display_update_line, which does some optimization of the redisplay.
         Otherwise, the entire line is output in this function. */
      if (writing_out == DEFAULT)
        {
          finish = display_update_line (win->first_row + pl_num,
                      text_buffer_base (&tb_printed_line),
                      text_buffer_off (&tb_printed_line) - 1,
                      pl_chars);
        }
      else if (writing_out == COLLECT)
        {
          /* Check if the line differs from what is already on the display,
             and if so, go back to the start of the line and display it for
             real. */
          DISPLAY_LINE *entry = the_display[win->first_row + pl_num];
          if (strcmp (tb_printed_line.base,
                      the_display[win->first_row + pl_num]->text))
            {
              if (tb_printed_line.off > screenwidth)
                {
                  entry->text = xrealloc (entry->text,
                                          tb_printed_line.off + 1);
                }
              strcpy (entry->text, tb_printed_line.base);
              /* Record that the contents of this DISPLAY_LINE isn't
                 literally what is on the display. */
              entry->textlen = 0;
              entry->inverse = 1;
              mbi_copy (&iter, &bol_iter);
              mbi_avail (bol_iter);
              ref_index = bol_ref_index;
              match_index = bol_match_index;
              terminal_goto_xy (0, win->first_row + pl_num);
              ref_highlighted = bol_ref_highlighted;
              writing_out = WRITEOUT;
              goto start_of_line;
            }
          else
            writing_out = DEFAULT;
        }
      else /* writing_out == WRITEOUT */
        {
          /* We have just written out this line to the display. */
          terminal_clear_to_eol ();
          writing_out = DEFAULT;
        }

      /* Check if a line continuation character should be displayed.
         Don't print one on the very last line of the display, as this could 
         cause it to scroll. */
      if (delim)
        mbi_advance (iter);
      else if (win->first_row + pl_num <= the_screen->height - 2)
        {
          terminal_goto_xy (win->width - 1, win->first_row + pl_num);

          if (!(win->flags & W_NoWrap))
            terminal_put_text ("\\");
          else
            {
              terminal_put_text ("$");

              /* If this window has chosen not to wrap lines, skip to the
                 end of the logical line in the buffer, and start a new
                 line here. */
              for (; mbi_avail (iter); mbi_advance (iter))
                if (mb_len (mbi_cur (iter)) == 1
                    && *mbi_cur_ptr (iter) == '\n')
                  {
                    mbi_advance (iter);
                    break;
                  }
            }
          fflush (stdout);
        }

      pl_num++;
      if (finish)
        break; /* Display was interrupted by typed input. */

      if (!mbi_avail (iter))
        break;
    }

  /* Unlike search match highlighting, we always turn reference highlighting
     off at the end of each line, so the following isn't needed. */
  /* terminal_end_underline (); */

  return pl_num;
}
#undef DEFAULT
#undef COLLECT
#undef WRITEOUT /* values for writing_out global */

/* Update one window on the screen. */
void
display_update_one_window (WINDOW *win)
{
  size_t line_index = 0;
  DISPLAY_LINE **display = the_display;

  signal_block_winch ();

  /* If display is inhibited, that counts as an interrupted display. */
  if (display_inhibited)
    {
      display_was_interrupted_p = 1;
      goto funexit;
    }

  /* If the window has no height, quit now.  Strictly speaking, it
     should only be necessary to test if the values are equal to zero, since
     window_new_screen_size should ensure that the window height/width never
     becomes negative, but since historically this has often been the culprit
     for crashes, do our best to be doubly safe.  */
  if (win->height <= 0 || win->width <= 0)
    goto funexit;

  /* If the window's first row doesn't appear in the_screen, then it
     cannot be displayed.  This can happen when the_echo_area is the
     window to be displayed, and the screen has shrunk to less than one
     line. */
  if ((win->first_row < 0) || (win->first_row > the_screen->height))
    goto funexit;

  /* If this window has a modeline, it might need to be redisplayed.  Do
     this before the rest of the window to aid in navigation in case the
     rest of the window is slow to update (for example, if it has lots of
     search matches to be displayed). */
  if (!(win->flags & W_InhibitMode))
    {
      window_make_modeline (win);
      line_index = win->first_row + win->height;

      /* This display line must both be in inverse, and have the same
         contents. */
      if ((!display[line_index]->inverse
           || (strcmp (display[line_index]->text, win->modeline) != 0))
          /* Check screen isn't very small. */
          && line_index < the_screen->height)
        {
          terminal_goto_xy (0, line_index);
          terminal_begin_inverse ();
          terminal_put_text (win->modeline);
          terminal_end_inverse ();
          strcpy (display[line_index]->text, win->modeline);
          display[line_index]->inverse = 1;
          display[line_index]->textlen = strlen (win->modeline);
        }
    }

  if (win->node)
    {
      if (!win->line_starts)
        calculate_line_starts (win);
      line_index = display_update_node_text (win);

      if (display_was_interrupted_p)
	goto funexit;
    }

  /* We have reached the end of the node or the end of the window.  If it
     is the end of the node, then clear the lines of the window from here
     to the end of the window. */
  for (; line_index < win->height; line_index++)
    {
      DISPLAY_LINE *entry = display[win->first_row + line_index];

      /* If this line has text on it, or if we don't know what is on the line,
         clear this line. */
      if (entry->textlen || entry->inverse)
        {
          entry->textlen = 0;
          entry->text[0] = '\0';
          entry->inverse = 0;

          terminal_goto_xy (0, win->first_row + line_index);
          terminal_clear_to_eol ();
          fflush (stdout);

          if (info_any_buffered_input_p ())
            {
              display_was_interrupted_p = 1;
              goto funexit;
            }
        }
    }

  fflush (stdout);

  /* Okay, this window doesn't need updating anymore. */
  win->flags &= ~W_UpdateWindow;
funexit:
  signal_unblock_winch ();
}

/* Scroll screen lines from START inclusive to END exclusive down
   by AMOUNT lines.  Negative AMOUNT means move them up. */
static void
display_scroll_region (int start, int end, int amount)
{
  int i;
  DISPLAY_LINE *temp;

  /* Do it on the screen. */
  terminal_scroll_region (start, end, amount);

  /* Now do it in the display buffer so our contents match the screen. */
  if (amount > 0)
    {
      for (i = end - 1; i >= start + amount; i--)
        {
          /* Swap rows i and (i - amount). */
          temp = the_display[i];
          the_display[i] = the_display[i - amount];
          the_display[i - amount] = temp;
        }

      /* Clear vacated lines */
      for (i = start; i < start + amount && i < end; i++)
        {
          the_display[i]->text[0] = '\0';
          the_display[i]->textlen = 0;
          the_display[i]->inverse = 0;
        }
    }
  else
    {
      amount *= -1;
      for (i = start; i <= end - 1 - amount; i++)
        {
          /* Swap rows i and (i + amount). */
          temp = the_display[i];
          the_display[i] = the_display[i + amount];
          the_display[i + amount] = temp;
        }

      /* Clear vacated lines */
      for (i = end - 1; i >= end - amount && i >= start; i--)
        {
          the_display[i]->text[0] = '\0';
          the_display[i]->textlen = 0;
          the_display[i]->inverse = 0;
        }
    }
}

/* Scroll the region of the_display starting at START, ending at END, and
   moving the lines AMOUNT lines.  If AMOUNT is less than zero, the lines
   are moved up in the screen, otherwise down.  Actually, it is possible
   for no scrolling to take place in the case that the terminal doesn't
   support it.  This doesn't matter to us. */
void
display_scroll_display (int start, int end, int amount)
{
  register int i, last;
  DISPLAY_LINE *temp;

  /* If this terminal cannot do scrolling, give up now. */
  if (!terminal_can_scroll && !terminal_can_scroll_region)
    return;

  /* If there isn't anything displayed on the screen because it is too
     small, quit now. */
  if (!the_display[0])
    return;

  /* If there is typeahead pending, then don't actually do any scrolling. */
  if (info_any_buffered_input_p ())
    return;

  /* Use scrolling region if we can because it doesn't affect the area
     below the area we want to scroll. */
  if (terminal_can_scroll_region)
    {
      display_scroll_region (start, end, amount);
      return;
    }

  /* Otherwise scroll by deleting and inserting lines. */

  if (amount < 0)
    start -= amount;
  else
    end -= amount;

  /* Do it on the screen. */
  terminal_scroll_terminal (start, end, amount);

  /* Now do it in the display buffer so our contents match the screen. */
  if (amount > 0)
    {
      last = end + amount;

      /* Shift the lines to scroll right into place. */
      for (i = 1; i <= (end - start); i++)
        {
          temp = the_display[last - i];
          the_display[last - i] = the_display[end - i];
          the_display[end - i] = temp;
        }

      /* The lines have been shifted down in the buffer.  Clear all of the
         lines that were vacated. */
      for (i = start; i != (start + amount); i++)
        {
          the_display[i]->text[0] = '\0';
          the_display[i]->textlen = 0;
          the_display[i]->inverse = 0;
        }
    }
  else
    {
      last = start + amount;
      for (i = 0; i < (end - start); i++)
        {
          temp = the_display[last + i];
          the_display[last + i] = the_display[start + i];
          the_display[start + i] = temp;
        }

      /* The lines have been shifted up in the buffer.  Clear all of the
         lines that are left over. */
      for (i = end + amount; i != end; i++)
        {
          the_display[i]->text[0] = '\0';
          the_display[i]->textlen = 0;
          the_display[i]->inverse = 0;
        }
    }
}

/* Try to scroll lines in WINDOW.  OLD_PAGETOP is the pagetop of WINDOW before
   having had its line starts recalculated.  OLD_STARTS is the list of line
   starts that used to appear in this window.  OLD_COUNT is the number of lines
   that appear in the OLD_STARTS array. */
void
display_scroll_line_starts (WINDOW *window, int old_pagetop,
    long *old_starts, int old_count)
{
  register int i, old, new;     /* Indices into the line starts arrays. */
  int last_new, last_old;       /* Index of the last visible line. */
  int old_first, new_first;     /* Index of the first changed line. */
  int unchanged_at_top = 0;
  int already_scrolled = 0;

  /* Locate the first line which was displayed on the old window. */
  old_first = old_pagetop;
  new_first = window->pagetop;

  /* Find the last line currently visible in this window. */
  last_new = window->pagetop + (window->height - 1);
  if (last_new > window->line_count)
    last_new = window->line_count - 1;

  /* Find the last line which used to be currently visible in this window. */
  last_old = old_pagetop + (window->height - 1);
  if (last_old > old_count)
    last_old = old_count - 1;

  for (old = old_first, new = new_first;
       old < last_old && new < last_new;
       old++, new++)
    if (old_starts[old] != window->line_starts[new])
      break;
    else
      unchanged_at_top++;

  /* Loop through the old lines looking for a match in the new lines. */
  for (old = old_first + unchanged_at_top; old < last_old; old++)
    {
      for (new = new_first; new < last_new; new++)
        if (old_starts[old] == window->line_starts[new])
          {
            /* Find the extent of the matching lines. */
            for (i = 0; (old + i) < last_old; i++)
              if (old_starts[old + i] != window->line_starts[new + i])
                break;

            /* Scroll these lines if there are enough of them. */
            {
              int start, end, amount;

              start = (window->first_row
                       + ((old + already_scrolled) - old_pagetop));
              amount = new - (old + already_scrolled);
              end = window->first_row + window->height;

              /* If we are shifting the block of lines down, then the last
                 AMOUNT lines will become invisible.  Thus, don't bother
                 scrolling them. */
              if (amount > 0)
                end -= amount;

              if ((end - start) > 0)
                {
                  display_scroll_display (start, end, amount);

                  /* Some lines have been scrolled.  Simulate the scrolling
                     by offsetting the value of the old index. */
                  old += i;
                  already_scrolled += amount;
                }
            }
          }
    }
}

/* Move the screen cursor to directly over the current character in WINDOW. */
void
display_cursor_at_point (WINDOW *window)
{
  int vpos, hpos;

  vpos = window_line_of_point (window) - window->pagetop + window->first_row;
  hpos = window_get_cursor_column (window);
  terminal_goto_xy (hpos, vpos);
  fflush (stdout);
}

/* **************************************************************** */
/*                                                                  */
/*                   Functions Static to this File                  */
/*                                                                  */
/* **************************************************************** */

/* Make a DISPLAY_LINE ** with width and height. */
static DISPLAY_LINE **
make_display (int width, int height)
{
  register int i;
  DISPLAY_LINE **display;

  display = xmalloc ((1 + height) * sizeof (DISPLAY_LINE *));

  for (i = 0; i < height; i++)
    {
      display[i] = xmalloc (sizeof (DISPLAY_LINE));
      display[i]->text = xmalloc (1 + width);
      display[i]->textlen = 0;
      display[i]->inverse = 0;
    }
  display[i] = NULL;
  return display;
}

/* Free the storage allocated to DISPLAY. */
static void
free_display (DISPLAY_LINE **display)
{
  register int i;
  register DISPLAY_LINE *display_line;

  if (!display)
    return;

  for (i = 0; (display_line = display[i]); i++)
    {
      free (display_line->text);
      free (display_line);
    }
  free (display);
}

