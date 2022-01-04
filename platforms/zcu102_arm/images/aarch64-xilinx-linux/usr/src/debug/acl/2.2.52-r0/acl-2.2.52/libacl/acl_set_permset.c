/*
  File: acl_set_permset.c

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

#include "libacl.h"


/* 23.4.23 */
int
acl_set_permset(acl_entry_t entry_d, acl_permset_t permset_d)
{
	acl_entry_obj *entry_obj_p = ext2int(acl_entry, entry_d);
	acl_permset_obj *acl_permset_obj_p = ext2int(acl_permset, permset_d);
	if (!entry_obj_p || !acl_permset_obj_p)
		return -1;
	entry_obj_p->eperm.i = acl_permset_obj_p->i;
	return 0;
}

