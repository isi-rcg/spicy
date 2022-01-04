/* Generic video mixer plugin
 *
 * GStreamer
 * Copyright (C) 2015 Matthew Waters <matthew@centricular.com>
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

#include <gst/gst.h>
#include <gst/video/video.h>

#include "gstglbasemixer.h"

#define GST_CAT_DEFAULT gst_gl_base_mixer_debug
GST_DEBUG_CATEGORY (gst_gl_base_mixer_debug);

static void gst_gl_base_mixer_pad_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);
static void gst_gl_base_mixer_pad_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);

static void gst_gl_base_mixer_set_context (GstElement * element,
    GstContext * context);
static GstStateChangeReturn gst_gl_base_mixer_change_state (GstElement *
    element, GstStateChange transition);

struct _GstGLBaseMixerPrivate
{
  gboolean negotiated;

  GstGLContext *other_context;
};

#define gst_gl_base_mixer_parent_class parent_class
G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE (GstGLBaseMixer, gst_gl_base_mixer,
    GST_TYPE_VIDEO_AGGREGATOR);

G_DEFINE_TYPE (GstGLBaseMixerPad, gst_gl_base_mixer_pad,
    GST_TYPE_VIDEO_AGGREGATOR_PAD);

static void
gst_gl_base_mixer_pad_class_init (GstGLBaseMixerPadClass * klass)
{
  GObjectClass *gobject_class = (GObjectClass *) klass;
  GstVideoAggregatorPadClass *vaggpad_class =
      (GstVideoAggregatorPadClass *) klass;

  gobject_class->set_property = gst_gl_base_mixer_pad_set_property;
  gobject_class->get_property = gst_gl_base_mixer_pad_get_property;

  vaggpad_class->prepare_frame = NULL;
  vaggpad_class->clean_frame = NULL;
}

static void
gst_gl_base_mixer_pad_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  switch (prop_id) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_gl_base_mixer_pad_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  switch (prop_id) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}


static gboolean
_find_local_gl_context (GstGLBaseMixer * mix)
{
  if (gst_gl_query_local_gl_context (GST_ELEMENT (mix), GST_PAD_SRC,
          &mix->context))
    return TRUE;
  if (gst_gl_query_local_gl_context (GST_ELEMENT (mix), GST_PAD_SINK,
          &mix->context))
    return TRUE;
  return FALSE;
}

static gboolean
_get_gl_context (GstGLBaseMixer * mix)
{
  GstGLBaseMixerClass *mix_class = GST_GL_BASE_MIXER_GET_CLASS (mix);
  GError *error = NULL;

  if (!gst_gl_ensure_element_data (mix, &mix->display,
          &mix->priv->other_context))
    return FALSE;

  gst_gl_display_filter_gl_api (mix->display, mix_class->supported_gl_api);

  _find_local_gl_context (mix);

  GST_OBJECT_LOCK (mix->display);
  if (!mix->context) {
    do {
      if (mix->context) {
        gst_object_unref (mix->context);
        mix->context = NULL;
      }
      /* just get a GL context.  we don't care */
      mix->context =
          gst_gl_display_get_gl_context_for_thread (mix->display, NULL);
      if (!mix->context) {
        if (!gst_gl_display_create_context (mix->display,
                mix->priv->other_context, &mix->context, &error)) {
          GST_OBJECT_UNLOCK (mix->display);
          goto context_error;
        }
      }
    } while (!gst_gl_display_add_context (mix->display, mix->context));
  }
  GST_OBJECT_UNLOCK (mix->display);

  {
    GstGLAPI current_gl_api = gst_gl_context_get_gl_api (mix->context);
    if ((current_gl_api & mix_class->supported_gl_api) == 0)
      goto unsupported_gl_api;
  }

  return TRUE;

unsupported_gl_api:
  {
    GstGLAPI gl_api = gst_gl_context_get_gl_api (mix->context);
    gchar *gl_api_str = gst_gl_api_to_string (gl_api);
    gchar *supported_gl_api_str =
        gst_gl_api_to_string (mix_class->supported_gl_api);
    GST_ELEMENT_ERROR (mix, RESOURCE, BUSY,
        ("GL API's not compatible context: %s supported: %s", gl_api_str,
            supported_gl_api_str), (NULL));

    g_free (supported_gl_api_str);
    g_free (gl_api_str);
    return FALSE;
  }
context_error:
  {
    GST_ELEMENT_ERROR (mix, RESOURCE, NOT_FOUND, ("%s", error->message),
        (NULL));
    g_clear_error (&error);
    return FALSE;
  }
}

static gboolean
gst_gl_base_mixer_propose_allocation (GstAggregator * agg,
    GstAggregatorPad * aggpad, GstQuery * decide_query, GstQuery * query)
{
  GstGLBaseMixer *mix = GST_GL_BASE_MIXER (agg);

  if (!_get_gl_context (mix))
    return FALSE;

  return TRUE;
}

static gboolean
gst_gl_base_mixer_sink_query (GstAggregator * agg, GstAggregatorPad * bpad,
    GstQuery * query)
{
  GstGLBaseMixer *mix = GST_GL_BASE_MIXER (agg);

  GST_TRACE ("QUERY %" GST_PTR_FORMAT, query);

  switch (GST_QUERY_TYPE (query)) {
    case GST_QUERY_CONTEXT:
    {
      if (gst_gl_handle_context_query ((GstElement *) mix, query,
              mix->display, mix->context, mix->priv->other_context))
        return TRUE;
      break;
    }
    default:
      break;
  }

  return GST_AGGREGATOR_CLASS (parent_class)->sink_query (agg, bpad, query);;
}

static void
gst_gl_base_mixer_pad_init (GstGLBaseMixerPad * mixerpad)
{
}

/* GLBaseMixer signals and args */
enum
{
  /* FILL ME */
  LAST_SIGNAL
};

enum
{
  PROP_0,
  PROP_CONTEXT
};

static gboolean gst_gl_base_mixer_src_query (GstAggregator * agg,
    GstQuery * query);

static gboolean
gst_gl_base_mixer_src_activate_mode (GstAggregator * aggregator,
    GstPadMode mode, gboolean active);
static gboolean gst_gl_base_mixer_stop (GstAggregator * agg);
static gboolean gst_gl_base_mixer_start (GstAggregator * agg);

static void gst_gl_base_mixer_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void gst_gl_base_mixer_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);

static gboolean gst_gl_base_mixer_decide_allocation (GstAggregator * agg,
    GstQuery * query);

static void
gst_gl_base_mixer_class_init (GstGLBaseMixerClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *element_class;
  GstAggregatorClass *agg_class = (GstAggregatorClass *) klass;

  GST_DEBUG_CATEGORY_INIT (GST_CAT_DEFAULT, "glmixer", 0, "opengl mixer");

  gobject_class = (GObjectClass *) klass;
  element_class = GST_ELEMENT_CLASS (klass);

  gobject_class->get_property = gst_gl_base_mixer_get_property;
  gobject_class->set_property = gst_gl_base_mixer_set_property;

  element_class->set_context =
      GST_DEBUG_FUNCPTR (gst_gl_base_mixer_set_context);
  element_class->change_state = gst_gl_base_mixer_change_state;

  agg_class->sink_query = gst_gl_base_mixer_sink_query;
  agg_class->src_query = gst_gl_base_mixer_src_query;
  agg_class->src_activate = gst_gl_base_mixer_src_activate_mode;
  agg_class->stop = gst_gl_base_mixer_stop;
  agg_class->start = gst_gl_base_mixer_start;
  agg_class->decide_allocation = gst_gl_base_mixer_decide_allocation;
  agg_class->propose_allocation = gst_gl_base_mixer_propose_allocation;

  g_object_class_install_property (gobject_class, PROP_CONTEXT,
      g_param_spec_object ("context",
          "OpenGL context",
          "Get OpenGL context",
          GST_TYPE_GL_CONTEXT, G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

  /* Register the pad class */
  g_type_class_ref (GST_TYPE_GL_BASE_MIXER_PAD);

  klass->supported_gl_api = GST_GL_API_ANY;
}

static void
gst_gl_base_mixer_init (GstGLBaseMixer * mix)
{
  mix->priv = gst_gl_base_mixer_get_instance_private (mix);
}

static void
gst_gl_base_mixer_set_context (GstElement * element, GstContext * context)
{
  GstGLBaseMixer *mix = GST_GL_BASE_MIXER (element);
  GstGLBaseMixerClass *mix_class = GST_GL_BASE_MIXER_GET_CLASS (mix);

  gst_gl_handle_set_context (element, context, &mix->display,
      &mix->priv->other_context);

  if (mix->display)
    gst_gl_display_filter_gl_api (mix->display, mix_class->supported_gl_api);

  GST_ELEMENT_CLASS (parent_class)->set_context (element, context);
}

static gboolean
gst_gl_base_mixer_activate (GstGLBaseMixer * mix, gboolean active)
{
  GstGLBaseMixerClass *mix_class = GST_GL_BASE_MIXER_GET_CLASS (mix);
  gboolean result = TRUE;

  if (active) {
    if (!gst_gl_ensure_element_data (mix, &mix->display,
            &mix->priv->other_context))
      return FALSE;

    gst_gl_display_filter_gl_api (mix->display, mix_class->supported_gl_api);
  }

  return result;
}

static gboolean
gst_gl_base_mixer_src_activate_mode (GstAggregator * aggregator,
    GstPadMode mode, gboolean active)
{
  GstGLBaseMixer *mix;
  gboolean result = FALSE;

  mix = GST_GL_BASE_MIXER (aggregator);

  switch (mode) {
    case GST_PAD_MODE_PUSH:
    case GST_PAD_MODE_PULL:
      result = gst_gl_base_mixer_activate (mix, active);
      break;
    default:
      result = TRUE;
      break;
  }
  return result;
}

static gboolean
gst_gl_base_mixer_src_query (GstAggregator * agg, GstQuery * query)
{
  GstGLBaseMixer *mix = GST_GL_BASE_MIXER (agg);

  switch (GST_QUERY_TYPE (query)) {
    case GST_QUERY_CONTEXT:
    {
      if (gst_gl_handle_context_query ((GstElement *) mix, query,
              mix->display, mix->context, mix->priv->other_context))
        return TRUE;
      break;
    }
    default:
      break;
  }

  return GST_AGGREGATOR_CLASS (parent_class)->src_query (agg, query);
}

static gboolean
gst_gl_base_mixer_decide_allocation (GstAggregator * agg, GstQuery * query)
{
  GstGLBaseMixer *mix = GST_GL_BASE_MIXER (agg);

  if (!_get_gl_context (mix))
    return FALSE;

  return TRUE;
}

static void
gst_gl_base_mixer_get_property (GObject * object,
    guint prop_id, GValue * value, GParamSpec * pspec)
{
  GstGLBaseMixer *mixer = GST_GL_BASE_MIXER (object);

  switch (prop_id) {
    case PROP_CONTEXT:
      g_value_set_object (value, mixer->context);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_gl_base_mixer_set_property (GObject * object,
    guint prop_id, const GValue * value, GParamSpec * pspec)
{
  switch (prop_id) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static gboolean
gst_gl_base_mixer_start (GstAggregator * agg)
{
  return GST_AGGREGATOR_CLASS (parent_class)->start (agg);;
}

static gboolean
gst_gl_base_mixer_stop (GstAggregator * agg)
{
  GstGLBaseMixer *mix = GST_GL_BASE_MIXER (agg);

  if (mix->context) {
    gst_object_unref (mix->context);
    mix->context = NULL;
  }

  return TRUE;
}

static GstStateChangeReturn
gst_gl_base_mixer_change_state (GstElement * element, GstStateChange transition)
{
  GstGLBaseMixer *mix = GST_GL_BASE_MIXER (element);
  GstGLBaseMixerClass *mix_class = GST_GL_BASE_MIXER_GET_CLASS (mix);
  GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;

  GST_DEBUG_OBJECT (mix, "changing state: %s => %s",
      gst_element_state_get_name (GST_STATE_TRANSITION_CURRENT (transition)),
      gst_element_state_get_name (GST_STATE_TRANSITION_NEXT (transition)));

  switch (transition) {
    case GST_STATE_CHANGE_NULL_TO_READY:
      if (!gst_gl_ensure_element_data (element, &mix->display,
              &mix->priv->other_context))
        return GST_STATE_CHANGE_FAILURE;

      gst_gl_display_filter_gl_api (mix->display, mix_class->supported_gl_api);
      break;
    default:
      break;
  }

  ret = GST_ELEMENT_CLASS (parent_class)->change_state (element, transition);
  if (ret == GST_STATE_CHANGE_FAILURE)
    return ret;

  switch (transition) {
    case GST_STATE_CHANGE_READY_TO_NULL:
      if (mix->priv->other_context) {
        gst_object_unref (mix->priv->other_context);
        mix->priv->other_context = NULL;
      }

      if (mix->display) {
        gst_object_unref (mix->display);
        mix->display = NULL;
      }
      break;
    default:
      break;
  }

  return ret;
}
