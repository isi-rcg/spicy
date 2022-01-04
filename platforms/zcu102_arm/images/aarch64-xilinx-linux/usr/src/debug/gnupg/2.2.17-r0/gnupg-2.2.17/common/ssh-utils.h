/* ssh-utils.c - Secure Shell helper function definitions
 * Copyright (C) 2011 Free Software Foundation, Inc.
 *
 * This file is part of GnuPG.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either
 *
 *   - the GNU Lesser General Public License as published by the Free
 *     Software Foundation; either version 3 of the License, or (at
 *     your option) any later version.
 *
 * or
 *
 *   - the GNU General Public License as published by the Free
 *     Software Foundation; either version 2 of the License, or (at
 *     your option) any later version.
 *
 * or both in parallel, as here.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <https://www.gnu.org/licenses/>.
 */

#ifndef GNUPG_COMMON_SSH_UTILS_H
#define GNUPG_COMMON_SSH_UTILS_H


gpg_error_t ssh_get_fingerprint (gcry_sexp_t key, int algo,
				 void **r_fpr, size_t *r_len);

gpg_error_t ssh_get_fingerprint_string (gcry_sexp_t key, int algo,
					char **r_fprstr);


#endif /*GNUPG_COMMON_SSH_UTILS_H*/
