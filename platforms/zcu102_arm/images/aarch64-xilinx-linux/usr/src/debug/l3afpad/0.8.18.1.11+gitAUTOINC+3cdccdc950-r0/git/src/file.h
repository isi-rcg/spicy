/*
 *  L3afpad - GTK+ based simple text editor
 *  Copyright (C) 2004-2005 Tarot Osuji
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _FILE_H
#define _FILE_H

typedef struct {
	gchar *filename;
	gchar *charset;
	gboolean charset_flag; /* T:unique F:listed */
	gchar lineend;
} FileInfo;

gboolean check_file_writable(gchar *filename);
gchar *get_file_basename(gchar *filename, gboolean bracket);
gchar *parse_file_uri(gchar *uri);
gint file_open_real(GtkWidget *view, FileInfo *fi);
gint file_save_real(GtkWidget *view, FileInfo *fi);
gchar *file_stats(GtkWidget *view, FileInfo *fi);

#endif /* _FILE_H */
