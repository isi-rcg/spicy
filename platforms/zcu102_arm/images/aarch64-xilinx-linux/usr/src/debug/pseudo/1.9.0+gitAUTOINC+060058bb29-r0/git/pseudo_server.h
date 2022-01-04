/*
 * pseudo_server.h, pseudo server declarations and definitions
 *
 * Copyright (c) 2008-2009 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */
extern int pseudo_server_start(int);
extern int pseudo_server_response(pseudo_msg_t *msg, const char *program, const char *tag, char **response_path, size_t *response_len);
extern int pseudo_server_timeout;
extern int opt_l;
