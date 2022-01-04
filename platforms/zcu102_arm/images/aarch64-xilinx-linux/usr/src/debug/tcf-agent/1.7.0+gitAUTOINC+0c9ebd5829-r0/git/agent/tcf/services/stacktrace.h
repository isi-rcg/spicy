/*******************************************************************************
 * Copyright (c) 2007, 2015 Wind River Systems, Inc. and others.
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
 * Target service implementation: stack trace (TCF name StackTrace)
 */

#ifndef D_stacktrace
#define D_stacktrace

#include <tcf/framework/protocol.h>
#include <tcf/framework/context.h>
#include <tcf/services/stacktrace-ext.h>


/*
 * Return 1 if 'frame' is the top frame of the context call stack.
 */
#define is_top_frame(ctx, frame) ((frame) == 0 || (frame) == STACK_TOP_FRAME)

/*
 * Get frame number for 'info'.
 */
#define get_info_frame(ctx, info) (info ? info->frame : STACK_NO_FRAME)

#if SERVICE_StackTrace || ENABLE_ContextProxy

/*
 * Get index of the top and bootom frames of a context.
 */
extern int get_top_frame(Context * ctx);
extern int get_bottom_frame(Context * ctx);

/*
 * Get index of the prev and next frames of a context.
 * Prev frame is caller (parent) frame.
 * Next frame is callee (child) frame.
 */
extern int get_prev_frame(Context * ctx, int frame);
extern int get_next_frame(Context * ctx, int frame);

/*
 * Get information about given stack frame.
 * Return -1 and errno on error.
 */
extern int get_frame_info(Context * ctx, int frame, StackFrame ** info);

/*
 * Get information about given stack frame from stack trace cache.
 * Don't try to run backtrace if the info is not vailable in the cache.
 * Return -1 and errno on error.
 */
extern int get_cached_frame_info(Context * ctx, int frame, StackFrame ** info);

/*
 * Simulated step into fake stack frame of inlined function.
 * 'cnt' step count.
 * Return -1 and errno on error.
 * Return 0 and *done == 0 if top frame in not inlined frame.
 * Return 0 and *done == 1 on success.
 */
extern void set_inlined_frame_level(Context * ctx, int level);
extern int get_inlined_frame_level(Context * ctx);

/* Deprecated, use get_frame_info */
extern int get_next_stack_frame(StackFrame * frame, StackFrame * down);

/*
 * Initialize stack trace service.
 */
extern void ini_stack_trace_service(Protocol *, TCFBroadcastGroup *);

#else /* SERVICE_StackTrace */

#define get_top_frame(ctx) 0
#define get_next_frame(ctx, frame) ((void)(frame), errno = ERR_UNSUPPORTED, STACK_NO_FRAME)
#define get_prev_frame(ctx, frame) ((void)(frame), errno = ERR_UNSUPPORTED, STACK_NO_FRAME)
#define get_frame_info(ctx, frame, info) ((void)(frame), errno = ERR_UNSUPPORTED, -1)
#define get_cached_frame_info(ctx, frame, info) ((void)(frame), errno = ERR_UNSUPPORTED, -1)
#define set_inlined_frame_level(ctx, level)
#define get_inlined_frame_level(ctx) 0

#endif /* SERVICE_StackTrace */
#endif /* D_stacktrace */
