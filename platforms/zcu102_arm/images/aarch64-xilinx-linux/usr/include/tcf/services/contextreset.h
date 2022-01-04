/*******************************************************************************
 * Copyright (c) 2019.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 * The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 * http://www.eclipse.org/org/documents/edl-v10.php.
 * You may elect to redistribute this code under either of these licenses.
 *******************************************************************************/
#ifndef D_contextreset
#define D_contextreset

#include <tcf/config.h>
#include <tcf/framework/protocol.h>
#include <tcf/framework/context.h>

typedef struct ResetParameter ResetParameter;

struct ResetParameter {
    ResetParameter * next;
    const char * name;        /* Parameter name */
    const char * value;       /* Parameter value as JSON string */
};

typedef struct ResetParams {
    unsigned suspend;         /* If true, context should be suspended after reset. */
    ResetParameter * list;    /* List of additional parameters */
} ResetParams;

/* Reset callback. */
typedef int ContextReset(Context * /* ctx */, const ResetParams * /* params */);

/* Register reset callback function for a context */
extern void add_reset(Context * ctx, const char * type, const char * desc, ContextReset * reset);

/*
 * Set reset group of a context .
 * By default, a reset function is shared by members of CONTEXT_GROUP_CPU.
 */
extern void set_reset_group(Context * ctx, int group);

extern void ini_context_reset_service(Protocol * proto);

#endif /* D_contextreset */
