/*
 * Copyright (C) 2001,2002,2003,2009,2010 Red Hat, Inc.
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef __VTE_VTE_GLOBALS_H__
#define __VTE_VTE_GLOBALS_H__

#include <glib.h>

#include "vtemacros.h"

G_BEGIN_DECLS

_VTE_PUBLIC
char *vte_get_user_shell(void);

_VTE_PUBLIC
const char *vte_get_features (void);

#define VTE_TEST_FLAGS_NONE (G_GUINT64_CONSTANT(0))
#define VTE_TEST_FLAGS_ALL (~G_GUINT64_CONSTANT(0))

_VTE_PUBLIC
void vte_set_test_flags(guint64 flags);

G_END_DECLS

#endif /* __VTE_VTE_GLOBALS_H__ */
