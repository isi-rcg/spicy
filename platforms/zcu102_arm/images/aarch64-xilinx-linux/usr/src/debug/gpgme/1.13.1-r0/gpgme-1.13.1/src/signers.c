/* signers.c - Maintain signer sets.
 * Copyright (C) 2001 Werner Koch (dd9jn)
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

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>

#include "gpgme.h"
#include "util.h"
#include "context.h"
#include "debug.h"


/* Delete all signers from CTX.  */
void
_gpgme_signers_clear (gpgme_ctx_t ctx)
{
  unsigned int i;

  if (!ctx || !ctx->signers)
    return;

  for (i = 0; i < ctx->signers_len; i++)
    {
      assert (ctx->signers[i]);
      gpgme_key_unref (ctx->signers[i]);
      ctx->signers[i] = NULL;
    }
  ctx->signers_len = 0;
}


void
gpgme_signers_clear (gpgme_ctx_t ctx)
{
  TRACE (DEBUG_CTX, "gpgme_signers_clear", ctx, "");
  _gpgme_signers_clear (ctx);
}


/* Add KEY to list of signers in CTX.  */
gpgme_error_t
gpgme_signers_add (gpgme_ctx_t ctx, const gpgme_key_t key)
{
  TRACE_BEG  (DEBUG_CTX, "gpgme_signers_add", ctx,
	      "key=%p (%s)", key, (key && key->subkeys && key->subkeys->fpr) ?
	      key->subkeys->fpr : "invalid");

  if (!ctx || !key)
    return TRACE_ERR (gpg_error (GPG_ERR_INV_VALUE));

  if (ctx->signers_len == ctx->signers_size)
    {
      gpgme_key_t *newarr;
      int n = ctx->signers_size + 5;
      int j;

      newarr = realloc (ctx->signers, n * sizeof (*newarr));
      if (!newarr)
	return TRACE_ERR (gpg_error_from_syserror ());
      for (j = ctx->signers_size; j < n; j++)
	newarr[j] = NULL;
      ctx->signers = newarr;
      ctx->signers_size = n;
    }

  gpgme_key_ref (key);
  ctx->signers[ctx->signers_len++] = key;
  TRACE_SUC ("");
  return 0;
}


/* Return the number of signers in CTX.  */
unsigned int
gpgme_signers_count (const gpgme_ctx_t ctx)
{
  return ctx? ctx->signers_len : 0;
}


/* Return the SEQth signer's key in CTX with one reference.  */
gpgme_key_t
gpgme_signers_enum (const gpgme_ctx_t ctx, int seq)
{
  unsigned int seqno;

  if (!ctx || seq < 0)
    return NULL;

  seqno = (unsigned int) seq;
  if (seqno >= ctx->signers_len)
    return NULL;
  gpgme_key_ref (ctx->signers[seqno]);
  return ctx->signers[seqno];
}
