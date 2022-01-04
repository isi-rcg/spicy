/*
 *      main-win.h
 *
 *      Copyright 2009 PCMan <pcman.tw@gmail.com>
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


#ifndef __MAIN_WIN_H__
#define __MAIN_WIN_H__

#include <gtk/gtk.h>
#include <libfm/fm-gtk.h>
#include "tab-page.h"

G_BEGIN_DECLS

#define FM_MAIN_WIN_TYPE                (fm_main_win_get_type())
#define FM_MAIN_WIN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj),\
            FM_MAIN_WIN_TYPE, FmMainWin))
#define FM_MAIN_WIN_CLASS(klass)        (G_TYPE_CHECK_CLASS_CAST((klass),\
            FM_MAIN_WIN_TYPE, FmMainWinClass))
#define IS_FM_MAIN_WIN(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj),\
            FM_MAIN_WIN_TYPE))
#define IS_FM_MAIN_WIN_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),\
            FM_MAIN_WIN_TYPE))

typedef struct _FmMainWin           FmMainWin;
typedef struct _FmMainWinClass      FmMainWinClass;

struct _FmMainWin
{
    GtkWindow parent;
    GtkWindowGroup* win_group;

    GtkUIManager* ui;
    GtkRadioAction* first_view_mode;
    GtkRadioAction* first_side_pane_mode;
    GtkToolbar* toolbar;
    FmPathEntry* location;
    FmPathBar* path_bar;
    GtkNotebook* notebook;
    FmTabPage* current_page;
    FmSidePane* side_pane;
    FmFolderView* folder_view;
    GtkStatusbar* statusbar;
    GtkFrame* vol_status;
    GtkMenuShell* bookmarks_menu;
    GtkWidget* history_menu;
    GtkMenu* popup;
    /* <private> */
    FmNavHistory* nav_history;
    guint statusbar_ctx;
    guint statusbar_ctx2;
    FmBookmarks* bookmarks;
    guint idle_handler; /* fix for GtkEntry bug */
    gboolean fullscreen;
    gboolean maximized;
    gboolean in_update;
    gboolean enable_passive_view;
    gboolean passive_view_on_right;
};

struct _FmMainWinClass
{
    GtkWindowClass parent_class;
};

GType       fm_main_win_get_type        (void);
FmMainWin*  fm_main_win_new             (void);
void fm_main_win_chdir(FmMainWin* win, FmPath* path);
void fm_main_win_chdir_by_name(FmMainWin* win, const char* path_str);
gint fm_main_win_add_tab(FmMainWin* win, FmPath* path);
FmMainWin* fm_main_win_add_win(FmMainWin* win, FmPath* path);

FmMainWin* fm_main_win_get_last_active(void);
void fm_main_win_open_in_last_active(FmPath* path);

G_END_DECLS

#endif /* __MAIN-WIN_H__ */
