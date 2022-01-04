/*******************************************************************************
 * Copyright (c) 2007-2018 Wind River Systems, Inc. and others.
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
 * TCF service line Numbers - ELF version.
 *
 * The service associates locations in the source files with the corresponding
 * machine instruction addresses in the executable object.
 */

#include <tcf/config.h>

#if SERVICE_LineNumbers && (!ENABLE_LineNumbersProxy || ENABLE_LineNumbersMux) && ENABLE_ELF

#include <errno.h>
#include <assert.h>
#include <stdio.h>
#include <tcf/framework/context.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/exceptions.h>
#include <tcf/framework/cache.h>
#include <tcf/framework/trace.h>
#include <tcf/framework/json.h>
#include <tcf/framework/protocol.h>
#include <tcf/services/linenumbers.h>
#include <tcf/services/tcf_elf.h>
#include <tcf/services/dwarfio.h>
#include <tcf/services/dwarf.h>
#include <tcf/services/dwarfcache.h>
#include <tcf/services/stacktrace.h>
#include <tcf/services/pathmap.h>
#if ENABLE_LineNumbersMux
#define LINENUMBERS_READER_PREFIX elf_reader_
#include <tcf/services/linenumbers_mux.h>
#endif
#include <tcf/services/linenumbers_elf-ext.h>

static int compare_path(Channel * chnl, Context * ctx, const char * file, const char * pwd, const char * dir, const char * name) {
    int i, j;
    char buf[FILE_PATH_SIZE];
    char * full_name = NULL;

    if (file == NULL) return 0;
    if (name == NULL) return 0;

    while (file[0] == '.') {
        if (file[1] == '.' && file[2] == '/') file += 3;
        else if (file[1] == '/') file += 2;
        else break;
    }
    i = strlen(file);

    if (is_absolute_path(name)) {
        full_name = (char *)name;
    }
    else if (dir != NULL && is_absolute_path(dir)) {
        snprintf(full_name = buf, sizeof(buf), "%s/%s", dir, name);
    }
    else if (dir != NULL && pwd != NULL) {
        snprintf(full_name = buf, sizeof(buf), "%s/%s/%s", pwd, dir, name);
    }
    else if (pwd != NULL) {
        snprintf(full_name = buf, sizeof(buf), "%s/%s", pwd, name);
    }
    else {
        full_name = (char *)name;
    }
    full_name = canonic_path_map_file_name(full_name);
    j = strlen(full_name);
    if (i <= j && strcmp(file, full_name + j - i) == 0) return 1;
#if SERVICE_PathMap
    {
        char * s = apply_path_map(chnl, ctx, full_name, PATH_MAP_TO_CLIENT);
        if (s != full_name) {
            full_name = canonic_path_map_file_name(s);
            j = strlen(full_name);
            if (i <= j && strcmp(file, full_name + j - i) == 0) return 1;
        }
    }
#endif
    return 0;
}

static LineNumbersState * get_next_in_text(CompUnit * unit, LineNumbersState * state) {
    LineNumbersState * next = NULL;
    U4_T index = state->mStatesIndexPos + 1;
    if (index >= unit->mStatesCnt) return NULL;
    next = unit->mStatesIndex[index++];
    while (next->mLine == state->mLine && next->mColumn == state->mColumn) {
        if (index >= unit->mStatesCnt) return NULL;
        next = unit->mStatesIndex[index++];
    }
    if (state->mFile != next->mFile) return NULL;
    return next;
}

static LineNumbersState * get_next_in_code(CompUnit * unit, LineNumbersState * state) {
    LineNumbersState * next = state;
    if (state->mFlags & LINE_EndSequence) return NULL;
    if (state + 1 >= unit->mStates + unit->mStatesCnt) return NULL;
    for (;;) {
        next++;
        if (next->mFile != state->mFile) break;
        if (next->mLine != state->mLine) break;
        if (next->mColumn != state->mColumn) break;
        if (next->mSection != state->mSection) return NULL;
        if (next->mFlags != state->mFlags) break;
        if (next->mISA != state->mISA) break;
        if (next->mOpIndex != state->mOpIndex) break;
        if (next->mDiscriminator != state->mDiscriminator) break;
        if (next + 1 >= unit->mStates + unit->mStatesCnt) break;
    }
    return next;
}

static LineNumbersState * get_next_statement(CompUnit * unit, LineNumbersState * state) {
    /* Select addreess most suitable for breakpoint planting.
     * DWARF 3 standard says:
     * "is_stmt: A boolean indicating that the current instruction is a recommended
     * breakpoint location. A recommended breakpoint location is intended to
     * "represent" a line, a statement and/or a semantically distinct subpart of a
     * statement."
     */
    LineNumbersState * next = state;
    if (next == NULL) return NULL;
    if (next->mFlags & LINE_IsStmt) return next;
    for (;;) {
        U4_T index = next->mStatesIndexPos + 1;
        if (index >= unit->mStatesCnt) break;
        next = unit->mStatesIndex[index];
        if (next->mFile != state->mFile) break;
        if (next->mLine != state->mLine) break;
        if (next->mFlags & LINE_IsStmt) return next;
    }
    return NULL;
}

static void call_client(Context * ctx, CompUnit * unit, LineNumbersState * state,
                        LineNumbersState * code_next, LineNumbersState * text_next,
                        ContextAddress state_addr, LineNumbersCallBack * client, void * args) {
    CodeArea area;
    FileInfo * file_info = unit->mFiles + state->mFile;
    LineNumbersState * text_next_stmt = get_next_statement(unit, text_next);

    if (code_next == NULL) return;
    assert(state->mSection == code_next->mSection);

    memset(&area, 0, sizeof(area));
    area.start_line = state->mLine;
    area.start_column = state->mColumn;
    area.end_line = text_next ? text_next->mLine : state->mLine + 1;
    area.end_column = text_next ? text_next->mColumn : 0;

    area.directory = unit->mDir;
    if (state->mFileName != NULL) {
        area.file = state->mFileName;
    }
    else if (is_absolute_path(file_info->mName) || file_info->mDir == NULL) {
        area.file = file_info->mName;
    }
    else if (is_absolute_path(file_info->mDir)) {
        area.directory = file_info->mDir;
        area.file = file_info->mName;
    }
    else {
        char buf[FILE_PATH_SIZE];
        snprintf(buf, sizeof(buf), "%s/%s", file_info->mDir, file_info->mName);
        area.file = state->mFileName = loc_strdup(buf);
    }

    area.file_mtime = file_info->mModTime;
    area.file_size = file_info->mSize;
    area.start_address = state_addr;
    area.end_address = code_next->mAddress - state->mAddress + state_addr;
    if (text_next != NULL) {
        if (text_next->mSection == state->mSection) {
            area.next_address = text_next->mAddress - state->mAddress + state_addr;
        }
        else {
            ELF_Section * s = NULL;
            if (text_next->mSection) s = unit->mFile->sections + text_next->mSection;
            area.next_address = elf_map_to_run_time_address(ctx, unit->mFile, s, text_next->mAddress);
        }
    }
    if (text_next_stmt != NULL) {
        if (text_next_stmt->mSection == state->mSection) {
            area.next_stmt_address = text_next_stmt->mAddress - state->mAddress + state_addr;
        }
        else {
            ELF_Section * s = NULL;
            if (text_next_stmt->mSection) s = unit->mFile->sections + text_next_stmt->mSection;
            area.next_stmt_address = elf_map_to_run_time_address(ctx, unit->mFile, s, text_next_stmt->mAddress);
        }
    }
    area.isa = state->mISA;
    area.is_statement = (state->mFlags & LINE_IsStmt) != 0;
    area.basic_block = (state->mFlags & LINE_BasicBlock) != 0;
    area.prologue_end = (state->mFlags & LINE_PrologueEnd) != 0;
    area.epilogue_begin = (state->mFlags & LINE_EpilogueBegin) != 0;
    area.op_index = state->mOpIndex;
    area.discriminator = state->mDiscriminator;
    client(&area, args);
}

static void unit_line_to_address(Context * ctx, MemoryRegion * mem, CompUnit * unit,
                                 unsigned file, unsigned line, unsigned column,
                                 LineNumbersCallBack * client, void * args) {
    if (unit->mStatesCnt >= 2) {
        unsigned l = 0;
        unsigned h = unit->mStatesCnt;
        while (l < h) {
            unsigned k = (h + l) / 2;
            LineNumbersState * state = unit->mStatesIndex[k];
            if (state->mFile < file) {
                l = k + 1;
            }
            else if (state->mFile > file || state->mLine > line || (column && state->mLine == line && state->mColumn > column)) {
                h = k;
            }
            else {
                LineNumbersState * next = get_next_in_text(unit, state);
                U4_T next_line = next ? next->mLine : state->mLine + 1;
                U4_T next_column = next ? next->mColumn : 0;
                if (next_line < line || (column && next_line == line && next_column <= column)) {
                    l = k + 1;
                }
                else {
                    assert(state->mFile == file);
                    while (k > 0) {
                        LineNumbersState * prev = unit->mStatesIndex[k - 1];
                        if (prev->mFile != state->mFile) break;
                        if (prev->mLine != state->mLine) break;
                        if (column && prev->mColumn != state->mColumn) break;
                        state = prev;
                        k--;
                    }
                    for (;;) {
                        ELF_Section * sec = state->mSection ? unit->mFile->sections + state->mSection : NULL;
                        ContextAddress addr = elf_run_time_address_in_region(ctx, mem, unit->mFile, sec, state->mAddress);
                        if (errno == 0) {
                            LineNumbersState * code_next = get_next_in_code(unit, state);
                            /* Note: area code size 0 is OK as long as it has valid next statement address */
                            if (code_next != NULL && state->mAddress <= code_next->mAddress) {
                                LineNumbersState * text_next = get_next_in_text(unit, state);
                                U4_T next_line = text_next ? text_next->mLine : state->mLine + 1;
                                U4_T next_column = text_next ? text_next->mColumn : 0;
                                if (next_line > line || (next_line == line && next_column > column)) {
                                    UNIT_TO_LINE_ADDR_LOCALS_HOOK
                                    assert(state->mLine <= line);
                                    UNIT_TO_LINE_ADDR_HOOK
                                    {
                                    call_client(ctx, unit, state, code_next, text_next, addr, client, args);
                                    }
                                }
                            }
                        }
                        if (++k >= unit->mStatesCnt) break;
                        state = unit->mStatesIndex[k];
                        if (state->mFile > file) break;
                        if (state->mLine > line) break;
                        if (column && state->mColumn > column) break;
                    }
                    break;
                }
            }
        }
    }
}

int line_to_address(Context * ctx, const char * file_name, int line, int column,
                    LineNumbersCallBack * client, void * args) {
    int err = 0;
    Channel * chnl = cache_channel();
    static MemoryMap map;
    LINE_TO_ADDR_HOOK_0

    if (ctx == NULL) err = ERR_INV_CONTEXT;
    else if (ctx->exited) err = ERR_ALREADY_EXITED;

    if (err == 0 && elf_get_map(ctx, 0, ~(ContextAddress)0, &map) < 0) err = errno;

    if (err == 0) {
        unsigned i, j;
        unsigned h = 0;
        char * fnm = NULL;
        for (i = 0; i < map.region_cnt; i++) {
            Trap trap;
            MemoryRegion * r = map.regions + i;
            ELF_File * file = elf_open_memory_region_file(r, NULL);
            if (file == NULL) continue;
            if (set_trap(&trap)) {
                DWARFCache * cache = get_dwarf_cache(get_dwarf_file(file));
                if (!cache->mLineInfoLoaded) {
                    for (j = 0; j < file->section_cnt; j++) {
                        ObjectInfo * info = cache->mObjectHashTable[j].mCompUnits;
                        while (info != NULL) {
                            CompUnit * unit = info->mCompUnit;
                            if (!unit->mLineInfoLoaded) load_line_numbers(unit);
                            info = info->mSibling;
                        }
                    }
                    cache->mLineInfoLoaded = 1;
                }
                if (cache->mFileInfoHash) {
                    FileInfo * f = NULL;
                    if (fnm == NULL) {
                        fnm = canonic_path_map_file_name(file_name);
                        LINE_TO_ADDR_HOOK_1
                        h = calc_file_name_hash(fnm);
                    }
                    LINE_TO_ADDR_HOOK_BP
                    f = cache->mFileInfoHash[h % cache->mFileInfoHashSize];
                    while (f != NULL) {
                        if (f->mNameHash == h && compare_path(chnl, ctx, fnm, f->mCompUnit->mDir, f->mDir, f->mName)) {
                            CompUnit * unit = f->mCompUnit;
                            unsigned j = f - unit->mFiles;
                            LINE_TO_ADDR_HOOK_2
                            unit_line_to_address(ctx, r, unit, j, line, column, client, args);
                        }
                        f = f->mNextInHash;
                    }
                }
                clear_trap(&trap);
            }
            else {
                err = trap.error;
                trace(LOG_ALWAYS, "Cannot load DWARF line numbers section: %s", errno_to_str(err));
                break;
            }
        }
    }

    if (err != 0) {
        errno = err;
        return -1;
    }
    return 0;
}

int address_to_line(Context * ctx, ContextAddress addr0, ContextAddress addr1, LineNumbersCallBack * client, void * args) {
    Trap trap;

    /* TODO: make addr0..addr1 range inclusive */
    if (addr1 == 0) return 0;
    addr1--;

    if (!set_trap(&trap)) return -1;
    if (ctx == NULL) exception(ERR_INV_CONTEXT);
    if (ctx->exited) exception(ERR_ALREADY_EXITED);
    while (addr0 <= addr1) {
        ContextAddress range_rt_addr = 0;
        UnitAddressRange * range = elf_find_unit(ctx, addr0, addr1, &range_rt_addr);
        if (range == NULL) break;
        assert(range->mSize > 0);
        assert(range->mAddr + range->mSize > range->mAddr || range->mAddr + range->mSize == 0);
        assert(range_rt_addr + range->mSize > range_rt_addr || range_rt_addr + range->mSize == 0);
        assert(addr1 >= range_rt_addr);
        assert(addr0 <= range_rt_addr + range->mSize - 1);
        if (!range->mUnit->mLineInfoLoaded) load_line_numbers(range->mUnit);
        if (range->mUnit->mStatesCnt >= 2) {
            CompUnit * unit = range->mUnit;
            unsigned l = 0;
            unsigned h = unit->mStatesCnt;
            ContextAddress addr_min = range->mAddr;
            ContextAddress addr_max = range->mAddr + range->mSize - 1;
            if (addr0 > range_rt_addr) addr_min = addr0 - range_rt_addr + range->mAddr;
            if (addr1 < range_rt_addr + range->mSize - 1) addr_max = addr1 - range_rt_addr + range->mAddr;
            assert(addr_min >= range->mAddr);
            assert(addr_max <= range->mAddr + range->mSize - 1);
            while (l < h) {
                unsigned k = (h + l) / 2;
                LineNumbersState * state = unit->mStates + k;
                if (state->mSection > range->mSection) {
                    h = k;
                }
                else if (state->mSection < range->mSection) {
                    l = k + 1;
                }
                else if (state->mAddress > addr_max) {
                    h = k;
                }
                else {
                    LineNumbersState * next = get_next_in_code(unit, state);
                    if (next == NULL || next->mAddress <= addr_min) {
                        l = k + 1;
                    }
                    else {
                        while (k > 0) {
                            LineNumbersState * prev = unit->mStates + k - 1;
                            if (state->mAddress <= addr_min) break;
                            if (prev->mAddress > addr_max) break;
                            state = prev;
                            k--;
                        }
                        for (;;) {
                            LineNumbersState * code_next = get_next_in_code(unit, state);
                            if (code_next != NULL) {
                                if (state->mAddress < code_next->mAddress) {
                                    LineNumbersState * text_next = get_next_in_text(unit, state);
                                    ADDR_TO_LINE_HOOK
                                    {
                                    call_client(ctx, unit, state, code_next, text_next, state->mAddress - range->mAddr + range_rt_addr, client, args);
                                    }
                                }
                                assert(code_next > state);
                                k = code_next - unit->mStates;
                            }
                            else {
                                k++;
                            }
                            if (k >= unit->mStatesCnt) break;
                            state = unit->mStates + k;
                            if (state->mAddress > addr_max) break;
                        }
                        break;
                    }
                }
            }
        }
        addr0 = range_rt_addr + range->mSize;
    }
    clear_trap(&trap);
    return 0;
}

void ini_line_numbers_lib(void) {
#if ENABLE_LineNumbersMux
    add_line_numbers_reader(&line_numbers_reader);
#endif
}

#endif /* SERVICE_LineNumbers && !ENABLE_LineNumbersProxy && ENABLE_ELF */
