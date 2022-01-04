/*
 * Copyright (c) 2008-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */
/* standard ranges/values/keys */

/* This file is generated and should not be modified.  See the maketables
 * script if you want to modify this. */

/* NULL, strcmp */
#include <string.h>
/* tables for debug_type */
/* Note: For convenience/consistency with the old numerc debug levels
 * stuff, these are sorted in a very rough approximation of "likelihood
 * that the user will care." So, PSEUDO_DEBUG=1 will pick up the
 * consistency checks. In general, most numeric debug levels will be
 * significantly less chatty than they used to be; there was one
 * level 5 message, I think, and nothing else above a 4. Which was a
 * problem.
 * Note: Descriptions should be under 32 characters to match the formatting
 * in pseudo's help message. */

typedef enum {
	PDBG_UNKNOWN = -1,
	PDBG_NONE = 0,
	PDBG_CONSISTENCY,
	PDBG_FILE,
	PDBG_OP,
	PDBG_PID,
	PDBG_CLIENT,
	PDBG_SERVER,
	PDBG_DB,
	PDBG_XATTRDB,
	PDBG_PROFILE,
	PDBG_SYSCALL,
	PDBG_ENV,
	PDBG_CHROOT,
	PDBG_PATH,
	PDBG_SQL,
	PDBG_WRAPPER,
	PDBG_IPC,
	PDBG_INVOKE,
	PDBG_BENCHMARK,
	PDBG_VERBOSE,
	PDBG_XATTR,
	PDBG_MAX
} pseudo_debug_type_t;
typedef enum {
	PDBGF_CONSISTENCY = (1 << PDBG_CONSISTENCY),
	PDBGF_FILE = (1 << PDBG_FILE),
	PDBGF_OP = (1 << PDBG_OP),
	PDBGF_PID = (1 << PDBG_PID),
	PDBGF_CLIENT = (1 << PDBG_CLIENT),
	PDBGF_SERVER = (1 << PDBG_SERVER),
	PDBGF_DB = (1 << PDBG_DB),
	PDBGF_XATTRDB = (1 << PDBG_XATTRDB),
	PDBGF_PROFILE = (1 << PDBG_PROFILE),
	PDBGF_SYSCALL = (1 << PDBG_SYSCALL),
	PDBGF_ENV = (1 << PDBG_ENV),
	PDBGF_CHROOT = (1 << PDBG_CHROOT),
	PDBGF_PATH = (1 << PDBG_PATH),
	PDBGF_SQL = (1 << PDBG_SQL),
	PDBGF_WRAPPER = (1 << PDBG_WRAPPER),
	PDBGF_IPC = (1 << PDBG_IPC),
	PDBGF_INVOKE = (1 << PDBG_INVOKE),
	PDBGF_BENCHMARK = (1 << PDBG_BENCHMARK),
	PDBGF_VERBOSE = (1 << PDBG_VERBOSE),
	PDBGF_XATTR = (1 << PDBG_XATTR),
} pseudo_debug_type_f;
extern const char *pseudo_debug_type_name(pseudo_debug_type_t);
extern pseudo_debug_type_t pseudo_debug_type_id(const char *);
extern unsigned char pseudo_debug_type_symbolic(pseudo_debug_type_t id);
extern int pseudo_debug_type_symbolic_id(unsigned char val);
extern const char * pseudo_debug_type_description(pseudo_debug_type_t id);

/* tables for exit_status */
/* 0 indicates success. The others indicate where in the startup process
 * something went wrong, for any point at which we'd exit. */

typedef enum {
	PSEUDO_EXIT_UNKNOWN = -1,
	PSEUDO_EXIT_NONE = 0,
	PSEUDO_EXIT_GENERAL,
	PSEUDO_EXIT_FORK_FAILED,
	PSEUDO_EXIT_LOCK_PATH,
	PSEUDO_EXIT_LOCK_HELD,
	PSEUDO_EXIT_LOCK_FAILED,
	PSEUDO_EXIT_TIMEOUT,
	PSEUDO_EXIT_WAITPID,
	PSEUDO_EXIT_SOCKET_CREATE,
	PSEUDO_EXIT_SOCKET_FD,
	PSEUDO_EXIT_SOCKET_PATH,
	PSEUDO_EXIT_SOCKET_UNLINK,
	PSEUDO_EXIT_SOCKET_BIND,
	PSEUDO_EXIT_SOCKET_LISTEN,
	PSEUDO_EXIT_LISTEN_FD,
	PSEUDO_EXIT_PSEUDO_LOADED,
	PSEUDO_EXIT_PSEUDO_PREFIX,
	PSEUDO_EXIT_PSEUDO_INVOCATION,
	PSEUDO_EXIT_EPOLL_CREATE,
	PSEUDO_EXIT_EPOLL_CTL,
	PSEUDO_EXIT_,
	PSEUDO_EXIT_MAX
} pseudo_exit_status_t;

extern const char *pseudo_exit_status_name(pseudo_exit_status_t);
extern pseudo_exit_status_t pseudo_exit_status_id(const char *);
extern char * pseudo_exit_status_message(pseudo_exit_status_t id);

/* tables for msg_type */

typedef enum {
	PSEUDO_MSG_UNKNOWN = -1,
	PSEUDO_MSG_NONE = 0,
	PSEUDO_MSG_PING,
	PSEUDO_MSG_SHUTDOWN,
	PSEUDO_MSG_OP,
	PSEUDO_MSG_ACK,
	PSEUDO_MSG_NAK,
	PSEUDO_MSG_FASTOP,
	PSEUDO_MSG_MAX
} pseudo_msg_type_t;

extern const char *pseudo_msg_type_name(pseudo_msg_type_t);
extern pseudo_msg_type_t pseudo_msg_type_id(const char *);


/* tables for op */

typedef enum {
	OP_UNKNOWN = -1,
	OP_NONE = 0,
	OP_CHDIR,
	OP_CHMOD,
	OP_CHOWN,
	OP_CHROOT,
	OP_CLOSE,
	OP_CREAT,
	OP_DUP,
	OP_FCHMOD,
	OP_FCHOWN,
	OP_FSTAT,
	OP_LINK,
	OP_MKDIR,
	OP_MKNOD,
	OP_OPEN,
	OP_RENAME,
	OP_STAT,
	OP_UNLINK,
	OP_SYMLINK,
	OP_EXEC,
	OP_MAY_UNLINK,
	OP_DID_UNLINK,
	OP_CANCEL_UNLINK,
	OP_GET_XATTR,
	OP_LIST_XATTR,
	OP_REMOVE_XATTR,
	OP_SET_XATTR,
	OP_CREATE_XATTR,
	OP_REPLACE_XATTR,
	OP_MAX
} pseudo_op_t;

extern const char *pseudo_op_name(pseudo_op_t);
extern pseudo_op_t pseudo_op_id(const char *);
extern int pseudo_op_wait(pseudo_op_t id);

/* tables for query_field */
/* Note:  These are later used as bitwise masks into a value,
 * currently an unsigned long; if the number of these gets up
 * near 32, that may take rethinking.  The first thing to
 * go would probably be something special to do for FTYPE and
 * PERM because they aren't "real" database fields -- both
 * of them actually imply MODE. */

typedef enum {
	PSQF_UNKNOWN = -1,
	PSQF_NONE = 0,
	PSQF_ACCESS,
	PSQF_CLIENT,
	PSQF_DEV,
	PSQF_FD,
	PSQF_FTYPE,
	PSQF_GID,
	PSQF_ID,
	PSQF_INODE,
	PSQF_MODE,
	PSQF_OP,
	PSQF_ORDER,
	PSQF_PATH,
	PSQF_PERM,
	PSQF_PROGRAM,
	PSQF_RESULT,
	PSQF_SEVERITY,
	PSQF_STAMP,
	PSQF_TAG,
	PSQF_TEXT,
	PSQF_TYPE,
	PSQF_UID,
	PSQF_MAX
} pseudo_query_field_t;

extern const char *pseudo_query_field_name(pseudo_query_field_t);
extern pseudo_query_field_t pseudo_query_field_id(const char *);


/* tables for query_type */

typedef enum {
	PSQT_UNKNOWN = -1,
	PSQT_NONE = 0,
	PSQT_EXACT,
	PSQT_LESS,
	PSQT_GREATER,
	PSQT_BITAND,
	PSQT_NOTEQUAL,
	PSQT_LIKE,
	PSQT_NOTLIKE,
	PSQT_SQLPAT,
	PSQT_MAX
} pseudo_query_type_t;

extern const char *pseudo_query_type_name(pseudo_query_type_t);
extern pseudo_query_type_t pseudo_query_type_id(const char *);
extern const char * pseudo_query_type_sql(pseudo_query_type_t id);

/* tables for res */

typedef enum {
	RESULT_UNKNOWN = -1,
	RESULT_NONE = 0,
	RESULT_SUCCEED,
	RESULT_FAIL,
	RESULT_ERROR,
	RESULT_MAX
} pseudo_res_t;

extern const char *pseudo_res_name(pseudo_res_t);
extern pseudo_res_t pseudo_res_id(const char *);


/* tables for sev */

typedef enum {
	SEVERITY_UNKNOWN = -1,
	SEVERITY_NONE = 0,
	SEVERITY_DEBUG,
	SEVERITY_INFO,
	SEVERITY_WARN,
	SEVERITY_ERROR,
	SEVERITY_CRITICAL,
	SEVERITY_MAX
} pseudo_sev_t;

extern const char *pseudo_sev_name(pseudo_sev_t);
extern pseudo_sev_t pseudo_sev_id(const char *);


