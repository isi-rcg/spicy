/* FriBidi
 * fribidi.c - Unicode bidirectional and Arabic joining/shaping algorithms
 *
 * Authors:
 *   Behdad Esfahbod, 2001, 2002, 2004
 *   Dov Grobgeld, 1999, 2000
 *
 * Copyright (C) 2004 Sharif FarsiWeb, Inc
 * Copyright (C) 2001,2002 Behdad Esfahbod
 * Copyright (C) 1999,2000 Dov Grobgeld
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library, in a file named COPYING; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA
 * 
 * For licensing issues, contact <fribidi.license@gmail.com>.
 */

#include "common.h"

#include <fribidi.h>

#ifdef DEBUG
static int flag_debug = false;
#endif

FRIBIDI_ENTRY fribidi_boolean
fribidi_debug_status (
  void
)
{
#ifdef DEBUG
  return flag_debug;
#else
  return false;
#endif
}

FRIBIDI_ENTRY fribidi_boolean
fribidi_set_debug (
  /* input */
  fribidi_boolean state
)
{
#ifdef DEBUG
  return flag_debug = state;
#else
  return false;
#endif
}



const char *fribidi_unicode_version = FRIBIDI_UNICODE_VERSION;

const char *fribidi_version_info =
  "(" FRIBIDI_NAME ") " FRIBIDI_VERSION "\n"
  "interface version " FRIBIDI_INTERFACE_VERSION_STRING ",\n"
  "Unicode Character Database version " FRIBIDI_UNICODE_VERSION ",\n"
  "Configure options"
#ifdef DEBUG
  " --enable-debug"
#endif /* DEBUG */
  ".\n\n"
  "Copyright (C) 2004  Sharif FarsiWeb, Inc.\n"
  "Copyright (C) 2001, 2002, 2004, 2005  Behdad Esfahbod\n"
  "Copyright (C) 1999, 2000, 2017  Dov Grobgeld\n"
  FRIBIDI_NAME " comes with NO WARRANTY, to the extent permitted by law.\n"
  "You may redistribute copies of " FRIBIDI_NAME " under\n"
  "the terms of the GNU Lesser General Public License.\n"
  "For more information about these matters, see the file named COPYING.\n\n"
  "Written by Behdad Esfahbod and Dov Grobgeld.\n";

/* Editor directions:
 * vim:textwidth=78:tabstop=8:shiftwidth=2:autoindent:cindent
 */
