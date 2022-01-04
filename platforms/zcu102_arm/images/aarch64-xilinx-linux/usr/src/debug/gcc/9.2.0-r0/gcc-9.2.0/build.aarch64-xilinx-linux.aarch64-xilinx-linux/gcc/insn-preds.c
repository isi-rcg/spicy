/* Generated automatically by the program 'build/genpreds'
   from the machine description file '../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md'.  */

#define IN_TARGET_CODE 1
#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "backend.h"
#include "predict.h"
#include "tree.h"
#include "rtl.h"
#include "alias.h"
#include "varasm.h"
#include "stor-layout.h"
#include "calls.h"
#include "memmodel.h"
#include "tm_p.h"
#include "insn-config.h"
#include "recog.h"
#include "output.h"
#include "flags.h"
#include "df.h"
#include "resource.h"
#include "diagnostic-core.h"
#include "reload.h"
#include "regs.h"
#include "emit-rtl.h"
#include "tm-constrs.h"
#include "target.h"

int
cc_register (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (GET_CODE (op) == REG) && ((
#line 23 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(REGNO (op) == CC_REGNUM)) && ((
#line 24 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(mode == GET_MODE (op))) || (
#line 25 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(mode == VOIDmode
			      && GET_MODE_CLASS (GET_MODE (op)) == MODE_CC))));
}

int
aarch64_call_insn_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return ((GET_CODE (op) == SYMBOL_REF) && (
(mode == VOIDmode || GET_MODE (op) == mode))) || (register_operand (op, mode));
}

int
aarch64_general_reg (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (register_operand (op, mode)) && (
#line 35 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(REGNO_REG_CLASS (REGNO (op)) == GENERAL_REGS));
}

int
const0_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (GET_CODE (op) == CONST_INT) && (
#line 40 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(op == CONST0_RTX (mode)));
}

int
subreg_lowpart_operator (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (GET_CODE (op) == SUBREG) && (
#line 44 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(subreg_lowpart_p (op)));
}

int
aarch64_ccmp_immediate (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (GET_CODE (op) == CONST_INT) && (
#line 48 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(IN_RANGE (INTVAL (op), -31, 31)));
}

int
aarch64_ccmp_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (register_operand (op, mode)) || (aarch64_ccmp_immediate (op, mode));
}

int
aarch64_simd_register (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return ((GET_CODE (op) == REG) && ((
#line 56 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(REGNO_REG_CLASS (REGNO (op)) == FP_LO_REGS)) || (
#line 57 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(REGNO_REG_CLASS (REGNO (op)) == FP_REGS)))) && (
(mode == VOIDmode || GET_MODE (op) == mode));
}

int
aarch64_reg_or_zero (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  switch (GET_CODE (op))
    {
    case REG:
    case SUBREG:
    case CONST_INT:
      break;
    default:
      return false;
    }
  return ((register_operand (op, mode)) || (
#line 62 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(op == const0_rtx))) && (
(mode == VOIDmode || GET_MODE (op) == mode || GET_MODE (op) == VOIDmode));
}

int
aarch64_reg_or_fp_zero (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (register_operand (op, mode)) || (((GET_CODE (op) == CONST_DOUBLE) && (
#line 67 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(aarch64_float_const_zero_rtx_p (op)))) && (
(mode == VOIDmode || GET_MODE (op) == mode)));
}

int
aarch64_reg_zero_or_fp_zero (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (aarch64_reg_or_fp_zero (op, mode)) || (aarch64_reg_or_zero (op, mode));
}

int
aarch64_reg_zero_or_m1_or_1 (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  switch (GET_CODE (op))
    {
    case REG:
    case SUBREG:
    case CONST_INT:
      break;
    default:
      return false;
    }
  return ((register_operand (op, mode)) || ((
#line 76 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(op == const0_rtx)) || ((
#line 77 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(op == constm1_rtx)) || (
#line 78 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(op == const1_rtx))))) && (
(mode == VOIDmode || GET_MODE (op) == mode || GET_MODE (op) == VOIDmode));
}

int
aarch64_reg_or_orr_imm (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (register_operand (op, mode)) || (((GET_CODE (op) == CONST_VECTOR) && (
#line 83 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(aarch64_simd_valid_immediate (op, NULL,
							AARCH64_CHECK_ORR)))) && (
(mode == VOIDmode || GET_MODE (op) == mode)));
}

int
aarch64_reg_or_bic_imm (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (register_operand (op, mode)) || (((GET_CODE (op) == CONST_VECTOR) && (
#line 89 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(aarch64_simd_valid_immediate (op, NULL,
							AARCH64_CHECK_BIC)))) && (
(mode == VOIDmode || GET_MODE (op) == mode)));
}

int
aarch64_fp_compare_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (register_operand (op, mode)) || (((GET_CODE (op) == CONST_DOUBLE) && (
#line 95 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(aarch64_float_const_zero_rtx_p (op)))) && (
(mode == VOIDmode || GET_MODE (op) == mode)));
}

int
aarch64_fp_pow2 (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return ((GET_CODE (op) == CONST_DOUBLE) && (
#line 99 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(aarch64_fpconst_pow_of_2 (op) > 0))) && (
(mode == VOIDmode || GET_MODE (op) == mode));
}

int
aarch64_fp_vec_pow2 (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (
#line 102 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(aarch64_vec_fpconst_pow_of_2 (op) > 0)) && (
(mode == VOIDmode || GET_MODE (op) == mode || GET_MODE (op) == VOIDmode));
}

int
aarch64_sve_cnt_immediate (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return ((GET_CODE (op) == CONST_POLY_INT) && (
#line 106 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(aarch64_sve_cnt_immediate_p (op)))) && (
(mode == VOIDmode || GET_MODE (op) == mode));
}

int
aarch64_sub_immediate (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (GET_CODE (op) == CONST_INT) && (
#line 110 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(aarch64_uimm12_shift (-INTVAL (op))));
}

int
aarch64_plus_immediate (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (GET_CODE (op) == CONST_INT) && ((
#line 114 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(aarch64_uimm12_shift (INTVAL (op)))) || (
#line 115 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(aarch64_uimm12_shift (-INTVAL (op)))));
}

int
aarch64_plus_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (register_operand (op, mode)) || (aarch64_plus_immediate (op, mode));
}

static inline int
aarch64_plushi_immediate_1 (rtx op ATTRIBUTE_UNUSED, machine_mode mode ATTRIBUTE_UNUSED)
#line 123 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
{
  HOST_WIDE_INT val = INTVAL (op);
  /* The HImode value must be zero-extendable to an SImode plus_operand.  */
  return ((val & 0xfff) == val || sext_hwi (val & 0xf000, 16) == val);
}

int
aarch64_plushi_immediate (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (GET_CODE (op) == CONST_INT) && (
(aarch64_plushi_immediate_1 (op, mode)));
}

int
aarch64_plushi_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (register_operand (op, mode)) || (aarch64_plushi_immediate (op, mode));
}

int
aarch64_pluslong_immediate (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (GET_CODE (op) == CONST_INT) && (
#line 135 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
((INTVAL (op) < 0xffffff && INTVAL (op) > -0xffffff)));
}

int
aarch64_pluslong_strict_immedate (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (aarch64_pluslong_immediate (op, mode)) && (!(aarch64_plus_immediate (op, mode)));
}

int
aarch64_sve_addvl_addpl_immediate (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return ((GET_CODE (op) == CONST_POLY_INT) && (
#line 143 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(aarch64_sve_addvl_addpl_immediate_p (op)))) && (
(mode == VOIDmode || GET_MODE (op) == mode));
}

int
aarch64_split_add_offset_immediate (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return ((GET_CODE (op) == CONST_POLY_INT) && (
#line 147 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(aarch64_add_offset_temporaries (op) == 1))) && (
(mode == VOIDmode || GET_MODE (op) == mode));
}

int
aarch64_pluslong_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (register_operand (op, mode)) || ((aarch64_pluslong_immediate (op, mode)) || (aarch64_sve_addvl_addpl_immediate (op, mode)));
}

int
aarch64_pluslong_or_poly_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (aarch64_pluslong_operand (op, mode)) || (aarch64_split_add_offset_immediate (op, mode));
}

int
aarch64_logical_immediate (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (GET_CODE (op) == CONST_INT) && (
#line 160 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(aarch64_bitmask_imm (INTVAL (op), mode)));
}

int
aarch64_logical_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (register_operand (op, mode)) || (aarch64_logical_immediate (op, mode));
}

int
aarch64_mov_imm_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (GET_CODE (op) == CONST_INT) && (
#line 168 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(aarch64_move_imm (INTVAL (op), mode)));
}

int
aarch64_logical_and_immediate (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (GET_CODE (op) == CONST_INT) && (
#line 172 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(aarch64_and_bitmask_imm (INTVAL (op), mode)));
}

int
aarch64_shift_imm_si (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (GET_CODE (op) == CONST_INT) && (
#line 176 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
((unsigned HOST_WIDE_INT) INTVAL (op) < 32));
}

int
aarch64_shift_imm_di (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (GET_CODE (op) == CONST_INT) && (
#line 180 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
((unsigned HOST_WIDE_INT) INTVAL (op) < 64));
}

int
aarch64_shift_imm64_di (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (GET_CODE (op) == CONST_INT) && (
#line 184 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
((unsigned HOST_WIDE_INT) INTVAL (op) <= 64));
}

int
aarch64_reg_or_shift_imm_si (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (register_operand (op, mode)) || (aarch64_shift_imm_si (op, mode));
}

int
aarch64_reg_or_shift_imm_di (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (register_operand (op, mode)) || (aarch64_shift_imm_di (op, mode));
}

int
aarch64_imm3 (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (GET_CODE (op) == CONST_INT) && (
#line 198 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
((unsigned HOST_WIDE_INT) INTVAL (op) <= 4));
}

int
aarch64_imm2 (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (GET_CODE (op) == CONST_INT) && (
#line 204 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(UINTVAL (op) <= 3));
}

int
aarch64_lane_imm3 (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (GET_CODE (op) == CONST_INT) && (
#line 210 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(UINTVAL (op) <= 7));
}

int
aarch64_imm24 (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (GET_CODE (op) == CONST_INT) && (
#line 215 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(IN_RANGE (UINTVAL (op), 0, 0xffffff)));
}

int
aarch64_pwr_imm3 (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (GET_CODE (op) == CONST_INT) && (
#line 219 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(INTVAL (op) != 0
		    && (unsigned) exact_log2 (INTVAL (op)) <= 4));
}

int
aarch64_pwr_2_si (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (GET_CODE (op) == CONST_INT) && (
#line 224 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(INTVAL (op) != 0
		    && (unsigned) exact_log2 (INTVAL (op)) < 32));
}

int
aarch64_pwr_2_di (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (GET_CODE (op) == CONST_INT) && (
#line 229 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(INTVAL (op) != 0
		    && (unsigned) exact_log2 (INTVAL (op)) < 64));
}

int
aarch64_mem_pair_offset (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (GET_CODE (op) == CONST_INT) && (
#line 234 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(aarch64_offset_7bit_signed_scaled_p (mode, INTVAL (op))));
}

int
aarch64_mem_pair_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return ((GET_CODE (op) == MEM) && (
#line 238 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(aarch64_legitimate_address_p (mode, XEXP (op, 0), false,
						  ADDR_QUERY_LDP_STP)))) && (
(mode == VOIDmode || GET_MODE (op) == mode));
}

int
aarch64_mem_pair_lanes_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return ((GET_CODE (op) == MEM) && (
#line 245 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(aarch64_legitimate_address_p (GET_MODE (op), XEXP (op, 0),
						  false,
						  ADDR_QUERY_LDP_STP_N)))) && (
(mode == VOIDmode || GET_MODE (op) == mode));
}

int
aarch64_prefetch_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (
#line 250 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(aarch64_address_valid_for_prefetch_p (op, false))) && (
(mode == VOIDmode || GET_MODE (op) == mode || GET_MODE (op) == VOIDmode));
}

static inline int
aarch64_valid_symref_1 (rtx op ATTRIBUTE_UNUSED, machine_mode mode ATTRIBUTE_UNUSED)
#line 254 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
{
  return (aarch64_classify_symbolic_expression (op)
	  != SYMBOL_FORCE_TO_MEM);
}

int
aarch64_valid_symref (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  switch (GET_CODE (op))
    {
    case CONST:
    case SYMBOL_REF:
    case LABEL_REF:
      break;
    default:
      return false;
    }
  return (
(aarch64_valid_symref_1 (op, mode))) && (
(mode == VOIDmode || GET_MODE (op) == mode || GET_MODE (op) == VOIDmode));
}

static inline int
aarch64_tls_ie_symref_1 (rtx op ATTRIBUTE_UNUSED, machine_mode mode ATTRIBUTE_UNUSED)
#line 261 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
{
  switch (GET_CODE (op))
    {
    case CONST:
      op = XEXP (op, 0);
      if (GET_CODE (op) != PLUS
	  || GET_CODE (XEXP (op, 0)) != SYMBOL_REF
	  || GET_CODE (XEXP (op, 1)) != CONST_INT)
	return false;
      op = XEXP (op, 0);
      /* FALLTHRU */

    case SYMBOL_REF:
      return SYMBOL_REF_TLS_MODEL (op) == TLS_MODEL_INITIAL_EXEC;

    default:
      gcc_unreachable ();
    }
}

int
aarch64_tls_ie_symref (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  switch (GET_CODE (op))
    {
    case CONST:
    case SYMBOL_REF:
    case LABEL_REF:
      break;
    default:
      return false;
    }
  return (
(aarch64_tls_ie_symref_1 (op, mode))) && (
(mode == VOIDmode || GET_MODE (op) == mode || GET_MODE (op) == VOIDmode));
}

static inline int
aarch64_tls_le_symref_1 (rtx op ATTRIBUTE_UNUSED, machine_mode mode ATTRIBUTE_UNUSED)
#line 283 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
{
  switch (GET_CODE (op))
    {
    case CONST:
      op = XEXP (op, 0);
      if (GET_CODE (op) != PLUS
	  || GET_CODE (XEXP (op, 0)) != SYMBOL_REF
	  || GET_CODE (XEXP (op, 1)) != CONST_INT)
	return false;
      op = XEXP (op, 0);
      /* FALLTHRU */

    case SYMBOL_REF:
      return SYMBOL_REF_TLS_MODEL (op) == TLS_MODEL_LOCAL_EXEC;

    default:
      gcc_unreachable ();
    }
}

int
aarch64_tls_le_symref (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  switch (GET_CODE (op))
    {
    case CONST:
    case SYMBOL_REF:
    case LABEL_REF:
      break;
    default:
      return false;
    }
  return (
(aarch64_tls_le_symref_1 (op, mode))) && (
(mode == VOIDmode || GET_MODE (op) == mode || GET_MODE (op) == VOIDmode));
}

int
aarch64_mov_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  switch (GET_CODE (op))
    {
    case REG:
    case SUBREG:
    case MEM:
    case CONST:
    case CONST_INT:
    case SYMBOL_REF:
    case LABEL_REF:
    case HIGH:
    case CONST_POLY_INT:
    case CONST_VECTOR:
      break;
    default:
      return false;
    }
  return ((register_operand (op, mode)) || ((memory_operand (op, mode)) || (
#line 308 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(aarch64_mov_operand_p (op, mode))))) && (
(mode == VOIDmode || GET_MODE (op) == mode || GET_MODE (op) == VOIDmode));
}

int
aarch64_nonmemory_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  switch (GET_CODE (op))
    {
    case REG:
    case SUBREG:
    case CONST:
    case CONST_INT:
    case SYMBOL_REF:
    case LABEL_REF:
    case HIGH:
    case CONST_POLY_INT:
    case CONST_VECTOR:
      break;
    default:
      return false;
    }
  return ((register_operand (op, mode)) || (
#line 314 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(aarch64_mov_operand_p (op, mode)))) && (
(mode == VOIDmode || GET_MODE (op) == mode || GET_MODE (op) == VOIDmode));
}

int
aarch64_movti_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (register_operand (op, mode)) || ((memory_operand (op, mode)) || ((const_scalar_int_operand (op, mode)) && (
#line 320 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(aarch64_mov128_immediate (op)))));
}

int
aarch64_reg_or_imm (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (register_operand (op, mode)) || (const_scalar_int_operand (op, mode));
}

int
aarch64_smin (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (GET_CODE (op) == SMIN) && (
(mode == VOIDmode || GET_MODE (op) == mode));
}

int
aarch64_umin (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (GET_CODE (op) == UMIN) && (
(mode == VOIDmode || GET_MODE (op) == mode));
}

int
aarch64_comparison_operator (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  switch (GET_CODE (op))
    {
    case EQ:
    case NE:
    case LE:
    case LT:
    case GE:
    case GT:
    case GEU:
    case GTU:
    case LEU:
    case LTU:
    case UNORDERED:
    case ORDERED:
    case UNLT:
    case UNLE:
    case UNGE:
    case UNGT:
      return true;
    default:
      break;
    }
  return false;
}

int
aarch64_comparison_operator_mode (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  switch (GET_CODE (op))
    {
    case EQ:
    case NE:
    case LE:
    case LT:
    case GE:
    case GT:
    case GEU:
    case GTU:
    case LEU:
    case LTU:
    case UNORDERED:
    case ORDERED:
    case UNLT:
    case UNLE:
    case UNGE:
    case UNGT:
      break;
    default:
      return false;
    }
  return 
(mode == VOIDmode || GET_MODE (op) == mode);
}

static inline int
aarch64_comparison_operation_1 (rtx op ATTRIBUTE_UNUSED, machine_mode mode ATTRIBUTE_UNUSED)
#line 347 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
{
  if (XEXP (op, 1) != const0_rtx)
    return false;
  rtx op0 = XEXP (op, 0);
  if (!REG_P (op0) || REGNO (op0) != CC_REGNUM)
    return false;
  return aarch64_get_condition_code (op) >= 0;
}

int
aarch64_comparison_operation (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  switch (GET_CODE (op))
    {
    case EQ:
    case NE:
    case LE:
    case LT:
    case GE:
    case GT:
    case GEU:
    case GTU:
    case LEU:
    case LTU:
    case UNORDERED:
    case ORDERED:
    case UNLT:
    case UNLE:
    case UNGE:
    case UNGT:
      break;
    default:
      return false;
    }
  return 
(aarch64_comparison_operation_1 (op, mode));
}

int
aarch64_equality_operator (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  switch (GET_CODE (op))
    {
    case EQ:
    case NE:
      return true;
    default:
      break;
    }
  return false;
}

static inline int
aarch64_carry_operation_1 (rtx op ATTRIBUTE_UNUSED, machine_mode mode ATTRIBUTE_UNUSED)
#line 361 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
{
  if (XEXP (op, 1) != const0_rtx)
    return false;
  rtx op0 = XEXP (op, 0);
  if (!REG_P (op0) || REGNO (op0) != CC_REGNUM)
    return false;
  machine_mode ccmode = GET_MODE (op0);
  if (ccmode == CC_Cmode)
    return GET_CODE (op) == LTU;
  if (ccmode == CC_ADCmode || ccmode == CCmode)
    return GET_CODE (op) == GEU;
  return false;
}

int
aarch64_carry_operation (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  switch (GET_CODE (op))
    {
    case LTU:
    case GEU:
      break;
    default:
      return false;
    }
  return 
(aarch64_carry_operation_1 (op, mode));
}

static inline int
aarch64_borrow_operation_1 (rtx op ATTRIBUTE_UNUSED, machine_mode mode ATTRIBUTE_UNUSED)
#line 379 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
{
  if (XEXP (op, 1) != const0_rtx)
    return false;
  rtx op0 = XEXP (op, 0);
  if (!REG_P (op0) || REGNO (op0) != CC_REGNUM)
    return false;
  machine_mode ccmode = GET_MODE (op0);
  if (ccmode == CC_Cmode)
    return GET_CODE (op) == GEU;
  if (ccmode == CC_ADCmode || ccmode == CCmode)
    return GET_CODE (op) == LTU;
  return false;
}

int
aarch64_borrow_operation (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  switch (GET_CODE (op))
    {
    case GEU:
    case LTU:
      break;
    default:
      return false;
    }
  return 
(aarch64_borrow_operation_1 (op, mode));
}

int
aarch64_sync_memory_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (memory_operand (op, mode)) && (GET_CODE (XEXP (op, 0)) == REG);
}

static inline int
aarch64_9bit_offset_memory_operand_1 (rtx op ATTRIBUTE_UNUSED, machine_mode mode ATTRIBUTE_UNUSED)
#line 404 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
{
  rtx mem_op = XEXP (op, 0);

  if (REG_P (mem_op))
    return GET_MODE (mem_op) == DImode;

  rtx plus_op0 = XEXP (mem_op, 0);
  rtx plus_op1 = XEXP (mem_op, 1);

  if (GET_MODE (plus_op0) != DImode)
    return false;

  poly_int64 offset;
  if (!poly_int_rtx_p (plus_op1, &offset))
    gcc_unreachable ();

  return aarch64_offset_9bit_signed_unscaled_p (mode, offset);
}

int
aarch64_9bit_offset_memory_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return ((memory_operand (op, mode)) && ((GET_CODE (XEXP (op, 0)) == REG) || ((GET_CODE (XEXP (op, 0)) == PLUS) && ((GET_CODE (XEXP (XEXP (op, 0), 0)) == REG) && (GET_CODE (XEXP (XEXP (op, 0), 1)) == CONST_INT))))) && (
(aarch64_9bit_offset_memory_operand_1 (op, mode)));
}

int
aarch64_rcpc_memory_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (
#line 424 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(AARCH64_ISA_RCPC8_4)) ? (aarch64_9bit_offset_memory_operand (op, mode)) : (aarch64_sync_memory_operand (op, mode));
}

static inline int
vect_par_cnst_hi_half_1 (rtx op ATTRIBUTE_UNUSED, machine_mode mode ATTRIBUTE_UNUSED)
#line 431 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
{
  return aarch64_simd_check_vect_par_cnst_half (op, mode, true);
}

int
vect_par_cnst_hi_half (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (GET_CODE (op) == PARALLEL) && (
(vect_par_cnst_hi_half_1 (op, mode)));
}

static inline int
vect_par_cnst_lo_half_1 (rtx op ATTRIBUTE_UNUSED, machine_mode mode ATTRIBUTE_UNUSED)
#line 437 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
{
  return aarch64_simd_check_vect_par_cnst_half (op, mode, false);
}

int
vect_par_cnst_lo_half (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (GET_CODE (op) == PARALLEL) && (
(vect_par_cnst_lo_half_1 (op, mode)));
}

static inline int
aarch64_simd_lshift_imm_1 (rtx op ATTRIBUTE_UNUSED, machine_mode mode ATTRIBUTE_UNUSED)
#line 443 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
{
  return aarch64_simd_shift_imm_p (op, mode, true);
}

int
aarch64_simd_lshift_imm (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  switch (GET_CODE (op))
    {
    case CONST:
    case CONST_VECTOR:
      break;
    default:
      return false;
    }
  return 
(aarch64_simd_lshift_imm_1 (op, mode));
}

static inline int
aarch64_simd_rshift_imm_1 (rtx op ATTRIBUTE_UNUSED, machine_mode mode ATTRIBUTE_UNUSED)
#line 449 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
{
  return aarch64_simd_shift_imm_p (op, mode, false);
}

int
aarch64_simd_rshift_imm (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  switch (GET_CODE (op))
    {
    case CONST:
    case CONST_VECTOR:
      break;
    default:
      return false;
    }
  return 
(aarch64_simd_rshift_imm_1 (op, mode));
}

int
aarch64_simd_imm_zero (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  switch (GET_CODE (op))
    {
    case CONST:
    case CONST_VECTOR:
      break;
    default:
      return false;
    }
  return (
#line 455 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(op == CONST0_RTX (GET_MODE (op)))) && (
(mode == VOIDmode || GET_MODE (op) == mode));
}

int
aarch64_simd_or_scalar_imm_zero (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  switch (GET_CODE (op))
    {
    case CONST_INT:
    case CONST_DOUBLE:
    case CONST:
    case CONST_VECTOR:
      break;
    default:
      return false;
    }
  return (
#line 459 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(op == CONST0_RTX (GET_MODE (op)))) && (
(mode == VOIDmode || GET_MODE (op) == mode || GET_MODE (op) == VOIDmode));
}

int
aarch64_simd_imm_minus_one (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  switch (GET_CODE (op))
    {
    case CONST:
    case CONST_VECTOR:
      break;
    default:
      return false;
    }
  return (
#line 463 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(op == CONSTM1_RTX (GET_MODE (op)))) && (
(mode == VOIDmode || GET_MODE (op) == mode));
}

int
aarch64_simd_reg_or_zero (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  switch (GET_CODE (op))
    {
    case REG:
    case SUBREG:
    case CONST_INT:
    case CONST_DOUBLE:
    case CONST:
    case CONST_VECTOR:
      break;
    default:
      return false;
    }
  return ((register_operand (op, mode)) || ((
#line 468 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(op == const0_rtx)) || (aarch64_simd_or_scalar_imm_zero (op, mode)))) && (
(mode == VOIDmode || GET_MODE (op) == mode || GET_MODE (op) == VOIDmode));
}

int
aarch64_simd_struct_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return ((GET_CODE (op) == MEM) && (
#line 473 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(TARGET_SIMD && aarch64_simd_mem_operand_p (op)))) && (
(mode == VOIDmode || GET_MODE (op) == mode));
}

int
aarch64_simd_general_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (general_operand (op, mode)) && (
#line 478 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(!MEM_P (op)
		    || GET_CODE (XEXP (op, 0)) == POST_INC
		    || GET_CODE (XEXP (op, 0)) == REG));
}

int
aarch64_simd_nonimmediate_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (nonimmediate_operand (op, mode)) && (
#line 485 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(!MEM_P (op)
		    || GET_CODE (XEXP (op, 0)) == POST_INC
		    || GET_CODE (XEXP (op, 0)) == REG));
}

int
aarch64_simd_shift_imm_qi (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (GET_CODE (op) == CONST_INT) && (
#line 496 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(IN_RANGE (INTVAL (op), 0, 7)));
}

int
aarch64_simd_shift_imm_hi (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (GET_CODE (op) == CONST_INT) && (
#line 500 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(IN_RANGE (INTVAL (op), 0, 15)));
}

int
aarch64_simd_shift_imm_si (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (GET_CODE (op) == CONST_INT) && (
#line 504 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(IN_RANGE (INTVAL (op), 0, 31)));
}

int
aarch64_simd_shift_imm_di (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (GET_CODE (op) == CONST_INT) && (
#line 508 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(IN_RANGE (INTVAL (op), 0, 63)));
}

int
aarch64_simd_shift_imm_offset_qi (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (GET_CODE (op) == CONST_INT) && (
#line 512 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(IN_RANGE (INTVAL (op), 1, 8)));
}

int
aarch64_simd_shift_imm_offset_hi (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (GET_CODE (op) == CONST_INT) && (
#line 516 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(IN_RANGE (INTVAL (op), 1, 16)));
}

int
aarch64_simd_shift_imm_offset_si (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (GET_CODE (op) == CONST_INT) && (
#line 520 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(IN_RANGE (INTVAL (op), 1, 32)));
}

int
aarch64_simd_shift_imm_offset_di (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (GET_CODE (op) == CONST_INT) && (
#line 524 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(IN_RANGE (INTVAL (op), 1, 64)));
}

int
aarch64_simd_shift_imm_bitsize_qi (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (GET_CODE (op) == CONST_INT) && (
#line 528 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(IN_RANGE (INTVAL (op), 0, 8)));
}

int
aarch64_simd_shift_imm_bitsize_hi (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (GET_CODE (op) == CONST_INT) && (
#line 532 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(IN_RANGE (INTVAL (op), 0, 16)));
}

int
aarch64_simd_shift_imm_bitsize_si (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (GET_CODE (op) == CONST_INT) && (
#line 536 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(IN_RANGE (INTVAL (op), 0, 32)));
}

int
aarch64_simd_shift_imm_bitsize_di (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (GET_CODE (op) == CONST_INT) && (
#line 540 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(IN_RANGE (INTVAL (op), 0, 64)));
}

int
aarch64_constant_pool_symref (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return ((GET_CODE (op) == SYMBOL_REF) && (
#line 544 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(CONSTANT_POOL_ADDRESS_P (op)))) && (
(mode == VOIDmode || GET_MODE (op) == mode));
}

int
aarch64_constant_vector_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  switch (GET_CODE (op))
    {
    case CONST:
    case CONST_VECTOR:
      break;
    default:
      return false;
    }
  return 
(mode == VOIDmode || GET_MODE (op) == mode);
}

int
aarch64_sve_ld1r_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (memory_operand (op, mode)) && (
#line 551 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(aarch64_sve_ld1r_operand_p (op)));
}

int
aarch64_sve_ldr_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return ((GET_CODE (op) == MEM) && (
#line 557 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(aarch64_sve_ldr_operand_p (op)))) && (
(mode == VOIDmode || GET_MODE (op) == mode));
}

int
aarch64_sve_nonimmediate_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (register_operand (op, mode)) || (aarch64_sve_ldr_operand (op, mode));
}

int
aarch64_sve_general_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  switch (GET_CODE (op))
    {
    case REG:
    case SUBREG:
    case MEM:
    case CONST:
    case CONST_VECTOR:
      break;
    default:
      return false;
    }
  return ((register_operand (op, mode)) || ((aarch64_sve_ldr_operand (op, mode)) || (
#line 567 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(aarch64_mov_operand_p (op, mode))))) && (
(mode == VOIDmode || GET_MODE (op) == mode));
}

int
aarch64_sve_struct_memory_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return ((GET_CODE (op) == MEM) && (
#line 571 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(aarch64_sve_struct_memory_operand_p (op)))) && (
(mode == VOIDmode || GET_MODE (op) == mode));
}

int
aarch64_sve_struct_nonimmediate_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (register_operand (op, mode)) || (aarch64_sve_struct_memory_operand (op, mode));
}

int
aarch64_sve_dup_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (register_operand (op, mode)) || (aarch64_sve_ld1r_operand (op, mode));
}

int
aarch64_sve_arith_immediate (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  switch (GET_CODE (op))
    {
    case CONST:
    case CONST_VECTOR:
      break;
    default:
      return false;
    }
  return (
#line 585 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(aarch64_sve_arith_immediate_p (op, false))) && (
(mode == VOIDmode || GET_MODE (op) == mode));
}

int
aarch64_sve_sub_arith_immediate (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  switch (GET_CODE (op))
    {
    case CONST:
    case CONST_VECTOR:
      break;
    default:
      return false;
    }
  return (
#line 589 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(aarch64_sve_arith_immediate_p (op, true))) && (
(mode == VOIDmode || GET_MODE (op) == mode));
}

int
aarch64_sve_inc_dec_immediate (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  switch (GET_CODE (op))
    {
    case CONST:
    case CONST_VECTOR:
      break;
    default:
      return false;
    }
  return (
#line 593 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(aarch64_sve_inc_dec_immediate_p (op))) && (
(mode == VOIDmode || GET_MODE (op) == mode));
}

int
aarch64_sve_logical_immediate (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  switch (GET_CODE (op))
    {
    case CONST:
    case CONST_VECTOR:
      break;
    default:
      return false;
    }
  return (
#line 597 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(aarch64_sve_bitmask_immediate_p (op))) && (
(mode == VOIDmode || GET_MODE (op) == mode));
}

int
aarch64_sve_mul_immediate (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  switch (GET_CODE (op))
    {
    case CONST:
    case CONST_VECTOR:
      break;
    default:
      return false;
    }
  return (
#line 601 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(aarch64_const_vec_all_same_in_range_p (op, -128, 127))) && (
(mode == VOIDmode || GET_MODE (op) == mode));
}

int
aarch64_sve_dup_immediate (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  switch (GET_CODE (op))
    {
    case CONST:
    case CONST_VECTOR:
      break;
    default:
      return false;
    }
  return (
#line 605 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(aarch64_sve_dup_immediate_p (op))) && (
(mode == VOIDmode || GET_MODE (op) == mode));
}

int
aarch64_sve_cmp_vsc_immediate (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  switch (GET_CODE (op))
    {
    case CONST:
    case CONST_VECTOR:
      break;
    default:
      return false;
    }
  return (
#line 609 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(aarch64_sve_cmp_immediate_p (op, true))) && (
(mode == VOIDmode || GET_MODE (op) == mode));
}

int
aarch64_sve_cmp_vsd_immediate (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  switch (GET_CODE (op))
    {
    case CONST:
    case CONST_VECTOR:
      break;
    default:
      return false;
    }
  return (
#line 613 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(aarch64_sve_cmp_immediate_p (op, false))) && (
(mode == VOIDmode || GET_MODE (op) == mode));
}

int
aarch64_sve_index_immediate (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (GET_CODE (op) == CONST_INT) && (
#line 617 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(aarch64_sve_index_immediate_p (op)));
}

int
aarch64_sve_float_arith_immediate (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  switch (GET_CODE (op))
    {
    case CONST:
    case CONST_VECTOR:
      break;
    default:
      return false;
    }
  return (
#line 621 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(aarch64_sve_float_arith_immediate_p (op, false))) && (
(mode == VOIDmode || GET_MODE (op) == mode));
}

int
aarch64_sve_float_arith_with_sub_immediate (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  switch (GET_CODE (op))
    {
    case CONST:
    case CONST_VECTOR:
      break;
    default:
      return false;
    }
  return (
#line 625 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(aarch64_sve_float_arith_immediate_p (op, true))) && (
(mode == VOIDmode || GET_MODE (op) == mode));
}

int
aarch64_sve_float_mul_immediate (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  switch (GET_CODE (op))
    {
    case CONST:
    case CONST_VECTOR:
      break;
    default:
      return false;
    }
  return (
#line 629 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(aarch64_sve_float_mul_immediate_p (op))) && (
(mode == VOIDmode || GET_MODE (op) == mode));
}

int
aarch64_sve_arith_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (register_operand (op, mode)) || (aarch64_sve_arith_immediate (op, mode));
}

int
aarch64_sve_add_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (aarch64_sve_arith_operand (op, mode)) || ((aarch64_sve_sub_arith_immediate (op, mode)) || (aarch64_sve_inc_dec_immediate (op, mode)));
}

int
aarch64_sve_logical_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (register_operand (op, mode)) || (aarch64_sve_logical_immediate (op, mode));
}

int
aarch64_sve_lshift_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (register_operand (op, mode)) || ((aarch64_simd_lshift_imm (op, mode)) && (
(mode == VOIDmode || GET_MODE (op) == mode)));
}

int
aarch64_sve_rshift_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (register_operand (op, mode)) || ((aarch64_simd_rshift_imm (op, mode)) && (
(mode == VOIDmode || GET_MODE (op) == mode)));
}

int
aarch64_sve_mul_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (register_operand (op, mode)) || (aarch64_sve_mul_immediate (op, mode));
}

int
aarch64_sve_cmp_vsc_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (register_operand (op, mode)) || (aarch64_sve_cmp_vsc_immediate (op, mode));
}

int
aarch64_sve_cmp_vsd_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (register_operand (op, mode)) || (aarch64_sve_cmp_vsd_immediate (op, mode));
}

int
aarch64_sve_index_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (register_operand (op, mode)) || (aarch64_sve_index_immediate (op, mode));
}

int
aarch64_sve_float_arith_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (register_operand (op, mode)) || (aarch64_sve_float_arith_immediate (op, mode));
}

int
aarch64_sve_float_arith_with_sub_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (aarch64_sve_float_arith_operand (op, mode)) || (aarch64_sve_float_arith_with_sub_immediate (op, mode));
}

int
aarch64_sve_float_mul_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (register_operand (op, mode)) || (aarch64_sve_float_mul_immediate (op, mode));
}

int
aarch64_sve_vec_perm_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (register_operand (op, mode)) || (aarch64_constant_vector_operand (op, mode));
}

int
aarch64_gather_scale_operand_w (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (GET_CODE (op) == CONST_INT) && (
#line 686 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(INTVAL (op) == 1 || INTVAL (op) == 4));
}

int
aarch64_gather_scale_operand_d (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return (GET_CODE (op) == CONST_INT) && (
#line 690 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/predicates.md"
(INTVAL (op) == 1 || INTVAL (op) == 8));
}

int
aarch64_any_register_operand (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  return GET_CODE (op) == REG;
}

int
aarch64_sve_any_binary_operator (rtx op, machine_mode mode ATTRIBUTE_UNUSED)
{
  switch (GET_CODE (op))
    {
    case PLUS:
    case MINUS:
    case MULT:
    case DIV:
    case UDIV:
    case SMAX:
    case UMAX:
    case SMIN:
    case UMIN:
    case AND:
    case IOR:
    case XOR:
      break;
    default:
      return false;
    }
  return 
(mode == VOIDmode || GET_MODE (op) == mode);
}

enum constraint_num
lookup_constraint_1 (const char *str)
{
  switch (str[0])
    {
    case '<':
      return CONSTRAINT__l;
    case '>':
      return CONSTRAINT__g;
    case 'D':
      if (!strncmp (str + 1, "b", 1))
        return CONSTRAINT_Db;
      if (!strncmp (str + 1, "n", 1))
        return CONSTRAINT_Dn;
      if (!strncmp (str + 1, "h", 1))
        return CONSTRAINT_Dh;
      if (!strncmp (str + 1, "q", 1))
        return CONSTRAINT_Dq;
      if (!strncmp (str + 1, "l", 1))
        return CONSTRAINT_Dl;
      if (!strncmp (str + 1, "r", 1))
        return CONSTRAINT_Dr;
      if (!strncmp (str + 1, "z", 1))
        return CONSTRAINT_Dz;
      if (!strncmp (str + 1, "m", 1))
        return CONSTRAINT_Dm;
      if (!strncmp (str + 1, "d", 1))
        return CONSTRAINT_Dd;
      if (!strncmp (str + 1, "s", 1))
        return CONSTRAINT_Ds;
      if (!strncmp (str + 1, "p", 1))
        return CONSTRAINT_Dp;
      if (!strncmp (str + 1, "o", 1))
        return CONSTRAINT_Do;
      break;
    case 'E':
      return CONSTRAINT_E;
    case 'F':
      return CONSTRAINT_F;
    case 'I':
      return CONSTRAINT_I;
    case 'J':
      return CONSTRAINT_J;
    case 'K':
      return CONSTRAINT_K;
    case 'L':
      return CONSTRAINT_L;
    case 'M':
      return CONSTRAINT_M;
    case 'N':
      return CONSTRAINT_N;
    case 'Q':
      return CONSTRAINT_Q;
    case 'S':
      return CONSTRAINT_S;
    case 'U':
      if (!strncmp (str + 1, "pa", 2))
        return CONSTRAINT_Upa;
      if (!strncmp (str + 1, "pl", 2))
        return CONSTRAINT_Upl;
      if (!strncmp (str + 1, "aa", 2))
        return CONSTRAINT_Uaa;
      if (!strncmp (str + 1, "av", 2))
        return CONSTRAINT_Uav;
      if (!strncmp (str + 1, "at", 2))
        return CONSTRAINT_Uat;
      if (!strncmp (str + 1, "ti", 2))
        return CONSTRAINT_Uti;
      if (!strncmp (str + 1, "sO", 2))
        return CONSTRAINT_UsO;
      if (!strncmp (str + 1, "sP", 2))
        return CONSTRAINT_UsP;
      if (!strncmp (str + 1, "sh", 2))
        return CONSTRAINT_Ush;
      if (!strncmp (str + 1, "sa", 2))
        return CONSTRAINT_Usa;
      if (!strncmp (str + 1, "ss", 2))
        return CONSTRAINT_Uss;
      if (!strncmp (str + 1, "sn", 2))
        return CONSTRAINT_Usn;
      if (!strncmp (str + 1, "sd", 2))
        return CONSTRAINT_Usd;
      if (!strncmp (str + 1, "sf", 2))
        return CONSTRAINT_Usf;
      if (!strncmp (str + 1, "sg", 2))
        return CONSTRAINT_Usg;
      if (!strncmp (str + 1, "sj", 2))
        return CONSTRAINT_Usj;
      if (!strncmp (str + 1, "sM", 2))
        return CONSTRAINT_UsM;
      if (!strncmp (str + 1, "lc", 2))
        return CONSTRAINT_Ulc;
      if (!strncmp (str + 1, "sv", 2))
        return CONSTRAINT_Usv;
      if (!strncmp (str + 1, "si", 2))
        return CONSTRAINT_Usi;
      if (!strncmp (str + 1, "i1", 2))
        return CONSTRAINT_Ui1;
      if (!strncmp (str + 1, "i2", 2))
        return CONSTRAINT_Ui2;
      if (!strncmp (str + 1, "i3", 2))
        return CONSTRAINT_Ui3;
      if (!strncmp (str + 1, "i7", 2))
        return CONSTRAINT_Ui7;
      if (!strncmp (str + 1, "p3", 2))
        return CONSTRAINT_Up3;
      if (!strncmp (str + 1, "st", 2))
        return CONSTRAINT_Ust;
      if (!strncmp (str + 1, "mp", 2))
        return CONSTRAINT_Ump;
      if (!strncmp (str + 1, "mn", 2))
        return CONSTRAINT_Umn;
      if (!strncmp (str + 1, "tr", 2))
        return CONSTRAINT_Utr;
      if (!strncmp (str + 1, "tv", 2))
        return CONSTRAINT_Utv;
      if (!strncmp (str + 1, "tq", 2))
        return CONSTRAINT_Utq;
      if (!strncmp (str + 1, "ty", 2))
        return CONSTRAINT_Uty;
      if (!strncmp (str + 1, "tx", 2))
        return CONSTRAINT_Utx;
      if (!strncmp (str + 1, "fc", 2))
        return CONSTRAINT_Ufc;
      if (!strncmp (str + 1, "vi", 2))
        return CONSTRAINT_Uvi;
      if (!strncmp (str + 1, "cs", 2))
        return CONSTRAINT_Ucs;
      break;
    case 'V':
      return CONSTRAINT_V;
    case 'X':
      return CONSTRAINT_X;
    case 'Y':
      return CONSTRAINT_Y;
    case 'Z':
      return CONSTRAINT_Z;
    case 'i':
      return CONSTRAINT_i;
    case 'k':
      return CONSTRAINT_k;
    case 'm':
      return CONSTRAINT_m;
    case 'n':
      return CONSTRAINT_n;
    case 'o':
      return CONSTRAINT_o;
    case 'p':
      return CONSTRAINT_p;
    case 'r':
      return CONSTRAINT_r;
    case 's':
      return CONSTRAINT_s;
    case 'v':
      if (!strncmp (str + 1, "sc", 2))
        return CONSTRAINT_vsc;
      if (!strncmp (str + 1, "sd", 2))
        return CONSTRAINT_vsd;
      if (!strncmp (str + 1, "si", 2))
        return CONSTRAINT_vsi;
      if (!strncmp (str + 1, "sn", 2))
        return CONSTRAINT_vsn;
      if (!strncmp (str + 1, "sl", 2))
        return CONSTRAINT_vsl;
      if (!strncmp (str + 1, "sm", 2))
        return CONSTRAINT_vsm;
      if (!strncmp (str + 1, "sA", 2))
        return CONSTRAINT_vsA;
      if (!strncmp (str + 1, "sM", 2))
        return CONSTRAINT_vsM;
      if (!strncmp (str + 1, "sN", 2))
        return CONSTRAINT_vsN;
      if (!strncmp (str + 1, "sa", 2))
        return CONSTRAINT_vsa;
      break;
    case 'w':
      return CONSTRAINT_w;
    case 'x':
      return CONSTRAINT_x;
    default: break;
    }
  return CONSTRAINT__UNKNOWN;
}

const unsigned char lookup_constraint_array[] = {
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  MIN ((int) CONSTRAINT__l, (int) UCHAR_MAX),
  CONSTRAINT__UNKNOWN,
  MIN ((int) CONSTRAINT__g, (int) UCHAR_MAX),
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  UCHAR_MAX,
  MIN ((int) CONSTRAINT_E, (int) UCHAR_MAX),
  MIN ((int) CONSTRAINT_F, (int) UCHAR_MAX),
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  MIN ((int) CONSTRAINT_I, (int) UCHAR_MAX),
  MIN ((int) CONSTRAINT_J, (int) UCHAR_MAX),
  MIN ((int) CONSTRAINT_K, (int) UCHAR_MAX),
  MIN ((int) CONSTRAINT_L, (int) UCHAR_MAX),
  MIN ((int) CONSTRAINT_M, (int) UCHAR_MAX),
  MIN ((int) CONSTRAINT_N, (int) UCHAR_MAX),
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  MIN ((int) CONSTRAINT_Q, (int) UCHAR_MAX),
  CONSTRAINT__UNKNOWN,
  MIN ((int) CONSTRAINT_S, (int) UCHAR_MAX),
  CONSTRAINT__UNKNOWN,
  UCHAR_MAX,
  MIN ((int) CONSTRAINT_V, (int) UCHAR_MAX),
  CONSTRAINT__UNKNOWN,
  MIN ((int) CONSTRAINT_X, (int) UCHAR_MAX),
  MIN ((int) CONSTRAINT_Y, (int) UCHAR_MAX),
  MIN ((int) CONSTRAINT_Z, (int) UCHAR_MAX),
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  MIN ((int) CONSTRAINT_i, (int) UCHAR_MAX),
  CONSTRAINT__UNKNOWN,
  MIN ((int) CONSTRAINT_k, (int) UCHAR_MAX),
  CONSTRAINT__UNKNOWN,
  MIN ((int) CONSTRAINT_m, (int) UCHAR_MAX),
  MIN ((int) CONSTRAINT_n, (int) UCHAR_MAX),
  MIN ((int) CONSTRAINT_o, (int) UCHAR_MAX),
  MIN ((int) CONSTRAINT_p, (int) UCHAR_MAX),
  CONSTRAINT__UNKNOWN,
  MIN ((int) CONSTRAINT_r, (int) UCHAR_MAX),
  MIN ((int) CONSTRAINT_s, (int) UCHAR_MAX),
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  UCHAR_MAX,
  MIN ((int) CONSTRAINT_w, (int) UCHAR_MAX),
  MIN ((int) CONSTRAINT_x, (int) UCHAR_MAX),
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN,
  CONSTRAINT__UNKNOWN
};

enum reg_class
reg_class_for_constraint_1 (enum constraint_num c)
{
  switch (c)
    {
    case CONSTRAINT_r: return GENERAL_REGS;
    case CONSTRAINT_k: return STACK_REG;
    case CONSTRAINT_Ucs: return TAILCALL_ADDR_REGS;
    case CONSTRAINT_w: return FP_REGS;
    case CONSTRAINT_Upa: return PR_REGS;
    case CONSTRAINT_Upl: return PR_LO_REGS;
    case CONSTRAINT_x: return FP_LO_REGS;
    default: break;
    }
  return NO_REGS;
}

bool (*constraint_satisfied_p_array[]) (rtx) = {
  satisfies_constraint_I,
  satisfies_constraint_J,
  satisfies_constraint_K,
  satisfies_constraint_L,
  satisfies_constraint_M,
  satisfies_constraint_N,
  satisfies_constraint_m,
  satisfies_constraint_o,
  satisfies_constraint_Q,
  satisfies_constraint_Ust,
  satisfies_constraint_Ump,
  satisfies_constraint_Umn,
  satisfies_constraint_Utr,
  satisfies_constraint_Utv,
  satisfies_constraint_Utq,
  satisfies_constraint_Uty,
  satisfies_constraint_Utx,
  satisfies_constraint_p,
  satisfies_constraint_Dp,
  satisfies_constraint_Uaa,
  satisfies_constraint_Uav,
  satisfies_constraint_Uat,
  satisfies_constraint_Uti,
  satisfies_constraint_UsO,
  satisfies_constraint_UsP,
  satisfies_constraint_S,
  satisfies_constraint_Y,
  satisfies_constraint_Ush,
  satisfies_constraint_Usa,
  satisfies_constraint_Uss,
  satisfies_constraint_Usn,
  satisfies_constraint_Usd,
  satisfies_constraint_Usf,
  satisfies_constraint_Usg,
  satisfies_constraint_Usj,
  satisfies_constraint_Ulc,
  satisfies_constraint_Usv,
  satisfies_constraint_Usi,
  satisfies_constraint_Ui2,
  satisfies_constraint_Ui3,
  satisfies_constraint_Ui7,
  satisfies_constraint_Up3,
  satisfies_constraint_Ufc,
  satisfies_constraint_Uvi,
  satisfies_constraint_Do,
  satisfies_constraint_Db,
  satisfies_constraint_Dn,
  satisfies_constraint_Dh,
  satisfies_constraint_Dq,
  satisfies_constraint_Dl,
  satisfies_constraint_Dr,
  satisfies_constraint_Dz,
  satisfies_constraint_Dm,
  satisfies_constraint_Dd,
  satisfies_constraint_Ds,
  satisfies_constraint_vsa,
  satisfies_constraint_vsc,
  satisfies_constraint_vsd,
  satisfies_constraint_vsi,
  satisfies_constraint_vsn,
  satisfies_constraint_vsl,
  satisfies_constraint_vsm,
  satisfies_constraint_vsA,
  satisfies_constraint_vsM,
  satisfies_constraint_vsN,
  satisfies_constraint_V,
  satisfies_constraint__l,
  satisfies_constraint__g,
  satisfies_constraint_i,
  satisfies_constraint_s,
  satisfies_constraint_n,
  satisfies_constraint_E,
  satisfies_constraint_F,
  satisfies_constraint_X,
  satisfies_constraint_Z,
  satisfies_constraint_UsM,
  satisfies_constraint_Ui1
};

bool
insn_const_int_ok_for_constraint (HOST_WIDE_INT ival, enum constraint_num c)
{
  switch (c)
    {
    case CONSTRAINT_I:
      return 
#line 42 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/constraints.md"
(aarch64_uimm12_shift (ival));

    case CONSTRAINT_J:
      return 
#line 64 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/constraints.md"
(aarch64_uimm12_shift (-ival));

    case CONSTRAINT_K:
      return 
#line 73 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/constraints.md"
(aarch64_bitmask_imm (ival, SImode));

    case CONSTRAINT_L:
      return 
#line 78 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/constraints.md"
(aarch64_bitmask_imm (ival, DImode));

    case CONSTRAINT_M:
      return 
#line 83 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/constraints.md"
(aarch64_move_imm (ival, SImode));

    case CONSTRAINT_N:
      return 
#line 88 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/constraints.md"
(aarch64_move_imm (ival, DImode));

    default: break;
    }
  return false;
}

