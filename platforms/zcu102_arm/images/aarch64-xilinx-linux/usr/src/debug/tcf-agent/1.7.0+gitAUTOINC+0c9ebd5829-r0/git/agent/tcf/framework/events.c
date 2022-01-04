/*******************************************************************************
 * Copyright (c) 2007-2018 Wind River Systems, Inc. and others.
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
 * Event queue manager.
 * Event is a data pointer plus a function pointer (a.k.a. event handler).
 *
 * Posting event means placing event into event queue.
 * Dispatching event means removing event from the queue and then calling
 * event function with event data as argument.
 *
 * All events are dispatched by single thread - dispatch thread. This makes it safe
 * to access global data structures from event handlers without further synchronization,
 * while allows for high level of concurrency.
 */

#include <tcf/config.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <tcf/framework/mdep-threads.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/errors.h>
#include <tcf/framework/trace.h>
#include <tcf/framework/events.h>

#if !defined(ENABLE_FastMemAlloc)
#  define ENABLE_FastMemAlloc 1
#endif

#if !defined(USE_CLOCK_MONOTONIC)
#  if defined(__UCLIBC__)
#    define USE_CLOCK_MONOTONIC 0
#  elif defined(__linux__) && !(defined(ANDROID) && defined(HAVE_PTHREAD_COND_TIMEDWAIT_MONOTONIC))
#    define USE_CLOCK_MONOTONIC 1
#  elif (defined(_WIN32) || defined(__CYGWIN__)) && !defined(DISABLE_PTHREADS_WIN32)
#    define USE_CLOCK_MONOTONIC 1
#  elif (defined(__VXWORKS__))
#    define USE_CLOCK_MONOTONIC 1
#  else
#    define USE_CLOCK_MONOTONIC 0
#  endif
#endif

#if USE_CLOCK_MONOTONIC
#  define EVENTS_CLOCK_TYPE CLOCK_MONOTONIC
#else
#  define EVENTS_CLOCK_TYPE CLOCK_REALTIME
#endif

typedef struct event_node event_node;

struct event_node {
    event_node *        next;
    struct timespec     runtime;
    EventCallBack *     handler;
    void *              arg;
};

#if defined(_WIN32) || defined(__CYGWIN__)
   static DWORD event_thread;
#  define current_thread GetCurrentThreadId()
#  define is_event_thread (event_thread == current_thread)
#else
   static pthread_t event_thread;
#  define current_thread pthread_self()
#  define is_event_thread pthread_equal(event_thread, current_thread)
#endif

#if ENABLE_Trace
#  undef trace
#  define trace if ((log_mode & LOG_EVENTCORE) && log_file) print_trace
#endif

#if ENABLE_FastMemAlloc

#define EVENT_BUF_SIZE 0x200
static event_node event_buf[EVENT_BUF_SIZE];
static event_node * free_queue = NULL;
static event_node * free_bg_queue = NULL;

#define alloc_event_node(ev) \
    ev = free_queue; \
    if (ev != NULL) free_queue = ev->next; \
    else ev = (event_node *)loc_alloc(sizeof(event_node));

#define alloc_event_node_bg(ev) \
    ev = free_bg_queue; \
    if (ev != NULL) free_bg_queue = ev->next; \
    else ev = (event_node *)loc_alloc(sizeof(event_node));

#define free_event_node(ev) \
    if (ev >= event_buf && ev < event_buf + EVENT_BUF_SIZE) { \
        ev->next = free_queue; \
        free_queue = ev; \
    } \
    else { \
        loc_free(ev); \
    }

#else

#define alloc_event_node(ev) ev = (event_node *)loc_alloc(sizeof(event_node))
#define alloc_event_node_bg(ev) alloc_event_node(ev)
#define free_event_node(ev) loc_free(ev)

#endif

static pthread_mutex_t event_lock;
static pthread_cond_t event_cond;
static pthread_cond_t cancel_cond;

static event_node * event_queue = NULL;
static event_node * event_last = NULL;
static event_node * timer_queue = NULL;
static EventCallBack * cancel_handler = NULL;
static void * cancel_arg = NULL;
static int process_events = 0;
static event_node * exit_event = NULL;

uint32_t events_timer_ms = 0;

static int time_cmp(const struct timespec * tv1, const struct timespec * tv2) {
    assert(tv1->tv_nsec < 1000000000);
    assert(tv2->tv_nsec < 1000000000);
    if (tv1->tv_sec < tv2->tv_sec) return -1;
    if (tv1->tv_sec > tv2->tv_sec) return 1;
    if (tv1->tv_nsec < tv2->tv_nsec) return -1;
    if (tv1->tv_nsec > tv2->tv_nsec) return 1;
    return 0;
}

/* Add microsecond value to timespec. */
static void time_add_usec(struct timespec * tv, unsigned long usec) {
    tv->tv_sec += usec / 1000000;
    tv->tv_nsec += (usec % 1000000) * 1000;
    if (tv->tv_nsec >= 1000000000) {
        tv->tv_sec++;
        tv->tv_nsec -= 1000000000;
    }
}

static void post_from_bg_thread(EventCallBack * handler, void * arg, unsigned long delay) {
    event_node * ev;
    event_node * next;
    event_node * prev;
    struct timespec runtime;

    if (clock_gettime(EVENTS_CLOCK_TYPE, &runtime)) check_error(errno);
    time_add_usec(&runtime, delay);

    check_error(pthread_mutex_lock(&event_lock));
    if (cancel_handler == handler && cancel_arg == arg) {
        cancel_handler = NULL;
        check_error(pthread_cond_signal(&cancel_cond));
        check_error(pthread_mutex_unlock(&event_lock));
        return;
    }
    alloc_event_node_bg(ev);
    ev->runtime = runtime;
    ev->handler = handler;
    ev->arg = arg;

    prev = NULL;
    next = timer_queue;
    while (next != NULL && time_cmp(&ev->runtime, &next->runtime) >= 0) {
        prev = next;
        next = next->next;
    }
    ev->next = next;
    if (prev == NULL) {
        timer_queue = ev;
        check_error(pthread_cond_signal(&event_cond));
    }
    else {
        prev->next = ev;
    }
    trace(LOG_EVENTCORE, "post_event: event %#" PRIxPTR ", handler %#" PRIxPTR ", arg %#" PRIxPTR ", runtime %02u:%02u.%03u",
        (uintptr_t)ev, (uintptr_t)ev->handler, (uintptr_t)ev->arg,
        (unsigned)(ev->runtime.tv_sec / 60 % 60),
        (unsigned)(ev->runtime.tv_sec % 60),
        (unsigned)(ev->runtime.tv_nsec / 1000000));
    check_error(pthread_mutex_unlock(&event_lock));
}

void post_event_with_delay(EventCallBack * handler, void * arg, unsigned long delay) {
    if (is_event_thread && cancel_handler == NULL) {
        event_node * ev;
        event_node * next;
        event_node * prev;
        struct timespec runtime;

        if (clock_gettime(EVENTS_CLOCK_TYPE, &runtime)) check_error(errno);
        time_add_usec(&runtime, delay);

        alloc_event_node(ev);
        ev->runtime = runtime;
        ev->handler = handler;
        ev->arg = arg;

        check_error(pthread_mutex_lock(&event_lock));

        prev = NULL;
        next = timer_queue;
        while (next != NULL && time_cmp(&ev->runtime, &next->runtime) >= 0) {
            prev = next;
            next = next->next;
        }
        ev->next = next;
        if (prev == NULL) {
            timer_queue = ev;
        }
        else {
            prev->next = ev;
        }
        check_error(pthread_mutex_unlock(&event_lock));

        trace(LOG_EVENTCORE, "post_event: event %#" PRIxPTR ", handler %#" PRIxPTR ", arg %#" PRIxPTR ", runtime %02u%02u.%03u",
            (uintptr_t)ev, (uintptr_t)ev->handler, (uintptr_t)ev->arg,
            (unsigned)(ev->runtime.tv_sec / 60 % 60),
            (unsigned)(ev->runtime.tv_sec % 60),
            (unsigned)(ev->runtime.tv_nsec / 1000000));
    }
    else {
        post_from_bg_thread(handler, arg, delay);
    }
}

void post_event(EventCallBack * handler, void * arg) {
    if (is_event_thread && cancel_handler == NULL) {
        event_node * ev;

        alloc_event_node(ev);
        ev->runtime.tv_nsec = 0;
        ev->runtime.tv_sec = 0;
        ev->handler = handler;
        ev->arg = arg;
        ev->next = NULL;
        if (event_queue == NULL) {
            assert(event_last == NULL);
            event_last = event_queue = ev;
        }
        else {
            event_last->next = ev;
            event_last = ev;
        }
        trace(LOG_EVENTCORE, "post_event: event %#" PRIxPTR ", handler %#" PRIxPTR ", arg %#" PRIxPTR,
            (uintptr_t)ev, (uintptr_t)ev->handler, (uintptr_t)ev->arg);
    }
    else {
        post_from_bg_thread(handler, arg, 0);
    }
}

int cancel_event(EventCallBack * handler, void * arg, int wait) {
    event_node * ev;
    event_node * prev;

    assert(is_dispatch_thread());
    assert(handler != NULL);
    assert(cancel_handler == NULL);

    trace(LOG_EVENTCORE, "cancel_event: handler %#" PRIxPTR ", arg %#" PRIxPTR ", wait %d", (uintptr_t)handler, (uintptr_t)arg, wait);
    prev = NULL;
    ev = event_queue;
    while (ev != NULL) {
        if (ev->handler == handler && ev->arg == arg) {
            if (ev->next == NULL) {
                assert(event_last == ev);
                event_last = prev;
            }
            if (prev == NULL) {
                event_queue = ev->next;
            }
            else {
                prev->next = ev->next;
            }
            free_event_node(ev);
            return 1;
        }
        prev = ev;
        ev = ev->next;
    }

    check_error(pthread_mutex_lock(&event_lock));
    prev = NULL;
    ev = timer_queue;
    while (ev != NULL) {
        if (ev->handler == handler && ev->arg == arg) {
            if (prev == NULL) {
                timer_queue = ev->next;
            }
            else {
                prev->next = ev->next;
            }
            free_event_node(ev);
            check_error(pthread_mutex_unlock(&event_lock));
            return 1;
        }
        prev = ev;
        ev = ev->next;
    }

    if (!wait) {
        check_error(pthread_mutex_unlock(&event_lock));
        return 0;
    }

    cancel_handler = handler;
    cancel_arg = arg;
    do check_error(pthread_cond_wait(&cancel_cond, &event_lock));
    while (cancel_handler != NULL);
    check_error(pthread_mutex_unlock(&event_lock));
    return 1;
}

int is_dispatch_thread(void) {
    return is_event_thread;
}

void ini_events_queue(void) {
    event_thread = current_thread;
    check_error(pthread_mutex_init(&event_lock, NULL));
#if USE_CLOCK_MONOTONIC
    {
        pthread_condattr_t attr;
        check_error(pthread_condattr_init(&attr));
        check_error(pthread_condattr_setclock(&attr, EVENTS_CLOCK_TYPE));
        check_error(pthread_cond_init(&event_cond, &attr));
        check_error(pthread_condattr_destroy(&attr));
    }
#else
    check_error(pthread_cond_init(&event_cond, NULL));
#endif
    check_error(pthread_cond_init(&cancel_cond, NULL));
#if ENABLE_FastMemAlloc
    {
        int i;
        assert(free_queue == NULL);
        assert(free_bg_queue == NULL);
        for (i = 0; i < EVENT_BUF_SIZE; i++) {
            event_node * ev = event_buf + i;
            ev->next = free_queue;
            free_queue = ev;
        }
    }
#endif
    exit_event = (event_node *)loc_alloc_zero(sizeof(event_node));
}

void cancel_event_loop(void) {
    process_events = 0;
}

static void exit_event_handler(void * args) {
    if (event_queue == NULL) {
        process_events = 0;
    }
    else {
        post_event(exit_event_handler, NULL);
    }
}

void exit_event_loop(void) {
    /* Note: need to wake main thread in case exit_event_loop() is called from signal handler */
    check_error(pthread_mutex_lock(&event_lock));
    if (exit_event != NULL) {
        exit_event->handler = exit_event_handler;
        exit_event->next = timer_queue;
        timer_queue = exit_event;
        exit_event = NULL;
        check_error(pthread_cond_signal(&event_cond));
    }
    check_error(pthread_mutex_unlock(&event_lock));
}

void run_event_loop(void) {
    unsigned event_cnt = 0;
    uint32_t last_tick_count_ms = events_timer_ms;

    /* Allow the event loop to run on a thread other than the initializing thread */
    event_thread = current_thread;
    assert(is_dispatch_thread());

    process_events = 1;
    while (process_events) {

        event_node * ev = NULL;

        if (event_queue == NULL || event_cnt >= 100 || (event_cnt >= 1 && events_timer_ms - last_tick_count_ms >= 100)) {
            check_error(pthread_mutex_lock(&event_lock));
            event_cnt = 0;
#if ENABLE_FastMemAlloc
            while (free_queue != NULL && (free_bg_queue == NULL || free_bg_queue->next == NULL)) {
                event_node * x = free_queue;
                free_queue = x->next;
                x->next = free_bg_queue;
                free_bg_queue = x;
            }
#endif
            for (;;) {
                last_tick_count_ms = events_timer_ms;
                if ((ev = timer_queue) != NULL) {
                    struct timespec timenow;
                    event_node * evlast = NULL;
                    if (clock_gettime(EVENTS_CLOCK_TYPE, &timenow)) check_error(errno);
                    while (ev != NULL && time_cmp(&ev->runtime, &timenow) <= 0) {
                        evlast = ev;
                        ev = ev->next;
                    }
                    if (evlast != NULL) {
                        /* Move timed events that are ready to the
                         * beginning of the untimed event queue. */
                        ev = timer_queue;
                        timer_queue = evlast->next;
                        evlast->next = event_queue;
                        if (event_queue == NULL) {
                            assert(event_last == NULL);
                            event_last = evlast;
                        }
                        event_queue = ev;
                        break;
                    }
                    if (event_queue == NULL) {
                        int error = pthread_cond_timedwait(&event_cond, &event_lock, &timer_queue->runtime);
                        if (error && error != ETIMEDOUT) check_error(error);
                    }
                    else {
                        break;
                    }
                }
                else if (event_queue == NULL) {
                    check_error(pthread_cond_wait(&event_cond, &event_lock));
                }
                else {
                    break;
                }
            }
            check_error(pthread_mutex_unlock(&event_lock));
        }

        ev = event_queue;
        event_queue = ev->next;
        if (event_queue == NULL) {
            assert(event_last == ev);
            event_last = NULL;
        }

        trace(LOG_EVENTCORE, "run_event_loop: event %#" PRIxPTR ", handler %#" PRIxPTR ", arg %#" PRIxPTR,
            (uintptr_t)ev, (uintptr_t)ev->handler, (uintptr_t)ev->arg);
        if (ev->runtime.tv_sec == 0 && ev->runtime.tv_nsec == 0) {
            /* Don't count timer queue events since getting new timer events
             * before the previous batch of timer events are processed
             * can cause starvation of the main queue */
            event_cnt++;
        }
        ev->handler(ev->arg);
        free_event_node(ev);
    }
}
