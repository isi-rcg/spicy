/* -*- mode: C; c-file-style: "gnu" -*- */
/* dbus-glib.c General GLib binding stuff
 *
 * Copyright (C) 2004 Red Hat, Inc.
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

#include <config.h>
#include "dbus/dbus-glib.h"
#include "dbus/dbus-glib-lowlevel.h"
#include "dbus-gmain/dbus-gmain.h"
#include "dbus-gtest.h"
#include "dbus-gutils.h"
#include "dbus-gvalue.h"
#include "dbus-gobject.h"
#include <string.h>

/**
 * SECTION:dbus-gconnection
 * @title: DBusGConnection
 * @short_description: DBus Connection
 * @see_also: #DBusConnection
 * @stability: Stable
 *
 * A #DBusGConnection is a boxed type abstracting a DBusConnection.
 */

/**
 * DBusGConnection:
 *
 * A #DBusGConnection is a boxed type abstracting a DBusConnection from
 * libdbus.
 *
 * Deprecated: New code should use #GDBusConnection from the GIO library,
 *  which is not based on libdbus or dbus-glib.
 */

/**
 * dbus_g_connection_flush:
 * @connection: the #DBusGConnection to flush
 *
 * Blocks until outgoing calls and signal emissions have been sent.
 *
 * Deprecated: The closest equivalent in GDBus is
 *  #g_dbus_connection_flush_sync().
 */
void
dbus_g_connection_flush (DBusGConnection *connection)
{
  dbus_connection_flush (DBUS_CONNECTION_FROM_G_CONNECTION (connection));
}

/**
 * dbus_g_connection_ref:
 * @connection: the #DBusGConnection to ref
 *
 * Increment refcount on a #DBusGConnection
 * 
 * Returns: the connection that was ref'd
 *
 * Deprecated: New code should use #GDBusConnection instead.
 */
DBusGConnection*
dbus_g_connection_ref (DBusGConnection *connection)
{
  DBusConnection *c;

  c = DBUS_CONNECTION_FROM_G_CONNECTION (connection);
  dbus_connection_ref (c);
  return connection;
}


/**
 * dbus_g_connection_unref:
 * @connection: the connection to unref
 * 
 * Decrement refcount on a #DBusGConnection
 *
 * Deprecated: New code should use #GDBusConnection instead.
 */
void
dbus_g_connection_unref (DBusGConnection *connection)
{
  DBusConnection *c;

  c = DBUS_CONNECTION_FROM_G_CONNECTION (connection);
  dbus_connection_unref (c);
}


/**
 * SECTION:dbus-gmessage
 * @title: DBusGMessage
 * @short_description: DBus Message
 * @see_also: #DBusMessage
 * @stability: Stable
 *
 * A #DBusGMessage is a boxed type abstracting a DBusMessage.
 */

/**
 * DBusGMessage:
 *
 * A #DBusGMessage is a boxed type abstracting a DBusMessage from
 * libdbus.
 *
 * Deprecated: New code should use #GDBusMessage instead.
 */

/**
 * dbus_g_message_ref:
 * @message: the message to ref
 *
 * Increment refcount on a #DBusGMessage
 * 
 * Returns: the message that was ref'd
 *
 * Deprecated: New code should use #GDBusMessage instead.
 */
DBusGMessage*
dbus_g_message_ref (DBusGMessage *message)
{
  DBusMessage *c;

  c = DBUS_MESSAGE_FROM_G_MESSAGE (message);
  dbus_message_ref (c);
  return message;
}

/**
 * dbus_g_message_unref:
 * @message: the message to unref
 * 
 * Decrement refcount on a #DBusGMessage
 *
 * Deprecated: New code should use #GDBusMessage instead.
 */
void
dbus_g_message_unref (DBusGMessage *message)
{
  DBusMessage *c;

  c = DBUS_MESSAGE_FROM_G_MESSAGE (message);
  dbus_message_unref (c);
}

/**
 * SECTION:dbus-gerror
 * @title: DBusGError
 * @short_description: DBus GError
 * @see_also: #GError
 * @stability: Stable
 *
 * #DBusGError is the #GError used by DBus.
 */

/**
 * DBusGError:
 *
 * A #GError enumeration for the domain %DBUS_GERROR. The values' meanings
 * can be found by looking at the comments for the corresponding constants
 * in dbus-protocol.h.
 *
 * Deprecated: New code should use GDBus and its #GDBusError enum instead.
 */

/**
 * DBUS_GERROR:
 *
 * Expands to a function call returning the error domain quark for #DBusGError,
 * for use with #GError.
 *
 * Deprecated: New code should use GDBus and its #GDBusError enum instead.
 */
GQuark
dbus_g_error_quark (void)
{
  static GQuark quark = 0;
  if (quark == 0)
    quark = g_quark_from_static_string ("dbus-glib-error-quark");
  return quark;
}

/**
 * dbus_g_error_has_name:
 * @error: the GError given from the remote method
 * @name: the D-BUS error name
 *
 * Determine whether D-BUS error name for a remote exception matches
 * the given name.  This function is intended to be invoked on a
 * #GError returned from an invocation of a remote method, e.g. via
 * dbus_g_proxy_end_call().  It will silently return %FALSE for errors
 * which are not remote D-BUS exceptions (i.e. with a domain other
 * than %DBUS_GERROR or a code other than
 * %DBUS_GERROR_REMOTE_EXCEPTION).
 *
 * Returns: %TRUE if and only if the remote error has the given name
 *
 * Deprecated: New code should use GDBus instead. The closest equivalent
 *  is g_dbus_error_get_remote_error().
 */
gboolean
dbus_g_error_has_name (GError *error, const char *name)
{
  g_return_val_if_fail (error != NULL, FALSE);

  if (error->domain != DBUS_GERROR
      || error->code != DBUS_GERROR_REMOTE_EXCEPTION)
    return FALSE;

  return !strcmp (dbus_g_error_get_name (error), name);
}

/**
 * dbus_g_error_get_name:
 * @error: the #GError given from the remote method
 *
 * This function may only be invoked on a #GError returned from an
 * invocation of a remote method, e.g. via dbus_g_proxy_end_call().
 * Moreover, you must ensure that the error's domain is %DBUS_GERROR,
 * and the code is %DBUS_GERROR_REMOTE_EXCEPTION.
 *
 * Returns: the D-BUS name for a remote exception.
 *
 * Deprecated: New code should use GDBus instead. The closest equivalent
 *  is g_dbus_error_get_remote_error().
 */
const char *
dbus_g_error_get_name (GError *error)
{
  g_return_val_if_fail (error != NULL, NULL);
  g_return_val_if_fail (error->domain == DBUS_GERROR, NULL);
  g_return_val_if_fail (error->code == DBUS_GERROR_REMOTE_EXCEPTION, NULL);

  return error->message + strlen (error->message) + 1;
}

/**
 * DBUS_TYPE_CONNECTION:
 *
 * Expands to a function call returning a boxed #GType representing a
 * #DBusConnection pointer from libdbus. Not to be confused with
 * %DBUS_TYPE_G_CONNECTION, which you should usually use instead.
 *
 * Returns: the GLib type
 *
 * Deprecated: New code should use GDBus instead.
 */
GType
dbus_connection_get_g_type (void)
{
  static GType our_type = 0;
  
  if (our_type == 0)
    our_type = g_boxed_type_register_static ("DBusConnection",
                                             (GBoxedCopyFunc) dbus_connection_ref,
                                             (GBoxedFreeFunc) dbus_connection_unref);

  return our_type;
}

/**
 * DBUS_TYPE_MESSAGE:
 *
 * Expands to a function call returning a boxed #GType representing a
 * #DBusMessage pointer from libdbus. Not to be confused with
 * %DBUS_TYPE_G_MESSAGE, which you should usually use instead.
 *
 *
 * Returns: the GLib type
 *
 * Deprecated: New code should use GDBus instead.
 */
GType
dbus_message_get_g_type (void)
{
  static GType our_type = 0;
  
  if (our_type == 0)
    our_type = g_boxed_type_register_static ("DBusMessage",
                                             (GBoxedCopyFunc) dbus_message_ref,
                                             (GBoxedFreeFunc) dbus_message_unref);

  return our_type;
}

/**
 * DBUS_TYPE_G_CONNECTION:
 *
 * Expands to a function call returning the boxed #GType of a #DBusGConnection.
 *
 * Returns: the GLib type
 *
 * Deprecated: New code should use GDBus instead.
 */
GType
dbus_g_connection_get_g_type (void)
{
  static GType our_type = 0;
  
  if (our_type == 0)
    our_type = g_boxed_type_register_static ("DBusGConnection",
                                             (GBoxedCopyFunc) dbus_g_connection_ref,
                                             (GBoxedFreeFunc) dbus_g_connection_unref);

  return our_type;
}

/**
 * DBUS_TYPE_G_MESSAGE:
 *
 * Expands to a function call returning the boxed #GType of a #DBusGConnection.
 *
 * Returns: the GLib type
 *
 * Deprecated: New code should use GDBus instead.
 */
GType
dbus_g_message_get_g_type (void)
{
  static GType our_type = 0;
  
  if (our_type == 0)
    our_type = g_boxed_type_register_static ("DBusGMessage",
                                             (GBoxedCopyFunc) dbus_g_message_ref,
                                             (GBoxedFreeFunc) dbus_g_message_unref);

  return our_type;
}

/**
 * SECTION:dbus-glib-lowlevel
 * @title: DBus GLib low level
 * @short_description: DBus lower level functions
 * @stability: Unstable
 *
 * These functions can be used to access lower level of DBus.
 */

/**
 * dbus_g_connection_get_connection:
 * @gconnection:  a #DBusGConnection
 *
 * Get the #DBusConnection corresponding to this #DBusGConnection.
 * The return value does not have its refcount incremented.
 *
 * Returns: #DBusConnection 
 *
 * Deprecated: New code should use GDBus instead.
 */
DBusConnection*
dbus_g_connection_get_connection (DBusGConnection *gconnection)
{
  g_return_val_if_fail (gconnection, NULL);
  return DBUS_CONNECTION_FROM_G_CONNECTION (gconnection);
}

extern dbus_int32_t _dbus_gmain_connection_slot;

/**
 * dbus_connection_get_g_connection:
 * @connection:  a #DBusConnection
 *
 * Get the #DBusGConnection corresponding to this #DBusConnection.  This only
 * makes sense if the #DBusConnection was originally a #DBusGConnection that was
 * registered with the GLib main loop.  The return value does not have its
 * refcount incremented.
 *
 * Returns: #DBusGConnection 
 *
 * Deprecated: New code should use GDBus instead.
 */
DBusGConnection*
dbus_connection_get_g_connection (DBusConnection *connection)
{
  g_return_val_if_fail (connection, NULL);
  g_return_val_if_fail (dbus_connection_get_data (connection, _dbus_gmain_connection_slot), NULL);
  
  return DBUS_G_CONNECTION_FROM_CONNECTION (connection);
}


/**
 * dbus_g_message_get_message:
 * @gmessage: a #DBusGMessage
 *
 * Get the #DBusMessage corresponding to this #DBusGMessage.
 * The return value does not have its refcount incremented.
 *
 * Returns: #DBusMessage 
 *
 * Deprecated: New code should use GDBus instead.
 */
DBusMessage*
dbus_g_message_get_message (DBusGMessage *gmessage)
{
  return DBUS_MESSAGE_FROM_G_MESSAGE (gmessage);
}

/**
 * dbus_g_connection_open:
 * @address: address of the connection to open
 * @error: address where an error can be returned.
 *
 * Returns a connection to the given address.
 *
 * (Internally, calls dbus_connection_open() then calls
 * dbus_connection_setup_with_g_main() on the result.)
 *
 * Returns: a DBusConnection
 *
 * Deprecated: New code should use GDBus instead. The closest equivalent
 *  is g_dbus_connection_new_for_address_sync().
 */
DBusGConnection*
dbus_g_connection_open (const gchar  *address,
                        GError      **error)
{
  DBusConnection *connection;
  DBusError derror;

  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  _dbus_g_value_types_init ();

  dbus_error_init (&derror);

  connection = dbus_connection_open (address, &derror);
  if (connection == NULL)
    {
      dbus_set_g_error (error, &derror);
      dbus_error_free (&derror);
      return NULL;
    }

  /* does nothing if it's already been done */
  dbus_connection_setup_with_g_main (connection, NULL);

  return DBUS_G_CONNECTION_FROM_CONNECTION (connection);
}

/**
 * dbus_g_connection_open_private:
 * @address: address of the connection to open
 * @context: the #GMainContext or %NULL for default context
 * @error: address where an error can be returned.
 *
 * Returns a private connection to the given address; this
 * connection does not talk to a bus daemon and thus the caller
 * must set up any authentication by itself.  If the address
 * refers to a message bus, the caller must call dbus_bus_register().
 *
 * (Internally, calls dbus_connection_open_private() then calls
 * dbus_connection_setup_with_g_main() on the result.)
 *
 * Returns: (transfer full): a #DBusGConnection
 *
 * Deprecated: New code should use GDBus instead. The closest equivalent
 *  is g_dbus_connection_new_for_address_sync().
 */
DBusGConnection *
dbus_g_connection_open_private (const gchar  *address,
                                GMainContext *context,
                                GError      **error)
{
  DBusConnection *connection;
  DBusError derror;

  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  _dbus_g_value_types_init ();

  dbus_error_init (&derror);

  connection = dbus_connection_open_private (address, &derror);
  if (connection == NULL)
    {
      dbus_set_g_error (error, &derror);
      dbus_error_free (&derror);
      return NULL;
    }

  dbus_connection_setup_with_g_main (connection, context);

  return DBUS_G_CONNECTION_FROM_CONNECTION (connection);
}

/**
 * dbus_g_bus_get:
 * @type: bus type
 * @error: address where an error can be returned.
 *
 * Returns a connection to the given bus. The connection is a global variable
 * shared with other callers of this function.
 *
 * (Internally, calls dbus_bus_get() then calls
 * dbus_connection_setup_with_g_main() on the result.)
 *
 * Returns: a DBusConnection
 *
 * Deprecated: New code should use GDBus instead. The closest equivalent
 *  is g_bus_get_sync().
 */
DBusGConnection*
dbus_g_bus_get (DBusBusType     type,
                GError        **error)
{
  DBusConnection *connection;
  DBusError derror;

  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  _dbus_g_value_types_init ();

  dbus_error_init (&derror);

  connection = dbus_bus_get (type, &derror);
  if (connection == NULL)
    {
      dbus_set_g_error (error, &derror);
      dbus_error_free (&derror);
      return NULL;
    }

  /* does nothing if it's already been done */
  dbus_connection_setup_with_g_main (connection, NULL);

  return DBUS_G_CONNECTION_FROM_CONNECTION (connection);
}

/**
 * dbus_g_bus_get_private:
 * @type: bus type
 * @context: Mainloop context to attach to
 * @error: address where an error can be returned.
 *
 * Returns a connection to the given bus. The connection will be a private
 * non-shared connection and should be closed when usage is complete.
 *
 * Internally this function calls dbus_bus_get_private() then calls
 * dbus_connection_setup_with_g_main() on the result; see the documentation
 * of the former function for more information on private connections.
 *
 * Returns: a DBusConnection
 *
 * Deprecated: New code should use GDBus instead. The closest equivalent
 *  is g_bus_get_sync().
 */
DBusGConnection*
dbus_g_bus_get_private (DBusBusType     type,
                        GMainContext   *context,
                        GError        **error)
{
  DBusConnection *connection;
  DBusError derror;

  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  _dbus_g_value_types_init ();

  dbus_error_init (&derror);

  connection = dbus_bus_get_private (type, &derror);
  if (connection == NULL)
    {
      dbus_set_g_error (error, &derror);
      dbus_error_free (&derror);
      return NULL;
    }

  /* does nothing if it's already been done */
  dbus_connection_setup_with_g_main (connection, context);

  return DBUS_G_CONNECTION_FROM_CONNECTION (connection);
}

/**
 * dbus_connection_setup_with_g_main:
 * @connection: the connection
 * @context: the #GMainContext or %NULL for default context
 *
 * Sets the watch and timeout functions of a #DBusConnection
 * to integrate the connection with the GLib main loop.
 * Pass in %NULL for the #GMainContext unless you're
 * doing something specialized.
 *
 * If called twice for the same context, does nothing the second
 * time. If called once with context A and once with context B,
 * context B replaces context A as the context monitoring the
 * connection.
 *
 * Deprecated: New code should use GDBus instead.
 */
void
dbus_connection_setup_with_g_main (DBusConnection  *connection,
                                   GMainContext    *context)
{
  _dbus_g_set_up_connection (connection, context);
}

/**
 * dbus_server_setup_with_g_main:
 * @server: the server
 * @context: the #GMainContext or %NULL for default
 *
 * Sets the watch and timeout functions of a #DBusServer
 * to integrate the server with the GLib main loop.
 * In most cases the context argument should be %NULL.
 *
 * If called twice for the same context, does nothing the second
 * time. If called once with context A and once with context B,
 * context B replaces context A as the context monitoring the
 * connection.
 *
 * Deprecated: New code should use GDBus instead.
 */
void
dbus_server_setup_with_g_main     (DBusServer      *server,
                                   GMainContext    *context)
{
  _dbus_g_set_up_server (server, context);
}
