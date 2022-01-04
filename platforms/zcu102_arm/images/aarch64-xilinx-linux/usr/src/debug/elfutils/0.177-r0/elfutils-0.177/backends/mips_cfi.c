/* MIPS ABI-specified defaults for DWARF CFI.
   Copyright (C) 2018 Kurt Roeckx, Inc.
   This file is part of elfutils.

   This file is free software; you can redistribute it and/or modify
   it under the terms of either

     * the GNU Lesser General Public License as published by the Free
       Software Foundation; either version 3 of the License, or (at
       your option) any later version

   or

     * the GNU General Public License as published by the Free
       Software Foundation; either version 2 of the License, or (at
       your option) any later version

   or both in parallel, as here.

   elfutils is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received copies of the GNU General Public License and
   the GNU Lesser General Public License along with this program.  If
   not, see <http://www.gnu.org/licenses/>.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <dwarf.h>

#define BACKEND mips_
#include "libebl_CPU.h"

int
mips_abi_cfi (Ebl *ebl __attribute__ ((unused)), Dwarf_CIE *abi_info)
{
  static const uint8_t abi_cfi[] =
    {
      /* Call-saved regs.  */
      DW_CFA_same_value, ULEB128_7 (16), /* $16 */
      DW_CFA_same_value, ULEB128_7 (17), /* $17 */
      DW_CFA_same_value, ULEB128_7 (18), /* $18 */
      DW_CFA_same_value, ULEB128_7 (19), /* $19 */
      DW_CFA_same_value, ULEB128_7 (20), /* $20 */
      DW_CFA_same_value, ULEB128_7 (21), /* $21 */
      DW_CFA_same_value, ULEB128_7 (22), /* $22 */
      DW_CFA_same_value, ULEB128_7 (23), /* $23 */
      DW_CFA_same_value, ULEB128_7 (28), /* $28 */
      DW_CFA_same_value, ULEB128_7 (29), /* $29 */
      DW_CFA_same_value, ULEB128_7 (30), /* $30 */

      DW_CFA_same_value, ULEB128_7 (52), /* $f20 */
      DW_CFA_same_value, ULEB128_7 (53), /* $f21 */
      DW_CFA_same_value, ULEB128_7 (54), /* $f22 */
      DW_CFA_same_value, ULEB128_7 (55), /* $f23 */
      DW_CFA_same_value, ULEB128_7 (56), /* $f24 */
      DW_CFA_same_value, ULEB128_7 (57), /* $f25 */
      DW_CFA_same_value, ULEB128_7 (58), /* $f26 */
      DW_CFA_same_value, ULEB128_7 (59), /* $f27 */
      DW_CFA_same_value, ULEB128_7 (60), /* $f28 */
      DW_CFA_same_value, ULEB128_7 (61), /* $f29 */
      DW_CFA_same_value, ULEB128_7 (62), /* $f30 */
      DW_CFA_same_value, ULEB128_7 (63), /* $f31 */

      /* The CFA is the SP.  */
      DW_CFA_def_cfa, ULEB128_7 (29), ULEB128_7 (0),
    };

  abi_info->initial_instructions = abi_cfi;
  abi_info->initial_instructions_end = &abi_cfi[sizeof abi_cfi];
  abi_info->data_alignment_factor = 4;

  abi_info->return_address_register = 31; /* $31 */

  return 0;
}
