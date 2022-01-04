/* GStreamer
 * Copyright (C) 2016 Carlos Rafael Giani <dv@pseudoterminal.org>
 *
 * gstunalignedaudioparse.h:
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

#ifndef __GST_UNALIGNED_AUDIO_PARSE_H___
#define __GST_UNALIGNED_AUDIO_PARSE_H___

#include <gst/gst.h>

G_BEGIN_DECLS

#define GST_TYPE_UNALIGNED_AUDIO_PARSE \
  (gst_unaligned_audio_parse_get_type())
#define GST_UNALIGNED_AUDIO_PARSE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj), GST_TYPE_UNALIGNED_AUDIO_PARSE, GstUnalignedAudioParse))
#define GST_UNALIGNED_AUDIO_PARSE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass), GST_TYPE_UNALIGNED_AUDIO_PARSE, GstUnalignedAudioParseClass))
#define GST_UNALIGNED_AUDIO_PARSE_CAST(obj) \
  ((GstUnalignedAudioParse *)(obj))
#define GST_IS_UNALIGNED_AUDIO_PARSE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj), GST_TYPE_UNALIGNED_AUDIO_PARSE))
#define GST_IS_UNALIGNED_AUDIO_PARSE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass), GST_TYPE_UNALIGNED_AUDIO_PARSE))

typedef struct _GstUnalignedAudioParse GstUnalignedAudioParse;
typedef struct _GstUnalignedAudioParseClass GstUnalignedAudioParseClass;

GType gst_unaligned_audio_parse_get_type (void);

G_END_DECLS

#endif /* __GST_UNALIGNED_AUDIO_PARSE_H___ */
