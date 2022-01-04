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
 * This module implements access to ELF dynamic loader data.
 */

#ifndef D_elf_loader
#define D_elf_loader

#include <tcf/config.h>

#if ENABLE_ELF && ENABLE_DebugContext

#include <tcf/framework/context.h>
#include <tcf/services/tcf_elf.h>

/*
 * Return run-time address of the debug structure that is normally pointed by DT_DEBUG entry in ".dynamic" section.
 * "file" is assigned a file that contains DT_DEBUG entry.
 * Return 0 if the structure could not be found.
 */
extern ContextAddress elf_get_debug_structure_address(Context * ctx, ELF_File ** file_ptr);

extern ContextAddress get_tls_address(Context * ctx, ELF_File * file);

#endif /* ENABLE_ELF && ENABLE_DebugContext */
#endif /* D_elf_loader */
