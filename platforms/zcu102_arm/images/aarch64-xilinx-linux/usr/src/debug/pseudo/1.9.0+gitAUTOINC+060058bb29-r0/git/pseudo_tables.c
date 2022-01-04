/*
 * Copyright (c) 2008-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */
/* Tables matching enums to strings */

/* This file is generated and should not be modified.  See the maketables
 * script if you want to modify this. */

#include "pseudo_tables.h"

/* tables for debug_type */

static const char *debug_type_id_to_name[] = {
	"none",
	"consistency",
	"file",
	"op",
	"pid",
	"client",
	"server",
	"db",
	"xattrdb",
	"profile",
	"syscall",
	"env",
	"chroot",
	"path",
	"sql",
	"wrapper",
	"ipc",
	"invoke",
	"benchmark",
	"verbose",
	"xattr",
	NULL
};
static unsigned char debug_type_id_to_symbolic[] = {
	'\0',
	'n',
	'f',
	'o',
	'P',
	'c',
	'v',
	'd',
	'D',
	'R',
	'y',
	'e',
	'r',
	'p',
	's',
	'w',
	'i',
	'k',
	'b',
	'V',
	'x',
	0
};
static int debug_type_symbolic_to_id[] = {
	['n'] = 1,
	['f'] = 2,
	['o'] = 3,
	['P'] = 4,
	['c'] = 5,
	['v'] = 6,
	['d'] = 7,
	['D'] = 8,
	['R'] = 9,
	['y'] = 10,
	['e'] = 11,
	['r'] = 12,
	['p'] = 13,
	['s'] = 14,
	['w'] = 15,
	['i'] = 16,
	['k'] = 17,
	['b'] = 18,
	['V'] = 19,
	['x'] = 20,
};
static const char * debug_type_id_to_description[] = {
	NULL,
	"consistency checks",
	"file creation/deletion",
	"operations",
	"show process IDs",
	"client side startup/shutdown",
	"server side startup/shutdown",
	"database interactions",
	"xattr database",
	"profiling",
	"system calls",
	"environment manipulation",
	"chroot functionality",
	"path computations",
	"SQL query information",
	"wrapper functionality",
	"client/server interactions",
	"invocation and launching",
	"performance statistics",
	"extra detail",
	"extended attributes",
	0
};

/* functions for debug_type */
extern const char *
pseudo_debug_type_name(pseudo_debug_type_t id) {
	if (id < 0 || id >= PDBG_MAX)
		return "unknown";
	return debug_type_id_to_name[id];
}

extern pseudo_debug_type_t
pseudo_debug_type_id(const char *name) {
	int id;

	if (!name)
		return -1;

	for (id = 0; id < PDBG_MAX; ++id)
		if (!strcmp(debug_type_id_to_name[id], name))
			return id;

	return -1;
}
extern unsigned char
pseudo_debug_type_symbolic(pseudo_debug_type_t id) {
	if (id < 0 || id >= PDBG_MAX)
		return '\0';
	return debug_type_id_to_symbolic[id];
}
extern int
pseudo_debug_type_symbolic_id(unsigned char val) {
	if ((val < 'D') || (val > 'y')) {
		return -1;
	}
	if (debug_type_symbolic_to_id[val] != 0) {
		return debug_type_symbolic_to_id[val];
	}
	return -1;
}
extern const char *
pseudo_debug_type_description(pseudo_debug_type_t id) {
	if (id < 0 || id >= PDBG_MAX)
		return NULL;
	return debug_type_id_to_description[id];
}

/* tables for exit_status */

static const char *exit_status_id_to_name[] = {
	"none",
	"general",
	"fork_failed",
	"lock_path",
	"lock_held",
	"lock_failed",
	"timeout",
	"waitpid",
	"socket_create",
	"socket_fd",
	"socket_path",
	"socket_unlink",
	"socket_bind",
	"socket_listen",
	"listen_fd",
	"pseudo_loaded",
	"pseudo_prefix",
	"pseudo_invocation",
	"epoll_create",
	"epoll_ctl",
	"",
	NULL
};
static char * exit_status_id_to_message[] = {
	"exit status unknown",
	"unspecified error",
	"fork failed",
	"path allocation failure for lock file",
	"lock already held by another process",
	"could not create/lock lockfile",
	"child process timed out",
	"waitpid() for child process failed unexpectedly",
	"couldn't create socket",
	"couldn't move socket to safe file descriptor",
	"path allocation failure for server socket",
	"couldn't unlink existing server socket",
	"couldn't bind server socket",
	"couldn't listen on server socket",
	"server loop had no valid listen fd",
	"server couldn't get out of pseudo environment",
	"couldn't get valid pseudo prefix",
	"invalid server command arguments",
	"epoll_create() failed",
	"epoll_ctl() failed",
	"exit status unknown",
	0
};

/* functions for exit_status */
extern const char *
pseudo_exit_status_name(pseudo_exit_status_t id) {
	if (id < 0 || id >= PSEUDO_EXIT_MAX)
		return "unknown";
	return exit_status_id_to_name[id];
}

extern pseudo_exit_status_t
pseudo_exit_status_id(const char *name) {
	int id;

	if (!name)
		return -1;

	for (id = 0; id < PSEUDO_EXIT_MAX; ++id)
		if (!strcmp(exit_status_id_to_name[id], name))
			return id;

	return -1;
}
extern char *
pseudo_exit_status_message(pseudo_exit_status_t id) {
	if (id < 0 || id >= PSEUDO_EXIT_MAX)
		return "exit status unknown";
	return exit_status_id_to_message[id];
}

/* tables for msg_type */

static const char *msg_type_id_to_name[] = {
	"none",
	"ping",
	"shutdown",
	"op",
	"ack",
	"nak",
	"fastop",
	NULL
};


/* functions for msg_type */
extern const char *
pseudo_msg_type_name(pseudo_msg_type_t id) {
	if (id < 0 || id >= PSEUDO_MSG_MAX)
		return "unknown";
	return msg_type_id_to_name[id];
}

extern pseudo_msg_type_t
pseudo_msg_type_id(const char *name) {
	int id;

	if (!name)
		return -1;

	for (id = 0; id < PSEUDO_MSG_MAX; ++id)
		if (!strcmp(msg_type_id_to_name[id], name))
			return id;

	return -1;
}


/* tables for op */

static const char *op_id_to_name[] = {
	"none",
	"chdir",
	"chmod",
	"chown",
	"chroot",
	"close",
	"creat",
	"dup",
	"fchmod",
	"fchown",
	"fstat",
	"link",
	"mkdir",
	"mknod",
	"open",
	"rename",
	"stat",
	"unlink",
	"symlink",
	"exec",
	"may-unlink",
	"did-unlink",
	"cancel-unlink",
	"get-xattr",
	"list-xattr",
	"remove-xattr",
	"set-xattr",
	"create-xattr",
	"replace-xattr",
	NULL
};
static int op_id_to_wait[] = {
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	1,
	0,
	0,
	1,
	0,
	0,
	1,
	0,
	0,
	0,
	1,
	0,
	0,
	1,
	1,
	1,
	0,
	1,
	1,
	0
};

/* functions for op */
extern const char *
pseudo_op_name(pseudo_op_t id) {
	if (id < 0 || id >= OP_MAX)
		return "unknown";
	return op_id_to_name[id];
}

extern pseudo_op_t
pseudo_op_id(const char *name) {
	int id;

	if (!name)
		return -1;

	for (id = 0; id < OP_MAX; ++id)
		if (!strcmp(op_id_to_name[id], name))
			return id;

	return -1;
}
extern int
pseudo_op_wait(pseudo_op_t id) {
	if (id < 0 || id >= OP_MAX)
		return 0;
	return op_id_to_wait[id];
}

/* tables for query_field */

static const char *query_field_id_to_name[] = {
	"none",
	"access",
	"client",
	"dev",
	"fd",
	"ftype",
	"gid",
	"id",
	"inode",
	"mode",
	"op",
	"order",
	"path",
	"perm",
	"program",
	"result",
	"severity",
	"stamp",
	"tag",
	"text",
	"type",
	"uid",
	NULL
};


/* functions for query_field */
extern const char *
pseudo_query_field_name(pseudo_query_field_t id) {
	if (id < 0 || id >= PSQF_MAX)
		return "unknown";
	return query_field_id_to_name[id];
}

extern pseudo_query_field_t
pseudo_query_field_id(const char *name) {
	int id;

	if (!name)
		return -1;

	for (id = 0; id < PSQF_MAX; ++id)
		if (!strcmp(query_field_id_to_name[id], name))
			return id;

	return -1;
}


/* tables for query_type */

static const char *query_type_id_to_name[] = {
	"none",
	"exact",
	"less",
	"greater",
	"bitand",
	"notequal",
	"like",
	"notlike",
	"sqlpat",
	NULL
};
static const char * query_type_id_to_sql[] = {
	"LITTLE BOBBY TABLES",
	"=",
	"<",
	">",
	"&",
	"!=",
	"LIKE",
	"NOT LIKE",
	"LIKE",
	0
};

/* functions for query_type */
extern const char *
pseudo_query_type_name(pseudo_query_type_t id) {
	if (id < 0 || id >= PSQT_MAX)
		return "unknown";
	return query_type_id_to_name[id];
}

extern pseudo_query_type_t
pseudo_query_type_id(const char *name) {
	int id;

	if (!name)
		return -1;

	for (id = 0; id < PSQT_MAX; ++id)
		if (!strcmp(query_type_id_to_name[id], name))
			return id;

	return -1;
}
extern const char *
pseudo_query_type_sql(pseudo_query_type_t id) {
	if (id < 0 || id >= PSQT_MAX)
		return "LITTLE BOBBY TABLES";
	return query_type_id_to_sql[id];
}

/* tables for res */

static const char *res_id_to_name[] = {
	"none",
	"succeed",
	"fail",
	"error",
	NULL
};


/* functions for res */
extern const char *
pseudo_res_name(pseudo_res_t id) {
	if (id < 0 || id >= RESULT_MAX)
		return "unknown";
	return res_id_to_name[id];
}

extern pseudo_res_t
pseudo_res_id(const char *name) {
	int id;

	if (!name)
		return -1;

	for (id = 0; id < RESULT_MAX; ++id)
		if (!strcmp(res_id_to_name[id], name))
			return id;

	return -1;
}


/* tables for sev */

static const char *sev_id_to_name[] = {
	"none",
	"debug",
	"info",
	"warn",
	"error",
	"critical",
	NULL
};


/* functions for sev */
extern const char *
pseudo_sev_name(pseudo_sev_t id) {
	if (id < 0 || id >= SEVERITY_MAX)
		return "unknown";
	return sev_id_to_name[id];
}

extern pseudo_sev_t
pseudo_sev_id(const char *name) {
	int id;

	if (!name)
		return -1;

	for (id = 0; id < SEVERITY_MAX; ++id)
		if (!strcmp(sev_id_to_name[id], name))
			return id;

	return -1;
}


