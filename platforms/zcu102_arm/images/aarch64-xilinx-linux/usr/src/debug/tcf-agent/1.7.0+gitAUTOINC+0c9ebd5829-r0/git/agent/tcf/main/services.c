/*******************************************************************************
 * Copyright (c) 2007-2019 Wind River Systems, Inc. and others.
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
 * Services initialization code.
 */

#include <tcf/config.h>

#include <tcf/framework/proxy.h>
#include <tcf/framework/plugins.h>
#include <tcf/framework/context-dispatcher.h>
#include <tcf/services/discovery.h>
#include <tcf/services/runctrl.h>
#include <tcf/services/breakpoints.h>
#include <tcf/services/memoryservice.h>
#include <tcf/services/memorymap.h>
#include <tcf/services/contextquery.h>
#include <tcf/services/contextreset.h>
#include <tcf/services/registers.h>
#include <tcf/services/stacktrace.h>
#include <tcf/services/symbols.h>
#include <tcf/services/linenumbers.h>
#include <tcf/services/processes.h>
#include <tcf/services/filesystem.h>
#include <tcf/services/sysmon.h>
#include <tcf/services/diagnostics.h>
#include <tcf/services/expressions.h>
#include <tcf/services/streamsservice.h>
#include <tcf/services/pathmap.h>
#include <tcf/services/tcf_elf.h>
#include <tcf/services/terminals.h>
#include <tcf/services/dprintf.h>
#include <tcf/services/disassembly.h>
#include <tcf/services/profiler.h>
#include <tcf/services/profiler_sst.h>
#include <tcf/services/portforward_proxy.h>
#include <tcf/services/portforward_service.h>
#include <tcf/main/services.h>

#include <tcf/main/services-ext.h>

void ini_services(Protocol * proto, TCFBroadcastGroup * bcg) {
#if SERVICE_Locator
    ini_locator_service(proto, bcg);
#endif
#if SERVICE_ContextQuery
    ini_context_query_service(proto);
#endif
#if SERVICE_ContextReset
    ini_context_reset_service(proto);
#endif
#if SERVICE_RunControl
    ini_run_ctrl_service(proto, bcg);
#endif
#if SERVICE_Breakpoints
    ini_breakpoints_service(proto, bcg);
#endif
#if SERVICE_Memory
    ini_memory_service(proto, bcg);
#endif
#if SERVICE_MemoryMap
    ini_memory_map_service(proto, bcg);
#endif
#if SERVICE_Registers
    ini_registers_service(proto, bcg);
#endif
#if SERVICE_StackTrace
    ini_stack_trace_service(proto, bcg);
#endif
#if SERVICE_Symbols
    ini_symbols_service(proto);
#elif ENABLE_SymbolsProxy
    ini_symbols_lib();
#endif
#if ENABLE_SymbolsProxy
    protocol_get_service(proto, "SymbolsProxyV1");
    protocol_get_service(proto, "SymbolsProxyV2");
#endif
#if SERVICE_LineNumbers
    ini_line_numbers_service(proto);
#elif ENABLE_LineNumbersProxy
    ini_line_numbers_lib();
#endif
#if SERVICE_Processes
    ini_processes_service(proto);
#endif
#if SERVICE_FileSystem
    ini_file_system_service(proto);
#endif
#if SERVICE_SysMonitor
    ini_sys_mon_service(proto);
#endif
#if SERVICE_Expressions
    ini_expressions_service(proto);
#endif
#if SERVICE_Streams
    ini_streams_service(proto);
#endif
#if SERVICE_PathMap
    ini_path_map_service(proto, bcg);
#endif
#if SERVICE_Terminals
    ini_terminals_service(proto);
#endif
#if SERVICE_DPrintf
    ini_dprintf_service(proto);
#endif
#if SERVICE_Disassembly
    ini_disassembly_service(proto);
#endif
#if SERVICE_Profiler
    ini_profiler_service(proto);
#endif
#if ENABLE_DebugContext
    ini_contexts();
#endif
#if ENABLE_ContextMux
    ini_context_dispatcher();
#endif
#if ENABLE_ELF
    ini_elf();
#endif
#if ENABLE_ProfilerSST
    ini_profiler_sst();
#endif
#if ENABLE_Plugins
    plugins_load(proto, bcg);
#endif
#if SERVICE_PortForward
    ini_port_forward_service(proto, bcg);
#endif
#if SERVICE_PortServer
    ini_port_server_service(NULL, proto, bcg);
#endif

    ini_diagnostics_service(proto);
    ini_ext_services(proto, bcg);
}
