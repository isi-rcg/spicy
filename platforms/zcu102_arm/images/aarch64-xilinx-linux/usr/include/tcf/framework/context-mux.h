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

#include <tcf/config.h>

#include <tcf/framework/context.h>
#include <tcf/framework/context-dispatcher.h>
#include <tcf/framework/context-mux-ext.h>

#if ENABLE_ContextProxy
#include <tcf/framework/cpudefs-mux.h>
#include <tcf/framework/cpudefs-mdep-mux.h>
#endif

static const char * sys_context_suspend_reason(Context * ctx);
static int sys_context_stop(Context * ctx);
static int sys_context_continue(Context * ctx);
static int sys_context_resume(Context * ctx, int mode, ContextAddress range_start, ContextAddress range_end);
static int sys_context_can_resume(Context * ctx, int mode);
static int sys_context_single_step(Context * ctx);
static int sys_context_get_canonical_addr(Context * ctx, ContextAddress addr,
                Context ** canonical_ctx, ContextAddress * canonical_addr,
                ContextAddress * block_addr, ContextAddress * block_size);
static int sys_context_get_supported_bp_access_types(Context * ctx);
static int sys_context_plant_breakpoint(ContextBreakpoint * bp);
static int sys_context_unplant_breakpoint(ContextBreakpoint * bp);
static int sys_context_has_state(Context * ctx);
static int sys_context_write_mem(Context * ctx, ContextAddress address, void * buf, size_t size);
static int sys_context_read_mem(Context * ctx, ContextAddress address, void * buf, size_t size);
static unsigned sys_context_word_size(Context * ctx);
static int sys_context_read_reg(Context * ctx, RegisterDefinition * def,
                unsigned offs, unsigned size, void * buf);
static int sys_context_write_reg(Context * ctx, RegisterDefinition * def,
                unsigned offs, unsigned size, void * buf);
static Context * sys_context_get_group(Context * ctx, int group);
static int sys_context_get_memory_map(Context * ctx, MemoryMap * map);
#if ENABLE_ContextStateProperties
static int sys_context_get_state_properties(Context * ctx, const char *** names, const char *** values, int * cnt);
#endif
#if ENABLE_ExtendedMemoryErrorReports
static int sys_context_get_mem_error_info(MemoryErrorInfo * info);
#endif
#if ENABLE_ExtendedBreakpointStatus
static int sys_context_get_breakpoint_status(ContextBreakpoint * bp, const char *** names, const char *** values, int * cnt);
#endif
#if ENABLE_ContextBreakpointCapabilities
static int sys_context_get_breakpoint_capabilities(Context * ctx, const char *** names, const char *** values, int * cnt);
#endif
#if ENABLE_ContextMemoryProperties
static int sys_context_get_memory_properties(Context * ctx, const char *** names, const char *** values, int * cnt);
#endif
#if ENABLE_ContextExtraProperties
static int sys_context_get_extra_properties(Context * ctx, const char *** names, const char *** values, int * cnt);
#endif
#if ENABLE_ContextISA
static int sys_context_get_isa(Context * ctx, ContextAddress addr, ContextISA * isa);
#endif
#if ENABLE_MemoryAccessModes
static int sys_context_write_mem_ext(Context * ctx, MemoryAccessMode * mode, ContextAddress address, void * buf, size_t size);
static int sys_context_read_mem_ext(Context * ctx, MemoryAccessMode * mode, ContextAddress address, void * buf, size_t size);
#endif

ContextIf sys_ctx_if = {
        sys_context_has_state,
        sys_context_suspend_reason,
        sys_context_stop,
        sys_context_continue,
        sys_context_resume,
        sys_context_can_resume,
        sys_context_single_step,
        sys_context_write_mem,
        sys_context_read_mem,
        sys_context_word_size,
        sys_context_read_reg,
        sys_context_write_reg,
        sys_context_get_group,
        sys_context_get_canonical_addr,
        sys_context_get_memory_map,
        sys_context_get_supported_bp_access_types,
        sys_context_plant_breakpoint,
        sys_context_unplant_breakpoint,
#if ENABLE_ContextStateProperties
        sys_context_get_state_properties,
#endif
#if ENABLE_ExtendedMemoryErrorReports
        sys_context_get_mem_error_info,
#endif
#if ENABLE_ExtendedBreakpointStatus
        sys_context_get_breakpoint_status,
#endif
#if ENABLE_ContextMemoryProperties
        sys_context_get_memory_properties,
#endif
#if ENABLE_ContextExtraProperties
        sys_context_get_extra_properties,
#endif
#if ENABLE_ContextISA
        sys_context_get_isa,
#endif
#if ENABLE_MemoryAccessModes
        sys_context_write_mem_ext,
        sys_context_read_mem_ext,
#endif
#if ENABLE_ContextBreakpointCapabilities
        sys_context_get_breakpoint_capabilities,
#endif
        { 0 },
        { 0 }
};

static void sys_send_context_created_event(Context * ctx) {
    static int initialized = 0;
    if (!initialized) {
        sys_ctx_if.cpudefs_if = sys_cpudefs_if;
        sys_ctx_if.ctxext_if = sys_ctxext_if;
        initialized = 1;
    }
    context_set_interface(ctx, &sys_ctx_if);
    send_context_created_event(ctx);
}

#define context_suspend_reason          sys_context_suspend_reason
#define context_has_state               sys_context_has_state
#define context_stop                    sys_context_stop
#define context_continue                sys_context_continue
#define context_resume                  sys_context_resume
#define context_can_resume              sys_context_can_resume
#define context_single_step             sys_context_single_step
#define context_write_mem               sys_context_write_mem
#define context_read_mem                sys_context_read_mem
#define context_access_mem              sys_context_access_mem
#define context_word_size               sys_context_word_size
#define context_read_reg                sys_context_read_reg
#define context_write_reg               sys_context_write_reg
#define context_get_group               sys_context_get_group
#define context_get_canonical_addr      sys_context_get_canonical_addr
#define context_get_memory_map          sys_context_get_memory_map
#define send_context_created_event      sys_send_context_created_event
#define context_get_supported_bp_access_types sys_context_get_supported_bp_access_types
#define context_plant_breakpoint        sys_context_plant_breakpoint
#define context_unplant_breakpoint      sys_context_unplant_breakpoint
#if ENABLE_ContextStateProperties
#define context_get_state_properties    sys_context_get_state_properties
#endif
#if ENABLE_ExtendedMemoryErrorReports
#define context_get_mem_error_info      sys_context_get_mem_error_info
#endif
#if ENABLE_ExtendedBreakpointStatus
#define context_get_breakpoint_status   sys_context_get_breakpoint_status
#endif
#if ENABLE_ContextBreakpointCapabilities
#define context_get_breakpoint_capabilities sys_context_get_breakpoint_capabilities
#endif
#if ENABLE_ContextMemoryProperties
#define context_get_memory_properties   sys_context_get_memory_properties
#endif
#if ENABLE_ContextExtraProperties
#define context_get_extra_properties    sys_context_get_extra_properties
#endif
#if ENABLE_ContextISA
#define context_get_isa                 sys_context_get_isa
#endif
#if ENABLE_MemoryAccessModes
#define context_write_mem_ext           sys_context_write_mem_ext
#define context_read_mem_ext            sys_context_read_mem_ext
#endif
