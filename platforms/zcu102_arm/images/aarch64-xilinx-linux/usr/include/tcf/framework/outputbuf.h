/*******************************************************************************
 * Copyright (c) 2007, 2014 Wind River Systems, Inc. and others.
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
 * Utility module that implements an abstract output queue.
 */

#ifndef D_outputbuf
#define D_outputbuf

#include <tcf/config.h>
#include <tcf/framework/link.h>

#if defined(_WRS_KERNEL)
/* Bug in VxWorks: send() crashes if buffer is too large */
#  define OUTPUT_QUEUE_BUF_SIZE 0x100
#else
#  define OUTPUT_QUEUE_BUF_SIZE (128 * MEM_USAGE_FACTOR)
#endif


typedef struct OutputQueue OutputQueue;
typedef struct OutputBuffer OutputBuffer;

struct OutputQueue {
    int error;
    LINK queue;
    void (*post_io_request)(OutputBuffer *);
};

struct OutputBuffer {
    LINK link;
    OutputQueue * queue;
    unsigned char buf[OUTPUT_QUEUE_BUF_SIZE];
    size_t buf_len;
    size_t buf_pos;
};

#define output_queue_is_empty(q) (list_is_empty(&(q)->queue))

extern OutputBuffer * output_queue_alloc_obuf(void);
extern void output_queue_free_obuf(OutputBuffer * bf);

extern void output_queue_ini(OutputQueue * q);
extern void output_queue_add(OutputQueue * q, const void * buf, size_t size);
extern void output_queue_add_obuf(OutputQueue * q, OutputBuffer * buf);
extern void output_queue_done(OutputQueue * q, int error, int size);
extern void output_queue_clear(OutputQueue * q);

#endif /* D_outputbuf */
