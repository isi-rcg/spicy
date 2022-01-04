/* progress.c -  status handler for progress status
 * Copyright (C) 2000 Werner Koch (dd9jn)
 * Copyright (C) 2001, 2002, 2003, 2004 g10 Code GmbH
 *
 * This file is part of GPGME.
 *
 * GPGME is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * GPGME is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, see <https://gnu.org/licenses/>.
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "util.h"
#include "context.h"
#include "debug.h"


/* The status handler for progress status lines which also monitors
 * the PINENTRY_LAUNCHED status.  */
gpgme_error_t
_gpgme_progress_status_handler (void *priv, gpgme_status_code_t code,
				char *args)
{
  gpgme_ctx_t ctx = (gpgme_ctx_t) priv;
  char *p;
  char *args_cpy;
  int type = 0;
  int current = 0;
  int total = 0;

  if (code == GPGME_STATUS_PINENTRY_LAUNCHED)
    {
      ctx->redraw_suggested = 1;
      return 0;
    }

  if (code != GPGME_STATUS_PROGRESS || !*args || !ctx->progress_cb)
    return 0;

  args_cpy = strdup (args);
  if (!args_cpy)
    return gpg_error_from_syserror ();

  p = strchr (args_cpy, ' ');
  if (p)
    {
      *p++ = 0;
      if (*p)
	{
	  type = *(unsigned char *)p;
	  p = strchr (p+1, ' ');
	  if (p)
	    {
	      *p++ = 0;
	      if (*p)
		{
		  current = atoi (p);
		  p = strchr (p+1, ' ');
		  if (p)
		    {
		      *p++ = 0;
		      total = atoi (p);
		    }
		}
	    }
	}
    }

  if (type != 'X')
    ctx->progress_cb (ctx->progress_cb_value, args_cpy, type, current, total);

  free (args_cpy);
  return 0;
}
