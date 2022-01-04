/* gpgconf.c - GnuPG Made Easy.
 * Copyright (C) 2007 g10 Code GmbH
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

#include "gpgme.h"

#include "ops.h"
#include "engine.h"
#include "debug.h"

#include "engine-backend.h"


/* Allocate a new gpgme_conf_arg_t.  */
gpgme_error_t
gpgme_conf_arg_new (gpgme_conf_arg_t *arg_p,
		    gpgme_conf_type_t type, const void *value)
{
  return _gpgme_conf_arg_new (arg_p, type, value);
}


/* This also releases all chained argument structures!  */
void
gpgme_conf_arg_release (gpgme_conf_arg_t arg, gpgme_conf_type_t type)
{
  _gpgme_conf_arg_release (arg, type);
}


/* Register a change for the value of OPT to ARG.  */
gpgme_error_t
gpgme_conf_opt_change (gpgme_conf_opt_t opt, int reset, gpgme_conf_arg_t arg)
{
  return _gpgme_conf_opt_change (opt, reset, arg);
}



/* Public function to release a gpgme_conf_comp list.  */
void
gpgme_conf_release (gpgme_conf_comp_t conf)
{
  _gpgme_conf_release (conf);
}


/* Public function to load a configuration list.  No
   asynchronous interface for now.  */
gpgme_error_t
gpgme_op_conf_load (gpgme_ctx_t ctx, gpgme_conf_comp_t *conf_p)
{
  gpgme_error_t err;
  gpgme_protocol_t proto;

  if (!ctx)
    return gpg_error (GPG_ERR_INV_VALUE);

  proto = ctx->protocol;
  ctx->protocol = GPGME_PROTOCOL_GPGCONF;
  err = _gpgme_op_reset (ctx, 1);
  if (err)
    return err;

  err = _gpgme_engine_op_conf_load (ctx->engine, conf_p);
  ctx->protocol = proto;
  return err;
}


/* This function does not follow chained components!  */
gpgme_error_t
gpgme_op_conf_save (gpgme_ctx_t ctx, gpgme_conf_comp_t comp)
{
  gpgme_error_t err;
  gpgme_protocol_t proto;

  if (!ctx)
    return gpg_error (GPG_ERR_INV_VALUE);

  proto = ctx->protocol;
  ctx->protocol = GPGME_PROTOCOL_GPGCONF;
  err = _gpgme_op_reset (ctx, 1);
  if (err)
    return err;

  err = _gpgme_engine_op_conf_save (ctx->engine, comp);
  ctx->protocol = proto;
  return err;
}


gpgme_error_t
gpgme_op_conf_dir (gpgme_ctx_t ctx, const char *what, char **result)
{
  gpgme_error_t err;
  gpgme_protocol_t proto;

  if (!ctx)
    return gpg_error (GPG_ERR_INV_VALUE);

  proto = ctx->protocol;
  ctx->protocol = GPGME_PROTOCOL_GPGCONF;
  err = _gpgme_op_reset (ctx, 1);
  if (err)
    return err;

  err = _gpgme_engine_op_conf_dir (ctx->engine, what, result);
  ctx->protocol = proto;
  return err;
}
