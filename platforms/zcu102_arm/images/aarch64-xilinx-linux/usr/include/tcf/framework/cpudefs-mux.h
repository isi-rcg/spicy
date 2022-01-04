/*******************************************************************************
 * Copyright (c) 2013 Wind River Systems, Inc. and others.
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

#include <tcf/config.h>

#if ENABLE_ContextMux
#include <tcf/framework/context.h>
#include <tcf/framework/context-dispatcher.h>

static RegisterDefinition * sys_get_reg_definitions(Context * ctx);
extern RegisterDefinition * sys_get_PC_definition(Context * ctx);
static RegisterDefinition * sys_get_reg_by_id(Context * ctx, unsigned id, RegisterIdScope * scope);
static int sys_read_reg_bytes(StackFrame * frame, RegisterDefinition * reg_def, unsigned offs,
        unsigned size, uint8_t * buf);
static int sys_write_reg_bytes(StackFrame * frame, RegisterDefinition * reg_def, unsigned offs,
        unsigned size, uint8_t * buf);
static uint8_t * sys_get_break_instruction(Context * ctx, size_t * size);
extern int sys_crawl_stack_frame(StackFrame * frame, StackFrame * down);

CpuDefsIf sys_cpudefs_if = {
                sys_get_reg_definitions,
                sys_get_PC_definition,
                sys_get_reg_by_id,
                sys_read_reg_bytes,
                sys_write_reg_bytes,
                sys_get_break_instruction,
#if ENABLE_StackCrawlMux
                sys_crawl_stack_frame,
#endif
                };

#define get_reg_definitions     sys_get_reg_definitions
#define get_reg_by_id           sys_get_reg_by_id
#define read_reg_bytes          sys_read_reg_bytes
#define write_reg_bytes         sys_write_reg_bytes
#define get_break_instruction   sys_get_break_instruction

#endif /* ENABLE_ContextMux */
