/* slutty.c --- Unix Low level terminal (tty) functions for S-Lang */
/*
Copyright (C) 2004-2017,2018 John E. Davis

This file is part of the S-Lang Library.

The S-Lang Library is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

The S-Lang Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
USA.
*/

#include "slinclud.h"

#include <signal.h>
/* sequent support thanks to Kenneth Lorber <keni@oasys.dt.navy.mil> */
/* SYSV (SYSV ISC R3.2 v3.0) provided by iain.lea@erlm.siemens.de */

#if defined (_AIX) && !defined (_ALL_SOURCE)
# define _ALL_SOURCE	/* so NBBY is defined in <sys/types.h> */
#endif

#include <sys/time.h>
#include <sys/types.h>

#ifdef SYSV
# include <fcntl.h>
# ifndef CRAY
#  include <sys/termio.h>
#  include <sys/stream.h>
#  include <sys/ptem.h>
#  include <sys/tty.h>
# endif
#endif

#if defined(__BEOS__) && !defined(__HAIKU__)
/* Prototype for select */
# include <net/socket.h>
#endif

#include <sys/file.h>

#ifndef sun
# include <sys/ioctl.h>
#endif

#if defined(__QNX__) || defined(__HAIKU__)
# include <sys/select.h>
#endif

#include <sys/stat.h>
#include <errno.h>

#if defined (_AIX) && !defined (FD_SET)
# include <sys/select.h>	/* for FD_ISSET, FD_SET, FD_ZERO */
#endif

#if !defined(O_RDWR) || !defined(FD_CLOEXEC)
# include <fcntl.h>
#endif

#include "slang.h"
#include "_slang.h"

int SLang_TT_Read_FD = -1;
int SLang_TT_Baud_Rate = 0;

#ifdef HAVE_TERMIOS_H
# if !defined(HAVE_TCGETATTR) || !defined(HAVE_TCSETATTR)
#   undef HAVE_TERMIOS_H
# endif
#endif

#ifndef HAVE_TERMIOS_H

# if !defined(CBREAK) && defined(sun)
#  ifndef BSD_COMP
#   define BSD_COMP 1
#  endif
#  include <sys/ioctl.h>
# endif

typedef struct
  {
      struct tchars t;
      struct ltchars lt;
      struct sgttyb s;
  }
TTY_Termio_Type;
#else
# include <termios.h>
typedef struct termios TTY_Termio_Type;
#endif

static TTY_Termio_Type Old_TTY;

#ifdef HAVE_TERMIOS_H
typedef SLCONST struct
{
   unsigned int key;
   unsigned int value;
} Baud_Rate_Type;

static Baud_Rate_Type Baud_Rates [] =
{
#ifdef B0
     {B0, 0},
#endif
#ifdef B50
     {B50, 50},
#endif
#ifdef B75
     {B75, 75},
#endif
#ifdef B110
     {B110, 110},
#endif
#ifdef B134
     {B134, 134},
#endif
#ifdef B150
     {B150, 150},
#endif
#ifdef B200
     {B200, 200},
#endif
#ifdef B300
     {B300, 300},
#endif
#ifdef B600
     {B600, 600},
#endif
#ifdef B1200
     {B1200, 1200},
#endif
#ifdef B1800
     {B1800, 1800},
#endif
#ifdef B2400
     {B2400, 2400},
#endif
#ifdef B4800
     {B4800, 4800},
#endif
#ifdef B9600
     {B9600, 9600},
#endif
#ifdef B19200
     {B19200, 19200},
#endif
#ifdef B38400
     {B38400, 38400},
#endif
#ifdef B57600
     {B57600, 57600},
#endif
#ifdef B115200
     {B115200, 115200},
#endif
#ifdef B230400
     {B230400, 230400},
#endif
#ifdef B460800
     {B460800, 460800},
#endif
#ifdef B500000
     {B500000, 500000},
#endif
#ifdef B576000
     {B576000, 576000},
#endif
#ifdef B921600
     {B921600, 921600},
#endif
#ifdef B1000000
     {B1000000, 1000000},
#endif
#ifdef B1152000
     {B1152000, 1152000},
#endif
#ifdef B1500000
     {B1500000, 1500000},
#endif
#ifdef B2000000
     {B2000000, 2000000},
#endif
#ifdef B2500000
     {B2500000, 2500000},
#endif
#ifdef B3000000
     {B3000000, 3000000},
#endif
#ifdef B3500000
     {B3500000, 3500000},
#endif
#ifdef B4000000
     {B4000000, 4000000},
#endif
     {0, 0}
};

static void
set_baud_rate (TTY_Termio_Type *tty)
{
#ifdef HAVE_CFGETOSPEED
   unsigned int speed;
   Baud_Rate_Type *b, *bmax;

   if (SLang_TT_Baud_Rate)
     return;			       /* already set */

   speed = (unsigned int) cfgetospeed (tty);

   b = Baud_Rates;
   bmax = b + (sizeof (Baud_Rates)/sizeof(Baud_Rates[0]));
   while (b < bmax)
     {
	if (b->key == speed)
	  {
	     SLang_TT_Baud_Rate = b->value;
	     return;
	  }
	b++;
     }
#else
   (void) tty;
#endif
}

#endif				       /* HAVE_TERMIOS_H */

#ifdef HAVE_TERMIOS_H
# define GET_TERMIOS(fd, x) tcgetattr(fd, x)
# define SET_TERMIOS(fd, x) tcsetattr(fd, TCSADRAIN, x)
#else
# ifdef TCGETS
#  define GET_TERMIOS(fd, x) ioctl(fd, TCGETS, x)
#  define SET_TERMIOS(fd, x) ioctl(fd, TCSETS, x)
# else
#  define X(x,m)  &(((TTY_Termio_Type *)(x))->m)
#  define GET_TERMIOS(fd, x)	\
    ((ioctl(fd, TIOCGETC, X(x,t)) || \
      ioctl(fd, TIOCGLTC, X(x,lt)) || \
      ioctl(fd, TIOCGETP, X(x,s))) ? -1 : 0)
#  define SET_TERMIOS(fd, x)	\
    ((ioctl(fd, TIOCSETC, X(x,t)) ||\
      ioctl(fd, TIOCSLTC, X(x,lt)) || \
      ioctl(fd, TIOCSETP, X(x,s))) ? -1 : 0)
# endif
#endif

static int TTY_Inited = 0;
static int TTY_Open = 0;

#ifdef ultrix   /* Ultrix gets _POSIX_VDISABLE wrong! */
# define NULL_VALUE -1
#else
# ifdef _POSIX_VDISABLE
#  define NULL_VALUE _POSIX_VDISABLE
# else
#  define NULL_VALUE 255
# endif
#endif

/* If no_flow_control < 0, do not change the tty IXON bit */
int SLang_init_tty (int abort_char, int no_flow_control, int opost)
{
   TTY_Termio_Type newtty;

   SLsig_block_signals ();

   if (TTY_Inited)
     {
	SLsig_unblock_signals ();
	return 0;
     }

   TTY_Open = 0;
   SLKeyBoard_Quit = 0;

   if ((SLang_TT_Read_FD == -1)
       || (1 != isatty (SLang_TT_Read_FD)))
     {
#ifdef O_RDWR
# if !defined(__BEOS__) && !defined(__APPLE__)
	/* I have been told that BEOS will HANG if passed /dev/tty */
	if ((SLang_TT_Read_FD = open("/dev/tty", O_RDWR)) >= 0)
	  {
#  ifdef FD_CLOEXEC
	     /* Make sure /dev/tty is closed upon exec */
	     int flags = fcntl (SLang_TT_Read_FD, F_GETFD);
	     if (flags >= 0)
	       (void) fcntl(SLang_TT_Read_FD, F_SETFD, flags | FD_CLOEXEC);
#  endif
	     TTY_Open = 1;
	  }
# endif
#endif
	if (TTY_Open == 0)
	  {
	     SLang_TT_Read_FD = fileno (stderr);
	     if (1 != isatty (SLang_TT_Read_FD))
	       {
		  SLang_TT_Read_FD = fileno (stdin);
		  if (1 != isatty (SLang_TT_Read_FD))
		    {
		       fprintf (stderr, "Failed to open terminal.");
		       return -1;
		    }
	       }
	  }
     }

   SLang_Abort_Char = abort_char;

   /* Some systems may not permit signals to be blocked.  As a result, the
    * return code must be checked.
    */
   while (-1 == GET_TERMIOS(SLang_TT_Read_FD, &Old_TTY))
     {
	if (errno != EINTR)
	  {
	     SLsig_unblock_signals ();
	     return -1;
	  }
     }

   while (-1 == GET_TERMIOS(SLang_TT_Read_FD, &newtty))
     {
	if (errno != EINTR)
	  {
	     SLsig_unblock_signals ();
	     return -1;
	  }
     }

#ifndef HAVE_TERMIOS_H
   (void) opost;
   (void) no_flow_control;
   newtty.s.sg_flags &= ~(ECHO);
   newtty.s.sg_flags &= ~(CRMOD);
   /*   if (Flow_Control == 0) newtty.s.sg_flags &= ~IXON; */
   newtty.t.t_eofc = 1;
   if (abort_char == -1) SLang_Abort_Char = newtty.t.t_intrc;
   newtty.t.t_intrc = SLang_Abort_Char;	/* ^G */
   newtty.t.t_quitc = 255;
   newtty.lt.t_suspc = 255;   /* to ignore ^Z */
   newtty.lt.t_dsuspc = 255;    /* to ignore ^Y */
   newtty.lt.t_lnextc = 255;
   newtty.s.sg_flags |= CBREAK;		/* do I want cbreak or raw????? */
#else

   /* get baud rate */

   newtty.c_iflag &= ~(ECHO | INLCR | ICRNL);
#ifdef ISTRIP
   /* newtty.c_iflag &= ~ISTRIP; */
#endif
   if (opost == 0) newtty.c_oflag &= ~OPOST;

   set_baud_rate (&newtty);

   if (no_flow_control > 0)
     newtty.c_iflag &= ~IXON;
   else if (no_flow_control == 0)
     newtty.c_iflag |= IXON;

   newtty.c_cc[VEOF] = 1;
   newtty.c_cc[VMIN] = 1;
   newtty.c_cc[VTIME] = 0;
   newtty.c_lflag = ISIG | NOFLSH;
   if (abort_char == -1) SLang_Abort_Char = newtty.c_cc[VINTR];
   newtty.c_cc[VINTR] = SLang_Abort_Char;   /* ^G */
   newtty.c_cc[VQUIT] = NULL_VALUE;
   newtty.c_cc[VSUSP] = NULL_VALUE;   /* to ignore ^Z */
#ifdef VDSUSP
   newtty.c_cc[VDSUSP] = NULL_VALUE;   /* to ignore ^Y */
#endif
#ifdef VLNEXT
   newtty.c_cc[VLNEXT] = NULL_VALUE;   /* to ignore ^V ? */
#endif
#ifdef VSWTCH
   newtty.c_cc[VSWTCH] = NULL_VALUE;   /* to ignore who knows what */
#endif
#endif /* NOT HAVE_TERMIOS_H */

   while (-1 == SET_TERMIOS(SLang_TT_Read_FD, &newtty))
     {
	if (errno != EINTR)
	  {
	     SLsig_unblock_signals ();
	     return -1;
	  }
     }

   TTY_Inited = 1;
   SLsig_unblock_signals ();
   return 0;
}

void SLtty_set_suspend_state (int mode)
{
   TTY_Termio_Type newtty;

   SLsig_block_signals ();

   if (TTY_Inited == 0)
     {
	SLsig_unblock_signals ();
	return;
     }

   while ((-1 == GET_TERMIOS (SLang_TT_Read_FD, &newtty))
	  && (errno == EINTR))
     ;

#ifndef HAVE_TERMIOS_H
   /* I do not know if all systems define the t_dsuspc field */
   if (mode == 0)
     {
	newtty.lt.t_suspc = 255;
	newtty.lt.t_dsuspc = 255;
     }
   else
     {
	newtty.lt.t_suspc = Old_TTY.lt.t_suspc;
	newtty.lt.t_dsuspc = Old_TTY.lt.t_dsuspc;
     }
#else
   if (mode == 0)
     {
	newtty.c_cc[VSUSP] = NULL_VALUE;
#ifdef VDSUSP
	newtty.c_cc[VDSUSP] = NULL_VALUE;
#endif
     }
   else
     {
	newtty.c_cc[VSUSP] = Old_TTY.c_cc[VSUSP];
#ifdef VDSUSP
	newtty.c_cc[VDSUSP] = Old_TTY.c_cc[VDSUSP];
#endif
     }
#endif

   while ((-1 == SET_TERMIOS (SLang_TT_Read_FD, &newtty))
	  && (errno == EINTR))
     ;

   SLsig_unblock_signals ();
}

void SLang_reset_tty (void)
{
   SLsig_block_signals ();

   if (TTY_Inited == 0)
     {
	SLsig_unblock_signals ();
	return;
     }

   while ((-1 == SET_TERMIOS(SLang_TT_Read_FD, &Old_TTY))
	  && (errno == EINTR))
     ;

   if (TTY_Open)
     {
	(void) close (SLang_TT_Read_FD);

	TTY_Open = 0;
	SLang_TT_Read_FD = -1;
     }

   TTY_Inited = 0;
   SLsig_unblock_signals ();
}

static void default_sigint (int sig)
{
   sig = errno;			       /* use parameter */

   SLKeyBoard_Quit = 1;
   if (SLang_Ignore_User_Abort == 0) SLang_set_error (SL_USER_BREAK);
   SLsignal_intr (SIGINT, default_sigint);
   errno = sig;
}

int SLang_set_abort_signal (void (*hand)(int))
{
   int save_errno = errno;
   SLSig_Fun_Type *f;

   if (hand == NULL) hand = default_sigint;
   f = SLsignal_intr (SIGINT, hand);

   errno = save_errno;

   if (f == (SLSig_Fun_Type *) SIG_ERR)
     return -1;

   return 0;
}

#ifndef FD_SET
#define FD_SET(fd, tthis) *(tthis) = 1 << (fd)
#define FD_ZERO(tthis)    *(tthis) = 0
#define FD_ISSET(fd, tthis) (*(tthis) & (1 << fd))
typedef int fd_set;
#endif

static fd_set Read_FD_Set;

/* HACK: If > 0, use 1/10 seconds.  If < 0, use 1/1000 seconds */

int _pSLsys_input_pending(int tsecs)
{
   struct timeval wait;
   long usecs, secs;

   if ((TTY_Inited == 0)
       || (SLang_TT_Read_FD < 0))
     {
	errno = EBADF;
	return -1;
     }

   if (tsecs >= 0)
     {
	secs = tsecs / 10;
	usecs = (tsecs % 10) * 100000;
     }
   else
     {
	tsecs = -tsecs;
	secs = tsecs / 1000;
	usecs = (tsecs % 1000) * 1000;
     }

   wait.tv_sec = secs;
   wait.tv_usec = usecs;

   FD_ZERO(&Read_FD_Set);
   FD_SET(SLang_TT_Read_FD, &Read_FD_Set);

   return select(SLang_TT_Read_FD + 1, &Read_FD_Set, NULL, NULL, &wait);
}

int (*SLang_getkey_intr_hook) (void) = NULL;

static int handle_interrupt (void)
{
   if (SLang_getkey_intr_hook != NULL)
     {
	/* int save_tty_fd = SLang_TT_Read_FD; */

	if (-1 == (*SLang_getkey_intr_hook) ())
	  return -1;

	/* The interrupt hook may suspend the process and reset the tty.
	 * When it comes back up, a new descriptor may allocated.
	 * Allow that here.
	 */

	/* if (save_tty_fd != SLang_TT_Read_FD) */
	  /* return -1; */
     }

   return 0;
}

unsigned int _pSLsys_getkey (void)
{
   unsigned char c;

   if (TTY_Inited == 0)
     {
	int ic = fgetc (stdin);
	if (ic == EOF) return SLANG_GETKEY_ERROR;
	return (unsigned int) ic;
     }

   while (1)
     {
	int ret;

	if (SLKeyBoard_Quit)
	  return SLang_Abort_Char;

	if (0 == (ret = _pSLsys_input_pending (100)))
	  continue;

	if (ret != -1)
	  break;

	if (errno == EINTR)
	  {
	     if (-1 == handle_interrupt ())
	       {
		  errno = EINTR;
		  return SLANG_GETKEY_ERROR;
	       }

	     if (SLKeyBoard_Quit)
	       return SLang_Abort_Char;

	     continue;
	  }

	if (SLKeyBoard_Quit)
	  return SLang_Abort_Char;

	break;			       /* let read handle it */
     }

   while (1)
     {
	ssize_t status = read(SLang_TT_Read_FD, (char *) &c, 1);

	if (status > 0)
	  break;

	if (status == 0)
	  {
	     /* We are at the end of a file.  Let application handle it. */
	     return SLANG_GETKEY_ERROR;
	  }

	if (errno == EINTR)
	  {
	     if (-1 == handle_interrupt ())
	       {
		  errno = EINTR;
		  return SLANG_GETKEY_ERROR;
	       }

	     if (SLKeyBoard_Quit)
	       return SLang_Abort_Char;

	     continue;
	  }
#ifdef EAGAIN
	if (errno == EAGAIN)
	  {
	     sleep (1);
	     continue;
	  }
#endif
#ifdef EWOULDBLOCK
	if (errno == EWOULDBLOCK)
	  {
	     sleep (1);
	     continue;
	  }
#endif
#ifdef EIO
	if (errno == EIO)
	  {
	     _pSLang_verror (SL_Read_Error, "_pSLsys_getkey: EIO error");
	     errno = EIO;
	  }
#endif
	return SLANG_GETKEY_ERROR;
     }

   return((unsigned int) c);
}

