/*******************************************************************************
 * Copyright (c) 2007, 2016 Wind River Systems, Inc. and others.
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
 * Target service implementation: run control (TCF name RunControl)
 *
 * All run control contexts are grouped by corresponding memory address spaces.
 * Other service implementations can ask run control to execute arbitrary code while all contexts in
 * a particular group - address space - are stopped.
 * Run control service manages a queue of such requests ("safe events queue") and
 * makes a fair effort to optimize such requests. It can, for example, coalesce
 * multiple requests for same address space, which often significantly improves performance.
 * "Safe event" code is guaranteed to run while relevant contexts are stopped and will not be resumed
 * during the event execution, with a single exception of single instruction step support
 * that would resume only the specified context.
 *
 * Run control service distinguishes between context being stopped and being intercepted.
 * A context is stopped when it is suspended for any reason - breakpoint, safe event, a suspend request, etc..
 * A context is intercepted when it is reported up (to UI or value-add) as suspended.
 *
 * For example, a breakpoint hit handling sequence looks like this:
 *   1. context stopped by breakpoint, call post_safe_event()
 *   2. run control suspends all contexts in the "stop" context group.
 *   3. run control calls safe event callback that evaluates breakpoint condition:
 *          if condition false, do step over the breakpoint - call safe_context_single_step()
 *          else intercept the context and report breakpoint hit up to UI - call suspend_debug_context()
 *   4. if no more safe events, run control resumes all contexts that are not intercepted.
 * Despite the fact that many contexts can be stopped as result of breakpoint hit,
 * normally, only one of them will be intercepted, and only one "suspended" message will be sent up to UI.
 * If a breakpoint needs to intercept more then one context,
 * it can be be done using breakpoint attribute "StopGroup".
 */

#ifndef D_runctrl
#define D_runctrl

#include <tcf/config.h>
#include <tcf/framework/events.h>
#include <tcf/framework/context.h>
#include <tcf/framework/protocol.h>
#include <tcf/services/runctrl-ext.h>

#if SERVICE_RunControl

/*
 * Lock run control: don't resume any thread while run control is locked.
 * Each call to run_ctrl_lock() increments lock counter.
 */
extern void run_ctrl_lock(void);

/*
 * Unlock run control: resume debuggee threads that are not intercepted.
 * Each call to run_ctrl_unlock() decrements lock counter, debuggee resumed when the counter reaches zero.
 */
extern void run_ctrl_unlock(void);

/*
 * Lock run control: don't resume the context while its run control is locked.
 * Each call to run_ctrl_ctx_lock() increments lock counter.
 */
extern void run_ctrl_ctx_lock(Context * ctx);

/*
 * Unlock run control: resume the context - if it is not intercepted.
 * Each call to run_ctrl_unlock() decrements lock counter, debuggee resumed when the counter reaches zero.
 */
extern void run_ctrl_ctx_unlock(Context * ctx);

/*
 * Add "safe" event.
 * Stops debuggee threads.
 * Callback function 'done' will be called when threads are stopped and
 * it is safe to access debuggee memory, plant breakpoints, etc.
 * Only threads that belong to same "stop" context group as 'ctx' are stopped.
 * post_safe_event() uses run_ctrl_lock()/run_ctrl_unlock() to suspend/resume debuggee.
 */
extern void post_safe_event(Context * ctx, EventCallBack * done, void * arg);

/*
 * Return 1 if called from safe event handler.
 */
extern int is_safe_event(void);

/*
 * Single step a context during handling of safe event.
 * "Safe" step is executed with all other contexts stopped,
 * and it is expected to take only a short time to execute.
 * It is intended to be used in breakpoints implementation.
 * Returns 0 if no errors, otherwise returns -1 and sets errno.
 * Note: this function is asynchronous, it returns before context finishes the step.
 */
extern int safe_context_single_step(Context * ctx);

/*
 * Check that all threads in a context are stopped and
 * it is safe to access debuggee memory, plant breakpoints, etc.
 * The function is intended to be used in an ACPM transaction.
 * If the check fails, the function requests the threads to stop and
 * calls cache_wait() to abort current transaction. The transaction will
 * be re-executed later, when relevant threads are stopped.
 */
extern void check_all_stopped(Context * ctx);

#ifdef NDEBUG
#  define assert_all_stopped(ctx) ((void)0)
#else
#  define assert_all_stopped(ctx) assert(print_not_stopped_contexts(ctx))
extern int print_not_stopped_contexts(Context * ctx);
#endif

/*
 * Suspend current ACPM transaction until pending safe events are precessed.
 */
extern void wait_safe_events_done(void);

/*
 * Return 1 the context is stopped.
 * Return 0 and set errno if not stopped.
 */
extern int is_ctx_stopped(Context * ctx);

/*
 * Return 1 if all threads in a debuggee are stopped and it is safe to access
 * debuggee memory, plant breakpoints, etc.
 * Only threads that belong to CONTEXT_GROUP_STOP of 'ctx' are checked.
 * Return 0 and set errno if not all stopped.
 */
extern int is_all_stopped(Context * ctx);

/*
 * Return 1 if the context is intercepted.
 * A context is intercepted when it is reported up (to UI or value-add) as suspended.
 */
extern int is_intercepted(Context * ctx);

/*
 * Terminate debug context - thread or process.
 * Returns 0 if no errors, otherwise returns -1 and sets errno.
 * Note: this function is asynchronous, it returns before context is terminated.
 */
extern int terminate_debug_context(Context * ctx);

/*
 * Detach debug context.
 * Returns 0 if no errors, otherwise returns -1 and sets errno.
 * Note: this function is asynchronous, it returns before context is detached.
 */
extern int detach_debug_context(Context * ctx);

/*
 * Resume debug context.
 * If "ctx" is a process, resume all children.
 * 'c' - channel to access symbols info, can be NULL if the info is provided locally.
 * 'mode' - of context resume modes defined in context.h
 * 'count' - number of steps if 'mode' is stepping mode.
 * 'range_start', range_end' - instruction address range if 'mode' is stepping in a range.
 * Returns 0 if no errors, otherwise returns -1 and sets errno.
 * Note: this function is asynchronous, it returns before contexts are resumed.
 */
extern int continue_debug_context(Context * ctx, Channel * c,
        int mode, int count, ContextAddress range_start, ContextAddress range_end);

/*
 * Suspend (stop and intercept) debug context - thread or process.
 * If "ctx" is a process, suspend all children.
 * RunControl.suspended event is sent if it was not sent before.
 * Returns 0 if no errors, otherwise returns -1 and sets errno.
 * Note: this function is asynchronous, it returns before contexts are suspended.
 */
extern int suspend_debug_context(Context * ctx);

/*
 * Suspend (stop and intercept) debug context - thread or process.
 * This function is intended to be used by the Breakpoints service.
 */
extern int suspend_by_breakpoint(Context * ctx, Context * trigger, const char * bp, int skip_prologue);

/*
 * Return NULL-terminated array of breakpoint IDs if the context is suspended by breakpoint.
 * Otherwise return NULL.
 */
extern char ** get_context_breakpoint_ids(Context * ctx);

/*
 * Return current stepping mode of a debug context.
 * Return RM_RESUME if the context in not being stepped.
 */
extern int get_stepping_mode(Context * ctx);

/**
 * Set context state name if it is other than Running,
 * for example: "Sleeping", "Reset", "No Clock"
 * The name is only applicable if the context is not intercepted.
 */
extern void set_context_state_name(Context * ctx, const char * name);
extern const char * get_context_state_name(Context * ctx);

/**
 * Check that no safe events are pending, run control is not locked,
 * and no temporary suspended contexts are waiting to be resumed.
 * Clients might want to delay disconnecting from a target until run control is idle.
 */
extern int is_run_ctrl_idle(void);

/* RunControl event listener */
typedef struct RunControlEventListener {
    void (*context_intercepted)(Context * ctx, void * args);
    void (*context_released)(Context * ctx, void * args);
} RunControlEventListener;

/*
 * Add/remove a listener for RunControl service events.
 */
extern void add_run_control_event_listener(RunControlEventListener * listener, void * args);
extern void rem_run_control_event_listener(RunControlEventListener * listener);

/*
 * Initialize run control service.
 */
extern void ini_run_ctrl_service(Protocol * proto, TCFBroadcastGroup * bcg);

#else

#define run_ctrl_lock() do {} while(0)
#define run_ctrl_unlock() do {} while(0)
#define is_intercepted(x) 0
#define is_safe_event() 0
#define post_safe_event(ctx, done, arg) ((void)ctx, post_event(done, arg))
#define check_all_stopped(x) do {} while(0)
extern int is_ctx_stopped(Context * ctx);
extern int is_all_stopped(Context * ctx);

#endif /* SERVICE_RunControl */

#endif /* D_runctrl */
