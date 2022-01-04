/*
#             (C) 2008 Hans de Goede <hdegoede@redhat.com>

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; either version 2.1 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA  02110-1335  USA
 */

/* MAKING CHANGES TO THIS FILE??   READ THIS FIRST!!!

   This file implements libv4l2, which offers v4l2_ prefixed versions of
   open/close/etc. The API is 100% the same as directly opening /dev/videoX
   using regular open/close/etc, the big difference is that format conversion
   is done if necessary when capturing. That is if you (try to) set a capture
   format which is not supported by the cam, but is supported by libv4lconvert,
   then the try_fmt / set_fmt will succeed as if the cam supports the format
   and on dqbuf / read the data will be converted for you and returned in
   the request format.

   Important note to people making changes to this file: All functions
   (v4l2_close, v4l2_ioctl, etc.) are designed to function as their regular
   counterpart when they get passed a fd that is not "registered" by libv4l2,
   there are 2 reasons for this:
   1) This allows us to get completely out of the way when dealing with non
   capture devices.
   2) libv4l2 is the base of the v4l2convert.so wrapper lib, which is a .so
   which can be LD_PRELOAD-ed and the overrules the libc's open/close/etc,
   and when opening /dev/videoX or /dev/v4l/ calls v4l2_open.  Because we
   behave as the regular counterpart when the fd is not known (instead of say
   throwing an error), v4l2convert.so can simply call the v4l2_ prefixed
   function for all wrapped functions (except for v4l2_open which will fail
   when not called on a v4l2 device). This way the wrapper does not have to
   keep track of which fd's are being handled by libv4l2, as libv4l2 already
   keeps track of this itself.

   This also means that libv4l2 may not use any of the regular functions
   it mimics, as for example open could be a symbol in v4l2convert.so, which
   in turn will call v4l2_open, so therefore v4l2_open (for example) may not
   use the regular open()!

   Another important note: libv4l2 does conversion for capture usage only, if
   any calls are made which are passed a v4l2_buffer or v4l2_format with a
   v4l2_buf_type which is different from V4L2_BUF_TYPE_VIDEO_CAPTURE, then
   the v4l2_ methods behave exactly the same as their regular counterparts.
   When modifications are made, one should be careful that this behavior is
   preserved.
 */
#ifdef ANDROID
#include <android-config.h>
#else
#include <config.h>
#endif
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "libv4l2.h"
#include "libv4l2-priv.h"
#include "libv4l-plugin.h"

/* Note these flags are stored together with the flags passed to v4l2_fd_open()
   in v4l2_dev_info's flags member, so care should be taken that the do not
   use the same bits! */
#define V4L2_STREAMON			0x0100
#define V4L2_BUFFERS_REQUESTED_BY_READ	0x0200
#define V4L2_STREAM_CONTROLLED_BY_READ	0x0400
#define V4L2_SUPPORTS_READ		0x0800
#define V4L2_STREAM_TOUCHED		0x1000
#define V4L2_USE_READ_FOR_READ		0x2000
#define V4L2_SUPPORTS_TIMEPERFRAME	0x4000

#define V4L2_MMAP_OFFSET_MAGIC      0xABCDEF00u

static void v4l2_adjust_src_fmt_to_fps(int index, int fps);
static void v4l2_set_src_and_dest_format(int index,
		struct v4l2_format *src_fmt, struct v4l2_format *dest_fmt);

static pthread_mutex_t v4l2_open_mutex = PTHREAD_MUTEX_INITIALIZER;
static struct v4l2_dev_info devices[V4L2_MAX_DEVICES] = {
	{ .fd = -1 },
	{ .fd = -1 }, { .fd = -1 }, { .fd = -1 }, { .fd = -1 }, { .fd = -1 },
	{ .fd = -1 }, { .fd = -1 }, { .fd = -1 }, { .fd = -1 }, { .fd = -1 },
	{ .fd = -1 }, { .fd = -1 }, { .fd = -1 }, { .fd = -1 }, { .fd = -1 }
};
static int devices_used;

static int v4l2_ensure_convert_mmap_buf(int index)
{
	if (devices[index].convert_mmap_buf != MAP_FAILED) {
		return 0;
	}

	devices[index].convert_mmap_buf_size =
		devices[index].convert_mmap_frame_size * devices[index].no_frames;

	devices[index].convert_mmap_buf = (void *)SYS_MMAP(NULL,
			devices[index].convert_mmap_buf_size,
			PROT_READ | PROT_WRITE,
			MAP_ANONYMOUS | MAP_PRIVATE,
			-1, 0);

	if (devices[index].convert_mmap_buf == MAP_FAILED) {
		devices[index].convert_mmap_buf_size = 0;

		int saved_err = errno;
		V4L2_LOG_ERR("allocating conversion buffer\n");
		errno = saved_err;
		return -1;
	}

	return 0;
}

static int v4l2_request_read_buffers(int index)
{
	int result;
	struct v4l2_requestbuffers req;

	/* Note we re-request the buffers if they are already requested as the format
	   and thus the needed buffer size may have changed. */
	req.count = (devices[index].no_frames) ? devices[index].no_frames :
		devices[index].nreadbuffers;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;
	result = devices[index].dev_ops->ioctl(devices[index].dev_ops_priv,
			devices[index].fd, VIDIOC_REQBUFS, &req);
	if (result < 0) {
		int saved_err = errno;

		V4L2_LOG("warning reqbuf (%u) failed: %s\n", req.count, strerror(errno));
		errno = saved_err;
		return result;
	}

	if (!devices[index].no_frames && req.count)
		devices[index].flags |= V4L2_BUFFERS_REQUESTED_BY_READ;

	devices[index].no_frames = MIN(req.count, V4L2_MAX_NO_FRAMES);
	return 0;
}

static void v4l2_unrequest_read_buffers(int index)
{
	struct v4l2_requestbuffers req;

	if (!(devices[index].flags & V4L2_BUFFERS_REQUESTED_BY_READ) ||
			devices[index].no_frames == 0)
		return;

	/* (Un)Request buffers, note not all driver support this, and those
	   who do not support it don't need it. */
	req.count = 0;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;
	if (devices[index].dev_ops->ioctl(devices[index].dev_ops_priv,
			devices[index].fd, VIDIOC_REQBUFS, &req) < 0)
		return;

	devices[index].no_frames = MIN(req.count, V4L2_MAX_NO_FRAMES);
	if (devices[index].no_frames == 0)
		devices[index].flags &= ~V4L2_BUFFERS_REQUESTED_BY_READ;
}

static int v4l2_map_buffers(int index)
{
	int result = 0;
	unsigned int i;
	struct v4l2_buffer buf;

	for (i = 0; i < devices[index].no_frames; i++) {
		if (devices[index].frame_pointers[i] != MAP_FAILED)
			continue;

		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
		buf.reserved = buf.reserved2 = 0;
		result = devices[index].dev_ops->ioctl(
				devices[index].dev_ops_priv,
				devices[index].fd, VIDIOC_QUERYBUF, &buf);
		if (result) {
			int saved_err = errno;

			V4L2_PERROR("querying buffer %u", i);
			errno = saved_err;
			break;
		}

		devices[index].frame_pointers[i] = (void *)SYS_MMAP(NULL,
				(size_t)buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, devices[index].fd,
				buf.m.offset);
		if (devices[index].frame_pointers[i] == MAP_FAILED) {
			int saved_err = errno;

			V4L2_PERROR("mmapping buffer %u", i);
			errno = saved_err;
			result = -1;
			break;
		}
		V4L2_LOG("mapped buffer %u at %p\n", i,
				devices[index].frame_pointers[i]);

		devices[index].frame_sizes[i] = buf.length;
	}

	return result;
}

static void v4l2_unmap_buffers(int index)
{
	unsigned int i;

	/* unmap the buffers */
	for (i = 0; i < devices[index].no_frames; i++) {
		if (devices[index].frame_pointers[i] != MAP_FAILED) {
			SYS_MUNMAP(devices[index].frame_pointers[i],
					devices[index].frame_sizes[i]);
			devices[index].frame_pointers[i] = MAP_FAILED;
			V4L2_LOG("unmapped buffer %u\n", i);
		}
	}
}

static int v4l2_streamon(int index)
{
	int result;
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (!(devices[index].flags & V4L2_STREAMON)) {
		result = devices[index].dev_ops->ioctl(
				devices[index].dev_ops_priv,
				devices[index].fd, VIDIOC_STREAMON, &type);
		if (result) {
			int saved_err = errno;

			V4L2_PERROR("turning on stream");
			errno = saved_err;
			return result;
		}
		devices[index].flags |= V4L2_STREAMON;
		devices[index].first_frame = V4L2_IGNORE_FIRST_FRAME_ERRORS;
	}

	return 0;
}

static int v4l2_streamoff(int index)
{
	int result;
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (devices[index].flags & V4L2_STREAMON) {
		result = devices[index].dev_ops->ioctl(
				devices[index].dev_ops_priv,
				devices[index].fd, VIDIOC_STREAMOFF, &type);
		if (result) {
			int saved_err = errno;

			V4L2_PERROR("turning off stream");
			errno = saved_err;
			return result;
		}
		devices[index].flags &= ~V4L2_STREAMON;

		/* Stream off also dequeues all our buffers! */
		devices[index].frame_queued = 0;
	}

	return 0;
}

static int v4l2_queue_read_buffer(int index, int buffer_index)
{
	int result;
	struct v4l2_buffer buf;

	if (devices[index].frame_queued & (1 << buffer_index))
		return 0;

	memset(&buf, 0, sizeof(buf));
	buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	buf.index  = buffer_index;
	result = devices[index].dev_ops->ioctl(devices[index].dev_ops_priv,
			devices[index].fd, VIDIOC_QBUF, &buf);
	if (result) {
		int saved_err = errno;

		V4L2_PERROR("queuing buf %d", buffer_index);
		errno = saved_err;
		return result;
	}

	devices[index].frame_queued |= 1 << buffer_index;
	return 0;
}

static int v4l2_dequeue_and_convert(int index, struct v4l2_buffer *buf,
		unsigned char *dest, int dest_size)
{
	const int max_tries = V4L2_IGNORE_FIRST_FRAME_ERRORS + 1;
	int result, tries = max_tries, frame_info_gen;

	/* Make sure we have the real v4l2 buffers mapped */
	result = v4l2_map_buffers(index);
	if (result)
		return result;

	do {
		frame_info_gen = devices[index].frame_info_generation;
		pthread_mutex_unlock(&devices[index].stream_lock);
		result = devices[index].dev_ops->ioctl(
				devices[index].dev_ops_priv,
				devices[index].fd, VIDIOC_DQBUF, buf);
		pthread_mutex_lock(&devices[index].stream_lock);
		if (result) {
			if (errno != EAGAIN) {
				int saved_err = errno;

				V4L2_PERROR("dequeuing buf");
				errno = saved_err;
			}
			return result;
		}

		devices[index].frame_queued &= ~(1 << buf->index);

		if (frame_info_gen != devices[index].frame_info_generation) {
			errno = -EINVAL;
			return -1;
		}

		result = v4lconvert_convert(devices[index].convert,
				&devices[index].src_fmt, &devices[index].dest_fmt,
				devices[index].frame_pointers[buf->index],
				buf->bytesused, dest ? dest : (devices[index].convert_mmap_buf +
					buf->index * devices[index].convert_mmap_frame_size),
				dest_size);

		if (devices[index].first_frame) {
			/* Always treat convert errors as EAGAIN during the first few frames, as
			   some cams produce bad frames at the start of the stream
			   (hsync and vsync still syncing ??). */
			if (result < 0)
				errno = EAGAIN;
			devices[index].first_frame--;
		}

		if (result < 0) {
			int saved_err = errno;

			if (errno == EAGAIN || errno == EPIPE)
				V4L2_LOG("warning error while converting frame data: %s",
						v4lconvert_get_error_message(devices[index].convert));
			else
				V4L2_LOG_ERR("converting / decoding frame data: %s",
						v4lconvert_get_error_message(devices[index].convert));

			/*
			 * If this is the last try, and the frame is short
			 * we will return the (short) buffer to the caller,
			 * so we must not re-queue it then!
			 */
			if (!(tries == 1 && errno == EPIPE))
				v4l2_queue_read_buffer(index, buf->index);
			errno = saved_err;
		}
		tries--;
	} while (result < 0 && (errno == EAGAIN || errno == EPIPE) && tries);

	if (result < 0 && errno == EAGAIN) {
		V4L2_LOG_ERR("got %d consecutive frame decode errors, last error: %s",
				max_tries, v4lconvert_get_error_message(devices[index].convert));
		errno = EIO;
	}

	if (result < 0 && errno == EPIPE) {
		V4L2_LOG("got %d consecutive short frame errors, "
			 "returning short frame", max_tries);
		result = devices[index].dest_fmt.fmt.pix.sizeimage;
		errno = 0;
	}

	return result;
}

static int v4l2_read_and_convert(int index, unsigned char *dest, int dest_size)
{
	const int max_tries = V4L2_IGNORE_FIRST_FRAME_ERRORS + 1;
	int result, buf_size, tries = max_tries;

	buf_size = devices[index].dest_fmt.fmt.pix.sizeimage;

	if (devices[index].readbuf_size < buf_size) {
		unsigned char *new_buf;

		new_buf = realloc(devices[index].readbuf, buf_size);
		if (!new_buf)
			return -1;

		devices[index].readbuf = new_buf;
		devices[index].readbuf_size = buf_size;
	}

	do {
		result = devices[index].dev_ops->read(
				devices[index].dev_ops_priv,
				devices[index].fd, devices[index].readbuf,
				buf_size);
		if (result <= 0) {
			if (result && errno != EAGAIN) {
				int saved_err = errno;

				V4L2_PERROR("reading");
				errno = saved_err;
			}
			return result;
		}

		result = v4lconvert_convert(devices[index].convert,
				&devices[index].src_fmt, &devices[index].dest_fmt,
				devices[index].readbuf, result, dest, dest_size);

		if (devices[index].first_frame) {
			/* Always treat convert errors as EAGAIN during the first few frames, as
			   some cams produce bad frames at the start of the stream
			   (hsync and vsync still syncing ??). */
			if (result < 0)
				errno = EAGAIN;
			devices[index].first_frame--;
		}

		if (result < 0) {
			int saved_err = errno;

			if (errno == EAGAIN || errno == EPIPE)
				V4L2_LOG("warning error while converting frame data: %s",
						v4lconvert_get_error_message(devices[index].convert));
			else
				V4L2_LOG_ERR("converting / decoding frame data: %s",
						v4lconvert_get_error_message(devices[index].convert));

			errno = saved_err;
		}
		tries--;
	} while (result < 0 && (errno == EAGAIN || errno == EPIPE) && tries);

	if (result < 0 && errno == EAGAIN) {
		V4L2_LOG_ERR("got %d consecutive frame decode errors, last error: %s",
				max_tries, v4lconvert_get_error_message(devices[index].convert));
		errno = EIO;
	}

	if (result < 0 && errno == EPIPE) {
		V4L2_LOG("got %d consecutive short frame errors, "
			 "returning short frame", max_tries);
		result = devices[index].dest_fmt.fmt.pix.sizeimage;
		errno = 0;
	}

	return result;
}

static int v4l2_queue_read_buffers(int index)
{
	unsigned int i;
	int last_error = EIO, queued = 0;

	for (i = 0; i < devices[index].no_frames; i++) {
		/* Don't queue unmapped buffers (should never happen) */
		if (devices[index].frame_pointers[i] != MAP_FAILED) {
			if (v4l2_queue_read_buffer(index, i)) {
				last_error = errno;
				continue;
			}
			queued++;
		}
	}

	if (!queued) {
		errno = last_error;
		return -1;
	}
	return 0;
}

static int v4l2_activate_read_stream(int index)
{
	int result;

	if ((devices[index].flags & V4L2_STREAMON) || devices[index].frame_queued) {
		errno = EBUSY;
		return -1;
	}

	result = v4l2_request_read_buffers(index);
	if (!result)
		result = v4l2_map_buffers(index);
	if (!result)
		result = v4l2_queue_read_buffers(index);
	if (result)
		return result;

	devices[index].flags |= V4L2_STREAM_CONTROLLED_BY_READ;

	return v4l2_streamon(index);
}

static int v4l2_deactivate_read_stream(int index)
{
	int result;

	result = v4l2_streamoff(index);
	if (result)
		return result;

	/* No need to dequeue our buffers, streamoff does that for us */

	v4l2_unmap_buffers(index);

	v4l2_unrequest_read_buffers(index);

	devices[index].flags &= ~V4L2_STREAM_CONTROLLED_BY_READ;

	return 0;
}

static int v4l2_needs_conversion(int index)
{
	if (devices[index].convert == NULL)
		return 0;

	return v4lconvert_needs_conversion(devices[index].convert,
			&devices[index].src_fmt, &devices[index].dest_fmt);
}

static void v4l2_set_conversion_buf_params(int index, struct v4l2_buffer *buf)
{
	if (!v4l2_needs_conversion(index))
		return;

	/* This may happen if the ioctl failed */
	if (buf->index >= devices[index].no_frames)
		buf->index = 0;

	buf->m.offset = V4L2_MMAP_OFFSET_MAGIC | buf->index;
	buf->length = devices[index].convert_mmap_frame_size;
	if (devices[index].frame_map_count[buf->index])
		buf->flags |= V4L2_BUF_FLAG_MAPPED;
	else
		buf->flags &= ~V4L2_BUF_FLAG_MAPPED;
}

static int v4l2_buffers_mapped(int index)
{
	unsigned int i;

	if (!v4l2_needs_conversion(index)) {
		/* Normal (no conversion) mode */
		struct v4l2_buffer buf;

		for (i = 0; i < devices[index].no_frames; i++) {
			buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory = V4L2_MEMORY_MMAP;
			buf.index = i;
			buf.reserved = buf.reserved2 = 0;
			if (devices[index].dev_ops->ioctl(
					devices[index].dev_ops_priv,
					devices[index].fd, VIDIOC_QUERYBUF,
					&buf)) {
				int saved_err = errno;

				V4L2_PERROR("querying buffer %u", i);
				errno = saved_err;
				break;
			}
			if (buf.flags & V4L2_BUF_FLAG_MAPPED)
				break;
		}
	} else {
		/* Conversion mode */
		for (i = 0; i < devices[index].no_frames; i++)
			if (devices[index].frame_map_count[i])
				break;
	}

	if (i != devices[index].no_frames)
		V4L2_LOG("v4l2_buffers_mapped(): buffers still mapped\n");

	return i != devices[index].no_frames;
}

static void v4l2_update_fps(int index, struct v4l2_streamparm *parm)
{
	if ((devices[index].flags & V4L2_SUPPORTS_TIMEPERFRAME) &&
	    parm->parm.capture.timeperframe.numerator != 0) {
		int fps = parm->parm.capture.timeperframe.denominator;
		fps += parm->parm.capture.timeperframe.numerator - 1;
		fps /= parm->parm.capture.timeperframe.numerator;
		devices[index].fps = fps;
	} else
		devices[index].fps = 0;
}

int v4l2_open(const char *file, int oflag, ...)
{
	int fd;

	/* original open code */
	if (oflag & O_CREAT) {
		va_list ap;
		mode_t mode;

		va_start(ap, oflag);
		mode = va_arg(ap, PROMOTED_MODE_T);

		fd = SYS_OPEN(file, oflag, mode);

		va_end(ap);
	} else {
		fd = SYS_OPEN(file, oflag, 0);
	}
	/* end of original open code */

	if (fd == -1)
		return fd;

	if (v4l2_fd_open(fd, 0) == -1) {
		int saved_err = errno;

		SYS_CLOSE(fd);
		errno = saved_err;
		return -1;
	}

	return fd;
}

int v4l2_fd_open(int fd, int v4l2_flags)
{
	int i, index;
	char *lfname;
	struct v4l2_capability cap;
	struct v4l2_format fmt = { 0, };
	struct v4l2_streamparm parm = { 0, };
	struct v4lconvert_data *convert = NULL;
	void *plugin_library;
	void *dev_ops_priv;
	const struct libv4l_dev_ops *dev_ops;
	long page_size;

	v4l2_plugin_init(fd, &plugin_library, &dev_ops_priv, &dev_ops);

	/* If no log file was set by the app, see if one was specified through the
	   environment */
	if (!v4l2_log_file) {
		lfname = getenv("LIBV4L2_LOG_FILENAME");
		if (lfname)
			v4l2_log_file = fopen(lfname, "w");
	}

	/* Get page_size (for mmap emulation) */
	page_size = sysconf(_SC_PAGESIZE);
	if (page_size < 0) {
		int saved_err = errno;
		V4L2_LOG_ERR("unable to retrieve page size: %s\n",
			     strerror(errno));
		v4l2_plugin_cleanup(plugin_library, dev_ops_priv, dev_ops);
		errno = saved_err;
		return -1;
	}

	/* check that this is a v4l2 device */
	if (dev_ops->ioctl(dev_ops_priv, fd, VIDIOC_QUERYCAP, &cap)) {
		int saved_err = errno;
		V4L2_LOG_ERR("getting capabilities: %s\n", strerror(errno));
		v4l2_plugin_cleanup(plugin_library, dev_ops_priv, dev_ops);
		errno = saved_err;
		return -1;
	}

	if (cap.capabilities & V4L2_CAP_DEVICE_CAPS)
		cap.capabilities = cap.device_caps;
	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) ||
	    !(cap.capabilities & (V4L2_CAP_STREAMING | V4L2_CAP_READWRITE)))
		goto no_capture;

	/* Get current cam format */
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (dev_ops->ioctl(dev_ops_priv, fd, VIDIOC_G_FMT, &fmt)) {
		int saved_err = errno;
		V4L2_LOG_ERR("getting pixformat: %s\n", strerror(errno));
		v4l2_plugin_cleanup(plugin_library, dev_ops_priv, dev_ops);
		errno = saved_err;
		return -1;
	}

	/* Check for frame rate setting support */
	parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (dev_ops->ioctl(dev_ops_priv, fd, VIDIOC_G_PARM, &parm))
		parm.type = 0;

	/* init libv4lconvert */
	if (!(v4l2_flags & V4L2_DISABLE_CONVERSION)) {
		convert = v4lconvert_create_with_dev_ops(fd, dev_ops_priv, dev_ops);
		if (!convert) {
			int saved_err = errno;
			v4l2_plugin_cleanup(plugin_library, dev_ops_priv,
					    dev_ops);
			errno = saved_err;
			return -1;
		}
	}

no_capture:
	/* So we have a v4l2 capture device, register it in our devices array */
	pthread_mutex_lock(&v4l2_open_mutex);
	for (index = 0; index < V4L2_MAX_DEVICES; index++) {
		if (devices[index].fd == -1) {
			devices[index].fd = fd;
			devices[index].plugin_library = plugin_library;
			devices[index].dev_ops_priv = dev_ops_priv;
			devices[index].dev_ops = dev_ops;
			break;
		}
	}
	pthread_mutex_unlock(&v4l2_open_mutex);

	if (index == V4L2_MAX_DEVICES) {
		V4L2_LOG_ERR("attempting to open more then %d video devices\n",
				V4L2_MAX_DEVICES);
		v4l2_plugin_cleanup(plugin_library, dev_ops_priv, dev_ops);
		errno = EBUSY;
		return -1;
	}

	devices[index].flags = v4l2_flags;
	if (cap.capabilities & V4L2_CAP_READWRITE)
		devices[index].flags |= V4L2_SUPPORTS_READ;
	if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
		devices[index].flags |= V4L2_USE_READ_FOR_READ;
		/* This device only supports read so the stream gets started by the
		   driver on the first read */
		devices[index].first_frame = V4L2_IGNORE_FIRST_FRAME_ERRORS;
	}
	if ((parm.type == V4L2_BUF_TYPE_VIDEO_CAPTURE) &&
	    (parm.parm.capture.capability & V4L2_CAP_TIMEPERFRAME))
		devices[index].flags |= V4L2_SUPPORTS_TIMEPERFRAME;
	devices[index].open_count = 1;
	devices[index].page_size = page_size;
	devices[index].src_fmt  = fmt;
	devices[index].dest_fmt = fmt;
	v4l2_set_src_and_dest_format(index, &devices[index].src_fmt,
				     &devices[index].dest_fmt);

	pthread_mutex_init(&devices[index].stream_lock, NULL);

	devices[index].no_frames = 0;
	devices[index].nreadbuffers = V4L2_DEFAULT_NREADBUFFERS;
	devices[index].convert = convert;
	devices[index].convert_mmap_buf = MAP_FAILED;
	devices[index].convert_mmap_buf_size = 0;
	for (i = 0; i < V4L2_MAX_NO_FRAMES; i++) {
		devices[index].frame_pointers[i] = MAP_FAILED;
		devices[index].frame_map_count[i] = 0;
	}
	devices[index].frame_queued = 0;
	devices[index].readbuf = NULL;
	devices[index].readbuf_size = 0;

	if (index >= devices_used)
		devices_used = index + 1;

	/* Note we always tell v4lconvert to optimize src fmt selection for
	   our default fps, the only exception is the app explicitly selecting
	   a frame rate using the S_PARM ioctl after a S_FMT */
	if (devices[index].convert)
		v4lconvert_set_fps(devices[index].convert, V4L2_DEFAULT_FPS);
	v4l2_update_fps(index, &parm);

	V4L2_LOG("open: %d\n", fd);

	return fd;
}

/* Is this an fd for which we are emulating v4l1 ? */
static int v4l2_get_index(int fd)
{
	int index;

	/* We never handle fd -1 */
	if (fd == -1)
		return -1;

	for (index = 0; index < devices_used; index++)
		if (devices[index].fd == fd)
			break;

	if (index == devices_used)
		return -1;

	return index;
}


int v4l2_close(int fd)
{
	int index, result;

	index = v4l2_get_index(fd);
	if (index == -1)
		return SYS_CLOSE(fd);

	/* Abuse stream_lock to stop 2 closes from racing and trying to free
	   the resources twice */
	pthread_mutex_lock(&devices[index].stream_lock);
	devices[index].open_count--;
	result = devices[index].open_count != 0;
	pthread_mutex_unlock(&devices[index].stream_lock);

	if (result)
		return 0;

	v4l2_plugin_cleanup(devices[index].plugin_library,
			devices[index].dev_ops_priv,
			devices[index].dev_ops);

	/* Free resources */
	v4l2_unmap_buffers(index);
	if (devices[index].convert_mmap_buf != MAP_FAILED) {
		if (v4l2_buffers_mapped(index)) {
			if (!devices[index].gone)
				V4L2_LOG_WARN("v4l2 mmap buffers still mapped on close()\n");
		} else {
			SYS_MUNMAP(devices[index].convert_mmap_buf,
					devices[index].convert_mmap_buf_size);
		}
		devices[index].convert_mmap_buf = MAP_FAILED;
		devices[index].convert_mmap_buf_size = 0;
	}
	v4lconvert_destroy(devices[index].convert);
	free(devices[index].readbuf);
	devices[index].readbuf = NULL;
	devices[index].readbuf_size = 0;

	/* Remove the fd from our list of managed fds before closing it, because as
	   soon as we've done the actual close, the fd maybe returned by an open() in
	   another thread and we don't want to intercept calls to this new fd. */
	devices[index].fd = -1;

	/* Since we've marked the fd as no longer used, and freed the resources,
	   redo the close in case it was interrupted */
	do {
		result = SYS_CLOSE(fd);
	} while (result == -1 && errno == EINTR);

	V4L2_LOG("close: %d\n", fd);

	return result;
}

int v4l2_dup(int fd)
{
	int index = v4l2_get_index(fd);

	if (index == -1)
		return syscall(SYS_dup, fd);

	devices[index].open_count++;

	return fd;
}

static int v4l2_check_buffer_change_ok(int index)
{
	devices[index].frame_info_generation++;
	v4l2_unmap_buffers(index);

	/* Check if the app itself still is using the stream */
	if (v4l2_buffers_mapped(index) ||
			(!(devices[index].flags & V4L2_STREAM_CONTROLLED_BY_READ) &&
			 ((devices[index].flags & V4L2_STREAMON) ||
			  devices[index].frame_queued))) {
		V4L2_LOG("v4l2_check_buffer_change_ok(): stream busy\n");
		errno = EBUSY;
		return -1;
	}

	/* We may change from convert to non conversion mode and
	   v4l2_unrequest_read_buffers may change the no_frames, so free the
	   convert mmap buffer */
	SYS_MUNMAP(devices[index].convert_mmap_buf,
			devices[index].convert_mmap_buf_size);
	devices[index].convert_mmap_buf = MAP_FAILED;
	devices[index].convert_mmap_buf_size = 0;

	if (devices[index].flags & V4L2_STREAM_CONTROLLED_BY_READ) {
		V4L2_LOG("deactivating read-stream for settings change\n");
		return v4l2_deactivate_read_stream(index);
	}

	return 0;
}

static int v4l2_pix_fmt_compat(struct v4l2_format *a, struct v4l2_format *b)
{
	if (a->fmt.pix.width == b->fmt.pix.width &&
			a->fmt.pix.height == b->fmt.pix.height &&
			a->fmt.pix.pixelformat == b->fmt.pix.pixelformat &&
			a->fmt.pix.field == b->fmt.pix.field)
		return 1;

	return 0;
}

static int v4l2_pix_fmt_identical(struct v4l2_format *a, struct v4l2_format *b)
{
	if (v4l2_pix_fmt_compat(a, b) &&
	    a->fmt.pix.bytesperline == b->fmt.pix.bytesperline &&
	    a->fmt.pix.sizeimage == b->fmt.pix.sizeimage)
		return 1;

	return 0;
}

static void v4l2_set_src_and_dest_format(int index,
		struct v4l2_format *src_fmt, struct v4l2_format *dest_fmt)
{
	/*
	 * When a user does a try_fmt with the current dest_fmt and the
	 * dest_fmt is a supported one we will align the resolution (see
	 * libv4lconvert_try_fmt). We do this here too, in case dest_fmt gets
	 * set without having gone through libv4lconvert_try_fmt, so that a
	 * try_fmt on the result of a get_fmt always returns the same result.
	 */
	if (v4lconvert_supported_dst_format(dest_fmt->fmt.pix.pixelformat)) {
		dest_fmt->fmt.pix.width &= ~7;
		dest_fmt->fmt.pix.height &= ~1;
	}

	/* Sigh some drivers (pwc) do not properly reflect what one really gets
	   after a s_fmt in their try_fmt answer. So update dest format (which we
	   report as result from s_fmt / g_fmt to the app) with all info from the src
	   format not changed by conversion */
	dest_fmt->fmt.pix.field = src_fmt->fmt.pix.field;
	dest_fmt->fmt.pix.colorspace = src_fmt->fmt.pix.colorspace;
	dest_fmt->fmt.pix.xfer_func = src_fmt->fmt.pix.xfer_func;
	dest_fmt->fmt.pix.ycbcr_enc = src_fmt->fmt.pix.ycbcr_enc;
	dest_fmt->fmt.pix.quantization = src_fmt->fmt.pix.quantization;
	/* When we're not converting use bytesperline and imagesize from src_fmt */
	if (v4l2_pix_fmt_compat(src_fmt, dest_fmt)) {
		dest_fmt->fmt.pix.bytesperline = src_fmt->fmt.pix.bytesperline;
		dest_fmt->fmt.pix.sizeimage = src_fmt->fmt.pix.sizeimage;
	} else
		v4lconvert_fixup_fmt(dest_fmt);

	devices[index].src_fmt = *src_fmt;
	devices[index].dest_fmt = *dest_fmt;
	/* round up to full page size */
	devices[index].convert_mmap_frame_size =
		(((dest_fmt->fmt.pix.sizeimage + devices[index].page_size - 1)
		/ devices[index].page_size) * devices[index].page_size);
}

static int v4l2_s_fmt(int index, struct v4l2_format *dest_fmt)
{
	struct v4l2_format src_fmt;
	struct v4l2_pix_format req_pix_fmt;
	int result;

	if (v4l2_log_file) {
		int pixfmt = dest_fmt->fmt.pix.pixelformat;

		fprintf(v4l2_log_file, "VIDIOC_S_FMT app requesting: %c%c%c%c\n",
				pixfmt & 0xff,
				(pixfmt >> 8) & 0xff,
				(pixfmt >> 16) & 0xff,
				pixfmt >> 24);
	}

	result = v4lconvert_try_format(devices[index].convert,
				       dest_fmt, &src_fmt);
	if (result) {
		int saved_err = errno;
		V4L2_LOG("S_FMT error trying format: %s\n", strerror(errno));
		errno = saved_err;
		return result;
	}

	if (src_fmt.fmt.pix.pixelformat != dest_fmt->fmt.pix.pixelformat &&
			v4l2_log_file) {
		int pixfmt = src_fmt.fmt.pix.pixelformat;

		fprintf(v4l2_log_file,
			"VIDIOC_S_FMT converting from: %c%c%c%c\n",
			pixfmt & 0xff, (pixfmt >> 8) & 0xff,
			(pixfmt >> 16) & 0xff, pixfmt >> 24);
	}

	result = v4l2_check_buffer_change_ok(index);
	if (result)
		return result;

	req_pix_fmt = src_fmt.fmt.pix;
	result = devices[index].dev_ops->ioctl(devices[index].dev_ops_priv,
					       devices[index].fd,
					       VIDIOC_S_FMT, &src_fmt);
	if (result) {
		int saved_err = errno;
		V4L2_PERROR("setting pixformat");
		/* Report to the app dest_fmt has not changed */
		*dest_fmt = devices[index].dest_fmt;
		errno = saved_err;
		return result;
	}

	/* See if we've gotten what try_fmt promised us
	   (this check should never fail) */
	if (src_fmt.fmt.pix.width != req_pix_fmt.width ||
	    src_fmt.fmt.pix.height != req_pix_fmt.height ||
	    src_fmt.fmt.pix.pixelformat != req_pix_fmt.pixelformat) {
		V4L2_LOG_ERR("set_fmt gave us a different result then try_fmt!\n");
		/* Not what we expected / wanted, disable conversion */
		*dest_fmt = src_fmt;
	}

	v4l2_set_src_and_dest_format(index, &src_fmt, dest_fmt);

	if (devices[index].flags & V4L2_SUPPORTS_TIMEPERFRAME) {
		struct v4l2_streamparm parm = {
			.type = V4L2_BUF_TYPE_VIDEO_CAPTURE,
		};
		if (devices[index].dev_ops->ioctl(devices[index].dev_ops_priv,
						  devices[index].fd,
						  VIDIOC_G_PARM, &parm))
			return 0;
		v4l2_update_fps(index, &parm);
	}

	return 0;
}

int v4l2_ioctl(int fd, unsigned long int request, ...)
{
	void *arg;
	va_list ap;
	int result, index, saved_err;
	int is_capture_request = 0, stream_needs_locking = 0;

	va_start(ap, request);
	arg = va_arg(ap, void *);
	va_end(ap);

	index = v4l2_get_index(fd);
	if (index == -1)
		return SYS_IOCTL(fd, request, arg);

	/* Apparently the kernel and / or glibc ignore the 32 most significant bits
	   when long = 64 bits, and some applications pass an int holding the req to
	   ioctl, causing it to get sign extended, depending upon this behavior */
	request = (unsigned int)request;

	if (devices[index].convert == NULL)
		goto no_capture_request;

	/* Is this a capture request and do we need to take the stream lock? */
	switch (request) {
	case VIDIOC_QUERYCAP:
	case VIDIOC_QUERYCTRL:
	case VIDIOC_G_CTRL:
	case VIDIOC_S_CTRL:
	case VIDIOC_G_EXT_CTRLS:
	case VIDIOC_TRY_EXT_CTRLS:
	case VIDIOC_S_EXT_CTRLS:
	case VIDIOC_ENUM_FRAMESIZES:
	case VIDIOC_ENUM_FRAMEINTERVALS:
		is_capture_request = 1;
		break;
	case VIDIOC_ENUM_FMT:
		if (((struct v4l2_fmtdesc *)arg)->type ==
				V4L2_BUF_TYPE_VIDEO_CAPTURE)
			is_capture_request = 1;
		break;
	case VIDIOC_TRY_FMT:
		if (((struct v4l2_format *)arg)->type ==
				V4L2_BUF_TYPE_VIDEO_CAPTURE)
			is_capture_request = 1;
		break;
	case VIDIOC_S_FMT:
	case VIDIOC_G_FMT:
		if (((struct v4l2_format *)arg)->type ==
				V4L2_BUF_TYPE_VIDEO_CAPTURE) {
			is_capture_request = 1;
			stream_needs_locking = 1;
		}
		break;
	case VIDIOC_REQBUFS:
		if (((struct v4l2_requestbuffers *)arg)->type ==
				V4L2_BUF_TYPE_VIDEO_CAPTURE) {
			is_capture_request = 1;
			stream_needs_locking = 1;
		}
		break;
	case VIDIOC_QUERYBUF:
	case VIDIOC_QBUF:
	case VIDIOC_DQBUF:
		if (((struct v4l2_buffer *)arg)->type ==
				V4L2_BUF_TYPE_VIDEO_CAPTURE) {
			is_capture_request = 1;
			stream_needs_locking = 1;
		}
		break;
	case VIDIOC_STREAMON:
	case VIDIOC_STREAMOFF:
		if (*((enum v4l2_buf_type *)arg) ==
				V4L2_BUF_TYPE_VIDEO_CAPTURE) {
			is_capture_request = 1;
			stream_needs_locking = 1;
		}
		break;
	case VIDIOC_S_PARM:
		if (((struct v4l2_streamparm *)arg)->type ==
				V4L2_BUF_TYPE_VIDEO_CAPTURE) {
			is_capture_request = 1;
			if (devices[index].flags & V4L2_SUPPORTS_TIMEPERFRAME)
				stream_needs_locking = 1;
		}
		break;
	case VIDIOC_S_STD:
	case VIDIOC_S_INPUT:
	case VIDIOC_S_DV_TIMINGS:
		is_capture_request = 1;
		stream_needs_locking = 1;
		break;		
	}

	if (!is_capture_request) {
no_capture_request:
		result = devices[index].dev_ops->ioctl(
				devices[index].dev_ops_priv,
				fd, request, arg);
		saved_err = errno;
		v4l2_log_ioctl(request, arg, result);
		errno = saved_err;
		return result;
	}


	if (stream_needs_locking) {
		pthread_mutex_lock(&devices[index].stream_lock);
		/* If this is the first stream-related ioctl, and we should only allow
		   libv4lconvert supported destination formats (so that it can do flipping,
		   processing, etc.) and the current destination format is not supported,
		   try setting the format to RGB24 (which is a supported dest. format). */
		if (!(devices[index].flags & V4L2_STREAM_TOUCHED) &&
				v4lconvert_supported_dst_fmt_only(devices[index].convert) &&
				!v4lconvert_supported_dst_format(
					devices[index].dest_fmt.fmt.pix.pixelformat)) {
			struct v4l2_format fmt = devices[index].dest_fmt;

			V4L2_LOG("Setting pixelformat to RGB24 (supported_dst_fmt_only)");
			fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
			v4l2_s_fmt(index, &fmt);
			V4L2_LOG("Done setting pixelformat (supported_dst_fmt_only)");
		}
		devices[index].flags |= V4L2_STREAM_TOUCHED;
	}

	switch (request) {
	case VIDIOC_QUERYCTRL:
		result = v4lconvert_vidioc_queryctrl(devices[index].convert, arg);
		break;

	case VIDIOC_G_CTRL:
		result = v4lconvert_vidioc_g_ctrl(devices[index].convert, arg);
		break;

	case VIDIOC_S_CTRL:
		result = v4lconvert_vidioc_s_ctrl(devices[index].convert, arg);
		break;

	case VIDIOC_G_EXT_CTRLS:
		result = v4lconvert_vidioc_g_ext_ctrls(devices[index].convert, arg);
		break;

	case VIDIOC_TRY_EXT_CTRLS:
		result = v4lconvert_vidioc_try_ext_ctrls(devices[index].convert, arg);
		break;

	case VIDIOC_S_EXT_CTRLS:
		result = v4lconvert_vidioc_s_ext_ctrls(devices[index].convert, arg);
		break;

	case VIDIOC_QUERYCAP: {
		struct v4l2_capability *cap = arg;

		result = devices[index].dev_ops->ioctl(
				devices[index].dev_ops_priv,
				fd, VIDIOC_QUERYCAP, cap);
		if (result == 0) {
			/* We always support read() as we fake it using mmap mode */
			cap->capabilities |= V4L2_CAP_READWRITE;
			cap->device_caps |= V4L2_CAP_READWRITE;
		}
		break;
	}

	case VIDIOC_ENUM_FMT:
		result = v4lconvert_enum_fmt(devices[index].convert, arg);
		break;

	case VIDIOC_ENUM_FRAMESIZES:
		result = v4lconvert_enum_framesizes(devices[index].convert, arg);
		break;

	case VIDIOC_ENUM_FRAMEINTERVALS:
		result = v4lconvert_enum_frameintervals(devices[index].convert, arg);
		if (result)
			V4L2_LOG("ENUM_FRAMEINTERVALS Error: %s",
					v4lconvert_get_error_message(devices[index].convert));
		break;

	case VIDIOC_TRY_FMT:
		result = v4lconvert_try_format(devices[index].convert,
					       arg, NULL);
		break;

	case VIDIOC_S_FMT:
		result = v4l2_s_fmt(index, arg);
		break;

	case VIDIOC_G_FMT: {
		struct v4l2_format *fmt = arg;

		*fmt = devices[index].dest_fmt;
		result = 0;
		break;
	}

	case VIDIOC_S_STD:
	case VIDIOC_S_INPUT:
	case VIDIOC_S_DV_TIMINGS: {
		struct v4l2_format src_fmt = { 0 };
		unsigned int orig_dest_pixelformat =
			devices[index].dest_fmt.fmt.pix.pixelformat;

		result = devices[index].dev_ops->ioctl(
				devices[index].dev_ops_priv,
				fd, request, arg);
		if (result)
			break;

		/* These ioctls may have changed the device's fmt */
		src_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		result = devices[index].dev_ops->ioctl(
				devices[index].dev_ops_priv,
				fd, VIDIOC_G_FMT, &src_fmt);
		if (result) {
			V4L2_PERROR("getting pixformat after %s",
				     v4l2_ioctls[_IOC_NR(request)]);
			result = 0; /* The original command did succeed */
			break;
		}

		if (v4l2_pix_fmt_compat(&devices[index].src_fmt, &src_fmt)) {
			v4l2_set_src_and_dest_format(index, &src_fmt,
						     &devices[index].dest_fmt);
			break;
		}

		/* The fmt has been changed, remember the new format ... */
		devices[index].src_fmt  = src_fmt;
		devices[index].dest_fmt = src_fmt;
		v4l2_set_src_and_dest_format(index, &devices[index].src_fmt,
					     &devices[index].dest_fmt);
		/* and try to restore the last set destination pixelformat. */
		src_fmt.fmt.pix.pixelformat = orig_dest_pixelformat;
		result = v4l2_s_fmt(index, &src_fmt);
		if (result) {
			V4L2_LOG_WARN("restoring destination pixelformat after %s failed\n",
				      v4l2_ioctls[_IOC_NR(request)]);
			result = 0; /* The original command did succeed */
		}

		break;
	}

	case VIDIOC_REQBUFS: {
		struct v4l2_requestbuffers *req = arg;

		/* IMPROVEME (maybe?) add support for userptr's? */
		if (req->memory != V4L2_MEMORY_MMAP) {
			errno = EINVAL;
			result = -1;
			break;
		}

		result = v4l2_check_buffer_change_ok(index);
		if (result)
			break;

		/* No more buffers then we can manage please */
		if (req->count > V4L2_MAX_NO_FRAMES)
			req->count = V4L2_MAX_NO_FRAMES;

		result = devices[index].dev_ops->ioctl(
				devices[index].dev_ops_priv,
				fd, VIDIOC_REQBUFS, req);
		if (result < 0)
			break;
		result = 0; /* some drivers return the number of buffers on success */

		devices[index].no_frames = MIN(req->count, V4L2_MAX_NO_FRAMES);
		devices[index].flags &= ~V4L2_BUFFERS_REQUESTED_BY_READ;
		break;
	}

	case VIDIOC_QUERYBUF: {
		struct v4l2_buffer *buf = arg;

		if (devices[index].flags & V4L2_STREAM_CONTROLLED_BY_READ) {
			result = v4l2_deactivate_read_stream(index);
			if (result)
				break;
		}

		/* Do a real query even when converting to let the driver fill in
		   things like buf->field */
		result = devices[index].dev_ops->ioctl(
				devices[index].dev_ops_priv,
				fd, VIDIOC_QUERYBUF, buf);

		v4l2_set_conversion_buf_params(index, buf);
		break;
	}

	case VIDIOC_QBUF: {
		struct v4l2_buffer *buf = arg;

		if (devices[index].flags & V4L2_STREAM_CONTROLLED_BY_READ) {
			result = v4l2_deactivate_read_stream(index);
			if (result)
				break;
		}

		/* With some drivers the buffers must be mapped before queuing */
		if (v4l2_needs_conversion(index)) {
			result = v4l2_map_buffers(index);
			if (result)
				break;
		}

		result = devices[index].dev_ops->ioctl(
				devices[index].dev_ops_priv,
				fd, VIDIOC_QBUF, arg);

		v4l2_set_conversion_buf_params(index, buf);
		break;
	}

	case VIDIOC_DQBUF: {
		struct v4l2_buffer *buf = arg;

		if (devices[index].flags & V4L2_STREAM_CONTROLLED_BY_READ) {
			result = v4l2_deactivate_read_stream(index);
			if (result)
				break;
		}

		if (!v4l2_needs_conversion(index)) {
			pthread_mutex_unlock(&devices[index].stream_lock);
			result = devices[index].dev_ops->ioctl(
					devices[index].dev_ops_priv,
					fd, VIDIOC_DQBUF, buf);
			pthread_mutex_lock(&devices[index].stream_lock);
			if (result) {
				saved_err = errno;
				V4L2_PERROR("dequeuing buf");
				errno = saved_err;
			}
			break;
		}

		/* An application can do a DQBUF before mmap-ing in the buffer,
		   but we need the buffer _now_ to write our converted data
		   to it! */
		result = v4l2_ensure_convert_mmap_buf(index);
		if (result)
			break;

		result = v4l2_dequeue_and_convert(index, buf, 0,
				devices[index].convert_mmap_frame_size);
		if (result >= 0) {
			buf->bytesused = result;
			result = 0;
		}

		v4l2_set_conversion_buf_params(index, buf);
		break;
	}

	case VIDIOC_STREAMON:
	case VIDIOC_STREAMOFF:
		if (devices[index].flags & V4L2_STREAM_CONTROLLED_BY_READ) {
			result = v4l2_deactivate_read_stream(index);
			if (result)
				break;
		}

		if (request == VIDIOC_STREAMON)
			result = v4l2_streamon(index);
		else
			result = v4l2_streamoff(index);
		break;

	case VIDIOC_S_PARM: {
		struct v4l2_streamparm *parm = arg;

		/* See if libv4lconvert wishes to use a different src_fmt
		   for the new frame rate and set that first */
		if ((devices[index].flags & V4L2_SUPPORTS_TIMEPERFRAME) &&
		    parm->parm.capture.timeperframe.numerator != 0) {
			int fps = parm->parm.capture.timeperframe.denominator;
			fps += parm->parm.capture.timeperframe.numerator - 1;
			fps /= parm->parm.capture.timeperframe.numerator;
			v4l2_adjust_src_fmt_to_fps(index, fps);
		}

		result = devices[index].dev_ops->ioctl(
						devices[index].dev_ops_priv,
						fd, VIDIOC_S_PARM, parm);
		if (result)
			break;

		v4l2_update_fps(index, parm);
		break;
	}

	default:
		result = devices[index].dev_ops->ioctl(
				devices[index].dev_ops_priv,
				fd, request, arg);
		break;
	}

	if (stream_needs_locking)
		pthread_mutex_unlock(&devices[index].stream_lock);

	saved_err = errno;
	v4l2_log_ioctl(request, arg, result);
	errno = saved_err;

	return result;
}

static void v4l2_adjust_src_fmt_to_fps(int index, int fps)
{
	struct v4l2_pix_format req_pix_fmt;
	struct v4l2_format src_fmt;
	struct v4l2_format dest_fmt = devices[index].dest_fmt;
	struct v4l2_format orig_src_fmt = devices[index].src_fmt;
	struct v4l2_format orig_dest_fmt = devices[index].dest_fmt;
	int r;

	if (fps == devices[index].fps)
		return;

	if (v4l2_check_buffer_change_ok(index))
		return;

	v4lconvert_set_fps(devices[index].convert, fps);
	r = v4lconvert_try_format(devices[index].convert, &dest_fmt, &src_fmt);
	v4lconvert_set_fps(devices[index].convert, V4L2_DEFAULT_FPS);
	if (r)
		return;

	if (orig_src_fmt.fmt.pix.pixelformat == src_fmt.fmt.pix.pixelformat ||
	    !v4l2_pix_fmt_compat(&orig_dest_fmt, &dest_fmt))
		return;

	req_pix_fmt = src_fmt.fmt.pix;
	if (devices[index].dev_ops->ioctl(devices[index].dev_ops_priv,
			devices[index].fd, VIDIOC_S_FMT, &src_fmt))
		return;

	v4l2_set_src_and_dest_format(index, &src_fmt, &dest_fmt);

	/* Check we've gotten what try_fmt promised us and that the
	   new dest fmt matches the original, if this is true we're done. */
	if (src_fmt.fmt.pix.width == req_pix_fmt.width &&
	    src_fmt.fmt.pix.height == req_pix_fmt.height &&
	    src_fmt.fmt.pix.pixelformat == req_pix_fmt.pixelformat &&
	    v4l2_pix_fmt_identical(&orig_dest_fmt, &dest_fmt)) {
		if (v4l2_log_file) {
			int pixfmt = src_fmt.fmt.pix.pixelformat;
			fprintf(v4l2_log_file,
				"new src fmt for fps change: %c%c%c%c\n",
				pixfmt & 0xff, (pixfmt >> 8) & 0xff,
				(pixfmt >> 16) & 0xff, pixfmt >> 24);
		}
		return;
	}

	/* Not identical!! */
	V4L2_LOG_WARN("dest fmt changed after adjusting src fmt for fps "
		      "change, restoring original src fmt");
	src_fmt = orig_src_fmt;
	dest_fmt = orig_dest_fmt;
	req_pix_fmt = src_fmt.fmt.pix;
	if (devices[index].dev_ops->ioctl(devices[index].dev_ops_priv,
			devices[index].fd, VIDIOC_S_FMT, &src_fmt)) {
		V4L2_PERROR("restoring src fmt");
		return;
	}
	v4l2_set_src_and_dest_format(index, &src_fmt, &dest_fmt);
	if (src_fmt.fmt.pix.width != req_pix_fmt.width ||
	    src_fmt.fmt.pix.height != req_pix_fmt.height ||
	    src_fmt.fmt.pix.pixelformat != req_pix_fmt.pixelformat ||
	    !v4l2_pix_fmt_identical(&orig_dest_fmt, &dest_fmt))
		V4L2_LOG_ERR("dest fmt different after restoring src fmt");
}

ssize_t v4l2_read(int fd, void *dest, size_t n)
{
	ssize_t result;
	int saved_errno;
	int index = v4l2_get_index(fd);

	if (index == -1)
		return SYS_READ(fd, dest, n);

	if (!devices[index].dev_ops->read) {
		errno = EINVAL;
		return -1;
	}

	pthread_mutex_lock(&devices[index].stream_lock);

	/* When not converting and the device supports read(), let the kernel handle
	   it */
	if (devices[index].convert == NULL ||
	    ((devices[index].flags & V4L2_SUPPORTS_READ) &&
			!v4l2_needs_conversion(index))) {
		result = devices[index].dev_ops->read(
				devices[index].dev_ops_priv,
				fd, dest, n);
		goto leave;
	}

	/* Since we need to do conversion try to use mmap (streaming) mode under
	   the hood as that safes a memcpy for each frame read.

	   Note sometimes this will fail as some drivers (at least gspca) do not allow
	   switching from read mode to mmap mode and they assume read() mode if a
	   select or poll() is done before any buffers are requested. So using mmap
	   mode under the hood will fail if a select() or poll() is done before the
	   first emulated read() call. */
	if (!(devices[index].flags & V4L2_STREAM_CONTROLLED_BY_READ) &&
			!(devices[index].flags & V4L2_USE_READ_FOR_READ)) {
		result = v4l2_activate_read_stream(index);
		if (result) {
			/* Activating mmap mode failed, use read() instead */
			devices[index].flags |= V4L2_USE_READ_FOR_READ;
			/* The read call done by v4l2_read_and_convert will start the stream */
			devices[index].first_frame = V4L2_IGNORE_FIRST_FRAME_ERRORS;
		}
	}

	if (devices[index].flags & V4L2_USE_READ_FOR_READ) {
		result = v4l2_read_and_convert(index, dest, n);
	} else {
		struct v4l2_buffer buf;

		buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		result = v4l2_dequeue_and_convert(index, &buf, dest, n);

		if (result >= 0)
			v4l2_queue_read_buffer(index, buf.index);
	}

leave:
	saved_errno = errno;
	pthread_mutex_unlock(&devices[index].stream_lock);
	errno = saved_errno;

	return result;
}

ssize_t v4l2_write(int fd, const void *buffer, size_t n)
{
	int index = v4l2_get_index(fd);

	if (index == -1)
		return SYS_WRITE(fd, buffer, n);

	if (!devices[index].dev_ops->write) {
		errno = EINVAL;
		return -1;
	}

	return devices[index].dev_ops->write(
			devices[index].dev_ops_priv, fd, buffer, n);
}

void *v4l2_mmap(void *start, size_t length, int prot, int flags, int fd,
		int64_t offset)
{
	int index;
	unsigned int buffer_index;
	void *result;

	index = v4l2_get_index(fd);
	if (index == -1 ||
			/* Check if the mmap data matches our answer to QUERY_BUF. If it doesn't,
			   let the kernel handle it (to allow for mmap-based non capture use) */
			start || length != devices[index].convert_mmap_frame_size ||
			((unsigned int)offset & ~0xFFu) != V4L2_MMAP_OFFSET_MAGIC) {
		if (index != -1)
			V4L2_LOG("Passing mmap(%p, %d, ..., %x, through to the driver\n",
					start, (int)length, (int)offset);

		if (offset & ((1 << MMAP2_PAGE_SHIFT) - 1)) {
			errno = EINVAL;
			return MAP_FAILED;
		}

		return (void *)SYS_MMAP(start, length, prot, flags, fd, offset);
	}

	pthread_mutex_lock(&devices[index].stream_lock);

	buffer_index = offset & 0xff;
	if (buffer_index >= devices[index].no_frames ||
			/* Got magic offset and not converting ?? */
			!v4l2_needs_conversion(index)) {
		errno = EINVAL;
		result = MAP_FAILED;
		goto leave;
	}

	if (v4l2_ensure_convert_mmap_buf(index)) {
		errno = EINVAL;
		result = MAP_FAILED;
		goto leave;
	}

	devices[index].frame_map_count[buffer_index]++;

	result = devices[index].convert_mmap_buf +
		buffer_index * devices[index].convert_mmap_frame_size;

	V4L2_LOG("Fake (conversion) mmap buf %u, seen by app at: %p\n",
			buffer_index, result);

leave:
	pthread_mutex_unlock(&devices[index].stream_lock);

	return result;
}

int v4l2_munmap(void *_start, size_t length)
{
	int index;
	unsigned int buffer_index;
	unsigned char *start = _start;

	/* Is this memory ours? */
	if (start != MAP_FAILED) {
		for (index = 0; index < devices_used; index++)
			if (devices[index].fd != -1 &&
					devices[index].convert_mmap_buf != MAP_FAILED &&
					length == devices[index].convert_mmap_frame_size &&
					start >= devices[index].convert_mmap_buf &&
					(start - devices[index].convert_mmap_buf) % length == 0)
				break;

		if (index != devices_used) {
			int unmapped = 0;

			pthread_mutex_lock(&devices[index].stream_lock);

			buffer_index = (start - devices[index].convert_mmap_buf) / length;

			/* Re-do our checks now that we have the lock, things may have changed */
			if (devices[index].convert_mmap_buf != MAP_FAILED &&
					length == devices[index].convert_mmap_frame_size &&
					start >= devices[index].convert_mmap_buf &&
					(start - devices[index].convert_mmap_buf) % length == 0 &&
					buffer_index < devices[index].no_frames) {
				if (devices[index].frame_map_count[buffer_index] > 0)
					devices[index].frame_map_count[buffer_index]--;
				unmapped = 1;
			}

			pthread_mutex_unlock(&devices[index].stream_lock);

			if (unmapped) {
				V4L2_LOG("v4l2 fake buffer munmap %p, %d\n", start, (int)length);
				return 0;
			}
		}
	}

	V4L2_LOG("v4l2 unknown munmap %p, %d\n", start, (int)length);

	return SYS_MUNMAP(_start, length);
}

/* Misc utility functions */
int v4l2_set_control(int fd, int cid, int value)
{
	struct v4l2_queryctrl qctrl = { .id = cid };
	struct v4l2_control ctrl = { .id = cid };
	int index, result;

	index = v4l2_get_index(fd);
	if (index == -1 || devices[index].convert == NULL) {
		V4L2_LOG_ERR("v4l2_set_control called with invalid fd: %d\n", fd);
		errno = EBADF;
		return -1;
	}

	result = v4lconvert_vidioc_queryctrl(devices[index].convert, &qctrl);
	if (result)
		return result;

	if (!(qctrl.flags & V4L2_CTRL_FLAG_DISABLED) &&
			!(qctrl.flags & V4L2_CTRL_FLAG_GRABBED)) {
		if (qctrl.type == V4L2_CTRL_TYPE_BOOLEAN)
			ctrl.value = value ? 1 : 0;
		else
			ctrl.value = ((long long) value * (qctrl.maximum - qctrl.minimum) + 32767) / 65535 +
				qctrl.minimum;

		result = v4lconvert_vidioc_s_ctrl(devices[index].convert, &ctrl);
	}

	return result;
}

int v4l2_get_control(int fd, int cid)
{
	struct v4l2_queryctrl qctrl = { .id = cid };
	struct v4l2_control ctrl = { .id = cid };
	int index = v4l2_get_index(fd);

	if (index == -1 || devices[index].convert == NULL) {
		V4L2_LOG_ERR("v4l2_set_control called with invalid fd: %d\n", fd);
		errno = EBADF;
		return -1;
	}

	if (v4lconvert_vidioc_queryctrl(devices[index].convert, &qctrl))
		return -1;

	if (qctrl.flags & V4L2_CTRL_FLAG_DISABLED) {
		errno = EINVAL;
		return -1;
	}

	if (v4lconvert_vidioc_g_ctrl(devices[index].convert, &ctrl))
		return -1;

	return (((long long) ctrl.value - qctrl.minimum) * 65535 +
			(qctrl.maximum - qctrl.minimum) / 2) /
		(qctrl.maximum - qctrl.minimum);
}
