/*******************************************************************************
 * Copyright (c) 2009, 2016 Wind River Systems, Inc. and others.
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
 * This module holds execution context memory maps.
 */
#ifndef D_memorymap
#define D_memorymap

#include <tcf/config.h>
#include <tcf/framework/context.h>
#include <tcf/framework/protocol.h>

#if !defined(ENABLE_MemoryMap)
#  define ENABLE_MemoryMap      ((ENABLE_DebugContext && ENABLE_ContextProxy) || SERVICE_MemoryMap)
#endif

#if ENABLE_MemoryMap

/*
 * Get memory maps for given context.
 * 'client_map' returns map entries that are created by the agent clients.
 * 'target_map' returns map entries that the agent has found on a target.
 * Return -1 and set errno if the context memory map cannot be retrieved.
 * Return 0 on success.
 */
extern int memory_map_get(Context * ctx, MemoryMap ** client_map, MemoryMap ** target_map);

/*
 * Override memory_map_get() function for given context.
 * This is used by OS awareness modules to amend memory map of a CPU core
 * using data from OS kernel introspection.
 */
typedef int MemoryMapOverrideCallBack(Context * ctx, MemoryMap ** client_map, MemoryMap ** target_map);
extern int memory_map_override(Context * ctx, MemoryMapOverrideCallBack * cb);
extern int memory_map_get_original(Context * ctx, MemoryMap ** client_map, MemoryMap ** target_map);

/*
 * Functions that are used by context implementation to notify memory map service about map changes.
 */
extern void memory_map_event_module_loaded(Context * ctx);
extern void memory_map_event_code_section_ummapped(Context * ctx, ContextAddress addr, ContextAddress size);
extern void memory_map_event_module_unloaded(Context * ctx);
extern void memory_map_event_mapping_changed(Context * ctx);

/*
 * Memory map listener.
 *
 * Note: calling 'module_loaded', 'code_section_ummapped' and 'module_unloaded' instead of 'mapping_changed'
 * is optional optimization. In some cases, it allows clients to handle memory map changes faster.
 * If a context cannot distinguish module loading/unloading from other memory map changes,
 * it will call 'mapping_changed' for any change.
 */
typedef struct MemoryMapEventListener {
    void (*module_loaded)(Context * ctx, void * client_data);
    void (*code_section_ummapped)(Context * ctx, ContextAddress addr, ContextAddress size, void * client_data);
    void (*module_unloaded)(Context * ctx, void * client_data);
    void (*mapping_changed)(Context * ctx, void * client_data);
} MemoryMapEventListener;

/*
 * Add memory map listener.
 */
extern void add_memory_map_event_listener(MemoryMapEventListener * listener, void * client_data);

extern void ini_memory_map_service(Protocol * proto, TCFBroadcastGroup * bcg);

extern void write_map_region(OutputStream * out, MemoryRegion * m);

#else

#define memory_map_event_module_loaded(ctx)
#define memory_map_event_code_section_ummapped(ctx, addr, size)
#define memory_map_event_module_unloaded(ctx)
#define memory_map_event_mapping_changed(ctx)

#endif /* ENABLE_MemoryMap */
#endif /* D_memorymap */
