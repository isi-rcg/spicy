#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
/* A Bison parser, made by GNU Bison 3.4.1.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2019 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         priv_gst_parse_yyparse
#define yylex           priv_gst_parse_yylex
#define yyerror         priv_gst_parse_yyerror
#define yydebug         priv_gst_parse_yydebug
#define yynerrs         priv_gst_parse_yynerrs


/* First part of user prologue.  */
#line 1 "../../../git/gst/parse/grammar.y"

#include "../gst_private.h"

#include <glib-object.h>
#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../gst-i18n-lib.h"

#include "../gstconfig.h"
#include "../gstparse.h"
#include "../gstinfo.h"
#include "../gsterror.h"
#include "../gststructure.h"
#include "../gsturi.h"
#include "../gstutils.h"
#include "../gstvalue.h"
#include "../gstchildproxy.h"
#include "types.h"

/* All error messages in this file are user-visible and need to be translated.
 * Don't start the message with a capital, and don't end them with a period,
 * as they will be presented inside a sentence/error.
 */

#define YYERROR_VERBOSE 1

#define YYENABLE_NLS 0

#ifndef YYLTYPE_IS_TRIVIAL
#define YYLTYPE_IS_TRIVIAL 0
#endif

/*******************************************************************************************
*** Tracing memory leaks
*******************************************************************************************/

#ifdef __GST_PARSE_TRACE
static guint __strings;
static guint __links;
static guint __chains;
gchar *
__gst_parse_strdup (gchar *org)
{
  gchar *ret;
  __strings++;
  ret = g_strdup (org);
  /* g_print ("ALLOCATED STR   (%3u): %p %s\n", __strings, ret, ret); */
  return ret;
}
void
__gst_parse_strfree (gchar *str)
{
  if (str) {
    /* g_print ("FREEING STR     (%3u): %p %s\n", __strings - 1, str, str); */
    g_free (str);
    g_return_if_fail (__strings > 0);
    __strings--;
  }
}
link_t *__gst_parse_link_new (void)
{
  link_t *ret;
  __links++;
  ret = g_slice_new0 (link_t);
  /* g_print ("ALLOCATED LINK  (%3u): %p\n", __links, ret); */
  return ret;
}
void
__gst_parse_link_free (link_t *data)
{
  if (data) {
    /* g_print ("FREEING LINK    (%3u): %p\n", __links - 1, data); */
    g_slice_free (link_t, data);
    g_return_if_fail (__links > 0);
    __links--;
  }
}
chain_t *
__gst_parse_chain_new (void)
{
  chain_t *ret;
  __chains++;
  ret = g_slice_new0 (chain_t);
  /* g_print ("@%p: ALLOCATED CHAIN (%3u):\n", ret, __chains); */
  return ret;
}
void
__gst_parse_chain_free (chain_t *data)
{
  /* g_print ("@%p: FREEING CHAIN   (%3u):\n", data, __chains - 1); */
  g_slice_free (chain_t, data);
  g_return_if_fail (__chains > 0);
  __chains--;
}

#endif /* __GST_PARSE_TRACE */

/*******************************************************************************************
*** define SET_ERROR macro/function
*******************************************************************************************/
#ifdef G_HAVE_ISO_VARARGS

#  define SET_ERROR(error, type, ...) \
G_STMT_START { \
  GST_CAT_ERROR (GST_CAT_PIPELINE, __VA_ARGS__); \
  if ((error) && !*(error)) { \
    g_set_error ((error), GST_PARSE_ERROR, (type), __VA_ARGS__); \
  } \
} G_STMT_END

#elif defined(G_HAVE_GNUC_VARARGS)

#  define SET_ERROR(error, type, args...) \
G_STMT_START { \
  GST_CAT_ERROR (GST_CAT_PIPELINE, args ); \
  if ((error) && !*(error)) { \
    g_set_error ((error), GST_PARSE_ERROR, (type), args ); \
  } \
} G_STMT_END

#else

static inline void
SET_ERROR (GError **error, gint type, const char *format, ...)
{
  if (error) {
    if (*error) {
      g_warning ("error while parsing");
    } else {
      va_list varargs;
      char *string;

      va_start (varargs, format);
      string = g_strdup_vprintf (format, varargs);
      va_end (varargs);

      g_set_error (error, GST_PARSE_ERROR, type, string);

      g_free (string);
    }
  }
}

#endif /* G_HAVE_ISO_VARARGS */

/*** define YYPRINTF macro/function if we're debugging */

/* bison 1.35 calls this macro with side effects, we need to make sure the
   side effects work - crappy bison */

#ifndef GST_DISABLE_GST_DEBUG
#  define YYDEBUG 1

#  ifdef G_HAVE_ISO_VARARGS

/* #  define YYFPRINTF(a, ...) GST_CAT_DEBUG (GST_CAT_PIPELINE, __VA_ARGS__) */
#    define YYFPRINTF(a, ...) \
G_STMT_START { \
     GST_CAT_LOG (GST_CAT_PIPELINE, __VA_ARGS__); \
} G_STMT_END

#  elif defined(G_HAVE_GNUC_VARARGS)

#    define YYFPRINTF(a, args...) \
G_STMT_START { \
     GST_CAT_LOG (GST_CAT_PIPELINE, args); \
} G_STMT_END

#  else

static inline void
YYPRINTF(const char *format, ...)
{
  va_list varargs;
  gchar *temp;

  va_start (varargs, format);
  temp = g_strdup_vprintf (format, varargs);
  GST_CAT_LOG (GST_CAT_PIPELINE, "%s", temp);
  g_free (temp);
  va_end (varargs);
}

#  endif /* G_HAVE_ISO_VARARGS */

#endif /* GST_DISABLE_GST_DEBUG */


/*
 * include headers generated by bison & flex, after defining (or not defining) YYDEBUG
 */
#include "grammar.tab.h"
#include "parse_lex.h"

/*******************************************************************************************
*** report missing elements/bins/..
*******************************************************************************************/


static void  add_missing_element(graph_t *graph,gchar *name){
  if ((graph)->ctx){
    (graph)->ctx->missing_elements = g_list_append ((graph)->ctx->missing_elements, g_strdup (name));
    }
}


/*******************************************************************************************
*** helpers for pipeline-setup
*******************************************************************************************/

#define TRY_SETUP_LINK(l) G_STMT_START { \
   if( (!(l)->src.element) && (!(l)->src.name) ){ \
     SET_ERROR (graph->error, GST_PARSE_ERROR_LINK, _("link has no source [sink=%s@%p]"), \
	(l)->sink.name ? (l)->sink.name : "", \
	(l)->sink.element); \
     gst_parse_free_link (l); \
   }else if( (!(l)->sink.element) && (!(l)->sink.name) ){ \
     SET_ERROR (graph->error, GST_PARSE_ERROR_LINK, _("link has no sink [source=%s@%p]"), \
	(l)->src.name ? (l)->src.name : "", \
	(l)->src.element); \
     gst_parse_free_link (l); \
   }else{ \
     graph->links = g_slist_append (graph->links, l ); \
   }   \
} G_STMT_END

typedef struct {
  gchar *src_pad;
  gchar *sink_pad;
  GstElement *sink;
  GstCaps *caps;
  gulong pad_added_signal_id, no_more_pads_signal_id;
  gboolean all_pads;
} DelayedLink;

typedef struct {
  gchar *name;
  gchar *value_str;
  gulong signal_id;
} DelayedSet;

static int  gst_resolve_reference(reference_t *rr, GstElement *pipeline){
  GstBin *bin;

  if(rr->element) return  0;  /* already resolved! */
  if(!rr->name)   return -2;  /* no chance! */

  if (GST_IS_BIN (pipeline)){
    bin = GST_BIN (pipeline);
    rr->element = gst_bin_get_by_name_recurse_up (bin, rr->name);
  } else {
    rr->element = strcmp (GST_ELEMENT_NAME (pipeline), rr->name) == 0 ?
		gst_object_ref(pipeline) : NULL;
  }
  if(rr->element) return 0; /* resolved */
  else            return -1; /* not found */
}

static void gst_parse_free_delayed_set (DelayedSet *set)
{
  g_free(set->name);
  g_free(set->value_str);
  g_slice_free(DelayedSet, set);
}

static void gst_parse_new_child(GstChildProxy *child_proxy, GObject *object,
    const gchar * name, gpointer data);

static void gst_parse_add_delayed_set (GstElement *element, gchar *name, gchar *value_str)
{
  DelayedSet *data = g_slice_new0 (DelayedSet);

  GST_CAT_LOG_OBJECT (GST_CAT_PIPELINE, element, "delaying property set %s to %s",
    name, value_str);

  data->name = g_strdup(name);
  data->value_str = g_strdup(value_str);
  data->signal_id = g_signal_connect_data(element, "child-added",
      G_CALLBACK (gst_parse_new_child), data, (GClosureNotify)
      gst_parse_free_delayed_set, (GConnectFlags) 0);

  /* FIXME: we would need to listen on all intermediate bins too */
  if (GST_IS_BIN (element)) {
    gchar **names, **current;
    GstElement *parent, *child;

    current = names = g_strsplit (name, "::", -1);
    parent = gst_bin_get_by_name (GST_BIN_CAST (element), current[0]);
    current++;
    while (parent && current[0]) {
      child = gst_bin_get_by_name (GST_BIN (parent), current[0]);
      if (!child && current[1]) {
        char *sub_name = g_strjoinv ("::", &current[0]);

        gst_parse_add_delayed_set(parent, sub_name, value_str);
        g_free (sub_name);
      }
      gst_object_unref (parent);
      parent = child;
      current++;
    }
    if (parent)
      gst_object_unref (parent);
    g_strfreev (names);
  }
}

static void gst_parse_new_child(GstChildProxy *child_proxy, GObject *object,
    const gchar * name, gpointer data)
{
  DelayedSet *set = (DelayedSet *) data;
  GParamSpec *pspec;
  GValue v = { 0, };
  GObject *target = NULL;
  GType value_type;

  GST_CAT_LOG_OBJECT (GST_CAT_PIPELINE, child_proxy, "new child %s, checking property %s",
      name, set->name);

  if (gst_child_proxy_lookup (child_proxy, set->name, &target, &pspec)) {
    gboolean got_value = FALSE;

    value_type = pspec->value_type;

    GST_CAT_LOG_OBJECT (GST_CAT_PIPELINE, child_proxy, "parsing delayed property %s as a %s from %s",
      pspec->name, g_type_name (value_type), set->value_str);
    g_value_init (&v, value_type);
    if (gst_value_deserialize (&v, set->value_str))
      got_value = TRUE;
    else if (g_type_is_a (value_type, GST_TYPE_ELEMENT)) {
       GstElement *bin;

       bin = gst_parse_bin_from_description_full (set->value_str, TRUE, NULL,
           GST_PARSE_FLAG_NO_SINGLE_ELEMENT_BINS | GST_PARSE_FLAG_PLACE_IN_BIN, NULL);
       if (bin) {
         g_value_set_object (&v, bin);
         got_value = TRUE;
       }
    }
    g_signal_handler_disconnect (child_proxy, set->signal_id);
    if (!got_value)
      goto error;
    g_object_set_property (target, pspec->name, &v);
  } else {
    const gchar *obj_name = GST_OBJECT_NAME(object);
    gint len = strlen (obj_name);

    /* do a delayed set */
    if ((strlen (set->name) > (len + 2)) && !strncmp (set->name, obj_name, len) && !strncmp (&set->name[len], "::", 2)) {
      gst_parse_add_delayed_set (GST_ELEMENT(child_proxy), set->name, set->value_str);
    }
  }

out:
  if (G_IS_VALUE (&v))
    g_value_unset (&v);
  if (target)
    g_object_unref (target);
  return;

error:
  GST_CAT_ERROR (GST_CAT_PIPELINE, "could not set property \"%s\" in %"
      GST_PTR_FORMAT, pspec->name, target);
  goto out;
}

static void gst_parse_element_set (gchar *value, GstElement *element, graph_t *graph)
{
  GParamSpec *pspec = NULL;
  gchar *pos = value;
  GValue v = { 0, };
  GObject *target = NULL;
  GType value_type;

  /* do nothing if assignment is for missing element */
  if (element == NULL)
    goto out;

  /* parse the string, so the property name is null-terminated and pos points
     to the beginning of the value */
  while (!g_ascii_isspace (*pos) && (*pos != '=')) pos++;
  if (*pos == '=') {
    *pos = '\0';
  } else {
    *pos = '\0';
    pos++;
    while (g_ascii_isspace (*pos)) pos++;
  }
  pos++;
  while (g_ascii_isspace (*pos)) pos++;
  /* truncate a string if it is delimited with double quotes */
  if (*pos == '"' && pos[strlen (pos) - 1] == '"') {
    pos++;
    pos[strlen (pos) - 1] = '\0';
  }
  gst_parse_unescape (pos);

  if (GST_IS_CHILD_PROXY (element)) {
    if (!gst_child_proxy_lookup (GST_CHILD_PROXY (element), value, &target, &pspec)) {
      /* do a delayed set */
      gst_parse_add_delayed_set (element, value, pos);
    }
  } else {
    pspec = g_object_class_find_property (G_OBJECT_GET_CLASS (element), value);
    if (pspec != NULL) {
      target = G_OBJECT (g_object_ref (element));
      GST_CAT_LOG_OBJECT (GST_CAT_PIPELINE, target, "found %s property", value);
    } else {
      SET_ERROR (graph->error, GST_PARSE_ERROR_NO_SUCH_PROPERTY, \
          _("no property \"%s\" in element \"%s\""), value, \
          GST_ELEMENT_NAME (element));
    }
  }

  if (pspec != NULL && target != NULL) {
    gboolean got_value = FALSE;

    value_type = pspec->value_type;

    GST_CAT_LOG_OBJECT (GST_CAT_PIPELINE, element, "parsing property %s as a %s",
        pspec->name, g_type_name (value_type));

    g_value_init (&v, value_type);
    if (gst_value_deserialize (&v, pos))
      got_value = TRUE;
    else if (g_type_is_a (value_type, GST_TYPE_ELEMENT)) {
       GstElement *bin;

       bin = gst_parse_bin_from_description_full (pos, TRUE, NULL,
           GST_PARSE_FLAG_NO_SINGLE_ELEMENT_BINS | GST_PARSE_FLAG_PLACE_IN_BIN, NULL);
       if (bin) {
         g_value_set_object (&v, bin);
         got_value = TRUE;
       }
    }
    if (!got_value)
      goto error;
    g_object_set_property (target, pspec->name, &v);
  }

out:
  gst_parse_strfree (value);
  if (G_IS_VALUE (&v))
    g_value_unset (&v);
  if (target)
    g_object_unref (target);
  return;

error:
  SET_ERROR (graph->error, GST_PARSE_ERROR_COULD_NOT_SET_PROPERTY,
         _("could not set property \"%s\" in element \"%s\" to \"%s\""),
	 value, GST_ELEMENT_NAME (element), pos);
  goto out;
}

static void gst_parse_free_reference (reference_t *rr)
{
  if(rr->element) gst_object_unref(rr->element);
  gst_parse_strfree (rr->name);
  g_slist_foreach (rr->pads, (GFunc) gst_parse_strfree, NULL);
  g_slist_free (rr->pads);
}

static void gst_parse_free_link (link_t *link)
{
  gst_parse_free_reference (&(link->src));
  gst_parse_free_reference (&(link->sink));
  if (link->caps) gst_caps_unref (link->caps);
  gst_parse_link_free (link);
}

static void gst_parse_free_chain (chain_t *ch)
{
  GSList *walk;
  gst_parse_free_reference (&(ch->first));
  gst_parse_free_reference (&(ch->last));
  for(walk=ch->elements;walk;walk=walk->next)
    gst_object_unref (walk->data);
  g_slist_free (ch->elements);
  gst_parse_chain_free (ch);
}

static void gst_parse_free_delayed_link (DelayedLink *link)
{
  g_free (link->src_pad);
  g_free (link->sink_pad);
  if (link->caps) gst_caps_unref (link->caps);
  g_slice_free (DelayedLink, link);
}

#define PRETTY_PAD_NAME_FMT "%s %s of %s named %s"
#define PRETTY_PAD_NAME_ARGS(elem, pad_name) \
  (pad_name ? "pad " : "some"), (pad_name ? pad_name : "pad"), \
  G_OBJECT_TYPE_NAME(elem), GST_STR_NULL (GST_ELEMENT_NAME (elem))

static void gst_parse_no_more_pads (GstElement *src, gpointer data)
{
  DelayedLink *link = data;

  /* Don't warn for all-pads links, as we expect those to
   * still be active at no-more-pads */
  if (!link->all_pads) {
    GST_ELEMENT_WARNING(src, PARSE, DELAYED_LINK,
      (_("Delayed linking failed.")),
      ("failed delayed linking " PRETTY_PAD_NAME_FMT " to " PRETTY_PAD_NAME_FMT,
          PRETTY_PAD_NAME_ARGS (src, link->src_pad),
          PRETTY_PAD_NAME_ARGS (link->sink, link->sink_pad)));
  }
  /* we keep the handlers connected, so that in case an element still adds a pad
   * despite no-more-pads, we will consider it for pending delayed links */
}

static void gst_parse_found_pad (GstElement *src, GstPad *pad, gpointer data)
{
  DelayedLink *link = data;

  GST_CAT_INFO (GST_CAT_PIPELINE,
                "trying delayed linking %s " PRETTY_PAD_NAME_FMT " to " PRETTY_PAD_NAME_FMT,
		            link->all_pads ? "all pads" : "one pad",
                PRETTY_PAD_NAME_ARGS (src, link->src_pad),
                PRETTY_PAD_NAME_ARGS (link->sink, link->sink_pad));

  if (gst_element_link_pads_filtered (src, link->src_pad, link->sink,
      link->sink_pad, link->caps)) {
    /* do this here, we don't want to get any problems later on when
     * unlocking states */
    GST_CAT_DEBUG (GST_CAT_PIPELINE,
                   "delayed linking %s " PRETTY_PAD_NAME_FMT " to " PRETTY_PAD_NAME_FMT " worked",
		               link->all_pads ? "all pads" : "one pad",
               	   PRETTY_PAD_NAME_ARGS (src, link->src_pad),
                   PRETTY_PAD_NAME_ARGS (link->sink, link->sink_pad));
    g_signal_handler_disconnect (src, link->no_more_pads_signal_id);
    /* releases 'link' */
    if (!link->all_pads)
      g_signal_handler_disconnect (src, link->pad_added_signal_id);
  }
}

/* both padnames and the caps may be NULL */
static gboolean
gst_parse_perform_delayed_link (GstElement *src, const gchar *src_pad,
                                GstElement *sink, const gchar *sink_pad,
                                GstCaps *caps, gboolean all_pads)
{
  GList *templs = gst_element_class_get_pad_template_list (
      GST_ELEMENT_GET_CLASS (src));

  for (; templs; templs = templs->next) {
    GstPadTemplate *templ = (GstPadTemplate *) templs->data;
    if ((GST_PAD_TEMPLATE_DIRECTION (templ) == GST_PAD_SRC) &&
        (GST_PAD_TEMPLATE_PRESENCE(templ) == GST_PAD_SOMETIMES))
    {
      DelayedLink *data = g_slice_new (DelayedLink);

      data->all_pads = all_pads;

      /* TODO: maybe we should check if src_pad matches this template's names */

      GST_CAT_DEBUG (GST_CAT_PIPELINE,
                     "trying delayed link " PRETTY_PAD_NAME_FMT " to " PRETTY_PAD_NAME_FMT,
                     PRETTY_PAD_NAME_ARGS (src, src_pad),
                     PRETTY_PAD_NAME_ARGS (sink, sink_pad));

      data->src_pad = g_strdup (src_pad);
      data->sink = sink;
      data->sink_pad = g_strdup (sink_pad);
      if (caps) {
      	data->caps = gst_caps_copy (caps);
      } else {
      	data->caps = NULL;
      }
      data->pad_added_signal_id = g_signal_connect_data (src, "pad-added",
          G_CALLBACK (gst_parse_found_pad), data,
          (GClosureNotify) gst_parse_free_delayed_link, (GConnectFlags) 0);
      data->no_more_pads_signal_id = g_signal_connect (src, "no-more-pads",
          G_CALLBACK (gst_parse_no_more_pads), data);
      return TRUE;
    }
  }
  return FALSE;
}

static gboolean
gst_parse_element_can_do_caps (GstElement * e, GstPadDirection dir,
    GstCaps * link_caps)
{
  gboolean can_do = FALSE, done = FALSE;
  GstIterator *it;

  it = (dir == GST_PAD_SRC) ? gst_element_iterate_src_pads (e) : gst_element_iterate_sink_pads (e);

  while (!done && !can_do) {
    GValue v = G_VALUE_INIT;
    GstPad *pad;
    GstCaps *caps;

    switch (gst_iterator_next (it, &v)) {
      case GST_ITERATOR_OK:
        pad = g_value_get_object (&v);

        caps = gst_pad_get_current_caps (pad);
        if (caps == NULL)
          caps = gst_pad_query_caps (pad, NULL);

        can_do = gst_caps_can_intersect (caps, link_caps);

        GST_TRACE ("can_do: %d for %" GST_PTR_FORMAT " and %" GST_PTR_FORMAT,
            can_do, caps, link_caps);

        gst_caps_unref (caps);

        g_value_unset (&v);
        break;
      case GST_ITERATOR_DONE:
      case GST_ITERATOR_ERROR:
        done = TRUE;
        break;
      case GST_ITERATOR_RESYNC:
        gst_iterator_resync (it);
        break;
    }
  }

  gst_iterator_free (it);

  return can_do;
}

/*
 * performs a link and frees the struct. src and sink elements must be given
 * return values   0 - link performed
 *                 1 - link delayed
 *                <0 - error
 */
static gint
gst_parse_perform_link (link_t *link, graph_t *graph)
{
  GstElement *src = link->src.element;
  GstElement *sink = link->sink.element;
  GSList *srcs = link->src.pads;
  GSList *sinks = link->sink.pads;
  g_assert (GST_IS_ELEMENT (src));
  g_assert (GST_IS_ELEMENT (sink));

  GST_CAT_INFO (GST_CAT_PIPELINE,
      "linking " PRETTY_PAD_NAME_FMT " to " PRETTY_PAD_NAME_FMT " (%u/%u) with caps \"%" GST_PTR_FORMAT "\"",
      PRETTY_PAD_NAME_ARGS (src, link->src.name),
      PRETTY_PAD_NAME_ARGS (sink, link->sink.name),
      g_slist_length (srcs), g_slist_length (sinks), link->caps);

  if (!srcs || !sinks) {
    gboolean found_one = gst_element_link_pads_filtered (src,
        srcs ? (const gchar *) srcs->data : NULL, sink,
        sinks ? (const gchar *) sinks->data : NULL, link->caps);

    if (found_one) {
      if (!link->all_pads)
        goto success; /* Linked one, and not an all-pads link = we're done */

      /* Try and link more available pads */
      while (gst_element_link_pads_filtered (src,
        srcs ? (const gchar *) srcs->data : NULL, sink,
        sinks ? (const gchar *) sinks->data : NULL, link->caps));
    }

    /* We either didn't find any static pads, or this is a all-pads link,
     * in which case watch for future pads and link those. Not a failure
     * in the all-pads case if there's no sometimes pads to watch */
    if (gst_parse_perform_delayed_link (src,
          srcs ? (const gchar *) srcs->data : NULL,
          sink, sinks ? (const gchar *) sinks->data : NULL, link->caps,
	  link->all_pads) || link->all_pads) {
      goto success;
    } else {
      goto error;
    }
  }
  if (g_slist_length (link->src.pads) != g_slist_length (link->sink.pads)) {
    goto error;
  }
  while (srcs && sinks) {
    const gchar *src_pad = (const gchar *) srcs->data;
    const gchar *sink_pad = (const gchar *) sinks->data;
    srcs = g_slist_next (srcs);
    sinks = g_slist_next (sinks);
    if (gst_element_link_pads_filtered (src, src_pad, sink, sink_pad,
        link->caps)) {
      continue;
    } else {
      if (gst_parse_perform_delayed_link (src, src_pad,
                                          sink, sink_pad,
					  link->caps, link->all_pads)) {
	continue;
      } else {
        goto error;
      }
    }
  }

success:
  gst_parse_free_link (link);
  return 0;

error:
  if (link->caps != NULL) {
    gboolean src_can_do_caps, sink_can_do_caps;
    gchar *caps_str = gst_caps_to_string (link->caps);

    src_can_do_caps =
        gst_parse_element_can_do_caps (src, GST_PAD_SRC, link->caps);
    sink_can_do_caps =
        gst_parse_element_can_do_caps (sink, GST_PAD_SINK, link->caps);

    if (!src_can_do_caps && sink_can_do_caps) {
      SET_ERROR (graph->error, GST_PARSE_ERROR_LINK,
          _("could not link %s to %s, %s can't handle caps %s"),
          GST_ELEMENT_NAME (src), GST_ELEMENT_NAME (sink),
          GST_ELEMENT_NAME (src), caps_str);
    } else if (src_can_do_caps && !sink_can_do_caps) {
      SET_ERROR (graph->error, GST_PARSE_ERROR_LINK,
          _("could not link %s to %s, %s can't handle caps %s"),
          GST_ELEMENT_NAME (src), GST_ELEMENT_NAME (sink),
          GST_ELEMENT_NAME (sink), caps_str);
    } else if (!src_can_do_caps && !sink_can_do_caps) {
      SET_ERROR (graph->error, GST_PARSE_ERROR_LINK,
          _("could not link %s to %s, neither element can handle caps %s"),
          GST_ELEMENT_NAME (src), GST_ELEMENT_NAME (sink), caps_str);
    } else {
      SET_ERROR (graph->error, GST_PARSE_ERROR_LINK,
          _("could not link %s to %s with caps %s"),
          GST_ELEMENT_NAME (src), GST_ELEMENT_NAME (sink), caps_str);
    }
    g_free (caps_str);
  } else {
    SET_ERROR (graph->error, GST_PARSE_ERROR_LINK,
        _("could not link %s to %s"), GST_ELEMENT_NAME (src),
        GST_ELEMENT_NAME (sink));
  }
  gst_parse_free_link (link);
  return -1;
}


static int yyerror (void *scanner, graph_t *graph, const char *s);

#line 825 "grammar.tab.c"

# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_PRIV_GST_PARSE_YY_GRAMMAR_TAB_H_INCLUDED
# define YY_PRIV_GST_PARSE_YY_GRAMMAR_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int priv_gst_parse_yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    PARSE_URL = 258,
    IDENTIFIER = 259,
    REF = 260,
    PADREF = 261,
    BINREF = 262,
    ASSIGNMENT = 263,
    LINK = 264,
    LINK_ALL = 265
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 750 "../../../git/gst/parse/grammar.y"

    gchar *ss;
    chain_t *cc;
    link_t *ll;
    reference_t rr;
    GstElement *ee;
    GSList *pp;
    graph_t *gg;

#line 889 "grammar.tab.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int priv_gst_parse_yyparse (void *scanner, graph_t *graph);

#endif /* !YY_PRIV_GST_PARSE_YY_GRAMMAR_TAB_H_INCLUDED  */



#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  16
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   37

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  18
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  14
/* YYNRULES -- Number of rules.  */
#define YYNRULES  28
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  38

#define YYUNDEFTOK  2
#define YYMAXUTOK   265

/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  ((unsigned) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    15,     2,     2,     2,     2,     2,     2,
      11,    12,     2,     2,    13,     2,    14,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    17,     2,
       2,    16,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   813,   813,   820,   835,   843,   874,   892,   896,   910,
     919,   929,   930,   934,   935,   947,   966,   998,  1010,  1018,
    1049,  1050,  1059,  1071,  1072,  1075,  1076,  1078,  1122
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "PARSE_URL", "IDENTIFIER", "REF",
  "PADREF", "BINREF", "ASSIGNMENT", "LINK", "LINK_ALL", "'('", "')'",
  "','", "'.'", "'!'", "'='", "':'", "$accept", "element", "elementary",
  "chain", "openchain", "link", "pads", "morepads", "reference",
  "chainlist", "assignments", "binopener", "bin", "graph", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,    40,    41,    44,    46,    33,    61,    58
};
# endif

#define YYPACT_NINF -23

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-23)))

#define YYTABLE_NINF -29

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int8 yypact[] =
{
     -23,     0,    24,   -23,   -23,   -23,    14,   -23,   -23,    21,
      22,   -23,    10,   -23,    23,   -23,   -23,    26,   -23,   -23,
      14,   -23,   -23,   -23,    20,    23,   -23,    14,   -23,   -23,
      11,   -23,   -23,     5,   -23,    22,   -23,   -23
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
      20,     0,     0,    22,    16,     2,    13,    26,    25,     4,
      11,    21,     6,    18,    23,     5,     1,     0,    19,     3,
      13,     7,     9,    10,    11,    23,    20,    13,    12,    15,
       0,    17,    24,     0,    14,    11,    27,     8
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -23,   -23,     2,   -23,   -23,   -23,   -22,    -6,     9,     8,
      12,   -23,   -23,   -23
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     9,    10,    11,    12,    24,    21,    18,    13,     1,
      26,    14,    15,     2
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
     -28,     3,    30,     4,     5,     6,     3,     7,     4,     5,
       6,     8,     7,    37,    28,     5,     8,    36,     7,    22,
      23,    34,     8,    29,    16,     6,    20,    17,    20,    19,
      27,    25,    35,    31,    33,     0,     0,    32
};

static const yytype_int8 yycheck[] =
{
       0,     1,    24,     3,     4,     5,     1,     7,     3,     4,
       5,    11,     7,    35,    20,     4,    11,    12,     7,     9,
      10,    27,    11,     3,     0,     5,     6,    13,     6,     8,
       4,     8,    30,    24,    26,    -1,    -1,    25
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    27,    31,     1,     3,     4,     5,     7,    11,    19,
      20,    21,    22,    26,    29,    30,     0,    13,    25,     8,
       6,    24,     9,    10,    23,     8,    28,     4,    25,     3,
      24,    26,    28,    27,    25,    20,    12,    24
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    18,    19,    19,    20,    20,    21,    22,    22,    23,
      23,    24,    24,    25,    25,    21,    22,    21,    22,    26,
      27,    27,    27,    28,    28,    29,    29,    30,    31
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     1,     1,     1,     2,     5,     1,
       1,     0,     2,     0,     3,     3,     1,     3,     1,     2,
       0,     2,     2,     0,     2,     1,     1,     4,     1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (scanner, graph, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, scanner, graph); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep, void *scanner, graph_t *graph)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  YYUSE (scanner);
  YYUSE (graph);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep, void *scanner, graph_t *graph)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyo, yytype, yyvaluep, scanner, graph);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule, void *scanner, graph_t *graph)
{
  unsigned long yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                                              , scanner, graph);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule, scanner, graph); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return (YYSIZE_T) (yystpcpy (yyres, yystr) - yyres);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, void *scanner, graph_t *graph)
{
  YYUSE (yyvaluep);
  YYUSE (scanner);
  YYUSE (graph);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  switch (yytype)
    {
    case 3: /* PARSE_URL  */
#line 778 "../../../git/gst/parse/grammar.y"
      {	gst_parse_strfree (((*yyvaluep).ss));		}
#line 1745 "grammar.tab.c"
        break;

    case 4: /* IDENTIFIER  */
#line 778 "../../../git/gst/parse/grammar.y"
      {	gst_parse_strfree (((*yyvaluep).ss));		}
#line 1751 "grammar.tab.c"
        break;

    case 5: /* REF  */
#line 778 "../../../git/gst/parse/grammar.y"
      {	gst_parse_strfree (((*yyvaluep).ss));		}
#line 1757 "grammar.tab.c"
        break;

    case 6: /* PADREF  */
#line 778 "../../../git/gst/parse/grammar.y"
      {	gst_parse_strfree (((*yyvaluep).ss));		}
#line 1763 "grammar.tab.c"
        break;

    case 7: /* BINREF  */
#line 778 "../../../git/gst/parse/grammar.y"
      {	gst_parse_strfree (((*yyvaluep).ss));		}
#line 1769 "grammar.tab.c"
        break;

    case 8: /* ASSIGNMENT  */
#line 778 "../../../git/gst/parse/grammar.y"
      {	gst_parse_strfree (((*yyvaluep).ss));		}
#line 1775 "grammar.tab.c"
        break;

    case 9: /* LINK  */
#line 778 "../../../git/gst/parse/grammar.y"
      {	gst_parse_strfree (((*yyvaluep).ss));		}
#line 1781 "grammar.tab.c"
        break;

    case 10: /* LINK_ALL  */
#line 778 "../../../git/gst/parse/grammar.y"
      {	gst_parse_strfree (((*yyvaluep).ss));		}
#line 1787 "grammar.tab.c"
        break;

    case 19: /* element  */
#line 783 "../../../git/gst/parse/grammar.y"
      {	gst_object_unref (((*yyvaluep).ee));		}
#line 1793 "grammar.tab.c"
        break;

    case 20: /* elementary  */
#line 779 "../../../git/gst/parse/grammar.y"
      {	if(((*yyvaluep).cc))
		  gst_parse_free_chain(((*yyvaluep).cc));	}
#line 1800 "grammar.tab.c"
        break;

    case 21: /* chain  */
#line 779 "../../../git/gst/parse/grammar.y"
      {	if(((*yyvaluep).cc))
		  gst_parse_free_chain(((*yyvaluep).cc));	}
#line 1807 "grammar.tab.c"
        break;

    case 22: /* openchain  */
#line 779 "../../../git/gst/parse/grammar.y"
      {	if(((*yyvaluep).cc))
		  gst_parse_free_chain(((*yyvaluep).cc));	}
#line 1814 "grammar.tab.c"
        break;

    case 23: /* link  */
#line 781 "../../../git/gst/parse/grammar.y"
      {	gst_parse_free_link (((*yyvaluep).ll));	}
#line 1820 "grammar.tab.c"
        break;

    case 24: /* pads  */
#line 784 "../../../git/gst/parse/grammar.y"
      {	GSList *walk;
		for(walk=((*yyvaluep).pp);walk;walk=walk->next)
		  gst_parse_strfree (walk->data);
		g_slist_free (((*yyvaluep).pp));		}
#line 1829 "grammar.tab.c"
        break;

    case 25: /* morepads  */
#line 784 "../../../git/gst/parse/grammar.y"
      {	GSList *walk;
		for(walk=((*yyvaluep).pp);walk;walk=walk->next)
		  gst_parse_strfree (walk->data);
		g_slist_free (((*yyvaluep).pp));		}
#line 1838 "grammar.tab.c"
        break;

    case 26: /* reference  */
#line 782 "../../../git/gst/parse/grammar.y"
      {	gst_parse_free_reference(&(((*yyvaluep).rr)));}
#line 1844 "grammar.tab.c"
        break;

    case 27: /* chainlist  */
#line 779 "../../../git/gst/parse/grammar.y"
      {	if(((*yyvaluep).cc))
		  gst_parse_free_chain(((*yyvaluep).cc));	}
#line 1851 "grammar.tab.c"
        break;

    case 28: /* assignments  */
#line 784 "../../../git/gst/parse/grammar.y"
      {	GSList *walk;
		for(walk=((*yyvaluep).pp);walk;walk=walk->next)
		  gst_parse_strfree (walk->data);
		g_slist_free (((*yyvaluep).pp));		}
#line 1860 "grammar.tab.c"
        break;

    case 29: /* binopener  */
#line 778 "../../../git/gst/parse/grammar.y"
      {	gst_parse_strfree (((*yyvaluep).ss));		}
#line 1866 "grammar.tab.c"
        break;

    case 30: /* bin  */
#line 779 "../../../git/gst/parse/grammar.y"
      {	if(((*yyvaluep).cc))
		  gst_parse_free_chain(((*yyvaluep).cc));	}
#line 1873 "grammar.tab.c"
        break;

      default:
        break;
    }
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void *scanner, graph_t *graph)
{
/* The lookahead symbol.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yynewstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  *yyssp = (yytype_int16) yystate;

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = (YYSIZE_T) (yyssp - yyss + 1);

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
# undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex (&yylval, scanner);
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2:
#line 813 "../../../git/gst/parse/grammar.y"
    { (yyval.ee) = gst_element_factory_make ((yyvsp[0].ss), NULL);
						if ((yyval.ee) == NULL) {
						  add_missing_element(graph, (yyvsp[0].ss));
						  SET_ERROR (graph->error, GST_PARSE_ERROR_NO_SUCH_ELEMENT, _("no element \"%s\""), (yyvsp[0].ss));
						}
						gst_parse_strfree ((yyvsp[0].ss));
                                              }
#line 2149 "grammar.tab.c"
    break;

  case 3:
#line 820 "../../../git/gst/parse/grammar.y"
    { gst_parse_element_set ((yyvsp[0].ss), (yyvsp[-1].ee), graph);
						(yyval.ee) = (yyvsp[-1].ee);
	                                      }
#line 2157 "grammar.tab.c"
    break;

  case 4:
#line 835 "../../../git/gst/parse/grammar.y"
    { (yyval.cc) = gst_parse_chain_new ();
						/* g_print ("@%p: CHAINing elementary\n", $$); */
						(yyval.cc)->first.element = (yyvsp[0].ee)? gst_object_ref((yyvsp[0].ee)) : NULL;
						(yyval.cc)->last.element = (yyvsp[0].ee)? gst_object_ref((yyvsp[0].ee)) : NULL;
						(yyval.cc)->first.name = (yyval.cc)->last.name = NULL;
						(yyval.cc)->first.pads = (yyval.cc)->last.pads = NULL;
						(yyval.cc)->elements = (yyvsp[0].ee) ? g_slist_prepend (NULL, (yyvsp[0].ee)) : NULL;
					      }
#line 2170 "grammar.tab.c"
    break;

  case 5:
#line 843 "../../../git/gst/parse/grammar.y"
    { (yyval.cc)=(yyvsp[0].cc); }
#line 2176 "grammar.tab.c"
    break;

  case 6:
#line 874 "../../../git/gst/parse/grammar.y"
    { (yyval.cc)=(yyvsp[0].cc);
						if((yyval.cc)->last.name){
							SET_ERROR (graph->error, GST_PARSE_ERROR_SYNTAX,
							_("unexpected reference \"%s\" - ignoring"), (yyval.cc)->last.name);
							gst_parse_strfree((yyval.cc)->last.name);
							(yyval.cc)->last.name=NULL;
						}
						if((yyval.cc)->last.pads){
							SET_ERROR (graph->error, GST_PARSE_ERROR_SYNTAX,
							_("unexpected pad-reference \"%s\" - ignoring"), (gchar*)(yyval.cc)->last.pads->data);
							g_slist_foreach ((yyval.cc)->last.pads, (GFunc) gst_parse_strfree, NULL);
							g_slist_free ((yyval.cc)->last.pads);
							(yyval.cc)->last.pads=NULL;
						}
					      }
#line 2196 "grammar.tab.c"
    break;

  case 7:
#line 892 "../../../git/gst/parse/grammar.y"
    { (yyval.cc)=(yyvsp[-1].cc);
						(yyval.cc)->last.pads = g_slist_concat ((yyval.cc)->last.pads, (yyvsp[0].pp));
						/* g_print ("@%p@%p: FKI elementary pads\n", $1, $$->last.pads); */
					      }
#line 2205 "grammar.tab.c"
    break;

  case 8:
#line 897 "../../../git/gst/parse/grammar.y"
    {
						(yyvsp[-3].ll)->src  = (yyvsp[-4].cc)->last;
						(yyvsp[-3].ll)->sink = (yyvsp[-1].cc)->first;
						(yyvsp[-3].ll)->sink.pads = g_slist_concat ((yyvsp[-2].pp), (yyvsp[-3].ll)->sink.pads);
						TRY_SETUP_LINK((yyvsp[-3].ll));
						(yyvsp[-1].cc)->first = (yyvsp[-4].cc)->first;
						(yyvsp[-1].cc)->elements = g_slist_concat ((yyvsp[-4].cc)->elements, (yyvsp[-1].cc)->elements);
						gst_parse_chain_free((yyvsp[-4].cc));
						(yyval.cc) = (yyvsp[-1].cc);
						(yyval.cc)->last.pads = g_slist_concat ((yyval.cc)->last.pads, (yyvsp[0].pp));
					      }
#line 2221 "grammar.tab.c"
    break;

  case 9:
#line 910 "../../../git/gst/parse/grammar.y"
    { (yyval.ll) = gst_parse_link_new ();
						(yyval.ll)->all_pads = FALSE;
						if ((yyvsp[0].ss)) {
						  (yyval.ll)->caps = gst_caps_from_string ((yyvsp[0].ss));
						  if ((yyval.ll)->caps == NULL)
						    SET_ERROR (graph->error, GST_PARSE_ERROR_LINK, _("could not parse caps \"%s\""), (yyvsp[0].ss));
						  gst_parse_strfree ((yyvsp[0].ss));
						}
					      }
#line 2235 "grammar.tab.c"
    break;

  case 10:
#line 919 "../../../git/gst/parse/grammar.y"
    { (yyval.ll) = gst_parse_link_new ();
						(yyval.ll)->all_pads = TRUE;
						if ((yyvsp[0].ss)) {
						  (yyval.ll)->caps = gst_caps_from_string ((yyvsp[0].ss));
						  if ((yyval.ll)->caps == NULL)
						    SET_ERROR (graph->error, GST_PARSE_ERROR_LINK, _("could not parse caps \"%s\""), (yyvsp[0].ss));
						  gst_parse_strfree ((yyvsp[0].ss));
						}
					      }
#line 2249 "grammar.tab.c"
    break;

  case 11:
#line 929 "../../../git/gst/parse/grammar.y"
    { (yyval.pp) = NULL; }
#line 2255 "grammar.tab.c"
    break;

  case 12:
#line 930 "../../../git/gst/parse/grammar.y"
    { (yyval.pp) = (yyvsp[0].pp);
						(yyval.pp) = g_slist_prepend ((yyval.pp), (yyvsp[-1].ss));
					      }
#line 2263 "grammar.tab.c"
    break;

  case 13:
#line 934 "../../../git/gst/parse/grammar.y"
    { (yyval.pp) = NULL; }
#line 2269 "grammar.tab.c"
    break;

  case 14:
#line 935 "../../../git/gst/parse/grammar.y"
    { (yyval.pp) = g_slist_prepend ((yyvsp[0].pp), (yyvsp[-1].ss)); }
#line 2275 "grammar.tab.c"
    break;

  case 15:
#line 947 "../../../git/gst/parse/grammar.y"
    { GstElement *element =
							  gst_element_make_from_uri (GST_URI_SINK, (yyvsp[0].ss), NULL, NULL);
						/* FIXME: get and parse error properly */
						if (!element) {
						  SET_ERROR (graph->error, GST_PARSE_ERROR_NO_SUCH_ELEMENT,
							  _("no sink element for URI \"%s\""), (yyvsp[0].ss));
						}
						(yyval.cc) = (yyvsp[-2].cc);
						(yyvsp[-1].ll)->sink.element = element?gst_object_ref(element):NULL;
						(yyvsp[-1].ll)->src = (yyvsp[-2].cc)->last;
						TRY_SETUP_LINK((yyvsp[-1].ll));
						(yyval.cc)->last.element = NULL;
						(yyval.cc)->last.name = NULL;
						(yyval.cc)->last.pads = NULL;
						if(element) (yyval.cc)->elements = g_slist_append ((yyval.cc)->elements, element);
						g_free ((yyvsp[0].ss));
					      }
#line 2297 "grammar.tab.c"
    break;

  case 16:
#line 966 "../../../git/gst/parse/grammar.y"
    { GstElement *element =
							  gst_element_make_from_uri (GST_URI_SRC, (yyvsp[0].ss), NULL, NULL);
						/* FIXME: get and parse error properly */
						if (!element) {
						  SET_ERROR (graph->error, GST_PARSE_ERROR_NO_SUCH_ELEMENT,
						    _("no source element for URI \"%s\""), (yyvsp[0].ss));
						}
						(yyval.cc) = gst_parse_chain_new ();
						/* g_print ("@%p: CHAINing srcURL\n", $$); */
						(yyval.cc)->first.element = NULL;
						(yyval.cc)->first.name = NULL;
						(yyval.cc)->first.pads = NULL;
						(yyval.cc)->last.element = element ? gst_object_ref(element):NULL;
						(yyval.cc)->last.name = NULL;
						(yyval.cc)->last.pads = NULL;
						(yyval.cc)->elements = element ? g_slist_prepend (NULL, element)  : NULL;
						g_free((yyvsp[0].ss));
					      }
#line 2320 "grammar.tab.c"
    break;

  case 17:
#line 998 "../../../git/gst/parse/grammar.y"
    { (yyval.cc) = (yyvsp[-2].cc);
						(yyvsp[-1].ll)->sink= (yyvsp[0].rr);
						(yyvsp[-1].ll)->src = (yyvsp[-2].cc)->last;
						TRY_SETUP_LINK((yyvsp[-1].ll));
						(yyval.cc)->last.element = NULL;
						(yyval.cc)->last.name = NULL;
						(yyval.cc)->last.pads = NULL;
					      }
#line 2333 "grammar.tab.c"
    break;

  case 18:
#line 1010 "../../../git/gst/parse/grammar.y"
    { (yyval.cc) = gst_parse_chain_new ();
						(yyval.cc)->last=(yyvsp[0].rr);
						(yyval.cc)->first.element = NULL;
						(yyval.cc)->first.name = NULL;
						(yyval.cc)->first.pads = NULL;
						(yyval.cc)->elements = NULL;
					      }
#line 2345 "grammar.tab.c"
    break;

  case 19:
#line 1018 "../../../git/gst/parse/grammar.y"
    {
						gchar *padname = (yyvsp[-1].ss);
						GSList *pads = (yyvsp[0].pp);
						if (padname) {
						  while (*padname != '.') padname++;
						  *padname = '\0';
						  padname++;
						  if (*padname != '\0')
						    pads = g_slist_prepend (pads, gst_parse_strdup (padname));
						}
						(yyval.rr).element=NULL;
						(yyval.rr).name=(yyvsp[-1].ss);
						(yyval.rr).pads=pads;
					      }
#line 2364 "grammar.tab.c"
    break;

  case 20:
#line 1049 "../../../git/gst/parse/grammar.y"
    { (yyval.cc) = NULL; }
#line 2370 "grammar.tab.c"
    break;

  case 21:
#line 1050 "../../../git/gst/parse/grammar.y"
    { if ((yyvsp[-1].cc)){
						  gst_parse_free_reference(&((yyvsp[-1].cc)->last));
						  gst_parse_free_reference(&((yyvsp[0].cc)->first));
						  (yyvsp[0].cc)->first = (yyvsp[-1].cc)->first;
						  (yyvsp[0].cc)->elements = g_slist_concat ((yyvsp[-1].cc)->elements, (yyvsp[0].cc)->elements);
						  gst_parse_chain_free ((yyvsp[-1].cc));
						}
						(yyval.cc) = (yyvsp[0].cc);
					      }
#line 2384 "grammar.tab.c"
    break;

  case 22:
#line 1059 "../../../git/gst/parse/grammar.y"
    { (yyval.cc)=(yyvsp[-1].cc);
						GST_CAT_DEBUG (GST_CAT_PIPELINE,"trying to recover from syntax error");
						SET_ERROR (graph->error, GST_PARSE_ERROR_SYNTAX, _("syntax error"));
					      }
#line 2393 "grammar.tab.c"
    break;

  case 23:
#line 1071 "../../../git/gst/parse/grammar.y"
    { (yyval.pp) = NULL; }
#line 2399 "grammar.tab.c"
    break;

  case 24:
#line 1072 "../../../git/gst/parse/grammar.y"
    { (yyval.pp) = g_slist_prepend ((yyvsp[0].pp), (yyvsp[-1].ss)); }
#line 2405 "grammar.tab.c"
    break;

  case 25:
#line 1075 "../../../git/gst/parse/grammar.y"
    { (yyval.ss) = gst_parse_strdup("bin"); }
#line 2411 "grammar.tab.c"
    break;

  case 26:
#line 1076 "../../../git/gst/parse/grammar.y"
    { (yyval.ss) = (yyvsp[0].ss); }
#line 2417 "grammar.tab.c"
    break;

  case 27:
#line 1078 "../../../git/gst/parse/grammar.y"
    {
						chain_t *chain = (yyvsp[-1].cc);
						GSList *walk;
						GstBin *bin = (GstBin *) gst_element_factory_make ((yyvsp[-3].ss), NULL);
						if (!chain) {
						  SET_ERROR (graph->error, GST_PARSE_ERROR_EMPTY_BIN,
						    _("specified empty bin \"%s\", not allowed"), (yyvsp[-3].ss));
						  chain = gst_parse_chain_new ();
						  chain->first.element = chain->last.element = NULL;
						  chain->first.name    = chain->last.name    = NULL;
						  chain->first.pads    = chain->last.pads    = NULL;
						  chain->elements = NULL;
						}
						if (!bin) {
						  add_missing_element(graph, (yyvsp[-3].ss));
						  SET_ERROR (graph->error, GST_PARSE_ERROR_NO_SUCH_ELEMENT,
						    _("no bin \"%s\", unpacking elements"), (yyvsp[-3].ss));
						  /* clear property-list */
						  g_slist_foreach ((yyvsp[-2].pp), (GFunc) gst_parse_strfree, NULL);
						  g_slist_free ((yyvsp[-2].pp));
						  (yyvsp[-2].pp) = NULL;
						} else {
						  for (walk = chain->elements; walk; walk = walk->next )
						    gst_bin_add (bin, GST_ELEMENT (walk->data));
						  g_slist_free (chain->elements);
						  chain->elements = g_slist_prepend (NULL, bin);
						}
						(yyval.cc) = chain;
						/* set the properties now
						 * HINT: property-list cleared above, if bin==NULL
						 */
						for (walk = (yyvsp[-2].pp); walk; walk = walk->next)
						  gst_parse_element_set ((gchar *) walk->data,
							GST_ELEMENT (bin), graph);
						g_slist_free ((yyvsp[-2].pp));
						gst_parse_strfree ((yyvsp[-3].ss));
					      }
#line 2459 "grammar.tab.c"
    break;

  case 28:
#line 1122 "../../../git/gst/parse/grammar.y"
    { (yyval.gg) = graph;
						(yyval.gg)->chain = (yyvsp[0].cc);
						if(!(yyvsp[0].cc)) {
						  SET_ERROR (graph->error, GST_PARSE_ERROR_EMPTY, _("empty pipeline not allowed"));
						}
					      }
#line 2470 "grammar.tab.c"
    break;


#line 2474 "grammar.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (scanner, graph, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (scanner, graph, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, scanner, graph);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, scanner, graph);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (scanner, graph, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, scanner, graph);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, scanner, graph);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 1130 "../../../git/gst/parse/grammar.y"



static int
yyerror (void *scanner, graph_t *graph, const char *s)
{
  /* FIXME: This should go into the GError somehow, but how? */
  GST_WARNING ("Error during parsing: %s", s);
  return -1;
}


GstElement *
priv_gst_parse_launch (const gchar *str, GError **error, GstParseContext *ctx,
    GstParseFlags flags)
{
  graph_t g;
  gchar *dstr;
  GSList *walk;
  GstElement *ret;
  yyscan_t scanner;

  g_return_val_if_fail (str != NULL, NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  g.chain = NULL;
  g.links = NULL;
  g.error = error;
  g.ctx = ctx;
  g.flags = flags;

#ifdef __GST_PARSE_TRACE
  GST_CAT_DEBUG (GST_CAT_PIPELINE, "TRACE: tracing enabled");
  __strings = __chains = __links = 0;
#endif /* __GST_PARSE_TRACE */

  /* g_print("Now scanning: %s\n", str); */

  dstr = g_strdup (str);
  priv_gst_parse_yylex_init (&scanner);
  priv_gst_parse_yy_scan_string (dstr, scanner);

#if YYDEBUG
  yydebug = 1;
#endif

  if (yyparse (scanner, &g) != 0) {
    SET_ERROR (error, GST_PARSE_ERROR_SYNTAX,
        "Unrecoverable syntax error while parsing pipeline %s", str);

    priv_gst_parse_yylex_destroy (scanner);
    g_free (dstr);

    goto error1;
  }
  priv_gst_parse_yylex_destroy (scanner);
  g_free (dstr);

  GST_CAT_DEBUG (GST_CAT_PIPELINE, "got %u elements and %u links",
      g.chain ? g_slist_length (g.chain->elements) : 0,
      g_slist_length (g.links));

  /* ensure chain is not NULL */
  if (!g.chain){
    g.chain=gst_parse_chain_new ();
    g.chain->elements=NULL;
    g.chain->first.element=NULL;
    g.chain->first.name=NULL;
    g.chain->first.pads=NULL;
    g.chain->last.element=NULL;
    g.chain->last.name=NULL;
    g.chain->last.pads=NULL;
  };

  /* ensure elements is not empty */
  if(!g.chain->elements){
    g.chain->elements= g_slist_prepend (NULL, NULL);
  };

  /* put all elements in our bin if necessary */
  if(g.chain->elements->next){
    GstBin *bin;
    if (flags & GST_PARSE_FLAG_PLACE_IN_BIN)
      bin = GST_BIN (gst_element_factory_make ("bin", NULL));
    else
      bin = GST_BIN (gst_element_factory_make ("pipeline", NULL));
    g_assert (bin);

    for (walk = g.chain->elements; walk; walk = walk->next) {
      if (walk->data != NULL)
        gst_bin_add (bin, GST_ELEMENT (walk->data));
    }
    g_slist_free (g.chain->elements);
    g.chain->elements = g_slist_prepend (NULL, bin);
  }

  ret = (GstElement *) g.chain->elements->data;
  g_slist_free (g.chain->elements);
  g.chain->elements=NULL;
  gst_parse_free_chain (g.chain);
  g.chain = NULL;


  /* resolve and perform links */
  for (walk = g.links; walk; walk = walk->next) {
    link_t *l = (link_t *) walk->data;
    int err;
    err=gst_resolve_reference( &(l->src), ret);
    if (err) {
       if(-1==err){
          SET_ERROR (error, GST_PARSE_ERROR_NO_SUCH_ELEMENT,
              "No src-element named \"%s\" - omitting link", l->src.name);
       }else{
          /* probably a missing element which we've handled already */
          SET_ERROR (error, GST_PARSE_ERROR_NO_SUCH_ELEMENT,
              "No src-element found - omitting link");
       }
       gst_parse_free_link (l);
       continue;
    }

    err=gst_resolve_reference( &(l->sink), ret);
    if (err) {
       if(-1==err){
          SET_ERROR (error, GST_PARSE_ERROR_NO_SUCH_ELEMENT,
              "No sink-element named \"%s\" - omitting link", l->src.name);
       }else{
          /* probably a missing element which we've handled already */
          SET_ERROR (error, GST_PARSE_ERROR_NO_SUCH_ELEMENT,
              "No sink-element found - omitting link");
       }
       gst_parse_free_link (l);
       continue;
    }
    gst_parse_perform_link (l, &g);
  }
  g_slist_free (g.links);

out:
#ifdef __GST_PARSE_TRACE
  GST_CAT_DEBUG (GST_CAT_PIPELINE,
      "TRACE: %u strings, %u chains and %u links left", __strings, __chains,
      __links);
  if (__strings || __chains || __links) {
    g_warning ("TRACE: %u strings, %u chains and %u links left", __strings,
        __chains, __links);
  }
#endif /* __GST_PARSE_TRACE */

  return ret;

error1:
  if (g.chain) {
    gst_parse_free_chain (g.chain);
    g.chain=NULL;
  }

  g_slist_foreach (g.links, (GFunc)gst_parse_free_link, NULL);
  g_slist_free (g.links);

  if (error)
    g_assert (*error);
  ret = NULL;

  goto out;
}
