/*******************************************************************************
 * Copyright (c) 2013, 2015 Wind River Systems, Inc. and others.
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

#ifndef D_context_dispatcher
#define D_context_dispatcher

#include <tcf/config.h>
#if ENABLE_ContextMux
#include <tcf/framework/context.h>
#include <tcf/framework/cpudefs.h>
#include <tcf/framework/context-dispatcher-ext.h>

typedef struct CpuDefsIf {
    RegisterDefinition * (*get_reg_definitions)(Context * ctx);
    RegisterDefinition * (*get_PC_definition)(Context * ctx);
    RegisterDefinition * (*get_reg_by_id)(Context * ctx, unsigned id, RegisterIdScope * scope);
    int (*read_reg_bytes)(StackFrame * frame, RegisterDefinition * reg_def, unsigned offs, unsigned size, uint8_t * buf);
    int (*write_reg_bytes)(StackFrame * frame, RegisterDefinition * reg_def, unsigned offs, unsigned size, uint8_t * buf);
    uint8_t * (*get_break_instruction)(Context * ctx, size_t * size);
#if ENABLE_StackCrawlMux
    int (*crawl_stack_frame)(StackFrame * frame, StackFrame * down);
#endif
#if ENABLE_StackRegisterLocations
    int (*write_reg_location)(StackFrame * frame, RegisterDefinition * reg_def, LocationExpressionCommand * cmds, unsigned cmds_cnt);
#endif
} CpuDefsIf;

typedef struct ContextIf {
    int (*context_has_state)(Context * ctx);
    const char * (*context_suspend_reason)(Context * ctx);
    int (*context_stop)(Context * ctx);
    int (*context_continue)(Context * ctx);
    int (*context_resume)(Context * ctx, int mode, ContextAddress range_start, ContextAddress range_end);
    int (*context_can_resume)(Context * ctx, int mode);
    int (*context_single_step)(Context * ctx);
    int (*context_write_mem)(Context * ctx, ContextAddress address, void * buf, size_t size);
    int (*context_read_mem)(Context * ctx, ContextAddress address, void * buf, size_t size);
    unsigned (*context_word_size)(Context * ctx);
    int (*context_read_reg)(Context * ctx, RegisterDefinition * def,
            unsigned offs, unsigned size, void * buf);
    int (*context_write_reg)(Context * ctx, RegisterDefinition * def,
            unsigned offs, unsigned size, void * buf);
    Context * (*context_get_group)(Context * ctx, int group);
    int (*context_get_canonical_addr)(Context * ctx, ContextAddress addr,
            Context ** canonical_ctx, ContextAddress * canonical_addr,
            ContextAddress * block_addr, ContextAddress * block_size);
    int (*context_get_memory_map)(Context * ctx, MemoryMap * map);
    int (*context_get_supported_bp_access_types) (Context * ctx);
    int (*context_plant_breakpoint) (ContextBreakpoint * bp);
    int (*context_unplant_breakpoint) (ContextBreakpoint * bp);
#if ENABLE_ContextStateProperties
    int (*context_get_state_properties)(Context * ctx, const char *** names, const char *** values, int * cnt);
#endif
#if ENABLE_ExtendedMemoryErrorReports
    int (*context_get_mem_error_info) (MemoryErrorInfo * info);
#endif
#if ENABLE_ExtendedBreakpointStatus
    int (*context_get_breakpoint_status) (ContextBreakpoint * bp, const char *** names, const char *** values, int * cnt);
#endif
#if ENABLE_ContextMemoryProperties
    int (*context_get_memory_properties) (Context * ctx, const char *** names, const char *** values, int * cnt);
#endif
#if ENABLE_ContextExtraProperties
    int (*context_get_extra_properties) (Context * ctx, const char *** names, const char *** values, int * cnt);
#endif
#if ENABLE_ContextISA
    int (*context_get_isa)(Context * ctx, ContextAddress addr, ContextISA * isa);
#endif
#if ENABLE_MemoryAccessModes
    int (*context_write_mem_ext)(Context * ctx, MemoryAccessMode * mode, ContextAddress address, void * buf, size_t size);
    int (*context_read_mem_ext)(Context * ctx, MemoryAccessMode * mode, ContextAddress address, void * buf, size_t size);
#endif
#if ENABLE_ContextBreakpointCapabilities
    int (*context_get_breakpoint_capabilities)(Context * ctx, const char *** names, const char *** values, int * cnt);
#endif
    CpuDefsIf cpudefs_if;
    ContextExtIf ctxext_if;
} ContextIf;

extern ContextIf sys_ctx_if;
extern CpuDefsIf sys_cpudefs_if;

/* Set the context interface for specified context.
 * Return -1 and set errno if the context interface cannot be set.
 * Return 0 on success.
 */
extern int context_set_interface(Context * ctx, ContextIf * ctx_iface);


/* Return the current context interface for the specified context.
 * Returns a pointer on context current interface table or NULL if there is non.e
 */
extern ContextIf * context_get_interface(Context * ctx);

#if ENABLE_ExtendedMemoryErrorReports
/* Set the context dispatcher context memory error info.
 * Deprecated: clients should not modify dispatcher internal data.
 */
extern int set_dispatcher_mem_error_info(MemoryErrorInfo * info);
#endif

extern void ini_context_dispatcher(void );

#endif /* ENABLE_ContextMux */
#endif /* D_context_dispatcher */
