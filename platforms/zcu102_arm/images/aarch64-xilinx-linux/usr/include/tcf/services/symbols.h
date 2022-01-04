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
 * Symbols service.
 */

#ifndef D_symbols
#define D_symbols

#include <tcf/framework/context.h>
#include <tcf/framework/protocol.h>

/*
 * Symbol information can change at any time as result of target background activities.
 * Clients should not cache symbol information, and should not retain the information
 * longer than one dispatch cycle.
 */

typedef struct Symbol Symbol;
typedef struct FunctionCallInfo FunctionCallInfo;

#define SYM_CLASS_UNKNOWN       0
#define SYM_CLASS_VALUE         1   /* Symbol represents a constant value */
#define SYM_CLASS_REFERENCE     2   /* Symbol is an address of an object (variable) in memory */
#define SYM_CLASS_FUNCTION      3   /* Symbol is an address of a function */
#define SYM_CLASS_TYPE          4   /* Symbol represents a type declaration */
#define SYM_CLASS_COMP_UNIT     5   /* Symbol represents a compilation unit */
#define SYM_CLASS_BLOCK         6   /* Symbol represents a block of code */
#define SYM_CLASS_NAMESPACE     7   /* Symbol represents a namespace */
#define SYM_CLASS_VARIANT_PART  8   /* Symbol represents a variant part of a structure */
#define SYM_CLASS_VARIANT       9   /* Symbol represents a member of a variant part of a structure */

#define TYPE_CLASS_UNKNOWN      0
#define TYPE_CLASS_CARDINAL     1
#define TYPE_CLASS_INTEGER      2
#define TYPE_CLASS_REAL         3
#define TYPE_CLASS_POINTER      4
#define TYPE_CLASS_ARRAY        5
#define TYPE_CLASS_COMPOSITE    6
#define TYPE_CLASS_ENUMERATION  7
#define TYPE_CLASS_FUNCTION     8
#define TYPE_CLASS_MEMBER_PTR   9
#define TYPE_CLASS_COMPLEX      10

typedef uint32_t SYM_FLAGS;

#define SYM_FLAG_PARAMETER      0x00000001
#define SYM_FLAG_TYPEDEF        0x00000002
#define SYM_FLAG_CONST_TYPE     0x00000004
#define SYM_FLAG_PACKET_TYPE    0x00000008
#define SYM_FLAG_SUBRANGE_TYPE  0x00000010
#define SYM_FLAG_VOLATILE_TYPE  0x00000020
#define SYM_FLAG_RESTRICT_TYPE  0x00000040
#define SYM_FLAG_UNION_TYPE     0x00000080
#define SYM_FLAG_CLASS_TYPE     0x00000100
#define SYM_FLAG_INTERFACE_TYPE 0x00000200
#define SYM_FLAG_SHARED_TYPE    0x00000400
#define SYM_FLAG_REFERENCE      0x00000800
#define SYM_FLAG_BIG_ENDIAN     0x00001000
#define SYM_FLAG_LITTLE_ENDIAN  0x00002000
#define SYM_FLAG_OPTIONAL       0x00004000
#define SYM_FLAG_EXTERNAL       0x00008000
#define SYM_FLAG_VARARG         0x00010000
#define SYM_FLAG_ARTIFICIAL     0x00020000
#define SYM_FLAG_TYPE_PARAMETER 0x00040000
#define SYM_FLAG_PRIVATE        0x00080000
#define SYM_FLAG_PROTECTED      0x00100000
#define SYM_FLAG_PUBLIC         0x00200000
#define SYM_FLAG_ENUM_TYPE      0x00400000
#define SYM_FLAG_STRUCT_TYPE    0x00800000
#define SYM_FLAG_STRING_TYPE    0x01000000
#define SYM_FLAG_INHERITANCE    0x02000000
#define SYM_FLAG_BOOL_TYPE      0x04000000
#define SYM_FLAG_INDIRECT       0x08000000
#define SYM_FLAG_RVALUE         0x10000000

/* Additional (uncommon) symbol properties */
typedef struct SymbolProperties {
    int binary_scale;   /* The exponent of the base two scale factor to be applied to an instance of the type */
    int decimal_scale;  /* The exponent of the base ten scale factor to be applied to an instance of the type */
    unsigned bit_stride;
    unsigned local_entry_offset; /* Only PPC64 v2 supports it */
    const char * linkage_name;
} SymbolProperties;

/* Symbol properties update policies */
#define UPDATE_ON_MEMORY_MAP_CHANGES 0
#define UPDATE_ON_EXE_STATE_CHANGES  1

typedef void EnumerateSymbolsCallBack(void *, Symbol *);

#if ENABLE_DebugContext

typedef struct LocationCommands {
    LocationExpressionCommand * cmds;
    unsigned cnt;
    unsigned max;
} LocationCommands;

typedef struct DiscriminantRange {
    int64_t x;
    int64_t y;
} DiscriminantRange;

typedef struct LocationInfo {
    ContextAddress code_addr;
    ContextAddress code_size;
    int big_endian;
    unsigned args_cnt;
    LocationCommands value_cmds;
    DiscriminantRange * discr_lst;
    unsigned discr_cnt;
} LocationInfo;

/* Stack tracing command sequence */
typedef struct StackFrameRegisterLocation {
    RegisterDefinition * reg;
    unsigned cmds_cnt;
    unsigned cmds_max;
    LocationExpressionCommand cmds[1];
} StackFrameRegisterLocation;

typedef struct StackFrameInlinedSubroutine {
    const char * func_id;
    CodeArea area;
} StackFrameInlinedSubroutine;

/* Complete stack tracing info for a range of instruction addresses */
typedef struct StackTracingInfo {
    ContextAddress addr;
    ContextAddress size;
    StackFrameRegisterLocation * fp;
    StackFrameRegisterLocation ** regs;
    int reg_cnt;
    StackFrameInlinedSubroutine ** subs;
    int sub_cnt;
} StackTracingInfo;

typedef struct SymbolFileInfo {
    ContextAddress addr;
    ContextAddress size;
    char * file_name;
    int file_error;
    int dyn_loader;
} SymbolFileInfo;

#endif

#if ENABLE_Symbols

/*
 * Find symbol information for given symbol name in given context.
 * On error, returns -1 and sets errno.
 * On success returns 0.
 */
extern int find_symbol_by_name(Context * ctx, int frame, ContextAddress ip, const char * name, Symbol ** sym);

/*
 * Find symbol information for given symbol name in given context and visibility scope.
 * On error, returns -1 and sets errno.
 * On success returns 0.
 */
extern int find_symbol_in_scope(Context * ctx, int frame, ContextAddress ip, Symbol * scope, const char * name, Symbol ** sym);

/*
 * Find symbol information for given address in given context.
 * On error, returns -1 and sets errno.
 * On success returns 0.
 */
extern int find_symbol_by_addr(Context * ctx, int frame, ContextAddress addr, Symbol ** sym);

/*
 * find_symbol_* functions return first symbol that matches the search criteria.
 * Clients can use find_next_symbol() to get the rest of matching symbols,
 * for example, to get all symbols for overloaded functions.
 */
extern int find_next_symbol(Symbol ** sym);

/*
 * Enumerate symbols in given context.
 * If frame >= 0 enumerates local symbols and function arguments.
 * If frame < 0 enumerates global symbols.
 * On error returns -1 and sets errno.
 * On success returns 0.
 */
extern int enumerate_symbols(Context * ctx, int frame, EnumerateSymbolsCallBack *, void * args);

/*
 * Get (relatively) permanent symbol ID that can be used across dispatch cycles.
 */
extern const char * symbol2id(const Symbol * sym);

/*
 * Find symbol by symbol ID.
 * On error, returns -1 and sets errno.
 * On success returns 0.
 */
extern int id2symbol(const char * id, Symbol ** sym);

/*************** Functions for retrieving symbol properties ***************************************/
/*
 * Each function retrieves one particular attribute of an object or type.
 * On error returns -1 and sets errno.
 * On success returns 0.
 */

/* Get symbol class */
extern int get_symbol_class(const Symbol * sym, int * symbol_class);

/* Get symbol type.
 * If the symbol is a modified type, like "volatile int", return original (unmodified) type.
 * If the symbol is unmodified type, return the symbol itself. */
extern int get_symbol_type(const Symbol * sym, Symbol ** type);

/* Get type class, see TYPE_CLASS_* */
extern int get_symbol_type_class(const Symbol * sym, int * type_class);

/* Get symbol owner ID and update policy ID.
 * Symbol owner can be memory space or executable context.
 * Certain changes in owner state can invalidate cached symbol properties.
 * Update policy ID selects a specific set of rules that a client should follow
 * if it wants to cache symbol properties.
 * The string returned shall not be modified by the client,
 * and it may be overwritten by a subsequent calls to symbol functions */
extern int get_symbol_update_policy(const Symbol * sym, char ** parent_id, int * policy);

/* Get symbol name.
 * The string returned shall not be modified by the client,
 * and it may be overwritten by a subsequent calls to symbol functions */
extern int get_symbol_name(const Symbol * sym, char ** name);

/* Get value size of the type, in bytes */
extern int get_symbol_size(const Symbol * sym, ContextAddress * size);

/* Get base type: pointer or member pointer - pointed object type,
 * array - elements type, function - result type */
extern int get_symbol_base_type(const Symbol * sym, Symbol ** base_type);

/* Get array index type */
extern int get_symbol_index_type(const Symbol * sym, Symbol ** index_type);

/* Get containing type: field (member) or member pointer - parent structure */
extern int get_symbol_container(const Symbol * sym, Symbol ** container);

/* Get array length (number of elements) */
extern int get_symbol_length(const Symbol * sym, ContextAddress * length);

/* Get array index lower bound (index of first element) */
extern int get_symbol_lower_bound(const Symbol * sym, int64_t * value);

/* Get children IDs of a type (struct, union, class, function and enum).
 * The array returned is allocated by tmp_alloc() */
extern int get_symbol_children(const Symbol * sym, Symbol *** children, int * count);

/* Get offset in parent type (fields) */
extern int get_symbol_offset(const Symbol * sym, ContextAddress * offset);

/* Get value (constant objects and enums).
 * The array returned shall not be modified by the client,
 * and it may be overwritten by a subsequent calls to symbol functions */
extern int get_symbol_value(const Symbol * sym, void ** value, size_t * size, int * big_endian);

/* Get address (variables) */
extern int get_symbol_address(const Symbol * sym, ContextAddress * address);

/* Get register if the symbol is a register variable */
extern int get_symbol_register(const Symbol * sym, Context ** ctx, int * frame, RegisterDefinition ** reg);

/* Get symbol flags, see SYM_FLAG_* */
extern int get_symbol_flags(const Symbol * sym, SYM_FLAGS * flags);

/* Get additional symbol properties, see SymbolProperties */
extern int get_symbol_props(const Symbol * sym, SymbolProperties * props);

/* Get symbol stack frame */
extern int get_symbol_frame(const Symbol * sym, Context ** ctx, int * frame);

/* Get a type that represents an array of elements of given base type.
 * If 'length' is zero, returned type represents pointer to given type */
extern int get_array_symbol(const Symbol * sym, ContextAddress length, Symbol ** ptr);

/*************************************************************************************************/

/*
 * Check if given address is inside a PLT section, then return run-time address of the section.
 * Return 0 and set errno in case of an error.
 * If not a PLT address, return 0;
 */
extern ContextAddress is_plt_section(Context * ctx, ContextAddress addr);

/*
 * Get instruction set architecture name for given address in a context memory.
 * 'range_addr' and 'range_size' are assigned an address range of same ISA.
 * Return -1 and set errno in case of an error.
 * Return 0 on success.
 */
extern int get_context_isa(Context * ctx, ContextAddress addr, const char ** isa,
        ContextAddress * range_addr, ContextAddress * range_size);

/*
 * Get object location information
 * Return -1 and set errno in case of an error.
 * Return 0 on success.
 */
extern int get_location_info(const Symbol * sym, LocationInfo ** info);

/*
 * Get information about function call injection.
 * Return -1 and set errno in case of an error.
 * Return 0 on success.
 */
extern int get_funccall_info(const Symbol * func,
        const Symbol ** args, unsigned args_cnt, FunctionCallInfo ** info);

/*
 * For given context and instruction address,
 * search for stack tracing information.
 * Return -1 and set errno in case of an error.
 * Return 0 on success.
 * Set 'info' to NULL if no stack tracing information found for the address.
 */
extern int get_stack_tracing_info(Context * ctx, ContextAddress addr, StackTracingInfo ** info);

/*
 * Get info on a symbol file that is used for a given address.
 * Return -1 and set errno in case of an error.
 * Return 0 on success.
 * Set 'info' to NULL if no symbol file found for the address.
 */
extern int get_symbol_file_info(Context * ctx, ContextAddress addr, SymbolFileInfo ** info);

/*
 * Initialize symbol service.
 */
extern void ini_symbols_service(Protocol * proto);
extern void ini_symbols_lib(void);

#endif /* ENABLE_Symbols */

#endif /* D_symbols */
