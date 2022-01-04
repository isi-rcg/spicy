/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 *
 * Copyright (C) 2013-2015 Richard Hughes <richard@hughsie.com>
 *
 * Licensed under the GNU Lesser General Public License Version 2.1
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
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef __DNF_KEYRING_H
#define __DNF_KEYRING_H

#include <glib.h>

#include <rpm/rpmkeyring.h>

G_BEGIN_DECLS

gboolean         dnf_keyring_add_public_key     (rpmKeyring              keyring,
                                                 const gchar            *filename,
                                                 GError                 **error);
gboolean         dnf_keyring_add_public_keys    (rpmKeyring              keyring,
                                                 GError                 **error);
gboolean         dnf_keyring_check_untrusted_file (rpmKeyring            keyring,
                                                 const gchar            *filename,
                                                 GError                 **error);

G_END_DECLS

#endif /* __DNF_KEYRING_H */
