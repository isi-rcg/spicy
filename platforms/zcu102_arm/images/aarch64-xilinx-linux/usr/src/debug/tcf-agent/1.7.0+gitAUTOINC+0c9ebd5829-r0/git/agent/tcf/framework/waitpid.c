/*******************************************************************************
 * Copyright (c) 2009-2018 Wind River Systems, Inc. and others.
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
 * This module provides notifications of process/thread exited or stopped.
 */

#include <tcf/config.h>

#if (ENABLE_DebugContext && !ENABLE_ContextProxy) || SERVICE_Processes || SERVICE_Terminals

#include <assert.h>
#include <errno.h>
#include <tcf/framework/errors.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/events.h>
#include <tcf/framework/trace.h>
#include <tcf/framework/asyncreq.h>
#include <tcf/framework/waitpid.h>

typedef struct WaitPIDListenerInfo {
    WaitPIDListener * listener;
    void * args;
} WaitPIDListenerInfo;

#define MAX_LISTENERS 8

static WaitPIDListenerInfo listeners[MAX_LISTENERS];
static int listener_cnt = 0;

static void init(void);

void add_waitpid_listener(WaitPIDListener * listener, void * args) {
    assert(listener_cnt < MAX_LISTENERS);
    if (listener_cnt == 0) init();
    listeners[listener_cnt].listener = listener;
    listeners[listener_cnt].args = args;
    listener_cnt++;
}

#if defined(_WIN32) || defined(__CYGWIN__)

typedef struct WaitPIDThread {
    DWORD thread;
    HANDLE handles[MAXIMUM_WAIT_OBJECTS];
    DWORD handle_cnt;
    int shutdown;
    struct WaitPIDThread * next;
} WaitPIDThread;

static WaitPIDThread * threads = NULL;
static HANDLE semaphore = NULL;

#define check_error_win32(ok) { if (!(ok)) check_error(set_win32_errno(GetLastError())); }

static void waitpid_event(void * args) {
    int i;
    HANDLE prs = args;
    DWORD pid = GetProcessId(prs);
    DWORD exit_code = 0;
    check_error_win32(GetExitCodeProcess(prs, &exit_code));
    for (i = 0; i < listener_cnt; i++) {
        listeners[i].listener(pid, 1, exit_code, 0, 0, 0, listeners[i].args);
    }
    check_error_win32(CloseHandle(prs));
}

static DWORD WINAPI waitpid_thread_func(LPVOID x) {
    WaitPIDThread * thread = (WaitPIDThread *)x;
    check_error_win32(WaitForSingleObject(semaphore, INFINITE) != WAIT_FAILED);
    while (!thread->shutdown) {
        DWORD n = 0;
        HANDLE arr[MAXIMUM_WAIT_OBJECTS];
        DWORD cnt = thread->handle_cnt;
        memcpy(arr, thread->handles, cnt * sizeof(HANDLE));
        check_error_win32(ReleaseSemaphore(semaphore, 1, 0));
        n = WaitForMultipleObjects(cnt, arr, FALSE, INFINITE);
        check_error_win32(n != WAIT_FAILED);
        check_error_win32(WaitForSingleObject(semaphore, INFINITE) != WAIT_FAILED);
        assert(n != WAIT_TIMEOUT);
        if (n > 0) {
            assert(thread->handles[n] == arr[n]);
            post_event(waitpid_event, thread->handles[n]);
            memmove(thread->handles + n, thread->handles + n + 1, (thread->handle_cnt - n - 1) * sizeof(HANDLE));
            thread->handle_cnt--;
        }
    }
    return 0;
}

static void init(void) {
    assert(threads == NULL);
    semaphore = CreateSemaphore(NULL, 1, 1, NULL);
}

void add_waitpid_process(int pid) {
    HANDLE prs = NULL;
    WaitPIDThread * thread = threads;
    assert(listener_cnt > 0);
    check_error_win32(WaitForSingleObject(semaphore, INFINITE) != WAIT_FAILED);
    while (thread != NULL && thread->handle_cnt >= MAXIMUM_WAIT_OBJECTS) thread = thread->next;
    if (thread == NULL) {
        thread = (WaitPIDThread *)loc_alloc_zero(sizeof(WaitPIDThread));
        thread->next = threads;
        threads = thread;
        check_error_win32((thread->handles[thread->handle_cnt++] = CreateEvent(NULL, 0, 0, NULL)) != NULL);
        check_error_win32(CreateThread(NULL, 0, waitpid_thread_func, thread, 0, &thread->thread) != NULL);
    }
    check_error_win32((prs = OpenProcess(PROCESS_QUERY_INFORMATION | SYNCHRONIZE, FALSE, pid)) != NULL);
    thread->handles[thread->handle_cnt++] = prs;
    check_error_win32(SetEvent(thread->handles[0]));
    check_error_win32(ReleaseSemaphore(semaphore, 1, 0));
}

void detach_waitpid_process(void) {
    assert(0);
}

#elif defined(_WRS_KERNEL)

#include <taskHookLib.h>

typedef struct EventInfo {
    UINT32 pid;
    SEM_ID signal;
} EventInfo;

static WIND_TCB * main_thread;

static void task_delete_event(void * args) {
    int i;
    EventInfo * info = args;
    for (i = 0; i < listener_cnt; i++) {
        listeners[i].listener(info->pid, 1, 0, 0, 0, 0, listeners[i].args);
    }
    semGive(info->signal);
}

static void task_delete_hook(WIND_TCB * tcb) {
    if (tcb != main_thread && taskIdCurrent != main_thread) {
        EventInfo info;
        VX_COUNTING_SEMAPHORE(signal_mem);
        info.signal = semCInitialize(signal_mem, SEM_Q_FIFO, 0);
        info.pid = (UINT32)tcb;
        post_event(task_delete_event, &info);
        semTake(info.signal, WAIT_FOREVER);
        semTerminate(info.signal);
    }
}

static void init(void) {
    main_thread = taskIdCurrent;
    taskDeleteHookAdd((FUNCPTR)task_delete_hook);
}

void add_waitpid_process(int pid) {
}

void detach_waitpid_process(void) {
}

#else

#include <sys/wait.h>

static int detach = 0;

static void waitpid_done(void * arg) {
    int i;
    AsyncReqInfo * req = (AsyncReqInfo *)arg;
    pid_t pid = req->u.wpid.pid;
    int status = req->u.wpid.status;
    int error = req->error;
    int exited = 0;
    int exit_code = 0;
    int signal = 0;
    int event_code = 0;
    int syscall = 0;

    trace(LOG_WAITPID, "waitpid: pid %d status %#x, error %d", pid, status, error);
    assert(req->u.wpid.rval == -1 || req->u.wpid.rval == pid);
    detach = 0;

    if (req->u.wpid.rval == -1) {
        assert(error);
        trace(error == ECHILD ? LOG_WAITPID : LOG_ALWAYS, "waitpid error (pid %d): %d %s", pid, error, errno_to_str(error));
        exited = 1;
        exit_code = error;
    }
    else if (WIFEXITED(status)) {
        exited = 1;
        exit_code = WEXITSTATUS(status);
        trace(LOG_WAITPID, "waitpid: pid %d exited, exit code %d", pid, exit_code);
    }
    else if (WIFSIGNALED(status)) {
        exited = 1;
        signal = WTERMSIG(status);
        trace(LOG_WAITPID, "waitpid: pid %d terminated, signal %d", pid, signal);
    }
    else if (WIFSTOPPED(status)) {
        signal = WSTOPSIG(status) & 0x7f;
        event_code = status >> 16;
        syscall = (WSTOPSIG(status) & 0x80) != 0;
        trace(LOG_WAITPID, "waitpid: pid %d suspended, signal %d, event code %d", pid, signal, event_code);
    }
    else {
        trace(LOG_ALWAYS, "unexpected status (0x%x) from waitpid (pid %d)", status, pid);
        exited = 1;
    }
    for (i = 0; i < listener_cnt; i++) {
        listeners[i].listener(pid, exited, exit_code, signal, event_code, syscall, listeners[i].args);
    }
    if (exited) {
        loc_free(req);
    }
    else if (detach) {
        trace(LOG_WAITPID, "waitpid: pid %d detached", pid);
        loc_free(req);
    }
    else {
        req->error = 0;
        req->u.wpid.status = 0;
        async_req_post(req);
    }
}

void add_waitpid_process(int pid) {
    AsyncReqInfo * req = (AsyncReqInfo *)loc_alloc_zero(sizeof(AsyncReqInfo));
    assert(listener_cnt > 0);
    trace(LOG_WAITPID, "waitpid: add pid %d", pid);
    req->done = waitpid_done;
    req->type = AsyncReqWaitpid;
    req->u.wpid.pid = pid;
#if defined(__linux__)
    req->u.wpid.options |= __WALL;
#endif
    async_req_post(req);
}

void detach_waitpid_process(void) {
    detach = 1;
}

static void init(void) {
}

#endif
#endif
