/*******************************************************************************
 * Copyright (c) 2007, 2011 Wind River Systems, Inc. and others.
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

#ifndef D_test
#define D_test

#include <tcf/config.h>

#if ENABLE_RCBP_TEST

#include <tcf/framework/context.h>
#include <tcf/services/symbols.h>

extern void test_proc(void);
extern int run_test_process(ContextAttachCallBack * done, void * data);
extern int find_test_symbol(Context * ctx, const char * name, void ** addr, int * sym_class);

#else /* ENABLE_RCBP_TEST */

#include <tcf/framework/errors.h>

#define find_test_symbol(ctx, name, addr, sym_class) (errno = ERR_SYM_NOT_FOUND, -1)

#endif /* ENABLE_RCBP_TEST */

#endif /* D_test */
