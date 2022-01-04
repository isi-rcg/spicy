/*
 * GStreamer
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

#ifndef __EXT_GL_GST_GL_UTILS_H__
#define __EXT_GL_GST_GL_UTILS_H__

#include <gst/gl/gl.h>

G_BEGIN_DECLS

gboolean gst_gl_context_gen_shader (GstGLContext * context,
    const gchar * shader_vertex_source,
    const gchar * shader_fragment_source, GstGLShader ** shader);
void gst_gl_multiply_matrix4 (const gfloat * a, const gfloat * b, gfloat * result);
void gst_gl_get_affine_transformation_meta_as_ndc_ext (GstVideoAffineTransformationMeta *
    meta, gfloat * matrix);
void gst_gl_set_affine_transformation_meta_from_ndc_ext (GstVideoAffineTransformationMeta * meta, const gfloat * matrix);

G_END_DECLS

#endif /* __EXT_GL_GST_GL_UTILS_H__ */
