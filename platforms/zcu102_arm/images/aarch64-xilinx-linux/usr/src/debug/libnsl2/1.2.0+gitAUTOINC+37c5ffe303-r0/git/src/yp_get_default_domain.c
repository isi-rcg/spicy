/* Copyright (C) 2014 Thorsten Kukuk
   Author: Thorsten Kukuk <kukuk@suse.de>

   This library is free software: you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   in version 2.1 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>. */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <unistd.h>
#include <rpcsvc/nis.h>
#include <rpcsvc/ypclnt.h>

#include "libc-lock.h"

__libc_lock_define_initialized (static, domainname_lock)
static char ypdomainname[NIS_MAXNAMELEN + 1];

int
yp_get_default_domain (char **outdomain)
{
  int result = YPERR_SUCCESS;;
  *outdomain = NULL;

  __libc_lock_lock (domainname_lock);

  if (ypdomainname[0] == '\0')
    {
      if (getdomainname (ypdomainname, NIS_MAXNAMELEN))
        result = YPERR_NODOM;
      else if (strcmp (ypdomainname, "(none)") == 0)
        {
          /* If domainname is not set, some systems will return "(none)" */
          ypdomainname[0] = '\0';
          result = YPERR_NODOM;
        }
      else
        *outdomain = ypdomainname;
    }
  else
    *outdomain = ypdomainname;

  __libc_lock_unlock (domainname_lock);

  return result;
}
