/* signals.c -- install and maintain signal handlers.
   $Id: signals.c 5823 2014-09-12 17:22:40Z gavin $

   Copyright 1993, 1994, 1995, 1998, 2002, 2003, 2004, 2007, 2012, 2013, 2014
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
#include "footnotes.h"
#include "window.h"
#include "signals.h"

void initialize_info_signal_handler (void);

/* **************************************************************** */
/*                                                                  */
/*              Pretending That We Have POSIX Signals               */
/*                                                                  */
/* **************************************************************** */

#if !defined (HAVE_SIGPROCMASK) && defined (HAVE_SIGSETMASK)
/* Perform OPERATION on NEWSET, perhaps leaving information in OLDSET. */
static void
sigprocmask (int operation, int *newset, int *oldset)
{
  switch (operation)
    {
    case SIG_UNBLOCK:
      sigsetmask (sigblock (0) & ~(*newset));
      break;

    case SIG_BLOCK:
      *oldset = sigblock (*newset);
      break;

    case SIG_SETMASK:
      sigsetmask (*newset);
      break;

    default:
      abort ();
    }
}
#endif /* !HAVE_SIGPROCMASK && HAVE_SIGSETMASK */

/* **************************************************************** */
/*                                                                  */
/*                  Signal Handling for Info                        */
/*                                                                  */
/* **************************************************************** */

#if defined (HAVE_SIGACTION) || defined (HAVE_SIGPROCMASK) ||\
  defined (HAVE_SIGSETMASK)
static void
mask_termsig (sigset_t *set)
{
# if defined (SIGTSTP)
  sigaddset (set, SIGTSTP);
  sigaddset (set, SIGTTOU);
  sigaddset (set, SIGTTIN);
# endif
# if defined (SIGWINCH)
  sigaddset (set, SIGWINCH);
# endif
#if defined (SIGQUIT)
  sigaddset (set, SIGQUIT);
#endif
#if defined (SIGINT)
  sigaddset (set, SIGINT);
#endif
#if defined (SIGTERM)
  sigaddset (set, SIGTERM);
#endif
# if defined (SIGUSR1)
  sigaddset (set, SIGUSR1);
# endif
}
#endif /* HAVE_SIGACTION || HAVE_SIGPROCMASK || HAVE_SIGSETMASK */

static RETSIGTYPE info_signal_proc (int sig);
#if defined (HAVE_SIGACTION)
typedef struct sigaction signal_info;
signal_info info_signal_handler;

static void
set_termsig (int sig, signal_info *old)
{
  sigaction (sig, &info_signal_handler, old);
}

static void
restore_termsig (int sig, const signal_info *saved)
{
  sigaction (sig, saved, NULL);
}
#else /* !HAVE_SIGACTION */
typedef RETSIGTYPE (*signal_info) ();
#define set_termsig(sig, old) (void)(*(old) = signal (sig, info_signal_proc))
#define restore_termsig(sig, saved) (void)signal (sig, *(saved))
#define info_signal_handler info_signal_proc
static int term_conf_busy = 0;
#endif /* !HAVE_SIGACTION */

static signal_info old_TSTP, old_TTOU, old_TTIN;
static signal_info old_WINCH, old_INT, old_TERM, old_USR1;
static signal_info old_QUIT;

void
initialize_info_signal_handler (void)
{
#ifdef SA_NOCLDSTOP
  /* (Based on info from Paul Eggert found in coreutils.)  Don't use
     HAVE_SIGACTION to decide whether to use the sa_handler, sa_flags,
     sa_mask members, as some systems (Solaris 7+) don't define them.  Use
     SA_NOCLDSTOP instead; it's been part of POSIX.1 since day 1 (in 1988).  */
  info_signal_handler.sa_handler = info_signal_proc;
  info_signal_handler.sa_flags = 0;
  mask_termsig (&info_signal_handler.sa_mask);
#endif /* SA_NOCLDSTOP */

#if defined (SIGTSTP)
  set_termsig (SIGTSTP, &old_TSTP);
  set_termsig (SIGTTOU, &old_TTOU);
  set_termsig (SIGTTIN, &old_TTIN);
#endif /* SIGTSTP */

#if defined (SIGWINCH)
  set_termsig (SIGWINCH, &old_WINCH);
#endif

#if defined (SIGQUIT)
  set_termsig (SIGQUIT, &old_QUIT);
#endif

#if defined (SIGINT)
  set_termsig (SIGINT, &old_INT);
#endif

#if defined (SIGTERM)
  set_termsig (SIGTERM, &old_TERM);
#endif

#if defined (SIGUSR1)
  /* Used by DJGPP to simulate SIGTSTP on Ctrl-Z.  */
  set_termsig (SIGUSR1, &old_USR1);
#endif
}

void
redisplay_after_signal (void)
{
  terminal_clear_screen ();
  display_clear_display (the_display);
  if (auto_footnotes_p)
    info_get_or_remove_footnotes (active_window);
  window_mark_chain (windows, W_UpdateWindow);
  display_update_display ();
  display_cursor_at_point (active_window);
  fflush (stdout);
}

void
reset_info_window_sizes (void)
{
  terminal_get_screen_size ();
  display_initialize_display (screenwidth, screenheight);
  window_new_screen_size (screenwidth, screenheight);
  redisplay_after_signal ();
}

/* Number of times we were told to ignore SIGWINCH. */
static int sigwinch_block_count = 0;

void
signal_block_winch (void)
{
#if defined (SIGWINCH)
  if (sigwinch_block_count == 0)
    BLOCK_SIGNAL (SIGWINCH);
  sigwinch_block_count++;
#endif
}

void
signal_unblock_winch (void)
{
#if defined (SIGWINCH)
  sigwinch_block_count--;
  if (sigwinch_block_count == 0)
    UNBLOCK_SIGNAL (SIGWINCH);
#endif
}

static RETSIGTYPE
info_signal_proc (int sig)
{
  signal_info *old_signal_handler = NULL;

#if !defined (HAVE_SIGACTION)
  /* best effort: first increment this counter and later block signals */
  if (term_conf_busy)
    return;
  term_conf_busy++;
#if defined (HAVE_SIGPROCMASK) || defined (HAVE_SIGSETMASK)
    {
      sigset_t nvar, ovar;
      sigemptyset (&nvar);
      mask_termsig (&nvar);
      sigprocmask (SIG_BLOCK, &nvar, &ovar);
    }
#endif /* HAVE_SIGPROCMASK || HAVE_SIGSETMASK */
#endif /* !HAVE_SIGACTION */
  switch (sig)
    {
#if defined (SIGTSTP)
    case SIGTSTP:
    case SIGTTOU:
    case SIGTTIN:
#endif
#if defined (SIGQUIT)
    case SIGQUIT:
#endif
#if defined (SIGINT)
    case SIGINT:
#endif
#if defined (SIGTERM)
    case SIGTERM:
#endif
      {
#if defined (SIGTSTP)
        if (sig == SIGTSTP)
          old_signal_handler = &old_TSTP;
        if (sig == SIGTTOU)
          old_signal_handler = &old_TTOU;
        if (sig == SIGTTIN)
          old_signal_handler = &old_TTIN;
#endif /* SIGTSTP */
#if defined (SIGQUIT)
        if (sig == SIGQUIT)
          old_signal_handler = &old_QUIT;
#endif /* SIGQUIT */
#if defined (SIGINT)
        if (sig == SIGINT)
          old_signal_handler = &old_INT;
#endif /* SIGINT */
#if defined (SIGTERM)
        if (sig == SIGTERM)
          old_signal_handler = &old_TERM;
#endif /* SIGTERM */

        /* For stop signals, restore the terminal IO, leave the cursor
           at the bottom of the window, and stop us. */
        terminal_goto_xy (0, screenheight - 1);
        terminal_clear_to_eol ();
        fflush (stdout);
        terminal_unprep_terminal ();
	restore_termsig (sig, old_signal_handler);
	UNBLOCK_SIGNAL (sig);
	kill (getpid (), sig);

        /* The program is returning now.  Restore our signal handler,
           turn on terminal handling, redraw the screen, and place the
           cursor where it belongs. */
        terminal_prep_terminal ();
	set_termsig (sig, old_signal_handler);
	/* window size might be changed while sleeping */
	reset_info_window_sizes ();
      }
      break;

#if defined (SIGWINCH) || defined (SIGUSR1)
#ifdef SIGWINCH
    case SIGWINCH:
#endif
#ifdef SIGUSR1
    case SIGUSR1:
#endif
      {
	/* Turn off terminal IO, tell our parent that the window has changed,
	   then reinitialize the terminal and rebuild our windows. */
#ifdef SIGWINCH
	if (sig == SIGWINCH)
	  old_signal_handler = &old_WINCH;
#endif
#ifdef SIGUSR1
	if (sig == SIGUSR1)
	  old_signal_handler = &old_USR1;
#endif

        /* This seems risky: what if we receive a (real) signal before
           the next line is reached? */
#if 0
	restore_termsig (sig, old_signal_handler);
	kill (getpid (), sig);
#endif

	/* After our old signal handler returns... */
	set_termsig (sig, old_signal_handler); /* needless? */

        if (sigwinch_block_count != 0)
          abort ();

        /* Avoid any of the code unblocking the signal too early.  This
           should set the variable to 1 because we shouldn't be here if
           sigwinch_block_count > 0. */
        sigwinch_block_count++;

	reset_info_window_sizes ();

        sigwinch_block_count--;
        /* Don't unblock the signal until after we've finished. */
	UNBLOCK_SIGNAL (sig);
      }
      break;
#endif /* SIGWINCH || SIGUSR1 */
    }
#if !defined (HAVE_SIGACTION)
  /* at this time it is safer to perform unblock after decrement */
  term_conf_busy--;
#if defined (HAVE_SIGPROCMASK) || defined (HAVE_SIGSETMASK)
    {
      sigset_t nvar, ovar;
      sigemptyset (&nvar);
      mask_termsig (&nvar);
      sigprocmask (SIG_UNBLOCK, &nvar, &ovar);
    }
#endif /* HAVE_SIGPROCMASK || HAVE_SIGSETMASK */
#endif /* !HAVE_SIGACTION */
}


/* vim: set sw=2 cino={1s>2sn-s^-se-s: */
