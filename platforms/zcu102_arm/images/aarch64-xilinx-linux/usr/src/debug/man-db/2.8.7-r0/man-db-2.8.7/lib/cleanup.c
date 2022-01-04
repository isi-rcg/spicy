/*
 * cleanup.c -- simple dynamic cleanup function management
 * Copyright (C) 1995 Markus Armbruster.
 * Copyright (C) 2007 Colin Watson.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; see the file COPYING.LIB.  If not,
 * write to the Free Software Foundation, Inc., 51 Franklin St, Fifth
 * Floor, Boston, MA  02110-1301  USA.
*/

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <stdio.h>		/* SunOS's loosing assert.h needs it */
#include <assert.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#include "manconfig.h"		/* for FATAL */
#include "cleanup.h"



/* Dealing with signals */


/* saved signal actions */
static struct sigaction saved_hup_action;
static struct sigaction saved_int_action;
static struct sigaction saved_term_action;


/* Run cleanups, then reraise signal with default handler. */
static _Noreturn void
sighandler (int signo)
{
  struct sigaction act;
  sigset_t set;

  do_cleanups_sigsafe (1);

  /* set default signal action */
  memset (&act, 0, sizeof act);
  act.sa_handler = SIG_DFL;
  sigemptyset (&act.sa_mask);
  act.sa_flags = 0;
  if (sigaction (signo, &act, NULL)) {
    /* should not happen */
    _exit (FATAL);		/* exit() is taboo from signal handlers! */
  }

  /* unmask signo */
  if (   sigemptyset (&set)
      || sigaddset (&set, signo)
      || sigprocmask (SIG_UNBLOCK, &set, NULL)) {
    /* shouldn't happen */
    _exit (FATAL);		/* exit() is taboo from signal handlers! */
  }

  /* signal has now default action and is unmasked,
     reraise it to terminate program abnormally */
  kill (getpid(), signo);
  abort();
}


/* Save signo's current action to oldact, if its handler is SIG_DFL
   install sighandler, return 0 on success, -1 on failure. */
static int
trap_signal (int signo, struct sigaction *oldact)
{
  if (sigaction (signo, NULL, oldact)) {
    return -1;
  }

  if (oldact->sa_handler == SIG_DFL) {
    struct sigaction act;

    memset (&act, 0, sizeof act);
    act.sa_handler = sighandler;
    sigemptyset (&act.sa_mask);
    act.sa_flags = 0;
    return sigaction (signo, &act, oldact);
  }

  return 0;
}


/* Trap some abnormal exits to call do_cleanups(). */
static int
trap_abnormal_exits (void)
{
  if (   trap_signal (SIGHUP, &saved_hup_action)
      || trap_signal (SIGINT, &saved_int_action)
      || trap_signal (SIGTERM, &saved_term_action))
    return -1;
  return 0;
}


/* Restore signo's action from oldact if its current handler is
   sighandler, return 0 on success, -1 on failure. */
static int
untrap_signal (int signo, struct sigaction *oldact)
{
  struct sigaction act;
  if (sigaction (signo, NULL, &act)) {
    return -1;
  }

  if (act.sa_handler == sighandler) {
    return sigaction (signo, oldact, NULL);
  }

  return 0;
}


/* Undo a previous trap_abnormal_exits(). */
static int
untrap_abnormal_exits (void)
{
  if (  untrap_signal (SIGHUP, &saved_hup_action)
      | untrap_signal (SIGINT, &saved_int_action)
      | untrap_signal (SIGTERM, &saved_term_action))
    return -1;
  return 0;
}



typedef struct {
  cleanup_fun fun;
  void *arg;
  int sigsafe;
} slot;

static slot *stack = NULL;	/* stack of cleanup functions */
static unsigned nslots = 0;	/* #slots in stack */
static unsigned tos = 0;	/* top of stack, 0 <= tos <= nslots */

/* Call cleanup functions in stack from from top to bottom,
 * Automatically called on program termination via exit(3) or default
 * action for SIGHUP, SIGINT or SIGTERM.
 * Since this may be called from a signal handler, do not use free().
 * If in_sighandler is true, cleanup functions with sigsafe=0 will not be
 * called.
 */
void
do_cleanups_sigsafe (int in_sighandler)
{
  unsigned i;

  assert (tos <= nslots);
  for (i = tos; i > 0; --i)
    if (!in_sighandler || stack[i-1].sigsafe)
      stack[i-1].fun (stack[i-1].arg);
}

/* Call cleanup functions in stack from from top to bottom,
 * Automatically called on program termination via exit(3).
 */
void
do_cleanups (void)
{
  do_cleanups_sigsafe (0);
  tos = 0;
  nslots = 0;
  free (stack);
  stack = NULL;
}


/* Push a cleanup function on the cleanup stack,
 * return 0 on success, -1 on failure.
 * Caution: the cleanup function may be called from signal handlers if
 * sigsafe=1. If you just want a convenient atexit() wrapper, pass
 * sigsafe=0.
 */
int
push_cleanup (cleanup_fun fun, void *arg, int sigsafe)
{
  static int handler_installed = 0;

  assert (tos <= nslots);

  if (!handler_installed) {
    if (atexit (do_cleanups))
      return -1;
    handler_installed = 1;
  }

  if (tos == nslots) {
    /* stack is full, allocate another slot */
    /* stack is not expected to grow much, otherwise we would double it */
    slot *new_stack;

    if (stack) {
      new_stack = xnrealloc (stack, nslots+1, sizeof (slot));
    } else {
      new_stack = xnmalloc (nslots+1, sizeof (slot));
    }
      
    if (!new_stack) return -1;
    stack = new_stack;
    ++nslots;
  }

  assert (tos < nslots);
  stack[tos].fun = fun;
  stack[tos].arg = arg;
  stack[tos].sigsafe = sigsafe;
  ++tos;


  trap_abnormal_exits();

  return 0;
}


/* Remove topmost cleanup function from the cleanup stack that matches the
 * given values.
 */
void
pop_cleanup (cleanup_fun fun, void *arg)
{
  unsigned i, j;

  assert (tos > 0);

  for (i = tos; i > 0; --i) {
    if (stack[i-1].fun == fun && stack[i-1].arg == arg) {
      for (j = i; j < tos; ++j)
        stack[j-1] = stack[j];
      --tos;
      break;
    }
  }

  if (tos == 0) untrap_abnormal_exits();
}


/* Pop all cleanup functions from the cleanup stack. */
void
pop_all_cleanups (void)
{
  tos = 0;
  untrap_abnormal_exits();
}
