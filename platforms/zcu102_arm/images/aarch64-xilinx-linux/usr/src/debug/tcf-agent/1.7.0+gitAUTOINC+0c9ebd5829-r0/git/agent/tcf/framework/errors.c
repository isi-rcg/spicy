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
 * This module defines agent error codes in addition to system codes defined in errno.h
 */

#include <tcf/config.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <time.h>
#include <tcf/framework/mdep-inet.h>
#include <tcf/framework/errors.h>
#include <tcf/framework/events.h>
#include <tcf/framework/exceptions.h>
#include <tcf/framework/streams.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/json.h>
#include <tcf/framework/trace.h>

#define ERR_MESSAGE_MIN         (STD_ERR_BASE + 100)
#if MEM_USAGE_FACTOR <= 2
#define ERR_MESSAGE_MAX         (STD_ERR_BASE + 129)
#else
#define ERR_MESSAGE_MAX         (STD_ERR_BASE + 199)
#endif

#define MESSAGE_CNT             (ERR_MESSAGE_MAX - ERR_MESSAGE_MIN + 1)

#if defined(_WIN32) || defined(__CYGWIN__)
#  define ERR_WINDOWS_MIN       (STD_ERR_BASE + 0x10000)
#  define ERR_WINDOWS_MAX       (ERR_WINDOWS_MIN + 0xffff)
#  define ERR_WINDOWS_CNT       (ERR_WINDOWS_MAX - ERR_WINDOWS_MIN + 1)
#endif

#define SRC_GAI     2
#define SRC_MESSAGE 3
#define SRC_REPORT  4

typedef struct ReportBuffer {
    ErrorReport pub; /* public part of error report */
    int posix_code;
    int refs;
    int gets;
} ReportBuffer;

typedef struct ErrorMessage {
    int source;
    int error;
    char * text;
    ReportBuffer * report;
} ErrorMessage;

static ErrorMessage msgs[MESSAGE_CNT];
static int msgs_pos = 0;

static char * msg_buf = NULL;
static size_t msg_buf_max = 0;
static size_t msg_buf_len = 0;

static void realloc_msg_buf(void) {
    assert(is_dispatch_thread());
    if (msg_buf_max <= msg_buf_len + 128 || msg_buf_max > msg_buf_len + 2048) {
        msg_buf_max = msg_buf_len + 256;
        msg_buf = (char *)loc_realloc(msg_buf, msg_buf_max);
    }
}

static void release_report(ReportBuffer * report) {
    if (report == NULL) return;
    assert(report->refs > report->gets);
    report->refs--;
    if (report->refs == 0) {
        while (report->pub.props != NULL) {
            ErrorReportItem * i = report->pub.props;
            report->pub.props = i->next;
            loc_free(i->name);
            loc_free(i->value);
            loc_free(i);
        }
        while (report->pub.param_cnt > 0) {
            loc_free(report->pub.params[--report->pub.param_cnt]);
        }
        loc_free(report->pub.params);
        loc_free(report->pub.format);
        loc_free(report);
    }
}

static ErrorMessage * alloc_msg(int source) {
    ErrorMessage * m = msgs + msgs_pos;
    assert(is_dispatch_thread());
    errno = ERR_MESSAGE_MIN + msgs_pos++;
    if (msgs_pos >= MESSAGE_CNT) msgs_pos = 0;
    release_report(m->report);
    loc_free(m->text);
    m->source = source;
    m->error = 0;
    m->report = NULL;
    m->text = NULL;
    return m;
}

#if defined(_WIN32) || defined(__CYGWIN__)

static char * system_strerror(DWORD error_code, HMODULE module) {
    WCHAR * buf = NULL;
    assert(is_dispatch_thread());
    msg_buf_len = 0;
    if (FormatMessageW(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS |
            FORMAT_MESSAGE_MAX_WIDTH_MASK |
            (module ? FORMAT_MESSAGE_FROM_HMODULE : 0),
            module,
            error_code,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), /* Default language */
            (LPWSTR)&buf, 0, NULL)) {
        msg_buf_len = WideCharToMultiByte(CP_UTF8, 0, buf, -1, NULL, 0, NULL, NULL);
        if (msg_buf_len > 0) {
            realloc_msg_buf();
            msg_buf_len = WideCharToMultiByte(CP_UTF8, 0, buf, -1, msg_buf, (int)msg_buf_max, NULL, NULL);
        }
    }
    if (msg_buf_len == 0) {
        realloc_msg_buf();
        msg_buf_len = snprintf(msg_buf, msg_buf_max, "System error code 0x%08x", (unsigned)error_code);
    }
    if (buf != NULL) LocalFree(buf);
    while (msg_buf_len > 0 && msg_buf[msg_buf_len - 1] <= ' ') msg_buf_len--;
    if (msg_buf_len > 0 && msg_buf[msg_buf_len - 1] == '.') msg_buf_len--;
    msg_buf[msg_buf_len] = 0;
    return msg_buf;
}

int set_win32_errno(DWORD win32_error_code) {
    if (win32_error_code == 0) return errno = 0;
    if (win32_error_code >= ERR_WINDOWS_CNT) {
        if (!is_dispatch_thread()) return errno = ERR_OTHER;
        return set_errno(ERR_OTHER, system_strerror(win32_error_code, NULL));
    }
    return errno = ERR_WINDOWS_MIN + win32_error_code;
}

int set_nt_status_errno(DWORD status) {
    int error = 0;
    assert(is_dispatch_thread());
    if (status != 0) {
        HMODULE module = LoadLibrary("NTDLL.DLL");
        char * msg = system_strerror(status, module);
        error = set_errno(ERR_OTHER, msg);
        FreeLibrary(module);
    }
    return errno = error;
}

#elif defined(__SYMBIAN32__)

#include <e32err.h>

static char * system_strerror(int err) {
    static char static_error[32];
    switch (err) {
    case KErrNotFound:
        return "item not found";
    case KErrNotSupported:
        return "functionality is not supported";
    case KErrBadHandle:
        return "an invalid handle";
    case KErrAccessDenied:
        return "access to a file is denied";
    case KErrAlreadyExists:
        return "an object already exists";
    case KErrWrite:
        return "error in write operation";
    case KErrPermissionDenied:
        return "permission denied";
    case KErrBadDescriptor:
        return "bad descriptor";
    default:
        snprintf(static_error, sizeof(static_error), "Error code %d", err);
        return static_error;
    }
}

#endif

static void append_format_parameter(char * type, char * style, char * param) {
    /* Note: 'param' is UTF-8 encoded JSON text */
    char str[64];
    if (param != NULL && (*param == '"' || strcmp(type, "number") == 0)) {
        Trap trap;
        ByteArrayInputStream buf;
        InputStream * inp = create_byte_array_input_stream(&buf, param, strlen(param));
        if (set_trap(&trap)) {
            if (*param == '"') {
                char * x = json_read_alloc_string(inp);
                if (x != NULL) {
                    char * s = x;
                    while (*s) {
                        realloc_msg_buf();
                        msg_buf[msg_buf_len++] = *s++;
                    }
                    loc_free(x);
                }
                param = NULL;
            }
            else {
                double x = json_read_double(inp);
                if (strcmp(style, "percent") == 0) {
                    snprintf(str, sizeof(str), "%ld%%", (long)(x * 100));
                }
                else if (strcmp(style, "integer") == 0) {
                    snprintf(str, sizeof(str), "%ld", (long)x);
                }
                else {
                    snprintf(str, sizeof(str), "%g", x);
                }
                param = str;
            }
            clear_trap(&trap);
        }
    }
    if (param != NULL) {
        while (*param) {
            realloc_msg_buf();
            msg_buf[msg_buf_len++] = *param++;
        }
    }
}

static const char * format_error_report_message(const char * fmt, char ** params, int param_cnt) {
    int fmt_pos = 0;
    int in_quotes = 0;

    msg_buf_len = 0;
    while (fmt[fmt_pos]) {
        char ch = fmt[fmt_pos++];
        realloc_msg_buf();
        if (in_quotes && ch == '\'') {
            in_quotes = 0;
        }
        else if (in_quotes) {
            msg_buf[msg_buf_len++] = ch;
        }
        else if (ch == '\'' && fmt[fmt_pos] == '\'') {
            msg_buf[msg_buf_len++] = ch;
            fmt_pos++;
        }
        else if (ch =='\'') {
            in_quotes = 1;
        }
        else if (ch == '{') {
            size_t j;
            int index = 0;
            char type[16];
            char style[16];
            type[0] = style[0] = 0;
            while (fmt[fmt_pos] >= '0' && fmt[fmt_pos] <= '9') {
                index = index * 10 + (fmt[fmt_pos++] - '0');
            }
            if (fmt[fmt_pos] == ',') {
                fmt_pos++;
                j = 0;
                while (fmt[fmt_pos] >= 'a' && fmt[fmt_pos] <= 'z') {
                    ch = fmt[fmt_pos++];
                    if (j < sizeof(type) - 1) type[j++] = ch;
                }
                type[j] = 0;
                if (fmt[fmt_pos] == ',') {
                    fmt_pos++;
                    j = 0;
                    while (fmt[fmt_pos] >= 'a' && fmt[fmt_pos] <= 'z') {
                        ch = fmt[fmt_pos++];
                        if (j < sizeof(style) - 1) style[j++] = ch;
                    }
                    style[j] = 0;
                }
            }
            if (index < param_cnt) append_format_parameter(type, style, params[index]);
            while (fmt[fmt_pos] && fmt[fmt_pos] != '}') fmt_pos++;
            if (fmt[fmt_pos] == '}') fmt_pos++;
        }
        else {
            msg_buf[msg_buf_len++] = ch;
        }
    }
    realloc_msg_buf();
    msg_buf[msg_buf_len++] = 0;
    return msg_buf;
}

static const char * posix_strerror(int err) {
    int n = errno;
    const char * msg = NULL;
    static char buf[32];
    errno = 0;
    msg = strerror(err);
    if (errno != 0 || msg == NULL || msg[0] == 0) {
        snprintf(buf, sizeof(buf), "Error 0x%08x", err);
        msg = buf;
    }
    errno = n;
    return msg;
}

const char * errno_to_str(int err) {
    switch (err) {
    case ERR_OTHER:             return "Unspecified failure";
    case ERR_JSON_SYNTAX:       return "JSON syntax error";
    case ERR_PROTOCOL:          return "Protocol format error";
    case ERR_BUFFER_OVERFLOW:   return "Buffer overflow";
    case ERR_CHANNEL_CLOSED:    return "Channel closed";
    case ERR_COMMAND_CANCELLED: return "Command canceled";
    case ERR_UNKNOWN_PEER:      return "Unknown peer";
    case ERR_BASE64:            return "Invalid BASE64 string";
    case ERR_EOF:               return "End of file";
    case ERR_ALREADY_STOPPED:   return "Already stopped";
    case ERR_ALREADY_EXITED:    return "Already exited";
    case ERR_ALREADY_RUNNING:   return "Already running";
    case ERR_ALREADY_ATTACHED:  return "Already attached";
    case ERR_IS_RUNNING:        return "Execution context is running";
    case ERR_INV_DATA_SIZE:     return "Invalid data size";
    case ERR_INV_CONTEXT:       return "Invalid context";
    case ERR_INV_ADDRESS:       return "Invalid address";
    case ERR_INV_EXPRESSION:    return "Invalid expression";
    case ERR_INV_FORMAT:        return "Format is not supported";
    case ERR_INV_NUMBER:        return "Invalid number";
    case ERR_INV_DWARF:         return "Error reading DWARF data";
    case ERR_SYM_NOT_FOUND:     return "Symbol not found";
    case ERR_UNSUPPORTED:       return "Unsupported command";
    case ERR_INV_DATA_TYPE:     return "Invalid data type";
    case ERR_INV_COMMAND:       return "Command is not recognized";
    case ERR_INV_TRANSPORT:     return "Invalid transport name";
    case ERR_CACHE_MISS:        return "Invalid data cache state";
    case ERR_NOT_ACTIVE:        return "Context is not active";
    case ERR_INV_CONT_OBJ:      return "Invalid address of containing object";
    default:
        if (err == 0) return "Success";
        if (err >= ERR_MESSAGE_MIN && err <= ERR_MESSAGE_MAX) {
            if (is_dispatch_thread()) {
                ErrorMessage * m = msgs + (err - ERR_MESSAGE_MIN);
                if (m->report != NULL && m->report->pub.format != NULL) {
                    return format_error_report_message(m->report->pub.format, m->report->pub.params, m->report->pub.param_cnt);
                }
                switch (m->source) {
                case SRC_GAI:
                    return loc_gai_strerror(m->error);
                case SRC_MESSAGE:
                    return m->text;
                case SRC_REPORT:
                    return errno_to_str(m->error);
                }
            }
            else {
                return "Cannot get error message text: errno_to_str() must be called from the main thread";
            }
        }
#if defined(_WIN32) || defined(__CYGWIN__)
        if (err >= ERR_WINDOWS_MIN && err <= ERR_WINDOWS_MAX) {
            if (is_dispatch_thread()) {
                return system_strerror(err - ERR_WINDOWS_MIN, NULL);
            }
            else {
                return "Cannot get error message text: errno_to_str() must be called from the main thread";
            }
        }
#endif
#ifdef __SYMBIAN32__
        if (err < 0) return system_strerror(err);
#endif
        break;
    }
    return posix_strerror(err);
}

int set_errno(int no, const char * msg) {
    errno = no;
    if (no != 0 && msg != NULL) {
        ErrorMessage * m = alloc_msg(SRC_MESSAGE);
        /* alloc_msg() assigns new value to 'errno',
         * need to be sure it does not change until this function exits.
         */
        int err = errno;
        m->error = get_error_code(no);
        if (no == ERR_OTHER) {
            m->text = loc_strdup(msg);
        }
        else {
            size_t msg_len = strlen(msg);
            if (msg_len == 0) {
                m->text = loc_strdup(errno_to_str(no));
            }
            else {
                const char * str = errno_to_str(no);
                if (msg[msg_len - 1] == '.' || msg[msg_len - 1] == '\n') {
                    size_t len = msg_len + strlen(str) + 2;
                    m->text = (char *)loc_alloc(len);
                    snprintf(m->text, len, "%s %s", msg, str);
                }
                else {
                    size_t len = msg_len + strlen(str) + 3;
                    m->text = (char *)loc_alloc(len);
                    snprintf(m->text, len, "%s. %s", msg, str);
                }
            }
        }
        errno = err;
    }
    return errno;
}

int set_fmt_errno(int no, const char * fmt, ...) {
    va_list ap;
    char arr[0x100];
    void * mem = NULL;
    char * buf = arr;
    size_t len = sizeof(arr);
    int err, n;

    while (1) {
        va_start(ap, fmt);
        n = vsnprintf(buf, len, fmt, ap);
        va_end(ap);
        if (n < 0) {
            if (len > 0x1000) break;
            len *= 2;
        }
        else {
            if (n < (int)len) break;
            len = n + 1;
        }
        mem = loc_realloc(mem, len);
        buf = (char *)mem;
    }
    err = n <= 0 ? no : set_errno(no, buf);
    if (mem != NULL) loc_free(mem);
    return errno = err;
}

int set_gai_errno(int no) {
    errno = no;
    if (no != 0) {
        ErrorMessage * m = alloc_msg(SRC_GAI);
        m->error = no;
    }
    return errno;
}

int set_error_report_errno(ErrorReport * r) {
    errno = 0;
    if (r != NULL) {
        ReportBuffer * report = (ReportBuffer *)((char *)r - offsetof(ReportBuffer, pub));
        ErrorMessage * m = alloc_msg(SRC_REPORT);
        m->error = report->pub.code + STD_ERR_BASE;
        m->report = report;
        report->refs++;
    }
    return errno;
}

int get_error_code(int no) {
    while (no >= ERR_MESSAGE_MIN && no <= ERR_MESSAGE_MAX) {
        ErrorMessage * m = msgs + (no - ERR_MESSAGE_MIN);
        assert(is_dispatch_thread());
        switch (m->source) {
        case SRC_REPORT:
        case SRC_MESSAGE:
            if (m->report != NULL && m->report->posix_code != 0) return m->report->posix_code;
            no = m->error;
            continue;
        }
        return ERR_OTHER;
    }
    return no;
}

static void add_report_prop(ReportBuffer * report, const char * name, ByteArrayOutputStream * buf) {
    ErrorReportItem * i = (ErrorReportItem *)loc_alloc(sizeof(ErrorReportItem));
    i->name = loc_strdup(name);
    get_byte_array_output_stream_data(buf, &i->value, NULL);
    i->next = report->pub.props;
    report->pub.props = i;
}

static void add_report_prop_int(ReportBuffer * report, const char * name, unsigned long n) {
    ByteArrayOutputStream buf;
    OutputStream * out = create_byte_array_output_stream(&buf);
    json_write_ulong(out, n);
    write_stream(out, 0);
    add_report_prop(report, name, &buf);
}

static void add_report_prop_str(ReportBuffer * report, const char * name, const char * str) {
    ByteArrayOutputStream buf;
    OutputStream * out = create_byte_array_output_stream(&buf);
    json_write_string(out, str);
    write_stream(out, 0);
    add_report_prop(report, name, &buf);
}

ErrorReport * get_error_report(int err) {
    ErrorMessage * m = NULL;
    if (err >= ERR_MESSAGE_MIN && err <= ERR_MESSAGE_MAX) {
        assert(is_dispatch_thread());
        m = msgs + (err - ERR_MESSAGE_MIN);
        if (m->report != NULL) {
            m->report->refs++;
            m->report->gets++;
            return &m->report->pub;
        }
    }
    if (err != 0) {
        ReportBuffer * report = (ReportBuffer *)loc_alloc_zero(sizeof(ReportBuffer));
        struct timespec timenow;

        if (clock_gettime(CLOCK_REALTIME, &timenow) == 0) {
            report->pub.time_stamp = (uint64_t)timenow.tv_sec * 1000 + timenow.tv_nsec / 1000000;
        }

        report->pub.format = loc_strdup(errno_to_str(err));

#if defined(_WIN32) || defined(__CYGWIN__)
        if (err >= ERR_WINDOWS_MIN && err <= ERR_WINDOWS_MAX) {
            add_report_prop_int(report, "AltCode", err - ERR_WINDOWS_MIN);
            add_report_prop_str(report, "AltOrg", "_WIN32");
            err = ERR_OTHER;
        }
#endif

        if (m != NULL) {
            if (m->source == SRC_MESSAGE) {
                err = m->error;
            }
            else {
                err = ERR_OTHER;
            }
        }

        if (err < STD_ERR_BASE || err > ERR_MESSAGE_MAX) {
            report->posix_code = err;
            add_report_prop_int(report, "AltCode", err);
#if defined(_MSC_VER)
            add_report_prop_str(report, "AltOrg", "MSC");
#elif defined(_WRS_KERNEL)
            add_report_prop_str(report, "AltOrg", "VxWorks");
#elif defined(__CYGWIN__)
            add_report_prop_str(report, "AltOrg", "CygWin");
#elif defined(__linux__)
            add_report_prop_str(report, "AltOrg", "Linux");
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__APPLE__)
            add_report_prop_str(report, "AltOrg", "BSD");
#elif defined(__SYMBIAN32__)
            add_report_prop_str(report, "AltOrg", "Symbian");
#else
            add_report_prop_str(report, "AltOrg", "POSIX");
#endif
            err = ERR_OTHER;
        }

        assert(err >= STD_ERR_BASE);
        assert(err < ERR_MESSAGE_MIN);

        report->pub.code = err - STD_ERR_BASE;
        report->refs = 1;
        report->gets = 1;
        if (m != NULL) {
            assert(m->report == NULL);
            m->report = report;
            report->refs++;
        }
        return &report->pub;
    }
    return NULL;
}

ErrorReport * create_error_report(void) {
    ReportBuffer * report = (ReportBuffer *)loc_alloc_zero(sizeof(ReportBuffer));
    report->refs = 1;
    report->gets = 1;
    return &report->pub;
}

void release_error_report(ErrorReport * r) {
    if (r != NULL) {
        ReportBuffer * report = (ReportBuffer *)((char *)r - offsetof(ReportBuffer, pub));
        assert(is_dispatch_thread());
        assert(report->gets > 0);
        report->gets--;
        release_report(report);
    }
}

int compare_error_reports(ErrorReport * x, ErrorReport * y) {
    int i;
    if (x == y) return 1;
    if (x == NULL || y == NULL) return 0;
    if (x->code != y->code) return 0;
    if (x->format != y->format) {
        if (x->format == NULL || y->format == NULL) return 0;
        if (strcmp(x->format, y->format)) return 0;
    }
    if (x->param_cnt != y->param_cnt) return 0;
    for (i = 0; i < x->param_cnt; i++) {
        char * px = x->params[i];
        char * py = y->params[i];
        if (px != py) {
            if (px == NULL || py == NULL) return 0;
            if (strcmp(px, py)) return 0;
        }
    }
    if (x->props != y->props) {
        ErrorReportItem * px = x->props;
        ErrorReportItem * py = x->props;
        while (px != NULL || py  != NULL) {
            if (px != py) {
                if (px == NULL || py == NULL) return 0;
                if (strcmp(px->name, py->name)) return 0;
                if (strcmp(px->value, py->value)) return 0;
            }
            px = px->next;
            py = py->next;
        }
    }
    return 1;
}

#ifdef NDEBUG

void check_error(int error) {
    if (error == 0) return;
#if ENABLE_Trace
    trace(LOG_ALWAYS, "Fatal error %d: %s", error, errno_to_str(error));
    trace(LOG_ALWAYS, "  Exiting agent...");
    if (log_file == stderr) exit(1);
#endif
    fprintf(stderr, "Fatal error %d: %s", error, errno_to_str(error));
    fprintf(stderr, "  Exiting agent...");
    exit(1);
}

#else /* NDEBUG */

void check_error_debug(const char * file, int line, int error) {
    if (error == 0) return;
#if ENABLE_Trace
    trace(LOG_ALWAYS, "Fatal error %d: %s", error, errno_to_str(error));
    trace(LOG_ALWAYS, "  At %s:%d", file, line);
    trace(LOG_ALWAYS, "  Exiting agent...");
    if (log_file == stderr) exit(1);
#endif
    fprintf(stderr, "Fatal error %d: %s\n", error, errno_to_str(error));
    fprintf(stderr, "  At %s:%d\n", file, line);
    fprintf(stderr, "  Exiting agent...\n");
    exit(1);
}

#endif /* NDEBUG */
