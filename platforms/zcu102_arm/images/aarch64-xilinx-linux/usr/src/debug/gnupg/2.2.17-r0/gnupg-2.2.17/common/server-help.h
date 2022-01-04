/* server-help.h - Helper functions for writing Assuan servers.
 *	Copyright (C) 2003, 2009, 2010 g10 Code GmbH
 *
 * This file is part of GnuPG.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either
 *
 *   - the GNU Lesser General Public License as published by the Free
 *     Software Foundation; either version 3 of the License, or (at
 *     your option) any later version.
 *
 * or
 *
 *   - the GNU General Public License as published by the Free
 *     Software Foundation; either version 2 of the License, or (at
 *     your option) any later version.
 *
 * or both in parallel, as here.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <https://www.gnu.org/licenses/>.
 */

#ifndef GNUPG_COMMON_SERVER_HELP_H
#define GNUPG_COMMON_SERVER_HELP_H

/* Skip over options in LINE.

   Blanks after the options are also removed.  Options are indicated
   by two leading dashes followed by a string consisting of non-space
   characters.  The special option "--" indicates an explicit end of
   options; all what follows will not be considered an option.  The
   first no-option string also indicates the end of option parsing. */
char *skip_options (const char *line);

/* Check whether the option NAME appears in LINE.  */
int has_option (const char *line, const char *name);

/* Same as has_option but only considers options at the begin of the
   line.  This is useful for commands which allow arbitrary strings on
   the line.  */
int has_leading_option (const char *line, const char *name);

/* Same as has_option but does only test for the name of the option
   and ignores an argument, i.e. with NAME being "--hash" it would
   return a pointer for "--hash" as well as for "--hash=foo".  If
   there is no such option NULL is returned.  The pointer returned
   points right behind the option name, this may be an equal sign, Nul
   or a space.  */
const char *has_option_name (const char *line, const char *name);

/* Same as has_option_name but ignores all options after a "--" and
 * does not return a const char ptr.  */
char *has_leading_option_name (char *line, const char *name);

/* Parse an option with the format "--NAME=VALUE" and return the value
 * as a malloced string.  */
gpg_error_t get_option_value (char *line, const char *name, char **r_value);

/* Return a pointer to the argument of the option with NAME.  If such
   an option is not given, NULL is returned. */
char *option_value (const char *line, const char *name);

#endif	/* GNUPG_COMMON_SERVER_HELP_H */
