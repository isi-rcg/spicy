/*******************************************************************************
 * Copyright (c) 2007-2020 Wind River Systems, Inc. and others.
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
 * This module implements Breakpoints service.
 * The service maintains a list of breakpoints.
 * Each breakpoint consists of one or more conditions that determine
 * when a program's execution should be interrupted.
 */

#include <tcf/config.h>

#if SERVICE_Breakpoints

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <tcf/framework/errors.h>
#include <tcf/framework/trace.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/exceptions.h>
#include <tcf/framework/cache.h>
#include <tcf/framework/json.h>
#include <tcf/framework/link.h>
#include <tcf/services/symbols.h>
#include <tcf/services/runctrl.h>
#include <tcf/services/contextquery.h>
#include <tcf/services/breakpoints.h>
#include <tcf/services/breakpoints-ext.h>
#include <tcf/services/expressions.h>
#include <tcf/services/linenumbers.h>
#include <tcf/services/stacktrace.h>
#include <tcf/services/memorymap.h>
#include <tcf/services/pathmap.h>


/* ENABLE_SkipPrologueWhenPlanting: select how "skip prologue" is implemented:
 * 0 - plant breakpoint at function entry, then step until after prologue;
 * 1 - when planting, adjust breakpoint address to a location right after prologue;
 */
#if !defined(ENABLE_SkipPrologueWhenPlanting)
#  define ENABLE_SkipPrologueWhenPlanting 0
#endif

typedef struct BreakpointRef BreakpointRef;
typedef struct InstructionRef InstructionRef;
typedef struct BreakInstruction BreakInstruction;
typedef struct EvaluationArgs EvaluationArgs;
typedef struct EvaluationRequest EvaluationRequest;
typedef struct LocationEvaluationRequest LocationEvaluationRequest;
typedef struct ConditionEvaluationRequest ConditionEvaluationRequest;
typedef struct ContextExtensionBP ContextExtensionBP;
typedef struct BreakpointHitCount BreakpointHitCount;

struct BreakpointRef {
    LINK link_inp;
    LINK link_bp;
    Channel * channel; /* NULL means API client */
    BreakpointInfo * bp;
};

struct BreakpointInfo {
    Context * ctx; /* NULL means all contexts */
    LINK link_all;
    LINK link_id;
    LINK link_clients;
    char id[256];
    int enabled;
    int client_cnt;
    int instruction_cnt;
    ErrorReport * error;
    BreakpointAttribute * inv_attr;
    char * type;
    char * location;
    char * condition;
    char * context_query;
    char ** context_ids;
    char ** context_names;
    char ** stop_group;
    char * file;
    char * client_data;
    int temporary;
    int skip_prologue;
    int access_mode;
    int access_size;
    int line;
    int line_offs_limit;
    int line_offs_check;
    int column;
    unsigned ignore_count;
    BreakpointAttribute * attrs;

    EventPointCallBack * event_callback;
    void * event_callback_args;

    int attrs_changed;
    int status_changed;
    LINK link_hit_count;
};

struct BreakpointHitCount {
    LINK link_bp;
    LINK link_ctx;
    Context * ctx;
    unsigned count;
};

struct InstructionRef {
    BreakpointInfo * bp;
    Context * ctx; /* "breakpoint" group context, see CONTEXT_GROUP_BREAKPOINT */
    ContextAddress addr;
    unsigned cnt;
    int line_offs_error;
};

#define MAX_BI_SIZE 16

struct BreakInstruction {
    LINK link_all;
    LINK link_adr;
    LINK link_lst;
    ContextBreakpoint cb; /* cb.ctx is "canonical" context, see context_get_canonical_addr() */
    char saved_code[MAX_BI_SIZE];
    char planted_code[MAX_BI_SIZE];
    size_t saved_size;
    ErrorReport * planting_error;
    ErrorReport * address_error;
    ErrorReport * ph_address_error;
    ErrorReport * condition_error;
    int stepping_over_bp;
    InstructionRef * refs;
    unsigned ref_size;
    unsigned ref_cnt;
    uint8_t no_addr;
    uint8_t virtual_addr;
    uint8_t hardware;
    uint8_t valid;       /* 1 if 'refs' array is valid */
    uint8_t planted;
    uint8_t dirty;       /* the instruction is planted, but planting data is obsolete */
    uint8_t unsupported; /* context_plant_breakpoint() returned ERR_UNSUPPORTED */
    uint8_t planted_as_sw_bp;
    int isa_conflict;
    uint8_t * bp_encoding;  /* Encoding of breakpoint instruction */
    size_t bp_size;         /* Size of breakpoint instruction */
    Context * ph_ctx;
    ContextAddress ph_addr;
};

struct EvaluationArgs {
    BreakpointInfo * bp;
    Context * ctx;
    unsigned index;
};

struct ConditionEvaluationRequest {
    Context * ctx;
    BreakpointInfo * bp;
    BreakInstruction * bi;
    int condition_ok;
    int triggered;
};

struct LocationEvaluationRequest {
#   define LOC_EVALUATION_BP_MAX 8
#   define LOC_EVALUATION_BP_ALL 9
    BreakpointInfo * bp_arr[LOC_EVALUATION_BP_MAX];
    unsigned bp_cnt; /* bp_cnt > LOC_EVALUATION_BP_MAX means all breakpoints */
};

struct EvaluationRequest {
    Context * ctx; /* Must be breakpoints group context */
    LINK link_posted;
    LINK link_active;
    LocationEvaluationRequest loc_posted;
    LocationEvaluationRequest loc_active;
    ConditionEvaluationRequest * bp_arr;
    unsigned bp_cnt;
    unsigned bp_max;
};

struct ContextExtensionBP {
    int step_over_bp_cnt;
    BreakInstruction * stepping_over_bp;    /* if not NULL, the context is stepping over a breakpoint instruction */
    EvaluationRequest * req;
    Context * bp_grp;
    int empty_bp_grp;
    int instruction_cnt;
    LINK link_hit_count;
};

static const char * BREAKPOINTS = "Breakpoints";

static size_t context_extension_offset = 0;

typedef struct {
    BreakpointsEventListener * listener;
    void * args;
} Listener;

static Listener * listeners = NULL;
static unsigned listener_cnt = 0;
static unsigned listener_max = 0;

#define EXT(ctx) ((ContextExtensionBP *)((char *)(ctx) + context_extension_offset))

#define is_disabled(bp) (bp->enabled == 0 || bp->client_cnt == 0)

#define ADDR2INSTR_HASH_SIZE (32 * MEM_USAGE_FACTOR - 1)
#define addr2instr_hash(ctx, addr) ((unsigned)((uintptr_t)(ctx) + (uintptr_t)(addr) + ((uintptr_t)(addr) >> 8)) % ADDR2INSTR_HASH_SIZE)

#define link_all2bi(A)  ((BreakInstruction *)((char *)(A) - offsetof(BreakInstruction, link_all)))
#define link_adr2bi(A)  ((BreakInstruction *)((char *)(A) - offsetof(BreakInstruction, link_adr)))
#define link_lst2bi(A)  ((BreakInstruction *)((char *)(A) - offsetof(BreakInstruction, link_lst)))

#define ID2BP_HASH_SIZE (32 * MEM_USAGE_FACTOR - 1)

#define link_all2bp(A)  ((BreakpointInfo *)((char *)(A) - offsetof(BreakpointInfo, link_all)))
#define link_id2bp(A)   ((BreakpointInfo *)((char *)(A) - offsetof(BreakpointInfo, link_id)))

#define INP2BR_HASH_SIZE (4 * MEM_USAGE_FACTOR - 1)

#define link_inp2br(A)  ((BreakpointRef *)((char *)(A) - offsetof(BreakpointRef, link_inp)))
#define link_bp2br(A)   ((BreakpointRef *)((char *)(A) - offsetof(BreakpointRef, link_bp)))

#define link_posted2erl(A)  ((EvaluationRequest *)((char *)(A) - offsetof(EvaluationRequest, link_posted)))
#define link_active2erl(A)  ((EvaluationRequest *)((char *)(A) - offsetof(EvaluationRequest, link_active)))
#define link_bcg2chnl(A) ((Channel *)((char *)(A) - offsetof(Channel, bclink)))

#define link_bp2hcnt(A)  ((BreakpointHitCount *)((char *)(A) - offsetof(BreakpointHitCount, link_bp)))
#define link_ctx2hcnt(A)  ((BreakpointHitCount *)((char *)(A) - offsetof(BreakpointHitCount, link_ctx)))

#if ENABLE_SkipPrologueWhenPlanting
#  define suspend_by_bp(ctx, trigger, bp, skip_prologue) suspend_by_breakpoint(ctx, trigger, bp, 0)
#else
#  define suspend_by_bp(ctx, trigger, bp, skip_prologue) suspend_by_breakpoint(ctx, trigger, bp, skip_prologue)
#endif

static LINK breakpoints = TCF_LIST_INIT(breakpoints);
static LINK id2bp[ID2BP_HASH_SIZE];

static LINK instructions = TCF_LIST_INIT(instructions);
static LINK addr2instr[ADDR2INSTR_HASH_SIZE];

static LINK inp2br[INP2BR_HASH_SIZE];

static LINK evaluations_posted = TCF_LIST_INIT(evaluations_posted);
static LINK evaluations_active = TCF_LIST_INIT(evaluations_active);
static uintptr_t generation_posted = 0;
static uintptr_t generation_active = 0;
static uintptr_t generation_done = 0;
static int planting_instruction = 0;
static int cache_enter_cnt = 0;
static int planted_sw_bp_cnt = 0;

static int bp_location_error = 0;
#if ENABLE_LineNumbers
static unsigned bp_line_cnt = 0;
static unsigned bp_stmt_cnt = 0;
static unsigned bp_line_addr_cnt = 0;
static unsigned bp_line_addr_max = 0;
static ContextAddress * bp_line_addr = NULL;
#endif

static TCFBroadcastGroup * broadcast_group = NULL;

static unsigned id2bp_hash(const char * id) {
    unsigned hash = 0;
    while (*id) hash = (hash >> 16) + hash + (unsigned char)*id++;
    return hash % ID2BP_HASH_SIZE;
}

static unsigned get_bp_access_types(BreakpointInfo * bp, int virtual_addr) {
    char * type = bp->type;
    unsigned access_types = bp->access_mode;
    if (access_types == 0 && (bp->file != NULL || bp->location != NULL)) access_types |= CTX_BP_ACCESS_INSTRUCTION;
    if (virtual_addr && type != NULL && strcmp(type, "Software") == 0) access_types |= CTX_BP_ACCESS_SOFTWARE;
    if (virtual_addr) access_types |= CTX_BP_ACCESS_VIRTUAL;
    return access_types;
}

static int select_sw_breakpoint_isa(BreakInstruction * sw, uint8_t ** bp_encoding, size_t * bp_size) {
    /* Software breakpoint should be rejected if opcode is unknown or ambiguous */
    size_t size = 0;
    uint8_t * encoding = NULL;
    LINK * l = instructions.next;
    while (l != &instructions) {
        BreakInstruction * bi = link_all2bi(l);
        if (bi->ph_ctx == sw->cb.ctx && bi->ph_addr == sw->cb.address) {
            /* Virtual address breakpoint has same canonical address, check opcode */
            int isa_conflict = bi->isa_conflict;
            if (bi->bp_encoding != NULL) {
                if (encoding == NULL) {
                    encoding = bi->bp_encoding;
                    size = bi->bp_size;
                }
                else if (size != bi->bp_size) {
                    isa_conflict = 1;
                }
                else if (memcmp(encoding, bi->bp_encoding, bi->bp_size) != 0) {
                    isa_conflict = 1;
                }
            }
            if (isa_conflict) {
                set_errno(ERR_OTHER, "Conflicting software breakpoint in shared memory");
                return -1;
            }
        }
        l = l->next;
    }
    *bp_encoding = encoding;
    *bp_size = size;
    return 0;
}


static void plant_instruction(BreakInstruction * bi) {
    int error = 0;
    size_t saved_size = bi->saved_size;
    ErrorReport * rp = NULL;

    assert(!bi->stepping_over_bp);
    assert(!bi->planted);
    assert(!bi->dirty);
    assert(!bi->cb.ctx->exited);
    assert(!bi->cb.ctx->exiting);
    assert(bi->valid);
    assert(bi->ref_cnt > 0);
    assert(bi->address_error == NULL);
    assert_all_stopped(bi->cb.ctx);

    bi->saved_size = 0;
    bi->unsupported = 0;

    if (bi->virtual_addr && (bi->cb.access_types & CTX_BP_ACCESS_INSTRUCTION) != 0) {
        unsigned i;
        int line_offs_ok = 0;
        for (i = 0; i < bi->ref_cnt; i++) {
            InstructionRef * ref = bi->refs + i;
            if (ref->cnt > 0 && !ref->line_offs_error) {
                line_offs_ok = 1;
                break;
            }
        }
        if (!line_offs_ok) {
            error = set_errno(ERR_OTHER, "No code at requested breakpoint position");
        }
    }

    if (error == 0) {
        if (context_plant_breakpoint(&bi->cb) < 0) error = errno;
        bi->unsupported = error && get_error_code(error) == ERR_UNSUPPORTED;
    }

    if (bi->unsupported && !bi->virtual_addr && !bi->hardware) {
        uint8_t * bp_encoding = NULL;
        size_t bp_size = 0;
        error = 0;
        assert(!list_is_empty(&bi->link_all));
        if (select_sw_breakpoint_isa(bi, &bp_encoding, &bp_size) < 0) {
            error = errno;
        }
        else if (bp_encoding == NULL) {
            error = set_errno(ERR_OTHER, "Cannot find instruction opcode for software breakpoint");
        }
        else {
            bi->saved_size = bp_size;
            assert(bi->saved_size > 0);
            assert(sizeof(bi->saved_code) >= bi->saved_size);
            assert(!bi->virtual_addr);
            planting_instruction = 1;
            memcpy(bi->planted_code, bp_encoding, bi->saved_size);
            if (context_read_mem(bi->cb.ctx, bi->cb.address, bi->saved_code, bi->saved_size) < 0) {
                error = errno;
            }
            else if (context_write_mem(bi->cb.ctx, bi->cb.address, bi->planted_code, bi->saved_size) < 0) {
                error = errno;
            }
            planting_instruction = 0;
        }
    }
    else if (error == ERR_UNSUPPORTED) {
        if (bi->ph_address_error) error = set_error_report_errno(bi->ph_address_error);
        else error = set_errno(ERR_OTHER, "Unsupported set of breakpoint attributes");
    }

    rp = get_error_report(error);
    if (saved_size != bi->saved_size || !compare_error_reports(bi->planting_error, rp)) {
        unsigned i;
        release_error_report(bi->planting_error);
        bi->planting_error = rp;
        for (i = 0; i < bi->ref_cnt; i++) {
            bi->refs[i].bp->status_changed = 1;
        }
    }
    else {
        release_error_report(rp);
    }
    bi->planted = bi->planting_error == NULL;
    assert(bi->planted || !bi->dirty);
    if (bi->planted && !bi->virtual_addr) planted_sw_bp_cnt++;
}

static int remove_instruction(BreakInstruction * bi) {
    assert(bi->planted);
    assert(bi->planting_error == NULL);
    assert(bi->address_error == NULL);
    assert_all_stopped(bi->cb.ctx);
    if (bi->saved_size) {
        if (!bi->cb.ctx->exited) {
            int r = 0;
            char buf[MAX_BI_SIZE];
            planting_instruction = 1;
            r = context_read_mem(bi->cb.ctx, bi->cb.address, buf, bi->saved_size);
            if (r >= 0 && memcmp(buf, bi->planted_code, bi->saved_size) == 0) {
                r = context_write_mem(bi->cb.ctx, bi->cb.address, bi->saved_code, bi->saved_size);
            }
            planting_instruction = 0;
            if (r < 0) return -1;
        }
    }
    else {
        if (context_unplant_breakpoint(&bi->cb) < 0) return -1;
        if (bi->cb.ctx->stopped_by_cb != NULL) {
            ContextBreakpoint ** p = bi->cb.ctx->stopped_by_cb;
            while (*p != NULL && *p != &bi->cb) p++;
            while (*p != NULL && (*p = *(p + 1)) != NULL) p++;
        }
    }
    if (!bi->virtual_addr) planted_sw_bp_cnt--;
    bi->planted = 0;
    bi->dirty = 0;
    return 0;
}

#ifndef NDEBUG
static int is_canonical_addr(Context * ctx, ContextAddress address) {
    Context * mem = NULL;
    ContextAddress mem_addr = 0;
    if (context_get_canonical_addr(ctx, address, &mem, &mem_addr, NULL, NULL) < 0) return 0;
    return mem == ctx && address == mem_addr;
}
#endif

static BreakInstruction * find_instruction(Context * ctx, int virtual_addr,
        ContextAddress address, unsigned access_types, ContextAddress access_size) {
    int hash = addr2instr_hash(ctx, address);
    LINK * l = addr2instr[hash].next;
    assert(virtual_addr || is_canonical_addr(ctx, address));
    while (l != addr2instr + hash) {
        BreakInstruction * bi = link_adr2bi(l);
        if (bi->cb.ctx == ctx &&
            bi->cb.address == address &&
            bi->cb.length == access_size &&
            bi->cb.access_types == access_types &&
            bi->virtual_addr == virtual_addr &&
            bi->no_addr == 0)
        {
            return bi;
        }
        l = l->next;
    }
    return NULL;
}

static BreakInstruction * add_instruction(Context * ctx, int virtual_addr,
        ContextAddress address, unsigned access_types, ContextAddress access_size) {
    int hash = addr2instr_hash(ctx, address);
    BreakInstruction * bi = (BreakInstruction *)loc_alloc_zero(sizeof(BreakInstruction));
    assert(find_instruction(ctx, virtual_addr, address, access_types, access_size) == NULL);
    list_add_last(&bi->link_all, &instructions);
    list_add_last(&bi->link_adr, addr2instr + hash);
    context_lock(ctx);
    bi->cb.ctx = ctx;
    bi->cb.address = address;
    bi->cb.length = access_size;
    bi->cb.access_types = access_types;
    bi->virtual_addr = (uint8_t)virtual_addr;
    return bi;
}

static void clear_instruction_refs(Context * ctx, BreakpointInfo * bp) {
    LINK * l = instructions.next;
    assert_all_stopped(ctx);
    while (l != &instructions) {
        unsigned i;
        BreakInstruction * bi = link_all2bi(l);
        for (i = 0; i < bi->ref_cnt; i++) {
            InstructionRef * ref = bi->refs + i;
            if (ref->ctx != ctx) continue;
            if (bp != NULL && ref->bp != bp) continue;
            ref->cnt = 0;
            bi->valid = 0;
        }
        l = l->next;
    }
}

static void free_instruction(BreakInstruction * bi) {
    assert(bi->dirty == 0);
    assert(bi->planted == 0);
    assert(bi->ref_cnt == 0);
    assert(bi->stepping_over_bp == 0);
    list_remove(&bi->link_all);
    list_remove(&bi->link_adr);
    context_unlock(bi->cb.ctx);
    release_error_report(bi->address_error);
    release_error_report(bi->ph_address_error);
    release_error_report(bi->planting_error);
    release_error_report(bi->condition_error);
    loc_free(bi->bp_encoding);
    loc_free(bi->refs);
    loc_free(bi);
}

static BreakInstruction ** plant_at_canonical_address(BreakInstruction * v_bi);

static void validate_bi_refs(BreakInstruction * bi) {
    unsigned i = 0;
    assert(!bi->valid);
    while (i < bi->ref_cnt) {
        InstructionRef * ref = bi->refs + i;
        if (ref->cnt == 0 || ref->ctx->exiting || ref->ctx->exited) {
            ref->bp->instruction_cnt--;
            ref->bp->status_changed = 1;
            EXT(ref->ctx)->instruction_cnt--;
            context_unlock(ref->ctx);
            memmove(ref, ref + 1, sizeof(InstructionRef) * (bi->ref_cnt - i - 1));
            if (bi->planted) bi->dirty = 1;
            bi->ref_cnt--;
        }
        else {
            if (ref->bp->attrs_changed && bi->planted) bi->dirty = 1;
            i++;
        }
    }
    bi->valid = 1;
}

static void flush_instructions(void) {
    LINK lst;
    LINK * l;

    list_init(&lst);

    /* Validate references */
    l = instructions.next;
    while (l != &instructions) {
        BreakInstruction * bi = link_all2bi(l);
        list_init(&bi->link_lst);
        l = l->next;
        if (bi->address_error) {
            assert(bi->no_addr);
            assert(!bi->planted);
            if (!bi->valid) {
                validate_bi_refs(bi);
                bi->planted_as_sw_bp = 0;
                bi->hardware = 0;
            }
        }
        else if (!bi->valid) {
            bi->planted_as_sw_bp = 0;
            bi->hardware = 1;
            if (bi->no_addr == 0) {
                static const unsigned mask = ~(unsigned)(CTX_BP_ACCESS_VIRTUAL | CTX_BP_ACCESS_SOFTWARE);
                if ((bi->cb.access_types & mask) == CTX_BP_ACCESS_INSTRUCTION && bi->cb.length == 1) {
                    unsigned i;
                    bi->hardware = 0;
                    for (i = 0; i < bi->ref_cnt; i++) {
                        char * type = bi->refs[i].bp->type;
                        if (bi->refs[i].cnt == 0) continue;
                        if (type == NULL) continue;
                        if (strcmp(type, "Hardware") == 0) {
                            bi->hardware = 1;
                            break;
                        }
                    }
                }
            }
            if (bi->hardware || bi->virtual_addr) {
                validate_bi_refs(bi);
            }
            list_add_last(&bi->link_lst, &lst);
        }
        else if (bi->hardware && !bi->planted && is_all_stopped(bi->cb.ctx)) {
            /* Hardware resource might be available now, try to re-plant */
            list_add_last(&bi->link_lst, &lst);
        }
    }

    /* Unplant breakpoints */
    l = lst.next;
    while (l != &lst) {
        BreakInstruction * bi = link_lst2bi(l);
        l = l->next;
        if (!bi->valid) {
            assert(!bi->hardware);
            assert(!bi->virtual_addr);
            continue;
        }
        if (bi->stepping_over_bp) continue;
        if (bi->planted) {
            assert(!bi->address_error);
            if (bi->dirty) {
                remove_instruction(bi);
            }
            else if (bi->ref_cnt == 0 && bi->virtual_addr) {
                remove_instruction(bi);
            }
#if !defined(_WRS_KERNEL)
            /*
             * It is an issue if the TCF agent removes and install again and again
             * the breakpoints on a VxWorks target even if the breakpoint has not
             * changed. It may cause some breakpoints to not be hit.
             * This is caused by the optimisation below; this optimisation
             * does not apply to VxWorks and can be safely disabled.
             */
            else if (bi->saved_size == 0 && !bi->hardware) {
                /* Free space for hardware breakpoints */
                remove_instruction(bi);
            }
#endif  /* _WRS_KERNEL */
        }
        if (bi->planted || bi->ref_cnt == 0 || bi->cb.ctx->exiting || bi->cb.ctx->exited) {
            list_remove(&bi->link_lst);
        }
    }

    /* Plant hardware breakpoints first */
    l = lst.next;
    while (l != &lst) {
        BreakInstruction * bi = link_lst2bi(l);
        l = l->next;
        if (bi->hardware) {
            if (!bi->planted) plant_instruction(bi);
            list_remove(&bi->link_lst);
        }
    }

    /* Plant the rest of virtual address breakpoints */
    l = lst.next;
    while (l != &lst) {
        BreakInstruction * bi = link_lst2bi(l);
        l = l->next;
        if (bi->virtual_addr) {
            assert(bi->ref_cnt > 0);
            if (!bi->planted) plant_instruction(bi);
            if (!bi->planted && bi->unsupported && bi->cb.length == 1 && bi->ph_ctx != NULL) {
                unsigned i;
                BreakInstruction ** ca_lst = plant_at_canonical_address(bi);
                bi->planted_as_sw_bp = 1;
                for (i = 0; i < bi->ref_cnt; i++) {
                    BreakInstruction * ca = ca_lst[i];
                    assert(!ca->hardware);
                    assert(!ca->virtual_addr);
                    if (list_is_empty(&ca->link_lst)) {
                        list_add_last(&ca->link_lst, &lst);
                    }
                }
            }
            list_remove(&bi->link_lst);
        }
    }

    /* Validate and plant canonical address breakpoints */
    l = lst.next;
    while (l != &lst) {
        BreakInstruction * bi = link_lst2bi(l);
        l = l->next;
        assert(!bi->hardware);
        assert(!bi->virtual_addr);
        if (!bi->valid) validate_bi_refs(bi);
        if (bi->address_error) continue;
        assert(!bi->no_addr);
        if (bi->stepping_over_bp) continue;
        if (bi->ref_cnt == 0) continue;
        if (!bi->planted) plant_instruction(bi);
    }

    /* Free unused break instructions */
    l = instructions.next;
    while (l != &instructions) {
        BreakInstruction * bi = link_all2bi(l);
        l = l->next;
        assert(bi->valid);
        list_init(&bi->link_lst);
        if (bi->ref_cnt > 0) continue;
        if (bi->stepping_over_bp) continue;
        if (bi->planted && is_all_stopped(bi->cb.ctx)) remove_instruction(bi);
        if (!bi->planted) free_instruction(bi);
    }
}

static unsigned get_bp_hit_count(BreakpointInfo * bp, Context * ctx) {
    unsigned count = 0;
    LINK * l = bp->link_hit_count.next;
    while (l != &bp->link_hit_count) {
        BreakpointHitCount * c = link_bp2hcnt(l);
        if (c->ctx == ctx) count += c->count;
        l = l->next;
    }
    return count;
}

static unsigned inc_bp_hit_count(BreakpointInfo * bp, Context * ctx) {
    unsigned count = 0;
    LINK * l = bp->link_hit_count.next;
    while (l != &bp->link_hit_count) {
        BreakpointHitCount * c = link_bp2hcnt(l);
        if (c->ctx == ctx) {
            c->count++;
            count += c->count;
            break;
        }
        l = l->next;
    }
    if (count == 0) {
        BreakpointHitCount * c = (BreakpointHitCount *)loc_alloc_zero(sizeof(BreakpointHitCount));
        list_add_first(&c->link_bp, &bp->link_hit_count);
        list_add_first(&c->link_ctx, &(EXT(ctx))->link_hit_count);
        c->count = count = 1;
        c->ctx = ctx;
    }
    bp->status_changed = 1;
    return count;
}

static void reset_bp_hit_count(BreakpointInfo * bp) {
    LINK * l = bp->link_hit_count.next;
    while (l != &bp->link_hit_count) {
        BreakpointHitCount * c = link_bp2hcnt(l);
        l = l->next;
        list_remove(&c->link_bp);
        list_remove(&c->link_ctx);
        loc_free(c);
        bp->status_changed = 1;
    }
}

void clone_breakpoints_on_process_fork(Context * parent, Context * child) {
    Context * mem = context_get_group(parent, CONTEXT_GROUP_PROCESS);
    LINK * l = instructions.next;
    assert(child == context_get_group(child, CONTEXT_GROUP_PROCESS));
    assert(child != mem);
    while (l != &instructions) {
        unsigned i;
        BreakInstruction * ci = NULL;
        BreakInstruction * bi = link_all2bi(l);
        l = l->next;
        if (!bi->planted) continue;
        if (!bi->saved_size) continue;
        if (bi->cb.ctx != mem) continue;
        ci = add_instruction(child, bi->virtual_addr, bi->cb.address, bi->cb.access_types, bi->cb.length);
        memcpy(ci->saved_code, bi->saved_code, bi->saved_size);
        memcpy(ci->planted_code, bi->planted_code, bi->saved_size);
        ci->saved_size = bi->saved_size;
        ci->ref_size = bi->ref_size;
        ci->ref_cnt = bi->ref_cnt;
        ci->refs = (InstructionRef *)loc_alloc_zero(sizeof(InstructionRef) * ci->ref_size);
        for (i = 0; i < bi->ref_cnt; i++) {
            BreakpointInfo * bp = bi->refs[i].bp;
            ci->refs[i] = bi->refs[i];
            ci->refs[i].ctx = child;
            context_lock(child);
            EXT(child)->instruction_cnt++;
            bp->instruction_cnt++;
            bp->status_changed = 1;
        }
        ci->valid = 1;
        ci->planted = 1;
        if (!bi->virtual_addr) planted_sw_bp_cnt++;
    }
}

void invalidate_breakpoints_on_process_exec(Context * ctx) {
    Context * mem = context_get_group(ctx, CONTEXT_GROUP_PROCESS);
    LINK * l = instructions.next;
    while (l != &instructions) {
        BreakInstruction * bi = link_all2bi(l);
        l = l->next;
        if (!bi->planted) continue;
        if (!bi->saved_size) continue;
        if (bi->cb.ctx != mem) continue;
        if (!bi->virtual_addr) planted_sw_bp_cnt--;
        bi->planted = 0;
        bi->dirty = 0;
    }
}

int unplant_breakpoints(Context * ctx) {
    int error = 0;
    LINK * l = instructions.next;
    /* Note: the function can be called for a fork child process
     * that we don't want to attach. All references to such process
     * should be removed immediately, we cannot rely on
     * event_replant_breakpoints() to do that. */
    while (l != &instructions) {
        unsigned i;
        BreakInstruction * bi = link_all2bi(l);
        l = l->next;
        if (bi->cb.ctx != ctx) continue;
        if (bi->planted && remove_instruction(bi) < 0) {
            error = errno;
            continue;
        }
        for (i = 0; i < bi->ref_cnt; i++) {
            BreakpointInfo * bp = bi->refs[i].bp;
            Context * bx = bi->refs[i].ctx;
            assert(bp->instruction_cnt > 0);
            bp->instruction_cnt--;
            bp->status_changed = 1;
            EXT(bx)->instruction_cnt--;
            context_unlock(bx);
        }
        bi->ref_cnt = 0;
        free_instruction(bi);
    }
    if (!error) return 0;
    errno = error;
    return -1;
}

int check_breakpoints_on_memory_read(Context * ctx, ContextAddress address, void * p, size_t size) {
    if (!planting_instruction) {
        while (size > 0) {
            size_t sz = size;
            uint8_t * buf = (uint8_t *)p;
            LINK * l = instructions.next;
            Context * mem = NULL;
            ContextAddress mem_addr = 0;
            ContextAddress mem_base = 0;
            ContextAddress mem_size = 0;
            while (l != &instructions) {
                BreakInstruction * bi = link_all2bi(l);
                size_t i;
                l = l->next;
                if (!bi->planted) continue;
                if (!bi->saved_size) continue;
                if (mem == NULL) {
                    if (context_get_canonical_addr(ctx, address, &mem, &mem_addr, &mem_base, &mem_size) < 0) return -1;
                    if ((size_t)(mem_base + mem_size - mem_addr) < sz) sz = (size_t)(mem_base + mem_size - mem_addr);
                }
                if (bi->cb.ctx != mem) continue;
                if (bi->cb.address + bi->saved_size <= mem_addr) continue;
                if (bi->cb.address >= mem_addr + sz) continue;
                for (i = 0; i < bi->saved_size; i++) {
                    if (bi->cb.address + i < mem_addr) continue;
                    if (bi->cb.address + i >= mem_addr + sz) continue;
                    buf[bi->cb.address + i - mem_addr] = bi->saved_code[i];
                }
            }
            p = (uint8_t *)p + sz;
            address += sz;
            size -= sz;
        }
    }
    return 0;
}

int check_breakpoints_on_memory_write(Context * ctx, ContextAddress address, void * p, size_t size) {
    if (!planting_instruction) {
        while (size > 0) {
            size_t sz = size;
            uint8_t * buf = (uint8_t *)p;
            LINK * l = instructions.next;
            Context * mem = NULL;
            ContextAddress mem_addr = 0;
            ContextAddress mem_base = 0;
            ContextAddress mem_size = 0;
            while (l != &instructions) {
                size_t i;
                BreakInstruction * bi = link_all2bi(l);
                l = l->next;
                if (!bi->planted) continue;
                if (!bi->saved_size) continue;
                if (mem == NULL) {
                    if (context_get_canonical_addr(ctx, address, &mem, &mem_addr, &mem_base, &mem_size) < 0) return -1;
                    if ((size_t)(mem_base + mem_size - mem_addr) < sz) sz = (size_t)(mem_base + mem_size - mem_addr);
                }
                if (bi->cb.ctx != mem) continue;
                if (bi->cb.address + bi->saved_size <= mem_addr) continue;
                if (bi->cb.address >= mem_addr + sz) continue;
                for (i = 0; i < bi->saved_size; i++) {
                    if (bi->cb.address + i < mem_addr) continue;
                    if (bi->cb.address + i >= mem_addr + sz) continue;
                    bi->saved_code[i] = buf[bi->cb.address + i - mem_addr];
                    buf[bi->cb.address + i - mem_addr] = bi->planted_code[i];
                }
            }
            p = (uint8_t *)p + sz;
            address += sz;
            size -= sz;
        }
    }
    return 0;
}

static void write_breakpoint_status(OutputStream * out, BreakpointInfo * bp) {
    write_stream(out, '{');

    if (bp->instruction_cnt) {
        int cnt = 0;
        LINK * l = instructions.next;
        json_write_string(out, "Instances");
        write_stream(out, ':');
        write_stream(out, '[');
        while (l != &instructions) {
            unsigned i;
            BreakInstruction * bi = link_all2bi(l);
            l = l->next;
            if (bi->planted_as_sw_bp) continue;
            for (i = 0; i < bi->ref_cnt; i++) {
                if (bi->refs[i].bp != bp) continue;
                if (cnt > 0) write_stream(out, ',');
                write_stream(out, '{');
                json_write_string(out, "LocationContext");
                write_stream(out, ':');
                json_write_string(out, bi->refs[i].ctx->id);
                if (bi->address_error != NULL) {
                    write_stream(out, ',');
                    json_write_string(out, "Error");
                    write_stream(out, ':');
                    json_write_string(out, errno_to_str(set_error_report_errno(bi->address_error)));
                }
                else {
                    write_stream(out, ',');
                    json_write_string(out, "HitCount");
                    write_stream(out, ':');
                    json_write_ulong(out, get_bp_hit_count(bp, bi->refs[i].ctx));
                    if (!bi->no_addr) {
                        write_stream(out, ',');
                        json_write_string(out, "Address");
                        write_stream(out, ':');
                        json_write_uint64(out, bi->refs[i].addr);
                    }
                    if (bi->cb.length > 0) {
                        write_stream(out, ',');
                        json_write_string(out, "Size");
                        write_stream(out, ':');
                        json_write_uint64(out, bi->cb.length);
                    }
                    if (bi->planting_error != NULL) {
                        write_stream(out, ',');
                        json_write_string(out, "Error");
                        write_stream(out, ':');
                        json_write_string(out, errno_to_str(set_error_report_errno(bi->planting_error)));
                    }
                    else if (bi->planted) {
                        int bp_type_is_set = 0;
#if ENABLE_ExtendedBreakpointStatus
                        if (bi->saved_size == 0) {
                            /* Back-end context breakpoint status */
                            int st_len = 0;
                            const char ** names = NULL;
                            const char ** values = NULL;
                            if (context_get_breakpoint_status(&bi->cb, &names, &values, &st_len) == 0) {
                                while (st_len > 0) {
                                    if (*values != NULL) {
                                        if (strcmp (*names, "BreakpointType") == 0) bp_type_is_set = 1;
                                        write_stream(out, ',');
                                        json_write_string(out, *names);
                                        write_stream(out, ':');
                                        write_string(out, *values);
                                    }
                                    names++;
                                    values++;
                                    st_len--;
                                }
                            }
                        }
#endif
                        if (bp_type_is_set == 0) {
                            write_stream(out, ',');
                            json_write_string(out, "BreakpointType");
                            write_stream(out, ':');
                            json_write_string(out, bi->saved_size ? "Software" : "Hardware");
                        }
                        if (bi->condition_error != NULL) {
                            write_stream(out, ',');
                            json_write_string(out, "ConditionError");
                            write_stream(out, ':');
                            json_write_string(out, errno_to_str(set_error_report_errno(bi->condition_error)));
                        }
                    }
                }
                write_stream(out, '}');
                cnt++;
            }
        }
        write_stream(out, ']');
        assert(generation_done != generation_active || cnt > 0);
    }
    else if (bp->error) {
        json_write_string(out, "Error");
        write_stream(out, ':');
        json_write_string(out, errno_to_str(set_error_report_errno(bp->error)));
    }

    write_stream(out, '}');
}

static void send_event_breakpoint_status(Channel * channel, BreakpointInfo * bp) {
    OutputStream * out = channel ? &channel->out : &broadcast_group->out;
    unsigned i;

    assert(*bp->id);
    write_stringz(out, "E");
    write_stringz(out, BREAKPOINTS);
    write_stringz(out, "status");

    json_write_string(out, bp->id);
    write_stream(out, 0);
    write_breakpoint_status(out, bp);
    write_stream(out, 0);
    write_stream(out, MARKER_EOM);
    if (channel) return;

    for (i = 0; i < listener_cnt; i++) {
        Listener * l = listeners + i;
        if (l->listener->breakpoint_status_changed == NULL) continue;
        l->listener->breakpoint_status_changed(bp, l->args);
    }
}

typedef struct LineOffsCheckArgs {
    BreakpointInfo * bp;
    BreakInstruction * bi;
    unsigned line_offs_ok;
    char * file;
} LineOffsCheckArgs;

#if ENABLE_LineNumbers
static void line_offs_check(CodeArea * area, void * x) {
    LineOffsCheckArgs * args = (LineOffsCheckArgs *)x;
    assert(area->start_address <= args->bi->cb.address);
    assert(area->end_address > args->bi->cb.address);
    if (area->file != NULL &&
            area->start_line >= args->bp->line - args->bp->line_offs_limit &&
            area->start_line <= args->bp->line + args->bp->line_offs_limit) {
        char buf[FILE_PATH_SIZE];
        char * file = NULL;
        if (area->directory == NULL || is_absolute_path(area->file)) {
            file = (char *)area->file;
        }
        else {
            snprintf(file = buf, sizeof(buf), "%s/%s", area->directory, area->file);
        }
        file = canonic_path_map_file_name(file);
        args->line_offs_ok = strcmp(args->file, file) == 0;
    }
}
#endif

static void verify_line_offset(BreakInstruction * bi, InstructionRef * ref) {
    ref->line_offs_error = 0;
    if (bi->virtual_addr && (bi->cb.access_types & CTX_BP_ACCESS_INSTRUCTION) != 0) {
        LineOffsCheckArgs args;
        assert(ref->ctx == bi->cb.ctx);
        assert(ref->addr == bi->cb.address);
        memset(&args, 0, sizeof(args));
        if (ref->bp->file != NULL && ref->bp->line_offs_check) {
            args.bi = bi;
            args.bp = ref->bp;
            args.file = canonic_path_map_file_name(ref->bp->file);
            if (address_to_line(ref->ctx, ref->addr, ref->addr + 1, line_offs_check, &args) < 0) {
                ref->line_offs_error = 1;
            }
            else if (!args.line_offs_ok) {
                ref->line_offs_error = 1;
            }
        }
    }
}

static void get_bp_opcodes(void) {
    LINK * l = instructions.next;
    while (l != &instructions) {
        BreakInstruction * bi = link_all2bi(l);
        if (!bi->valid && !bi->no_addr && !bi->hardware && bi->virtual_addr) {
            bi->isa_conflict = 0;
            loc_free(bi->bp_encoding);
            bi->bp_encoding = NULL;
            bi->bp_size = 0;
            bi->ph_ctx = NULL;
            bi->ph_addr = 0;
            release_error_report(bi->ph_address_error);
            bi->ph_address_error = 0;
            if (bi->ref_cnt > 0 && !bi->cb.ctx->exiting && !bi->cb.ctx->exited &&
                    (bi->cb.access_types & CTX_BP_ACCESS_INSTRUCTION) != 0) {
                Context * ph_ctx = NULL;
                ContextAddress ph_addr = 0;
                if (context_get_canonical_addr(bi->cb.ctx, bi->cb.address, &ph_ctx, &ph_addr, NULL, NULL) < 0) {
                    bi->ph_address_error = get_error_report(errno);
                }
                else {
#if ENABLE_ContextISA
                    ContextISA isa;
                    LINK * m = channel_root.next;
                    while (m != &channel_root) {
                        Channel * c = chanlink2channelp(m);
                        if (!is_channel_closed(c)) {
                            cache_set_def_channel(c);
                            if (context_get_isa(bi->cb.ctx, bi->cb.address, &isa) == 0 &&
                                    isa.isa != NULL && isa.bp_size > 0 && isa.bp_encoding) {
                                if (bi->bp_encoding == NULL) {
                                    bi->bp_encoding = (uint8_t *)loc_alloc(isa.bp_size);
                                    memcpy(bi->bp_encoding, isa.bp_encoding, isa.bp_size);
                                    bi->bp_size = isa.bp_size;
                                }
                                else if (bi->bp_size != isa.bp_size) {
                                    bi->isa_conflict = 1;
                                    break;
                                }
                                else if (memcmp(isa.bp_encoding, bi->bp_encoding, bi->bp_size) != 0) {
                                    bi->isa_conflict = 1;
                                    break;
                                }
                            }
                        }
                        m = m->next;
                    }
                    cache_set_def_channel(NULL);
                    if (bi->bp_encoding == NULL &&
                            context_get_isa(bi->cb.ctx, bi->cb.address, &isa) == 0 &&
                            isa.bp_size > 0 && isa.bp_encoding) {
                        bi->bp_encoding = (uint8_t *)loc_alloc(isa.bp_size);
                        memcpy(bi->bp_encoding, isa.bp_encoding, isa.bp_size);
                        bi->bp_size = isa.bp_size;
                    }
#endif
                    if (bi->bp_encoding == NULL) {
                        uint8_t * encoding = get_break_instruction(bi->cb.ctx, &bi->bp_size);
                        if (encoding != NULL) {
                            bi->bp_encoding = (uint8_t *)loc_alloc(bi->bp_size);
                            memcpy(bi->bp_encoding, encoding, bi->bp_size);
                        }
                    }
                    bi->ph_ctx = ph_ctx;
                    bi->ph_addr = ph_addr;
                }
            }
        }
        l = l->next;
    }
}

static BreakInstruction * link_breakpoint_instruction(
        BreakpointInfo * bp, Context * ctx,
        ContextAddress ctx_addr, ContextAddress size,
        Context * mem, int virtual_addr, ContextAddress mem_addr,
        ErrorReport * address_error) {

    BreakInstruction * bi = NULL;
    InstructionRef * ref = NULL;

    assert(mem == NULL || address_error == NULL);

    if (mem == NULL) {
        /* Breakpoint does not have an address, e.g. breakpoint on a signal or I/O event */
        int hash = addr2instr_hash(ctx, bp);
        LINK * l = addr2instr[hash].next;
        assert(ctx_addr == 0);
        assert(mem_addr == 0);
        assert(virtual_addr == 0);
        while (l != addr2instr + hash) {
            BreakInstruction * i = link_adr2bi(l);
            if (i->cb.ctx == ctx && i->no_addr && i->ref_cnt == 1 &&
                    i->refs[0].ctx == ctx && i->refs[0].bp == bp &&
                    compare_error_reports(address_error, i->address_error)) {
                release_error_report(address_error);
                i->refs[0].cnt++;
                return i;
            }
            l = l->next;
        }
        bi = (BreakInstruction *)loc_alloc_zero(sizeof(BreakInstruction));
        list_add_last(&bi->link_all, &instructions);
        list_add_last(&bi->link_adr, addr2instr + hash);
        context_lock(ctx);
        bi->cb.ctx = ctx;
        bi->no_addr = 1;
        bi->cb.access_types = CTX_BP_ACCESS_NO_ADDRESS;
        bi->address_error = address_error;
    }
    else {
        unsigned access_types = get_bp_access_types(bp, virtual_addr);
        bi = find_instruction(mem, virtual_addr, mem_addr, access_types, size);
        if (bi == NULL) {
            bi = add_instruction(mem, virtual_addr, mem_addr, access_types, size);
        }
        else {
            unsigned i = 0;
            while (i < bi->ref_cnt) {
                ref = bi->refs + i;
                if (ref->bp == bp && ref->ctx == ctx) {
                    assert(!bi->valid || !bi->virtual_addr);
                    ref->addr = ctx_addr;
                    ref->cnt++;
                    verify_line_offset(bi, ref);
                    return bi;
                }
                i++;
            }
        }
    }
    if (bi->ref_cnt >= bi->ref_size) {
        bi->ref_size = bi->ref_size == 0 ? 8 : bi->ref_size * 2;
        bi->refs = (InstructionRef *)loc_realloc(bi->refs, sizeof(InstructionRef) * bi->ref_size);
    }
    bi->valid = 0;
    ref = bi->refs + bi->ref_cnt++;
    memset(ref, 0, sizeof(InstructionRef));
    ref->bp = bp;
    ref->ctx = ctx;
    ref->addr = ctx_addr;
    ref->cnt = 1;
    context_lock(ctx);
    EXT(ctx)->instruction_cnt++;
    bp->instruction_cnt++;
    bp->status_changed = 1;
    verify_line_offset(bi, ref);
    return bi;
}

static BreakInstruction * address_expression_error(Context * ctx, BreakpointInfo * bp, int error) {
    ErrorReport * rp = NULL;
    assert(error != 0);
    assert(bp->error == NULL);
    if (get_error_code(error) == ERR_CACHE_MISS) return NULL;
    rp = get_error_report(error);
    assert(rp != NULL);
    return link_breakpoint_instruction(bp, ctx, 0, 0, NULL, 0, 0, rp);
}

static void plant_breakpoint(Context * ctx, BreakpointInfo * bp, ContextAddress addr, ContextAddress size) {
    link_breakpoint_instruction(bp, ctx, addr, size, ctx, 1, addr, NULL);
}

static BreakInstruction ** plant_at_canonical_address(BreakInstruction * v_bi) {
    Context * ctx = v_bi->cb.ctx;
    ContextAddress addr = v_bi->cb.address;
    ContextAddress size = v_bi->cb.length;
    BreakInstruction ** c_bi = (BreakInstruction **)tmp_alloc_zero(v_bi->ref_cnt * sizeof(BreakInstruction *));
    unsigned i;

    assert(v_bi->valid && !v_bi->no_addr && !v_bi->hardware && v_bi->virtual_addr);
    assert(v_bi->ph_ctx != NULL);

    for (i = 0; i < v_bi->ref_cnt; i++) {
        BreakpointInfo * bp = v_bi->refs[i].bp;
        assert(v_bi->refs[i].ctx == ctx);
        c_bi[i] = link_breakpoint_instruction(bp, ctx, addr, size, v_bi->ph_ctx, 0, v_bi->ph_addr, NULL);
        assert(c_bi[i]->ref_cnt > 0);
        assert(c_bi[i]->virtual_addr == 0);
        assert(c_bi[i]->ref_cnt > 1 || c_bi[i]->refs[0].bp == bp);
    }
    return c_bi;
}

static void event_replant_breakpoints(void * arg);

static EvaluationRequest * create_evaluation_request(Context * ctx) {
    EvaluationRequest * req = EXT(ctx)->req;
    if (req == NULL) {
        req = (EvaluationRequest *)loc_alloc_zero(sizeof(EvaluationRequest));
        req->ctx = ctx;
        list_init(&req->link_posted);
        list_init(&req->link_active);
        EXT(ctx)->req = req;
    }
    assert(req->ctx == ctx);
    return req;
}

static ConditionEvaluationRequest * add_condition_evaluation_request(
        EvaluationRequest * req, Context * ctx, BreakpointInfo * bp, BreakInstruction * bi) {
    unsigned i;
    ConditionEvaluationRequest * c = NULL;

    assert(bp->instruction_cnt);
    assert(bp->error == NULL);
    assert(ctx->stopped_by_bp || ctx->stopped_by_cb);

    for (i = 0; i < req->bp_cnt; i++) {
        if (req->bp_arr[i].ctx == ctx && req->bp_arr[i].bp == bp && req->bp_arr[i].bi == bi) return NULL;
    }

    if (req->bp_max <= req->bp_cnt) {
        req->bp_max = req->bp_cnt + 4;
        req->bp_arr = (ConditionEvaluationRequest *)loc_realloc(req->bp_arr, sizeof(ConditionEvaluationRequest) * req->bp_max);
    }
    c = req->bp_arr + req->bp_cnt++;
    context_lock(c->ctx = ctx);
    c->bp = bp;
    c->bi = bi;
    c->condition_ok = 0;
    c->triggered = 0;
    return c;
}

static void post_evaluation_request(EvaluationRequest * req) {
    if (list_is_empty(&req->link_posted)) {
        context_lock(req->ctx);
        list_add_last(&req->link_posted, &evaluations_posted);
        if (generation_posted == generation_done) run_ctrl_lock();
        post_event(event_replant_breakpoints, (void *)++generation_posted);
    }
}

static int check_context_ids_location(BreakpointInfo * bp, Context * ctx);

static void post_location_evaluation_request(Context * ctx, BreakpointInfo * bp) {
    ContextExtensionBP * ext = EXT(ctx);
    Context * grp = context_get_group(ctx, CONTEXT_GROUP_BREAKPOINT);

    assert(ctx->exited || id2ctx(ctx->id) == ctx);
    if (ext->bp_grp != NULL && ext->bp_grp != grp && !ext->bp_grp->exited) {
        /* The context has migrated into another breakpoint group.
         * If the old group became empty, we need to remove breakpoints in it.
         */
        int ctx_found = 0;
        LINK * l = context_root.next;
        while (l != &context_root) {
            Context * c = ctxl2ctxp(l);
            l = l->next;
            if (c->exited) continue;
            if (context_get_group(c, CONTEXT_GROUP_BREAKPOINT) == ext->bp_grp) {
                ctx_found = 1;
                break;
            }
        }
        if (!ctx_found) {
            EvaluationRequest * req = create_evaluation_request(ext->bp_grp);
            req->loc_posted.bp_cnt = LOC_EVALUATION_BP_ALL;
            post_evaluation_request(req);
            EXT(ext->bp_grp)->empty_bp_grp = 1;
        }
    }
    else if (bp == NULL && grp != NULL && EXT(grp)->instruction_cnt == 0) {
        EvaluationRequest * req = EXT(grp)->req;
        if (req == NULL || list_is_empty(&req->link_posted)) {
            /* Optimization: if no breakpoints to replant, ignore the request */
            int bp_found = 0;
            LINK * l = breakpoints.next;
            while (l != &breakpoints) {
                BreakpointInfo * b = link_all2bp(l);
                if (!is_disabled(b) && (b->context_query || check_context_ids_location(b, grp))) {
                    bp_found = 1;
                    break;
                }
                l = l->next;
            }
            if (!bp_found) return;
        }
    }
    ext->bp_grp = grp;
    if (grp != NULL) {
        EvaluationRequest * req = create_evaluation_request(grp);
        if (bp != NULL && req->loc_posted.bp_cnt < LOC_EVALUATION_BP_MAX) {
            req->loc_posted.bp_arr[req->loc_posted.bp_cnt++] = bp;
        }
        else {
            req->loc_posted.bp_cnt = LOC_EVALUATION_BP_ALL;
        }
        post_evaluation_request(req);
        EXT(grp)->empty_bp_grp = 0;
    }
}

static void run_bp_evaluation(CacheClient * client, BreakpointInfo * bp, Context * ctx, int index) {
    int cnt = 0;
    EvaluationArgs args;

    if (*bp->id && list_is_empty(&bp->link_clients)) return;

    args.bp = bp;
    args.ctx = ctx;
    args.index = index;

    if (*bp->id) {
        LINK * l = bp->link_clients.next;
        while (l != &bp->link_clients) {
            BreakpointRef * br = link_bp2br(l);
            Channel * c = br->channel;
            assert(br->bp == bp);
            if (c != NULL) {
                assert(!is_channel_closed(c));
                cache_set_def_channel(c);
                client(&args);
                cnt++;
            }
            l = l->next;
        }
    }
    if (cnt == 0) {
        LINK * l = channel_root.next;
        while (l != &channel_root) {
            Channel * c = chanlink2channelp(l);
            if (!is_channel_closed(c)) {
                cache_set_def_channel(c);
                client(&args);
                cnt++;
            }
            l = l->next;
        }
    }
    cache_set_def_channel(NULL);
    if (cnt == 0) client(&args);
}

static void free_bp(BreakpointInfo * bp) {
    assert(list_is_empty(&evaluations_posted));
    assert(list_is_empty(&evaluations_active));
    assert(list_is_empty(&bp->link_clients));
    assert(bp->instruction_cnt == 0);
    assert(bp->client_cnt == 0);
    reset_bp_hit_count(bp);
    list_remove(&bp->link_all);
    if (*bp->id) list_remove(&bp->link_id);
    if (bp->ctx) context_unlock(bp->ctx);
    release_error_report(bp->error);
    loc_free(bp->type);
    loc_free(bp->location);
    loc_free(bp->context_ids);
    loc_free(bp->context_names);
    loc_free(bp->context_query);
    loc_free(bp->stop_group);
    loc_free(bp->file);
    loc_free(bp->condition);
    loc_free(bp->client_data);
    while (bp->attrs != NULL) {
        BreakpointAttribute * attr = bp->attrs;
        bp->attrs = attr->next;
        loc_free(attr->name);
        loc_free(attr->value);
        loc_free(attr);
    }
    loc_free(bp);
}

static void remove_ref(BreakpointRef * br);
static void send_event_context_removed(BreakpointInfo * bp);

static void notify_breakpoint_status(BreakpointInfo * bp) {
    assert(generation_done == generation_posted);
#ifndef NDEBUG
    {
        /* Verify breakpoints data structure */
        LINK * m = NULL;
        int instruction_cnt = 0;
        int planted_as_sw_cnt = 0;
        int planted_cnt = 0;
        for (m = instructions.next; m != &instructions; m = m->next) {
            unsigned i;
            BreakInstruction * bi = link_all2bi(m);
            assert(bi->valid);
            assert(bi->ref_cnt <= bi->ref_size);
            assert(bi->cb.ctx->ref_count > 0);
            if (bi->planted && !bi->virtual_addr) planted_cnt++;
            for (i = 0; i < bi->ref_cnt; i++) {
                assert(bi->refs[i].cnt > 0);
                assert(bi->refs[i].ctx->ref_count > 0);
                assert(!bi->refs[i].ctx->exited);
                assert(!bi->cb.ctx->exited);
                if (bi->virtual_addr || bi->address_error || bi->no_addr) {
                    assert(bi->refs[i].ctx == bi->cb.ctx);
                }
                if (bi->refs[i].bp == bp) {
                    instruction_cnt++;
                    if (bi->planted_as_sw_bp) {
                        assert(bi->virtual_addr);
                        planted_as_sw_cnt++;
                    }
                    assert(id2ctx(bi->refs[i].ctx->id) == NULL ||
                        check_context_ids_location(bp, bi->refs[i].ctx));
                }
            }
        }
        assert(bp->enabled || instruction_cnt == 0);
        assert(bp->instruction_cnt == instruction_cnt);
        assert(planted_as_sw_cnt == 0 || planted_as_sw_cnt < instruction_cnt);
        assert(planted_sw_bp_cnt == planted_cnt);
        if (*bp->id) {
            int i;
            int client_cnt = 0;
            for (i = 0; i < INP2BR_HASH_SIZE; i++) {
                for (m = inp2br[i].next; m != &inp2br[i]; m = m->next) {
                    BreakpointRef * br = link_inp2br(m);
                    if (br->bp == bp) client_cnt++;
                }
            }
            assert(bp->client_cnt == client_cnt);
        }
        else {
            assert(list_is_empty(&bp->link_clients));
        }
    }
#endif
    if (bp->client_cnt == 0) {
        if (bp->instruction_cnt == 0) {
            if (*bp->id) send_event_context_removed(bp);
            free_bp(bp);
        }
    }
    else if (bp->status_changed) {
        if (*bp->id) send_event_breakpoint_status(NULL, bp);
        bp->status_changed = 0;
    }
}

static void done_replanting_breakpoints(void) {
    LINK * l = NULL;
    assert(list_is_empty(&evaluations_posted));
    assert(list_is_empty(&evaluations_active));
    assert(generation_done == generation_active);
    for (l = breakpoints.next; l != &breakpoints;) {
        BreakpointInfo * bp = link_all2bp(l);
        l = l->next;
        bp->attrs_changed = 0;
        notify_breakpoint_status(bp);
    }
}

static void done_condition_evaluation(EvaluationRequest * req) {
    unsigned i;

    for (i = 0; i < req->bp_cnt; i++) {
        Context * ctx = req->bp_arr[i].ctx;
        BreakpointInfo * bp = req->bp_arr[i].bp;
        if (ctx->exited || ctx->exiting) continue;
        assert(ctx->stopped);
        if (!req->bp_arr[i].condition_ok) continue;
        if (inc_bp_hit_count(bp, req->ctx) <= bp->ignore_count) continue;
        if (bp->event_callback != NULL) {
            bp->event_callback(ctx, bp->event_callback_args);
        }
        else {
            assert(bp->id[0] != 0);
            req->bp_arr[i].triggered = 1;
            if (bp->stop_group == NULL) suspend_by_bp(ctx, ctx, bp->id, bp->skip_prologue);
        }
    }
}

static void done_all_evaluations(void) {
    LINK * l;

    l = evaluations_active.next;
    while (l != &evaluations_active) {
        EvaluationRequest * req = link_active2erl(l);
        l = l->next;
        done_condition_evaluation(req);
    }

    l = evaluations_active.next;
    while (l != &evaluations_active) {
        EvaluationRequest * req = link_active2erl(l);
        unsigned i;

        l = l->next;

        for (i = 0; i < req->bp_cnt; i++) {
            Context * ctx = req->bp_arr[i].ctx;
            if (req->bp_arr[i].triggered) {
                BreakpointInfo * bp = req->bp_arr[i].bp;
                if (bp->stop_group != NULL) {
                    /* Intercept contexts in BP stop group */
                    char ** ids = bp->stop_group;
                    while (*ids) {
                        Context * c = id2ctx(*ids++);
                        if (c == NULL) continue;
                        suspend_by_bp(c, ctx, bp->id, bp->skip_prologue);
                    }
                }
                if (bp->temporary) {
                    LINK * m = bp->link_clients.next;
                    while (m != &bp->link_clients) {
                        BreakpointRef * br = link_bp2br(m);
                        m = m->next;
                        assert(br->bp == bp);
                        remove_ref(br);
                    }
                }
            }
            context_unlock(ctx);
        }

        req->bp_cnt = 0;
        list_remove(&req->link_active);
        context_unlock(req->ctx);
    }
}

#if ENABLE_SkipPrologueWhenPlanting

static void function_prolog_line_info(CodeArea * area, void * args) {
    CodeArea * res = (CodeArea *)args;
    if (res->file != NULL) return;
    *res = *area;
}

static int skip_function_prologue(Context * ctx, Symbol * sym, ContextAddress * addr) {
#if ENABLE_Symbols
    int sym_class = SYM_CLASS_UNKNOWN;
    ContextAddress sym_size = 0;
    CodeArea area;

    if (get_symbol_class(sym, &sym_class) < 0) return -1;
    if (sym_class != SYM_CLASS_FUNCTION) return 0;
    if (get_symbol_size(sym, &sym_size) < 0) sym_size = 0;
    memset(&area, 0, sizeof(area));
    if (address_to_line(ctx, *addr, *addr + 1, function_prolog_line_info, &area) < 0) return -1;
    if (area.start_address > *addr || area.end_address <= *addr) return 0;
    if (sym_size != 0 && *addr + sym_size <= area.end_address) return 0;
    *addr = area.end_address;
#endif
    return 0;
}

#endif /* ENABLE_SkipPrologueWhenPlanting */

static void plant_at_address_expression(Context * ctx, ContextAddress ip, BreakpointInfo * bp) {
    ContextAddress addr = 0;
    ContextAddress size = 1;
    int error = 0;
#if ENABLE_Expressions
    Value v;

    memset (&v, 0, sizeof (Value));

    if (evaluate_expression(ctx, STACK_NO_FRAME, ip, bp->location, 1, &v) < 0) error = errno;
    if (!error && value_to_address(&v, &addr) < 0) error = errno;
#if ENABLE_Symbols
    if (!error && v.sym != NULL) {
        SymbolProperties props;
        /* We want to add the LocalEntryOffset */
        get_symbol_props(v.sym, &props);
        /* If the symbol is not a PPC64 function, offset should be 0, so it is safe to add */
        addr += props.local_entry_offset;
    }
#endif
#endif
#if ENABLE_SkipPrologueWhenPlanting
    /* Even if addr is incremented by local_entry_offset, we still should be on right code area */
    if (!error && bp->skip_prologue && v.sym != NULL && skip_function_prologue(ctx, v.sym, &addr) < 0) error = errno;
#endif
    if (bp->access_size > 0) {
        size = bp->access_size;
    }
    else if (bp->access_mode & (CTX_BP_ACCESS_DATA_READ | CTX_BP_ACCESS_DATA_WRITE)) {
        size = context_word_size(ctx);
#if ENABLE_Symbols
        if (v.type != NULL) {
            Symbol * type = v.type;
            int type_class = 0;
            if (!error && get_symbol_type_class(type, &type_class) < 0) error = errno;
            if (!error && type_class == TYPE_CLASS_POINTER) {
                Symbol * base_type = NULL;
                if (!error && get_symbol_base_type(type, &base_type) < 0) error = errno;
                if (!error && base_type != NULL && get_symbol_size(base_type, &size) < 0) error = errno;
            }
        }
#endif
    }
    if (error) {
        address_expression_error(ctx, bp, error);
    }
    else {
        plant_breakpoint(ctx, bp, addr, size);
#if ENABLE_Symbols
        /* If the expression returns multiple symbols, plant multiple breakpoints */
        if (v.sym_list != NULL) {
            unsigned n = 0;
            while (v.sym_list[n] != NULL) {
                Symbol * sym = v.sym_list[n++];
                if (get_symbol_address(sym, &addr) == 0) {
                    SymbolProperties props;
                    /* We want to add the LocalEntryOffset */
                    get_symbol_props(sym, &props);
                    /* If the symbol is not a PPC64 function, offset should be 0, so it is safe to add */
                    addr += props.local_entry_offset;
#if ENABLE_SkipPrologueWhenPlanting
                    /* Even if addr is incremented by local_entry_offset, we still should be on right code area */
                    if (bp->skip_prologue) skip_function_prologue(ctx, sym, &addr);
#endif
                    plant_breakpoint(ctx, bp, addr, size);
                }
            }
        }
#endif
    }
}

#if ENABLE_LineNumbers
static void plant_breakpoint_address_iterator(CodeArea * area, void * x) {
    EvaluationArgs * args = (EvaluationArgs *)x;
    bp_line_cnt++;
    if (args->bp->location == NULL) {
        ContextAddress addr = area->start_address;
        if (addr == area->end_address || area->start_line != args->bp->line) {
            if (area->next_stmt_address != 0) addr = area->next_stmt_address;
            else if (area->next_address != 0) addr = area->next_address;
        }
#ifdef  BREAKPOINT_ADDR_ADJUST
        /* Adjust breakpoint address <addr> if needed */
        BREAKPOINT_ADDR_ADJUST;
#endif
        if ((addr == area->start_address && area->is_statement) || addr == area->next_stmt_address) {
            plant_breakpoint(args->ctx, args->bp, addr, 1);
            bp_stmt_cnt++;
        }
        else {
            if (bp_line_addr_cnt >= bp_line_addr_max) {
                bp_line_addr_max += 256;
                bp_line_addr = (ContextAddress *)tmp_realloc(bp_line_addr, sizeof(ContextAddress) * bp_line_addr_max);
            }
            bp_line_addr[bp_line_addr_cnt++] = addr;
        }
    }
    else {
        plant_at_address_expression(args->ctx, area->start_address, args->bp);
    }
}
#endif

static int check_context_ids_location(BreakpointInfo * bp, Context * ctx) {
    /* Check context IDs attribute and return 1 if the breakpoint should be planted in 'ctx' */
    assert(ctx == context_get_group(ctx, CONTEXT_GROUP_BREAKPOINT));
    if (bp->ctx != NULL) {
        if (context_get_group(bp->ctx, CONTEXT_GROUP_BREAKPOINT) != ctx) return 0;
    }
    if (bp->context_ids != NULL) {
        int ok = 0;
        char ** ids = bp->context_ids;
        while (!ok && *ids != NULL) {
            Context * c = id2ctx(*ids++);
            if (c == NULL) continue;
            ok = context_get_group(c, CONTEXT_GROUP_BREAKPOINT) == ctx;
        }
        if (!ok) return 0;
    }
    if (bp->context_names != NULL) {
        int ok = 0;
        char ** names = bp->context_names;
        while (!ok && *names != NULL) {
            char * name = *names++;
            LINK * l = context_root.next;
            while (!ok && l != &context_root) {
                Context * c = ctxl2ctxp(l);
                l = l->next;
                if (c->exited) continue;
                if (c->name == NULL) continue;
                if (context_get_group(c, CONTEXT_GROUP_BREAKPOINT) != ctx) continue;
                ok = strcmp(c->name, name) == 0;
            }
        }
        if (!ok) return 0;
    }
    if (bp->context_query != NULL) {
        int ok = 0;
        LINK * l = context_root.next;
        if (parse_context_query(bp->context_query) < 0) {
            bp_location_error = errno;
            return 0;
        }
        while (!ok && l != &context_root) {
            Context * c = ctxl2ctxp(l);
            l = l->next;
            if (c->exited) continue;
            if (context_get_group(c, CONTEXT_GROUP_BREAKPOINT) != ctx) continue;
            ok = run_context_query(c);
        }
        if (!ok) return 0;
    }
    return 1;
}

static int check_context_ids_condition(BreakpointInfo * bp, Context * ctx) {
    /* Check context IDs attribute and return 1 if the breakpoint should be triggered by 'ctx' */
    assert(context_has_state(ctx));
    if (bp->ctx != NULL) {
        if (bp->ctx != ctx && bp->ctx != context_get_group(ctx, CONTEXT_GROUP_PROCESS)) return 0;
    }
    if (bp->context_ids != NULL) {
        int ok = 0;
        char ** ids = bp->context_ids;
        Context * prs = context_get_group(ctx, CONTEXT_GROUP_PROCESS);
        while (!ok && *ids != NULL) {
            char * id = *ids++;
            ok = strcmp(id, ctx->id) == 0 || (prs && strcmp(id, prs->id) == 0);
        }
        if (!ok) return 0;
    }
    if (bp->context_names != NULL) {
        int ok = 0;
        if (ctx->name) {
            char * name = ctx->name;
            char ** names = bp->context_names;
            while (!ok && *names != NULL) {
                ok = strcmp(name, *names++) == 0;
            }
        }
        if (!ok) {
            Context * prs = context_get_group(ctx, CONTEXT_GROUP_PROCESS);
            if (prs && prs->name) {
                char * name = prs->name;
                char ** names = bp->context_names;
                while (!ok && *names != NULL) {
                    ok = strcmp(name, *names++) == 0;
                }
            }
        }
        if (!ok) return 0;
    }
    if (bp->context_query != NULL) {
        parse_context_query(bp->context_query);
        if (!run_context_query(ctx)) return 0;
    }
    return 1;
}

static void evaluate_condition(void * x) {
    EvaluationArgs * args = (EvaluationArgs *)x;
    EvaluationRequest * req = EXT(args->ctx)->req;
    ConditionEvaluationRequest * ce = req->bp_arr + args->index;
    BreakpointInfo * bp = ce->bp;
    BreakInstruction * bi = ce->bi;
    ErrorReport * condition_error = NULL;

    assert(req != NULL);
    assert(req->bp_cnt > 0);
    assert(args->index < req->bp_cnt);
    assert(cache_enter_cnt > 0);
    assert(args->bp == ce->bp);

    if (!is_disabled(bp)) {
        Context * ctx = ce->ctx;
        assert(ctx->stopped);
        assert(ctx->stopped_by_bp || ctx->stopped_by_cb);

        if (check_context_ids_condition(bp, ctx)) {
            if (bp->condition != NULL) {
#if ENABLE_Expressions
                Value v;
                int b = 0;
                if (evaluate_expression(ctx, STACK_TOP_FRAME, 0, bp->condition, 1, &v) < 0 ||
                        (v.size > 0 && value_to_boolean(&v, &b) < 0)) {
                    int error = errno;
                    Channel * c = cache_channel();
                    if (c == NULL || !is_channel_closed(c)) {
                        condition_error = get_error_report(error);
                        ce->condition_ok = 1;
                    }
                }
                else if (b) {
                    ce->condition_ok = 1;
                }
#endif
            }
            else {
                ce->condition_ok = 1;
            }
        }
    }
    if (cache_miss_count() > 0 || compare_error_reports(bi->condition_error, condition_error)) {
        release_error_report(condition_error);
    }
    else {
        bp->status_changed = 1;
        release_error_report(bi->condition_error);
        bi->condition_error = condition_error;
    }
}

static void evaluate_bp_location(void * x) {
    EvaluationArgs * args = (EvaluationArgs *)x;
    BreakpointInfo * bp = args->bp;
    Context * ctx = args->ctx;

    assert(cache_enter_cnt > 0);
    bp_location_error = 0;
    if (!ctx->exited && !ctx->exiting && !EXT(ctx)->empty_bp_grp && !is_disabled(bp) &&
            bp->error == NULL && check_context_ids_location(bp, ctx) && bp_location_error == 0) {
        if (bp->file != NULL) {
#if ENABLE_LineNumbers
            bp_line_cnt = 0;
            bp_stmt_cnt = 0;
            bp_line_addr_cnt = 0;
            bp_line_addr_max = 0;
            bp_line_addr = NULL;
            if (line_to_address(ctx, bp->file, bp->line, bp->column, plant_breakpoint_address_iterator, args) < 0) {
                bp_location_error = errno;
            }
            else if (bp_line_cnt == 0) {
                bp_location_error = set_errno(ERR_OTHER, "Unresolved source line information");
            }
            else if (bp_stmt_cnt == 0) {
                unsigned i;
                for (i = 0; i < bp_line_addr_cnt; i++) {
                    plant_breakpoint(ctx, bp, bp_line_addr[i], 1);
                }
            }
#else
            bp_location_error = set_errno(ERR_UNSUPPORTED, "LineNumbers service not available");
#endif
        }
        else if (bp->location != NULL) {
            plant_at_address_expression(ctx, 0, bp);
        }
        else {
            link_breakpoint_instruction(bp, ctx, 0, bp->access_size, NULL, 0, 0, NULL);
        }
    }
    if (bp_location_error) address_expression_error(ctx, bp, bp_location_error);
}

static void validate_bp_attrs(void) {
    LINK * l = breakpoints.next;
    while (l != &breakpoints) {
        BreakpointInfo * bp = link_all2bp(l);
        if (bp->instruction_cnt == 0) {
            ErrorReport * error = NULL;
            if (bp->inv_attr != NULL) {
                error = get_error_report(set_fmt_errno(ERR_OTHER, "Invalid value of '%s'", bp->inv_attr->name));
            }
            else if (bp->context_query != NULL) {
                if (parse_context_query(bp->context_query) < 0) {
                    error = get_error_report(errno);
                }
            }
            if (compare_error_reports(bp->error, error)) {
                release_error_report(error);
            }
            else {
                bp->status_changed = 1;
                release_error_report(bp->error);
                bp->error = error;
            }
        }
        l = l->next;
    }
}

static void replant_breakpoints_cache_client(void * args) {
    EvaluationRequest * req = *(EvaluationRequest **)args;
    Context * ctx = req->ctx;
    unsigned i;
    LINK * l;

    assert(!list_is_empty(&req->link_active));
    l = evaluations_active.next;
    while (l != &evaluations_active) {
        EvaluationRequest * r = link_active2erl(l);
        check_all_stopped(r->ctx);
        l = l->next;
    }

    if (req->loc_active.bp_cnt > LOC_EVALUATION_BP_MAX) {
        clear_instruction_refs(ctx, NULL);
        if (!ctx->exiting && !ctx->exited && !EXT(ctx)->empty_bp_grp) {
            l = breakpoints.next;
            while (l != &breakpoints) {
                run_bp_evaluation(evaluate_bp_location, link_all2bp(l), ctx, -1);
                l = l->next;
            }
        }
    }
    else if (req->loc_active.bp_cnt > 0) {
        for (i = 0; i < req->loc_active.bp_cnt; i++) {
            BreakpointInfo * bp = req->loc_active.bp_arr[i];
            clear_instruction_refs(ctx, bp);
            if (!ctx->exiting && !ctx->exited && !EXT(ctx)->empty_bp_grp) {
                run_bp_evaluation(evaluate_bp_location, bp, ctx, -1);
            }
        }
    }

    if (req->bp_cnt > 0) {
        for (i = 0; i < req->bp_cnt; i++) {
            ConditionEvaluationRequest * r = req->bp_arr + i;
            r->condition_ok = 0;
            if (is_disabled(r->bp)) continue;
            run_bp_evaluation(evaluate_condition, r->bp, ctx, i);
        }
    }

    get_bp_opcodes();

    cache_exit();

    cache_enter_cnt--;
    assert(cache_enter_cnt >= 0);
    if (cache_enter_cnt == 0) {
        done_all_evaluations();

        assert(list_is_empty(&evaluations_active));
        if (list_is_empty(&evaluations_posted)) {
            flush_instructions();
        }

        if (!list_is_empty(&evaluations_posted)) {
            post_event(event_replant_breakpoints, (void *)++generation_posted);
        }
        else {
            assert(generation_done != generation_active);
            generation_done = generation_active;
            assert(generation_posted == generation_done);
            done_replanting_breakpoints();
            run_ctrl_unlock();
        }
    }
}

static void event_replant_breakpoints(void * arg) {
    LINK * q;

    assert(!list_is_empty(&evaluations_posted));
    if ((uintptr_t)arg != generation_posted) return;
    if (cache_enter_cnt > 0) return;

    validate_bp_attrs();

    generation_active = generation_posted;
    while (!list_is_empty(&evaluations_posted)) {
        EvaluationRequest * req = link_posted2erl(evaluations_posted.next);
        req->loc_active = req->loc_posted;
        memset(&req->loc_posted, 0, sizeof(LocationEvaluationRequest));
        assert(list_is_empty(&req->link_active));
        list_add_last(&req->link_active, &evaluations_active);
        list_remove(&req->link_posted);
        cache_enter_cnt++;
    }

    q = evaluations_active.next;
    while (q != &evaluations_active) {
        EvaluationRequest * req = link_active2erl(q);
        q = q->next;
        cache_enter(replant_breakpoints_cache_client, NULL, &req, sizeof(req));
    }
}

static void replant_breakpoint(BreakpointInfo * bp) {
    int check_intsructions = 0;
    if (bp->client_cnt == 0) {
        check_intsructions = 1;
    }
    else if (bp->ctx != NULL) {
        post_location_evaluation_request(bp->ctx, bp);
    }
    else if (bp->context_ids) {
        char ** ids = bp->context_ids;
        while (*ids != NULL) {
            Context * ctx = id2ctx(*ids++);
            if (ctx == NULL) continue;
            if (ctx->exited) continue;
            post_location_evaluation_request(ctx, bp);
        }
        check_intsructions = 1;
    }
    else if (bp->context_names) {
        char ** names = bp->context_names;
        while (*names != NULL) {
            char * name = *names++;
            LINK * l = context_root.next;
            while (l != &context_root) {
                Context * ctx = ctxl2ctxp(l);
                l = l->next;
                if (ctx->exited) continue;
                if (ctx->name == NULL) continue;
                if (strcmp(ctx->name, name)) continue;
                post_location_evaluation_request(ctx, bp);
            }
        }
        check_intsructions = 1;
    }
    else if (bp->context_query && parse_context_query(bp->context_query) == 0) {
        LINK * l = context_root.next;
        while (l != &context_root) {
            Context * ctx = ctxl2ctxp(l);
            l = l->next;
            if (ctx->exited) continue;
            if (!run_context_query(ctx)) continue;
            post_location_evaluation_request(ctx, bp);
        }
        check_intsructions = 1;
    }
    else {
        LINK * l = context_root.next;
        while (l != &context_root) {
            Context * ctx = ctxl2ctxp(l);
            l = l->next;
            if (ctx->exited) continue;
            post_location_evaluation_request(ctx, bp);
        }
    }
    if (check_intsructions && bp->instruction_cnt > 0) {
        LINK * l = instructions.next;
        while (l != &instructions) {
            unsigned i;
            BreakInstruction * bi = link_all2bi(l);
            for (i = 0; i < bi->ref_cnt; i++) {
                InstructionRef * ref = bi->refs + i;
                if (ref->bp != bp) continue;
                if (ref->ctx->exited) continue;
                /* Check for fork child that is going to be detached */
                if (id2ctx(ref->ctx->id) == NULL) continue;
                post_location_evaluation_request(ref->ctx, bp);
            }
            l = l->next;
        }
    }
}

static BreakpointInfo * find_breakpoint(const char * id) {
    int hash = id2bp_hash(id);
    LINK * l = id2bp[hash].next;
    while (l != id2bp + hash) {
        BreakpointInfo * bp = link_id2bp(l);
        l = l->next;
        if (strcmp(bp->id, id) == 0) return bp;
    }
    return NULL;
}

static BreakpointRef * find_breakpoint_ref(BreakpointInfo * bp, Channel * channel) {
    LINK * l;
    if (bp == NULL) return NULL;
    l = bp->link_clients.next;
    while (l != &bp->link_clients) {
        BreakpointRef * br = link_bp2br(l);
        assert(br->bp == bp);
        if (br->channel == channel) return br;
        l = l->next;
    }
    return NULL;
}

static void read_breakpoint_property(InputStream * inp, const char * name, void * args) {
    BreakpointAttribute *** p = (BreakpointAttribute ***)args;
    BreakpointAttribute * attr = (BreakpointAttribute *)loc_alloc_zero(sizeof(BreakpointAttribute));
    attr->name = loc_strdup(name);
    attr->value = json_read_object(inp);
    **p = attr;
    *p = &attr->next;
}

static BreakpointAttribute * read_breakpoint_properties(InputStream * inp) {
    BreakpointAttribute * attrs = NULL;
    BreakpointAttribute ** p = &attrs;
    json_read_struct(inp, read_breakpoint_property, &p);
    return attrs;
}

static void read_id_attribute(BreakpointAttribute * attrs, char * id, size_t id_size) {
    while (attrs != NULL) {
        if (strcmp(attrs->name, BREAKPOINT_ID) == 0) {
            ByteArrayInputStream buf;
            InputStream * inp = create_byte_array_input_stream(&buf, attrs->value, strlen(attrs->value));
            json_read_string(inp, id, id_size);
            json_test_char(inp, MARKER_EOS);
            return;
        }
        attrs = attrs->next;
    }
    str_exception(ERR_OTHER, "Breakpoint must have an ID");
}

static void set_breakpoint_attribute(BreakpointInfo * bp, const char * name, const char * value) {
    BreakpointAttribute * attr = bp->attrs;
    BreakpointAttribute ** ref = &bp->attrs;

    while (attr != NULL) {
        if (strcmp(attr->name, name) == 0) {
            loc_free(attr->value);
            attr->value = loc_strdup(value);
            return;
        }
        ref = &attr->next;
        attr = attr->next;
    }
    attr = (BreakpointAttribute *)loc_alloc_zero(sizeof(BreakpointAttribute));
    attr->name = loc_strdup(name);
    attr->value = loc_strdup(value);
    *ref = attr;
}

static int set_breakpoint_attributes(BreakpointInfo * bp, BreakpointAttribute * new_attrs) {
    int diff = 0;
    BreakpointAttribute * old_attrs = bp->attrs;
    BreakpointAttribute ** new_ref = &bp->attrs;
    bp->inv_attr = NULL;
    bp->attrs = NULL;

    while (new_attrs != NULL) {
        BreakpointAttribute * new_attr = new_attrs;
        BreakpointAttribute * old_attr = old_attrs;
        BreakpointAttribute ** old_ref = &old_attrs;
        InputStream * buf_inp = NULL;
        ByteArrayInputStream buf;
        int unsupported_attr = 0;
        char * name = new_attr->name;
        Trap trap;

        new_attrs = new_attr->next;
        new_attr->next = NULL;
        while (old_attr && strcmp(old_attr->name, name)) {
            old_ref = &old_attr->next;
            old_attr = old_attr->next;
        }

        if (old_attr != NULL) {
            assert(old_attr == *old_ref);
            *old_ref = old_attr->next;
            old_attr->next = NULL;
            if (strcmp(old_attr->value, new_attr->value) == 0) {
                *new_ref = old_attr;
                new_ref = &old_attr->next;
                loc_free(new_attr->value);
                loc_free(new_attr->name);
                loc_free(new_attr);
                continue;
            }
            loc_free(old_attr->value);
            loc_free(old_attr->name);
            loc_free(old_attr);
            old_attr = NULL;
        }
        diff++;

        *new_ref = new_attr;
        new_ref = &new_attr->next;

        buf_inp = create_byte_array_input_stream(&buf, new_attr->value, strlen(new_attr->value));

        if (set_trap(&trap)) {
            if (strcmp(name, BREAKPOINT_ID) == 0) {
                json_read_string(buf_inp, bp->id, sizeof(bp->id));
            }
            else if (strcmp(name, BREAKPOINT_TYPE) == 0) {
                loc_free(bp->type);
                bp->type = json_read_alloc_string(buf_inp);
            }
            else if (strcmp(name, BREAKPOINT_LOCATION) == 0) {
                loc_free(bp->location);
                bp->location = json_read_alloc_string(buf_inp);
            }
            else if (strcmp(name, BREAKPOINT_ACCESSMODE) == 0) {
                bp->access_mode = json_read_long(buf_inp);
            }
            else if (strcmp(name, BREAKPOINT_SIZE) == 0) {
                bp->access_size = json_read_long(buf_inp);
            }
            else if (strcmp(name, BREAKPOINT_CONDITION) == 0) {
                loc_free(bp->condition);
                bp->condition = json_read_alloc_string(buf_inp);
            }
            else if (strcmp(name, BREAKPOINT_CONTEXTIDS) == 0) {
                loc_free(bp->context_ids);
                bp->context_ids = json_read_alloc_string_array(buf_inp, NULL);
            }
            else if (strcmp(name, BREAKPOINT_CONTEXTNAMES) == 0) {
                loc_free(bp->context_names);
                bp->context_names = json_read_alloc_string_array(buf_inp, NULL);
            }
            else if (strcmp(name, BREAKPOINT_CONTEXT_QUERY) == 0) {
                loc_free(bp->context_query);
                bp->context_query = json_read_alloc_string(buf_inp);
            }
            else if (strcmp(name, BREAKPOINT_STOP_GROUP) == 0) {
                loc_free(bp->stop_group);
                bp->stop_group = json_read_alloc_string_array(buf_inp, NULL);
            }
            else if (strcmp(name, BREAKPOINT_TEMPORARY) == 0) {
                bp->temporary = json_read_boolean(buf_inp);
            }
            else if (strcmp(name, BREAKPOINT_SKIP_PROLOGUE) == 0) {
                bp->skip_prologue = json_read_boolean(buf_inp);
            }
            else if (strcmp(name, BREAKPOINT_LINE_OFFSET) == 0) {
                bp->line_offs_limit = json_read_long(buf_inp);
                bp->line_offs_check = 1;
            }
            else if (strcmp(name, BREAKPOINT_FILE) == 0) {
                loc_free(bp->file);
                bp->file = json_read_alloc_string(buf_inp);
            }
            else if (strcmp(name, BREAKPOINT_LINE) == 0) {
                bp->line = json_read_long(buf_inp);
            }
            else if (strcmp(name, BREAKPOINT_COLUMN) == 0) {
                bp->column = json_read_long(buf_inp);
            }
            else if (strcmp(name, BREAKPOINT_IGNORECOUNT) == 0) {
                bp->ignore_count = json_read_ulong(buf_inp);
            }
            else if (strcmp(name, BREAKPOINT_ENABLED) == 0) {
                bp->enabled = json_read_boolean(buf_inp);
            }
            else {
                unsupported_attr = 1;
            }

            if (!unsupported_attr) json_test_char(buf_inp, MARKER_EOS);
            clear_trap(&trap);
        }
        else if (bp->inv_attr == NULL) {
            bp->inv_attr = new_attr;
        }
    }

    while (old_attrs != NULL) {
        BreakpointAttribute * old_attr = old_attrs;
        char * name = old_attr->name;
        old_attrs = old_attr->next;

        if (strcmp(name, BREAKPOINT_ID) == 0) {
            bp->id[0] = 0;
        }
        else if (strcmp(name, BREAKPOINT_TYPE) == 0) {
            loc_free(bp->type);
            bp->type = NULL;
        }
        else if (strcmp(name, BREAKPOINT_LOCATION) == 0) {
            loc_free(bp->location);
            bp->location = NULL;
        }
        else if (strcmp(name, BREAKPOINT_ACCESSMODE) == 0) {
            bp->access_mode = 0;
        }
        else if (strcmp(name, BREAKPOINT_SIZE) == 0) {
            bp->access_size = 0;
        }
        else if (strcmp(name, BREAKPOINT_CONDITION) == 0) {
            loc_free(bp->condition);
            bp->condition = NULL;
        }
        else if (strcmp(name, BREAKPOINT_CONTEXTIDS) == 0) {
            loc_free(bp->context_ids);
            bp->context_ids = NULL;
        }
        else if (strcmp(name, BREAKPOINT_CONTEXTNAMES) == 0) {
            loc_free(bp->context_names);
            bp->context_names = NULL;
        }
        else if (strcmp(name, BREAKPOINT_CONTEXT_QUERY) == 0) {
            loc_free(bp->context_query);
            bp->context_query = NULL;
        }
        else if (strcmp(name, BREAKPOINT_STOP_GROUP) == 0) {
            loc_free(bp->stop_group);
            bp->stop_group = NULL;
        }
        else if (strcmp(name, BREAKPOINT_TEMPORARY) == 0) {
            bp->temporary = 0;
        }
        else if (strcmp(name, BREAKPOINT_SKIP_PROLOGUE) == 0) {
            bp->skip_prologue = 0;
        }
        else if (strcmp(name, BREAKPOINT_LINE_OFFSET) == 0) {
            bp->line_offs_limit = 0;
            bp->line_offs_check = 0;
        }
        else if (strcmp(name, BREAKPOINT_FILE) == 0) {
            loc_free(bp->file);
            bp->file = NULL;
        }
        else if (strcmp(name, BREAKPOINT_LINE) == 0) {
            bp->line = 0;
        }
        else if (strcmp(name, BREAKPOINT_COLUMN) == 0) {
            bp->column = 0;
        }
        else if (strcmp(name, BREAKPOINT_IGNORECOUNT) == 0) {
            bp->ignore_count = 0;
        }
        else if (strcmp(name, BREAKPOINT_ENABLED) == 0) {
            bp->enabled = 0;
        }

        loc_free(old_attr->value);
        loc_free(old_attr->name);
        loc_free(old_attr);
        diff++;
    }

    return diff;
}

static void write_breakpoint_properties(OutputStream * out, BreakpointInfo * bp) {
    int cnt = 0;
    BreakpointAttribute * attr = bp->attrs;

    write_stream(out, '{');

    while (attr != NULL) {
        if (cnt > 0) write_stream(out, ',');
        json_write_string(out, attr->name);
        write_stream(out, ':');
        write_string(out, attr->value);
        attr = attr->next;
        cnt++;
    }

    write_stream(out, '}');
}

static void send_event_context_added(Channel * channel, BreakpointInfo * bp) {
    OutputStream * out = channel ? &channel->out : &broadcast_group->out;
    unsigned i;

    assert(bp->id[0] != 0);
    write_stringz(out, "E");
    write_stringz(out, BREAKPOINTS);
    write_stringz(out, "contextAdded");

    write_stream(out, '[');
    write_breakpoint_properties(out, bp);
    write_stream(out, ']');
    write_stream(out, 0);
    write_stream(out, MARKER_EOM);
    if (channel) return;

    for (i = 0; i < listener_cnt; i++) {
        Listener * l = listeners + i;
        if (l->listener->breakpoint_created == NULL) continue;
        l->listener->breakpoint_created(bp, l->args);
    }
}

static void send_event_context_changed(BreakpointInfo * bp) {
    OutputStream * out = &broadcast_group->out;
    unsigned i;

    assert(bp->id[0] != 0);
    write_stringz(out, "E");
    write_stringz(out, BREAKPOINTS);
    write_stringz(out, "contextChanged");

    write_stream(out, '[');
    write_breakpoint_properties(out, bp);
    write_stream(out, ']');
    write_stream(out, 0);
    write_stream(out, MARKER_EOM);

    for (i = 0; i < listener_cnt; i++) {
        Listener * l = listeners + i;
        if (l->listener->breakpoint_changed == NULL) continue;
        l->listener->breakpoint_changed(bp, l->args);
    }
}

static void send_event_context_removed(BreakpointInfo * bp) {
    OutputStream * out = &broadcast_group->out;
    unsigned i;

    write_stringz(out, "E");
    write_stringz(out, BREAKPOINTS);
    write_stringz(out, "contextRemoved");

    write_stream(out, '[');
    json_write_string(out, bp->id);
    write_stream(out, ']');
    write_stream(out, 0);
    write_stream(out, MARKER_EOM);

    for (i = 0; i < listener_cnt; i++) {
        Listener * l = listeners + i;
        if (l->listener->breakpoint_deleted == NULL) continue;
        l->listener->breakpoint_deleted(bp, l->args);
    }
}

static BreakpointInfo * add_breakpoint(Channel * c, BreakpointAttribute * attrs) {
    char id[256];
    BreakpointRef * r = NULL;
    BreakpointInfo * bp = NULL;
    int ref_added = 0;
    int added = 0;
    int chng = 0;

    read_id_attribute(attrs, id, sizeof(id));
    bp = find_breakpoint(id);
    if (bp == NULL) {
        int hash = id2bp_hash(id);
        bp = (BreakpointInfo *)loc_alloc_zero(sizeof(BreakpointInfo));
        list_init(&bp->link_clients);
        list_init(&bp->link_hit_count);
        list_add_last(&bp->link_all, &breakpoints);
        list_add_last(&bp->link_id, id2bp + hash);
        set_breakpoint_attributes(bp, attrs);
    }
    else {
        chng = set_breakpoint_attributes(bp, attrs);
        if (chng) bp->attrs_changed = 1;
    }
    if (list_is_empty(&bp->link_clients)) added = 1;
    else r = find_breakpoint_ref(bp, c);
    if (r == NULL) {
        unsigned inp_hash = (unsigned)(uintptr_t)c / 16 % INP2BR_HASH_SIZE;
        r = (BreakpointRef *)loc_alloc_zero(sizeof(BreakpointRef));
        list_add_last(&r->link_inp, inp2br + inp_hash);
        list_add_last(&r->link_bp, &bp->link_clients);
        r->channel = c;
        r->bp = bp;
        bp->client_cnt++;
        ref_added = 1;
    }
    assert(r->bp == bp);
    assert(!list_is_empty(&bp->link_clients));
    if (chng || added || ref_added) replant_breakpoint(bp);
    if (added) send_event_context_added(NULL, bp);
    else if (chng) send_event_context_changed(bp);
    return bp;
}

static void remove_ref(BreakpointRef * br) {
    BreakpointInfo * bp = br->bp;
    bp->client_cnt--;
    list_remove(&br->link_inp);
    list_remove(&br->link_bp);
    loc_free(br);
    replant_breakpoint(bp);
    if (list_is_empty(&bp->link_clients)) {
        assert(bp->client_cnt == 0);
        if (generation_done == generation_posted) notify_breakpoint_status(bp);
    }
}

static void delete_breakpoint_refs(Channel * c) {
    unsigned hash = (unsigned)(uintptr_t)c / 16 % INP2BR_HASH_SIZE;
    LINK * l = inp2br[hash].next;
    while (l != &inp2br[hash]) {
        BreakpointRef * br = link_inp2br(l);
        l = l->next;
        if (br->channel == c) remove_ref(br);
    }
}

static void command_set_array_cb(InputStream * inp, void * args) {
    add_breakpoint((Channel *)args, read_breakpoint_properties(inp));
}

static void command_set(char * token, Channel * c) {
    LINK * l = NULL;

    /* Delete all breakpoints of this channel */
    delete_breakpoint_refs(c);

    /* Report breakpoints from other channels */
    l = breakpoints.next;
    while (l != &breakpoints) {
        BreakpointInfo * bp = link_all2bp(l);
        l = l->next;
        if (list_is_empty(&bp->link_clients)) continue;
        assert(*bp->id);
        send_event_context_added(c, bp);
        send_event_breakpoint_status(c, bp);
    }

    /* Add breakpoints for this channel */
    json_read_array(&c->inp, command_set_array_cb, c);
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_errno(&c->out, 0);
    write_stream(&c->out, MARKER_EOM);
}

static void command_get_ids(char * token, Channel * c) {
    LINK * l = breakpoints.next;
    int cnt = 0;

    json_test_char(&c->inp, MARKER_EOM);
    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_errno(&c->out, 0);
    write_stream(&c->out, '[');

    while (l != &breakpoints) {
        BreakpointInfo * bp = link_all2bp(l);
        l = l->next;
        if (list_is_empty(&bp->link_clients)) continue;
        assert(*bp->id);
        if (cnt > 0) write_stream(&c->out, ',');
        json_write_string(&c->out, bp->id);
        cnt++;
    }

    write_stream(&c->out, ']');
    write_stream(&c->out, 0);
    write_stream(&c->out, MARKER_EOM);
}

static void command_get_properties(char * token, Channel * c) {
    char id[256];
    BreakpointInfo * bp = NULL;
    int err = 0;

    json_read_string(&c->inp, id, sizeof(id));
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    bp = find_breakpoint(id);
    if (bp == NULL || list_is_empty(&bp->link_clients)) err = ERR_INV_CONTEXT;

    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_errno(&c->out, err);
    if (err) {
        write_stringz(&c->out, "null");
    }
    else {
        write_breakpoint_properties(&c->out, bp);
        write_stream(&c->out, 0);
    }
    write_stream(&c->out, MARKER_EOM);
}

static void command_get_status(char * token, Channel * c) {
    char id[256];
    BreakpointInfo * bp = NULL;
    int err = 0;

    json_read_string(&c->inp, id, sizeof(id));
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    bp = find_breakpoint(id);
    if (bp == NULL || list_is_empty(&bp->link_clients)) err = ERR_INV_CONTEXT;

    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_errno(&c->out, err);
    if (err) {
        write_stringz(&c->out, "null");
    }
    else {
        write_breakpoint_status(&c->out, bp);
        write_stream(&c->out, 0);
    }
    write_stream(&c->out, MARKER_EOM);
}

static void command_add(char * token, Channel * c) {
    BreakpointAttribute * props = read_breakpoint_properties(&c->inp);
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    add_breakpoint(c, props);

    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_errno(&c->out, 0);
    write_stream(&c->out, MARKER_EOM);
}

static void command_change(char * token, Channel * c) {
    BreakpointAttribute * props = read_breakpoint_properties(&c->inp);
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    add_breakpoint(c, props);

    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_errno(&c->out, 0);
    write_stream(&c->out, MARKER_EOM);
}

static void command_enable_array_cb(InputStream * inp, void * args) {
    char id[256];
    BreakpointInfo * bp;
    json_read_string(inp, id, sizeof(id));
    bp = find_breakpoint(id);
    if (bp != NULL && !list_is_empty(&bp->link_clients) && !bp->enabled) {
        bp->enabled = 1;
        set_breakpoint_attribute(bp, BREAKPOINT_ENABLED, "true");
        replant_breakpoint(bp);
        send_event_context_changed(bp);
    }
}

static void command_enable(char * token, Channel * c) {
    json_read_array(&c->inp, command_enable_array_cb, NULL);
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_errno(&c->out, 0);
    write_stream(&c->out, MARKER_EOM);
}

static void command_disable_array_cb(InputStream * inp, void * args) {
    char id[256];
    BreakpointInfo * bp;
    json_read_string(inp, id, sizeof(id));
    bp = find_breakpoint(id);
    if (bp != NULL && !list_is_empty(&bp->link_clients) && bp->enabled) {
        bp->enabled = 0;
        set_breakpoint_attribute(bp, BREAKPOINT_ENABLED, "false");
        replant_breakpoint(bp);
        send_event_context_changed(bp);
    }
}

static void command_disable(char * token, Channel * c) {
    json_read_array(&c->inp, command_disable_array_cb, NULL);
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_errno(&c->out, 0);
    write_stream(&c->out, MARKER_EOM);
}

static void command_remove_array_cb(InputStream * inp, void * args) {
    char id[256];
    BreakpointRef * br;
    json_read_string(inp, id, sizeof(id));
    br = find_breakpoint_ref(find_breakpoint(id), (Channel *)args);
    if (br != NULL) remove_ref(br);
}

static void command_remove(char * token, Channel * c) {
    json_read_array(&c->inp, command_remove_array_cb, c);
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_errno(&c->out, 0);
    write_stream(&c->out, MARKER_EOM);
}

static void command_get_capabilities(char * token, Channel * c) {
    char id[256];
    Context * ctx;
    OutputStream * out = &c->out;
    int err = 0;

    json_read_string(&c->inp, id, sizeof(id));
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    ctx = id2ctx(id);
    if (ctx == NULL && strlen(id) > 0) err = ERR_INV_CONTEXT;

    write_stringz(out, "R");
    write_stringz(out, token);
    write_errno(out, err);
    if (err) {
        write_stringz(&c->out, "null");
    }
    else {
        write_stream(out, '{');
        json_write_string(out, "ID");
        write_stream(out, ':');
        json_write_string(out, id);
        write_stream(out, ',');
        json_write_string(out, "BreakpointType");
        write_stream(out, ':');
        json_write_boolean(out, 1);
        write_stream(out, ',');
        json_write_string(out, "Location");
        write_stream(out, ':');
        json_write_boolean(out, 1);
        write_stream(out, ',');
        json_write_string(out, "FileLine");
        write_stream(out, ':');
        json_write_boolean(out, ENABLE_LineNumbers);
        write_stream(out, ',');
        json_write_string(out, "FileMapping");
        write_stream(out, ':');
        json_write_boolean(out, SERVICE_PathMap);
        write_stream(out, ',');
        json_write_string(out, "IgnoreCount");
        write_stream(out, ':');
        json_write_boolean(out, 1);
        write_stream(out, ',');
        json_write_string(out, "Condition");
        write_stream(out, ':');
        json_write_boolean(out, 1);
        if (ctx != NULL) {
            int md = CTX_BP_ACCESS_INSTRUCTION;
            md |= context_get_supported_bp_access_types(ctx);
            md &= ~CTX_BP_ACCESS_VIRTUAL;
            write_stream(out, ',');
            json_write_string(out, "AccessMode");
            write_stream(out, ':');
            json_write_long(out, md);
        }
        write_stream(out, ',');
        json_write_string(out, "ContextIds");
        write_stream(out, ':');
        json_write_boolean(out, 1);
        write_stream(out, ',');
        json_write_string(out, "ContextNames");
        write_stream(out, ':');
        json_write_boolean(out, 1);
#if SERVICE_ContextQuery
        write_stream(out, ',');
        json_write_string(out, "ContextQuery");
        write_stream(out, ':');
        json_write_boolean(out, 1);
#endif
        write_stream(out, ',');
        json_write_string(out, "StopGroup");
        write_stream(out, ':');
        json_write_boolean(out, 1);
        write_stream(out, ',');
        json_write_string(out, "ClientData");
        write_stream(out, ':');
        json_write_boolean(out, 1);
        write_stream(out, ',');
        json_write_string(out, "Temporary");
        write_stream(out, ':');
        json_write_boolean(out, 1);
        write_stream(out, ',');
        json_write_string(out, "SkipPrologue");
        write_stream(out, ':');
        json_write_boolean(out, 1);
        write_stream(out, ',');
        json_write_string(out, "LineOffset");
        write_stream(out, ':');
        json_write_boolean(out, 1);
#if ENABLE_ContextBreakpointCapabilities
        {
            /* Back-end context breakpoint capabilities */
            int cnt = 0;
            const char ** names = NULL;
            const char ** values = NULL;
            if (context_get_breakpoint_capabilities(ctx, &names, &values, &cnt) == 0) {
                while (cnt > 0) {
                    if (*values != NULL) {
                        write_stream(out, ',');
                        json_write_string(out, *names);
                        write_stream(out, ':');
                        write_string(out, *values);
                    }
                    names++;
                    values++;
                    cnt--;
                }
            }
        }
#endif
        write_stream(out, '}');
        write_stream(out, 0);
    }

    write_stream(out, MARKER_EOM);
}

void add_breakpoint_event_listener(BreakpointsEventListener * listener, void * args) {
    if (listener_cnt >= listener_max) {
        listener_max += 8;
        listeners = (Listener *)loc_realloc(listeners, listener_max * sizeof(Listener));
    }
    listeners[listener_cnt].listener = listener;
    listeners[listener_cnt].args = args;
    listener_cnt++;
}

void rem_breakpoint_event_listener(BreakpointsEventListener * listener) {
    unsigned i = 0;
    while (i < listener_cnt) {
        if (listeners[i++].listener == listener) {
            while (i < listener_cnt) {
                listeners[i - 1] = listeners[i];
                i++;
            }
            listener_cnt--;
            break;
        }
    }
}

void iterate_breakpoints(IterateBreakpointsCallBack * callback, void * args) {
    LINK * l = breakpoints.next;
    while (l != &breakpoints) {
        BreakpointInfo * bp = link_all2bp(l);
        l = l->next;
        callback(bp, args);
    }
}

BreakpointAttribute * get_breakpoint_attributes(BreakpointInfo * bp) {
    return bp->attrs;
}

BreakpointInfo * create_breakpoint(BreakpointAttribute * attrs) {
    return add_breakpoint(NULL, attrs);
}

void change_breakpoint_attributes(BreakpointInfo * bp, BreakpointAttribute * attrs) {
    int chng = set_breakpoint_attributes(bp, attrs);
    assert(!list_is_empty(&bp->link_clients));
    if (chng) {
        bp->attrs_changed = 1;
        replant_breakpoint(bp);
        if (bp->id[0] != 0) send_event_context_changed(bp);
    }
}

char * get_breakpoint_status(BreakpointInfo * bp) {
    char * res = NULL;
    ByteArrayOutputStream buf;
    OutputStream * out = create_byte_array_output_stream(&buf);
    write_breakpoint_status(out, bp);
    write_stream(out, 0);
    get_byte_array_output_stream_data(&buf, &res, NULL);
    return res;
}

void delete_breakpoint(BreakpointInfo * bp) {
    BreakpointRef * br = find_breakpoint_ref(bp, NULL);
    assert(br != NULL && br->channel == NULL);
    remove_ref(br);
}

void iterate_context_breakpoint_links(Context * ctx, ContextBreakpoint * cb, IterateCBLinksCallBack * callback, void * args) {
    unsigned i;
    Context * grp = context_get_group(ctx, CONTEXT_GROUP_BREAKPOINT);
    BreakInstruction * bi = (BreakInstruction *)((char *)cb - offsetof(BreakInstruction, cb));
    for (i = 0; i < bi->ref_cnt; i++) {
        if (bi->refs[i].ctx == grp) callback(bi->refs[i].bp, args);
    }
}

int is_breakpoint_address(Context * ctx, ContextAddress address) {
    Context * mem = NULL;
    ContextAddress mem_addr = 0;
    BreakInstruction * bi = NULL;
    if (planted_sw_bp_cnt == 0) return 0;
    if (context_get_canonical_addr(ctx, address, &mem, &mem_addr, NULL, NULL) < 0) return 0;
    bi = find_instruction(mem, 0, mem_addr, CTX_BP_ACCESS_INSTRUCTION, 1);
    assert(bi == NULL || !bi->virtual_addr);
    return bi != NULL && bi->planted;
}

void evaluate_breakpoint(Context * ctx) {
    unsigned i;
    Context * grp = context_get_group(ctx, CONTEXT_GROUP_BREAKPOINT);
    EvaluationRequest * req = create_evaluation_request(grp);
    int already_posted = !list_is_empty(&req->link_posted) || !list_is_empty(&req->link_active);
    int need_to_post = already_posted || cache_enter_cnt > 0;

    assert(context_has_state(ctx));
    assert(ctx->stopped);
    assert(ctx->stopped_by_bp || ctx->stopped_by_cb);
    assert(ctx->exited == 0);
    assert(!is_intercepted(ctx));

    if (ctx->stopped_by_bp) {
        Context * mem = NULL;
        ContextAddress mem_addr = 0;
        ContextAddress pc = 0;
        BreakInstruction * bi = NULL;
        if (get_PC(ctx, &pc) < 0 ||
            context_get_canonical_addr(ctx, pc, &mem, &mem_addr, NULL, NULL) < 0) {
            int error = set_errno(errno, "Cannot evaluate breakpoint");
            ctx->pending_intercept = 1;
            ctx->stopped_by_exception = 1;
            loc_free(ctx->exception_description);
            ctx->exception_description = loc_strdup(errno_to_str(error));
        }
        else {
            bi = find_instruction(mem, 0, mem_addr, CTX_BP_ACCESS_INSTRUCTION, 1);
        }
        if (bi != NULL) {
            for (i = 0; i < bi->ref_cnt; i++) {
                if (bi->refs[i].ctx == grp) {
                    BreakpointInfo * bp = bi->refs[i].bp;
                    ConditionEvaluationRequest * c = add_condition_evaluation_request(req, ctx, bp, bi);
                    if (c == NULL) continue;
                    if (need_to_post) continue;
                    assert(bi->valid);
                    if (is_disabled(bp)) continue;
                    if (bp->condition != NULL || bp->stop_group != NULL || bp->temporary) {
                        need_to_post = 1;
                        continue;
                    }
                    if (!check_context_ids_condition(bp, ctx)) continue;
                    c->condition_ok = 1;
                }
            }
        }
    }
    if (ctx->stopped_by_cb) {
        int j;
        assert(ctx->stopped_by_cb[0] != NULL);
        for (j = 0; ctx->stopped_by_cb[j]; j++) {
            BreakInstruction * bi = (BreakInstruction *)((char *)ctx->stopped_by_cb[j] - offsetof(BreakInstruction, cb));
            for (i = 0; i < bi->ref_cnt; i++) {
                if (bi->refs[i].ctx == grp) {
                    BreakpointInfo * bp = bi->refs[i].bp;
                    ConditionEvaluationRequest * c = add_condition_evaluation_request(req, ctx, bp, bi);
                    if (c == NULL) continue;
                    if (need_to_post) continue;
                    assert(bi->valid);
                    if (is_disabled(bp)) continue;
                    if (bp->condition != NULL || bp->stop_group != NULL || bp->temporary) {
                        need_to_post = 1;
                        continue;
                    }
                    if (!check_context_ids_condition(bp, ctx)) continue;
                    c->condition_ok = 1;
                }
            }
        }
    }

    if (need_to_post) {
        if (!already_posted) post_evaluation_request(req);
    }
    else {
        done_condition_evaluation(req);
        for (i = 0; i < req->bp_cnt; i++) {
            Context * c = req->bp_arr[i].ctx;
            BreakpointInfo * bp = req->bp_arr[i].bp;
            if (bp->status_changed && generation_done == generation_posted) {
                assert(bp->client_cnt > 0);
                notify_breakpoint_status(bp);
            }
            context_unlock(c);
        }
        req->bp_cnt = 0;
    }
}

static void safe_skip_breakpoint(void * arg);

static void safe_restore_breakpoint(void * arg) {
    Context * ctx = (Context *)arg;
    ContextExtensionBP * ext = EXT(ctx);
    BreakInstruction * bi = ext->stepping_over_bp;

    assert(!bi->virtual_addr);
    assert(bi->stepping_over_bp > 0);
    assert(find_instruction(bi->cb.ctx, 0, bi->cb.address, bi->cb.access_types, bi->cb.length) == bi);
    if (!ctx->exiting && ctx->stopped && !ctx->stopped_by_exception && !ctx->advanced) {
        Context * mem = NULL;
        ContextAddress mem_addr = 0;
        ContextAddress pc = 0;
        if (get_PC(ctx, &pc) == 0 &&
                context_get_canonical_addr(ctx, pc, &mem, &mem_addr, NULL, NULL) == 0 &&
                bi->cb.ctx == mem && bi->cb.address == mem_addr) {
            if (ext->step_over_bp_cnt < 100) {
                ext->step_over_bp_cnt++;
                safe_skip_breakpoint(arg);
                return;
            }
            trace(LOG_ALWAYS, "Skip breakpoint error: wrong PC %#" PRIx64, (uint64_t)pc);
        }
    }
    ext->stepping_over_bp = NULL;
    ext->step_over_bp_cnt = 0;
    bi->stepping_over_bp--;
    if (bi->stepping_over_bp == 0 && bi->valid && bi->ref_cnt > 0 &&
            !bi->cb.ctx->exited && !bi->cb.ctx->exiting && !bi->planted) {
        plant_instruction(bi);
    }
    context_unlock(ctx);
}

static void safe_skip_breakpoint(void * arg) {
    Context * ctx = (Context *)arg;
    ContextExtensionBP * ext = EXT(ctx);
    BreakInstruction * bi = ext->stepping_over_bp;
    int error = 0;

    assert(bi != NULL);
    assert(bi->stepping_over_bp > 0);
    assert(find_instruction(bi->cb.ctx, 0, bi->cb.address, bi->cb.access_types, bi->cb.length) == bi);

    post_safe_event(ctx, safe_restore_breakpoint, ctx);

    if (ctx->exited || ctx->exiting || is_intercepted(ctx)) return;

#ifndef NDEBUG
    {
        Context * mem = NULL;
        ContextAddress mem_addr = 0;
        ContextAddress pc = 0;
        assert(ctx->stopped);
        if (get_PC(ctx, &pc) == 0) {
            assert(context_get_canonical_addr(ctx, pc, &mem, &mem_addr, NULL, NULL) == 0);
            assert(bi->cb.address == mem_addr);
        }
    }
#endif

    if (bi->planted && remove_instruction(bi) < 0) error = errno;
    if (error == 0 && safe_context_single_step(ctx) < 0) error = errno;
    if (error) {
        error = set_errno(error, "Cannot step over breakpoint");
        ctx->signal = 0;
        ctx->stopped = 1;
        ctx->stopped_by_bp = 0;
        ctx->stopped_by_cb = NULL;
        ctx->pending_intercept = 1;
        ctx->stopped_by_exception = 1;
        loc_free(ctx->exception_description);
        ctx->exception_description = loc_strdup(errno_to_str(error));
    }
}

/*
 * When a context is stopped by breakpoint, it is necessary to disable
 * the breakpoint temporarily before the context can be resumed.
 * This function removes break instruction, then does single step
 * over breakpoint location, then restores break intruction.
 * Return: 0 if it is OK to resume context from current state,
 * return 1 if context needs to step over a breakpoint.
 */
int skip_breakpoint(Context * ctx, int single_step) {
    ContextExtensionBP * ext = EXT(ctx);
    Context * mem = NULL;
    ContextAddress pc = 0;
    ContextAddress mem_addr = 0;
    BreakInstruction * bi;

    assert(ctx->stopped);
    assert(!ctx->exited);
    assert(single_step || ext->stepping_over_bp == NULL);

    if (ext->stepping_over_bp != NULL) return 0;
    if (!ctx->stopped_by_bp && ctx->stopped_by_cb == NULL) return 0;
    if (ctx->exited || ctx->exiting) return 0;

    if (get_PC(ctx, &pc) < 0) return -1;
    if (context_get_canonical_addr(ctx, pc, &mem, &mem_addr, NULL, NULL) < 0) return -1;
    bi = find_instruction(mem, 0, mem_addr, CTX_BP_ACCESS_INSTRUCTION, 1);
    if (bi == NULL || bi->planting_error) return 0;
    bi->stepping_over_bp++;
    ext->stepping_over_bp = bi;
    ext->step_over_bp_cnt = 1;
    assert(bi->stepping_over_bp > 0);
    context_lock(ctx);
    post_safe_event(ctx, safe_skip_breakpoint, ctx);
    return 1;
}

int is_skipping_breakpoint(Context * ctx) {
    ContextExtensionBP * ext = EXT(ctx);
    return ext->stepping_over_bp != NULL;
}

BreakpointInfo * create_eventpoint_ext(BreakpointAttribute * attrs, Context * ctx, EventPointCallBack * callback, void * callback_args) {
    BreakpointInfo * bp = (BreakpointInfo *)loc_alloc_zero(sizeof(BreakpointInfo));

    bp->client_cnt = 1;
    if (ctx != NULL) context_lock(bp->ctx = ctx);
    bp->event_callback = callback;
    bp->event_callback_args = callback_args;
    list_init(&bp->link_clients);
    list_init(&bp->link_hit_count);
    list_add_last(&bp->link_all, &breakpoints);
    set_breakpoint_attributes(bp, attrs);
    replant_breakpoint(bp);
    return bp;
}

BreakpointInfo * create_eventpoint(const char * location, Context * ctx, EventPointCallBack * callback, void * callback_args) {
    static const char * attr_list[] = { BREAKPOINT_ENABLED, BREAKPOINT_LOCATION };
    BreakpointAttribute * attrs = NULL;
    BreakpointAttribute ** ref = &attrs;
    unsigned i;

    /* Create attributes to allow get_breakpoint_attributes() and change_breakpoint_attributes() calls */
    for (i = 0; i < sizeof(attr_list) / sizeof(char *); i++) {
        ByteArrayOutputStream buf;
        BreakpointAttribute * attr = (BreakpointAttribute *)loc_alloc_zero(sizeof(BreakpointAttribute));
        OutputStream * out = create_byte_array_output_stream(&buf);
        attr->name = loc_strdup(attr_list[i]);
        switch (i) {
        case 0:
            json_write_boolean(out, 1);
            break;
        case 1:
            json_write_string(out, location);
            break;
        }
        write_stream(out, 0);
        get_byte_array_output_stream_data(&buf, &attr->value, NULL);
        *ref = attr; ref = &attr->next;
    }

    return create_eventpoint_ext(attrs, ctx, callback, callback_args);
}

void destroy_eventpoint(BreakpointInfo * bp) {
    assert(bp->id[0] == 0);
    assert(bp->client_cnt == 1);
    assert(list_is_empty(&bp->link_clients));
    bp->client_cnt = 0;
    replant_breakpoint(bp);
}

static void event_context_created(Context * ctx, void * args) {
    post_location_evaluation_request(ctx, NULL);
    list_init(&EXT(ctx)->link_hit_count);
}

static void event_context_changed(Context * ctx, void * args) {
    if (ctx->mem_access && context_get_group(ctx, CONTEXT_GROUP_PROCESS) == ctx) {
        /* If the context is a memory space, we need to update
         * breakpoints on all members of the group */
        LINK * l = context_root.next;
        while (l != &context_root) {
            Context * x = ctxl2ctxp(l);
            l = l->next;
            if (x->exited) continue;
            if (context_get_group(x, CONTEXT_GROUP_PROCESS) != ctx) continue;
            post_location_evaluation_request(x, NULL);
        }
    }
    else {
        post_location_evaluation_request(ctx, NULL);
    }
}

static void event_context_exited(Context * ctx, void * args) {
    post_location_evaluation_request(ctx, NULL);
}

static void event_context_disposed(Context * ctx, void * args) {
    LINK * l = NULL;
    ContextExtensionBP * ext = EXT(ctx);
    EvaluationRequest * req = ext->req;
    if (req != NULL) {
        assert(list_is_empty(&req->link_posted));
        assert(list_is_empty(&req->link_active));
        loc_free(req->bp_arr);
        loc_free(req);
        ext->req = NULL;
    }
    l = ext->link_hit_count.next;
    if (l != NULL) { /* link_hit_count can be uninitialized */
        while (l != &ext->link_hit_count) {
            BreakpointHitCount * c = link_ctx2hcnt(l);
            l = l->next;
            list_remove(&c->link_bp);
            list_remove(&c->link_ctx);
            loc_free(c);
        }
    }
}

#if SERVICE_MemoryMap
static void event_code_unmapped(Context * ctx, ContextAddress addr, ContextAddress size, void * args) {
    /* Unmapping a code section unplants all breakpoint instructions in that section as side effect.
     * This function udates service data structure to reflect that.
     */
    int cnt = 0;
    while (size > 0) {
        ContextAddress sz = size;
        LINK * l = instructions.next;
        Context * mem = NULL;
        ContextAddress mem_addr = 0;
        ContextAddress mem_base = 0;
        ContextAddress mem_size = 0;
        if (context_get_canonical_addr(ctx, addr, &mem, &mem_addr, &mem_base, &mem_size) < 0) break;
        if (mem_base + mem_size - mem_addr < sz) sz = mem_base + mem_size - mem_addr;
        while (l != &instructions) {
            unsigned i;
            BreakInstruction * bi = link_all2bi(l);
            l = l->next;
            if (!bi->planted) continue;
            if (!bi->saved_size) continue;
            if (bi->cb.ctx != mem) continue;
            if (bi->cb.address < mem_addr || bi->cb.address >= mem_addr + sz) continue;
            for (i = 0; i < bi->ref_cnt; i++) {
                bi->refs[i].bp->status_changed = 1;
                cnt++;
            }
            if (!bi->virtual_addr) planted_sw_bp_cnt--;
            bi->planted = 0;
            bi->dirty = 0;
        }
        addr += sz;
        size -= sz;
    }
    if (cnt > 0 && generation_done == generation_posted) done_replanting_breakpoints();
}
#endif

#if SERVICE_PathMap
static void event_path_map_changed(Channel * c, void * args) {
    unsigned hash = (unsigned)(uintptr_t)c / 16 % INP2BR_HASH_SIZE;
    LINK * l = inp2br[hash].next;
    while (l != &inp2br[hash]) {
        BreakpointRef * br = link_inp2br(l);
        l = l->next;
        if (br->channel == c && br->bp->file != NULL) replant_breakpoint(br->bp);
    }
}
#endif

static void channel_close_listener(Channel * c) {
    delete_breakpoint_refs(c);
}

void ini_breakpoints_service(Protocol * proto, TCFBroadcastGroup * bcg) {
    static int ini_done = 0;
    if (!ini_done) {
        int i;
        ini_done = 1;
        {
            static ContextEventListener listener = {
                event_context_created,
                event_context_exited,
                NULL,
                NULL,
                event_context_changed,
                event_context_disposed
            };
            add_context_event_listener(&listener, NULL);
        }
#if SERVICE_MemoryMap
        {
            static MemoryMapEventListener listener = {
                event_context_changed,
                event_code_unmapped,
                event_context_changed,
                event_context_changed,
            };
            add_memory_map_event_listener(&listener, NULL);
        }
#endif
#if SERVICE_PathMap
        {
            static PathMapEventListener listener = {
                event_path_map_changed,
            };
            add_path_map_event_listener(&listener, NULL);
        }
#endif
        for (i = 0; i < ADDR2INSTR_HASH_SIZE; i++) list_init(addr2instr + i);
        for (i = 0; i < ID2BP_HASH_SIZE; i++) list_init(id2bp + i);
        for (i = 0; i < INP2BR_HASH_SIZE; i++) list_init(inp2br + i);
        add_channel_close_listener(channel_close_listener);
        context_extension_offset = context_extension(sizeof(ContextExtensionBP));
        broadcast_group = bcg;
    }
    assert(broadcast_group == bcg);
    add_command_handler(proto, BREAKPOINTS, "set", command_set);
    add_command_handler(proto, BREAKPOINTS, "add", command_add);
    add_command_handler(proto, BREAKPOINTS, "change", command_change);
    add_command_handler(proto, BREAKPOINTS, "enable", command_enable);
    add_command_handler(proto, BREAKPOINTS, "disable", command_disable);
    add_command_handler(proto, BREAKPOINTS, "remove", command_remove);
    add_command_handler(proto, BREAKPOINTS, "getIDs", command_get_ids);
    add_command_handler(proto, BREAKPOINTS, "getProperties", command_get_properties);
    add_command_handler(proto, BREAKPOINTS, "getStatus", command_get_status);
    add_command_handler(proto, BREAKPOINTS, "getCapabilities", command_get_capabilities);
}

#endif /* SERVICE_Breakpoints */
