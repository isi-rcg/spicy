/*
 * Copyright (C) 2003 Benjamin Otte <in7y118@public.uni-hamburg.de>
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

#include <gst/check/gstcheck.h>

#include <string.h>

/* multiple artists are possible (unfixed) */
#define UTAG GST_TAG_ARTIST
#define UNFIXED1 "Britney Spears"
#define UNFIXED2 "Evanescence"
#define UNFIXED3 "AC/DC"
#define UNFIXED4 "The Prodigy"

/* license is fixed */
#define FTAG GST_TAG_LICENSE
#define FIXED1 "Lesser General Public License"
#define FIXED2 "Microsoft End User License Agreement"
#define FIXED3 "Mozilla Public License"
#define FIXED4 "Public Domain"

/* checks that a tag contains the given values and not more values */
static void
check_tags (const GstTagList * list, const gchar * tag, const gchar * value,
    ...)
{
  va_list args;
  gchar *str;
  guint i = 0;

  va_start (args, value);
  while (value != NULL) {
    fail_unless (gst_tag_list_get_string_index (list, tag, i, &str));
    fail_unless (strcmp (value, str) == 0);
    g_free (str);

    value = va_arg (args, gchar *);
    i++;
  }
  fail_unless (i == gst_tag_list_get_tag_size (list, tag));
  va_end (args);
}

static void
check_tags_empty (const GstTagList * list)
{
  GST_DEBUG ("taglist: %" GST_PTR_FORMAT, list);
  fail_unless ((list == NULL) || (gst_tag_list_is_empty (list)));
}

#define NEW_LIST_FIXED(mode)                                    \
G_STMT_START {                                                  \
  if (list) gst_tag_list_unref (list);                          \
  list = gst_tag_list_new_empty ();                             \
  gst_tag_list_add (list, mode, FTAG, FIXED1, FTAG, FIXED2,     \
                    FTAG, FIXED3, FTAG, FIXED4, NULL);          \
  mark_point();                                                 \
} G_STMT_END;

#define NEW_LIST_UNFIXED(mode)                                  \
G_STMT_START {                                                  \
  if (list) gst_tag_list_unref (list);                          \
  list = gst_tag_list_new_empty ();                             \
  gst_tag_list_add (list, mode, UTAG, UNFIXED1, UTAG, UNFIXED2, \
                    UTAG, UNFIXED3, UTAG, UNFIXED4, NULL);      \
  mark_point();                                                 \
} G_STMT_END;

#define NEW_LISTS_FIXED(mode)                                   \
G_STMT_START {                                                  \
  if (list) gst_tag_list_unref (list);                          \
  list = gst_tag_list_new_empty ();                             \
  gst_tag_list_add (list, GST_TAG_MERGE_APPEND, FTAG, FIXED1,   \
                    FTAG, FIXED2, NULL);                        \
  if (list2) gst_tag_list_unref (list2);                        \
  list2 = gst_tag_list_new_empty ();                            \
  gst_tag_list_add (list2, GST_TAG_MERGE_APPEND, FTAG, FIXED3,  \
                    FTAG, FIXED4, NULL);                        \
  if (merge) gst_tag_list_unref (merge);                        \
  merge = gst_tag_list_merge (list, list2, mode);               \
  mark_point();                                                 \
} G_STMT_END;

#define NEW_LISTS_UNFIXED(mode)                                 \
G_STMT_START {                                                  \
  if (list) gst_tag_list_unref (list);                          \
  list = gst_tag_list_new_empty ();                             \
  gst_tag_list_add (list, GST_TAG_MERGE_APPEND, UTAG, UNFIXED1, \
                    UTAG, UNFIXED2, NULL);                      \
  if (list2) gst_tag_list_unref (list2);                        \
  list2 = gst_tag_list_new_empty ();                            \
  gst_tag_list_add (list2, GST_TAG_MERGE_APPEND, UTAG, UNFIXED3,\
                    UTAG, UNFIXED4, NULL);                      \
  if (merge) gst_tag_list_unref (merge);                        \
  merge = gst_tag_list_merge (list, list2, mode);               \
  mark_point();                                                 \
} G_STMT_END;

#define NEW_LISTS_EMPTY1(mode)                                  \
G_STMT_START {                                                  \
  if (list) gst_tag_list_unref (list);                          \
  list = NULL;                                                  \
  if (list2) gst_tag_list_unref (list2);                        \
  list2 = gst_tag_list_new_empty ();                            \
  gst_tag_list_add (list2, GST_TAG_MERGE_APPEND, FTAG, FIXED3,  \
                    FTAG, FIXED4, NULL);                        \
  if (merge) gst_tag_list_unref (merge);                        \
  merge = gst_tag_list_merge (list, list2, mode);               \
  mark_point();                                                 \
} G_STMT_END;

#define NEW_LISTS_EMPTY2(mode)                                  \
G_STMT_START {                                                  \
  if (list) gst_tag_list_unref (list);                          \
  list = gst_tag_list_new_empty ();                             \
  gst_tag_list_add (list, GST_TAG_MERGE_APPEND, FTAG, FIXED1,   \
                    FTAG, FIXED2, NULL);                        \
  if (list2) gst_tag_list_unref (list2);                        \
  list2 = NULL;                                                 \
  if (merge) gst_tag_list_unref (merge);                        \
  merge = gst_tag_list_merge (list, list2, mode);               \
  mark_point();                                                 \
} G_STMT_END;


GST_START_TEST (test_basics)
{
  /* make sure the assumptions work */
  fail_unless (gst_tag_is_fixed (FTAG));
  fail_unless (!gst_tag_is_fixed (UTAG));
  /* we check string here only */
  fail_unless (gst_tag_get_type (FTAG) == G_TYPE_STRING);
  fail_unless (gst_tag_get_type (UTAG) == G_TYPE_STRING);
}

GST_END_TEST
GST_START_TEST (test_add)
{
  GstTagList *list = NULL;

  /* check additions */
  /* unfixed */
  NEW_LIST_UNFIXED (GST_TAG_MERGE_REPLACE_ALL);
  check_tags (list, UTAG, UNFIXED4, NULL);
  NEW_LIST_UNFIXED (GST_TAG_MERGE_REPLACE);
  check_tags (list, UTAG, UNFIXED4, NULL);
  NEW_LIST_UNFIXED (GST_TAG_MERGE_PREPEND);
  check_tags (list, UTAG, UNFIXED4, UNFIXED3, UNFIXED2, UNFIXED1, NULL);
  NEW_LIST_UNFIXED (GST_TAG_MERGE_APPEND);
  check_tags (list, UTAG, UNFIXED1, UNFIXED2, UNFIXED3, UNFIXED4, NULL);
  NEW_LIST_UNFIXED (GST_TAG_MERGE_KEEP);
  check_tags (list, UTAG, UNFIXED1, NULL);
  NEW_LIST_UNFIXED (GST_TAG_MERGE_KEEP_ALL);
  check_tags (list, UTAG, NULL);

  /* fixed */
  NEW_LIST_FIXED (GST_TAG_MERGE_REPLACE_ALL);
  check_tags (list, FTAG, FIXED4, NULL);
  NEW_LIST_FIXED (GST_TAG_MERGE_REPLACE);
  check_tags (list, FTAG, FIXED4, NULL);
  NEW_LIST_FIXED (GST_TAG_MERGE_PREPEND);
  check_tags (list, FTAG, FIXED4, NULL);
  NEW_LIST_FIXED (GST_TAG_MERGE_APPEND);
  check_tags (list, FTAG, FIXED1, NULL);
  NEW_LIST_FIXED (GST_TAG_MERGE_KEEP);
  check_tags (list, FTAG, FIXED1, NULL);
  NEW_LIST_FIXED (GST_TAG_MERGE_KEEP_ALL);
  check_tags (list, FTAG, NULL);

  /* clean up */
  if (list)
    gst_tag_list_unref (list);
}

GST_END_TEST
GST_START_TEST (test_merge)
{
  GstTagList *list = NULL, *list2 = NULL, *merge = NULL;

  /* check merging */
  /* unfixed */
  GST_DEBUG ("unfixed");
  NEW_LISTS_UNFIXED (GST_TAG_MERGE_REPLACE_ALL);
  check_tags (merge, UTAG, UNFIXED3, UNFIXED4, NULL);
  NEW_LISTS_UNFIXED (GST_TAG_MERGE_REPLACE);
  check_tags (merge, UTAG, UNFIXED3, UNFIXED4, NULL);
  NEW_LISTS_UNFIXED (GST_TAG_MERGE_PREPEND);
  check_tags (merge, UTAG, UNFIXED3, UNFIXED4, UNFIXED1, UNFIXED2, NULL);
  NEW_LISTS_UNFIXED (GST_TAG_MERGE_APPEND);
  check_tags (merge, UTAG, UNFIXED1, UNFIXED2, UNFIXED3, UNFIXED4, NULL);
  NEW_LISTS_UNFIXED (GST_TAG_MERGE_KEEP);
  check_tags (merge, UTAG, UNFIXED1, UNFIXED2, NULL);
  NEW_LISTS_UNFIXED (GST_TAG_MERGE_KEEP_ALL);
  check_tags (merge, UTAG, UNFIXED1, UNFIXED2, NULL);

  /* fixed */
  GST_DEBUG ("fixed");
  NEW_LISTS_FIXED (GST_TAG_MERGE_REPLACE_ALL);
  check_tags (merge, FTAG, FIXED3, NULL);
  NEW_LISTS_FIXED (GST_TAG_MERGE_REPLACE);
  check_tags (merge, FTAG, FIXED3, NULL);
  NEW_LISTS_FIXED (GST_TAG_MERGE_PREPEND);
  check_tags (merge, FTAG, FIXED3, NULL);
  NEW_LISTS_FIXED (GST_TAG_MERGE_APPEND);
  check_tags (merge, FTAG, FIXED1, NULL);
  NEW_LISTS_FIXED (GST_TAG_MERGE_KEEP);
  check_tags (merge, FTAG, FIXED1, NULL);
  NEW_LISTS_FIXED (GST_TAG_MERGE_KEEP_ALL);
  check_tags (merge, FTAG, FIXED1, NULL);

  /* first list empty */
  GST_DEBUG ("first empty");
  NEW_LISTS_EMPTY1 (GST_TAG_MERGE_REPLACE_ALL);
  check_tags (merge, FTAG, FIXED3, NULL);
  NEW_LISTS_EMPTY1 (GST_TAG_MERGE_REPLACE);
  check_tags (merge, FTAG, FIXED3, NULL);
  NEW_LISTS_EMPTY1 (GST_TAG_MERGE_PREPEND);
  check_tags (merge, FTAG, FIXED3, NULL);
  NEW_LISTS_EMPTY1 (GST_TAG_MERGE_APPEND);
  check_tags (merge, FTAG, FIXED3, NULL);
  NEW_LISTS_EMPTY1 (GST_TAG_MERGE_KEEP);
  check_tags (merge, FTAG, FIXED3, NULL);
  NEW_LISTS_EMPTY1 (GST_TAG_MERGE_KEEP_ALL);
  check_tags_empty (merge);

  /* second list empty */
  GST_DEBUG ("second empty");
  NEW_LISTS_EMPTY2 (GST_TAG_MERGE_REPLACE_ALL);
  check_tags_empty (merge);
  NEW_LISTS_EMPTY2 (GST_TAG_MERGE_REPLACE);
  check_tags (merge, FTAG, FIXED1, NULL);
  NEW_LISTS_EMPTY2 (GST_TAG_MERGE_PREPEND);
  check_tags (merge, FTAG, FIXED1, NULL);
  NEW_LISTS_EMPTY2 (GST_TAG_MERGE_APPEND);
  check_tags (merge, FTAG, FIXED1, NULL);
  NEW_LISTS_EMPTY2 (GST_TAG_MERGE_KEEP);
  check_tags (merge, FTAG, FIXED1, NULL);
  NEW_LISTS_EMPTY2 (GST_TAG_MERGE_KEEP_ALL);
  check_tags (merge, FTAG, FIXED1, NULL);

  /* clean up */
  if (list)
    gst_tag_list_unref (list);
  if (list2)
    gst_tag_list_unref (list2);
  if (merge)
    gst_tag_list_unref (merge);
}

GST_END_TEST
GST_START_TEST (test_date_tags)
{
  GstTagList *tag_list, *tag_list2;
  GDate *date, *date2;
  gchar *str;

  date = g_date_new_dmy (14, 10, 2005);
  tag_list = gst_tag_list_new_empty ();
  gst_tag_list_add (tag_list, GST_TAG_MERGE_APPEND, GST_TAG_DATE, date, NULL);

  str = gst_tag_list_to_string (tag_list);
  fail_if (str == NULL);
  fail_if (strstr (str, "2005-10-14") == NULL);

  tag_list2 = gst_tag_list_new_from_string (str);
  fail_if (tag_list2 == NULL);
  fail_if (!gst_tag_list_get_date (tag_list2, GST_TAG_DATE, &date2));
  fail_unless (gst_tag_list_is_equal (tag_list2, tag_list));
  gst_tag_list_unref (tag_list2);
  g_free (str);

  fail_if (g_date_compare (date, date2) != 0);
  fail_if (g_date_get_day (date) != 14);
  fail_if (g_date_get_month (date) != 10);
  fail_if (g_date_get_year (date) != 2005);
  fail_if (g_date_get_day (date2) != 14);
  fail_if (g_date_get_month (date2) != 10);
  fail_if (g_date_get_year (date2) != 2005);
  g_date_free (date2);

  gst_tag_list_unref (tag_list);
  g_date_free (date);
}

GST_END_TEST;

GST_START_TEST (test_type)
{
  GstTagList *taglist;

  taglist = gst_tag_list_new_empty ();
  fail_unless (GST_IS_TAG_LIST (taglist));
  gst_tag_list_unref (taglist);

  /* this should be fine */
  fail_if (GST_IS_TAG_LIST (NULL));

  /* check gst_tag_list_is_empty */
  ASSERT_CRITICAL (gst_tag_list_is_empty (NULL));
  taglist = gst_tag_list_new_empty ();
  fail_unless (gst_tag_list_is_empty (taglist));
  gst_tag_list_add (taglist, GST_TAG_MERGE_APPEND, GST_TAG_ARTIST, "JD", NULL);
  fail_if (gst_tag_list_is_empty (taglist));
  gst_tag_list_unref (taglist);
}

GST_END_TEST;

GST_START_TEST (test_set_non_utf8_string)
{
  GstTagList *taglist;
  guint8 foobar[2] = { 0xff, 0x00 };    /* not UTF-8 */

  taglist = gst_tag_list_new_empty ();
  fail_unless (taglist != NULL);

  ASSERT_WARNING (gst_tag_list_add (taglist, GST_TAG_MERGE_APPEND,
          GST_TAG_ARTIST, (gchar *) foobar, NULL));

  /* That string field with a non-UTF8 string should not have been added */
  fail_unless (gst_tag_list_is_empty (taglist));

  gst_tag_list_unref (taglist);
}

GST_END_TEST;

GST_START_TEST (test_buffer_tags)
{
  GstTagList *tags;
  GstBuffer *buf1, *buf2;
  GstSample *s1, *s2;

  tags = gst_tag_list_new_empty ();

  buf1 = gst_buffer_new_and_alloc (222);
  s1 = gst_sample_new (buf1, NULL, NULL, NULL);
  gst_buffer_unref (buf1);

  buf2 = gst_buffer_new_and_alloc (100);
  s2 = gst_sample_new (buf2, NULL, NULL, NULL);
  gst_buffer_unref (buf2);

  gst_tag_list_add (tags, GST_TAG_MERGE_APPEND, GST_TAG_IMAGE, s1,
      GST_TAG_PREVIEW_IMAGE, s2, NULL);

  gst_sample_unref (s1);
  gst_sample_unref (s2);
  s1 = s2 = NULL;

  fail_if (!gst_tag_list_get_sample (tags, GST_TAG_IMAGE, &s1));
  fail_unless (gst_sample_get_buffer (s1) == buf1);
  gst_sample_unref (s1);

  fail_if (!gst_tag_list_get_sample (tags, GST_TAG_PREVIEW_IMAGE, &s2));
  fail_unless (gst_sample_get_buffer (s2) == buf2);
  gst_sample_unref (s2);

  fail_if (gst_tag_list_get_sample_index (tags, GST_TAG_IMAGE, 1, &s1));
  fail_if (gst_tag_list_get_sample_index (tags, GST_TAG_IMAGE, 2, &s1));
  fail_if (gst_tag_list_get_sample_index (tags, GST_TAG_PREVIEW_IMAGE, 1, &s1));
  fail_if (gst_tag_list_get_sample_index (tags, GST_TAG_PREVIEW_IMAGE, 2, &s1));

  fail_if (!gst_tag_list_get_sample_index (tags, GST_TAG_IMAGE, 0, &s1));
  fail_if (!gst_tag_list_get_sample_index (tags, GST_TAG_PREVIEW_IMAGE, 0,
          &s2));
  buf1 = gst_sample_get_buffer (s1);
  fail_unless_equals_int (gst_buffer_get_size (buf1), 222);
  buf2 = gst_sample_get_buffer (s2);
  fail_unless_equals_int (gst_buffer_get_size (buf2), 100);

  gst_sample_unref (s1);
  gst_sample_unref (s2);

  gst_tag_list_unref (tags);
}

GST_END_TEST;

GST_START_TEST (test_empty_tags)
{
  GstTagList *tags;

  /* only get g_warnings() with git */
  if (GST_VERSION_NANO != 1)
    return;

  tags = gst_tag_list_new_empty ();
  ASSERT_WARNING (gst_tag_list_add (tags, GST_TAG_MERGE_APPEND,
          GST_TAG_ARTIST, NULL, NULL));
  ASSERT_WARNING (gst_tag_list_add (tags, GST_TAG_MERGE_APPEND,
          GST_TAG_ARTIST, "", NULL));
  gst_tag_list_add (tags, GST_TAG_MERGE_APPEND, GST_TAG_ARTIST, "xyz", NULL);
  gst_tag_list_unref (tags);
}

GST_END_TEST;

GST_START_TEST (test_new_full)
{
  GstTagList *tags;
  gchar *artist, *title;
  gdouble track_gain;
  guint track_num;

  tags = gst_tag_list_new (GST_TAG_ARTIST, "Arty Ist",
      GST_TAG_TRACK_NUMBER, 9, GST_TAG_TRACK_GAIN, 4.242, GST_TAG_TITLE,
      "Title!", NULL);

  fail_unless (gst_tag_list_get_string (tags, GST_TAG_ARTIST, &artist));
  fail_unless_equals_string (artist, "Arty Ist");
  fail_unless (gst_tag_list_get_string (tags, GST_TAG_TITLE, &title));
  fail_unless_equals_string (title, "Title!");
  fail_unless (gst_tag_list_get_uint (tags, GST_TAG_TRACK_NUMBER, &track_num));
  fail_unless_equals_int (track_num, 9);
  fail_unless (gst_tag_list_get_double (tags, GST_TAG_TRACK_GAIN, &track_gain));
  fail_unless_equals_float (track_gain, 4.242);
  fail_unless (tags != NULL);

  gst_tag_list_unref (tags);
  g_free (artist);
  g_free (title);
}

GST_END_TEST;

GST_START_TEST (test_merge_strings_with_comma)
{
  GstTagList *tags;
  gchar *artists = NULL;

  tags = gst_tag_list_new_empty ();
  gst_tag_list_add (tags, GST_TAG_MERGE_APPEND, GST_TAG_ARTIST, "Foo", NULL);
  gst_tag_list_add (tags, GST_TAG_MERGE_APPEND, GST_TAG_ARTIST, "Bar", NULL);
  gst_tag_list_add (tags, GST_TAG_MERGE_APPEND, GST_TAG_ARTIST, "Yay", NULL);
  gst_tag_list_get_string (tags, GST_TAG_ARTIST, &artists);
  fail_unless (artists != NULL);
  /* can't check for exact string since the comma separator is i18n-ed */
  fail_unless (strstr (artists, "Foo") != NULL);
  fail_unless (strstr (artists, "Bar") != NULL);
  fail_unless (strstr (artists, "Yay") != NULL);
  g_free (artists);
  gst_tag_list_unref (tags);
}

GST_END_TEST;

GST_START_TEST (test_equal)
{
  GstTagList *tags, *tags2;
  GstSample *sample1, *sample2, *sample11;
  GstBuffer *buf;

  tags = gst_tag_list_new_empty ();
  gst_tag_list_add (tags, GST_TAG_MERGE_APPEND, GST_TAG_ARTIST, "Foo", NULL);
  gst_tag_list_add (tags, GST_TAG_MERGE_APPEND, GST_TAG_ARTIST, "Bar", NULL);
  gst_tag_list_add (tags, GST_TAG_MERGE_APPEND, GST_TAG_ARTIST, "Yay", NULL);

  tags2 = gst_tag_list_new_empty ();
  fail_unless (!gst_tag_list_is_equal (tags2, tags));
  gst_tag_list_add (tags2, GST_TAG_MERGE_APPEND, GST_TAG_ARTIST, "Yay", NULL);
  fail_unless (!gst_tag_list_is_equal (tags2, tags));
  gst_tag_list_add (tags2, GST_TAG_MERGE_APPEND, GST_TAG_ARTIST, "Bar", NULL);
  fail_unless (!gst_tag_list_is_equal (tags2, tags));
  gst_tag_list_add (tags2, GST_TAG_MERGE_APPEND, GST_TAG_ARTIST, "Foo", NULL);
  fail_unless (gst_tag_list_is_equal (tags2, tags));

  gst_tag_list_add (tags, GST_TAG_MERGE_APPEND, GST_TAG_REFERENCE_LEVEL,
      9.87654321, NULL);
  fail_unless (!gst_tag_list_is_equal (tags2, tags));
  gst_tag_list_add (tags2, GST_TAG_MERGE_APPEND, GST_TAG_REFERENCE_LEVEL,
      9.87654320, NULL);
  /* want these two double values to be equal despite minor differences */
  fail_unless (gst_tag_list_is_equal (tags2, tags));

  /* want this to be unequal though, difference too large */
  gst_tag_list_add (tags2, GST_TAG_MERGE_REPLACE, GST_TAG_REFERENCE_LEVEL,
      9.87654310, NULL);
  fail_unless (!gst_tag_list_is_equal (tags2, tags));

  gst_tag_list_unref (tags);
  gst_tag_list_unref (tags2);

  /* samples */
  buf = gst_buffer_new_wrapped (g_strdup ("test 1-2-3"), 10);
  sample1 = gst_sample_new (buf, NULL, NULL, NULL);
  gst_buffer_unref (buf);

  buf = gst_buffer_new_wrapped (g_strdup ("test 1-2-3"), 10);
  sample11 = gst_sample_new (buf, NULL, NULL, NULL);
  gst_buffer_unref (buf);

  buf = gst_buffer_new_wrapped (g_strdup ("test 2-3-4-5"), 12);
  sample2 = gst_sample_new (buf, NULL, NULL, NULL);
  gst_buffer_unref (buf);

  tags = gst_tag_list_new_empty ();
  gst_tag_list_add (tags, GST_TAG_MERGE_APPEND, GST_TAG_IMAGE, sample1, NULL);

  tags2 = gst_tag_list_new_empty ();
  fail_unless (!gst_tag_list_is_equal (tags2, tags));

  /* sample sample, should be very equal now */
  gst_tag_list_add (tags2, GST_TAG_MERGE_APPEND, GST_TAG_IMAGE, sample1, NULL);
  fail_unless (gst_tag_list_is_equal (tags2, tags));
  gst_tag_list_unref (tags2);

  /* same buffer content, but different buffer instances, still rather equal */
  tags2 = gst_tag_list_new_empty ();
  gst_tag_list_add (tags2, GST_TAG_MERGE_APPEND, GST_TAG_IMAGE, sample11, NULL);
  fail_unless (gst_tag_list_is_equal (tags2, tags));
  gst_tag_list_unref (tags2);

  /* different buffer content, should not be equal */
  tags2 = gst_tag_list_new_empty ();
  gst_tag_list_add (tags2, GST_TAG_MERGE_APPEND, GST_TAG_IMAGE, sample2, NULL);
  fail_unless (!gst_tag_list_is_equal (tags2, tags));
  gst_tag_list_unref (tags2);

  gst_tag_list_unref (tags);

  gst_sample_unref (sample1);
  gst_sample_unref (sample11);
  gst_sample_unref (sample2);
}

GST_END_TEST;

GST_START_TEST (test_writability)
{
  GstTagList *tags, *wtags;

  tags = gst_tag_list_new_empty ();
  gst_tag_list_add (tags, GST_TAG_MERGE_APPEND, GST_TAG_ARTIST, "Foo", NULL);

  /* take ref, should no longer be writable */
  gst_tag_list_ref (tags);
  ASSERT_CRITICAL (gst_tag_list_add (tags, GST_TAG_MERGE_APPEND,
          GST_TAG_ARTIST, "Bar", NULL));

  wtags = gst_tag_list_make_writable (tags);
  /* should be ok now */
  gst_tag_list_add (wtags, GST_TAG_MERGE_APPEND, GST_TAG_ARTIST, "Bar", NULL);
  gst_tag_list_unref (wtags);

  /* this too, since we dropped the second ref in make_writable */
  gst_tag_list_add (tags, GST_TAG_MERGE_APPEND, GST_TAG_ARTIST, "Bar", NULL);
  gst_tag_list_unref (tags);
}

GST_END_TEST;

/* this tests GstSample serialisation/deserialisation, esp. with multiple
 * samples in a tag list */
GST_START_TEST (test_serialization)
{
  GstTagList *tags, *tags2;
  GstBuffer *b1, *b2;
  GstSample *s1, *s2;
  GstCaps *c2;
  gchar *s;

  b1 = gst_buffer_new_allocate (NULL, 1, NULL);
  gst_buffer_memset (b1, 0, 0xb3, -1);
  s1 = gst_sample_new (b1, NULL, NULL, NULL);
  gst_buffer_unref (b1);

  b2 = gst_buffer_new_allocate (NULL, 8, NULL);
  gst_buffer_memset (b2, 0, 0x2f, -1);
  c2 = gst_caps_new_empty_simple ("foo/bar");
  s2 = gst_sample_new (b2, c2, NULL, NULL);
  gst_buffer_unref (b2);
  gst_caps_unref (c2);
  c2 = NULL;

  tags = gst_tag_list_new (GST_TAG_ATTACHMENT, s1, NULL);
  gst_tag_list_add (tags, GST_TAG_MERGE_APPEND, GST_TAG_ATTACHMENT, s2, NULL);
  GST_INFO ("tags: %" GST_PTR_FORMAT, tags);

  s = gst_tag_list_to_string (tags);
  GST_INFO ("taglist -> string: %s", s);
  tags2 = gst_tag_list_new_from_string (s);
  GST_INFO ("string -> taglist: %" GST_PTR_FORMAT, tags2);
  fail_unless (gst_tag_list_is_equal (tags, tags2));
  gst_tag_list_unref (tags2);
  g_free (s);

  gst_sample_unref (s1);
  gst_sample_unref (s2);
  gst_tag_list_unref (tags);
}

GST_END_TEST;

GST_START_TEST (test_empty_taglist_serialization)
{
  GstTagList *taglist, *taglist2;
  gchar *str;

  taglist = gst_tag_list_new_empty ();
  str = gst_tag_list_to_string (taglist);
  taglist2 = gst_tag_list_new_from_string (str);
  fail_if (taglist2 == NULL);
  fail_unless (gst_tag_list_is_equal (taglist, taglist2));

  gst_tag_list_unref (taglist);
  gst_tag_list_unref (taglist2);
  g_free (str);
}

GST_END_TEST;


static Suite *
gst_tag_suite (void)
{
  Suite *s = suite_create ("GstTag");
  TCase *tc_chain = tcase_create ("general");

  suite_add_tcase (s, tc_chain);
  tcase_add_test (tc_chain, test_basics);
  tcase_add_test (tc_chain, test_add);
  tcase_add_test (tc_chain, test_merge);
  tcase_add_test (tc_chain, test_merge_strings_with_comma);
  tcase_add_test (tc_chain, test_date_tags);
  tcase_add_test (tc_chain, test_type);
  tcase_add_test (tc_chain, test_set_non_utf8_string);
  tcase_add_test (tc_chain, test_buffer_tags);
  tcase_add_test (tc_chain, test_empty_tags);
  tcase_add_test (tc_chain, test_new_full);
  tcase_add_test (tc_chain, test_equal);
  tcase_add_test (tc_chain, test_writability);
  tcase_add_test (tc_chain, test_serialization);
  tcase_add_test (tc_chain, test_empty_taglist_serialization);

  return s;
}

GST_CHECK_MAIN (gst_tag);
