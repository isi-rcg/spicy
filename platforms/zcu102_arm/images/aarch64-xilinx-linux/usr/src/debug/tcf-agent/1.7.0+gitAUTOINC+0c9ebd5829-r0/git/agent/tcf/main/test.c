/*******************************************************************************
 * Copyright (c) 2007-2020 Wind River Systems, Inc. and others.
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
 * Agent self-testing service.
 */

#ifndef PURE_RCBP_TEST
#  include <tcf/config.h>
#else
#  include <stdlib.h>
#  include <errno.h>
#  define usleep(x) {}
#  ifndef ENABLE_RCBP_TEST
#    define ENABLE_RCBP_TEST 1
#  endif
#endif

#if ENABLE_RCBP_TEST

#ifndef PURE_RCBP_TEST

#ifndef ENABLE_TestSymbols
#  define ENABLE_TestSymbols (SERVICE_Expressions && !ENABLE_ELF)
#endif

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <assert.h>
#include <tcf/framework/mdep-threads.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/errors.h>
#include <tcf/services/diagnostics.h>
#include <tcf/main/test.h>
#if defined(_WIN32) || defined(__CYGWIN__)
#  include <system/Windows/tcf/context-win32.h>
#endif

#endif /* PURE_RCBP_TEST */

#ifdef __cplusplus

bool tcf_cpp_test_bool = false;

template <class T> T tcf_cpp_test_template_func(T a, T b) {
    return a + b;
}

template <class T> class tcf_cpp_test_template_class {
    T values[2];
public:
    tcf_cpp_test_template_class(T a, T b) {
        values[0] = a;
        values[1] = b;
    }
};

tcf_cpp_test_template_class<int> tcf_cpp_test_template_class_int(1, 2);
tcf_cpp_test_template_class<double> tcf_cpp_test_template_class_double(1.0, 2.0);

class tcf_cpp_test_class {
public:
    static int s_int;
    class tcf_cpp_test_class_nested;
};

class tcf_cpp_test_class_extension : tcf_cpp_test_class {
public:
    char f_char;
    int f_int;
};

class tcf_cpp_test_class::tcf_cpp_test_class_nested {
public:
    static int s_int;
    int f_int;
};

typedef int tcf_cpp_test_class_extension::* tcf_cpp_test_class_extension_member_ptr_type;

class tcf_cpp_test_anonymous_union_class {
public:
    int f1;
    union {
        int f2;
        int f3;
    };
};

int tcf_cpp_test_class::s_int = 1;
int & tcf_cpp_test_int_ref = tcf_cpp_test_class::s_int;
int tcf_cpp_test_class::tcf_cpp_test_class_nested::s_int = 2;
tcf_cpp_test_class_extension tcf_cpp_test_class_extension_var;
tcf_cpp_test_class_extension * tcf_cpp_test_class_extension_ptr = &tcf_cpp_test_class_extension_var;
tcf_cpp_test_class_extension & tcf_cpp_test_class_extension_ref = tcf_cpp_test_class_extension_var;
tcf_cpp_test_class_extension_member_ptr_type tcf_cpp_test_class_extension_member_ptr = &tcf_cpp_test_class_extension::f_int;
int tcf_cpp_test_class_extension_member_val = tcf_cpp_test_class_extension_var.*tcf_cpp_test_class_extension_member_ptr;
tcf_cpp_test_anonymous_union_class tcf_cpp_test_anonymous_union_var;

extern "C" {

#endif /* __cplusplus */

typedef enum test_enum {
    enum_val1 = 1,
    enum_val2 = 2,
    enum_val3 = 3
} test_enum;

typedef union test_union {
    int x;
    float y;
} test_union;

typedef struct test_struct {
    test_enum f_enum;
    int f_int;
    struct test_struct * f_struct;
    float f_float;
    double f_double;
    test_union f_union;
} test_struct;

typedef struct test_bitfields {
    unsigned f_0;
    unsigned f_ubit1 : 1;
    unsigned f_ubit2 : 2;
    unsigned f_ubit3 : 3;
    unsigned f_ubit4 : 4;
    unsigned f_ubit7 : 7;
    unsigned f_ubit9 : 9;
    unsigned f_ubit17: 17;
    int f_ibit1 : 1;
    int f_ibit2 : 2;
    int f_ibit3 : 3;
    int f_ibit4 : 4;
    int f_ibit7 : 7;
    int f_ibit9 : 9;
    int f_ibit17: 17;
} test_bitfields;

typedef struct test_array_field {
    unsigned char buf[3][5];
} test_array_field;

typedef int test_array[10001];

extern int tcf_test_func_int(int x, int y);
extern long tcf_test_func_long(long x, long y);
extern double tcf_test_func_double(double x, double y);
extern void tcf_test_func4(void);
extern void tcf_test_func3(void);
extern int tcf_test_func2(void);
extern void tcf_test_func1(void);
extern void tcf_test_func0(enum test_enum);

/* Main purpose of this declaration is to pull basic types info into DWARF */
char tcf_test_char = 0;
short tcf_test_short = 0;
int tcf_test_int = 0;
long tcf_test_long = 0;
const char * tcf_test_str = "abc";

unsigned tcf_test_func_call_cnt = 0;

static test_array_field tcf_test_array_field;

#if defined(__clang_major__) && __clang_major__ == 7
/* TLS debug info is broken in clang 7.0 */
#  define TLS_SUPPORTED 0
#elif defined(__linux__) && defined(__GNUC__)
#  define TLS_SUPPORTED 1
#else
#  define TLS_SUPPORTED 0
#endif

#if TLS_SUPPORTED
__thread uint32_t tcf_test_tls = 0;
__thread uint32_t tcf_test_tls2 = 0;
#endif

int tcf_test_func_int(int x, int y) {
    tcf_test_func_call_cnt++;
    return x + y;
}

long tcf_test_func_long(long x, long y) {
    tcf_test_func_call_cnt++;
    return x + y;
}

double tcf_test_func_double(double x, double y) {
    tcf_test_func_call_cnt++;
    return x + y;
}

void tcf_test_func4(void) {
}

void tcf_test_func3(void) {
    tcf_test_char++;
    usleep(1000);
    tcf_test_func4();
}

int tcf_test_func2(void) {
    int func2_local1 = 1;
    int func2_local2 = 2;
    test_struct func2_local3 = { enum_val3, 153, NULL, 3.14f, 2.71 };
    int * func2_local4 = NULL;
    test_bitfields func2_local5 = { 0, 1, 2, 3, 4, 7, 9, 17, 1, 2, 3, 4, 7, 9, 17 };
    const char * func2_local_str = "bcd";

func2_label:
    if (tcf_test_int) goto func2_label;

    func2_local3.f_struct = &func2_local3;
    tcf_test_short++;
    errno = tcf_test_short;
    func2_local4 = &errno;
#if TLS_SUPPORTED
    tcf_test_tls++;
    tcf_test_tls2 += 2;
#endif
    tcf_test_func3();
    func2_local1++;
    func2_local2 = func2_local1;
    return func2_local2 + *func2_local4 + func2_local5.f_0 + func2_local_str[0];
}

void tcf_test_func1(void) {
    tcf_test_long++;
    tcf_test_func2();
}

void tcf_test_func0(test_enum e) {
    tcf_test_func1();
}

static char array[0x1000];
char * tcf_test_array = array;

#ifdef __cplusplus
}
#endif

static void test_start(void) {
    int i, j;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 5; j++) {
            tcf_test_array_field.buf[i][j] = (unsigned char)(i * 5 + j);
        }
    }
#ifdef __cplusplus
    tcf_cpp_test_anonymous_union_var.f1 = 234;
    tcf_cpp_test_anonymous_union_var.f2 = 235;
    tcf_cpp_test_class_extension_var.f_int = 345;
#endif
    tcf_test_func0(enum_val1);
}

#ifdef PURE_RCBP_TEST

void test_proc(void) {
    int i;
    test_start();
    for (i = 0; i < 9; i++) {
        tcf_test_func0(enum_val2);
    }
}

#else

static void * test_sub(void * x) {
    volatile int * test_done = (int *)x;
    while (!*test_done) {
        tcf_test_func0(enum_val3);
    }
    return NULL;
}

void test_proc(void) {
    int i;
    int test_done = 0;
    pthread_t thread[4];
    for (i = 0; i < 4; i++) thread[i] = 0;
    test_start();
    for (i = 0; i < 4; i++) {
        if (pthread_create(thread + i, &pthread_create_attr, test_sub, &test_done) != 0) {
            perror("pthread_create");
            break;
        }
    }
    for (i = 0; i < 9; i++) {
        tcf_test_func0(enum_val2);
    }
    test_done = 1;
    for (i = 0; i < 4; i++) {
        if (thread[i]) pthread_join(thread[i], NULL);
    }
}

int find_test_symbol(Context * ctx, const char * name, void ** addr, int * sym_class) {
    /* This code allows to run TCF diagnostic tests when symbols info is not available */
    *addr = NULL;
    *sym_class = SYM_CLASS_UNKNOWN;
#if ENABLE_TestSymbols && !defined(ALT_RCBP_TEST)
    if (is_test_process(ctx) && strncmp(name, "tcf_test_", 9) == 0) {
        if (strcmp(name, "tcf_test_array") == 0) {
            *sym_class = SYM_CLASS_REFERENCE;
            *addr = &tcf_test_array;
        }
        else if (strcmp(name, "tcf_test_char") == 0) {
            *sym_class = SYM_CLASS_REFERENCE;
            *addr = &tcf_test_char;
        }
        else {
            *sym_class = SYM_CLASS_FUNCTION;
            if (strcmp(name, "tcf_test_func0") == 0) *addr = (void *)tcf_test_func0;
            else if (strcmp(name, "tcf_test_func1") == 0) *addr = (void *)tcf_test_func1;
            else if (strcmp(name, "tcf_test_func2") == 0) *addr = (void *)tcf_test_func2;
            else if (strcmp(name, "tcf_test_func3") == 0) *addr = (void *)tcf_test_func3;
            else if (strcmp(name, "tcf_test_func4") == 0) *addr = (void *)tcf_test_func4;
        }
        if (*addr != NULL) return 0;
    }
#endif
    errno = ERR_SYM_NOT_FOUND;
    return -1;
}

#if defined(_WIN32) || defined(__CYGWIN__)
typedef struct ContextAttachArgs {
    ContextAttachCallBack * done;
    void * data;
    HANDLE thread;
    HANDLE process;
} ContextAttachArgs;

static void done_context_attach(int error, Context * ctx, void * data) {
    ContextAttachArgs * args = (ContextAttachArgs *)data;
    args->done(error, ctx, args->data);
    assert(error || args->process != get_context_handle(ctx));
    CloseHandle(args->thread);
    CloseHandle(args->process);
    loc_free(args);
}
#endif /* defined(_WIN32) || defined(__CYGWIN__) */

int run_test_process(ContextAttachCallBack * done, void * data) {
#if defined(_WIN32) || defined(__CYGWIN__)
    char fnm[FILE_PATH_SIZE];
    char cmd[FILE_PATH_SIZE];
    int res = 0;
    STARTUPINFO si;
    PROCESS_INFORMATION prs;
    ContextAttachArgs * args;

    memset(&si, 0, sizeof(si));
    memset(&prs, 0, sizeof(prs));
    memset(fnm, 0, sizeof(fnm));
    if (GetModuleFileName(NULL, fnm, sizeof(fnm)) == 0) {
        set_win32_errno(GetLastError());
        return -1;
    }
    si.cb = sizeof(si);
    strcpy(cmd, "agent.exe -t");
    if (CreateProcess(fnm, cmd, NULL, NULL,
            FALSE, CREATE_SUSPENDED | CREATE_DEFAULT_ERROR_MODE | CREATE_NO_WINDOW,
            NULL, NULL, &si, &prs) == 0) {
        set_win32_errno(GetLastError());
        return -1;
    }
    args = (ContextAttachArgs *)loc_alloc(sizeof(ContextAttachArgs));
    args->done = done;
    args->data = data;
    args->thread = prs.hThread;
    args->process = prs.hProcess;
    res = context_attach(prs.dwProcessId, done_context_attach, args, 0);
    if (res != 0) loc_free(args);
    return res;
#elif defined(_WRS_KERNEL)
    int tid = taskCreate("tTcf", 100, 0, 0x4000, (FUNCPTR)test_proc, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    if (tid == 0) return -1;
    taskStop(tid);
    taskActivate(tid);
    assert(taskIsStopped(tid));
    return context_attach(tid, done, data, 0);
#else
    int pid = fork();
    if (pid < 0) return -1;
    if (pid == 0) {
        int fd = sysconf(_SC_OPEN_MAX);
        while (fd > 3) close(--fd);
        if (context_attach_self() < 0) exit(1);
#if defined(ALT_RCBP_TEST)
        {
            char * fnm = canonicalize_file_name(ALT_RCBP_TEST);
            if (fnm != NULL) execl(fnm, fnm, "-t", (char *)NULL);
            exit(1);
        }
#elif defined(__linux__) && !ENABLE_TestSymbols
        {
            char buf[32];
            char * fnm = NULL;
            snprintf(buf, sizeof(buf), "/proc/%d/exe", getpid());
            fnm = canonicalize_file_name(buf);
            if (fnm != NULL) execl(fnm, fnm, "-t", (char *)NULL);
            exit(1);
        }
#else
        {
            if (tkill(getpid(), SIGSTOP) < 0) exit(1);
            test_proc();
            exit(0);
        }
#endif
    }
    return context_attach(pid, done, data, CONTEXT_ATTACH_SELF);
#endif
}

#endif /* PURE_RCBP_TEST */

#endif /* ENABLE_RCBP_TEST */
