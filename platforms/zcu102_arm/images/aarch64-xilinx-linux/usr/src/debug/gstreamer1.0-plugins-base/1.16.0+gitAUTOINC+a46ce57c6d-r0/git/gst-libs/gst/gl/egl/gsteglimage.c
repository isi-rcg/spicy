/*
 * GStreamer
 * Copyright (C) 2012 Collabora Ltd.
 *   @author: Sebastian Dröge <sebastian.droege@collabora.co.uk>
 * Copyright (C) 2014 Julien Isorce <julien.isorce@gmail.com>
 * Copyright (C) 2016 Matthew Waters <matthew@centricular.com>
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
 * SECTION:gsteglimage
 * @short_description: EGLImage abstraction
 * @title: GstEGLImage
 * @see_also: #GstGLMemoryEGL, #GstGLContext
 *
 * #GstEGLImage represents and holds an #EGLImage handle.
 *
 * A #GstEGLImage can be created from a dmabuf with gst_egl_image_from_dmabuf(),
 * or gst_egl_image_from_dmabuf_direct(), or #GstGLMemoryEGL provides a
 * #GstAllocator to allocate #EGLImage's bound to and OpenGL texture.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gsteglimage.h"

#include <string.h>

#include <gst/gl/gstglfeature.h>
#include <gst/gl/gstglmemory.h>

#include "gst/gl/egl/gstegl.h"
#include "gst/gl/egl/gstglcontext_egl.h"
#include "gst/gl/egl/gstgldisplay_egl.h"

#if GST_GL_HAVE_DMABUF
#include <gst/allocators/gstdmabuf.h>
#include <libdrm/drm_fourcc.h>

#ifndef DRM_FORMAT_R8
#define DRM_FORMAT_R8 fourcc_code('R', '8', ' ', ' ')
#endif

#ifndef DRM_FORMAT_RG88
#define DRM_FORMAT_RG88 fourcc_code('R', 'G', '8', '8')
#endif

#ifndef DRM_FORMAT_GR88
#define DRM_FORMAT_GR88 fourcc_code('G', 'R', '8', '8')
#endif

#ifndef DRM_FORMAT_NV24
#define DRM_FORMAT_NV24 fourcc_code('N', 'V', '2', '4')
#endif
#endif

#ifndef EGL_LINUX_DMA_BUF_EXT
#define EGL_LINUX_DMA_BUF_EXT 0x3270
#endif

#ifndef EGL_LINUX_DRM_FOURCC_EXT
#define EGL_LINUX_DRM_FOURCC_EXT 0x3271
#endif

#ifndef EGL_DMA_BUF_PLANE0_FD_EXT
#define EGL_DMA_BUF_PLANE0_FD_EXT 0x3272
#endif

#ifndef EGL_DMA_BUF_PLANE0_OFFSET_EXT
#define EGL_DMA_BUF_PLANE0_OFFSET_EXT 0x3273
#endif

#ifndef EGL_DMA_BUF_PLANE0_PITCH_EXT
#define EGL_DMA_BUF_PLANE0_PITCH_EXT 0x3274
#endif

#ifndef EGL_DMA_BUF_PLANE1_FD_EXT
#define EGL_DMA_BUF_PLANE1_FD_EXT 0x3275
#endif

#ifndef EGL_DMA_BUF_PLANE1_OFFSET_EXT
#define EGL_DMA_BUF_PLANE1_OFFSET_EXT 0x3276
#endif

#ifndef EGL_DMA_BUF_PLANE1_PITCH_EXT
#define EGL_DMA_BUF_PLANE1_PITCH_EXT 0x3277
#endif

#ifndef EGL_DMA_BUF_PLANE2_FD_EXT
#define EGL_DMA_BUF_PLANE2_FD_EXT 0x3278
#endif

#ifndef EGL_DMA_BUF_PLANE2_OFFSET_EXT
#define EGL_DMA_BUF_PLANE2_OFFSET_EXT 0x3279
#endif

#ifndef EGL_DMA_BUF_PLANE2_PITCH_EXT
#define EGL_DMA_BUF_PLANE2_PITCH_EXT 0x327A
#endif

#ifndef DRM_FORMAT_MOD_LINEAR
#define DRM_FORMAT_MOD_LINEAR 0ULL
#endif

#ifndef EGL_DMA_BUF_PLANE0_MODIFIER_LO_EXT
#define EGL_DMA_BUF_PLANE0_MODIFIER_LO_EXT 0x3443
#endif

#ifndef EGL_DMA_BUF_PLANE0_MODIFIER_HI_EXT
#define EGL_DMA_BUF_PLANE0_MODIFIER_HI_EXT 0x3444
#endif

#ifndef EGL_DMA_BUF_PLANE1_MODIFIER_LO_EXT
#define EGL_DMA_BUF_PLANE1_MODIFIER_LO_EXT 0x3445
#endif

#ifndef EGL_DMA_BUF_PLANE1_MODIFIER_HI_EXT
#define EGL_DMA_BUF_PLANE1_MODIFIER_HI_EXT 0x3446
#endif

#ifndef EGL_DMA_BUF_PLANE2_MODIFIER_LO_EXT
#define EGL_DMA_BUF_PLANE2_MODIFIER_LO_EXT 0x3447
#endif

#ifndef EGL_DMA_BUF_PLANE2_MODIFIER_HI_EXT
#define EGL_DMA_BUF_PLANE2_MODIFIER_HI_EXT 0x3448
#endif

#ifndef EGL_ITU_REC601_EXT
#define EGL_ITU_REC601_EXT 0x327F
#endif

#ifndef EGL_ITU_REC709_EXT
#define EGL_ITU_REC709_EXT 0x3280
#endif

#ifndef EGL_ITU_REC2020_EXT
#define EGL_ITU_REC2020_EXT 0x3281
#endif

#ifndef EGL_SAMPLE_RANGE_HINT_EXT
#define EGL_SAMPLE_RANGE_HINT_EXT 0x327C
#endif

#ifndef EGL_YUV_COLOR_SPACE_HINT_EXT
#define EGL_YUV_COLOR_SPACE_HINT_EXT 0x327B
#endif

#ifndef EGL_YUV_FULL_RANGE_EXT
#define EGL_YUV_FULL_RANGE_EXT 0x3282
#endif

#ifndef EGL_YUV_NARROW_RANGE_EXT
#define EGL_YUV_NARROW_RANGE_EXT 0x3283
#endif

#if !GST_GL_HAVE_EGLUINT64KHR
typedef khronos_uint64_t EGLuint64KHR;
#endif

GST_DEFINE_MINI_OBJECT_TYPE (GstEGLImage, gst_egl_image);

#ifndef GST_DISABLE_GST_DEBUG
#define GST_CAT_DEFAULT gst_egl_image_ensure_debug_category()

static GstDebugCategory *
gst_egl_image_ensure_debug_category (void)
{
  static gsize cat_gonce = 0;

  if (g_once_init_enter (&cat_gonce)) {
    GstDebugCategory *cat = NULL;

    GST_DEBUG_CATEGORY_INIT (cat, "gleglimage", 0, "EGLImage wrapper");

    g_once_init_leave (&cat_gonce, (gsize) cat);
  }

  return (GstDebugCategory *) cat_gonce;
}
#endif /* GST_DISABLE_GST_DEBUG */

/**
 * gst_egl_image_get_image:
 * @image: a #GstEGLImage
 *
 * Returns: the #EGLImageKHR of @image
 */
gpointer
gst_egl_image_get_image (GstEGLImage * image)
{
  g_return_val_if_fail (GST_IS_EGL_IMAGE (image), EGL_NO_IMAGE_KHR);

  return image->image;
}

static void
_gst_egl_image_free_thread (GstGLContext * context, GstEGLImage * image)
{
  if (image->destroy_notify)
    image->destroy_notify (image, image->destroy_data);
}

static void
_gst_egl_image_free (GstMiniObject * object)
{
  GstEGLImage *image = GST_EGL_IMAGE (object);

  if (image->context) {
    gst_gl_context_thread_add (GST_GL_CONTEXT (image->context),
        (GstGLContextThreadFunc) _gst_egl_image_free_thread, image);
    gst_object_unref (image->context);
  }

  g_free (image);
}

static GstMiniObject *
_gst_egl_image_copy (GstMiniObject * obj)
{
  return gst_mini_object_ref (obj);
}

/**
 * gst_egl_image_new_wrapped:
 * @context: a #GstGLContext (must be an EGL context)
 * @image: the image to wrap
 * @format: the #GstGLFormat
 * @user_data: user data
 * @user_data_destroy: (destroy user_data): called when @user_data is no longer needed
 *
 * Returns: a new #GstEGLImage wrapping @image
 */
GstEGLImage *
gst_egl_image_new_wrapped (GstGLContext * context, gpointer image,
    GstGLFormat format, gpointer user_data,
    GstEGLImageDestroyNotify user_data_destroy)
{
  GstEGLImage *img = NULL;

  g_return_val_if_fail (context != NULL, NULL);
  g_return_val_if_fail ((gst_gl_context_get_gl_platform (context) &
          GST_GL_PLATFORM_EGL) != 0, NULL);
  g_return_val_if_fail (image != EGL_NO_IMAGE_KHR, NULL);

  img = g_new0 (GstEGLImage, 1);
  gst_mini_object_init (GST_MINI_OBJECT_CAST (img), 0, GST_TYPE_EGL_IMAGE,
      (GstMiniObjectCopyFunction) _gst_egl_image_copy, NULL,
      (GstMiniObjectFreeFunction) _gst_egl_image_free);

  img->context = gst_object_ref (context);
  img->image = image;
  img->format = format;

  img->destroy_data = user_data;
  img->destroy_notify = user_data_destroy;

  return img;
}

static EGLImageKHR
_gst_egl_image_create (GstGLContext * context, guint target,
    EGLClientBuffer buffer, guintptr * attribs)
{
  EGLDisplay egl_display = EGL_DEFAULT_DISPLAY;
  EGLContext egl_context = EGL_NO_CONTEXT;
  EGLImageKHR img = EGL_NO_IMAGE_KHR;
  GstGLDisplayEGL *display_egl;
  gint plat_major, plat_minor;
  guint attrib_len = 0;

  gst_gl_context_get_gl_platform_version (context, &plat_major, &plat_minor);

  display_egl = gst_gl_display_egl_from_gl_display (context->display);
  if (!display_egl) {
    GST_WARNING_OBJECT (context, "Failed to retrieve GstGLDisplayEGL from %"
        GST_PTR_FORMAT, context->display);
    return EGL_NO_IMAGE_KHR;
  }
  egl_display =
      (EGLDisplay) gst_gl_display_get_handle (GST_GL_DISPLAY (display_egl));
  gst_object_unref (display_egl);

  if (target != EGL_LINUX_DMA_BUF_EXT)
    egl_context = (EGLContext) gst_gl_context_get_gl_context (context);

  if (attribs)
    while (attribs[attrib_len++] != EGL_NONE) {
    }
#ifdef EGL_VERSION_1_5
  if (GST_GL_CHECK_GL_VERSION (plat_major, plat_minor, 1, 5)) {
    EGLImageKHR (*gst_eglCreateImage) (EGLDisplay dpy, EGLContext ctx,
        EGLenum target, EGLClientBuffer buffer, const EGLAttrib * attrib_list);
    EGLAttrib *egl_attribs = NULL;
    guint i;

    gst_eglCreateImage = gst_gl_context_get_proc_address (context,
        "eglCreateImage");
    if (!gst_eglCreateImage) {
      GST_ERROR_OBJECT (context, "\"eglCreateImage\" not exposed by the "
          "implementation as required by EGL >= 1.5");
      return EGL_NO_IMAGE_KHR;
    }

    if (attribs) {
      egl_attribs = g_new0 (EGLAttrib, attrib_len);
      for (i = 0; i < attrib_len; i++)
        egl_attribs[i] = (EGLAttrib) attribs[i];
    }

    img = gst_eglCreateImage (egl_display, egl_context, target, buffer,
        egl_attribs);

    g_free (egl_attribs);
  } else
#endif
  {
    EGLImageKHR (*gst_eglCreateImageKHR) (EGLDisplay dpy, EGLContext ctx,
        EGLenum target, EGLClientBuffer buffer, const EGLint * attrib_list);
    EGLint *egl_attribs = NULL;
    gint i;

    gst_eglCreateImageKHR = gst_gl_context_get_proc_address (context,
        "eglCreateImageKHR");
    if (!gst_eglCreateImageKHR) {
      GST_WARNING_OBJECT (context, "\"eglCreateImageKHR\" not exposed by the "
          "implementation");
      return EGL_NO_IMAGE_KHR;
    }

    if (attribs) {
      egl_attribs = g_new0 (EGLint, attrib_len);
      for (i = 0; i < attrib_len; i++)
        egl_attribs[i] = (EGLint) attribs[i];
    }

    img = gst_eglCreateImageKHR (egl_display, egl_context, target, buffer,
        egl_attribs);

    g_free (egl_attribs);
  }

  return img;
}

static void
_gst_egl_image_destroy (GstGLContext * context, EGLImageKHR image)
{
  EGLBoolean (*gst_eglDestroyImage) (EGLDisplay dpy, EGLImageKHR image);
  EGLDisplay egl_display = EGL_DEFAULT_DISPLAY;
  GstGLDisplayEGL *display_egl;

  gst_eglDestroyImage = gst_gl_context_get_proc_address (context,
      "eglDestroyImage");
  if (!gst_eglDestroyImage) {
    gst_eglDestroyImage = gst_gl_context_get_proc_address (context,
        "eglDestroyImageKHR");
    if (!gst_eglDestroyImage) {
      GST_ERROR_OBJECT (context, "\"eglDestroyImage\" not exposed by the "
          "implementation");
      return;
    }
  }

  display_egl = gst_gl_display_egl_from_gl_display (context->display);
  if (!display_egl) {
    GST_WARNING_OBJECT (context, "Failed to retrieve GstGLDisplayEGL from %"
        GST_PTR_FORMAT, context->display);
    return;
  }
  egl_display =
      (EGLDisplay) gst_gl_display_get_handle (GST_GL_DISPLAY (display_egl));
  gst_object_unref (display_egl);

  if (!gst_eglDestroyImage (egl_display, image))
    GST_WARNING_OBJECT (context, "eglDestroyImage failed");
}

static void
_destroy_egl_image (GstEGLImage * image, gpointer user_data)
{
  _gst_egl_image_destroy (image->context, image->image);
}

/**
 * gst_egl_image_from_texture:
 * @context: a #GstGLContext (must be an EGL context)
 * @gl_mem: a #GstGLMemory
 * @attribs: additional attributes to add to the eglCreateImage() call.
 *
 * Returns: (transfer full): a #GstEGLImage wrapping @gl_mem or %NULL on failure
 */
GstEGLImage *
gst_egl_image_from_texture (GstGLContext * context, GstGLMemory * gl_mem,
    guintptr * attribs)
{
  EGLenum egl_target;
  EGLImageKHR img;

  if (gl_mem->tex_target != GST_GL_TEXTURE_TARGET_2D) {
    GST_FIXME_OBJECT (context, "Only know how to create EGLImage's from 2D "
        "textures");
    return NULL;
  }

  egl_target = EGL_GL_TEXTURE_2D_KHR;

  img = _gst_egl_image_create (context, egl_target,
      (EGLClientBuffer) (guintptr) gl_mem->tex_id, attribs);
  if (!img)
    return NULL;

  return gst_egl_image_new_wrapped (context, img, gl_mem->tex_format, NULL,
      (GstEGLImageDestroyNotify) _destroy_egl_image);
}

#if GST_GL_HAVE_DMABUF
/*
 * GStreamer format descriptions differ from DRM formats as the representation
 * is relative to a register, hence in native endianness. To reduce the driver
 * requirement, we only import with a subset of texture formats and use
 * shaders to convert. This way we avoid having to use external texture
 * target.
 */
static int
_drm_rgba_fourcc_from_info (GstVideoInfo * info, int plane,
    GstGLFormat * out_format)
{
  GstVideoFormat format = GST_VIDEO_INFO_FORMAT (info);
#if G_BYTE_ORDER == G_LITTLE_ENDIAN
  const gint rgba_fourcc = DRM_FORMAT_ABGR8888;
  const gint rgb_fourcc = DRM_FORMAT_BGR888;
  const gint rg_fourcc = DRM_FORMAT_GR88;
#else
  const gint rgba_fourcc = DRM_FORMAT_RGBA8888;
  const gint rgb_fourcc = DRM_FORMAT_RGB888;
  const gint rg_fourcc = DRM_FORMAT_RG88;
#endif

  GST_DEBUG ("Getting DRM fourcc for %s plane %i",
      gst_video_format_to_string (format), plane);

  switch (format) {
    case GST_VIDEO_FORMAT_RGB16:
    case GST_VIDEO_FORMAT_BGR16:
      *out_format = GST_GL_RGB565;
      return DRM_FORMAT_RGB565;

    case GST_VIDEO_FORMAT_RGB:
    case GST_VIDEO_FORMAT_BGR:
      *out_format = GST_GL_RGB;
      return rgb_fourcc;

    case GST_VIDEO_FORMAT_RGBA:
    case GST_VIDEO_FORMAT_RGBx:
    case GST_VIDEO_FORMAT_BGRA:
    case GST_VIDEO_FORMAT_BGRx:
    case GST_VIDEO_FORMAT_ARGB:
    case GST_VIDEO_FORMAT_xRGB:
    case GST_VIDEO_FORMAT_ABGR:
    case GST_VIDEO_FORMAT_xBGR:
    case GST_VIDEO_FORMAT_AYUV:
      *out_format = GST_GL_RGBA;
      return rgba_fourcc;

    case GST_VIDEO_FORMAT_GRAY8:
      *out_format = GST_GL_RED;
      return DRM_FORMAT_R8;

    case GST_VIDEO_FORMAT_YUY2:
    case GST_VIDEO_FORMAT_UYVY:
    case GST_VIDEO_FORMAT_GRAY16_LE:
    case GST_VIDEO_FORMAT_GRAY16_BE:
      *out_format = GST_GL_RG;
      return rg_fourcc;

    case GST_VIDEO_FORMAT_NV12:
    case GST_VIDEO_FORMAT_NV21:
      *out_format = plane == 0 ? GST_GL_RED : GST_GL_RG;
      return plane == 0 ? DRM_FORMAT_R8 : rg_fourcc;

    case GST_VIDEO_FORMAT_I420:
    case GST_VIDEO_FORMAT_YV12:
    case GST_VIDEO_FORMAT_Y41B:
    case GST_VIDEO_FORMAT_Y42B:
    case GST_VIDEO_FORMAT_Y444:
      *out_format = GST_GL_RED;
      return DRM_FORMAT_R8;

    default:
      GST_ERROR ("Unsupported format for DMABuf.");
      return -1;
  }
}

/**
 * gst_egl_image_from_dmabuf:
 * @context: a #GstGLContext (must be an EGL context)
 * @dmabuf: the DMA-Buf file descriptor
 * @in_info: the #GstVideoInfo in @dmabuf
 * @plane: the plane in @in_info to create and #GstEGLImage for
 * @offset: the byte-offset in the data
 *
 * Creates an EGL image that imports the dmabuf FD. The dmabuf data
 * is passed as RGBA data. Shaders later take this "RGBA" data and
 * convert it from its true format (described by in_info) to actual
 * RGBA output. For example, with I420, three EGL images are created,
 * one for each plane, each EGL image with a single-channel R format.
 * With NV12, two EGL images are created, one with R format, one
 * with RG format etc.
 *
 * Returns: a #GstEGLImage wrapping @dmabuf or %NULL on failure
 */
GstEGLImage *
gst_egl_image_from_dmabuf (GstGLContext * context,
    gint dmabuf, GstVideoInfo * in_info, gint plane, gsize offset)
{
  GstGLFormat format = 0;
  guintptr attribs[13];
  EGLImageKHR img;
  gint atti = 0;
  gint fourcc;
  gint i;

  fourcc = _drm_rgba_fourcc_from_info (in_info, plane, &format);
  GST_DEBUG ("fourcc %.4s (%d) plane %d (%dx%d)",
      (char *) &fourcc, fourcc, plane,
      GST_VIDEO_INFO_COMP_WIDTH (in_info, plane),
      GST_VIDEO_INFO_COMP_HEIGHT (in_info, plane));

  attribs[atti++] = EGL_WIDTH;
  attribs[atti++] = GST_VIDEO_INFO_COMP_WIDTH (in_info, plane);
  attribs[atti++] = EGL_HEIGHT;
  attribs[atti++] = GST_VIDEO_INFO_COMP_HEIGHT (in_info, plane);
  attribs[atti++] = EGL_LINUX_DRM_FOURCC_EXT;
  attribs[atti++] = fourcc;
  attribs[atti++] = EGL_DMA_BUF_PLANE0_FD_EXT;
  attribs[atti++] = dmabuf;
  attribs[atti++] = EGL_DMA_BUF_PLANE0_OFFSET_EXT;
  attribs[atti++] = offset;
  attribs[atti++] = EGL_DMA_BUF_PLANE0_PITCH_EXT;
  attribs[atti++] = GST_VIDEO_INFO_PLANE_STRIDE (in_info, plane);
  attribs[atti] = EGL_NONE;
  g_assert (atti == G_N_ELEMENTS (attribs) - 1);

  for (i = 0; i < atti; i++)
    GST_LOG ("attr %i: %" G_GINTPTR_FORMAT, i, attribs[i]);

  img = _gst_egl_image_create (context, EGL_LINUX_DMA_BUF_EXT, NULL, attribs);
  if (!img) {
    GST_WARNING ("eglCreateImage failed: %s",
        gst_egl_get_error_string (eglGetError ()));
    return NULL;
  }

  return gst_egl_image_new_wrapped (context, img, format, NULL,
      (GstEGLImageDestroyNotify) _destroy_egl_image);
}

/*
 * Variant of _drm_rgba_fourcc_from_info() that is used in case the GPU can
 * handle YUV formats directly (by using internal shaders, or hardwired
 * YUV->RGB conversion matrices etc.)
 */
static int
_drm_direct_fourcc_from_info (GstVideoInfo * info)
{
  GstVideoFormat format = GST_VIDEO_INFO_FORMAT (info);

  GST_DEBUG ("Getting DRM fourcc for %s", gst_video_format_to_string (format));

  switch (format) {
    case GST_VIDEO_FORMAT_YUY2:
      return DRM_FORMAT_YUYV;

    case GST_VIDEO_FORMAT_YVYU:
      return DRM_FORMAT_YVYU;

    case GST_VIDEO_FORMAT_UYVY:
      return DRM_FORMAT_UYVY;

    case GST_VIDEO_FORMAT_VYUY:
      return DRM_FORMAT_VYUY;

    case GST_VIDEO_FORMAT_AYUV:
      return DRM_FORMAT_AYUV;

    case GST_VIDEO_FORMAT_NV12:
      return DRM_FORMAT_NV12;

    case GST_VIDEO_FORMAT_NV21:
      return DRM_FORMAT_NV21;

    case GST_VIDEO_FORMAT_NV16:
      return DRM_FORMAT_NV16;

    case GST_VIDEO_FORMAT_NV61:
      return DRM_FORMAT_NV61;

    case GST_VIDEO_FORMAT_NV24:
      return DRM_FORMAT_NV24;

    case GST_VIDEO_FORMAT_YUV9:
      return DRM_FORMAT_YUV410;

    case GST_VIDEO_FORMAT_YVU9:
      return DRM_FORMAT_YVU410;

    case GST_VIDEO_FORMAT_Y41B:
      return DRM_FORMAT_YUV411;

    case GST_VIDEO_FORMAT_I420:
      return DRM_FORMAT_YUV420;

    case GST_VIDEO_FORMAT_YV12:
      return DRM_FORMAT_YVU420;

    case GST_VIDEO_FORMAT_Y42B:
      return DRM_FORMAT_YUV422;

    case GST_VIDEO_FORMAT_Y444:
      return DRM_FORMAT_YUV444;

    case GST_VIDEO_FORMAT_RGB16:
      return DRM_FORMAT_RGB565;

    case GST_VIDEO_FORMAT_BGR16:
      return DRM_FORMAT_BGR565;

    case GST_VIDEO_FORMAT_RGBA:
      return DRM_FORMAT_ABGR8888;

    case GST_VIDEO_FORMAT_RGBx:
      return DRM_FORMAT_XBGR8888;

    case GST_VIDEO_FORMAT_BGRA:
      return DRM_FORMAT_ARGB8888;

    case GST_VIDEO_FORMAT_BGRx:
      return DRM_FORMAT_XRGB8888;

    case GST_VIDEO_FORMAT_ARGB:
      return DRM_FORMAT_BGRA8888;

    case GST_VIDEO_FORMAT_xRGB:
      return DRM_FORMAT_BGRX8888;

    case GST_VIDEO_FORMAT_ABGR:
      return DRM_FORMAT_RGBA8888;

    case GST_VIDEO_FORMAT_xBGR:
      return DRM_FORMAT_RGBX8888;

    default:
      GST_INFO ("Unsupported format for direct DMABuf.");
      return -1;
  }
}

static gboolean
_gst_egl_image_check_dmabuf_direct (GstGLContext * context, int fourcc)
{
  EGLDisplay egl_display = EGL_DEFAULT_DISPLAY;
  GstGLDisplayEGL *display_egl;
  EGLint *formats;
  EGLint num_formats;
  EGLuint64KHR *modifiers;
  EGLBoolean *external_only;
  int num_modifiers;
  gboolean ret;
  int i;

  EGLBoolean (*gst_eglQueryDmaBufFormatsEXT) (EGLDisplay dpy,
      EGLint max_formats, EGLint * formats, EGLint * num_formats);
  EGLBoolean (*gst_eglQueryDmaBufModifiersEXT) (EGLDisplay dpy,
      int format, int max_modifiers, EGLuint64KHR * modifiers,
      EGLBoolean * external_only, int *num_modifiers);

  gst_eglQueryDmaBufFormatsEXT =
      gst_gl_context_get_proc_address (context, "eglQueryDmaBufFormatsEXT");
  gst_eglQueryDmaBufModifiersEXT =
      gst_gl_context_get_proc_address (context, "eglQueryDmaBufModifiersEXT");

  if (!gst_eglQueryDmaBufFormatsEXT || !gst_eglQueryDmaBufModifiersEXT)
    return FALSE;

  display_egl = gst_gl_display_egl_from_gl_display (context->display);
  if (!display_egl) {
    GST_WARNING_OBJECT (context,
        "Failed to retrieve GstGLDisplayEGL from %" GST_PTR_FORMAT,
        context->display);
    return FALSE;
  }
  egl_display =
      (EGLDisplay) gst_gl_display_get_handle (GST_GL_DISPLAY (display_egl));
  gst_object_unref (display_egl);

  ret = gst_eglQueryDmaBufFormatsEXT (egl_display, 0, NULL, &num_formats);
  if (!ret || num_formats == 0)
    return FALSE;

  formats = g_new (EGLint, num_formats);

  ret = gst_eglQueryDmaBufFormatsEXT (egl_display, num_formats, formats,
      &num_formats);
  if (!ret || num_formats == 0) {
    g_free (formats);
    return FALSE;
  }

  for (i = 0; i < num_formats; i++) {
    if (formats[i] == fourcc)
      break;
  }
  g_free (formats);
  if (i == num_formats) {
    GST_DEBUG ("driver does not support importing fourcc %" GST_FOURCC_FORMAT,
        GST_FOURCC_ARGS (fourcc));
    return FALSE;
  }

  ret = gst_eglQueryDmaBufModifiersEXT (egl_display, fourcc, 0, NULL, NULL,
      &num_modifiers);
  if (!ret || num_modifiers == 0) {
    GST_DEBUG ("driver does not report modifiers for fourcc %"
        GST_FOURCC_FORMAT, GST_FOURCC_ARGS (fourcc));
    return FALSE;
  }

  modifiers = g_new (EGLuint64KHR, num_modifiers);
  external_only = g_new (EGLBoolean, num_modifiers);

  ret = gst_eglQueryDmaBufModifiersEXT (egl_display, fourcc, num_modifiers,
      modifiers, external_only, &num_modifiers);
  if (!ret || num_modifiers == 0) {
    g_free (modifiers);
    g_free (external_only);
    return FALSE;
  }

  for (i = 0; i < num_modifiers; ++i) {
    if (modifiers[i] == DRM_FORMAT_MOD_LINEAR) {
      if (external_only[i]) {
        GST_DEBUG ("driver only supports external import of fourcc %"
            GST_FOURCC_FORMAT, GST_FOURCC_ARGS (fourcc));
      }
      ret = !external_only[i];
      g_free (modifiers);
      g_free (external_only);
      return ret;
    }
  }
  GST_DEBUG ("driver only supports non-linear import of fourcc %"
      GST_FOURCC_FORMAT, GST_FOURCC_ARGS (fourcc));
  g_free (modifiers);
  g_free (external_only);
  return FALSE;
}

/**
 * gst_egl_image_from_dmabuf_direct:
 * @context: a #GstGLContext (must be an EGL context)
 * @fd: Array of DMABuf file descriptors
 * @offset: Array of offsets, relative to the DMABuf
 * @in_info: the #GstVideoInfo
 *
 * Creates an EGL image that imports the dmabuf FD. The dmabuf data
 * is passed directly as the format described in in_info. This is
 * useful if the hardware is capable of performing color space conversions
 * internally. The appropriate DRM format is picked, and the EGL image
 * is created with this DRM format.
 *
 * Another notable difference to gst_egl_image_from_dmabuf()
 * is that this function creates one EGL image for all planes, not one for
 * a single plane.
 *
 * Returns: a #GstEGLImage wrapping @dmabuf or %NULL on failure
 */
GstEGLImage *
gst_egl_image_from_dmabuf_direct (GstGLContext * context,
    gint * fd, gsize * offset, GstVideoInfo * in_info)
{

  EGLImageKHR img;
  guint n_planes = GST_VIDEO_INFO_N_PLANES (in_info);
  gint fourcc;
  gint i;
  gboolean with_modifiers;

  /* Explanation of array length:
   * - 6 plane independent values are at the start (width, height, format FourCC)
   * - 10 values per plane, and there are up to MAX_NUM_DMA_BUF_PLANES planes
   * - 4 values for color space and range
   * - 1 extra value for the EGL_NONE sentinel
   */
  guintptr attribs[41];         /* 6 + 10 * 3 + 4 + 1 */
  gint atti = 0;

  fourcc = _drm_direct_fourcc_from_info (in_info);
  if (fourcc == -1)
    return NULL;

  if (!_gst_egl_image_check_dmabuf_direct (context, fourcc))
    return NULL;

  with_modifiers = gst_gl_context_check_feature (context,
      "EGL_EXT_image_dma_buf_import_with_modifiers");

  /* EGL DMABuf importation supports a maximum of 3 planes */
  if (G_UNLIKELY (n_planes > 3))
    return NULL;

  attribs[atti++] = EGL_WIDTH;
  attribs[atti++] = GST_VIDEO_INFO_WIDTH (in_info);
  attribs[atti++] = EGL_HEIGHT;
  attribs[atti++] = GST_VIDEO_INFO_HEIGHT (in_info);
  attribs[atti++] = EGL_LINUX_DRM_FOURCC_EXT;
  attribs[atti++] = fourcc;

  /* first plane */
  {
    attribs[atti++] = EGL_DMA_BUF_PLANE0_FD_EXT;
    attribs[atti++] = fd[0];
    attribs[atti++] = EGL_DMA_BUF_PLANE0_OFFSET_EXT;
    attribs[atti++] = offset[0];
    attribs[atti++] = EGL_DMA_BUF_PLANE0_PITCH_EXT;
    attribs[atti++] = in_info->stride[0];
    if (with_modifiers) {
      attribs[atti++] = EGL_DMA_BUF_PLANE0_MODIFIER_LO_EXT;
      attribs[atti++] = DRM_FORMAT_MOD_LINEAR & 0xffffffff;
      attribs[atti++] = EGL_DMA_BUF_PLANE0_MODIFIER_HI_EXT;
      attribs[atti++] = (DRM_FORMAT_MOD_LINEAR >> 32) & 0xffffffff;
    }
  }

  /* second plane */
  if (n_planes >= 2) {
    attribs[atti++] = EGL_DMA_BUF_PLANE1_FD_EXT;
    attribs[atti++] = fd[1];
    attribs[atti++] = EGL_DMA_BUF_PLANE1_OFFSET_EXT;
    attribs[atti++] = offset[1];
    attribs[atti++] = EGL_DMA_BUF_PLANE1_PITCH_EXT;
    attribs[atti++] = in_info->stride[1];
    if (with_modifiers) {
      attribs[atti++] = EGL_DMA_BUF_PLANE1_MODIFIER_LO_EXT;
      attribs[atti++] = DRM_FORMAT_MOD_LINEAR & 0xffffffff;
      attribs[atti++] = EGL_DMA_BUF_PLANE1_MODIFIER_HI_EXT;
      attribs[atti++] = (DRM_FORMAT_MOD_LINEAR >> 32) & 0xffffffff;
    }
  }

  /* third plane */
  if (n_planes == 3) {
    attribs[atti++] = EGL_DMA_BUF_PLANE2_FD_EXT;
    attribs[atti++] = fd[2];
    attribs[atti++] = EGL_DMA_BUF_PLANE2_OFFSET_EXT;
    attribs[atti++] = offset[2];
    attribs[atti++] = EGL_DMA_BUF_PLANE2_PITCH_EXT;
    attribs[atti++] = in_info->stride[2];
    if (with_modifiers) {
      attribs[atti++] = EGL_DMA_BUF_PLANE2_MODIFIER_LO_EXT;
      attribs[atti++] = DRM_FORMAT_MOD_LINEAR & 0xffffffff;
      attribs[atti++] = EGL_DMA_BUF_PLANE2_MODIFIER_HI_EXT;
      attribs[atti++] = (DRM_FORMAT_MOD_LINEAR >> 32) & 0xffffffff;
    }
  }

  {
    uint32_t color_space;
    switch (in_info->colorimetry.matrix) {
      case GST_VIDEO_COLOR_MATRIX_BT601:
        color_space = EGL_ITU_REC601_EXT;
        break;
      case GST_VIDEO_COLOR_MATRIX_BT709:
        color_space = EGL_ITU_REC709_EXT;
        break;
      case GST_VIDEO_COLOR_MATRIX_BT2020:
        color_space = EGL_ITU_REC2020_EXT;
        break;
      default:
        color_space = 0;
        break;
    }
    if (color_space != 0) {
      attribs[atti++] = EGL_YUV_COLOR_SPACE_HINT_EXT;
      attribs[atti++] = color_space;
    }
  }

  {
    uint32_t range;
    switch (in_info->colorimetry.range) {
      case GST_VIDEO_COLOR_RANGE_0_255:
        range = EGL_YUV_FULL_RANGE_EXT;
        break;
      case GST_VIDEO_COLOR_RANGE_16_235:
        range = EGL_YUV_NARROW_RANGE_EXT;
        break;
      default:
        range = 0;
        break;
    }
    if (range != 0) {
      attribs[atti++] = EGL_SAMPLE_RANGE_HINT_EXT;
      attribs[atti++] = range;
    }
  }

  /* Add the EGL_NONE sentinel */
  attribs[atti] = EGL_NONE;
  g_assert (atti <= G_N_ELEMENTS (attribs) - 1);

  for (i = 0; i < atti; i++)
    GST_LOG ("attr %i: %" G_GINTPTR_FORMAT, i, attribs[i]);

  img = _gst_egl_image_create (context, EGL_LINUX_DMA_BUF_EXT, NULL, attribs);
  if (!img) {
    GST_WARNING ("eglCreateImage failed: %s",
        gst_egl_get_error_string (eglGetError ()));
    return NULL;
  }

  return gst_egl_image_new_wrapped (context, img, GST_GL_RGBA, NULL,
      (GstEGLImageDestroyNotify) _destroy_egl_image);
}

gboolean
gst_egl_image_export_dmabuf (GstEGLImage * image, int *fd, gint * stride,
    gsize * offset)
{
  EGLBoolean (*gst_eglExportDMABUFImageQueryMESA) (EGLDisplay dpy,
      EGLImageKHR image, int *fourcc, int *num_planes,
      EGLuint64KHR * modifiers);
  EGLBoolean (*gst_eglExportDMABUFImageMESA) (EGLDisplay dpy, EGLImageKHR image,
      int *fds, EGLint * strides, EGLint * offsets);
  GstGLDisplayEGL *display_egl;
  EGLDisplay egl_display = EGL_DEFAULT_DISPLAY;
  int num_planes = 0;
  int egl_fd = 0;
  EGLint egl_stride = 0;
  EGLint egl_offset = 0;

  gst_eglExportDMABUFImageQueryMESA =
      gst_gl_context_get_proc_address (image->context,
      "eglExportDMABUFImageQueryMESA");
  gst_eglExportDMABUFImageMESA =
      gst_gl_context_get_proc_address (image->context,
      "eglExportDMABUFImageMESA");

  if (!gst_eglExportDMABUFImageQueryMESA || !gst_eglExportDMABUFImageMESA)
    return FALSE;

  display_egl =
      (GstGLDisplayEGL *) gst_gl_display_egl_from_gl_display (image->
      context->display);
  if (!display_egl) {
    GST_WARNING_OBJECT (image->context,
        "Failed to retrieve GstGLDisplayEGL from %" GST_PTR_FORMAT,
        image->context->display);
    return FALSE;
  }
  egl_display =
      (EGLDisplay) gst_gl_display_get_handle (GST_GL_DISPLAY (display_egl));
  gst_object_unref (display_egl);

  if (!gst_eglExportDMABUFImageQueryMESA (egl_display, image->image,
          NULL, &num_planes, NULL))
    return FALSE;

  /* Don't allow multi-plane dmabufs */
  if (num_planes > 1)
    return FALSE;

  if (!gst_eglExportDMABUFImageMESA (egl_display, image->image, &egl_fd,
          &egl_stride, &egl_offset))
    return FALSE;

  *fd = egl_fd;
  *stride = egl_stride;
  *offset = egl_offset;

  return TRUE;
}

#endif /* GST_GL_HAVE_DMABUF */
