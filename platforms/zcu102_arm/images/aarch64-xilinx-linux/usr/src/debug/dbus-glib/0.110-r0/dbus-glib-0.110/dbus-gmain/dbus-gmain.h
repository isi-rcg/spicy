/* -*- mode: C; c-file-style: "gnu" -*- */
/* dbus-gmain.h — GLib main loop integration for libdbus
 *
 * Copyright (C) 2002, 2003  CodeFactory AB
 * Copyright (C) 2003, 2004 Red Hat, Inc.
 *
 * Licensed under the Academic Free License version 2.1
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */
#ifndef DBUS_GMAIN_H
#define DBUS_GMAIN_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <dbus/dbus.h>
#include <glib.h>

#ifndef DBUS_GMAIN_FUNCTION_NAME
# define DBUS_GMAIN_FUNCTION_NAME(name) dbus_gmain_ ## name
#endif

#ifndef DBUS_GMAIN_FUNCTION
# define DBUS_GMAIN_FUNCTION(ret, name, ...) \
  G_GNUC_INTERNAL ret DBUS_GMAIN_FUNCTION_NAME (name) (__VA_ARGS__)
#endif

G_BEGIN_DECLS

DBUS_GMAIN_FUNCTION (void, set_up_connection,
                     DBusConnection *connection,
                     GMainContext *context);
DBUS_GMAIN_FUNCTION (void, set_up_server,
                     DBusServer *server,
                     GMainContext *context);

G_END_DECLS

#endif /* DBUS_GMAIN_H */




