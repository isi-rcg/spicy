/*******************************************************************************
 * Copyright (c) 2007-2019 Wind River Systems, Inc. and others.
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
 * This module provides POSIX signal names and descriptions.
 */

#include <tcf/config.h>

#include <stdio.h>
#include <signal.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/sigsets.h>
#include <tcf/framework/signames.h>

#if defined(_WIN32) || defined(__CYGWIN__)

typedef struct ExceptionName {
    DWORD code;
    const char * name;
    const char * desc;
} ExceptionName;

static ExceptionName exception_names[] = {
    { 0x40010005, NULL, "Control-C" },
    { 0x40010008, NULL, "Control-Break" },
    { 0x80000001, "EXCEPTION_GUARD_PAGE", "Guard Page" },
    { 0x80000002, "EXCEPTION_DATATYPE_MISALIGNMENT", "Datatype Misalignment" },
    { 0xc0000005, "EXCEPTION_ACCESS_VIOLATION", "Access Violation" },
    { 0xc0000006, "EXCEPTION_IN_PAGE_ERROR", "In Page Error" },
    { 0xc0000008, "EXCEPTION_INVALID_HANDLE", "Invalid Handle" },
    { 0xc0000017, NULL, "Not Enough Quota" },
    { 0xc000001d, "EXCEPTION_ILLEGAL_INSTRUCTION", "Illegal Instruction" },
    { 0xc0000025, "EXCEPTION_NONCONTINUABLE_EXCEPTION", "Noncontinuable Exception" },
    { 0xc0000026, "EXCEPTION_INVALID_DISPOSITION", "Invalid Disposition" },
    { 0xc000008c, "EXCEPTION_ARRAY_BOUNDS_EXCEEDED", "Array Bounds Exceeded" },
    { 0xc000008d, "EXCEPTION_FLT_DENORMAL_OPERAND", "Float Denormal Operand" },
    { 0xc000008e, "EXCEPTION_FLT_DIVIDE_BY_ZERO", "Float Divide by Zero" },
    { 0xc000008f, "EXCEPTION_FLT_INEXACT_RESULT", "Float Inexact Result" },
    { 0xc0000090, "EXCEPTION_FLT_INVALID_OPERATION", "Float Invalid Operation" },
    { 0xc0000091, "EXCEPTION_FLT_OVERFLOW", "Float Overflow" },
    { 0xc0000092, "EXCEPTION_FLT_STACK_CHECK", "Float Stack Check" },
    { 0xc0000093, "EXCEPTION_FLT_UNDERFLOW", "Float Underflow" },
    { 0xc0000094, "EXCEPTION_INT_DIVIDE_BY_ZERO", "Integer Divide by Zero" },
    { 0xc0000095, "EXCEPTION_INT_OVERFLOW", "Integer Overflow" },
    { 0xc0000096, "EXCEPTION_PRIV_INSTRUCTION", "Privileged Instruction" },
    { 0xc00000fd, "EXCEPTION_STACK_OVERFLOW", "Stack Overflow" },
    { 0xc0000135, NULL, "DLL Not Found" },
    { 0xc0000138, NULL, "Ordinal Not Found" },
    { 0xc0000139, NULL, "Entry Point Not Found" },
    { 0xc0000142, NULL, "DLL Initialization Failed" },
    { 0xc000014a, "STATUS_ILLEGAL_FLOAT_CONTEXT", "Floating-point hardware is not present" },
    { 0xc0000194, "EXCEPTION_POSSIBLE_DEADLOCK", "Possible Deadlock" },
    { 0xc00002b4, "STATUS_FLOAT_MULTIPLE_FAULTS", "Multiple floating-point faults" },
    { 0xc00002b5, "STATUS_FLOAT_MULTIPLE_TRAPS", "Multiple floating-point traps" },
    { 0xc00002c9, "STATUS_REG_NAT_CONSUMPTION", "Register NaT consumption faults" },
    { 0xc06d007e, NULL, "Module Not Found" },
    { 0xc06d007f, NULL, "Procedure Not Found" },
    { 0xe06d7363, NULL, "Microsoft C++ Exception" },
};

#define EXCEPTION_NAMES_CNT ((int)(sizeof(exception_names) / sizeof(ExceptionName)))

int signal_cnt(void) {
    return EXCEPTION_NAMES_CNT;
}

const char * signal_name(int signal) {
    int n = signal - 1;
    if (n >= 0 && n < EXCEPTION_NAMES_CNT) return exception_names[n].name;
    return NULL;
}

const char * signal_description(int signal) {
    int n = signal - 1;
    if (n >= 0 && n < EXCEPTION_NAMES_CNT) return exception_names[n].desc;
    return NULL;
}

unsigned signal_code(int signal) {
    int n = signal - 1;
    if (n >= 0 && n < EXCEPTION_NAMES_CNT) return exception_names[n].code;
    return 0;
}

int get_signal_from_code(unsigned code) {
    int n = 0;
    while (n < EXCEPTION_NAMES_CNT) {
        if (exception_names[n].code == code) return n + 1;
        n++;
    }
    return 0;
}

#else

/*
 * POSIX signals info
 */

typedef struct SignalInfo {
    int signal;
    const char * name;
    const char * desc;
} SignalInfo;

#define SigDesc(sig, desc) { sig, ""#sig, desc },
static SignalInfo info[] = {
    SigDesc(SIGHUP,    "Hangup")
    SigDesc(SIGINT,    "Interrupt")
    SigDesc(SIGQUIT,   "Quit and dump core")
    SigDesc(SIGILL,    "Illegal instruction")
    SigDesc(SIGTRAP,   "Trace/breakpoint trap")
    SigDesc(SIGABRT,   "Process aborted")
    SigDesc(SIGBUS,    "Bus error")
    SigDesc(SIGFPE,    "Floating point exception")
    SigDesc(SIGKILL,   "Request to kill")
    SigDesc(SIGUSR1,   "User-defined signal 1")
    SigDesc(SIGSEGV,   "Segmentation violation")
    SigDesc(SIGUSR2,   "User-defined signal 2")
    SigDesc(SIGPIPE,   "Write to pipe with no one reading")
    SigDesc(SIGALRM,   "Signal raised by alarm")
    SigDesc(SIGTERM,   "Request to terminate")
#ifdef SIGSTKFLT
    SigDesc(SIGSTKFLT, "Stack fault")
#endif
    SigDesc(SIGCHLD,   "Child process terminated or stopped")
    SigDesc(SIGCONT,   "Continue if stopped")
    SigDesc(SIGSTOP,   "Stop executing temporarily")
    SigDesc(SIGTSTP,   "Terminal stop signal")
    SigDesc(SIGTTIN,   "Background process attempting to read from tty")
    SigDesc(SIGTTOU,   "Background process attempting to write to tty")
    SigDesc(SIGURG,    "Urgent data available on socket")
#ifdef SIGXCPU
    SigDesc(SIGXCPU,   "CPU time limit exceeded")
#endif
#ifdef SIGXFSZ
    SigDesc(SIGXFSZ,   "File size limit exceeded")
#endif
#ifdef SIGVTALRM
    SigDesc(SIGVTALRM, "Virtual time timer expired")
#endif
#ifdef SIGPROF
    SigDesc(SIGPROF,   "Profiling timer expired")
#endif
#ifdef SIGWINCH
    SigDesc(SIGWINCH,  "Window resize signal")
#endif
#ifdef SIGIO
    SigDesc(SIGIO,     "Asynchronous I/O event")
#elif defined(SIGPOLL)
    SigDesc(SIGPOLL,   "Asynchronous I/O event")
#endif
#ifdef SIGINFO
    SigDesc(SIGINFO,   "Information request")
#endif
#ifdef SIGPWR
    SigDesc(SIGPWR,    "Power failure")
#endif
    SigDesc(SIGSYS,    "Bad syscall")
};
#undef SigDesc

#define INFO_CNT ((int)(sizeof(info) / sizeof(SignalInfo)))

static int index_len = 0;

static SignalInfo * get_info(int signal) {
    static SignalInfo ** index = NULL;
    if (index_len == 0) {
        int i;
#if defined(SIGRTMAX)
        index_len = SIGRTMAX + 1;
#else
        for (i = 0; i < INFO_CNT; i++) {
            if (info[i].signal >= index_len) index_len = info[i].signal + 1;
        }
#endif
        if (index_len < 65) index_len = 65;
        index = (SignalInfo **)loc_alloc_zero(sizeof(SignalInfo *) * index_len);
        for (i = 0; i < INFO_CNT; i++) {
            index[info[i].signal] = &info[i];
        }
#if defined(SIGRTMIN) && defined(SIGRTMAX)
        for (i = SIGRTMIN; i <= SIGRTMAX; i++) {
            if (index[i] == NULL) {
                SignalInfo * s = (SignalInfo *)loc_alloc_zero(sizeof(SignalInfo));
                s->name = loc_printf("SIGRTMIN+%d", i - SIGRTMIN);
                s->desc = loc_printf("Real-time Signal %d", i);
                s->signal = i;
                index[i] = s;
            }
        }
#endif
        for (i = 1; i < index_len; i++) {
            if (index[i] == NULL) {
                SignalInfo * s = (SignalInfo *)loc_alloc_zero(sizeof(SignalInfo));
                s->name = loc_printf("SIGNAL%d", i);
                s->desc = loc_printf("Reserved Signal %d", i);
                s->signal = i;
                index[i] = s;
            }
        }
    }
    if (signal < 0 || signal >= index_len) return NULL;
    return index[signal];
}

int signal_cnt(void) {
    get_info(0);
    return index_len;
}

const char * signal_name(int signal) {
    SignalInfo * i = get_info(signal);
    if (i != NULL) return i->name;
    return NULL;
}

const char * signal_description(int signal) {
    SignalInfo * i = get_info(signal);
    if (i != NULL) return i->desc;
    return NULL;
}

unsigned signal_code(int signal) {
    return signal;
}

int get_signal_from_code(unsigned code) {
    return code;
}

#endif
