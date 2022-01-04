/* Generated by ./xlat/gen.sh from ./xlat/v4l2_colorspaces.in; do not edit. */

#include "gcc_compat.h"
#include "static_assert.h"


#ifndef XLAT_MACROS_ONLY

# ifdef IN_MPERS

extern const struct xlat v4l2_colorspaces[];

# else

static const struct xlat_data v4l2_colorspaces_xdata[] = {
#if defined(V4L2_COLORSPACE_SMPTE170M) || (defined(HAVE_DECL_V4L2_COLORSPACE_SMPTE170M) && HAVE_DECL_V4L2_COLORSPACE_SMPTE170M)
  XLAT(V4L2_COLORSPACE_SMPTE170M),
#endif
#if defined(V4L2_COLORSPACE_SMPTE240M) || (defined(HAVE_DECL_V4L2_COLORSPACE_SMPTE240M) && HAVE_DECL_V4L2_COLORSPACE_SMPTE240M)
  XLAT(V4L2_COLORSPACE_SMPTE240M),
#endif
#if defined(V4L2_COLORSPACE_REC709) || (defined(HAVE_DECL_V4L2_COLORSPACE_REC709) && HAVE_DECL_V4L2_COLORSPACE_REC709)
  XLAT(V4L2_COLORSPACE_REC709),
#endif
#if defined(V4L2_COLORSPACE_BT878) || (defined(HAVE_DECL_V4L2_COLORSPACE_BT878) && HAVE_DECL_V4L2_COLORSPACE_BT878)
  XLAT(V4L2_COLORSPACE_BT878),
#endif
#if defined(V4L2_COLORSPACE_470_SYSTEM_M) || (defined(HAVE_DECL_V4L2_COLORSPACE_470_SYSTEM_M) && HAVE_DECL_V4L2_COLORSPACE_470_SYSTEM_M)
  XLAT(V4L2_COLORSPACE_470_SYSTEM_M),
#endif
#if defined(V4L2_COLORSPACE_470_SYSTEM_BG) || (defined(HAVE_DECL_V4L2_COLORSPACE_470_SYSTEM_BG) && HAVE_DECL_V4L2_COLORSPACE_470_SYSTEM_BG)
  XLAT(V4L2_COLORSPACE_470_SYSTEM_BG),
#endif
#if defined(V4L2_COLORSPACE_JPEG) || (defined(HAVE_DECL_V4L2_COLORSPACE_JPEG) && HAVE_DECL_V4L2_COLORSPACE_JPEG)
  XLAT(V4L2_COLORSPACE_JPEG),
#endif
#if defined(V4L2_COLORSPACE_SRGB) || (defined(HAVE_DECL_V4L2_COLORSPACE_SRGB) && HAVE_DECL_V4L2_COLORSPACE_SRGB)
  XLAT(V4L2_COLORSPACE_SRGB),
#endif
#if defined(V4L2_COLORSPACE_ADOBERGB) || (defined(HAVE_DECL_V4L2_COLORSPACE_ADOBERGB) && HAVE_DECL_V4L2_COLORSPACE_ADOBERGB)
  XLAT(V4L2_COLORSPACE_ADOBERGB),
#endif
#if defined(V4L2_COLORSPACE_BT2020) || (defined(HAVE_DECL_V4L2_COLORSPACE_BT2020) && HAVE_DECL_V4L2_COLORSPACE_BT2020)
  XLAT(V4L2_COLORSPACE_BT2020),
#endif
#if defined(V4L2_COLORSPACE_RAW) || (defined(HAVE_DECL_V4L2_COLORSPACE_RAW) && HAVE_DECL_V4L2_COLORSPACE_RAW)
  XLAT(V4L2_COLORSPACE_RAW),
#endif
#if defined(V4L2_COLORSPACE_DCI_P3) || (defined(HAVE_DECL_V4L2_COLORSPACE_DCI_P3) && HAVE_DECL_V4L2_COLORSPACE_DCI_P3)
  XLAT(V4L2_COLORSPACE_DCI_P3),
#endif
};
#  if !(defined HAVE_M32_MPERS || defined HAVE_MX32_MPERS)
static
#  endif
const struct xlat v4l2_colorspaces[1] = { {
 .data = v4l2_colorspaces_xdata,
 .size = ARRAY_SIZE(v4l2_colorspaces_xdata),
 .type = XT_NORMAL,
} };

# endif /* !IN_MPERS */

#endif /* !XLAT_MACROS_ONLY */