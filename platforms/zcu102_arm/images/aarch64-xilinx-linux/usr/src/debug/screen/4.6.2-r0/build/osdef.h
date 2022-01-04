/*
 * This file is automagically created from osdef.sh -- DO NOT EDIT
 */
/* Copyright (c) 1993-2000
 *      Juergen Weigert (jnweiger@immd4.informatik.uni-erlangen.de)
 *      Michael Schroeder (mlschroe@immd4.informatik.uni-erlangen.de)
 * Copyright (c) 1987 Oliver Laumann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program (see the file COPYING); if not, see
 * http://www.gnu.org/licenses/, or contact Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02111-1301  USA
 *
 ****************************************************************
 * $Id$ GNU
 */

/****************************************************************
 * Thanks to Christos S. Zoulas (christos@ee.cornell.edu) who 
 * mangled the screen source through 'gcc -Wall'.
 ****************************************************************
 */

#ifdef USEVARARGS
#endif

#ifdef LOG_NOTICE
#endif

#if defined(sun) || defined(_SEQUENT_)
extern int   _flsbuf __P((int, FILE *));
#endif

#ifdef SYSV
#else
#endif

#ifndef USEBCOPY
# ifdef USEMEMCPY
# else
#  ifdef USEMEMMOVE
#  else
#  endif
# endif
#else
#endif

#ifdef BSDWAIT
#else
#endif


#ifdef HAVE_SETRESUID
#endif
#ifdef HAVE_SETREUID
#endif
#ifdef HAVE_SETEUID
#endif


extern int   tgetent __P((char *, char *));
extern char *tgetstr __P((char *, char **));
extern int   tgetnum __P((char *));
extern int   tgetflag __P((char *));
extern void  tputs __P((char *, int, int (*)(int)));
extern char *tgoto __P((char *, int, int));

#ifdef POSIX
#include <string.h>
#endif







#ifdef _AIX
#else
#endif

#if defined(UTMPOK) && defined(GETUTENT)
#endif

#if defined(sequent) || defined(_SEQUENT_)
extern int   getpseudotty __P((char **, char **));
#ifdef _SEQUENT_
extern int   fvhangup __P((char *));
#endif
#endif

#ifdef HAVE_UTIMES
#endif


# if defined(GETTTYENT) && !defined(GETUTENT) && !defined(UTNOKEEP)
struct ttyent;		/* for getttyent __P */
extern void  setttyent __P((void));
extern struct ttyent *getttyent __P((void));
# endif

#ifdef SVR4
struct rlimit;		/* for getrlimit __P */
extern int getrlimit __P((int, struct rlimit *));
#endif

struct stat;

#if defined(LOADAV) && defined(LOADAV_GETLOADAVG)
extern int getloadavg(double *, int);
#endif

