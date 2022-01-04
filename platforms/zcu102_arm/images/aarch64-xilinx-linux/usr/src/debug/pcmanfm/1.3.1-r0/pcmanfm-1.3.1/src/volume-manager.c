/*
 *      volume-manager.c
 *
 *      Copyright 2010 Hong Jen Yee (PCMan) <pcman.tw@gmail.com>
 *      Copyright 2013 Andriy Grytsenko (LStranger) <andrej@rep.kiev.ua>
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "volume-manager.h"
#include <libfm/fm-gtk.h>
#include <glib/gi18n.h>
#include <gio/gdesktopappinfo.h>
#include "pcmanfm.h"
#include "main-win.h"
#include "app-config.h"

static GVolumeMonitor* vol_mon = NULL;

static guint on_idle_handler = 0;

typedef struct _AutoRun AutoRun;
struct _AutoRun
{
    GtkDialog* dlg;
    GtkTreeView* view;
    GtkLabel* type;
    GtkToggleButton *dont_ask;
    GtkListStore* store;
    GMount* mount;
    GCancellable* cancel;
    char *content_type;
    gboolean skipped;
};

static void on_dlg_response(GtkDialog* dlg, int res, gpointer user_data);

static void on_unmount(GMount* mount, AutoRun* data)
{
    g_debug("on umount");
    on_dlg_response(data->dlg, GTK_RESPONSE_CLOSE, data);
}

static void on_row_activated(GtkTreeView* view, GtkTreePath* tp, GtkTreeViewColumn* col, gpointer user_data)
{
    AutoRun* data = (AutoRun*)user_data;
    on_dlg_response(data->dlg, GTK_RESPONSE_OK, data);
}

/* will unref app */
static void _run_app(GAppInfo *app, GMount *mount)
{
    GFile* gf = g_mount_get_root(mount);

    if(app)
    {
        GList* filelist = g_list_prepend(NULL, gf);

        g_app_info_launch(app, filelist, NULL, NULL);
        g_list_free(filelist);
        g_object_unref(app);
    }
    else
    {
        FmPath* path = fm_path_new_for_gfile(gf);

        if (app_config->media_in_new_tab)
            fm_main_win_open_in_last_active(path);
        else
            fm_main_win_add_win(NULL, path);
        fm_path_unref(path);
    }
    g_object_unref(gf);
}

static void on_dlg_response(GtkDialog* dlg, int res, gpointer user_data)
{
    AutoRun* data = (AutoRun*)user_data;
    gboolean dont_ask = FALSE;

    /* stop the detection */
    g_cancellable_cancel(data->cancel);

    if (data->dont_ask)
        dont_ask = gtk_toggle_button_get_active(data->dont_ask);
    if (data->skipped)
        ; /* do nothing */
    else if(res == GTK_RESPONSE_OK)
    {
        GtkTreeModel* model;
        GtkTreeSelection* sel = gtk_tree_view_get_selection(data->view);
        GtkTreeIter it;
        if( gtk_tree_selection_get_selected(sel, &model, &it) )
        {
            GAppInfo* app;

            gtk_tree_model_get(model, &it, 2, &app, -1);
            fm_app_config_set_autorun_choice(app_config, data->content_type,
                                             app ? g_app_info_get_id(app) : "pcmanfm",
                                             dont_ask);
            _run_app(app, data->mount);
            pcmanfm_save_config(FALSE);
        }
    }
    else if (dont_ask) /* user chose don't ask and don't run */
    {
        fm_app_config_set_autorun_choice(app_config, data->content_type, NULL, TRUE);
        pcmanfm_save_config(FALSE);
    }
    g_signal_handlers_disconnect_by_func(dlg, on_dlg_response, data);
    g_signal_handlers_disconnect_by_func(data->view, on_row_activated, data);
    g_signal_handlers_disconnect_by_func(data->mount, on_unmount, data);
    gtk_widget_destroy(GTK_WIDGET(dlg));

    g_object_unref(data->cancel);
    g_object_unref(data->store);
    g_object_unref(data->mount);
    g_free(data->content_type);
    g_slice_free(AutoRun, data);

    pcmanfm_unref();
}

static void on_content_type_finished(GObject* src_obj, GAsyncResult* res, gpointer user_data)
{
    AutoRun* data = (AutoRun*)user_data;
    GMount* mount = G_MOUNT(src_obj);
    char** types;
    char* desc = NULL;
    const char *def_type = NULL;
    GAppInfo *app;
    gboolean mixed_content = FALSE;

    types = g_mount_guess_content_type_finish(mount, res, NULL);
    if(types)
    {
        GtkTreeIter it;
        GList* apps = NULL, *l;
        char** type;
        int pos = 0;
        if(types[0])
        {
            FmAutorunChoice *choice;
            choice = g_hash_table_lookup(app_config->autorun_choices, types[0]);
            data->content_type = g_strdup(types[0]);
            if (choice)
            {
                def_type = choice->last_used;
                if (choice->dont_ask)
                {
                    /* skip the dialog and run the app */
                    if (def_type)
                    {
                        if (strcmp(def_type, "pcmanfm") == 0)
                            _run_app(NULL, data->mount);
                        else if ((app = G_APP_INFO(g_desktop_app_info_new(def_type))))
                            _run_app(app, data->mount);
                        else /* chosen application doesn't exist anymore */
                            goto _do_types;
                    }
                    /* else user has chosen don't run anything */
                    g_strfreev(types);
                    data->skipped = TRUE;
                    on_dlg_response(data->dlg, GTK_RESPONSE_CLOSE, data);
                    return;
                }
            }
_do_types:
            for(type=types;*type;++type)
            {
                l = g_app_info_get_all_for_type(*type);
                if(l)
                    apps = g_list_concat(apps, l);
            }
            if (types[1])
                mixed_content = TRUE;
            else
                desc = g_content_type_get_description(types[0]);
        }
        g_strfreev(types);

        if (def_type)
        {
            if (strcmp(def_type, "pcmanfm") == 0)
                pos++; /* leave internal handler in row 0 */
            else if ((app = G_APP_INFO(g_desktop_app_info_new(def_type))))
            {
                gtk_list_store_insert_with_values(data->store, &it, pos++,
                                                      0, g_app_info_get_icon(app),
                                                      1, g_app_info_get_name(app),
                                                      2, app, -1);
                g_object_unref(app);
            }
        }

        if(apps)
        {
            GtkTreePath* tp;
            for(l = apps; l; l=l->next, ++pos)
            {
                app = G_APP_INFO(l->data);
                if (g_strcmp0(def_type, g_app_info_get_id(app)) != 0)
                    gtk_list_store_insert_with_values(data->store, &it, pos,
                                   0, g_app_info_get_icon(app),
                                   1, g_app_info_get_name(app),
                                   2, app, -1);
                g_object_unref(app);
            }
            g_list_free(apps);

            gtk_tree_model_get_iter_first(GTK_TREE_MODEL(data->store), &it);
            gtk_tree_selection_select_iter(gtk_tree_view_get_selection(data->view), &it);
            tp = gtk_tree_path_new_first();
            gtk_tree_view_set_cursor(data->view, tp, NULL, FALSE);
            gtk_tree_path_free(tp);
        }
    }

    if (mixed_content)
        gtk_label_set_text(data->type, _("mixed content"));
    else if(desc)
    {
        if (data->dont_ask)
            gtk_widget_show(GTK_WIDGET(data->dont_ask));
        gtk_label_set_text(data->type, desc);
        g_free(desc);
    }
    else
        gtk_label_set_text(data->type, _("removable disk"));

}


inline static void show_autorun_dlg(GVolume* vol, GMount* mount)
{
    GtkBuilder* builder;
    GtkTreeIter it;
    GtkTreeViewColumn* col;
    GtkTreeSelection* tree_sel;
    GtkCellRenderer*render;
    GtkImage* icon;
    GIcon* gicon;
    AutoRun* data;

    data = g_slice_new(AutoRun);
    data->cancel = g_cancellable_new();

    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, PACKAGE_UI_DIR "/autorun.ui", NULL);
    data->dlg = GTK_DIALOG(gtk_builder_get_object(builder, "dlg"));
    data->view = GTK_TREE_VIEW(gtk_builder_get_object(builder, "listview"));
    data->type = GTK_LABEL(gtk_builder_get_object(builder, "type"));
    data->dont_ask = (GtkToggleButton*)gtk_builder_get_object(builder, "dont_ask");
    data->content_type = NULL;
    data->skipped = FALSE;
    icon = GTK_IMAGE(gtk_builder_get_object(builder, "icon"));
    g_object_unref(builder);

    gicon = g_volume_get_icon(vol);
    gtk_image_set_from_gicon(icon, gicon, GTK_ICON_SIZE_DIALOG);
    g_object_unref(gicon);

    gtk_dialog_set_default_response(data->dlg, GTK_RESPONSE_OK);
    gtk_dialog_set_alternative_button_order(data->dlg, GTK_RESPONSE_OK, GTK_RESPONSE_CANCEL, -1);

    tree_sel = gtk_tree_view_get_selection(data->view);
    gtk_tree_selection_set_mode(tree_sel, GTK_SELECTION_BROWSE);

    col = gtk_tree_view_column_new();
    render = gtk_cell_renderer_pixbuf_new();
    gtk_tree_view_column_pack_start(col, render, FALSE);
    gtk_tree_view_column_set_attributes(col, render, "gicon", 0, NULL);

    render = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(col, render, FALSE);
    gtk_tree_view_column_set_attributes(col, render, "text", 1, NULL);

    gtk_tree_view_append_column(data->view, col);

    data->store = gtk_list_store_new(3, G_TYPE_ICON, G_TYPE_STRING, G_TYPE_OBJECT);
    data->mount = (GMount*)g_object_ref(mount);

    gtk_list_store_append(data->store, &it);
    gicon = g_themed_icon_new("system-file-manager");
    gtk_list_store_set(data->store, &it, 0, gicon, 1, _("Open in File Manager"), -1);
    g_object_unref(gicon);

    gtk_tree_view_set_model(data->view, GTK_TREE_MODEL(data->store));

    gtk_tree_model_get_iter_first(GTK_TREE_MODEL(data->store), &it);
    gtk_tree_selection_select_iter(tree_sel, &it);

    g_signal_connect(data->view, "row-activated", G_CALLBACK(on_row_activated), data);
    g_signal_connect(data->dlg, "response", G_CALLBACK(on_dlg_response), data);

    g_signal_connect(data->mount, "unmounted", G_CALLBACK(on_unmount), data);

    gtk_window_set_keep_above(GTK_WINDOW(data->dlg), TRUE);
    gtk_window_present(GTK_WINDOW(data->dlg));

    g_mount_guess_content_type(mount, TRUE, data->cancel, on_content_type_finished, data);

    pcmanfm_ref();
}

inline static gboolean automount_volume(GVolume* vol, gboolean silent)
{
    GMount* mount;

    if(!g_volume_should_automount(vol) || !g_volume_can_mount(vol))
        return FALSE;
    mount = g_volume_get_mount(vol);
    if(!mount) /* not mounted, automount is needed */
    {
        g_debug("try automount");
        if(!fm_mount_volume(NULL, vol, !silent))
            return FALSE;
        if(silent)
            return TRUE;
        mount = g_volume_get_mount(vol);
        g_debug("mount = %p", mount);
    }
    if(mount)
    {
        if(!silent && app_config->autorun) /* show autorun dialog */
            show_autorun_dlg(vol, mount);
        g_object_unref(mount);
    }
    return TRUE;
}

static gboolean on_vol_added_timeout(gpointer vol)
{
    automount_volume(vol, FALSE);
    g_object_unref(vol);
    return FALSE;
}

static void on_vol_added(GVolumeMonitor* vm, GVolume* vol, gpointer user_data)
{
    if(app_config->mount_removable)
        /* bug #3615195: GLib reports unrelated volume after mount sometimes
           if it's mounted before reported so let delay it a tiny bit */
        gdk_threads_add_timeout(100, on_vol_added_timeout, g_object_ref(vol));
    /* TODO: show icons in systray */
}

#ifdef G_ENABLE_DEBUG

static void on_vol_removed(GVolumeMonitor* vm, GVolume* vol, gpointer user_data)
{
    g_debug("vol: %p is removed", vol);
}

static void on_vol_changed(GVolumeMonitor* vm, GVolume* vol, gpointer user_data)
{
    GMount* mount = g_volume_get_mount(vol);
    g_debug("vol: %p is changed, mount = %p", vol, mount);
    if(mount)
        g_object_unref(mount);
}
#endif

static gboolean fm_volume_manager_delay_init(gpointer user_data)
{
    GList* vols, *l;
    vol_mon = g_volume_monitor_get();
    if(G_UNLIKELY(!vol_mon))
        goto _end;

    g_signal_connect(vol_mon, "volume-added", G_CALLBACK(on_vol_added), NULL);

#ifdef G_ENABLE_DEBUG
    g_signal_connect(vol_mon, "volume-removed", G_CALLBACK(on_vol_removed), NULL);
    g_signal_connect(vol_mon, "volume-changed", G_CALLBACK(on_vol_changed), NULL);
#endif

    if(app_config->mount_on_startup)
    {
        /* try to automount all volumes */
        vols = g_volume_monitor_get_volumes(vol_mon);
        for(l=vols;l;l=l->next)
        {
            GVolume* vol = G_VOLUME(l->data);
            if(g_volume_should_automount(vol))
                automount_volume(vol, TRUE);
            g_object_unref(vol);
        }
        g_list_free(vols);
    }
_end:
    on_idle_handler = 0;
    return FALSE;
}

void fm_volume_manager_init()
{
    /* init the volume manager when idle */
    on_idle_handler = gdk_threads_add_idle_full(G_PRIORITY_LOW, fm_volume_manager_delay_init, NULL, NULL);
}

void fm_volume_manager_finalize()
{
    if(G_LIKELY(vol_mon))
    {
        g_signal_handlers_disconnect_by_func(vol_mon, on_vol_added, NULL);

#ifdef G_ENABLE_DEBUG
        g_signal_handlers_disconnect_by_func(vol_mon, on_vol_removed, NULL);
        g_signal_handlers_disconnect_by_func(vol_mon, on_vol_changed, NULL);
#endif
        g_object_unref(vol_mon);
        vol_mon = NULL;
    }
    if(on_idle_handler)
        g_source_remove(on_idle_handler);
    on_idle_handler = 0;
}
