/* GStreamer
 * Copyright (C) 2012 Wim Taymans <wim.taymans@gmail.com>
 *
 * libsabi.c: Unit test for ABI compatibility
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

#include <config.h>
#include <gst/check/gstcheck.h>

#include <gst/base/gstadapter.h>
#include <gst/base/gstbaseparse.h>
#include <gst/base/gstbasesink.h>
#include <gst/base/gstbasesrc.h>
#include <gst/base/gstbasetransform.h>
#include <gst/base/gstbitreader.h>
#include <gst/base/gstbytereader.h>
#include <gst/base/gstbytewriter.h>
#include <gst/base/gstcollectpads.h>
#include <gst/base/gstpushsrc.h>
#include <gst/base/gsttypefindhelper.h>
#include <gst/controller/gstinterpolationcontrolsource.h>
#include <gst/controller/gstlfocontrolsource.h>
#include <gst/controller/gsttriggercontrolsource.h>
#include <gst/controller/gstargbcontrolbinding.h>
#include <gst/controller/gstdirectcontrolbinding.h>
#include <gst/controller/gsttimedvaluecontrolsource.h>
#include <gst/net/gstnet.h>
#include <gst/net/gstnetclientclock.h>
#include <gst/net/gstnettimepacket.h>
#include <gst/net/gstnettimeprovider.h>

#ifdef HAVE_CPU_I386
#include "struct_i386.h"
#define HAVE_ABI_SIZES TRUE
#else
#ifdef __powerpc64__
#include "struct_ppc64.h"
#define HAVE_ABI_SIZES FALSE
#else
#ifdef __powerpc__
#include "struct_ppc32.h"
#define HAVE_ABI_SIZES TRUE
#else
#ifdef HAVE_CPU_X86_64
#include "struct_x86_64.h"
#define HAVE_ABI_SIZES TRUE
#else
#ifdef HAVE_CPU_HPPA
#include "struct_hppa.h"
#define HAVE_ABI_SIZES FALSE
#else
#ifdef HAVE_CPU_SPARC
#include "struct_sparc.h"
#define HAVE_ABI_SIZES FALSE
#else
#ifdef HAVE_CPU_ARM
#include "struct_arm.h"
#define HAVE_ABI_SIZES TRUE
#else
/* in case someone wants to generate a new arch */
#include "struct_i386.h"
#define HAVE_ABI_SIZES FALSE
#endif
#endif
#endif
#endif
#endif
#endif
#endif

GST_START_TEST (test_ABI)
{
  gst_check_abi_list (list, HAVE_ABI_SIZES);
}

GST_END_TEST;

static Suite *
libsabi_suite (void)
{
  Suite *s = suite_create ("LibsABI");
  TCase *tc_chain = tcase_create ("size check");

  tcase_set_timeout (tc_chain, 0);

  suite_add_tcase (s, tc_chain);
  tcase_add_test (tc_chain, test_ABI);
  return s;
}

GST_CHECK_MAIN (libsabi);
