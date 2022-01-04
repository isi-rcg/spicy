/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
 *  gtksourceiter.h
 *
 *  Copyright (C) 2000, 2002 Paolo Maggi
 *  Copyright (C) 2002, 2003 Jeroen Zwartepoorte
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef __GTK_SOURCE_ITER_H__
#define __GTK_SOURCE_ITER_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef enum
{
	GTK_SOURCE_SEARCH_VISIBLE_ONLY,
	GTK_SOURCE_SEARCH_TEXT_ONLY,
	GTK_SOURCE_SEARCH_CASE_INSENSITIVE
	/* Possible future plans: SEARCH_REGEXP */
} GtkSourceSearchFlags;

gboolean gtk_source_iter_forward_search (const GtkTextIter   *iter,
						 const gchar         *str,
						 GtkSourceSearchFlags flags,
						 GtkTextIter         *match_start,
						 GtkTextIter         *match_end,
						 const GtkTextIter   *limit);

gboolean gtk_source_iter_backward_search (const GtkTextIter   *iter,
						 const gchar         *str,
						 GtkSourceSearchFlags flags,
						 GtkTextIter         *match_start,
						 GtkTextIter         *match_end,
						 const GtkTextIter   *limit);

gboolean gtk_source_iter_find_matching_bracket (GtkTextIter         *iter);

G_END_DECLS

#endif /* __GTK_SOURCE_ITER_H__ */
