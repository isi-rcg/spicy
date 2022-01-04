/*
 * pseudo_ipc.c, IPC code for pseudo client/server
 *
 * Copyright (c) 2008-2010 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/socket.h>

#include "pseudo.h"
#include "pseudo_ipc.h"

/* Short reads or writes can cause a sigpipe, killing the program, so
 * we trap it and report that something happened. We can avoid the
 * overhead of setting/restoring the SIGPIPE handler if MSG_NOSIGNAL
 * is available.
 */
#ifndef MSG_NOSIGNAL
static sig_atomic_t pipe_error = 0;
static void (*old_handler)(int) = SIG_DFL;

static void
sigpipe_trap(int unused __attribute__((unused))) {
	pipe_error = 1;
}

static void
ignore_sigpipe(void) {
	pipe_error = 0;
	old_handler = signal(SIGPIPE, sigpipe_trap);
}

static void
allow_sigpipe(void) {
	signal(SIGPIPE, old_handler);
}
#else
#define pipe_error 0
#endif

#if 0
/* useful only when debugging crazy stuff */
static void
display_msg_header(pseudo_msg_t *msg) {
	pseudo_debug(PDBGF_IPC | PDBGF_VERBOSE, "type: %d\n", msg->type);
	pseudo_debug(PDBGF_IPC | PDBGF_VERBOSE, "inode: %llu\n", (unsigned long long) msg->ino);
	pseudo_debug(PDBGF_IPC | PDBGF_VERBOSE, "uid: %d\n", msg->uid);
	pseudo_debug(PDBGF_IPC | PDBGF_VERBOSE, "pathlen: %d\n", (int) msg->pathlen);
	if (msg->pathlen) {
		pseudo_debug(PDBGF_IPC | PDBGF_VERBOSE, "path: %s\n", msg->path);
	}
}
#endif

#ifdef MSG_NOSIGNAL
static int
do_send(int fd, struct iovec *iov, int iovlen)
{
	struct msghdr hdr;

	memset(&hdr, 0, sizeof(hdr));
	hdr.msg_iov = iov;
	hdr.msg_iovlen = iovlen;

	return sendmsg(fd, &hdr, MSG_NOSIGNAL);
}
#else
static int
do_send(int fd, struct iovec *iov, int iovlen)
{
	int r, s = 0, i;

	ignore_sigpipe();
	for (i = 0; i < iovlen; ++i) {
		r = write(fd, iov[i].iov_base, iov[i].iov_len);
		s += r;
		if (r != (int)iov[i].iov_len)
			break;
	}
	allow_sigpipe();
	return s;
}
#endif

/*
 * send message on fd
 * return:
 * 0 on success
 * >0 on error
 * <0 on error suggesting other end is dead
 */
int
pseudo_msg_send(int fd, pseudo_msg_t *msg, size_t len, const char *path) {
	struct iovec iov[2];
	int r, iovc;

	if (!msg)
		return 1;

	if (fd < 0)
		return -1;

	if (path) {
		pseudo_debug(PDBGF_IPC, "msg type %d (%s), external path %s, mode 0%o\n",
			msg->type, pseudo_op_name(msg->op), path, (int) msg->mode);
		if (len == (size_t) -1)
			len = strlen(path) + 1;
		msg->pathlen = len;
		iov[0].iov_base = msg;
		iov[0].iov_len = PSEUDO_HEADER_SIZE;
		iov[1].iov_base = (void*)path;
		iov[1].iov_len = len;
		iovc = 2;
	} else {
		pseudo_debug(PDBGF_IPC, "msg type %d (%s), result %d (%s), path %.*s, mode 0%o\n",
			msg->type, pseudo_op_name(msg->op),
			msg->result, pseudo_res_name(msg->result),
			msg->pathlen, msg->path, (int) msg->mode);
		// display_msg_header(msg);
		iov[0].iov_base = msg;
		iov[0].iov_len = PSEUDO_HEADER_SIZE + msg->pathlen;
		iovc = 1;
	}
	r = do_send(fd, iov, iovc);
	pseudo_debug(PDBGF_IPC | PDBGF_VERBOSE, "wrote %d bytes\n", r);
	if (pipe_error || (r == -1 && (errno == EBADF || errno == EPIPE)))
		return -1;
	return ((size_t) r != PSEUDO_HEADER_SIZE + msg->pathlen);
}

/* attempts to receive a message from fd
 * return is allocated message if one is provided
 */
pseudo_msg_t *
pseudo_msg_receive(int fd) {
	static pseudo_msg_t *incoming;
	static size_t incoming_pathlen;
	pseudo_msg_t *newmsg, header;
	int r;

	if (fd < 0)
		return 0;
	errno = 0;
	r = read(fd, &header, PSEUDO_HEADER_SIZE);
	if (r == -1) {
		pseudo_debug(PDBGF_IPC, "read failed: %s\n", strerror(errno));
		return 0;
	}
	if (r < (int) PSEUDO_HEADER_SIZE) {
		pseudo_debug(PDBGF_IPC, "got only %d bytes (%s)\n", r, strerror(errno));
		return 0;
	}
	pseudo_debug(PDBGF_IPC, "got header, type %d, pathlen %d\n", header.type, (int) header.pathlen);
	// display_msg_header(&header);
	if (!incoming || header.pathlen >= incoming_pathlen) {
		newmsg = pseudo_msg_new(header.pathlen + 128, 0);
		if (!newmsg) {
			pseudo_diag("Couldn't allocate header for path of %d bytes.\n",
				(int) header.pathlen);
			return 0;
		}
		free(incoming);
		incoming = newmsg;
		incoming_pathlen = header.pathlen + 128;
	}
	*incoming = header;
	if (incoming->pathlen) {
		r = read(fd, incoming->path, incoming->pathlen);
		if (r < (int) incoming->pathlen) {
			pseudo_debug(PDBGF_IPC, "short read on path, expecting %d, got %d\n",
				(int) incoming->pathlen, r);
			return 0;
		}
		/* ensure null termination */
		incoming->path[r] = '\0';
	}
	// display_msg_header(incoming);
	return incoming;
}

/* duplicate a message -- currently totally unused */
pseudo_msg_t *
pseudo_msg_dup(pseudo_msg_t *old) {
	pseudo_msg_t *newmsg;
	if (!old)
		return NULL;
	newmsg = malloc(sizeof(pseudo_msg_t) + old->pathlen);
	if (!newmsg)
		return NULL;
	memcpy(newmsg, old, sizeof(pseudo_msg_t) + old->pathlen);
	return newmsg;
}

/* allocate a message either with pathlen chars of storage or with enough
 * storage for path
 */
pseudo_msg_t *
pseudo_msg_new(size_t pathlen, const char *path) {
	pseudo_msg_t *newmsg;
	if (pathlen) {
		newmsg = malloc(sizeof(pseudo_msg_t) + pathlen);
		if (newmsg) {
			newmsg->pathlen = pathlen;
			if (path)
				memcpy(newmsg->path, path, pathlen);
			newmsg->path[pathlen - 1] = '\0';
		}
		return newmsg;
	} else {
		if (!path) {
			/* no pathlen, no path == purely informational */
			newmsg = malloc(sizeof(pseudo_msg_t));
			if (newmsg) {
				newmsg->pathlen = 0;
			}
			return newmsg;
		} else {
			pathlen = strlen(path) + 1;
			newmsg = malloc(sizeof(pseudo_msg_t) + pathlen);
			if (newmsg) {
				memcpy(newmsg->path, path, pathlen);
				newmsg->pathlen = pathlen;
			}
			return newmsg;
		}
	}
}

/* The following functions populate messages from statbufs and vice versa.
 * It is intentional that copying a message into a stat doesn't touch nlink;
 * the nlink value was not stored in the database (it is in the message at
 * all only so the server can do sanity checks).
 */

void
pseudo_msg_stat(pseudo_msg_t *msg, const PSEUDO_STATBUF *buf) {
	if (!msg || !buf)
		return;
	msg->uid = buf->st_uid;
	msg->gid = buf->st_gid;
	msg->dev = buf->st_dev;
	msg->ino = buf->st_ino;
	msg->mode = buf->st_mode;
	msg->rdev = buf->st_rdev;
	msg->nlink = buf->st_nlink;
}

void
pseudo_stat_msg(PSEUDO_STATBUF *buf, const pseudo_msg_t *msg) {
	if (!msg || !buf)
		return;
	buf->st_uid = msg->uid;
	buf->st_gid = msg->gid;
	buf->st_mode = msg->mode;
	buf->st_rdev = msg->rdev;
}
