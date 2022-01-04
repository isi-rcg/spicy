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
 * This module implements reading and caching of ELF files.
 */

#include <tcf/config.h>

#if ENABLE_ELF

#include <stddef.h>
#include <assert.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <tcf/framework/mdep-fs.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/exceptions.h>
#include <tcf/framework/compression.h>
#include <tcf/framework/events.h>
#include <tcf/framework/cache.h>
#include <tcf/framework/trace.h>
#include <tcf/framework/json.h>
#include <tcf/services/tcf_elf.h>
#include <tcf/services/memorymap.h>
#include <tcf/services/dwarfcache.h>
#include <tcf/services/dwarfreloc.h>
#include <tcf/services/pathmap.h>

#if defined(USE_MMAP)
#elif defined(_WRS_KERNEL)
#  define USE_MMAP 0
#elif defined(_MSC_VER)
/* Memoy mapped files appear broken on Windows 8 */
#  define USE_MMAP 0
#elif defined(_WIN32) || defined(__CYGWIN__)
#  define USE_MMAP 0
#else
#  include <sys/mman.h>
#  define USE_MMAP 1
#endif

#define MIN_FILE_AGE 3
#define MAX_FILE_AGE 60
#define MAX_FILE_CNT 100

#ifndef SHF_COMPRESSED
#define SHF_COMPRESSED 0x00000800
#endif
#ifndef ELFCOMPRESS_ZLIB
#define ELFCOMPRESS_ZLIB 1
#endif
#ifndef ARCH_SHF_SMALL
#define ARCH_SHF_SMALL 0
#endif

#define SHDR_BUF_SIZE 64

typedef struct FileINode {
    struct FileINode * next;
    char * name;
    ino_t ino;
} FileINode;

typedef struct ElfListState {
    Context * ctx;
    unsigned pos;
    MemoryMap map;
    MemoryRegion * region;
    struct ElfListState * next;
} ElfListState;

typedef struct KernelModuleAddress {
    U8_T module_init;
    U8_T module_core;
    U8_T init_size;
    U8_T core_size;
    U8_T init_text_size;
    U8_T core_text_size;
    U8_T init_ro_size;
    U8_T core_ro_size;
} KernelModuleAddress;

static ELF_File * files = NULL;
static FileINode * inodes = NULL;
static ELFOpenListener * openlisteners = NULL;
static unsigned openlisteners_cnt = 0;
static unsigned openlisteners_max = 0;
static ELFCloseListener * closelisteners = NULL;
static unsigned closelisteners_cnt = 0;
static unsigned closelisteners_max = 0;
static int elf_cleanup_posted = 0;
static ino_t elf_ino_cnt = 0;
static ElfListState * elf_list_state = NULL;

#if ENABLE_DebugContext

static MemoryMap elf_map;

#endif

static ELF_File * find_open_file_by_name(const char * name);

void elf_add_open_listener(ELFOpenListener listener) {
    if (openlisteners_cnt >= openlisteners_max) {
        openlisteners_max = openlisteners_max == 0 ? 16 : openlisteners_max * 2;
        openlisteners = (ELFOpenListener *)loc_realloc(openlisteners, sizeof(ELFOpenListener) * openlisteners_max);
    }
    openlisteners[openlisteners_cnt++] = listener;
}

void elf_add_close_listener(ELFCloseListener listener) {
    if (closelisteners_cnt >= closelisteners_max) {
        closelisteners_max = closelisteners_max == 0 ? 16 : closelisteners_max * 2;
        closelisteners = (ELFCloseListener *)loc_realloc(closelisteners, sizeof(ELFCloseListener) * closelisteners_max);
    }
    closelisteners[closelisteners_cnt++] = listener;
}

static void elf_dispose(ELF_File * file) {
    unsigned n;
    assert(file->lock_cnt == 0);
    trace(LOG_ELF, "Dispose ELF file cache %s", file->name);
    for (n = 0; n < closelisteners_cnt; n++) {
        closelisteners[n](file);
    }
    if (file->dwz_file) {
        assert(file->dwz_file->lock_cnt > 0);
        file->dwz_file->lock_cnt--;
        file->dwz_file = NULL;
    }
    if (file->fd >= 0) close(file->fd);
    if (file->sections != NULL) {
        for (n = 0; n < file->section_cnt; n++) {
            ELF_Section * s = file->sections + n;
#if !USE_MMAP
            loc_free(s->data);
#elif defined(_WIN32) || defined(__CYGWIN__)
            if (s->mmap_addr == NULL) loc_free(s->data);
            else UnmapViewOfFile(s->mmap_addr);
#else
            if (s->mmap_addr == NULL) loc_free(s->data);
            else munmap(s->mmap_addr, s->mmap_size);
#endif
            loc_free(s->sym_addr_table);
            loc_free(s->sym_names_hash);
            loc_free(s->sym_names_next);
            loc_free(s->reloc_zones_bondaries);
        }
        loc_free(file->sections);
    }
#if defined(_WIN32) || defined(__CYGWIN__)
    if (file->mmap_handle != NULL) CloseHandle(file->mmap_handle);
#endif
    for (n = 0; n < file->names_cnt; n++) {
        loc_free(file->names[n]);
    }
    loc_free(file->names);
    release_error_report(file->error);
    loc_free(file->pheaders);
    loc_free(file->str_pool);
    loc_free(file->debug_info_file_name);
    loc_free(file->dwz_file_name);
    loc_free(file->name);
    loc_free(file);
}

static void free_elf_list_state(ElfListState * state) {
    loc_free(state->map.regions);
    loc_free(state);
}

static void add_file_name(ELF_File * file, const char * name) {
    if (file->names_cnt >= file->names_max) {
        file->names_max += 8;
        file->names = (char **)loc_realloc(file->names, sizeof(char *) * file->names_max);
    }
    file->names[file->names_cnt++] = loc_strdup(name);
}

static int file_name_equ(ELF_File * file, const char * name) {
    unsigned i;
    if (name == NULL) return 0;
    if (strcmp(file->name, name) == 0) return 1;
    for (i = 0; i < file->names_cnt; i++) {
        if (strcmp(file->names[i], name) == 0) return 1;
    }
    return 0;
}

#if ENABLE_MemoryMap
static int is_file_mapped_by_mem_map(ELF_File * file, MemoryMap * map) {
    unsigned i;
    for (i = 0; i < map->region_cnt; i++) {
        MemoryRegion * r = map->regions + i;
        if (file->dev == r->dev && file->ino == r->ino) return 1;
        if (r->dev != 0 && r->dev != file->dev) continue;
        if (r->ino != 0 && r->ino != file->ino) continue;
        if (file_name_equ(file, r->file_name)) return 1;
    }
    return 0;
}

static int is_file_mapped(ELF_File * file, int * cache_miss) {
    LINK * l = context_root.next;
    while (l != &context_root) {
        MemoryMap * client_map = NULL;
        MemoryMap * target_map = NULL;
        Context * c = ctxl2ctxp(l);
        l = l->next;
        if (c->mem_access == 0 || c->exited) continue;
        if (c != context_get_group(c, CONTEXT_GROUP_PROCESS)) continue;
        if (memory_map_get(c, &client_map, &target_map) < 0) {
            if (get_error_code(errno) == ERR_CACHE_MISS) *cache_miss = 1;
            continue;
        }
        if (is_file_mapped_by_mem_map(file, client_map) ||
            is_file_mapped_by_mem_map(file, target_map)) return 1;
    }
    return 0;
}
#endif /* ENABLE_MemoryMap */

static void elf_cleanup_event(void * arg);

static void elf_cleanup_cache_client(void * arg) {
    ELF_File * prev = NULL;
    ELF_File * file = NULL;
    unsigned file_cnt = 0;
    unsigned max_file_age = MAX_FILE_AGE;
    static unsigned event_cnt = 0;

    assert(elf_cleanup_posted);

    file = files;
    while (file != NULL) {
        if (event_cnt % 5 == 0) {
            if (!file->mtime_changed) {
                struct stat st;
                if (stat(file->name, &st) < 0 ||
                    file->size != st.st_size ||
                    file->mtime != st.st_mtime ||
                    (st.st_ino != 0 && st.st_ino != file->ino))
                    file->mtime_changed = 1;
            }
        }
        file = file->next;
        file_cnt++;
    }

    if (file_cnt > MAX_FILE_AGE + MAX_FILE_CNT - MIN_FILE_AGE) {
        max_file_age = MIN_FILE_AGE;
    }
    else if (file_cnt > MAX_FILE_CNT) {
        max_file_age = MAX_FILE_AGE + MAX_FILE_CNT - file_cnt;
    }

#if ENABLE_MemoryMap
    file = files;
    while (file != NULL) {
        int cache_miss = 0;
        if (!file->mtime_changed && file->age > max_file_age && is_file_mapped(file, &cache_miss)) {
            file->age = 0;
            if (file->debug_info_file_name) {
                ELF_File * dbg = find_open_file_by_name(file->debug_info_file_name);
                if (dbg != NULL) dbg->age = 0;
            }
        }
        else if (cache_miss) {
            /* May be mapped, don't dispose this time */
            assert(file->age > max_file_age);
            file->age = max_file_age;
            if (file->debug_info_file_name) {
                ELF_File * dbg = find_open_file_by_name(file->debug_info_file_name);
                if (dbg != NULL && dbg->age > max_file_age) dbg->age = max_file_age;
            }
        }
        file = file->next;
    }
#endif

    cache_exit();
    elf_cleanup_posted = 0;

    file = files;
    while (file != NULL) {
        ELF_File * next = file->next;
        if (file->lock_cnt > 0) {
            prev = file;
        }
        else if (file->age > max_file_age || (file->age > MIN_FILE_AGE && list_is_empty(&context_root))) {
            elf_dispose(file);
            if (prev != NULL) prev->next = next;
            else files = next;
        }
        else {
#if !USE_MMAP
            if (file->fd >= 0 && close(file->fd) >= 0) file->fd = -1;
#endif
            file->age++;
            prev = file;
        }
        file = next;
    }

    if (files != NULL) {
        post_event_with_delay(elf_cleanup_event, NULL, 1000000);
        elf_cleanup_posted = 1;
    }
    else if (list_is_empty(&context_root)) {
        while (inodes != NULL) {
            FileINode * n = inodes;
            inodes = n->next;
            loc_free(n->name);
            loc_free(n);
        }
    }

    while (elf_list_state != NULL) {
        ElfListState * state = elf_list_state;
        elf_list_state = state->next;
        free_elf_list_state(state);
    }

    event_cnt++;
}

static void elf_cleanup_event(void * arg) {
    cache_enter(elf_cleanup_cache_client, NULL, NULL, 0);
}

static ino_t add_ino(const char * fnm, ino_t ino) {
    FileINode * n = (FileINode *)loc_alloc_zero(sizeof(*n));
    n->next = inodes;
    n->name = loc_strdup(fnm);
    n->ino = ino;
    inodes = n;
    return ino;
}

static ino_t elf_ino(const char * fnm) {
    /*
     * Number of the information node (the inode) for the file is used as file ID.
     * Since some file systems don't support inodes, this function is used in such cases
     * to generate virtual inode numbers to be used as file IDs.
     */
    char * abs = NULL;
    FileINode * n = inodes;
    while (n != NULL) {
        if (strcmp(n->name, fnm) == 0) return n->ino;
        n = n->next;
    }
    abs = canonicalize_file_name(fnm);
    if (abs == NULL) return add_ino(fnm, 0);
    n = inodes;
    while (n != NULL) {
        if (strcmp(n->name, abs) == 0) {
            free(abs);
            return add_ino(fnm, n->ino);
        }
        n = n->next;
    }
    if (elf_ino_cnt == 0) elf_ino_cnt++;
    add_ino(fnm, elf_ino_cnt);
    if (strcmp(abs, fnm) != 0) add_ino(abs, elf_ino_cnt);
    free(abs);
    return elf_ino_cnt++;
}

static ELF_File * find_open_file_by_name(const char * name) {
    ELF_File * prev = NULL;
    ELF_File * file = files;
    while (file != NULL) {
        if (!file->mtime_changed && file_name_equ(file, name)) {
            if (prev != NULL) {
                prev->next = file->next;
                file->next = files;
                files = file;
            }
            return file;
        }
        prev = file;
        file = file->next;
    }
    return NULL;
}

static ELF_File * find_open_file_by_inode(dev_t dev, ino_t ino, int64_t mtime) {
    ELF_File * prev = NULL;
    ELF_File * file = files;
    while (file != NULL) {
        if (file->dev == dev && file->ino == ino &&
            (mtime ? file->mtime == mtime : !file->mtime_changed)) {
            if (prev != NULL) {
                prev->next = file->next;
                file->next = files;
                files = file;
            }
            file->age = 0;
            return file;
        }
        prev = file;
        file = file->next;
    }
    return NULL;
}

static char * get_debug_info_file_name(ELF_File * file, int * error) {
    unsigned idx;
    char fnm[FILE_PATH_SIZE];
    struct stat buf;

    for (idx = 1; idx < file->section_cnt; idx++) {
        ELF_Section * sec = file->sections + idx;
        if (sec->size == 0) continue;
        if (sec->type == SHT_NOTE && (sec->flags & SHF_ALLOC)) {
            unsigned offs = 0;
            if (elf_load(sec) < 0) {
                *error = errno;
                return NULL;
            }
            while (offs < sec->size) {
                U4_T name_sz = *(U4_T *)((U1_T *)sec->data + offs);
                U4_T desc_sz = *(U4_T *)((U1_T *)sec->data + offs + 4);
                U4_T type = *(U4_T *)((U1_T *)sec->data + offs + 8);
                char * name = NULL;
                offs += 12;
                if (file->byte_swap) {
                    SWAP(name_sz);
                    SWAP(desc_sz);
                    SWAP(type);
                }
                name = (char *)((U1_T *)sec->data + offs);
                offs += name_sz;
                while (offs % 4 != 0) offs++;
                if (type == 3 && strcmp(name, "GNU") == 0) {
                    char * lnm = fnm;
                    char id[64];
                    size_t id_size = 0;
                    U1_T * desc = (U1_T *)sec->data + offs;
                    U4_T i = 0;
                    while (i < desc_sz) {
                        U1_T j = (desc[i] >> 4) & 0xf;
                        U1_T k = desc[i++] & 0xf;
                        id[id_size++] = j < 10 ? '0' + j : 'a' + j - 10;
                        id[id_size++] = k < 10 ? '0' + k : 'a' + k - 10;
                    }
                    id[id_size++] = 0;
                    trace(LOG_ELF, "Found GNU build ID %s", id);
                    snprintf(fnm, sizeof(fnm), "/usr/lib/debug/.build-id/%.2s/%s.debug", id, id + 2);
#if SERVICE_PathMap
                    lnm = apply_path_map(NULL, NULL, lnm, PATH_MAP_TO_LOCAL);
#endif
                    if (stat(lnm, &buf) == 0) return loc_strdup(lnm);
                    trace(LOG_ALWAYS, "Cannot open debug info file '%s': %s", lnm, errno_to_str(errno));
                }
                offs += desc_sz;
                while (offs % 4 != 0) offs++;
            }
        }
        else if (sec->name != NULL && strcmp(sec->name, ".gnu_debuglink") == 0) {
            if (elf_load(sec) < 0) {
                *error = errno;
                return NULL;
            }
            else {
                /* TODO: check debug info CRC */
                char * lnm = fnm;
                char * name = (char *)sec->data;
                int l = (int)strlen(file->name);
                while (l > 0 && file->name[l - 1] != '/' && file->name[l - 1] != '\\') l--;
                if (strcmp(file->name + l, name) != 0) {
                    snprintf(fnm, sizeof(fnm), "%.*s%s", l, file->name, name);
                    if (stat(fnm, &buf) == 0) return loc_strdup(fnm);
                }
                snprintf(fnm, sizeof(fnm), "%.*s.debug/%s", l, file->name, name);
                if (stat(fnm, &buf) == 0) return loc_strdup(fnm);
                snprintf(fnm, sizeof(fnm), "/usr/lib/debug%.*s%s", l, file->name, name);
#if SERVICE_PathMap
                lnm = apply_path_map(NULL, NULL, lnm, PATH_MAP_TO_LOCAL);
#endif
                if (stat(lnm, &buf) == 0) return loc_strdup(lnm);
                trace(LOG_ALWAYS, "Cannot open debug info file '%s': %s", lnm, errno_to_str(errno));
            }
        }
    }
    {
        char * lnm = fnm;
        snprintf(fnm, sizeof(fnm), "%s.debug", file->name);
#if SERVICE_PathMap
        lnm = apply_path_map(NULL, NULL, lnm, PATH_MAP_TO_LOCAL);
#endif
        if (stat(lnm, &buf) == 0) return loc_strdup(lnm);
    }
    return NULL;
}

static char * get_dwz_file_name(ELF_File * file, int * error) {
    unsigned idx;

    for (idx = 1; idx < file->section_cnt; idx++) {
        ELF_Section * sec = file->sections + idx;
        if (sec->size == 0) continue;
        if (sec->name == NULL) continue;
        if (strcmp(sec->name, ".gnu_debugaltlink") == 0) {
            if (elf_load(sec) < 0) {
                *error = errno;
                return NULL;
            }
            else {
                char * lnm = NULL;
                struct stat buf;
                char * name = (char *)sec->data;
                int l = (int)strlen(file->name);
                int m = (int)strlen(name);
                while (l > 0 && file->name[l - 1] != '/' && file->name[l - 1] != '\\') l--;
                if (strcmp(file->name + l, name) != 0) {
                    char fnm[FILE_PATH_SIZE];
                    snprintf(fnm, sizeof(fnm), "%.*s%s", l, file->name, name);
                    if (stat(fnm, &buf) == 0) return loc_strdup(fnm);
                }
                while (m > 0 && name[m - 1] != '/' && name[m - 1] != '\\') m--;
                if (strcmp(file->name + l, name) != 0) {
                    char fnm[FILE_PATH_SIZE];
                    snprintf(fnm, sizeof(fnm), "%.*s%s", l, file->name, name + m);
                    if (stat(fnm, &buf) == 0) return loc_strdup(fnm);
                }
                lnm = name;
#if SERVICE_PathMap
                lnm = apply_path_map(NULL, NULL, lnm, PATH_MAP_TO_LOCAL);
#endif
                if (stat(lnm, &buf) == 0) return loc_strdup(lnm);
                trace(LOG_ALWAYS, "Cannot open DWZ file '%s': %s", lnm, errno_to_str(errno));
            }
        }
    }
    return NULL;
}

static int is_debug_info_file(ELF_File * file) {
    size_t l;
    unsigned i;
    for (i = 1; i < file->section_cnt; i++) {
        ELF_Section * sec = file->sections + i;
        if (sec->size == 0 || sec->name == NULL) continue;
        if (sec->type == SHT_NOBITS) {
            if (strcmp(sec->name, ".text") == 0) return 1;
            if (strcmp(sec->name, ".data") == 0) return 1;
        }
        if (strcmp(sec->name, ".gnu_debuglink") == 0) return 0;
    }
    l = strlen(file->name);
    if (l > 6 && strcmp(file->name + l - 6, ".debug") == 0) return 1;
    return 0;
}

static int create_symbol_names_hash(ELF_Section * tbl);

static void reopen_file(ELF_File * file) {
    int error = 0;
    unsigned n = 0;
    if (file->fd >= 0) return;
    if (file->error != NULL) return;
    if ((file->fd = open(file->name, O_RDONLY | O_BINARY, 0)) < 0) error = errno;
    for (n = 0; n < openlisteners_cnt; n++) {
        openlisteners[n](file);
    }
    if (!error) {
        struct stat st;
        if (fstat(file->fd, &st) < 0) error = errno;
        else if (st.st_mtime != file->mtime) error = set_errno(ERR_OTHER, "Invalid file mtime");
    }
    if (error) {
        if (file->fd >= 0) {
            close(file->fd);
            file->fd = -1;
        }
        trace(LOG_ELF, "Error re-opening ELF file: %d %s", error, errno_to_str(error));
        file->error = get_error_report(error);
    }
}

static int read_fully(int fd, void * buf, size_t size) {
    char * p = (char *)buf;
    while (size > 0) {
        int rd = read(fd, p, size);
        if (rd <= 0) {
            if (rd == 0) set_errno(ERR_INV_FORMAT, "Unexpected end of file");
            return -1;
        }
        size -= rd;
        p += rd;
    }
    return 0;
}

static ELF_File * create_elf_cache(const char * file_name) {
    struct stat st;
    int error = 0;
    ELF_File * file = NULL;
    unsigned str_index = 0;
    char * real_name = NULL;

    file = find_open_file_by_name(file_name);
    if (file != NULL) {
        file->age = 0;
        return file;
    }

    if (stat(file_name, &st) < 0) {
        error = errno;
        memset(&st, 0, sizeof(st));
    }
    else if (st.st_ino == 0) {
        st.st_ino = elf_ino(file_name);
    }

    if (!error) {
        file = find_open_file_by_inode(st.st_dev, st.st_ino, st.st_mtime);
        if (file != NULL) {
            add_file_name(file, file_name);
            return file;
        }
    }

    trace(LOG_ELF, "Create ELF file cache %s", file_name);

    file = (ELF_File *)loc_alloc_zero(sizeof(ELF_File));
    file->fd = -1;

    if (error == 0) real_name = canonicalize_file_name(file_name);

    if (real_name == NULL || strcmp(real_name, file_name) == 0) {
        file->name = loc_strdup(file_name);
    }
    else {
        file->name = loc_strdup(real_name);
        add_file_name(file, file_name);
    }

    /* Note: fstat() returns st_dev = 0 on Windows */
    file->dev = st.st_dev;

    if (error == 0 && (file->fd = open(file->name, O_RDONLY | O_BINARY, 0)) < 0) error = errno;
    if (error == 0 && fstat(file->fd, &st) < 0) error = errno;
    if (error == 0 && st.st_ino == 0) st.st_ino = elf_ino(file->name);

    if (st.st_dev != 0) file->dev = st.st_dev;
    file->ino = st.st_ino;
    file->mtime = st.st_mtime;
    file->size = st.st_size;

    if (error == 0) {
        Elf32_Ehdr hdr;

        memset(&hdr, 0, sizeof(hdr));
        if (read_fully(file->fd, (char *)&hdr, sizeof(hdr)) < 0) error = errno;
        if (error == 0 && strncmp((char *)hdr.e_ident, ELFMAG, SELFMAG) != 0) {
            error = set_errno(ERR_INV_FORMAT, "Unsupported ELF identification code");
        }
        if (error == 0) {
            if (hdr.e_ident[EI_DATA] == ELFDATA2LSB) {
                file->big_endian = 0;
            }
            else if (hdr.e_ident[EI_DATA] == ELFDATA2MSB) {
                file->big_endian = 1;
            }
            else {
                error = set_errno(ERR_INV_FORMAT, "Invalid ELF data encoding ID");
            }
            file->byte_swap = file->big_endian != big_endian_host();
        }

        if (error != 0) {
            /* Nothing */
        }
        else if (hdr.e_ident[EI_CLASS] == ELFCLASS32) {
            if (file->byte_swap) {
                SWAP(hdr.e_type);
                SWAP(hdr.e_machine);
                SWAP(hdr.e_version);
                SWAP(hdr.e_entry);
                SWAP(hdr.e_phoff);
                SWAP(hdr.e_shoff);
                SWAP(hdr.e_flags);
                SWAP(hdr.e_ehsize);
                SWAP(hdr.e_phentsize);
                SWAP(hdr.e_phnum);
                SWAP(hdr.e_shentsize);
                SWAP(hdr.e_shnum);
                SWAP(hdr.e_shstrndx);
            }
            file->type = hdr.e_type;
            file->machine = hdr.e_machine;
            file->flags = hdr.e_flags;
            file->os_abi = hdr.e_ident[EI_OSABI];
            file->entry_address = (ContextAddress)hdr.e_entry;
            if (error == 0 && hdr.e_type != ET_EXEC && hdr.e_type != ET_DYN && hdr.e_type != ET_REL) {
                error = set_errno(ERR_INV_FORMAT, "Invalid ELF type ID");
            }
            if (error == 0 && hdr.e_version != EV_CURRENT) {
                error = set_errno(ERR_INV_FORMAT, "Unsupported ELF version");
            }
            if (error == 0 && hdr.e_shoff == 0) {
                error = set_errno(ERR_INV_FORMAT, "Invalid section header table's file offset");
            }
            if (error == 0) {
                unsigned cnt = 0;
                uint8_t * shdr_buf = (uint8_t *)tmp_alloc(hdr.e_shentsize * SHDR_BUF_SIZE);
                file->sections = (ELF_Section *)loc_alloc_zero(sizeof(ELF_Section) * hdr.e_shnum);
                file->section_cnt = hdr.e_shnum;
                while (error == 0 && cnt < hdr.e_shnum) {
                    if (cnt % SHDR_BUF_SIZE == 0) {
                        unsigned n = hdr.e_shnum - cnt;
                        if (n > SHDR_BUF_SIZE) n = SHDR_BUF_SIZE;
                        if (error == 0 && lseek(file->fd, hdr.e_shoff + cnt * hdr.e_shentsize, SEEK_SET) == (off_t)-1) error = errno;
                        if (error == 0 && read_fully(file->fd, shdr_buf, hdr.e_shentsize * n) < 0) error = errno;
                    }
                    if (error == 0) {
                        Elf32_Shdr shdr;
                        ELF_Section * sec = file->sections + cnt;
                        if (hdr.e_shentsize < sizeof(shdr)) {
                            memcpy(&shdr, shdr_buf + cnt % SHDR_BUF_SIZE * hdr.e_shentsize, hdr.e_shentsize);
                            memset((uint8_t *)&shdr + hdr.e_shentsize, 0, sizeof(shdr) - hdr.e_shentsize);
                        }
                        else {
                            memcpy(&shdr, shdr_buf + cnt % SHDR_BUF_SIZE * hdr.e_shentsize, sizeof(shdr));
                        }
                        if (file->byte_swap) {
                            SWAP(shdr.sh_name);
                            SWAP(shdr.sh_type);
                            SWAP(shdr.sh_flags);
                            SWAP(shdr.sh_addr);
                            SWAP(shdr.sh_offset);
                            SWAP(shdr.sh_size);
                            SWAP(shdr.sh_link);
                            SWAP(shdr.sh_info);
                            SWAP(shdr.sh_addralign);
                            SWAP(shdr.sh_entsize);
                        }
                        sec->file = file;
                        sec->index = cnt;
                        sec->name_offset = shdr.sh_name;
                        sec->type = shdr.sh_type;
                        sec->alignment = (U4_T)shdr.sh_addralign;
                        sec->offset = shdr.sh_offset;
                        sec->size = shdr.sh_size;
                        sec->flags = shdr.sh_flags;
                        sec->addr = shdr.sh_addr;
                        sec->link = shdr.sh_link;
                        sec->info = shdr.sh_info;
                        sec->entsize = shdr.sh_entsize;
                        if (sec->flags & SHF_COMPRESSED) {
                            struct {
                                uint32_t type;
                                uint32_t size;
                                uint32_t alignment;
                            } buf;
                            if (sec->size < sizeof(buf)) {
                                error = set_errno(ERR_INV_FORMAT, "Invalid compressed section header");
                                break;
                            }
                            sec->compressed_size = sec->size - sizeof(buf);
                            sec->compressed_offset = sec->offset + sizeof(buf);
                            sec->size = 0;
                            if (lseek(file->fd, sec->offset, SEEK_SET) == (off_t)-1 ||
                                read_fully(file->fd, &buf, sizeof(buf)) < 0) {
                                error = set_errno(errno, "Cannot read symbol file");
                                break;
                            }
                            if (file->byte_swap) {
                                SWAP(buf.type);
                                SWAP(buf.size);
                                SWAP(buf.alignment);
                            }
                            sec->compressed_type = buf.type;
                            sec->size = buf.size;
                            sec->alignment = buf.alignment;
                        }
                        if (sec->type == SHT_SYMTAB || sec->type == SHT_DYNSYM) {
                            sec->sym_count = (unsigned)(sec->size / sizeof(Elf32_Sym));
                        }
                        cnt++;
                    }
                }
            }
            if (error == 0 && hdr.e_phnum > 0) {
                if (lseek(file->fd, hdr.e_phoff, SEEK_SET) == (off_t)-1) error = errno;
                if (error == 0) {
                    unsigned cnt = 0;
                    file->pheaders = (ELF_PHeader *)loc_alloc_zero(sizeof(ELF_PHeader) * hdr.e_phnum);
                    file->pheader_cnt = hdr.e_phnum;
                    while (error == 0 && cnt < hdr.e_phnum) {
                        Elf32_Phdr phdr;
                        memset(&phdr, 0, sizeof(phdr));
                        if (error == 0 && sizeof(phdr) < hdr.e_phentsize) error = ERR_INV_FORMAT;
                        if (error == 0 && read_fully(file->fd, (char *)&phdr, hdr.e_phentsize) < 0) error = errno;
                        if (error == 0) {
                            ELF_PHeader * p = file->pheaders + cnt;
                            if (file->byte_swap) {
                                SWAP(phdr.p_type);
                                SWAP(phdr.p_offset);
                                SWAP(phdr.p_vaddr);
                                SWAP(phdr.p_paddr);
                                SWAP(phdr.p_filesz);
                                SWAP(phdr.p_memsz);
                                SWAP(phdr.p_flags);
                                SWAP(phdr.p_align);
                            }
                            p->type = phdr.p_type;
                            p->offset = phdr.p_offset;
                            p->address = phdr.p_vaddr;
                            p->physical_address = phdr.p_paddr;
                            p->file_size = phdr.p_filesz;
                            p->mem_size = phdr.p_memsz;
                            p->flags = phdr.p_flags;
                            p->align = phdr.p_align;
                            cnt++;
                        }
                    }
                }
            }
            str_index = hdr.e_shstrndx;
        }
        else if (hdr.e_ident[EI_CLASS] == ELFCLASS64) {
            Elf64_Ehdr hdr;
            file->elf64 = 1;
            memset(&hdr, 0, sizeof(hdr));
            if (error == 0 && lseek(file->fd, 0, SEEK_SET) == (off_t)-1) error = errno;
            if (error == 0 && read_fully(file->fd, (char *)&hdr, sizeof(hdr)) < 0) error = errno;
            if (file->byte_swap) {
                SWAP(hdr.e_type);
                SWAP(hdr.e_machine);
                SWAP(hdr.e_version);
                SWAP(hdr.e_entry);
                SWAP(hdr.e_phoff);
                SWAP(hdr.e_shoff);
                SWAP(hdr.e_flags);
                SWAP(hdr.e_ehsize);
                SWAP(hdr.e_phentsize);
                SWAP(hdr.e_phnum);
                SWAP(hdr.e_shentsize);
                SWAP(hdr.e_shnum);
                SWAP(hdr.e_shstrndx);
            }
            file->type = hdr.e_type;
            file->machine = hdr.e_machine;
            file->flags = hdr.e_flags;
            file->os_abi = hdr.e_ident[EI_OSABI];
            file->entry_address = (ContextAddress)hdr.e_entry;
            if (error == 0 && hdr.e_type != ET_EXEC && hdr.e_type != ET_DYN && hdr.e_type != ET_REL) {
                error = set_errno(ERR_INV_FORMAT, "Invalid ELF type ID");
            }
            if (error == 0 && hdr.e_version != EV_CURRENT) {
                error = set_errno(ERR_INV_FORMAT, "Unsupported ELF version");
            }
            if (error == 0 && hdr.e_shoff == 0) {
                error = set_errno(ERR_INV_FORMAT, "Invalid section header table's file offset");
            }
            if (error == 0) {
                unsigned cnt = 0;
                uint8_t * shdr_buf = (uint8_t *)tmp_alloc(hdr.e_shentsize * SHDR_BUF_SIZE);
                file->sections = (ELF_Section *)loc_alloc_zero(sizeof(ELF_Section) * hdr.e_shnum);
                file->section_cnt = hdr.e_shnum;
                while (error == 0 && cnt < hdr.e_shnum) {
                    if (cnt % SHDR_BUF_SIZE == 0) {
                        unsigned n = hdr.e_shnum - cnt;
                        if (n > SHDR_BUF_SIZE) n = SHDR_BUF_SIZE;
                        if (error == 0 && lseek(file->fd, hdr.e_shoff + cnt * hdr.e_shentsize, SEEK_SET) == (off_t)-1) error = errno;
                        if (error == 0 && read_fully(file->fd, shdr_buf, hdr.e_shentsize * n) < 0) error = errno;
                    }
                    if (error == 0) {
                        Elf64_Shdr shdr;
                        ELF_Section * sec = file->sections + cnt;
                        if (hdr.e_shentsize < sizeof(shdr)) {
                            memcpy(&shdr, shdr_buf + cnt % SHDR_BUF_SIZE * hdr.e_shentsize, hdr.e_shentsize);
                            memset((uint8_t *)&shdr + hdr.e_shentsize, 0, sizeof(shdr) - hdr.e_shentsize);
                        }
                        else {
                            memcpy(&shdr, shdr_buf + cnt % SHDR_BUF_SIZE * hdr.e_shentsize, sizeof(shdr));
                        }
                        if (file->byte_swap) {
                            SWAP(shdr.sh_name);
                            SWAP(shdr.sh_type);
                            SWAP(shdr.sh_flags);
                            SWAP(shdr.sh_addr);
                            SWAP(shdr.sh_offset);
                            SWAP(shdr.sh_size);
                            SWAP(shdr.sh_link);
                            SWAP(shdr.sh_info);
                            SWAP(shdr.sh_addralign);
                            SWAP(shdr.sh_entsize);
                        }
                        sec->file = file;
                        sec->index = cnt;
                        sec->name_offset = shdr.sh_name;
                        sec->type = shdr.sh_type;
                        sec->alignment = (U4_T)shdr.sh_addralign;
                        sec->offset = shdr.sh_offset;
                        sec->size = shdr.sh_size;
                        sec->flags = (U4_T)shdr.sh_flags;
                        sec->addr = shdr.sh_addr;
                        sec->link = shdr.sh_link;
                        sec->info = shdr.sh_info;
                        sec->entsize = (U4_T)shdr.sh_entsize;
                        if (sec->flags & SHF_COMPRESSED) {
                            struct {
                                uint32_t type;
                                uint32_t reserved;
                                uint64_t size;
                                uint64_t alignment;
                            } buf;
                            if (sec->size < sizeof(buf)) {
                                error = set_errno(ERR_INV_FORMAT, "Invalid compressed section header");
                                break;
                            }
                            sec->compressed_size = sec->size - sizeof(buf);
                            sec->compressed_offset = sec->offset + sizeof(buf);
                            sec->size = 0;
                            if (lseek(file->fd, sec->offset, SEEK_SET) == (off_t)-1 ||
                                read_fully(file->fd, &buf, sizeof(buf)) < 0) {
                                error = set_errno(errno, "Cannot read symbol file");
                                break;
                            }
                            if (file->byte_swap) {
                                SWAP(buf.type);
                                SWAP(buf.size);
                                SWAP(buf.alignment);
                            }
                            sec->compressed_type = buf.type;
                            sec->size = buf.size;
                            sec->alignment = (U4_T)buf.alignment;
                        }
                        if (sec->type == SHT_SYMTAB || sec->type == SHT_DYNSYM) {
                            sec->sym_count = (unsigned)(sec->size / sizeof(Elf64_Sym));
                        }
                        cnt++;
                    }
                }
            }
            if (error == 0 && hdr.e_phnum > 0) {
                if (lseek(file->fd, hdr.e_phoff, SEEK_SET) == (off_t)-1) error = errno;
                if (error == 0) {
                    unsigned cnt = 0;
                    file->pheaders = (ELF_PHeader *)loc_alloc_zero(sizeof(ELF_PHeader) * hdr.e_phnum);
                    file->pheader_cnt = hdr.e_phnum;
                    while (error == 0 && cnt < hdr.e_phnum) {
                        Elf64_Phdr phdr;
                        memset(&phdr, 0, sizeof(phdr));
                        if (error == 0 && sizeof(phdr) < hdr.e_phentsize) error = ERR_INV_FORMAT;
                        if (error == 0 && read_fully(file->fd, (char *)&phdr, hdr.e_phentsize) < 0) error = errno;
                        if (error == 0) {
                            ELF_PHeader * p = file->pheaders + cnt;
                            if (file->byte_swap) {
                                SWAP(phdr.p_type);
                                SWAP(phdr.p_offset);
                                SWAP(phdr.p_vaddr);
                                SWAP(phdr.p_paddr);
                                SWAP(phdr.p_filesz);
                                SWAP(phdr.p_memsz);
                                SWAP(phdr.p_flags);
                                SWAP(phdr.p_align);
                            }
                            p->type = phdr.p_type;
                            p->offset = phdr.p_offset;
                            p->address = phdr.p_vaddr;
                            p->physical_address = phdr.p_paddr;
                            p->file_size = phdr.p_filesz;
                            p->mem_size = phdr.p_memsz;
                            p->flags = phdr.p_flags;
                            p->align = (U4_T)phdr.p_align;
                            cnt++;
                        }
                    }
                }
            }
            str_index = hdr.e_shstrndx;
        }
        else {
            error = set_errno(ERR_INV_FORMAT, "Invalid ELF class ID");
        }
        if (error == 0 && str_index != 0 && str_index < file->section_cnt) {
            ELF_Section * str = file->sections + str_index;
            file->str_pool = (char *)loc_alloc((size_t)str->size);
            if (str->offset == 0 || str->size == 0) error = set_errno(ERR_INV_FORMAT, "Invalid ELF string pool offset or size");
            if (error == 0 && lseek(file->fd, str->offset, SEEK_SET) == (off_t)-1) error = errno;
            if (error == 0 && read_fully(file->fd, file->str_pool, (size_t)str->size) < 0) error = errno;
            if (error == 0) {
                unsigned i;
                for (i = 1; i < file->section_cnt; i++) {
                    ELF_Section * sec = file->sections + i;
                    sec->name = file->str_pool + sec->name_offset;
                }
            }
        }
    }
    if (error == 0) {
        unsigned m = 0;
        file->section_opd = 0;
        for (m = 1; m < file->section_cnt; m++) {
            ELF_Section * tbl = file->sections + m;
            if (file->machine == EM_PPC64 && strcmp(tbl->name, ".opd") == 0) file->section_opd = m;
            if (tbl->sym_count == 0) continue;
            if (create_symbol_names_hash(tbl) < 0) {
                error = errno;
                break;
            }
        }
    }
    if (error == 0) {
        file->debug_info_file = is_debug_info_file(file);
        if (!file->debug_info_file) file->debug_info_file_name = get_debug_info_file_name(file, &error);
        if (file->debug_info_file_name) trace(LOG_ELF, "Debug info file found %s", file->debug_info_file_name);
        if (file->machine == EM_PPC64) {
            if ((file->flags & 0x3) < 2 && file->section_opd == 0)
                error = set_errno(ERR_INV_FORMAT, "PPC64 ELFv1 file must contain an OPD section");
            if ((file->flags & 0x3) == 2 && file->section_opd != 0)
                error = set_errno(ERR_INV_FORMAT, "PPC64 ELFv2 file should not contain an OPD section");
        }
    }
    if (error == 0) {
        file->dwz_file_name = get_dwz_file_name(file, &error);
        if (file->dwz_file_name) trace(LOG_ELF, "DWZ file found %s", file->dwz_file_name);
    }
    if (error != 0) {
        trace(LOG_ELF, "Error opening ELF file: %d %s", error, errno_to_str(error));
        file->error = get_error_report(error);
    }
    if (!elf_cleanup_posted) {
        post_event_with_delay(elf_cleanup_event, NULL, 1000000);
        elf_cleanup_posted = 1;
    }
    free(real_name);
    file->next = files;
    return files = file;
}

ELF_File * elf_open(const char * file_name) {
    ELF_File * file = create_elf_cache(file_name);
    unsigned n = 0;

    for (n = 0; n < openlisteners_cnt; n++) {
        openlisteners[n](file);
    }
    if (file->error == NULL) return file;
    set_error_report_errno(file->error);
    return NULL;
}

int elf_load(ELF_Section * s) {

    if (s->data != NULL) return 0;
    if (s->size == 0) return 0;

    s->relocate = NULL;
    if (s->type != SHT_REL && s->type != SHT_RELA) {
        unsigned i;
        for (i = 1; i < s->file->section_cnt; i++) {
            ELF_Section * r = s->file->sections + i;
            if (r->entsize == 0 || r->size == 0) continue;
            if (r->type != SHT_REL && r->type != SHT_RELA) continue;
            if (r->info == s->index) {
                s->relocate = r;
                break;
            }
        }
    }

#if USE_MMAP
#if defined(_WIN32) || defined(__CYGWIN__)
    if (s->size >= 0x100000 && (s->flags & SHF_COMPRESSED) == 0) {
        ELF_File * file = s->file;
        if (file->mmap_handle == NULL) {
            file->mmap_handle = CreateFileMapping(
                (HANDLE)_get_osfhandle(file->fd), NULL, PAGE_READONLY,
                (DWORD)(file->size >> 32), (DWORD)file->size, NULL);
            if (file->mmap_handle == NULL) {
                trace(LOG_ALWAYS, "Cannot create file mapping object: %s",
                    errno_to_str(set_win32_errno(GetLastError())));
            }
        }
        if (file->mmap_handle != NULL) {
            SYSTEM_INFO info;
            U8_T offs = s->offset;
            GetSystemInfo(&info);
            offs -= offs % info.dwAllocationGranularity;
            s->mmap_size = (size_t)(s->offset - offs + s->size);
            s->mmap_addr = MapViewOfFile(file->mmap_handle, FILE_MAP_READ,
                (DWORD)(offs >> 32), (DWORD)offs, s->mmap_size);
            if (s->mmap_addr == NULL) {
                trace(LOG_ALWAYS, "Cannot create file mapping view: %s",
                    errno_to_str(set_win32_errno(GetLastError())));
            }
            else {
                s->data = (char *)s->mmap_addr + (size_t)(s->offset - offs);
                trace(LOG_ELF, "Section %s in ELF file %s is mapped to %#" PRIxPTR, s->name, s->file->name, (uintptr_t)s->data);
            }
        }
    }
#else
    if (s->size >= 0x100000 && (s->flags & SHF_COMPRESSED) == 0) {
        long page = sysconf(_SC_PAGE_SIZE);
        off_t offs = (off_t)s->offset;
        offs -= offs % page;
        s->mmap_size = (size_t)(s->offset - offs + s->size);
        s->mmap_addr = mmap(0, s->mmap_size, PROT_READ, MAP_PRIVATE, s->file->fd, offs);
        if (s->mmap_addr == MAP_FAILED) {
            s->mmap_addr = NULL;
            trace(LOG_ALWAYS, "Cannot mmap section %s in ELF file %s", s->name, s->file->name);
        }
        else {
            s->data = (char *)s->mmap_addr + (size_t)(s->offset - offs);
            trace(LOG_ELF, "Section %s in ELF file %s is mapped to %#" PRIxPTR, s->name, s->file->name, (uintptr_t)s->data);
        }
    }
#endif
#endif

    if (s->data == NULL) {
        ELF_File * file = s->file;
        reopen_file(file);
        if (file->error) {
            set_error_report_errno(file->error);
            return -1;
        }
        if (s->flags & SHF_COMPRESSED) {
            if (s->compressed_type == ELFCOMPRESS_ZLIB && s->compressed_size > 6) {
                uint8_t * buf = (uint8_t *)loc_alloc((size_t)s->compressed_size);
                if (lseek(file->fd, s->compressed_offset, SEEK_SET) == (off_t)-1 ||
                    read_fully(file->fd, buf, (size_t)s->compressed_size) < 0) {
                    int error = errno;
                    loc_free(buf);
                    set_errno(error, "Cannot read symbol file");
                    return -1;
                }
                else {
                    unsigned pos = 0;
                    unsigned cmf = buf[pos++];
                    unsigned flg = buf[pos++];
                    if ((cmf & 0xf) != 0x08) {
                        loc_free(buf);
                        set_errno(ERR_INV_DWARF, "Unsupported compression type");
                        return -1;
                    }
                    if ((cmf * 256 + flg) % 31 == 0 && (flg & 0x20) == 0) {
                        Trap trap;
                        s->data = loc_alloc_zero((size_t)s->size);
                        if (set_trap(&trap)) {
                            pos += decompress(buf + pos, (size_t)(s->compressed_size - pos), s->data, (size_t)s->size);
                            if (pos + 4 <= s->compressed_size) {
                                /* TODO: checksum verification */
                                clear_trap(&trap);
                                loc_free(buf);
                                return 0;
                            }
                        }
                        loc_free(s->data);
                        s->data = NULL;
                    }
                }
                loc_free(buf);
            }
            set_errno(ERR_INV_DWARF, "Corrupted compressed section");
            return -1;
        }
        s->data = loc_alloc((size_t)s->size);
        if (lseek(file->fd, s->offset, SEEK_SET) == (off_t)-1 ||
                read_fully(file->fd, s->data, (size_t)s->size) < 0) {
            int error = errno;
            loc_free(s->data);
            s->data = NULL;
            set_errno(error, "Cannot read symbol file");
            return -1;
        }
        trace(LOG_ELF, "Section %s in ELF file %s is loaded", s->name, s->file->name);
    }
    return 0;
}

ELF_File * get_dwarf_file(ELF_File * file) {
    if (file != NULL && file->debug_info_file_name != NULL) {
        ELF_File * debug = elf_open(file->debug_info_file_name);
        if (debug != NULL) {
            /* See https://bugs.eclipse.org/bugs/show_bug.cgi?id=446410 */
            if (debug != file && debug->debug_info_file_name != NULL) {
                debug = elf_open(debug->debug_info_file_name);
            }
            if (debug != NULL && debug->debug_info_file) return debug;
        }
    }
    return file;
}

#if ENABLE_DebugContext

static U8_T get_pheader_file_size(ELF_File * file, ELF_PHeader * p, MemoryRegion * r) {
    assert(p >= file->pheaders && p < file->pheaders + file->pheader_cnt);
    /* p->file_size is not valid if the file is debug info file */
    if (file->debug_info_file) {
        ELF_File * exec = elf_open_memory_region_file(r, NULL);
        if (get_dwarf_file(exec) == file) {
            unsigned i;
            for (i = 0; i < exec->pheader_cnt; i++) {
                ELF_PHeader * q = exec->pheaders + i;
                if (q->type == p->type && q->offset == p->offset)
                    return q->file_size;
            }
        }
        return 0;
    }
    return p->file_size;
}

static U8_T get_debug_pheader_address(ELF_File * file, ELF_File * debug, ELF_PHeader * p) {
    assert(p >= file->pheaders && p < file->pheaders + file->pheader_cnt);
    /* p->address is not valid if the file is exec file with split debug info */
    if (file != debug) {
        unsigned i;
        for (i = 0; i < debug->pheader_cnt; i++) {
            ELF_PHeader * q = debug->pheaders + i;
            if (q->type == p->type && q->offset == p->offset)
                return q->address;
        }
    }
    return p->address;
}

ELF_File * elf_open_memory_region_file(MemoryRegion * r, int * error) {
    ELF_File * file = NULL;
    ino_t ino = r->ino;
    dev_t dev = r->dev;

    if (dev != 0) {
        if (ino == 0 && r->file_name != NULL) ino = elf_ino(r->file_name);
        if (ino != 0) file = find_open_file_by_inode(dev, ino, 0);
    }
    if (file == NULL) {
        if (r->file_name == NULL) return NULL;
        file = create_elf_cache(r->file_name);
    }
    if (file->error == NULL) {
        if (r->dev != 0 && file->dev != r->dev) return NULL;
        if (r->ino != 0 && file->ino != r->ino) return NULL;
        return file;
    }
    if (error != NULL && *error == 0) {
        int no = set_error_report_errno(file->error);
        if (get_error_code(no) != ERR_INV_FORMAT) *error = no;
    }
    return NULL;
}

static MemoryRegion * add_region(MemoryMap * map) {
    MemoryRegion * r = NULL;
    if (map->region_cnt >= map->region_max) {
        map->region_max += 8;
        map->regions = (MemoryRegion *)loc_realloc(map->regions, sizeof(MemoryRegion) * map->region_max);
    }
    r = map->regions + map->region_cnt++;
    memset(r, 0, sizeof(MemoryRegion));
    return r;
}

static void program_headers_ranges(ELF_File * file, ContextAddress addr0, ContextAddress addr1, MemoryMap * res) {
    unsigned j;
    for (j = 0; j < file->pheader_cnt; j++) {
        ELF_PHeader * p = file->pheaders + j;
        if (p->mem_size == 0) continue;
        if (p->type != PT_LOAD) continue;
        if (p->address <= addr1 && p->address + p->mem_size - 1 >= addr0) {
            MemoryRegion * x = add_region(res);
            x->addr = (ContextAddress)p->address;
            x->size = (ContextAddress)p->mem_size;
            x->dev = file->dev;
            x->ino = file->ino;
            x->file_name = file->name;
            x->file_offs = p->offset;
            x->file_size = p->file_size;
            x->bss = p->file_size == 0 && p->mem_size != 0;
            if (p->flags & PF_R) x->flags |= MM_FLAG_R;
            if (p->flags & PF_W) x->flags |= MM_FLAG_W;
            if (p->flags & PF_X) x->flags |= MM_FLAG_X;
            x->valid = MM_VALID_ADDR | MM_VALID_SIZE | MM_VALID_FILE_OFFS | MM_VALID_FILE_SIZE;
        }
    }
}

static void read_module_struct(InputStream * inp, const char * name, void * args) {
    KernelModuleAddress * module = (KernelModuleAddress *)args;
    if (strcmp(name, "Init") == 0) module->module_init = json_read_uint64(inp);
    else if (strcmp(name, "Core") == 0) module->module_core = json_read_uint64(inp);
    else if (strcmp(name, "InitSize") == 0) module->init_size = json_read_uint64(inp);
    else if (strcmp(name, "CoreSize") == 0) module->core_size = json_read_uint64(inp);
    else if (strcmp(name, "InitTextSize") == 0) module->init_text_size = json_read_uint64(inp);
    else if (strcmp(name, "CoreTextSize") == 0) module->core_text_size = json_read_uint64(inp);
    else if (strcmp(name, "InitROSize") == 0) module->init_ro_size = json_read_uint64(inp);
    else if (strcmp(name, "CoreROSize") == 0) module->core_ro_size = json_read_uint64(inp);
    else json_skip_object(inp);
}

static void linux_kernel_module_ranges(ELF_File * file, KernelModuleAddress * module, ContextAddress addr0, ContextAddress addr1, MemoryMap * res) {
    unsigned i, m;
    U8_T * sec_addr = (U8_T *)tmp_alloc_zero(file->section_cnt * 8);
    static const U4_T masks[][2] = {
            { SHF_EXECINSTR | SHF_ALLOC, ARCH_SHF_SMALL },
            { SHF_ALLOC, SHF_WRITE | ARCH_SHF_SMALL },
            { SHF_WRITE | SHF_ALLOC, ARCH_SHF_SMALL },
            { ARCH_SHF_SMALL | SHF_ALLOC, 0 }
    };

    if (module->module_core != 0) {
        /* Core section allocation */
        U8_T addr = module->module_core;
        for (m = 0; m < 4; ++m) {
            for (i = 0; i < file->section_cnt; i++) {
                ELF_Section * s = file->sections + i;
                if (sec_addr[i] != 0) continue;
                if (s->flags & masks[m][1]) continue;
                if ((s->flags & masks[m][0]) != masks[m][0]) continue;
                if (strcmp(s->name, ".data..percpu") == 0) continue;
                if (strcmp(s->name, ".modinfo") == 0) continue;
                if (strncmp(s->name, ".init", 5) == 0) continue;
                if (s->alignment > 1) {
                    U8_T alignment = s->alignment - 1;
                    if (addr & alignment) addr = (addr | alignment) + 1;
                }
                sec_addr[i] = addr;
                addr += s->size;
            }
            switch (m) {
            case 0: /* text */
                addr = module->module_core + module->core_text_size;
                break;
            case 1: /* read only */
                addr = module->module_core + module->core_ro_size;
                break;
            }
        }
    }

    if (module->module_init != 0) {
        /* Init section allocation */
        U8_T addr = module->module_init;
        for (m = 0; m < 4; ++m) {
            for (i = 0; i < file->section_cnt; i++) {
                ELF_Section * s = file->sections + i;
                if (sec_addr[i] != 0) continue;
                if (s->flags & masks[m][1]) continue;
                if ((s->flags & masks[m][0]) != masks[m][0]) continue;
                if (strcmp(s->name, ".data..percpu") == 0) continue;
                if (strcmp(s->name, ".modinfo") == 0) continue;
                if (strncmp(s->name, ".init", 5) != 0) continue;
                if (s->alignment > 1) {
                    U8_T alignment = s->alignment - 1;
                    if (addr & alignment) addr = (addr | alignment) + 1;
                }
                sec_addr[i] = addr;
                addr += s->size;
            }
            switch (m) {
            case 0: /* text */
                addr = module->module_init + module->init_text_size;
                break;
            case 1: /* read only */
                addr = module->module_init + module->init_ro_size;
                break;
            }
        }
    }

    for (i = 0; i < file->section_cnt; i++) {
        if (sec_addr[i] != 0) {
            ELF_Section * s = file->sections + i;
            if (s->size > 0) {
                ContextAddress s_addr0 = (ContextAddress)sec_addr[i];
                ContextAddress s_addr1 = (ContextAddress)(sec_addr[i] + s->size - 1);
                if (s_addr0 <= addr1 && s_addr1 >= addr0) {
                    MemoryRegion * r = add_region(res);
                    r->addr = s_addr0;
                    r->size = (ContextAddress)s->size;
                    r->sect_name = s->name;
                    r->file_name = file->name;
                    r->dev = file->dev;
                    r->ino = file->ino;
                    r->flags |= MM_FLAG_R;
                    if (s->flags & SHF_WRITE) r->flags |= MM_FLAG_W;
                    if (s->flags & SHF_EXECINSTR) r->flags |= MM_FLAG_X;
                    r->valid = MM_VALID_ADDR | MM_VALID_SIZE;
                }
            }
        }
    }
}

static void search_regions(MemoryMap * map, ContextAddress addr0, ContextAddress addr1, MemoryMap * res) {
    unsigned i;
    for (i = 0; i < map->region_cnt; i++) {
        MemoryRegion * r = map->regions + i;
        int no_addr = r->addr == 0 && (r->valid & MM_VALID_ADDR) == 0;
        int no_size = r->size == 0 && (r->valid & MM_VALID_SIZE) == 0;
        int no_file_offs = r->file_offs == 0 && (r->valid & MM_VALID_FILE_OFFS) == 0;
        int no_file_size = r->file_size == 0 && (r->valid & MM_VALID_FILE_SIZE) == 0;
        if (r->file_name == NULL) continue;
        if (no_addr && no_size && no_file_offs && no_file_size && r->sect_name == NULL) {
            ELF_File * file = elf_open_memory_region_file(r, NULL);
            if (file != NULL) {
                KernelModuleAddress * module = NULL;
                if (r->attrs != NULL) {
                    MemoryRegionAttribute * a = r->attrs;
                    while (a != NULL) {
                        if (strcmp(a->name, "KernelModule") == 0) {
                            ByteArrayInputStream buf;
                            InputStream * inp = create_byte_array_input_stream(&buf, a->value, strlen(a->value));
                            module = (KernelModuleAddress *)tmp_alloc_zero(sizeof(KernelModuleAddress));
                            json_read_struct(inp, read_module_struct, module);
                            json_test_char(inp, MARKER_EOS);
                            break;
                        }
                        a = a->next;
                    }
                }
                if (module != NULL) {
                    linux_kernel_module_ranges(file, module, addr0, addr1, res);
                }
                else {
                    program_headers_ranges(file, addr0, addr1, res);
                }
            }
        }
        else if (no_size && no_file_size && r->sect_name == NULL) {
            if (no_file_offs) {
                /* Linux module (shared library): r->addr is "memory load address".
                 * See System V Application Binary Interface for description of
                 * "memory load address" and "base address" */
                ELF_File * file = elf_open_memory_region_file(r, NULL);
                if (file != NULL) {
                    unsigned j;
                    uint64_t base_offs = ~(uint64_t)0;
                    ContextAddress base_addr = 0;
                    for (j = 0; j < file->pheader_cnt; j++) {
                        ELF_PHeader * p = file->pheaders + j;
                        if (p->type == PT_LOAD && p->offset < base_offs) {
                            base_addr = r->addr - (ContextAddress)p->address;
                            base_offs = p->offset;
                            if (p->offset == 0) break;
                        }
                    }
                    for (j = 0; j < file->pheader_cnt; j++) {
                        ELF_PHeader * p = file->pheaders + j;
                        if (p->type == PT_LOAD && p->mem_size > 0) {
                            ContextAddress p_addr0 = (ContextAddress)p->address + base_addr;
                            ContextAddress p_addr1 = (ContextAddress)(p->address + p->mem_size - 1) + base_addr;
                            if (p_addr0 <= addr1 && p_addr1 >= addr0) {
                                MemoryRegion * x = add_region(res);
                                x->addr = p_addr0;
                                x->size = (ContextAddress)p->mem_size;
                                x->dev = file->dev;
                                x->ino = file->ino;
                                x->file_name = file->name;
                                x->file_offs = p->offset;
                                x->file_size = p->file_size;
                                x->flags = MM_FLAG_R | MM_FLAG_W | MM_FLAG_X;
                                x->valid = MM_VALID_ADDR | MM_VALID_SIZE | MM_VALID_FILE_OFFS | MM_VALID_FILE_SIZE;
                            }
                        }
                    }
                }
            }
        }
        else if (r->addr <= addr1 && no_size && r->sect_name != NULL) {
            ELF_File * file = elf_open_memory_region_file(r, NULL);
            if (file != NULL) {
                unsigned j;
                for (j = 0; j < file->section_cnt; j++) {
                    ELF_Section * s = file->sections + j;
                    if (s == NULL || s->name == NULL || s->size == 0) continue;
                    if (r->addr + s->size - 1 < addr0) continue;
                    if (strcmp(s->name, r->sect_name) == 0) {
                        MemoryRegion * x = add_region(res);
                        x->addr = r->addr;
                        x->size = (ContextAddress)s->size;
                        x->dev = file->dev;
                        x->ino = file->ino;
                        x->file_name = file->name;
                        x->sect_name = s->name;
                        x->flags = r->flags;
                        if (x->flags == 0) x->flags = MM_FLAG_R | MM_FLAG_W | MM_FLAG_X;
                        x->valid = MM_VALID_ADDR | MM_VALID_SIZE;
                    }
                }
            }
        }
        else if (r->size > 0 && r->addr <= addr1 && r->addr + r->size - 1 >= addr0) {
            /* Even if the addresses are in the given range, we MUST check that the
             * region actually belongs to an ELF file. Without the check below, we
             * could end up using the ELF reader even if the region belongs to a
             * PE/COFF file.
             */
            const ELF_File * file = elf_open_memory_region_file(r, NULL);
            if (file) {
                *add_region(res) = *r;
            }
        }
        else if (r->size == 0 && r->sect_name != NULL && r->addr <= addr1 && r->addr >= addr0) {
            /* Special case: section symbol of a zero length section */
            *add_region(res) = *r;
        }
    }
}

int elf_get_map(Context * ctx, ContextAddress addr0, ContextAddress addr1, MemoryMap * map) {
    unsigned i;
    map->region_cnt = 0;
    ctx = context_get_group(ctx, CONTEXT_GROUP_PROCESS);
#if ENABLE_MemoryMap
    {
        MemoryMap * client_map = NULL;
        MemoryMap * target_map = NULL;
        if (memory_map_get(ctx, &client_map, &target_map) < 0) return -1;
        search_regions(client_map, addr0, addr1, map);
        search_regions(target_map, addr0, addr1, map);
    }
#else
    {
        int error = 0;
        MemoryMap target_map;
        memset(&target_map, 0, sizeof(target_map));
        if (context_get_memory_map(ctx, &target_map) < 0) error = errno;
        if (!error) search_regions(&target_map, addr0, addr1, map);
        context_clear_memory_map(&target_map);
        loc_free(target_map.regions);
        if (error) {
            errno = error;
            return -1;
        }
    }
#endif
    for (i = 0; i + 1 < map->region_cnt; i++) {
        MemoryRegion * m = map->regions + i;
        MemoryRegion * r = m + 1;
        if (m->file_size == m->size && m->file_offs < r->file_offs && m->file_offs + m->file_size > r->file_offs &&
                m->file_name != NULL && r->file_name != NULL && strcmp(m->file_name, r->file_name) == 0) {
            /* Ambiguity: overlapping regions */
            ELF_File * elf = elf_open(r->file_name);
            for (i = 0; i < elf->pheader_cnt; i++) {
                ELF_PHeader * p = elf->pheaders + i;
                if (p->type == PT_LOAD && p->offset == m->file_offs && p->mem_size < m->size) {
                    m->file_size = p->mem_size;
                    m->size = p->mem_size;
                    if (m->file_offs + m->size > r->file_offs && p->mem_size == p->file_size) {
                        uint64_t diff = m->file_offs + m->file_size - r->file_offs;
                        r->file_offs += diff;
                        r->file_size -= diff;
                        r->addr += diff;
                        r->size -= diff;
                    }
                    break;
                }
            }
        }
    }
    return 0;
}

ELF_File * elf_open_inode(Context * ctx, dev_t dev, ino_t ino, int64_t mtime) {
    unsigned i;
    int error = 0;
    ELF_File * file = find_open_file_by_inode(dev, ino, mtime);
    if (file != NULL) {
        if (file->error == NULL) return file;
        set_error_report_errno(file->error);
        return NULL;
    }
    if (elf_get_map(ctx, 0, ~(ContextAddress)0, &elf_map) < 0) return NULL;
    for (i = 0; i < elf_map.region_cnt; i++) {
        MemoryRegion * r = elf_map.regions + i;
        file = elf_open_memory_region_file(r, &error);
        if (file == NULL) continue;
        if (file->dev == dev && file->ino == ino && file->mtime == mtime) return file;
        file = get_dwarf_file(file);
        if (file->dev == dev && file->ino == ino && file->mtime == mtime) return file;
    }
    if (error == 0) error = ENOENT;
    errno = error;
    return NULL;
}

ELF_File * elf_list_first(Context * ctx, ContextAddress addr_min, ContextAddress addr_max) {
    ElfListState * state = (ElfListState *)loc_alloc_zero(sizeof(ElfListState));
    state->next = elf_list_state;
    elf_list_state = state;
    state->ctx = ctx;
    if (elf_get_map(ctx, addr_min, addr_max, &state->map) < 0) return NULL;
    if (state->map.region_cnt > 0) {
        ELF_File * f = files;
        while (f != NULL) {
            f->listed = 0;
            f = f->next;
        }
        return elf_list_next(ctx);
    }
    errno = 0;
    return NULL;
}

MemoryRegion * elf_list_region(Context * ctx) {
    ElfListState * state = elf_list_state;
    assert(state != NULL);
    assert(state->ctx == ctx);
    return state->region;
}

ELF_File * elf_list_next(Context * ctx) {
    ElfListState * state = elf_list_state;
    assert(state != NULL);
    assert(state->ctx == ctx);
    assert(state->map.region_cnt > 0);
    while (state->pos < state->map.region_cnt) {
        int error = 0;
        MemoryRegion * r = state->map.regions + state->pos++;
        ELF_File * file = elf_open_memory_region_file(r, &error);
        if (file != NULL) {
            if (file->listed) continue;
            state->region = r;
            file->listed = 1;
            return file;
        }
        if (error && r->id == NULL && get_error_code(error) != ENOENT) {
            errno = error;
            return NULL;
        }
    }
    errno = 0;
    return NULL;
}

void elf_list_done(Context * ctx) {
    ElfListState * state = elf_list_state;
    assert(state != NULL);
    assert(state->ctx == ctx);
    elf_list_state = state->next;
    free_elf_list_state(state);
}

static ELF_Section * find_section_by_offset(ELF_File * file, U8_T offs) {
    unsigned i;
    /* Note: debug info file has invalid section offsets */
    assert(!file->debug_info_file);
    for (i = 1; i < file->section_cnt; i++) {
        ELF_Section * sec = file->sections + i;
        if (sec->size == 0) continue;
        if (offs < sec->offset) continue;
        if (offs < sec->offset + sec->size) return sec;
    }
    return NULL;
}

static ELF_Section * find_section_by_address(ELF_File * file, U8_T addr, int to_dwarf) {
    unsigned i;
    /* Note: section->addr not valid in exec file with split debug info */
    assert(!to_dwarf || get_dwarf_file(file) == file);
    for (i = 1; i < file->section_cnt; i++) {
        ELF_Section * s = file->sections + i;
        if ((s->flags & SHF_ALLOC) == 0) continue;
        if (s->addr <= addr && s->addr + s->size > addr) return s;
    }
    return NULL;
}

static int is_p_header_region(ELF_File * file, ELF_PHeader * p, MemoryRegion * r) {
    assert(p >= file->pheaders && p < file->pheaders + file->pheader_cnt);
    if (p->type != PT_LOAD) return 0;
    if (r->sect_name != NULL) return 0;
    if (r->flags) {
        if ((p->flags & PF_R) && !(r->flags & MM_FLAG_R)) return 0;
        if ((p->flags & PF_W) && !(r->flags & MM_FLAG_W)) return 0;
        if ((p->flags & PF_X) && !(r->flags & MM_FLAG_X)) return 0;
    }
    /*
     * Note: "file_size" is not set in most cases. The field is set and used internally
     * in tcf_elf.c, but it is not set in memory regions received from target or clients,
     * and it is not part of memory map service public API.
     * Nomally, ELF segment with file size < memory size is represented by 2 adjacent memory regions.
     */
    if (r->file_size > 0) {
        if (r->file_offs + r->file_size <= p->offset) return 0;
    }
    else if (r->bss) {
        if (get_pheader_file_size(file, p, r) >= p->mem_size) return 0;
        if (file->bss_segment_cnt == 0) {
            unsigned i;
            for (i = 0; i < file->pheader_cnt; i++) {
                ELF_PHeader * x = file->pheaders + i;
                if (x->type != PT_LOAD) continue;
                if (get_pheader_file_size(file, x, r) >= x->mem_size) continue;
                file->bss_segment_cnt++;
            }
        }
        assert(file->bss_segment_cnt > 0);
        if (file->bss_segment_cnt > 1) {
            /*
             * File has multiple BSS segments.
             * BSS segment is not required to have a valid file offset,
             * but we still use the offset to resolve ambiguity,
             * because there is no other way to differentiate such segments.
             */
            if (r->file_offs != p->offset + p->file_size) return 0;
        }
        return 1;
    }
    else {
        /* If not BSS, file size is supposed to be same as memory size */
        if (r->file_offs + r->size <= p->offset) return 0;
    }
    if (r->file_offs >= p->offset + get_pheader_file_size(file, p, r)) return 0;
    return 1;
}

UnitAddressRange * elf_find_unit(Context * ctx, ContextAddress addr_min, ContextAddress addr_max, ContextAddress * range_rt_addr) {
    unsigned i, j;
    UnitAddressRange * range = NULL;
    int error = 0;

    if (elf_get_map(ctx, addr_min, addr_max, &elf_map) < 0) return NULL;
    for (i = 0; range == NULL && i < elf_map.region_cnt; i++) {
        ContextAddress link_addr_min, link_addr_max;
        MemoryRegion * r = elf_map.regions + i;
        ELF_File * file = NULL;
        assert(r->addr <= addr_max);
        if (r->size == 0) continue;
        assert(r->addr + r->size - 1 >= addr_min);
        file = elf_open_memory_region_file(r, &error);
        if (file == NULL) {
            if (error) {
                if (r->id != NULL) continue;
                if (get_error_code(error) == ENOENT) continue;
                exception(error);
            }
            continue;
        }
        if (r->sect_name == NULL) {
            U8_T offs_min = r->file_offs;
            U8_T offs_max = r->file_offs + r->size - 1;
            ELF_File * debug = get_dwarf_file(file);
            if (addr_min > r->addr) offs_min = addr_min - r->addr + r->file_offs;
            if (addr_max < r->addr + r->size - 1) offs_max = addr_max - r->addr + r->file_offs;
            assert(offs_min <= offs_max);
            assert(offs_max >= r->file_offs);
            assert(offs_min <= r->file_offs + r->size - 1);
            for (j = 0; range == NULL && j < file->pheader_cnt; j++) {
                U8_T pheader_address = 0;
                U8_T pheader_file_size = 0;
                ELF_PHeader * p = file->pheaders + j;
                ELF_Section * sec = NULL;
                if (p->offset > offs_max) continue;
                if (!is_p_header_region(file, p, r)) continue;
                pheader_address = get_debug_pheader_address(file, debug, p);
                pheader_file_size = get_pheader_file_size(file, p, r);
                if (pheader_file_size == 0) continue;
                if (p->offset + pheader_file_size <= offs_min) continue;
                link_addr_min = (ContextAddress)(offs_min - p->offset + pheader_address);
                link_addr_max = (ContextAddress)(offs_max - p->offset + pheader_address);
                if (link_addr_min < pheader_address) link_addr_min = (ContextAddress)pheader_address;
                if (link_addr_max > pheader_address + pheader_file_size - 1) link_addr_max = (ContextAddress)(pheader_address + pheader_file_size - 1);
                if (!file->debug_info_file) sec = find_section_by_offset(file, offs_min);
                range = find_comp_unit_addr_range(get_dwarf_cache(debug), sec, link_addr_min, link_addr_max);
                if (range != NULL && range_rt_addr != NULL) {
                    *range_rt_addr = (ContextAddress)(range->mAddr - pheader_address + p->offset - r->file_offs + r->addr);
                }
            }
        }
        else {
            unsigned idx;
            ELF_File * debug = get_dwarf_file(file);
            for (idx = 1; range == NULL && idx < debug->section_cnt; idx++) {
                ELF_Section * sec = debug->sections + idx;
                if (sec->name != NULL && strcmp(sec->name, r->sect_name) == 0) {
                    if (addr_min < r->addr) link_addr_min = sec->addr;
                    else link_addr_min = (ContextAddress)(addr_min - r->addr + sec->addr);
                    if (addr_max > r->addr + r->size - 1) link_addr_max = sec->addr + r->size - 1;
                    else link_addr_max = (ContextAddress)(addr_max - r->addr + sec->addr);
                    assert(link_addr_min >= sec->addr);
                    assert(link_addr_max <= sec->addr + r->size - 1);
                    range = find_comp_unit_addr_range(get_dwarf_cache(debug), sec, link_addr_min, link_addr_max);
                    if (range != NULL && range_rt_addr != NULL) {
                        *range_rt_addr = (ContextAddress)(range->mAddr - sec->addr + r->addr);
                    }
                }
            }
        }
    }
    errno = 0;
    return range;
}

ContextAddress elf_run_time_address_in_region(Context * ctx, MemoryRegion * r, ELF_File * file, ELF_Section * sec, ContextAddress addr) {
    /* Note: when debug info is in a separate file, debug file link-time addresses are not same as exec file link-time addresses,
     * because Linux has a habbit of re-linking execs after extracting debug info */
    unsigned i;
    errno = 0;
    if (r->sect_name == NULL) {
        ContextAddress rt = 0;
        if (r->size == 0) {
            errno = ERR_INV_ADDRESS;
            return 0;
        }
        for (i = 0; i < file->pheader_cnt; i++) {
            ELF_PHeader * p = file->pheaders + i;
            if (!is_p_header_region(file, p, r)) continue;
            if (addr < p->address || addr >= p->address + p->mem_size) continue;
            rt = (ContextAddress)(addr - p->address + p->offset - r->file_offs + r->addr);
            if (rt < r->addr || rt > r->addr + r->size - 1) continue;
            return rt;
        }
    }
    else if (sec != NULL) {
        if (strcmp(sec->name, r->sect_name) == 0) {
            return (ContextAddress)(addr - sec->addr + r->addr);
        }
    }
    else if (file->type == ET_EXEC || file->type == ET_DYN) {
        for (i = 1; i < file->section_cnt; i++) {
            ELF_Section * s = file->sections + i;
            if (s->addr <= addr && s->addr + s->size > addr &&
                s->name != NULL && strcmp(s->name, r->sect_name) == 0) {
                return (ContextAddress)(addr - s->addr + r->addr);
            }
        }
    }
    errno = ERR_INV_ADDRESS;
    return 0;
}

ContextAddress elf_map_to_run_time_address(Context * ctx, ELF_File * file, ELF_Section * sec, ContextAddress addr) {
    unsigned i;
    unsigned cnt = 0;
    ContextAddress rt = 0;

    /* Note: 'addr' is link-time address - it cannot be used as elf_get_map() argument */
    if (elf_get_map(ctx, 0, ~(ContextAddress)0, &elf_map) < 0) return 0;
    for (i = 0; i < elf_map.region_cnt; i++) {
        MemoryRegion * r = elf_map.regions + i;
        ContextAddress a = 0;
        int same_file = 0;
        if (r->dev == 0) {
            same_file = file_name_equ(file, r->file_name);
        }
        else {
           ino_t ino = r->ino;
           if (ino == 0) ino = elf_ino(r->file_name);
           same_file = file->ino == ino && file->dev == r->dev;
        }
        if (!same_file) {
            /* Check if the memory map entry has a separate debug info file */
            ELF_File * exec = NULL;
            if (!file->debug_info_file) continue;
            exec = elf_open_memory_region_file(r, NULL);
            if (exec == NULL) continue;
            if (get_dwarf_file(exec) != file) continue;
        }
        a = elf_run_time_address_in_region(ctx, r, file, sec, addr);
        if (errno == 0) {
            rt = a;
            cnt++;
        }
    }
    if (cnt == 1) {
        errno = 0;
        return rt;
    }
    if (file->type == ET_EXEC) {
        errno = 0;
        return addr;
    }
    set_errno(ERR_INV_ADDRESS, "Invalid memory map, cannot compute run-time address");
    return 0;
}

ContextAddress elf_map_to_link_time_address(Context * ctx, ContextAddress addr, int to_dwarf, ELF_File ** file, ELF_Section ** sec) {
    unsigned i;
    unsigned cnt = 0;
    ContextAddress lt = 0;
    ELF_Section * exec_sec = NULL;

    if (elf_get_map(ctx, addr, addr, &elf_map) < 0) return 0;
    for (i = 0; i < elf_map.region_cnt; i++) {
        MemoryRegion * r = elf_map.regions + i;
        ELF_File * f = NULL;
        ELF_File * d = NULL;
        assert(r->addr <= addr);
        f = elf_open_memory_region_file(r, NULL);
        if (f == NULL) continue;
        d = to_dwarf ? get_dwarf_file(f) : f;
        if (r->sect_name == NULL) {
            unsigned j;
            if (r->size == 0) continue;
            assert(r->addr + r->size - 1 >= addr);
            if (exec_sec == NULL && f->type == ET_EXEC) {
                exec_sec = find_section_by_address(d, addr, to_dwarf);
            }
            for (j = 0; j < f->pheader_cnt; j++) {
                U8_T offs = addr - r->addr + r->file_offs;
                ELF_PHeader * p = f->pheaders + j;
                U8_T pheader_address = 0;
                if (!is_p_header_region(f, p, r)) continue;
                if (offs < p->offset || offs >= p->offset + p->mem_size) continue;
                pheader_address = get_debug_pheader_address(f, d, p);
                lt = (ContextAddress)(offs - p->offset + pheader_address);
                if (sec != NULL) *sec = find_section_by_address(d, lt, to_dwarf);
                *file = d;
                cnt++;
            }
        }
        else {
            unsigned j;
            for (j = 1; j < d->section_cnt; j++) {
                ELF_Section * s = d->sections + j;
                if (strcmp(s->name, r->sect_name) == 0) {
                    lt = (ContextAddress)(addr - r->addr + s->addr);
                    if (sec != NULL) *sec = s;
                    *file = d;
                    cnt++;
                }
            }
        }
    }
    if (cnt == 1) {
        assert(*file != NULL);
        errno = 0;
        return lt;
    }
    if (exec_sec != NULL) {
        *file = exec_sec->file;
        if (sec != NULL) *sec = exec_sec;
        errno = 0;
        return addr;
    }
    *file = NULL;
    if (sec != NULL) *sec = NULL;
    errno = 0;
    return 0;
}

int elf_read_memory_word(Context * ctx, ELF_File * file, ContextAddress addr, ContextAddress * word) {
    size_t size = file->elf64 ? 8 : 4;
    size_t i = 0;
    U8_T n = 0;
    U1_T buf[8];

    if (ctx->mem_access == 0) ctx = context_get_group(ctx, CONTEXT_GROUP_PROCESS);
    if (context_read_mem(ctx, addr, buf, size) < 0) return -1;
    for (i = 0; i < size; i++) {
        n = (n << 8) | buf[file->big_endian ? i : size - i - 1];
    }
    *word = (ContextAddress)n;
    return 0;
}

#endif /* ENABLE_DebugContext */


/************************ ELF symbol tables *****************************************/

unsigned calc_symbol_name_hash(const char * s) {
    unsigned h = 0;
    while (*s) {
        unsigned g;
        if (s[0] == '@' && s[1] == '@') break;
        if (s[0] == ' ' && (s[1] == '{' || s[1] == '(' || s[1] == '[')) {
            s++;
            continue;
        }
        h = (h << 4) + (unsigned char)*s++;
        g = h & 0xf0000000;
        if (g) h = (h ^ (g >> 24)) & ~g;
    }
    return h;
}

int cmp_symbol_names(const char * x, const char * y) {
    for (;;) {
        if (*x != *y) {
            if (*x == 0 && *y == '@' && y[1] == '@') return 0;
            if (*y == 0 && *x == '@' && x[1] == '@') return 0;
            if (*y == ' ' && (*x == '{' || *x == '(' || *x == '[')) {
                y++;
                continue;
            }
            if (*x == ' ' && (*y == '{' || *y == '(' || *y == '[')) {
                x++;
                continue;
            }
            break;
        }
        else if (*x == 0) {
            return 0;
        }
        else {
            x++;
            y++;
        }
    }
    if (*x < *y) return -1;
    if (*x > *y) return +1;
    return 0;
}

void unpack_elf_symbol_info(ELF_Section * sym_sec, U4_T index, ELF_SymbolInfo * info) {
    ELF_File * file = sym_sec->file;
    size_t st_name = 0;

    memset(info, 0, sizeof(ELF_SymbolInfo));
    if (index >= sym_sec->size / sym_sec->entsize) str_exception(ERR_INV_FORMAT, "Invalid ELF symbol index");
    if (elf_load(sym_sec) < 0) exception(errno);
    info->sym_section = sym_sec;
    info->sym_index = index;

    if (file->elf64) {
        Elf64_Sym s = *(Elf64_Sym *)((U1_T *)sym_sec->data + sym_sec->entsize * index);
        if (file->byte_swap) {
            SWAP(s.st_name);
            SWAP(s.st_shndx);
            SWAP(s.st_size);
            SWAP(s.st_value);
            SWAP(s.st_other);
        }
        st_name = (size_t)s.st_name;
        info->section_index = s.st_shndx;
        info->bind = ELF64_ST_BIND(s.st_info);
        info->type = ELF64_ST_TYPE(s.st_info);
        info->value = s.st_value;
        info->size = s.st_size;
        info->other = s.st_other;
    }
    else {
        Elf32_Sym s = *(Elf32_Sym *)((U1_T *)sym_sec->data + sym_sec->entsize * index);
        if (file->byte_swap) {
            SWAP(s.st_name);
            SWAP(s.st_shndx);
            SWAP(s.st_size);
            SWAP(s.st_value);
            SWAP(s.st_other);
        }
        st_name = (size_t)s.st_name;
        info->section_index = s.st_shndx;
        info->bind = ELF32_ST_BIND(s.st_info);
        info->type = ELF32_ST_TYPE(s.st_info);
        info->value = s.st_value;
        info->size = s.st_size;
        info->other = s.st_other;
    }

    if (info->section_index > 0 && info->section_index < file->section_cnt) {
        info->section = file->sections + info->section_index;
    }

    if (st_name > 0) {
        ELF_Section * str_sec = NULL;
        if (sym_sec->link == 0 || sym_sec->link >= file->section_cnt) str_exception(ERR_INV_FORMAT, "Invalid symbol section");
        str_sec = file->sections + sym_sec->link;
        if (st_name >= str_sec->size) str_exception(ERR_INV_FORMAT, "Invalid ELF string pool index");
        if (elf_load(str_sec) < 0) exception(errno);
        info->name = (char *)str_sec->data + (size_t)st_name;
    }

    if (info->name == NULL && info->type == STT_SECTION &&
            info->section != NULL && info->value == info->section->addr) {
        info->name = info->section->name;
    }

    if (file->machine == EM_ARM) {
        if (info->type == STT_ARM_16BIT) {
            info->type = STT_OBJECT;
            info->type16bit = 1;
        }
        if (info->type == STT_ARM_TFUNC) {
            info->type = STT_FUNC;
            info->type16bit = 1;
        }
        if (info->type == STT_FUNC || info->type16bit) {
            if (info->value & 1) info->type16bit = 1;
            info->value = info->value & ~(U8_T)1;
        }
    }
    else if (IS_PPC64_FUNC_OPD(file, info)) {
        info->type = STT_FUNC;
    }
}

static int create_symbol_names_hash(ELF_Section * tbl) {
    Trap trap;
    unsigned sym_size = tbl->file->elf64 ? sizeof(Elf64_Sym) : sizeof(Elf32_Sym);
    unsigned sym_cnt = (unsigned)(tbl->size / sym_size);
    tbl->sym_names_hash_size = sym_cnt;
    tbl->sym_names_hash = (unsigned *)loc_alloc_zero(sym_cnt * sizeof(unsigned));
    tbl->sym_names_next = (unsigned *)loc_alloc_zero(sym_cnt * sizeof(unsigned));
    /* unpack_elf_symbol_info() can throw exception */
    if (set_trap(&trap)) {
        unsigned i;
        for (i = 0; i < sym_cnt; i++) {
            ELF_SymbolInfo sym;
            unpack_elf_symbol_info(tbl, i, &sym);
            if (sym.name != NULL) {
                if (sym.bind == STB_GLOBAL && sym.name[0] == '_' && sym.name[1] == '_') {
                    if (strcmp(sym.name, "__GOTT_BASE__") == 0) tbl->file->vxworks_got = 1;
                    else if (strcmp(sym.name, "__GOTT_INDEX__") == 0) tbl->file->vxworks_got = 1;
                }
                if (sym.section_index != SHN_UNDEF && sym.type != STT_FILE) {
                    unsigned h = calc_symbol_name_hash(sym.name) % sym_cnt;
                    tbl->sym_names_next[i] = tbl->sym_names_hash[h];
                    tbl->sym_names_hash[h] = i;
                }
            }
        }
        clear_trap(&trap);
    }
    else if (tbl->type == SHT_DYNSYM && tbl->file->type != ET_DYN && get_error_code(trap.error) == ERR_INV_FORMAT) {
        /* Ignore brocken dynsym section if the file is not a dyn executable */
        trace(LOG_ELF, "Ignoring broken symbol section %s: %s.", tbl->name, errno_to_str(trap.error));
        tbl->sym_names_hash_size = 0;
        loc_free(tbl->sym_names_hash);
        loc_free(tbl->sym_names_next);
        tbl->sym_names_hash = NULL;
        tbl->sym_names_next = NULL;
    }
    else {
        errno = trap.error;
        return -1;
    }
    return 0;
}

static int section_symbol_comparator(const void * x, const void * y) {
    ELF_SecSymbol * rx = (ELF_SecSymbol *)x;
    ELF_SecSymbol * ry = (ELF_SecSymbol *)y;
    if (rx->address < ry->address) return -1;
    if (rx->address > ry->address) return +1;
    if (rx->index < ry->index) return -1;
    if (rx->index > ry->index) return +1;
    return 0;
}

static void create_symbol_addr_search_index(ELF_Section * sec) {
    ELF_File * file = sec->file;
    int elf64 = file->elf64;
    int swap = file->byte_swap;
    int rel = file->type == ET_REL;
    unsigned m = 0;

    for (m = 1; m < file->section_cnt; m++) {
        unsigned n = 1;
        ELF_Section * tbl = file->sections + m;
        if (tbl->sym_count == 0) continue;
        if (elf_load(tbl) < 0) exception(errno);
        while (n < tbl->sym_count) {
            int add = 0;
            U8_T addr = 0;
            U1_T type = 0;

            /* this is a workaround for the extra relocs for local symbols in the binutils RISC-V port */
            if (file->machine == EM_RISCV && file->type == ET_REL) {
                ELF_SymbolInfo sym_info = {0};
                unpack_elf_symbol_info(tbl, n, &sym_info);
                if (sym_info.name && sym_info.name[0] == '.' && sym_info.name[1] == 'L') {
                    n++;
                    continue;
                }
            }

            if (file->machine == EM_PPC64) {
                ELF_SymbolInfo sym_info;
                unpack_elf_symbol_info(tbl, n, &sym_info);
                /* Don't register PPC64 dot function name */
                add = IS_PPC64_FUNC_OPD(file, &sym_info) || \
                        (sym_info.section == sec && !IS_PPC64_FUNC_DOT(file, &sym_info) && sym_info.type != STT_GNU_IFUNC);
                if (add) {
                    addr = sym_info.value + (rel ? sec->addr : 0);
                    if (add && IS_PPC64_FUNC_OPD(file, &sym_info)) {
                        /*
                         * For PPC64, an ELF function symbol address is not described by
                         * the symbol value. In that case the symbol value points to a
                         * function descriptor in the OPD section. The first entry of the
                         * descriptor is the real function address. This value is
                         * relocatable.
                         */
                        U8_T offset;
                        ELF_Section * opd = file->sections + file->section_opd;
                        if (elf_load(opd) < 0) exception(errno);
                        offset = addr - opd->addr;
                        addr = *(U8_T *)((U1_T *)opd->data + offset);
                        if (swap) SWAP(addr);
                        drl_relocate(opd, offset, &addr, sizeof(addr), NULL);
                    }
                }
            }
            else {
                if (elf64) {
                    Elf64_Sym s = ((Elf64_Sym *) tbl->data)[n];
                    if (swap) SWAP(s.st_shndx);
                    if (s.st_shndx == sec->index) {
                        if (swap) SWAP(s.st_value);
                        addr = s.st_value;
                        type = ELF64_ST_TYPE(s.st_info);
                        if (rel) addr += sec->addr;
                        add = 1;
                    }
                }
                else {
                    Elf32_Sym s = ((Elf32_Sym *)tbl->data)[n];
                    if (swap) SWAP(s.st_shndx);
                    if (s.st_shndx == sec->index) {
                        if (swap) SWAP(s.st_value);
                        addr = s.st_value;
                        type = ELF32_ST_TYPE(s.st_info);
                        if (rel) addr += sec->addr;
                        add = 1;
                    }
                }
                add = add && type != STT_GNU_IFUNC;
                if (add && file->machine == EM_ARM) {
                    if (type == STT_FUNC || type == STT_ARM_TFUNC || type == STT_ARM_16BIT) {
                        addr = addr & ~(U8_T)1;
                    }
                }
            }
            if (add) {
                ELF_SecSymbol * s = NULL;
                if (sec->sym_addr_cnt >= sec->sym_addr_max) {
                    if (sec->sym_addr_table == NULL) {
                        sec->sym_addr_max = (unsigned)(tbl->sym_count / 2) + 16;
                        if (sec->sym_addr_max > 0x10000) sec->sym_addr_max = 0x10000;
                    }
                    else {
                        sec->sym_addr_max = sec->sym_addr_max * 3 / 2;
                    }
                    sec->sym_addr_table = (ELF_SecSymbol *)loc_realloc(sec->sym_addr_table, sec->sym_addr_max * sizeof(ELF_SecSymbol));
                }
                s = sec->sym_addr_table + sec->sym_addr_cnt++;
                s->address = addr;
                s->section = tbl;
                s->index = n;
            }
            n++;
        }
    }

    qsort(sec->sym_addr_table, sec->sym_addr_cnt, sizeof(ELF_SecSymbol), section_symbol_comparator);
}

void elf_find_symbol_by_address(ELF_Section * sec, ContextAddress addr, ELF_SymbolInfo * sym_info) {
    unsigned l = 0;
    unsigned h = 0;
    memset(sym_info, 0, sizeof(ELF_SymbolInfo));
    if (sec == NULL || addr < sec->addr) return;
    if (sec->sym_addr_table == NULL) create_symbol_addr_search_index(sec);
    h = sec->sym_addr_cnt;
    while (l < h) {
        unsigned k = (h + l) / 2;
        ELF_SecSymbol * info = sec->sym_addr_table + k;
        if (info->address > addr) {
            h = k;
        }
        else {
            ContextAddress next = (ContextAddress)(k < sec->sym_addr_cnt - 1 ?
                (info + 1)->address : sec->addr + sec->size);
            assert(next >= info->address);
            if (next <= addr) {
                l = k + 1;
            }
            else {
                unpack_elf_symbol_info(info->section, info->index, sym_info);
                assert(IS_PPC64_FUNC_OPD(info->section->file, sym_info) || sym_info->section == sec);
                sym_info->addr_index = k;
                return;
            }
        }
    }
}

void elf_prev_symbol_by_address(ELF_SymbolInfo * sym_info) {
    if (sym_info->section != NULL && sym_info->addr_index > 0) {
        U4_T index = sym_info->addr_index - 1;
        ELF_SecSymbol * info = sym_info->section->sym_addr_table + index;
        unpack_elf_symbol_info(info->section, info->index, sym_info);
        sym_info->addr_index = index;
    }
    else {
        memset(sym_info, 0, sizeof(ELF_SymbolInfo));
    }
}

void elf_next_symbol_by_address(ELF_SymbolInfo * sym_info) {
    if (sym_info->section != NULL && sym_info->addr_index + 1 < sym_info->section->sym_addr_cnt) {
        U4_T index = sym_info->addr_index + 1;
        ELF_SecSymbol * info = sym_info->section->sym_addr_table + index;
        unpack_elf_symbol_info(info->section, info->index, sym_info);
        sym_info->addr_index = index;
    }
    else {
        memset(sym_info, 0, sizeof(ELF_SymbolInfo));
    }
}

int elf_find_got_entry(ELF_File * file, const char * name, ContextAddress * addr) {
    Trap trap;
    unsigned idx;
    if (!set_trap(&trap)) return -1;
    for (idx = 1; idx < file->section_cnt; idx++) {
        U4_T i = 0;
        U4_T n = 0;
        ELF_Section * sec = file->sections + idx;
        if (sec->type != SHT_RELA) continue;
        if (sec->link == 0 || sec->link >= file->section_cnt) continue;
        if ((file->sections + sec->link)->type != SHT_DYNSYM) continue;
        if (elf_load(sec) < 0) exception(errno);
        n = (U4_T)(sec->size / sec->entsize);
        while (i < n) {
            U4_T sym_index = 0;
            U8_T got_addr = 0;
            ELF_SymbolInfo sym_info;
            if (!file->elf64) {
                Elf32_Rela bf = *(Elf32_Rela *)((U1_T *)sec->data + i * sec->entsize);
                if (file->byte_swap) {
                    SWAP(bf.r_offset);
                    SWAP(bf.r_info);
                }
                sym_index = ELF32_R_SYM(bf.r_info);
                got_addr = bf.r_offset;
            }
            else {
                Elf64_Rela bf = *(Elf64_Rela *)((U1_T *)sec->data + i * sec->entsize);
                if (file->byte_swap) {
                    SWAP(bf.r_offset);
                    SWAP(bf.r_info);
                }
                sym_index = ELF64_R_SYM(bf.r_info);
                got_addr = bf.r_offset;
            }
            unpack_elf_symbol_info(file->sections + sec->link, sym_index, &sym_info);
            if (sym_info.name != NULL && strcmp(sym_info.name, name) == 0) {
                *addr = (ContextAddress)got_addr;
                clear_trap(&trap);
                return 0;
            }
            i++;
        }
    }
    clear_trap(&trap);
    *addr = 0;
    return 0;
}

int elf_find_plt_dynsym(ELF_Section * plt, unsigned entry, ELF_SymbolInfo * sym_info, ContextAddress * offs) {
    Trap trap;
    unsigned idx;
    ELF_File * file = plt->file;

    if (!set_trap(&trap)) return -1;
    for (idx = 1; idx < file->section_cnt; idx++) {
        U4_T sym_index = 0;
        U8_T sym_offset = 0;
        ELF_Section * sec = file->sections + idx;
        if (sec->name == NULL || sec->entsize == 0) continue;
        if (sec->type != SHT_REL && sec->type != SHT_RELA) continue;
        if (sec->link == 0 || sec->link >= file->section_cnt) continue;
        if ((file->sections + sec->link)->type != SHT_DYNSYM) continue;
        if (strcmp(sec->name, ".rel.plt") != 0 && strcmp(sec->name, ".rela.plt") != 0) continue;
        if (entry >= sec->size / sec->entsize) break;
        if (elf_load(sec) < 0) exception(errno);
        if (sec->type == SHT_REL) {
            if (!file->elf64) {
                Elf32_Rel bf = *(Elf32_Rel *)((U1_T *)sec->data + entry * sec->entsize);
                if (file->byte_swap) {
                    SWAP(bf.r_info);
                }
                sym_index = ELF32_R_SYM(bf.r_info);
            }
            else {
                Elf64_Rel bf = *(Elf64_Rel *)((U1_T *)sec->data + entry * sec->entsize);
                if (file->byte_swap) {
                    SWAP(bf.r_info);
                }
                sym_index = ELF64_R_SYM(bf.r_info);
            }
        }
        else {
            if (!file->elf64) {
                Elf32_Rela bf = *(Elf32_Rela *)((U1_T *)sec->data + entry * sec->entsize);
                if (file->byte_swap) {
                    SWAP(bf.r_addend);
                    SWAP(bf.r_info);
                }
                sym_index = ELF32_R_SYM(bf.r_info);
                sym_offset = bf.r_addend;
            }
            else {
                Elf64_Rela bf = *(Elf64_Rela *)((U1_T *)sec->data + entry * sec->entsize);
                if (file->byte_swap) {
                    SWAP(bf.r_addend);
                    SWAP(bf.r_info);
                }
                sym_index = ELF64_R_SYM(bf.r_info);
                sym_offset = bf.r_addend;
            }
        }
        *offs = (ContextAddress)sym_offset;
        unpack_elf_symbol_info(file->sections + sec->link, sym_index, sym_info);
        clear_trap(&trap);
        return 0;
    }
    clear_trap(&trap);
    memset(sym_info, 0, sizeof(ELF_SymbolInfo));
    return 0;
}

int elf_get_plt_entry_size(ELF_File * file, unsigned * first_size, unsigned * entry_size) {
    switch (file->machine) {
    case EM_386:
    case EM_X86_64:
        *first_size = 16;
        *entry_size = 16;
        return 0;
    case EM_PPC:
        if (file->vxworks_got) {
            *first_size = 32;
            *entry_size = 32;
            return 0;
        }
        *first_size = 72;
        *entry_size = 12;
        return 0;
    case EM_PPC64:
        *first_size = 24;
        *entry_size = 24;
        return 0;
    case EM_ARM:
        *first_size = 20;
        *entry_size = 12;
        return 0;
    case EM_MIPS:
        if (file->vxworks_got) {
            *first_size = 24;
            *entry_size = 8;
            return 0;
        }
        *first_size = 32;
        *entry_size = 16;
        return 0;
    }
    set_errno(ERR_OTHER, "Unknown PLT entry size");
    return -1;
}

void elf_invalidate(void) {
    ELF_File * prev = NULL;
    ELF_File * file = files;
    while (file != NULL) {
        ELF_File * next = file->next;
        if (!file->mtime_changed) {
            struct stat st;
            if (stat(file->name, &st) < 0 ||
                file->size != st.st_size ||
                file->mtime != st.st_mtime ||
                (st.st_ino != 0 && st.st_ino != file->ino))
                file->mtime_changed = 1;
        }
        if (file->lock_cnt > 0) {
            prev = file;
        }
        else if (file->mtime_changed) {
            if (prev == NULL) files = next;
            else prev->next = next;
            elf_dispose(file);
        }
        else {
            prev = file;
        }
        file = next;
    }
}

void ini_elf(void) {
}

#endif /* ENABLE_ELF */
