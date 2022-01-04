/* man.h: Defines and external function declarations for man.c.
   $Id: man.h 5526 2014-05-07 14:37:33Z karl $

   Copyright 1993, 1997, 2004, 2007, 2013, 2014 Free Software Foundation, Inc.

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

#ifndef INFO_MAN_H
#define INFO_MAN_H

#define MANPAGE_FILE_BUFFER_NAME "*manpages*"

extern NODE *get_manpage_node (char *pagename);

#endif /* INFO_MAN_H */
