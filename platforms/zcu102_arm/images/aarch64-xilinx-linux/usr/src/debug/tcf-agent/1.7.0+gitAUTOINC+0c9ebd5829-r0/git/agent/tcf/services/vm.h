/*******************************************************************************
 * Copyright (c) 2011, 2012 Wind River Systems, Inc. and others.
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
 * A virtual machine that executes DWARF expressions.
 */

#ifndef D_vm
#define D_vm

#include <tcf/config.h>

#if ENABLE_DebugContext

#include <tcf/framework/cpudefs.h>

extern int evaluate_vm_expression(LocationExpressionState * state);

#endif /* ENABLE_DebugContext */

#endif /* D_vm */
