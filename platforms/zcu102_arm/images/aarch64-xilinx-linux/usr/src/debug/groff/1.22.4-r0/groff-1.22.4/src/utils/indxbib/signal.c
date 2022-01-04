/* Copyright (C) 1992-2018 Free Software Foundation, Inc.
     Written by James Clark (jjc@jclark.com)

This file is part of groff.

groff is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation, either version 3 of the License, or
(at your option) any later version.

groff is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>. */

/* Unfortunately vendors seem to have problems writing a <signal.h>
that is correct for C++, so we implement all signal handling in C. */

#include <stdlib.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <signal.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Prototype */
void catch_fatal_signals(void);

extern void cleanup(void);

static RETSIGTYPE handle_fatal_signal(int signum)
{
  signal(signum, SIG_DFL);
  cleanup();
#ifdef HAVE_KILL
  kill(getpid(), signum);
#else
  /* MS-DOS and Win32 don't have kill(); the best compromise is
     probably to use exit() instead. */
  exit(signum);
#endif
}

void catch_fatal_signals(void)
{
#ifdef SIGHUP
  signal(SIGHUP, handle_fatal_signal);
#endif
  signal(SIGINT, handle_fatal_signal);
  signal(SIGTERM, handle_fatal_signal);
}

#ifdef __cplusplus
}
#endif

#ifndef HAVE_RENAME

void ignore_fatal_signals()
{
#ifdef SIGHUP
  signal(SIGHUP, SIG_IGN);
#endif
  signal(SIGINT, SIG_IGN);
  signal(SIGTERM, SIG_IGN);
}

#endif /* not HAVE_RENAME */
