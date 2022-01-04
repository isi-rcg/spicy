/*
 * GStreamer Funnel element
 *
 * Copyright 2007 Collabora Ltd.
 *  @author: Olivier Crete <olivier.crete@collabora.co.uk>
 * Copyright 2007 Nokia Corp.
 *
 * gstfunnel.c: Simple Funnel element
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 */

/**
 * SECTION:element-funnel
 * @title: funnel
 *
 * Takes packets from various input sinks into one output source.
 *
 * funnel always outputs a single, open ended segment from
 * 0 with in %GST_FORMAT_TIME and outputs the buffers of the
 * different sinkpads with timestamps that are set to the
 * running time for that stream. funnel does not synchronize
 * the different input streams but simply forwards all buffers
 * immediately when they arrive.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gstfunnel.h"

GST_DEBUG_CATEGORY_STATIC (gst_funnel_debug);
#define GST_CAT_DEFAULT gst_funnel_debug

GType gst_funnel_pad_get_type (void);
#define GST_TYPE_FUNNEL_PAD \
  (gst_funnel_pad_get_type())
#define GST_FUNNEL_PAD(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_FUNNEL_PAD, GstFunnelPad))
#define GST_FUNNEL_PAD_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), GST_TYPE_FUNNEL_PAD, GstFunnelPadClass))
#define GST_IS_FUNNEL_PAD(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_FUNNEL_PAD))
#define GST_IS_FUNNEL_PAD_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), GST_TYPE_FUNNEL_PAD))
#define GST_FUNNEL_PAD_CAST(obj) \
  ((GstFunnelPad *)(obj))

typedef struct _GstFunnelPad GstFunnelPad;
typedef struct _GstFunnelPadClass GstFunnelPadClass;

struct _GstFunnelPad
{
  GstPad parent;

  gboolean got_eos;
};

struct _GstFunnelPadClass
{
  GstPadClass parent;
};

G_DEFINE_TYPE (GstFunnelPad, gst_funnel_pad, GST_TYPE_PAD);

#define DEFAULT_FORWARD_STICKY_EVENTS	TRUE

enum
{
  PROP_0,
  PROP_FORWARD_STICKY_EVENTS
};

static void
gst_funnel_pad_class_init (GstFunnelPadClass * klass)
{
}

static void
gst_funnel_pad_init (GstFunnelPad * pad)
{
  pad->got_eos = FALSE;
}

static GstStaticPadTemplate sink_template = GST_STATIC_PAD_TEMPLATE ("sink_%u",
    GST_PAD_SINK,
    GST_PAD_REQUEST,
    GST_STATIC_CAPS_ANY);

static GstStaticPadTemplate src_template = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS_ANY);

#define _do_init \
  GST_DEBUG_CATEGORY_INIT (gst_funnel_debug, "funnel", 0, "funnel element");
#define gst_funnel_parent_class parent_class
G_DEFINE_TYPE_WITH_CODE (GstFunnel, gst_funnel, GST_TYPE_ELEMENT, _do_init);

static GstStateChangeReturn gst_funnel_change_state (GstElement * element,
    GstStateChange transition);
static GstPad *gst_funnel_request_new_pad (GstElement * element,
    GstPadTemplate * templ, const gchar * name, const GstCaps * caps);
static void gst_funnel_release_pad (GstElement * element, GstPad * pad);

static GstFlowReturn gst_funnel_sink_chain (GstPad * pad, GstObject * parent,
    GstBuffer * buffer);
static GstFlowReturn gst_funnel_sink_chain_list (GstPad * pad,
    GstObject * parent, GstBufferList * list);
static gboolean gst_funnel_sink_event (GstPad * pad, GstObject * parent,
    GstEvent * event);

static void
gst_funnel_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstFunnel *funnel = GST_FUNNEL_CAST (object);

  switch (prop_id) {
    case PROP_FORWARD_STICKY_EVENTS:
      funnel->forward_sticky_events = g_value_get_boolean (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_funnel_get_property (GObject * object, guint prop_id, GValue * value,
    GParamSpec * pspec)
{
  GstFunnel *funnel = GST_FUNNEL_CAST (object);

  switch (prop_id) {
    case PROP_FORWARD_STICKY_EVENTS:
      g_value_set_boolean (value, funnel->forward_sticky_events);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_funnel_dispose (GObject * object)
{
  GstFunnel *funnel = GST_FUNNEL_CAST (object);
  GList *item;

  gst_object_replace ((GstObject **) & funnel->last_sinkpad, NULL);

restart:
  for (item = GST_ELEMENT_PADS (object); item; item = g_list_next (item)) {
    GstPad *pad = GST_PAD (item->data);

    if (GST_PAD_IS_SINK (pad)) {
      gst_element_release_request_pad (GST_ELEMENT (object), pad);
      goto restart;
    }
  }

  G_OBJECT_CLASS (parent_class)->dispose (object);
}

static void
gst_funnel_class_init (GstFunnelClass * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GstElementClass *gstelement_class = GST_ELEMENT_CLASS (klass);

  gobject_class->set_property = gst_funnel_set_property;
  gobject_class->get_property = gst_funnel_get_property;
  gobject_class->dispose = GST_DEBUG_FUNCPTR (gst_funnel_dispose);

  g_object_class_install_property (gobject_class, PROP_FORWARD_STICKY_EVENTS,
      g_param_spec_boolean ("forward-sticky-events", "Forward sticky events",
          "Forward sticky events on stream changes",
          DEFAULT_FORWARD_STICKY_EVENTS,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS |
          GST_PARAM_MUTABLE_READY));

  gst_element_class_set_static_metadata (gstelement_class,
      "Funnel pipe fitting", "Generic", "N-to-1 pipe fitting",
      "Olivier Crete <olivier.crete@collabora.co.uk>");

  gst_element_class_add_static_pad_template (gstelement_class, &sink_template);
  gst_element_class_add_static_pad_template (gstelement_class, &src_template);

  gstelement_class->request_new_pad =
      GST_DEBUG_FUNCPTR (gst_funnel_request_new_pad);
  gstelement_class->release_pad = GST_DEBUG_FUNCPTR (gst_funnel_release_pad);
  gstelement_class->change_state = GST_DEBUG_FUNCPTR (gst_funnel_change_state);
}

static void
gst_funnel_init (GstFunnel * funnel)
{
  funnel->srcpad = gst_pad_new_from_static_template (&src_template, "src");
  gst_pad_use_fixed_caps (funnel->srcpad);

  gst_element_add_pad (GST_ELEMENT (funnel), funnel->srcpad);

  funnel->forward_sticky_events = DEFAULT_FORWARD_STICKY_EVENTS;
}

static GstPad *
gst_funnel_request_new_pad (GstElement * element, GstPadTemplate * templ,
    const gchar * name, const GstCaps * caps)
{
  GstPad *sinkpad;

  GST_DEBUG_OBJECT (element, "requesting pad");

  sinkpad = GST_PAD_CAST (g_object_new (GST_TYPE_FUNNEL_PAD,
          "name", name, "direction", templ->direction, "template", templ,
          NULL));

  gst_pad_set_chain_function (sinkpad,
      GST_DEBUG_FUNCPTR (gst_funnel_sink_chain));
  gst_pad_set_chain_list_function (sinkpad,
      GST_DEBUG_FUNCPTR (gst_funnel_sink_chain_list));
  gst_pad_set_event_function (sinkpad,
      GST_DEBUG_FUNCPTR (gst_funnel_sink_event));

  GST_OBJECT_FLAG_SET (sinkpad, GST_PAD_FLAG_PROXY_CAPS);
  GST_OBJECT_FLAG_SET (sinkpad, GST_PAD_FLAG_PROXY_ALLOCATION);

  gst_pad_set_active (sinkpad, TRUE);

  gst_element_add_pad (element, sinkpad);

  GST_DEBUG_OBJECT (element, "requested pad %s:%s",
      GST_DEBUG_PAD_NAME (sinkpad));

  return sinkpad;
}

static gboolean
gst_funnel_all_sinkpads_eos_unlocked (GstFunnel * funnel, GstPad * pad)
{
  GstElement *element = GST_ELEMENT_CAST (funnel);
  GList *item;
  gboolean all_eos = FALSE;


  if (element->numsinkpads == 0)
    goto done;

  for (item = element->sinkpads; item != NULL; item = g_list_next (item)) {
    GstFunnelPad *sinkpad = GST_FUNNEL_PAD_CAST (item->data);

    if (!sinkpad->got_eos) {
      return FALSE;
    }
  }

  all_eos = TRUE;

done:
  return all_eos;
}

static void
gst_funnel_release_pad (GstElement * element, GstPad * pad)
{
  GstFunnel *funnel = GST_FUNNEL_CAST (element);
  GstFunnelPad *fpad = GST_FUNNEL_PAD_CAST (pad);
  gboolean got_eos;
  gboolean send_eos = FALSE;

  GST_DEBUG_OBJECT (funnel, "releasing pad %s:%s", GST_DEBUG_PAD_NAME (pad));

  gst_pad_set_active (pad, FALSE);

  got_eos = fpad->got_eos;

  gst_element_remove_pad (GST_ELEMENT_CAST (funnel), pad);

  GST_OBJECT_LOCK (funnel);
  if (!got_eos && gst_funnel_all_sinkpads_eos_unlocked (funnel, NULL)) {
    GST_DEBUG_OBJECT (funnel, "Pad removed. All others are EOS. Sending EOS");
    send_eos = TRUE;
  }
  GST_OBJECT_UNLOCK (funnel);

  if (send_eos)
    if (!gst_pad_push_event (funnel->srcpad, gst_event_new_eos ()))
      GST_WARNING_OBJECT (funnel, "Failure pushing EOS");
}

static gboolean
forward_events (GstPad * pad, GstEvent ** event, gpointer user_data)
{
  GstPad *srcpad = user_data;

  if (GST_EVENT_TYPE (*event) != GST_EVENT_EOS)
    gst_pad_push_event (srcpad, gst_event_ref (*event));

  return TRUE;
}

static GstFlowReturn
gst_funnel_sink_chain_object (GstPad * pad, GstFunnel * funnel,
    gboolean is_list, GstMiniObject * obj)
{
  GstFlowReturn res;

  GST_DEBUG_OBJECT (pad, "received %" GST_PTR_FORMAT, obj);

  GST_PAD_STREAM_LOCK (funnel->srcpad);

  if ((funnel->last_sinkpad == NULL) || (funnel->forward_sticky_events
          && (funnel->last_sinkpad != pad))) {
    gst_object_replace ((GstObject **) & funnel->last_sinkpad,
        GST_OBJECT (pad));

    GST_DEBUG_OBJECT (pad, "Forwarding sticky events");
    gst_pad_sticky_events_foreach (pad, forward_events, funnel->srcpad);
  }

  if (is_list)
    res = gst_pad_push_list (funnel->srcpad, GST_BUFFER_LIST_CAST (obj));
  else
    res = gst_pad_push (funnel->srcpad, GST_BUFFER_CAST (obj));

  GST_PAD_STREAM_UNLOCK (funnel->srcpad);

  GST_LOG_OBJECT (pad, "handled buffer%s %s", (is_list ? "list" : ""),
      gst_flow_get_name (res));

  return res;
}

static GstFlowReturn
gst_funnel_sink_chain_list (GstPad * pad, GstObject * parent,
    GstBufferList * list)
{
  GstFunnel *funnel = GST_FUNNEL_CAST (parent);

  return gst_funnel_sink_chain_object (pad, funnel, TRUE,
      GST_MINI_OBJECT_CAST (list));
}

static GstFlowReturn
gst_funnel_sink_chain (GstPad * pad, GstObject * parent, GstBuffer * buffer)
{
  GstFunnel *funnel = GST_FUNNEL_CAST (parent);

  return gst_funnel_sink_chain_object (pad, funnel, FALSE,
      GST_MINI_OBJECT_CAST (buffer));
}

static gboolean
gst_funnel_sink_event (GstPad * pad, GstObject * parent, GstEvent * event)
{
  GstFunnel *funnel = GST_FUNNEL_CAST (parent);
  GstFunnelPad *fpad = GST_FUNNEL_PAD_CAST (pad);
  gboolean forward = TRUE;
  gboolean res = TRUE;
  gboolean unlock = FALSE;

  GST_DEBUG_OBJECT (pad, "received event %" GST_PTR_FORMAT, event);

  if (GST_EVENT_IS_STICKY (event)) {
    unlock = TRUE;
    GST_PAD_STREAM_LOCK (funnel->srcpad);

    if (GST_EVENT_TYPE (event) == GST_EVENT_EOS) {
      GST_OBJECT_LOCK (funnel);
      fpad->got_eos = TRUE;
      if (!gst_funnel_all_sinkpads_eos_unlocked (funnel, pad)) {
        forward = FALSE;
      } else {
        forward = TRUE;
      }
      GST_OBJECT_UNLOCK (funnel);
    } else if (pad != funnel->last_sinkpad) {
      forward = FALSE;
    }
  } else if (GST_EVENT_TYPE (event) == GST_EVENT_FLUSH_STOP) {
    unlock = TRUE;
    GST_PAD_STREAM_LOCK (funnel->srcpad);
    GST_OBJECT_LOCK (funnel);
    fpad->got_eos = FALSE;
    GST_OBJECT_UNLOCK (funnel);
  }

  if (forward && GST_EVENT_IS_SERIALIZED (event)) {
    /* If no data is coming and we receive serialized event, need to forward all sticky events.
     * Otherwise downstream has an inconsistent set of sticky events when
     * handling the new event. */
    if (!unlock) {
      unlock = TRUE;
      GST_PAD_STREAM_LOCK (funnel->srcpad);
    }

    if ((funnel->last_sinkpad == NULL) || (funnel->forward_sticky_events
            && (funnel->last_sinkpad != pad))) {
      gst_object_replace ((GstObject **) & funnel->last_sinkpad,
          GST_OBJECT (pad));
      gst_pad_sticky_events_foreach (pad, forward_events, funnel->srcpad);
    }
  }

  if (forward)
    res = gst_pad_push_event (funnel->srcpad, event);
  else
    gst_event_unref (event);

  if (unlock)
    GST_PAD_STREAM_UNLOCK (funnel->srcpad);

  return res;
}

static void
reset_pad (const GValue * data, gpointer user_data)
{
  GstPad *pad = g_value_get_object (data);
  GstFunnelPad *fpad = GST_FUNNEL_PAD_CAST (pad);
  GstFunnel *funnel = user_data;

  GST_OBJECT_LOCK (funnel);
  fpad->got_eos = FALSE;
  GST_OBJECT_UNLOCK (funnel);
}

static GstStateChangeReturn
gst_funnel_change_state (GstElement * element, GstStateChange transition)
{
  GstStateChangeReturn ret;

  ret = GST_ELEMENT_CLASS (parent_class)->change_state (element, transition);
  if (ret == GST_STATE_CHANGE_FAILURE)
    return ret;

  switch (transition) {
    case GST_STATE_CHANGE_PAUSED_TO_READY:
    {
      GstIterator *iter = gst_element_iterate_sink_pads (element);
      GstIteratorResult res;

      do {
        res = gst_iterator_foreach (iter, reset_pad, element);
        if (res == GST_ITERATOR_RESYNC)
          gst_iterator_resync (iter);
      } while (res == GST_ITERATOR_RESYNC);
      gst_iterator_free (iter);

      if (res == GST_ITERATOR_ERROR)
        return GST_STATE_CHANGE_FAILURE;

    }
      break;
    default:
      break;
  }

  return ret;
}
