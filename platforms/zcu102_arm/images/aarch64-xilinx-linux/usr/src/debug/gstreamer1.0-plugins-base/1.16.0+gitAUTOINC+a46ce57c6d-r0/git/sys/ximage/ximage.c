/* GStreamer
 * Copyright (C) <2003> Julien Moutte <julien@moutte.net>
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ximagesink.h"

GST_DEBUG_CATEGORY (gst_debug_x_image_pool);
GST_DEBUG_CATEGORY (gst_debug_x_image_sink);
GST_DEBUG_CATEGORY (CAT_PERFORMANCE);

static gboolean
plugin_init (GstPlugin * plugin)
{
  if (!gst_element_register (plugin, "ximagesink",
          GST_RANK_SECONDARY, GST_TYPE_X_IMAGE_SINK))
    return FALSE;

  GST_DEBUG_CATEGORY_INIT (gst_debug_x_image_sink, "ximagesink", 0,
      "ximagesink element");
  GST_DEBUG_CATEGORY_INIT (gst_debug_x_image_pool, "ximagepool", 0,
      "ximagepool object");

  GST_DEBUG_CATEGORY_GET (CAT_PERFORMANCE, "GST_PERFORMANCE");

  return TRUE;
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    ximagesink,
    "X11 video output element based on standard Xlib calls",
    plugin_init, VERSION, GST_LICENSE, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)
