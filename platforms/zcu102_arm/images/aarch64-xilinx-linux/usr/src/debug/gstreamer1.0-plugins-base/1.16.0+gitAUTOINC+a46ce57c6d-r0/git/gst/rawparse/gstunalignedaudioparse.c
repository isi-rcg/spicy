/* GStreamer
 * Copyright (C) 2016 Carlos Rafael Giani <dv@pseudoterminal.org>
 *
 * gstunalignedaudioparse.c:
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

#include <string.h>
#include <stdio.h>
#include <gst/gst.h>
#include <gst/audio/audio.h>
#include "gstunalignedaudioparse.h"
#include "unalignedaudio.h"

GST_DEBUG_CATEGORY (unaligned_audio_parse_debug);
#define GST_CAT_DEFAULT unaligned_audio_parse_debug

struct _GstUnalignedAudioParse
{
  GstBin parent;
  GstElement *inner_parser;
};

struct _GstUnalignedAudioParseClass
{
  GstBinClass parent_class;
};

static GstStaticPadTemplate static_sink_template =
GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (GST_UNALIGNED_RAW_AUDIO_CAPS)
    );

static GstStaticPadTemplate static_src_template =
GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (GST_AUDIO_CAPS_MAKE (GST_AUDIO_FORMATS_ALL)
        ", layout = (string) { interleaved, non-interleaved }")
    );

G_DEFINE_TYPE (GstUnalignedAudioParse, gst_unaligned_audio_parse, GST_TYPE_BIN);

static void
gst_unaligned_audio_parse_class_init (GstUnalignedAudioParseClass * klass)
{
  GstElementClass *element_class;

  GST_DEBUG_CATEGORY_INIT (unaligned_audio_parse_debug, "unalignedaudioparse",
      0, "Unaligned raw audio parser");

  element_class = GST_ELEMENT_CLASS (klass);

  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&static_sink_template));
  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&static_src_template));

  gst_element_class_set_static_metadata (element_class,
      "unalignedaudioparse",
      "Codec/Parser/Bin/Audio",
      "Parse unaligned raw audio data",
      "Carlos Rafael Giani <dv@pseudoterminal.org>");
}

static void
gst_unaligned_audio_parse_init (GstUnalignedAudioParse * unaligned_audio_parse)
{
  GstPad *inner_pad;
  GstPad *ghostpad;

  unaligned_audio_parse->inner_parser =
      gst_element_factory_make ("rawaudioparse", "inner_parser");
  g_assert (unaligned_audio_parse->inner_parser != NULL);

  g_object_set (G_OBJECT (unaligned_audio_parse->inner_parser),
      "use-sink-caps", TRUE, NULL);

  gst_bin_add (GST_BIN (unaligned_audio_parse),
      unaligned_audio_parse->inner_parser);

  inner_pad =
      gst_element_get_static_pad (unaligned_audio_parse->inner_parser, "sink");
  ghostpad =
      gst_ghost_pad_new_from_template ("sink", inner_pad,
      gst_element_class_get_pad_template (GST_ELEMENT_GET_CLASS
          (unaligned_audio_parse), "sink"));
  gst_element_add_pad (GST_ELEMENT (unaligned_audio_parse), ghostpad);
  gst_object_unref (GST_OBJECT (inner_pad));

  inner_pad = gst_element_get_static_pad (unaligned_audio_parse->inner_parser,
      "src");
  ghostpad =
      gst_ghost_pad_new_from_template ("src", inner_pad,
      gst_element_class_get_pad_template (GST_ELEMENT_GET_CLASS
          (unaligned_audio_parse), "src"));
  gst_element_add_pad (GST_ELEMENT (unaligned_audio_parse), ghostpad);
  gst_object_unref (GST_OBJECT (inner_pad));
}
