/*
 * linelength.c: find the terminal line length
 * Preferences: 1. MANWIDTH, 2. COLUMNS, 3. ioctl, 4. 80
 *
 * Originally adapted from Andries Brouwer's man implementation, also
 * released under the GPL: authors believed to include Martin Schulze and
 * Jon Tombs, dated 1995/09/02.
 *
 * Changes for man-db copyright (C) 2001, 2003, 2007 Colin Watson.
 *
 * This file is part of man-db.
 *
 * man-db is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * man-db is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with man-db; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA.
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "linelength.h"

#ifndef _PATH_TTY
# define _PATH_TTY "/dev/tty"
#endif /* _PATH_TTY */

static int line_length = -1;

int get_line_length (void)
{
	const char *columns;
	int width;
#ifdef TIOCGWINSZ
	int dev_tty, tty_fd = -1;
#endif

	if (line_length != -1)
		return line_length;

	line_length = 80;

	columns = getenv ("MANWIDTH");
	if (columns != NULL) {
		width = atoi (columns);
		if (width > 0)
			return line_length = width;
	}

	columns = getenv ("COLUMNS");
	if (columns != NULL) {
		width = atoi (columns);
		if (width > 0)
			return line_length = width;
	}

#ifdef TIOCGWINSZ
	/* Original TIOCGWINSZ approach was from Jon Tombs.
	 * We don't require both stdin and stdout to be a tty, and line
	 * length is primarily a property of output. However, if it happens
	 * that stdin is connected to a terminal but stdout isn't, then that
	 * may well be because the user is trying something like
	 * 'MAN_KEEP_STDERR=1 man foo >/dev/null' to see just the error
	 * messages, so use the window size from stdin as a fallback.
	 * In some cases we may have neither (e.g. if man is running inside
	 * lesspipe); /dev/tty should be a reliable way to get to the
	 * current tty if it exists.
	 */
	dev_tty = open (_PATH_TTY, O_RDONLY);
	if (dev_tty >= 0)
		tty_fd = dev_tty;
	else if (isatty (STDOUT_FILENO))
		tty_fd = STDOUT_FILENO;
	else if (isatty (STDIN_FILENO))
		tty_fd = STDIN_FILENO;
	if (tty_fd >= 0) {
		int ret;
		struct winsize wsz;

		ret = ioctl (tty_fd, TIOCGWINSZ, &wsz);
		if (dev_tty >= 0)
			close (dev_tty);
		if (ret)
			perror ("TIOCGWINSZ failed");
		else if (wsz.ws_col)
			return line_length = wsz.ws_col;
	}
#endif

	return line_length = 80;
}
