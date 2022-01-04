/* Generated automatically by the program `genoutput'
   from the machine description file `md'.  */

#define IN_TARGET_CODE 1
#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "backend.h"
#include "predict.h"
#include "tree.h"
#include "rtl.h"
#include "flags.h"
#include "alias.h"
#include "varasm.h"
#include "stor-layout.h"
#include "calls.h"
#include "insn-config.h"
#include "expmed.h"
#include "dojump.h"
#include "explow.h"
#include "memmodel.h"
#include "emit-rtl.h"
#include "stmt.h"
#include "expr.h"
#include "insn-codes.h"
#include "tm_p.h"
#include "regs.h"
#include "conditions.h"
#include "insn-attr.h"

#include "recog.h"

#include "diagnostic-core.h"
#include "output.h"
#include "target.h"
#include "tm-constrs.h"

static const char * const output_3[] = {
  "ccmp\t%w2, %w3, %k5, %m4",
  "ccmp\t%w2, %3, %k5, %m4",
  "ccmn\t%w2, #%n3, %k5, %m4",
};

static const char * const output_4[] = {
  "ccmp\t%x2, %x3, %k5, %m4",
  "ccmp\t%x2, %3, %k5, %m4",
  "ccmn\t%x2, #%n3, %k5, %m4",
};

static const char *
output_9 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 532 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    if (get_attr_length (insn) == 8)
      return aarch64_gen_far_branch (operands, 2, "Lbcond", "b%M0\t");
    else
      return  "b%m0\t%l2";
  }
}

static const char *
output_18 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 657 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"

  return aarch64_output_casesi (operands);
  
}

static const char *
output_20 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 676 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    const char * pftype[2][4] =
    {
      {"prfm\tPLDL1STRM, %0",
       "prfm\tPLDL3KEEP, %0",
       "prfm\tPLDL2KEEP, %0",
       "prfm\tPLDL1KEEP, %0"},
      {"prfm\tPSTL1STRM, %0",
       "prfm\tPSTL3KEEP, %0",
       "prfm\tPSTL2KEEP, %0",
       "prfm\tPSTL1KEEP, %0"},
    };

    int locality = INTVAL (operands[2]);

    gcc_assert (IN_RANGE (locality, 0, 3));

    /* PRFM accepts the same addresses as a 64-bit LDR so wrap
       the address into a DImode MEM so that aarch64_print_operand knows
       how to print it.  */
    operands[0] = gen_rtx_MEM (DImode, operands[0]);
    return pftype[INTVAL(operands[1])][locality];
  }
}

static const char *
output_22 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 738 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    if (aarch64_return_address_signing_enabled ()
	&& TARGET_ARMV8_3
	&& !crtl->calls_eh_return)
      return "retaa";

    return "ret";
  }
}

static const char *
output_24 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 768 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    if (get_attr_length (insn) == 8)
      return aarch64_gen_far_branch (operands, 1, "Lcb", "cbnz\t%w0, ");
    else
      return "cbz\t%w0, %l1";
  }
}

static const char *
output_25 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 768 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    if (get_attr_length (insn) == 8)
      return aarch64_gen_far_branch (operands, 1, "Lcb", "cbz\t%w0, ");
    else
      return "cbnz\t%w0, %l1";
  }
}

static const char *
output_26 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 768 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    if (get_attr_length (insn) == 8)
      return aarch64_gen_far_branch (operands, 1, "Lcb", "cbnz\t%x0, ");
    else
      return "cbz\t%x0, %l1";
  }
}

static const char *
output_27 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 768 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    if (get_attr_length (insn) == 8)
      return aarch64_gen_far_branch (operands, 1, "Lcb", "cbz\t%x0, ");
    else
      return "cbnz\t%x0, %l1";
  }
}

static const char *
output_28 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 798 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    if (get_attr_length (insn) == 8)
      {
	if (get_attr_far_branch (insn) == 1)
	  return aarch64_gen_far_branch (operands, 2, "Ltb",
					 "tbnz\t%w0, %1, ");
	else
	  {
	    operands[1] = GEN_INT (HOST_WIDE_INT_1U << UINTVAL (operands[1]));
	    return "tst\t%w0, %1\n\tbeq\t%l2";
	  }
      }
    else
      return "tbz\t%w0, %1, %l2";
  }
}

static const char *
output_29 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 798 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    if (get_attr_length (insn) == 8)
      {
	if (get_attr_far_branch (insn) == 1)
	  return aarch64_gen_far_branch (operands, 2, "Ltb",
					 "tbz\t%w0, %1, ");
	else
	  {
	    operands[1] = GEN_INT (HOST_WIDE_INT_1U << UINTVAL (operands[1]));
	    return "tst\t%w0, %1\n\tbne\t%l2";
	  }
      }
    else
      return "tbnz\t%w0, %1, %l2";
  }
}

static const char *
output_30 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 798 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    if (get_attr_length (insn) == 8)
      {
	if (get_attr_far_branch (insn) == 1)
	  return aarch64_gen_far_branch (operands, 2, "Ltb",
					 "tbnz\t%x0, %1, ");
	else
	  {
	    operands[1] = GEN_INT (HOST_WIDE_INT_1U << UINTVAL (operands[1]));
	    return "tst\t%x0, %1\n\tbeq\t%l2";
	  }
      }
    else
      return "tbz\t%x0, %1, %l2";
  }
}

static const char *
output_31 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 798 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    if (get_attr_length (insn) == 8)
      {
	if (get_attr_far_branch (insn) == 1)
	  return aarch64_gen_far_branch (operands, 2, "Ltb",
					 "tbz\t%x0, %1, ");
	else
	  {
	    operands[1] = GEN_INT (HOST_WIDE_INT_1U << UINTVAL (operands[1]));
	    return "tst\t%x0, %1\n\tbne\t%l2";
	  }
      }
    else
      return "tbnz\t%x0, %1, %l2";
  }
}

static const char *
output_32 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 834 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    if (get_attr_length (insn) == 8)
      {
	if (get_attr_far_branch (insn) == 1)
	  return aarch64_gen_far_branch (operands, 1, "Ltb",
					 "tbz\t%w0, #7, ");
	else
	  {
	    char buf[64];
	    uint64_t val = ((uint64_t) 1)
		<< (GET_MODE_SIZE (QImode) * BITS_PER_UNIT - 1);
	    sprintf (buf, "tst\t%%w0, %" PRId64, val);
	    output_asm_insn (buf, operands);
	    return "bne\t%l1";
	  }
      }
    else
      return "tbnz\t%w0, #7, %l1";
  }
}

static const char *
output_33 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 834 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    if (get_attr_length (insn) == 8)
      {
	if (get_attr_far_branch (insn) == 1)
	  return aarch64_gen_far_branch (operands, 1, "Ltb",
					 "tbnz\t%w0, #7, ");
	else
	  {
	    char buf[64];
	    uint64_t val = ((uint64_t) 1)
		<< (GET_MODE_SIZE (QImode) * BITS_PER_UNIT - 1);
	    sprintf (buf, "tst\t%%w0, %" PRId64, val);
	    output_asm_insn (buf, operands);
	    return "beq\t%l1";
	  }
      }
    else
      return "tbz\t%w0, #7, %l1";
  }
}

static const char *
output_34 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 834 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    if (get_attr_length (insn) == 8)
      {
	if (get_attr_far_branch (insn) == 1)
	  return aarch64_gen_far_branch (operands, 1, "Ltb",
					 "tbz\t%w0, #15, ");
	else
	  {
	    char buf[64];
	    uint64_t val = ((uint64_t) 1)
		<< (GET_MODE_SIZE (HImode) * BITS_PER_UNIT - 1);
	    sprintf (buf, "tst\t%%w0, %" PRId64, val);
	    output_asm_insn (buf, operands);
	    return "bne\t%l1";
	  }
      }
    else
      return "tbnz\t%w0, #15, %l1";
  }
}

static const char *
output_35 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 834 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    if (get_attr_length (insn) == 8)
      {
	if (get_attr_far_branch (insn) == 1)
	  return aarch64_gen_far_branch (operands, 1, "Ltb",
					 "tbnz\t%w0, #15, ");
	else
	  {
	    char buf[64];
	    uint64_t val = ((uint64_t) 1)
		<< (GET_MODE_SIZE (HImode) * BITS_PER_UNIT - 1);
	    sprintf (buf, "tst\t%%w0, %" PRId64, val);
	    output_asm_insn (buf, operands);
	    return "beq\t%l1";
	  }
      }
    else
      return "tbz\t%w0, #15, %l1";
  }
}

static const char *
output_36 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 834 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    if (get_attr_length (insn) == 8)
      {
	if (get_attr_far_branch (insn) == 1)
	  return aarch64_gen_far_branch (operands, 1, "Ltb",
					 "tbz\t%w0, #31, ");
	else
	  {
	    char buf[64];
	    uint64_t val = ((uint64_t) 1)
		<< (GET_MODE_SIZE (SImode) * BITS_PER_UNIT - 1);
	    sprintf (buf, "tst\t%%w0, %" PRId64, val);
	    output_asm_insn (buf, operands);
	    return "bne\t%l1";
	  }
      }
    else
      return "tbnz\t%w0, #31, %l1";
  }
}

static const char *
output_37 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 834 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    if (get_attr_length (insn) == 8)
      {
	if (get_attr_far_branch (insn) == 1)
	  return aarch64_gen_far_branch (operands, 1, "Ltb",
					 "tbnz\t%w0, #31, ");
	else
	  {
	    char buf[64];
	    uint64_t val = ((uint64_t) 1)
		<< (GET_MODE_SIZE (SImode) * BITS_PER_UNIT - 1);
	    sprintf (buf, "tst\t%%w0, %" PRId64, val);
	    output_asm_insn (buf, operands);
	    return "beq\t%l1";
	  }
      }
    else
      return "tbz\t%w0, #31, %l1";
  }
}

static const char *
output_38 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 834 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    if (get_attr_length (insn) == 8)
      {
	if (get_attr_far_branch (insn) == 1)
	  return aarch64_gen_far_branch (operands, 1, "Ltb",
					 "tbz\t%x0, #63, ");
	else
	  {
	    char buf[64];
	    uint64_t val = ((uint64_t) 1)
		<< (GET_MODE_SIZE (DImode) * BITS_PER_UNIT - 1);
	    sprintf (buf, "tst\t%%x0, %" PRId64, val);
	    output_asm_insn (buf, operands);
	    return "bne\t%l1";
	  }
      }
    else
      return "tbnz\t%x0, #63, %l1";
  }
}

static const char *
output_39 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 834 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    if (get_attr_length (insn) == 8)
      {
	if (get_attr_far_branch (insn) == 1)
	  return aarch64_gen_far_branch (operands, 1, "Ltb",
					 "tbnz\t%x0, #63, ");
	else
	  {
	    char buf[64];
	    uint64_t val = ((uint64_t) 1)
		<< (GET_MODE_SIZE (DImode) * BITS_PER_UNIT - 1);
	    sprintf (buf, "tst\t%%x0, %" PRId64, val);
	    output_asm_insn (buf, operands);
	    return "beq\t%l1";
	  }
      }
    else
      return "tbz\t%x0, #63, %l1";
  }
}

static const char * const output_40[] = {
  "blr\t%0",
  "bl\t%c0",
};

static const char * const output_41[] = {
  "blr\t%1",
  "bl\t%c1",
};

static const char * const output_42[] = {
  "br\t%0",
  "b\t%c0",
};

static const char * const output_43[] = {
  "br\t%1",
  "b\t%c1",
};

static const char *
output_44 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 1021 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
   switch (which_alternative)
     {
     case 0:
       return "mov\t%w0, %w1";
     case 1:
       return "mov\t%w0, %1";
     case 2:
       return aarch64_output_scalar_simd_mov_immediate (operands[1],
							QImode);
     case 3:
       return aarch64_output_sve_cnt_immediate ("cnt", "%x0", operands[1]);
     case 4:
       return "ldrb\t%w0, %1";
     case 5:
       return "ldr\t%b0, %1";
     case 6:
       return "strb\t%w1, %0";
     case 7:
       return "str\t%b1, %0";
     case 8:
       return "umov\t%w0, %1.b[0]";
     case 9:
       return "dup\t%0.8b, %w1";
     case 10:
       return "dup\t%b0, %1.b[0]";
     default:
       gcc_unreachable ();
     }
}
}

static const char *
output_45 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 1021 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
   switch (which_alternative)
     {
     case 0:
       return "mov\t%w0, %w1";
     case 1:
       return "mov\t%w0, %1";
     case 2:
       return aarch64_output_scalar_simd_mov_immediate (operands[1],
							HImode);
     case 3:
       return aarch64_output_sve_cnt_immediate ("cnt", "%x0", operands[1]);
     case 4:
       return "ldrh\t%w0, %1";
     case 5:
       return "ldr\t%h0, %1";
     case 6:
       return "strh\t%w1, %0";
     case 7:
       return "str\t%h1, %0";
     case 8:
       return "umov\t%w0, %1.h[0]";
     case 9:
       return "dup\t%0.4h, %w1";
     case 10:
       return "dup\t%h0, %1.h[0]";
     default:
       gcc_unreachable ();
     }
}
}

static const char *
output_46 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
  switch (which_alternative)
    {
    case 0: return "mov\t%w0, %w1";
    case 1: return "mov\t%w0, %w1";
    case 2: return "mov\t%w0, %w1";
    case 3: return "mov\t%w0, %1";
    case 4: return "#";
    case 5:
       return aarch64_output_sve_cnt_immediate ("cnt", "%x0", operands[1]);
    case 6: return "ldr\t%w0, %1";
    case 7: return "ldr\t%s0, %1";
    case 8: return "str\t%w1, %0";
    case 9: return "str\t%s1, %0";
    case 10: return "adr\t%x0, %c1";
    case 11: return "adrp\t%x0, %A1";
    case 12: return "fmov\t%s0, %w1";
    case 13: return "fmov\t%w0, %s1";
    case 14: return "fmov\t%s0, %s1";
    case 15:
       return aarch64_output_scalar_simd_mov_immediate (operands[1], SImode);
      default: gcc_unreachable ();
    }
}

static const char *
output_47 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
  switch (which_alternative)
    {
    case 0: return "mov\t%x0, %x1";
    case 1: return "mov\t%0, %x1";
    case 2: return "mov\t%x0, %1";
    case 3: return "mov\t%x0, %1";
    case 4: return "mov\t%w0, %1";
    case 5: return "#";
    case 6:
       return aarch64_output_sve_cnt_immediate ("cnt", "%x0", operands[1]);
    case 7: return "ldr\t%x0, %1";
    case 8: return "ldr\t%d0, %1";
    case 9: return "str\t%x1, %0";
    case 10: return "str\t%d1, %0";
    case 11: return "adr\t%x0, %c1";
    case 12: return "adrp\t%x0, %A1";
    case 13: return "fmov\t%d0, %x1";
    case 14: return "fmov\t%x0, %d1";
    case 15: return "fmov\t%d0, %d1";
    case 16:
       return aarch64_output_scalar_simd_mov_immediate (operands[1], DImode);
      default: gcc_unreachable ();
    }
}

static const char * const output_50[] = {
  "#",
  "#",
  "#",
  "mov\t%0.16b, %1.16b",
  "ldp\t%0, %H0, %1",
  "stp\t%1, %H1, %0",
  "stp\txzr, xzr, %0",
  "ldr\t%q0, %1",
  "str\t%q1, %0",
};

static const char *
output_51 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
  switch (which_alternative)
    {
    case 0: return "movi\t%0.4h, #0";
    case 1: return "fmov\t%h0, %w1";
    case 2: return "dup\t%w0.4h, %w1";
    case 3: return "umov\t%w0, %1.h[0]";
    case 4: return "mov\t%0.h[0], %1.h[0]";
    case 5: return "fmov\t%h0, %1";
    case 6:
       return aarch64_output_scalar_simd_mov_immediate (operands[1], HImode);
    case 7: return "ldr\t%h0, %1";
    case 8: return "str\t%h1, %0";
    case 9: return "ldrh\t%w0, %1";
    case 10: return "strh\t%w1, %0";
    case 11: return "mov\t%w0, %w1";
      default: gcc_unreachable ();
    }
}

static const char *
output_52 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
  switch (which_alternative)
    {
    case 0: return "movi\t%0.2s, #0";
    case 1: return "fmov\t%s0, %w1";
    case 2: return "fmov\t%w0, %s1";
    case 3: return "fmov\t%s0, %s1";
    case 4: return "fmov\t%s0, %1";
    case 5:
       return aarch64_output_scalar_simd_mov_immediate (operands[1], SImode);
    case 6: return "ldr\t%s0, %1";
    case 7: return "str\t%s1, %0";
    case 8: return "ldr\t%w0, %1";
    case 9: return "str\t%w1, %0";
    case 10: return "mov\t%w0, %w1";
    case 11: return "mov\t%w0, %1";
      default: gcc_unreachable ();
    }
}

static const char *
output_53 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
  switch (which_alternative)
    {
    case 0: return "movi\t%d0, #0";
    case 1: return "fmov\t%d0, %x1";
    case 2: return "fmov\t%x0, %d1";
    case 3: return "fmov\t%d0, %d1";
    case 4: return "fmov\t%d0, %1";
    case 5:
       return aarch64_output_scalar_simd_mov_immediate (operands[1], DImode);
    case 6: return "ldr\t%d0, %1";
    case 7: return "str\t%d1, %0";
    case 8: return "ldr\t%x0, %1";
    case 9: return "str\t%x1, %0";
    case 10: return "mov\t%x0, %x1";
    case 11: return "mov\t%x0, %1";
      default: gcc_unreachable ();
    }
}

static const char * const output_54[] = {
  "mov\t%0.16b, %1.16b",
  "#",
  "#",
  "#",
  "movi\t%0.2d, #0",
  "fmov\t%s0, wzr",
  "ldr\t%q0, %1",
  "str\t%q1, %0",
  "ldp\t%0, %H0, %1",
  "stp\t%1, %H1, %0",
  "stp\txzr, xzr, %0",
};

static const char * const output_55[] = {
  "ldp\t%w0, %w2, %1",
  "ldp\t%s0, %s2, %1",
};

static const char * const output_56[] = {
  "ldp\t%w0, %w2, %1",
  "ldp\t%s0, %s2, %1",
};

static const char * const output_57[] = {
  "ldp\t%w0, %w2, %1",
  "ldp\t%s0, %s2, %1",
};

static const char * const output_58[] = {
  "ldp\t%w0, %w2, %1",
  "ldp\t%s0, %s2, %1",
};

static const char * const output_59[] = {
  "ldp\t%x0, %x2, %1",
  "ldp\t%d0, %d2, %1",
};

static const char * const output_60[] = {
  "ldp\t%x0, %x2, %1",
  "ldp\t%d0, %d2, %1",
};

static const char * const output_61[] = {
  "ldp\t%x0, %x2, %1",
  "ldp\t%d0, %d2, %1",
};

static const char * const output_62[] = {
  "ldp\t%x0, %x2, %1",
  "ldp\t%d0, %d2, %1",
};

static const char * const output_64[] = {
  "stp\t%w1, %w3, %0",
  "stp\t%s1, %s3, %0",
};

static const char * const output_65[] = {
  "stp\t%w1, %w3, %0",
  "stp\t%s1, %s3, %0",
};

static const char * const output_66[] = {
  "stp\t%w1, %w3, %0",
  "stp\t%s1, %s3, %0",
};

static const char * const output_67[] = {
  "stp\t%w1, %w3, %0",
  "stp\t%s1, %s3, %0",
};

static const char * const output_68[] = {
  "stp\t%x1, %x3, %0",
  "stp\t%d1, %d3, %0",
};

static const char * const output_69[] = {
  "stp\t%x1, %x3, %0",
  "stp\t%d1, %d3, %0",
};

static const char * const output_70[] = {
  "stp\t%x1, %x3, %0",
  "stp\t%d1, %d3, %0",
};

static const char * const output_71[] = {
  "stp\t%x1, %x3, %0",
  "stp\t%d1, %d3, %0",
};

static const char * const output_97[] = {
  "sxtw\t%0, %w1",
  "ldrsw\t%0, %1",
};

static const char * const output_99[] = {
  "uxtw\t%0, %w1",
  "ldr\t%w0, %1",
  "fmov\t%s0, %w1",
  "ldr\t%s0, %1",
  "fmov\t%w0, %s1",
  "fmov\t%s0, %s1",
};

static const char * const output_100[] = {
  "ldp\t%w0, %w2, %1",
  "ldp\t%s0, %s2, %1",
};

static const char * const output_101[] = {
  "sxtb\t%w0, %w1",
  "ldrsb\t%w0, %1",
};

static const char * const output_102[] = {
  "sxtb\t%x0, %w1",
  "ldrsb\t%x0, %1",
};

static const char * const output_103[] = {
  "sxth\t%w0, %w1",
  "ldrsh\t%w0, %1",
};

static const char * const output_104[] = {
  "sxth\t%x0, %w1",
  "ldrsh\t%x0, %1",
};

static const char * const output_105[] = {
  "and\t%w0, %w1, 255",
  "ldrb\t%w0, %1",
  "ldr\t%b0, %1",
};

static const char * const output_106[] = {
  "and\t%x0, %x1, 255",
  "ldrb\t%w0, %1",
  "ldr\t%b0, %1",
};

static const char * const output_107[] = {
  "and\t%w0, %w1, 65535",
  "ldrh\t%w0, %1",
  "ldr\t%h0, %1",
};

static const char * const output_108[] = {
  "and\t%x0, %x1, 65535",
  "ldrh\t%w0, %1",
  "ldr\t%h0, %1",
};

static const char * const output_109[] = {
  "sxtb\t%w0, %w1",
  "ldrsb\t%w0, %1",
};

static const char * const output_110[] = {
  "and\t%w0, %w1, 255",
  "ldrb\t%w0, %1",
};

static const char *
output_111 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
  switch (which_alternative)
    {
    case 0: return "add\t%w0, %w1, %2";
    case 1: return "add\t%w0, %w1, %w2";
    case 2: return "add\t%0.2s, %1.2s, %2.2s";
    case 3: return "sub\t%w0, %w1, #%n2";
    case 4: return "#";
    case 5:
       return aarch64_output_sve_addvl_addpl (operands[0], operands[1], operands[2]);
      default: gcc_unreachable ();
    }
}

static const char *
output_112 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
  switch (which_alternative)
    {
    case 0: return "add\t%x0, %x1, %2";
    case 1: return "add\t%x0, %x1, %x2";
    case 2: return "add\t%d0, %d1, %d2";
    case 3: return "sub\t%x0, %x1, #%n2";
    case 4: return "#";
    case 5:
       return aarch64_output_sve_addvl_addpl (operands[0], operands[1], operands[2]);
      default: gcc_unreachable ();
    }
}

static const char * const output_113[] = {
  "add\t%w0, %w1, %2",
  "add\t%w0, %w1, %w2",
  "sub\t%w0, %w1, #%n2",
  "#",
};

static const char *
output_114 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
  switch (which_alternative)
    {
    case 0: return "add\t%w0, %w1, %2";
    case 1: return "add\t%w0, %w1, %w2";
    case 2: return "sub\t%w0, %w1, #%n2";
    case 3: return "#";
    case 4:
       return aarch64_output_sve_addvl_addpl (operands[0], operands[1], operands[2]);
    case 5: return "#";
      default: gcc_unreachable ();
    }
}

static const char *
output_115 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
  switch (which_alternative)
    {
    case 0: return "add\t%x0, %x1, %2";
    case 1: return "add\t%x0, %x1, %x2";
    case 2: return "sub\t%x0, %x1, #%n2";
    case 3: return "#";
    case 4:
       return aarch64_output_sve_addvl_addpl (operands[0], operands[1], operands[2]);
    case 5: return "#";
      default: gcc_unreachable ();
    }
}

static const char * const output_116[] = {
  "adds\t%w0, %w1, %w2",
  "adds\t%w0, %w1, %2",
  "subs\t%w0, %w1, #%n2",
};

static const char * const output_117[] = {
  "adds\t%x0, %x1, %x2",
  "adds\t%x0, %x1, %2",
  "subs\t%x0, %x1, #%n2",
};

static const char * const output_118[] = {
  "adds\t%w0, %w1, %w2",
  "adds\t%w0, %w1, %2",
  "subs\t%w0, %w1, #%n2",
};

static const char * const output_119[] = {
  "cmn\t%w0, %w1",
  "cmn\t%w0, %1",
  "cmp\t%w0, #%n1",
};

static const char * const output_120[] = {
  "cmn\t%x0, %x1",
  "cmn\t%x0, %1",
  "cmp\t%x0, #%n1",
};

static const char * const output_121[] = {
  "adds\t%w0, %w1, %w2",
  "adds\t%w0, %w1, %2",
  "subs\t%w0, %w1, #%n2",
};

static const char * const output_122[] = {
  "adds\t%x0, %x1, %x2",
  "adds\t%x0, %x1, %2",
  "subs\t%x0, %x1, #%n2",
};

static const char * const output_123[] = {
  "cmn\t%w0, %w1",
  "cmp\t%w0, #%n1",
};

static const char * const output_124[] = {
  "cmn\t%x0, %x1",
  "cmp\t%x0, #%n1",
};

static const char * const output_127[] = {
  "adds\t%w0, %w1, %w2",
  "subs\t%w0, %w1, #%n2",
};

static const char * const output_128[] = {
  "adds\t%x0, %x1, %x2",
  "subs\t%x0, %x1, #%n2",
};

static const char * const output_203[] = {
  "cmn\t%w0, %w1",
  "cmn\t%w0, %1",
  "cmp\t%w0, #%n1",
};

static const char * const output_204[] = {
  "cmn\t%x0, %x1",
  "cmn\t%x0, %1",
  "cmp\t%x0, #%n1",
};

static const char *
output_289 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 2633 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"

  operands[3] = GEN_INT (aarch64_uxt_size (INTVAL(operands[2]),
					   INTVAL (operands[3])));
  return "add\t%w0, %w4, %w1, uxt%e3 %2";
}

static const char *
output_290 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 2633 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"

  operands[3] = GEN_INT (aarch64_uxt_size (INTVAL(operands[2]),
					   INTVAL (operands[3])));
  return "add\t%x0, %x4, %x1, uxt%e3 %2";
}

static const char *
output_291 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 2650 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"

  operands[3] = GEN_INT (aarch64_uxt_size (INTVAL (operands[2]),
					   INTVAL (operands[3])));
  return "add\t%w0, %w4, %w1, uxt%e3 %2";
}

static const char *
output_292 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 2665 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"

  operands[3] = GEN_INT (aarch64_uxt_size (exact_log2 (INTVAL (operands[2])),
					   INTVAL (operands[3])));
  return "add\t%w0, %w4, %w1, uxt%e3 %p2";
}

static const char *
output_293 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 2665 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"

  operands[3] = GEN_INT (aarch64_uxt_size (exact_log2 (INTVAL (operands[2])),
					   INTVAL (operands[3])));
  return "add\t%x0, %x4, %x1, uxt%e3 %p2";
}

static const char *
output_294 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 2682 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"

  operands[3] = GEN_INT (aarch64_uxt_size (exact_log2 (INTVAL (operands[2])),
					   INTVAL (operands[3])));
  return "add\t%w0, %w4, %w1, uxt%e3 %p2";
}

static const char * const output_297[] = {
  "sub\t%x0, %x1, %x2",
  "sub\t%d0, %d1, %d2",
};

static const char * const output_300[] = {
  "subs\t%w0, %w1, %2",
  "adds\t%w0, %w1, #%n2",
};

static const char * const output_301[] = {
  "subs\t%x0, %x1, %2",
  "adds\t%x0, %x1, #%n2",
};

static const char * const output_306[] = {
  "cmp\t%w0, %w1",
  "cmp\t%w0, %1",
  "cmp\t%w0, #%n1",
};

static const char * const output_307[] = {
  "cmp\t%x0, %x1",
  "cmp\t%x0, %1",
  "cmp\t%x0, #%n1",
};

static const char * const output_313[] = {
  "subs\t%w0, %w1, %2",
  "adds\t%w0, %w1, #%n2",
};

static const char * const output_314[] = {
  "subs\t%x0, %x1, %2",
  "adds\t%x0, %x1, #%n2",
};

static const char *
output_386 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3421 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"

  operands[3] = GEN_INT (aarch64_uxt_size (INTVAL (operands[2]),
					   INTVAL (operands[3])));
  return "sub\t%w0, %w4, %w1, uxt%e3 %2";
}

static const char *
output_387 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3421 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"

  operands[3] = GEN_INT (aarch64_uxt_size (INTVAL (operands[2]),
					   INTVAL (operands[3])));
  return "sub\t%x0, %x4, %x1, uxt%e3 %2";
}

static const char *
output_388 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3438 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"

  operands[3] = GEN_INT (aarch64_uxt_size (INTVAL (operands[2]),
					   INTVAL (operands[3])));
  return "sub\t%w0, %w4, %w1, uxt%e3 %2";
}

static const char *
output_389 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3453 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"

  operands[3] = GEN_INT (aarch64_uxt_size (exact_log2 (INTVAL (operands[2])),
					   INTVAL (operands[3])));
  return "sub\t%w0, %w4, %w1, uxt%e3 %p2";
}

static const char *
output_390 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3453 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"

  operands[3] = GEN_INT (aarch64_uxt_size (exact_log2 (INTVAL (operands[2])),
					   INTVAL (operands[3])));
  return "sub\t%x0, %x4, %x1, uxt%e3 %p2";
}

static const char *
output_391 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3470 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"

  operands[3] = GEN_INT (aarch64_uxt_size (exact_log2 (INTVAL (operands[2])),
					   INTVAL (operands[3])));
  return "sub\t%w0, %w4, %w1, uxt%e3 %p2";
}

static const char * const output_392[] = {
  "neg\t%w0, %w1",
  "neg\t%0.2s, %1.2s",
};

static const char * const output_393[] = {
  "neg\t%x0, %x1",
  "neg\t%d0, %d1",
};

static const char * const output_447[] = {
  "cmp\t%w0, %w1",
  "cmp\t%w0, %1",
  "cmn\t%w0, #%n1",
};

static const char * const output_448[] = {
  "cmp\t%x0, %x1",
  "cmp\t%x0, %1",
  "cmn\t%x0, #%n1",
};

static const char * const output_449[] = {
  "fcmp\t%s0, #0.0",
  "fcmp\t%s0, %s1",
};

static const char * const output_450[] = {
  "fcmp\t%d0, #0.0",
  "fcmp\t%d0, %d1",
};

static const char * const output_451[] = {
  "fcmpe\t%s0, #0.0",
  "fcmpe\t%s0, %s1",
};

static const char * const output_452[] = {
  "fcmpe\t%d0, #0.0",
  "fcmpe\t%d0, %d1",
};

static const char * const output_497[] = {
  "csel\t%w0, %w3, %w4, %m1",
  "csinv\t%w0, %w3, wzr, %m1",
  "csinv\t%w0, %w4, wzr, %M1",
  "csinc\t%w0, %w3, wzr, %m1",
  "csinc\t%w0, %w4, wzr, %M1",
  "mov\t%w0, -1",
  "mov\t%w0, 1",
};

static const char * const output_498[] = {
  "csel\t%w0, %w3, %w4, %m1",
  "csinv\t%w0, %w3, wzr, %m1",
  "csinv\t%w0, %w4, wzr, %M1",
  "csinc\t%w0, %w3, wzr, %m1",
  "csinc\t%w0, %w4, wzr, %M1",
  "mov\t%w0, -1",
  "mov\t%w0, 1",
};

static const char * const output_499[] = {
  "csel\t%w0, %w3, %w4, %m1",
  "csinv\t%w0, %w3, wzr, %m1",
  "csinv\t%w0, %w4, wzr, %M1",
  "csinc\t%w0, %w3, wzr, %m1",
  "csinc\t%w0, %w4, wzr, %M1",
  "mov\t%w0, -1",
  "mov\t%w0, 1",
};

static const char * const output_500[] = {
  "csel\t%x0, %x3, %x4, %m1",
  "csinv\t%x0, %x3, xzr, %m1",
  "csinv\t%x0, %x4, xzr, %M1",
  "csinc\t%x0, %x3, xzr, %m1",
  "csinc\t%x0, %x4, xzr, %M1",
  "mov\t%x0, -1",
  "mov\t%x0, 1",
};

static const char * const output_501[] = {
  "csel\t%w0, %w3, %w4, %m1",
  "csinv\t%w0, %w3, wzr, %m1",
  "csinv\t%w0, %w4, wzr, %M1",
  "csinc\t%w0, %w3, wzr, %m1",
  "csinc\t%w0, %w4, wzr, %M1",
  "mov\t%w0, -1",
  "mov\t%w0, 1",
};

static const char *
output_505 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4187 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    if (GET_MODE_BITSIZE (QImode) >= 64)
      return "crc32b\t%w0, %w1, %x2";
    else
      return "crc32b\t%w0, %w1, %w2";
  }
}

static const char *
output_506 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4187 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    if (GET_MODE_BITSIZE (HImode) >= 64)
      return "crc32h\t%w0, %w1, %x2";
    else
      return "crc32h\t%w0, %w1, %w2";
  }
}

static const char *
output_507 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4187 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    if (GET_MODE_BITSIZE (SImode) >= 64)
      return "crc32w\t%w0, %w1, %x2";
    else
      return "crc32w\t%w0, %w1, %w2";
  }
}

static const char *
output_508 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4187 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    if (GET_MODE_BITSIZE (DImode) >= 64)
      return "crc32x\t%w0, %w1, %x2";
    else
      return "crc32x\t%w0, %w1, %w2";
  }
}

static const char *
output_509 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4187 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    if (GET_MODE_BITSIZE (QImode) >= 64)
      return "crc32cb\t%w0, %w1, %x2";
    else
      return "crc32cb\t%w0, %w1, %w2";
  }
}

static const char *
output_510 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4187 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    if (GET_MODE_BITSIZE (HImode) >= 64)
      return "crc32ch\t%w0, %w1, %x2";
    else
      return "crc32ch\t%w0, %w1, %w2";
  }
}

static const char *
output_511 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4187 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    if (GET_MODE_BITSIZE (SImode) >= 64)
      return "crc32cw\t%w0, %w1, %x2";
    else
      return "crc32cw\t%w0, %w1, %w2";
  }
}

static const char *
output_512 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4187 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    if (GET_MODE_BITSIZE (DImode) >= 64)
      return "crc32cx\t%w0, %w1, %x2";
    else
      return "crc32cx\t%w0, %w1, %w2";
  }
}

static const char *
output_522 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4303 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    return aarch64_output_sve_cnt_immediate ("uqdec", "%w0", operands[2]);
  }
}

static const char *
output_523 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4303 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    return aarch64_output_sve_cnt_immediate ("uqdec", "%x0", operands[2]);
  }
}

static const char * const output_526[] = {
  "and\t%w0, %w1, %w2",
  "and\t%w0, %w1, %2",
  "and\t%0.8b, %1.8b, %2.8b",
};

static const char * const output_527[] = {
  "orr\t%w0, %w1, %w2",
  "orr\t%w0, %w1, %2",
  "orr\t%0.8b, %1.8b, %2.8b",
};

static const char * const output_528[] = {
  "eor\t%w0, %w1, %w2",
  "eor\t%w0, %w1, %2",
  "eor\t%0.8b, %1.8b, %2.8b",
};

static const char * const output_529[] = {
  "and\t%x0, %x1, %x2",
  "and\t%x0, %x1, %2",
  "and\t%0.8b, %1.8b, %2.8b",
};

static const char * const output_530[] = {
  "orr\t%x0, %x1, %x2",
  "orr\t%x0, %x1, %2",
  "orr\t%0.8b, %1.8b, %2.8b",
};

static const char * const output_531[] = {
  "eor\t%x0, %x1, %x2",
  "eor\t%x0, %x1, %2",
  "eor\t%0.8b, %1.8b, %2.8b",
};

static const char * const output_532[] = {
  "and\t%w0, %w1, %w2",
  "and\t%w0, %w1, %2",
};

static const char * const output_533[] = {
  "orr\t%w0, %w1, %w2",
  "orr\t%w0, %w1, %2",
};

static const char * const output_534[] = {
  "eor\t%w0, %w1, %w2",
  "eor\t%w0, %w1, %2",
};

static const char * const output_535[] = {
  "ands\t%w0, %w1, %w2",
  "ands\t%w0, %w1, %2",
};

static const char * const output_536[] = {
  "ands\t%x0, %x1, %x2",
  "ands\t%x0, %x1, %2",
};

static const char * const output_537[] = {
  "ands\t%w0, %w1, %w2",
  "ands\t%w0, %w1, %2",
};

static const char * const output_595[] = {
  "mvn\t%w0, %w1",
  "mvn\t%0.8b, %1.8b",
};

static const char * const output_596[] = {
  "mvn\t%x0, %x1",
  "mvn\t%0.8b, %1.8b",
};

static const char * const output_605[] = {
  "bic\t%w0, %w2, %w1",
  "bic\t%0.8b, %2.8b, %1.8b",
};

static const char * const output_606[] = {
  "orn\t%w0, %w2, %w1",
  "orn\t%0.8b, %2.8b, %1.8b",
};

static const char * const output_607[] = {
  "bic\t%x0, %x2, %x1",
  "bic\t%0.8b, %2.8b, %1.8b",
};

static const char * const output_608[] = {
  "orn\t%x0, %x2, %x1",
  "orn\t%0.8b, %2.8b, %1.8b",
};

static const char * const output_612[] = {
  "eon\t%w0, %w1, %w2",
  "#",
};

static const char * const output_613[] = {
  "eon\t%x0, %x1, %x2",
  "#",
};

static const char * const output_689[] = {
  "tst\t%w0, %w1",
  "tst\t%w0, %1",
};

static const char * const output_690[] = {
  "tst\t%x0, %x1",
  "tst\t%x0, %1",
};

static const char *
output_691 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4827 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    operands[1]
      = aarch64_mask_from_zextract_ops (operands[1], operands[2]);
    return "tst\t%w0, %1";
  }
}

static const char *
output_692 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4827 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    operands[1]
      = aarch64_mask_from_zextract_ops (operands[1], operands[2]);
    return "tst\t%x0, %1";
  }
}

static const char *
output_719 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5031 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  rtx xop[3];
  xop[0] = operands[0];
  xop[1] = operands[1];
  xop[2] = gen_lowpart (GET_MODE (operands[4]), operands[2]);
  output_asm_insn ("lsl\t%x0, %x1, %x2", xop);
  return "";
}
}

static const char *
output_720 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5031 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  rtx xop[3];
  xop[0] = operands[0];
  xop[1] = operands[1];
  xop[2] = gen_lowpart (GET_MODE (operands[4]), operands[2]);
  output_asm_insn ("asr\t%x0, %x1, %x2", xop);
  return "";
}
}

static const char *
output_721 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5031 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  rtx xop[3];
  xop[0] = operands[0];
  xop[1] = operands[1];
  xop[2] = gen_lowpart (GET_MODE (operands[4]), operands[2]);
  output_asm_insn ("lsr\t%x0, %x1, %x2", xop);
  return "";
}
}

static const char *
output_722 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5031 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  rtx xop[3];
  xop[0] = operands[0];
  xop[1] = operands[1];
  xop[2] = gen_lowpart (GET_MODE (operands[4]), operands[2]);
  output_asm_insn ("ror\t%x0, %x1, %x2", xop);
  return "";
}
}

static const char * const output_729[] = {
  "lsl\t%w0, %w1, %2",
  "lsl\t%w0, %w1, %w2",
  "shl\t%0.2s, %1.2s, %2",
  "ushl\t%0.2s, %1.2s, %2.2s",
};

static const char * const output_730[] = {
  "lsl\t%x0, %x1, %2",
  "lsl\t%x0, %x1, %x2",
  "shl\t%d0, %d1, %2",
  "ushl\t%d0, %d1, %d2",
};

static const char * const output_731[] = {
  "lsr\t%w0, %w1, %2",
  "lsr\t%w0, %w1, %w2",
  "ushr\t%0.2s, %1.2s, %2",
  "#",
  "#",
};

static const char * const output_732[] = {
  "lsr\t%x0, %x1, %2",
  "lsr\t%x0, %x1, %x2",
  "ushr\t%d0, %d1, %2",
  "#",
  "#",
};

static const char * const output_733[] = {
  "asr\t%w0, %w1, %2",
  "asr\t%w0, %w1, %w2",
  "sshr\t%0.2s, %1.2s, %2",
  "#",
  "#",
};

static const char * const output_734[] = {
  "asr\t%x0, %x1, %2",
  "asr\t%x0, %x1, %x2",
  "sshr\t%d0, %d1, %2",
  "#",
  "#",
};

static const char * const output_740[] = {
  "ror\t%w0, %w1, %2",
  "ror\t%w0, %w1, %w2",
};

static const char * const output_741[] = {
  "ror\t%x0, %x1, %2",
  "ror\t%x0, %x1, %x2",
};

static const char * const output_742[] = {
  "lsl\t%w0, %w1, %2",
  "lsl\t%w0, %w1, %w2",
};

static const char * const output_743[] = {
  "asr\t%w0, %w1, %2",
  "asr\t%w0, %w1, %w2",
};

static const char * const output_744[] = {
  "lsr\t%w0, %w1, %2",
  "lsr\t%w0, %w1, %w2",
};

static const char * const output_745[] = {
  "ror\t%w0, %w1, %2",
  "ror\t%w0, %w1, %w2",
};

static const char *
output_746 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5263 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  operands[3] = GEN_INT (8 - UINTVAL (operands[2]));
  return "ubfiz\t%w0, %w1, %2, %3";
}
}

static const char *
output_747 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5263 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  operands[3] = GEN_INT (8 - UINTVAL (operands[2]));
  return "sbfx\t%w0, %w1, %2, %3";
}
}

static const char *
output_748 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5263 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  operands[3] = GEN_INT (8 - UINTVAL (operands[2]));
  return "ubfx\t%w0, %w1, %2, %3";
}
}

static const char *
output_749 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5263 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  operands[3] = GEN_INT (16 - UINTVAL (operands[2]));
  return "ubfiz\t%w0, %w1, %2, %3";
}
}

static const char *
output_750 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5263 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  operands[3] = GEN_INT (16 - UINTVAL (operands[2]));
  return "sbfx\t%w0, %w1, %2, %3";
}
}

static const char *
output_751 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5263 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  operands[3] = GEN_INT (16 - UINTVAL (operands[2]));
  return "ubfx\t%w0, %w1, %2, %3";
}
}

static const char *
output_758 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5329 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  operands[3] = GEN_INT (32 - UINTVAL (operands[2]));
  return "ror\t%w0, %w1, %3";
}
}

static const char *
output_759 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5329 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  operands[3] = GEN_INT (64 - UINTVAL (operands[2]));
  return "ror\t%x0, %x1, %3";
}
}

static const char *
output_760 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5343 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  operands[3] = GEN_INT (32 - UINTVAL (operands[2]));
  return "ror\t%w0, %w1, %3";
}
}

static const char *
output_761 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5356 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  operands[3] = GEN_INT (8 - UINTVAL (operands[2]));
  return "sbfiz\t%w0, %w1, %2, %3";
}
}

static const char *
output_762 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5356 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  operands[3] = GEN_INT (8 - UINTVAL (operands[2]));
  return "ubfiz\t%w0, %w1, %2, %3";
}
}

static const char *
output_763 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5356 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  operands[3] = GEN_INT (8 - UINTVAL (operands[2]));
  return "sbfiz\t%x0, %x1, %2, %3";
}
}

static const char *
output_764 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5356 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  operands[3] = GEN_INT (8 - UINTVAL (operands[2]));
  return "ubfiz\t%x0, %x1, %2, %3";
}
}

static const char *
output_765 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5356 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  operands[3] = GEN_INT (16 - UINTVAL (operands[2]));
  return "sbfiz\t%w0, %w1, %2, %3";
}
}

static const char *
output_766 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5356 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  operands[3] = GEN_INT (16 - UINTVAL (operands[2]));
  return "ubfiz\t%w0, %w1, %2, %3";
}
}

static const char *
output_767 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5356 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  operands[3] = GEN_INT (16 - UINTVAL (operands[2]));
  return "sbfiz\t%x0, %x1, %2, %3";
}
}

static const char *
output_768 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5356 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  operands[3] = GEN_INT (16 - UINTVAL (operands[2]));
  return "ubfiz\t%x0, %x1, %2, %3";
}
}

static const char *
output_769 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5369 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  operands[3] = GEN_INT (8 - UINTVAL (operands[2]));
  return "ubfx\t%w0, %w1, %2, %3";
}
}

static const char *
output_770 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5369 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  operands[3] = GEN_INT (8 - UINTVAL (operands[2]));
  return "ubfx\t%x0, %x1, %2, %3";
}
}

static const char *
output_771 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5369 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  operands[3] = GEN_INT (16 - UINTVAL (operands[2]));
  return "ubfx\t%w0, %w1, %2, %3";
}
}

static const char *
output_772 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5369 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  operands[3] = GEN_INT (16 - UINTVAL (operands[2]));
  return "ubfx\t%x0, %x1, %2, %3";
}
}

static const char *
output_773 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5382 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  operands[3] = GEN_INT (8 - UINTVAL (operands[2]));
  return "sbfx\t%w0, %w1, %2, %3";
}
}

static const char *
output_774 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5382 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  operands[3] = GEN_INT (8 - UINTVAL (operands[2]));
  return "sbfx\t%x0, %x1, %2, %3";
}
}

static const char *
output_775 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5382 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  operands[3] = GEN_INT (16 - UINTVAL (operands[2]));
  return "sbfx\t%w0, %w1, %2, %3";
}
}

static const char *
output_776 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5382 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  operands[3] = GEN_INT (16 - UINTVAL (operands[2]));
  return "sbfx\t%x0, %x1, %2, %3";
}
}

static const char *
output_793 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5543 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  operands[5] = GEN_INT (GET_MODE_BITSIZE (SImode) - UINTVAL (operands[4]));
  return "bfi\t%w0, %w3, %4, %5";
}
}

static const char *
output_794 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5543 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  operands[5] = GEN_INT (GET_MODE_BITSIZE (DImode) - UINTVAL (operands[4]));
  return "bfi\t%x0, %x3, %4, %5";
}
}

static const char *
output_795 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5560 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  operands[5] = GEN_INT (GET_MODE_BITSIZE (SImode) - UINTVAL (operands[2]));
  return "bfi\t%w0, %w1, %2, %5";
}
}

static const char *
output_796 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5560 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  operands[5] = GEN_INT (GET_MODE_BITSIZE (DImode) - UINTVAL (operands[2]));
  return "bfi\t%x0, %x1, %2, %5";
}
}

static const char *
output_803 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5615 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  operands[3] = (8 <= (32 - UINTVAL (operands[2])))
	      ? GEN_INT (8)
	      : GEN_INT (32 - UINTVAL (operands[2]));
  return "sbfiz\t%w0, %w1, %2, %3";
}
}

static const char *
output_804 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5615 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  operands[3] = (8 <= (32 - UINTVAL (operands[2])))
	      ? GEN_INT (8)
	      : GEN_INT (32 - UINTVAL (operands[2]));
  return "ubfiz\t%w0, %w1, %2, %3";
}
}

static const char *
output_805 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5615 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  operands[3] = (8 <= (64 - UINTVAL (operands[2])))
	      ? GEN_INT (8)
	      : GEN_INT (64 - UINTVAL (operands[2]));
  return "sbfiz\t%x0, %x1, %2, %3";
}
}

static const char *
output_806 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5615 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  operands[3] = (8 <= (64 - UINTVAL (operands[2])))
	      ? GEN_INT (8)
	      : GEN_INT (64 - UINTVAL (operands[2]));
  return "ubfiz\t%x0, %x1, %2, %3";
}
}

static const char *
output_807 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5615 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  operands[3] = (16 <= (32 - UINTVAL (operands[2])))
	      ? GEN_INT (16)
	      : GEN_INT (32 - UINTVAL (operands[2]));
  return "sbfiz\t%w0, %w1, %2, %3";
}
}

static const char *
output_808 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5615 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  operands[3] = (16 <= (32 - UINTVAL (operands[2])))
	      ? GEN_INT (16)
	      : GEN_INT (32 - UINTVAL (operands[2]));
  return "ubfiz\t%w0, %w1, %2, %3";
}
}

static const char *
output_809 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5615 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  operands[3] = (16 <= (64 - UINTVAL (operands[2])))
	      ? GEN_INT (16)
	      : GEN_INT (64 - UINTVAL (operands[2]));
  return "sbfiz\t%x0, %x1, %2, %3";
}
}

static const char *
output_810 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5615 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  operands[3] = (16 <= (64 - UINTVAL (operands[2])))
	      ? GEN_INT (16)
	      : GEN_INT (64 - UINTVAL (operands[2]));
  return "ubfiz\t%x0, %x1, %2, %3";
}
}

static const char *
output_811 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5615 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  operands[3] = (32 <= (32 - UINTVAL (operands[2])))
	      ? GEN_INT (32)
	      : GEN_INT (32 - UINTVAL (operands[2]));
  return "sbfiz\t%w0, %w1, %2, %3";
}
}

static const char *
output_812 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5615 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  operands[3] = (32 <= (32 - UINTVAL (operands[2])))
	      ? GEN_INT (32)
	      : GEN_INT (32 - UINTVAL (operands[2]));
  return "ubfiz\t%w0, %w1, %2, %3";
}
}

static const char *
output_813 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5615 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  operands[3] = (32 <= (64 - UINTVAL (operands[2])))
	      ? GEN_INT (32)
	      : GEN_INT (64 - UINTVAL (operands[2]));
  return "sbfiz\t%x0, %x1, %2, %3";
}
}

static const char *
output_814 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5615 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  operands[3] = (32 <= (64 - UINTVAL (operands[2])))
	      ? GEN_INT (32)
	      : GEN_INT (64 - UINTVAL (operands[2]));
  return "ubfiz\t%x0, %x1, %2, %3";
}
}

static const char *
output_822 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5693 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    switch (which_alternative)
    {
      case 0:
	operands[3] = GEN_INT (ctz_hwi (~INTVAL (operands[3])));
	return "bfxil\t%w0, %w1, 0, %3";
      case 1:
	operands[3] = GEN_INT (ctz_hwi (~INTVAL (operands[4])));
	return "bfxil\t%w0, %w2, 0, %3";
      default:
	gcc_unreachable ();
    }
  }
}

static const char *
output_823 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5693 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    switch (which_alternative)
    {
      case 0:
	operands[3] = GEN_INT (ctz_hwi (~INTVAL (operands[3])));
	return "bfxil\t%x0, %x1, 0, %3";
      case 1:
	operands[3] = GEN_INT (ctz_hwi (~INTVAL (operands[4])));
	return "bfxil\t%x0, %x2, 0, %3";
      default:
	gcc_unreachable ();
    }
  }
}

static const char *
output_824 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5720 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    switch (which_alternative)
    {
      case 0:
	operands[3] = GEN_INT (ctz_hwi (~INTVAL (operands[3])));
	return "bfxil\t%0, %1, 0, %3";
      case 1:
	operands[3] = GEN_INT (ctz_hwi (~INTVAL (operands[4])));
	return "bfxil\t%0, %2, 0, %3";
      default:
	gcc_unreachable ();
    }
  }
}

static const char *
output_911 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5814 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    int fbits = aarch64_fpconst_pow_of_2 (operands[2]);
    char buf[64];
    snprintf (buf, 64, "fcvtzs\t%%w0, %%s1, #%d", fbits);
    output_asm_insn (buf, operands);
    return "";
  }
}

static const char *
output_912 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5814 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    int fbits = aarch64_fpconst_pow_of_2 (operands[2]);
    char buf[64];
    snprintf (buf, 64, "fcvtzu\t%%w0, %%s1, #%d", fbits);
    output_asm_insn (buf, operands);
    return "";
  }
}

static const char *
output_913 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5814 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    int fbits = aarch64_fpconst_pow_of_2 (operands[2]);
    char buf[64];
    snprintf (buf, 64, "fcvtzs\t%%x0, %%s1, #%d", fbits);
    output_asm_insn (buf, operands);
    return "";
  }
}

static const char *
output_914 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5814 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    int fbits = aarch64_fpconst_pow_of_2 (operands[2]);
    char buf[64];
    snprintf (buf, 64, "fcvtzu\t%%x0, %%s1, #%d", fbits);
    output_asm_insn (buf, operands);
    return "";
  }
}

static const char *
output_915 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5814 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    int fbits = aarch64_fpconst_pow_of_2 (operands[2]);
    char buf[64];
    snprintf (buf, 64, "fcvtzs\t%%w0, %%d1, #%d", fbits);
    output_asm_insn (buf, operands);
    return "";
  }
}

static const char *
output_916 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5814 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    int fbits = aarch64_fpconst_pow_of_2 (operands[2]);
    char buf[64];
    snprintf (buf, 64, "fcvtzu\t%%w0, %%d1, #%d", fbits);
    output_asm_insn (buf, operands);
    return "";
  }
}

static const char *
output_917 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5814 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    int fbits = aarch64_fpconst_pow_of_2 (operands[2]);
    char buf[64];
    snprintf (buf, 64, "fcvtzs\t%%x0, %%d1, #%d", fbits);
    output_asm_insn (buf, operands);
    return "";
  }
}

static const char *
output_918 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5814 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    int fbits = aarch64_fpconst_pow_of_2 (operands[2]);
    char buf[64];
    snprintf (buf, 64, "fcvtzu\t%%x0, %%d1, #%d", fbits);
    output_asm_insn (buf, operands);
    return "";
  }
}

static const char * const output_937[] = {
  "fcvtzs\t%s0, %s1",
  "fcvtzs\t%w0, %s1",
};

static const char * const output_938[] = {
  "fcvtzu\t%s0, %s1",
  "fcvtzu\t%w0, %s1",
};

static const char * const output_939[] = {
  "fcvtzs\t%d0, %d1",
  "fcvtzs\t%x0, %d1",
};

static const char * const output_940[] = {
  "fcvtzu\t%d0, %d1",
  "fcvtzu\t%x0, %d1",
};

static const char * const output_951[] = {
  "scvtf\t%s0, %s1",
  "scvtf\t%s0, %w1",
};

static const char * const output_952[] = {
  "ucvtf\t%s0, %s1",
  "ucvtf\t%s0, %w1",
};

static const char * const output_953[] = {
  "scvtf\t%d0, %d1",
  "scvtf\t%d0, %x1",
};

static const char * const output_954[] = {
  "ucvtf\t%d0, %d1",
  "ucvtf\t%d0, %x1",
};

static const char * const output_963[] = {
  "fcvtzs\t%w0, %s1, #%2",
  "fcvtzs\t%s0, %s1, #%2",
};

static const char * const output_964[] = {
  "fcvtzu\t%w0, %s1, #%2",
  "fcvtzu\t%s0, %s1, #%2",
};

static const char * const output_965[] = {
  "fcvtzs\t%x0, %d1, #%2",
  "fcvtzs\t%d0, %d1, #%2",
};

static const char * const output_966[] = {
  "fcvtzu\t%x0, %d1, #%2",
  "fcvtzu\t%d0, %d1, #%2",
};

static const char * const output_967[] = {
  "scvtf\t%s0, %w1, #%2",
  "scvtf\t%s0, %s1, #%2",
};

static const char * const output_968[] = {
  "ucvtf\t%s0, %w1, #%2",
  "ucvtf\t%s0, %s1, #%2",
};

static const char * const output_969[] = {
  "scvtf\t%d0, %x1, #%2",
  "scvtf\t%d0, %d1, #%2",
};

static const char * const output_970[] = {
  "ucvtf\t%d0, %x1, #%2",
  "ucvtf\t%d0, %d1, #%2",
};

static const char * const output_1024[] = {
  "bsl\t%0.8b, %2.8b, %1.8b",
  "bit\t%0.8b, %2.8b, %3.8b",
  "bif\t%0.8b, %1.8b, %3.8b",
  "bfxil\t%w0, %w1, #0, #31",
};

static const char * const output_1025[] = {
  "bsl\t%0.8b, %2.8b, %1.8b",
  "bit\t%0.8b, %2.8b, %3.8b",
  "bif\t%0.8b, %1.8b, %3.8b",
  "bfxil\t%x0, %x1, #0, #63",
};

static const char *
output_1072 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 6851 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  return aarch64_output_probe_stack_range (operands[0], operands[2]);
}
}

static const char *
output_1073 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 6869 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  return aarch64_output_probe_sve_stack_clash (operands[0], operands[2],
					       operands[3], operands[4]);
}
}

static const char *
output_1074 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 6869 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
  return aarch64_output_probe_sve_stack_clash (operands[0], operands[2],
					       operands[3], operands[4]);
}
}

static const char *
output_1075 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 6925 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
   char buf[150];
   snprintf (buf, 150, "mrs\t%%w0, %s",
	    aarch64_stack_protector_guard_reg_str);
   output_asm_insn (buf, operands);
   return "";
 }
}

static const char *
output_1076 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 6925 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
   char buf[150];
   snprintf (buf, 150, "mrs\t%%x0, %s",
	    aarch64_stack_protector_guard_reg_str);
   output_asm_insn (buf, operands);
   return "";
 }
}

static const char *
output_1085 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 7078 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    operands[1] = gen_rtx_REG (DImode, SPECULATION_TRACKER_REGNUM);
    output_asm_insn ("csel\t%1, %1, xzr, %m0", operands);
    return "";
  }
}

static const char *
output_1091 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 7170 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    operands[3] = gen_rtx_REG (DImode, SPECULATION_TRACKER_REGNUM);
    output_asm_insn ("cmp\t%3, #0\n\tcsel\t%w0, %w1, %w2, ne\n\thint\t0x14 // csdb",
		     operands);
    return "";
  }
}

static const char *
output_1092 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 7170 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    operands[3] = gen_rtx_REG (DImode, SPECULATION_TRACKER_REGNUM);
    output_asm_insn ("cmp\t%3, #0\n\tcsel\t%w0, %w1, %w2, ne\n\thint\t0x14 // csdb",
		     operands);
    return "";
  }
}

static const char *
output_1093 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 7170 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    operands[3] = gen_rtx_REG (DImode, SPECULATION_TRACKER_REGNUM);
    output_asm_insn ("cmp\t%3, #0\n\tcsel\t%w0, %w1, %w2, ne\n\thint\t0x14 // csdb",
		     operands);
    return "";
  }
}

static const char *
output_1094 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 7170 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    operands[3] = gen_rtx_REG (DImode, SPECULATION_TRACKER_REGNUM);
    output_asm_insn ("cmp\t%3, #0\n\tcsel\t%x0, %x1, %x2, ne\n\thint\t0x14 // csdb",
		     operands);
    return "";
  }
}

static const char *
output_1095 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 7190 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64.md"
{
    operands[3] = gen_rtx_REG (DImode, SPECULATION_TRACKER_REGNUM);
    output_asm_insn
      ("cmp\t%3, #0\n\tcsel\t%0, %1, %2, ne\n\tcsel\t%H0, %H1, %H2, ne\n\thint\t0x14 // csdb",
       operands);
    return "";
  }
}

static const char * const output_1101[] = {
  "dup\t%0.8b, %1.b[0]",
  "dup\t%0.8b, %w1",
};

static const char * const output_1102[] = {
  "dup\t%0.16b, %1.b[0]",
  "dup\t%0.16b, %w1",
};

static const char * const output_1103[] = {
  "dup\t%0.4h, %1.h[0]",
  "dup\t%0.4h, %w1",
};

static const char * const output_1104[] = {
  "dup\t%0.8h, %1.h[0]",
  "dup\t%0.8h, %w1",
};

static const char * const output_1105[] = {
  "dup\t%0.2s, %1.s[0]",
  "dup\t%0.2s, %w1",
};

static const char * const output_1106[] = {
  "dup\t%0.4s, %1.s[0]",
  "dup\t%0.4s, %w1",
};

static const char * const output_1107[] = {
  "dup\t%0.2d, %1.d[0]",
  "dup\t%0.2d, %x1",
};

static const char *
output_1113 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 82 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V8QImode, INTVAL (operands[2]));
    return "dup\t%0.8b, %1.b[%2]";
  }
}

static const char *
output_1114 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 82 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V16QImode, INTVAL (operands[2]));
    return "dup\t%0.16b, %1.b[%2]";
  }
}

static const char *
output_1115 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 82 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4HImode, INTVAL (operands[2]));
    return "dup\t%0.4h, %1.h[%2]";
  }
}

static const char *
output_1116 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 82 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V8HImode, INTVAL (operands[2]));
    return "dup\t%0.8h, %1.h[%2]";
  }
}

static const char *
output_1117 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 82 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V2SImode, INTVAL (operands[2]));
    return "dup\t%0.2s, %1.s[%2]";
  }
}

static const char *
output_1118 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 82 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4SImode, INTVAL (operands[2]));
    return "dup\t%0.4s, %1.s[%2]";
  }
}

static const char *
output_1119 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 82 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V2DImode, INTVAL (operands[2]));
    return "dup\t%0.2d, %1.d[%2]";
  }
}

static const char *
output_1120 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 82 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4HFmode, INTVAL (operands[2]));
    return "dup\t%0.4h, %1.h[%2]";
  }
}

static const char *
output_1121 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 82 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V8HFmode, INTVAL (operands[2]));
    return "dup\t%0.8h, %1.h[%2]";
  }
}

static const char *
output_1122 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 82 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V2SFmode, INTVAL (operands[2]));
    return "dup\t%0.2s, %1.s[%2]";
  }
}

static const char *
output_1123 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 82 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4SFmode, INTVAL (operands[2]));
    return "dup\t%0.4s, %1.s[%2]";
  }
}

static const char *
output_1124 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 82 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V2DFmode, INTVAL (operands[2]));
    return "dup\t%0.2d, %1.d[%2]";
  }
}

static const char *
output_1125 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 97 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V16QImode, INTVAL (operands[2]));
    return "dup\t%0.8b, %1.b[%2]";
  }
}

static const char *
output_1126 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 97 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V8QImode, INTVAL (operands[2]));
    return "dup\t%0.16b, %1.b[%2]";
  }
}

static const char *
output_1127 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 97 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V8HImode, INTVAL (operands[2]));
    return "dup\t%0.4h, %1.h[%2]";
  }
}

static const char *
output_1128 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 97 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4HImode, INTVAL (operands[2]));
    return "dup\t%0.8h, %1.h[%2]";
  }
}

static const char *
output_1129 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 97 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4SImode, INTVAL (operands[2]));
    return "dup\t%0.2s, %1.s[%2]";
  }
}

static const char *
output_1130 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 97 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V2SImode, INTVAL (operands[2]));
    return "dup\t%0.4s, %1.s[%2]";
  }
}

static const char *
output_1131 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 97 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V8HFmode, INTVAL (operands[2]));
    return "dup\t%0.4h, %1.h[%2]";
  }
}

static const char *
output_1132 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 97 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4HFmode, INTVAL (operands[2]));
    return "dup\t%0.8h, %1.h[%2]";
  }
}

static const char *
output_1133 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 97 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4SFmode, INTVAL (operands[2]));
    return "dup\t%0.2s, %1.s[%2]";
  }
}

static const char *
output_1134 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 97 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V2SFmode, INTVAL (operands[2]));
    return "dup\t%0.4s, %1.s[%2]";
  }
}

static const char *
output_1135 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 112 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
   switch (which_alternative)
     {
     case 0: return "ldr\t%d0, %1";
     case 1: return "str\txzr, %0";
     case 2: return "str\t%d1, %0";
     case 3: return "mov\t%0.8b, %1.8b";
     case 4: return "umov\t%0, %1.d[0]";
     case 5: return "fmov\t%d0, %1";
     case 6: return "mov\t%0, %1";
     case 7:
	return aarch64_output_simd_mov_immediate (operands[1], 64);
     default: gcc_unreachable ();
     }
}
}

static const char *
output_1136 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 112 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
   switch (which_alternative)
     {
     case 0: return "ldr\t%d0, %1";
     case 1: return "str\txzr, %0";
     case 2: return "str\t%d1, %0";
     case 3: return "mov\t%0.8b, %1.8b";
     case 4: return "umov\t%0, %1.d[0]";
     case 5: return "fmov\t%d0, %1";
     case 6: return "mov\t%0, %1";
     case 7:
	return aarch64_output_simd_mov_immediate (operands[1], 64);
     default: gcc_unreachable ();
     }
}
}

static const char *
output_1137 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 112 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
   switch (which_alternative)
     {
     case 0: return "ldr\t%d0, %1";
     case 1: return "str\txzr, %0";
     case 2: return "str\t%d1, %0";
     case 3: return "mov\t%0.8b, %1.8b";
     case 4: return "umov\t%0, %1.d[0]";
     case 5: return "fmov\t%d0, %1";
     case 6: return "mov\t%0, %1";
     case 7:
	return aarch64_output_simd_mov_immediate (operands[1], 64);
     default: gcc_unreachable ();
     }
}
}

static const char *
output_1138 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 112 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
   switch (which_alternative)
     {
     case 0: return "ldr\t%d0, %1";
     case 1: return "str\txzr, %0";
     case 2: return "str\t%d1, %0";
     case 3: return "mov\t%0.8b, %1.8b";
     case 4: return "umov\t%0, %1.d[0]";
     case 5: return "fmov\t%d0, %1";
     case 6: return "mov\t%0, %1";
     case 7:
	return aarch64_output_simd_mov_immediate (operands[1], 64);
     default: gcc_unreachable ();
     }
}
}

static const char *
output_1139 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 112 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
   switch (which_alternative)
     {
     case 0: return "ldr\t%d0, %1";
     case 1: return "str\txzr, %0";
     case 2: return "str\t%d1, %0";
     case 3: return "mov\t%0.8b, %1.8b";
     case 4: return "umov\t%0, %1.d[0]";
     case 5: return "fmov\t%d0, %1";
     case 6: return "mov\t%0, %1";
     case 7:
	return aarch64_output_simd_mov_immediate (operands[1], 64);
     default: gcc_unreachable ();
     }
}
}

static const char *
output_1140 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 140 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  switch (which_alternative)
    {
    case 0:
	return "ldr\t%q0, %1";
    case 1:
	return "stp\txzr, xzr, %0";
    case 2:
	return "str\t%q1, %0";
    case 3:
	return "mov\t%0.16b, %1.16b";
    case 4:
    case 5:
    case 6:
	return "#";
    case 7:
	return aarch64_output_simd_mov_immediate (operands[1], 128);
    default:
	gcc_unreachable ();
    }
}
}

static const char *
output_1141 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 140 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  switch (which_alternative)
    {
    case 0:
	return "ldr\t%q0, %1";
    case 1:
	return "stp\txzr, xzr, %0";
    case 2:
	return "str\t%q1, %0";
    case 3:
	return "mov\t%0.16b, %1.16b";
    case 4:
    case 5:
    case 6:
	return "#";
    case 7:
	return aarch64_output_simd_mov_immediate (operands[1], 128);
    default:
	gcc_unreachable ();
    }
}
}

static const char *
output_1142 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 140 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  switch (which_alternative)
    {
    case 0:
	return "ldr\t%q0, %1";
    case 1:
	return "stp\txzr, xzr, %0";
    case 2:
	return "str\t%q1, %0";
    case 3:
	return "mov\t%0.16b, %1.16b";
    case 4:
    case 5:
    case 6:
	return "#";
    case 7:
	return aarch64_output_simd_mov_immediate (operands[1], 128);
    default:
	gcc_unreachable ();
    }
}
}

static const char *
output_1143 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 140 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  switch (which_alternative)
    {
    case 0:
	return "ldr\t%q0, %1";
    case 1:
	return "stp\txzr, xzr, %0";
    case 2:
	return "str\t%q1, %0";
    case 3:
	return "mov\t%0.16b, %1.16b";
    case 4:
    case 5:
    case 6:
	return "#";
    case 7:
	return aarch64_output_simd_mov_immediate (operands[1], 128);
    default:
	gcc_unreachable ();
    }
}
}

static const char *
output_1144 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 140 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  switch (which_alternative)
    {
    case 0:
	return "ldr\t%q0, %1";
    case 1:
	return "stp\txzr, xzr, %0";
    case 2:
	return "str\t%q1, %0";
    case 3:
	return "mov\t%0.16b, %1.16b";
    case 4:
    case 5:
    case 6:
	return "#";
    case 7:
	return aarch64_output_simd_mov_immediate (operands[1], 128);
    default:
	gcc_unreachable ();
    }
}
}

static const char *
output_1145 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 140 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  switch (which_alternative)
    {
    case 0:
	return "ldr\t%q0, %1";
    case 1:
	return "stp\txzr, xzr, %0";
    case 2:
	return "str\t%q1, %0";
    case 3:
	return "mov\t%0.16b, %1.16b";
    case 4:
    case 5:
    case 6:
	return "#";
    case 7:
	return aarch64_output_simd_mov_immediate (operands[1], 128);
    default:
	gcc_unreachable ();
    }
}
}

static const char *
output_1146 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 140 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  switch (which_alternative)
    {
    case 0:
	return "ldr\t%q0, %1";
    case 1:
	return "stp\txzr, xzr, %0";
    case 2:
	return "str\t%q1, %0";
    case 3:
	return "mov\t%0.16b, %1.16b";
    case 4:
    case 5:
    case 6:
	return "#";
    case 7:
	return aarch64_output_simd_mov_immediate (operands[1], 128);
    default:
	gcc_unreachable ();
    }
}
}

static const char *
output_1414 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 458 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  operands[4] = aarch64_endian_lane_rtx (V2HFmode, INTVAL (operands[4]));
  return "fcmla\t%0.4h, %2.4h, %3.h[%4], #0";
}
}

static const char *
output_1415 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 458 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  operands[4] = aarch64_endian_lane_rtx (V2HFmode, INTVAL (operands[4]));
  return "fcmla\t%0.4h, %2.4h, %3.h[%4], #90";
}
}

static const char *
output_1416 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 458 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  operands[4] = aarch64_endian_lane_rtx (V2HFmode, INTVAL (operands[4]));
  return "fcmla\t%0.4h, %2.4h, %3.h[%4], #180";
}
}

static const char *
output_1417 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 458 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  operands[4] = aarch64_endian_lane_rtx (V2HFmode, INTVAL (operands[4]));
  return "fcmla\t%0.4h, %2.4h, %3.h[%4], #270";
}
}

static const char *
output_1418 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 458 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  operands[4] = aarch64_endian_lane_rtx (V4HFmode, INTVAL (operands[4]));
  return "fcmla\t%0.8h, %2.8h, %3.h[%4], #0";
}
}

static const char *
output_1419 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 458 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  operands[4] = aarch64_endian_lane_rtx (V4HFmode, INTVAL (operands[4]));
  return "fcmla\t%0.8h, %2.8h, %3.h[%4], #90";
}
}

static const char *
output_1420 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 458 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  operands[4] = aarch64_endian_lane_rtx (V4HFmode, INTVAL (operands[4]));
  return "fcmla\t%0.8h, %2.8h, %3.h[%4], #180";
}
}

static const char *
output_1421 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 458 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  operands[4] = aarch64_endian_lane_rtx (V4HFmode, INTVAL (operands[4]));
  return "fcmla\t%0.8h, %2.8h, %3.h[%4], #270";
}
}

static const char *
output_1422 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 458 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  operands[4] = aarch64_endian_lane_rtx (SFmode, INTVAL (operands[4]));
  return "fcmla\t%0.2s, %2.2s, %3.2s, #0";
}
}

static const char *
output_1423 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 458 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  operands[4] = aarch64_endian_lane_rtx (SFmode, INTVAL (operands[4]));
  return "fcmla\t%0.2s, %2.2s, %3.2s, #90";
}
}

static const char *
output_1424 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 458 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  operands[4] = aarch64_endian_lane_rtx (SFmode, INTVAL (operands[4]));
  return "fcmla\t%0.2s, %2.2s, %3.2s, #180";
}
}

static const char *
output_1425 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 458 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  operands[4] = aarch64_endian_lane_rtx (SFmode, INTVAL (operands[4]));
  return "fcmla\t%0.2s, %2.2s, %3.2s, #270";
}
}

static const char *
output_1426 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 458 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  operands[4] = aarch64_endian_lane_rtx (V2SFmode, INTVAL (operands[4]));
  return "fcmla\t%0.4s, %2.4s, %3.s[%4], #0";
}
}

static const char *
output_1427 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 458 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  operands[4] = aarch64_endian_lane_rtx (V2SFmode, INTVAL (operands[4]));
  return "fcmla\t%0.4s, %2.4s, %3.s[%4], #90";
}
}

static const char *
output_1428 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 458 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  operands[4] = aarch64_endian_lane_rtx (V2SFmode, INTVAL (operands[4]));
  return "fcmla\t%0.4s, %2.4s, %3.s[%4], #180";
}
}

static const char *
output_1429 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 458 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  operands[4] = aarch64_endian_lane_rtx (V2SFmode, INTVAL (operands[4]));
  return "fcmla\t%0.4s, %2.4s, %3.s[%4], #270";
}
}

static const char *
output_1430 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 458 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  operands[4] = aarch64_endian_lane_rtx (DFmode, INTVAL (operands[4]));
  return "fcmla\t%0.2d, %2.2d, %3.<FCMLA_maybe_lane>, #0";
}
}

static const char *
output_1431 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 458 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  operands[4] = aarch64_endian_lane_rtx (DFmode, INTVAL (operands[4]));
  return "fcmla\t%0.2d, %2.2d, %3.<FCMLA_maybe_lane>, #90";
}
}

static const char *
output_1432 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 458 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  operands[4] = aarch64_endian_lane_rtx (DFmode, INTVAL (operands[4]));
  return "fcmla\t%0.2d, %2.2d, %3.<FCMLA_maybe_lane>, #180";
}
}

static const char *
output_1433 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 458 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  operands[4] = aarch64_endian_lane_rtx (DFmode, INTVAL (operands[4]));
  return "fcmla\t%0.2d, %2.2d, %3.<FCMLA_maybe_lane>, #270";
}
}

static const char *
output_1434 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 473 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  operands[4] = aarch64_endian_lane_rtx (V4HFmode, INTVAL (operands[4]));
  return "fcmla\t%0.4h, %2.4h, %3.h[%4], #0";
}
}

static const char *
output_1435 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 473 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  operands[4] = aarch64_endian_lane_rtx (V4HFmode, INTVAL (operands[4]));
  return "fcmla\t%0.4h, %2.4h, %3.h[%4], #90";
}
}

static const char *
output_1436 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 473 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  operands[4] = aarch64_endian_lane_rtx (V4HFmode, INTVAL (operands[4]));
  return "fcmla\t%0.4h, %2.4h, %3.h[%4], #180";
}
}

static const char *
output_1437 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 473 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  operands[4] = aarch64_endian_lane_rtx (V4HFmode, INTVAL (operands[4]));
  return "fcmla\t%0.4h, %2.4h, %3.h[%4], #270";
}
}

static const char *
output_1438 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 488 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  int nunits = GET_MODE_NUNITS (V4HFmode).to_constant ();
  operands[4]
    = gen_int_mode (ENDIAN_LANE_N (nunits / 2, INTVAL (operands[4])), SImode);
  return "fcmla\t%0.8h, %2.8h, %3.h[%4], #0";
}
}

static const char *
output_1439 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 488 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  int nunits = GET_MODE_NUNITS (V4HFmode).to_constant ();
  operands[4]
    = gen_int_mode (ENDIAN_LANE_N (nunits / 2, INTVAL (operands[4])), SImode);
  return "fcmla\t%0.8h, %2.8h, %3.h[%4], #90";
}
}

static const char *
output_1440 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 488 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  int nunits = GET_MODE_NUNITS (V4HFmode).to_constant ();
  operands[4]
    = gen_int_mode (ENDIAN_LANE_N (nunits / 2, INTVAL (operands[4])), SImode);
  return "fcmla\t%0.8h, %2.8h, %3.h[%4], #180";
}
}

static const char *
output_1441 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 488 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  int nunits = GET_MODE_NUNITS (V4HFmode).to_constant ();
  operands[4]
    = gen_int_mode (ENDIAN_LANE_N (nunits / 2, INTVAL (operands[4])), SImode);
  return "fcmla\t%0.8h, %2.8h, %3.h[%4], #270";
}
}

static const char *
output_1442 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 488 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  int nunits = GET_MODE_NUNITS (V2SFmode).to_constant ();
  operands[4]
    = gen_int_mode (ENDIAN_LANE_N (nunits / 2, INTVAL (operands[4])), SImode);
  return "fcmla\t%0.4s, %2.4s, %3.s[%4], #0";
}
}

static const char *
output_1443 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 488 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  int nunits = GET_MODE_NUNITS (V2SFmode).to_constant ();
  operands[4]
    = gen_int_mode (ENDIAN_LANE_N (nunits / 2, INTVAL (operands[4])), SImode);
  return "fcmla\t%0.4s, %2.4s, %3.s[%4], #90";
}
}

static const char *
output_1444 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 488 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  int nunits = GET_MODE_NUNITS (V2SFmode).to_constant ();
  operands[4]
    = gen_int_mode (ENDIAN_LANE_N (nunits / 2, INTVAL (operands[4])), SImode);
  return "fcmla\t%0.4s, %2.4s, %3.s[%4], #180";
}
}

static const char *
output_1445 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 488 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  int nunits = GET_MODE_NUNITS (V2SFmode).to_constant ();
  operands[4]
    = gen_int_mode (ENDIAN_LANE_N (nunits / 2, INTVAL (operands[4])), SImode);
  return "fcmla\t%0.4s, %2.4s, %3.s[%4], #270";
}
}

static const char *
output_1450 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 554 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[4] = aarch64_endian_lane_rtx (V8QImode, INTVAL (operands[4]));
    return "sdot\t%0.2s, %2.8b, %3.4b[%4]";
  }
}

static const char *
output_1451 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 554 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[4] = aarch64_endian_lane_rtx (V8QImode, INTVAL (operands[4]));
    return "udot\t%0.2s, %2.8b, %3.4b[%4]";
  }
}

static const char *
output_1452 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 554 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[4] = aarch64_endian_lane_rtx (V8QImode, INTVAL (operands[4]));
    return "sdot\t%0.4s, %2.16b, %3.4b[%4]";
  }
}

static const char *
output_1453 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 554 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[4] = aarch64_endian_lane_rtx (V8QImode, INTVAL (operands[4]));
    return "udot\t%0.4s, %2.16b, %3.4b[%4]";
  }
}

static const char *
output_1454 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 569 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[4] = aarch64_endian_lane_rtx (V16QImode, INTVAL (operands[4]));
    return "sdot\t%0.2s, %2.8b, %3.4b[%4]";
  }
}

static const char *
output_1455 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 569 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[4] = aarch64_endian_lane_rtx (V16QImode, INTVAL (operands[4]));
    return "udot\t%0.2s, %2.8b, %3.4b[%4]";
  }
}

static const char *
output_1456 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 569 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[4] = aarch64_endian_lane_rtx (V16QImode, INTVAL (operands[4]));
    return "sdot\t%0.4s, %2.16b, %3.4b[%4]";
  }
}

static const char *
output_1457 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 569 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[4] = aarch64_endian_lane_rtx (V16QImode, INTVAL (operands[4]));
    return "udot\t%0.4s, %2.16b, %3.4b[%4]";
  }
}

static const char *
output_1458 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 603 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4HImode, INTVAL (operands[2]));
    return "mul\t%0.4h, %3.4h, %1.h[%2]";
  }
}

static const char *
output_1459 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 603 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V8HImode, INTVAL (operands[2]));
    return "mul\t%0.8h, %3.8h, %1.h[%2]";
  }
}

static const char *
output_1460 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 603 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V2SImode, INTVAL (operands[2]));
    return "mul\t%0.2s, %3.2s, %1.s[%2]";
  }
}

static const char *
output_1461 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 603 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4SImode, INTVAL (operands[2]));
    return "mul\t%0.4s, %3.4s, %1.s[%2]";
  }
}

static const char *
output_1462 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 603 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4HFmode, INTVAL (operands[2]));
    return "fmul\t%0.4h, %3.4h, %1.h[%2]";
  }
}

static const char *
output_1463 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 603 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V8HFmode, INTVAL (operands[2]));
    return "fmul\t%0.8h, %3.8h, %1.h[%2]";
  }
}

static const char *
output_1464 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 603 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V2SFmode, INTVAL (operands[2]));
    return "fmul\t%0.2s, %3.2s, %1.s[%2]";
  }
}

static const char *
output_1465 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 603 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4SFmode, INTVAL (operands[2]));
    return "fmul\t%0.4s, %3.4s, %1.s[%2]";
  }
}

static const char *
output_1466 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 603 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V2DFmode, INTVAL (operands[2]));
    return "fmul\t%0.2d, %3.2d, %1.d[%2]";
  }
}

static const char *
output_1467 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 619 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V8HImode, INTVAL (operands[2]));
    return "mul\t%0.4h, %3.4h, %1.h[%2]";
  }
}

static const char *
output_1468 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 619 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4HImode, INTVAL (operands[2]));
    return "mul\t%0.8h, %3.8h, %1.h[%2]";
  }
}

static const char *
output_1469 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 619 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4SImode, INTVAL (operands[2]));
    return "mul\t%0.2s, %3.2s, %1.s[%2]";
  }
}

static const char *
output_1470 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 619 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V2SImode, INTVAL (operands[2]));
    return "mul\t%0.4s, %3.4s, %1.s[%2]";
  }
}

static const char *
output_1471 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 619 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4SFmode, INTVAL (operands[2]));
    return "fmul\t%0.2s, %3.2s, %1.s[%2]";
  }
}

static const char *
output_1472 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 619 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V2SFmode, INTVAL (operands[2]));
    return "fmul\t%0.4s, %3.4s, %1.s[%2]";
  }
}

static const char *
output_1498 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 672 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V2DFmode, INTVAL (operands[2]));
    return "fmul\t%0.2d, %3.2d, %1.d[%2]";
  }
}

static const char *
output_1577 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 816 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    switch (which_alternative)
      {
      case 0:
	return "and\t%0.8b, %1.8b, %2.8b";
      case 1:
	return aarch64_output_simd_mov_immediate (operands[2], 64,
						  AARCH64_CHECK_BIC);
      default:
	gcc_unreachable ();
      }
  }
}

static const char *
output_1578 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 816 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    switch (which_alternative)
      {
      case 0:
	return "and\t%0.16b, %1.16b, %2.16b";
      case 1:
	return aarch64_output_simd_mov_immediate (operands[2], 128,
						  AARCH64_CHECK_BIC);
      default:
	gcc_unreachable ();
      }
  }
}

static const char *
output_1579 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 816 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    switch (which_alternative)
      {
      case 0:
	return "and\t%0.8b, %1.8b, %2.8b";
      case 1:
	return aarch64_output_simd_mov_immediate (operands[2], 64,
						  AARCH64_CHECK_BIC);
      default:
	gcc_unreachable ();
      }
  }
}

static const char *
output_1580 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 816 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    switch (which_alternative)
      {
      case 0:
	return "and\t%0.16b, %1.16b, %2.16b";
      case 1:
	return aarch64_output_simd_mov_immediate (operands[2], 128,
						  AARCH64_CHECK_BIC);
      default:
	gcc_unreachable ();
      }
  }
}

static const char *
output_1581 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 816 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    switch (which_alternative)
      {
      case 0:
	return "and\t%0.8b, %1.8b, %2.8b";
      case 1:
	return aarch64_output_simd_mov_immediate (operands[2], 64,
						  AARCH64_CHECK_BIC);
      default:
	gcc_unreachable ();
      }
  }
}

static const char *
output_1582 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 816 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    switch (which_alternative)
      {
      case 0:
	return "and\t%0.16b, %1.16b, %2.16b";
      case 1:
	return aarch64_output_simd_mov_immediate (operands[2], 128,
						  AARCH64_CHECK_BIC);
      default:
	gcc_unreachable ();
      }
  }
}

static const char *
output_1583 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 816 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    switch (which_alternative)
      {
      case 0:
	return "and\t%0.16b, %1.16b, %2.16b";
      case 1:
	return aarch64_output_simd_mov_immediate (operands[2], 128,
						  AARCH64_CHECK_BIC);
      default:
	gcc_unreachable ();
      }
  }
}

static const char *
output_1584 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 837 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    switch (which_alternative)
      {
      case 0:
	return "orr\t%0.8b, %1.8b, %2.8b";
      case 1:
	return aarch64_output_simd_mov_immediate (operands[2], 64,
						  AARCH64_CHECK_ORR);
      default:
	gcc_unreachable ();
      }
  }
}

static const char *
output_1585 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 837 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    switch (which_alternative)
      {
      case 0:
	return "orr\t%0.16b, %1.16b, %2.16b";
      case 1:
	return aarch64_output_simd_mov_immediate (operands[2], 128,
						  AARCH64_CHECK_ORR);
      default:
	gcc_unreachable ();
      }
  }
}

static const char *
output_1586 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 837 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    switch (which_alternative)
      {
      case 0:
	return "orr\t%0.8b, %1.8b, %2.8b";
      case 1:
	return aarch64_output_simd_mov_immediate (operands[2], 64,
						  AARCH64_CHECK_ORR);
      default:
	gcc_unreachable ();
      }
  }
}

static const char *
output_1587 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 837 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    switch (which_alternative)
      {
      case 0:
	return "orr\t%0.16b, %1.16b, %2.16b";
      case 1:
	return aarch64_output_simd_mov_immediate (operands[2], 128,
						  AARCH64_CHECK_ORR);
      default:
	gcc_unreachable ();
      }
  }
}

static const char *
output_1588 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 837 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    switch (which_alternative)
      {
      case 0:
	return "orr\t%0.8b, %1.8b, %2.8b";
      case 1:
	return aarch64_output_simd_mov_immediate (operands[2], 64,
						  AARCH64_CHECK_ORR);
      default:
	gcc_unreachable ();
      }
  }
}

static const char *
output_1589 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 837 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    switch (which_alternative)
      {
      case 0:
	return "orr\t%0.16b, %1.16b, %2.16b";
      case 1:
	return aarch64_output_simd_mov_immediate (operands[2], 128,
						  AARCH64_CHECK_ORR);
      default:
	gcc_unreachable ();
      }
  }
}

static const char *
output_1590 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 837 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    switch (which_alternative)
      {
      case 0:
	return "orr\t%0.16b, %1.16b, %2.16b";
      case 1:
	return aarch64_output_simd_mov_immediate (operands[2], 128,
						  AARCH64_CHECK_ORR);
      default:
	gcc_unreachable ();
      }
  }
}

static const char *
output_1605 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 877 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
   int elt = ENDIAN_LANE_N (8, exact_log2 (INTVAL (operands[2])));
   operands[2] = GEN_INT ((HOST_WIDE_INT) 1 << elt);
   switch (which_alternative)
     {
     case 0:
	return "ins\t%0.b[%p2], %1.b[0]";
     case 1:
	return "ins\t%0.b[%p2], %w1";
     case 2:
        return "ld1\t{%0.b}[%p2], %1";
     default:
	gcc_unreachable ();
     }
  }
}

static const char *
output_1606 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 877 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
   int elt = ENDIAN_LANE_N (16, exact_log2 (INTVAL (operands[2])));
   operands[2] = GEN_INT ((HOST_WIDE_INT) 1 << elt);
   switch (which_alternative)
     {
     case 0:
	return "ins\t%0.b[%p2], %1.b[0]";
     case 1:
	return "ins\t%0.b[%p2], %w1";
     case 2:
        return "ld1\t{%0.b}[%p2], %1";
     default:
	gcc_unreachable ();
     }
  }
}

static const char *
output_1607 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 877 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
   int elt = ENDIAN_LANE_N (4, exact_log2 (INTVAL (operands[2])));
   operands[2] = GEN_INT ((HOST_WIDE_INT) 1 << elt);
   switch (which_alternative)
     {
     case 0:
	return "ins\t%0.h[%p2], %1.h[0]";
     case 1:
	return "ins\t%0.h[%p2], %w1";
     case 2:
        return "ld1\t{%0.h}[%p2], %1";
     default:
	gcc_unreachable ();
     }
  }
}

static const char *
output_1608 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 877 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
   int elt = ENDIAN_LANE_N (8, exact_log2 (INTVAL (operands[2])));
   operands[2] = GEN_INT ((HOST_WIDE_INT) 1 << elt);
   switch (which_alternative)
     {
     case 0:
	return "ins\t%0.h[%p2], %1.h[0]";
     case 1:
	return "ins\t%0.h[%p2], %w1";
     case 2:
        return "ld1\t{%0.h}[%p2], %1";
     default:
	gcc_unreachable ();
     }
  }
}

static const char *
output_1609 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 877 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
   int elt = ENDIAN_LANE_N (2, exact_log2 (INTVAL (operands[2])));
   operands[2] = GEN_INT ((HOST_WIDE_INT) 1 << elt);
   switch (which_alternative)
     {
     case 0:
	return "ins\t%0.s[%p2], %1.s[0]";
     case 1:
	return "ins\t%0.s[%p2], %w1";
     case 2:
        return "ld1\t{%0.s}[%p2], %1";
     default:
	gcc_unreachable ();
     }
  }
}

static const char *
output_1610 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 877 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
   int elt = ENDIAN_LANE_N (4, exact_log2 (INTVAL (operands[2])));
   operands[2] = GEN_INT ((HOST_WIDE_INT) 1 << elt);
   switch (which_alternative)
     {
     case 0:
	return "ins\t%0.s[%p2], %1.s[0]";
     case 1:
	return "ins\t%0.s[%p2], %w1";
     case 2:
        return "ld1\t{%0.s}[%p2], %1";
     default:
	gcc_unreachable ();
     }
  }
}

static const char *
output_1611 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 877 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
   int elt = ENDIAN_LANE_N (2, exact_log2 (INTVAL (operands[2])));
   operands[2] = GEN_INT ((HOST_WIDE_INT) 1 << elt);
   switch (which_alternative)
     {
     case 0:
	return "ins\t%0.d[%p2], %1.d[0]";
     case 1:
	return "ins\t%0.d[%p2], %x1";
     case 2:
        return "ld1\t{%0.d}[%p2], %1";
     default:
	gcc_unreachable ();
     }
  }
}

static const char *
output_1612 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 877 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
   int elt = ENDIAN_LANE_N (4, exact_log2 (INTVAL (operands[2])));
   operands[2] = GEN_INT ((HOST_WIDE_INT) 1 << elt);
   switch (which_alternative)
     {
     case 0:
	return "ins\t%0.h[%p2], %1.h[0]";
     case 1:
	return "ins\t%0.h[%p2], %w1";
     case 2:
        return "ld1\t{%0.h}[%p2], %1";
     default:
	gcc_unreachable ();
     }
  }
}

static const char *
output_1613 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 877 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
   int elt = ENDIAN_LANE_N (8, exact_log2 (INTVAL (operands[2])));
   operands[2] = GEN_INT ((HOST_WIDE_INT) 1 << elt);
   switch (which_alternative)
     {
     case 0:
	return "ins\t%0.h[%p2], %1.h[0]";
     case 1:
	return "ins\t%0.h[%p2], %w1";
     case 2:
        return "ld1\t{%0.h}[%p2], %1";
     default:
	gcc_unreachable ();
     }
  }
}

static const char *
output_1614 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 877 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
   int elt = ENDIAN_LANE_N (2, exact_log2 (INTVAL (operands[2])));
   operands[2] = GEN_INT ((HOST_WIDE_INT) 1 << elt);
   switch (which_alternative)
     {
     case 0:
	return "ins\t%0.s[%p2], %1.s[0]";
     case 1:
	return "ins\t%0.s[%p2], %w1";
     case 2:
        return "ld1\t{%0.s}[%p2], %1";
     default:
	gcc_unreachable ();
     }
  }
}

static const char *
output_1615 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 877 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
   int elt = ENDIAN_LANE_N (4, exact_log2 (INTVAL (operands[2])));
   operands[2] = GEN_INT ((HOST_WIDE_INT) 1 << elt);
   switch (which_alternative)
     {
     case 0:
	return "ins\t%0.s[%p2], %1.s[0]";
     case 1:
	return "ins\t%0.s[%p2], %w1";
     case 2:
        return "ld1\t{%0.s}[%p2], %1";
     default:
	gcc_unreachable ();
     }
  }
}

static const char *
output_1616 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 877 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
   int elt = ENDIAN_LANE_N (2, exact_log2 (INTVAL (operands[2])));
   operands[2] = GEN_INT ((HOST_WIDE_INT) 1 << elt);
   switch (which_alternative)
     {
     case 0:
	return "ins\t%0.d[%p2], %1.d[0]";
     case 1:
	return "ins\t%0.d[%p2], %x1";
     case 2:
        return "ld1\t{%0.d}[%p2], %1";
     default:
	gcc_unreachable ();
     }
  }
}

static const char *
output_1617 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 906 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    int elt = ENDIAN_LANE_N (8, exact_log2 (INTVAL (operands[2])));
    operands[2] = GEN_INT (HOST_WIDE_INT_1 << elt);
    operands[4] = aarch64_endian_lane_rtx (V8QImode, INTVAL (operands[4]));

    return "ins\t%0.b[%p2], %3.b[%4]";
  }
}

static const char *
output_1618 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 906 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    int elt = ENDIAN_LANE_N (16, exact_log2 (INTVAL (operands[2])));
    operands[2] = GEN_INT (HOST_WIDE_INT_1 << elt);
    operands[4] = aarch64_endian_lane_rtx (V16QImode, INTVAL (operands[4]));

    return "ins\t%0.b[%p2], %3.b[%4]";
  }
}

static const char *
output_1619 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 906 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    int elt = ENDIAN_LANE_N (4, exact_log2 (INTVAL (operands[2])));
    operands[2] = GEN_INT (HOST_WIDE_INT_1 << elt);
    operands[4] = aarch64_endian_lane_rtx (V4HImode, INTVAL (operands[4]));

    return "ins\t%0.h[%p2], %3.h[%4]";
  }
}

static const char *
output_1620 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 906 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    int elt = ENDIAN_LANE_N (8, exact_log2 (INTVAL (operands[2])));
    operands[2] = GEN_INT (HOST_WIDE_INT_1 << elt);
    operands[4] = aarch64_endian_lane_rtx (V8HImode, INTVAL (operands[4]));

    return "ins\t%0.h[%p2], %3.h[%4]";
  }
}

static const char *
output_1621 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 906 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    int elt = ENDIAN_LANE_N (2, exact_log2 (INTVAL (operands[2])));
    operands[2] = GEN_INT (HOST_WIDE_INT_1 << elt);
    operands[4] = aarch64_endian_lane_rtx (V2SImode, INTVAL (operands[4]));

    return "ins\t%0.s[%p2], %3.s[%4]";
  }
}

static const char *
output_1622 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 906 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    int elt = ENDIAN_LANE_N (4, exact_log2 (INTVAL (operands[2])));
    operands[2] = GEN_INT (HOST_WIDE_INT_1 << elt);
    operands[4] = aarch64_endian_lane_rtx (V4SImode, INTVAL (operands[4]));

    return "ins\t%0.s[%p2], %3.s[%4]";
  }
}

static const char *
output_1623 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 906 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    int elt = ENDIAN_LANE_N (2, exact_log2 (INTVAL (operands[2])));
    operands[2] = GEN_INT (HOST_WIDE_INT_1 << elt);
    operands[4] = aarch64_endian_lane_rtx (V2DImode, INTVAL (operands[4]));

    return "ins\t%0.d[%p2], %3.d[%4]";
  }
}

static const char *
output_1624 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 906 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    int elt = ENDIAN_LANE_N (4, exact_log2 (INTVAL (operands[2])));
    operands[2] = GEN_INT (HOST_WIDE_INT_1 << elt);
    operands[4] = aarch64_endian_lane_rtx (V4HFmode, INTVAL (operands[4]));

    return "ins\t%0.h[%p2], %3.h[%4]";
  }
}

static const char *
output_1625 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 906 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    int elt = ENDIAN_LANE_N (8, exact_log2 (INTVAL (operands[2])));
    operands[2] = GEN_INT (HOST_WIDE_INT_1 << elt);
    operands[4] = aarch64_endian_lane_rtx (V8HFmode, INTVAL (operands[4]));

    return "ins\t%0.h[%p2], %3.h[%4]";
  }
}

static const char *
output_1626 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 906 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    int elt = ENDIAN_LANE_N (2, exact_log2 (INTVAL (operands[2])));
    operands[2] = GEN_INT (HOST_WIDE_INT_1 << elt);
    operands[4] = aarch64_endian_lane_rtx (V2SFmode, INTVAL (operands[4]));

    return "ins\t%0.s[%p2], %3.s[%4]";
  }
}

static const char *
output_1627 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 906 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    int elt = ENDIAN_LANE_N (4, exact_log2 (INTVAL (operands[2])));
    operands[2] = GEN_INT (HOST_WIDE_INT_1 << elt);
    operands[4] = aarch64_endian_lane_rtx (V4SFmode, INTVAL (operands[4]));

    return "ins\t%0.s[%p2], %3.s[%4]";
  }
}

static const char *
output_1628 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 906 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    int elt = ENDIAN_LANE_N (2, exact_log2 (INTVAL (operands[2])));
    operands[2] = GEN_INT (HOST_WIDE_INT_1 << elt);
    operands[4] = aarch64_endian_lane_rtx (V2DFmode, INTVAL (operands[4]));

    return "ins\t%0.d[%p2], %3.d[%4]";
  }
}

static const char *
output_1629 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 927 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    int elt = ENDIAN_LANE_N (8, exact_log2 (INTVAL (operands[2])));
    operands[2] = GEN_INT (HOST_WIDE_INT_1 << elt);
    operands[4] = aarch64_endian_lane_rtx (V16QImode,
					   INTVAL (operands[4]));

    return "ins\t%0.b[%p2], %3.b[%4]";
  }
}

static const char *
output_1630 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 927 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    int elt = ENDIAN_LANE_N (16, exact_log2 (INTVAL (operands[2])));
    operands[2] = GEN_INT (HOST_WIDE_INT_1 << elt);
    operands[4] = aarch64_endian_lane_rtx (V8QImode,
					   INTVAL (operands[4]));

    return "ins\t%0.b[%p2], %3.b[%4]";
  }
}

static const char *
output_1631 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 927 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    int elt = ENDIAN_LANE_N (4, exact_log2 (INTVAL (operands[2])));
    operands[2] = GEN_INT (HOST_WIDE_INT_1 << elt);
    operands[4] = aarch64_endian_lane_rtx (V8HImode,
					   INTVAL (operands[4]));

    return "ins\t%0.h[%p2], %3.h[%4]";
  }
}

static const char *
output_1632 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 927 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    int elt = ENDIAN_LANE_N (8, exact_log2 (INTVAL (operands[2])));
    operands[2] = GEN_INT (HOST_WIDE_INT_1 << elt);
    operands[4] = aarch64_endian_lane_rtx (V4HImode,
					   INTVAL (operands[4]));

    return "ins\t%0.h[%p2], %3.h[%4]";
  }
}

static const char *
output_1633 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 927 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    int elt = ENDIAN_LANE_N (2, exact_log2 (INTVAL (operands[2])));
    operands[2] = GEN_INT (HOST_WIDE_INT_1 << elt);
    operands[4] = aarch64_endian_lane_rtx (V4SImode,
					   INTVAL (operands[4]));

    return "ins\t%0.s[%p2], %3.s[%4]";
  }
}

static const char *
output_1634 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 927 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    int elt = ENDIAN_LANE_N (4, exact_log2 (INTVAL (operands[2])));
    operands[2] = GEN_INT (HOST_WIDE_INT_1 << elt);
    operands[4] = aarch64_endian_lane_rtx (V2SImode,
					   INTVAL (operands[4]));

    return "ins\t%0.s[%p2], %3.s[%4]";
  }
}

static const char *
output_1635 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 927 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    int elt = ENDIAN_LANE_N (4, exact_log2 (INTVAL (operands[2])));
    operands[2] = GEN_INT (HOST_WIDE_INT_1 << elt);
    operands[4] = aarch64_endian_lane_rtx (V8HFmode,
					   INTVAL (operands[4]));

    return "ins\t%0.h[%p2], %3.h[%4]";
  }
}

static const char *
output_1636 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 927 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    int elt = ENDIAN_LANE_N (8, exact_log2 (INTVAL (operands[2])));
    operands[2] = GEN_INT (HOST_WIDE_INT_1 << elt);
    operands[4] = aarch64_endian_lane_rtx (V4HFmode,
					   INTVAL (operands[4]));

    return "ins\t%0.h[%p2], %3.h[%4]";
  }
}

static const char *
output_1637 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 927 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    int elt = ENDIAN_LANE_N (2, exact_log2 (INTVAL (operands[2])));
    operands[2] = GEN_INT (HOST_WIDE_INT_1 << elt);
    operands[4] = aarch64_endian_lane_rtx (V4SFmode,
					   INTVAL (operands[4]));

    return "ins\t%0.s[%p2], %3.s[%4]";
  }
}

static const char *
output_1638 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 927 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    int elt = ENDIAN_LANE_N (4, exact_log2 (INTVAL (operands[2])));
    operands[2] = GEN_INT (HOST_WIDE_INT_1 << elt);
    operands[4] = aarch64_endian_lane_rtx (V2SFmode,
					   INTVAL (operands[4]));

    return "ins\t%0.s[%p2], %3.s[%4]";
  }
}

static const char *
output_1681 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 1213 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    if (BYTES_BIG_ENDIAN)
      return "shl %d0, %d1, %2";
    else
      return "ushr %d0, %d1, %2";
  }
}

static const char *
output_1682 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 1213 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    if (BYTES_BIG_ENDIAN)
      return "shl %d0, %d1, %2";
    else
      return "ushr %d0, %d1, %2";
  }
}

static const char *
output_1683 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 1213 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    if (BYTES_BIG_ENDIAN)
      return "shl %d0, %d1, %2";
    else
      return "ushr %d0, %d1, %2";
  }
}

static const char *
output_1684 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 1213 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    if (BYTES_BIG_ENDIAN)
      return "shl %d0, %d1, %2";
    else
      return "ushr %d0, %d1, %2";
  }
}

static const char *
output_1685 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 1213 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    if (BYTES_BIG_ENDIAN)
      return "shl %d0, %d1, %2";
    else
      return "ushr %d0, %d1, %2";
  }
}

static const char *
output_1692 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 1258 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4HImode, INTVAL (operands[2]));
    return "mla\t%0.4h, %3.4h, %1.4h[%2]";
  }
}

static const char *
output_1693 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 1258 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V8HImode, INTVAL (operands[2]));
    return "mla\t%0.8h, %3.8h, %1.8h[%2]";
  }
}

static const char *
output_1694 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 1258 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V2SImode, INTVAL (operands[2]));
    return "mla\t%0.2s, %3.2s, %1.2s[%2]";
  }
}

static const char *
output_1695 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 1258 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4SImode, INTVAL (operands[2]));
    return "mla\t%0.4s, %3.4s, %1.4s[%2]";
  }
}

static const char *
output_1696 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 1276 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V8HImode, INTVAL (operands[2]));
    return "mla\t%0.4h, %3.4h, %1.4h[%2]";
  }
}

static const char *
output_1697 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 1276 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4HImode, INTVAL (operands[2]));
    return "mla\t%0.8h, %3.8h, %1.8h[%2]";
  }
}

static const char *
output_1698 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 1276 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4SImode, INTVAL (operands[2]));
    return "mla\t%0.2s, %3.2s, %1.2s[%2]";
  }
}

static const char *
output_1699 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 1276 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V2SImode, INTVAL (operands[2]));
    return "mla\t%0.4s, %3.4s, %1.4s[%2]";
  }
}

static const char *
output_1710 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 1316 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4HImode, INTVAL (operands[2]));
    return "mls\t%0.4h, %3.4h, %1.4h[%2]";
  }
}

static const char *
output_1711 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 1316 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V8HImode, INTVAL (operands[2]));
    return "mls\t%0.8h, %3.8h, %1.8h[%2]";
  }
}

static const char *
output_1712 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 1316 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V2SImode, INTVAL (operands[2]));
    return "mls\t%0.2s, %3.2s, %1.2s[%2]";
  }
}

static const char *
output_1713 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 1316 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4SImode, INTVAL (operands[2]));
    return "mls\t%0.4s, %3.4s, %1.4s[%2]";
  }
}

static const char *
output_1714 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 1334 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V8HImode, INTVAL (operands[2]));
    return "mls\t%0.4h, %3.4h, %1.4h[%2]";
  }
}

static const char *
output_1715 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 1334 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4HImode, INTVAL (operands[2]));
    return "mls\t%0.8h, %3.8h, %1.8h[%2]";
  }
}

static const char *
output_1716 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 1334 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4SImode, INTVAL (operands[2]));
    return "mls\t%0.2s, %3.2s, %1.2s[%2]";
  }
}

static const char *
output_1717 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 1334 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V2SImode, INTVAL (operands[2]));
    return "mls\t%0.4s, %3.4s, %1.4s[%2]";
  }
}

static const char * const output_1790[] = {
  "dup\t%d0, %1.d[0]",
  "fmov\t%d0, %1",
  "dup\t%d0, %1",
};

static const char * const output_1791[] = {
  "dup\t%d0, %1.d[0]",
  "fmov\t%d0, %1",
  "dup\t%d0, %1",
};

static const char * const output_1792[] = {
  "dup\t%d0, %1.d[0]",
  "fmov\t%d0, %1",
  "dup\t%d0, %1",
};

static const char * const output_1793[] = {
  "dup\t%d0, %1.d[0]",
  "fmov\t%d0, %1",
  "dup\t%d0, %1",
};

static const char * const output_1794[] = {
  "dup\t%d0, %1.d[0]",
  "fmov\t%d0, %1",
  "dup\t%d0, %1",
};

static const char * const output_1795[] = {
  "dup\t%d0, %1.d[0]",
  "fmov\t%d0, %1",
  "dup\t%d0, %1",
};

static const char * const output_1796[] = {
  "dup\t%d0, %1.d[0]",
  "fmov\t%d0, %1",
  "dup\t%d0, %1",
};

static const char * const output_1797[] = {
  "dup\t%d0, %1.d[0]",
  "fmov\t%d0, %1",
  "dup\t%d0, %1",
};

static const char * const output_1798[] = {
  "dup\t%d0, %1.d[0]",
  "fmov\t%d0, %1",
  "dup\t%d0, %1",
};

static const char * const output_1799[] = {
  "dup\t%d0, %1.d[0]",
  "fmov\t%d0, %1",
  "dup\t%d0, %1",
};

static const char * const output_1800[] = {
  "dup\t%d0, %1.d[0]",
  "fmov\t%d0, %1",
  "dup\t%d0, %1",
};

static const char * const output_1801[] = {
  "dup\t%d0, %1.d[0]",
  "fmov\t%d0, %1",
  "dup\t%d0, %1",
};

static const char * const output_1802[] = {
  "dup\t%d0, %1.d[0]",
  "fmov\t%d0, %1",
  "dup\t%d0, %1",
};

static const char * const output_1803[] = {
  "dup\t%d0, %1.d[0]",
  "fmov\t%d0, %1",
  "dup\t%d0, %1",
};

static const char * const output_1804[] = {
  "ins\t%0.d[1], %1.d[0]",
  "ins\t%0.d[1], %1",
};

static const char * const output_1805[] = {
  "ins\t%0.d[1], %1.d[0]",
  "ins\t%0.d[1], %1",
};

static const char * const output_1806[] = {
  "ins\t%0.d[1], %1.d[0]",
  "ins\t%0.d[1], %1",
};

static const char * const output_1807[] = {
  "ins\t%0.d[1], %1.d[0]",
  "ins\t%0.d[1], %1",
};

static const char * const output_1808[] = {
  "ins\t%0.d[1], %1.d[0]",
  "ins\t%0.d[1], %1",
};

static const char * const output_1809[] = {
  "ins\t%0.d[1], %1.d[0]",
  "ins\t%0.d[1], %1",
};

static const char * const output_1810[] = {
  "ins\t%0.d[1], %1.d[0]",
  "ins\t%0.d[1], %1",
};

static const char * const output_1811[] = {
  "ins\t%0.d[1], %1.d[0]",
  "ins\t%0.d[1], %1",
};

static const char * const output_1812[] = {
  "ins\t%0.d[1], %1.d[0]",
  "ins\t%0.d[1], %1",
};

static const char * const output_1813[] = {
  "ins\t%0.d[1], %1.d[0]",
  "ins\t%0.d[1], %1",
};

static const char * const output_1814[] = {
  "ins\t%0.d[1], %1.d[0]",
  "ins\t%0.d[1], %1",
};

static const char * const output_1815[] = {
  "ins\t%0.d[1], %1.d[0]",
  "ins\t%0.d[1], %1",
};

static const char * const output_1816[] = {
  "ins\t%0.d[1], %1.d[0]",
  "ins\t%0.d[1], %1",
};

static const char * const output_1817[] = {
  "ins\t%0.d[1], %1.d[0]",
  "ins\t%0.d[1], %1",
};

static const char *
output_1821 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 1584 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
   if (BYTES_BIG_ENDIAN)
     return "xtn\t%0.8b, %2.8h\n\txtn2\t%0.16b, %1.8h";
   else
     return "xtn\t%0.8b, %1.8h\n\txtn2\t%0.16b, %2.8h";
 }
}

static const char *
output_1822 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 1584 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
   if (BYTES_BIG_ENDIAN)
     return "xtn\t%0.4h, %2.4s\n\txtn2\t%0.8h, %1.4s";
   else
     return "xtn\t%0.4h, %1.4s\n\txtn2\t%0.8h, %2.4s";
 }
}

static const char *
output_1823 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 1584 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
   if (BYTES_BIG_ENDIAN)
     return "xtn\t%0.2s, %2.2d\n\txtn2\t%0.4s, %1.2d";
   else
     return "xtn\t%0.2s, %1.2d\n\txtn2\t%0.4s, %2.2d";
 }
}

static const char *
output_1919 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 1900 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V2SFmode, INTVAL (operands[2]));
    return "fmla\t%0.2s, %3.2s, %1.2s[%2]";
  }
}

static const char *
output_1920 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 1900 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4SFmode, INTVAL (operands[2]));
    return "fmla\t%0.4s, %3.4s, %1.4s[%2]";
  }
}

static const char *
output_1921 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 1900 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V2DFmode, INTVAL (operands[2]));
    return "fmla\t%0.2d, %3.2d, %1.2d[%2]";
  }
}

static const char *
output_1922 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 1917 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4SFmode, INTVAL (operands[2]));
    return "fmla\t%0.2s, %3.2s, %1.2s[%2]";
  }
}

static const char *
output_1923 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 1917 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V2SFmode, INTVAL (operands[2]));
    return "fmla\t%0.4s, %3.4s, %1.4s[%2]";
  }
}

static const char *
output_1933 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 1945 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V2DFmode, INTVAL (operands[2]));
    return "fmla\t%0.2d, %3.2d, %1.2d[%2]";
  }
}

static const char *
output_1939 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 1974 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V2SFmode, INTVAL (operands[2]));
    return "fmls\t%0.2s, %3.2s, %1.2s[%2]";
  }
}

static const char *
output_1940 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 1974 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4SFmode, INTVAL (operands[2]));
    return "fmls\t%0.4s, %3.4s, %1.4s[%2]";
  }
}

static const char *
output_1941 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 1974 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V2DFmode, INTVAL (operands[2]));
    return "fmls\t%0.2d, %3.2d, %1.2d[%2]";
  }
}

static const char *
output_1942 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 1992 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4SFmode, INTVAL (operands[2]));
    return "fmls\t%0.2s, %3.2s, %1.2s[%2]";
  }
}

static const char *
output_1943 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 1992 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V2SFmode, INTVAL (operands[2]));
    return "fmls\t%0.4s, %3.4s, %1.4s[%2]";
  }
}

static const char *
output_1953 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 2022 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V2DFmode, INTVAL (operands[2]));
    return "fmls\t%0.2d, %3.2d, %1.2d[%2]";
  }
}

static const char *
output_2053 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 2088 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    int fbits = aarch64_vec_fpconst_pow_of_2 (operands[2]);
    char buf[64];
    snprintf (buf, 64, "fcvtzs\t%%0.2s, %%1.2s, #%d", fbits);
    output_asm_insn (buf, operands);
    return "";
  }
}

static const char *
output_2054 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 2088 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    int fbits = aarch64_vec_fpconst_pow_of_2 (operands[2]);
    char buf[64];
    snprintf (buf, 64, "fcvtzu\t%%0.2s, %%1.2s, #%d", fbits);
    output_asm_insn (buf, operands);
    return "";
  }
}

static const char *
output_2055 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 2088 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    int fbits = aarch64_vec_fpconst_pow_of_2 (operands[2]);
    char buf[64];
    snprintf (buf, 64, "fcvtzs\t%%0.4s, %%1.4s, #%d", fbits);
    output_asm_insn (buf, operands);
    return "";
  }
}

static const char *
output_2056 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 2088 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    int fbits = aarch64_vec_fpconst_pow_of_2 (operands[2]);
    char buf[64];
    snprintf (buf, 64, "fcvtzu\t%%0.4s, %%1.4s, #%d", fbits);
    output_asm_insn (buf, operands);
    return "";
  }
}

static const char *
output_2057 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 2088 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    int fbits = aarch64_vec_fpconst_pow_of_2 (operands[2]);
    char buf[64];
    snprintf (buf, 64, "fcvtzs\t%%0.2d, %%1.2d, #%d", fbits);
    output_asm_insn (buf, operands);
    return "";
  }
}

static const char *
output_2058 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 2088 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    int fbits = aarch64_vec_fpconst_pow_of_2 (operands[2]);
    char buf[64];
    snprintf (buf, 64, "fcvtzu\t%%0.2d, %%1.2d, #%d", fbits);
    output_asm_insn (buf, operands);
    return "";
  }
}

static const char * const output_2203[] = {
  "bsl\t%0.8b, %2.8b, %3.8b",
  "bit\t%0.8b, %2.8b, %1.8b",
  "bif\t%0.8b, %3.8b, %1.8b",
};

static const char * const output_2204[] = {
  "bsl\t%0.16b, %2.16b, %3.16b",
  "bit\t%0.16b, %2.16b, %1.16b",
  "bif\t%0.16b, %3.16b, %1.16b",
};

static const char * const output_2205[] = {
  "bsl\t%0.8b, %2.8b, %3.8b",
  "bit\t%0.8b, %2.8b, %1.8b",
  "bif\t%0.8b, %3.8b, %1.8b",
};

static const char * const output_2206[] = {
  "bsl\t%0.16b, %2.16b, %3.16b",
  "bit\t%0.16b, %2.16b, %1.16b",
  "bif\t%0.16b, %3.16b, %1.16b",
};

static const char * const output_2207[] = {
  "bsl\t%0.8b, %2.8b, %3.8b",
  "bit\t%0.8b, %2.8b, %1.8b",
  "bif\t%0.8b, %3.8b, %1.8b",
};

static const char * const output_2208[] = {
  "bsl\t%0.16b, %2.16b, %3.16b",
  "bit\t%0.16b, %2.16b, %1.16b",
  "bif\t%0.16b, %3.16b, %1.16b",
};

static const char * const output_2209[] = {
  "bsl\t%0.16b, %2.16b, %3.16b",
  "bit\t%0.16b, %2.16b, %1.16b",
  "bif\t%0.16b, %3.16b, %1.16b",
};

static const char * const output_2210[] = {
  "bsl\t%0.8b, %3.8b, %2.8b",
  "bit\t%0.8b, %3.8b, %1.8b",
  "bif\t%0.8b, %2.8b, %1.8b",
};

static const char * const output_2211[] = {
  "bsl\t%0.16b, %3.16b, %2.16b",
  "bit\t%0.16b, %3.16b, %1.16b",
  "bif\t%0.16b, %2.16b, %1.16b",
};

static const char * const output_2212[] = {
  "bsl\t%0.8b, %3.8b, %2.8b",
  "bit\t%0.8b, %3.8b, %1.8b",
  "bif\t%0.8b, %2.8b, %1.8b",
};

static const char * const output_2213[] = {
  "bsl\t%0.16b, %3.16b, %2.16b",
  "bit\t%0.16b, %3.16b, %1.16b",
  "bif\t%0.16b, %2.16b, %1.16b",
};

static const char * const output_2214[] = {
  "bsl\t%0.8b, %3.8b, %2.8b",
  "bit\t%0.8b, %3.8b, %1.8b",
  "bif\t%0.8b, %2.8b, %1.8b",
};

static const char * const output_2215[] = {
  "bsl\t%0.16b, %3.16b, %2.16b",
  "bit\t%0.16b, %3.16b, %1.16b",
  "bif\t%0.16b, %2.16b, %1.16b",
};

static const char * const output_2216[] = {
  "bsl\t%0.16b, %3.16b, %2.16b",
  "bit\t%0.16b, %3.16b, %1.16b",
  "bif\t%0.16b, %2.16b, %1.16b",
};

static const char * const output_2217[] = {
  "bsl\t%0.8b, %2.8b, %3.8b",
  "bit\t%0.8b, %2.8b, %1.8b",
  "bif\t%0.8b, %3.8b, %1.8b",
  "#",
};

static const char * const output_2218[] = {
  "bsl\t%0.8b, %3.8b, %2.8b",
  "bit\t%0.8b, %3.8b, %1.8b",
  "bif\t%0.8b, %2.8b, %1.8b",
  "#",
};

static const char *
output_2219 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3109 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V8QImode, INTVAL (operands[2]));
    return "smov\t%w0, %1.b[%2]";
  }
}

static const char *
output_2220 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3109 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V8QImode, INTVAL (operands[2]));
    return "smov\t%x0, %1.b[%2]";
  }
}

static const char *
output_2221 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3109 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V16QImode, INTVAL (operands[2]));
    return "smov\t%w0, %1.b[%2]";
  }
}

static const char *
output_2222 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3109 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V16QImode, INTVAL (operands[2]));
    return "smov\t%x0, %1.b[%2]";
  }
}

static const char *
output_2223 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3109 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4HImode, INTVAL (operands[2]));
    return "smov\t%w0, %1.h[%2]";
  }
}

static const char *
output_2224 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3109 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4HImode, INTVAL (operands[2]));
    return "smov\t%x0, %1.h[%2]";
  }
}

static const char *
output_2225 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3109 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V8HImode, INTVAL (operands[2]));
    return "smov\t%w0, %1.h[%2]";
  }
}

static const char *
output_2226 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3109 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V8HImode, INTVAL (operands[2]));
    return "smov\t%x0, %1.h[%2]";
  }
}

static const char *
output_2227 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3123 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V8QImode,
					   INTVAL (operands[2]));
    return "umov\t%w0, %1.b[%2]";
  }
}

static const char *
output_2228 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3123 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V8QImode,
					   INTVAL (operands[2]));
    return "umov\t%w0, %1.b[%2]";
  }
}

static const char *
output_2229 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3123 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V16QImode,
					   INTVAL (operands[2]));
    return "umov\t%w0, %1.b[%2]";
  }
}

static const char *
output_2230 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3123 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V16QImode,
					   INTVAL (operands[2]));
    return "umov\t%w0, %1.b[%2]";
  }
}

static const char *
output_2231 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3123 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4HImode,
					   INTVAL (operands[2]));
    return "umov\t%w0, %1.h[%2]";
  }
}

static const char *
output_2232 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3123 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4HImode,
					   INTVAL (operands[2]));
    return "umov\t%w0, %1.h[%2]";
  }
}

static const char *
output_2233 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3123 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V8HImode,
					   INTVAL (operands[2]));
    return "umov\t%w0, %1.h[%2]";
  }
}

static const char *
output_2234 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3123 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V8HImode,
					   INTVAL (operands[2]));
    return "umov\t%w0, %1.h[%2]";
  }
}

static const char *
output_2235 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3140 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V8QImode, INTVAL (operands[2]));
    switch (which_alternative)
      {
	case 0:
	  return "umov\t%w0, %1.b[%2]";
	case 1:
	  return "dup\t%b0, %1.b[%2]";
	case 2:
	  return "st1\t{%1.b}[%2], %0";
	default:
	  gcc_unreachable ();
      }
  }
}

static const char *
output_2236 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3140 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V16QImode, INTVAL (operands[2]));
    switch (which_alternative)
      {
	case 0:
	  return "umov\t%w0, %1.b[%2]";
	case 1:
	  return "dup\t%b0, %1.b[%2]";
	case 2:
	  return "st1\t{%1.b}[%2], %0";
	default:
	  gcc_unreachable ();
      }
  }
}

static const char *
output_2237 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3140 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4HImode, INTVAL (operands[2]));
    switch (which_alternative)
      {
	case 0:
	  return "umov\t%w0, %1.h[%2]";
	case 1:
	  return "dup\t%h0, %1.h[%2]";
	case 2:
	  return "st1\t{%1.h}[%2], %0";
	default:
	  gcc_unreachable ();
      }
  }
}

static const char *
output_2238 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3140 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V8HImode, INTVAL (operands[2]));
    switch (which_alternative)
      {
	case 0:
	  return "umov\t%w0, %1.h[%2]";
	case 1:
	  return "dup\t%h0, %1.h[%2]";
	case 2:
	  return "st1\t{%1.h}[%2], %0";
	default:
	  gcc_unreachable ();
      }
  }
}

static const char *
output_2239 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3140 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V2SImode, INTVAL (operands[2]));
    switch (which_alternative)
      {
	case 0:
	  return "umov\t%w0, %1.s[%2]";
	case 1:
	  return "dup\t%s0, %1.s[%2]";
	case 2:
	  return "st1\t{%1.s}[%2], %0";
	default:
	  gcc_unreachable ();
      }
  }
}

static const char *
output_2240 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3140 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4SImode, INTVAL (operands[2]));
    switch (which_alternative)
      {
	case 0:
	  return "umov\t%w0, %1.s[%2]";
	case 1:
	  return "dup\t%s0, %1.s[%2]";
	case 2:
	  return "st1\t{%1.s}[%2], %0";
	default:
	  gcc_unreachable ();
      }
  }
}

static const char *
output_2241 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3140 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V2DImode, INTVAL (operands[2]));
    switch (which_alternative)
      {
	case 0:
	  return "umov\t%x0, %1.d[%2]";
	case 1:
	  return "dup\t%d0, %1.d[%2]";
	case 2:
	  return "st1\t{%1.d}[%2], %0";
	default:
	  gcc_unreachable ();
      }
  }
}

static const char *
output_2242 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3140 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4HFmode, INTVAL (operands[2]));
    switch (which_alternative)
      {
	case 0:
	  return "umov\t%w0, %1.h[%2]";
	case 1:
	  return "dup\t%h0, %1.h[%2]";
	case 2:
	  return "st1\t{%1.h}[%2], %0";
	default:
	  gcc_unreachable ();
      }
  }
}

static const char *
output_2243 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3140 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V8HFmode, INTVAL (operands[2]));
    switch (which_alternative)
      {
	case 0:
	  return "umov\t%w0, %1.h[%2]";
	case 1:
	  return "dup\t%h0, %1.h[%2]";
	case 2:
	  return "st1\t{%1.h}[%2], %0";
	default:
	  gcc_unreachable ();
      }
  }
}

static const char *
output_2244 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3140 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V2SFmode, INTVAL (operands[2]));
    switch (which_alternative)
      {
	case 0:
	  return "umov\t%w0, %1.s[%2]";
	case 1:
	  return "dup\t%s0, %1.s[%2]";
	case 2:
	  return "st1\t{%1.s}[%2], %0";
	default:
	  gcc_unreachable ();
      }
  }
}

static const char *
output_2245 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3140 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4SFmode, INTVAL (operands[2]));
    switch (which_alternative)
      {
	case 0:
	  return "umov\t%w0, %1.s[%2]";
	case 1:
	  return "dup\t%s0, %1.s[%2]";
	case 2:
	  return "st1\t{%1.s}[%2], %0";
	default:
	  gcc_unreachable ();
      }
  }
}

static const char *
output_2246 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3140 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V2DFmode, INTVAL (operands[2]));
    switch (which_alternative)
      {
	case 0:
	  return "umov\t%x0, %1.d[%2]";
	case 1:
	  return "dup\t%d0, %1.d[%2]";
	case 2:
	  return "st1\t{%1.d}[%2], %0";
	default:
	  gcc_unreachable ();
      }
  }
}

static const char * const output_2254[] = {
  "stp\t%d1, %d2, %y0",
  "stp\t%x1, %x2, %y0",
};

static const char * const output_2255[] = {
  "stp\t%d1, %d2, %y0",
  "stp\t%x1, %x2, %y0",
};

static const char * const output_2256[] = {
  "stp\t%d1, %d2, %y0",
  "stp\t%x1, %x2, %y0",
};

static const char * const output_2257[] = {
  "stp\t%d1, %d2, %y0",
  "stp\t%x1, %x2, %y0",
};

static const char * const output_2258[] = {
  "stp\t%d1, %d2, %y0",
  "stp\t%x1, %x2, %y0",
};

static const char * const output_2259[] = {
  "stp\t%d1, %d2, %y0",
  "stp\t%x1, %x2, %y0",
};

static const char * const output_2260[] = {
  "stp\t%d1, %d2, %y0",
  "stp\t%x1, %x2, %y0",
};

static const char * const output_2261[] = {
  "mov\t%0.8b, %1.8b",
  "fmov\t%d0, %1",
  "ldr\t%d0, %1",
};

static const char * const output_2262[] = {
  "mov\t%0.8b, %1.8b",
  "fmov\t%d0, %1",
  "ldr\t%d0, %1",
};

static const char * const output_2263[] = {
  "mov\t%0.8b, %1.8b",
  "fmov\t%d0, %1",
  "ldr\t%d0, %1",
};

static const char * const output_2264[] = {
  "mov\t%0.8b, %1.8b",
  "fmov\t%d0, %1",
  "ldr\t%d0, %1",
};

static const char * const output_2265[] = {
  "mov\t%0.8b, %1.8b",
  "fmov\t%d0, %1",
  "ldr\t%d0, %1",
};

static const char * const output_2266[] = {
  "mov\t%0.8b, %1.8b",
  "fmov\t%d0, %1",
  "ldr\t%d0, %1",
};

static const char * const output_2267[] = {
  "mov\t%0.8b, %1.8b",
  "fmov\t%d0, %1",
  "ldr\t%d0, %1",
};

static const char * const output_2268[] = {
  "mov\t%0.8b, %1.8b",
  "fmov\t%d0, %1",
  "ldr\t%d0, %1",
};

static const char * const output_2269[] = {
  "mov\t%0.8b, %1.8b",
  "fmov\t%d0, %1",
  "ldr\t%d0, %1",
};

static const char * const output_2270[] = {
  "mov\t%0.8b, %1.8b",
  "fmov\t%d0, %1",
  "ldr\t%d0, %1",
};

static const char * const output_2271[] = {
  "mov\t%0.8b, %1.8b",
  "fmov\t%d0, %1",
  "ldr\t%d0, %1",
};

static const char * const output_2272[] = {
  "mov\t%0.8b, %1.8b",
  "fmov\t%d0, %1",
  "ldr\t%d0, %1",
};

static const char * const output_2273[] = {
  "mov\t%0.8b, %1.8b",
  "fmov\t%d0, %1",
  "ldr\t%d0, %1",
};

static const char * const output_2274[] = {
  "mov\t%0.8b, %1.8b",
  "fmov\t%d0, %1",
  "ldr\t%d0, %1",
};

static const char *
output_2429 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3592 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V4SFmode, INTVAL (operands[3]));
    return "fmulx\t%0.2s, %1.2s, %2.s[%3]";
  }
}

static const char *
output_2430 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3592 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V2SFmode, INTVAL (operands[3]));
    return "fmulx\t%0.4s, %1.4s, %2.s[%3]";
  }
}

static const char *
output_2431 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3611 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V2SFmode, INTVAL (operands[3]));
    return "fmulx\t%0.2s, %1.2s, %2.s[%3]";
  }
}

static const char *
output_2432 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3611 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V4SFmode, INTVAL (operands[3]));
    return "fmulx\t%0.4s, %1.4s, %2.s[%3]";
  }
}

static const char *
output_2433 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3611 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V2DFmode, INTVAL (operands[3]));
    return "fmulx\t%0.2d, %1.2d, %2.d[%3]";
  }
}

static const char *
output_2439 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3645 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V2SFmode, INTVAL (operands[3]));
    return "fmulx\t%s0, %s1, %2.s[%3]";
  }
}

static const char *
output_2440 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3645 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V4SFmode, INTVAL (operands[3]));
    return "fmulx\t%s0, %s1, %2.s[%3]";
  }
}

static const char *
output_2441 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3645 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V2DFmode, INTVAL (operands[3]));
    return "fmulx\t%d0, %d1, %2.d[%3]";
  }
}

static const char *
output_2560 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3731 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"

   operands[3] = aarch64_endian_lane_rtx (V4HImode, INTVAL (operands[3]));
   return "sqdmulh\t%0.4h, %1.4h, %2.h[%3]";
}

static const char *
output_2561 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3731 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"

   operands[3] = aarch64_endian_lane_rtx (V4HImode, INTVAL (operands[3]));
   return "sqrdmulh\t%0.4h, %1.4h, %2.h[%3]";
}

static const char *
output_2562 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3731 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"

   operands[3] = aarch64_endian_lane_rtx (V4HImode, INTVAL (operands[3]));
   return "sqdmulh\t%0.8h, %1.8h, %2.h[%3]";
}

static const char *
output_2563 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3731 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"

   operands[3] = aarch64_endian_lane_rtx (V4HImode, INTVAL (operands[3]));
   return "sqrdmulh\t%0.8h, %1.8h, %2.h[%3]";
}

static const char *
output_2564 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3731 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"

   operands[3] = aarch64_endian_lane_rtx (V2SImode, INTVAL (operands[3]));
   return "sqdmulh\t%0.2s, %1.2s, %2.s[%3]";
}

static const char *
output_2565 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3731 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"

   operands[3] = aarch64_endian_lane_rtx (V2SImode, INTVAL (operands[3]));
   return "sqrdmulh\t%0.2s, %1.2s, %2.s[%3]";
}

static const char *
output_2566 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3731 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"

   operands[3] = aarch64_endian_lane_rtx (V2SImode, INTVAL (operands[3]));
   return "sqdmulh\t%0.4s, %1.4s, %2.s[%3]";
}

static const char *
output_2567 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3731 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"

   operands[3] = aarch64_endian_lane_rtx (V2SImode, INTVAL (operands[3]));
   return "sqrdmulh\t%0.4s, %1.4s, %2.s[%3]";
}

static const char *
output_2568 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3746 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"

   operands[3] = aarch64_endian_lane_rtx (V8HImode, INTVAL (operands[3]));
   return "sqdmulh\t%0.4h, %1.4h, %2.h[%3]";
}

static const char *
output_2569 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3746 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"

   operands[3] = aarch64_endian_lane_rtx (V8HImode, INTVAL (operands[3]));
   return "sqrdmulh\t%0.4h, %1.4h, %2.h[%3]";
}

static const char *
output_2570 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3746 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"

   operands[3] = aarch64_endian_lane_rtx (V8HImode, INTVAL (operands[3]));
   return "sqdmulh\t%0.8h, %1.8h, %2.h[%3]";
}

static const char *
output_2571 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3746 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"

   operands[3] = aarch64_endian_lane_rtx (V8HImode, INTVAL (operands[3]));
   return "sqrdmulh\t%0.8h, %1.8h, %2.h[%3]";
}

static const char *
output_2572 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3746 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"

   operands[3] = aarch64_endian_lane_rtx (V4SImode, INTVAL (operands[3]));
   return "sqdmulh\t%0.2s, %1.2s, %2.s[%3]";
}

static const char *
output_2573 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3746 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"

   operands[3] = aarch64_endian_lane_rtx (V4SImode, INTVAL (operands[3]));
   return "sqrdmulh\t%0.2s, %1.2s, %2.s[%3]";
}

static const char *
output_2574 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3746 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"

   operands[3] = aarch64_endian_lane_rtx (V4SImode, INTVAL (operands[3]));
   return "sqdmulh\t%0.4s, %1.4s, %2.s[%3]";
}

static const char *
output_2575 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3746 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"

   operands[3] = aarch64_endian_lane_rtx (V4SImode, INTVAL (operands[3]));
   return "sqrdmulh\t%0.4s, %1.4s, %2.s[%3]";
}

static const char *
output_2576 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3761 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"

   operands[3] = aarch64_endian_lane_rtx (V4HImode, INTVAL (operands[3]));
   return "sqdmulh\t%h0, %h1, %2.h[%3]";
}

static const char *
output_2577 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3761 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"

   operands[3] = aarch64_endian_lane_rtx (V4HImode, INTVAL (operands[3]));
   return "sqrdmulh\t%h0, %h1, %2.h[%3]";
}

static const char *
output_2578 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3761 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"

   operands[3] = aarch64_endian_lane_rtx (V2SImode, INTVAL (operands[3]));
   return "sqdmulh\t%s0, %s1, %2.s[%3]";
}

static const char *
output_2579 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3761 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"

   operands[3] = aarch64_endian_lane_rtx (V2SImode, INTVAL (operands[3]));
   return "sqrdmulh\t%s0, %s1, %2.s[%3]";
}

static const char *
output_2580 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3776 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"

   operands[3] = aarch64_endian_lane_rtx (V8HImode, INTVAL (operands[3]));
   return "sqdmulh\t%h0, %h1, %2.h[%3]";
}

static const char *
output_2581 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3776 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"

   operands[3] = aarch64_endian_lane_rtx (V8HImode, INTVAL (operands[3]));
   return "sqrdmulh\t%h0, %h1, %2.h[%3]";
}

static const char *
output_2582 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3776 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"

   operands[3] = aarch64_endian_lane_rtx (V4SImode, INTVAL (operands[3]));
   return "sqdmulh\t%s0, %s1, %2.s[%3]";
}

static const char *
output_2583 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3776 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"

   operands[3] = aarch64_endian_lane_rtx (V4SImode, INTVAL (operands[3]));
   return "sqrdmulh\t%s0, %s1, %2.s[%3]";
}

static const char *
output_2596 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3808 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
     operands[4] = aarch64_endian_lane_rtx (V4HImode, INTVAL (operands[4]));
     return
      "sqrdmlah\t%0.4h, %2.4h, %3.h[%4]";
   }
}

static const char *
output_2597 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3808 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
     operands[4] = aarch64_endian_lane_rtx (V4HImode, INTVAL (operands[4]));
     return
      "sqrdmlsh\t%0.4h, %2.4h, %3.h[%4]";
   }
}

static const char *
output_2598 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3808 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
     operands[4] = aarch64_endian_lane_rtx (V4HImode, INTVAL (operands[4]));
     return
      "sqrdmlah\t%0.8h, %2.8h, %3.h[%4]";
   }
}

static const char *
output_2599 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3808 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
     operands[4] = aarch64_endian_lane_rtx (V4HImode, INTVAL (operands[4]));
     return
      "sqrdmlsh\t%0.8h, %2.8h, %3.h[%4]";
   }
}

static const char *
output_2600 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3808 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
     operands[4] = aarch64_endian_lane_rtx (V2SImode, INTVAL (operands[4]));
     return
      "sqrdmlah\t%0.2s, %2.2s, %3.s[%4]";
   }
}

static const char *
output_2601 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3808 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
     operands[4] = aarch64_endian_lane_rtx (V2SImode, INTVAL (operands[4]));
     return
      "sqrdmlsh\t%0.2s, %2.2s, %3.s[%4]";
   }
}

static const char *
output_2602 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3808 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
     operands[4] = aarch64_endian_lane_rtx (V2SImode, INTVAL (operands[4]));
     return
      "sqrdmlah\t%0.4s, %2.4s, %3.s[%4]";
   }
}

static const char *
output_2603 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3808 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
     operands[4] = aarch64_endian_lane_rtx (V2SImode, INTVAL (operands[4]));
     return
      "sqrdmlsh\t%0.4s, %2.4s, %3.s[%4]";
   }
}

static const char *
output_2604 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3826 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
     operands[4] = aarch64_endian_lane_rtx (V4HImode, INTVAL (operands[4]));
     return
      "sqrdmlah\t%h0, %h2, %3.h[%4]";
   }
}

static const char *
output_2605 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3826 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
     operands[4] = aarch64_endian_lane_rtx (V4HImode, INTVAL (operands[4]));
     return
      "sqrdmlsh\t%h0, %h2, %3.h[%4]";
   }
}

static const char *
output_2606 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3826 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
     operands[4] = aarch64_endian_lane_rtx (V2SImode, INTVAL (operands[4]));
     return
      "sqrdmlah\t%s0, %s2, %3.s[%4]";
   }
}

static const char *
output_2607 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3826 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
     operands[4] = aarch64_endian_lane_rtx (V2SImode, INTVAL (operands[4]));
     return
      "sqrdmlsh\t%s0, %s2, %3.s[%4]";
   }
}

static const char *
output_2608 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3846 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
     operands[4] = aarch64_endian_lane_rtx (V8HImode, INTVAL (operands[4]));
     return
      "sqrdmlah\t%0.4h, %2.4h, %3.h[%4]";
   }
}

static const char *
output_2609 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3846 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
     operands[4] = aarch64_endian_lane_rtx (V8HImode, INTVAL (operands[4]));
     return
      "sqrdmlsh\t%0.4h, %2.4h, %3.h[%4]";
   }
}

static const char *
output_2610 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3846 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
     operands[4] = aarch64_endian_lane_rtx (V8HImode, INTVAL (operands[4]));
     return
      "sqrdmlah\t%0.8h, %2.8h, %3.h[%4]";
   }
}

static const char *
output_2611 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3846 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
     operands[4] = aarch64_endian_lane_rtx (V8HImode, INTVAL (operands[4]));
     return
      "sqrdmlsh\t%0.8h, %2.8h, %3.h[%4]";
   }
}

static const char *
output_2612 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3846 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
     operands[4] = aarch64_endian_lane_rtx (V4SImode, INTVAL (operands[4]));
     return
      "sqrdmlah\t%0.2s, %2.2s, %3.s[%4]";
   }
}

static const char *
output_2613 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3846 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
     operands[4] = aarch64_endian_lane_rtx (V4SImode, INTVAL (operands[4]));
     return
      "sqrdmlsh\t%0.2s, %2.2s, %3.s[%4]";
   }
}

static const char *
output_2614 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3846 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
     operands[4] = aarch64_endian_lane_rtx (V4SImode, INTVAL (operands[4]));
     return
      "sqrdmlah\t%0.4s, %2.4s, %3.s[%4]";
   }
}

static const char *
output_2615 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3846 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
     operands[4] = aarch64_endian_lane_rtx (V4SImode, INTVAL (operands[4]));
     return
      "sqrdmlsh\t%0.4s, %2.4s, %3.s[%4]";
   }
}

static const char *
output_2616 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3864 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
     operands[4] = aarch64_endian_lane_rtx (V8HImode, INTVAL (operands[4]));
     return
      "sqrdmlah\t%h0, %h2, %3.h[%4]";
   }
}

static const char *
output_2617 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3864 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
     operands[4] = aarch64_endian_lane_rtx (V8HImode, INTVAL (operands[4]));
     return
      "sqrdmlsh\t%h0, %h2, %3.h[%4]";
   }
}

static const char *
output_2618 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3864 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
     operands[4] = aarch64_endian_lane_rtx (V4SImode, INTVAL (operands[4]));
     return
      "sqrdmlah\t%s0, %s2, %3.s[%4]";
   }
}

static const char *
output_2619 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3864 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
     operands[4] = aarch64_endian_lane_rtx (V4SImode, INTVAL (operands[4]));
     return
      "sqrdmlsh\t%s0, %s2, %3.s[%4]";
   }
}

static const char *
output_2628 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3908 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[4] = aarch64_endian_lane_rtx (V4HImode, INTVAL (operands[4]));
    return
      "sqdmlal\t%0.4s, %2.4h, %3.h[%4]";
  }
}

static const char *
output_2629 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3908 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[4] = aarch64_endian_lane_rtx (V4HImode, INTVAL (operands[4]));
    return
      "sqdmlsl\t%0.4s, %2.4h, %3.h[%4]";
  }
}

static const char *
output_2630 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3908 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[4] = aarch64_endian_lane_rtx (V2SImode, INTVAL (operands[4]));
    return
      "sqdmlal\t%0.2d, %2.2s, %3.s[%4]";
  }
}

static const char *
output_2631 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3908 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[4] = aarch64_endian_lane_rtx (V2SImode, INTVAL (operands[4]));
    return
      "sqdmlsl\t%0.2d, %2.2s, %3.s[%4]";
  }
}

static const char *
output_2632 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3932 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[4] = aarch64_endian_lane_rtx (V8HImode, INTVAL (operands[4]));
    return
      "sqdmlal\t%0.4s, %2.4h, %3.h[%4]";
  }
}

static const char *
output_2633 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3932 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[4] = aarch64_endian_lane_rtx (V8HImode, INTVAL (operands[4]));
    return
      "sqdmlsl\t%0.4s, %2.4h, %3.h[%4]";
  }
}

static const char *
output_2634 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3932 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[4] = aarch64_endian_lane_rtx (V4SImode, INTVAL (operands[4]));
    return
      "sqdmlal\t%0.2d, %2.2s, %3.s[%4]";
  }
}

static const char *
output_2635 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3932 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[4] = aarch64_endian_lane_rtx (V4SImode, INTVAL (operands[4]));
    return
      "sqdmlsl\t%0.2d, %2.2s, %3.s[%4]";
  }
}

static const char *
output_2636 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3955 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[4] = aarch64_endian_lane_rtx (V4HImode, INTVAL (operands[4]));
    return
      "sqdmlal\t%s0, %h2, %3.h[%4]";
  }
}

static const char *
output_2637 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3955 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[4] = aarch64_endian_lane_rtx (V4HImode, INTVAL (operands[4]));
    return
      "sqdmlsl\t%s0, %h2, %3.h[%4]";
  }
}

static const char *
output_2638 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3955 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[4] = aarch64_endian_lane_rtx (V2SImode, INTVAL (operands[4]));
    return
      "sqdmlal\t%d0, %s2, %3.s[%4]";
  }
}

static const char *
output_2639 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3955 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[4] = aarch64_endian_lane_rtx (V2SImode, INTVAL (operands[4]));
    return
      "sqdmlsl\t%d0, %s2, %3.s[%4]";
  }
}

static const char *
output_2640 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3978 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[4] = aarch64_endian_lane_rtx (V8HImode, INTVAL (operands[4]));
    return
      "sqdmlal\t%s0, %h2, %3.h[%4]";
  }
}

static const char *
output_2641 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3978 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[4] = aarch64_endian_lane_rtx (V8HImode, INTVAL (operands[4]));
    return
      "sqdmlsl\t%s0, %h2, %3.h[%4]";
  }
}

static const char *
output_2642 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3978 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[4] = aarch64_endian_lane_rtx (V4SImode, INTVAL (operands[4]));
    return
      "sqdmlal\t%d0, %s2, %3.s[%4]";
  }
}

static const char *
output_2643 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 3978 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[4] = aarch64_endian_lane_rtx (V4SImode, INTVAL (operands[4]));
    return
      "sqdmlsl\t%d0, %s2, %3.s[%4]";
  }
}

static const char *
output_2652 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4073 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[4] = aarch64_endian_lane_rtx (V4HImode, INTVAL (operands[4]));
    return
     "sqdmlal2\t%0.4s, %2.8h, %3.h[%4]";
  }
}

static const char *
output_2653 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4073 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[4] = aarch64_endian_lane_rtx (V4HImode, INTVAL (operands[4]));
    return
     "sqdmlsl2\t%0.4s, %2.8h, %3.h[%4]";
  }
}

static const char *
output_2654 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4073 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[4] = aarch64_endian_lane_rtx (V2SImode, INTVAL (operands[4]));
    return
     "sqdmlal2\t%0.2d, %2.4s, %3.s[%4]";
  }
}

static const char *
output_2655 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4073 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[4] = aarch64_endian_lane_rtx (V2SImode, INTVAL (operands[4]));
    return
     "sqdmlsl2\t%0.2d, %2.4s, %3.s[%4]";
  }
}

static const char *
output_2656 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4099 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[4] = aarch64_endian_lane_rtx (V8HImode, INTVAL (operands[4]));
    return
     "sqdmlal2\t%0.4s, %2.8h, %3.h[%4]";
  }
}

static const char *
output_2657 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4099 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[4] = aarch64_endian_lane_rtx (V8HImode, INTVAL (operands[4]));
    return
     "sqdmlsl2\t%0.4s, %2.8h, %3.h[%4]";
  }
}

static const char *
output_2658 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4099 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[4] = aarch64_endian_lane_rtx (V4SImode, INTVAL (operands[4]));
    return
     "sqdmlal2\t%0.2d, %2.4s, %3.s[%4]";
  }
}

static const char *
output_2659 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4099 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[4] = aarch64_endian_lane_rtx (V4SImode, INTVAL (operands[4]));
    return
     "sqdmlsl2\t%0.2d, %2.4s, %3.s[%4]";
  }
}

static const char *
output_2668 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4246 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V4HImode, INTVAL (operands[3]));
    return "sqdmull\t%0.4s, %1.4h, %2.h[%3]";
  }
}

static const char *
output_2669 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4246 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V2SImode, INTVAL (operands[3]));
    return "sqdmull\t%0.2d, %1.2s, %2.s[%3]";
  }
}

static const char *
output_2670 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4267 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V8HImode, INTVAL (operands[3]));
    return "sqdmull\t%0.4s, %1.4h, %2.h[%3]";
  }
}

static const char *
output_2671 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4267 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V4SImode, INTVAL (operands[3]));
    return "sqdmull\t%0.2d, %1.2s, %2.s[%3]";
  }
}

static const char *
output_2672 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4287 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V4HImode, INTVAL (operands[3]));
    return "sqdmull\t%s0, %h1, %2.h[%3]";
  }
}

static const char *
output_2673 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4287 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V2SImode, INTVAL (operands[3]));
    return "sqdmull\t%d0, %s1, %2.s[%3]";
  }
}

static const char *
output_2674 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4307 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V8HImode, INTVAL (operands[3]));
    return "sqdmull\t%s0, %h1, %2.h[%3]";
  }
}

static const char *
output_2675 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4307 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V4SImode, INTVAL (operands[3]));
    return "sqdmull\t%d0, %s1, %2.s[%3]";
  }
}

static const char *
output_2680 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4385 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V4HImode, INTVAL (operands[3]));
    return "sqdmull2\t%0.4s, %1.8h, %2.h[%3]";
  }
}

static const char *
output_2681 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4385 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V2SImode, INTVAL (operands[3]));
    return "sqdmull2\t%0.2d, %1.4s, %2.s[%3]";
  }
}

static const char *
output_2682 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4408 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V8HImode, INTVAL (operands[3]));
    return "sqdmull2\t%0.4s, %1.8h, %2.h[%3]";
  }
}

static const char *
output_2683 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4408 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V4SImode, INTVAL (operands[3]));
    return "sqdmull2\t%0.2d, %1.4s, %2.s[%3]";
  }
}

static const char *
output_2762 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4511 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    if (INTVAL (operands[2]) == GET_MODE_UNIT_BITSIZE (V8QImode))
      return "shll\t%0.8h, %1.8b, %2";
    else
      return "sshll\t%0.8h, %1.8b, %2";
  }
}

static const char *
output_2763 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4511 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    if (INTVAL (operands[2]) == GET_MODE_UNIT_BITSIZE (V8QImode))
      return "shll\t%0.8h, %1.8b, %2";
    else
      return "ushll\t%0.8h, %1.8b, %2";
  }
}

static const char *
output_2764 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4511 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    if (INTVAL (operands[2]) == GET_MODE_UNIT_BITSIZE (V4HImode))
      return "shll\t%0.4s, %1.4h, %2";
    else
      return "sshll\t%0.4s, %1.4h, %2";
  }
}

static const char *
output_2765 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4511 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    if (INTVAL (operands[2]) == GET_MODE_UNIT_BITSIZE (V4HImode))
      return "shll\t%0.4s, %1.4h, %2";
    else
      return "ushll\t%0.4s, %1.4h, %2";
  }
}

static const char *
output_2766 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4511 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    if (INTVAL (operands[2]) == GET_MODE_UNIT_BITSIZE (V2SImode))
      return "shll\t%0.2d, %1.2s, %2";
    else
      return "sshll\t%0.2d, %1.2s, %2";
  }
}

static const char *
output_2767 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4511 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    if (INTVAL (operands[2]) == GET_MODE_UNIT_BITSIZE (V2SImode))
      return "shll\t%0.2d, %1.2s, %2";
    else
      return "ushll\t%0.2d, %1.2s, %2";
  }
}

static const char *
output_2768 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4528 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    if (INTVAL (operands[2]) == GET_MODE_UNIT_BITSIZE (V16QImode))
      return "shll2\t%0.8h, %1.16b, %2";
    else
      return "sshll2\t%0.8h, %1.16b, %2";
  }
}

static const char *
output_2769 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4528 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    if (INTVAL (operands[2]) == GET_MODE_UNIT_BITSIZE (V16QImode))
      return "shll2\t%0.8h, %1.16b, %2";
    else
      return "ushll2\t%0.8h, %1.16b, %2";
  }
}

static const char *
output_2770 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4528 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    if (INTVAL (operands[2]) == GET_MODE_UNIT_BITSIZE (V8HImode))
      return "shll2\t%0.4s, %1.8h, %2";
    else
      return "sshll2\t%0.4s, %1.8h, %2";
  }
}

static const char *
output_2771 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4528 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    if (INTVAL (operands[2]) == GET_MODE_UNIT_BITSIZE (V8HImode))
      return "shll2\t%0.4s, %1.8h, %2";
    else
      return "ushll2\t%0.4s, %1.8h, %2";
  }
}

static const char *
output_2772 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4528 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    if (INTVAL (operands[2]) == GET_MODE_UNIT_BITSIZE (V4SImode))
      return "shll2\t%0.2d, %1.4s, %2";
    else
      return "sshll2\t%0.2d, %1.4s, %2";
  }
}

static const char *
output_2773 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4528 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    if (INTVAL (operands[2]) == GET_MODE_UNIT_BITSIZE (V4SImode))
      return "shll2\t%0.2d, %1.4s, %2";
    else
      return "ushll2\t%0.2d, %1.4s, %2";
  }
}

static const char * const output_2923[] = {
  "cmgt\t%0.8b, %2.8b, %1.8b",
  "cmlt\t%0.8b, %1.8b, #0",
};

static const char * const output_2924[] = {
  "cmge\t%0.8b, %2.8b, %1.8b",
  "cmle\t%0.8b, %1.8b, #0",
};

static const char * const output_2925[] = {
  "cmeq\t%0.8b, %1.8b, %2.8b",
  "cmeq\t%0.8b, %1.8b, #0",
};

static const char * const output_2926[] = {
  "cmge\t%0.8b, %1.8b, %2.8b",
  "cmge\t%0.8b, %1.8b, #0",
};

static const char * const output_2927[] = {
  "cmgt\t%0.8b, %1.8b, %2.8b",
  "cmgt\t%0.8b, %1.8b, #0",
};

static const char * const output_2928[] = {
  "cmgt\t%0.16b, %2.16b, %1.16b",
  "cmlt\t%0.16b, %1.16b, #0",
};

static const char * const output_2929[] = {
  "cmge\t%0.16b, %2.16b, %1.16b",
  "cmle\t%0.16b, %1.16b, #0",
};

static const char * const output_2930[] = {
  "cmeq\t%0.16b, %1.16b, %2.16b",
  "cmeq\t%0.16b, %1.16b, #0",
};

static const char * const output_2931[] = {
  "cmge\t%0.16b, %1.16b, %2.16b",
  "cmge\t%0.16b, %1.16b, #0",
};

static const char * const output_2932[] = {
  "cmgt\t%0.16b, %1.16b, %2.16b",
  "cmgt\t%0.16b, %1.16b, #0",
};

static const char * const output_2933[] = {
  "cmgt\t%0.4h, %2.4h, %1.4h",
  "cmlt\t%0.4h, %1.4h, #0",
};

static const char * const output_2934[] = {
  "cmge\t%0.4h, %2.4h, %1.4h",
  "cmle\t%0.4h, %1.4h, #0",
};

static const char * const output_2935[] = {
  "cmeq\t%0.4h, %1.4h, %2.4h",
  "cmeq\t%0.4h, %1.4h, #0",
};

static const char * const output_2936[] = {
  "cmge\t%0.4h, %1.4h, %2.4h",
  "cmge\t%0.4h, %1.4h, #0",
};

static const char * const output_2937[] = {
  "cmgt\t%0.4h, %1.4h, %2.4h",
  "cmgt\t%0.4h, %1.4h, #0",
};

static const char * const output_2938[] = {
  "cmgt\t%0.8h, %2.8h, %1.8h",
  "cmlt\t%0.8h, %1.8h, #0",
};

static const char * const output_2939[] = {
  "cmge\t%0.8h, %2.8h, %1.8h",
  "cmle\t%0.8h, %1.8h, #0",
};

static const char * const output_2940[] = {
  "cmeq\t%0.8h, %1.8h, %2.8h",
  "cmeq\t%0.8h, %1.8h, #0",
};

static const char * const output_2941[] = {
  "cmge\t%0.8h, %1.8h, %2.8h",
  "cmge\t%0.8h, %1.8h, #0",
};

static const char * const output_2942[] = {
  "cmgt\t%0.8h, %1.8h, %2.8h",
  "cmgt\t%0.8h, %1.8h, #0",
};

static const char * const output_2943[] = {
  "cmgt\t%0.2s, %2.2s, %1.2s",
  "cmlt\t%0.2s, %1.2s, #0",
};

static const char * const output_2944[] = {
  "cmge\t%0.2s, %2.2s, %1.2s",
  "cmle\t%0.2s, %1.2s, #0",
};

static const char * const output_2945[] = {
  "cmeq\t%0.2s, %1.2s, %2.2s",
  "cmeq\t%0.2s, %1.2s, #0",
};

static const char * const output_2946[] = {
  "cmge\t%0.2s, %1.2s, %2.2s",
  "cmge\t%0.2s, %1.2s, #0",
};

static const char * const output_2947[] = {
  "cmgt\t%0.2s, %1.2s, %2.2s",
  "cmgt\t%0.2s, %1.2s, #0",
};

static const char * const output_2948[] = {
  "cmgt\t%0.4s, %2.4s, %1.4s",
  "cmlt\t%0.4s, %1.4s, #0",
};

static const char * const output_2949[] = {
  "cmge\t%0.4s, %2.4s, %1.4s",
  "cmle\t%0.4s, %1.4s, #0",
};

static const char * const output_2950[] = {
  "cmeq\t%0.4s, %1.4s, %2.4s",
  "cmeq\t%0.4s, %1.4s, #0",
};

static const char * const output_2951[] = {
  "cmge\t%0.4s, %1.4s, %2.4s",
  "cmge\t%0.4s, %1.4s, #0",
};

static const char * const output_2952[] = {
  "cmgt\t%0.4s, %1.4s, %2.4s",
  "cmgt\t%0.4s, %1.4s, #0",
};

static const char * const output_2953[] = {
  "cmgt\t%0.2d, %2.2d, %1.2d",
  "cmlt\t%0.2d, %1.2d, #0",
};

static const char * const output_2954[] = {
  "cmge\t%0.2d, %2.2d, %1.2d",
  "cmle\t%0.2d, %1.2d, #0",
};

static const char * const output_2955[] = {
  "cmeq\t%0.2d, %1.2d, %2.2d",
  "cmeq\t%0.2d, %1.2d, #0",
};

static const char * const output_2956[] = {
  "cmge\t%0.2d, %1.2d, %2.2d",
  "cmge\t%0.2d, %1.2d, #0",
};

static const char * const output_2957[] = {
  "cmgt\t%0.2d, %1.2d, %2.2d",
  "cmgt\t%0.2d, %1.2d, #0",
};

static const char * const output_2963[] = {
  "cmgt\t%d0, %d2, %d1",
  "cmlt\t%d0, %d1, #0",
};

static const char * const output_2964[] = {
  "cmge\t%d0, %d2, %d1",
  "cmle\t%d0, %d1, #0",
};

static const char * const output_2965[] = {
  "cmeq\t%d0, %d1, %d2",
  "cmeq\t%d0, %d1, #0",
};

static const char * const output_2966[] = {
  "cmge\t%d0, %d1, %d2",
  "cmge\t%d0, %d1, #0",
};

static const char * const output_2967[] = {
  "cmgt\t%d0, %d1, %d2",
  "cmgt\t%d0, %d1, #0",
};

static const char * const output_3013[] = {
  "fcmgt\t%0.4h, %2.4h, %1.4h",
  "fcmlt\t%0.4h, %1.4h, 0",
};

static const char * const output_3014[] = {
  "fcmge\t%0.4h, %2.4h, %1.4h",
  "fcmle\t%0.4h, %1.4h, 0",
};

static const char * const output_3015[] = {
  "fcmeq\t%0.4h, %1.4h, %2.4h",
  "fcmeq\t%0.4h, %1.4h, 0",
};

static const char * const output_3016[] = {
  "fcmge\t%0.4h, %1.4h, %2.4h",
  "fcmge\t%0.4h, %1.4h, 0",
};

static const char * const output_3017[] = {
  "fcmgt\t%0.4h, %1.4h, %2.4h",
  "fcmgt\t%0.4h, %1.4h, 0",
};

static const char * const output_3018[] = {
  "fcmgt\t%0.8h, %2.8h, %1.8h",
  "fcmlt\t%0.8h, %1.8h, 0",
};

static const char * const output_3019[] = {
  "fcmge\t%0.8h, %2.8h, %1.8h",
  "fcmle\t%0.8h, %1.8h, 0",
};

static const char * const output_3020[] = {
  "fcmeq\t%0.8h, %1.8h, %2.8h",
  "fcmeq\t%0.8h, %1.8h, 0",
};

static const char * const output_3021[] = {
  "fcmge\t%0.8h, %1.8h, %2.8h",
  "fcmge\t%0.8h, %1.8h, 0",
};

static const char * const output_3022[] = {
  "fcmgt\t%0.8h, %1.8h, %2.8h",
  "fcmgt\t%0.8h, %1.8h, 0",
};

static const char * const output_3023[] = {
  "fcmgt\t%0.2s, %2.2s, %1.2s",
  "fcmlt\t%0.2s, %1.2s, 0",
};

static const char * const output_3024[] = {
  "fcmge\t%0.2s, %2.2s, %1.2s",
  "fcmle\t%0.2s, %1.2s, 0",
};

static const char * const output_3025[] = {
  "fcmeq\t%0.2s, %1.2s, %2.2s",
  "fcmeq\t%0.2s, %1.2s, 0",
};

static const char * const output_3026[] = {
  "fcmge\t%0.2s, %1.2s, %2.2s",
  "fcmge\t%0.2s, %1.2s, 0",
};

static const char * const output_3027[] = {
  "fcmgt\t%0.2s, %1.2s, %2.2s",
  "fcmgt\t%0.2s, %1.2s, 0",
};

static const char * const output_3028[] = {
  "fcmgt\t%0.4s, %2.4s, %1.4s",
  "fcmlt\t%0.4s, %1.4s, 0",
};

static const char * const output_3029[] = {
  "fcmge\t%0.4s, %2.4s, %1.4s",
  "fcmle\t%0.4s, %1.4s, 0",
};

static const char * const output_3030[] = {
  "fcmeq\t%0.4s, %1.4s, %2.4s",
  "fcmeq\t%0.4s, %1.4s, 0",
};

static const char * const output_3031[] = {
  "fcmge\t%0.4s, %1.4s, %2.4s",
  "fcmge\t%0.4s, %1.4s, 0",
};

static const char * const output_3032[] = {
  "fcmgt\t%0.4s, %1.4s, %2.4s",
  "fcmgt\t%0.4s, %1.4s, 0",
};

static const char * const output_3033[] = {
  "fcmgt\t%0.2d, %2.2d, %1.2d",
  "fcmlt\t%0.2d, %1.2d, 0",
};

static const char * const output_3034[] = {
  "fcmge\t%0.2d, %2.2d, %1.2d",
  "fcmle\t%0.2d, %1.2d, 0",
};

static const char * const output_3035[] = {
  "fcmeq\t%0.2d, %1.2d, %2.2d",
  "fcmeq\t%0.2d, %1.2d, 0",
};

static const char * const output_3036[] = {
  "fcmge\t%0.2d, %1.2d, %2.2d",
  "fcmge\t%0.2d, %1.2d, 0",
};

static const char * const output_3037[] = {
  "fcmgt\t%0.2d, %1.2d, %2.2d",
  "fcmgt\t%0.2d, %1.2d, 0",
};

static const char * const output_3038[] = {
  "fcmgt\t%h0, %h2, %h1",
  "fcmlt\t%h0, %h1, 0",
};

static const char * const output_3039[] = {
  "fcmge\t%h0, %h2, %h1",
  "fcmle\t%h0, %h1, 0",
};

static const char * const output_3040[] = {
  "fcmeq\t%h0, %h1, %h2",
  "fcmeq\t%h0, %h1, 0",
};

static const char * const output_3041[] = {
  "fcmge\t%h0, %h1, %h2",
  "fcmge\t%h0, %h1, 0",
};

static const char * const output_3042[] = {
  "fcmgt\t%h0, %h1, %h2",
  "fcmgt\t%h0, %h1, 0",
};

static const char * const output_3043[] = {
  "fcmgt\t%s0, %s2, %s1",
  "fcmlt\t%s0, %s1, 0",
};

static const char * const output_3044[] = {
  "fcmge\t%s0, %s2, %s1",
  "fcmle\t%s0, %s1, 0",
};

static const char * const output_3045[] = {
  "fcmeq\t%s0, %s1, %s2",
  "fcmeq\t%s0, %s1, 0",
};

static const char * const output_3046[] = {
  "fcmge\t%s0, %s1, %s2",
  "fcmge\t%s0, %s1, 0",
};

static const char * const output_3047[] = {
  "fcmgt\t%s0, %s1, %s2",
  "fcmgt\t%s0, %s1, 0",
};

static const char * const output_3048[] = {
  "fcmgt\t%d0, %d2, %d1",
  "fcmlt\t%d0, %d1, 0",
};

static const char * const output_3049[] = {
  "fcmge\t%d0, %d2, %d1",
  "fcmle\t%d0, %d1, 0",
};

static const char * const output_3050[] = {
  "fcmeq\t%d0, %d1, %d2",
  "fcmeq\t%d0, %d1, 0",
};

static const char * const output_3051[] = {
  "fcmge\t%d0, %d1, %d2",
  "fcmge\t%d0, %d1, 0",
};

static const char * const output_3052[] = {
  "fcmgt\t%d0, %d1, %d2",
  "fcmgt\t%d0, %d1, 0",
};

static const char *
output_3115 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4914 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V8QImode, INTVAL (operands[3]));
    return "ld2\t{%S0.b - %T0.b}[%3], %1";
  }
}

static const char *
output_3116 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4914 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V16QImode, INTVAL (operands[3]));
    return "ld2\t{%S0.b - %T0.b}[%3], %1";
  }
}

static const char *
output_3117 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4914 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V4HImode, INTVAL (operands[3]));
    return "ld2\t{%S0.h - %T0.h}[%3], %1";
  }
}

static const char *
output_3118 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4914 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V8HImode, INTVAL (operands[3]));
    return "ld2\t{%S0.h - %T0.h}[%3], %1";
  }
}

static const char *
output_3119 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4914 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V2SImode, INTVAL (operands[3]));
    return "ld2\t{%S0.s - %T0.s}[%3], %1";
  }
}

static const char *
output_3120 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4914 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V4SImode, INTVAL (operands[3]));
    return "ld2\t{%S0.s - %T0.s}[%3], %1";
  }
}

static const char *
output_3121 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4914 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V2DImode, INTVAL (operands[3]));
    return "ld2\t{%S0.d - %T0.d}[%3], %1";
  }
}

static const char *
output_3122 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4914 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V4HFmode, INTVAL (operands[3]));
    return "ld2\t{%S0.h - %T0.h}[%3], %1";
  }
}

static const char *
output_3123 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4914 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V8HFmode, INTVAL (operands[3]));
    return "ld2\t{%S0.h - %T0.h}[%3], %1";
  }
}

static const char *
output_3124 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4914 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V2SFmode, INTVAL (operands[3]));
    return "ld2\t{%S0.s - %T0.s}[%3], %1";
  }
}

static const char *
output_3125 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4914 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V4SFmode, INTVAL (operands[3]));
    return "ld2\t{%S0.s - %T0.s}[%3], %1";
  }
}

static const char *
output_3126 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4914 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V2DFmode, INTVAL (operands[3]));
    return "ld2\t{%S0.d - %T0.d}[%3], %1";
  }
}

static const char *
output_3127 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4914 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (DImode, INTVAL (operands[3]));
    return "ld2\t{%S0.d - %T0.d}[%3], %1";
  }
}

static const char *
output_3128 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4914 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (DFmode, INTVAL (operands[3]));
    return "ld2\t{%S0.d - %T0.d}[%3], %1";
  }
}

static const char *
output_3136 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4958 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V8QImode, INTVAL (operands[2]));
    return "st2\t{%S1.b - %T1.b}[%2], %0";
  }
}

static const char *
output_3137 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4958 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V16QImode, INTVAL (operands[2]));
    return "st2\t{%S1.b - %T1.b}[%2], %0";
  }
}

static const char *
output_3138 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4958 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4HImode, INTVAL (operands[2]));
    return "st2\t{%S1.h - %T1.h}[%2], %0";
  }
}

static const char *
output_3139 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4958 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V8HImode, INTVAL (operands[2]));
    return "st2\t{%S1.h - %T1.h}[%2], %0";
  }
}

static const char *
output_3140 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4958 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V2SImode, INTVAL (operands[2]));
    return "st2\t{%S1.s - %T1.s}[%2], %0";
  }
}

static const char *
output_3141 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4958 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4SImode, INTVAL (operands[2]));
    return "st2\t{%S1.s - %T1.s}[%2], %0";
  }
}

static const char *
output_3142 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4958 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V2DImode, INTVAL (operands[2]));
    return "st2\t{%S1.d - %T1.d}[%2], %0";
  }
}

static const char *
output_3143 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4958 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4HFmode, INTVAL (operands[2]));
    return "st2\t{%S1.h - %T1.h}[%2], %0";
  }
}

static const char *
output_3144 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4958 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V8HFmode, INTVAL (operands[2]));
    return "st2\t{%S1.h - %T1.h}[%2], %0";
  }
}

static const char *
output_3145 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4958 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V2SFmode, INTVAL (operands[2]));
    return "st2\t{%S1.s - %T1.s}[%2], %0";
  }
}

static const char *
output_3146 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4958 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4SFmode, INTVAL (operands[2]));
    return "st2\t{%S1.s - %T1.s}[%2], %0";
  }
}

static const char *
output_3147 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4958 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V2DFmode, INTVAL (operands[2]));
    return "st2\t{%S1.d - %T1.d}[%2], %0";
  }
}

static const char *
output_3148 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4958 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (DImode, INTVAL (operands[2]));
    return "st2\t{%S1.d - %T1.d}[%2], %0";
  }
}

static const char *
output_3149 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 4958 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (DFmode, INTVAL (operands[2]));
    return "st2\t{%S1.d - %T1.d}[%2], %0";
  }
}

static const char *
output_3171 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5012 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V8QImode, INTVAL (operands[3]));
    return "ld3\t{%S0.b - %U0.b}[%3], %1";
}
}

static const char *
output_3172 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5012 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V16QImode, INTVAL (operands[3]));
    return "ld3\t{%S0.b - %U0.b}[%3], %1";
}
}

static const char *
output_3173 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5012 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V4HImode, INTVAL (operands[3]));
    return "ld3\t{%S0.h - %U0.h}[%3], %1";
}
}

static const char *
output_3174 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5012 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V8HImode, INTVAL (operands[3]));
    return "ld3\t{%S0.h - %U0.h}[%3], %1";
}
}

static const char *
output_3175 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5012 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V2SImode, INTVAL (operands[3]));
    return "ld3\t{%S0.s - %U0.s}[%3], %1";
}
}

static const char *
output_3176 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5012 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V4SImode, INTVAL (operands[3]));
    return "ld3\t{%S0.s - %U0.s}[%3], %1";
}
}

static const char *
output_3177 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5012 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V2DImode, INTVAL (operands[3]));
    return "ld3\t{%S0.d - %U0.d}[%3], %1";
}
}

static const char *
output_3178 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5012 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V4HFmode, INTVAL (operands[3]));
    return "ld3\t{%S0.h - %U0.h}[%3], %1";
}
}

static const char *
output_3179 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5012 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V8HFmode, INTVAL (operands[3]));
    return "ld3\t{%S0.h - %U0.h}[%3], %1";
}
}

static const char *
output_3180 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5012 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V2SFmode, INTVAL (operands[3]));
    return "ld3\t{%S0.s - %U0.s}[%3], %1";
}
}

static const char *
output_3181 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5012 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V4SFmode, INTVAL (operands[3]));
    return "ld3\t{%S0.s - %U0.s}[%3], %1";
}
}

static const char *
output_3182 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5012 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V2DFmode, INTVAL (operands[3]));
    return "ld3\t{%S0.d - %U0.d}[%3], %1";
}
}

static const char *
output_3183 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5012 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (DImode, INTVAL (operands[3]));
    return "ld3\t{%S0.d - %U0.d}[%3], %1";
}
}

static const char *
output_3184 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5012 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (DFmode, INTVAL (operands[3]));
    return "ld3\t{%S0.d - %U0.d}[%3], %1";
}
}

static const char *
output_3192 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5056 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V8QImode, INTVAL (operands[2]));
    return "st3\t{%S1.b - %U1.b}[%2], %0";
  }
}

static const char *
output_3193 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5056 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V16QImode, INTVAL (operands[2]));
    return "st3\t{%S1.b - %U1.b}[%2], %0";
  }
}

static const char *
output_3194 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5056 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4HImode, INTVAL (operands[2]));
    return "st3\t{%S1.h - %U1.h}[%2], %0";
  }
}

static const char *
output_3195 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5056 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V8HImode, INTVAL (operands[2]));
    return "st3\t{%S1.h - %U1.h}[%2], %0";
  }
}

static const char *
output_3196 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5056 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V2SImode, INTVAL (operands[2]));
    return "st3\t{%S1.s - %U1.s}[%2], %0";
  }
}

static const char *
output_3197 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5056 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4SImode, INTVAL (operands[2]));
    return "st3\t{%S1.s - %U1.s}[%2], %0";
  }
}

static const char *
output_3198 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5056 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V2DImode, INTVAL (operands[2]));
    return "st3\t{%S1.d - %U1.d}[%2], %0";
  }
}

static const char *
output_3199 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5056 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4HFmode, INTVAL (operands[2]));
    return "st3\t{%S1.h - %U1.h}[%2], %0";
  }
}

static const char *
output_3200 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5056 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V8HFmode, INTVAL (operands[2]));
    return "st3\t{%S1.h - %U1.h}[%2], %0";
  }
}

static const char *
output_3201 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5056 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V2SFmode, INTVAL (operands[2]));
    return "st3\t{%S1.s - %U1.s}[%2], %0";
  }
}

static const char *
output_3202 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5056 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4SFmode, INTVAL (operands[2]));
    return "st3\t{%S1.s - %U1.s}[%2], %0";
  }
}

static const char *
output_3203 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5056 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V2DFmode, INTVAL (operands[2]));
    return "st3\t{%S1.d - %U1.d}[%2], %0";
  }
}

static const char *
output_3204 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5056 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (DImode, INTVAL (operands[2]));
    return "st3\t{%S1.d - %U1.d}[%2], %0";
  }
}

static const char *
output_3205 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5056 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (DFmode, INTVAL (operands[2]));
    return "st3\t{%S1.d - %U1.d}[%2], %0";
  }
}

static const char *
output_3227 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5110 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V8QImode, INTVAL (operands[3]));
    return "ld4\t{%S0.b - %V0.b}[%3], %1";
}
}

static const char *
output_3228 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5110 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V16QImode, INTVAL (operands[3]));
    return "ld4\t{%S0.b - %V0.b}[%3], %1";
}
}

static const char *
output_3229 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5110 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V4HImode, INTVAL (operands[3]));
    return "ld4\t{%S0.h - %V0.h}[%3], %1";
}
}

static const char *
output_3230 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5110 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V8HImode, INTVAL (operands[3]));
    return "ld4\t{%S0.h - %V0.h}[%3], %1";
}
}

static const char *
output_3231 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5110 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V2SImode, INTVAL (operands[3]));
    return "ld4\t{%S0.s - %V0.s}[%3], %1";
}
}

static const char *
output_3232 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5110 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V4SImode, INTVAL (operands[3]));
    return "ld4\t{%S0.s - %V0.s}[%3], %1";
}
}

static const char *
output_3233 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5110 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V2DImode, INTVAL (operands[3]));
    return "ld4\t{%S0.d - %V0.d}[%3], %1";
}
}

static const char *
output_3234 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5110 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V4HFmode, INTVAL (operands[3]));
    return "ld4\t{%S0.h - %V0.h}[%3], %1";
}
}

static const char *
output_3235 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5110 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V8HFmode, INTVAL (operands[3]));
    return "ld4\t{%S0.h - %V0.h}[%3], %1";
}
}

static const char *
output_3236 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5110 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V2SFmode, INTVAL (operands[3]));
    return "ld4\t{%S0.s - %V0.s}[%3], %1";
}
}

static const char *
output_3237 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5110 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V4SFmode, INTVAL (operands[3]));
    return "ld4\t{%S0.s - %V0.s}[%3], %1";
}
}

static const char *
output_3238 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5110 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (V2DFmode, INTVAL (operands[3]));
    return "ld4\t{%S0.d - %V0.d}[%3], %1";
}
}

static const char *
output_3239 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5110 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (DImode, INTVAL (operands[3]));
    return "ld4\t{%S0.d - %V0.d}[%3], %1";
}
}

static const char *
output_3240 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5110 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[3] = aarch64_endian_lane_rtx (DFmode, INTVAL (operands[3]));
    return "ld4\t{%S0.d - %V0.d}[%3], %1";
}
}

static const char *
output_3248 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5154 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V8QImode, INTVAL (operands[2]));
    return "st4\t{%S1.b - %V1.b}[%2], %0";
  }
}

static const char *
output_3249 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5154 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V16QImode, INTVAL (operands[2]));
    return "st4\t{%S1.b - %V1.b}[%2], %0";
  }
}

static const char *
output_3250 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5154 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4HImode, INTVAL (operands[2]));
    return "st4\t{%S1.h - %V1.h}[%2], %0";
  }
}

static const char *
output_3251 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5154 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V8HImode, INTVAL (operands[2]));
    return "st4\t{%S1.h - %V1.h}[%2], %0";
  }
}

static const char *
output_3252 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5154 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V2SImode, INTVAL (operands[2]));
    return "st4\t{%S1.s - %V1.s}[%2], %0";
  }
}

static const char *
output_3253 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5154 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4SImode, INTVAL (operands[2]));
    return "st4\t{%S1.s - %V1.s}[%2], %0";
  }
}

static const char *
output_3254 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5154 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V2DImode, INTVAL (operands[2]));
    return "st4\t{%S1.d - %V1.d}[%2], %0";
  }
}

static const char *
output_3255 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5154 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4HFmode, INTVAL (operands[2]));
    return "st4\t{%S1.h - %V1.h}[%2], %0";
  }
}

static const char *
output_3256 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5154 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V8HFmode, INTVAL (operands[2]));
    return "st4\t{%S1.h - %V1.h}[%2], %0";
  }
}

static const char *
output_3257 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5154 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V2SFmode, INTVAL (operands[2]));
    return "st4\t{%S1.s - %V1.s}[%2], %0";
  }
}

static const char *
output_3258 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5154 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V4SFmode, INTVAL (operands[2]));
    return "st4\t{%S1.s - %V1.s}[%2], %0";
  }
}

static const char *
output_3259 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5154 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (V2DFmode, INTVAL (operands[2]));
    return "st4\t{%S1.d - %V1.d}[%2], %0";
  }
}

static const char *
output_3260 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5154 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (DImode, INTVAL (operands[2]));
    return "st4\t{%S1.d - %V1.d}[%2], %0";
  }
}

static const char *
output_3261 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5154 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
    operands[2] = aarch64_endian_lane_rtx (DFmode, INTVAL (operands[2]));
    return "st4\t{%S1.d - %V1.d}[%2], %0";
  }
}

static const char * const output_3307[] = {
  "#",
  "st1\t{%S1.16b - %T1.16b}, %0",
  "ld1\t{%S0.16b - %T0.16b}, %1",
};

static const char * const output_3308[] = {
  "#",
  "st1\t{%S1.16b - %U1.16b}, %0",
  "ld1\t{%S0.16b - %U0.16b}, %1",
};

static const char * const output_3309[] = {
  "#",
  "st1\t{%S1.16b - %V1.16b}, %0",
  "ld1\t{%S0.16b - %V0.16b}, %1",
};

static const char * const output_3336[] = {
  "#",
  "stp\t%q1, %R1, %0",
  "ldp\t%q0, %R0, %1",
};

static const char *
output_3448 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5759 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  operands[3] = GEN_INT (INTVAL (operands[3])
      * GET_MODE_UNIT_SIZE (V8QImode));
  return "ext\t%0.8b, %1.8b, %2.8b, #%3";
}
}

static const char *
output_3449 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5759 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  operands[3] = GEN_INT (INTVAL (operands[3])
      * GET_MODE_UNIT_SIZE (V16QImode));
  return "ext\t%0.16b, %1.16b, %2.16b, #%3";
}
}

static const char *
output_3450 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5759 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  operands[3] = GEN_INT (INTVAL (operands[3])
      * GET_MODE_UNIT_SIZE (V4HImode));
  return "ext\t%0.8b, %1.8b, %2.8b, #%3";
}
}

static const char *
output_3451 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5759 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  operands[3] = GEN_INT (INTVAL (operands[3])
      * GET_MODE_UNIT_SIZE (V8HImode));
  return "ext\t%0.16b, %1.16b, %2.16b, #%3";
}
}

static const char *
output_3452 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5759 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  operands[3] = GEN_INT (INTVAL (operands[3])
      * GET_MODE_UNIT_SIZE (V2SImode));
  return "ext\t%0.8b, %1.8b, %2.8b, #%3";
}
}

static const char *
output_3453 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5759 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  operands[3] = GEN_INT (INTVAL (operands[3])
      * GET_MODE_UNIT_SIZE (V4SImode));
  return "ext\t%0.16b, %1.16b, %2.16b, #%3";
}
}

static const char *
output_3454 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5759 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  operands[3] = GEN_INT (INTVAL (operands[3])
      * GET_MODE_UNIT_SIZE (V2DImode));
  return "ext\t%0.16b, %1.16b, %2.16b, #%3";
}
}

static const char *
output_3455 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5759 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  operands[3] = GEN_INT (INTVAL (operands[3])
      * GET_MODE_UNIT_SIZE (V4HFmode));
  return "ext\t%0.8b, %1.8b, %2.8b, #%3";
}
}

static const char *
output_3456 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5759 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  operands[3] = GEN_INT (INTVAL (operands[3])
      * GET_MODE_UNIT_SIZE (V8HFmode));
  return "ext\t%0.16b, %1.16b, %2.16b, #%3";
}
}

static const char *
output_3457 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5759 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  operands[3] = GEN_INT (INTVAL (operands[3])
      * GET_MODE_UNIT_SIZE (V2SFmode));
  return "ext\t%0.8b, %1.8b, %2.8b, #%3";
}
}

static const char *
output_3458 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5759 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  operands[3] = GEN_INT (INTVAL (operands[3])
      * GET_MODE_UNIT_SIZE (V4SFmode));
  return "ext\t%0.16b, %1.16b, %2.16b, #%3";
}
}

static const char *
output_3459 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 5759 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-simd.md"
{
  operands[3] = GEN_INT (INTVAL (operands[3])
      * GET_MODE_UNIT_SIZE (V2DFmode));
  return "ext\t%0.16b, %1.16b, %2.16b, #%3";
}
}

static const char *
output_3639 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 102 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
  enum memmodel model = memmodel_from_int (INTVAL (operands[3]));
  if (is_mm_relaxed (model))
    return "casb\t%w0, %w2, %1";
  else if (is_mm_acquire (model) || is_mm_consume (model))
    return "casab\t%w0, %w2, %1";
  else if (is_mm_release (model))
    return "caslb\t%w0, %w2, %1";
  else
    return "casalb\t%w0, %w2, %1";
}
}

static const char *
output_3640 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 102 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
  enum memmodel model = memmodel_from_int (INTVAL (operands[3]));
  if (is_mm_relaxed (model))
    return "cash\t%w0, %w2, %1";
  else if (is_mm_acquire (model) || is_mm_consume (model))
    return "casah\t%w0, %w2, %1";
  else if (is_mm_release (model))
    return "caslh\t%w0, %w2, %1";
  else
    return "casalh\t%w0, %w2, %1";
}
}

static const char *
output_3641 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 124 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
  enum memmodel model = memmodel_from_int (INTVAL (operands[3]));
  if (is_mm_relaxed (model))
    return "cas\t%w0, %w2, %1";
  else if (is_mm_acquire (model) || is_mm_consume (model))
    return "casa\t%w0, %w2, %1";
  else if (is_mm_release (model))
    return "casl\t%w0, %w2, %1";
  else
    return "casal\t%w0, %w2, %1";
}
}

static const char *
output_3642 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 124 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
  enum memmodel model = memmodel_from_int (INTVAL (operands[3]));
  if (is_mm_relaxed (model))
    return "cas\t%x0, %x2, %1";
  else if (is_mm_acquire (model) || is_mm_consume (model))
    return "casa\t%x0, %x2, %1";
  else if (is_mm_release (model))
    return "casl\t%x0, %x2, %1";
  else
    return "casal\t%x0, %x2, %1";
}
}

static const char *
output_3647 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 187 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
    enum memmodel model = memmodel_from_int (INTVAL (operands[3]));
    if (is_mm_relaxed (model))
      return "swpb\t%w2, %w0, %1";
    else if (is_mm_acquire (model) || is_mm_consume (model))
      return "swpab\t%w2, %w0, %1";
    else if (is_mm_release (model))
      return "swplb\t%w2, %w0, %1";
    else
      return "swpalb\t%w2, %w0, %1";
  }
}

static const char *
output_3648 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 187 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
    enum memmodel model = memmodel_from_int (INTVAL (operands[3]));
    if (is_mm_relaxed (model))
      return "swph\t%w2, %w0, %1";
    else if (is_mm_acquire (model) || is_mm_consume (model))
      return "swpah\t%w2, %w0, %1";
    else if (is_mm_release (model))
      return "swplh\t%w2, %w0, %1";
    else
      return "swpalh\t%w2, %w0, %1";
  }
}

static const char *
output_3649 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 187 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
    enum memmodel model = memmodel_from_int (INTVAL (operands[3]));
    if (is_mm_relaxed (model))
      return "swp\t%w2, %w0, %1";
    else if (is_mm_acquire (model) || is_mm_consume (model))
      return "swpa\t%w2, %w0, %1";
    else if (is_mm_release (model))
      return "swpl\t%w2, %w0, %1";
    else
      return "swpal\t%w2, %w0, %1";
  }
}

static const char *
output_3650 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 187 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
    enum memmodel model = memmodel_from_int (INTVAL (operands[3]));
    if (is_mm_relaxed (model))
      return "swp\t%x2, %x0, %1";
    else if (is_mm_acquire (model) || is_mm_consume (model))
      return "swpa\t%x2, %x0, %1";
    else if (is_mm_release (model))
      return "swpl\t%x2, %x0, %1";
    else
      return "swpal\t%x2, %x0, %1";
  }
}

static const char *
output_3671 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 287 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
   enum memmodel model = memmodel_from_int (INTVAL (operands[2]));
   if (is_mm_relaxed (model))
     return "ldsetb\t%w1, %w3, %0";
   else if (is_mm_release (model))
     return "ldsetlb\t%w1, %w3, %0";
   else if (is_mm_acquire (model) || is_mm_consume (model))
     return "ldsetab\t%w1, %w3, %0";
   else
     return "ldsetalb\t%w1, %w3, %0";
  }
}

static const char *
output_3672 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 287 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
   enum memmodel model = memmodel_from_int (INTVAL (operands[2]));
   if (is_mm_relaxed (model))
     return "ldclrb\t%w1, %w3, %0";
   else if (is_mm_release (model))
     return "ldclrlb\t%w1, %w3, %0";
   else if (is_mm_acquire (model) || is_mm_consume (model))
     return "ldclrab\t%w1, %w3, %0";
   else
     return "ldclralb\t%w1, %w3, %0";
  }
}

static const char *
output_3673 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 287 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
   enum memmodel model = memmodel_from_int (INTVAL (operands[2]));
   if (is_mm_relaxed (model))
     return "ldeorb\t%w1, %w3, %0";
   else if (is_mm_release (model))
     return "ldeorlb\t%w1, %w3, %0";
   else if (is_mm_acquire (model) || is_mm_consume (model))
     return "ldeorab\t%w1, %w3, %0";
   else
     return "ldeoralb\t%w1, %w3, %0";
  }
}

static const char *
output_3674 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 287 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
   enum memmodel model = memmodel_from_int (INTVAL (operands[2]));
   if (is_mm_relaxed (model))
     return "ldaddb\t%w1, %w3, %0";
   else if (is_mm_release (model))
     return "ldaddlb\t%w1, %w3, %0";
   else if (is_mm_acquire (model) || is_mm_consume (model))
     return "ldaddab\t%w1, %w3, %0";
   else
     return "ldaddalb\t%w1, %w3, %0";
  }
}

static const char *
output_3675 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 287 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
   enum memmodel model = memmodel_from_int (INTVAL (operands[2]));
   if (is_mm_relaxed (model))
     return "ldseth\t%w1, %w3, %0";
   else if (is_mm_release (model))
     return "ldsetlh\t%w1, %w3, %0";
   else if (is_mm_acquire (model) || is_mm_consume (model))
     return "ldsetah\t%w1, %w3, %0";
   else
     return "ldsetalh\t%w1, %w3, %0";
  }
}

static const char *
output_3676 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 287 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
   enum memmodel model = memmodel_from_int (INTVAL (operands[2]));
   if (is_mm_relaxed (model))
     return "ldclrh\t%w1, %w3, %0";
   else if (is_mm_release (model))
     return "ldclrlh\t%w1, %w3, %0";
   else if (is_mm_acquire (model) || is_mm_consume (model))
     return "ldclrah\t%w1, %w3, %0";
   else
     return "ldclralh\t%w1, %w3, %0";
  }
}

static const char *
output_3677 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 287 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
   enum memmodel model = memmodel_from_int (INTVAL (operands[2]));
   if (is_mm_relaxed (model))
     return "ldeorh\t%w1, %w3, %0";
   else if (is_mm_release (model))
     return "ldeorlh\t%w1, %w3, %0";
   else if (is_mm_acquire (model) || is_mm_consume (model))
     return "ldeorah\t%w1, %w3, %0";
   else
     return "ldeoralh\t%w1, %w3, %0";
  }
}

static const char *
output_3678 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 287 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
   enum memmodel model = memmodel_from_int (INTVAL (operands[2]));
   if (is_mm_relaxed (model))
     return "ldaddh\t%w1, %w3, %0";
   else if (is_mm_release (model))
     return "ldaddlh\t%w1, %w3, %0";
   else if (is_mm_acquire (model) || is_mm_consume (model))
     return "ldaddah\t%w1, %w3, %0";
   else
     return "ldaddalh\t%w1, %w3, %0";
  }
}

static const char *
output_3679 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 287 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
   enum memmodel model = memmodel_from_int (INTVAL (operands[2]));
   if (is_mm_relaxed (model))
     return "ldset\t%w1, %w3, %0";
   else if (is_mm_release (model))
     return "ldsetl\t%w1, %w3, %0";
   else if (is_mm_acquire (model) || is_mm_consume (model))
     return "ldseta\t%w1, %w3, %0";
   else
     return "ldsetal\t%w1, %w3, %0";
  }
}

static const char *
output_3680 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 287 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
   enum memmodel model = memmodel_from_int (INTVAL (operands[2]));
   if (is_mm_relaxed (model))
     return "ldclr\t%w1, %w3, %0";
   else if (is_mm_release (model))
     return "ldclrl\t%w1, %w3, %0";
   else if (is_mm_acquire (model) || is_mm_consume (model))
     return "ldclra\t%w1, %w3, %0";
   else
     return "ldclral\t%w1, %w3, %0";
  }
}

static const char *
output_3681 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 287 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
   enum memmodel model = memmodel_from_int (INTVAL (operands[2]));
   if (is_mm_relaxed (model))
     return "ldeor\t%w1, %w3, %0";
   else if (is_mm_release (model))
     return "ldeorl\t%w1, %w3, %0";
   else if (is_mm_acquire (model) || is_mm_consume (model))
     return "ldeora\t%w1, %w3, %0";
   else
     return "ldeoral\t%w1, %w3, %0";
  }
}

static const char *
output_3682 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 287 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
   enum memmodel model = memmodel_from_int (INTVAL (operands[2]));
   if (is_mm_relaxed (model))
     return "ldadd\t%w1, %w3, %0";
   else if (is_mm_release (model))
     return "ldaddl\t%w1, %w3, %0";
   else if (is_mm_acquire (model) || is_mm_consume (model))
     return "ldadda\t%w1, %w3, %0";
   else
     return "ldaddal\t%w1, %w3, %0";
  }
}

static const char *
output_3683 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 287 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
   enum memmodel model = memmodel_from_int (INTVAL (operands[2]));
   if (is_mm_relaxed (model))
     return "ldset\t%x1, %x3, %0";
   else if (is_mm_release (model))
     return "ldsetl\t%x1, %x3, %0";
   else if (is_mm_acquire (model) || is_mm_consume (model))
     return "ldseta\t%x1, %x3, %0";
   else
     return "ldsetal\t%x1, %x3, %0";
  }
}

static const char *
output_3684 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 287 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
   enum memmodel model = memmodel_from_int (INTVAL (operands[2]));
   if (is_mm_relaxed (model))
     return "ldclr\t%x1, %x3, %0";
   else if (is_mm_release (model))
     return "ldclrl\t%x1, %x3, %0";
   else if (is_mm_acquire (model) || is_mm_consume (model))
     return "ldclra\t%x1, %x3, %0";
   else
     return "ldclral\t%x1, %x3, %0";
  }
}

static const char *
output_3685 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 287 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
   enum memmodel model = memmodel_from_int (INTVAL (operands[2]));
   if (is_mm_relaxed (model))
     return "ldeor\t%x1, %x3, %0";
   else if (is_mm_release (model))
     return "ldeorl\t%x1, %x3, %0";
   else if (is_mm_acquire (model) || is_mm_consume (model))
     return "ldeora\t%x1, %x3, %0";
   else
     return "ldeoral\t%x1, %x3, %0";
  }
}

static const char *
output_3686 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 287 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
   enum memmodel model = memmodel_from_int (INTVAL (operands[2]));
   if (is_mm_relaxed (model))
     return "ldadd\t%x1, %x3, %0";
   else if (is_mm_release (model))
     return "ldaddl\t%x1, %x3, %0";
   else if (is_mm_acquire (model) || is_mm_consume (model))
     return "ldadda\t%x1, %x3, %0";
   else
     return "ldaddal\t%x1, %x3, %0";
  }
}

static const char *
output_3711 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 402 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
   enum memmodel model = memmodel_from_int (INTVAL (operands[3]));
   if (is_mm_relaxed (model))
     return "ldsetb\t%w2, %w0, %1";
   else if (is_mm_acquire (model) || is_mm_consume (model))
     return "ldsetab\t%w2, %w0, %1";
   else if (is_mm_release (model))
     return "ldsetlb\t%w2, %w0, %1";
   else
     return "ldsetalb\t%w2, %w0, %1";
  }
}

static const char *
output_3712 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 402 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
   enum memmodel model = memmodel_from_int (INTVAL (operands[3]));
   if (is_mm_relaxed (model))
     return "ldclrb\t%w2, %w0, %1";
   else if (is_mm_acquire (model) || is_mm_consume (model))
     return "ldclrab\t%w2, %w0, %1";
   else if (is_mm_release (model))
     return "ldclrlb\t%w2, %w0, %1";
   else
     return "ldclralb\t%w2, %w0, %1";
  }
}

static const char *
output_3713 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 402 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
   enum memmodel model = memmodel_from_int (INTVAL (operands[3]));
   if (is_mm_relaxed (model))
     return "ldeorb\t%w2, %w0, %1";
   else if (is_mm_acquire (model) || is_mm_consume (model))
     return "ldeorab\t%w2, %w0, %1";
   else if (is_mm_release (model))
     return "ldeorlb\t%w2, %w0, %1";
   else
     return "ldeoralb\t%w2, %w0, %1";
  }
}

static const char *
output_3714 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 402 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
   enum memmodel model = memmodel_from_int (INTVAL (operands[3]));
   if (is_mm_relaxed (model))
     return "ldaddb\t%w2, %w0, %1";
   else if (is_mm_acquire (model) || is_mm_consume (model))
     return "ldaddab\t%w2, %w0, %1";
   else if (is_mm_release (model))
     return "ldaddlb\t%w2, %w0, %1";
   else
     return "ldaddalb\t%w2, %w0, %1";
  }
}

static const char *
output_3715 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 402 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
   enum memmodel model = memmodel_from_int (INTVAL (operands[3]));
   if (is_mm_relaxed (model))
     return "ldseth\t%w2, %w0, %1";
   else if (is_mm_acquire (model) || is_mm_consume (model))
     return "ldsetah\t%w2, %w0, %1";
   else if (is_mm_release (model))
     return "ldsetlh\t%w2, %w0, %1";
   else
     return "ldsetalh\t%w2, %w0, %1";
  }
}

static const char *
output_3716 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 402 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
   enum memmodel model = memmodel_from_int (INTVAL (operands[3]));
   if (is_mm_relaxed (model))
     return "ldclrh\t%w2, %w0, %1";
   else if (is_mm_acquire (model) || is_mm_consume (model))
     return "ldclrah\t%w2, %w0, %1";
   else if (is_mm_release (model))
     return "ldclrlh\t%w2, %w0, %1";
   else
     return "ldclralh\t%w2, %w0, %1";
  }
}

static const char *
output_3717 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 402 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
   enum memmodel model = memmodel_from_int (INTVAL (operands[3]));
   if (is_mm_relaxed (model))
     return "ldeorh\t%w2, %w0, %1";
   else if (is_mm_acquire (model) || is_mm_consume (model))
     return "ldeorah\t%w2, %w0, %1";
   else if (is_mm_release (model))
     return "ldeorlh\t%w2, %w0, %1";
   else
     return "ldeoralh\t%w2, %w0, %1";
  }
}

static const char *
output_3718 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 402 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
   enum memmodel model = memmodel_from_int (INTVAL (operands[3]));
   if (is_mm_relaxed (model))
     return "ldaddh\t%w2, %w0, %1";
   else if (is_mm_acquire (model) || is_mm_consume (model))
     return "ldaddah\t%w2, %w0, %1";
   else if (is_mm_release (model))
     return "ldaddlh\t%w2, %w0, %1";
   else
     return "ldaddalh\t%w2, %w0, %1";
  }
}

static const char *
output_3719 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 402 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
   enum memmodel model = memmodel_from_int (INTVAL (operands[3]));
   if (is_mm_relaxed (model))
     return "ldset\t%w2, %w0, %1";
   else if (is_mm_acquire (model) || is_mm_consume (model))
     return "ldseta\t%w2, %w0, %1";
   else if (is_mm_release (model))
     return "ldsetl\t%w2, %w0, %1";
   else
     return "ldsetal\t%w2, %w0, %1";
  }
}

static const char *
output_3720 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 402 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
   enum memmodel model = memmodel_from_int (INTVAL (operands[3]));
   if (is_mm_relaxed (model))
     return "ldclr\t%w2, %w0, %1";
   else if (is_mm_acquire (model) || is_mm_consume (model))
     return "ldclra\t%w2, %w0, %1";
   else if (is_mm_release (model))
     return "ldclrl\t%w2, %w0, %1";
   else
     return "ldclral\t%w2, %w0, %1";
  }
}

static const char *
output_3721 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 402 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
   enum memmodel model = memmodel_from_int (INTVAL (operands[3]));
   if (is_mm_relaxed (model))
     return "ldeor\t%w2, %w0, %1";
   else if (is_mm_acquire (model) || is_mm_consume (model))
     return "ldeora\t%w2, %w0, %1";
   else if (is_mm_release (model))
     return "ldeorl\t%w2, %w0, %1";
   else
     return "ldeoral\t%w2, %w0, %1";
  }
}

static const char *
output_3722 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 402 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
   enum memmodel model = memmodel_from_int (INTVAL (operands[3]));
   if (is_mm_relaxed (model))
     return "ldadd\t%w2, %w0, %1";
   else if (is_mm_acquire (model) || is_mm_consume (model))
     return "ldadda\t%w2, %w0, %1";
   else if (is_mm_release (model))
     return "ldaddl\t%w2, %w0, %1";
   else
     return "ldaddal\t%w2, %w0, %1";
  }
}

static const char *
output_3723 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 402 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
   enum memmodel model = memmodel_from_int (INTVAL (operands[3]));
   if (is_mm_relaxed (model))
     return "ldset\t%x2, %x0, %1";
   else if (is_mm_acquire (model) || is_mm_consume (model))
     return "ldseta\t%x2, %x0, %1";
   else if (is_mm_release (model))
     return "ldsetl\t%x2, %x0, %1";
   else
     return "ldsetal\t%x2, %x0, %1";
  }
}

static const char *
output_3724 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 402 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
   enum memmodel model = memmodel_from_int (INTVAL (operands[3]));
   if (is_mm_relaxed (model))
     return "ldclr\t%x2, %x0, %1";
   else if (is_mm_acquire (model) || is_mm_consume (model))
     return "ldclra\t%x2, %x0, %1";
   else if (is_mm_release (model))
     return "ldclrl\t%x2, %x0, %1";
   else
     return "ldclral\t%x2, %x0, %1";
  }
}

static const char *
output_3725 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 402 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
   enum memmodel model = memmodel_from_int (INTVAL (operands[3]));
   if (is_mm_relaxed (model))
     return "ldeor\t%x2, %x0, %1";
   else if (is_mm_acquire (model) || is_mm_consume (model))
     return "ldeora\t%x2, %x0, %1";
   else if (is_mm_release (model))
     return "ldeorl\t%x2, %x0, %1";
   else
     return "ldeoral\t%x2, %x0, %1";
  }
}

static const char *
output_3726 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 402 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
   enum memmodel model = memmodel_from_int (INTVAL (operands[3]));
   if (is_mm_relaxed (model))
     return "ldadd\t%x2, %x0, %1";
   else if (is_mm_acquire (model) || is_mm_consume (model))
     return "ldadda\t%x2, %x0, %1";
   else if (is_mm_release (model))
     return "ldaddl\t%x2, %x0, %1";
   else
     return "ldaddal\t%x2, %x0, %1";
  }
}

static const char *
output_3755 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 523 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
    enum memmodel model = memmodel_from_int (INTVAL (operands[2]));
    if (is_mm_relaxed (model) || is_mm_consume (model) || is_mm_release (model))
      return "ldrb\t%w0, %1";
    else
      return "ldarb\t%w0, %1";
  }
}

static const char *
output_3756 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 523 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
    enum memmodel model = memmodel_from_int (INTVAL (operands[2]));
    if (is_mm_relaxed (model) || is_mm_consume (model) || is_mm_release (model))
      return "ldrh\t%w0, %1";
    else
      return "ldarh\t%w0, %1";
  }
}

static const char *
output_3757 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 523 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
    enum memmodel model = memmodel_from_int (INTVAL (operands[2]));
    if (is_mm_relaxed (model) || is_mm_consume (model) || is_mm_release (model))
      return "ldr\t%w0, %1";
    else
      return "ldar\t%w0, %1";
  }
}

static const char *
output_3758 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 523 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
    enum memmodel model = memmodel_from_int (INTVAL (operands[2]));
    if (is_mm_relaxed (model) || is_mm_consume (model) || is_mm_release (model))
      return "ldr\t%x0, %1";
    else
      return "ldar\t%x0, %1";
  }
}

static const char *
output_3759 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 539 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
    enum memmodel model = memmodel_from_int (INTVAL (operands[2]));
    if (is_mm_relaxed (model) || is_mm_consume (model) || is_mm_acquire (model))
      return "strb\t%w1, %0";
    else if (which_alternative == 0)
      return "stlrb\t%w1, %0";
    else
      return "stlurb\t%w1, %0";
  }
}

static const char *
output_3760 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 539 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
    enum memmodel model = memmodel_from_int (INTVAL (operands[2]));
    if (is_mm_relaxed (model) || is_mm_consume (model) || is_mm_acquire (model))
      return "strh\t%w1, %0";
    else if (which_alternative == 0)
      return "stlrh\t%w1, %0";
    else
      return "stlurh\t%w1, %0";
  }
}

static const char *
output_3761 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 539 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
    enum memmodel model = memmodel_from_int (INTVAL (operands[2]));
    if (is_mm_relaxed (model) || is_mm_consume (model) || is_mm_acquire (model))
      return "str\t%w1, %0";
    else if (which_alternative == 0)
      return "stlr\t%w1, %0";
    else
      return "stlur\t%w1, %0";
  }
}

static const char *
output_3762 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 539 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
    enum memmodel model = memmodel_from_int (INTVAL (operands[2]));
    if (is_mm_relaxed (model) || is_mm_consume (model) || is_mm_acquire (model))
      return "str\t%x1, %0";
    else if (which_alternative == 0)
      return "stlr\t%x1, %0";
    else
      return "stlur\t%x1, %0";
  }
}

static const char *
output_3763 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 559 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
    enum memmodel model = memmodel_from_int (INTVAL (operands[2]));
    if (is_mm_relaxed (model) || is_mm_consume (model) || is_mm_release (model))
      return "ldxrb\t%w0, %1";
    else
      return "ldaxrb\t%w0, %1";
  }
}

static const char *
output_3764 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 559 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
    enum memmodel model = memmodel_from_int (INTVAL (operands[2]));
    if (is_mm_relaxed (model) || is_mm_consume (model) || is_mm_release (model))
      return "ldxrh\t%w0, %1";
    else
      return "ldaxrh\t%w0, %1";
  }
}

static const char *
output_3765 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 575 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
    enum memmodel model = memmodel_from_int (INTVAL (operands[2]));
    if (is_mm_relaxed (model) || is_mm_consume (model) || is_mm_release (model))
      return "ldxr\t%w0, %1";
    else
      return "ldaxr\t%w0, %1";
  }
}

static const char *
output_3766 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 575 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
    enum memmodel model = memmodel_from_int (INTVAL (operands[2]));
    if (is_mm_relaxed (model) || is_mm_consume (model) || is_mm_release (model))
      return "ldxr\t%x0, %1";
    else
      return "ldaxr\t%x0, %1";
  }
}

static const char *
output_3767 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 593 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
    enum memmodel model = memmodel_from_int (INTVAL (operands[3]));
    if (is_mm_relaxed (model) || is_mm_consume (model) || is_mm_acquire (model))
      return "stxrb\t%w0, %w2, %1";
    else
      return "stlxrb\t%w0, %w2, %1";
  }
}

static const char *
output_3768 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 593 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
    enum memmodel model = memmodel_from_int (INTVAL (operands[3]));
    if (is_mm_relaxed (model) || is_mm_consume (model) || is_mm_acquire (model))
      return "stxrh\t%w0, %w2, %1";
    else
      return "stlxrh\t%w0, %w2, %1";
  }
}

static const char *
output_3769 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 593 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
    enum memmodel model = memmodel_from_int (INTVAL (operands[3]));
    if (is_mm_relaxed (model) || is_mm_consume (model) || is_mm_acquire (model))
      return "stxr\t%w0, %w2, %1";
    else
      return "stlxr\t%w0, %w2, %1";
  }
}

static const char *
output_3770 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 593 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
    enum memmodel model = memmodel_from_int (INTVAL (operands[3]));
    if (is_mm_relaxed (model) || is_mm_consume (model) || is_mm_acquire (model))
      return "stxr\t%w0, %x2, %1";
    else
      return "stlxr\t%w0, %x2, %1";
  }
}

static const char *
output_3771 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 629 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/atomics.md"
{
    enum memmodel model = memmodel_from_int (INTVAL (operands[1]));
    if (is_mm_acquire (model))
      return "dmb\tishld";
    else
      return "dmb\tish";
  }
}

static const char *
output_3779 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
  switch (which_alternative)
    {
    case 0: return "ldr\t%0, %1";
    case 1: return "str\t%1, %0";
    case 2: return "mov\t%0.d, %1.d";
    case 3:
       return aarch64_output_sve_mov_immediate (operands[1]);
      default: gcc_unreachable ();
    }
}

static const char *
output_3780 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
  switch (which_alternative)
    {
    case 0: return "ldr\t%0, %1";
    case 1: return "str\t%1, %0";
    case 2: return "mov\t%0.d, %1.d";
    case 3:
       return aarch64_output_sve_mov_immediate (operands[1]);
      default: gcc_unreachable ();
    }
}

static const char *
output_3781 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
  switch (which_alternative)
    {
    case 0: return "ldr\t%0, %1";
    case 1: return "str\t%1, %0";
    case 2: return "mov\t%0.d, %1.d";
    case 3:
       return aarch64_output_sve_mov_immediate (operands[1]);
      default: gcc_unreachable ();
    }
}

static const char *
output_3782 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
  switch (which_alternative)
    {
    case 0: return "ldr\t%0, %1";
    case 1: return "str\t%1, %0";
    case 2: return "mov\t%0.d, %1.d";
    case 3:
       return aarch64_output_sve_mov_immediate (operands[1]);
      default: gcc_unreachable ();
    }
}

static const char *
output_3783 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
  switch (which_alternative)
    {
    case 0: return "ldr\t%0, %1";
    case 1: return "str\t%1, %0";
    case 2: return "mov\t%0.d, %1.d";
    case 3:
       return aarch64_output_sve_mov_immediate (operands[1]);
      default: gcc_unreachable ();
    }
}

static const char *
output_3784 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
  switch (which_alternative)
    {
    case 0: return "ldr\t%0, %1";
    case 1: return "str\t%1, %0";
    case 2: return "mov\t%0.d, %1.d";
    case 3:
       return aarch64_output_sve_mov_immediate (operands[1]);
      default: gcc_unreachable ();
    }
}

static const char *
output_3785 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
  switch (which_alternative)
    {
    case 0: return "ldr\t%0, %1";
    case 1: return "str\t%1, %0";
    case 2: return "mov\t%0.d, %1.d";
    case 3:
       return aarch64_output_sve_mov_immediate (operands[1]);
      default: gcc_unreachable ();
    }
}

static const char *
output_3786 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
  switch (which_alternative)
    {
    case 0: return "mov\t%0.d, %1.d";
    case 1:
       return aarch64_output_sve_mov_immediate (operands[1]);
      default: gcc_unreachable ();
    }
}

static const char *
output_3787 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
  switch (which_alternative)
    {
    case 0: return "mov\t%0.d, %1.d";
    case 1:
       return aarch64_output_sve_mov_immediate (operands[1]);
      default: gcc_unreachable ();
    }
}

static const char *
output_3788 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
  switch (which_alternative)
    {
    case 0: return "mov\t%0.d, %1.d";
    case 1:
       return aarch64_output_sve_mov_immediate (operands[1]);
      default: gcc_unreachable ();
    }
}

static const char *
output_3789 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
  switch (which_alternative)
    {
    case 0: return "mov\t%0.d, %1.d";
    case 1:
       return aarch64_output_sve_mov_immediate (operands[1]);
      default: gcc_unreachable ();
    }
}

static const char *
output_3790 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
  switch (which_alternative)
    {
    case 0: return "mov\t%0.d, %1.d";
    case 1:
       return aarch64_output_sve_mov_immediate (operands[1]);
      default: gcc_unreachable ();
    }
}

static const char *
output_3791 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
  switch (which_alternative)
    {
    case 0: return "mov\t%0.d, %1.d";
    case 1:
       return aarch64_output_sve_mov_immediate (operands[1]);
      default: gcc_unreachable ();
    }
}

static const char *
output_3792 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
  switch (which_alternative)
    {
    case 0: return "mov\t%0.d, %1.d";
    case 1:
       return aarch64_output_sve_mov_immediate (operands[1]);
      default: gcc_unreachable ();
    }
}

static const char * const output_3793[] = {
  "#",
  "ld1b\t%0.b, %1/z, %2",
  "st1b\t%2.b, %1, %0",
};

static const char * const output_3794[] = {
  "#",
  "ld1h\t%0.h, %1/z, %2",
  "st1h\t%2.h, %1, %0",
};

static const char * const output_3795[] = {
  "#",
  "ld1w\t%0.s, %1/z, %2",
  "st1w\t%2.s, %1, %0",
};

static const char * const output_3796[] = {
  "#",
  "ld1d\t%0.d, %1/z, %2",
  "st1d\t%2.d, %1, %0",
};

static const char * const output_3797[] = {
  "#",
  "ld1h\t%0.h, %1/z, %2",
  "st1h\t%2.h, %1, %0",
};

static const char * const output_3798[] = {
  "#",
  "ld1w\t%0.s, %1/z, %2",
  "st1w\t%2.s, %1, %0",
};

static const char * const output_3799[] = {
  "#",
  "ld1d\t%0.d, %1/z, %2",
  "st1d\t%2.d, %1, %0",
};

static const char * const output_3814[] = {
  "ld1w\t%0.s, %5/z, [%2.s]",
  "ld1w\t%0.s, %5/z, [%1, %2.s, sxtw]",
  "ld1w\t%0.s, %5/z, [%1, %2.s, uxtw]",
  "ld1w\t%0.s, %5/z, [%1, %2.s, sxtw %p4]",
  "ld1w\t%0.s, %5/z, [%1, %2.s, uxtw %p4]",
};

static const char * const output_3815[] = {
  "ld1w\t%0.s, %5/z, [%2.s]",
  "ld1w\t%0.s, %5/z, [%1, %2.s, sxtw]",
  "ld1w\t%0.s, %5/z, [%1, %2.s, uxtw]",
  "ld1w\t%0.s, %5/z, [%1, %2.s, sxtw %p4]",
  "ld1w\t%0.s, %5/z, [%1, %2.s, uxtw %p4]",
};

static const char * const output_3816[] = {
  "ld1d\t%0.d, %5/z, [%2.d]",
  "ld1d\t%0.d, %5/z, [%1, %2.d]",
  "ld1d\t%0.d, %5/z, [%1, %2.d, lsl %p4]",
};

static const char * const output_3817[] = {
  "ld1d\t%0.d, %5/z, [%2.d]",
  "ld1d\t%0.d, %5/z, [%1, %2.d]",
  "ld1d\t%0.d, %5/z, [%1, %2.d, lsl %p4]",
};

static const char * const output_3818[] = {
  "st1w\t%4.s, %5, [%1.s]",
  "st1w\t%4.s, %5, [%0, %1.s, sxtw]",
  "st1w\t%4.s, %5, [%0, %1.s, uxtw]",
  "st1w\t%4.s, %5, [%0, %1.s, sxtw %p3]",
  "st1w\t%4.s, %5, [%0, %1.s, uxtw %p3]",
};

static const char * const output_3819[] = {
  "st1w\t%4.s, %5, [%1.s]",
  "st1w\t%4.s, %5, [%0, %1.s, sxtw]",
  "st1w\t%4.s, %5, [%0, %1.s, uxtw]",
  "st1w\t%4.s, %5, [%0, %1.s, sxtw %p3]",
  "st1w\t%4.s, %5, [%0, %1.s, uxtw %p3]",
};

static const char * const output_3820[] = {
  "st1d\t%4.d, %5, [%1.d]",
  "st1d\t%4.d, %5, [%0, %1.d]",
  "st1d\t%4.d, %5, [%0, %1.d, lsl %p3]",
};

static const char * const output_3821[] = {
  "st1d\t%4.d, %5, [%1.d]",
  "st1d\t%4.d, %5, [%0, %1.d]",
  "st1d\t%4.d, %5, [%0, %1.d, lsl %p3]",
};

static const char *
output_3885 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
  switch (which_alternative)
    {
    case 0: return "mov\t%0.b, %1.b";
    case 1: return "str\t%1, %0";
    case 2: return "ldr\t%0, %1";
    case 3: return "pfalse\t%0.b";
    case 4:
       return aarch64_output_ptrue (VNx16BImode, 'b');
      default: gcc_unreachable ();
    }
}

static const char *
output_3886 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
  switch (which_alternative)
    {
    case 0: return "mov\t%0.b, %1.b";
    case 1: return "str\t%1, %0";
    case 2: return "ldr\t%0, %1";
    case 3: return "pfalse\t%0.b";
    case 4:
       return aarch64_output_ptrue (VNx8BImode, 'h');
      default: gcc_unreachable ();
    }
}

static const char *
output_3887 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
  switch (which_alternative)
    {
    case 0: return "mov\t%0.b, %1.b";
    case 1: return "str\t%1, %0";
    case 2: return "ldr\t%0, %1";
    case 3: return "pfalse\t%0.b";
    case 4:
       return aarch64_output_ptrue (VNx4BImode, 's');
      default: gcc_unreachable ();
    }
}

static const char *
output_3888 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
  switch (which_alternative)
    {
    case 0: return "mov\t%0.b, %1.b";
    case 1: return "str\t%1, %0";
    case 2: return "ldr\t%0, %1";
    case 3: return "pfalse\t%0.b";
    case 4:
       return aarch64_output_ptrue (VNx2BImode, 'd');
      default: gcc_unreachable ();
    }
}

static const char *
output_3889 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 526 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-sve.md"
{
    operands[1] = gen_rtx_REG (V16QImode, REGNO (operands[1]));
    switch (which_alternative)
      {
	case 0:
	  return "umov\t%w0, %1.b[0]";
	case 1:
	  return "#";
	case 2:
	  return "st1\t{%1.b}[0], %0";
	default:
	  gcc_unreachable ();
      }
  }
}

static const char *
output_3890 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 526 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-sve.md"
{
    operands[1] = gen_rtx_REG (V8HImode, REGNO (operands[1]));
    switch (which_alternative)
      {
	case 0:
	  return "umov\t%w0, %1.h[0]";
	case 1:
	  return "#";
	case 2:
	  return "st1\t{%1.h}[0], %0";
	default:
	  gcc_unreachable ();
      }
  }
}

static const char *
output_3891 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 526 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-sve.md"
{
    operands[1] = gen_rtx_REG (V4SImode, REGNO (operands[1]));
    switch (which_alternative)
      {
	case 0:
	  return "umov\t%w0, %1.s[0]";
	case 1:
	  return "#";
	case 2:
	  return "st1\t{%1.s}[0], %0";
	default:
	  gcc_unreachable ();
      }
  }
}

static const char *
output_3892 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 526 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-sve.md"
{
    operands[1] = gen_rtx_REG (V2DImode, REGNO (operands[1]));
    switch (which_alternative)
      {
	case 0:
	  return "umov\t%x0, %1.d[0]";
	case 1:
	  return "#";
	case 2:
	  return "st1\t{%1.d}[0], %0";
	default:
	  gcc_unreachable ();
      }
  }
}

static const char *
output_3893 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 526 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-sve.md"
{
    operands[1] = gen_rtx_REG (V8HFmode, REGNO (operands[1]));
    switch (which_alternative)
      {
	case 0:
	  return "umov\t%w0, %1.h[0]";
	case 1:
	  return "#";
	case 2:
	  return "st1\t{%1.h}[0], %0";
	default:
	  gcc_unreachable ();
      }
  }
}

static const char *
output_3894 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 526 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-sve.md"
{
    operands[1] = gen_rtx_REG (V4SFmode, REGNO (operands[1]));
    switch (which_alternative)
      {
	case 0:
	  return "umov\t%w0, %1.s[0]";
	case 1:
	  return "#";
	case 2:
	  return "st1\t{%1.s}[0], %0";
	default:
	  gcc_unreachable ();
      }
  }
}

static const char *
output_3895 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 526 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-sve.md"
{
    operands[1] = gen_rtx_REG (V2DFmode, REGNO (operands[1]));
    switch (which_alternative)
      {
	case 0:
	  return "umov\t%x0, %1.d[0]";
	case 1:
	  return "#";
	case 2:
	  return "st1\t{%1.d}[0], %0";
	default:
	  gcc_unreachable ();
      }
  }
}

static const char *
output_3896 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 561 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-sve.md"
{
    operands[1] = gen_rtx_REG (V16QImode, REGNO (operands[1]));
    switch (which_alternative)
      {
	case 0:
	  return "umov\t%w0, %1.b[%2]";
	case 1:
	  return "dup\t%b0, %1.b[%2]";
	case 2:
	  return "st1\t{%1.b}[%2], %0";
	default:
	  gcc_unreachable ();
      }
  }
}

static const char *
output_3897 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 561 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-sve.md"
{
    operands[1] = gen_rtx_REG (V8HImode, REGNO (operands[1]));
    switch (which_alternative)
      {
	case 0:
	  return "umov\t%w0, %1.h[%2]";
	case 1:
	  return "dup\t%h0, %1.h[%2]";
	case 2:
	  return "st1\t{%1.h}[%2], %0";
	default:
	  gcc_unreachable ();
      }
  }
}

static const char *
output_3898 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 561 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-sve.md"
{
    operands[1] = gen_rtx_REG (V4SImode, REGNO (operands[1]));
    switch (which_alternative)
      {
	case 0:
	  return "umov\t%w0, %1.s[%2]";
	case 1:
	  return "dup\t%s0, %1.s[%2]";
	case 2:
	  return "st1\t{%1.s}[%2], %0";
	default:
	  gcc_unreachable ();
      }
  }
}

static const char *
output_3899 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 561 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-sve.md"
{
    operands[1] = gen_rtx_REG (V2DImode, REGNO (operands[1]));
    switch (which_alternative)
      {
	case 0:
	  return "umov\t%x0, %1.d[%2]";
	case 1:
	  return "dup\t%d0, %1.d[%2]";
	case 2:
	  return "st1\t{%1.d}[%2], %0";
	default:
	  gcc_unreachable ();
      }
  }
}

static const char *
output_3900 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 561 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-sve.md"
{
    operands[1] = gen_rtx_REG (V8HFmode, REGNO (operands[1]));
    switch (which_alternative)
      {
	case 0:
	  return "umov\t%w0, %1.h[%2]";
	case 1:
	  return "dup\t%h0, %1.h[%2]";
	case 2:
	  return "st1\t{%1.h}[%2], %0";
	default:
	  gcc_unreachable ();
      }
  }
}

static const char *
output_3901 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 561 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-sve.md"
{
    operands[1] = gen_rtx_REG (V4SFmode, REGNO (operands[1]));
    switch (which_alternative)
      {
	case 0:
	  return "umov\t%w0, %1.s[%2]";
	case 1:
	  return "dup\t%s0, %1.s[%2]";
	case 2:
	  return "st1\t{%1.s}[%2], %0";
	default:
	  gcc_unreachable ();
      }
  }
}

static const char *
output_3902 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 561 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-sve.md"
{
    operands[1] = gen_rtx_REG (V2DFmode, REGNO (operands[1]));
    switch (which_alternative)
      {
	case 0:
	  return "umov\t%x0, %1.d[%2]";
	case 1:
	  return "dup\t%d0, %1.d[%2]";
	case 2:
	  return "st1\t{%1.d}[%2], %0";
	default:
	  gcc_unreachable ();
      }
  }
}

static const char *
output_3903 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 587 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-sve.md"
{
    operands[0] = gen_rtx_REG (VNx16QImode, REGNO (operands[0]));
    return "dup\t%0.b, %1.b[%2]";
  }
}

static const char *
output_3904 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 587 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-sve.md"
{
    operands[0] = gen_rtx_REG (VNx8HImode, REGNO (operands[0]));
    return "dup\t%0.h, %1.h[%2]";
  }
}

static const char *
output_3905 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 587 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-sve.md"
{
    operands[0] = gen_rtx_REG (VNx4SImode, REGNO (operands[0]));
    return "dup\t%0.s, %1.s[%2]";
  }
}

static const char *
output_3906 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 587 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-sve.md"
{
    operands[0] = gen_rtx_REG (VNx2DImode, REGNO (operands[0]));
    return "dup\t%0.d, %1.d[%2]";
  }
}

static const char *
output_3907 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 587 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-sve.md"
{
    operands[0] = gen_rtx_REG (VNx8HFmode, REGNO (operands[0]));
    return "dup\t%0.h, %1.h[%2]";
  }
}

static const char *
output_3908 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 587 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-sve.md"
{
    operands[0] = gen_rtx_REG (VNx4SFmode, REGNO (operands[0]));
    return "dup\t%0.s, %1.s[%2]";
  }
}

static const char *
output_3909 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 587 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-sve.md"
{
    operands[0] = gen_rtx_REG (VNx2DFmode, REGNO (operands[0]));
    return "dup\t%0.d, %1.d[%2]";
  }
}

static const char *
output_3910 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 601 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-sve.md"
{
    operands[0] = gen_rtx_REG (VNx16QImode, REGNO (operands[0]));
    operands[2] = GEN_INT (INTVAL (operands[2]) * GET_MODE_SIZE (QImode));
    return "ext\t%0.b, %0.b, %0.b, #%2";
  }
}

static const char *
output_3911 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 601 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-sve.md"
{
    operands[0] = gen_rtx_REG (VNx8HImode, REGNO (operands[0]));
    operands[2] = GEN_INT (INTVAL (operands[2]) * GET_MODE_SIZE (HImode));
    return "ext\t%0.b, %0.b, %0.b, #%2";
  }
}

static const char *
output_3912 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 601 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-sve.md"
{
    operands[0] = gen_rtx_REG (VNx4SImode, REGNO (operands[0]));
    operands[2] = GEN_INT (INTVAL (operands[2]) * GET_MODE_SIZE (SImode));
    return "ext\t%0.b, %0.b, %0.b, #%2";
  }
}

static const char *
output_3913 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 601 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-sve.md"
{
    operands[0] = gen_rtx_REG (VNx2DImode, REGNO (operands[0]));
    operands[2] = GEN_INT (INTVAL (operands[2]) * GET_MODE_SIZE (DImode));
    return "ext\t%0.b, %0.b, %0.b, #%2";
  }
}

static const char *
output_3914 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 601 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-sve.md"
{
    operands[0] = gen_rtx_REG (VNx8HFmode, REGNO (operands[0]));
    operands[2] = GEN_INT (INTVAL (operands[2]) * GET_MODE_SIZE (HFmode));
    return "ext\t%0.b, %0.b, %0.b, #%2";
  }
}

static const char *
output_3915 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 601 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-sve.md"
{
    operands[0] = gen_rtx_REG (VNx4SFmode, REGNO (operands[0]));
    operands[2] = GEN_INT (INTVAL (operands[2]) * GET_MODE_SIZE (SFmode));
    return "ext\t%0.b, %0.b, %0.b, #%2";
  }
}

static const char *
output_3916 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 601 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-sve.md"
{
    operands[0] = gen_rtx_REG (VNx2DFmode, REGNO (operands[0]));
    operands[2] = GEN_INT (INTVAL (operands[2]) * GET_MODE_SIZE (DFmode));
    return "ext\t%0.b, %0.b, %0.b, #%2";
  }
}

static const char * const output_3917[] = {
  "lastb\t%w0, %1, %2.b",
  "lastb\t%b0, %1, %2.b",
};

static const char * const output_3918[] = {
  "lastb\t%w0, %1, %2.h",
  "lastb\t%h0, %1, %2.h",
};

static const char * const output_3919[] = {
  "lastb\t%w0, %1, %2.s",
  "lastb\t%s0, %1, %2.s",
};

static const char * const output_3920[] = {
  "lastb\t%x0, %1, %2.d",
  "lastb\t%d0, %1, %2.d",
};

static const char * const output_3921[] = {
  "lastb\t%w0, %1, %2.h",
  "lastb\t%h0, %1, %2.h",
};

static const char * const output_3922[] = {
  "lastb\t%w0, %1, %2.s",
  "lastb\t%s0, %1, %2.s",
};

static const char * const output_3923[] = {
  "lastb\t%x0, %1, %2.d",
  "lastb\t%d0, %1, %2.d",
};

static const char * const output_3924[] = {
  "mov\t%0.b, %w1",
  "mov\t%0.b, %b1",
  "#",
};

static const char * const output_3925[] = {
  "mov\t%0.h, %w1",
  "mov\t%0.h, %h1",
  "#",
};

static const char * const output_3926[] = {
  "mov\t%0.s, %w1",
  "mov\t%0.s, %s1",
  "#",
};

static const char * const output_3927[] = {
  "mov\t%0.d, %x1",
  "mov\t%0.d, %d1",
  "#",
};

static const char * const output_3928[] = {
  "mov\t%0.h, %w1",
  "mov\t%0.h, %h1",
  "#",
};

static const char * const output_3929[] = {
  "mov\t%0.s, %w1",
  "mov\t%0.s, %s1",
  "#",
};

static const char * const output_3930[] = {
  "mov\t%0.d, %x1",
  "mov\t%0.d, %d1",
  "#",
};

static const char * const output_3945[] = {
  "index\t%0.b, #%1, %w2",
  "index\t%0.b, %w1, #%2",
  "index\t%0.b, %w1, %w2",
};

static const char * const output_3946[] = {
  "index\t%0.h, #%1, %w2",
  "index\t%0.h, %w1, #%2",
  "index\t%0.h, %w1, %w2",
};

static const char * const output_3947[] = {
  "index\t%0.s, #%1, %w2",
  "index\t%0.s, %w1, #%2",
  "index\t%0.s, %w1, %w2",
};

static const char * const output_3948[] = {
  "index\t%0.d, #%1, %x2",
  "index\t%0.d, %x1, #%2",
  "index\t%0.d, %x1, %x2",
};

static const char *
output_3949 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 732 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-sve.md"
{
    operands[2] = aarch64_check_zero_based_sve_index_immediate (operands[2]);
    return "index\t%0.b, %w1, #%2";
  }
}

static const char *
output_3950 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 732 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-sve.md"
{
    operands[2] = aarch64_check_zero_based_sve_index_immediate (operands[2]);
    return "index\t%0.h, %w1, #%2";
  }
}

static const char *
output_3951 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 732 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-sve.md"
{
    operands[2] = aarch64_check_zero_based_sve_index_immediate (operands[2]);
    return "index\t%0.s, %w1, #%2";
  }
}

static const char *
output_3952 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 732 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-sve.md"
{
    operands[2] = aarch64_check_zero_based_sve_index_immediate (operands[2]);
    return "index\t%0.d, %x1, #%2";
  }
}

static const char *
output_4091 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 894 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-sve.md"
{
    operands[3] = GEN_INT (INTVAL (operands[3]) * GET_MODE_SIZE (QImode));
    return "ext\t%0.b, %0.b, %2.b, #%3";
  }
}

static const char *
output_4092 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 894 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-sve.md"
{
    operands[3] = GEN_INT (INTVAL (operands[3]) * GET_MODE_SIZE (HImode));
    return "ext\t%0.b, %0.b, %2.b, #%3";
  }
}

static const char *
output_4093 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 894 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-sve.md"
{
    operands[3] = GEN_INT (INTVAL (operands[3]) * GET_MODE_SIZE (SImode));
    return "ext\t%0.b, %0.b, %2.b, #%3";
  }
}

static const char *
output_4094 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 894 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-sve.md"
{
    operands[3] = GEN_INT (INTVAL (operands[3]) * GET_MODE_SIZE (DImode));
    return "ext\t%0.b, %0.b, %2.b, #%3";
  }
}

static const char *
output_4095 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 894 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-sve.md"
{
    operands[3] = GEN_INT (INTVAL (operands[3]) * GET_MODE_SIZE (HFmode));
    return "ext\t%0.b, %0.b, %2.b, #%3";
  }
}

static const char *
output_4096 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 894 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-sve.md"
{
    operands[3] = GEN_INT (INTVAL (operands[3]) * GET_MODE_SIZE (SFmode));
    return "ext\t%0.b, %0.b, %2.b, #%3";
  }
}

static const char *
output_4097 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
#line 894 "../../../../../../../work-shared/gcc-9.2.0-r0/gcc-9.2.0/gcc/config/aarch64/aarch64-sve.md"
{
    operands[3] = GEN_INT (INTVAL (operands[3]) * GET_MODE_SIZE (DFmode));
    return "ext\t%0.b, %0.b, %2.b, #%3";
  }
}

static const char *
output_4098 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
  switch (which_alternative)
    {
    case 0: return "add\t%0.b, %0.b, #%D2";
    case 1: return "sub\t%0.b, %0.b, #%N2";
    case 2:
       return aarch64_output_sve_inc_dec_immediate ("%0.b", operands[2]);
    case 3: return "add\t%0.b, %1.b, %2.b";
      default: gcc_unreachable ();
    }
}

static const char *
output_4099 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
  switch (which_alternative)
    {
    case 0: return "add\t%0.h, %0.h, #%D2";
    case 1: return "sub\t%0.h, %0.h, #%N2";
    case 2:
       return aarch64_output_sve_inc_dec_immediate ("%0.h", operands[2]);
    case 3: return "add\t%0.h, %1.h, %2.h";
      default: gcc_unreachable ();
    }
}

static const char *
output_4100 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
  switch (which_alternative)
    {
    case 0: return "add\t%0.s, %0.s, #%D2";
    case 1: return "sub\t%0.s, %0.s, #%N2";
    case 2:
       return aarch64_output_sve_inc_dec_immediate ("%0.s", operands[2]);
    case 3: return "add\t%0.s, %1.s, %2.s";
      default: gcc_unreachable ();
    }
}

static const char *
output_4101 (rtx *operands ATTRIBUTE_UNUSED, rtx_insn *insn ATTRIBUTE_UNUSED)
{
  switch (which_alternative)
    {
    case 0: return "add\t%0.d, %0.d, #%D2";
    case 1: return "sub\t%0.d, %0.d, #%N2";
    case 2:
       return aarch64_output_sve_inc_dec_immediate ("%0.d", operands[2]);
    case 3: return "add\t%0.d, %1.d, %2.d";
      default: gcc_unreachable ();
    }
}

static const char * const output_4102[] = {
  "sub\t%0.b, %1.b, %2.b",
  "subr\t%0.b, %0.b, #%D1",
};

static const char * const output_4103[] = {
  "sub\t%0.h, %1.h, %2.h",
  "subr\t%0.h, %0.h, #%D1",
};

static const char * const output_4104[] = {
  "sub\t%0.s, %1.s, %2.s",
  "subr\t%0.s, %0.s, #%D1",
};

static const char * const output_4105[] = {
  "sub\t%0.d, %1.d, %2.d",
  "subr\t%0.d, %0.d, #%D1",
};

static const char * const output_4106[] = {
  "#",
  "mul\t%0.b, %1/m, %0.b, %3.b",
  "movprfx\t%0, %2\n\tmul\t%0.b, %1/m, %0.b, %3.b",
};

static const char * const output_4107[] = {
  "#",
  "mul\t%0.h, %1/m, %0.h, %3.h",
  "movprfx\t%0, %2\n\tmul\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_4108[] = {
  "#",
  "mul\t%0.s, %1/m, %0.s, %3.s",
  "movprfx\t%0, %2\n\tmul\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4109[] = {
  "#",
  "mul\t%0.d, %1/m, %0.d, %3.d",
  "movprfx\t%0, %2\n\tmul\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_4114[] = {
  "mad\t%0.b, %1/m, %3.b, %4.b",
  "mla\t%0.b, %1/m, %2.b, %3.b",
  "movprfx\t%0, %4\n\tmla\t%0.b, %1/m, %2.b, %3.b",
};

static const char * const output_4115[] = {
  "mad\t%0.h, %1/m, %3.h, %4.h",
  "mla\t%0.h, %1/m, %2.h, %3.h",
  "movprfx\t%0, %4\n\tmla\t%0.h, %1/m, %2.h, %3.h",
};

static const char * const output_4116[] = {
  "mad\t%0.s, %1/m, %3.s, %4.s",
  "mla\t%0.s, %1/m, %2.s, %3.s",
  "movprfx\t%0, %4\n\tmla\t%0.s, %1/m, %2.s, %3.s",
};

static const char * const output_4117[] = {
  "mad\t%0.d, %1/m, %3.d, %4.d",
  "mla\t%0.d, %1/m, %2.d, %3.d",
  "movprfx\t%0, %4\n\tmla\t%0.d, %1/m, %2.d, %3.d",
};

static const char * const output_4118[] = {
  "msb\t%0.b, %1/m, %3.b, %4.b",
  "mls\t%0.b, %1/m, %2.b, %3.b",
  "movprfx\t%0, %4\n\tmls\t%0.b, %1/m, %2.b, %3.b",
};

static const char * const output_4119[] = {
  "msb\t%0.h, %1/m, %3.h, %4.h",
  "mls\t%0.h, %1/m, %2.h, %3.h",
  "movprfx\t%0, %4\n\tmls\t%0.h, %1/m, %2.h, %3.h",
};

static const char * const output_4120[] = {
  "msb\t%0.s, %1/m, %3.s, %4.s",
  "mls\t%0.s, %1/m, %2.s, %3.s",
  "movprfx\t%0, %4\n\tmls\t%0.s, %1/m, %2.s, %3.s",
};

static const char * const output_4121[] = {
  "msb\t%0.d, %1/m, %3.d, %4.d",
  "mls\t%0.d, %1/m, %2.d, %3.d",
  "movprfx\t%0, %4\n\tmls\t%0.d, %1/m, %2.d, %3.d",
};

static const char * const output_4122[] = {
  "smulh\t%0.b, %1/m, %0.b, %3.b",
  "movprfx\t%0, %2\n\tsmulh\t%0.b, %1/m, %0.b, %3.b",
};

static const char * const output_4123[] = {
  "umulh\t%0.b, %1/m, %0.b, %3.b",
  "movprfx\t%0, %2\n\tumulh\t%0.b, %1/m, %0.b, %3.b",
};

static const char * const output_4124[] = {
  "smulh\t%0.h, %1/m, %0.h, %3.h",
  "movprfx\t%0, %2\n\tsmulh\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_4125[] = {
  "umulh\t%0.h, %1/m, %0.h, %3.h",
  "movprfx\t%0, %2\n\tumulh\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_4126[] = {
  "smulh\t%0.s, %1/m, %0.s, %3.s",
  "movprfx\t%0, %2\n\tsmulh\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4127[] = {
  "umulh\t%0.s, %1/m, %0.s, %3.s",
  "movprfx\t%0, %2\n\tumulh\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4128[] = {
  "smulh\t%0.d, %1/m, %0.d, %3.d",
  "movprfx\t%0, %2\n\tsmulh\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_4129[] = {
  "umulh\t%0.d, %1/m, %0.d, %3.d",
  "movprfx\t%0, %2\n\tumulh\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_4130[] = {
  "sdiv\t%0.s, %1/m, %0.s, %3.s",
  "sdivr\t%0.s, %1/m, %0.s, %2.s",
  "movprfx\t%0, %2\n\tsdiv\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4131[] = {
  "udiv\t%0.s, %1/m, %0.s, %3.s",
  "udivr\t%0.s, %1/m, %0.s, %2.s",
  "movprfx\t%0, %2\n\tudiv\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4132[] = {
  "sdiv\t%0.d, %1/m, %0.d, %3.d",
  "sdivr\t%0.d, %1/m, %0.d, %2.d",
  "movprfx\t%0, %2\n\tsdiv\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_4133[] = {
  "udiv\t%0.d, %1/m, %0.d, %3.d",
  "udivr\t%0.d, %1/m, %0.d, %2.d",
  "movprfx\t%0, %2\n\tudiv\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_4150[] = {
  "and\t%0.b, %0.b, #%C2",
  "and\t%0.d, %1.d, %2.d",
};

static const char * const output_4151[] = {
  "orr\t%0.b, %0.b, #%C2",
  "orr\t%0.d, %1.d, %2.d",
};

static const char * const output_4152[] = {
  "eor\t%0.b, %0.b, #%C2",
  "eor\t%0.d, %1.d, %2.d",
};

static const char * const output_4153[] = {
  "and\t%0.h, %0.h, #%C2",
  "and\t%0.d, %1.d, %2.d",
};

static const char * const output_4154[] = {
  "orr\t%0.h, %0.h, #%C2",
  "orr\t%0.d, %1.d, %2.d",
};

static const char * const output_4155[] = {
  "eor\t%0.h, %0.h, #%C2",
  "eor\t%0.d, %1.d, %2.d",
};

static const char * const output_4156[] = {
  "and\t%0.s, %0.s, #%C2",
  "and\t%0.d, %1.d, %2.d",
};

static const char * const output_4157[] = {
  "orr\t%0.s, %0.s, #%C2",
  "orr\t%0.d, %1.d, %2.d",
};

static const char * const output_4158[] = {
  "eor\t%0.s, %0.s, #%C2",
  "eor\t%0.d, %1.d, %2.d",
};

static const char * const output_4159[] = {
  "and\t%0.d, %0.d, #%C2",
  "and\t%0.d, %1.d, %2.d",
};

static const char * const output_4160[] = {
  "orr\t%0.d, %0.d, #%C2",
  "orr\t%0.d, %1.d, %2.d",
};

static const char * const output_4161[] = {
  "eor\t%0.d, %0.d, #%C2",
  "eor\t%0.d, %1.d, %2.d",
};

static const char * const output_4223[] = {
  "#",
  "lsl\t%0.b, %1/m, %0.b, %3.b",
  "movprfx\t%0, %2\n\tlsl\t%0.b, %1/m, %0.b, %3.b",
};

static const char * const output_4224[] = {
  "#",
  "asr\t%0.b, %1/m, %0.b, %3.b",
  "movprfx\t%0, %2\n\tasr\t%0.b, %1/m, %0.b, %3.b",
};

static const char * const output_4225[] = {
  "#",
  "lsr\t%0.b, %1/m, %0.b, %3.b",
  "movprfx\t%0, %2\n\tlsr\t%0.b, %1/m, %0.b, %3.b",
};

static const char * const output_4226[] = {
  "#",
  "lsl\t%0.h, %1/m, %0.h, %3.h",
  "movprfx\t%0, %2\n\tlsl\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_4227[] = {
  "#",
  "asr\t%0.h, %1/m, %0.h, %3.h",
  "movprfx\t%0, %2\n\tasr\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_4228[] = {
  "#",
  "lsr\t%0.h, %1/m, %0.h, %3.h",
  "movprfx\t%0, %2\n\tlsr\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_4229[] = {
  "#",
  "lsl\t%0.s, %1/m, %0.s, %3.s",
  "movprfx\t%0, %2\n\tlsl\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4230[] = {
  "#",
  "asr\t%0.s, %1/m, %0.s, %3.s",
  "movprfx\t%0, %2\n\tasr\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4231[] = {
  "#",
  "lsr\t%0.s, %1/m, %0.s, %3.s",
  "movprfx\t%0, %2\n\tlsr\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4232[] = {
  "#",
  "lsl\t%0.d, %1/m, %0.d, %3.d",
  "movprfx\t%0, %2\n\tlsl\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_4233[] = {
  "#",
  "asr\t%0.d, %1/m, %0.d, %3.d",
  "movprfx\t%0, %2\n\tasr\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_4234[] = {
  "#",
  "lsr\t%0.d, %1/m, %0.d, %3.d",
  "movprfx\t%0, %2\n\tlsr\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_4267[] = {
  "cmplt\t%0.b, %1/z, %2.b, #%3",
  "cmplt\t%0.b, %1/z, %2.b, %3.b",
};

static const char * const output_4268[] = {
  "cmple\t%0.b, %1/z, %2.b, #%3",
  "cmple\t%0.b, %1/z, %2.b, %3.b",
};

static const char * const output_4269[] = {
  "cmpeq\t%0.b, %1/z, %2.b, #%3",
  "cmpeq\t%0.b, %1/z, %2.b, %3.b",
};

static const char * const output_4270[] = {
  "cmpne\t%0.b, %1/z, %2.b, #%3",
  "cmpne\t%0.b, %1/z, %2.b, %3.b",
};

static const char * const output_4271[] = {
  "cmpge\t%0.b, %1/z, %2.b, #%3",
  "cmpge\t%0.b, %1/z, %2.b, %3.b",
};

static const char * const output_4272[] = {
  "cmpgt\t%0.b, %1/z, %2.b, #%3",
  "cmpgt\t%0.b, %1/z, %2.b, %3.b",
};

static const char * const output_4273[] = {
  "cmplo\t%0.b, %1/z, %2.b, #%3",
  "cmplo\t%0.b, %1/z, %2.b, %3.b",
};

static const char * const output_4274[] = {
  "cmpls\t%0.b, %1/z, %2.b, #%3",
  "cmpls\t%0.b, %1/z, %2.b, %3.b",
};

static const char * const output_4275[] = {
  "cmphs\t%0.b, %1/z, %2.b, #%3",
  "cmphs\t%0.b, %1/z, %2.b, %3.b",
};

static const char * const output_4276[] = {
  "cmphi\t%0.b, %1/z, %2.b, #%3",
  "cmphi\t%0.b, %1/z, %2.b, %3.b",
};

static const char * const output_4277[] = {
  "cmplt\t%0.h, %1/z, %2.h, #%3",
  "cmplt\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4278[] = {
  "cmple\t%0.h, %1/z, %2.h, #%3",
  "cmple\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4279[] = {
  "cmpeq\t%0.h, %1/z, %2.h, #%3",
  "cmpeq\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4280[] = {
  "cmpne\t%0.h, %1/z, %2.h, #%3",
  "cmpne\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4281[] = {
  "cmpge\t%0.h, %1/z, %2.h, #%3",
  "cmpge\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4282[] = {
  "cmpgt\t%0.h, %1/z, %2.h, #%3",
  "cmpgt\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4283[] = {
  "cmplo\t%0.h, %1/z, %2.h, #%3",
  "cmplo\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4284[] = {
  "cmpls\t%0.h, %1/z, %2.h, #%3",
  "cmpls\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4285[] = {
  "cmphs\t%0.h, %1/z, %2.h, #%3",
  "cmphs\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4286[] = {
  "cmphi\t%0.h, %1/z, %2.h, #%3",
  "cmphi\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4287[] = {
  "cmplt\t%0.s, %1/z, %2.s, #%3",
  "cmplt\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4288[] = {
  "cmple\t%0.s, %1/z, %2.s, #%3",
  "cmple\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4289[] = {
  "cmpeq\t%0.s, %1/z, %2.s, #%3",
  "cmpeq\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4290[] = {
  "cmpne\t%0.s, %1/z, %2.s, #%3",
  "cmpne\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4291[] = {
  "cmpge\t%0.s, %1/z, %2.s, #%3",
  "cmpge\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4292[] = {
  "cmpgt\t%0.s, %1/z, %2.s, #%3",
  "cmpgt\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4293[] = {
  "cmplo\t%0.s, %1/z, %2.s, #%3",
  "cmplo\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4294[] = {
  "cmpls\t%0.s, %1/z, %2.s, #%3",
  "cmpls\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4295[] = {
  "cmphs\t%0.s, %1/z, %2.s, #%3",
  "cmphs\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4296[] = {
  "cmphi\t%0.s, %1/z, %2.s, #%3",
  "cmphi\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4297[] = {
  "cmplt\t%0.d, %1/z, %2.d, #%3",
  "cmplt\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4298[] = {
  "cmple\t%0.d, %1/z, %2.d, #%3",
  "cmple\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4299[] = {
  "cmpeq\t%0.d, %1/z, %2.d, #%3",
  "cmpeq\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4300[] = {
  "cmpne\t%0.d, %1/z, %2.d, #%3",
  "cmpne\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4301[] = {
  "cmpge\t%0.d, %1/z, %2.d, #%3",
  "cmpge\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4302[] = {
  "cmpgt\t%0.d, %1/z, %2.d, #%3",
  "cmpgt\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4303[] = {
  "cmplo\t%0.d, %1/z, %2.d, #%3",
  "cmplo\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4304[] = {
  "cmpls\t%0.d, %1/z, %2.d, #%3",
  "cmpls\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4305[] = {
  "cmphs\t%0.d, %1/z, %2.d, #%3",
  "cmphs\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4306[] = {
  "cmphi\t%0.d, %1/z, %2.d, #%3",
  "cmphi\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4307[] = {
  "cmplt\t%0.b, %1/z, %2.b, #%3",
  "cmplt\t%0.b, %1/z, %2.b, %3.b",
};

static const char * const output_4308[] = {
  "cmple\t%0.b, %1/z, %2.b, #%3",
  "cmple\t%0.b, %1/z, %2.b, %3.b",
};

static const char * const output_4309[] = {
  "cmpeq\t%0.b, %1/z, %2.b, #%3",
  "cmpeq\t%0.b, %1/z, %2.b, %3.b",
};

static const char * const output_4310[] = {
  "cmpne\t%0.b, %1/z, %2.b, #%3",
  "cmpne\t%0.b, %1/z, %2.b, %3.b",
};

static const char * const output_4311[] = {
  "cmpge\t%0.b, %1/z, %2.b, #%3",
  "cmpge\t%0.b, %1/z, %2.b, %3.b",
};

static const char * const output_4312[] = {
  "cmpgt\t%0.b, %1/z, %2.b, #%3",
  "cmpgt\t%0.b, %1/z, %2.b, %3.b",
};

static const char * const output_4313[] = {
  "cmplo\t%0.b, %1/z, %2.b, #%3",
  "cmplo\t%0.b, %1/z, %2.b, %3.b",
};

static const char * const output_4314[] = {
  "cmpls\t%0.b, %1/z, %2.b, #%3",
  "cmpls\t%0.b, %1/z, %2.b, %3.b",
};

static const char * const output_4315[] = {
  "cmphs\t%0.b, %1/z, %2.b, #%3",
  "cmphs\t%0.b, %1/z, %2.b, %3.b",
};

static const char * const output_4316[] = {
  "cmphi\t%0.b, %1/z, %2.b, #%3",
  "cmphi\t%0.b, %1/z, %2.b, %3.b",
};

static const char * const output_4317[] = {
  "cmplt\t%0.h, %1/z, %2.h, #%3",
  "cmplt\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4318[] = {
  "cmple\t%0.h, %1/z, %2.h, #%3",
  "cmple\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4319[] = {
  "cmpeq\t%0.h, %1/z, %2.h, #%3",
  "cmpeq\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4320[] = {
  "cmpne\t%0.h, %1/z, %2.h, #%3",
  "cmpne\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4321[] = {
  "cmpge\t%0.h, %1/z, %2.h, #%3",
  "cmpge\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4322[] = {
  "cmpgt\t%0.h, %1/z, %2.h, #%3",
  "cmpgt\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4323[] = {
  "cmplo\t%0.h, %1/z, %2.h, #%3",
  "cmplo\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4324[] = {
  "cmpls\t%0.h, %1/z, %2.h, #%3",
  "cmpls\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4325[] = {
  "cmphs\t%0.h, %1/z, %2.h, #%3",
  "cmphs\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4326[] = {
  "cmphi\t%0.h, %1/z, %2.h, #%3",
  "cmphi\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4327[] = {
  "cmplt\t%0.s, %1/z, %2.s, #%3",
  "cmplt\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4328[] = {
  "cmple\t%0.s, %1/z, %2.s, #%3",
  "cmple\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4329[] = {
  "cmpeq\t%0.s, %1/z, %2.s, #%3",
  "cmpeq\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4330[] = {
  "cmpne\t%0.s, %1/z, %2.s, #%3",
  "cmpne\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4331[] = {
  "cmpge\t%0.s, %1/z, %2.s, #%3",
  "cmpge\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4332[] = {
  "cmpgt\t%0.s, %1/z, %2.s, #%3",
  "cmpgt\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4333[] = {
  "cmplo\t%0.s, %1/z, %2.s, #%3",
  "cmplo\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4334[] = {
  "cmpls\t%0.s, %1/z, %2.s, #%3",
  "cmpls\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4335[] = {
  "cmphs\t%0.s, %1/z, %2.s, #%3",
  "cmphs\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4336[] = {
  "cmphi\t%0.s, %1/z, %2.s, #%3",
  "cmphi\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4337[] = {
  "cmplt\t%0.d, %1/z, %2.d, #%3",
  "cmplt\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4338[] = {
  "cmple\t%0.d, %1/z, %2.d, #%3",
  "cmple\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4339[] = {
  "cmpeq\t%0.d, %1/z, %2.d, #%3",
  "cmpeq\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4340[] = {
  "cmpne\t%0.d, %1/z, %2.d, #%3",
  "cmpne\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4341[] = {
  "cmpge\t%0.d, %1/z, %2.d, #%3",
  "cmpge\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4342[] = {
  "cmpgt\t%0.d, %1/z, %2.d, #%3",
  "cmpgt\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4343[] = {
  "cmplo\t%0.d, %1/z, %2.d, #%3",
  "cmplo\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4344[] = {
  "cmpls\t%0.d, %1/z, %2.d, #%3",
  "cmpls\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4345[] = {
  "cmphs\t%0.d, %1/z, %2.d, #%3",
  "cmphs\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4346[] = {
  "cmphi\t%0.d, %1/z, %2.d, #%3",
  "cmphi\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4347[] = {
  "cmplt\t%0.b, %1/z, %2.b, #%3",
  "cmplt\t%0.b, %1/z, %2.b, %3.b",
};

static const char * const output_4348[] = {
  "cmple\t%0.b, %1/z, %2.b, #%3",
  "cmple\t%0.b, %1/z, %2.b, %3.b",
};

static const char * const output_4349[] = {
  "cmpeq\t%0.b, %1/z, %2.b, #%3",
  "cmpeq\t%0.b, %1/z, %2.b, %3.b",
};

static const char * const output_4350[] = {
  "cmpne\t%0.b, %1/z, %2.b, #%3",
  "cmpne\t%0.b, %1/z, %2.b, %3.b",
};

static const char * const output_4351[] = {
  "cmpge\t%0.b, %1/z, %2.b, #%3",
  "cmpge\t%0.b, %1/z, %2.b, %3.b",
};

static const char * const output_4352[] = {
  "cmpgt\t%0.b, %1/z, %2.b, #%3",
  "cmpgt\t%0.b, %1/z, %2.b, %3.b",
};

static const char * const output_4353[] = {
  "cmplo\t%0.b, %1/z, %2.b, #%3",
  "cmplo\t%0.b, %1/z, %2.b, %3.b",
};

static const char * const output_4354[] = {
  "cmpls\t%0.b, %1/z, %2.b, #%3",
  "cmpls\t%0.b, %1/z, %2.b, %3.b",
};

static const char * const output_4355[] = {
  "cmphs\t%0.b, %1/z, %2.b, #%3",
  "cmphs\t%0.b, %1/z, %2.b, %3.b",
};

static const char * const output_4356[] = {
  "cmphi\t%0.b, %1/z, %2.b, #%3",
  "cmphi\t%0.b, %1/z, %2.b, %3.b",
};

static const char * const output_4357[] = {
  "cmplt\t%0.h, %1/z, %2.h, #%3",
  "cmplt\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4358[] = {
  "cmple\t%0.h, %1/z, %2.h, #%3",
  "cmple\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4359[] = {
  "cmpeq\t%0.h, %1/z, %2.h, #%3",
  "cmpeq\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4360[] = {
  "cmpne\t%0.h, %1/z, %2.h, #%3",
  "cmpne\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4361[] = {
  "cmpge\t%0.h, %1/z, %2.h, #%3",
  "cmpge\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4362[] = {
  "cmpgt\t%0.h, %1/z, %2.h, #%3",
  "cmpgt\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4363[] = {
  "cmplo\t%0.h, %1/z, %2.h, #%3",
  "cmplo\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4364[] = {
  "cmpls\t%0.h, %1/z, %2.h, #%3",
  "cmpls\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4365[] = {
  "cmphs\t%0.h, %1/z, %2.h, #%3",
  "cmphs\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4366[] = {
  "cmphi\t%0.h, %1/z, %2.h, #%3",
  "cmphi\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4367[] = {
  "cmplt\t%0.s, %1/z, %2.s, #%3",
  "cmplt\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4368[] = {
  "cmple\t%0.s, %1/z, %2.s, #%3",
  "cmple\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4369[] = {
  "cmpeq\t%0.s, %1/z, %2.s, #%3",
  "cmpeq\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4370[] = {
  "cmpne\t%0.s, %1/z, %2.s, #%3",
  "cmpne\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4371[] = {
  "cmpge\t%0.s, %1/z, %2.s, #%3",
  "cmpge\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4372[] = {
  "cmpgt\t%0.s, %1/z, %2.s, #%3",
  "cmpgt\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4373[] = {
  "cmplo\t%0.s, %1/z, %2.s, #%3",
  "cmplo\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4374[] = {
  "cmpls\t%0.s, %1/z, %2.s, #%3",
  "cmpls\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4375[] = {
  "cmphs\t%0.s, %1/z, %2.s, #%3",
  "cmphs\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4376[] = {
  "cmphi\t%0.s, %1/z, %2.s, #%3",
  "cmphi\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4377[] = {
  "cmplt\t%0.d, %1/z, %2.d, #%3",
  "cmplt\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4378[] = {
  "cmple\t%0.d, %1/z, %2.d, #%3",
  "cmple\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4379[] = {
  "cmpeq\t%0.d, %1/z, %2.d, #%3",
  "cmpeq\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4380[] = {
  "cmpne\t%0.d, %1/z, %2.d, #%3",
  "cmpne\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4381[] = {
  "cmpge\t%0.d, %1/z, %2.d, #%3",
  "cmpge\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4382[] = {
  "cmpgt\t%0.d, %1/z, %2.d, #%3",
  "cmpgt\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4383[] = {
  "cmplo\t%0.d, %1/z, %2.d, #%3",
  "cmplo\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4384[] = {
  "cmpls\t%0.d, %1/z, %2.d, #%3",
  "cmpls\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4385[] = {
  "cmphs\t%0.d, %1/z, %2.d, #%3",
  "cmphs\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4386[] = {
  "cmphi\t%0.d, %1/z, %2.d, #%3",
  "cmphi\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4427[] = {
  "cmplt\t%0.b, %1/z, %2.b, #%3",
  "cmplt\t%0.b, %1/z, %2.b, %3.b",
};

static const char * const output_4428[] = {
  "cmple\t%0.b, %1/z, %2.b, #%3",
  "cmple\t%0.b, %1/z, %2.b, %3.b",
};

static const char * const output_4429[] = {
  "cmpeq\t%0.b, %1/z, %2.b, #%3",
  "cmpeq\t%0.b, %1/z, %2.b, %3.b",
};

static const char * const output_4430[] = {
  "cmpne\t%0.b, %1/z, %2.b, #%3",
  "cmpne\t%0.b, %1/z, %2.b, %3.b",
};

static const char * const output_4431[] = {
  "cmpge\t%0.b, %1/z, %2.b, #%3",
  "cmpge\t%0.b, %1/z, %2.b, %3.b",
};

static const char * const output_4432[] = {
  "cmpgt\t%0.b, %1/z, %2.b, #%3",
  "cmpgt\t%0.b, %1/z, %2.b, %3.b",
};

static const char * const output_4433[] = {
  "cmplo\t%0.b, %1/z, %2.b, #%3",
  "cmplo\t%0.b, %1/z, %2.b, %3.b",
};

static const char * const output_4434[] = {
  "cmpls\t%0.b, %1/z, %2.b, #%3",
  "cmpls\t%0.b, %1/z, %2.b, %3.b",
};

static const char * const output_4435[] = {
  "cmphs\t%0.b, %1/z, %2.b, #%3",
  "cmphs\t%0.b, %1/z, %2.b, %3.b",
};

static const char * const output_4436[] = {
  "cmphi\t%0.b, %1/z, %2.b, #%3",
  "cmphi\t%0.b, %1/z, %2.b, %3.b",
};

static const char * const output_4437[] = {
  "cmplt\t%0.h, %1/z, %2.h, #%3",
  "cmplt\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4438[] = {
  "cmple\t%0.h, %1/z, %2.h, #%3",
  "cmple\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4439[] = {
  "cmpeq\t%0.h, %1/z, %2.h, #%3",
  "cmpeq\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4440[] = {
  "cmpne\t%0.h, %1/z, %2.h, #%3",
  "cmpne\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4441[] = {
  "cmpge\t%0.h, %1/z, %2.h, #%3",
  "cmpge\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4442[] = {
  "cmpgt\t%0.h, %1/z, %2.h, #%3",
  "cmpgt\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4443[] = {
  "cmplo\t%0.h, %1/z, %2.h, #%3",
  "cmplo\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4444[] = {
  "cmpls\t%0.h, %1/z, %2.h, #%3",
  "cmpls\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4445[] = {
  "cmphs\t%0.h, %1/z, %2.h, #%3",
  "cmphs\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4446[] = {
  "cmphi\t%0.h, %1/z, %2.h, #%3",
  "cmphi\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4447[] = {
  "cmplt\t%0.s, %1/z, %2.s, #%3",
  "cmplt\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4448[] = {
  "cmple\t%0.s, %1/z, %2.s, #%3",
  "cmple\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4449[] = {
  "cmpeq\t%0.s, %1/z, %2.s, #%3",
  "cmpeq\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4450[] = {
  "cmpne\t%0.s, %1/z, %2.s, #%3",
  "cmpne\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4451[] = {
  "cmpge\t%0.s, %1/z, %2.s, #%3",
  "cmpge\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4452[] = {
  "cmpgt\t%0.s, %1/z, %2.s, #%3",
  "cmpgt\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4453[] = {
  "cmplo\t%0.s, %1/z, %2.s, #%3",
  "cmplo\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4454[] = {
  "cmpls\t%0.s, %1/z, %2.s, #%3",
  "cmpls\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4455[] = {
  "cmphs\t%0.s, %1/z, %2.s, #%3",
  "cmphs\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4456[] = {
  "cmphi\t%0.s, %1/z, %2.s, #%3",
  "cmphi\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4457[] = {
  "cmplt\t%0.d, %1/z, %2.d, #%3",
  "cmplt\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4458[] = {
  "cmple\t%0.d, %1/z, %2.d, #%3",
  "cmple\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4459[] = {
  "cmpeq\t%0.d, %1/z, %2.d, #%3",
  "cmpeq\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4460[] = {
  "cmpne\t%0.d, %1/z, %2.d, #%3",
  "cmpne\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4461[] = {
  "cmpge\t%0.d, %1/z, %2.d, #%3",
  "cmpge\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4462[] = {
  "cmpgt\t%0.d, %1/z, %2.d, #%3",
  "cmpgt\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4463[] = {
  "cmplo\t%0.d, %1/z, %2.d, #%3",
  "cmplo\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4464[] = {
  "cmpls\t%0.d, %1/z, %2.d, #%3",
  "cmpls\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4465[] = {
  "cmphs\t%0.d, %1/z, %2.d, #%3",
  "cmphs\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4466[] = {
  "cmphi\t%0.d, %1/z, %2.d, #%3",
  "cmphi\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4467[] = {
  "fcmlt\t%0.h, %1/z, %2.h, #0.0",
  "fcmlt\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4468[] = {
  "fcmle\t%0.h, %1/z, %2.h, #0.0",
  "fcmle\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4469[] = {
  "fcmeq\t%0.h, %1/z, %2.h, #0.0",
  "fcmeq\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4470[] = {
  "fcmne\t%0.h, %1/z, %2.h, #0.0",
  "fcmne\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4471[] = {
  "fcmge\t%0.h, %1/z, %2.h, #0.0",
  "fcmge\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4472[] = {
  "fcmgt\t%0.h, %1/z, %2.h, #0.0",
  "fcmgt\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4473[] = {
  "fcmlt\t%0.s, %1/z, %2.s, #0.0",
  "fcmlt\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4474[] = {
  "fcmle\t%0.s, %1/z, %2.s, #0.0",
  "fcmle\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4475[] = {
  "fcmeq\t%0.s, %1/z, %2.s, #0.0",
  "fcmeq\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4476[] = {
  "fcmne\t%0.s, %1/z, %2.s, #0.0",
  "fcmne\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4477[] = {
  "fcmge\t%0.s, %1/z, %2.s, #0.0",
  "fcmge\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4478[] = {
  "fcmgt\t%0.s, %1/z, %2.s, #0.0",
  "fcmgt\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4479[] = {
  "fcmlt\t%0.d, %1/z, %2.d, #0.0",
  "fcmlt\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4480[] = {
  "fcmle\t%0.d, %1/z, %2.d, #0.0",
  "fcmle\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4481[] = {
  "fcmeq\t%0.d, %1/z, %2.d, #0.0",
  "fcmeq\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4482[] = {
  "fcmne\t%0.d, %1/z, %2.d, #0.0",
  "fcmne\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4483[] = {
  "fcmge\t%0.d, %1/z, %2.d, #0.0",
  "fcmge\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4484[] = {
  "fcmgt\t%0.d, %1/z, %2.d, #0.0",
  "fcmgt\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4509[] = {
  "fcmlt\t%0.h, %1/z, %2.h, #0.0",
  "fcmlt\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4510[] = {
  "fcmle\t%0.h, %1/z, %2.h, #0.0",
  "fcmle\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4511[] = {
  "fcmeq\t%0.h, %1/z, %2.h, #0.0",
  "fcmeq\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4512[] = {
  "fcmne\t%0.h, %1/z, %2.h, #0.0",
  "fcmne\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4513[] = {
  "fcmge\t%0.h, %1/z, %2.h, #0.0",
  "fcmge\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4514[] = {
  "fcmgt\t%0.h, %1/z, %2.h, #0.0",
  "fcmgt\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4515[] = {
  "fcmlt\t%0.s, %1/z, %2.s, #0.0",
  "fcmlt\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4516[] = {
  "fcmle\t%0.s, %1/z, %2.s, #0.0",
  "fcmle\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4517[] = {
  "fcmeq\t%0.s, %1/z, %2.s, #0.0",
  "fcmeq\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4518[] = {
  "fcmne\t%0.s, %1/z, %2.s, #0.0",
  "fcmne\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4519[] = {
  "fcmge\t%0.s, %1/z, %2.s, #0.0",
  "fcmge\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4520[] = {
  "fcmgt\t%0.s, %1/z, %2.s, #0.0",
  "fcmgt\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4521[] = {
  "fcmlt\t%0.d, %1/z, %2.d, #0.0",
  "fcmlt\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4522[] = {
  "fcmle\t%0.d, %1/z, %2.d, #0.0",
  "fcmle\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4523[] = {
  "fcmeq\t%0.d, %1/z, %2.d, #0.0",
  "fcmeq\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4524[] = {
  "fcmne\t%0.d, %1/z, %2.d, #0.0",
  "fcmne\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4525[] = {
  "fcmge\t%0.d, %1/z, %2.d, #0.0",
  "fcmge\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4526[] = {
  "fcmgt\t%0.d, %1/z, %2.d, #0.0",
  "fcmgt\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4530[] = {
  "fcmlt\t%0.h, %1/z, %2.h, #0.0",
  "fcmlt\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4531[] = {
  "fcmle\t%0.h, %1/z, %2.h, #0.0",
  "fcmle\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4532[] = {
  "fcmeq\t%0.h, %1/z, %2.h, #0.0",
  "fcmeq\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4533[] = {
  "fcmne\t%0.h, %1/z, %2.h, #0.0",
  "fcmne\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4534[] = {
  "fcmge\t%0.h, %1/z, %2.h, #0.0",
  "fcmge\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4535[] = {
  "fcmgt\t%0.h, %1/z, %2.h, #0.0",
  "fcmgt\t%0.h, %1/z, %2.h, %3.h",
};

static const char * const output_4536[] = {
  "fcmlt\t%0.s, %1/z, %2.s, #0.0",
  "fcmlt\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4537[] = {
  "fcmle\t%0.s, %1/z, %2.s, #0.0",
  "fcmle\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4538[] = {
  "fcmeq\t%0.s, %1/z, %2.s, #0.0",
  "fcmeq\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4539[] = {
  "fcmne\t%0.s, %1/z, %2.s, #0.0",
  "fcmne\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4540[] = {
  "fcmge\t%0.s, %1/z, %2.s, #0.0",
  "fcmge\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4541[] = {
  "fcmgt\t%0.s, %1/z, %2.s, #0.0",
  "fcmgt\t%0.s, %1/z, %2.s, %3.s",
};

static const char * const output_4542[] = {
  "fcmlt\t%0.d, %1/z, %2.d, #0.0",
  "fcmlt\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4543[] = {
  "fcmle\t%0.d, %1/z, %2.d, #0.0",
  "fcmle\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4544[] = {
  "fcmeq\t%0.d, %1/z, %2.d, #0.0",
  "fcmeq\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4545[] = {
  "fcmne\t%0.d, %1/z, %2.d, #0.0",
  "fcmne\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4546[] = {
  "fcmge\t%0.d, %1/z, %2.d, #0.0",
  "fcmge\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4547[] = {
  "fcmgt\t%0.d, %1/z, %2.d, #0.0",
  "fcmgt\t%0.d, %1/z, %2.d, %3.d",
};

static const char * const output_4559[] = {
  "smax\t%0.b, %1/m, %0.b, %3.b",
  "movprfx\t%0, %2\n\tsmax\t%0.b, %1/m, %0.b, %3.b",
};

static const char * const output_4560[] = {
  "smin\t%0.b, %1/m, %0.b, %3.b",
  "movprfx\t%0, %2\n\tsmin\t%0.b, %1/m, %0.b, %3.b",
};

static const char * const output_4561[] = {
  "umax\t%0.b, %1/m, %0.b, %3.b",
  "movprfx\t%0, %2\n\tumax\t%0.b, %1/m, %0.b, %3.b",
};

static const char * const output_4562[] = {
  "umin\t%0.b, %1/m, %0.b, %3.b",
  "movprfx\t%0, %2\n\tumin\t%0.b, %1/m, %0.b, %3.b",
};

static const char * const output_4563[] = {
  "smax\t%0.h, %1/m, %0.h, %3.h",
  "movprfx\t%0, %2\n\tsmax\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_4564[] = {
  "smin\t%0.h, %1/m, %0.h, %3.h",
  "movprfx\t%0, %2\n\tsmin\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_4565[] = {
  "umax\t%0.h, %1/m, %0.h, %3.h",
  "movprfx\t%0, %2\n\tumax\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_4566[] = {
  "umin\t%0.h, %1/m, %0.h, %3.h",
  "movprfx\t%0, %2\n\tumin\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_4567[] = {
  "smax\t%0.s, %1/m, %0.s, %3.s",
  "movprfx\t%0, %2\n\tsmax\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4568[] = {
  "smin\t%0.s, %1/m, %0.s, %3.s",
  "movprfx\t%0, %2\n\tsmin\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4569[] = {
  "umax\t%0.s, %1/m, %0.s, %3.s",
  "movprfx\t%0, %2\n\tumax\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4570[] = {
  "umin\t%0.s, %1/m, %0.s, %3.s",
  "movprfx\t%0, %2\n\tumin\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4571[] = {
  "smax\t%0.d, %1/m, %0.d, %3.d",
  "movprfx\t%0, %2\n\tsmax\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_4572[] = {
  "smin\t%0.d, %1/m, %0.d, %3.d",
  "movprfx\t%0, %2\n\tsmin\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_4573[] = {
  "umax\t%0.d, %1/m, %0.d, %3.d",
  "movprfx\t%0, %2\n\tumax\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_4574[] = {
  "umin\t%0.d, %1/m, %0.d, %3.d",
  "movprfx\t%0, %2\n\tumin\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_4575[] = {
  "fmaxnm\t%0.h, %1/m, %0.h, %3.h",
  "movprfx\t%0, %2\n\tfmaxnm\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_4576[] = {
  "fminnm\t%0.h, %1/m, %0.h, %3.h",
  "movprfx\t%0, %2\n\tfminnm\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_4577[] = {
  "fmaxnm\t%0.s, %1/m, %0.s, %3.s",
  "movprfx\t%0, %2\n\tfmaxnm\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4578[] = {
  "fminnm\t%0.s, %1/m, %0.s, %3.s",
  "movprfx\t%0, %2\n\tfminnm\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4579[] = {
  "fmaxnm\t%0.d, %1/m, %0.d, %3.d",
  "movprfx\t%0, %2\n\tfmaxnm\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_4580[] = {
  "fminnm\t%0.d, %1/m, %0.d, %3.d",
  "movprfx\t%0, %2\n\tfminnm\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_4581[] = {
  "fmax\t%0.h, %1/m, %0.h, %3.h",
  "movprfx\t%0, %2\n\tfmax\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_4582[] = {
  "fmin\t%0.h, %1/m, %0.h, %3.h",
  "movprfx\t%0, %2\n\tfmin\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_4583[] = {
  "fmaxnm\t%0.h, %1/m, %0.h, %3.h",
  "movprfx\t%0, %2\n\tfmaxnm\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_4584[] = {
  "fminnm\t%0.h, %1/m, %0.h, %3.h",
  "movprfx\t%0, %2\n\tfminnm\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_4585[] = {
  "fmax\t%0.s, %1/m, %0.s, %3.s",
  "movprfx\t%0, %2\n\tfmax\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4586[] = {
  "fmin\t%0.s, %1/m, %0.s, %3.s",
  "movprfx\t%0, %2\n\tfmin\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4587[] = {
  "fmaxnm\t%0.s, %1/m, %0.s, %3.s",
  "movprfx\t%0, %2\n\tfmaxnm\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4588[] = {
  "fminnm\t%0.s, %1/m, %0.s, %3.s",
  "movprfx\t%0, %2\n\tfminnm\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4589[] = {
  "fmax\t%0.d, %1/m, %0.d, %3.d",
  "movprfx\t%0, %2\n\tfmax\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_4590[] = {
  "fmin\t%0.d, %1/m, %0.d, %3.d",
  "movprfx\t%0, %2\n\tfmin\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_4591[] = {
  "fmaxnm\t%0.d, %1/m, %0.d, %3.d",
  "movprfx\t%0, %2\n\tfmaxnm\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_4592[] = {
  "fminnm\t%0.d, %1/m, %0.d, %3.d",
  "movprfx\t%0, %2\n\tfminnm\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_4593[] = {
  "add\t%0.b, %1/m, %0.b, %3.b",
  "add\t%0.b, %1/m, %0.b, %2.b",
  "movprfx\t%0, %1/m, %2\n\tadd\t%0.b, %1/m, %0.b, %3.b",
};

static const char * const output_4594[] = {
  "sub\t%0.b, %1/m, %0.b, %3.b",
  "subr\t%0.b, %1/m, %0.b, %2.b",
  "movprfx\t%0, %1/m, %2\n\tsub\t%0.b, %1/m, %0.b, %3.b",
};

static const char * const output_4595[] = {
  "mul\t%0.b, %1/m, %0.b, %3.b",
  "mul\t%0.b, %1/m, %0.b, %2.b",
  "movprfx\t%0, %1/m, %2\n\tmul\t%0.b, %1/m, %0.b, %3.b",
};

static const char * const output_4596[] = {
  "smax\t%0.b, %1/m, %0.b, %3.b",
  "smax\t%0.b, %1/m, %0.b, %2.b",
  "movprfx\t%0, %1/m, %2\n\tsmax\t%0.b, %1/m, %0.b, %3.b",
};

static const char * const output_4597[] = {
  "umax\t%0.b, %1/m, %0.b, %3.b",
  "umax\t%0.b, %1/m, %0.b, %2.b",
  "movprfx\t%0, %1/m, %2\n\tumax\t%0.b, %1/m, %0.b, %3.b",
};

static const char * const output_4598[] = {
  "smin\t%0.b, %1/m, %0.b, %3.b",
  "smin\t%0.b, %1/m, %0.b, %2.b",
  "movprfx\t%0, %1/m, %2\n\tsmin\t%0.b, %1/m, %0.b, %3.b",
};

static const char * const output_4599[] = {
  "umin\t%0.b, %1/m, %0.b, %3.b",
  "umin\t%0.b, %1/m, %0.b, %2.b",
  "movprfx\t%0, %1/m, %2\n\tumin\t%0.b, %1/m, %0.b, %3.b",
};

static const char * const output_4600[] = {
  "and\t%0.b, %1/m, %0.b, %3.b",
  "and\t%0.b, %1/m, %0.b, %2.b",
  "movprfx\t%0, %1/m, %2\n\tand\t%0.b, %1/m, %0.b, %3.b",
};

static const char * const output_4601[] = {
  "orr\t%0.b, %1/m, %0.b, %3.b",
  "orr\t%0.b, %1/m, %0.b, %2.b",
  "movprfx\t%0, %1/m, %2\n\torr\t%0.b, %1/m, %0.b, %3.b",
};

static const char * const output_4602[] = {
  "eor\t%0.b, %1/m, %0.b, %3.b",
  "eor\t%0.b, %1/m, %0.b, %2.b",
  "movprfx\t%0, %1/m, %2\n\teor\t%0.b, %1/m, %0.b, %3.b",
};

static const char * const output_4603[] = {
  "add\t%0.h, %1/m, %0.h, %3.h",
  "add\t%0.h, %1/m, %0.h, %2.h",
  "movprfx\t%0, %1/m, %2\n\tadd\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_4604[] = {
  "sub\t%0.h, %1/m, %0.h, %3.h",
  "subr\t%0.h, %1/m, %0.h, %2.h",
  "movprfx\t%0, %1/m, %2\n\tsub\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_4605[] = {
  "mul\t%0.h, %1/m, %0.h, %3.h",
  "mul\t%0.h, %1/m, %0.h, %2.h",
  "movprfx\t%0, %1/m, %2\n\tmul\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_4606[] = {
  "smax\t%0.h, %1/m, %0.h, %3.h",
  "smax\t%0.h, %1/m, %0.h, %2.h",
  "movprfx\t%0, %1/m, %2\n\tsmax\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_4607[] = {
  "umax\t%0.h, %1/m, %0.h, %3.h",
  "umax\t%0.h, %1/m, %0.h, %2.h",
  "movprfx\t%0, %1/m, %2\n\tumax\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_4608[] = {
  "smin\t%0.h, %1/m, %0.h, %3.h",
  "smin\t%0.h, %1/m, %0.h, %2.h",
  "movprfx\t%0, %1/m, %2\n\tsmin\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_4609[] = {
  "umin\t%0.h, %1/m, %0.h, %3.h",
  "umin\t%0.h, %1/m, %0.h, %2.h",
  "movprfx\t%0, %1/m, %2\n\tumin\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_4610[] = {
  "and\t%0.h, %1/m, %0.h, %3.h",
  "and\t%0.h, %1/m, %0.h, %2.h",
  "movprfx\t%0, %1/m, %2\n\tand\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_4611[] = {
  "orr\t%0.h, %1/m, %0.h, %3.h",
  "orr\t%0.h, %1/m, %0.h, %2.h",
  "movprfx\t%0, %1/m, %2\n\torr\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_4612[] = {
  "eor\t%0.h, %1/m, %0.h, %3.h",
  "eor\t%0.h, %1/m, %0.h, %2.h",
  "movprfx\t%0, %1/m, %2\n\teor\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_4613[] = {
  "add\t%0.s, %1/m, %0.s, %3.s",
  "add\t%0.s, %1/m, %0.s, %2.s",
  "movprfx\t%0, %1/m, %2\n\tadd\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4614[] = {
  "sub\t%0.s, %1/m, %0.s, %3.s",
  "subr\t%0.s, %1/m, %0.s, %2.s",
  "movprfx\t%0, %1/m, %2\n\tsub\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4615[] = {
  "mul\t%0.s, %1/m, %0.s, %3.s",
  "mul\t%0.s, %1/m, %0.s, %2.s",
  "movprfx\t%0, %1/m, %2\n\tmul\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4616[] = {
  "smax\t%0.s, %1/m, %0.s, %3.s",
  "smax\t%0.s, %1/m, %0.s, %2.s",
  "movprfx\t%0, %1/m, %2\n\tsmax\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4617[] = {
  "umax\t%0.s, %1/m, %0.s, %3.s",
  "umax\t%0.s, %1/m, %0.s, %2.s",
  "movprfx\t%0, %1/m, %2\n\tumax\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4618[] = {
  "smin\t%0.s, %1/m, %0.s, %3.s",
  "smin\t%0.s, %1/m, %0.s, %2.s",
  "movprfx\t%0, %1/m, %2\n\tsmin\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4619[] = {
  "umin\t%0.s, %1/m, %0.s, %3.s",
  "umin\t%0.s, %1/m, %0.s, %2.s",
  "movprfx\t%0, %1/m, %2\n\tumin\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4620[] = {
  "and\t%0.s, %1/m, %0.s, %3.s",
  "and\t%0.s, %1/m, %0.s, %2.s",
  "movprfx\t%0, %1/m, %2\n\tand\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4621[] = {
  "orr\t%0.s, %1/m, %0.s, %3.s",
  "orr\t%0.s, %1/m, %0.s, %2.s",
  "movprfx\t%0, %1/m, %2\n\torr\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4622[] = {
  "eor\t%0.s, %1/m, %0.s, %3.s",
  "eor\t%0.s, %1/m, %0.s, %2.s",
  "movprfx\t%0, %1/m, %2\n\teor\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4623[] = {
  "add\t%0.d, %1/m, %0.d, %3.d",
  "add\t%0.d, %1/m, %0.d, %2.d",
  "movprfx\t%0, %1/m, %2\n\tadd\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_4624[] = {
  "sub\t%0.d, %1/m, %0.d, %3.d",
  "subr\t%0.d, %1/m, %0.d, %2.d",
  "movprfx\t%0, %1/m, %2\n\tsub\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_4625[] = {
  "mul\t%0.d, %1/m, %0.d, %3.d",
  "mul\t%0.d, %1/m, %0.d, %2.d",
  "movprfx\t%0, %1/m, %2\n\tmul\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_4626[] = {
  "smax\t%0.d, %1/m, %0.d, %3.d",
  "smax\t%0.d, %1/m, %0.d, %2.d",
  "movprfx\t%0, %1/m, %2\n\tsmax\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_4627[] = {
  "umax\t%0.d, %1/m, %0.d, %3.d",
  "umax\t%0.d, %1/m, %0.d, %2.d",
  "movprfx\t%0, %1/m, %2\n\tumax\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_4628[] = {
  "smin\t%0.d, %1/m, %0.d, %3.d",
  "smin\t%0.d, %1/m, %0.d, %2.d",
  "movprfx\t%0, %1/m, %2\n\tsmin\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_4629[] = {
  "umin\t%0.d, %1/m, %0.d, %3.d",
  "umin\t%0.d, %1/m, %0.d, %2.d",
  "movprfx\t%0, %1/m, %2\n\tumin\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_4630[] = {
  "and\t%0.d, %1/m, %0.d, %3.d",
  "and\t%0.d, %1/m, %0.d, %2.d",
  "movprfx\t%0, %1/m, %2\n\tand\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_4631[] = {
  "orr\t%0.d, %1/m, %0.d, %3.d",
  "orr\t%0.d, %1/m, %0.d, %2.d",
  "movprfx\t%0, %1/m, %2\n\torr\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_4632[] = {
  "eor\t%0.d, %1/m, %0.d, %3.d",
  "eor\t%0.d, %1/m, %0.d, %2.d",
  "movprfx\t%0, %1/m, %2\n\teor\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_4633[] = {
  "sdiv\t%0.s, %1/m, %0.s, %3.s",
  "sdivr\t%0.s, %1/m, %0.s, %2.s",
  "movprfx\t%0, %1/m, %2\n\tsdiv\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4634[] = {
  "udiv\t%0.s, %1/m, %0.s, %3.s",
  "udivr\t%0.s, %1/m, %0.s, %2.s",
  "movprfx\t%0, %1/m, %2\n\tudiv\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4635[] = {
  "sdiv\t%0.d, %1/m, %0.d, %3.d",
  "sdivr\t%0.d, %1/m, %0.d, %2.d",
  "movprfx\t%0, %1/m, %2\n\tsdiv\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_4636[] = {
  "udiv\t%0.d, %1/m, %0.d, %3.d",
  "udivr\t%0.d, %1/m, %0.d, %2.d",
  "movprfx\t%0, %1/m, %2\n\tudiv\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_4637[] = {
  "add\t%0.b, %1/m, %0.b, %3.b",
  "movprfx\t%0, %2\n\tadd\t%0.b, %1/m, %0.b, %3.b",
};

static const char * const output_4638[] = {
  "sub\t%0.b, %1/m, %0.b, %3.b",
  "movprfx\t%0, %2\n\tsub\t%0.b, %1/m, %0.b, %3.b",
};

static const char * const output_4639[] = {
  "mul\t%0.b, %1/m, %0.b, %3.b",
  "movprfx\t%0, %2\n\tmul\t%0.b, %1/m, %0.b, %3.b",
};

static const char * const output_4640[] = {
  "smax\t%0.b, %1/m, %0.b, %3.b",
  "movprfx\t%0, %2\n\tsmax\t%0.b, %1/m, %0.b, %3.b",
};

static const char * const output_4641[] = {
  "umax\t%0.b, %1/m, %0.b, %3.b",
  "movprfx\t%0, %2\n\tumax\t%0.b, %1/m, %0.b, %3.b",
};

static const char * const output_4642[] = {
  "smin\t%0.b, %1/m, %0.b, %3.b",
  "movprfx\t%0, %2\n\tsmin\t%0.b, %1/m, %0.b, %3.b",
};

static const char * const output_4643[] = {
  "umin\t%0.b, %1/m, %0.b, %3.b",
  "movprfx\t%0, %2\n\tumin\t%0.b, %1/m, %0.b, %3.b",
};

static const char * const output_4644[] = {
  "and\t%0.b, %1/m, %0.b, %3.b",
  "movprfx\t%0, %2\n\tand\t%0.b, %1/m, %0.b, %3.b",
};

static const char * const output_4645[] = {
  "orr\t%0.b, %1/m, %0.b, %3.b",
  "movprfx\t%0, %2\n\torr\t%0.b, %1/m, %0.b, %3.b",
};

static const char * const output_4646[] = {
  "eor\t%0.b, %1/m, %0.b, %3.b",
  "movprfx\t%0, %2\n\teor\t%0.b, %1/m, %0.b, %3.b",
};

static const char * const output_4647[] = {
  "add\t%0.h, %1/m, %0.h, %3.h",
  "movprfx\t%0, %2\n\tadd\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_4648[] = {
  "sub\t%0.h, %1/m, %0.h, %3.h",
  "movprfx\t%0, %2\n\tsub\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_4649[] = {
  "mul\t%0.h, %1/m, %0.h, %3.h",
  "movprfx\t%0, %2\n\tmul\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_4650[] = {
  "smax\t%0.h, %1/m, %0.h, %3.h",
  "movprfx\t%0, %2\n\tsmax\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_4651[] = {
  "umax\t%0.h, %1/m, %0.h, %3.h",
  "movprfx\t%0, %2\n\tumax\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_4652[] = {
  "smin\t%0.h, %1/m, %0.h, %3.h",
  "movprfx\t%0, %2\n\tsmin\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_4653[] = {
  "umin\t%0.h, %1/m, %0.h, %3.h",
  "movprfx\t%0, %2\n\tumin\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_4654[] = {
  "and\t%0.h, %1/m, %0.h, %3.h",
  "movprfx\t%0, %2\n\tand\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_4655[] = {
  "orr\t%0.h, %1/m, %0.h, %3.h",
  "movprfx\t%0, %2\n\torr\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_4656[] = {
  "eor\t%0.h, %1/m, %0.h, %3.h",
  "movprfx\t%0, %2\n\teor\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_4657[] = {
  "add\t%0.s, %1/m, %0.s, %3.s",
  "movprfx\t%0, %2\n\tadd\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4658[] = {
  "sub\t%0.s, %1/m, %0.s, %3.s",
  "movprfx\t%0, %2\n\tsub\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4659[] = {
  "mul\t%0.s, %1/m, %0.s, %3.s",
  "movprfx\t%0, %2\n\tmul\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4660[] = {
  "smax\t%0.s, %1/m, %0.s, %3.s",
  "movprfx\t%0, %2\n\tsmax\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4661[] = {
  "umax\t%0.s, %1/m, %0.s, %3.s",
  "movprfx\t%0, %2\n\tumax\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4662[] = {
  "smin\t%0.s, %1/m, %0.s, %3.s",
  "movprfx\t%0, %2\n\tsmin\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4663[] = {
  "umin\t%0.s, %1/m, %0.s, %3.s",
  "movprfx\t%0, %2\n\tumin\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4664[] = {
  "and\t%0.s, %1/m, %0.s, %3.s",
  "movprfx\t%0, %2\n\tand\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4665[] = {
  "orr\t%0.s, %1/m, %0.s, %3.s",
  "movprfx\t%0, %2\n\torr\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4666[] = {
  "eor\t%0.s, %1/m, %0.s, %3.s",
  "movprfx\t%0, %2\n\teor\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4667[] = {
  "add\t%0.d, %1/m, %0.d, %3.d",
  "movprfx\t%0, %2\n\tadd\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_4668[] = {
  "sub\t%0.d, %1/m, %0.d, %3.d",
  "movprfx\t%0, %2\n\tsub\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_4669[] = {
  "mul\t%0.d, %1/m, %0.d, %3.d",
  "movprfx\t%0, %2\n\tmul\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_4670[] = {
  "smax\t%0.d, %1/m, %0.d, %3.d",
  "movprfx\t%0, %2\n\tsmax\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_4671[] = {
  "umax\t%0.d, %1/m, %0.d, %3.d",
  "movprfx\t%0, %2\n\tumax\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_4672[] = {
  "smin\t%0.d, %1/m, %0.d, %3.d",
  "movprfx\t%0, %2\n\tsmin\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_4673[] = {
  "umin\t%0.d, %1/m, %0.d, %3.d",
  "movprfx\t%0, %2\n\tumin\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_4674[] = {
  "and\t%0.d, %1/m, %0.d, %3.d",
  "movprfx\t%0, %2\n\tand\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_4675[] = {
  "orr\t%0.d, %1/m, %0.d, %3.d",
  "movprfx\t%0, %2\n\torr\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_4676[] = {
  "eor\t%0.d, %1/m, %0.d, %3.d",
  "movprfx\t%0, %2\n\teor\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_4677[] = {
  "sdiv\t%0.s, %1/m, %0.s, %3.s",
  "movprfx\t%0, %2\n\tsdiv\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4678[] = {
  "udiv\t%0.s, %1/m, %0.s, %3.s",
  "movprfx\t%0, %2\n\tudiv\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4679[] = {
  "sdiv\t%0.d, %1/m, %0.d, %3.d",
  "movprfx\t%0, %2\n\tsdiv\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_4680[] = {
  "udiv\t%0.d, %1/m, %0.d, %3.d",
  "movprfx\t%0, %2\n\tudiv\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_4681[] = {
  "add\t%0.b, %1/m, %0.b, %2.b",
  "movprfx\t%0, %3\n\tadd\t%0.b, %1/m, %0.b, %2.b",
};

static const char * const output_4682[] = {
  "subr\t%0.b, %1/m, %0.b, %2.b",
  "movprfx\t%0, %3\n\tsubr\t%0.b, %1/m, %0.b, %2.b",
};

static const char * const output_4683[] = {
  "mul\t%0.b, %1/m, %0.b, %2.b",
  "movprfx\t%0, %3\n\tmul\t%0.b, %1/m, %0.b, %2.b",
};

static const char * const output_4684[] = {
  "smax\t%0.b, %1/m, %0.b, %2.b",
  "movprfx\t%0, %3\n\tsmax\t%0.b, %1/m, %0.b, %2.b",
};

static const char * const output_4685[] = {
  "umax\t%0.b, %1/m, %0.b, %2.b",
  "movprfx\t%0, %3\n\tumax\t%0.b, %1/m, %0.b, %2.b",
};

static const char * const output_4686[] = {
  "smin\t%0.b, %1/m, %0.b, %2.b",
  "movprfx\t%0, %3\n\tsmin\t%0.b, %1/m, %0.b, %2.b",
};

static const char * const output_4687[] = {
  "umin\t%0.b, %1/m, %0.b, %2.b",
  "movprfx\t%0, %3\n\tumin\t%0.b, %1/m, %0.b, %2.b",
};

static const char * const output_4688[] = {
  "and\t%0.b, %1/m, %0.b, %2.b",
  "movprfx\t%0, %3\n\tand\t%0.b, %1/m, %0.b, %2.b",
};

static const char * const output_4689[] = {
  "orr\t%0.b, %1/m, %0.b, %2.b",
  "movprfx\t%0, %3\n\torr\t%0.b, %1/m, %0.b, %2.b",
};

static const char * const output_4690[] = {
  "eor\t%0.b, %1/m, %0.b, %2.b",
  "movprfx\t%0, %3\n\teor\t%0.b, %1/m, %0.b, %2.b",
};

static const char * const output_4691[] = {
  "add\t%0.h, %1/m, %0.h, %2.h",
  "movprfx\t%0, %3\n\tadd\t%0.h, %1/m, %0.h, %2.h",
};

static const char * const output_4692[] = {
  "subr\t%0.h, %1/m, %0.h, %2.h",
  "movprfx\t%0, %3\n\tsubr\t%0.h, %1/m, %0.h, %2.h",
};

static const char * const output_4693[] = {
  "mul\t%0.h, %1/m, %0.h, %2.h",
  "movprfx\t%0, %3\n\tmul\t%0.h, %1/m, %0.h, %2.h",
};

static const char * const output_4694[] = {
  "smax\t%0.h, %1/m, %0.h, %2.h",
  "movprfx\t%0, %3\n\tsmax\t%0.h, %1/m, %0.h, %2.h",
};

static const char * const output_4695[] = {
  "umax\t%0.h, %1/m, %0.h, %2.h",
  "movprfx\t%0, %3\n\tumax\t%0.h, %1/m, %0.h, %2.h",
};

static const char * const output_4696[] = {
  "smin\t%0.h, %1/m, %0.h, %2.h",
  "movprfx\t%0, %3\n\tsmin\t%0.h, %1/m, %0.h, %2.h",
};

static const char * const output_4697[] = {
  "umin\t%0.h, %1/m, %0.h, %2.h",
  "movprfx\t%0, %3\n\tumin\t%0.h, %1/m, %0.h, %2.h",
};

static const char * const output_4698[] = {
  "and\t%0.h, %1/m, %0.h, %2.h",
  "movprfx\t%0, %3\n\tand\t%0.h, %1/m, %0.h, %2.h",
};

static const char * const output_4699[] = {
  "orr\t%0.h, %1/m, %0.h, %2.h",
  "movprfx\t%0, %3\n\torr\t%0.h, %1/m, %0.h, %2.h",
};

static const char * const output_4700[] = {
  "eor\t%0.h, %1/m, %0.h, %2.h",
  "movprfx\t%0, %3\n\teor\t%0.h, %1/m, %0.h, %2.h",
};

static const char * const output_4701[] = {
  "add\t%0.s, %1/m, %0.s, %2.s",
  "movprfx\t%0, %3\n\tadd\t%0.s, %1/m, %0.s, %2.s",
};

static const char * const output_4702[] = {
  "subr\t%0.s, %1/m, %0.s, %2.s",
  "movprfx\t%0, %3\n\tsubr\t%0.s, %1/m, %0.s, %2.s",
};

static const char * const output_4703[] = {
  "mul\t%0.s, %1/m, %0.s, %2.s",
  "movprfx\t%0, %3\n\tmul\t%0.s, %1/m, %0.s, %2.s",
};

static const char * const output_4704[] = {
  "smax\t%0.s, %1/m, %0.s, %2.s",
  "movprfx\t%0, %3\n\tsmax\t%0.s, %1/m, %0.s, %2.s",
};

static const char * const output_4705[] = {
  "umax\t%0.s, %1/m, %0.s, %2.s",
  "movprfx\t%0, %3\n\tumax\t%0.s, %1/m, %0.s, %2.s",
};

static const char * const output_4706[] = {
  "smin\t%0.s, %1/m, %0.s, %2.s",
  "movprfx\t%0, %3\n\tsmin\t%0.s, %1/m, %0.s, %2.s",
};

static const char * const output_4707[] = {
  "umin\t%0.s, %1/m, %0.s, %2.s",
  "movprfx\t%0, %3\n\tumin\t%0.s, %1/m, %0.s, %2.s",
};

static const char * const output_4708[] = {
  "and\t%0.s, %1/m, %0.s, %2.s",
  "movprfx\t%0, %3\n\tand\t%0.s, %1/m, %0.s, %2.s",
};

static const char * const output_4709[] = {
  "orr\t%0.s, %1/m, %0.s, %2.s",
  "movprfx\t%0, %3\n\torr\t%0.s, %1/m, %0.s, %2.s",
};

static const char * const output_4710[] = {
  "eor\t%0.s, %1/m, %0.s, %2.s",
  "movprfx\t%0, %3\n\teor\t%0.s, %1/m, %0.s, %2.s",
};

static const char * const output_4711[] = {
  "add\t%0.d, %1/m, %0.d, %2.d",
  "movprfx\t%0, %3\n\tadd\t%0.d, %1/m, %0.d, %2.d",
};

static const char * const output_4712[] = {
  "subr\t%0.d, %1/m, %0.d, %2.d",
  "movprfx\t%0, %3\n\tsubr\t%0.d, %1/m, %0.d, %2.d",
};

static const char * const output_4713[] = {
  "mul\t%0.d, %1/m, %0.d, %2.d",
  "movprfx\t%0, %3\n\tmul\t%0.d, %1/m, %0.d, %2.d",
};

static const char * const output_4714[] = {
  "smax\t%0.d, %1/m, %0.d, %2.d",
  "movprfx\t%0, %3\n\tsmax\t%0.d, %1/m, %0.d, %2.d",
};

static const char * const output_4715[] = {
  "umax\t%0.d, %1/m, %0.d, %2.d",
  "movprfx\t%0, %3\n\tumax\t%0.d, %1/m, %0.d, %2.d",
};

static const char * const output_4716[] = {
  "smin\t%0.d, %1/m, %0.d, %2.d",
  "movprfx\t%0, %3\n\tsmin\t%0.d, %1/m, %0.d, %2.d",
};

static const char * const output_4717[] = {
  "umin\t%0.d, %1/m, %0.d, %2.d",
  "movprfx\t%0, %3\n\tumin\t%0.d, %1/m, %0.d, %2.d",
};

static const char * const output_4718[] = {
  "and\t%0.d, %1/m, %0.d, %2.d",
  "movprfx\t%0, %3\n\tand\t%0.d, %1/m, %0.d, %2.d",
};

static const char * const output_4719[] = {
  "orr\t%0.d, %1/m, %0.d, %2.d",
  "movprfx\t%0, %3\n\torr\t%0.d, %1/m, %0.d, %2.d",
};

static const char * const output_4720[] = {
  "eor\t%0.d, %1/m, %0.d, %2.d",
  "movprfx\t%0, %3\n\teor\t%0.d, %1/m, %0.d, %2.d",
};

static const char * const output_4721[] = {
  "sdivr\t%0.s, %1/m, %0.s, %2.s",
  "movprfx\t%0, %3\n\tsdivr\t%0.s, %1/m, %0.s, %2.s",
};

static const char * const output_4722[] = {
  "udivr\t%0.s, %1/m, %0.s, %2.s",
  "movprfx\t%0, %3\n\tudivr\t%0.s, %1/m, %0.s, %2.s",
};

static const char * const output_4723[] = {
  "sdivr\t%0.d, %1/m, %0.d, %2.d",
  "movprfx\t%0, %3\n\tsdivr\t%0.d, %1/m, %0.d, %2.d",
};

static const char * const output_4724[] = {
  "udivr\t%0.d, %1/m, %0.d, %2.d",
  "movprfx\t%0, %3\n\tudivr\t%0.d, %1/m, %0.d, %2.d",
};

static const char * const output_4825[] = {
  "clastb\t%w0, %2, %w0, %3.b",
  "clastb\t%w0, %2, %w0, %3.b",
};

static const char * const output_4826[] = {
  "clastb\t%w0, %2, %w0, %3.h",
  "clastb\t%w0, %2, %w0, %3.h",
};

static const char * const output_4827[] = {
  "clastb\t%w0, %2, %w0, %3.s",
  "clastb\t%w0, %2, %w0, %3.s",
};

static const char * const output_4828[] = {
  "clastb\t%x0, %2, %x0, %3.d",
  "clastb\t%x0, %2, %x0, %3.d",
};

static const char * const output_4829[] = {
  "clastb\t%w0, %2, %w0, %3.h",
  "clastb\t%h0, %2, %h0, %3.h",
};

static const char * const output_4830[] = {
  "clastb\t%w0, %2, %w0, %3.s",
  "clastb\t%s0, %2, %s0, %3.s",
};

static const char * const output_4831[] = {
  "clastb\t%x0, %2, %x0, %3.d",
  "clastb\t%d0, %2, %d0, %3.d",
};

static const char * const output_4885[] = {
  "fadd\t%0.h, %1/m, %0.h, #%3",
  "fsub\t%0.h, %1/m, %0.h, #%N3",
  "#",
};

static const char * const output_4886[] = {
  "fadd\t%0.s, %1/m, %0.s, #%3",
  "fsub\t%0.s, %1/m, %0.s, #%N3",
  "#",
};

static const char * const output_4887[] = {
  "fadd\t%0.d, %1/m, %0.d, #%3",
  "fsub\t%0.d, %1/m, %0.d, #%N3",
  "#",
};

static const char * const output_4888[] = {
  "fsub\t%0.h, %1/m, %0.h, #%3",
  "fadd\t%0.h, %1/m, %0.h, #%N3",
  "fsubr\t%0.h, %1/m, %0.h, #%2",
  "#",
};

static const char * const output_4889[] = {
  "fsub\t%0.s, %1/m, %0.s, #%3",
  "fadd\t%0.s, %1/m, %0.s, #%N3",
  "fsubr\t%0.s, %1/m, %0.s, #%2",
  "#",
};

static const char * const output_4890[] = {
  "fsub\t%0.d, %1/m, %0.d, #%3",
  "fadd\t%0.d, %1/m, %0.d, #%N3",
  "fsubr\t%0.d, %1/m, %0.d, #%2",
  "#",
};

static const char * const output_4891[] = {
  "fmul\t%0.h, %1/m, %0.h, #%3",
  "#",
};

static const char * const output_4892[] = {
  "fmul\t%0.s, %1/m, %0.s, #%3",
  "#",
};

static const char * const output_4893[] = {
  "fmul\t%0.d, %1/m, %0.d, #%3",
  "#",
};

static const char * const output_4903[] = {
  "fmad\t%0.h, %1/m, %4.h, %2.h",
  "fmla\t%0.h, %1/m, %3.h, %4.h",
  "movprfx\t%0, %2\n\tfmla\t%0.h, %1/m, %3.h, %4.h",
};

static const char * const output_4904[] = {
  "fmad\t%0.s, %1/m, %4.s, %2.s",
  "fmla\t%0.s, %1/m, %3.s, %4.s",
  "movprfx\t%0, %2\n\tfmla\t%0.s, %1/m, %3.s, %4.s",
};

static const char * const output_4905[] = {
  "fmad\t%0.d, %1/m, %4.d, %2.d",
  "fmla\t%0.d, %1/m, %3.d, %4.d",
  "movprfx\t%0, %2\n\tfmla\t%0.d, %1/m, %3.d, %4.d",
};

static const char * const output_4906[] = {
  "fmsb\t%0.h, %1/m, %4.h, %2.h",
  "fmls\t%0.h, %1/m, %3.h, %4.h",
  "movprfx\t%0, %2\n\tfmls\t%0.h, %1/m, %3.h, %4.h",
};

static const char * const output_4907[] = {
  "fmsb\t%0.s, %1/m, %4.s, %2.s",
  "fmls\t%0.s, %1/m, %3.s, %4.s",
  "movprfx\t%0, %2\n\tfmls\t%0.s, %1/m, %3.s, %4.s",
};

static const char * const output_4908[] = {
  "fmsb\t%0.d, %1/m, %4.d, %2.d",
  "fmls\t%0.d, %1/m, %3.d, %4.d",
  "movprfx\t%0, %2\n\tfmls\t%0.d, %1/m, %3.d, %4.d",
};

static const char * const output_4909[] = {
  "fnmsb\t%0.h, %1/m, %4.h, %2.h",
  "fnmls\t%0.h, %1/m, %3.h, %4.h",
  "movprfx\t%0, %2\n\tfnmls\t%0.h, %1/m, %3.h, %4.h",
};

static const char * const output_4910[] = {
  "fnmsb\t%0.s, %1/m, %4.s, %2.s",
  "fnmls\t%0.s, %1/m, %3.s, %4.s",
  "movprfx\t%0, %2\n\tfnmls\t%0.s, %1/m, %3.s, %4.s",
};

static const char * const output_4911[] = {
  "fnmsb\t%0.d, %1/m, %4.d, %2.d",
  "fnmls\t%0.d, %1/m, %3.d, %4.d",
  "movprfx\t%0, %2\n\tfnmls\t%0.d, %1/m, %3.d, %4.d",
};

static const char * const output_4912[] = {
  "fnmad\t%0.h, %1/m, %4.h, %2.h",
  "fnmla\t%0.h, %1/m, %3.h, %4.h",
  "movprfx\t%0, %2\n\tfnmla\t%0.h, %1/m, %3.h, %4.h",
};

static const char * const output_4913[] = {
  "fnmad\t%0.s, %1/m, %4.s, %2.s",
  "fnmla\t%0.s, %1/m, %3.s, %4.s",
  "movprfx\t%0, %2\n\tfnmla\t%0.s, %1/m, %3.s, %4.s",
};

static const char * const output_4914[] = {
  "fnmad\t%0.d, %1/m, %4.d, %2.d",
  "fnmla\t%0.d, %1/m, %3.d, %4.d",
  "movprfx\t%0, %2\n\tfnmla\t%0.d, %1/m, %3.d, %4.d",
};

static const char * const output_4915[] = {
  "fdiv\t%0.h, %1/m, %0.h, %3.h",
  "fdivr\t%0.h, %1/m, %0.h, %2.h",
  "movprfx\t%0, %2\n\tfdiv\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_4916[] = {
  "fdiv\t%0.s, %1/m, %0.s, %3.s",
  "fdivr\t%0.s, %1/m, %0.s, %2.s",
  "movprfx\t%0, %2\n\tfdiv\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_4917[] = {
  "fdiv\t%0.d, %1/m, %0.d, %3.d",
  "fdivr\t%0.d, %1/m, %0.d, %2.d",
  "movprfx\t%0, %2\n\tfdiv\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_5004[] = {
  "fadd\t%0.h, %1/m, %0.h, %3.h",
  "fadd\t%0.h, %1/m, %0.h, %2.h",
  "movprfx\t%0, %1/m, %2\n\tfadd\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_5005[] = {
  "fsub\t%0.h, %1/m, %0.h, %3.h",
  "fsubr\t%0.h, %1/m, %0.h, %2.h",
  "movprfx\t%0, %1/m, %2\n\tfsub\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_5006[] = {
  "fmul\t%0.h, %1/m, %0.h, %3.h",
  "fmul\t%0.h, %1/m, %0.h, %2.h",
  "movprfx\t%0, %1/m, %2\n\tfmul\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_5007[] = {
  "fdiv\t%0.h, %1/m, %0.h, %3.h",
  "fdivr\t%0.h, %1/m, %0.h, %2.h",
  "movprfx\t%0, %1/m, %2\n\tfdiv\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_5008[] = {
  "fmaxnm\t%0.h, %1/m, %0.h, %3.h",
  "fmaxnm\t%0.h, %1/m, %0.h, %2.h",
  "movprfx\t%0, %1/m, %2\n\tfmaxnm\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_5009[] = {
  "fminnm\t%0.h, %1/m, %0.h, %3.h",
  "fminnm\t%0.h, %1/m, %0.h, %2.h",
  "movprfx\t%0, %1/m, %2\n\tfminnm\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_5010[] = {
  "fadd\t%0.s, %1/m, %0.s, %3.s",
  "fadd\t%0.s, %1/m, %0.s, %2.s",
  "movprfx\t%0, %1/m, %2\n\tfadd\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_5011[] = {
  "fsub\t%0.s, %1/m, %0.s, %3.s",
  "fsubr\t%0.s, %1/m, %0.s, %2.s",
  "movprfx\t%0, %1/m, %2\n\tfsub\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_5012[] = {
  "fmul\t%0.s, %1/m, %0.s, %3.s",
  "fmul\t%0.s, %1/m, %0.s, %2.s",
  "movprfx\t%0, %1/m, %2\n\tfmul\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_5013[] = {
  "fdiv\t%0.s, %1/m, %0.s, %3.s",
  "fdivr\t%0.s, %1/m, %0.s, %2.s",
  "movprfx\t%0, %1/m, %2\n\tfdiv\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_5014[] = {
  "fmaxnm\t%0.s, %1/m, %0.s, %3.s",
  "fmaxnm\t%0.s, %1/m, %0.s, %2.s",
  "movprfx\t%0, %1/m, %2\n\tfmaxnm\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_5015[] = {
  "fminnm\t%0.s, %1/m, %0.s, %3.s",
  "fminnm\t%0.s, %1/m, %0.s, %2.s",
  "movprfx\t%0, %1/m, %2\n\tfminnm\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_5016[] = {
  "fadd\t%0.d, %1/m, %0.d, %3.d",
  "fadd\t%0.d, %1/m, %0.d, %2.d",
  "movprfx\t%0, %1/m, %2\n\tfadd\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_5017[] = {
  "fsub\t%0.d, %1/m, %0.d, %3.d",
  "fsubr\t%0.d, %1/m, %0.d, %2.d",
  "movprfx\t%0, %1/m, %2\n\tfsub\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_5018[] = {
  "fmul\t%0.d, %1/m, %0.d, %3.d",
  "fmul\t%0.d, %1/m, %0.d, %2.d",
  "movprfx\t%0, %1/m, %2\n\tfmul\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_5019[] = {
  "fdiv\t%0.d, %1/m, %0.d, %3.d",
  "fdivr\t%0.d, %1/m, %0.d, %2.d",
  "movprfx\t%0, %1/m, %2\n\tfdiv\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_5020[] = {
  "fmaxnm\t%0.d, %1/m, %0.d, %3.d",
  "fmaxnm\t%0.d, %1/m, %0.d, %2.d",
  "movprfx\t%0, %1/m, %2\n\tfmaxnm\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_5021[] = {
  "fminnm\t%0.d, %1/m, %0.d, %3.d",
  "fminnm\t%0.d, %1/m, %0.d, %2.d",
  "movprfx\t%0, %1/m, %2\n\tfminnm\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_5022[] = {
  "fadd\t%0.h, %1/m, %0.h, %3.h",
  "movprfx\t%0, %2\n\tfadd\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_5023[] = {
  "fsub\t%0.h, %1/m, %0.h, %3.h",
  "movprfx\t%0, %2\n\tfsub\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_5024[] = {
  "fmul\t%0.h, %1/m, %0.h, %3.h",
  "movprfx\t%0, %2\n\tfmul\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_5025[] = {
  "fdiv\t%0.h, %1/m, %0.h, %3.h",
  "movprfx\t%0, %2\n\tfdiv\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_5026[] = {
  "fmaxnm\t%0.h, %1/m, %0.h, %3.h",
  "movprfx\t%0, %2\n\tfmaxnm\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_5027[] = {
  "fminnm\t%0.h, %1/m, %0.h, %3.h",
  "movprfx\t%0, %2\n\tfminnm\t%0.h, %1/m, %0.h, %3.h",
};

static const char * const output_5028[] = {
  "fadd\t%0.s, %1/m, %0.s, %3.s",
  "movprfx\t%0, %2\n\tfadd\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_5029[] = {
  "fsub\t%0.s, %1/m, %0.s, %3.s",
  "movprfx\t%0, %2\n\tfsub\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_5030[] = {
  "fmul\t%0.s, %1/m, %0.s, %3.s",
  "movprfx\t%0, %2\n\tfmul\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_5031[] = {
  "fdiv\t%0.s, %1/m, %0.s, %3.s",
  "movprfx\t%0, %2\n\tfdiv\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_5032[] = {
  "fmaxnm\t%0.s, %1/m, %0.s, %3.s",
  "movprfx\t%0, %2\n\tfmaxnm\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_5033[] = {
  "fminnm\t%0.s, %1/m, %0.s, %3.s",
  "movprfx\t%0, %2\n\tfminnm\t%0.s, %1/m, %0.s, %3.s",
};

static const char * const output_5034[] = {
  "fadd\t%0.d, %1/m, %0.d, %3.d",
  "movprfx\t%0, %2\n\tfadd\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_5035[] = {
  "fsub\t%0.d, %1/m, %0.d, %3.d",
  "movprfx\t%0, %2\n\tfsub\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_5036[] = {
  "fmul\t%0.d, %1/m, %0.d, %3.d",
  "movprfx\t%0, %2\n\tfmul\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_5037[] = {
  "fdiv\t%0.d, %1/m, %0.d, %3.d",
  "movprfx\t%0, %2\n\tfdiv\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_5038[] = {
  "fmaxnm\t%0.d, %1/m, %0.d, %3.d",
  "movprfx\t%0, %2\n\tfmaxnm\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_5039[] = {
  "fminnm\t%0.d, %1/m, %0.d, %3.d",
  "movprfx\t%0, %2\n\tfminnm\t%0.d, %1/m, %0.d, %3.d",
};

static const char * const output_5040[] = {
  "fadd\t%0.h, %1/m, %0.h, %2.h",
  "movprfx\t%0, %3\n\tfadd\t%0.h, %1/m, %0.h, %2.h",
};

static const char * const output_5041[] = {
  "fsubr\t%0.h, %1/m, %0.h, %2.h",
  "movprfx\t%0, %3\n\tfsubr\t%0.h, %1/m, %0.h, %2.h",
};

static const char * const output_5042[] = {
  "fmul\t%0.h, %1/m, %0.h, %2.h",
  "movprfx\t%0, %3\n\tfmul\t%0.h, %1/m, %0.h, %2.h",
};

static const char * const output_5043[] = {
  "fdivr\t%0.h, %1/m, %0.h, %2.h",
  "movprfx\t%0, %3\n\tfdivr\t%0.h, %1/m, %0.h, %2.h",
};

static const char * const output_5044[] = {
  "fmaxnm\t%0.h, %1/m, %0.h, %2.h",
  "movprfx\t%0, %3\n\tfmaxnm\t%0.h, %1/m, %0.h, %2.h",
};

static const char * const output_5045[] = {
  "fminnm\t%0.h, %1/m, %0.h, %2.h",
  "movprfx\t%0, %3\n\tfminnm\t%0.h, %1/m, %0.h, %2.h",
};

static const char * const output_5046[] = {
  "fadd\t%0.s, %1/m, %0.s, %2.s",
  "movprfx\t%0, %3\n\tfadd\t%0.s, %1/m, %0.s, %2.s",
};

static const char * const output_5047[] = {
  "fsubr\t%0.s, %1/m, %0.s, %2.s",
  "movprfx\t%0, %3\n\tfsubr\t%0.s, %1/m, %0.s, %2.s",
};

static const char * const output_5048[] = {
  "fmul\t%0.s, %1/m, %0.s, %2.s",
  "movprfx\t%0, %3\n\tfmul\t%0.s, %1/m, %0.s, %2.s",
};

static const char * const output_5049[] = {
  "fdivr\t%0.s, %1/m, %0.s, %2.s",
  "movprfx\t%0, %3\n\tfdivr\t%0.s, %1/m, %0.s, %2.s",
};

static const char * const output_5050[] = {
  "fmaxnm\t%0.s, %1/m, %0.s, %2.s",
  "movprfx\t%0, %3\n\tfmaxnm\t%0.s, %1/m, %0.s, %2.s",
};

static const char * const output_5051[] = {
  "fminnm\t%0.s, %1/m, %0.s, %2.s",
  "movprfx\t%0, %3\n\tfminnm\t%0.s, %1/m, %0.s, %2.s",
};

static const char * const output_5052[] = {
  "fadd\t%0.d, %1/m, %0.d, %2.d",
  "movprfx\t%0, %3\n\tfadd\t%0.d, %1/m, %0.d, %2.d",
};

static const char * const output_5053[] = {
  "fsubr\t%0.d, %1/m, %0.d, %2.d",
  "movprfx\t%0, %3\n\tfsubr\t%0.d, %1/m, %0.d, %2.d",
};

static const char * const output_5054[] = {
  "fmul\t%0.d, %1/m, %0.d, %2.d",
  "movprfx\t%0, %3\n\tfmul\t%0.d, %1/m, %0.d, %2.d",
};

static const char * const output_5055[] = {
  "fdivr\t%0.d, %1/m, %0.d, %2.d",
  "movprfx\t%0, %3\n\tfdivr\t%0.d, %1/m, %0.d, %2.d",
};

static const char * const output_5056[] = {
  "fmaxnm\t%0.d, %1/m, %0.d, %2.d",
  "movprfx\t%0, %3\n\tfmaxnm\t%0.d, %1/m, %0.d, %2.d",
};

static const char * const output_5057[] = {
  "fminnm\t%0.d, %1/m, %0.d, %2.d",
  "movprfx\t%0, %3\n\tfminnm\t%0.d, %1/m, %0.d, %2.d",
};

static const char * const output_5094[] = {
  "fmad\t%0.h, %1/m, %3.h, %4.h",
  "movprfx\t%0, %2\n\tfmad\t%0.h, %1/m, %3.h, %4.h",
};

static const char * const output_5095[] = {
  "fmsb\t%0.h, %1/m, %3.h, %4.h",
  "movprfx\t%0, %2\n\tfmsb\t%0.h, %1/m, %3.h, %4.h",
};

static const char * const output_5096[] = {
  "fnmad\t%0.h, %1/m, %3.h, %4.h",
  "movprfx\t%0, %2\n\tfnmad\t%0.h, %1/m, %3.h, %4.h",
};

static const char * const output_5097[] = {
  "fnmsb\t%0.h, %1/m, %3.h, %4.h",
  "movprfx\t%0, %2\n\tfnmsb\t%0.h, %1/m, %3.h, %4.h",
};

static const char * const output_5098[] = {
  "fmad\t%0.s, %1/m, %3.s, %4.s",
  "movprfx\t%0, %2\n\tfmad\t%0.s, %1/m, %3.s, %4.s",
};

static const char * const output_5099[] = {
  "fmsb\t%0.s, %1/m, %3.s, %4.s",
  "movprfx\t%0, %2\n\tfmsb\t%0.s, %1/m, %3.s, %4.s",
};

static const char * const output_5100[] = {
  "fnmad\t%0.s, %1/m, %3.s, %4.s",
  "movprfx\t%0, %2\n\tfnmad\t%0.s, %1/m, %3.s, %4.s",
};

static const char * const output_5101[] = {
  "fnmsb\t%0.s, %1/m, %3.s, %4.s",
  "movprfx\t%0, %2\n\tfnmsb\t%0.s, %1/m, %3.s, %4.s",
};

static const char * const output_5102[] = {
  "fmad\t%0.d, %1/m, %3.d, %4.d",
  "movprfx\t%0, %2\n\tfmad\t%0.d, %1/m, %3.d, %4.d",
};

static const char * const output_5103[] = {
  "fmsb\t%0.d, %1/m, %3.d, %4.d",
  "movprfx\t%0, %2\n\tfmsb\t%0.d, %1/m, %3.d, %4.d",
};

static const char * const output_5104[] = {
  "fnmad\t%0.d, %1/m, %3.d, %4.d",
  "movprfx\t%0, %2\n\tfnmad\t%0.d, %1/m, %3.d, %4.d",
};

static const char * const output_5105[] = {
  "fnmsb\t%0.d, %1/m, %3.d, %4.d",
  "movprfx\t%0, %2\n\tfnmsb\t%0.d, %1/m, %3.d, %4.d",
};

static const char * const output_5106[] = {
  "fmla\t%0.h, %1/m, %2.h, %3.h",
  "movprfx\t%0, %4\n\tfmla\t%0.h, %1/m, %2.h, %3.h",
};

static const char * const output_5107[] = {
  "fmls\t%0.h, %1/m, %2.h, %3.h",
  "movprfx\t%0, %4\n\tfmls\t%0.h, %1/m, %2.h, %3.h",
};

static const char * const output_5108[] = {
  "fnmla\t%0.h, %1/m, %2.h, %3.h",
  "movprfx\t%0, %4\n\tfnmla\t%0.h, %1/m, %2.h, %3.h",
};

static const char * const output_5109[] = {
  "fnmls\t%0.h, %1/m, %2.h, %3.h",
  "movprfx\t%0, %4\n\tfnmls\t%0.h, %1/m, %2.h, %3.h",
};

static const char * const output_5110[] = {
  "fmla\t%0.s, %1/m, %2.s, %3.s",
  "movprfx\t%0, %4\n\tfmla\t%0.s, %1/m, %2.s, %3.s",
};

static const char * const output_5111[] = {
  "fmls\t%0.s, %1/m, %2.s, %3.s",
  "movprfx\t%0, %4\n\tfmls\t%0.s, %1/m, %2.s, %3.s",
};

static const char * const output_5112[] = {
  "fnmla\t%0.s, %1/m, %2.s, %3.s",
  "movprfx\t%0, %4\n\tfnmla\t%0.s, %1/m, %2.s, %3.s",
};

static const char * const output_5113[] = {
  "fnmls\t%0.s, %1/m, %2.s, %3.s",
  "movprfx\t%0, %4\n\tfnmls\t%0.s, %1/m, %2.s, %3.s",
};

static const char * const output_5114[] = {
  "fmla\t%0.d, %1/m, %2.d, %3.d",
  "movprfx\t%0, %4\n\tfmla\t%0.d, %1/m, %2.d, %3.d",
};

static const char * const output_5115[] = {
  "fmls\t%0.d, %1/m, %2.d, %3.d",
  "movprfx\t%0, %4\n\tfmls\t%0.d, %1/m, %2.d, %3.d",
};

static const char * const output_5116[] = {
  "fnmla\t%0.d, %1/m, %2.d, %3.d",
  "movprfx\t%0, %4\n\tfnmla\t%0.d, %1/m, %2.d, %3.d",
};

static const char * const output_5117[] = {
  "fnmls\t%0.d, %1/m, %2.d, %3.d",
  "movprfx\t%0, %4\n\tfnmls\t%0.d, %1/m, %2.d, %3.d",
};

static const char * const output_5118[] = {
  "movprfx\t%0.h, %1/z, %4.h\n\tfmla\t%0.h, %1/m, %2.h, %3.h",
  "movprfx\t%0.h, %1/m, %4.h\n\tfmla\t%0.h, %1/m, %2.h, %3.h",
  "#",
};

static const char * const output_5119[] = {
  "movprfx\t%0.h, %1/z, %4.h\n\tfmls\t%0.h, %1/m, %2.h, %3.h",
  "movprfx\t%0.h, %1/m, %4.h\n\tfmls\t%0.h, %1/m, %2.h, %3.h",
  "#",
};

static const char * const output_5120[] = {
  "movprfx\t%0.h, %1/z, %4.h\n\tfnmla\t%0.h, %1/m, %2.h, %3.h",
  "movprfx\t%0.h, %1/m, %4.h\n\tfnmla\t%0.h, %1/m, %2.h, %3.h",
  "#",
};

static const char * const output_5121[] = {
  "movprfx\t%0.h, %1/z, %4.h\n\tfnmls\t%0.h, %1/m, %2.h, %3.h",
  "movprfx\t%0.h, %1/m, %4.h\n\tfnmls\t%0.h, %1/m, %2.h, %3.h",
  "#",
};

static const char * const output_5122[] = {
  "movprfx\t%0.s, %1/z, %4.s\n\tfmla\t%0.s, %1/m, %2.s, %3.s",
  "movprfx\t%0.s, %1/m, %4.s\n\tfmla\t%0.s, %1/m, %2.s, %3.s",
  "#",
};

static const char * const output_5123[] = {
  "movprfx\t%0.s, %1/z, %4.s\n\tfmls\t%0.s, %1/m, %2.s, %3.s",
  "movprfx\t%0.s, %1/m, %4.s\n\tfmls\t%0.s, %1/m, %2.s, %3.s",
  "#",
};

static const char * const output_5124[] = {
  "movprfx\t%0.s, %1/z, %4.s\n\tfnmla\t%0.s, %1/m, %2.s, %3.s",
  "movprfx\t%0.s, %1/m, %4.s\n\tfnmla\t%0.s, %1/m, %2.s, %3.s",
  "#",
};

static const char * const output_5125[] = {
  "movprfx\t%0.s, %1/z, %4.s\n\tfnmls\t%0.s, %1/m, %2.s, %3.s",
  "movprfx\t%0.s, %1/m, %4.s\n\tfnmls\t%0.s, %1/m, %2.s, %3.s",
  "#",
};

static const char * const output_5126[] = {
  "movprfx\t%0.d, %1/z, %4.d\n\tfmla\t%0.d, %1/m, %2.d, %3.d",
  "movprfx\t%0.d, %1/m, %4.d\n\tfmla\t%0.d, %1/m, %2.d, %3.d",
  "#",
};

static const char * const output_5127[] = {
  "movprfx\t%0.d, %1/z, %4.d\n\tfmls\t%0.d, %1/m, %2.d, %3.d",
  "movprfx\t%0.d, %1/m, %4.d\n\tfmls\t%0.d, %1/m, %2.d, %3.d",
  "#",
};

static const char * const output_5128[] = {
  "movprfx\t%0.d, %1/z, %4.d\n\tfnmla\t%0.d, %1/m, %2.d, %3.d",
  "movprfx\t%0.d, %1/m, %4.d\n\tfnmla\t%0.d, %1/m, %2.d, %3.d",
  "#",
};

static const char * const output_5129[] = {
  "movprfx\t%0.d, %1/z, %4.d\n\tfnmls\t%0.d, %1/m, %2.d, %3.d",
  "movprfx\t%0.d, %1/m, %4.d\n\tfnmls\t%0.d, %1/m, %2.d, %3.d",
  "#",
};

static const char * const output_5130[] = {
  "insr\t%0.b, %w2",
  "insr\t%0.b, %b2",
};

static const char * const output_5131[] = {
  "insr\t%0.h, %w2",
  "insr\t%0.h, %h2",
};

static const char * const output_5132[] = {
  "insr\t%0.s, %w2",
  "insr\t%0.s, %s2",
};

static const char * const output_5133[] = {
  "insr\t%0.d, %x2",
  "insr\t%0.d, %d2",
};

static const char * const output_5134[] = {
  "insr\t%0.h, %w2",
  "insr\t%0.h, %h2",
};

static const char * const output_5135[] = {
  "insr\t%0.s, %w2",
  "insr\t%0.s, %s2",
};

static const char * const output_5136[] = {
  "insr\t%0.d, %x2",
  "insr\t%0.d, %d2",
};



static const struct insn_operand_data operand_data[] = 
{
  {
    0,
    "",
    E_VOIDmode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    cc_register,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    cc_register,
    "",
    E_CCmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r,r,r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_ccmp_operand,
    "r,Uss,Usn",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    immediate_operand,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    cc_register,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    cc_register,
    "",
    E_CCmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r,r,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_ccmp_operand,
    "r,Uss,Usn",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    immediate_operand,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    cc_register,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    cc_register,
    "",
    E_CCFPmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    immediate_operand,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    cc_register,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    cc_register,
    "",
    E_CCFPmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    immediate_operand,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    cc_register,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    cc_register,
    "",
    E_CCFPEmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    immediate_operand,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    cc_register,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    cc_register,
    "",
    E_CCFPEmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    immediate_operand,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    cc_register,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_imm24,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_imm24,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=r",
    E_DImode,
    0,
    0,
    0,
    0
  },
  {
    scratch_operand,
    "=r",
    E_DImode,
    0,
    0,
    0,
    0
  },
  {
    aarch64_prefetch_operand,
    "Dp",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    const_int_operand,
    "",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_si,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_di,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_call_insn_operand,
    "r,Usf",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_call_insn_operand,
    "Ucs,Usf",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    nonimmediate_operand,
    "=r,r,w,r,r,w,m,m,r,w,w",
    E_QImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_mov_operand,
    "r,M,Dq,Usv,m,m,rZ,w,w,r,w",
    E_QImode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "=r,r,w,r,r,w,m,m,r,w,w",
    E_HImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_mov_operand,
    "r,M,Dh,Usv,m,m,rZ,w,w,r,w",
    E_HImode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "=r,k,r,r,r,r,r,w,m,m,r,r,w,r,w,w",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_mov_operand,
    "r,r,k,M,n,Usv,m,m,rZ,w,Usa,Ush,rZ,w,w,Ds",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "=r,k,r,r,r,r,r,r,w,m,m,r,r,w,r,w,w",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_mov_operand,
    "r,r,k,N,M,n,Usv,m,m,rZ,w,Usa,Ush,rZ,w,w,Dd",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "+r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "+r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    nonimmediate_operand,
    "=r,w,r,w,r,m,m,w,m",
    E_TImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_movti_operand,
    "rUti,r,w,w,m,r,Z,m,w",
    E_TImode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "=w,w,w,?r,w,w,w,w,m,r,m,r",
    E_HFmode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "Y,?rY,?r,w,w,Ufc,Uvi,m,w,m,rY,r",
    E_HFmode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "=w,w,?r,w,w,w,w,m,r,m,r,r",
    E_SFmode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "Y,?rY,w,w,Ufc,Uvi,m,w,m,rY,r,M",
    E_SFmode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "=w,w,?r,w,w,w,w,m,r,m,r,r",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "Y,?rY,w,w,Ufc,Uvi,m,w,m,rY,r,N",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "=w,?&r,w,?r,w,?w,w,m,?r,m,m",
    E_TFmode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "w,?r,?r,w,Y,Y,m,w,m,?r,Y",
    E_TFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=r,w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump,Ump",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=r,w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m,m",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=r,w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump,Ump",
    E_SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=r,w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m,m",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=r,w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump,Ump",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=r,w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m,m",
    E_SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=r,w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump,Ump",
    E_SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=r,w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m,m",
    E_SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=r,w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump,Ump",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=r,w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m,m",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=r,w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump,Ump",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=r,w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m,m",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=r,w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump,Ump",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=r,w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m,m",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=r,w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump,Ump",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=r,w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m,m",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_TFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_TFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_TFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_TFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_mem_pair_operand,
    "=Ump,Ump",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_reg_zero_or_fp_zero,
    "rYZ,w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m,m",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_reg_zero_or_fp_zero,
    "rYZ,w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump,Ump",
    E_SFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_reg_zero_or_fp_zero,
    "rYZ,w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m,m",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_reg_zero_or_fp_zero,
    "rYZ,w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump,Ump",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_reg_zero_or_fp_zero,
    "rYZ,w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m,m",
    E_SFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_reg_zero_or_fp_zero,
    "rYZ,w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump,Ump",
    E_SFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_reg_zero_or_fp_zero,
    "rYZ,w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m,m",
    E_SFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_reg_zero_or_fp_zero,
    "rYZ,w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump,Ump",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_reg_zero_or_fp_zero,
    "rYZ,w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m,m",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_reg_zero_or_fp_zero,
    "rYZ,w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump,Ump",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_reg_zero_or_fp_zero,
    "rYZ,w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m,m",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_reg_zero_or_fp_zero,
    "rYZ,w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump,Ump",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_reg_zero_or_fp_zero,
    "rYZ,w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m,m",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_reg_zero_or_fp_zero,
    "rYZ,w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump,Ump",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_reg_zero_or_fp_zero,
    "rYZ,w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m,m",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_reg_zero_or_fp_zero,
    "rYZ,w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_TFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_TFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_TFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_TFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=k",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_offset,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=k",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_offset,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=k",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_offset,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=k",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_offset,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=k",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_offset,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=k",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_offset,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=k",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_offset,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=k",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_offset,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=k",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_TImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_TImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_offset,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=k",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_TImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_TImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_offset,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=k",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_TFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_TFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_offset,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=k",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_TFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_TFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_offset,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&k",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_offset,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&k",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_offset,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&k",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_offset,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&k",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_offset,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&k",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_offset,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&k",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_offset,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&k",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_offset,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&k",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_offset,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&k",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_TImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_TImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_offset,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&k",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_TImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_TImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_offset,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&k",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_TFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_TFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_offset,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&k",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_TFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_TFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_offset,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    nonimmediate_operand,
    "r,m",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=r,r,w,w,r,w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    nonimmediate_operand,
    "r,m,r,m,w,w",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=r,w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump,Ump",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=r,w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m,m",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=r,r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    nonimmediate_operand,
    "r,m",
    E_QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=r,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    nonimmediate_operand,
    "r,m",
    E_QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=r,r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    nonimmediate_operand,
    "r,m",
    E_HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=r,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    nonimmediate_operand,
    "r,m",
    E_HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=r,r,w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    nonimmediate_operand,
    "r,m,m",
    E_QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=r,r,w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    nonimmediate_operand,
    "r,m,m",
    E_QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=r,r,w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    nonimmediate_operand,
    "r,m,m",
    E_HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=r,r,w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    nonimmediate_operand,
    "r,m,m",
    E_HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=r,r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    nonimmediate_operand,
    "r,m",
    E_QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=rk,rk,w,rk,r,rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%rk,rk,w,rk,rk,rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_pluslong_operand,
    "I,r,w,J,Uaa,Uav",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk,rk,w,rk,r,rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%rk,rk,w,rk,rk,rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_pluslong_operand,
    "I,r,w,J,Uaa,Uav",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk,rk,rk,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%rk,rk,rk,rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_pluslong_operand,
    "I,r,J,Uaa",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,r,r,r,r,&r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%rk,rk,rk,rk,rk,rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_pluslong_or_poly_operand,
    "I,r,J,Uaa,Uav,Uat",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,r,r,r,r,&r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%rk,rk,rk,rk,rk,rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_pluslong_or_poly_operand,
    "I,r,J,Uaa,Uav,Uat",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,r,r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%rk,rk,rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_plus_operand,
    "r,I,J",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,r,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%rk,rk,rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_plus_operand,
    "r,I,J",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,r,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%rk,rk,rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_plus_operand,
    "r,I,J",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r,r,r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_plus_operand,
    "r,I,J",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r,r,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_plus_operand,
    "r,I,J",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,r,r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk,rk,rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_plus_operand,
    "r,I,J",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,r,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk,rk,rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_plus_operand,
    "r,I,J",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r,r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_scalar_int_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_plus_immediate,
    "I,J",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_scalar_int_operand,
    "",
    E_TImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_plus_immediate,
    "I,J",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk,rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_plus_immediate,
    "I,J",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk,rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_plus_immediate,
    "I,J",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_shift_imm_si,
    "n",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_shift_imm_di,
    "n",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_shift_imm_si,
    "n",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_shift_imm_di,
    "n",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_pwr_2_si,
    "n",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_pwr_2_di,
    "n",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_pwr_2_si,
    "n",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_pwr_2_di,
    "n",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_imm3,
    "Ui3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_imm3,
    "Ui3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_imm3,
    "Ui3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_imm3,
    "Ui3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_imm3,
    "Ui3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_imm3,
    "Ui3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_imm3,
    "Ui3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_imm3,
    "Ui3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_imm3,
    "Ui3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_imm3,
    "Ui3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_imm3,
    "Ui3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_imm3,
    "Ui3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_pwr_imm3,
    "Up3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_pwr_imm3,
    "Up3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%r,r,r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_plus_operand,
    "r,I,J",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%r,r,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_plus_operand,
    "r,I,J",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_plus_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_plus_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_shift_imm_si,
    "n",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_imm3,
    "Ui3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_imm3,
    "Ui3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_imm3,
    "Ui3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_imm3,
    "Ui3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_imm3,
    "Ui3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_imm3,
    "Ui3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_imm3,
    "Ui3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_imm3,
    "Ui3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_pwr_imm3,
    "Up3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_pwr_imm3,
    "Up3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_pwr_imm3,
    "Up3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_pwr_imm3,
    "Up3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_pwr_imm3,
    "Up3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_pwr_imm3,
    "Up3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_pwr_imm3,
    "Up3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_pwr_imm3,
    "Up3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_pwr_imm3,
    "Up3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_pwr_imm3,
    "Up3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_pwr_imm3,
    "Up3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_carry_operation,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_carry_operation,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_carry_operation,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_carry_operation,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_carry_operation,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_scalar_int_operand,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_carry_operation,
    "",
    E_TImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_carry_operation,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_scalar_int_operand,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_carry_operation,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_carry_operation,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_scalar_int_operand,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_carry_operation,
    "",
    E_TImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_carry_operation,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_scalar_int_operand,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_imm3,
    "Ui3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_imm3,
    "Ui3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_imm3,
    "Ui3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk,w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk,w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r,w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rkZ,rkZ",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_plus_immediate,
    "I,J",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_plus_immediate,
    "J,I",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rkZ,rkZ",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_plus_immediate,
    "I,J",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_plus_immediate,
    "J,I",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rkZ",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rkZ",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_pwr_2_si,
    "n",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_imm3,
    "Ui3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_imm3,
    "Ui3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_pwr_imm3,
    "Up3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_pwr_imm3,
    "Up3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_pwr_imm3,
    "Up3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_borrow_operation,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_borrow_operation,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_borrow_operation,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_borrow_operation,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_borrow_operation,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_borrow_operation,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_borrow_operation,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_borrow_operation,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_borrow_operation,
    "",
    E_TImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_borrow_operation,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_borrow_operation,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_borrow_operation,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_borrow_operation,
    "",
    E_TImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_borrow_operation,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_imm3,
    "Ui3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_imm3,
    "Ui3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_imm3,
    "Ui3",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r,w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r,w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_borrow_operation,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_borrow_operation,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_borrow_operation,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_fp_compare_operand,
    "Y,w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_fp_compare_operand,
    "Y,w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_shift_imm_si,
    "n",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_shift_imm_di,
    "n",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator_mode,
    "",
    E_QImode,
    0,
    1,
    0,
    0
  },
  {
    cc_register,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator_mode,
    "",
    E_HImode,
    0,
    1,
    0,
    0
  },
  {
    cc_register,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator_mode,
    "",
    E_SImode,
    0,
    1,
    0,
    0
  },
  {
    cc_register,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator_mode,
    "",
    E_DImode,
    0,
    1,
    0,
    0
  },
  {
    cc_register,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_imm24,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_imm24,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator_mode,
    "",
    E_SImode,
    0,
    1,
    0,
    0
  },
  {
    cc_register,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,r,r,r,r,r,r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    cc_register,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_zero_or_m1_or_1,
    "rZ,rZ,UsM,rZ,Ui1,UsM,Ui1",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_zero_or_m1_or_1,
    "rZ,UsM,rZ,Ui1,rZ,UsM,Ui1",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,r,r,r,r,r,r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    cc_register,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_zero_or_m1_or_1,
    "rZ,rZ,UsM,rZ,Ui1,UsM,Ui1",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_zero_or_m1_or_1,
    "rZ,UsM,rZ,Ui1,rZ,UsM,Ui1",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,r,r,r,r,r,r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    cc_register,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_zero_or_m1_or_1,
    "rZ,rZ,UsM,rZ,Ui1,UsM,Ui1",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_zero_or_m1_or_1,
    "rZ,UsM,rZ,Ui1,rZ,UsM,Ui1",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,r,r,r,r,r,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    cc_register,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_zero_or_m1_or_1,
    "rZ,rZ,UsM,rZ,Ui1,UsM,Ui1",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_zero_or_m1_or_1,
    "rZ,UsM,rZ,Ui1,rZ,UsM,Ui1",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,r,r,r,r,r,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    cc_register,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_zero_or_m1_or_1,
    "rZ,rZ,UsM,rZ,Ui1,UsM,Ui1",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_zero_or_m1_or_1,
    "rZ,UsM,rZ,Ui1,rZ,UsM,Ui1",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    cc_register,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    cc_register,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    cc_register,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operation,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operation,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operation,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operation,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operation,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_cnt_immediate,
    "Usv",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_cnt_immediate,
    "Usv",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_logical_and_immediate,
    "UsO",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_logical_and_immediate,
    "UsP",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,rk,w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%r,r,w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_logical_operand,
    "r,K,w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,rk,w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%r,r,w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_logical_operand,
    "r,L,w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%r,r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_logical_operand,
    "r,K",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%r,r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_logical_operand,
    "r,K",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%r,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_logical_operand,
    "r,L",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%r,r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_logical_operand,
    "r,K",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r,w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r,w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r,w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r,w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r,?w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r,w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r,?w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r,w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    subreg_lowpart_operator,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    subreg_lowpart_operator,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    register_operand,
    "=&r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    subreg_lowpart_operator,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    register_operand,
    "=&r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    subreg_lowpart_operator,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    register_operand,
    "=&r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    subreg_lowpart_operator,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    register_operand,
    "=&r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    subreg_lowpart_operator,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    subreg_lowpart_operator,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    register_operand,
    "=&r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,r,w,w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r,r,w,w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_shift_imm_si,
    "Uss,r,Uss,w",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,r,w,w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r,r,w,w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_shift_imm_di,
    "Usd,r,Usd,w",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,r,w,&w,&w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r,r,w,w,w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_shift_imm_si,
    "Uss,r,Usg,w,0",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,r,w,&w,&w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r,r,w,w,w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_shift_imm_di,
    "Usd,r,Usj,w,0",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,r,w,&w,&w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r,r,w,w,w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_shift_imm_di,
    "Uss,r,Usg,w,0",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r,r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_shift_imm_si,
    "Uss,r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_shift_imm_di,
    "Usd,r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r,r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_shift_imm_si,
    "Uss,r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_offset_si,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_si,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_offset_di,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_di,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "+r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "+r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "+r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "+r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "+r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "+r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "+r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_si,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_di,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_si,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_di,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_si,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_di,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "+r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "+r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r,0",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n,Ulc",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "Ulc,n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r,0",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n,Ulc",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "Ulc,n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r,0",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n,Ulc",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "Ulc,n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_fp_pow2,
    "F",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_fp_pow2,
    "F",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_fp_pow2,
    "F",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_fp_pow2,
    "F",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,?r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,?r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,?r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,?r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i,i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i,i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r,w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i,i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r,w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i,i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w,r",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w,r",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,0,0",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w,w,X",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w,r",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w,r",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,0,0",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w,w,X",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_TImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_TFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "+w",
    E_TImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "+w",
    E_TFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_TImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_TFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_TImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_TImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_valid_symref,
    "S",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_valid_symref,
    "S",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_valid_symref,
    "S",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_valid_symref,
    "S",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_valid_symref,
    "S",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_valid_symref,
    "S",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_valid_symref,
    "S",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_tls_ie_symref,
    "S",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_tls_ie_symref,
    "S",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_tls_ie_symref,
    "S",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_tls_ie_symref,
    "S",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_tls_le_symref,
    "S",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_tls_le_symref,
    "S",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_tls_le_symref,
    "S",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_tls_le_symref,
    "S",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_valid_symref,
    "S",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=r",
    E_DImode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_plus_immediate,
    "L",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_plus_immediate,
    "L",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    memory_operand,
    "m",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    scratch_operand,
    "=&r",
    E_SImode,
    0,
    0,
    0,
    0
  },
  {
    memory_operand,
    "=m",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    memory_operand,
    "m",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    scratch_operand,
    "=&r",
    E_DImode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    memory_operand,
    "m",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    scratch_operand,
    "=&r",
    E_SImode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    memory_operand,
    "m",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    scratch_operand,
    "=&r",
    E_DImode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "=r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_TImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_TImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_TImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_TImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_TImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,?r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,?r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,?r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,?r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,?r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,?r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,?r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    nonimmediate_operand,
    "=w,m,m,w,?r,?w,?r,w",
    E_V8QImode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "m,Dz,w,w,w,r,r,Dn",
    E_V8QImode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "=w,m,m,w,?r,?w,?r,w",
    E_V4HImode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "m,Dz,w,w,w,r,r,Dn",
    E_V4HImode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "=w,m,m,w,?r,?w,?r,w",
    E_V4HFmode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "m,Dz,w,w,w,r,r,Dn",
    E_V4HFmode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "=w,m,m,w,?r,?w,?r,w",
    E_V2SImode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "m,Dz,w,w,w,r,r,Dn",
    E_V2SImode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "=w,m,m,w,?r,?w,?r,w",
    E_V2SFmode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "m,Dz,w,w,w,r,r,Dn",
    E_V2SFmode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "=w,Umn,m,w,?r,?w,?r,w",
    E_V16QImode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "m,Dz,w,w,w,r,r,Dn",
    E_V16QImode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "=w,Umn,m,w,?r,?w,?r,w",
    E_V8HImode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "m,Dz,w,w,w,r,r,Dn",
    E_V8HImode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "=w,Umn,m,w,?r,?w,?r,w",
    E_V4SImode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "m,Dz,w,w,w,r,r,Dn",
    E_V4SImode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "=w,Umn,m,w,?r,?w,?r,w",
    E_V2DImode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "m,Dz,w,w,w,r,r,Dn",
    E_V2DImode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "=w,Umn,m,w,?r,?w,?r,w",
    E_V8HFmode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "m,Dz,w,w,w,r,r,Dn",
    E_V8HFmode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "=w,Umn,m,w,?r,?w,?r,w",
    E_V4SFmode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "m,Dz,w,w,w,r,r,Dn",
    E_V4SFmode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "=w,Umn,m,w,?r,?w,?r,w",
    E_V2DFmode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "m,Dz,w,w,w,r,r,Dn",
    E_V2DFmode,
    0,
    0,
    1,
    1
  },
  {
    memory_operand,
    "=m",
    E_QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V8QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V8QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V4HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V8QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V4HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V8QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V2SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V8QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V2SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V8QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V8QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V8QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V4HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V4HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V4HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V4HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V4HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V2SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V4HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V2SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V4HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V4HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V8QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V4HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V4HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V4HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V4HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V4HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V2SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V4HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V2SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V4HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V4HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V8QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V2SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V4HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V2SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V4HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V2SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V2SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V2SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V2SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V2SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V2SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V8QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V2SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V4HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V2SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V4HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V2SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V2SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V2SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V2SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V2SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V2SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V8QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V4HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V4HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V2SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V2SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V8QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V8QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V4HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V8QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V4HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V8QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V2SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V8QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V2SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V8QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V8QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V8QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V4HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V4HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V4HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V4HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V4HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V2SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V4HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V2SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V4HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V4HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V8QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V4HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V4HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V4HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V4HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V4HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V2SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V4HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V2SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V4HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V4HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V8QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V2SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V4HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V2SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V4HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V2SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V2SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V2SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V2SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V2SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V2SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V8QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V2SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V4HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V2SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V4HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V2SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V2SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V2SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V2SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V2SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V2SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V8QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V4HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V4HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V2SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V2SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V16QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V16QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V8HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V16QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V4SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V16QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V2DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V16QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V8HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V16QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V4SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V16QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V2DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V16QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V16QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V8HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V8HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V8HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V4SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V8HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V2DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V8HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V8HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V8HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V4SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V8HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V2DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V8HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V16QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V4SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V8HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V4SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V4SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V4SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V2DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V4SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V8HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V4SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V4SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V4SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V2DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V4SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V16QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V2DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V8HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V2DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V4SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V2DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V2DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V2DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V8HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V2DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V4SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V2DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V2DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V2DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V16QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V8HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V8HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V8HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V4SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V8HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V2DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V8HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V8HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V8HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V4SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V8HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V2DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V8HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V16QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V4SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V8HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V4SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V4SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V4SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V2DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V4SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V8HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V4SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V4SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V4SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V2DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V4SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V16QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V2DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V8HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V2DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V4SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V2DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V2DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V2DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V8HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V2DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V4SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V2DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "Ump",
    E_V2DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_V2DFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V16QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V16QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V8HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V16QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V4SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V16QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V2DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V16QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V8HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V16QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V4SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V16QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V2DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V16QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V16QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V8HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V8HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V8HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V4SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V8HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V2DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V8HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V8HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V8HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V4SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V8HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V2DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V8HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V16QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V4SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V8HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V4SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V4SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V4SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V2DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V4SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V8HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V4SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V4SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V4SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V2DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V4SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V16QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V2DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V8HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V2DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V4SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V2DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V2DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V2DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V8HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V2DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V4SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V2DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V2DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V2DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V16QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V8HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V8HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V8HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V4SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V8HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V2DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V8HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V8HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V8HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V4SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V8HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V2DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V8HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V16QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V4SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V8HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V4SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V4SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V4SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V2DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V4SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V8HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V4SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V4SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V4SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V2DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V4SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V16QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V2DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V8HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V2DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V4SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V2DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V2DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V2DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V8HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V2DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V4SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V2DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_operand,
    "=Ump",
    E_V2DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "=m",
    E_V2DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_lo_half,
    "",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_lo_half,
    "",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_lo_half,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_lo_half,
    "",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_lo_half,
    "",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_lo_half,
    "",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_lo_half,
    "",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "n",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_smin,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_umin,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_smin,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_umin,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_smin,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_umin,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_smin,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_umin,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_smin,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_umin,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_smin,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_umin,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_bic_imm,
    "w,Db",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_bic_imm,
    "w,Db",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_bic_imm,
    "w,Db",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_bic_imm,
    "w,Db",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_bic_imm,
    "w,Db",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_bic_imm,
    "w,Db",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_bic_imm,
    "w,Db",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_orr_imm,
    "w,Do",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_orr_imm,
    "w,Do",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_orr_imm,
    "w,Do",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_orr_imm,
    "w,Do",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_orr_imm,
    "w,Do",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_orr_imm,
    "w,Do",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_orr_imm,
    "w,Do",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_general_operand,
    "w,?r,Utv",
    E_QImode,
    0,
    0,
    1,
    1
  },
  {
    immediate_operand,
    "i,i,i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,0,0",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_general_operand,
    "w,?r,Utv",
    E_QImode,
    0,
    0,
    1,
    1
  },
  {
    immediate_operand,
    "i,i,i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,0,0",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_general_operand,
    "w,?r,Utv",
    E_HImode,
    0,
    0,
    1,
    1
  },
  {
    immediate_operand,
    "i,i,i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,0,0",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_general_operand,
    "w,?r,Utv",
    E_HImode,
    0,
    0,
    1,
    1
  },
  {
    immediate_operand,
    "i,i,i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,0,0",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_general_operand,
    "w,?r,Utv",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    immediate_operand,
    "i,i,i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,0,0",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_general_operand,
    "w,?r,Utv",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    immediate_operand,
    "i,i,i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,0,0",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_general_operand,
    "w,?r,Utv",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    immediate_operand,
    "i,i,i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,0,0",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_general_operand,
    "w,?r,Utv",
    E_HFmode,
    0,
    0,
    1,
    1
  },
  {
    immediate_operand,
    "i,i,i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,0,0",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_general_operand,
    "w,?r,Utv",
    E_HFmode,
    0,
    0,
    1,
    1
  },
  {
    immediate_operand,
    "i,i,i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,0,0",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_general_operand,
    "w,?r,Utv",
    E_SFmode,
    0,
    0,
    1,
    1
  },
  {
    immediate_operand,
    "i,i,i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,0,0",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_general_operand,
    "w,?r,Utv",
    E_SFmode,
    0,
    0,
    1,
    1
  },
  {
    immediate_operand,
    "i,i,i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,0,0",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_general_operand,
    "w,?r,Utv",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    immediate_operand,
    "i,i,i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,0,0",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_rshift_imm,
    "Dr",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_rshift_imm,
    "Dr",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_rshift_imm,
    "Dr",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_rshift_imm,
    "Dr",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_rshift_imm,
    "Dr",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_rshift_imm,
    "Dr",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_rshift_imm,
    "Dr",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_lshift_imm,
    "Dl",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_lshift_imm,
    "Dl",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_lshift_imm,
    "Dl",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_lshift_imm,
    "Dl",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_lshift_imm,
    "Dl",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_lshift_imm,
    "Dl",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_lshift_imm,
    "Dl",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,r,r",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,r,r",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,r,r",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,r,r",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,r,r",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,r,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,r,r",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "+w,w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,r",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_lo_half,
    "",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "+w,w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,r",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_lo_half,
    "",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "+w,w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,r",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_lo_half,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "+w,w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_lo_half,
    "",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "+w,w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,r",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_lo_half,
    "",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "+w,w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,r",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_lo_half,
    "",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "+w,w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,r",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_lo_half,
    "",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_lo_half,
    "",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_lo_half,
    "",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_lo_half,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_lo_half,
    "",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_lo_half,
    "",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_lo_half,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_lo_half,
    "",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_lo_half,
    "",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_lo_half,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_fp_vec_pow2,
    "",
    E_V2SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_fp_vec_pow2,
    "",
    E_V4SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_fp_vec_pow2,
    "",
    E_V2DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_lo_half,
    "",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_lo_half,
    "",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w,w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,0",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w,w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,0",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w,w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,0",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w,w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,0",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w,w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,0",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w,w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,0",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w,w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,0",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w,w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,0",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w,w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,0",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w,w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,0",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w,w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,0",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w,w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,0",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w,w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,0",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w,w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,0",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w,&r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w,w,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,0,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w,&r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w,w,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,0,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_nonimmediate_operand,
    "=?r,w,Utv",
    E_QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w,w,w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i,i,i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_nonimmediate_operand,
    "=?r,w,Utv",
    E_QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w,w,w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i,i,i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_nonimmediate_operand,
    "=?r,w,Utv",
    E_HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w,w,w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i,i,i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_nonimmediate_operand,
    "=?r,w,Utv",
    E_HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w,w,w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i,i,i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_nonimmediate_operand,
    "=?r,w,Utv",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w,w,w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i,i,i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_nonimmediate_operand,
    "=?r,w,Utv",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w,w,w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i,i,i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_nonimmediate_operand,
    "=?r,w,Utv",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w,w,w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i,i,i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_nonimmediate_operand,
    "=?r,w,Utv",
    E_HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w,w,w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i,i,i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_nonimmediate_operand,
    "=?r,w,Utv",
    E_HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w,w,w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i,i,i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_nonimmediate_operand,
    "=?r,w,Utv",
    E_SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w,w,w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i,i,i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_nonimmediate_operand,
    "=?r,w,Utv",
    E_SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w,w,w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i,i,i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_nonimmediate_operand,
    "=?r,w,Utv",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w,w,w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i,i,i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "Utq",
    E_V8QImode,
    0,
    0,
    1,
    1
  },
  {
    memory_operand,
    "m",
    E_V8QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "Utq",
    E_V4HImode,
    0,
    0,
    1,
    1
  },
  {
    memory_operand,
    "m",
    E_V4HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "Utq",
    E_V4HFmode,
    0,
    0,
    1,
    1
  },
  {
    memory_operand,
    "m",
    E_V4HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "Utq",
    E_V2SImode,
    0,
    0,
    1,
    1
  },
  {
    memory_operand,
    "m",
    E_V2SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "Utq",
    E_V2SFmode,
    0,
    0,
    1,
    1
  },
  {
    memory_operand,
    "m",
    E_V2SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "Utq",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    memory_operand,
    "m",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "Utq",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    memory_operand,
    "m",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_mem_pair_lanes_operand,
    "=Umn,Umn",
    E_V16QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w,r",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,r",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_lanes_operand,
    "=Umn,Umn",
    E_V8HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w,r",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,r",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_lanes_operand,
    "=Umn,Umn",
    E_V8HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w,r",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,r",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_lanes_operand,
    "=Umn,Umn",
    E_V4SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w,r",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,r",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_lanes_operand,
    "=Umn,Umn",
    E_V4SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w,r",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,r",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_lanes_operand,
    "=Umn,Umn",
    E_V2DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_mem_pair_lanes_operand,
    "=Umn,Umn",
    E_V2DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w,r",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,r",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    general_operand,
    "w,?r,m",
    E_V8QImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_simd_or_scalar_imm_zero,
    "",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    general_operand,
    "w,?r,m",
    E_V4HImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_simd_or_scalar_imm_zero,
    "",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    general_operand,
    "w,?r,m",
    E_V4HFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_simd_or_scalar_imm_zero,
    "",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    general_operand,
    "w,?r,m",
    E_V2SImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_simd_or_scalar_imm_zero,
    "",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    general_operand,
    "w,?r,m",
    E_V2SFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_simd_or_scalar_imm_zero,
    "",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    general_operand,
    "w,?r,m",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_simd_or_scalar_imm_zero,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    general_operand,
    "w,?r,m",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_simd_or_scalar_imm_zero,
    "",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_lo_half,
    "",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_lo_half,
    "",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_lo_half,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_bitsize_qi,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_bitsize_hi,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_bitsize_si,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_offset_qi,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_offset_qi,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_offset_hi,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_offset_hi,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_offset_si,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_offset_si,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_offset_di,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_offset_di,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_offset_qi,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_offset_qi,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_offset_hi,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_offset_hi,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_offset_si,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_offset_si,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_offset_di,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_offset_di,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_qi,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_qi,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_hi,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_hi,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_si,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_si,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_di,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_di,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_qi,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_qi,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_hi,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_hi,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_si,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_si,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_di,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_qi,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_hi,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_si,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_di,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_offset_hi,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_offset_si,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_offset_di,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_offset_hi,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_offset_si,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_offset_di,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_reg_or_zero,
    "w,ZDz",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_reg_or_zero,
    "w,ZDz",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_reg_or_zero,
    "w,ZDz",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_reg_or_zero,
    "w,ZDz",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_reg_or_zero,
    "w,ZDz",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_reg_or_zero,
    "w,ZDz",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_reg_or_zero,
    "w,ZDz",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_reg_or_zero,
    "w,ZDz,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_reg_or_zero,
    "w,ZDz",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_reg_or_zero,
    "w,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_reg_or_zero,
    "w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_imm_zero,
    "",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_imm_minus_one,
    "",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_imm_zero,
    "",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_imm_minus_one,
    "",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_imm_zero,
    "",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_imm_minus_one,
    "",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_imm_zero,
    "",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_imm_minus_one,
    "",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_imm_zero,
    "",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_imm_minus_one,
    "",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_imm_zero,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_imm_minus_one,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_imm_zero,
    "",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_imm_minus_one,
    "",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_reg_or_zero,
    "w,YDz",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_reg_or_zero,
    "w,YDz",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_reg_or_zero,
    "w,YDz",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_reg_or_zero,
    "w,YDz",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_reg_or_zero,
    "w,YDz",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_reg_or_zero,
    "w,YDz",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_reg_or_zero,
    "w,YDz",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_reg_or_zero,
    "w,YDz",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_OImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "Utv",
    E_OImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_OImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "Utv",
    E_BLKmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "0",
    E_OImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "=Utv",
    E_OImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_OImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "=Utv",
    E_BLKmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_OImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_CImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "Utv",
    E_CImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_CImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "Utv",
    E_BLKmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "0",
    E_CImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "=Utv",
    E_CImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_CImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "=Utv",
    E_BLKmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_CImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_XImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "Utv",
    E_XImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_XImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "Utv",
    E_BLKmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "0",
    E_XImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "=Utv",
    E_XImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_XImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "=Utv",
    E_BLKmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_XImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&w",
    E_OImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_OImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&w",
    E_CImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_CImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&w",
    E_XImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_XImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_nonimmediate_operand,
    "=w,Utv,w",
    E_OImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_simd_general_operand,
    "w,w,Utv",
    E_OImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_simd_nonimmediate_operand,
    "=w,Utv,w",
    E_CImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_simd_general_operand,
    "w,w,Utv",
    E_CImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_simd_nonimmediate_operand,
    "=w,Utv,w",
    E_XImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_simd_general_operand,
    "w,w,Utv",
    E_XImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "Utv",
    E_V8QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "Utv",
    E_V16QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "Utv",
    E_V4HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "Utv",
    E_V8HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "Utv",
    E_V2SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "Utv",
    E_V4SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "Utv",
    E_V2DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "Utv",
    E_V4HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "Utv",
    E_V8HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "Utv",
    E_V2SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "Utv",
    E_V4SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "Utv",
    E_V2DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "Utv",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_simd_struct_operand,
    "=Utv",
    E_V8QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "=Utv",
    E_V16QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "=Utv",
    E_V4HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "=Utv",
    E_V8HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "=Utv",
    E_V2SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "=Utv",
    E_V4SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "=Utv",
    E_V2DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "=Utv",
    E_V4HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "=Utv",
    E_V8HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "=Utv",
    E_V2SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "=Utv",
    E_V4SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "=Utv",
    E_V2DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "=Utv",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    nonimmediate_operand,
    "=w,m,w",
    E_OImode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "w,w,m",
    E_OImode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "=w,o,w",
    E_CImode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "w,w,o",
    E_CImode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "=w,o,w",
    E_XImode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "w,w,o",
    E_XImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_OImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_OImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_OImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_OImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_CImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_CImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_CImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_CImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_XImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_XImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_XImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_XImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_OImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "i",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "Utv",
    E_QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "Utv",
    E_QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "Utv",
    E_HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "Utv",
    E_HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "Utv",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "Utv",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "Utv",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "Utv",
    E_HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "Utv",
    E_HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "Utv",
    E_SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "Utv",
    E_SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_struct_operand,
    "Utv",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%0",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_imm_zero,
    "",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_di,
    "Usd",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_imm2,
    "Ui2",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_lo_half,
    "",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_lo_half,
    "",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_lo_half,
    "",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_lo_half,
    "",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_lo_half,
    "",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_imm2,
    "Ui2",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_imm2,
    "Ui2",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_lo_half,
    "",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_lane_imm3,
    "Ui7",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_lane_imm3,
    "Ui7",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_lo_half,
    "",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_lane_imm3,
    "Ui7",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_lane_imm3,
    "Ui7",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_lo_half,
    "",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_imm2,
    "Ui2",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "x",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    vect_par_cnst_hi_half,
    "",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_imm2,
    "Ui2",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_TImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_TImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_QImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_reg_or_imm,
    "rn",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_SImode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "=&r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_HImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_plushi_operand,
    "rn",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_SImode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "=&r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_plus_operand,
    "rIJ",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_SImode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "=&r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_plus_operand,
    "rIJ",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_SImode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "+r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_QImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "+r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_HImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "+r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "+r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_QImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_SImode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "=&r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_HImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_SImode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "=&r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_SImode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "=&r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_SImode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "=r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_QImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_HImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_QImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_plus_operand,
    "rIJ",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_QImode,
    0,
    0,
    0,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_SImode,
    0,
    0,
    0,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_QImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_logical_operand,
    "rK",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_QImode,
    0,
    0,
    0,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_SImode,
    0,
    0,
    0,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_HImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_plus_operand,
    "rIJ",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_HImode,
    0,
    0,
    0,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_SImode,
    0,
    0,
    0,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_HImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_logical_operand,
    "rK",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_HImode,
    0,
    0,
    0,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_SImode,
    0,
    0,
    0,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_plus_operand,
    "rIJ",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_SImode,
    0,
    0,
    0,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_SImode,
    0,
    0,
    0,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_logical_operand,
    "rK",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_SImode,
    0,
    0,
    0,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_SImode,
    0,
    0,
    0,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_plus_operand,
    "rIJ",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_DImode,
    0,
    0,
    0,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_SImode,
    0,
    0,
    0,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_logical_operand,
    "rL",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_DImode,
    0,
    0,
    0,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_SImode,
    0,
    0,
    0,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=r",
    E_QImode,
    0,
    0,
    0,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=r",
    E_HImode,
    0,
    0,
    0,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=r",
    E_SImode,
    0,
    0,
    0,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=r",
    E_DImode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "=&r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_QImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_plus_operand,
    "rIJ",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_QImode,
    0,
    0,
    0,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_SImode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "=&r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_QImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_logical_operand,
    "rK",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_QImode,
    0,
    0,
    0,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_SImode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "=&r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_HImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_plus_operand,
    "rIJ",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_HImode,
    0,
    0,
    0,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_SImode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "=&r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_HImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_logical_operand,
    "rK",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_HImode,
    0,
    0,
    0,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_SImode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "=&r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_plus_operand,
    "rIJ",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_SImode,
    0,
    0,
    0,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_SImode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "=&r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_logical_operand,
    "rK",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_SImode,
    0,
    0,
    0,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_SImode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "=&r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_plus_operand,
    "rIJ",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_DImode,
    0,
    0,
    0,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_SImode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "=&r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_logical_operand,
    "rL",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_DImode,
    0,
    0,
    0,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_SImode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "=r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_QImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_plus_operand,
    "rIJ",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_SImode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "=&r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_QImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_logical_operand,
    "rK",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_SImode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "=&r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_HImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_plus_operand,
    "rIJ",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_SImode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "=&r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_HImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_logical_operand,
    "rK",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_SImode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "=&r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_plus_operand,
    "rIJ",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_SImode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "=&r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "+Q",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_logical_operand,
    "rL",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=&r",
    E_SImode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "=r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "Q",
    E_QImode,
    0,
    0,
    1,
    1
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "Q",
    E_HImode,
    0,
    0,
    1,
    1
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "Q",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "Q",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_rcpc_memory_operand,
    "=Q,Ust",
    E_QImode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "rZ,rZ",
    E_QImode,
    0,
    0,
    1,
    1
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_rcpc_memory_operand,
    "=Q,Ust",
    E_HImode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "rZ,rZ",
    E_HImode,
    0,
    0,
    1,
    1
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_rcpc_memory_operand,
    "=Q,Ust",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "rZ,rZ",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_rcpc_memory_operand,
    "=Q,Ust",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "rZ,rZ",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "Q",
    E_QImode,
    0,
    0,
    1,
    1
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "Q",
    E_HImode,
    0,
    0,
    1,
    1
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "=Q",
    E_QImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "=Q",
    E_HImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "=Q",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sync_memory_operand,
    "=Q",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_BLKmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_nonimmediate_operand,
    "=w",
    E_VNx16QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_any_register_operand,
    "w",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_nonimmediate_operand,
    "=w",
    E_VNx8HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_any_register_operand,
    "w",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_nonimmediate_operand,
    "=w",
    E_VNx4SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_any_register_operand,
    "w",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_nonimmediate_operand,
    "=w",
    E_VNx2DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_any_register_operand,
    "w",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_nonimmediate_operand,
    "=w",
    E_VNx8HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_any_register_operand,
    "w",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_nonimmediate_operand,
    "=w",
    E_VNx4SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_any_register_operand,
    "w",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_nonimmediate_operand,
    "=w",
    E_VNx2DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_any_register_operand,
    "w",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_nonimmediate_operand,
    "=w,Utr,w,w",
    E_VNx16QImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_general_operand,
    "Utr,w,w,Dn",
    E_VNx16QImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_nonimmediate_operand,
    "=w,Utr,w,w",
    E_VNx8HImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_general_operand,
    "Utr,w,w,Dn",
    E_VNx8HImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_nonimmediate_operand,
    "=w,Utr,w,w",
    E_VNx4SImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_general_operand,
    "Utr,w,w,Dn",
    E_VNx4SImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_nonimmediate_operand,
    "=w,Utr,w,w",
    E_VNx2DImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_general_operand,
    "Utr,w,w,Dn",
    E_VNx2DImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_nonimmediate_operand,
    "=w,Utr,w,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_general_operand,
    "Utr,w,w,Dn",
    E_VNx8HFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_nonimmediate_operand,
    "=w,Utr,w,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_general_operand,
    "Utr,w,w,Dn",
    E_VNx4SFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_nonimmediate_operand,
    "=w,Utr,w,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_general_operand,
    "Utr,w,w,Dn",
    E_VNx2DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_nonmemory_operand,
    "w,Dn",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_nonmemory_operand,
    "w,Dn",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_nonmemory_operand,
    "w,Dn",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_nonmemory_operand,
    "w,Dn",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_nonmemory_operand,
    "w,Dn",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_nonmemory_operand,
    "w,Dn",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_nonmemory_operand,
    "w,Dn",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    nonimmediate_operand,
    "=w,w,m",
    E_VNx16QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    nonimmediate_operand,
    "w,m,w",
    E_VNx16QImode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "=w,w,m",
    E_VNx8HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    nonimmediate_operand,
    "w,m,w",
    E_VNx8HImode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "=w,w,m",
    E_VNx4SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    nonimmediate_operand,
    "w,m,w",
    E_VNx4SImode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "=w,w,m",
    E_VNx2DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    nonimmediate_operand,
    "w,m,w",
    E_VNx2DImode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "=w,w,m",
    E_VNx8HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    nonimmediate_operand,
    "w,m,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "=w,w,m",
    E_VNx4SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    nonimmediate_operand,
    "w,m,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "=w,w,m",
    E_VNx2DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    nonimmediate_operand,
    "w,m,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_VNx16QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_VNx8HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_VNx4SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_VNx2DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_VNx8HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_VNx4SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_VNx2DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "+m",
    E_VNx16QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "+m",
    E_VNx8HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "+m",
    E_VNx4SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "+m",
    E_VNx2DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "+m",
    E_VNx8HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "+m",
    E_VNx4SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "+m",
    E_VNx2DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w,w,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "Z,rk,rk,rk,rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,w,w,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "i,Z,Ui1,Z,Ui1",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_gather_scale_operand_w,
    "Ui1,Ui1,Ui1,i,i",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl,Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w,w,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "Z,rk,rk,rk,rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,w,w,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "i,Z,Ui1,Z,Ui1",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_gather_scale_operand_w,
    "Ui1,Ui1,Ui1,i,i",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl,Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "Z,rk,rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_gather_scale_operand_d,
    "Ui1,Ui1,i",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "Z,rk,rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_gather_scale_operand_d,
    "Ui1,Ui1,i",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "Z,rk,rk,rk,rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,w,w,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "i,Z,Ui1,Z,Ui1",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_gather_scale_operand_w,
    "Ui1,Ui1,Ui1,i,i",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,w,w,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl,Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "Z,rk,rk,rk,rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,w,w,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "i,Z,Ui1,Z,Ui1",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_gather_scale_operand_w,
    "Ui1,Ui1,Ui1,i,i",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,w,w,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl,Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "Z,rk,rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_gather_scale_operand_d,
    "Ui1,Ui1,i",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "Z,rk,rk",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_gather_scale_operand_d,
    "Ui1,Ui1,i",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_nonimmediate_operand,
    "=w,Utr,w,w",
    E_VNx32QImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_general_operand,
    "Utr,w,w,Dn",
    E_VNx32QImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_nonimmediate_operand,
    "=w,Utr,w,w",
    E_VNx16HImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_general_operand,
    "Utr,w,w,Dn",
    E_VNx16HImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_nonimmediate_operand,
    "=w,Utr,w,w",
    E_VNx8SImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_general_operand,
    "Utr,w,w,Dn",
    E_VNx8SImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_nonimmediate_operand,
    "=w,Utr,w,w",
    E_VNx4DImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_general_operand,
    "Utr,w,w,Dn",
    E_VNx4DImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_nonimmediate_operand,
    "=w,Utr,w,w",
    E_VNx16HFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_general_operand,
    "Utr,w,w,Dn",
    E_VNx16HFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_nonimmediate_operand,
    "=w,Utr,w,w",
    E_VNx8SFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_general_operand,
    "Utr,w,w,Dn",
    E_VNx8SFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_nonimmediate_operand,
    "=w,Utr,w,w",
    E_VNx4DFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_general_operand,
    "Utr,w,w,Dn",
    E_VNx4DFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_nonimmediate_operand,
    "=w,Utr,w,w",
    E_VNx48QImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_general_operand,
    "Utr,w,w,Dn",
    E_VNx48QImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_nonimmediate_operand,
    "=w,Utr,w,w",
    E_VNx24HImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_general_operand,
    "Utr,w,w,Dn",
    E_VNx24HImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_nonimmediate_operand,
    "=w,Utr,w,w",
    E_VNx12SImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_general_operand,
    "Utr,w,w,Dn",
    E_VNx12SImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_nonimmediate_operand,
    "=w,Utr,w,w",
    E_VNx6DImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_general_operand,
    "Utr,w,w,Dn",
    E_VNx6DImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_nonimmediate_operand,
    "=w,Utr,w,w",
    E_VNx24HFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_general_operand,
    "Utr,w,w,Dn",
    E_VNx24HFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_nonimmediate_operand,
    "=w,Utr,w,w",
    E_VNx12SFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_general_operand,
    "Utr,w,w,Dn",
    E_VNx12SFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_nonimmediate_operand,
    "=w,Utr,w,w",
    E_VNx6DFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_general_operand,
    "Utr,w,w,Dn",
    E_VNx6DFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_nonimmediate_operand,
    "=w,Utr,w,w",
    E_VNx64QImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_general_operand,
    "Utr,w,w,Dn",
    E_VNx64QImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_nonimmediate_operand,
    "=w,Utr,w,w",
    E_VNx32HImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_general_operand,
    "Utr,w,w,Dn",
    E_VNx32HImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_nonimmediate_operand,
    "=w,Utr,w,w",
    E_VNx16SImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_general_operand,
    "Utr,w,w,Dn",
    E_VNx16SImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_nonimmediate_operand,
    "=w,Utr,w,w",
    E_VNx8DImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_general_operand,
    "Utr,w,w,Dn",
    E_VNx8DImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_nonimmediate_operand,
    "=w,Utr,w,w",
    E_VNx32HFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_general_operand,
    "Utr,w,w,Dn",
    E_VNx32HFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_nonimmediate_operand,
    "=w,Utr,w,w",
    E_VNx16SFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_general_operand,
    "Utr,w,w,Dn",
    E_VNx16SFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_nonimmediate_operand,
    "=w,Utr,w,w",
    E_VNx8DFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_general_operand,
    "Utr,w,w,Dn",
    E_VNx8DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w,w",
    E_VNx32QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_nonmemory_operand,
    "w,Dn",
    E_VNx32QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx16HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_nonmemory_operand,
    "w,Dn",
    E_VNx16HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx8SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_nonmemory_operand,
    "w,Dn",
    E_VNx8SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx4DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_nonmemory_operand,
    "w,Dn",
    E_VNx4DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx16HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_nonmemory_operand,
    "w,Dn",
    E_VNx16HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx8SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_nonmemory_operand,
    "w,Dn",
    E_VNx8SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx4DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_nonmemory_operand,
    "w,Dn",
    E_VNx4DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx48QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_nonmemory_operand,
    "w,Dn",
    E_VNx48QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx24HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_nonmemory_operand,
    "w,Dn",
    E_VNx24HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx12SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_nonmemory_operand,
    "w,Dn",
    E_VNx12SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx6DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_nonmemory_operand,
    "w,Dn",
    E_VNx6DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx24HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_nonmemory_operand,
    "w,Dn",
    E_VNx24HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx12SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_nonmemory_operand,
    "w,Dn",
    E_VNx12SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx6DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_nonmemory_operand,
    "w,Dn",
    E_VNx6DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx64QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_nonmemory_operand,
    "w,Dn",
    E_VNx64QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx32HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_nonmemory_operand,
    "w,Dn",
    E_VNx32HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx16SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_nonmemory_operand,
    "w,Dn",
    E_VNx16SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx8DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_nonmemory_operand,
    "w,Dn",
    E_VNx8DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx32HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_nonmemory_operand,
    "w,Dn",
    E_VNx32HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx16SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_nonmemory_operand,
    "w,Dn",
    E_VNx16SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx8DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_nonmemory_operand,
    "w,Dn",
    E_VNx8DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_struct_nonimmediate_operand,
    "=w,w,Utx",
    E_VNx32QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_struct_nonimmediate_operand,
    "w,Utx,w",
    E_VNx32QImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_struct_nonimmediate_operand,
    "=w,w,Utx",
    E_VNx16HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_struct_nonimmediate_operand,
    "w,Utx,w",
    E_VNx16HImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_struct_nonimmediate_operand,
    "=w,w,Utx",
    E_VNx8SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_struct_nonimmediate_operand,
    "w,Utx,w",
    E_VNx8SImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_struct_nonimmediate_operand,
    "=w,w,Utx",
    E_VNx4DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_struct_nonimmediate_operand,
    "w,Utx,w",
    E_VNx4DImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_struct_nonimmediate_operand,
    "=w,w,Utx",
    E_VNx16HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_struct_nonimmediate_operand,
    "w,Utx,w",
    E_VNx16HFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_struct_nonimmediate_operand,
    "=w,w,Utx",
    E_VNx8SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_struct_nonimmediate_operand,
    "w,Utx,w",
    E_VNx8SFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_struct_nonimmediate_operand,
    "=w,w,Utx",
    E_VNx4DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_struct_nonimmediate_operand,
    "w,Utx,w",
    E_VNx4DFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_struct_nonimmediate_operand,
    "=w,w,Utx",
    E_VNx48QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_struct_nonimmediate_operand,
    "w,Utx,w",
    E_VNx48QImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_struct_nonimmediate_operand,
    "=w,w,Utx",
    E_VNx24HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_struct_nonimmediate_operand,
    "w,Utx,w",
    E_VNx24HImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_struct_nonimmediate_operand,
    "=w,w,Utx",
    E_VNx12SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_struct_nonimmediate_operand,
    "w,Utx,w",
    E_VNx12SImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_struct_nonimmediate_operand,
    "=w,w,Utx",
    E_VNx6DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_struct_nonimmediate_operand,
    "w,Utx,w",
    E_VNx6DImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_struct_nonimmediate_operand,
    "=w,w,Utx",
    E_VNx24HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_struct_nonimmediate_operand,
    "w,Utx,w",
    E_VNx24HFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_struct_nonimmediate_operand,
    "=w,w,Utx",
    E_VNx12SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_struct_nonimmediate_operand,
    "w,Utx,w",
    E_VNx12SFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_struct_nonimmediate_operand,
    "=w,w,Utx",
    E_VNx6DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_struct_nonimmediate_operand,
    "w,Utx,w",
    E_VNx6DFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_struct_nonimmediate_operand,
    "=w,w,Utx",
    E_VNx64QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_struct_nonimmediate_operand,
    "w,Utx,w",
    E_VNx64QImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_struct_nonimmediate_operand,
    "=w,w,Utx",
    E_VNx32HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_struct_nonimmediate_operand,
    "w,Utx,w",
    E_VNx32HImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_struct_nonimmediate_operand,
    "=w,w,Utx",
    E_VNx16SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_struct_nonimmediate_operand,
    "w,Utx,w",
    E_VNx16SImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_struct_nonimmediate_operand,
    "=w,w,Utx",
    E_VNx8DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_struct_nonimmediate_operand,
    "w,Utx,w",
    E_VNx8DImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_struct_nonimmediate_operand,
    "=w,w,Utx",
    E_VNx32HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_struct_nonimmediate_operand,
    "w,Utx,w",
    E_VNx32HFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_struct_nonimmediate_operand,
    "=w,w,Utx",
    E_VNx16SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_struct_nonimmediate_operand,
    "w,Utx,w",
    E_VNx16SFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_sve_struct_nonimmediate_operand,
    "=w,w,Utx",
    E_VNx8DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_struct_nonimmediate_operand,
    "w,Utx,w",
    E_VNx8DFmode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "=Upa,m,Upa,Upa,Upa",
    E_VNx16BImode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "Upa,Upa,m,Dz,Dm",
    E_VNx16BImode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "=Upa,m,Upa,Upa,Upa",
    E_VNx8BImode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "Upa,Upa,m,Dz,Dm",
    E_VNx8BImode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "=Upa,m,Upa,Upa,Upa",
    E_VNx4BImode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "Upa,Upa,m,Dz,Dm",
    E_VNx4BImode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "=Upa,m,Upa,Upa,Upa",
    E_VNx2BImode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "Upa,Upa,m,Dz,Dm",
    E_VNx2BImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_simd_nonimmediate_operand,
    "=r,w,Utv",
    E_QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w,0,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_nonimmediate_operand,
    "=r,w,Utv",
    E_HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w,0,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_nonimmediate_operand,
    "=r,w,Utv",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w,0,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_nonimmediate_operand,
    "=r,w,Utv",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w,0,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_nonimmediate_operand,
    "=r,w,Utv",
    E_HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w,0,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_nonimmediate_operand,
    "=r,w,Utv",
    E_SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w,0,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_nonimmediate_operand,
    "=r,w,Utv",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w,0,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_nonimmediate_operand,
    "=r,w,Utv",
    E_QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w,w,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_nonimmediate_operand,
    "=r,w,Utv",
    E_HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w,w,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_nonimmediate_operand,
    "=r,w,Utv",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w,w,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_nonimmediate_operand,
    "=r,w,Utv",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w,w,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_nonimmediate_operand,
    "=r,w,Utv",
    E_HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w,w,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_nonimmediate_operand,
    "=r,w,Utv",
    E_SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w,w,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_nonimmediate_operand,
    "=r,w,Utv",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w,w,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,w",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,w",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_dup_operand,
    "r,w,Uty",
    E_QImode,
    0,
    0,
    1,
    1
  },
  {
    scratch_operand,
    "=X,X,Upl",
    E_VNx16BImode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_dup_operand,
    "r,w,Uty",
    E_HImode,
    0,
    0,
    1,
    1
  },
  {
    scratch_operand,
    "=X,X,Upl",
    E_VNx8BImode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_dup_operand,
    "r,w,Uty",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    scratch_operand,
    "=X,X,Upl",
    E_VNx4BImode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_dup_operand,
    "r,w,Uty",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    scratch_operand,
    "=X,X,Upl",
    E_VNx2BImode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_dup_operand,
    "r,w,Uty",
    E_HFmode,
    0,
    0,
    1,
    1
  },
  {
    scratch_operand,
    "=X,X,Upl",
    E_VNx8BImode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_dup_operand,
    "r,w,Uty",
    E_SFmode,
    0,
    0,
    1,
    1
  },
  {
    scratch_operand,
    "=X,X,Upl",
    E_VNx4BImode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_dup_operand,
    "r,w,Uty",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    scratch_operand,
    "=X,X,Upl",
    E_VNx2BImode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_ld1r_operand,
    "Uty",
    E_QImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_simd_imm_zero,
    "",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_ld1r_operand,
    "Uty",
    E_HImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_simd_imm_zero,
    "",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_ld1r_operand,
    "Uty",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_simd_imm_zero,
    "",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_ld1r_operand,
    "Uty",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_simd_imm_zero,
    "",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_ld1r_operand,
    "Uty",
    E_HFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_simd_imm_zero,
    "",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_ld1r_operand,
    "Uty",
    E_SFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_simd_imm_zero,
    "",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_ld1r_operand,
    "Uty",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    aarch64_simd_imm_zero,
    "",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_ld1r_operand,
    "Uty",
    E_TImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_ld1r_operand,
    "Uty",
    E_TImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_ld1r_operand,
    "Uty",
    E_TImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_ld1r_operand,
    "Uty",
    E_TImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_ld1r_operand,
    "Uty",
    E_TImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_ld1r_operand,
    "Uty",
    E_TImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_ld1r_operand,
    "Uty",
    E_TImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "=w,w,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_index_operand,
    "Usi,r,r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_index_operand,
    "r,Usi,r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_index_operand,
    "Usi,r,r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_index_operand,
    "r,Usi,r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_index_operand,
    "Usi,r,r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_index_operand,
    "r,Usi,r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_index_operand,
    "Usi,r,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_index_operand,
    "r,Usi,r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx32QImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_VNx32QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx16HImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_VNx16HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx8SImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_VNx8SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx4DImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_VNx4DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx16HFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_VNx16HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx8SFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_VNx8SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx4DFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_VNx4DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx48QImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_VNx48QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx24HImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_VNx24HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx12SImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_VNx12SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx6DImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_VNx6DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx24HFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_VNx24HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx12SFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_VNx12SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx6DFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_VNx6DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx64QImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_VNx64QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx32HImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_VNx32HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx16SImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_VNx16SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx8DImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_VNx8DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx32HFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_VNx32HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx16SFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_VNx16SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx8DFmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "m",
    E_VNx8DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "+m",
    E_VNx32QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_VNx32QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "+m",
    E_VNx16HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_VNx16HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "+m",
    E_VNx8SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_VNx8SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "+m",
    E_VNx4DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_VNx4DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "+m",
    E_VNx16HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_VNx16HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "+m",
    E_VNx8SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_VNx8SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "+m",
    E_VNx4DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_VNx4DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "+m",
    E_VNx48QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_VNx48QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "+m",
    E_VNx24HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_VNx24HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "+m",
    E_VNx12SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_VNx12SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "+m",
    E_VNx6DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_VNx6DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "+m",
    E_VNx24HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_VNx24HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "+m",
    E_VNx12SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_VNx12SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "+m",
    E_VNx6DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_VNx6DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "+m",
    E_VNx64QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_VNx64QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "+m",
    E_VNx32HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_VNx32HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "+m",
    E_VNx16SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_VNx16SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "+m",
    E_VNx8DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_VNx8DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "+m",
    E_VNx32HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_VNx32HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "+m",
    E_VNx16SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_VNx16SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "+m",
    E_VNx8DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "w",
    E_VNx8DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upa",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upa",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upa",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upa",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upa",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upa",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upa",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upa",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%0,0,0,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_add_operand,
    "vsa,vsn,vsi,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%0,0,0,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_add_operand,
    "vsa,vsn,vsi,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%0,0,0,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_add_operand,
    "vsa,vsn,vsi,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%0,0,0,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_add_operand,
    "vsa,vsn,vsi,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_arith_operand,
    "w,vsa",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_arith_operand,
    "w,vsa",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_arith_operand,
    "w,vsa",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_arith_operand,
    "w,vsa",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,?&w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%0,0,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_mul_operand,
    "vsm,w,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,?&w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%0,0,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_mul_operand,
    "vsm,w,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,?&w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%0,0,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_mul_operand,
    "vsm,w,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,?&w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%0,0,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_mul_operand,
    "vsm,w,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_mul_immediate,
    "",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_mul_immediate,
    "",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_mul_immediate,
    "",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_mul_immediate,
    "",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,?&w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%0,w,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,?&w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%0,w,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,?&w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%0,w,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,?&w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%0,w,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,?&w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%0,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,?&w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%0,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,?&w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%0,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,?&w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%0,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,?&w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_mul_operand,
    "w,0,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,?&w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_mul_operand,
    "w,0,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%0,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_logical_operand,
    "vsl,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%0,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_logical_operand,
    "vsl,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%0,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_logical_operand,
    "vsl,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%0,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_logical_operand,
    "vsl,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upa",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upa",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upa",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upa",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upa",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upa",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upa",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upa",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upa",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upa",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upa",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upa",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,?&w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_lshift_operand,
    "Dl,w,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,?&w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_rshift_operand,
    "Dr,w,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,?&w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_lshift_operand,
    "Dl,w,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,?&w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_rshift_operand,
    "Dr,w,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,?&w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_lshift_operand,
    "Dl,w,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,?&w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_rshift_operand,
    "Dr,w,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,?&w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_lshift_operand,
    "Dl,w,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,?&w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_rshift_operand,
    "Dr,w,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_lshift_imm,
    "",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_rshift_imm,
    "",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_lshift_imm,
    "",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_rshift_imm,
    "",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_lshift_imm,
    "",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_rshift_imm,
    "",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_lshift_imm,
    "",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_rshift_imm,
    "",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "rZ",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa,Upa",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_cmp_vsc_operand,
    "vsc,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa,Upa",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_cmp_vsd_operand,
    "vsd,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa,Upa",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_cmp_vsc_operand,
    "vsc,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa,Upa",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_cmp_vsd_operand,
    "vsd,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa,Upa",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_cmp_vsc_operand,
    "vsc,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa,Upa",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_cmp_vsd_operand,
    "vsd,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa,Upa",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_cmp_vsc_operand,
    "vsc,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa,Upa",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_cmp_vsd_operand,
    "vsd,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=Upa,Upa",
    E_VNx16BImode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_cmp_vsc_operand,
    "vsc,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=Upa,Upa",
    E_VNx16BImode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_cmp_vsd_operand,
    "vsd,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=Upa,Upa",
    E_VNx8BImode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_cmp_vsc_operand,
    "vsc,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=Upa,Upa",
    E_VNx8BImode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_cmp_vsd_operand,
    "vsd,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=Upa,Upa",
    E_VNx4BImode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_cmp_vsc_operand,
    "vsc,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=Upa,Upa",
    E_VNx4BImode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_cmp_vsd_operand,
    "vsd,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=Upa,Upa",
    E_VNx2BImode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_cmp_vsc_operand,
    "vsc,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "=Upa,Upa",
    E_VNx2BImode,
    0,
    0,
    0,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_cmp_vsd_operand,
    "vsd,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa,Upa",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_cmp_vsc_operand,
    "vsc,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa,Upa",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_cmp_vsd_operand,
    "vsd,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa,Upa",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_cmp_vsc_operand,
    "vsc,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa,Upa",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_cmp_vsd_operand,
    "vsd,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa,Upa",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_cmp_vsc_operand,
    "vsc,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa,Upa",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_cmp_vsd_operand,
    "vsd,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa,Upa",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_cmp_vsc_operand,
    "vsc,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa,Upa",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_cmp_vsd_operand,
    "vsd,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa,Upa",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_reg_or_zero,
    "Dz,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa,Upa",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_reg_or_zero,
    "Dz,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa,Upa",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_reg_or_zero,
    "Dz,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa,Upa",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_reg_or_zero,
    "Dz,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa,Upa",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_reg_or_zero,
    "Dz,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa,Upa",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_reg_or_zero,
    "Dz,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upa",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upa",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upa",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upa",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upa",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upa",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upa",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_dup_immediate,
    "",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_imm_zero,
    "",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_dup_immediate,
    "",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_imm_zero,
    "",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_dup_immediate,
    "",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_imm_zero,
    "",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_dup_immediate,
    "",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_imm_zero,
    "",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,?&w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%0,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,?&w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%0,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,?&w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%0,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "+w,w,?&w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "+w,w,?&w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "+w,w,?&w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "+w,w,?&w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,?&w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,?&w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,?&w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,?&w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,?&w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,?&w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,?&w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,?&w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_imm_zero,
    "",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_imm_zero,
    "",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_imm_zero,
    "",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_imm_zero,
    "",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,w",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,0",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,0",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,0",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,0",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,w",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,0",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,0",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r,w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,0",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_imm_zero,
    "",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_imm_zero,
    "",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_imm_zero,
    "",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%0,0,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_float_arith_with_sub_operand,
    "vsA,vsN,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%0,0,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_float_arith_with_sub_operand,
    "vsA,vsN,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%0,0,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_float_arith_with_sub_operand,
    "vsA,vsN,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl,Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_float_arith_operand,
    "0,0,vsA,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_float_arith_with_sub_operand,
    "vsA,vsN,0,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_float_arith_operand,
    "0,0,vsA,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_float_arith_with_sub_operand,
    "vsA,vsN,0,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,w,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_float_arith_operand,
    "0,0,vsA,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_float_arith_with_sub_operand,
    "vsA,vsN,0,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%0,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_float_mul_operand,
    "vsM,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%0,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_float_mul_operand,
    "vsM,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%0,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_sve_float_mul_operand,
    "vsM,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,?&w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%0,w,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,?&w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%0,w,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,?&w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "%0,w,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,?&w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,?&w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w,?&w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upa",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upa",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upa",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa",
    E_VNx16BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upa",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upa",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upa",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upa",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=Upa",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upa",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upa",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "+w,w,?&w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "+w,w,?&w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "+w,w,?&w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,0,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,?&w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,?&w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,?&w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,?&w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,?&w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,?&w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_imm_zero,
    "",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_imm_zero,
    "",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_imm_zero,
    "",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,?&w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,?&w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,?&w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,?&w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,?&w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,?&w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&w,&w,?&w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx8BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_reg_or_zero,
    "Dz,0,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&w,&w,?&w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx4BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_reg_or_zero,
    "Dz,0,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&w,&w,?&w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "Upl,Upl,Upl",
    E_VNx2BImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w,w,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_reg_or_zero,
    "Dz,0,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,0",
    E_VNx16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rZ,w",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,0",
    E_VNx8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rZ,w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,0",
    E_VNx4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rZ,w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,0",
    E_VNx2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rZ,w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,0",
    E_VNx8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rZ,w",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,0",
    E_VNx4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rZ,w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w,w",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "0,0",
    E_VNx2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "rZ,w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    register_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_plus_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_plus_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    register_operand,
    "",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_fp_compare_operand,
    "",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    register_operand,
    "",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_fp_compare_operand,
    "",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    cc_register,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    const0_operand,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    scratch_operand,
    "",
    E_DImode,
    0,
    0,
    0,
    0
  },
  {
    scratch_operand,
    "",
    E_DImode,
    0,
    0,
    0,
    0
  },
  {
    memory_operand,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    1
  },
  {
    0,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    1
  },
  {
    0,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    nonimmediate_operand,
    "",
    E_QImode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "",
    E_QImode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "",
    E_HImode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "",
    E_HImode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "",
    E_TImode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "",
    E_TImode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "",
    E_HFmode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "",
    E_HFmode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "",
    E_SFmode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "",
    E_SFmode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "",
    E_TFmode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "",
    E_TFmode,
    0,
    0,
    1,
    1
  },
  {
    memory_operand,
    "",
    E_BLKmode,
    0,
    0,
    1,
    1
  },
  {
    memory_operand,
    "",
    E_BLKmode,
    0,
    0,
    1,
    1
  },
  {
    immediate_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    nonimmediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    nonimmediate_operand,
    "",
    E_QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    nonimmediate_operand,
    "",
    E_QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    nonimmediate_operand,
    "",
    E_HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    nonimmediate_operand,
    "",
    E_HImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    nonimmediate_operand,
    "",
    E_QImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_pluslong_or_poly_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_pluslong_or_poly_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_plus_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_plus_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_TImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_TImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_imm,
    "",
    E_TImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_TImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "",
    E_TImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_TImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_TImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_zero,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_TImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_TImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_TImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_TImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator,
    "",
    E_SImode,
    0,
    1,
    0,
    0
  },
  {
    register_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_plus_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator,
    "",
    E_SImode,
    0,
    1,
    0,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_plus_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator_mode,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    cc_register,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    const0_operand,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator_mode,
    "",
    E_SImode,
    0,
    1,
    0,
    0
  },
  {
    register_operand,
    "",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_fp_compare_operand,
    "",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator_mode,
    "",
    E_SImode,
    0,
    1,
    0,
    0
  },
  {
    register_operand,
    "",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_fp_compare_operand,
    "",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    register_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_plus_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_plus_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    register_operand,
    "",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_fp_compare_operand,
    "",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator,
    "",
    E_VOIDmode,
    0,
    1,
    0,
    0
  },
  {
    register_operand,
    "",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_fp_compare_operand,
    "",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_comparison_operator,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_imm,
    "",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_reg_or_imm,
    "",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_offset_di,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_simd_shift_imm_di,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    general_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    const_int_operand,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    general_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    general_operand,
    "",
    E_HFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    general_operand,
    "",
    E_SFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    general_operand,
    "",
    E_DFmode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_constant_pool_symref,
    "S",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_constant_pool_symref,
    "S",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_constant_pool_symref,
    "S",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_constant_pool_symref,
    "S",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_TFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_constant_pool_symref,
    "S",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_TFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_constant_pool_symref,
    "S",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_constant_pool_symref,
    "S",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_constant_pool_symref,
    "S",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_constant_pool_symref,
    "S",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_constant_pool_symref,
    "S",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_constant_pool_symref,
    "S",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_constant_pool_symref,
    "S",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_constant_pool_symref,
    "S",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_constant_pool_symref,
    "S",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_constant_pool_symref,
    "S",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_constant_pool_symref,
    "S",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&r",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_constant_pool_symref,
    "S",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_constant_pool_symref,
    "S",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_constant_pool_symref,
    "S",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_constant_pool_symref,
    "S",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_constant_pool_symref,
    "S",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_constant_pool_symref,
    "S",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_constant_pool_symref,
    "S",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_constant_pool_symref,
    "S",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_constant_pool_symref,
    "S",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_constant_pool_symref,
    "S",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_TImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_TImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_TFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_TFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=&r",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=r",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "r",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_valid_symref,
    "S",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_valid_symref,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_valid_symref,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_valid_symref,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    memory_operand,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    1
  },
  {
    memory_operand,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    1
  },
  {
    0,
    "",
    E_VOIDmode,
    0,
    0,
    1,
    0
  },
  {
    nonimmediate_operand,
    "",
    E_V8QImode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "",
    E_V8QImode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "",
    E_V16QImode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "",
    E_V16QImode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "",
    E_V4HImode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "",
    E_V4HImode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "",
    E_V8HImode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "",
    E_V8HImode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "",
    E_V2SImode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "",
    E_V2SImode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "",
    E_V4SImode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "",
    E_V4SImode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "",
    E_V2DImode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "",
    E_V2DImode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "",
    E_V4HFmode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "",
    E_V4HFmode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "",
    E_V8HFmode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "",
    E_V8HFmode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "",
    E_V2SFmode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "",
    E_V2SFmode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "",
    E_V4SFmode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "",
    E_V4SFmode,
    0,
    0,
    1,
    1
  },
  {
    nonimmediate_operand,
    "",
    E_V2DFmode,
    0,
    0,
    1,
    1
  },
  {
    general_operand,
    "",
    E_V2DFmode,
    0,
    0,
    1,
    1
  },
  {
    0,
    "",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    0,
    "",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    general_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    general_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    general_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    general_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    general_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    general_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    general_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    1
  },
  {
    register_operand,
    "",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    aarch64_shift_imm64_di,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "+w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "+w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "+w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "+w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "+w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "+w",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "+w",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "+w",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "+w",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "+w",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "+w",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "+w",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    immediate_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "=w",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "w",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V16QImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V8HImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V8HFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4SImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V4SFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V2DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DImode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_DFmode,
    0,
    0,
    1,
    0
  },
  {
    register_operand,
    "",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    nonmemory_operand,
    "",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    nonmemory_operand,
    "",
    E_V8QImode,
    0,
    0,
    1,
    0
  },
  {
    "",
    0,