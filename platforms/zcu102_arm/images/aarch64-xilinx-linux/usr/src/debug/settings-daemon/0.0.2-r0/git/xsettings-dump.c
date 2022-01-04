#include <config.h>

#include <glib.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>

#include "xsettings-client.h"

static GMainLoop *loop;

static void
settings_notify (const char *name,
                 XSettingsAction action, XSettingsSetting *setting,
                 void *cb_data)
{
  switch (action) {
  case XSETTINGS_ACTION_NEW:
    g_print ("Setting '%s' added\n", name);
    break;
  case XSETTINGS_ACTION_CHANGED:
    g_print ("Setting '%s' changed\n", name);
    break;
  case XSETTINGS_ACTION_DELETED:
    g_print ("Setting '%s' deleted\n", name);
    break;
  }

  if (action == XSETTINGS_ACTION_NEW || action == XSETTINGS_ACTION_CHANGED) {
    switch (setting->type) {
    case XSETTINGS_TYPE_INT:
      g_print ("Integer: %d\n\n", setting->data.v_int);
      break;
    case XSETTINGS_TYPE_STRING:
      g_print ("String: '%s'\n\n", setting->data.v_string);
      break;
    case XSETTINGS_TYPE_COLOR:
      g_print ("Colour: %d/%d/%d\n\n",
               setting->data.v_color.red,
               setting->data.v_color.green,
               setting->data.v_color.blue);
      break;
    }
  }
}

int
main(int argc, char **argv)
{
  GdkDisplay *display;
  int i, n_screens;

  g_type_init ();

  gdk_init (&argc, &argv);

  loop = g_main_loop_new (NULL, FALSE);

  display = gdk_display_get_default ();
  n_screens = gdk_display_get_n_screens (display);

  for (i = 0; i < n_screens; i++) {
    XSettingsClient *client;

    client = xsettings_client_new (gdk_x11_display_get_xdisplay (display), i, settings_notify, NULL, NULL);
  }

  g_main_loop_run (loop);

  return 0;
}
