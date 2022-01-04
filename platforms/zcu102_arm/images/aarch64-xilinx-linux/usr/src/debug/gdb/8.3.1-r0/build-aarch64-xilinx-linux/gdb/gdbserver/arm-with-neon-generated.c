/* *INDENT-OFF* */ /* THIS FILE IS GENERATED */

/* A register protocol for GDB, the GNU debugger.
   Copyright (C) 2001-2013 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* This file was created with the aid of ``regdat.sh'' and ``../../../gdb-8.3.1/gdb/gdbserver/../regformats/arm/arm-with-neon.dat''.  */

#include "server.h"
#include "regdef.h"
#include "tdesc.h"

const struct target_desc *tdesc_arm_with_neon;

void
init_registers_arm_with_neon (void)
{
  static struct target_desc tdesc_arm_with_neon_s;
  struct target_desc *result = &tdesc_arm_with_neon_s;
  struct tdesc_feature *feature = tdesc_create_feature (result, "arm_with_neon");
  tdesc_create_reg (feature, "r0",
  0, 0, NULL, 32, NULL);
  tdesc_create_reg (feature, "r1",
  0, 0, NULL, 32, NULL);
  tdesc_create_reg (feature, "r2",
  0, 0, NULL, 32, NULL);
  tdesc_create_reg (feature, "r3",
  0, 0, NULL, 32, NULL);
  tdesc_create_reg (feature, "r4",
  0, 0, NULL, 32, NULL);
  tdesc_create_reg (feature, "r5",
  0, 0, NULL, 32, NULL);
  tdesc_create_reg (feature, "r6",
  0, 0, NULL, 32, NULL);
  tdesc_create_reg (feature, "r7",
  0, 0, NULL, 32, NULL);
  tdesc_create_reg (feature, "r8",
  0, 0, NULL, 32, NULL);
  tdesc_create_reg (feature, "r9",
  0, 0, NULL, 32, NULL);
  tdesc_create_reg (feature, "r10",
  0, 0, NULL, 32, NULL);
  tdesc_create_reg (feature, "r11",
  0, 0, NULL, 32, NULL);
  tdesc_create_reg (feature, "r12",
  0, 0, NULL, 32, NULL);
  tdesc_create_reg (feature, "sp",
  0, 0, NULL, 32, NULL);
  tdesc_create_reg (feature, "lr",
  0, 0, NULL, 32, NULL);
  tdesc_create_reg (feature, "pc",
  0, 0, NULL, 32, NULL);
  tdesc_create_reg (feature, "",
  0, 0, NULL, 0, NULL);
  tdesc_create_reg (feature, "",
  0, 0, NULL, 0, NULL);
  tdesc_create_reg (feature, "",
  0, 0, NULL, 0, NULL);
  tdesc_create_reg (feature, "",
  0, 0, NULL, 0, NULL);
  tdesc_create_reg (feature, "",
  0, 0, NULL, 0, NULL);
  tdesc_create_reg (feature, "",
  0, 0, NULL, 0, NULL);
  tdesc_create_reg (feature, "",
  0, 0, NULL, 0, NULL);
  tdesc_create_reg (feature, "",
  0, 0, NULL, 0, NULL);
  tdesc_create_reg (feature, "",
  0, 0, NULL, 0, NULL);
  tdesc_create_reg (feature, "cpsr",
  0, 0, NULL, 32, NULL);
  tdesc_create_reg (feature, "d0",
  0, 0, NULL, 64, NULL);
  tdesc_create_reg (feature, "d1",
  0, 0, NULL, 64, NULL);
  tdesc_create_reg (feature, "d2",
  0, 0, NULL, 64, NULL);
  tdesc_create_reg (feature, "d3",
  0, 0, NULL, 64, NULL);
  tdesc_create_reg (feature, "d4",
  0, 0, NULL, 64, NULL);
  tdesc_create_reg (feature, "d5",
  0, 0, NULL, 64, NULL);
  tdesc_create_reg (feature, "d6",
  0, 0, NULL, 64, NULL);
  tdesc_create_reg (feature, "d7",
  0, 0, NULL, 64, NULL);
  tdesc_create_reg (feature, "d8",
  0, 0, NULL, 64, NULL);
  tdesc_create_reg (feature, "d9",
  0, 0, NULL, 64, NULL);
  tdesc_create_reg (feature, "d10",
  0, 0, NULL, 64, NULL);
  tdesc_create_reg (feature, "d11",
  0, 0, NULL, 64, NULL);
  tdesc_create_reg (feature, "d12",
  0, 0, NULL, 64, NULL);
  tdesc_create_reg (feature, "d13",
  0, 0, NULL, 64, NULL);
  tdesc_create_reg (feature, "d14",
  0, 0, NULL, 64, NULL);
  tdesc_create_reg (feature, "d15",
  0, 0, NULL, 64, NULL);
  tdesc_create_reg (feature, "d16",
  0, 0, NULL, 64, NULL);
  tdesc_create_reg (feature, "d17",
  0, 0, NULL, 64, NULL);
  tdesc_create_reg (feature, "d18",
  0, 0, NULL, 64, NULL);
  tdesc_create_reg (feature, "d19",
  0, 0, NULL, 64, NULL);
  tdesc_create_reg (feature, "d20",
  0, 0, NULL, 64, NULL);
  tdesc_create_reg (feature, "d21",
  0, 0, NULL, 64, NULL);
  tdesc_create_reg (feature, "d22",
  0, 0, NULL, 64, NULL);
  tdesc_create_reg (feature, "d23",
  0, 0, NULL, 64, NULL);
  tdesc_create_reg (feature, "d24",
  0, 0, NULL, 64, NULL);
  tdesc_create_reg (feature, "d25",
  0, 0, NULL, 64, NULL);
  tdesc_create_reg (feature, "d26",
  0, 0, NULL, 64, NULL);
  tdesc_create_reg (feature, "d27",
  0, 0, NULL, 64, NULL);
  tdesc_create_reg (feature, "d28",
  0, 0, NULL, 64, NULL);
  tdesc_create_reg (feature, "d29",
  0, 0, NULL, 64, NULL);
  tdesc_create_reg (feature, "d30",
  0, 0, NULL, 64, NULL);
  tdesc_create_reg (feature, "d31",
  0, 0, NULL, 64, NULL);
  tdesc_create_reg (feature, "fpscr",
  0, 0, NULL, 32, NULL);

static const char *expedite_regs_arm_with_neon[] = { "r11", "sp", "pc", 0 };
#ifndef IN_PROCESS_AGENT
static const char *xmltarget_arm_with_neon = "arm-with-neon.xml";

  result->xmltarget = xmltarget_arm_with_neon;
#endif

  init_target_desc (result, expedite_regs_arm_with_neon);

  tdesc_arm_with_neon = result;
}
