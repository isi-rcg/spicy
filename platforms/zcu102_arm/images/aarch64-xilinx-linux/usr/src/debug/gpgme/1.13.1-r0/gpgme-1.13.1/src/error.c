/* error.c - Error handling for GPGME.
 * Copyright (C) 2003, 2004 g10 Code GmbH
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

#include <gpgme.h>

/* Return a pointer to a string containing a description of the error
   code in the error value ERR.  */
const char *
gpgme_strerror (gpgme_error_t err)
{
  return gpg_strerror (err);
}


/* Return the error string for ERR in the user-supplied buffer BUF of
   size BUFLEN.  This function is, in contrast to gpg_strerror,
   thread-safe if a thread-safe strerror_r() function is provided by
   the system.  If the function succeeds, 0 is returned and BUF
   contains the string describing the error.  If the buffer was not
   large enough, ERANGE is returned and BUF contains as much of the
   beginning of the error string as fits into the buffer.  */
int
gpgme_strerror_r (gpg_error_t err, char *buf, size_t buflen)
{
  return gpg_strerror_r (err, buf, buflen);
}


/* Return a pointer to a string containing a description of the error
   source in the error value ERR.  */
const char *
gpgme_strsource (gpgme_error_t err)
{
  return gpg_strsource (err);
}


/* Retrieve the error code for the system error ERR.  This returns
   GPG_ERR_UNKNOWN_ERRNO if the system error is not mapped (report
   this).  */
gpgme_err_code_t
gpgme_err_code_from_errno (int err)
{
  return gpg_err_code_from_errno (err);
}


/* Retrieve the system error for the error code CODE.  This returns 0
   if CODE is not a system error code.  */
int
gpgme_err_code_to_errno (gpgme_err_code_t code)
{
  return gpg_err_code_to_errno (code);
}


/* Retrieve the error code directly from the ERRNO variable.  This
   returns GPG_ERR_UNKNOWN_ERRNO if the system error is not mapped
   (report this) and GPG_ERR_MISSING_ERRNO if ERRNO has the value 0. */
gpgme_err_code_t
gpgme_err_code_from_syserror (void)
{
  return gpg_err_code_from_syserror ();
}


/* Set the ERRNO variable.  This function is the preferred way to set
   ERRNO due to peculiarities on WindowsCE.  */
void
gpgme_err_set_errno (int err)
{
  gpg_err_set_errno (err);
}


/* Return an error value with the error source SOURCE and the system
   error ERR.  */
gpgme_error_t
gpgme_err_make_from_errno (gpg_err_source_t source, int err)
{
  return gpg_err_make_from_errno (source, err);
}


/* Return an error value with the system error ERR.  */
gpgme_error_t
gpgme_error_from_errno (int err)
{
  return gpgme_error (gpg_err_code_from_errno (err));
}
