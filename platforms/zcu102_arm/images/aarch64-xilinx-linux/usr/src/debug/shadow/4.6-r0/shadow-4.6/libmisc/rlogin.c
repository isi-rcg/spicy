/*
 * Copyright (c) 1989 - 1994, Julianne Frances Haugh
 * Copyright (c) 1996 - 1999, Marek Michałkiewicz
 * Copyright (c) 2003 - 2005, Tomasz Kłoczko
 * Copyright (c) 2007 - 2008, Nicolas François
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the copyright holders or contributors may not be used to
 *    endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT
 * HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <config.h>

#ifdef RLOGIN

#ident "$Id$"

#include "prototypes.h"
#include "defines.h"
#include <stdio.h>
#include <pwd.h>
#include <netdb.h>
static struct {
	int spd_name;
	int spd_baud;
} speed_table[] =
{
#ifdef B50
	{
	B50, 50},
#endif
#ifdef B75
	{
	B75, 75},
#endif
#ifdef B110
	{
	B110, 110},
#endif
#ifdef B134
	{
	B134, 134},
#endif
#ifdef B150
	{
	B150, 150},
#endif
#ifdef B200
	{
	B200, 200},
#endif
#ifdef B300
	{
	B300, 300},
#endif
#ifdef B600
	{
	B600, 600},
#endif
#ifdef B1200
	{
	B1200, 1200},
#endif
#ifdef B1800
	{
	B1800, 1800},
#endif
#ifdef B2400
	{
	B2400, 2400},
#endif
#ifdef B4800
	{
	B4800, 4800},
#endif
#ifdef B9600
	{
	B9600, 9600},
#endif
#ifdef B19200
	{
	B19200, 19200},
#endif
#ifdef B38400
	{
	B38400, 38400},
#endif
	{
	-1, -1}
};

static void get_remote_string (char *buf, size_t size)
{
	for (;;) {
		if (read (0, buf, 1) != 1) {
			exit (EXIT_FAILURE);
		}
		if ('\0' == *buf) {
			return;
		}
		--size;
		if (size > 0) {
			++buf;
		}
	}
 /*NOTREACHED*/}

int
do_rlogin (const char *remote_host, char *name, size_t namelen, char *term,
           size_t termlen)
{
	struct passwd *pwd;
	char remote_name[32];
	char *cp;
	unsigned long remote_speed = 9600;
	int speed_name = B9600;
	int i;
	TERMIO termio;

	get_remote_string (remote_name, sizeof remote_name);
	get_remote_string (name, namelen);
	get_remote_string (term, termlen);

	cp = strchr (term, '/');
	if (NULL != cp) {
		*cp = '\0';
		cp++;

		if (getulong (cp, &remote_speed) == 0) {
			remote_speed = 9600;
		}
	}
	for (i = 0;
	     (   (speed_table[i].spd_baud != remote_speed)
	      && (speed_table[i].spd_name != -1));
	     i++);

	if (-1 != speed_table[i].spd_name) {
		speed_name = speed_table[i].spd_name;
	}

	/*
	 * Put the terminal in cooked mode with echo turned on.
	 */

	GTTY (0, &termio);
	termio.c_iflag |= ICRNL | IXON;
	termio.c_oflag |= OPOST | ONLCR;
	termio.c_lflag |= ICANON | ECHO | ECHOE;
#ifdef CBAUD
	termio.c_cflag = (termio.c_cflag & ~CBAUD) | speed_name;
#else
	termio.c_cflag = (termio.c_cflag) | speed_name;
#endif
	STTY (0, &termio);

	pwd = getpwnam (name); /* local, no need for xgetpwnam */
	if (NULL == pwd) {
		return 0;
	}

	/*
	 * ruserok() returns 0 for success on modern systems, and 1 on
	 * older ones.  If you are having trouble with people logging
	 * in without giving a required password, THIS is the culprit -
	 * go fix the #define in config.h.
	 */

#ifndef	RUSEROK
	return 0;
#else
	return ruserok (remote_host, pwd->pw_uid == 0,
			remote_name, name) == RUSEROK;
#endif
}
#endif				/* RLOGIN */

