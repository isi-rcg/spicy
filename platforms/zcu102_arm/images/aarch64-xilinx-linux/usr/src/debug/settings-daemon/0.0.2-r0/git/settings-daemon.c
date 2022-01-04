/*
 * Setting-daemon - A very much stripped down gnome-settings-daemon,
 *                  Mostly basd on code from there, which is contains
 *                  the following licence;
 *
 * Copyright © 2001 Red Hat, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Red Hat not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  Red Hat makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * RED HAT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL RED HAT
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Authors:  Owen Taylor, Havoc Pennington
 */

#include <config.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <glib.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <gconf/gconf-client.h>

#include "xsettings-manager.h"

static GMainLoop *loop;
static GdkDisplay *display;
static GConfClient  *gconf_client;
static XSettingsManager **managers = NULL;

#ifdef HAVE_XFT2
#define FONT_RENDER_DIR "/platform/ui/font_rendering"
#define FONT_ANTIALIASING_KEY FONT_RENDER_DIR "/antialiasing"
#define FONT_HINTING_KEY      FONT_RENDER_DIR "/hinting"
#define FONT_RGBA_ORDER_KEY   FONT_RENDER_DIR "/rgba_order"
#define FONT_DPI_KEY          FONT_RENDER_DIR "/dpi"
#endif /* HAVE_XFT2 */

typedef struct _TranslationEntry TranslationEntry;
typedef void (* TranslationFunc) (const TranslationEntry *trans,
                                  GConfValue       *value);
struct _TranslationEntry
{
  const char *gconf_key;
  const char *xsetting_name;
  GConfValueType gconf_type;
  TranslationFunc translate;
};

static void
translate_bool_int (const TranslationEntry *trans,
		    GConfValue       *value)
{
  int i;

  g_assert (value->type == trans->gconf_type);
  
  for (i = 0; managers[i]; i++)  
    xsettings_manager_set_int (managers [i], trans->xsetting_name,
                               gconf_value_get_bool (value));
}

static void
translate_int_int (const TranslationEntry *trans,
                   GConfValue       *value)
{
  int i;

  g_assert (value->type == trans->gconf_type);

  for (i = 0; managers [i]; i++)  
    xsettings_manager_set_int (managers [i], trans->xsetting_name,
                               gconf_value_get_int (value));
}

static void
translate_string_string (const TranslationEntry *trans,
                         GConfValue       *value)
{
  int i;

  g_assert (value->type == trans->gconf_type);

  for (i = 0; managers [i]; i++)  
    xsettings_manager_set_string (managers [i],
                                  trans->xsetting_name,
                                  gconf_value_get_string (value));
}

static void
translate_string_string_toolbar (const TranslationEntry *trans,
				 GConfValue       *value)
{
  int i;
  const char *tmp;
  
  g_assert (value->type == trans->gconf_type);

  tmp = gconf_value_get_string (value);
  /* Is this still needed?
     if (tmp && strcmp (tmp, "both_horiz") == 0)
     tmp = "both-horiz";
  */

  for (i = 0; managers [i]; i++) 
    xsettings_manager_set_string (managers [i],
                                  trans->xsetting_name,
                                  tmp);
}

/**
 * The top-level GConf paths to catch all changes for.  Keep this in sync with
 * translations, below. The more specific the paths, the better.
 */
static const char* toplevels[] = {
  "/desktop/poky/interface",
  "/desktop/poky/peripherals",
#ifdef HAVE_XFT2
  FONT_RENDER_DIR
#endif
};

/**
 * The list of translations from GConf keys to XSettings keys, with type
 * information and a mapping function.
 */
/* TODO: include the Xft settings here for neatness */
static const TranslationEntry translations [] = {
  /* Hardware settings */
  { "/desktop/poky/peripherals/mouse/double_click", "Net/DoubleClickTime",
    GCONF_VALUE_INT, translate_int_int },
  { "/desktop/poky/peripherals/mouse/drag_threshold", "Net/DndDragThreshold",
    GCONF_VALUE_INT, translate_int_int },
  { "/desktop/poky/peripherals/stylus_mode", "Gtk/StylusMode",
    GCONF_VALUE_BOOL, translate_bool_int },
  /* Appearance */
  { "/desktop/poky/interface/gtk-color-palette", "Gtk/ColorPalette",
    GCONF_VALUE_STRING, translate_string_string },
  { "/desktop/poky/interface/font_name", "Gtk/FontName",
    GCONF_VALUE_STRING, translate_string_string },
  { "/desktop/poky/interface/gtk_key_theme", "Gtk/KeyThemeName",
    GCONF_VALUE_STRING, translate_string_string },
  { "/desktop/poky/interface/toolbar_style", "Gtk/ToolbarStyle",
    GCONF_VALUE_STRING, translate_string_string_toolbar },
  { "/desktop/poky/interface/toolbar_icon_size", "Gtk/ToolbarIconSize",
    GCONF_VALUE_STRING, translate_string_string },
  { "/desktop/poky/interface/can_change_accels", "Gtk/CanChangeAccels",
    GCONF_VALUE_BOOL, translate_bool_int },
  { "/desktop/poky/interface/cursor_blink", "Net/CursorBlink",
    GCONF_VALUE_BOOL,translate_bool_int },
  { "/desktop/poky/interface/cursor_blink_time", "Net/CursorBlinkTime",
    GCONF_VALUE_INT, translate_int_int },
  { "/desktop/poky/interface/theme", "Net/ThemeName",
    GCONF_VALUE_STRING, translate_string_string },
  { "/desktop/poky/interface/matchbox_theme", "MATCHBOX/THEME",
    GCONF_VALUE_STRING, translate_string_string },
  { "/desktop/poky/interface/gtk-im-preedit-style", "Gtk/IMPreeditStyle",
    GCONF_VALUE_STRING, translate_string_string },
  { "/desktop/poky/interface/gtk-im-status-style", "Gtk/IMStatusStyle",
    GCONF_VALUE_STRING, translate_string_string },
  { "/desktop/poky/interface/icon_theme", "Net/IconThemeName",
    GCONF_VALUE_STRING, translate_string_string },
  { "/desktop/poky/interface/file_chooser_backend", "Gtk/FileChooserBackend",
    GCONF_VALUE_STRING, translate_string_string },
  { "/desktop/poky/interface/menus_have_icons", "Gtk/MenuImages",
    GCONF_VALUE_BOOL, translate_bool_int },
  { "/desktop/poky/interface/touchscreen", "Gtk/TouchscreenMode",
    GCONF_VALUE_BOOL, translate_bool_int },
  { "/desktop/poky/interface/gtk-scrolled-window-placement", "Gtk/ScrolledWindowPlacement",
    GCONF_VALUE_STRING, translate_string_string },
  { "/desktop/poky/interface/gtk_color_scheme", "Gtk/ColorScheme",
    GCONF_VALUE_STRING, translate_string_string },
  { "/desktop/gnome/sound/theme_name", "Net/SoundThemeName",
    GCONF_VALUE_STRING,   translate_string_string },
  { "/desktop/gnome/sound/event_sounds", "Net/EnableEventSounds" ,
    GCONF_VALUE_BOOL,     translate_bool_int },
};

static const TranslationEntry*
find_translation_entry (const char *gconf_key)
{
  int i;

  i = 0;
  while (i < G_N_ELEMENTS (translations))
    {
      if (strcmp (translations[i].gconf_key, gconf_key) == 0)
        return &translations[i];

      ++i;
    }

  return NULL;
}

static const gchar* 
type_to_string (GConfValueType type)
{
  switch (type)
    {
    case GCONF_VALUE_INT:
      return "int";
      break;
    case GCONF_VALUE_STRING:
      return "string";
      break;
    case GCONF_VALUE_FLOAT:
      return "float";
      break;
    case GCONF_VALUE_BOOL:
      return "bool";
      break;
    case GCONF_VALUE_SCHEMA:
      return "schema";
      break;
    case GCONF_VALUE_LIST:
      return "list";
      break;
    case GCONF_VALUE_PAIR:
      return "pair";
      break;
    case GCONF_VALUE_INVALID:
      return "*invalid*";
      break;
    default:
      g_assert_not_reached();
      return NULL; /* for warnings */
      break;
    }
}

static void
process_value (const TranslationEntry *trans,
               GConfValue       *val)
{  
  if (val == NULL)
    {
      int i;

      for (i = 0; managers [i]; i++)  
        xsettings_manager_delete_setting (managers [i], trans->xsetting_name);
    }
  else
    {
      if (val->type == trans->gconf_type)
        {
          (* trans->translate) (trans, val);
        }
      else
        {
          g_warning ("GConf key %s set to type %s but its expected type was %s\n",
                     trans->gconf_key,
                     type_to_string (val->type),
                     type_to_string (trans->gconf_type));
        }
    }
}

static gboolean
xsettings_callback (GConfEntry *entry)
{
  const TranslationEntry *trans;

  trans = find_translation_entry (entry->key);
  if (trans == NULL)
    return FALSE;

  process_value (trans, entry->value);

  return TRUE;
}

#ifdef HAVE_XFT2
typedef struct
{
  gboolean antialias;
  gboolean hinting;
  int dpi;
  const char *rgba;
  const char *hintstyle;
} SdXftSettings;

static const char rgba_types[][5] = { "rgb", "bgr", "vbgr", "vrgb" };

static int
get_display_dpi (void)
{
  GdkScreen* screen;
  
  g_return_val_if_fail (display != NULL, 96);
  
  screen = gdk_display_get_default_screen  (display);
  
  return (gdk_screen_get_width (screen) * 254 + gdk_screen_get_width_mm (screen)*5)
    / (gdk_screen_get_width_mm (screen)*10);
}

/**
 * Read GConf settings and determine the appropriate Xft settings based on them
 * This probably could be done a bit more cleanly with gconf_string_to_enum
 */
static void
sd_xft_settings_get (GConfClient      *client,
			SdXftSettings *settings)
{
  char *antialiasing = gconf_client_get_string (client, FONT_ANTIALIASING_KEY, NULL);
  char *hinting = gconf_client_get_string (client, FONT_HINTING_KEY, NULL);
  char *rgba_order = gconf_client_get_string (client, FONT_RGBA_ORDER_KEY, NULL);
  double dpi = gconf_client_get_float (client, FONT_DPI_KEY, NULL);

  settings->antialias = TRUE;
  settings->hinting = TRUE;
  settings->hintstyle = "hintfull";
  settings->dpi = get_display_dpi () * 1024;
  settings->rgba = "rgb";


  if ((int)(1024 * dpi + 0.5) > 0)
    settings->dpi = (int)(1024 * dpi + 0.5);

  if (rgba_order)
    {
      int i;
      gboolean found = FALSE;

      for (i = 0; i < G_N_ELEMENTS (rgba_types) && !found; i++)
	if (strcmp (rgba_order, rgba_types[i]) == 0)
	  {
	    settings->rgba = rgba_types[i];
	    found = TRUE;
	  }

      if (!found)
	g_warning ("Invalid value for " FONT_RGBA_ORDER_KEY ": '%s'",
		   rgba_order);
    }
  
  if (hinting)
    {
      if (strcmp (hinting, "none") == 0)
	{
	  settings->hinting = 0;
	  settings->hintstyle = "hintnone";
	}
      else if (strcmp (hinting, "slight") == 0)
	{
	  settings->hinting = 1;
	  settings->hintstyle = "hintslight";
	}
      else if (strcmp (hinting, "medium") == 0)
	{
	  settings->hinting = 1;
	  settings->hintstyle = "hintmedium";
	}
      else if (strcmp (hinting, "full") == 0)
	{
	  settings->hinting = 1;
	  settings->hintstyle = "hintfull";
	}
      else
	g_warning ("Invalid value for " FONT_HINTING_KEY ": '%s'",
		   hinting);
    }
  
  if (antialiasing)
    {
      gboolean use_rgba = FALSE;
      
      if (strcmp (antialiasing, "none") == 0)
	settings->antialias = 0;
      else if (strcmp (antialiasing, "grayscale") == 0)
	settings->antialias = 1;
      else if (strcmp (antialiasing, "greyscale") == 0)
	settings->antialias = 1;
      else if (strcmp (antialiasing, "rgba") == 0)
	{
	  settings->antialias = 1;
	  use_rgba = TRUE;
	}
      else
	g_warning ("Invalid value for " FONT_ANTIALIASING_KEY " : '%s'",
		   antialiasing);

      if (!use_rgba)
	settings->rgba = "none";
    }

  g_free (rgba_order);
  g_free (hinting);
  g_free (antialiasing);
}

/**
 * Apply the given Xft settings via X Settings.
 */
static void
sd_xft_settings_set_xsettings (SdXftSettings *settings)
{
  int i;
  g_return_if_fail (settings != NULL);
  
  for (i = 0; managers [i]; i++)  
    {
      xsettings_manager_set_int (managers [i], "Xft/Antialias", settings->antialias);
      xsettings_manager_set_int (managers [i], "Xft/Hinting", settings->hinting);
      xsettings_manager_set_string (managers [i], "Xft/HintStyle", settings->hintstyle);
      xsettings_manager_set_int (managers [i], "Xft/DPI", settings->dpi);
      xsettings_manager_set_string (managers [i], "Xft/RGBA", settings->rgba);
    }
}

#if WITH_XRDB
static gboolean
write_all (int         fd,
	   const char *buf,
	   gsize       to_write)
{
  while (to_write > 0)
    {
      gssize count = write (fd, buf, to_write);
      if (count < 0)
	{
	  if (errno != EINTR)
	    return FALSE;
	}
      else
	{
	  to_write -= count;
	  buf += count;
	}
    }
  
  return TRUE;
}

static gboolean
wait_for_child (int  pid,
		int *status)
{
  gint ret;

 again:
  ret = waitpid (pid, status, 0);

  if (ret < 0)
    {
      if (errno == EINTR)
	goto again;
      else
	{
	  g_warning ("Unexpected error in waitpid() (%s)",
		     g_strerror (errno));
	  return FALSE;
	}
    }

  return TRUE;
}

/**
 * Utility function to call a command and pipe text to it.
 */
static void
sd_spawn_with_input (char      **argv,
		     const char *input)
{
  int exit_status;
  int child_pid;
  int inpipe;
  GError *err = NULL;
  
  if (!g_spawn_async_with_pipes (NULL /* working directory */, 
				 argv, 
				 NULL /* envp */,
				 G_SPAWN_SEARCH_PATH|G_SPAWN_DO_NOT_REAP_CHILD,
				 NULL, NULL, /* child setup and data */
				 &child_pid,
				 &inpipe, NULL, NULL, /* stdin,stdout,stderr */
				 &err))
    {
      gchar *command = g_strjoinv (" ", argv);
      g_warning ("Could not execute %s: %s", command, err->message);
      g_error_free (err);
      g_free (command);
      
      return;
    }

  if (input)
   {
     if (!write_all (inpipe, input, strlen (input)))
       {
	 gchar *command = g_strjoinv (" ", argv);
	 g_warning ("Could not write input to %s", command);
	 g_free (command);
       }
     
     close (inpipe);
   }
  
  wait_for_child (child_pid, &exit_status);
  
  if (!WIFEXITED (exit_status) || WEXITSTATUS (exit_status))
    {
      gchar *command = g_strjoinv (" ", argv);
      g_warning ("Command %s failed", command);
      g_free (command);
    }
}

/**
 * Apply the given Xft settings via X Resouces.
 */
static void
sd_xft_settings_set_xresources (SdXftSettings *settings)
{
  const char *add[] = { "xrdb", "-merge", "-nocpp", NULL };
  GString *add_string;
  
  g_return_if_fail (settings != NULL);
  
  add_string = g_string_new (NULL);

  /* TODO: output locale-independent floats */
  g_string_append_printf (add_string,
			  "Xft.dpi: %f\n", settings->dpi / 1024.);
  g_string_append_printf (add_string,
			  "Xft.antialias: %d\n", settings->antialias);
  g_string_append_printf (add_string,
			  "Xft.hinting: %d\n", settings->hinting);
  g_string_append_printf (add_string,
			  "Xft.hintstyle: %s\n", settings->hintstyle);
  g_string_append_printf (add_string,
			  "Xft.rgba: %s\n", settings->rgba);

  sd_spawn_with_input ((char**)add, add_string->str);
  
  g_string_free (add_string, TRUE);
}
#endif

/**
 * We mirror the Xft properties both through XSETTINGS and through X resources.
 */
static void
sd_settings_update_xft (void)
{
  SdXftSettings settings;
  
  sd_xft_settings_get (gconf_client, &settings);

  sd_xft_settings_set_xsettings (&settings);
#if WITH_XRDB
  sd_xft_settings_set_xresources (&settings);
#endif
}

/**
 * Called by the GConf notification to try and handle Xft keys.
 */
static gboolean
xft_callback (GConfEntry *entry)
{
  if (g_str_has_prefix (entry->key, FONT_RENDER_DIR)) {
    sd_settings_update_xft ();
    return TRUE;
  } else {
    return FALSE;
  }
}

#endif /* HAVE_XFT2 */

static void
sd_settings_xsettings_load (void)
{
  int i;

  i = 0;
  while (i < G_N_ELEMENTS (translations))
    {
      GConfValue *val;
      GError *err;

      err = NULL;
      val = gconf_client_get (gconf_client,
                              translations[i].gconf_key,
                              &err);

      if (err != NULL)
        {
          g_warning ("Error getting value for %s: %s",
                     translations[i].gconf_key, err->message);
          g_error_free (err);
        }
      else
        {
          process_value (&translations[i], val);
          if (val != NULL)
            gconf_value_free (val);
        }
      
      ++i;
    }

#ifdef HAVE_XFT2  
  sd_settings_update_xft ();
#endif /* HAVE_XFT */

  for (i = 0; managers [i]; i++)  
    xsettings_manager_notify (managers [i]);
}

static void
terminate_cb (void *data)
{
  gboolean *terminated = data;

  if (*terminated)
    return;
  
  *terminated = TRUE;

  g_main_loop_quit (loop);
}


static GdkFilterReturn
manager_event_filter (GdkXEvent *xevent,
                      GdkEvent  *event,
                      gpointer   data)
{
  int screen_num = GPOINTER_TO_INT (data);

  g_return_val_if_fail (managers != NULL, GDK_FILTER_CONTINUE);
  
  if (xsettings_manager_process_event (managers [screen_num], 
				       (XEvent *)xevent))
    return GDK_FILTER_REMOVE;
  else
    return GDK_FILTER_CONTINUE;
}

void
gconf_key_changed_callback (GConfClient *client,
			    guint        cnxn_id,
			    GConfEntry  *entry,
			    gpointer    user_data)
{
  gboolean found = FALSE;
  int i;

  found = xsettings_callback (entry);
  
#ifdef HAVE_XFT2
  if (!found)
    found = xft_callback(entry);
#endif

  if (found) {
    for (i = 0; managers [i]; i++)  
      {
        xsettings_manager_notify (managers [i]);
      }
  }
}


int
main(int argc, char **argv)
{
  GdkScreen    *screen;
  gboolean      terminated = FALSE;
  int           i, n_screens;
  pid_t         p;

  g_type_init();

  gdk_init(&argc, &argv);

  loop = g_main_loop_new(NULL, FALSE);

  display   = gdk_display_get_default ();
  n_screens = gdk_display_get_n_screens (display);
  
  managers = g_new (XSettingsManager *, n_screens + 1);

  for (i = 0; i < n_screens; i++)
    {
      screen = gdk_display_get_screen (display, i);
      
      if (xsettings_manager_check_running (gdk_x11_display_get_xdisplay (display), i))
	{
	  g_printerr ("You can only run one xsettings manager at a time; exiting\n");
	  exit (1);
	}
      
      managers [i] = xsettings_manager_new (gdk_x11_display_get_xdisplay (display),
					    i, terminate_cb, &terminated);
      if (!managers [i])
	{
	  g_error ("Could not create xsettings manager for screen %d!\n", i);
	}

      gdk_window_add_filter (gdk_screen_get_root_window (screen),
			     manager_event_filter, GINT_TO_POINTER (i));

    }

  managers [i] = NULL;

  gconf_client  = gconf_client_get_default();

   if (gconf_client != NULL)
     {
       for (i = 0; i < G_N_ELEMENTS (toplevels); i++) {
         gconf_client_add_dir(gconf_client,
                              toplevels[i],
                              GCONF_CLIENT_PRELOAD_RECURSIVE,
                              NULL);
         
         gconf_client_notify_add(gconf_client, toplevels[i],
                                 gconf_key_changed_callback,
                                 NULL, NULL, NULL);
       }
       
       sd_settings_xsettings_load ();

     }
   else g_error ("Failed to initialise gconf client");


  if (!(argc > 1 && !strcmp(argv[1],"-n")))
    {
      
      g_print("[settings daemon] Forking. run with -n to prevent fork\n");
      
      p = fork();
      
      switch (p) 
	{
	case -1:
	  g_error("Fork failed.\n");
	  break;
	case 0:
	  /* child */
	  /*
	  fclose( stdin );
	  fclose( stdout );
	  */
	  break;
	default:
	  exit(0);
	  break;
	}
    }


   g_main_loop_run(loop);

   g_object_unref (gconf_client);

   return 0;
}
