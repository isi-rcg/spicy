/*******************************************************************************
 * Copyright (c) 2011-2019 Wind River Systems, Inc. and others.
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


#define elf_relocate elf_relocate_i386
#include <machine/i386/tcf/dwarfreloc-mdep.h>
#undef elf_relocate

#define elf_relocate elf_relocate_x86_64
#include <machine/x86_64/tcf/dwarfreloc-mdep.h>
#undef elf_relocate

#define elf_relocate elf_relocate_arm
#include <machine/arm/tcf/dwarfreloc-mdep.h>
#undef elf_relocate

#define elf_relocate elf_relocate_a64
#include <machine/a64/tcf/dwarfreloc-mdep.h>
#undef elf_relocate

#define elf_relocate elf_relocate_powerpc
#include <machine/powerpc/tcf/dwarfreloc-mdep.h>
#undef elf_relocate

#define elf_relocate elf_relocate_ppc64
#include <machine/ppc64/tcf/dwarfreloc-mdep.h>
#undef elf_relocate

#define elf_relocate elf_relocate_microblaze
#include <machine/microblaze/tcf/dwarfreloc-mdep.h>
#undef elf_relocate

#define elf_relocate elf_relocate_sparc
#include <machine/sparc/tcf/dwarfreloc-mdep.h>
#undef elf_relocate

#define elf_relocate elf_relocate_riscv
#include <machine/riscv64/tcf/dwarfreloc-mdep.h>
#undef elf_relocate

static ElfRelocateFunc elf_relocate_funcs[] = {
    { EM_386,       elf_relocate_i386 },
    { EM_X86_64,    elf_relocate_x86_64 },
    { EM_ARM,       elf_relocate_arm },
    { EM_AARCH64,   elf_relocate_a64 },
    { EM_PPC,       elf_relocate_powerpc },
    { EM_PPC64,     elf_relocate_ppc64 },
    { EM_MICROBLAZE, elf_relocate_microblaze },
    { EM_SPARC,     elf_relocate_sparc },
    { EM_RISCV,     elf_relocate_riscv },
    { EM_NONE, NULL }
};
