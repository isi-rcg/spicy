/*******************************************************************************
 * Copyright (c) 2007, 2013 Wind River Systems, Inc. and others.
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
 * Diagnostics service.
 * This service is used for framework and agents testing.
 */

#ifndef D_diagnostics
#define D_diagnostics

#include <tcf/framework/protocol.h>
#include <tcf/framework/context.h>

#if ENABLE_RCBP_TEST
/*
 * Check if a context is a test started by Diagnostics.runTest command.
 */
extern int is_test_process(Context * ctx);

/*
 * Tell Diagnostics service that a context finished running a test.
 * Note: context implementation calls this function only if the context
 * does not exit after running a dignostic test.
 */
extern void test_process_done(Context * ctx);

#endif /* ENABLE_RCBP_TEST */

extern void ini_diagnostics_service(Protocol *);

#endif /* D_diagnostics */
