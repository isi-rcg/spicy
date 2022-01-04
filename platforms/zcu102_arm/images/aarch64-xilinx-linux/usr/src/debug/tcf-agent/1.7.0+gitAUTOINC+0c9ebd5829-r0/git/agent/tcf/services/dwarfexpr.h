/*******************************************************************************
 * Copyright (c) 2008, 2015 Wind River Systems, Inc. and others.
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
 * This module implements DWARF expressions evaluation.
 *
 * Functions in this module use exceptions to report errors, see exceptions.h
 */
#ifndef D_dwarfexpr
#define D_dwarfexpr

#include <tcf/config.h>

#if ENABLE_ELF && ENABLE_DebugContext

#include <tcf/services/dwarfcache.h>

/* PropertyValue format for expresson evaluation results */
#define FORM_EXPR_VALUE 0x00ff

typedef struct DWARFExpressionInfo {
    U8_T code_addr; /* Run-time address */
    U8_T code_size;
    ObjectInfo * object;
    ELF_Section * section;
    U1_T * expr_addr;
    size_t expr_size;
    U2_T attr;
    U2_T form;
    struct DWARFExpressionInfo * next;
} DWARFExpressionInfo;

extern void dwarf_get_expression_list(PropertyValue * Value, DWARFExpressionInfo ** List);
extern void dwarf_evaluate_expression(PropertyValue * value, uint64_t * args, unsigned args_cnt);

#endif /* ENABLE_ELF && ENABLE_DebugContext */

#endif /* D_dwarfexpr */
