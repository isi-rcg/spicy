/* tilde.c -- tilde expansion code (~/foo := $HOME/foo).
   $Id: tilde.c 7670 2017-02-05 13:00:17Z gavin $

   Copyright 1988, 1989, 1990, 1991, 1992, 1993, 1996, 1998, 1999,
   2002, 2004, 2006, 2007, 2008, 2012, 2013, 2017 Free Software
   Foundation, Inc.

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
#include "tilde.h"

/* Do the work of tilde expansion on FILENAME.  FILENAME starts with a
   tilde. */
char *
tilde_expand_word (const char *filename)
{
  char *dirname = filename ? xstrdup (filename) : NULL;

  if (dirname && *dirname == '~')
    {
      char *temp_name;
      if (!dirname[1] || IS_SLASH (dirname[1]))
        {
          /* Prepend $HOME to the rest of the string. */
          char *temp_home = getenv ("HOME");

          /* If there is no HOME variable, look up the directory in
             the password database. */
          if (!temp_home)
            {
#ifndef __MINGW32__
              struct passwd *entry;

              entry = (struct passwd *) getpwuid (getuid ());
              if (entry)
                temp_home = entry->pw_dir;
#else
	      temp_home = ".";
#endif
            }

          temp_name = xmalloc (1 + strlen (&dirname[1])
                               + (temp_home ? strlen (temp_home) : 0));
          if (temp_home)
            strcpy (temp_name, temp_home);
          else
            temp_name[0] = 0;
          strcat (temp_name, &dirname[1]);
          free (dirname);
          dirname = xstrdup (temp_name);
          free (temp_name);
        }
      else
        {
#ifndef __MINGW32__
          struct passwd *user_entry;
#endif
          char *username = xmalloc (257);
          int i, c;

          for (i = 1; (c = dirname[i]); i++)
            {
              if (IS_SLASH (c))
                break;
              else
                username[i - 1] = c;
            }
          username[i - 1] = 0;

#ifndef __MINGW32__
          user_entry = (struct passwd *) getpwnam (username);
          if (user_entry)
            {
              temp_name = xmalloc (1 + strlen (user_entry->pw_dir)
                                   + strlen (&dirname[i])); 
              strcpy (temp_name, user_entry->pw_dir);
              strcat (temp_name, &dirname[i]);

              free (dirname);
              dirname = xstrdup (temp_name);
              free (temp_name);
            }

          endpwent ();
          free (username);
#else
	  free (dirname);
	  dirname = xstrdup (temp_name);
	  free (temp_name);
#endif
        }
    }
  return dirname;
}
