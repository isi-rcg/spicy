/*******************************************************************************
 * Copyright (c) 2011, 2015 Wind River Systems, Inc. and others.
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
 * Transformation of DWARF expressions to a portable form.
 *
 * Functions in this module use exceptions to report errors, see exceptions.h
 */
#ifndef D_dwarfecomp
#define D_dwarfecomp

#include <tcf/config.h>

#if ENABLE_ELF && ENABLE_DebugContext

#include <tcf/services/dwarfexpr.h>

/* Check link-time address 'addr' belongs to an object address range(s) */
extern int dwarf_check_in_range(ObjectInfo * obj, ELF_Section * sec, U8_T addr);

extern void dwarf_transform_expression(Context * ctx, int frame, DWARFExpressionInfo * info);

#endif /* ENABLE_ELF && ENABLE_DebugContext */

#endif /* D_dwarfecomp */
