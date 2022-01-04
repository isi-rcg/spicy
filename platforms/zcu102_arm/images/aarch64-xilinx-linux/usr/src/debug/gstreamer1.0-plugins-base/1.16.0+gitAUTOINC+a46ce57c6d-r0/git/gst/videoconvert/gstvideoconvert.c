/* GStreamer
 * Copyright (C) <1999> Erik Walthinsen <omega@cse.ogi.edu>
 * This file:
 * Copyright (C) 2003 Ronald Bultje <rbultje@ronald.bitfreak.net>
 * Copyright (C) 2010 David Schleef <ds@schleef.org>
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

/**
 * SECTION:element-videoconvert
 * @title: videoconvert
 *
 * Convert video frames between a great variety of video formats.
 *
 * ## Example launch line
 * |[
 * gst-launch-1.0 -v videotestsrc ! video/x-raw,format=YUY2 ! videoconvert ! autovideosink
 * ]|
 *  This will output a test video (generated in YUY2 format) in a video
 * window. If the video sink selected does not support YUY2 videoconvert will
 * automatically convert the video to a format understood by the video sink.
 *
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "gstvideoconvert.h"

#include <gst/video/video.h>
#include <gst/video/gstvideometa.h>
#include <gst/video/gstvideopool.h>

#include <string.h>

GST_DEBUG_CATEGORY (videoconvert_debug);
#define GST_CAT_DEFAULT videoconvert_debug
GST_DEBUG_CATEGORY_STATIC (CAT_PERFORMANCE);

GType gst_video_convert_get_type (void);

static GQuark _colorspace_quark;

#define gst_video_convert_parent_class parent_class
G_DEFINE_TYPE (GstVideoConvert, gst_video_convert, GST_TYPE_VIDEO_FILTER);

#define DEFAULT_PROP_DITHER      GST_VIDEO_DITHER_BAYER
#define DEFAULT_PROP_DITHER_QUANTIZATION 1
#define DEFAULT_PROP_CHROMA_RESAMPLER	GST_VIDEO_RESAMPLER_METHOD_LINEAR
#define DEFAULT_PROP_ALPHA_MODE GST_VIDEO_ALPHA_MODE_COPY
#define DEFAULT_PROP_ALPHA_VALUE 1.0
#define DEFAULT_PROP_CHROMA_MODE GST_VIDEO_CHROMA_MODE_FULL
#define DEFAULT_PROP_MATRIX_MODE GST_VIDEO_MATRIX_MODE_FULL
#define DEFAULT_PROP_GAMMA_MODE GST_VIDEO_GAMMA_MODE_NONE
#define DEFAULT_PROP_PRIMARIES_MODE GST_VIDEO_PRIMARIES_MODE_NONE
#define DEFAULT_PROP_N_THREADS 1

enum
{
  PROP_0,
  PROP_DITHER,
  PROP_DITHER_QUANTIZATION,
  PROP_CHROMA_RESAMPLER,
  PROP_ALPHA_MODE,
  PROP_ALPHA_VALUE,
  PROP_CHROMA_MODE,
  PROP_MATRIX_MODE,
  PROP_GAMMA_MODE,
  PROP_PRIMARIES_MODE,
  PROP_N_THREADS
};

#define CSP_VIDEO_CAPS GST_VIDEO_CAPS_MAKE (GST_VIDEO_FORMATS_ALL) ";" \
    GST_VIDEO_CAPS_MAKE_WITH_FEATURES ("ANY", GST_VIDEO_FORMATS_ALL)

static GstStaticPadTemplate gst_video_convert_src_template =
GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (CSP_VIDEO_CAPS)
    );

static GstStaticPadTemplate gst_video_convert_sink_template =
GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (CSP_VIDEO_CAPS)
    );

static void gst_video_convert_set_property (GObject * object,
    guint property_id, const GValue * value, GParamSpec * pspec);
static void gst_video_convert_get_property (GObject * object,
    guint property_id, GValue * value, GParamSpec * pspec);

static gboolean gst_video_convert_set_info (GstVideoFilter * filter,
    GstCaps * incaps, GstVideoInfo * in_info, GstCaps * outcaps,
    GstVideoInfo * out_info);
static GstFlowReturn gst_video_convert_transform_frame (GstVideoFilter * filter,
    GstVideoFrame * in_frame, GstVideoFrame * out_frame);

/* copies the given caps */
static GstCaps *
gst_video_convert_caps_remove_format_info (GstCaps * caps)
{
  GstStructure *st;
  GstCapsFeatures *f;
  gint i, n;
  GstCaps *res;

  res = gst_caps_new_empty ();

  n = gst_caps_get_size (caps);
  for (i = 0; i < n; i++) {
    st = gst_caps_get_structure (caps, i);
    f = gst_caps_get_features (caps, i);

    /* If this is already expressed by the existing caps
     * skip this structure */
    if (i > 0 && gst_caps_is_subset_structure_full (res, st, f))
      continue;

    st = gst_structure_copy (st);
    /* Only remove format info for the cases when we can actually convert */
    if (!gst_caps_features_is_any (f)
        && gst_caps_features_is_equal (f,
            GST_CAPS_FEATURES_MEMORY_SYSTEM_MEMORY))
      gst_structure_remove_fields (st, "format", "colorimetry", "chroma-site",
          NULL);

    gst_caps_append_structure_full (res, st, gst_caps_features_copy (f));
  }

  return res;
}

/*
 * This is an incomplete matrix of in formats and a score for the prefered output
 * format.
 *
 *         out: RGB24   RGB16  ARGB  AYUV  YUV444  YUV422 YUV420 YUV411 YUV410  PAL  GRAY
 *  in
 * RGB24          0      2       1     2     2       3      4      5      6      7    8
 * RGB16          1      0       1     2     2       3      4      5      6      7    8
 * ARGB           2      3       0     1     4       5      6      7      8      9    10
 * AYUV           3      4       1     0     2       5      6      7      8      9    10
 * YUV444         2      4       3     1     0       5      6      7      8      9    10
 * YUV422         3      5       4     2     1       0      6      7      8      9    10
 * YUV420         4      6       5     3     2       1      0      7      8      9    10
 * YUV411         4      6       5     3     2       1      7      0      8      9    10
 * YUV410         6      8       7     5     4       3      2      1      0      9    10
 * PAL            1      3       2     6     4       6      7      8      9      0    10
 * GRAY           1      4       3     2     1       5      6      7      8      9    0
 *
 * PAL or GRAY are never prefered, if we can we would convert to PAL instead
 * of GRAY, though
 * less subsampling is prefered and if any, preferably horizontal
 * We would like to keep the alpha, even if we would need to to colorspace conversion
 * or lose depth.
 */
#define SCORE_FORMAT_CHANGE       1
#define SCORE_DEPTH_CHANGE        1
#define SCORE_ALPHA_CHANGE        1
#define SCORE_CHROMA_W_CHANGE     1
#define SCORE_CHROMA_H_CHANGE     1
#define SCORE_PALETTE_CHANGE      1

#define SCORE_COLORSPACE_LOSS     2     /* RGB <-> YUV */
#define SCORE_DEPTH_LOSS          4     /* change bit depth */
#define SCORE_ALPHA_LOSS          8     /* lose the alpha channel */
#define SCORE_CHROMA_W_LOSS      16     /* vertical subsample */
#define SCORE_CHROMA_H_LOSS      32     /* horizontal subsample */
#define SCORE_PALETTE_LOSS       64     /* convert to palette format */
#define SCORE_COLOR_LOSS        128     /* convert to GRAY */

#define COLORSPACE_MASK (GST_VIDEO_FORMAT_FLAG_YUV | \
                         GST_VIDEO_FORMAT_FLAG_RGB | GST_VIDEO_FORMAT_FLAG_GRAY)
#define ALPHA_MASK      (GST_VIDEO_FORMAT_FLAG_ALPHA)
#define PALETTE_MASK    (GST_VIDEO_FORMAT_FLAG_PALETTE)

/* calculate how much loss a conversion would be */
static void
score_value (GstBaseTransform * base, const GstVideoFormatInfo * in_info,
    const GValue * val, gint * min_loss, const GstVideoFormatInfo ** out_info)
{
  const gchar *fname;
  const GstVideoFormatInfo *t_info;
  GstVideoFormatFlags in_flags, t_flags;
  gint loss;

  fname = g_value_get_string (val);
  t_info = gst_video_format_get_info (gst_video_format_from_string (fname));
  if (!t_info)
    return;

  /* accept input format immediately without loss */
  if (in_info == t_info) {
    *min_loss = 0;
    *out_info = t_info;
    return;
  }

  loss = SCORE_FORMAT_CHANGE;

  in_flags = GST_VIDEO_FORMAT_INFO_FLAGS (in_info);
  in_flags &= ~GST_VIDEO_FORMAT_FLAG_LE;
  in_flags &= ~GST_VIDEO_FORMAT_FLAG_COMPLEX;
  in_flags &= ~GST_VIDEO_FORMAT_FLAG_UNPACK;

  t_flags = GST_VIDEO_FORMAT_INFO_FLAGS (t_info);
  t_flags &= ~GST_VIDEO_FORMAT_FLAG_LE;
  t_flags &= ~GST_VIDEO_FORMAT_FLAG_COMPLEX;
  t_flags &= ~GST_VIDEO_FORMAT_FLAG_UNPACK;

  if ((t_flags & PALETTE_MASK) != (in_flags & PALETTE_MASK)) {
    loss += SCORE_PALETTE_CHANGE;
    if (t_flags & PALETTE_MASK)
      loss += SCORE_PALETTE_LOSS;
  }

  if ((t_flags & COLORSPACE_MASK) != (in_flags & COLORSPACE_MASK)) {
    loss += SCORE_COLORSPACE_LOSS;
    if (t_flags & GST_VIDEO_FORMAT_FLAG_GRAY)
      loss += SCORE_COLOR_LOSS;
  }

  if ((t_flags & ALPHA_MASK) != (in_flags & ALPHA_MASK)) {
    loss += SCORE_ALPHA_CHANGE;
    if (in_flags & ALPHA_MASK)
      loss += SCORE_ALPHA_LOSS;
  }

  if ((in_info->h_sub[1]) != (t_info->h_sub[1])) {
    loss += SCORE_CHROMA_H_CHANGE;
    if ((in_info->h_sub[1]) < (t_info->h_sub[1]))
      loss += SCORE_CHROMA_H_LOSS;
  }
  if ((in_info->w_sub[1]) != (t_info->w_sub[1])) {
    loss += SCORE_CHROMA_W_CHANGE;
    if ((in_info->w_sub[1]) < (t_info->w_sub[1]))
      loss += SCORE_CHROMA_W_LOSS;
  }

  if ((in_info->bits) != (t_info->bits)) {
    loss += SCORE_DEPTH_CHANGE;
    if ((in_info->bits) > (t_info->bits))
      loss += SCORE_DEPTH_LOSS;
  }

  GST_DEBUG_OBJECT (base, "score %s -> %s = %d",
      GST_VIDEO_FORMAT_INFO_NAME (in_info),
      GST_VIDEO_FORMAT_INFO_NAME (t_info), loss);

  if (loss < *min_loss) {
    GST_DEBUG_OBJECT (base, "found new best %d", loss);
    *out_info = t_info;
    *min_loss = loss;
  }
}

static void
gst_video_convert_fixate_format (GstBaseTransform * base, GstCaps * caps,
    GstCaps * result)
{
  GstStructure *ins, *outs;
  const gchar *in_format;
  const GstVideoFormatInfo *in_info, *out_info = NULL;
  gint min_loss = G_MAXINT;
  guint i, capslen;

  ins = gst_caps_get_structure (caps, 0);
  in_format = gst_structure_get_string (ins, "format");
  if (!in_format)
    return;

  GST_DEBUG_OBJECT (base, "source format %s", in_format);

  in_info =
      gst_video_format_get_info (gst_video_format_from_string (in_format));
  if (!in_info)
    return;

  outs = gst_caps_get_structure (result, 0);

  capslen = gst_caps_get_size (result);
  GST_DEBUG_OBJECT (base, "iterate %d structures", capslen);
  for (i = 0; i < capslen; i++) {
    GstStructure *tests;
    const GValue *format;

    tests = gst_caps_get_structure (result, i);
    format = gst_structure_get_value (tests, "format");
    /* should not happen */
    if (format == NULL)
      continue;

    if (GST_VALUE_HOLDS_LIST (format)) {
      gint j, len;

      len = gst_value_list_get_size (format);
      GST_DEBUG_OBJECT (base, "have %d formats", len);
      for (j = 0; j < len; j++) {
        const GValue *val;

        val = gst_value_list_get_value (format, j);
        if (G_VALUE_HOLDS_STRING (val)) {
          score_value (base, in_info, val, &min_loss, &out_info);
          if (min_loss == 0)
            break;
        }
      }
    } else if (G_VALUE_HOLDS_STRING (format)) {
      score_value (base, in_info, format, &min_loss, &out_info);
    }
  }
  if (out_info)
    gst_structure_set (outs, "format", G_TYPE_STRING,
        GST_VIDEO_FORMAT_INFO_NAME (out_info), NULL);
}


static GstCaps *
gst_video_convert_fixate_caps (GstBaseTransform * trans,
    GstPadDirection direction, GstCaps * caps, GstCaps * othercaps)
{
  GstCaps *result;

  GST_DEBUG_OBJECT (trans, "trying to fixate othercaps %" GST_PTR_FORMAT
      " based on caps %" GST_PTR_FORMAT, othercaps, caps);

  result = gst_caps_intersect (othercaps, caps);
  if (gst_caps_is_empty (result)) {
    gst_caps_unref (result);
    result = othercaps;
  } else {
    gst_caps_unref (othercaps);
  }

  GST_DEBUG_OBJECT (trans, "now fixating %" GST_PTR_FORMAT, result);

  result = gst_caps_make_writable (result);
  gst_video_convert_fixate_format (trans, caps, result);

  /* fixate remaining fields */
  result = gst_caps_fixate (result);

  if (direction == GST_PAD_SINK) {
    if (gst_caps_is_subset (caps, result)) {
      gst_caps_replace (&result, caps);
    }
  }

  return result;
}

static gboolean
gst_video_convert_filter_meta (GstBaseTransform * trans, GstQuery * query,
    GType api, const GstStructure * params)
{
  /* This element cannot passthrough the crop meta, because it would convert the
   * wrong sub-region of the image, and worst, our output image may not be large
   * enough for the crop to be applied later */
  if (api == GST_VIDEO_CROP_META_API_TYPE)
    return FALSE;

  /* propose all other metadata upstream */
  return TRUE;
}

/* The caps can be transformed into any other caps with format info removed.
 * However, we should prefer passthrough, so if passthrough is possible,
 * put it first in the list. */
static GstCaps *
gst_video_convert_transform_caps (GstBaseTransform * btrans,
    GstPadDirection direction, GstCaps * caps, GstCaps * filter)
{
  GstCaps *tmp, *tmp2;
  GstCaps *result;

  /* Get all possible caps that we can transform to */
  tmp = gst_video_convert_caps_remove_format_info (caps);

  if (filter) {
    tmp2 = gst_caps_intersect_full (filter, tmp, GST_CAPS_INTERSECT_FIRST);
    gst_caps_unref (tmp);
    tmp = tmp2;
  }

  result = tmp;

  GST_DEBUG_OBJECT (btrans, "transformed %" GST_PTR_FORMAT " into %"
      GST_PTR_FORMAT, caps, result);

  return result;
}

static gboolean
gst_video_convert_transform_meta (GstBaseTransform * trans, GstBuffer * outbuf,
    GstMeta * meta, GstBuffer * inbuf)
{
  const GstMetaInfo *info = meta->info;
  gboolean ret;

  if (gst_meta_api_type_has_tag (info->api, _colorspace_quark)) {
    /* don't copy colorspace specific metadata, FIXME, we need a MetaTransform
     * for the colorspace metadata. */
    ret = FALSE;
  } else {
    /* copy other metadata */
    ret = TRUE;
  }
  return ret;
}

static gboolean
gst_video_convert_set_info (GstVideoFilter * filter,
    GstCaps * incaps, GstVideoInfo * in_info, GstCaps * outcaps,
    GstVideoInfo * out_info)
{
  GstVideoConvert *space;

  space = GST_VIDEO_CONVERT_CAST (filter);

  if (space->convert) {
    gst_video_converter_free (space->convert);
    space->convert = NULL;
  }

  /* these must match */
  if (in_info->width != out_info->width || in_info->height != out_info->height
      || in_info->fps_n != out_info->fps_n || in_info->fps_d != out_info->fps_d)
    goto format_mismatch;

  /* if present, these must match too */
  if (in_info->par_n != out_info->par_n || in_info->par_d != out_info->par_d)
    goto format_mismatch;

  /* if present, these must match too */
  if (in_info->interlace_mode != out_info->interlace_mode)
    goto format_mismatch;


  space->convert = gst_video_converter_new (in_info, out_info,
      gst_structure_new ("GstVideoConvertConfig",
          GST_VIDEO_CONVERTER_OPT_DITHER_METHOD, GST_TYPE_VIDEO_DITHER_METHOD,
          space->dither,
          GST_VIDEO_CONVERTER_OPT_DITHER_QUANTIZATION, G_TYPE_UINT,
          space->dither_quantization,
          GST_VIDEO_CONVERTER_OPT_CHROMA_RESAMPLER_METHOD,
          GST_TYPE_VIDEO_RESAMPLER_METHOD, space->chroma_resampler,
          GST_VIDEO_CONVERTER_OPT_ALPHA_MODE,
          GST_TYPE_VIDEO_ALPHA_MODE, space->alpha_mode,
          GST_VIDEO_CONVERTER_OPT_ALPHA_VALUE,
          G_TYPE_DOUBLE, space->alpha_value,
          GST_VIDEO_CONVERTER_OPT_CHROMA_MODE,
          GST_TYPE_VIDEO_CHROMA_MODE, space->chroma_mode,
          GST_VIDEO_CONVERTER_OPT_MATRIX_MODE,
          GST_TYPE_VIDEO_MATRIX_MODE, space->matrix_mode,
          GST_VIDEO_CONVERTER_OPT_GAMMA_MODE,
          GST_TYPE_VIDEO_GAMMA_MODE, space->gamma_mode,
          GST_VIDEO_CONVERTER_OPT_PRIMARIES_MODE,
          GST_TYPE_VIDEO_PRIMARIES_MODE, space->primaries_mode,
          GST_VIDEO_CONVERTER_OPT_THREADS, G_TYPE_UINT,
          space->n_threads, NULL));
  if (space->convert == NULL)
    goto no_convert;

  GST_DEBUG ("reconfigured %d %d", GST_VIDEO_INFO_FORMAT (in_info),
      GST_VIDEO_INFO_FORMAT (out_info));

  return TRUE;

  /* ERRORS */
format_mismatch:
  {
    GST_ERROR_OBJECT (space, "input and output formats do not match");
    return FALSE;
  }
no_convert:
  {
    GST_ERROR_OBJECT (space, "could not create converter");
    return FALSE;
  }
}

static void
gst_video_convert_finalize (GObject * obj)
{
  GstVideoConvert *space = GST_VIDEO_CONVERT (obj);

  if (space->convert) {
    gst_video_converter_free (space->convert);
  }

  G_OBJECT_CLASS (parent_class)->finalize (obj);
}

static void
gst_video_convert_class_init (GstVideoConvertClass * klass)
{
  GObjectClass *gobject_class = (GObjectClass *) klass;
  GstElementClass *gstelement_class = (GstElementClass *) klass;
  GstBaseTransformClass *gstbasetransform_class =
      (GstBaseTransformClass *) klass;
  GstVideoFilterClass *gstvideofilter_class = (GstVideoFilterClass *) klass;

  gobject_class->set_property = gst_video_convert_set_property;
  gobject_class->get_property = gst_video_convert_get_property;
  gobject_class->finalize = gst_video_convert_finalize;

  gst_element_class_add_static_pad_template (gstelement_class,
      &gst_video_convert_src_template);
  gst_element_class_add_static_pad_template (gstelement_class,
      &gst_video_convert_sink_template);

  gst_element_class_set_static_metadata (gstelement_class,
      "Colorspace converter", "Filter/Converter/Video",
      "Converts video from one colorspace to another",
      "GStreamer maintainers <gstreamer-devel@lists.freedesktop.org>");

  gstbasetransform_class->transform_caps =
      GST_DEBUG_FUNCPTR (gst_video_convert_transform_caps);
  gstbasetransform_class->fixate_caps =
      GST_DEBUG_FUNCPTR (gst_video_convert_fixate_caps);
  gstbasetransform_class->filter_meta =
      GST_DEBUG_FUNCPTR (gst_video_convert_filter_meta);
  gstbasetransform_class->transform_meta =
      GST_DEBUG_FUNCPTR (gst_video_convert_transform_meta);

  gstbasetransform_class->passthrough_on_same_caps = TRUE;

  gstvideofilter_class->set_info =
      GST_DEBUG_FUNCPTR (gst_video_convert_set_info);
  gstvideofilter_class->transform_frame =
      GST_DEBUG_FUNCPTR (gst_video_convert_transform_frame);

  g_object_class_install_property (gobject_class, PROP_DITHER,
      g_param_spec_enum ("dither", "Dither", "Apply dithering while converting",
          gst_video_dither_method_get_type (), DEFAULT_PROP_DITHER,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_DITHER_QUANTIZATION,
      g_param_spec_uint ("dither-quantization", "Dither Quantize",
          "Quantizer to use", 0, G_MAXUINT, DEFAULT_PROP_DITHER_QUANTIZATION,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_CHROMA_RESAMPLER,
      g_param_spec_enum ("chroma-resampler", "Chroma resampler",
          "Chroma resampler method", gst_video_resampler_method_get_type (),
          DEFAULT_PROP_CHROMA_RESAMPLER,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_ALPHA_MODE,
      g_param_spec_enum ("alpha-mode", "Alpha Mode",
          "Alpha Mode to use", gst_video_alpha_mode_get_type (),
          DEFAULT_PROP_ALPHA_MODE, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_ALPHA_VALUE,
      g_param_spec_double ("alpha-value", "Alpha Value",
          "Alpha Value to use", 0.0, 1.0,
          DEFAULT_PROP_ALPHA_VALUE,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_CHROMA_MODE,
      g_param_spec_enum ("chroma-mode", "Chroma Mode", "Chroma Resampling Mode",
          gst_video_chroma_mode_get_type (), DEFAULT_PROP_CHROMA_MODE,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_MATRIX_MODE,
      g_param_spec_enum ("matrix-mode", "Matrix Mode", "Matrix Conversion Mode",
          gst_video_matrix_mode_get_type (), DEFAULT_PROP_MATRIX_MODE,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_GAMMA_MODE,
      g_param_spec_enum ("gamma-mode", "Gamma Mode", "Gamma Conversion Mode",
          gst_video_gamma_mode_get_type (), DEFAULT_PROP_GAMMA_MODE,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_PRIMARIES_MODE,
      g_param_spec_enum ("primaries-mode", "Primaries Mode",
          "Primaries Conversion Mode", gst_video_primaries_mode_get_type (),
          DEFAULT_PROP_PRIMARIES_MODE,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_N_THREADS,
      g_param_spec_uint ("n-threads", "Threads",
          "Maximum number of threads to use", 0, G_MAXUINT,
          DEFAULT_PROP_N_THREADS, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

static void
gst_video_convert_init (GstVideoConvert * space)
{
  space->dither = DEFAULT_PROP_DITHER;
  space->dither_quantization = DEFAULT_PROP_DITHER_QUANTIZATION;
  space->chroma_resampler = DEFAULT_PROP_CHROMA_RESAMPLER;
  space->alpha_mode = DEFAULT_PROP_ALPHA_MODE;
  space->alpha_value = DEFAULT_PROP_ALPHA_VALUE;
  space->chroma_mode = DEFAULT_PROP_CHROMA_MODE;
  space->matrix_mode = DEFAULT_PROP_MATRIX_MODE;
  space->gamma_mode = DEFAULT_PROP_GAMMA_MODE;
  space->primaries_mode = DEFAULT_PROP_PRIMARIES_MODE;
  space->n_threads = DEFAULT_PROP_N_THREADS;
}

void
gst_video_convert_set_property (GObject * object, guint property_id,
    const GValue * value, GParamSpec * pspec)
{
  GstVideoConvert *csp;

  csp = GST_VIDEO_CONVERT (object);

  switch (property_id) {
    case PROP_DITHER:
      csp->dither = g_value_get_enum (value);
      break;
    case PROP_CHROMA_RESAMPLER:
      csp->chroma_resampler = g_value_get_enum (value);
      break;
    case PROP_ALPHA_MODE:
      csp->alpha_mode = g_value_get_enum (value);
      break;
    case PROP_ALPHA_VALUE:
      csp->alpha_value = g_value_get_double (value);
      break;
    case PROP_CHROMA_MODE:
      csp->chroma_mode = g_value_get_enum (value);
      break;
    case PROP_MATRIX_MODE:
      csp->matrix_mode = g_value_get_enum (value);
      break;
    case PROP_GAMMA_MODE:
      csp->gamma_mode = g_value_get_enum (value);
      break;
    case PROP_PRIMARIES_MODE:
      csp->primaries_mode = g_value_get_enum (value);
      break;
    case PROP_DITHER_QUANTIZATION:
      csp->dither_quantization = g_value_get_uint (value);
      break;
    case PROP_N_THREADS:
      csp->n_threads = g_value_get_uint (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

void
gst_video_convert_get_property (GObject * object, guint property_id,
    GValue * value, GParamSpec * pspec)
{
  GstVideoConvert *csp;

  csp = GST_VIDEO_CONVERT (object);

  switch (property_id) {
    case PROP_DITHER:
      g_value_set_enum (value, csp->dither);
      break;
    case PROP_CHROMA_RESAMPLER:
      g_value_set_enum (value, csp->chroma_resampler);
      break;
    case PROP_ALPHA_MODE:
      g_value_set_enum (value, csp->alpha_mode);
      break;
    case PROP_ALPHA_VALUE:
      g_value_set_double (value, csp->alpha_value);
      break;
    case PROP_CHROMA_MODE:
      g_value_set_enum (value, csp->chroma_mode);
      break;
    case PROP_MATRIX_MODE:
      g_value_set_enum (value, csp->matrix_mode);
      break;
    case PROP_GAMMA_MODE:
      g_value_set_enum (value, csp->gamma_mode);
      break;
    case PROP_PRIMARIES_MODE:
      g_value_set_enum (value, csp->primaries_mode);
      break;
    case PROP_DITHER_QUANTIZATION:
      g_value_set_uint (value, csp->dither_quantization);
      break;
    case PROP_N_THREADS:
      g_value_set_uint (value, csp->n_threads);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

static GstFlowReturn
gst_video_convert_transform_frame (GstVideoFilter * filter,
    GstVideoFrame * in_frame, GstVideoFrame * out_frame)
{
  GstVideoConvert *space;

  space = GST_VIDEO_CONVERT_CAST (filter);

  GST_CAT_DEBUG_OBJECT (CAT_PERFORMANCE, filter,
      "doing colorspace conversion from %s -> to %s",
      GST_VIDEO_INFO_NAME (&filter->in_info),
      GST_VIDEO_INFO_NAME (&filter->out_info));

  gst_video_converter_frame (space->convert, in_frame, out_frame);

  return GST_FLOW_OK;
}

static gboolean
plugin_init (GstPlugin * plugin)
{
  GST_DEBUG_CATEGORY_INIT (videoconvert_debug, "videoconvert", 0,
      "Colorspace Converter");

  GST_DEBUG_CATEGORY_GET (CAT_PERFORMANCE, "GST_PERFORMANCE");

  _colorspace_quark = g_quark_from_static_string ("colorspace");

  return gst_element_register (plugin, "videoconvert",
      GST_RANK_NONE, GST_TYPE_VIDEO_CONVERT);
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    videoconvert, "Colorspace conversion", plugin_init, VERSION, GST_LICENSE,
    GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)
