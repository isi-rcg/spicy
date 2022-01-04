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
 * This module implements reading and caching of ELF files.
 */
#ifndef D_elf
#define D_elf

#include <tcf/config.h>

#if ENABLE_ELF

#if !ENABLE_ContextProxy && !defined(_WIN32) && !defined(__CYGWIN__) && !defined(__APPLE__)
#  define INCLUDE_NATIVE_ELF_H 1
#  include <elf.h>
#endif
#include <tcf/framework/context.h>

#ifndef EM_RISCV
#  define EM_RISCV      243 /* RISC-V */
#endif

#if !defined(INCLUDE_NATIVE_ELF_H)

#define EI_MAG0        0
#define EI_MAG1        1
#define EI_MAG2        2
#define EI_MAG3        3
#define EI_CLASS       4
#define EI_DATA        5
#define EI_VERSION     6
#define EI_OSABI       7
#define EI_ABIVERSION  8
#define EI_PAD         9
#define EI_NIDENT     16

#define ELFMAG0 0x7F
#define ELFMAG1  'E'
#define ELFMAG2  'L'
#define ELFMAG3  'F'
#define ELFMAG   "\177ELF"
#define SELFMAG  4

#define ELFCLASSNONE   0
#define ELFCLASS32     1
#define ELFCLASS64     2

#define ELFDATANONE    0
#define ELFDATA2LSB    1
#define ELFDATA2MSB    2

#define EM_NONE         0
#define EM_M32          1 /* AT&T WE 32100 */
#define EM_SPARC        2 /* SPARC */
#define EM_386          3 /* Intel Architecture */
#define EM_68K          4 /* Motorola 68000 */
#define EM_88K          5 /* Motorola 88000 */
#define EM_860          7 /* Intel 80860 */
#define EM_MIPS         8 /* MIPS RS3000 Big-Endian */
#define EM_MIPS_RS4_BE 10 /* MIPS RS4000 Big-Endian */
#define EM_PPC         20 /* PowerPC */
#define EM_PPC64       21 /* PowerPC64 */
#define EM_V800        36 /* NEC V800 */
#define EM_ARM         40 /* ARM */
#define EM_SH          42 /* Hitachi Super-H */
#define EM_SPARCV9     43 /* SPARC Version 9 */
#define EM_TRICORE     44 /* Siemens Tricore */
#define EM_IA_64       50 /* HP/Intel IA-64 */
#define EM_MIPS_X      51 /* Stanford MIPS-X */
#define EM_COLDFIRE    52 /* Motorola Coldfire */
#define EM_X86_64      62 /* AMD x86-64 architecture */
#define EM_V850        87 /* NEC/Renesas RH850 */
#define EM_AARCH64    183 /* ARM 64-bit architecture */
#define EM_MICROBLAZE 189 /* Xilinx MicroBlaze */

#define ET_NONE         0
#define ET_REL          1
#define ET_EXEC         2
#define ET_DYN          3
#define ET_CORE         4
#define ET_LOOS    0xFE00
#define ET_HIOS    0xFEFF
#define ET_LOPROC  0xFF00
#define ET_HIPROC  0xFFFF

#define EV_CURRENT      1

#define SHN_UNDEF       0
#define SHN_ABS    0xfff1
#define SHN_COMMON 0xfff2

#define SHT_NULL        0
#define SHT_PROGBITS    1
#define SHT_SYMTAB      2
#define SHT_STRTAB      3
#define SHT_RELA        4
#define SHT_HASH        5
#define SHT_DYNAMIC     6
#define SHT_NOTE        7
#define SHT_NOBITS      8
#define SHT_REL         9
#define SHT_SHLIB      10
#define SHT_DYNSYM     11

#define STN_UNDEF       0

#define STB_LOCAL       0
#define STB_GLOBAL      1
#define STB_WEAK        2

#define STT_NOTYPE      0
#define STT_OBJECT      1
#define STT_FUNC        2
#define STT_SECTION     3
#define STT_FILE        4
#define STT_COMMON      5
#define STT_TLS         6
#define STT_LOPROC      13
#define STT_HIPROC      15
#define STT_ARM_TFUNC   STT_LOPROC
#define STT_ARM_16BIT   STT_HIPROC

#define PT_NULL         0
#define PT_LOAD         1
#define PT_DYNAMIC      2
#define PT_INTERP       3
#define PT_NOTE         4
#define PT_SHLIB        5
#define PT_PHDR         6
#define PT_TLS          7

#define PF_X            (1 << 0)
#define PF_W            (1 << 1)
#define PF_R            (1 << 2)

#define DT_NULL         0
#define DT_NEEDED       1
#define DT_PLTRELSZ     2
#define DT_PLTGOT       3
#define DT_HASH         4
#define DT_STRTAB       5
#define DT_SYMTAB       6
#define DT_RELA         7
#define DT_RELASZ       8
#define DT_RELAENT      9
#define DT_STRSZ        10
#define DT_SYMENT       11
#define DT_INIT         12
#define DT_FINI         13
#define DT_SONAME       14
#define DT_RPATH        15
#define DT_SYMBOLIC     16
#define DT_REL          17
#define DT_RELSZ        18
#define DT_RELENT       19
#define DT_PLTREL       20
#define DT_DEBUG        21
#define DT_TEXTREL      22
#define DT_JMPREL       23
#define DT_BIND_NOW     24
#define DT_INIT_ARRAY   25
#define DT_FINI_ARRAY   26
#define DT_INIT_ARRAYSZ 27
#define DT_FINI_ARRAYSZ 28
#define DT_RUNPATH      29
#define DT_FLAGS        30
#define DT_ENCODING     32
#define DT_PREINIT_ARRAY 32
#define DT_PREINIT_ARRAYSZ 33
#define DT_NUM          34
#define DT_LOOS         0x6000000d
#define DT_HIOS         0x6ffff000
#define DT_FLAGS_1      0x6ffffffb
#define DT_LOPROC       0x70000000
#define DT_HIPROC       0x7fffffff

/* Values of DT_FLAGS_1 */
#define DF_1_PIE        0x08000000

typedef uint32_t    Elf32_Addr;
typedef uint16_t    Elf32_Half;
typedef uint32_t    Elf32_Off;
typedef int32_t     Elf32_Sword;
typedef uint32_t    Elf32_Word;

typedef struct Elf32_Ehdr {
    unsigned char e_ident[EI_NIDENT];
    Elf32_Half    e_type;
    Elf32_Half    e_machine;
    Elf32_Word    e_version;
    Elf32_Addr    e_entry;
    Elf32_Off     e_phoff;
    Elf32_Off     e_shoff;
    Elf32_Word    e_flags;
    Elf32_Half    e_ehsize;
    Elf32_Half    e_phentsize;
    Elf32_Half    e_phnum;
    Elf32_Half    e_shentsize;
    Elf32_Half    e_shnum;
    Elf32_Half    e_shstrndx;
} Elf32_Ehdr;

typedef struct Elf32_Shdr {
    Elf32_Word sh_name;
    Elf32_Word sh_type;
    Elf32_Word sh_flags;
    Elf32_Addr sh_addr;
    Elf32_Off  sh_offset;
    Elf32_Word sh_size;
    Elf32_Word sh_link;
    Elf32_Word sh_info;
    Elf32_Word sh_addralign;
    Elf32_Word sh_entsize;
} Elf32_Shdr;

#define SHF_WRITE           0x00000001
#define SHF_ALLOC           0x00000002
#define SHF_EXECINSTR       0x00000004

typedef struct Elf32_Phdr {
    Elf32_Word p_type;
    Elf32_Off  p_offset;
    Elf32_Addr p_vaddr;
    Elf32_Addr p_paddr;
    Elf32_Word p_filesz;
    Elf32_Word p_memsz;
    Elf32_Word p_flags;
    Elf32_Word p_align;
} Elf32_Phdr;

typedef struct Elf32_Sym {
    Elf32_Word    st_name;
    Elf32_Addr    st_value;
    Elf32_Word    st_size;
    unsigned char st_info;
    unsigned char st_other;
    Elf32_Half    st_shndx;
} Elf32_Sym;

#define ELF32_ST_BIND(i)   ((i)>>4)
#define ELF32_ST_TYPE(i)   ((i)&0xf)

typedef struct {
    Elf32_Addr r_offset;
    Elf32_Word r_info;
} Elf32_Rel;

typedef struct {
    Elf32_Addr r_offset;
    Elf32_Word r_info;
    Elf32_Sword r_addend;
} Elf32_Rela;

#define ELF32_R_SYM(i)  ((i) >> 8)
#define ELF32_R_TYPE(i) ((uint8_t)(i))

typedef struct {
    Elf32_Sword d_tag;
    union {
        Elf32_Word d_val;
        Elf32_Addr d_ptr;
    } d_un;
} Elf32_Dyn;

#endif

#if !defined(INCLUDE_NATIVE_ELF_H) || \
    (defined(_WRS_KERNEL) && !defined(EM_X86_64))

typedef uint64_t        Elf64_Addr;
typedef uint16_t        Elf64_Half;
typedef uint32_t        Elf64_Word;
typedef int32_t         Elf64_Sword;
typedef uint64_t        Elf64_Xword;
typedef int64_t         Elf64_Sxword;
typedef uint64_t        Elf64_Off;
typedef uint16_t        Elf64_Section;
typedef Elf64_Half      Elf64_Versym;
typedef uint16_t        Elf64_Quarter;

typedef struct {
    uint8_t       e_ident[EI_NIDENT];
    Elf64_Half    e_type;
    Elf64_Half    e_machine;
    Elf64_Word    e_version;
    Elf64_Addr    e_entry;
    Elf64_Off     e_phoff;
    Elf64_Off     e_shoff;
    Elf64_Word    e_flags;
    Elf64_Half    e_ehsize;
    Elf64_Half    e_phentsize;
    Elf64_Half    e_phnum;
    Elf64_Half    e_shentsize;
    Elf64_Half    e_shnum;
    Elf64_Half    e_shstrndx;
} Elf64_Ehdr;

typedef struct {
    Elf64_Word    sh_name;
    Elf64_Word    sh_type;
    Elf64_Xword   sh_flags;
    Elf64_Addr    sh_addr;
    Elf64_Off     sh_offset;
    Elf64_Xword   sh_size;
    Elf64_Word    sh_link;
    Elf64_Word    sh_info;
    Elf64_Xword   sh_addralign;
    Elf64_Xword   sh_entsize;
} Elf64_Shdr;

typedef struct {
    Elf64_Word    st_name;
    uint8_t       st_info;
    uint8_t       st_other;
    Elf64_Section st_shndx;
    Elf64_Addr    st_value;
    Elf64_Xword   st_size;
} Elf64_Sym;

#define ELF64_ST_BIND(info)             ((info) >> 4)
#define ELF64_ST_TYPE(info)             ((info) & 0xf)

typedef struct {
    Elf64_Addr    r_offset;
    Elf64_Xword   r_info;
} Elf64_Rel;

typedef struct {
    Elf64_Addr    r_offset;
    Elf64_Xword   r_info;
    Elf64_Sxword  r_addend;
} Elf64_Rela;

#define ELF64_R_SYM(i)  ((uint32_t)((i) >> 32))
#define ELF64_R_TYPE(i) ((uint32_t)(i))

typedef struct {
    Elf64_Sxword d_tag;
    union {
        Elf64_Xword d_val;
        Elf64_Addr d_ptr;
    } d_un;
} Elf64_Dyn;

typedef struct {
    Elf64_Word    p_type;
    Elf64_Word    p_flags;
    Elf64_Off     p_offset;
    Elf64_Addr    p_vaddr;
    Elf64_Addr    p_paddr;
    Elf64_Xword   p_filesz;
    Elf64_Xword   p_memsz;
    Elf64_Xword   p_align;
} Elf64_Phdr;

#endif

#ifndef EM_ARM
#define EM_ARM 40
#endif
#ifndef EM_X86_64
#define EM_X86_64 62
#endif
#ifndef EM_PPC
#define EM_PPC 20
#endif
#ifndef EM_PPC64
#define EM_PPC64 21
#endif
#ifndef EM_V800
#define EM_V800 36 /* NEC V800 */
#endif
#ifndef EM_V850
#define EM_V850 87 /* NEC/Renesas RH850 */
#endif
#ifndef EM_AARCH64
#define EM_AARCH64 183
#endif
#ifndef EM_MICROBLAZE
#define EM_MICROBLAZE 189
#endif
#ifndef EM_SPARCV9
#define EM_SPARCV9     43 /* SPARC Version 9 */
#endif
#ifndef EM_TRICORE
#define EM_TRICORE     44 /* Siemens Tricore */
#endif
#ifndef STT_GNU_IFUNC
#define STT_GNU_IFUNC  10
#endif
#ifndef DT_FLAGS_1
#define DT_FLAGS_1      0x6ffffffb
#endif
#ifndef DF_1_PIE
#define DF_1_PIE        0x08000000
#endif

typedef struct ElfX_Sym {
    union {
        Elf32_Sym Elf32;
        Elf64_Sym Elf64;
    } u;
} ElfX_Sym;

typedef uint8_t  U1_T;
typedef int8_t   I1_T;
typedef uint16_t U2_T;
typedef int16_t  I2_T;
typedef uint32_t U4_T;
typedef int32_t  I4_T;
typedef uint64_t U8_T;
typedef int64_t  I8_T;

typedef struct ELF_File ELF_File;
typedef struct ELF_Section ELF_Section;
typedef struct ELF_SecSymbol ELF_SecSymbol;
typedef struct ELF_SymbolInfo ELF_SymbolInfo;
typedef struct ELF_PHeader ELF_PHeader;

/* TODO: fp_abi - value of Tag_GNU_Power_ABI_FP in gnu.attributes section */
struct ELF_File {
    ELF_File * next;

    char * name;
    dev_t dev;
    ino_t ino;
    int64_t mtime;
    int64_t size;
    int mtime_changed;
    ErrorReport * error;
    unsigned lock_cnt;
    int fd;

    /* Other names of the file, e.g. symbolic links */
    char ** names;
    unsigned names_cnt;
    unsigned names_max;

#if defined(_WIN32) || defined(__CYGWIN__)
    HANDLE mmap_handle;
#endif

    uint8_t big_endian; /* 0 - least significant first, 1 - most significat first */
    uint8_t byte_swap;  /* > 0 if file endianness not same as the agent endianness */
    uint8_t elf64;
    uint16_t type;
    uint16_t machine;
    uint32_t flags;     /* Contains ABI version for PPC64 */
    uint8_t os_abi;

    ContextAddress entry_address;

    unsigned section_cnt;
    ELF_Section * sections;
    char * str_pool;

    unsigned pheader_cnt;
    ELF_PHeader * pheaders;
    unsigned bss_segment_cnt;

    void * dwarf_io_cache;
    void * dwarf_dt_cache;

    unsigned age;   /* Seconds since last time the file was accessed */

    int listed;
    int debug_info_file; /* 1 means this file contains debug info only - no code */
    char * debug_info_file_name;
    char * dwz_file_name;
    ELF_File * dwz_file;

    int vxworks_got;
    unsigned section_opd;    /* PPC64 opd section number */
};

struct ELF_SecSymbol {
    ELF_Section * section;
    unsigned index;
    U8_T address;
};

struct ELF_SymbolInfo {
    ELF_Section * sym_section;
    U4_T sym_index;
    unsigned addr_index;
    ELF_Section * section;
    U4_T section_index;
    char * name;
    U1_T bind;
    U1_T type;
    U1_T type16bit;
    U8_T value;
    U8_T size;
    U8_T other;
};

struct ELF_Section {
    ELF_File * file;
    U4_T index;
    unsigned name_offset;
    char * name;
    void * data;
    U4_T type;
    U4_T flags;
    U4_T alignment;
    U8_T offset;
    U8_T size;
    U8_T addr;
    U4_T link;
    U4_T info;
    U4_T entsize;

    /* Compression info */
    U4_T compressed_type;
    U8_T compressed_size;
    U8_T compressed_offset;

    void * mmap_addr;
    size_t mmap_size;

    ELF_Section * relocate;

    unsigned sym_count;

    /* Symbol by address search index */
    ELF_SecSymbol * sym_addr_table;
    unsigned sym_addr_cnt;
    unsigned sym_addr_max;

    /* Symbol by name search index */
    unsigned sym_names_hash_size;
    unsigned * sym_names_hash;
    unsigned * sym_names_next;

    /* Relocations blocks */
    unsigned reloc_num_zones;
    unsigned * reloc_zones_bondaries;
};

struct ELF_PHeader {
    U4_T type;
    U8_T offset;
    U8_T address;
    U8_T physical_address;
    U8_T file_size;
    U8_T mem_size;
    U4_T flags;
    U4_T align;
};

#define IS_PPC64_FUNC_OPD(file, sym_info)   ((file)->machine == EM_PPC64 && ((file)->flags & 0x3) < 2 && \
                                            (sym_info)->section_index == (file)->section_opd && \
                                            ((sym_info)->type == STT_FUNC || (sym_info)->type == STT_NOTYPE))

#define IS_PPC64_FUNC_DOT(file, sym_info)   ((file)->machine == EM_PPC64 && ((file)->flags & 0x3) < 2 && \
                                            (sym_info)->type == STT_FUNC && (sym_info)->name != NULL && (sym_info)->name[0] == '.')

/*
 * Open ELF file for reading.
 * Same file can be opened mutiple times.
 * Returns the file descriptior on success. If error, returns NULL and sets errno.
 * The file descriptor is valid only during single dispatch cycle.
 */
extern ELF_File * elf_open(const char * file_name);

/*
 * Open ELF file that is mapped in the given memory region.
 */
extern ELF_File * elf_open_memory_region_file(MemoryRegion * module, int * error);

/*
 * Load section data into memory.
 * section->data is set to section data address in memory.
 * Data will stay in memory at least until file is closed.
 * Returns zero on success. If error, returns -1 and sets errno.
 */
extern int elf_load(ELF_Section * section);

/*
 * Register ELF file close callback.
 * The callback is called each time an ELF file data is about to be disposed.
 * Service implementation can use the callback to deallocate
 * cached data related to the file.
 */
typedef void (*ELFCloseListener)(ELF_File *);
extern void elf_add_close_listener(ELFCloseListener listener);

/*
 * Register ELF file open callback.
 * The callback is called each time an ELF file data is about to be opened.
 */
typedef void (*ELFOpenListener)(ELF_File *);
extern void elf_add_open_listener(ELFOpenListener listener);

/*
 * Return ELF file that contains DWARF info for given file.
 * On some systems, DWARF is kept in a separate file.
 * If such file is not available, return 'file'.
 */
extern ELF_File * get_dwarf_file(ELF_File * file);

#if ENABLE_DebugContext

/*
 * Open ELF file for reading for given device and inode.
 * Same file can be opened mutiple times.
 * Returns the file descriptior on success. If error, returns NULL and sets errno.
 * The file descriptor is valid only during single dispatch cycle.
 */
extern ELF_File * elf_open_inode(Context * ctx, dev_t dev, ino_t ino, int64_t mtime);

/*
 * Iterate context ELF files that are mapped in context memory in given address range (inclusive).
 * Returns the file descriptior on success. If error, returns NULL and sets errno.
 */
extern ELF_File * elf_list_first(Context * ctx, ContextAddress addr0, ContextAddress addr1);
extern MemoryRegion * elf_list_region(Context * ctx);
extern ELF_File * elf_list_next(Context * ctx);

/*
 * Finish iteration of context ELF files.
 * Clients should always call elf_list_done() after calling elf_list_first().
 */
extern void elf_list_done(Context * ctx);

/*
 * Get info about ELF files mapped or loaded in the given memory address range.
 * 'addr0' and 'addr1' are run-time addresses.
 * The range is inclusive.
 */
extern int elf_get_map(Context * ctx, ContextAddress addr0, ContextAddress addr1, MemoryMap * map);

/*
 * Map link-time address in an ELF file to run-time address in a context.
 * Clear errno if no errors found, otherwise set errno and return 0.
 */
extern ContextAddress elf_map_to_run_time_address(Context * ctx, ELF_File * file, ELF_Section * section, ContextAddress addr);

/*
 * Return run-time address for link-time address 'addr' in a file 'file' (and optional section 'sec'),
 * which is mapped to memory context 'ctx' at memory region 'region'.
 * Clear errno if no errors found, otherwise set errno and return 0.
 */
extern ContextAddress elf_run_time_address_in_region(Context * ctx, MemoryRegion * region, ELF_File * file, ELF_Section * sec, ContextAddress addr);

/*
 * Map run-time address in a context to link time address in an ELF file.
 * If debug info is moved into separate file,
 * to_dwarf = 0 - return address in the main file,
 * to_dwarf = 1 - return address in the debug info file.
 * Usually a separate (split) debug info file has the same addresses as
 * the object file it is derived from, but this is not the case if a library gets re-linked
 * after debug info has been split out (see also https://bugs.kde.org/show_bug.cgi?id=185816).
 * Return 0 if the address is not currently mapped.
 */
extern ContextAddress elf_map_to_link_time_address(Context * ctx, ContextAddress addr, int to_dwarf, ELF_File ** file, ELF_Section ** sec);

/*
 * Read a word from context memory. Word size and endianess are determened by ELF file.
 */
extern int elf_read_memory_word(Context * ctx, ELF_File * file, ContextAddress addr, ContextAddress * word);

#endif

/* Return symbol name hash. The hash is used to build sym_names_hash table. */
extern unsigned calc_symbol_name_hash(const char * s);

/* Compare symbol names. */
extern int cmp_symbol_names(const char * x, const char * y);

/*
 * Get ELF_SymbolInfo.
 * Call exception() on error.
 */
extern void unpack_elf_symbol_info(ELF_Section * section, U4_T index, ELF_SymbolInfo * info);

/*
 * Find ELF symbol by link-time address in a section.
 * Return info for nearest symbol at or before given address.
 * If not found, set all 'info' fields to zeros.
 * Call exception() on error.
 */
extern void elf_find_symbol_by_address(ELF_Section * section, ContextAddress addr, ELF_SymbolInfo * info);

/*
 * Get prev/next ELF symbol by link-time address in a section.
 * If not found, set all 'info' fields to zeros.
 * Call exception() on error.
 */
extern void elf_prev_symbol_by_address(ELF_SymbolInfo * info);
extern void elf_next_symbol_by_address(ELF_SymbolInfo * info);

/*
 * Find link-time address of GOT entry for given symbol name and assign it to *addr.
 * Returns 0 on success.
 * If entry not found, sets *addr = 0 and returns 0.
 * If error, sets errno and returns -1.
 */
extern int elf_find_got_entry(ELF_File * file, const char * name, ContextAddress * addr);

/*
 * Find target symbol for a PLT entry.
 */
extern int elf_find_plt_dynsym(ELF_Section * plt, unsigned entry,
                               ELF_SymbolInfo * sym_info, ContextAddress * offs);

/*
 * Get size of PLT enries.
 */
extern int elf_get_plt_entry_size(ELF_File * file, unsigned * first_size, unsigned * entry_size);

/*
 * Invalidate and dispose cached ELF data.
 */
extern void elf_invalidate(void);

/*
 * Initialize ELF support module.
 */
extern void ini_elf(void);

#endif /* ENABLE_ELF */

#endif /* D_elf */
