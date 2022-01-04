/*******************************************************************************
 * Copyright (c) 2012 Wind River Systems, Inc. and others.
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
 * This module contains definitions for enabling function call injection on the target.
 */

#ifndef D_funccall
#define D_funccall

#include <tcf/services/symbols.h>

#if ENABLE_Symbols && ENABLE_DebugContext

/*
 * Input and output parameters for get_function_call_location_expression()
 */
struct FunctionCallInfo {
    /* Inputs */
    RegisterIdScope scope;              /* Information about the target */
    Context * ctx;                      /* Execution context of the target */
    const Symbol * func;                /* Function declaration */
    const Symbol ** args;               /* Argument types given in the call */
    unsigned args_cnt;                  /* Number of arguments */

    /* Outputs */
    LocationExpressionCommand * cmds;   /* list of location expression commands */
    unsigned cmds_cnt;                  /* Number of location expression commands */

    RegisterDefinition ** saveregs;     /* List of registers to save before calling the function */
    unsigned saveregs_cnt;              /* Number of registers to save */

    RegisterDefinition * stak_pointer;  /* Stack pointer register definition */
    unsigned stack_alignment;           /* Stack pointer alignment */
    unsigned red_zone_size;             /* Size in bytes of area beyond the top of the call stack
                                         * that is considered to be reserved. This area is known as the red zone. */

    unsigned update_policy;             /* Cache update policy, see UPDATE_ON_* in symbols.h */
};

/*
 * Order of args array given to evaluate_location_expression() when
 * evaluating function calls.
 */
#define FUNCCALL_ARG_ADDR 0             /* Address of function */
#define FUNCCALL_ARG_RET  1             /* Return address */
#define FUNCCALL_ARG_ARGS 2             /* Function arguments */

/*
 * Get location expressions for calling functions on the target.
 *
 * The resulting location expression commands are intended to be used
 * to perform the function call, using evaluate_location_expression().
 * to build a LocationExpressionState.
 *
 * Memory allocated for the result information, e.g. cmds and
 * saveregs, should be allocated using tmp_alloc() and related
 * functions.  It is the clients reposibility to copy this information
 * to permanent storage if necessary.
 *
 * When calling the function, the client will save the registers enumerated by
 * saveregs before the call and restore the same registers after the
 * call. Before restoring the register it will retrieve the return
 * value. This allows the location expression commands to temporarily
 * store return information on the stack when needed.
 */

extern int get_function_call_location_expression(FunctionCallInfo * info);

#endif /* ENABLE_Symbols && ENABLE_DebugContext */

#endif /* D_funccall */
