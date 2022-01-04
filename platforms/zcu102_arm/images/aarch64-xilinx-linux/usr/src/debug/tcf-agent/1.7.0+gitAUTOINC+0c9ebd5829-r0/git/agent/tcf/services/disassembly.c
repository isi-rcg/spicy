/*******************************************************************************
 * Copyright (c) 2013-2020 Xilinx, Inc. and others.
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
 *     Xilinx - initial API and implementation
 *******************************************************************************/

#include <tcf/config.h>

#if SERVICE_Disassembly

#include <stdio.h>
#include <assert.h>
#include <tcf/framework/json.h>
#include <tcf/framework/context.h>
#include <tcf/framework/exceptions.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/cache.h>
#include <tcf/framework/trace.h>
#include <tcf/services/runctrl.h>
#include <tcf/services/symbols.h>
#include <tcf/services/linenumbers.h>
#include <tcf/services/memorymap.h>
#include <tcf/services/disassembly.h>

#define MAX_INSTRUCTION_SIZE 8
#define DEFAULT_ALIGMENT     16

typedef struct {
    const char * isa;
    Disassembler * disassembler;
} DisassemblerInfo;

typedef struct {
    DisassemblerInfo * disassemblers;
    unsigned disassemblers_cnt;
    unsigned disassemblers_max;
} ContextExtensionDS;

typedef struct {
    char token[256];
    char id[256];
    ContextAddress addr;
    ContextAddress size;
    char * isa;
    int simplified;
    int pseudo_instr;
    int opcode_value;
} DisassembleCmdArgs;

typedef struct {
    char token[256];
    char id[256];
} GetCapabilitiesCmdArgs;

static const char * DISASSEMBLY = "Disassembly";
static size_t context_extension_offset = 0;

#define EXT(ctx) (ctx ? ((ContextExtensionDS *)((char *)(ctx) + context_extension_offset)) : NULL)

static DisassemblerInfo * find_disassembler_info(Context * ctx, const char * isa) {
    if (isa != NULL) {
        unsigned i = 0;
        ContextExtensionDS * ext = EXT(ctx);
        while (i < ext->disassemblers_cnt) {
            if (strcmp(ext->disassemblers[i].isa, isa) == 0)
                return ext->disassemblers + i;
            i++;
        }
    }
    return NULL;
}

Disassembler * find_disassembler(Context * ctx, const char * isa) {
    DisassemblerInfo * i = find_disassembler_info(ctx, isa);
    return i ? i->disassembler : NULL;
}

void add_disassembler(Context * ctx, const char * isa, Disassembler disassembler) {
    DisassemblerInfo * i = NULL;
    ContextExtensionDS * ext = EXT(ctx);
    assert(ctx == context_get_group(ctx, CONTEXT_GROUP_CPU));
    if ((i = find_disassembler_info(ctx, isa)) == NULL) {
        if (ext->disassemblers_cnt >= ext->disassemblers_max) {
            ext->disassemblers_max += 8;
            ext->disassemblers = (DisassemblerInfo *)loc_realloc(ext->disassemblers,
                sizeof(DisassemblerInfo) * ext->disassemblers_max);
        }
        i = ext->disassemblers + ext->disassemblers_cnt++;
        i->isa = loc_strdup(isa);
    }
    i->disassembler = disassembler;
}

static void command_get_capabilities_cache_client(void * x) {
    int error = 0;
    Context * ctx = NULL;
    ContextExtensionDS * ext = NULL;
    GetCapabilitiesCmdArgs * args = (GetCapabilitiesCmdArgs *)x;
    Channel * c = cache_channel();

    ctx = id2ctx(args->id);
    if (ctx == NULL) error = ERR_INV_CONTEXT;
    else if (ctx->exited) error = ERR_ALREADY_EXITED;
    else ext = EXT(context_get_group(ctx, CONTEXT_GROUP_CPU));

    cache_exit();

    if (!is_channel_closed(c)) {
        OutputStream * out = &c->out;
        write_stringz(out, "R");
        write_stringz(out, args->token);
        write_errno(out, error);
        write_stream(out, '[');
        if (ext != NULL) {
            unsigned i;
            for (i = 0; i < ext->disassemblers_cnt; i++) {
                if (i > 0) write_stream(out, ',');
                write_stream(out, '{');
                json_write_string(out, "ISA");
                write_stream(out, ':');
                json_write_string(out, ext->disassemblers[i].isa);
                write_stream(out, '}');
            }
        }
        write_stream(out, ']');
        write_stream(out, 0);
        write_stream(out, MARKER_EOM);
    }
}

static void command_get_capabilities(char * token, Channel * c) {
    GetCapabilitiesCmdArgs args;

    json_read_string(&c->inp, args.id, sizeof(args.id));
    if (read_stream(&c->inp) != 0) exception(ERR_JSON_SYNTAX);
    if (read_stream(&c->inp) != MARKER_EOM) exception(ERR_JSON_SYNTAX);

    strlcpy(args.token, token, sizeof(args.token));
    cache_enter(command_get_capabilities_cache_client, c, &args, sizeof(args));
}

int get_disassembler_isa(Context * ctx, ContextAddress addr, ContextISA * isa) {
    if (context_get_isa(ctx, addr, isa) < 0) {
        memset(isa, 0, sizeof(ContextISA));
        return -1;
    }
#if ENABLE_MemoryMap
    if (isa->size == 0) {
        unsigned i, j;
        MemoryMap * client_map = NULL;
        MemoryMap * target_map = NULL;
        Context * mem = context_get_group(ctx, CONTEXT_GROUP_PROCESS);
        if (memory_map_get(mem, &client_map, &target_map) == 0) {
            isa->addr = addr;
            isa->size = ~addr + 1;
            for (j = 0; j < 2; j++) {
                MemoryMap * map = j ? target_map : client_map;
                for (i = 0; i < map->region_cnt; i++) {
                    MemoryRegion * r = map->regions + i;
                    ContextAddress x = r->addr;
                    ContextAddress y = r->addr + r->size;
                    if (x > addr && (addr + isa->size == 0 || x < addr + isa->size)) isa->size = x - addr;
                    if (y > addr && (addr + isa->size == 0 || y < addr + isa->size)) isa->size = y - addr;
                }
            }
        }
    }
#endif
    return 0;
}

static int disassemble_block(Context * ctx, OutputStream * out, uint8_t * mem_buf,
                              ContextAddress buf_addr, ContextAddress buf_size,
                              ContextAddress mem_size, ContextISA * isa,
                              DisassembleCmdArgs * args) {
    ContextAddress offs = 0;
    Disassembler * disassembler = NULL;
    Context * cpu = context_get_group(ctx, CONTEXT_GROUP_CPU);
    DisassemblerParams params;
    int disassembler_ok = 0;

    memset(&params, 0, sizeof(DisassemblerParams));

    params.ctx = ctx;
    params.big_endian = ctx->big_endian;
    params.pseudo_instr = args->pseudo_instr;
    params.simplified = args->simplified;
    if (args->isa) {
        isa->isa = args->isa;
        isa->addr = args->addr;
        isa->size = args->size;
    }

    write_stream(out, '[');
    while (offs < buf_size && offs < mem_size) {
        ContextAddress addr = buf_addr + offs;
        ContextAddress size = mem_size - offs;
        DisassemblyResult * dr = NULL;
        if (args->isa == NULL && (addr < isa->addr ||
                (isa->addr + isa->size >= isa->addr && addr >= isa->addr + isa->size))) {
            if (get_disassembler_isa(ctx, addr, isa) < 0) return -1;
            disassembler_ok = 0;
        }
        if (!disassembler_ok) {
            if (isa->isa != NULL) disassembler = find_disassembler(cpu, isa->isa);
            else disassembler = find_disassembler(cpu, isa->def);
            disassembler_ok = 1;
        }
        if (disassembler) dr = disassembler(mem_buf + (size_t)offs, addr, size, &params);
        if (dr == NULL) {
            static char buf[32];
            static DisassemblyResult dd;
            memset(&dd, 0, sizeof(dd));
            if (isa->alignment >= 4 && (addr & 0x3) == 0 && offs <= mem_size + 4) {
                unsigned i;
                unsigned v = 0;
                for (i = 0; i < 4; i++) v |= (unsigned)mem_buf[offs + i] << (i * 8);
                snprintf(buf, sizeof(buf), ".word 0x%08x", v);
                dd.size = 4;
            }
            else if (isa->alignment >= 2 && (addr & 0x1) == 0 && offs <= mem_size + 2) {
                unsigned i;
                uint16_t v = 0;
                for (i = 0; i < 2; i++) v |= (uint16_t)mem_buf[offs + i] << (i * 8);
                snprintf(buf, sizeof(buf), ".half 0x%04x", v);
                dd.size = 2;
            }
            else {
                snprintf(buf, sizeof(buf), ".byte 0x%02x", mem_buf[offs]);
                dd.size = 1;
            }
            dd.text = buf;
            dr = &dd;
        }
        assert(dr->size > 0);
        if (offs > 0) write_stream(out, ',');
        write_stream(out, '{');
        json_write_string(out, "Address");
        write_stream(out, ':');
        json_write_uint64(out, addr);
        write_stream(out, ',');
        json_write_string(out, "Size");
        write_stream(out, ':');
        json_write_uint64(out, dr->size);
        write_stream(out, ',');
        json_write_string(out, "Instruction");
        write_stream(out, ':');
        write_stream(out, '[');
        write_stream(out, '{');
        json_write_string(out, "Type");
        write_stream(out, ':');
        json_write_string(out, "String");
        write_stream(out, ',');
        json_write_string(out, "Text");
        write_stream(out, ':');
        json_write_string(out, dr->text);
        write_stream(out, '}');
        write_stream(out, ']');
        if (args->opcode_value) {
            write_stream(out, ',');
            json_write_string(out, "OpcodeValue");
            write_stream(out, ':');
            json_write_binary(out, mem_buf + (size_t)offs, (size_t)dr->size);
        }
        write_stream(out, '}');
        offs += dr->size;
    }
    write_stream(out, ']');
    loc_free(params.state);
    return 0;
}

#if ENABLE_LineNumbers
static void address_to_line_cb(CodeArea * area, void * args) {
    CodeArea ** p = (CodeArea **)args;
    if (*p == NULL || (*p)->start_address < area->start_address) {
        *p = (CodeArea *)tmp_alloc(sizeof(CodeArea));
        **p = *area;
    }
}
#endif

static void disassemble_cache_client(void * x) {
    DisassembleCmdArgs * args = (DisassembleCmdArgs *)x;

    int error = 0;
    Context * ctx = NULL;
    uint8_t * mem_buf = NULL;
    ContextAddress buf_addr = 0;
    ContextAddress buf_size = 0;
    size_t mem_size = 0;
    ByteArrayOutputStream buf;
    OutputStream * buf_out = create_byte_array_output_stream(&buf);
    Channel * c = cache_channel();
    char * data = NULL;
    size_t size = 0;
    ContextISA isa;

    memset(&isa, 0, sizeof(isa));

    ctx = id2ctx(args->id);
    if (ctx == NULL) error = ERR_INV_CONTEXT;
    else if (ctx->exited) error = ERR_ALREADY_EXITED;

    trace(LOG_DISASM, "%s: ctx %s address 0x%16" PRIx64 " size %" PRId64 " ISA %s",
          DISASSEMBLY, ctx->id, (uint64_t)args->addr, (uint64_t)args->size,
          args->isa == NULL ? "(null)" : args->isa);

    if (!error) {
        Context * mem = context_get_group(ctx, CONTEXT_GROUP_PROCESS);
        if ((mem->mem_access & MEM_ACCESS_RD_STOP) != 0) {
            check_all_stopped(ctx);
        }
        if ((mem->mem_access & MEM_ACCESS_RD_RUNNING) == 0) {
            if (!is_all_stopped(ctx)) error = set_errno(errno, "Cannot read memory if not stopped");
        }
    }

    if (!error) {
        ContextAddress sym_addr = 0;
        ContextAddress sym_size = 0;
        int sym_addr_ok = 0;
        int sym_size_ok = 0;
#if ENABLE_Symbols
        {
            Symbol * sym = NULL;
            if (find_symbol_by_addr(ctx, STACK_NO_FRAME, args->addr, &sym) == 0) {
                if (get_symbol_address(sym, &sym_addr) == 0) sym_addr_ok = 1;
                if (get_symbol_size(sym, &sym_size) == 0) sym_size_ok = 1;
            }
            if (sym_addr_ok && sym_addr <= args->addr && args->addr - sym_addr >= 0x1000) {
                sym_addr_ok = 0;
                sym_size_ok = 0;
            }
        }
#endif
#if ENABLE_LineNumbers
        if (!sym_addr_ok || !sym_size_ok) {
            CodeArea * area = NULL;
            address_to_line(ctx, args->addr, args->addr + 1, address_to_line_cb, &area);
            if (area != NULL) {
                sym_addr = area->start_address;
                sym_size = area->end_address - area->start_address;
                sym_addr_ok = 1;
                sym_size_ok = 1;
            }
        }
#endif
        if (sym_addr_ok && sym_addr <= args->addr) {
            if (get_disassembler_isa(ctx, sym_addr, &isa) < 0) {
                error = errno;
            }
            else {
                buf_addr = sym_addr;
                buf_size = args->addr + args->size - buf_addr;
                if (isa.max_instruction_size > 0) {
                    mem_size = (size_t)(buf_size + isa.max_instruction_size);
                }
                else {
                    mem_size = (size_t)(buf_size + MAX_INSTRUCTION_SIZE);
                }
            }
        }
        else {
            /* Use default address alignment */
            if (get_disassembler_isa(ctx, args->addr, &isa) < 0) {
                error = errno;
            }
            else {
                if (isa.alignment > 0) {
                    buf_addr = args->addr & ~(ContextAddress)(isa.alignment - 1);
                }
                else {
                    buf_addr = args->addr & ~(ContextAddress)(DEFAULT_ALIGMENT - 1);
                }
                buf_size = args->addr + args->size - buf_addr;
                if (isa.max_instruction_size > 0) {
                    mem_size = (size_t)(buf_size + isa.max_instruction_size);
                }
                else {
                    mem_size = (size_t)(buf_size + MAX_INSTRUCTION_SIZE);
                }
            }
        }

        while (!error) {
            if (sym_addr_ok && sym_size_ok &&
                    sym_addr <= buf_addr && sym_addr + sym_size > buf_addr &&
                    sym_addr + sym_size <= buf_addr + buf_size) {
                buf_size = sym_addr + sym_size - buf_addr;
                mem_size = (size_t)buf_size;
            }
            mem_buf = (uint8_t *)tmp_alloc(mem_size);
            if (context_read_mem(ctx, buf_addr, mem_buf, mem_size) == 0) break;
            error = errno;
#if ENABLE_ExtendedMemoryErrorReports
            {
                MemoryErrorInfo info;
                if (context_get_mem_error_info(&info) == 0) {
                    ContextAddress addr_valid = buf_addr + info.size_valid;
                    ContextAddress addr_error = addr_valid + info.size_error;
                    if (addr_valid < buf_addr || args->addr < addr_valid) {
                        mem_size = info.size_valid;
                        error = 0;
                        break;
                    }
                    if (addr_error < buf_addr || args->addr < addr_error) break;
                    buf_addr = addr_error;
                    buf_size = args->addr + args->size - buf_addr;
                    if (get_disassembler_isa(ctx, buf_addr, &isa) < 0) {
                        error = errno;
                        break;
                    }
                    if (isa.max_instruction_size > 0) {
                        mem_size = (size_t)(buf_size + isa.max_instruction_size);
                    }
                    else {
                        mem_size = (size_t)(buf_size + MAX_INSTRUCTION_SIZE);
                    }
                    /* Continue after unredable range */
                    error = 0;
                }
            }
#endif
        }
    }

    if (!error && disassemble_block(
            ctx, buf_out, mem_buf, buf_addr, buf_size,
            mem_size, &isa, args) < 0) error = errno;

    if (get_error_code(error) == ERR_CACHE_MISS) {
        loc_free(buf.mem);
        buf.mem = NULL;
        buf.max = 0;
        buf.pos = 0;
    }

    cache_exit();

    get_byte_array_output_stream_data(&buf, &data, &size);

    if (!is_channel_closed(c)) {
        OutputStream * out = &c->out;
        write_stringz(out, "R");
        write_stringz(out, args->token);
        write_errno(out, error);
        if (size > 0) {
            write_block_stream(out, data, size);
        }
        else {
            write_string(out, "null");
        }
        write_stream(out, 0);
        write_stream(out, MARKER_EOM);
    }

    loc_free(data);
    run_ctrl_unlock();
}

static void read_disassembly_params(InputStream * inp, const char * name, void * x) {
    DisassembleCmdArgs * args = (DisassembleCmdArgs *) x;

    if (strcmp(name, "ISA") == 0) {
        args->isa = json_read_alloc_string(inp);
    }
    else if (strcmp(name, "Simplified") == 0) {
        args->simplified = json_read_boolean(inp);
    }
    else if (strcmp(name, "Pseudo") == 0) {
        args->pseudo_instr = json_read_boolean(inp);
    }
    else if (strcmp(name, "OpcodeValue") == 0) {
        args->opcode_value = json_read_boolean(inp);
    }
    else {
        json_skip_object(inp);
    }
}

static void command_disassemble(char * token, Channel * c) {
    DisassembleCmdArgs args;

    memset(&args, 0, sizeof(args));
    json_read_string(&c->inp, args.id, sizeof(args.id));
    if (read_stream(&c->inp) != 0) exception(ERR_JSON_SYNTAX);
    args.addr = (ContextAddress)json_read_uint64(&c->inp);
    if (read_stream(&c->inp) != 0) exception(ERR_JSON_SYNTAX);
    args.size = (ContextAddress)json_read_uint64(&c->inp);
    if (read_stream(&c->inp) != 0) exception(ERR_JSON_SYNTAX);
    json_read_struct(&c->inp, read_disassembly_params, &args);
    if (read_stream(&c->inp) != 0) exception(ERR_JSON_SYNTAX);
    if (read_stream(&c->inp) != MARKER_EOM) exception(ERR_JSON_SYNTAX);

    run_ctrl_lock();
    strlcpy(args.token, token, sizeof(args.token));
    cache_enter(disassemble_cache_client, c, &args, sizeof(DisassembleCmdArgs));
}

static void event_context_disposed(Context * ctx, void * args) {
    unsigned i;
    ContextExtensionDS * ext = EXT(ctx);
    for (i = 0; i < ext->disassemblers_cnt; i++) {
        loc_free(ext->disassemblers[i].isa);
    }
    loc_free(ext->disassemblers);
}

void ini_disassembly_service(Protocol * proto) {
    static ContextEventListener listener = {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        event_context_disposed
    };
    if (context_extension_offset == 0) {
        add_context_event_listener(&listener, NULL);
        context_extension_offset = context_extension(sizeof(ContextExtensionDS));
    }
    add_command_handler(proto, DISASSEMBLY, "getCapabilities", command_get_capabilities);
    add_command_handler(proto, DISASSEMBLY, "disassemble", command_disassemble);
}

#endif /* SERVICE_Disassembly */
