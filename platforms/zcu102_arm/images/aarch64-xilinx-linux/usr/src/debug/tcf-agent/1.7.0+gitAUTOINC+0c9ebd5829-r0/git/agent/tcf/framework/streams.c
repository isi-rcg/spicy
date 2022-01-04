/*******************************************************************************
 * Copyright (c) 2007, 2013 Wind River Systems, Inc. and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 * The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 * http://www.eclipse.org/org/documents/edl-v10.php.
 * You may elect to redistribute this code under either of these licenses.
 *
 * Contributors:
 *     Wind River Systems - initial API and implementation
 *******************************************************************************/

/*
 * Abstract byte stream. Bytes in the stream can be divided into groups - messages.
 */

#include <tcf/config.h>
#ifdef ENABLE_STREAM_MACROS
#undef ENABLE_STREAM_MACROS
#endif
#define ENABLE_STREAM_MACROS 1

#include <stddef.h>
#include <string.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/streams.h>

int (read_stream)(InputStream * inp) {
    return (inp->cur < inp->end) ? *inp->cur++ : inp->read(inp);
}

int (peek_stream)(InputStream * inp) {
    return (inp->cur < inp->end) ? *inp->cur : inp->peek(inp);
}

void (write_stream)(OutputStream * out, int b) {
    if (b > ESC && out->cur < out->end) *out->cur++ = (unsigned char)b;
    else out->write(out, b);
}

void (write_block_stream)(OutputStream * out, const char * bytes, size_t size) {
    out->write_block(out, bytes, size);
}

ssize_t (splice_block_stream)(OutputStream * out, int fd, size_t size, int64_t * offset) {
    return out->splice_block(out, fd, size, offset);
}

void write_string(OutputStream * out, const char * str) {
    unsigned char * d = out->cur;
    if (d == NULL) {
        out->write_block(out, str, strlen(str));
    }
    else {
        unsigned char * e = out->end;
        unsigned char * s = (unsigned char *)str;
        for (;;) {
            unsigned char ch = *s++;
            if (ch != ESC && d < e) {
                if (ch == 0) {
                    out->cur = d;
                    break;
                }
                *d++ = ch;
            }
            else {
                out->cur = d;
                if (ch == 0) break;
                out->write(out, ch);
                d = out->cur;
                e = out->end;
            }
        }
    }
}

void write_stringz(OutputStream * out, const char * str) {
    unsigned char * d = out->cur;
    if (d == NULL) {
        out->write_block(out, str, strlen(str) + 1);
    }
    else {
        unsigned char * e = out->end;
        unsigned char * s = (unsigned char *)str;
        for (;;) {
            unsigned char ch = *s++;
            if (ch != ESC && d < e) {
                *d++ = ch;
                if (ch == 0) {
                    out->cur = d;
                    break;
                }
            }
            else {
                out->cur = d;
                out->write(out, ch);
                if (ch == 0) break;
                d = out->cur;
                e = out->end;
            }
        }
    }
}

static void write_byte_array_output_stream(OutputStream * out, int byte) {
    ByteArrayOutputStream * buf = (ByteArrayOutputStream *)((char *)out - offsetof(ByteArrayOutputStream, out));
    if (buf->pos < sizeof(buf->buf)) {
        buf->buf[buf->pos++] = (char)byte;
    }
    else {
        if (buf->mem == NULL) {
            buf->mem = (char *)loc_alloc(buf->max = buf->pos * 2);
            memcpy(buf->mem, buf->buf, buf->pos);
        }
        else if (buf->pos >= buf->max) {
            buf->mem = (char *)loc_realloc(buf->mem, buf->max *= 2);
        }
        buf->mem[buf->pos++] = (char)byte;
    }
}

static void write_block_byte_array_output_stream(OutputStream * out, const char * bytes, size_t size) {
    ByteArrayOutputStream * buf = (ByteArrayOutputStream *)((char *)out - offsetof(ByteArrayOutputStream, out));
    if (buf->pos + size <= sizeof(buf->buf)) {
        memcpy(buf->buf + buf->pos, bytes, size);
        buf->pos += size;
    }
    else {
        size_t pos = 0;
        while (pos < size) {
            size_t n = size - pos;
            if (buf->mem == NULL) {
                buf->mem = (char *)loc_alloc(buf->max = (buf->pos + n) * 2);
                memcpy(buf->mem, buf->buf, buf->pos);
            }
            else if (buf->pos >= buf->max) {
                buf->mem = (char *)loc_realloc(buf->mem, buf->max = (buf->pos + n) * 2);
            }
            else if (n > buf->max - buf->pos) {
                n = buf->max - buf->pos;
            }
            memcpy(buf->mem + buf->pos, bytes + pos, n);
            buf->pos += n;
            pos += n;
        }
    }
}

OutputStream * create_byte_array_output_stream(ByteArrayOutputStream * buf) {
    memset(buf, 0, sizeof(ByteArrayOutputStream));
    buf->out.write_block = write_block_byte_array_output_stream;
    buf->out.write = write_byte_array_output_stream;
    return &buf->out;
}

void get_byte_array_output_stream_data(ByteArrayOutputStream * buf, char ** data, size_t * size) {
    if (data != NULL) {
        if (buf->mem == NULL) {
            buf->max = buf->pos;
            buf->mem = (char *)loc_alloc(buf->max);
            memcpy(buf->mem, buf->buf, buf->pos);
        }
        *data = buf->mem;
    }
    else if (buf->mem != NULL) {
        loc_free(buf->mem);
    }
    if (size != NULL) *size = buf->pos;
    buf->mem = NULL;
    buf->max = 0;
    buf->pos = 0;
}

static int read_byte_array_input_stream(InputStream * inp) {
    ByteArrayInputStream * buf = (ByteArrayInputStream *)((char *)inp - offsetof(ByteArrayInputStream, inp));
    if (buf->inp.cur >= buf->inp.end) return MARKER_EOS;
    return *buf->inp.cur++;
}

static int peek_byte_array_input_stream(InputStream * inp) {
    ByteArrayInputStream * buf = (ByteArrayInputStream *)((char *)inp - offsetof(ByteArrayInputStream, inp));
    if (buf->inp.cur >= buf->inp.end) return MARKER_EOS;
    return *buf->inp.cur;
}

InputStream * create_byte_array_input_stream(ByteArrayInputStream * buf, const char * data, size_t size) {
    memset(buf, 0, sizeof(ByteArrayInputStream));
    buf->inp.read = read_byte_array_input_stream;
    buf->inp.peek = peek_byte_array_input_stream;
    buf->inp.cur = (unsigned char *)data;
    buf->inp.end = (unsigned char *)data + size;
    return &buf->inp;
}

static int read_forwarding_input_stream(InputStream * inp) {
    ForwardingInputStream * buf = (ForwardingInputStream *)((char *)inp - offsetof(ForwardingInputStream, fwd));
    int ch = read_stream(buf->inp);
    if (ch != MARKER_EOS) write_stream(buf->out, ch);
    return ch;
}

static int peek_forwarding_input_stream(InputStream * inp) {
    ForwardingInputStream * buf = (ForwardingInputStream *)((char *)inp - offsetof(ForwardingInputStream, fwd));
    return peek_stream(buf->inp);
}

InputStream * create_forwarding_input_stream(ForwardingInputStream * buf, InputStream * inp, OutputStream * out) {
    memset(buf, 0, sizeof(ForwardingInputStream));
    buf->fwd.read = read_forwarding_input_stream;
    buf->fwd.peek = peek_forwarding_input_stream;
    buf->inp = inp;
    buf->out = out;
    return &buf->fwd;
}
