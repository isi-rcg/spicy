/* GStreamer
 * Copyright (C) <2007> Wim Taymans <wim.taymans@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef __GST_PLAY_BACK_H__
#define __GST_PLAY_BACK_H__


#include <gst/gst.h>

gboolean gst_decode_bin_plugin_init (GstPlugin * plugin);
gboolean gst_decodebin3_plugin_init (GstPlugin * plugin);
gboolean gst_uri_decode_bin_plugin_init (GstPlugin * plugin);
gboolean gst_uri_decode_bin3_plugin_init (GstPlugin * plugin);
gboolean gst_uri_source_bin_plugin_init (GstPlugin * plugin);
gboolean gst_parse_bin_plugin_init (GstPlugin * plugin);

gboolean gst_play_bin_plugin_init (GstPlugin * plugin);
gboolean gst_play_bin2_plugin_init (GstPlugin * plugin);
gboolean gst_play_bin3_plugin_init (GstPlugin * plugin, gboolean as_playbin);


#endif /* __GST_PLAY_BACK_H__ */
