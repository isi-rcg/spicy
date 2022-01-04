/*
  File: acl_error.c

  Copyright (C) 1999, 2000
  Andreas Gruenbacher, <a.gruenbacher@bestbits.at>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include <stdio.h>
#include <errno.h>
#include <acl/libacl.h>
#include "libacl.h"
#include "config.h"


const char *
acl_error(int code)
{
	switch(code) {
		case ACL_MULTI_ERROR:
			return _("Multiple entries of same type");
		case ACL_DUPLICATE_ERROR:
			return _("Duplicate entries");
		case ACL_MISS_ERROR:
			return _("Missing or wrong entry");
		case ACL_ENTRY_ERROR:
			return _("Invalid entry type");
		default:
			return NULL;
	}
}

