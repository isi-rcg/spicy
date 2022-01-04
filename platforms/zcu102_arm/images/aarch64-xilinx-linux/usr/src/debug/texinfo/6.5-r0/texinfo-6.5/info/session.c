/* session.c -- user windowing interface to Info.
   $Id: session.c 7907 2017-07-05 19:16:44Z gavin $

   Copyright 1993, 1996, 1997, 1998, 1999, 2000, 2001, 2002, 2003,
   2004, 2007, 2008, 2009, 2011, 2012, 2013, 2014, 2015, 2016, 2017
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
#include "display.h"
#include "session.h"
#include "echo-area.h"
#include "search.h"
#include "footnotes.h"
#include "man.h"
#include "variables.h"

#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#ifdef __MINGW32__
# define read(f,b,s)	w32_read(f,b,s)
# define _read(f,b,s)	w32_read(f,b,s)
extern ssize_t w32_read (int, void *, size_t);
#endif

#if defined (HAVE_SYS_TIME_H)
#  include <sys/time.h>
#  define HAVE_STRUCT_TIMEVAL
#endif /* HAVE_SYS_TIME_H */

/* **************************************************************** */
/*                                                                  */
/*                   Running an Info Session                        */
/*                                                                  */
/* **************************************************************** */

static void mouse_event_handler (void);

/* The place that we are reading input from. */
static FILE *info_input_stream = NULL;

/* Becomes non-zero when 'q' is typed to an Info window. */
static int quit_info_immediately = 0;

static NODE *allfiles_node = 0;

DECLARE_INFO_COMMAND (info_all_files, _("Show all matching files"))
{
  if (!allfiles_node)
    {
      info_error (_("No file index"));
      return;
    }

  /* FIXME: Copy allfiles_node so it is unique in the window history? */
  info_set_node_of_window (window, allfiles_node);
}

static void
allfiles_create_node (char *term, REFERENCE **fref)
{
  int i;
  struct text_buffer text;
  
  text_buffer_init (&text);

  text_buffer_printf (&text,
                      "%s File names matching '%s'\n\n"
                      "Info File Index\n"
                      "***************\n\n"
                      "File names that match '%s':\n",
                      INFO_NODE_LABEL,
                      term, term);

  /* Mark as an index so that destinations are never hidden. */
  text_buffer_add_string (&text, "\0\b[index\0\b]", 11);
  text_buffer_printf (&text, "\n* Menu:\n\n");

  for (i = 0; fref[i]; i++)
    {
      text_buffer_printf (&text, "* %4i: (%s)", i+1, fref[i]->filename);
      if (fref[i]->nodename)
        text_buffer_printf (&text, "%s", fref[i]->nodename);
      text_buffer_printf (&text, ".\n");
    }

  allfiles_node = info_create_node ();
  allfiles_node->fullpath = xstrdup ("");
  allfiles_node->nodename = xstrdup ("*Info File Index*");
  allfiles_node->contents = text_buffer_base (&text);
  allfiles_node->nodelen = text_buffer_off (&text);
  allfiles_node->body_start = strcspn (allfiles_node->contents, "\n");

  scan_node_contents (allfiles_node, 0, 0);
}

/* Begin an info session finding the nodes specified by REFERENCES.  For
   each loaded node, create a new window.  Always split the largest of the
   available windows.  Display ERROR in echo area if non-null. */
static void
begin_multiple_window_info_session (REFERENCE **references, char *error)
{
  register int i;
  WINDOW *window = 0;

  for (i = 0; references && references[i]; i++)
    {
      if (!window)
        {
          window = active_window;
          info_select_reference (window, references[i]);
          if (!window->node)
            window = 0;
        }
      else
        {
          /* Find the largest window in WINDOWS, and make that be the active
             one.  Then split it and add our window and node to the list
             of remembered windows and nodes.  Then tile the windows. */
          WINDOW *win, *largest = NULL;
          int max_height = 0;

          for (win = windows; win; win = win->next)
            if (win->height > max_height)
              {
                max_height = win->height;
                largest = win;
              }

          if (!largest)
            {
              display_update_display ();
              info_error ("%s", msg_cant_find_window);
              return;
            }

          active_window = largest;
          window = window_make_window ();
          info_select_reference (window, references[i]);

          if (!window->node)
            {
              /* We couldn't find the node referenced. */
              window_delete_window (window);
              window = 0;
            }

          if (window)
            window_tile_windows (TILE_INTERNALS);
          else
            {
              display_update_display ();
              info_error ("%s", msg_win_too_small);
              return;
            }
        }
    }

  /* Load dir node as a back-up if there were no references given, or if
     none of them were valid. */
  if (!window)
    {
      info_set_node_of_window (active_window, get_dir_node ());
      return;
    }
}

static void
display_startup_message (void)
{
  char *format;

  format = replace_in_documentation
  /* TRANSLATORS: Try to keep this message (when "expanded") at most 79 
     characters; anything after the 79th character will not actually be 
     displayed on an 80-column terminal. */
(_("Welcome to Info version %s.  Type \\[get-help-window] for help, \\[get-info-help-node] for tutorial."),
     0);

  window_message_in_echo_area (format, VERSION, NULL);
}

/* Run an Info session.  If USER_FILENAME is null, create a window for each
   node referenced in REF_LIST.  If USER_FILENAME is not null, "--all" was
   used on the command line, and display a file index with entries in
   REF_LIST.  ERROR is an optional error message to display at start-up. */
void
info_session (REFERENCE **ref_list, char *user_filename, char *error)
{
  /* Initialize the Info session. */
  initialize_info_session ();

  if (!error)
    display_startup_message ();
  else
    show_error_node (error);

  if (!user_filename)
    begin_multiple_window_info_session (ref_list, error);
  else
    {
      allfiles_create_node (user_filename, ref_list);
      info_set_node_of_window (active_window, allfiles_node);
    }

  info_read_and_dispatch ();
  close_info_session ();
}

void info_next_line (WINDOW *, int count);
void info_prev_line (WINDOW *, int count);
static int info_keyseq_displayed_p;

void
info_read_and_dispatch (void)
{
  VFunction *cmd;
  int count;

  for (quit_info_immediately = 0; !quit_info_immediately; )
    {
      if (!info_any_buffered_input_p ())
        display_update_display ();

      /* Some redisplay might be necessary if the cursor has moved and
         a different reference (or no reference) has to be highlighted. */
      if (hl_ref_rendition.mask)
        display_update_one_window (active_window);

      display_cursor_at_point (active_window);

      cmd = read_key_sequence (info_keymap, 1, 1, 0, &count);
      if (cmd)
        {
          if (!info_keyseq_displayed_p)
            window_clear_echo_area ();

          (*cmd) (active_window, count);

          /* Don't change the goal column when going up and down.  This 
             means we can go from a long line to a short line and back to
             a long line and end back in the same column. */
          if (!(cmd == &info_next_line || cmd == &info_prev_line))
            active_window->goal_column = -1; /* Goal is current column. */
        }
    }
}

/* Found in signals.c */
extern void initialize_info_signal_handler (void );

/* Initialize terminal, read configuration file and set key bindings. */
void
initialize_terminal_and_keymaps (char *init_file)
{
  char *term_name = getenv ("TERM");
  terminal_initialize_terminal (term_name);
  read_init_file (init_file);
}

/* Initialize the first info session by starting the terminal, window,
   and display systems.  */
void
initialize_info_session (void)
{
  if (!terminal_prep_terminal ())
    {
      /* Terminal too dumb to run interactively. */
      char *term_name = getenv ("TERM");
      info_error (msg_term_too_dumb, term_name);
      exit (EXIT_FAILURE);
    }

  terminal_clear_screen ();

  window_initialize_windows (screenwidth, screenheight);
  initialize_info_signal_handler ();
  display_initialize_display (screenwidth, screenheight);

  /* If input has not been redirected yet, make it come from unbuffered
     standard input. */
  if (!info_input_stream)
    {
      setbuf (stdin, NULL);
      info_input_stream = stdin;
    }

  info_windows_initialized_p = 1;
}

/* On program exit, leave the cursor at the bottom of the window, and
   restore the terminal I/O. */
void
close_info_session (void)
{
  terminal_goto_xy (0, screenheight - 1);
  terminal_clear_to_eol ();
  fflush (stdout);
  terminal_unprep_terminal ();
  close_dribble_file ();
}

/* Tell Info that input is coming from the file FILENAME. */
void
info_set_input_from_file (char *filename)
{
  FILE *stream;

  /* Input may include binary characters.  */
  stream = fopen (filename, FOPEN_RBIN);

  if (!stream)
    return;

  if ((info_input_stream != NULL) &&
      (info_input_stream != stdin))
    fclose (info_input_stream);

  info_input_stream = stream;

  if (stream != stdin)
    display_inhibited = 1;
}


/* **************************************************************** */
/*                                                                  */
/*                      Input Character Buffering                   */
/*                                                                  */
/* **************************************************************** */

static void fill_input_buffer (int wait);
static int info_gather_typeahead (int);

/* Largest number of characters that we can read in advance. */
#define MAX_INFO_INPUT_BUFFERING 512

static int pop_index = 0; /* Where to remove bytes from input buffer. */
static int push_index = 0; /* Where to add bytes to input buffer. */
static unsigned char info_input_buffer[MAX_INFO_INPUT_BUFFERING];

/* Get a key from the buffer of characters to be read.
   Return the key in KEY.
   Result is non-zero if there was a key, or 0 if there wasn't. */
static int
get_byte_from_input_buffer (unsigned char *key)
{
  if (push_index == pop_index)
    return 0;

  *key = info_input_buffer[pop_index++];

  if (pop_index >= MAX_INFO_INPUT_BUFFERING)
    pop_index = 0;

  return 1;
}

int
info_any_buffered_input_p (void)
{
  fill_input_buffer (0);
  return push_index != pop_index;
}

/* Wrapper around info_gather_typeahead which handles read errors and reaching
   end-of-file. */
static void
fill_input_buffer (int wait)
{
  while (1)
    {
      int success;
      do
        {
          success = info_gather_typeahead (wait);
        }
      while (!success && errno == EINTR); /* Try again if the read was
                                             interrupted due to a signal. */
      if (success || !wait)
        return;

      /* Reading failed.  If we were reading from a dribble file with
         --restore, switch to standard input.  Otherwise quit. */
      if (info_input_stream != stdin)
        {
          fclose (info_input_stream);
          info_input_stream = stdin;
          display_inhibited = 0;
          display_update_display ();
          display_cursor_at_point (active_window);
        }
      else
        {
          close_info_session ();
          exit (EXIT_SUCCESS);
        }
    }
}

/* Read bytes and stuff them into info_input_buffer.  If WAIT is true, wait
   for input; otherwise don't do anything if there is no input waiting.
   Return 1 on success, 0 on error.  ERRNO may be set by read(). */
static int
info_gather_typeahead (int wait)
{
  register int i = 0;
  int tty, space_avail;
  long chars_avail;
  unsigned char input[MAX_INFO_INPUT_BUFFERING];

  tty = fileno (info_input_stream);
  chars_avail = 0;

  /* Clear errno. */
  errno = 0;

  /* There may be characters left over from last time, in which case we don't
     want to wait for another key to be pressed. */
  if (wait && pop_index == push_index)
    {
      char c;
      /* Wait until there is a byte waiting, and then stuff it into the input
         buffer. */
      if (read (tty, &c, 1) <= 0) 
        return 0;

      if (info_dribble_file)
        dribble (c);

      info_input_buffer[push_index++] = c;
      if (push_index >= MAX_INFO_INPUT_BUFFERING)
        push_index = 0;
      /* Continue to see if there are more bytes waiting. */
    }

  /* Get the amount of space available in INFO_INPUT_BUFFER for new chars. */
  if (pop_index > push_index)
    space_avail = pop_index - push_index;
  else
    space_avail = sizeof (info_input_buffer) - (push_index - pop_index);

  /* If we can just find out how many characters there are to read, do so. */
#if defined (FIONREAD)
  {
    ioctl (tty, FIONREAD, &chars_avail);

    if (chars_avail > space_avail)
      chars_avail = space_avail;

    if (chars_avail)
      chars_avail = read (tty, &input[0], chars_avail);
  }
#else /* !FIONREAD */
#  if defined (O_NDELAY) && defined (F_GETFL) && defined (F_SETFL)
  {
    int flags;

    flags = fcntl (tty, F_GETFL, 0);

    fcntl (tty, F_SETFL, (flags | O_NDELAY));
      chars_avail = read (tty, &input[0], space_avail);
    fcntl (tty, F_SETFL, flags);

    if (chars_avail == -1)
      chars_avail = 0;
  }
#  else  /* !O_NDELAY */
#   ifdef __DJGPP__
  {
    extern long pc_term_chars_avail (void);

    if (isatty (tty))
      chars_avail = pc_term_chars_avail ();
    else
      {
        /* We could be more accurate by calling ltell, but we have no idea
           whether tty is buffered by stdio functions, and if so, how many
           characters are already waiting in the buffer.  So we punt.  */
        struct stat st;

        if (fstat (tty, &st) < 0)
          chars_avail = 1;
        else
          chars_avail = st.st_size;
      }
    if (chars_avail > space_avail)
      chars_avail = space_avail;
    if (chars_avail)
      chars_avail = read (tty, &input[0], chars_avail);
  }
#   else
#    ifdef __MINGW32__
  {
    extern long w32_chars_avail (int);

    chars_avail = w32_chars_avail (tty);

    if (chars_avail > space_avail)
      chars_avail = space_avail;
    if (chars_avail)
      chars_avail = read (tty, &input[0], chars_avail);
  }
#    endif  /* _WIN32 */
#   endif/* __DJGPP__ */
#  endif /* O_NDELAY */
#endif /* !FIONREAD */

  while (i < chars_avail)
    {
      if (info_dribble_file)
        dribble (input[i]);

      /* Add KEY to the buffer of characters to be read. */
      if (input[i] != Control ('g'))
        {
          info_input_buffer[push_index++] = input[i];
          if (push_index >= MAX_INFO_INPUT_BUFFERING)
            push_index = 0;
        }
      else
        /* Flush all pending input in the case of C-g pressed. */
        push_index = pop_index;
      i++;
    }
  /* If wait is true, there is at least one byte left in the input buffer. */
  if (chars_avail <= 0 && !wait)
    return 0;
  return 1;
}

static int get_input_key_internal (void);

static void _scroll_forward (WINDOW *window, int count, int nodeonly);
static void _scroll_backward (WINDOW *window, int count, int nodeonly);

/* Whether to process or skip mouse events in the input stream. */
unsigned char mouse_cb, mouse_cx, mouse_cy;

/* Handle mouse event given that mouse_cb, mouse_cx and mouse_cy contain the
   data from the event.  See the "XTerm Control Sequences" document for their
   meanings. */
void
mouse_event_handler (void)
{
  if (mouse_cb & 0x40)
    {
      switch (mouse_cb & 0x03)
        {
        case 0: /* Mouse button 4 (scroll up). */
          _scroll_backward (active_window, 3, 1);
          break;
        case 1: /* Mouse button 5 (scroll down). */
          _scroll_forward (active_window, 3, 1);
          break;
        }
    }
}

/* Return number representing a key that has been pressed, which is an index
   into info_keymap and echo_area_keymap. */
int
get_input_key (void)
{
  int ret = -1;
  
  while (ret == -1)
    {
      ret = get_input_key_internal ();

      if (ret == KEY_MOUSE)
        {
          get_byte_from_input_buffer (&mouse_cb);
          get_byte_from_input_buffer (&mouse_cx);
          get_byte_from_input_buffer (&mouse_cy);
        }
    }
  return ret;
}

/* Time in milliseconds to wait for the next byte of a byte sequence
   corresponding to a key or key chord.  Settable with the 'key-time' user
   variable. */
int key_time = 100;

/* Read bytes from input and return what key has been pressed.  Return -1 on
   reading an unrecognized key. */
static int
get_input_key_internal (void)
{
  BYTEMAP_ENTRY *b;
  unsigned char c;
  int esc_seen = 0;
  int pop_start;
  int byte_count = 0;
  fill_input_buffer (1);

  if (pop_index == push_index)
    return -1; /* No input waiting.  This shouldn't happen. */

  b = byte_seq_to_key;
  pop_start = pop_index;

  while (pop_index != push_index)
    {
      int in_map = 0;
      int unknown = 0;
      if (!get_byte_from_input_buffer (&c))
        break; /* Incomplete byte sequence. */
      byte_count++;

      switch (b[c].type)
        {
        case BYTEMAP_KEY:
          return b[c].key;
        case BYTEMAP_ESC:
          esc_seen = 1;
          /* Fall through. */
        case BYTEMAP_MAP:
          in_map = 1;
          b = b[c].next;
          break;
        case BYTEMAP_NONE:
          unknown = 1;
          break;
        }

      if (unknown)
        break;

      /* If we read an incomplete byte sequence, pause a short while to
         see if more bytes follow. */
      if (in_map && pop_index == push_index)
        {
          int ready = 0;
#if defined (FD_SET)
          struct timeval timer, *timerp = 0;
          fd_set readfds;

          FD_ZERO (&readfds);
          FD_SET (fileno (info_input_stream), &readfds);

          timer.tv_sec = 0;
          timer.tv_usec = key_time * 1000;
          timerp = &timer;
          ready = select (fileno(info_input_stream)+1, &readfds,
                          NULL, NULL, timerp);
#else
          ready = 1;
#endif /* FD_SET */
          if (ready)
            fill_input_buffer (0);
        }
    }

  /* Incomplete or unknown byte sequence. Start again with the first byte.  */
  pop_index = pop_start;

  if (!esc_seen || (byte_count >= 3 && key_time == 0))
    {
      /* If the sequence was incomplete, return the first byte.
             Also return the first byte for sequences with ESC that are at
         least three bytes long if 'key_time' is 0, to give some support for 
         specifying byte sequences in .infokey for those sent by unrecognized
         special keys (which would otherwise be skipped below). */
      pop_index = pop_start;
      get_byte_from_input_buffer (&c);
      return c;
    }
  else
    {
      get_byte_from_input_buffer (&c); /* Should be ESC */

      /* If there are no more characters, then decide that the escape key
         itself has been pressed. */
      if (pop_index == push_index)
        return 033;

      /* Skip byte sequences that look like they could have come from
         unrecognized keys, e.g. F3 or C-S-Left, to avoid them as being
         interpreted as random garbage.  These might produce sequences
         that look like "ESC O R" or "ESC [ 1 ; 6 ~", depending on
         the terminal. */

      /* Check if the sequence starts ESC O. */
      get_byte_from_input_buffer (&c);
      if (c == 'O')
        {
          /* If no more bytes, call it M-O. */
          if (!info_any_buffered_input_p ())
            return 'O' + KEYMAP_META_BASE;

          /* Otherwise it could be an unrecognized key producing a sequence
             ESC O (byte).  Ignore it, discarding the next byte. */
          get_byte_from_input_buffer (&c);
          return -1;
        }

      /* Unknown CSI-style sequences. */
      else if (c == '[')
        {
          /* If no more bytes, call it M-[. */
          if (!get_byte_from_input_buffer (&c))
            return '[' + KEYMAP_META_BASE;

          /* Skip a control sequence as defined by ECMA-48. */
          while (c >= 0x30 && c <= 0x3f)
            if (!get_byte_from_input_buffer (&c))
              break;

          while (c >= 0x20 && c <= 0x2f)
            if (!get_byte_from_input_buffer (&c))
              break;

          return -1;
        }

      else
        {
          /* The sequence started with ESC, but wasn't recognized.  Treat it
             as introducing a sequence produced by a key chord with the meta
             key pressed. */
          return c + KEYMAP_META_BASE;
        }
    }
}

/* **************************************************************** */
/*                                                                  */
/*                           Error handling                         */
/*                                                                  */
/* **************************************************************** */

/* Debugging level */
unsigned debug_level;

static FILE *debug_file;

static void
close_debugfile (void)
{
  fclose (debug_file);
}

#define INFODEBUG_FILE "infodebug"

static void
vinfo_debug (const char *format, va_list ap)
{
  FILE *fp;

  if (!debug_file)
    {
      if (!info_windows_initialized_p || display_inhibited)
	fp = stderr;
      else
	{
	  debug_file = fopen (INFODEBUG_FILE, "w");
	  if (!debug_file)
	    {
	      info_error (_("can't open %s: %s"), INFODEBUG_FILE,
			  strerror (errno));
	      exit (EXIT_FAILURE);
	    }
	  atexit (close_debugfile);
	  fp = debug_file;
	  info_error (_("debugging output diverted to \"%s\""),
		      INFODEBUG_FILE);
	}
    }
  else
    fp = debug_file;
  
  fprintf (fp, "%s: ", program_name);
  vfprintf (fp, format, ap);
  fprintf (fp, "\n");
  fflush (stderr);
}

void
info_debug (const char *format, ...)
{
  va_list ap;
  va_start (ap, format);
  vinfo_debug (format, ap);
  va_end (ap);
}

/* Non-zero means ring terminal bell on errors. */
int info_error_rings_bell_p = 1;

/* Print AP according to FORMAT.  If the window system was initialized,
   then the message is printed in the echo area.  Otherwise, a message is
   output to stderr. */
static void
vinfo_error (const char *format, va_list ap)
{
  if (!info_windows_initialized_p || display_inhibited)
    {
      fprintf (stderr, "%s: ", program_name);
      vfprintf (stderr, format, ap);
      fprintf (stderr, "\n");
      fflush (stderr);
    }
  else
    {
      if (!echo_area_is_active)
        {
          if (info_error_rings_bell_p)
            terminal_ring_bell ();
          vwindow_message_in_echo_area (format, ap);
        }
      else
        {
          NODE *temp = build_message_node (format, ap);
          if (info_error_rings_bell_p)
            terminal_ring_bell ();
          inform_in_echo_area (temp->contents);
          free (temp->contents);
          free (temp);
        }
    }
}

void
info_error (const char *format, ...)
{
  va_list ap;
  va_start (ap, format);
  vinfo_error (format, ap);
  va_end (ap);
}

void
show_error_node (char *error)
{
  if (info_error_rings_bell_p)
    terminal_ring_bell ();
  if (!info_windows_initialized_p)
    {
      info_error ("%s", error);
    }
  else if (!echo_area_is_active)
    {
      window_message_in_echo_area ("%s", error);
    }
  else
    inform_in_echo_area (error);
}


/* **************************************************************** */
/*                                                                  */
/*                       Window node history                        */
/*                                                                  */
/* **************************************************************** */

/* Free a NODE object that is suitable for being placed in a window. */
void
free_history_node (NODE *n)
{
  if (n && (n->flags & N_IsInternal))
    {
      free (n->contents);
      info_free_references (n->references);
      free (n->next); free (n->prev); free (n->up);
      free (n->nodename);
    }
  free (n);
}

static void
put_node_in_window (WINDOW *win, NODE *node)
{
  win->node = node;
  win->pagetop = 0;
  win->point = 0;
  free_matches (&win->matches);
  free (win->line_starts); win->line_starts = 0;
  free (win->log_line_no); win->log_line_no = 0;
  win->flags |= W_UpdateWindow;
}

/* Go back one in the node history. */
int
forget_node_fast (WINDOW *win)
{
  int i = win->hist_index;
  if (i == 0)
    return 0;

  free_history_node (win->hist[i - 1]->node);
  free (win->hist[i - 1]);
  win->hist[i - 1] = 0;
  i = --win->hist_index;

  if (i == 0)
    /* Window history is empty. */
    win->node = 0;
  else
    {
      put_node_in_window (win, win->hist[i - 1]->node);
      win->point = win->hist[i - 1]->point;
    }
  return i;
}

void
forget_node (WINDOW *win)
{
  int i = forget_node_fast (win);

  if (i == 0)
    {
      win->node = 0;
      return; /* Window history is empty. */
    }

  window_set_node_of_window (win, win->hist[i - 1]->node);
  if (auto_footnotes_p)
    info_get_or_remove_footnotes (win);
  set_window_pagetop (win, win->hist[i - 1]->pagetop);
  win->point = win->hist[i - 1]->point;
  window_compute_line_map (win);
  win->node->display_pos = win->point;
}

/* Remove associated list of nodes of WINDOW. */
void
forget_window_and_nodes (WINDOW *win)
{
  int i;
  for (i = 0; i < win->hist_index; i++)
    {
      free_history_node (win->hist[i]->node);
      free (win->hist[i]);
    }
  free (win->hist);
}

/* Like info_set_node_of_window, but only do enough so to extend the
   window history, avoiding calculating line starts. */
void
info_set_node_of_window_fast (WINDOW *win, NODE *node)
{
  WINDOW_STATE *new;

  if (win->hist_index && win->hist[win->hist_index - 1]->node == win->node)
    {
      win->hist[win->hist_index - 1]->pagetop = win->pagetop;
      win->hist[win->hist_index - 1]->point = win->point;
    }
  put_node_in_window (win, node);

  new = xmalloc (sizeof (WINDOW_STATE));
  new->node = win->node;
  new->pagetop = win->pagetop;
  new->point = win->point;
  add_pointer_to_array (new, win->hist_index, win->hist, win->hist_slots, 16);
}

/* Set WINDOW to show NODE.  Remember the new window in our list of
   Info windows.  If we are doing automatic footnote display, try to display
   the footnotes for this window. */
void
info_set_node_of_window (WINDOW *win, NODE *node)
{
  WINDOW_STATE *new;

  /* Remember the current values of pagetop and point if the remembered node
     is the same as the current one being displayed. */
  if (win->hist_index && win->hist[win->hist_index - 1]->node == win->node)
    {
      win->hist[win->hist_index - 1]->pagetop = win->pagetop;
      win->hist[win->hist_index - 1]->point = win->point;
    }

  /* Put this node into the window. */
  window_set_node_of_window (win, node);

  /* Remember this node, the currently displayed pagetop, and the current
     location of point in this window. */
  new = xmalloc (sizeof (WINDOW_STATE));
  new->node = win->node;
  new->pagetop = win->pagetop;
  new->point = win->point;
  add_pointer_to_array (new, win->hist_index, win->hist, win->hist_slots, 16);

  /* If doing auto-footnote display/undisplay, show the footnotes belonging
     to this window's node.  Don't do that if it is a footnote node itself. */
  if (auto_footnotes_p
      && !((win->node->flags & N_IsInternal)
           && !strcmp (win->node->nodename, "*Footnotes*")))
    info_get_or_remove_footnotes (win);
}


/* **************************************************************** */
/*                                                                  */
/*                   Cursor movement within a node                  */
/*                                                                  */
/* **************************************************************** */

static int forward_move_node_structure (WINDOW *window, int behaviour);
static int backward_move_node_structure (WINDOW *window, int behaviour);

/* Controls whether scroll-behavior affects line movement commands */
int cursor_movement_scrolls_p = 1; 

/* Variable controlling redisplay of scrolled windows.  If non-zero,
   it is the desired number of lines to scroll the window in order to
   make point visible.  A value of 1 produces smooth scrolling.  If set
   to zero, the line containing point is centered within the window. */
int window_scroll_step = 1;

/* Used after cursor movement commands.  Scroll window so that point is
   visible, and move the terminal cursor there. */
static void
info_show_point (WINDOW *window)
{
  if (window_scroll_step == 0)
    window_adjust_pagetop (window);
  else
    {
      int new_pagetop = window->pagetop;
      int line = window_line_of_point (window);
      if (line < window->pagetop)
        new_pagetop -= window_scroll_step;
      else if (line >= window->pagetop + window->height)
        new_pagetop += window_scroll_step;

      /* It's possible that moving by 'scroll-step' still won't show the
         point.  If so, call window_adjust_pagetop as a backup. */
      if (line >= new_pagetop && line < new_pagetop + window->height)
        set_window_pagetop (window, new_pagetop);
      else
        window_adjust_pagetop (window);
    }

  if (window->flags & W_UpdateWindow)
    display_update_one_window (window);

  display_cursor_at_point (window);
}

/* Advance point of WIN to the beginning of the next logical line.  Compute
   line map of new line.  Return 0 if we can't go any further. */
static int
point_next_line (WINDOW *win)
{
  int line = window_line_of_point (win);
  if (line + 1 < win->line_count)
    {
      win->point = win->line_starts[line + 1];
      window_compute_line_map (win);
      return 1;
    }

  if (cursor_movement_scrolls_p
      && forward_move_node_structure (win, info_scroll_behaviour) == 0)
    {
      win->point = 0;
      window_compute_line_map (win);
      return 1;
    }

  win->point = win->node->nodelen - 1;
  return 0;
}

/* Move point of WIN to the end of the previous logical line.  Compute
   line map of new line.  Return 0 if we can't go any further. */
static int
point_prev_line (WINDOW *win)
{
  int line = window_line_of_point (win);
  if (line > 0)
    {
      win->point = win->line_starts[line - 1];
      window_compute_line_map (win);
      win->point = win->line_map.map[win->line_map.used - 1];
      return 1;
    }

  if (cursor_movement_scrolls_p
      && backward_move_node_structure (win, info_scroll_behaviour) == 0)
    {
      win->point = win->node->nodelen - 1;
      if (win->line_count > win->height)
        set_window_pagetop (win, win->line_count - win->height);
      window_compute_line_map (win);
      return 1;
    }

  win->point = 0;
  return 0;
}

/* Return true if POINT sits on a newline character. */
static int
looking_at_newline (WINDOW *win, long point)
{
  mbi_iterator_t iter;
  mbi_init (iter, win->node->contents + point,
	    win->node->nodelen - point);
  mbi_avail (iter);
  return mbi_cur (iter).wc_valid && mbi_cur (iter).wc == '\n';
}

/* Return true if WIN->point sits on an alphanumeric character. */
static int
looking_at_alnum (WINDOW *win)
{
  mbi_iterator_t iter;
  mbi_init (iter, win->node->contents + win->point,
	    win->node->nodelen - win->point);
  mbi_avail (iter);

  return mbi_cur (iter).wc_valid && iswalnum (mbi_cur (iter).wc);
}

/* Advance point to the next multibyte character. */
static void
point_forward_char (WINDOW *win)
{
  long point = win->point;
  int col;

  /* Find column in the line map after the current one that advances the
     point.  (This may not be the very next character if we are at a
     double-width character that occupies multiple columns.) */
  col = window_point_to_column (win, point, 0) + 1;
  for (; col < win->line_map.used && win->line_map.map[col] == point; col++)
    ;

  if (col < win->line_map.used)
    win->point = win->line_map.map[col];
  else
    point_next_line (win);
}

/* Set point to the previous multibyte character.  Return 0 if we can't
   go any further. */
static int
point_backward_char (WINDOW *win)
{
  long point = win->point;
  int col;

  /* Find column in the line map before the current one that moves the
     point backward. */
  col = window_point_to_column (win, point, 0) - 1;
  for (; col >= 0 && win->line_map.map[col] == point; col--)
    ;

  if (col >= 0)
    {
      win->point = win->line_map.map[col];
      return 1;
    }
  else
    return point_prev_line (win);
}

/* Advance window point to the beginning of the next word. */
static void
point_forward_word (WINDOW *win)
{
  int col;

  col = window_point_to_column (win, win->point, &win->point);

  /* Skip white space forwards. */
  while (1)
    {
      for (; col < win->line_map.used; col++)
	{
          win->point = win->line_map.map[col];
          if (looking_at_alnum (win))
            goto skipped_whitespace;
	}
      if (!point_next_line (win))
        return;
      col = 0;
    }
  skipped_whitespace:

  while (1)
    {
      for (; col < win->line_map.used; col++)
	{
          win->point = win->line_map.map[col];
	  if (!looking_at_alnum (win))
            return;
	}
      if (!point_next_line (win))
        return;
      col = 0;
    }
}

/* Set window point to the beginning of the previous word. */
static void
point_backward_word (WINDOW *win)
{
  /* Skip any white space before current cursor position. */
  while (point_backward_char (win))
    {
      if (looking_at_alnum (win))
        goto back_to_word_start;
    }

back_to_word_start:
  while (point_backward_char (win))
    {
      if (!looking_at_alnum (win))
        {
          point_forward_char (win);
          return;
        }
    }
}

void info_prev_line (WINDOW *, int count);

/* Move to goal column, or end of line. */
static void
move_to_goal_column (WINDOW *window)
{
  long goal;

  goal = window->goal_column;
  if (goal >= window->line_map.used)
    goal = window->line_map.used - 1;
  window->point = window->line_map.map[goal];
  info_show_point (window);
}

/* Move WINDOW's point down to the next line if possible. */
DECLARE_INFO_COMMAND (info_next_line, _("Move down to the next line"))
{
  if (count < 0)
    info_prev_line (window, -count);
  else
    {
      if (window->goal_column == -1)
        window->goal_column = window_get_cursor_column (window);
      while (count--)
        point_next_line (window);
      move_to_goal_column (window);
    }
}

/* Move WINDOW's point up to the previous line if possible. */
DECLARE_INFO_COMMAND (info_prev_line, _("Move up to the previous line"))
{
  if (count < 0)
    info_next_line (window, -count);
  else
    {
      if (window->goal_column == -1)
        window->goal_column = window_get_cursor_column (window);
      while (count--)
        point_prev_line (window);
      move_to_goal_column (window);
    }
}

/* Move the cursor to the desired line of the window. */
DECLARE_INFO_COMMAND (info_move_to_window_line,
   _("Move the cursor to a specific line of the window"))
{
  int line;

  /* With no numeric argument of any kind, default to the center line. */
  if (!info_explicit_arg && count == 1)
    line = (window->height / 2) + window->pagetop;
  else
    {
      if (count < 0)
        line = (window->height + count) + window->pagetop;
      else
        line = window->pagetop + count;
    }

  /* If the line doesn't appear in this window, make it do so. */
  if (line - window->pagetop >= window->height)
    line = window->pagetop + (window->height - 1);

  /* If the line is too small, make it fit. */
  if (line < window->pagetop)
    line = window->pagetop;

  /* If the selected line is past the bottom of the node, force it back. */
  if (line >= window->line_count)
    line = window->line_count - 1;

  window->point = window->line_starts[line];
  info_show_point (window);
}

/* Move WINDOW's point to the end of the logical line. */
DECLARE_INFO_COMMAND (info_end_of_line, _("Move to the end of the line"))
{
  long point;
  if (!window->node)
    return;

  /* Find physical line with end of logical line in it. */
  while (!looking_at_newline (window,
              window->line_map.map[window->line_map.used - 1]))
    point_next_line (window);

  if (window->line_map.used == 0)
    return; /* This shouldn't happen. */

  /* Return offset of terminating newline. */
  point = window->line_map.map[window->line_map.used - 1];

  if (point != window->point)
    {
      window->point = point;
      info_show_point (window);
    }
}

/* Move WINDOW's point to the beginning of the logical line. */
DECLARE_INFO_COMMAND (info_beginning_of_line, _("Move to the start of the line"))
{
  int old_point = window->point;
  int point;
  
  while (1)
    {
      point = window->line_map.map[0];
      if (point == 0 || looking_at_newline (window, point-1))
	break;
      point_prev_line (window);
    }

  if (point != old_point)
    {
      window->point = point;
      info_show_point (window);
    }
  else
    window->point = old_point;
}

void info_backward_char (WINDOW *, int count);

/* Move point forward in the node. */
DECLARE_INFO_COMMAND (info_forward_char, _("Move forward a character"))
{
  if (count < 0)
    info_backward_char (window, -count);
  else
    {
      while (count--)
        point_forward_char (window);
      info_show_point (window);
    }
}

/* Move point backward in the node. */
DECLARE_INFO_COMMAND (info_backward_char, _("Move backward a character"))
{
  if (count < 0)
    info_forward_char (window, -count);
  else
    {
      while (count--)
        point_backward_char (window);
      info_show_point (window);
    }
}

void info_backward_word (WINDOW *, int count);

/* Move forward a word in this node. */
DECLARE_INFO_COMMAND (info_forward_word, _("Move forward a word"))
{
  if (count < 0)
    {
      info_backward_word (window, -count);
      return;
    }

  while (count--)
    point_forward_word (window);

  info_show_point (window);
}

DECLARE_INFO_COMMAND (info_backward_word, _("Move backward a word"))
{
  if (count < 0)
    {
      info_forward_word (window, -count);
      return;
    }

  while (count--)
    point_backward_word (window);

  info_show_point (window);
}

/* Move to the beginning of the node. */
DECLARE_INFO_COMMAND (info_beginning_of_node, _("Move to the start of this node"))
{
  window->point = 0;
  info_show_point (window);
}

/* Move to the end of the node. */
DECLARE_INFO_COMMAND (info_end_of_node, _("Move to the end of this node"))
{
  window->point = window->node->nodelen - 1;
  info_show_point (window);
}


/* **************************************************************** */
/*                                                                  */
/*                     Scrolling window                             */
/*                                                                  */
/* **************************************************************** */

/* Variable controlling the behaviour of default scrolling when you are
   already at the bottom of a node.  Possible values are defined in session.h.
   The meanings are:

   IS_Continuous        Try to get first menu item, or failing that, the
                        "Next:" pointer, or failing that, the "Up:" and
                        "Next:" of the up.
   IS_NextOnly          Try to get "Next:" menu item.
   IS_PageOnly          Simply give up at the bottom of a node. */

int info_scroll_behaviour = IS_Continuous;

static void _scroll_forward (WINDOW *window, int count, int nodeonly);
static void _scroll_backward (WINDOW *window, int count, int nodeonly);

static void
_scroll_forward (WINDOW *window, int count, int nodeonly)
{
  if (count < 0)
    _scroll_backward (window, -count, nodeonly);
  else
    {
      if (window->pagetop >= window->line_count - window->height)
        {
          if (!nodeonly)
            {
              /* If there are no more lines to scroll here, error, or get
                 another node. */
              if (forward_move_node_structure (window, info_scroll_behaviour)
                  == 0)
                window->point = 0;
              else
                info_end_of_node (window, 1);
            }
          return;
        }
      set_window_pagetop (window, window->pagetop + count);
    }
}

static void
_scroll_backward (WINDOW *window, int count, int nodeonly)
{
  if (count < 0)
    _scroll_backward (window, -count, nodeonly);
  else
    {
      int desired_top;

      if (window->pagetop <= 0)
        {
          if (!nodeonly)
            {
              /* If there are no more lines to scroll here, error, or get
                 another node. */
              if (backward_move_node_structure (window, info_scroll_behaviour)
                  == 0)
                {
                  info_end_of_node (window, 1);
                  window->point = window->line_starts[window->pagetop];
                }
              else
                window->point = 0;
            }
          return;
        }

      desired_top = window->pagetop - count;
      if (desired_top < 0)
        desired_top = 0;
      set_window_pagetop (window, desired_top);
    }
}

/* Lines to scroll by.  -1 means scroll by screen size. */
int default_window_size = -1;

/* Show the next screen of WINDOW's node. */
DECLARE_INFO_COMMAND (info_scroll_forward, _("Scroll forward in this window"))
{
  int lines;

  if (info_explicit_arg)
    lines = count;
  else if (default_window_size > 0)
    lines = default_window_size * count;
  else
    lines = (window->height - 2) * count;
  _scroll_forward (window, lines, 0);
}

/* Show the previous screen of WINDOW's node. */
DECLARE_INFO_COMMAND (info_scroll_backward, _("Scroll backward in this window"))
{
  info_scroll_forward (window, -count);
}

/* Like info_scroll_forward, but sets default_window_size as a side
   effect.  */
DECLARE_INFO_COMMAND (info_scroll_forward_set_window,
                      _("Scroll forward in this window and set default window size"))
{
  if (info_explicit_arg)
    {
      default_window_size = count;
      if (default_window_size < 0)
        default_window_size *= -1;
    }

  info_scroll_forward (window, count);
}

/* Like info_scroll_backward, but sets default_window_size as a side
   effect.  */
DECLARE_INFO_COMMAND (info_scroll_backward_set_window,
                      _("Scroll backward in this window and set default window size"))
{
  info_scroll_forward_set_window (window, -count);
}

/* Show the next screen of WINDOW's node but never advance to next node. */
DECLARE_INFO_COMMAND (info_scroll_forward_page_only, _("Scroll forward in this window staying within node"))
{
  int lines;

  if (info_explicit_arg)
    lines = count;
  else if (default_window_size > 0)
    lines = default_window_size * count;
  else
    lines = (window->height - 2) * count;
  _scroll_forward (window, lines, 1);
}

/* Show the previous screen of WINDOW's node but never move to previous
   node. */
DECLARE_INFO_COMMAND (info_scroll_backward_page_only, _("Scroll backward in this window staying within node"))
{
  info_scroll_forward_page_only (window, -count);
}

/* Like info_scroll_forward_page_only, but sets default_window_size as a side
   effect.  */
DECLARE_INFO_COMMAND (info_scroll_forward_page_only_set_window,
                      _("Scroll forward in this window staying within node and set default window size"))
{
  int lines;

  if (info_explicit_arg)
    {
      default_window_size = count;
      count = 1;

      if (default_window_size < 0)
        {
          default_window_size *= -1;
          count = -1;
        }
    }

  if (default_window_size > 0)
    lines = default_window_size * count;
  else
    lines = (window->height - 2) * count;

  _scroll_forward (window, lines, 1);
}

/* Like info_scroll_backward_page_only, but sets default_window_size as a side
   effect.  */
DECLARE_INFO_COMMAND (info_scroll_backward_page_only_set_window,
                      _("Scroll backward in this window staying within node and set default window size"))
{
  info_scroll_forward_page_only_set_window (window, -count);
}

/* Scroll the window forward by N lines.  */
DECLARE_INFO_COMMAND (info_down_line, _("Scroll down by lines"))
{
  _scroll_forward (window, count, 0);
}

/* Scroll the window backward by N lines.  */
DECLARE_INFO_COMMAND (info_up_line, _("Scroll up by lines"))
{
  _scroll_backward (window, count, 0);
}

/* Lines to scroll when using commands that scroll by half screen size
   by default.  0 means scroll by half screen size. */
int default_scroll_size = 0;

/* Scroll the window forward by N lines and remember N as default for
   subsequent commands.  */
DECLARE_INFO_COMMAND (info_scroll_half_screen_down,
                      _("Scroll down by half screen size"))
{
  int lines;

  if (info_explicit_arg)
    {
      default_scroll_size = count;
      count = 1;

      if (default_scroll_size < 0)
        {
          default_scroll_size *= -1;
          count = -1;
        }
    }

  if (default_scroll_size != 0)
    lines = default_scroll_size * count;
  else
    lines = (window->height + 1) / 2 * count;

  _scroll_forward (window, lines, 1);
}

/* Scroll the window backward by N lines and remember N as default for
   subsequent commands.  */
DECLARE_INFO_COMMAND (info_scroll_half_screen_up,
                      _("Scroll up by half screen size"))
{
  info_scroll_half_screen_down (window, -count);
}

/* Scroll the "other" window of WINDOW. */
DECLARE_INFO_COMMAND (info_scroll_other_window, _("Scroll the other window"))
{
  WINDOW *other;

  /* If only one window, give up. */
  if (!windows->next)
    {
      info_error ("%s", msg_one_window);
      return;
    }

  other = window->next;

  if (!other)
    other = window->prev;

  info_scroll_forward (other, count);
}

/* Scroll the "other" window of WINDOW. */
DECLARE_INFO_COMMAND (info_scroll_other_window_backward,
                      _("Scroll the other window backward"))
{
  info_scroll_other_window (window, -count);
}


/* **************************************************************** */
/*                                                                  */
/*                      Garbage collection                          */
/*                                                                  */
/* **************************************************************** */

static void
gc_file_buffers_and_nodes (void)
{
  /* Array to record whether each file buffer was referenced or not. */
  int *fb_referenced = xcalloc (info_loaded_files_index, sizeof (int));
  WINDOW *win;
  int i;
  int fb_index;

  /* Loop over nodes in the history of displayed windows recording
     which file buffers were referenced. */
  for (win = windows; win; win = win->next)
    {
      if (!win->hist)
        continue;
      for (i = 0; win->hist[i]; i++)
        {
          NODE *n = win->hist[i]->node;

          /* Loop over file buffers. */
          for (fb_index = 0; fb_index < info_loaded_files_index; fb_index++)
            {
              FILE_BUFFER *fb = info_loaded_files[fb_index];

              /* Each node should match at most one file, either a subfile or a 
                 non-split file. */
              if (fb->flags & N_Subfile)
                {
                  if (n->subfile && !FILENAME_CMP (fb->fullpath, n->subfile))
                    {
                      fb_referenced[fb_index] = 1;
                      break;
                    }
                }
              else if (!(fb->flags & N_TagsIndirect))
                {
                  if (n->fullpath && !FILENAME_CMP (fb->fullpath, n->fullpath))
                    {
                      fb_referenced[fb_index] = 1;
                      break;
                    }
                }
            }
        }
    }

  /* Free unreferenced file buffers. */
  for (i = 0; i < info_loaded_files_index; i++)
    {
      if (!fb_referenced[i])
        {
          FILE_BUFFER *fb = info_loaded_files[i];
          TAG **t;

          if (fb->flags & N_TagsIndirect)
            continue;

          /* If already gc-ed, do nothing. */
          if (!fb->contents)
            continue;

          /* If this file had to be uncompressed, check to see if we should
             gc it.  This means that the user-variable "gc-compressed-files"
             is non-zero. */
          if ((fb->flags & N_IsCompressed) && !gc_compressed_files)
            continue;

          /* If this file's contents are not gc-able, move on. */
          if (fb->flags & N_CannotGC)
            continue;

          /* Don't free file buffers corresponding to files that aren't there 
             any more, because a node may still refer to them. */
          if (fb->flags & N_Gone)
            continue;

          free (fb->contents);
          fb->contents = 0;

          /* Clear pointers into the file contents in the tags table. */
          if (fb->tags)
            for (t = fb->tags; (*t); t++)
              {
                if (!((*t)->cache.flags & N_WasRewritten))
                  (*t)->cache.contents = 0;
              }
        }
    }

  free (fb_referenced);
}


/* **************************************************************** */
/*                                                                  */
/*                 Commands for Manipulating Windows                */
/*                                                                  */
/* **************************************************************** */

void info_prev_window (WINDOW *, int count);

/* Make the next window in the chain be the active window. */
DECLARE_INFO_COMMAND (info_next_window, _("Select the next window"))
{
  if (count < 0)
    {
      info_prev_window (window, -count);
      return;
    }

  /* If no other window, error now. */
  if (!windows->next || echo_area_is_active)
    {
      info_error ("%s", msg_one_window);
      return;
    }

  while (count--)
    {
      if (window->next)
        window = window->next;
      else
        window = windows;
    }

  if (active_window != window)
    {
      if (auto_footnotes_p)
        info_get_or_remove_footnotes (window);

      /* Point may not be within part of node displayed in window if there
         has been resizing of this window since the last time it was active. */
      window_adjust_pagetop (window);
      window->flags |= W_UpdateWindow;
      active_window = window;
    }
}

/* Make the previous window in the chain be the active window. */
DECLARE_INFO_COMMAND (info_prev_window, _("Select the previous window"))
{
  if (count < 0)
    {
      info_next_window (window, -count);
      return;
    }

  /* Only one window? */
  if (!windows->next || echo_area_is_active)
    {
      info_error ("%s", msg_one_window);
      return;
    }

  while (count--)
    {

      if (window != windows && window->prev)
        window = window->prev;
      else if (window == windows)
        {
          /* If we are in the first window, find the last window in the
             chain. */
          while (window->next)
            window = window->next;
        }
    }

  if (active_window != window)
    {
      if (auto_footnotes_p)
        info_get_or_remove_footnotes (window);

      /* Point may not be within part of node displayed in window if there
         has been resizing of this window since the last time it was active. */
      window_adjust_pagetop (window);
      window->flags |= W_UpdateWindow;
      active_window = window;
    }
}

/* Split active window into two windows, both showing the same node.  If we
   are automatically tiling windows, re-tile after the split. */
DECLARE_INFO_COMMAND (info_split_window, _("Split the current window"))
{
  WINDOW *split = window_make_window ();

  if (!split)
    info_error ("%s", msg_win_too_small);
  else
    {
      NODE *copy = xmalloc (sizeof (NODE));
      *copy = *window->node; /* Field-by-field copy of structure. */

      if (copy->flags & N_IsInternal)
        {
          /* This allows us to free nodes without checking if these fields
             are shared by NODE objects in other windows.  For non-internal
             nodes, this data is stored in the tag table. */
          copy->references = info_copy_references (copy->references);
          copy->nodename = xstrdup (copy->nodename);

          if (copy->up)
            copy->up = xstrdup (copy->up);
          if (copy->next)
            copy->next = xstrdup (copy->next);
          if (copy->prev)
            copy->prev = xstrdup (copy->prev);
          copy->contents = xstrdup (copy->contents);
        }

      info_set_node_of_window (split, copy);
      /* Make sure point still appears in the active window. */
      info_show_point (window);

      split->pagetop = window->pagetop;

      if (auto_tiling_p)
        window_tile_windows (DONT_TILE_INTERNALS);
      else
        window_adjust_pagetop (split);
    }
}

/* Delete WINDOW, forgetting the list of last visited nodes.  If we are
   automatically displaying footnotes, show or remove the footnotes
   window.  If we are automatically tiling windows, re-tile after the
   deletion. */
DECLARE_INFO_COMMAND (info_delete_window, _("Delete the current window"))
{
  if (!windows->next)
    info_error ("%s", msg_cant_kill_last);
  else if (window->flags & W_WindowIsPerm)
    info_error ("%s", _("Cannot delete a permanent window"));
  else
    {
      info_delete_window_internal (window);

      if (auto_footnotes_p)
        info_get_or_remove_footnotes (active_window);

      if (auto_tiling_p)
        window_tile_windows (DONT_TILE_INTERNALS);

      gc_file_buffers_and_nodes ();
    }
}

/* Do the physical deletion of WINDOW, and forget this window and
   associated nodes. */
void
info_delete_window_internal (WINDOW *window)
{
  if (windows->next && ((window->flags & W_WindowIsPerm) == 0))
    {
      forget_window_and_nodes (window);
      window_delete_window (window);

      if (echo_area_is_active)
        echo_area_inform_of_deleted_window (window);
    }
}

/* Just keep WINDOW, deleting all others. */
DECLARE_INFO_COMMAND (info_keep_one_window, _("Delete all other windows"))
{
  int num_deleted;              /* The number of windows we deleted. */
  int pagetop, start, end;

  /* Remember a few things about this window.  We may be able to speed up
     redisplay later by scrolling its contents. */
  pagetop = window->pagetop;
  start = window->first_row;
  end = start + window->height;

  num_deleted = 0;

  while (1)
    {
      WINDOW *win;

      /* Find an eligible window and delete it.  If no eligible windows
         are found, we are done.  A window is eligible for deletion if
         is it not permanent, and it is not WINDOW. */
      for (win = windows; win; win = win->next)
        if (win != window && ((win->flags & W_WindowIsPerm) == 0))
          break;

      if (!win)
        break;

      info_delete_window_internal (win);
      num_deleted++;
    }

  /* Scroll the contents of this window into the right place so that the
     user doesn't have to wait any longer than necessary for redisplay. */
  if (num_deleted)
    {
      int amount;

      amount = (window->first_row - start);
      amount -= (window->pagetop - pagetop);
      display_scroll_display (start, end, amount);
    }
  window->flags |= W_UpdateWindow;
  gc_file_buffers_and_nodes ();
}

/* Change the size of WINDOW by AMOUNT. */
DECLARE_INFO_COMMAND (info_grow_window, _("Grow (or shrink) this window"))
{
  window_change_window_height (window, count);
}

/* When non-zero, tiling takes place automatically when info_split_window
   is called. */
int auto_tiling_p = 0;

/* Tile all of the visible windows. */
DECLARE_INFO_COMMAND (info_tile_windows,
    _("Divide the available screen space among the visible windows"))
{
  window_tile_windows (TILE_INTERNALS);
}

/* Toggle the state of this window's wrapping of lines. */
DECLARE_INFO_COMMAND (info_toggle_wrap,
              _("Toggle the state of line wrapping in the current window"))
{
  window_toggle_wrap (window);
}


/* **************************************************************** */
/*                                                                  */
/*                    Cross-references and menus                    */
/*                                                                  */
/* **************************************************************** */

/* Using WINDOW for various defaults, select the node referenced by ENTRY
   in it.  If the node is selected, the window and node are remembered.
   Display an error message if reference couldn't be selected and return 0. */
int
info_select_reference (WINDOW *window, REFERENCE *entry)
{
  NODE *node;
  char *file_system_error = NULL;

  /* We need to copy everything from entry because the call to 
     info_get_node_with_defaults can free it if it came from
     the tag table of a file. */
  char *filename = entry->filename;
  char *nodename = entry->nodename;
  char *label = entry->label;
  int line_number = entry->line_number;

  node = info_get_node_with_defaults (filename, nodename, window->node);

  /* Try something a little weird.  If the node couldn't be found, and the
     reference was of the form "foo::", see if the entry->label can be found
     as a file, with a node of "Top". */
  if (!node)
    {
      if (info_recent_file_error)
        file_system_error = xstrdup (info_recent_file_error);

      if (nodename && label && !strcmp (nodename, label))
        {
          free (file_system_error);
          file_system_error = NULL;

          node = info_get_node (label, "Top");
          if (!node && info_recent_file_error)
            file_system_error = xstrdup (info_recent_file_error);
        }
    }

  if (!node)
    {
      if (file_system_error)
        {
          info_error ("%s", file_system_error);
          free (file_system_error);
        }
      else
        info_error (msg_cant_find_node, nodename ? nodename : "Top");
      return 0;
    }

  /* If in a footnotes window, try to switch to a window containing a
     node from the file. */
  if (window->node && (window->node->flags & N_IsInternal)
      && !strcmp (window->node->nodename, "*Footnotes*"))
        {
          WINDOW *w;

          for (w = windows; w; w = windows->next)
            {
              if (!strcmp (w->node->fullpath, window->node->fullpath)
                  && !(w->flags & W_TempWindow))
                {
                  /* Switch to this window. */
                  active_window = window = w;
                  break;
                }
            }
        }
  info_set_node_of_window (window, node);

  if (line_number > 0)
    {
      /* Go to the line given by entry->line_number. */
      long line = window_log_to_phys_line (window, line_number - 1);

      if (line >= 0 && line < window->line_count)
        {
          window->point = window->line_starts[line];
          window_adjust_pagetop (window);
        }
    }
  return 1;
}

/* Parse the node specification in LINE using WINDOW to default the filename.
   Select the parsed node in WINDOW and remember it, or error if the node
   couldn't be found. */
static void
info_parse_and_select (char *line, WINDOW *window)
{
  REFERENCE entry;

  info_parse_node (line);
  entry.filename = info_parsed_filename;
  entry.nodename = info_parsed_nodename;
  entry.line_number = 0;
  entry.label = "*info-parse-and-select*";

  info_select_reference (window, &entry);
}

/* Return menu entry indexed by KEY, where '1' is the first menu item, '2' is
   the second, etc., and '0' is the last.  Return value should not be freed. */
static REFERENCE *
select_menu_digit (WINDOW *window, unsigned char key)
{
  register int i, item;
  register REFERENCE **menu;

  menu = window->node->references;
  if (!menu)
    return 0;

  item = key - '0';

  /* Special case.  Item "0" is the last item in this menu. */
  if (item == 0)
    {
      int j;
      i = -1; /* Not found */
      for (j = 0; menu[j]; j++)
        if (menu[j]->type == REFERENCE_MENU_ITEM)
          i = j;
    }
  else
    {
      int k = 0;
      for (i = 0; menu[i]; i++)
        {
          if (menu[i]->type == REFERENCE_MENU_ITEM)
            k++;
          if (k == item)
            break;
        }
    }

  if (i == -1)
    return 0;

  return menu[i];
}

DECLARE_INFO_COMMAND (info_menu_digit, _("Select this menu item"))
{} /* Declaration only. */

/* Use KEY (a digit) to select the Nth menu item in WINDOW->node. */
void
menu_digit (WINDOW *window, int key)
{
  int item = key - '0';
  REFERENCE *entry;
  REFERENCE **references = window->node->references;

  /* Check if there is a menu in this node. */
  if (references)
    {
      int i; 
      for (i = 0; references[i]; i++)
        if (references[i]->type == REFERENCE_MENU_ITEM)
          goto has_menu;
    }

  info_error ("%s", msg_no_menu_node);
  return;
has_menu:

  if (entry = select_menu_digit (window, key))
    info_select_reference (window, entry);
  else if (key == '0')
    /* Don't print "There aren't 0 items in this menu" */
    info_error ("%s", msg_no_menu_node);
  else
    info_error (ngettext ("There isn't %d item in this menu",
                          "There aren't %d items in this menu",
                          item),
                item);
  return;
}

/* Select the last menu item in WINDOW->node. */
DECLARE_INFO_COMMAND (info_last_menu_item,
   _("Select the last item in this node's menu"))
{
  menu_digit (window, '0');
}

static int exclude_cross_references (REFERENCE *r)
{
  return r->type == REFERENCE_XREF;
}

static int exclude_menu_items (REFERENCE *r)
{
  return r->type == REFERENCE_MENU_ITEM;
}

static int exclude_nothing (REFERENCE *r)
{
  return 1;
}

/* Read a menu or followed reference from the user defaulting to the
   reference found on the current line, and select that node.  The
   reading is done with completion. ASK_P is non-zero if the user should
   be prompted, or zero to select the item on the current line.  MENU_ITEM
   and XREF control whether menu items and cross-references are eligible
   for selection. */
static void
info_menu_or_ref_item (WINDOW *window, int menu_item, int xref, int ask_p)
{
  REFERENCE *defentry = NULL; /* Default link */
  REFERENCE **refs = window->node->references;
  REFERENCE *entry;

  /* Name of destination */
  char *line;

  int line_no;
  int this_line, next_line;

  int which, closest = -1;

  reference_bool_fn exclude; 

  if (!refs)
    return;

  if (menu_item && !xref)
    {
      exclude = &exclude_cross_references;
    }
  else if (!menu_item && xref)
    {
      exclude = &exclude_menu_items;
    }
  else if (menu_item && xref)
    {
      exclude = &exclude_nothing;
    }
  else /* !menu_item && !xref */
    return;

  line_no = window_line_of_point (window);
  this_line = window->line_starts[line_no];
  if (window->line_starts[line_no + 1])
    next_line = window->line_starts[line_no + 1];
  else
    next_line = window->node->nodelen;

  /* Look for a reference in the current line, preferring one that
     the point is in, otherwise preferring after the point. */
  for (which = 0; refs[which]; which++)
    {
      /* If we got to the next line without finding an eligible reference. */
      if (refs[which]->start >= next_line)
        break;

      /* Check the type of reference is one we are looking for. */
      if (!(  (menu_item && refs[which]->type == REFERENCE_MENU_ITEM)
           || (xref      && refs[which]->type == REFERENCE_XREF)))
        continue;

      /* Reference is eligible if any part of it is in the line. */
      if (refs[which]->start >= this_line && refs[which]->start < next_line
          || refs[which]->start < this_line && refs[which]->end > this_line)
        {
          closest = which;

          /* If a reference contains the point, prefer it.  Otherwise prefer a
             reference after the cursor to one before it. */
          if (window->point >= refs[which]->start
              && window->point < refs[which]->end)
            break;
        }
    }
  if (closest != -1)
    defentry = refs[closest];

  if (ask_p)
    {
      char *prompt;

      /* Build the prompt string. */
      if (menu_item && !xref)
        {
          if (defentry)
            {
              prompt = xmalloc (strlen (defentry->label)
                                + strlen (_("Menu item (%s): ")));
              sprintf (prompt, _("Menu item (%s): "), defentry->label);
            }
          else
            prompt = xstrdup (_("Menu item: "));
        }
      else
        {
          if (defentry)
            {
              prompt = xmalloc (strlen (defentry->label)
                                + strlen (_("Follow xref (%s): ")));
              sprintf (prompt, _("Follow xref (%s): "), defentry->label);
            }
          else
            prompt = xstrdup (_("Follow xref: "));
        }

      line = info_read_completing_in_echo_area_with_exclusions (prompt, refs,
                                                                exclude);
      free (prompt);

      window = active_window;

      /* User aborts, just quit. */
      if (!line)
        {
          info_abort_key (window, 0);
          return;
        }

      /* If we had a default and the user accepted it, use that. */
      if (!*line)
        {
          free (line);
          if (defentry)
            line = xstrdup (defentry->label);
          else
            line = NULL;
        }
    }
  else
    {
      /* Not going to ask any questions.  If we have a default entry, use
         that, otherwise return. */
      if (!defentry)
        return;
      else
        line = xstrdup (defentry->label);
    }

  if (line)
    {
      /* It is possible that the references have more than a single
         entry with the same label, and also LINE is down-cased, which
         complicates matters even more.  Try to be as accurate as we
         can: if they've chosen the default, use defentry directly. */
      if (defentry && strcmp (line, defentry->label) == 0)
        entry = defentry;
      else
        /* Find the selected label in the references.  If there are
           more than one label which matches, find the one that's
           closest to point.  */
        {
          register int i;
          int best = -1, min_dist = window->node->nodelen;
          REFERENCE *ref;

          for (i = 0; refs && (ref = refs[i]); i++)
            {
              /* Need to use mbscasecmp because LINE is downcased
                 inside info_read_completing_in_echo_area.  */
              if (mbscasecmp (line, ref->label) == 0)
                {
                  /* ref->end is more accurate estimate of position
                     for menus than ref->start.  Go figure.  */
                  int dist = abs (window->point - ref->end);

                  if (dist < min_dist)
                    {
                      min_dist = dist;
                      best = i;
                    }
                }
            }
          if (best != -1)
            entry = refs[best];
          else
            entry = NULL;
        }

      if (!entry && defentry)
        info_error (_("The reference disappeared! (%s)"), line);
      else
        {
          info_select_reference (window, entry);
        }

      free (line);
    }
}

/* Read a line (with completion) which is the name of a menu item,
   and select that item. */
DECLARE_INFO_COMMAND (info_menu_item, _("Read a menu item and select its node"))
{
  if (window->node->references)
    {
      REFERENCE **r;
      
      /* Check if there is a menu in this node. */
      for (r = window->node->references; *r; r++)
        if ((*r)->type == REFERENCE_MENU_ITEM)
          break;

      if (*r)
        {
          info_menu_or_ref_item (window, 1, 0, 1);
          return;
        }
    }

  info_error ("%s", msg_no_menu_node);
  return;
}

/* Read a line (with completion) which is the name of a reference to
   follow, and select the node. */
DECLARE_INFO_COMMAND
  (info_xref_item, _("Read a footnote or cross reference and select its node"))
{
  if (window->node->references)
    {
      REFERENCE **r;
      
      /* Check if there is a cross-reference in this node. */
      for (r = window->node->references; *r; r++)
        if ((*r)->type == REFERENCE_XREF)
          break;

      if (*r)
        {
          info_menu_or_ref_item (window, 0, 1, 1);
          return;
        }
    }

  info_error ("%s", msg_no_xref_node);
  return;
}

/* Position the cursor at the start of this node's menu. */
DECLARE_INFO_COMMAND (info_find_menu, _("Move to the start of this node's menu"))
{
  SEARCH_BINDING binding;
  long position;

  binding.buffer = window->node->contents;
  binding.start  = 0;
  binding.end = window->node->nodelen;
  binding.flags = S_FoldCase | S_SkipDest;

  if (search (INFO_MENU_LABEL, &binding, &position) == search_success)
    {
      window->point = position;
      window_adjust_pagetop (window);
      window->flags |= W_UpdateWindow;
    }
  else
    info_error ("%s", msg_no_menu_node);
}

/* Visit as many menu items as is possible, each in a separate window. */
DECLARE_INFO_COMMAND (info_visit_menu,
  _("Visit as many menu items at once as possible"))
{
  register int i;
  REFERENCE *entry, **menu;

  menu = window->node->references;

  if (!menu)
    {
      info_error ("%s", msg_no_menu_node);
      return; 
    }

  for (i = 0; (entry = menu[i]); i++)
    {
      WINDOW *new;

      if (entry->type != REFERENCE_MENU_ITEM) continue;

      new = window_make_window ();
      info_set_node_of_window (new, window->node);
      window_tile_windows (TILE_INTERNALS);

      if (!new)
        {
          info_error ("%s", msg_win_too_small);
          break;
        }
      else
        {
          active_window = new;
          if (!info_select_reference (new, entry))
            break;
        }
    }
}

/* Move to the next or previous cross reference in this node.  Return 0 if
   there aren't any. */
static int
info_move_to_xref (WINDOW *window, int dir)
{
  long placement = -1;
  NODE *node = window->node;
  REFERENCE **ref;

  /* Fail if there are no references in node */
  if (!node->references || !node->references[0])
    return 0;

  if (dir == 1) /* Search forwards */
    for (ref = node->references; *ref != 0; ref++)
      {
        if ((*ref)->start > window->point)
          {
            placement = (*ref)->start;
            break;
          }
      }
  else /* Search backwards */
    for (ref = node->references; *ref != 0; ref++)
      {
        if ((*ref)->start >= window->point) break;
        placement = (*ref)->start;
      }

  if (placement == -1)
    {
      /* There was neither a menu or xref entry appearing in this node
         after point. */
      return 0;
    }

  window->point = placement;
  window_adjust_pagetop (window);
  return 1;
}

void info_move_to_next_xref (WINDOW *, int count);

/* Remove history entries from START inclusive to END exclusive.
   Warning: be careful about removing the last history entry, as
   info_set_node_of_window includes the currently displayed node in
   the history. */
static void
cleanup_history (WINDOW *window, int start, int end)
{
  int i;
  for (i = start; i < end; i++)
    {
      free_history_node (window->hist[i]->node);
      free (window->hist[i]);
    }
  memmove (&window->hist[start], &window->hist[end],
           (window->hist_index - end) * sizeof (WINDOW_STATE *));
  window->hist_index -= end - start;
  window->hist[window->hist_index] = 0;
}

DECLARE_INFO_COMMAND (info_move_to_prev_xref,
                      _("Move to the previous cross reference"))
{
  if (count < 0)
    info_move_to_next_xref (window, -count);
  else
    {
      size_t last_hist_index, starting_hist_index;
      char *initial_nodename = window->node->nodename;

      last_hist_index = starting_hist_index = window->hist_index - 1;

      while (count > 0)
        {
          if (info_move_to_xref (window, -1))
            {
              last_hist_index = window->hist_index - 1;
              count--;
              continue;
            }

          /* When cursor-movement-scrolls=Off, cycle round the node's
             references. */
          if (!cursor_movement_scrolls_p)
            {
              REFERENCE **r = window->node->references;
              if (r && r[0])
                {
                  int i = 0;
                  /* Choose the last menu or xref entry appearing in this
                     node. */
                  while (r[i + 1])
                    i++;
                  window->point = r[i]->start;
                  window_adjust_pagetop (window);
                  count--;
                  continue;
                }
                
              info_error ("%s", msg_no_xref_node);
              return;
            }

          if (backward_move_node_structure (window, info_scroll_behaviour != 0)
              || !strcmp (window->node->nodename, initial_nodename))
            {
              break; /* No earlier nodes in file, or we are back where we
                         started. */
            }
          window->point = window->node->nodelen - 1;
        }

      /* Go back to the last place a reference was found, or
         the starting place. */
      while (window->hist_index > last_hist_index + 1)
        forget_node (window);

      /* Remove any intermediate nodes. */
      if (last_hist_index != starting_hist_index)
        cleanup_history (window, starting_hist_index + 1, last_hist_index);
    }
}

DECLARE_INFO_COMMAND (info_move_to_next_xref,
                      _("Move to the next cross reference"))
{
  if (count < 0)
    info_move_to_prev_xref (window, -count);
  else
    {
      size_t last_hist_index, starting_hist_index;
      char *initial_nodename = window->node->nodename;

      last_hist_index = starting_hist_index = window->hist_index - 1;

      while (count > 0)
        {
          if (info_move_to_xref (window, 1))
            {
              last_hist_index = window->hist_index - 1;
              count--;
              continue;
            }

          /* When cursor-movement-scrolls=Off, cycle round the node's
             references. */
          if (!cursor_movement_scrolls_p)
            {
              REFERENCE **r = window->node->references;
              if (r && r[0])
                {
                  /* Choose the first menu or xref entry appearing in this
                     node. */
                  window->point = r[0]->start;
                  window_adjust_pagetop (window);
                  count--;
                  continue;
                }
                
              info_error ("%s", msg_no_xref_node);
              return;
            }

          if (forward_move_node_structure (window, info_scroll_behaviour) != 0
              || !strcmp (window->node->nodename, initial_nodename))
            {
              /*TODO: Print an error. */
              break; /* No later nodes in file, or we are back where we
                         started. */
            }
        }

      /* Go back to the last place a reference was found, or
         the starting place. */
      while (window->hist_index > last_hist_index + 1)
        forget_node (window);

      /* Remove any intermediate nodes. */
      if (last_hist_index != starting_hist_index)
        cleanup_history (window, starting_hist_index + 1, last_hist_index);
    }
}

/* Select the menu item or reference that appears on this line. */
DECLARE_INFO_COMMAND (info_select_reference_this_line,
                      _("Select reference or menu item appearing on this line"))
{
  REFERENCE **ref = window->node->references;

  if (!ref || !*ref) return; /* No references in node */

  info_menu_or_ref_item (window, 1, 1, 0);
}

/* Follow the menu list in MENUS (list of strings terminated by a NULL
   entry) from INITIAL_NODE.  If there is an error, place a message
   in ERROR.  STRICT says whether to accept incomplete strings as
   menu entries, and whether to return the node so far if we can't
   continue at any point (that might be INITIAL_NODE itself), or to
   return null.  This function frees INITIAL_NODE. */
NODE *
info_follow_menus (NODE *initial_node, char **menus, char **error,
		   int strict)
{
  NODE *node = NULL;

  for (; *menus; menus++)
    {
      REFERENCE *entry;
      char *arg = *menus; /* Remember the name of the menu entry we want. */

      debug (3, ("looking for %s in %s:%s", arg, initial_node->fullpath,
		 initial_node->nodename));

      if (!initial_node->references)
        {
          if (error)
            {
              free (*error);
              asprintf (error, _("No menu in node '%s'"),
                        node_printed_rep (initial_node));
            }
          debug (3, ("no menu found"));
          if (!strict)
            return initial_node;
          else
            {
              free_history_node (initial_node);
              return 0;
            }
        }

      /* Find the specified menu item. */
      entry = info_get_menu_entry_by_label (initial_node, arg, !strict);

      /* If we failed to find the reference: */
      if (!entry)
        {
          if (error)
            {
              free (*error);
              asprintf (error, _("No menu item '%s' in node '%s'"),
                        arg, node_printed_rep (initial_node));
            }
          debug (3, ("no entry found"));
          if (!strict)
            return initial_node;
          else
            {
              free_history_node (initial_node);
              return 0;
            }
        }

      debug (3, ("entry: %s, %s", entry->filename, entry->nodename));
      
      /* Try to find this node.  */
      node = info_get_node_with_defaults (entry->filename, entry->nodename,
                                          initial_node);
      if (!node)
        {
	  debug (3, ("no matching node found"));
	  if (error)
            {
              free (*error);
              asprintf (error,
                        _("Unable to find node referenced by '%s' in '%s'"),
                        entry->label,
                        node_printed_rep (initial_node));
            }
          if (strict)
            {
              free_history_node (initial_node);
              return 0;
            }
          else
            return initial_node;
        }

      debug (3, ("node: %s, %s", node->fullpath, node->nodename));
      
      /* Success.  Go round the loop again.  */
      free_history_node (initial_node);
      initial_node = node;
    }

  return initial_node;
}

/* Split STR into individual node names by writing null bytes in wherever
   there are commas and constructing a list of the resulting pointers.
   (We can do this since STR has had canonicalize_whitespace called on it.)
   Return array terminated with NULL.  */

static char **
split_list_of_nodenames (char *str)
{
  unsigned len = 2;
  char **nodes = xmalloc (len * sizeof (char *));

  nodes[len - 2] = str;

  while (*str++)
    {
      if (*str == ',')
        {
          *str++ = 0;           /* get past the null byte */
          len++;
          nodes = xrealloc (nodes, len * sizeof (char *));
          nodes[len - 2] = str;
        }
    }

  nodes[len - 1] = NULL;

  return nodes;
}


/* Read a line of input which is a sequence of menus (starting from
   dir), and follow them.  */
DECLARE_INFO_COMMAND (info_menu_sequence,
   _("Read a list of menus starting from dir and follow them"))
{
  char *line = info_read_in_echo_area (_("Follow menus: "));

  /* If the user aborted, quit now. */
  if (!line)
    {
      info_abort_key (window, 0);
      return;
    }

  canonicalize_whitespace (line);

  if (*line)
    {
      char *error = 0;
      NODE *dir_node = get_dir_node ();
      char **nodes = split_list_of_nodenames (line);
      NODE *node;

      /* If DIR_NODE is NULL, they might be reading a file directly,
         like in "info -d . -f ./foo".  Try using "Top" instead.  */
      if (!dir_node)
        dir_node = info_get_node (window->node->fullpath, 0);

      /* If we still cannot find the starting point, give up. */
      if (!dir_node)
        info_error (msg_cant_find_node, "Top");
      else
        {
          node = info_follow_menus (dir_node, nodes, &error, 0);
          info_set_node_of_window (window, node);
          if (error)
            show_error_node (error);
        }

      free (nodes);
    }
  free (line);
}


/* **************************************************************** */
/*                                                                  */
/*                 Navigation of document structure                 */
/*                                                                  */
/* **************************************************************** */

/* Get the node pointed to by the LABEL pointer of WINDOW->node into WINDOW.
   Display error message if there is no such pointer, and return zero. */
static int
info_handle_pointer (char *label, WINDOW *window)
{
  char *description;
  NODE *node;

  if (!strcmp (label, "Up"))
    description = window->node->up;
  else if (!strcmp (label, "Next"))
    description = window->node->next;
  else if (!strcmp (label, "Prev"))
    description = window->node->prev;

  if (!description)
    {
      info_error (msg_no_pointer, label);
      return 0;
    }

  info_parse_node (description);
  node = info_get_node_with_defaults (info_parsed_filename,
                                      info_parsed_nodename,
                                      window->node);
  if (!node)
    {
      if (info_recent_file_error)
        info_error ("%s", info_recent_file_error);
      else
        info_error (msg_cant_find_node, description);
      return 0;
    }

  /* If we are going up, set the cursor position to the last place it
     was in the node. */
  if (strcmp (label, "Up") == 0)
    {
      int i;

      for (i = window->hist_index - 1; i >= 0; i--)
        {
          NODE *p = window->hist[i]->node;

          if (p->fullpath && !strcmp (p->fullpath, node->fullpath)
              && p->nodename && !strcmp (p->nodename, node->nodename))
            break;
        }

      if (i >= 0)
        node->display_pos = window->hist[i]->point;
    }

  info_set_node_of_window (window, node);
  return 1;
}

/* Make WINDOW display the "Next:" node of the node currently being
   displayed. */
DECLARE_INFO_COMMAND (info_next_node, _("Select the Next node"))
{
  info_handle_pointer ("Next", window);
}

/* Make WINDOW display the "Prev:" node of the node currently being
   displayed. */
DECLARE_INFO_COMMAND (info_prev_node, _("Select the Prev node"))
{
  info_handle_pointer ("Prev", window);
}

/* Make WINDOW display the "Up:" node of the node currently being
   displayed. */
DECLARE_INFO_COMMAND (info_up_node, _("Select the Up node"))
{
  info_handle_pointer ("Up", window);
}

/* Make WINDOW display the last node of this info file. */
DECLARE_INFO_COMMAND (info_last_node, _("Select the last node in this file"))
{
  register int i;
  FILE_BUFFER *fb = file_buffer_of_window (window);
  NODE *node = NULL;

  if (fb && fb->tags)
    {
      int last_node_tag_idx = -1;

      /* If no explicit argument, or argument of zero, default to the
         last node.  */
      if (count == 0 || (count == 1 && !info_explicit_arg))
        count = -1;
      for (i = 0; count && fb->tags[i]; i++)
        if (fb->tags[i]->cache.nodelen != 0) /* don't count anchor tags */
          {
            count--;
            last_node_tag_idx = i;
          }
      if (count > 0)
        i = last_node_tag_idx + 1;
      if (i > 0)
        node = info_get_node (fb->filename, fb->tags[i - 1]->nodename);
    }

  if (!node)
    info_error ("%s", _("This window has no additional nodes"));
  else
    info_set_node_of_window (window, node);
}

/* Make WINDOW display the first node of this info file. */
DECLARE_INFO_COMMAND (info_first_node, _("Select the first node in this file"))
{
  FILE_BUFFER *fb = file_buffer_of_window (window);
  NODE *node = NULL;

  /* If no explicit argument, or argument of zero, default to the
     first node.  */
  if (count == 0)
    count = 1;
  if (fb && fb->tags)
    {
      register int i;
      int last_node_tag_idx = -1;

      for (i = 0; count && fb->tags[i]; i++)
        if (fb->tags[i]->cache.nodelen != 0) /* don't count anchor tags */
          {
            count--;
            last_node_tag_idx = i;
          }
      if (count > 0)
        i = last_node_tag_idx + 1;
      if (i > 0)
        node = info_get_node (fb->filename, fb->tags[i - 1]->nodename);
    }

  if (!node)
    info_error ("%s", _("This window has no additional nodes"));
  else
    info_set_node_of_window (window, node);
}

/* Controls what to do when a scrolling command is issued at the end of the
   last node. */
int scroll_last_node = SLN_Stop;

/* Move to 1st menu item, Next, Up/Next, or error in this window.  Return
   non-zero on error, 0 on success.  Display an error message if there is an
   error. */
static int
forward_move_node_structure (WINDOW *window, int behaviour)
{
  if (window->node->flags & (N_IsInternal | N_IsManPage))
    return 1;

  switch (behaviour)
    {
    case IS_PageOnly:
      info_error ("%s", msg_at_node_bottom);
      return 1;

    case IS_NextOnly:
      return !info_handle_pointer ("Next", window);
      break;

    case IS_Continuous:
      {
        /* If this node contains a menu, select its first entry.  Indices
           are an exception, as their menus lead nowhere meaningful.  Likewise
           for dir nodes. */
        if (!(window->node->flags & N_IsIndex)
            && !(window->node->flags & N_IsDir))
          {
            REFERENCE *entry;

            if (entry = select_menu_digit (window, '1'))
              {
                info_select_reference (window, entry);
                return 0;
              }
          }

        /* Okay, this node does not contain a menu.  If it contains a
           "Next:" pointer, use that. */
        if (window->node->next)
          {
            info_handle_pointer ("Next", window);
            return 0;
          }

        /* Okay, there wasn't a "Next:" for this node.  Move "Up:" until we
           can move "Next:".  If that isn't possible, complain that there
           are no more nodes. */
        {
          int up_counter;
          int starting_hist_index = window->hist_index;

          /* Back up through the "Up:" pointers until we have found a "Next:"
             that isn't the same as the first menu item found in that node. */
          up_counter = 0;
          while (1)
            {
              if (window->node->up)
                {
                  REFERENCE *entry;

                  if (!info_handle_pointer ("Up", window))
                    return 1;

                  up_counter++;

                  /* If no "Next" pointer, keep backing up. */
                  if (!window->node->next)
                    continue;

                  /* If this node's first menu item is the same as this node's
                     Next pointer, keep backing up. */
                  entry = select_menu_digit (window, '1');
                  if (entry && !strcmp (window->node->next, entry->nodename))
                    continue;

                  /* This node has a "Next" pointer, and it is not the
                     same as the first menu item found in this node. */
                  info_handle_pointer ("Next", window);

                  /* Don't include intermediate nodes in the window's
                     history.  */
                  cleanup_history (window, starting_hist_index,
                                   window->hist_index - 1);
                  return 0;
                }
              else
                {
                  /* No more "Up" pointers.  We are at the last node in the
                     file. */
                  register int i;

                  for (i = 0; i < up_counter; i++)
                    forget_node (window);

                  switch (scroll_last_node)
                    {
                    case SLN_Stop:
                      info_error ("%s",
                                  _("No more nodes within this document"));
                      return 1;
                      
                    case SLN_Top:
                      info_parse_and_select ("Top", window);
                      return 0;
                      
                    default:
                      abort ();
                    }
                }
            }
        }
        break;
      }
    }
  return 0;
}

/* Move to earlier node in node hierarchy in WINDOW depending on BEHAVIOUR.
   Display an error message if node wasn't changed. */
static int
backward_move_node_structure (WINDOW *window, int behaviour)
{
  if (window->node->flags & (N_IsInternal | N_IsManPage))
    return 1;

  switch (behaviour)
    {
    case IS_PageOnly:
      info_error ("%s", msg_at_node_top);
      return 1;

    case IS_NextOnly:
      return !info_handle_pointer ("Prev", window);
      break;

    case IS_Continuous:
      if (window->node->up)
        {
          /* If up is the dir node, we are at the top node.
             Don't do anything. */
          if (!strncasecmp (window->node->up, "(dir)", strlen ("(dir)")))
            {
              info_error ("%s", _("No 'Prev' or 'Up' for this node within this document"));
              return 1;
            }
          /* If 'Prev' and 'Up' are the same, we are at the first node
             of the 'Up' node's menu. Go to up node. */
          else if (window->node->prev
              && !strcmp(window->node->prev, window->node->up))
            {
              info_handle_pointer ("Up", window);
            }
          /* Otherwise, go to 'Prev' node and go down the last entry
             in the menus as far as possible. */
          else if (window->node->prev)
            {
              int starting_hist_index = window->hist_index;
              info_handle_pointer ("Prev", window);
              if (!(window->node->flags & N_IsIndex))
                {
                  while (1)
                    {
                      REFERENCE *entry = select_menu_digit (window, '0');
                      if (!entry)
                        break;
                      if (!info_select_reference (window, entry))
                        break;
                    }
                  /* Don't include intermediate nodes in the window's
                     history.  */
                  cleanup_history (window, starting_hist_index,
                                   window->hist_index - 1);
                }
            }
          else /* 'Up' but no 'Prev' */
            info_handle_pointer ("Up", window);
        }
      else if (window->node->prev) /* 'Prev' but no 'Up' */
        info_handle_pointer ("Prev", window);
      else
        {
          info_error ("%s", _("No 'Prev' or 'Up' for this node within this document"));
          return 1;
        }

      break; /* case IS_Continuous: */
    }
  return 0;
}

void info_global_prev_node (WINDOW *, int count);

/* Move continuously forward through the node structure of this info file. */
DECLARE_INFO_COMMAND (info_global_next_node,
                      _("Move forwards or down through node structure"))
{
  if (count < 0)
    info_global_prev_node (window, -count);
  else
    {
      while (count)
        {
          if (forward_move_node_structure (window, IS_Continuous))
            break;
          count--;
        }
    }
}

/* Move continuously backward through the node structure of this info file. */
DECLARE_INFO_COMMAND (info_global_prev_node,
                      _("Move backwards or up through node structure"))
{
  if (count < 0)
    info_global_next_node (window, -count);
  else
    {
      while (count)
        {
          if (backward_move_node_structure (window, IS_Continuous))
            break;
          count--;
        }
    }
}


/* **************************************************************** */
/*                                                                  */
/*                      Info Node Commands                          */
/*                                                                  */
/* **************************************************************** */

/* Read a line of input which is a node name, and go to that node. */
DECLARE_INFO_COMMAND (info_goto_node, _("Read a node name and select it"))
{
  char *line;

#define GOTO_COMPLETES
#if defined (GOTO_COMPLETES)
  /* Build a completion list of all of the known nodes. */
  {
    register int fbi, i;
    FILE_BUFFER *current;
    REFERENCE **items = NULL;
    size_t items_index = 0;
    size_t items_slots = 0;

    current = file_buffer_of_window (window);

    for (fbi = 0; info_loaded_files && info_loaded_files[fbi]; fbi++)
      {
        FILE_BUFFER *fb;
        REFERENCE *entry;
        int this_is_the_current_fb;

        fb = info_loaded_files[fbi];
        this_is_the_current_fb = (current == fb);

        entry = xmalloc (sizeof (REFERENCE));
        entry->filename = entry->nodename = NULL;
        entry->label = xmalloc (4 + strlen (fb->filename));
        sprintf (entry->label, "(%s)*", fb->filename);

        add_pointer_to_array (entry, items_index, items, items_slots, 10);

        if (fb->tags)
          {
            for (i = 0; fb->tags[i]; i++)
              {
                entry = xmalloc (sizeof (REFERENCE));
                entry->filename = entry->nodename = NULL;
                if (this_is_the_current_fb)
                  entry->label = xstrdup (fb->tags[i]->nodename);
                else
                  {
                    entry->label = xmalloc
                      (4 + strlen (fb->filename) +
                       strlen (fb->tags[i]->nodename));
                    sprintf (entry->label, "(%s)%s",
                             fb->filename, fb->tags[i]->nodename);
                  }

                add_pointer_to_array (entry, items_index, items, 
                                      items_slots, 100);
              }
          }
      }
    line = info_read_maybe_completing (_("Goto node: "), items);
    info_free_references (items);
  }
#else /* !GOTO_COMPLETES */
  line = info_read_in_echo_area (_("Goto node: "));
#endif /* !GOTO_COMPLETES */

  /* If the user aborted, quit now. */
  if (!line)
    {
      info_abort_key (window, 0);
      return;
    }

  canonicalize_whitespace (line);

  if (*line)
    info_parse_and_select (line, window);

  free (line);
}

static NODE *
find_invocation_node_by_nodename (FILE_BUFFER *fb, char *program)
{
  NODE *node = 0;
  TAG **n;
  char *try1, *try2;
  n = fb->tags;
  if (!n)
    return 0;

  asprintf (&try1, "Invoking %s", program);
  asprintf (&try2, "%s invocation", program);
  for (; *n; n++)
    {
      if ((*n)->nodename
          && (!strcasecmp ((*n)->nodename, try1)
              || !strcasecmp ((*n)->nodename, try2)))
        {
          node = info_get_node_of_file_buffer (fb, (*n)->nodename);
          break;
        }
    }
  free (try1); free (try2);

  return node;
}

/* Find the node in the file with name FILE that is the best candidate to
   list the PROGRAM's invocation info and its command-line options, by looking
   for menu items and chains of menu items with characteristic names.  This
   function frees NODE.  Return value should be freed by caller with 
   info_reference_free.  */
REFERENCE *
info_intuit_options_node (NODE *node, char *program)
{
  /* The list of node names typical for GNU manuals where the program
     usage and specifically the command-line arguments are described.
     This is pure heuristics.  I gathered these node names by looking
     at all the Info files I could put my hands on.  If you are
     looking for evidence to complain to the GNU project about
     non-uniform style of documentation, here you have your case!  */
  static const char *invocation_nodes[] = {
    "%s invocation",
    "Invoking %s",
    "Preliminaries",    /* m4 has Invoking under Preliminaries! */
    "Invocation",
    "Command Arguments",/* Emacs */
    "Invoking `%s'",
    "%s options",
    "Options",
    "Option ",          /* e.g. "Option Summary" */
    "Invoking",
    "All options",      /* tar, paxutils */
    "Arguments",
    "%s cmdline",       /* ar */
    "%s",               /* last resort */
    (const char *)0
  };

  char *filename = node->fullpath;
  if (!strcmp ("Top", node->nodename))
    {
      /* Look through the list of nodes (and anchors) in the file for a node to 
         start at.  There may be an invocation node that is not listed in the 
         top-level menu (this is the case for the Bash 4.2 manual), or it may 
         be referred to with an anchor ("Invoking makeinfo" in Texinfo 
         manual).  */
      FILE_BUFFER *fb;
      NODE *n;
      fb = info_find_file (filename);
      if (!fb)
        return 0;
      n = find_invocation_node_by_nodename (fb, program);
      if (n)
        {
          free_history_node (node);
          node = n;
        }
    }

  /* We keep looking deeper and deeper in the menu structure until
     there are no more menus or no menu items from the above list.
     Some manuals have the invocation node sitting 3 or 4 levels deep
     in the menu hierarchy...  */
  while (1)
    {
      const char **try_node;
      REFERENCE *entry = NULL;

      /* If no menu in this node, stop here.  Perhaps this node
         is the one they need.  */
      if (!node->references)
        break;

      /* Look for node names typical for usage nodes in this menu.  */
      for (try_node = invocation_nodes; *try_node; try_node++)
        {
          char *nodename;

          nodename = xmalloc (strlen (program) + strlen (*try_node));
          sprintf (nodename, *try_node, program);
          /* The last resort "%s" is dangerous, so we restrict it
             to exact matches here.  */
          entry = info_get_menu_entry_by_label
            (node, nodename, strcmp (*try_node, "%s"));
          free (nodename);
          if (entry)
            break;
        }

      if (!entry)
        break;

      /* Go down into menu, and repeat. */ 

      if (!entry->filename)
        entry->filename = xstrdup (filename);

      {
        NODE *node2;
        node2 = info_get_node (entry->filename, entry->nodename);
        if (!node2)
          break;
        free_history_node (node);
        node = node2;
      }
    }

  {
    char *n = node->nodename;
    node->nodename = 0;
    free_history_node (node);
    return info_new_reference (filename, n);
  }
}

/* Given a name of an Info file, find the name of the package it describes by 
   removing the leading directories and extensions.  Return value should be 
   freed by caller. */
char *
program_name_from_file_name (char *file_name)
{
  int i;
  char *program_name = xstrdup (filename_non_directory (file_name));

  for (i = strlen (program_name) - 1; i > 0; i--)
    if (program_name[i] == '.'
        && (FILENAME_CMPN (program_name + i, ".info", 5) == 0
            || FILENAME_CMPN (program_name + i, ".inf", 4) == 0
#ifdef __MSDOS__
            || FILENAME_CMPN (program_name + i, ".i", 2) == 0
#endif
            || isdigit (program_name[i + 1]))) /* a man page foo.1 */
      {
        program_name[i] = 0;
        break;
      }
  return program_name;
}

DECLARE_INFO_COMMAND (info_goto_invocation_node,
                      _("Find the node describing program invocation"))
{
  const char *invocation_prompt = _("Find Invocation node of [%s]: ");
  char *program_name, *line;
  char *default_program_name, *prompt, *file_name;
  NODE *top_node;
  REFERENCE *invocation_ref;

  /* Intuit the name of the program they are likely to want.
     We use the file name of the current Info file as a hint.  */
  file_name = window->node->fullpath;
  default_program_name = program_name_from_file_name (file_name);

  prompt = xmalloc (strlen (default_program_name) +
		    strlen (invocation_prompt));
  sprintf (prompt, invocation_prompt, default_program_name);
  line = info_read_in_echo_area (prompt);
  free (prompt);
  if (!line)
    {
      info_abort_key (window, 0);
      return;
    }
  if (*line)
    program_name = line;
  else
    program_name = default_program_name;

  /* In interactive usage they'd probably expect us to begin looking
     from the Top node.  */
  top_node = info_get_node (file_name, 0);
  if (!top_node)
    info_error (msg_cant_find_node, "Top");

  invocation_ref = info_intuit_options_node (top_node, program_name);

  /* We've got our best shot at the invocation node.  Now select it.  */
  if (invocation_ref)
    {
      info_select_reference (window, invocation_ref);
      info_reference_free (invocation_ref);
    }

  free (line);
  free (default_program_name);
}

DECLARE_INFO_COMMAND (info_man, _("Read a manpage reference and select it"))
{
  char *line;

  line = info_read_in_echo_area (_("Get Manpage: "));

  if (!line)
    {
      info_abort_key (window, 0);
      return;
    }

  canonicalize_whitespace (line);

  if (*line)
    {
      NODE *manpage = info_get_node (MANPAGE_FILE_BUFFER_NAME, line);
      if (manpage)
        info_set_node_of_window (window, manpage);
    }

  free (line);
}

/* Move to the "Top" node in this file. */
DECLARE_INFO_COMMAND (info_top_node, _("Select the node 'Top' in this file"))
{
  info_parse_and_select ("Top", window);
}

/* Move to the node "(dir)Top". */
DECLARE_INFO_COMMAND (info_dir_node, _("Select the node '(dir)'"))
{
  info_parse_and_select ("(dir)Top", window);
}

DECLARE_INFO_COMMAND (info_display_file_info,
		      _("Show full file name of node being displayed"))
{
  if (window->node->fullpath && *window->node->fullpath)
    {
      int line = window_line_of_point (window);
      window_message_in_echo_area ("File name: %s, line %d of %ld (%ld%%)",
                                   window->node->subfile
                                   ? window->node->subfile
                                   : window->node->fullpath,
                                   line, window->line_count,
				   line * 100 / window->line_count);
    }
  else
    window_message_in_echo_area ("Internal node");
}

DECLARE_INFO_COMMAND (info_history_node,
                      _("Select the most recently selected node"))
{
  if (window->hist_index > 1)
    forget_node (window);
  else
    info_error (_("No earlier node in history"));
}

/* Read the name of a file and select the entire file. */
DECLARE_INFO_COMMAND (info_view_file, _("Read the name of a file and select it"))
{
  char *line;

  line = info_read_in_echo_area (_("Find file: "));
  if (!line)
    {
      info_abort_key (active_window, 1);
      return;
    }

  if (*line)
    {
      NODE *node;

      node = info_get_node (line, "*");
      if (!node)
        {
          if (info_recent_file_error)
            info_error ("%s", info_recent_file_error);
          else
            info_error (_("Cannot find '%s'"), line);
        }
      else
        info_set_node_of_window (window, node);

      free (line);
    }
}

/* **************************************************************** */
/*                                                                  */
/*                 Dumping and Printing Nodes                       */
/*                                                                  */
/* **************************************************************** */

enum
  {
    DUMP_SUCCESS,
    DUMP_INFO_ERROR,
    DUMP_SYS_ERROR
  };

static int dump_node_to_stream (char *filename, char *nodename,
				FILE *stream, int dump_subnodes);
static void initialize_dumping (void);

/* Dump the nodes specified with REFERENCES to the file named
   in OUTPUT_FILENAME.  If DUMP_SUBNODES is set, recursively dump
   the nodes which appear in the menu of each node dumped. */
void
dump_nodes_to_file (REFERENCE **references,
                    char *output_filename, int dump_subnodes)
{
  int i;
  FILE *output_stream;
  
  if (!references)
    return;

  /* Get the stream to print the nodes to.  Special case of an output
     filename of "-" means to dump the nodes to stdout. */
  if (strcmp (output_filename, "-") == 0)
    output_stream = stdout;
  else
    output_stream = fopen (output_filename, "w");

  if (!output_stream)
    {
      info_error (_("Could not create output file '%s'"), output_filename);
      return;
    }

  initialize_dumping ();

  /* Print each node to stream. */
  for (i = 0; references[i]; i++)
    {
      if (dump_node_to_stream (references[i]->filename,
                               references[i]->nodename,
                               output_stream,
                               dump_subnodes) == DUMP_SYS_ERROR)
	{
	  info_error (_("error writing to %s: %s"), output_filename,
                      strerror (errno));
	  exit (EXIT_FAILURE);
	}
    }
  
  if (output_stream != stdout)
    fclose (output_stream);

  debug (1, (_("closing %s"), output_filename));
}

/* A place to remember already dumped nodes. */
static struct info_namelist_entry *dumped_already;

static void
initialize_dumping (void)
{
  info_namelist_free (dumped_already);
  dumped_already = NULL;
}

/* Get and print the node specified by FILENAME and NODENAME to STREAM.
   If DUMP_SUBNODES is non-zero, recursively dump the nodes which appear
   in the menu of each node dumped. */
static int
dump_node_to_stream (char *filename, char *nodename,
		     FILE *stream, int dump_subnodes)
{
  register int i;
  NODE *node;

  node = info_get_node (filename, nodename);

  if (!node)
    {
      if (info_recent_file_error)
        info_error ("%s", info_recent_file_error);
      else
        {
          if (filename && *nodename != '(')
            info_error (msg_cant_file_node,
                        filename_non_directory (filename),
                        nodename);
          else
            info_error (msg_cant_find_node, nodename);
        }
      return DUMP_INFO_ERROR;
    }

  /* If we have already dumped this node, don't dump it again. */
  if (info_namelist_add (&dumped_already, node->nodename))
    {
      free_history_node (node);
      return DUMP_SUCCESS;
    }

  /* Maybe we should print some information about the node being output. */
  debug (1, (_("writing node %s..."), node_printed_rep (node)));

  if (write_node_to_stream (node, stream))
    {
      free_history_node (node);
      return DUMP_SYS_ERROR;
    }

  /* If we are dumping subnodes, get the list of menu items in this node,
     and dump each one recursively. */
  if (dump_subnodes)
    {
      REFERENCE **menu = NULL;

      /* If this node is an Index, do not dump the menu references. */
      if (string_in_line ("Index", node->nodename) == -1)
        menu = node->references;

      if (menu)
        {
          for (i = 0; menu[i]; i++)
            {
              if (REFERENCE_MENU_ITEM != menu[i]->type) continue;

              /* We don't dump Info files which are different than the
                 current one. */
              if (!menu[i]->filename)
                if (dump_node_to_stream (filename, menu[i]->nodename,
                      stream, dump_subnodes) == DUMP_SYS_ERROR)
                  {
                    free_history_node (node);
                    return DUMP_SYS_ERROR;
                  }
            }
        }
    }

  free_history_node (node);
  return DUMP_SUCCESS;
}

#if !defined (DEFAULT_INFO_PRINT_COMMAND)
#  define DEFAULT_INFO_PRINT_COMMAND "lpr"
#endif /* !DEFAULT_INFO_PRINT_COMMAND */

DECLARE_INFO_COMMAND (info_print_node,
 _("Pipe the contents of this node through INFO_PRINT_COMMAND"))
{
  FILE *printer_pipe;
  char *print_command = getenv ("INFO_PRINT_COMMAND");
  int piping = 0;

  if (!print_command || !*print_command)
    print_command = DEFAULT_INFO_PRINT_COMMAND;

  /* Note that on MS-DOS/MS-Windows, this MUST open the pipe in the
     (default) text mode, since the printer drivers there need to see
     DOS-style CRLF pairs at the end of each line.

     FIXME: if we are to support Mac-style text files, we might need
     to convert the text here.  */

  /* INFO_PRINT_COMMAND which says ">file" means write to that file.
     Presumably, the name of the file is the local printer device.  */
  if (*print_command == '>')
    printer_pipe = fopen (++print_command, "w");
  else
    {
      printer_pipe = popen (print_command, "w");
      piping = 1;
    }

  if (!printer_pipe)
    {
      info_error (_("Cannot open pipe to '%s'"), print_command);
      return;
    }

  /* Maybe we should print some information about the node being output. */
  debug (1, (_("printing node %s..."), node_printed_rep (window->node)));

  write_node_to_stream (window->node, printer_pipe);
  if (piping)
    pclose (printer_pipe);
  else
    fclose (printer_pipe);

  debug (1, (_("finished printing node %s"), node_printed_rep (window->node)));
}

int
write_node_to_stream (NODE *node, FILE *stream)
{
  return fwrite (node->contents, node->nodelen, 1, stream) != 1;
}

/* **************************************************************** */
/*                                                                  */
/*                    Info Searching Commands                       */
/*                                                                  */
/* **************************************************************** */

/* Variable controlling the garbage collection of files briefly visited
   during searches.  Such files are normally gc'ed, unless they were
   compressed to begin with.  If this variable is non-zero, it says
   to gc even those file buffer contents which had to be uncompressed. */
int gc_compressed_files = 0;

static char *search_string = NULL;
static int isearch_is_active = 0;

static int last_search_direction = 0;
static int last_search_case_sensitive = 0;

/* Whether to use regexps or not for search.  */
static int use_regex = 1;

/* Toggle the usage of regular expressions in searches. */
DECLARE_INFO_COMMAND (info_toggle_regexp,
              _("Toggle the usage of regular expressions in searches"))
{
  use_regex = !use_regex;
  window_message_in_echo_area (use_regex
                               ? _("Using regular expressions for searches")
                               : _("Using literal strings for searches"));
}

/* Search for STRING in NODE starting at START.  The DIR argument says which
   direction to search in.  If it is positive, search forward, else backwards.

   If the string was found, return its location in POFF, set the
   window's node, its point to be the found string, and readjust
   the window's pagetop.  NODE can be retained as a field within WINDOW.

   WINDOW->matches should be a list of matches for NODE->contents, or null.
   If new matches are calculated, they are saved in WINDOW->matches.  */
static enum search_result
info_search_in_node_internal (WINDOW *window, NODE *node,
                              char *string, long start,
                              int dir, int case_sensitive,
                              int match_regexp, long *poff)
{
  enum search_result result = search_not_found;

  long start1, end1;
  int match_index;
  long new_point;

  MATCH_STATE matches;
    
  /* Check if we need to calculate new results. */
  if (!matches_ready (&window->matches)
      || strcmp (window->search_string, string)
      || window->search_is_case_sensitive != case_sensitive)
    {
      free_matches (&window->matches);
      free (window->search_string);
      window->search_string = xstrdup (string);
      window->search_is_case_sensitive = case_sensitive;
      result = regexp_search (string, !match_regexp, !case_sensitive,
                              node->contents, node->nodelen, &matches);
    }
  else
    {
      matches = window->matches;
      result = search_success;
    }
  
  if (result != search_success)
    return result;

  if (node->flags & N_Simple)
    {
      /* There are matches in the node, but it hasn't been scanned yet.  Get
         the node again, because its contents may differ. */
      enum search_result subresult;
      NODE *full_node;

      free_matches (&matches);
      full_node = info_get_node (node->fullpath, node->nodename);
      subresult = info_search_in_node_internal (window, full_node,
                                    string, start,
                                    dir, case_sensitive,
                                    match_regexp, poff);
      if (window->node != full_node)
        free (full_node);
      return subresult;
    }

  if (dir > 0)
    {
      start1 = start;
      end1 = node->nodelen;
    }
  else
    {
      start1 = 0;
      end1 = start + 1; /* include start byte in search area */
    }

  if (start1 < node->body_start)
    start1 = node->body_start;
  if (end1 < node->body_start)
    end1 = node->body_start;
  
  result = match_in_match_list (&matches, start1, end1, dir, &match_index);
  if (result != search_success)
    return result;

  *poff = match_by_index (&matches, match_index).rm_so;

  window->flags |= W_UpdateWindow;
  if (window->node != node)
    info_set_node_of_window (window, node);

  window->matches = matches;

  if (isearch_is_active && dir > 0)
    new_point = match_by_index (&matches, match_index).rm_eo;
  else
    new_point = match_by_index (&matches, match_index).rm_so;

  window->point = new_point;

  return result;
}

/* Search for STRING starting in WINDOW, starting at *START_OFF.

   If the string is found in this node, set point to that position.
   Otherwise, get the file buffer associated with WINDOW's node, and search
   through each node in that file.

   If the search succeeds, return non-zero.  *START_OFF is given the start of
   the found string instance.  Set node and point of window to where the string
   was found.  (If the variable ISEARCH_IS_ACTIVE is non-zero and the search is
   forwards, the point is set to the end of the search.)

   Return non-zero if the search fails. */
static int
info_search_internal (char *string, WINDOW *window,
		      int dir, int case_sensitive,
                      long *start_off)
{
  register int i;
  FILE_BUFFER *file_buffer;
  long start;
  enum search_result result;
  int search_other_nodes = 1;
  int number_of_tags = -1, starting_tag = -1, current_tag = -1;
  NODE *node = window->node; /* Node to search in. */
  char *subfile_name = 0;
  TAG *tag;
  char *msg = 0;
  int first_time = 1;
  
  /* If this node isn't part of a larger file, search this node only. */
  file_buffer = file_buffer_of_window (window);
  if (!file_buffer || !file_buffer->tags
      || !strcmp (window->node->nodename, "*"))
    search_other_nodes = 0;

  /* Find number of tags and current tag. */
  if (search_other_nodes)
    {
      char *initial_nodename = window->node->nodename;

      for (i = 0; file_buffer->tags[i]; i++)
        if (strcmp (initial_nodename, file_buffer->tags[i]->nodename) == 0)
          {
            starting_tag = i;
            subfile_name = file_buffer->tags[i]->filename;
          }

      number_of_tags = i;

      /* Our tag wasn't found.  This shouldn't happen. */
      if (starting_tag == -1)
        return -1;
      current_tag = starting_tag;
    }

  /* Set starting position of search. */
  start = *start_off;
  
  /* Search through subsequent nodes, wrapping around to the top
     of the Info file until we find the string or return to this
     window's node and point. */
  while (1)
    {
      result = info_search_in_node_internal (window, node, string, start, dir,
                 case_sensitive, use_regex, start_off);

      if (node != window->node)
        free_history_node (node);

      if (result == search_invalid)
        return 1;

      if (result == search_success)
        {
          if (!echo_area_is_active)
            {
              if (msg)
                window_message_in_echo_area ("%s", _(msg));
              else
                window_clear_echo_area ();
            }
          return 0;
        }

      if (!search_other_nodes)
        break;

      /* If we've searched our starting node twice, there are no matches.
         Bail out.  (We searched the second time in case there were matches 
         before the starting offset.) */
      if (current_tag == starting_tag && !first_time)
        break;
      first_time = 0;
  
      /* Find the next tag that isn't an anchor.  */
      for (i = current_tag + dir; ; i += dir)
        {
          if (i < 0)
            {
              msg = N_("Search continued from the end of the document");
              i = number_of_tags - 1;
            }
          else if (i == number_of_tags)
            {
              msg = N_("Search continued from the beginning of the document");
              i = 0;
            }
          
          tag = file_buffer->tags[i];
          if (tag->cache.nodelen != 0)
            break;
        }

      current_tag = i;

      /* Display message when searching a new subfile. */
      if (!echo_area_is_active && tag->filename != subfile_name)
        {
          subfile_name = tag->filename;

          window_message_in_echo_area
            (_("Searching subfile %s ..."),
             filename_non_directory (subfile_name));
        }

      /* Get a new node to search in. */
      free_matches (&window->matches);

      node = info_node_of_tag_fast (file_buffer, &tag);
      if (!node)
        {
          /* If not doing i-search... */
          if (!echo_area_is_active)
            {
              if (info_recent_file_error)
                info_error ("%s", info_recent_file_error);
              else
                info_error (msg_cant_file_node,
                            filename_non_directory (file_buffer->filename),
                            tag->nodename);
            }
          return -1;
        }

      if (dir < 0)
        start = tag->cache.nodelen;
      else
        start = 0;

      /* Allow C-g to quit the search, failing it if pressed. */
      fill_input_buffer (0); \
      if (info_input_buffer[pop_index] == Control ('g'))
        goto funexit;
    }

  /* Not in interactive search. */
  if (!echo_area_is_active)
    info_error ("%s", _("Search failed"));

funexit:
  return -1;
}

/* Minimal length of a search string */
int min_search_length = 1;

/* Read a string from the user, storing the result in SEARCH_STRING.  Return 0 
   if the user aborted. */
static int
ask_for_search_string (int case_sensitive, int use_regex, int direction)
{
  char *line, *prompt;

  if (search_string)
    asprintf (&prompt, _("%s%s%s [%s]: "),
             use_regex ? _("Regexp search") : _("Search"),
             case_sensitive ? _(" case-sensitively") : "",
             direction < 0 ? _(" backward") : "",
             search_string);
  else
    asprintf (&prompt, _("%s%s%s: "),
             use_regex ? _("Regexp search") : _("Search"),
             case_sensitive ? _(" case-sensitively") : "",
             direction < 0 ? _(" backward") : "");

  line = info_read_in_echo_area (prompt);
  free (prompt);

  if (!line) /* User aborted. */
    {
      return 0;
    }
  if (!*line)
    {
      free (line);
      return 1;
    }

  if (mbslen (line) < min_search_length)
    {
      info_error ("%s", _("Search string too short"));
      free (line);
      return 1;
    }

  free (search_string);
  search_string = line;

  return 1;
}

/* Common entry point for the search functions.  Arguments:
   WINDOW           The window to search in
   COUNT            The sign of this argument defines the search
                    direction (negative for searching backwards);
		    its absolute value gives number of repetitions.
   CASE_SENSITIVE   Whether the search is case-sensitive or not.
*/
static void
info_search_1 (WINDOW *window, int count, int case_sensitive)
{
  int result;
  int direction;
  long start_off;
  char *p;

  if (count < 0)
    {
      direction = -1;
      count = -count;
    }
  else
    {
      direction = 1;
      if (count == 0)
        count = 1;      /* for backward compatibility */
    }

  if (!ask_for_search_string (case_sensitive, use_regex, direction)
      || !search_string)
    return;

  start_off = window->point + direction;
  
  /* If the search string includes upper-case letters, make the search
     case-sensitive.  */
  if (case_sensitive == 0)
    for (p = search_string; *p; p++)
      if (isupper (*p))
        {
          case_sensitive = 1;
          break;
        }

  last_search_direction = direction;
  last_search_case_sensitive = case_sensitive;

  for (result = 0; result == 0 && count--; )
    result = info_search_internal (search_string,
                                   active_window, direction, case_sensitive,
				   &start_off);

  window_adjust_pagetop (window);

  /* Perhaps free the unreferenced file buffers that were searched, but
     not retained. */
  gc_file_buffers_and_nodes ();
}

/* Set *T to an offset within the tags table of the node referenced by R,
   using WINDOW for defaults.  *FB is set to the file buffer structure for
   the node. */
static int
tag_of_reference (REFERENCE *r, WINDOW *window, FILE_BUFFER **fb, TAG ***t)
{
  char *filename, *nodename;
  int i;

  filename = r->filename;
  nodename = r->nodename;
  if (!filename)
    filename = window->node->fullpath;
  if (!nodename || !*nodename)
    nodename = "Top";

  *fb = info_find_file (filename);
  if (!*fb)
    return 0;

  for (i = 0; *(*t = &(*fb)->tags[i]); i++)
    if (!strcmp (nodename, (**t)->nodename))
      goto found_tag;
  return 0;
found_tag: ;
  return 1;
}

/* Value for NODE.active_menu */
#define BEFORE_MENU -99

static void tree_search_check_node (WINDOW *window);
static void tree_search_check_node_backwards (WINDOW *window);

/* Search in WINDOW->node, and nodes reachable by menus from it, for
   WINDOW->search_string. */
static void
tree_search_check_node (WINDOW *window)
{
  long start_off;
  enum search_result result;
  char *string;
  int previous_match;

  if (window->node->active_menu != 0)
    previous_match = 1;
  else
    {
      previous_match = 0;
      window->node->active_menu = BEFORE_MENU;
    }
  string = xstrdup (window->search_string);
  goto check_node;

check_node:
  result = info_search_in_node_internal (window, window->node,
                                  string,
                                  window->point + 1,
                                  1, /* Search forwards */
                                  1, /* Case-sensitive */
                                  0, /* No regular expressions. */
                                  &start_off);
  if (result == search_success)
    {
      info_show_point (window);
      goto funexit;
    }

  /* Otherwise, try each menu entry in turn. */
  if (matches_ready (&window->matches))
    window->point++; /* Find this match again if/when we come back. */
  goto check_menus;

  /* At this juncture, window->node->active_menu is the index of the last
     reference in the node to have been checked, plus one.  BEFORE_MENU is a 
     special code to say that none of them have been checked. */
check_menus:
  if (!(window->node->flags & N_IsIndex)) /* Don't go down menus in index  */
    {                               /* nodes, because this leads to loops. */
      REFERENCE *r;
      int ref_index;
      if (window->node->active_menu != BEFORE_MENU)
        ref_index = window->node->active_menu;
      else
        ref_index = 0;
      for (; (r = window->node->references[ref_index]); ref_index++)
        if (r->type == REFERENCE_MENU_ITEM)
          {
            FILE_BUFFER *file_buffer;
            TAG **tag;
            NODE *node;

            if (!tag_of_reference (r, window, &file_buffer, &tag))
              continue;

            if ((*tag)->flags & N_SeenBySearch)
              continue;
            (*tag)->flags |= N_SeenBySearch;

            window->node->active_menu = ref_index + 1;
            node = info_node_of_tag (file_buffer, tag);
            if (!node)
              continue;
            info_set_node_of_window_fast (window, node);
            window->node->active_menu = BEFORE_MENU;
            goto check_node;
          }
    }
  goto go_up;

go_up:
  /* If no more menu entries, try going back. */
  if (window->hist_index >= 2
      && window->hist[window->hist_index - 2]->node->active_menu != 0)
    {
      forget_node_fast (window);
      goto check_menus;
    }

  /* Go back to the final match. */
  if (previous_match)
    {
      message_in_echo_area (_("Going back to last match from %s"),
                            window->node->nodename);

      /* This is a trick.
         Set active_menu to one more than the number of references,
         and add an arbitrary node to the window history.
         When we call tree_search_check_node_backwards, this will go
         backwards through the tree structure to the last match.
         Change active_menu back to a valid value afterwards .*/
      {
        int n = 0;

        while (window->node->references[n])
          n++;
        window->node->active_menu = n + 1;

        info_parse_and_select ("Top", window);
        /* Check if this worked. */
        if (strcmp (window->node->nodename, "Top"))
          {
            /* Loading "Top" node failed. */
            window->node->active_menu = 0;
            goto funexit;
          }
        window->node->active_menu = BEFORE_MENU;
      }
      window->point = window->node->body_start;
      tree_search_check_node_backwards (window);
    }
  info_error (previous_match ?  _("No more matches") : _("Search failed"));

funexit:
  free (string);
} /*********** end tree_search_check_node *************/


/* The same as tree_search_check_node, but backwards. */
static void
tree_search_check_node_backwards (WINDOW *window)
{
  long start_off;
  enum search_result result;
  char *string;
  int previous_match;

  previous_match = (window->node->active_menu != 0);

  string = xstrdup (window->search_string);
  goto check_node;

check_node:
  result = info_search_in_node_internal (window, window->node,
                                  string,
                                  window->point - 1,
                                 -1, /* Search backwards */
                                  1, /* Case-sensitive */
                                  0, /* No regular expressions. */
                                  &start_off);
  if (result == search_success)
    {
      info_show_point (window);
      goto funexit;
    }

  goto go_up;

  /* Check through menus in current node, in reverse order.
     At this juncture, window->node->active_menu is the index of the last
     reference in the node to have been checked, plus one.  BEFORE_MENU is a 
     special code to say that none of them have been checked. */
check_menus:
  if (!(window->node->flags & N_IsIndex)) /* Don't go down menus in index  */
    {                               /* nodes, because this leads to loops. */
      REFERENCE *r;
      int ref_index;
      if (window->node->active_menu == BEFORE_MENU)
        goto check_node;
      else
        ref_index = window->node->active_menu - 2;
      for (; ref_index >= 0; ref_index--)
        {
          r = window->node->references[ref_index];
          if (r->type == REFERENCE_MENU_ITEM)
            {
              TAG **tag;
              FILE_BUFFER *file_buffer;
              NODE *node;

              if (!tag_of_reference (r, window, &file_buffer, &tag))
                continue;

              /* This inverts what is done for the forwards search.  It's 
                 possible that we will visit the nodes in a different order if 
                 there is more than one reference to a node. */
              if (!((*tag)->flags & N_SeenBySearch))
                continue;

              node = info_node_of_tag (file_buffer, tag);
              if (!node)
                continue;
              window->node->active_menu = ref_index + 1;
              info_set_node_of_window_fast (window, node);
              window->point = window->node->nodelen;
              {
                /* Start at the last menu entry in the subordinate node. */
                int i;
                i = 0;
                while(window->node->references[i])
                  i++;
                window->node->active_menu = i + 1;
              }
              goto check_menus;
            }
        }
    }
  window->node->active_menu = BEFORE_MENU;
  goto check_node;

  /* Try going back. */
go_up:
  if (window->hist_index >= 2
      && window->hist[window->hist_index - 2]->node->active_menu != 0)
    {
      TAG **tag;
      REFERENCE *r;
      FILE_BUFFER *file_buffer;

      forget_node_fast (window);
      r = window->node->references[window->node->active_menu - 1];

      /* Clear the flag to say we've been to the node we just came back
         from.  This reverse the order from the forwards search, where
         we set this flag just before going down. */
      if (r && tag_of_reference (r, window, &file_buffer, &tag))
        {
          (*tag)->flags &= ~N_SeenBySearch;
        }

      goto check_menus;
    }

  /* Otherwise, no result. */
  info_error (previous_match ?  _("No more matches") : _("Search failed"));

funexit:
  free (string);
} /*********** end tree_search_check_node_backwards *************/


/* Clear N_SeenBySearch for all nodes. */
void
wipe_seen_flags (void)
{
  int fb_index;
  TAG **t;

  for (fb_index = 0; fb_index < info_loaded_files_index; fb_index++)
    {
      t = info_loaded_files[fb_index]->tags;
      if (!t)
        continue; /* Probably a sub-file of a split file. */
      for (; *t; t++)
        {
          (*t)->flags &= ~N_SeenBySearch;
        }
    }
}

DECLARE_INFO_COMMAND (info_tree_search,
                      _("Search this node and subnodes for a string"))
{
  char *prompt, *line;
  int i;

  /* TODO: Display manual name */
  /* TRANSLATORS: %s is the title of a node. */
  asprintf (&prompt, _("Search under %s: "),
            window->node->nodename);
  line = info_read_in_echo_area (prompt); free (prompt);
  if (!line)
    return;

  /* Remove relics of previous tree searches. */
  wipe_seen_flags ();
  for (i = 0; i < window->hist_index; i++)
    window->hist[i]->node->active_menu = 0;
  window->search_string = line;
  tree_search_check_node (window);
}

DECLARE_INFO_COMMAND (info_tree_search_next,
                      _("Go to next match in Info sub-tree"))
{
  if (!window->search_string || window->node->active_menu == 0)
    {
      info_error (_("No active search"));
      return;
    }

  tree_search_check_node (window);
}

DECLARE_INFO_COMMAND (info_tree_search_previous,
                      _("Go to previous match in Info sub-tree"))
{
  if (!window->search_string || window->node->active_menu == 0)
    {
      info_error (_("No active search"));
      return;
    }

  tree_search_check_node_backwards (window);
}
#undef BEFORE_MENU

DECLARE_INFO_COMMAND (info_search_case_sensitively,
                      _("Read a string and search for it case-sensitively"))
{
  info_search_1 (window, count, 1);
}

DECLARE_INFO_COMMAND (info_search, _("Read a string and search for it"))
{
  info_search_1 (window, count, 0);
}

DECLARE_INFO_COMMAND (info_search_backward,
                      _("Read a string and search backward for it"))
{
  info_search_1 (window, -count, 0);
}

int search_skip_screen_p = 0;

DECLARE_INFO_COMMAND (info_search_next,
                      _("Repeat last search in the same direction"))
{
  long start_off = window->point + 1;
  NODE *starting_node = window->node;
  int result;

  if (!last_search_direction || !search_string)
    {
      info_error ("%s", _("No previous search string"));
      return;
    }

  if (search_skip_screen_p)
    {
      /* Find window bottom */
      long n = window->height + window->pagetop;
      if (n < window->line_count)
	start_off = window->line_starts[n];
      else
	start_off = window->node->nodelen;
    }

  for (result = 0; result == 0 && count--; )
    result = info_search_internal (search_string,
                                   active_window, 1,
                                   last_search_case_sensitive,
				   &start_off);

  if (result == 0 && window->node == starting_node && search_skip_screen_p)
    {
      long match_line = window_line_of_point (window);
      long new_pagetop;

      /* Scroll down a whole number of screenfulls to make match visible. */
      new_pagetop = window->pagetop;
      new_pagetop += (match_line - window->pagetop) / window->height
                      * window->height;

      set_window_pagetop (window, new_pagetop);
    }
  else
    window_adjust_pagetop (window);
}

DECLARE_INFO_COMMAND (info_search_previous,
                      _("Repeat last search in the reverse direction"))
{
  long start_off = window->point - 1;
  NODE *starting_node = window->node;
  int result;

  if (!last_search_direction || !search_string)
    {
      info_error ("%s", _("No previous search string"));
      return;
    }

  if (search_skip_screen_p)
    start_off = window->line_starts[window->pagetop] - 1;

  /* start_off can be negative here, in which case info_search_internal
     will go to the previous node straight away. */

  for (result = 0; result == 0 && count--; )
    result = info_search_internal (search_string,
                                   active_window, -1,
                                   last_search_case_sensitive,
				   &start_off);

  if (result == 0 && window->node == starting_node && search_skip_screen_p)
    {
      long match_line = window_line_of_point (window);
      long new_pagetop;

      /* Scroll up a whole number of screenfulls to make match visible.
         This means if 'info_search_next' was the last command, we'll
         go back to the same place. */
      new_pagetop = window->pagetop - window->height;
      new_pagetop -= (window->pagetop - match_line - 1) / window->height
                      * window->height;

      if (new_pagetop < 0)
        new_pagetop = 0;
      set_window_pagetop (window, new_pagetop);
    }
  else
    window_adjust_pagetop (window);
}

/* If highlight-searches=On, this will clear any highlighted regions on the
   screen. */
DECLARE_INFO_COMMAND (info_clear_search,
                      _("Clear displayed search matches"))
{
  free_matches (&window->matches);
  window->flags |= W_UpdateWindow;
}


/* **************************************************************** */
/*                                                                  */
/*                      Incremental Searching                       */
/*                                                                  */
/* **************************************************************** */

static void incremental_search (WINDOW *window, int count);

DECLARE_INFO_COMMAND (isearch_forward,
                      _("Search interactively for a string as you type it"))
{
  incremental_search (window, count);
}

DECLARE_INFO_COMMAND (isearch_backward,
                      _("Search interactively for a string as you type it"))
{
  incremental_search (window, -count);
}

/* Structure defining the current state of an incremental search. */
typedef struct {
  char *fullpath;
  char *nodename;
  long pagetop;         /* LINE_STARTS[PAGETOP] is first line in WINDOW. */
  long point;           /* Point in window. */
  long start;           /* Offset in node contents where search started. */
  int search_index;     /* Offset of the last char in the search string. */
  int direction;        /* The direction that this search is heading in. */
  enum search_result failing;      /* Whether or not this search failed. */
} SEARCH_STATE;

/* Incrementally search for a string as it is typed. */
/* The last accepted incremental search string. */
static char *last_isearch_accepted = NULL;

/* The current incremental search string. */
static char *isearch_string = NULL;
static int isearch_string_index = 0;
static int isearch_string_size = 0;

/* Array of search states. */
static SEARCH_STATE **isearch_states = NULL;
static size_t isearch_states_index = 0;
static size_t isearch_states_slots = 0;

/* Get the state of WINDOW, and save it in STATE. */
static void
window_get_state (WINDOW *window, SEARCH_STATE *state)
{
  state->fullpath = window->node->fullpath;
  state->nodename = window->node->nodename;
  state->pagetop = window->pagetop;
  state->point = window->point;
}

/* Set the node, pagetop, and point of WINDOW. */
static void
window_set_state (WINDOW *window, SEARCH_STATE *state)
{
  if (strcmp(window->node->fullpath, state->fullpath)
      || strcmp(window->node->nodename, state->nodename))
    {
      NODE *n = info_get_node (state->fullpath, state->nodename);
      info_set_node_of_window (window, n);
    }
  window->pagetop = state->pagetop;
  window->point = state->point;
}

/* Push the state of this search. */
static void
push_isearch (WINDOW *window, int search_index, int direction,
              enum search_result failing, long start_off)
{
  SEARCH_STATE *state;

  state = xmalloc (sizeof (SEARCH_STATE));
  window_get_state (window, state);
  state->search_index = search_index;
  state->direction = direction;
  state->failing = failing;
  state->start = start_off;

  add_pointer_to_array (state, isearch_states_index, isearch_states,
                        isearch_states_slots, 20);
}

/* Pop the state of this search to WINDOW, SEARCH_INDEX, and DIRECTION. */
static void
pop_isearch (WINDOW *window, int *search_index, int *direction,
             enum search_result *failing, long *start_off)
{
  SEARCH_STATE *state;

  if (isearch_states_index)
    {
      isearch_states_index--;
      state = isearch_states[isearch_states_index];
      window_set_state (window, state);
      *search_index = state->search_index;
      *direction = state->direction;
      *failing = state->failing;
      *start_off = state->start;

      free (state);
      isearch_states[isearch_states_index] = NULL;
    }
}

/* Free the memory used by isearch_states. */
static void
free_isearch_states (void)
{
  register int i;

  for (i = 0; i < isearch_states_index; i++)
    {
      free (isearch_states[i]);
      isearch_states[i] = NULL;
    }
  isearch_states_index = 0;
}

/* Display the current search in the echo area. */
static void
show_isearch_prompt (int dir, unsigned char *string,
                     enum search_result failing)
{
  register int i;
  const char *prefix;
  char *prompt, *p_rep;
  unsigned int prompt_len, p_rep_index, p_rep_size;

  if (dir < 0)
    prefix = use_regex ? _("Regexp I-search backward: ")
                       : _("I-search backward: ");
  else
    prefix = use_regex ? _("Regexp I-search: ")
                       : _("I-search: ");

  p_rep_index = p_rep_size = 0;
  p_rep = NULL;
  for (i = 0; string[i]; i++)
    {
      char *rep;

      switch (string[i])
        {
        case ' ': rep = " "; break;
        case LFD: rep = "\\n"; break;
        case TAB: rep = "\\t"; break;
        default:
          rep = pretty_keyname (string[i]);
        }
      if ((p_rep_index + strlen (rep) + 1) >= p_rep_size)
        p_rep = xrealloc (p_rep, p_rep_size += 100);

      strcpy (p_rep + p_rep_index, rep);
      p_rep_index += strlen (rep);
    }

  prompt_len = strlen (prefix) + p_rep_index + 1;
  if (failing != search_success)
    prompt_len += strlen (_("Failing "));
  prompt = xmalloc (prompt_len);
  sprintf (prompt, "%s%s%s",
           failing != search_success ? _("Failing ") : "",
           prefix, p_rep ? p_rep : "");

  window_message_in_echo_area ("%s", prompt);
  free (p_rep);
  free (prompt);
  display_cursor_at_point (active_window);
}

/* Read and dispatch loop for incremental search mode. */
static void
incremental_search (WINDOW *window, int count)
{
  int key;
  enum search_result last_search_result, search_result;
  int dir;
  SEARCH_STATE orig_state; /* Window state at start of incremental search. */
  SEARCH_STATE mystate;    /* State before each search. */
  char *p;
  int case_sensitive;
  long start_off = window->point;
  int starting_history_entry = window->hist_index - 1;

  if (count < 0)
    dir = -1;
  else
    dir = 1;

  last_search_result = search_result = search_success;

  window_get_state (window, &orig_state);

  isearch_string_index = 0;
  if (!isearch_string_size)
    isearch_string = xmalloc (isearch_string_size = 50);

  isearch_string[isearch_string_index] = '\0';

  isearch_is_active = 1;

  /* Save starting position of search. */
  push_isearch (window, isearch_string_index, dir, search_result, start_off);

  while (isearch_is_active)
    {
      VFunction *func = NULL;
      int quoted = 0;

      /* Show the search string in the echo area. */
      show_isearch_prompt (dir, (unsigned char *) isearch_string,
                           search_result);

      /* If a recent display was interrupted, then do the redisplay now if
         it is convenient. */
      if (!info_any_buffered_input_p () && display_was_interrupted_p)
        {
          display_update_display ();
          display_cursor_at_point (active_window);
        }

      /* Read keys, looking in both keymaps for a recognized key sequence. */
      {
        Keymap info_kp, ea_kp;

        info_kp = info_keymap;
        ea_kp = echo_area_keymap;

        key = get_input_key ();

        while (1)
          {
            if (key >= 32 && key < 256)
              break;

            if (info_kp && info_kp[key].type == ISFUNC)
              {
                if (info_kp[key].value.function)
                  func = info_kp[key].value.function->func;
                if (func == &isearch_forward
                    || func == &isearch_backward
                    || func == &info_abort_key)
                  {
                    goto gotfunc;
                  }
                else
                  {
                    func = 0;
                    info_kp = 0;
                  }
              }
            else if (info_kp) /* ISKMAP */
              info_kp = info_kp[key].value.keymap;

            if (ea_kp && ea_kp[key].type == ISFUNC)
              {
                if (ea_kp[key].value.function)
                  func = ea_kp[key].value.function->func;
                if (func == &ea_abort
                    || func == &ea_quoted_insert
                    || func == &ea_rubout)
                  {
                    func = ea_kp[key].value.function->func;
                    goto gotfunc;
                  }
                else
                  {
                    func = 0;
                    ea_kp = 0;
                  }
              }
            else if (ea_kp) /* ISKMAP */
              ea_kp = ea_kp[key].value.keymap;

          if (!info_kp && !ea_kp)
            break;

          key = get_input_key ();
        }
      }
gotfunc:

      if (func == &ea_quoted_insert)
        {
          /* User wants to insert a character. */
          key = get_input_key ();
          if (key < 0 || key >= 256)
            continue; /* The user pressed a key like an arrow key. */
          quoted = 1;
        }

      if (quoted || (!func && key >= 32 && key < 256))
        {
          push_isearch (window, isearch_string_index, dir,
                        search_result, start_off);

          if (isearch_string_index + 2 >= isearch_string_size)
            isearch_string = xrealloc
              (isearch_string, isearch_string_size += 100);

          isearch_string[isearch_string_index++] = key;
          isearch_string[isearch_string_index] = '\0';

          if (search_result != search_success && !use_regex)
            continue;
        }
      else if (func == &ea_rubout)
        {
          /* User wants to delete one level of search? */
          if (!isearch_states_index)
            {
              terminal_ring_bell ();
              continue;
            }
          else
            {
              int end = isearch_string_index;
              /* Remove a complete multi-byte character from the end of the
                 search string. */
              do
                {
                  pop_isearch (window, &isearch_string_index,
                               &dir, &search_result, &start_off);
                }
              while (isearch_string_index > 0
                && (long) mbrlen (isearch_string + isearch_string_index,
                                  end - isearch_string_index, NULL) <= 0);
               
              isearch_string[isearch_string_index] = '\0';

              if (isearch_string_index == 0)
                {
                  /* Don't search for an empty string.  Clear the search. */
                  free_matches (&window->matches);
                  display_update_one_window (window);
                  continue;
                }
              if (search_result != search_success)
                {
                  display_update_one_window (window);
                  continue;
                }
            }
        }
      else if (func == &isearch_forward || func == &isearch_backward)
        {
          /* If this key invokes an incremental search, then this
             means that we will either search again in the same
             direction, search again in the reverse direction, or
             insert the last search string that was accepted through
             incremental searching. */
          if (func == &isearch_forward && dir > 0
              || func == &isearch_backward && dir < 0)
            {
              /* If the user has typed no characters, then insert the
                 last successful search into the current search string. */
              if (isearch_string_index == 0)
                {
                  /* Of course, there must be something to insert. */
                  if (last_isearch_accepted)
                    {
                      if (strlen ((char *) last_isearch_accepted) + 1
                          >= (unsigned int) isearch_string_size)
                        isearch_string = (char *)
                          xrealloc (isearch_string,
                                    isearch_string_size += 10 +
                                    strlen (last_isearch_accepted));
                      strcpy (isearch_string, last_isearch_accepted);
                      isearch_string_index = strlen (isearch_string);
                    }
                  else
                    continue;
                }
              else
                {
                  /* Search again in the same direction.  This means start
                     from a new place if the last search was successful. */
                  if (search_result == search_success)
                    {
                      start_off = window->point;
                      if (dir < 0)
                        /* Position before match to avoid finding same match
                           agin. */
                        start_off--;
                    }
                }
            }
          else
            {
              /* Reverse the direction of the search. */
              dir = -dir;
            }
        }
      else if (func == &info_abort_key
               && isearch_states_index && search_result != search_success)
        {
          /* If C-g pressed, and the search is failing, pop the search
             stack back to the last unfailed search. */
          terminal_ring_bell ();
          while (isearch_states_index && search_result != search_success)
            pop_isearch (window, &isearch_string_index, &dir,
                         &search_result, &start_off);
          isearch_string[isearch_string_index] = '\0';
          show_isearch_prompt (dir, (unsigned char *) isearch_string,
                               search_result);
          continue;
        }
      else if (func == &info_abort_key || func == &ea_abort || !func)
        {
          /* The character is not printable, or it has a function which is
             non-null.  Exit the search, remembering the search string. */
          if (isearch_string_index && func != &info_abort_key)
            {
              free (last_isearch_accepted);
              last_isearch_accepted = xstrdup (isearch_string);
            }

          if (func == &info_abort_key)
            {
              if (isearch_states_index)
                window_set_state (window, &orig_state);
            }

          if (!echo_area_is_active)
            window_clear_echo_area ();

          if (auto_footnotes_p)
            info_get_or_remove_footnotes (active_window);

          isearch_is_active = 0;
          continue;
        }

      /* Show the new search string in the prompt. */
      show_isearch_prompt (dir, (unsigned char *) isearch_string, 
                           search_result);

      /* Make the search case-sensitive only if the search string includes
         upper-case letters. */
      case_sensitive = 0;
      for (p = isearch_string; *p; p++)
        if (isupper (*p))
          {
            case_sensitive = 1;
            break;
          }

      last_search_result = search_result;

      window_get_state (window, &mystate);
      search_result = info_search_internal (isearch_string,
                                            window, dir, case_sensitive,
                                            &start_off);

      /* If this search failed, and we didn't already have a failed search,
         then ring the terminal bell. */
      if (search_result != search_success
          && last_search_result == search_success)
        {
          terminal_ring_bell ();
        }
      else if (search_result == search_success)
        {
          /* Make sure the match is visible, and update the display. */

          if (!strcmp(window->node->fullpath, mystate.fullpath)
              && !strcmp(window->node->nodename, mystate.nodename)
              && mystate.pagetop != window->pagetop)
            {
              int newtop = window->pagetop;
              window->pagetop = mystate.pagetop;
              set_window_pagetop (window, newtop);
            }
          window_adjust_pagetop (window);

          /* Call display_update_display to update the window and an automatic
             footnotes window if present. */
          display_update_display ();
          display_cursor_at_point (window);
        }
    }

  /* Free the memory used to remember each search state. */
  free_isearch_states ();

  /* Alter the window history so that we have added at most one node in the 
     incremental search, so that going back once with "l" goes to where we 
     started the incremental search if the match was in a different node. */
  {
    int i = window->hist_index - 1;
    int j = starting_history_entry;

    if (i > j)
      {
        if (!strcmp(window->hist[i]->node->nodename,
                    window->hist[j]->node->nodename)
            && !strcmp(window->hist[j]->node->fullpath,
                       window->hist[i]->node->fullpath))
          {
            /* If we end up at the same node we started at, don't extend
               the history at all. */
            cleanup_history (window, j, i);
          }
        else
          {
            cleanup_history (window, j + 1, i);
          }
      }
  }

  /* Perhaps GC some file buffers. */
  gc_file_buffers_and_nodes ();

  /* After searching, leave the window in the correct state. */
  if (!echo_area_is_active)
    window_clear_echo_area ();
}


/* **************************************************************** */
/*                                                                  */
/*                  Miscellaneous Info Commands                     */
/*                                                                  */
/* **************************************************************** */

/* What to do when C-g is pressed in a window. */
DECLARE_INFO_COMMAND (info_abort_key, _("Cancel current operation"))
{
  /* If error printing doesn't oridinarily ring the bell, do it now,
     since C-g always rings the bell.  Otherwise, let the error printer
     do it. */
  if (!info_error_rings_bell_p)
    terminal_ring_bell ();
  info_error ("%s", _("Quit"));

  info_initialize_numeric_arg ();
}

DECLARE_INFO_COMMAND (info_info_version, _("Display version of Info being run"))
{
  window_message_in_echo_area (_("GNU Info version %s"), VERSION);
}

/* Clear the screen and redraw its contents.  Given a numeric argument,
   move the line the cursor is on to the COUNT'th line of the window. */
DECLARE_INFO_COMMAND (info_redraw_display, _("Redraw the display"))
{
  if ((!info_explicit_arg && count == 1) || echo_area_is_active)
    {
      terminal_clear_screen ();
      display_clear_display (the_display);
      window_mark_chain (windows, W_UpdateWindow);
      display_update_display ();
    }
  else
    {
      int desired_line, point_line;
      int new_pagetop;

      point_line = window_line_of_point (window) - window->pagetop;

      if (count < 0)
        desired_line = window->height + count;
      else
        desired_line = count;

      if (desired_line < 0)
        desired_line = 0;

      if (desired_line >= window->height)
        desired_line = window->height - 1;

      if (desired_line == point_line)
        return;

      new_pagetop = window->pagetop + (point_line - desired_line);

      set_window_pagetop (window, new_pagetop);
    }
}

/* Exit from info */
DECLARE_INFO_COMMAND (info_quit, _("Quit using Info"))
{
  if (window->next || window->prev)
    info_delete_window (window, count);
  else
    quit_info_immediately = 1;
}


/* **************************************************************** */
/*                                                                  */
/*               Reading Keys and Dispatching on Them               */
/*                                                                  */
/* **************************************************************** */

DECLARE_INFO_COMMAND (info_do_lowercase_version,
                      _("Run command bound to this key's lowercase variant"))
{} /* Declaration only. */

static void
dispatch_error (int *keyseq)
{
  char *rep;

  rep = pretty_keyseq (keyseq);

  if (!echo_area_is_active)
    info_error (_("Unknown command (%s)"), rep);
  else
    {
      char *temp = xmalloc (1 + strlen (rep) + strlen (_("\"%s\" is invalid")));
      sprintf (temp, _("'%s' is invalid"), rep);
      terminal_ring_bell ();
      inform_in_echo_area (temp);
      free (temp);
    }
}

/* Keeping track of key sequences. */
static int *info_keyseq = NULL;
static int info_keyseq_index = 0;
static int info_keyseq_size = 0;
static int info_keyseq_displayed_p = 0;

/* Initialize the length of the current key sequence. */
void
initialize_keyseq (void)
{
  info_keyseq_index = 0;
  info_keyseq_displayed_p = 0;
}

/* Add CHARACTER to the current key sequence. */
void
add_char_to_keyseq (int character)
{
  if (info_keyseq_index + 2 >= info_keyseq_size)
    info_keyseq = xrealloc (info_keyseq,
                            sizeof (int) * (info_keyseq_size += 10));

  info_keyseq[info_keyseq_index++] = character;
  info_keyseq[info_keyseq_index] = '\0';
}

/* Display the current value of info_keyseq.  If argument EXPECTING is
   non-zero, input is expected to be read after the key sequence is
   displayed, so add an additional prompting character to the sequence. */
static void
display_info_keyseq (int expecting_future_input)
{
  char *rep;

  if (!info_keyseq || info_keyseq_index == 0)
    return;

  rep = pretty_keyseq (info_keyseq);
  if (expecting_future_input)
    strcat (rep, "-");

  if (echo_area_is_active)
    inform_in_echo_area (rep);
  else
    {
      window_message_in_echo_area (rep, NULL, NULL);
      display_cursor_at_point (active_window);
    }
  info_keyseq_displayed_p = 1;
}

/* Called by interactive commands to read another key when keys have already
   been read as part of the current command (and possibly displayed in status
   line with display_info_keyseq). */
int
get_another_input_key (void)
{
  int ready = !info_keyseq_displayed_p; /* ready if new and pending key */

  /* If there isn't any input currently available, then wait a
     moment looking for input.  If we don't get it fast enough,
     prompt a little bit with the current key sequence. */
  if (!info_keyseq_displayed_p)
    {
      ready = 1;
      if (!info_any_buffered_input_p ())
        {
#if defined (FD_SET)
          struct timeval timer;
          fd_set readfds;

          FD_ZERO (&readfds);
          FD_SET (fileno (info_input_stream), &readfds);
          timer.tv_sec = 1;
          timer.tv_usec = 750;
          ready = select (fileno(info_input_stream)+1, &readfds,
			  NULL, NULL, &timer);
#else
          ready = 0;
#endif /* FD_SET */
      }
    }

  if (!ready)
    display_info_keyseq (1);

  return get_input_key ();
}

/* Non-zero means that an explicit argument has been passed to this
   command, as in C-u C-v. */
int info_explicit_arg = 0;

/* As above, but used when C-u is typed in the echo area to avoid
   overwriting this information when "C-u ARG M-x" is typed. */
int ea_explicit_arg = 0;

void info_universal_argument (WINDOW *, int count);
void info_add_digit_to_numeric_arg (WINDOW *, int count);

/* Read a key sequence and look up its command in MAP.  Handle C-u style 
   numeric args, as well as M--, and M-digits.  Return argument in COUNT if it 
   is non-null.

   Some commands can be executed directly, in which case null is returned
   instead:

   If MENU, call info_menu_digit on ACTIVE_WINDOW if a number key was 
   pressed.

   If MOUSE, call mouse_event_handler if a mouse event occurred.

   If INSERT, call ea_insert if a printable character was input.
 */
VFunction *
read_key_sequence (Keymap map, int menu, int mouse,
                   int insert, int *count)
{
  int key;
  int reading_universal_argument = 0;

  int numeric_arg = 1, numeric_arg_sign = 1, *which_explicit_arg;
  VFunction *func;

  /* Process the right numeric argument. */
  if (!echo_area_is_active)
    which_explicit_arg = &info_explicit_arg;
  else
    which_explicit_arg = &ea_explicit_arg;

  *which_explicit_arg = 0;

  initialize_keyseq ();

  key = get_input_key ();
  if (key == KEY_MOUSE)
    {
      if (mouse)
        mouse_event_handler ();
      return 0;
    }

  if (insert
      && (key >= 040 && key < 0200
          || ISO_Latin_p && key >= 0200 && key < 0400))
    {
      ea_insert (the_echo_area, 1, key);
      return 0;
    }

  add_char_to_keyseq (key);

  while (1)
    {
      int dash_typed = 0, digit_typed = 0;
      func = 0;

      if (display_was_interrupted_p && !info_any_buffered_input_p ())
        display_update_display ();

      if (active_window != the_echo_area)
        display_cursor_at_point (active_window);

      /* If reading a universal argument, both <digit> and M-<digit> help form 
         the argument.  Don't look up the pressed key in the key map. */
      if (reading_universal_argument)
        {
          int k = key;
          if (k >= KEYMAP_META_BASE)
            k -= KEYMAP_META_BASE;
          if (k == '-')
            {
              dash_typed = 1;
            }
          else if (isdigit (k))
            {
              digit_typed = 1;
            }
          else
            /* Note: we may still read another C-u after this. */
            reading_universal_argument = 0;
        }

      if (!dash_typed && !digit_typed && map[key].type == ISFUNC)
        {
          func = map[key].value.function ? map[key].value.function->func : 0;
          if (!func)
            {
              dispatch_error (info_keyseq);
              return 0;
            }
        }

      if (dash_typed || digit_typed || func == &info_add_digit_to_numeric_arg)
        {
          int k = key;
          if (k > KEYMAP_META_BASE)
            k -= KEYMAP_META_BASE;
          reading_universal_argument = 1;
          if (dash_typed || k == '-')
            {
              if (!*which_explicit_arg)
                {
                  numeric_arg_sign = -1;
                  numeric_arg = 1;
                }

            }
          else if (digit_typed || isdigit (k))
            {
              if (*which_explicit_arg)
                numeric_arg = numeric_arg * 10 + (k - '0');
              else
                numeric_arg = (k - '0');
              *which_explicit_arg = 1;
            }
        }
      else if (func == info_do_lowercase_version)
        {
          int lowerkey;

          if (key >= KEYMAP_META_BASE)
            {
              lowerkey = key;
              lowerkey -= KEYMAP_META_BASE;
              lowerkey = tolower (lowerkey);
              lowerkey += KEYMAP_META_BASE;
            }
          else
            lowerkey = tolower (key);

          if (lowerkey == key)
            {
              dispatch_error (info_keyseq);
              return 0;
            }
          key = lowerkey;
          continue;
        }
      else if (func == &info_universal_argument)
        {
          /* Multiply by 4. */
          /* TODO: Maybe C-u should also terminate the universal argument 
             sequence, as in Emacs. (C-u 6 4 C-u 1 inserts 64 1's.) */
          if (!*which_explicit_arg)
            numeric_arg *= 4;
          reading_universal_argument = 1;
        }
      else if (menu && func == &info_menu_digit)
        {
          /* key can either be digit, or M-digit for --vi-keys. */

          int k = key;
          if (k > KEYMAP_META_BASE)
            k -= KEYMAP_META_BASE;
          window_clear_echo_area ();
          menu_digit (active_window, k);
          return 0;
        }
      else if (insert
               && (func == &ea_possible_completions || func == &ea_complete)
               && !echo_area_completion_items)
        {
          ea_insert (the_echo_area, 1, key);
          return 0;
        }
      else if (func)
        {
          /* Don't update the key sequence if we have finished reading a key 
             sequence in the echo area.  This means that a key sequence like 
             "C-u 2 Left" appears to take effect immediately, instead of there 
             being a delay while the message is displayed. */
          if (!echo_area_is_active && info_keyseq_displayed_p)
            display_info_keyseq (0);

          if (count)
            *count = numeric_arg * numeric_arg_sign;

          /* *which_explicit_arg has not been set yet if only a sequence of 
             C-u's was typed (each of which has multiplied the argument by 
             four).  */
          if (*count != 1 && !*which_explicit_arg)
            *which_explicit_arg = 1;

          return func;
        }
      else if (map[key].type == ISKMAP)
        {
          if (map[key].value.keymap != NULL)
            map = map[key].value.keymap;
          else
            {
              dispatch_error (info_keyseq);
              return 0;
            }

          if (info_keyseq_displayed_p)
            display_info_keyseq (1);
        }

      do
        key = get_another_input_key ();
      while (key == KEY_MOUSE);
      add_char_to_keyseq (key);
    }

  return 0;
}

/* Add the current digit to the argument in progress. */
DECLARE_INFO_COMMAND (info_add_digit_to_numeric_arg,
                      _("Add this digit to the current numeric argument"))
{} /* Declaration only. */

/* C-u, universal argument.  Multiply the current argument by 4.
   Read a key.  If the key has nothing to do with arguments, then
   dispatch on it.  If the key is the abort character then abort. */
DECLARE_INFO_COMMAND (info_universal_argument,
                      _("Start (or multiply by 4) the current numeric argument"))
{} /* Declaration only. */

/* Create a default argument. */
void
info_initialize_numeric_arg (void)
{
  if (!echo_area_is_active)
    {
      info_explicit_arg = 0;
    }
  else
    {
      ea_explicit_arg = 0;
    }
}

