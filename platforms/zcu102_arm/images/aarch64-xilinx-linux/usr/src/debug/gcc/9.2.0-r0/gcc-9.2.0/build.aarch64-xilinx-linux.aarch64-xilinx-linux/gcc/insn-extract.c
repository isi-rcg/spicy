/* Generated automatically by the program `genextract'
   from the machine description file `md'.  */

#define IN_TARGET_CODE 1
#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "rtl.h"
#include "insn-config.h"
#include "recog.h"
#include "diagnostic-core.h"

/* This variable is used as the "location" of any missing operand
   whose numbers are skipped by a given pattern.  */
static rtx junk ATTRIBUTE_UNUSED;

void
insn_extract (rtx_insn *insn)
{
  rtx *ro = recog_data.operand;
  rtx **ro_loc = recog_data.operand_loc;
  rtx pat = PATTERN (insn);
  int i ATTRIBUTE_UNUSED; /* only for peepholes */

  if (flag_checking)
    {
      memset (ro, 0xab, sizeof (*ro) * MAX_RECOG_OPERANDS);
      memset (ro_loc, 0xab, sizeof (*ro_loc) * MAX_RECOG_OPERANDS);
    }

  switch (INSN_CODE (insn))
    {
    default:
      /* Control reaches here if insn_extract has been called with an
         unrecognizable insn (code -1), or an insn whose INSN_CODE
         corresponds to a DEFINE_EXPAND in the machine description;
         either way, a bug.  */
      if (INSN_CODE (insn) < 0)
        fatal_insn ("unrecognizable insn:", insn);
      else
        fatal_insn ("insn with invalid code number:", insn);

    case 5129:  /* *cond_fmsvnx2df_any */
    case 5128:  /* *cond_fnmsvnx2df_any */
    case 5127:  /* *cond_fnmavnx2df_any */
    case 5126:  /* *cond_fmavnx2df_any */
    case 5125:  /* *cond_fmsvnx4sf_any */
    case 5124:  /* *cond_fnmsvnx4sf_any */
    case 5123:  /* *cond_fnmavnx4sf_any */
    case 5122:  /* *cond_fmavnx4sf_any */
    case 5121:  /* *cond_fmsvnx8hf_any */
    case 5120:  /* *cond_fnmsvnx8hf_any */
    case 5119:  /* *cond_fnmavnx8hf_any */
    case 5118:  /* *cond_fmavnx8hf_any */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (pat, 1), 0, 0));
      ro[2] = *(ro_loc[2] = &XVECEXP (XVECEXP (XEXP (pat, 1), 0, 1), 0, 0));
      ro[3] = *(ro_loc[3] = &XVECEXP (XVECEXP (XEXP (pat, 1), 0, 1), 0, 1));
      ro[4] = *(ro_loc[4] = &XVECEXP (XVECEXP (XEXP (pat, 1), 0, 1), 0, 2));
      ro[5] = *(ro_loc[5] = &XVECEXP (XEXP (pat, 1), 0, 2));
      break;

    case 5117:  /* *cond_fmsvnx2df_4 */
    case 5116:  /* *cond_fnmsvnx2df_4 */
    case 5115:  /* *cond_fnmavnx2df_4 */
    case 5114:  /* *cond_fmavnx2df_4 */
    case 5113:  /* *cond_fmsvnx4sf_4 */
    case 5112:  /* *cond_fnmsvnx4sf_4 */
    case 5111:  /* *cond_fnmavnx4sf_4 */
    case 5110:  /* *cond_fmavnx4sf_4 */
    case 5109:  /* *cond_fmsvnx8hf_4 */
    case 5108:  /* *cond_fnmsvnx8hf_4 */
    case 5107:  /* *cond_fnmavnx8hf_4 */
    case 5106:  /* *cond_fmavnx8hf_4 */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (pat, 1), 0, 0));
      ro[2] = *(ro_loc[2] = &XVECEXP (XVECEXP (XEXP (pat, 1), 0, 1), 0, 0));
      ro[3] = *(ro_loc[3] = &XVECEXP (XVECEXP (XEXP (pat, 1), 0, 1), 0, 1));
      ro[4] = *(ro_loc[4] = &XVECEXP (XVECEXP (XEXP (pat, 1), 0, 1), 0, 2));
      recog_data.dup_loc[0] = &XVECEXP (XEXP (pat, 1), 0, 2);
      recog_data.dup_num[0] = 4;
      break;

    case 5105:  /* *cond_fmsvnx2df_2 */
    case 5104:  /* *cond_fnmsvnx2df_2 */
    case 5103:  /* *cond_fnmavnx2df_2 */
    case 5102:  /* *cond_fmavnx2df_2 */
    case 5101:  /* *cond_fmsvnx4sf_2 */
    case 5100:  /* *cond_fnmsvnx4sf_2 */
    case 5099:  /* *cond_fnmavnx4sf_2 */
    case 5098:  /* *cond_fmavnx4sf_2 */
    case 5097:  /* *cond_fmsvnx8hf_2 */
    case 5096:  /* *cond_fnmsvnx8hf_2 */
    case 5095:  /* *cond_fnmavnx8hf_2 */
    case 5094:  /* *cond_fmavnx8hf_2 */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (pat, 1), 0, 0));
      ro[2] = *(ro_loc[2] = &XVECEXP (XVECEXP (XEXP (pat, 1), 0, 1), 0, 0));
      ro[3] = *(ro_loc[3] = &XVECEXP (XVECEXP (XEXP (pat, 1), 0, 1), 0, 1));
      ro[4] = *(ro_loc[4] = &XVECEXP (XVECEXP (XEXP (pat, 1), 0, 1), 0, 2));
      recog_data.dup_loc[0] = &XVECEXP (XEXP (pat, 1), 0, 2);
      recog_data.dup_num[0] = 2;
      break;

    case 5093:  /* *cond_sminvnx2df_any */
    case 5092:  /* *cond_smaxvnx2df_any */
    case 5091:  /* *cond_divvnx2df_any */
    case 5090:  /* *cond_mulvnx2df_any */
    case 5089:  /* *cond_subvnx2df_any */
    case 5088:  /* *cond_addvnx2df_any */
    case 5087:  /* *cond_sminvnx4sf_any */
    case 5086:  /* *cond_smaxvnx4sf_any */
    case 5085:  /* *cond_divvnx4sf_any */
    case 5084:  /* *cond_mulvnx4sf_any */
    case 5083:  /* *cond_subvnx4sf_any */
    case 5082:  /* *cond_addvnx4sf_any */
    case 5081:  /* *cond_sminvnx8hf_any */
    case 5080:  /* *cond_smaxvnx8hf_any */
    case 5079:  /* *cond_divvnx8hf_any */
    case 5078:  /* *cond_mulvnx8hf_any */
    case 5077:  /* *cond_subvnx8hf_any */
    case 5076:  /* *cond_addvnx8hf_any */
    case 5075:  /* *cond_sminvnx2df_z */
    case 5074:  /* *cond_smaxvnx2df_z */
    case 5073:  /* *cond_divvnx2df_z */
    case 5072:  /* *cond_mulvnx2df_z */
    case 5071:  /* *cond_subvnx2df_z */
    case 5070:  /* *cond_addvnx2df_z */
    case 5069:  /* *cond_sminvnx4sf_z */
    case 5068:  /* *cond_smaxvnx4sf_z */
    case 5067:  /* *cond_divvnx4sf_z */
    case 5066:  /* *cond_mulvnx4sf_z */
    case 5065:  /* *cond_subvnx4sf_z */
    case 5064:  /* *cond_addvnx4sf_z */
    case 5063:  /* *cond_sminvnx8hf_z */
    case 5062:  /* *cond_smaxvnx8hf_z */
    case 5061:  /* *cond_divvnx8hf_z */
    case 5060:  /* *cond_mulvnx8hf_z */
    case 5059:  /* *cond_subvnx8hf_z */
    case 5058:  /* *cond_addvnx8hf_z */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (pat, 1), 0, 0));
      ro[2] = *(ro_loc[2] = &XVECEXP (XVECEXP (XEXP (pat, 1), 0, 1), 0, 0));
      ro[3] = *(ro_loc[3] = &XVECEXP (XVECEXP (XEXP (pat, 1), 0, 1), 0, 1));
      ro[4] = *(ro_loc[4] = &XVECEXP (XEXP (pat, 1), 0, 2));
      break;

    case 5057:  /* *cond_sminvnx2df_3 */
    case 5056:  /* *cond_smaxvnx2df_3 */
    case 5055:  /* *cond_divvnx2df_3 */
    case 5054:  /* *cond_mulvnx2df_3 */
    case 5053:  /* *cond_subvnx2df_3 */
    case 5052:  /* *cond_addvnx2df_3 */
    case 5051:  /* *cond_sminvnx4sf_3 */
    case 5050:  /* *cond_smaxvnx4sf_3 */
    case 5049:  /* *cond_divvnx4sf_3 */
    case 5048:  /* *cond_mulvnx4sf_3 */
    case 5047:  /* *cond_subvnx4sf_3 */
    case 5046:  /* *cond_addvnx4sf_3 */
    case 5045:  /* *cond_sminvnx8hf_3 */
    case 5044:  /* *cond_smaxvnx8hf_3 */
    case 5043:  /* *cond_divvnx8hf_3 */
    case 5042:  /* *cond_mulvnx8hf_3 */
    case 5041:  /* *cond_subvnx8hf_3 */
    case 5040:  /* *cond_addvnx8hf_3 */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (pat, 1), 0, 0));
      ro[2] = *(ro_loc[2] = &XVECEXP (XVECEXP (XEXP (pat, 1), 0, 1), 0, 0));
      ro[3] = *(ro_loc[3] = &XVECEXP (XVECEXP (XEXP (pat, 1), 0, 1), 0, 1));
      recog_data.dup_loc[0] = &XVECEXP (XEXP (pat, 1), 0, 2);
      recog_data.dup_num[0] = 3;
      break;

    case 5039:  /* *cond_sminvnx2df_2 */
    case 5038:  /* *cond_smaxvnx2df_2 */
    case 5037:  /* *cond_divvnx2df_2 */
    case 5036:  /* *cond_mulvnx2df_2 */
    case 5035:  /* *cond_subvnx2df_2 */
    case 5034:  /* *cond_addvnx2df_2 */
    case 5033:  /* *cond_sminvnx4sf_2 */
    case 5032:  /* *cond_smaxvnx4sf_2 */
    case 5031:  /* *cond_divvnx4sf_2 */
    case 5030:  /* *cond_mulvnx4sf_2 */
    case 5029:  /* *cond_subvnx4sf_2 */
    case 5028:  /* *cond_addvnx4sf_2 */
    case 5027:  /* *cond_sminvnx8hf_2 */
    case 5026:  /* *cond_smaxvnx8hf_2 */
    case 5025:  /* *cond_divvnx8hf_2 */
    case 5024:  /* *cond_mulvnx8hf_2 */
    case 5023:  /* *cond_subvnx8hf_2 */
    case 5022:  /* *cond_addvnx8hf_2 */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (pat, 1), 0, 0));
      ro[2] = *(ro_loc[2] = &XVECEXP (XVECEXP (XEXP (pat, 1), 0, 1), 0, 0));
      ro[3] = *(ro_loc[3] = &XVECEXP (XVECEXP (XEXP (pat, 1), 0, 1), 0, 1));
      recog_data.dup_loc[0] = &XVECEXP (XEXP (pat, 1), 0, 2);
      recog_data.dup_num[0] = 2;
      break;

    case 5021:  /* *cond_sminvnx2df_0 */
    case 5020:  /* *cond_smaxvnx2df_0 */
    case 5019:  /* *cond_divvnx2df_0 */
    case 5018:  /* *cond_mulvnx2df_0 */
    case 5017:  /* *cond_subvnx2df_0 */
    case 5016:  /* *cond_addvnx2df_0 */
    case 5015:  /* *cond_sminvnx4sf_0 */
    case 5014:  /* *cond_smaxvnx4sf_0 */
    case 5013:  /* *cond_divvnx4sf_0 */
    case 5012:  /* *cond_mulvnx4sf_0 */
    case 5011:  /* *cond_subvnx4sf_0 */
    case 5010:  /* *cond_addvnx4sf_0 */
    case 5009:  /* *cond_sminvnx8hf_0 */
    case 5008:  /* *cond_smaxvnx8hf_0 */
    case 5007:  /* *cond_divvnx8hf_0 */
    case 5006:  /* *cond_mulvnx8hf_0 */
    case 5005:  /* *cond_subvnx8hf_0 */
    case 5004:  /* *cond_addvnx8hf_0 */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (pat, 1), 0, 0));
      ro[2] = *(ro_loc[2] = &XVECEXP (XVECEXP (XEXP (pat, 1), 0, 1), 0, 0));
      ro[3] = *(ro_loc[3] = &XVECEXP (XVECEXP (XEXP (pat, 1), 0, 1), 0, 1));
      recog_data.dup_loc[0] = &XVECEXP (XEXP (pat, 1), 0, 2);
      recog_data.dup_num[0] = 0;
      break;

    case 4914:  /* *fnmsvnx2df4 */
    case 4913:  /* *fnmsvnx4sf4 */
    case 4912:  /* *fnmsvnx8hf4 */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (pat, 1), 0, 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XVECEXP (XEXP (pat, 1), 0, 1), 2), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XVECEXP (XEXP (pat, 1), 0, 1), 0), 0));
      ro[4] = *(ro_loc[4] = &XEXP (XVECEXP (XEXP (pat, 1), 0, 1), 1));
      break;

    case 4911:  /* *fmsvnx2df4 */
    case 4910:  /* *fmsvnx4sf4 */
    case 4909:  /* *fmsvnx8hf4 */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (pat, 1), 0, 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XVECEXP (XEXP (pat, 1), 0, 1), 2), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XVECEXP (XEXP (pat, 1), 0, 1), 0));
      ro[4] = *(ro_loc[4] = &XEXP (XVECEXP (XEXP (pat, 1), 0, 1), 1));
      break;

    case 4908:  /* *fnmavnx2df4 */
    case 4907:  /* *fnmavnx4sf4 */
    case 4906:  /* *fnmavnx8hf4 */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (pat, 1), 0, 0));
      ro[2] = *(ro_loc[2] = &XEXP (XVECEXP (XEXP (pat, 1), 0, 1), 2));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XVECEXP (XEXP (pat, 1), 0, 1), 0), 0));
      ro[4] = *(ro_loc[4] = &XEXP (XVECEXP (XEXP (pat, 1), 0, 1), 1));
      break;

    case 4905:  /* *fmavnx2df4 */
    case 4904:  /* *fmavnx4sf4 */
    case 4903:  /* *fmavnx8hf4 */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (pat, 1), 0, 0));
      ro[2] = *(ro_loc[2] = &XEXP (XVECEXP (XEXP (pat, 1), 0, 1), 2));
      ro[3] = *(ro_loc[3] = &XEXP (XVECEXP (XEXP (pat, 1), 0, 1), 0));
      ro[4] = *(ro_loc[4] = &XEXP (XVECEXP (XEXP (pat, 1), 0, 1), 1));
      break;

    case 4884:  /* *pred_fold_left_plus_vnx2df */
    case 4883:  /* *pred_fold_left_plus_vnx4sf */
    case 4882:  /* *pred_fold_left_plus_vnx8hf */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (pat, 1), 0, 0));
      ro[2] = *(ro_loc[2] = &XVECEXP (XVECEXP (XEXP (pat, 1), 0, 1), 0, 0));
      ro[3] = *(ro_loc[3] = &XVECEXP (XVECEXP (XEXP (pat, 1), 0, 1), 0, 1));
      ro[4] = *(ro_loc[4] = &XVECEXP (XVECEXP (XEXP (pat, 1), 0, 1), 0, 2));
      break;

    case 4824:  /* *cond_udivvnx2di_any */
    case 4823:  /* *cond_divvnx2di_any */
    case 4822:  /* *cond_udivvnx2di_any */
    case 4821:  /* *cond_divvnx2di_any */
    case 4820:  /* *cond_udivvnx4si_any */
    case 4819:  /* *cond_divvnx4si_any */
    case 4818:  /* *cond_udivvnx4si_any */
    case 4817:  /* *cond_divvnx4si_any */
    case 4816:  /* *cond_udivvnx8hi_any */
    case 4815:  /* *cond_divvnx8hi_any */
    case 4814:  /* *cond_udivvnx8hi_any */
    case 4813:  /* *cond_divvnx8hi_any */
    case 4812:  /* *cond_udivvnx16qi_any */
    case 4811:  /* *cond_divvnx16qi_any */
    case 4810:  /* *cond_udivvnx16qi_any */
    case 4809:  /* *cond_divvnx16qi_any */
    case 4808:  /* *cond_xorvnx2di_any */
    case 4807:  /* *cond_iorvnx2di_any */
    case 4806:  /* *cond_andvnx2di_any */
    case 4805:  /* *cond_uminvnx2di_any */
    case 4804:  /* *cond_sminvnx2di_any */
    case 4803:  /* *cond_umaxvnx2di_any */
    case 4802:  /* *cond_smaxvnx2di_any */
    case 4801:  /* *cond_mulvnx2di_any */
    case 4800:  /* *cond_subvnx2di_any */
    case 4799:  /* *cond_addvnx2di_any */
    case 4798:  /* *cond_xorvnx4si_any */
    case 4797:  /* *cond_iorvnx4si_any */
    case 4796:  /* *cond_andvnx4si_any */
    case 4795:  /* *cond_uminvnx4si_any */
    case 4794:  /* *cond_sminvnx4si_any */
    case 4793:  /* *cond_umaxvnx4si_any */
    case 4792:  /* *cond_smaxvnx4si_any */
    case 4791:  /* *cond_mulvnx4si_any */
    case 4790:  /* *cond_subvnx4si_any */
    case 4789:  /* *cond_addvnx4si_any */
    case 4788:  /* *cond_xorvnx8hi_any */
    case 4787:  /* *cond_iorvnx8hi_any */
    case 4786:  /* *cond_andvnx8hi_any */
    case 4785:  /* *cond_uminvnx8hi_any */
    case 4784:  /* *cond_sminvnx8hi_any */
    case 4783:  /* *cond_umaxvnx8hi_any */
    case 4782:  /* *cond_smaxvnx8hi_any */
    case 4781:  /* *cond_mulvnx8hi_any */
    case 4780:  /* *cond_subvnx8hi_any */
    case 4779:  /* *cond_addvnx8hi_any */
    case 4778:  /* *cond_xorvnx16qi_any */
    case 4777:  /* *cond_iorvnx16qi_any */
    case 4776:  /* *cond_andvnx16qi_any */
    case 4775:  /* *cond_uminvnx16qi_any */
    case 4774:  /* *cond_sminvnx16qi_any */
    case 4773:  /* *cond_umaxvnx16qi_any */
    case 4772:  /* *cond_smaxvnx16qi_any */
    case 4771:  /* *cond_mulvnx16qi_any */
    case 4770:  /* *cond_subvnx16qi_any */
    case 4769:  /* *cond_addvnx16qi_any */
    case 4768:  /* *cond_udivvnx2di_z */
    case 4767:  /* *cond_divvnx2di_z */
    case 4766:  /* *cond_udivvnx4si_z */
    case 4765:  /* *cond_divvnx4si_z */
    case 4764:  /* *cond_xorvnx2di_z */
    case 4763:  /* *cond_iorvnx2di_z */
    case 4762:  /* *cond_andvnx2di_z */
    case 4761:  /* *cond_uminvnx2di_z */
    case 4760:  /* *cond_sminvnx2di_z */
    case 4759:  /* *cond_umaxvnx2di_z */
    case 4758:  /* *cond_smaxvnx2di_z */
    case 4757:  /* *cond_mulvnx2di_z */
    case 4756:  /* *cond_subvnx2di_z */
    case 4755:  /* *cond_addvnx2di_z */
    case 4754:  /* *cond_xorvnx4si_z */
    case 4753:  /* *cond_iorvnx4si_z */
    case 4752:  /* *cond_andvnx4si_z */
    case 4751:  /* *cond_uminvnx4si_z */
    case 4750:  /* *cond_sminvnx4si_z */
    case 4749:  /* *cond_umaxvnx4si_z */
    case 4748:  /* *cond_smaxvnx4si_z */
    case 4747:  /* *cond_mulvnx4si_z */
    case 4746:  /* *cond_subvnx4si_z */
    case 4745:  /* *cond_addvnx4si_z */
    case 4744:  /* *cond_xorvnx8hi_z */
    case 4743:  /* *cond_iorvnx8hi_z */
    case 4742:  /* *cond_andvnx8hi_z */
    case 4741:  /* *cond_uminvnx8hi_z */
    case 4740:  /* *cond_sminvnx8hi_z */
    case 4739:  /* *cond_umaxvnx8hi_z */
    case 4738:  /* *cond_smaxvnx8hi_z */
    case 4737:  /* *cond_mulvnx8hi_z */
    case 4736:  /* *cond_subvnx8hi_z */
    case 4735:  /* *cond_addvnx8hi_z */
    case 4734:  /* *cond_xorvnx16qi_z */
    case 4733:  /* *cond_iorvnx16qi_z */
    case 4732:  /* *cond_andvnx16qi_z */
    case 4731:  /* *cond_uminvnx16qi_z */
    case 4730:  /* *cond_sminvnx16qi_z */
    case 4729:  /* *cond_umaxvnx16qi_z */
    case 4728:  /* *cond_smaxvnx16qi_z */
    case 4727:  /* *cond_mulvnx16qi_z */
    case 4726:  /* *cond_subvnx16qi_z */
    case 4725:  /* *cond_addvnx16qi_z */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (pat, 1), 0, 0));
      ro[2] = *(ro_loc[2] = &XEXP (XVECEXP (XEXP (pat, 1), 0, 1), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XVECEXP (XEXP (pat, 1), 0, 1), 1));
      ro[4] = *(ro_loc[4] = &XVECEXP (XEXP (pat, 1), 0, 2));
      break;

    case 4724:  /* *cond_udivvnx2di_3 */
    case 4723:  /* *cond_divvnx2di_3 */
    case 4722:  /* *cond_udivvnx4si_3 */
    case 4721:  /* *cond_divvnx4si_3 */
    case 4720:  /* *cond_xorvnx2di_3 */
    case 4719:  /* *cond_iorvnx2di_3 */
    case 4718:  /* *cond_andvnx2di_3 */
    case 4717:  /* *cond_uminvnx2di_3 */
    case 4716:  /* *cond_sminvnx2di_3 */
    case 4715:  /* *cond_umaxvnx2di_3 */
    case 4714:  /* *cond_smaxvnx2di_3 */
    case 4713:  /* *cond_mulvnx2di_3 */
    case 4712:  /* *cond_subvnx2di_3 */
    case 4711:  /* *cond_addvnx2di_3 */
    case 4710:  /* *cond_xorvnx4si_3 */
    case 4709:  /* *cond_iorvnx4si_3 */
    case 4708:  /* *cond_andvnx4si_3 */
    case 4707:  /* *cond_uminvnx4si_3 */
    case 4706:  /* *cond_sminvnx4si_3 */
    case 4705:  /* *cond_umaxvnx4si_3 */
    case 4704:  /* *cond_smaxvnx4si_3 */
    case 4703:  /* *cond_mulvnx4si_3 */
    case 4702:  /* *cond_subvnx4si_3 */
    case 4701:  /* *cond_addvnx4si_3 */
    case 4700:  /* *cond_xorvnx8hi_3 */
    case 4699:  /* *cond_iorvnx8hi_3 */
    case 4698:  /* *cond_andvnx8hi_3 */
    case 4697:  /* *cond_uminvnx8hi_3 */
    case 4696:  /* *cond_sminvnx8hi_3 */
    case 4695:  /* *cond_umaxvnx8hi_3 */
    case 4694:  /* *cond_smaxvnx8hi_3 */
    case 4693:  /* *cond_mulvnx8hi_3 */
    case 4692:  /* *cond_subvnx8hi_3 */
    case 4691:  /* *cond_addvnx8hi_3 */
    case 4690:  /* *cond_xorvnx16qi_3 */
    case 4689:  /* *cond_iorvnx16qi_3 */
    case 4688:  /* *cond_andvnx16qi_3 */
    case 4687:  /* *cond_uminvnx16qi_3 */
    case 4686:  /* *cond_sminvnx16qi_3 */
    case 4685:  /* *cond_umaxvnx16qi_3 */
    case 4684:  /* *cond_smaxvnx16qi_3 */
    case 4683:  /* *cond_mulvnx16qi_3 */
    case 4682:  /* *cond_subvnx16qi_3 */
    case 4681:  /* *cond_addvnx16qi_3 */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (pat, 1), 0, 0));
      ro[2] = *(ro_loc[2] = &XEXP (XVECEXP (XEXP (pat, 1), 0, 1), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XVECEXP (XEXP (pat, 1), 0, 1), 1));
      recog_data.dup_loc[0] = &XVECEXP (XEXP (pat, 1), 0, 2);
      recog_data.dup_num[0] = 3;
      break;

    case 4680:  /* *cond_udivvnx2di_2 */
    case 4679:  /* *cond_divvnx2di_2 */
    case 4678:  /* *cond_udivvnx4si_2 */
    case 4677:  /* *cond_divvnx4si_2 */
    case 4676:  /* *cond_xorvnx2di_2 */
    case 4675:  /* *cond_iorvnx2di_2 */
    case 4674:  /* *cond_andvnx2di_2 */
    case 4673:  /* *cond_uminvnx2di_2 */
    case 4672:  /* *cond_sminvnx2di_2 */
    case 4671:  /* *cond_umaxvnx2di_2 */
    case 4670:  /* *cond_smaxvnx2di_2 */
    case 4669:  /* *cond_mulvnx2di_2 */
    case 4668:  /* *cond_subvnx2di_2 */
    case 4667:  /* *cond_addvnx2di_2 */
    case 4666:  /* *cond_xorvnx4si_2 */
    case 4665:  /* *cond_iorvnx4si_2 */
    case 4664:  /* *cond_andvnx4si_2 */
    case 4663:  /* *cond_uminvnx4si_2 */
    case 4662:  /* *cond_sminvnx4si_2 */
    case 4661:  /* *cond_umaxvnx4si_2 */
    case 4660:  /* *cond_smaxvnx4si_2 */
    case 4659:  /* *cond_mulvnx4si_2 */
    case 4658:  /* *cond_subvnx4si_2 */
    case 4657:  /* *cond_addvnx4si_2 */
    case 4656:  /* *cond_xorvnx8hi_2 */
    case 4655:  /* *cond_iorvnx8hi_2 */
    case 4654:  /* *cond_andvnx8hi_2 */
    case 4653:  /* *cond_uminvnx8hi_2 */
    case 4652:  /* *cond_sminvnx8hi_2 */
    case 4651:  /* *cond_umaxvnx8hi_2 */
    case 4650:  /* *cond_smaxvnx8hi_2 */
    case 4649:  /* *cond_mulvnx8hi_2 */
    case 4648:  /* *cond_subvnx8hi_2 */
    case 4647:  /* *cond_addvnx8hi_2 */
    case 4646:  /* *cond_xorvnx16qi_2 */
    case 4645:  /* *cond_iorvnx16qi_2 */
    case 4644:  /* *cond_andvnx16qi_2 */
    case 4643:  /* *cond_uminvnx16qi_2 */
    case 4642:  /* *cond_sminvnx16qi_2 */
    case 4641:  /* *cond_umaxvnx16qi_2 */
    case 4640:  /* *cond_smaxvnx16qi_2 */
    case 4639:  /* *cond_mulvnx16qi_2 */
    case 4638:  /* *cond_subvnx16qi_2 */
    case 4637:  /* *cond_addvnx16qi_2 */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (pat, 1), 0, 0));
      ro[2] = *(ro_loc[2] = &XEXP (XVECEXP (XEXP (pat, 1), 0, 1), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XVECEXP (XEXP (pat, 1), 0, 1), 1));
      recog_data.dup_loc[0] = &XVECEXP (XEXP (pat, 1), 0, 2);
      recog_data.dup_num[0] = 2;
      break;

    case 4636:  /* *cond_udivvnx2di_0 */
    case 4635:  /* *cond_divvnx2di_0 */
    case 4634:  /* *cond_udivvnx4si_0 */
    case 4633:  /* *cond_divvnx4si_0 */
    case 4632:  /* *cond_xorvnx2di_0 */
    case 4631:  /* *cond_iorvnx2di_0 */
    case 4630:  /* *cond_andvnx2di_0 */
    case 4629:  /* *cond_uminvnx2di_0 */
    case 4628:  /* *cond_sminvnx2di_0 */
    case 4627:  /* *cond_umaxvnx2di_0 */
    case 4626:  /* *cond_smaxvnx2di_0 */
    case 4625:  /* *cond_mulvnx2di_0 */
    case 4624:  /* *cond_subvnx2di_0 */
    case 4623:  /* *cond_addvnx2di_0 */
    case 4622:  /* *cond_xorvnx4si_0 */
    case 4621:  /* *cond_iorvnx4si_0 */
    case 4620:  /* *cond_andvnx4si_0 */
    case 4619:  /* *cond_uminvnx4si_0 */
    case 4618:  /* *cond_sminvnx4si_0 */
    case 4617:  /* *cond_umaxvnx4si_0 */
    case 4616:  /* *cond_smaxvnx4si_0 */
    case 4615:  /* *cond_mulvnx4si_0 */
    case 4614:  /* *cond_subvnx4si_0 */
    case 4613:  /* *cond_addvnx4si_0 */
    case 4612:  /* *cond_xorvnx8hi_0 */
    case 4611:  /* *cond_iorvnx8hi_0 */
    case 4610:  /* *cond_andvnx8hi_0 */
    case 4609:  /* *cond_uminvnx8hi_0 */
    case 4608:  /* *cond_sminvnx8hi_0 */
    case 4607:  /* *cond_umaxvnx8hi_0 */
    case 4606:  /* *cond_smaxvnx8hi_0 */
    case 4605:  /* *cond_mulvnx8hi_0 */
    case 4604:  /* *cond_subvnx8hi_0 */
    case 4603:  /* *cond_addvnx8hi_0 */
    case 4602:  /* *cond_xorvnx16qi_0 */
    case 4601:  /* *cond_iorvnx16qi_0 */
    case 4600:  /* *cond_andvnx16qi_0 */
    case 4599:  /* *cond_uminvnx16qi_0 */
    case 4598:  /* *cond_sminvnx16qi_0 */
    case 4597:  /* *cond_umaxvnx16qi_0 */
    case 4596:  /* *cond_smaxvnx16qi_0 */
    case 4595:  /* *cond_mulvnx16qi_0 */
    case 4594:  /* *cond_subvnx16qi_0 */
    case 4593:  /* *cond_addvnx16qi_0 */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (pat, 1), 0, 0));
      ro[2] = *(ro_loc[2] = &XEXP (XVECEXP (XEXP (pat, 1), 0, 1), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XVECEXP (XEXP (pat, 1), 0, 1), 1));
      recog_data.dup_loc[0] = &XVECEXP (XEXP (pat, 1), 0, 2);
      recog_data.dup_num[0] = 0;
      break;

    case 4554:  /* vcond_mask_vnx2dfvnx2bi */
    case 4553:  /* vcond_mask_vnx4sfvnx4bi */
    case 4552:  /* vcond_mask_vnx8hfvnx8bi */
    case 4551:  /* vcond_mask_vnx2divnx2bi */
    case 4550:  /* vcond_mask_vnx4sivnx4bi */
    case 4549:  /* vcond_mask_vnx8hivnx8bi */
    case 4548:  /* vcond_mask_vnx16qivnx16bi */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (pat, 1), 0, 1));
      ro[2] = *(ro_loc[2] = &XVECEXP (XEXP (pat, 1), 0, 2));
      ro[3] = *(ro_loc[3] = &XVECEXP (XEXP (pat, 1), 0, 0));
      break;

    case 4466:  /* *pred_cmphivnx2di */
    case 4465:  /* *pred_cmphsvnx2di */
    case 4464:  /* *pred_cmplsvnx2di */
    case 4463:  /* *pred_cmplovnx2di */
    case 4462:  /* *pred_cmpgtvnx2di */
    case 4461:  /* *pred_cmpgevnx2di */
    case 4460:  /* *pred_cmpnevnx2di */
    case 4459:  /* *pred_cmpeqvnx2di */
    case 4458:  /* *pred_cmplevnx2di */
    case 4457:  /* *pred_cmpltvnx2di */
    case 4456:  /* *pred_cmphivnx4si */
    case 4455:  /* *pred_cmphsvnx4si */
    case 4454:  /* *pred_cmplsvnx4si */
    case 4453:  /* *pred_cmplovnx4si */
    case 4452:  /* *pred_cmpgtvnx4si */
    case 4451:  /* *pred_cmpgevnx4si */
    case 4450:  /* *pred_cmpnevnx4si */
    case 4449:  /* *pred_cmpeqvnx4si */
    case 4448:  /* *pred_cmplevnx4si */
    case 4447:  /* *pred_cmpltvnx4si */
    case 4446:  /* *pred_cmphivnx8hi */
    case 4445:  /* *pred_cmphsvnx8hi */
    case 4444:  /* *pred_cmplsvnx8hi */
    case 4443:  /* *pred_cmplovnx8hi */
    case 4442:  /* *pred_cmpgtvnx8hi */
    case 4441:  /* *pred_cmpgevnx8hi */
    case 4440:  /* *pred_cmpnevnx8hi */
    case 4439:  /* *pred_cmpeqvnx8hi */
    case 4438:  /* *pred_cmplevnx8hi */
    case 4437:  /* *pred_cmpltvnx8hi */
    case 4436:  /* *pred_cmphivnx16qi */
    case 4435:  /* *pred_cmphsvnx16qi */
    case 4434:  /* *pred_cmplsvnx16qi */
    case 4433:  /* *pred_cmplovnx16qi */
    case 4432:  /* *pred_cmpgtvnx16qi */
    case 4431:  /* *pred_cmpgevnx16qi */
    case 4430:  /* *pred_cmpnevnx16qi */
    case 4429:  /* *pred_cmpeqvnx16qi */
    case 4428:  /* *pred_cmplevnx16qi */
    case 4427:  /* *pred_cmpltvnx16qi */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 1));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 1));
      break;

    case 4426:  /* *pred_cmphivnx2di_combine */
    case 4425:  /* *pred_cmphsvnx2di_combine */
    case 4424:  /* *pred_cmplsvnx2di_combine */
    case 4423:  /* *pred_cmplovnx2di_combine */
    case 4422:  /* *pred_cmpgtvnx2di_combine */
    case 4421:  /* *pred_cmpgevnx2di_combine */
    case 4420:  /* *pred_cmpnevnx2di_combine */
    case 4419:  /* *pred_cmpeqvnx2di_combine */
    case 4418:  /* *pred_cmplevnx2di_combine */
    case 4417:  /* *pred_cmpltvnx2di_combine */
    case 4416:  /* *pred_cmphivnx4si_combine */
    case 4415:  /* *pred_cmphsvnx4si_combine */
    case 4414:  /* *pred_cmplsvnx4si_combine */
    case 4413:  /* *pred_cmplovnx4si_combine */
    case 4412:  /* *pred_cmpgtvnx4si_combine */
    case 4411:  /* *pred_cmpgevnx4si_combine */
    case 4410:  /* *pred_cmpnevnx4si_combine */
    case 4409:  /* *pred_cmpeqvnx4si_combine */
    case 4408:  /* *pred_cmplevnx4si_combine */
    case 4407:  /* *pred_cmpltvnx4si_combine */
    case 4406:  /* *pred_cmphivnx8hi_combine */
    case 4405:  /* *pred_cmphsvnx8hi_combine */
    case 4404:  /* *pred_cmplsvnx8hi_combine */
    case 4403:  /* *pred_cmplovnx8hi_combine */
    case 4402:  /* *pred_cmpgtvnx8hi_combine */
    case 4401:  /* *pred_cmpgevnx8hi_combine */
    case 4400:  /* *pred_cmpnevnx8hi_combine */
    case 4399:  /* *pred_cmpeqvnx8hi_combine */
    case 4398:  /* *pred_cmplevnx8hi_combine */
    case 4397:  /* *pred_cmpltvnx8hi_combine */
    case 4396:  /* *pred_cmphivnx16qi_combine */
    case 4395:  /* *pred_cmphsvnx16qi_combine */
    case 4394:  /* *pred_cmplsvnx16qi_combine */
    case 4393:  /* *pred_cmplovnx16qi_combine */
    case 4392:  /* *pred_cmpgtvnx16qi_combine */
    case 4391:  /* *pred_cmpgevnx16qi_combine */
    case 4390:  /* *pred_cmpnevnx16qi_combine */
    case 4389:  /* *pred_cmpeqvnx16qi_combine */
    case 4388:  /* *pred_cmplevnx16qi_combine */
    case 4387:  /* *pred_cmpltvnx16qi_combine */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 0), 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0, 0));
      ro[2] = *(ro_loc[2] = &XEXP (XVECEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0, 1), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XVECEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0, 1), 1));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 1));
      break;

    case 4386:  /* *cmphivnx2di_cc */
    case 4385:  /* *cmphsvnx2di_cc */
    case 4384:  /* *cmplsvnx2di_cc */
    case 4383:  /* *cmplovnx2di_cc */
    case 4382:  /* *cmpgtvnx2di_cc */
    case 4381:  /* *cmpgevnx2di_cc */
    case 4380:  /* *cmpnevnx2di_cc */
    case 4379:  /* *cmpeqvnx2di_cc */
    case 4378:  /* *cmplevnx2di_cc */
    case 4377:  /* *cmpltvnx2di_cc */
    case 4376:  /* *cmphivnx4si_cc */
    case 4375:  /* *cmphsvnx4si_cc */
    case 4374:  /* *cmplsvnx4si_cc */
    case 4373:  /* *cmplovnx4si_cc */
    case 4372:  /* *cmpgtvnx4si_cc */
    case 4371:  /* *cmpgevnx4si_cc */
    case 4370:  /* *cmpnevnx4si_cc */
    case 4369:  /* *cmpeqvnx4si_cc */
    case 4368:  /* *cmplevnx4si_cc */
    case 4367:  /* *cmpltvnx4si_cc */
    case 4366:  /* *cmphivnx8hi_cc */
    case 4365:  /* *cmphsvnx8hi_cc */
    case 4364:  /* *cmplsvnx8hi_cc */
    case 4363:  /* *cmplovnx8hi_cc */
    case 4362:  /* *cmpgtvnx8hi_cc */
    case 4361:  /* *cmpgevnx8hi_cc */
    case 4360:  /* *cmpnevnx8hi_cc */
    case 4359:  /* *cmpeqvnx8hi_cc */
    case 4358:  /* *cmplevnx8hi_cc */
    case 4357:  /* *cmpltvnx8hi_cc */
    case 4356:  /* *cmphivnx16qi_cc */
    case 4355:  /* *cmphsvnx16qi_cc */
    case 4354:  /* *cmplsvnx16qi_cc */
    case 4353:  /* *cmplovnx16qi_cc */
    case 4352:  /* *cmpgtvnx16qi_cc */
    case 4351:  /* *cmpgevnx16qi_cc */
    case 4350:  /* *cmpnevnx16qi_cc */
    case 4349:  /* *cmpeqvnx16qi_cc */
    case 4348:  /* *cmplevnx16qi_cc */
    case 4347:  /* *cmpltvnx16qi_cc */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0, 0));
      ro[2] = *(ro_loc[2] = &XEXP (XVECEXP (XVECEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0, 1), 0, 1), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XVECEXP (XVECEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0, 1), 0, 1), 1));
      recog_data.dup_loc[0] = &XEXP (XVECEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0, 1), 0);
      recog_data.dup_num[0] = 2;
      recog_data.dup_loc[1] = &XEXP (XVECEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0, 1), 1);
      recog_data.dup_num[1] = 3;
      recog_data.dup_loc[2] = &XVECEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0, 0);
      recog_data.dup_num[2] = 1;
      recog_data.dup_loc[3] = &XVECEXP (XVECEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0, 1), 0, 0);
      recog_data.dup_num[3] = 1;
      break;

    case 4346:  /* *cmphivnx2di_ptest */
    case 4345:  /* *cmphsvnx2di_ptest */
    case 4344:  /* *cmplsvnx2di_ptest */
    case 4343:  /* *cmplovnx2di_ptest */
    case 4342:  /* *cmpgtvnx2di_ptest */
    case 4341:  /* *cmpgevnx2di_ptest */
    case 4340:  /* *cmpnevnx2di_ptest */
    case 4339:  /* *cmpeqvnx2di_ptest */
    case 4338:  /* *cmplevnx2di_ptest */
    case 4337:  /* *cmpltvnx2di_ptest */
    case 4336:  /* *cmphivnx4si_ptest */
    case 4335:  /* *cmphsvnx4si_ptest */
    case 4334:  /* *cmplsvnx4si_ptest */
    case 4333:  /* *cmplovnx4si_ptest */
    case 4332:  /* *cmpgtvnx4si_ptest */
    case 4331:  /* *cmpgevnx4si_ptest */
    case 4330:  /* *cmpnevnx4si_ptest */
    case 4329:  /* *cmpeqvnx4si_ptest */
    case 4328:  /* *cmplevnx4si_ptest */
    case 4327:  /* *cmpltvnx4si_ptest */
    case 4326:  /* *cmphivnx8hi_ptest */
    case 4325:  /* *cmphsvnx8hi_ptest */
    case 4324:  /* *cmplsvnx8hi_ptest */
    case 4323:  /* *cmplovnx8hi_ptest */
    case 4322:  /* *cmpgtvnx8hi_ptest */
    case 4321:  /* *cmpgevnx8hi_ptest */
    case 4320:  /* *cmpnevnx8hi_ptest */
    case 4319:  /* *cmpeqvnx8hi_ptest */
    case 4318:  /* *cmplevnx8hi_ptest */
    case 4317:  /* *cmpltvnx8hi_ptest */
    case 4316:  /* *cmphivnx16qi_ptest */
    case 4315:  /* *cmphsvnx16qi_ptest */
    case 4314:  /* *cmplsvnx16qi_ptest */
    case 4313:  /* *cmplovnx16qi_ptest */
    case 4312:  /* *cmpgtvnx16qi_ptest */
    case 4311:  /* *cmpgevnx16qi_ptest */
    case 4310:  /* *cmpnevnx16qi_ptest */
    case 4309:  /* *cmpeqvnx16qi_ptest */
    case 4308:  /* *cmplevnx16qi_ptest */
    case 4307:  /* *cmpltvnx16qi_ptest */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0, 0));
      ro[2] = *(ro_loc[2] = &XEXP (XVECEXP (XVECEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0, 1), 0, 1), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XVECEXP (XVECEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0, 1), 0, 1), 1));
      recog_data.dup_loc[0] = &XVECEXP (XVECEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0, 1), 0, 0);
      recog_data.dup_num[0] = 1;
      break;

    case 4306:  /* *cmphivnx2di */
    case 4305:  /* *cmphsvnx2di */
    case 4304:  /* *cmplsvnx2di */
    case 4303:  /* *cmplovnx2di */
    case 4302:  /* *cmpgtvnx2di */
    case 4301:  /* *cmpgevnx2di */
    case 4300:  /* *cmpnevnx2di */
    case 4299:  /* *cmpeqvnx2di */
    case 4298:  /* *cmplevnx2di */
    case 4297:  /* *cmpltvnx2di */
    case 4296:  /* *cmphivnx4si */
    case 4295:  /* *cmphsvnx4si */
    case 4294:  /* *cmplsvnx4si */
    case 4293:  /* *cmplovnx4si */
    case 4292:  /* *cmpgtvnx4si */
    case 4291:  /* *cmpgevnx4si */
    case 4290:  /* *cmpnevnx4si */
    case 4289:  /* *cmpeqvnx4si */
    case 4288:  /* *cmplevnx4si */
    case 4287:  /* *cmpltvnx4si */
    case 4286:  /* *cmphivnx8hi */
    case 4285:  /* *cmphsvnx8hi */
    case 4284:  /* *cmplsvnx8hi */
    case 4283:  /* *cmplovnx8hi */
    case 4282:  /* *cmpgtvnx8hi */
    case 4281:  /* *cmpgevnx8hi */
    case 4280:  /* *cmpnevnx8hi */
    case 4279:  /* *cmpeqvnx8hi */
    case 4278:  /* *cmplevnx8hi */
    case 4277:  /* *cmpltvnx8hi */
    case 4276:  /* *cmphivnx16qi */
    case 4275:  /* *cmphsvnx16qi */
    case 4274:  /* *cmplsvnx16qi */
    case 4273:  /* *cmplovnx16qi */
    case 4272:  /* *cmpgtvnx16qi */
    case 4271:  /* *cmpgevnx16qi */
    case 4270:  /* *cmpnevnx16qi */
    case 4269:  /* *cmpeqvnx16qi */
    case 4268:  /* *cmplevnx16qi */
    case 4267:  /* *cmpltvnx16qi */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 0), 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0, 0));
      ro[2] = *(ro_loc[2] = &XEXP (XVECEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0, 1), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XVECEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0, 1), 1));
      break;

    case 4266:  /* while_ultdivnx2bi_cc */
    case 4265:  /* while_ultsivnx2bi_cc */
    case 4264:  /* while_ultdivnx4bi_cc */
    case 4263:  /* while_ultsivnx4bi_cc */
    case 4262:  /* while_ultdivnx8bi_cc */
    case 4261:  /* while_ultsivnx8bi_cc */
    case 4260:  /* while_ultdivnx16bi_cc */
    case 4259:  /* while_ultsivnx16bi_cc */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0, 0));
      ro[2] = *(ro_loc[2] = &XVECEXP (XVECEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0, 1), 0, 0));
      ro[3] = *(ro_loc[3] = &XVECEXP (XVECEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0, 1), 0, 1));
      recog_data.dup_loc[0] = &XVECEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0, 1);
      recog_data.dup_num[0] = 3;
      recog_data.dup_loc[1] = &XVECEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0, 0);
      recog_data.dup_num[1] = 2;
      break;

    case 4258:  /* while_ultdivnx2bi */
    case 4257:  /* while_ultsivnx2bi */
    case 4256:  /* while_ultdivnx4bi */
    case 4255:  /* while_ultsivnx4bi */
    case 4254:  /* while_ultdivnx8bi */
    case 4253:  /* while_ultsivnx8bi */
    case 4252:  /* while_ultdivnx16bi */
    case 4251:  /* while_ultsivnx16bi */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 0), 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0, 0));
      ro[2] = *(ro_loc[2] = &XVECEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0, 1));
      break;

    case 4250:  /* ptest_ptruevnx2bi */
    case 4249:  /* ptest_ptruevnx4bi */
    case 4248:  /* ptest_ptruevnx8bi */
    case 4247:  /* ptest_ptruevnx16bi */
      ro[0] = *(ro_loc[0] = &XVECEXP (XEXP (XEXP (pat, 1), 0), 0, 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (XEXP (pat, 1), 0), 0, 1));
      break;

    case 4222:  /* *nandvnx2bi3 */
    case 4221:  /* *norvnx2bi3 */
    case 4220:  /* *nandvnx4bi3 */
    case 4219:  /* *norvnx4bi3 */
    case 4218:  /* *nandvnx8bi3 */
    case 4217:  /* *norvnx8bi3 */
    case 4216:  /* *nandvnx16bi3 */
    case 4215:  /* *norvnx16bi3 */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 1));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 1), 0));
      break;

    case 4214:  /* *ornvnx2bi3 */
    case 4213:  /* *bicvnx2bi3 */
    case 4212:  /* *ornvnx4bi3 */
    case 4211:  /* *bicvnx4bi3 */
    case 4210:  /* *ornvnx8bi3 */
    case 4209:  /* *bicvnx8bi3 */
    case 4208:  /* *ornvnx16bi3 */
    case 4207:  /* *bicvnx16bi3 */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 1));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (pat, 1), 0), 1));
      break;

    case 4202:  /* *xorvnx2bi3_cc */
    case 4201:  /* *iorvnx2bi3_cc */
    case 4200:  /* *andvnx2bi3_cc */
    case 4199:  /* *xorvnx4bi3_cc */
    case 4198:  /* *iorvnx4bi3_cc */
    case 4197:  /* *andvnx4bi3_cc */
    case 4196:  /* *xorvnx8bi3_cc */
    case 4195:  /* *iorvnx8bi3_cc */
    case 4194:  /* *andvnx8bi3_cc */
    case 4193:  /* *xorvnx16bi3_cc */
    case 4192:  /* *iorvnx16bi3_cc */
    case 4191:  /* *andvnx16bi3_cc */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0, 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XVECEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0, 1), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XVECEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0, 1), 0), 1));
      recog_data.dup_loc[0] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0), 0);
      recog_data.dup_num[0] = 2;
      recog_data.dup_loc[1] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0), 1);
      recog_data.dup_num[1] = 3;
      recog_data.dup_loc[2] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 1);
      recog_data.dup_num[2] = 1;
      recog_data.dup_loc[3] = &XEXP (XVECEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0, 1), 1);
      recog_data.dup_num[3] = 1;
      break;

    case 4592:  /* *fminvnx2df3 */
    case 4591:  /* *fmaxvnx2df3 */
    case 4590:  /* *smin_nanvnx2df3 */
    case 4589:  /* *smax_nanvnx2df3 */
    case 4588:  /* *fminvnx4sf3 */
    case 4587:  /* *fmaxvnx4sf3 */
    case 4586:  /* *smin_nanvnx4sf3 */
    case 4585:  /* *smax_nanvnx4sf3 */
    case 4584:  /* *fminvnx8hf3 */
    case 4583:  /* *fmaxvnx8hf3 */
    case 4582:  /* *smin_nanvnx8hf3 */
    case 4581:  /* *smax_nanvnx8hf3 */
    case 4129:  /* *umulvnx2di3_highpart */
    case 4128:  /* *smulvnx2di3_highpart */
    case 4127:  /* *umulvnx4si3_highpart */
    case 4126:  /* *smulvnx4si3_highpart */
    case 4125:  /* *umulvnx8hi3_highpart */
    case 4124:  /* *smulvnx8hi3_highpart */
    case 4123:  /* *umulvnx16qi3_highpart */
    case 4122:  /* *smulvnx16qi3_highpart */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (pat, 1), 0, 0));
      ro[2] = *(ro_loc[2] = &XVECEXP (XVECEXP (XEXP (pat, 1), 0, 1), 0, 0));
      ro[3] = *(ro_loc[3] = &XVECEXP (XVECEXP (XEXP (pat, 1), 0, 1), 0, 1));
      break;

    case 4121:  /* *msubvnx2di3 */
    case 4120:  /* *msubvnx4si3 */
    case 4119:  /* *msubvnx8hi3 */
    case 4118:  /* *msubvnx16qi3 */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (XEXP (pat, 1), 1), 0, 0));
      ro[2] = *(ro_loc[2] = &XEXP (XVECEXP (XEXP (XEXP (pat, 1), 1), 0, 1), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XVECEXP (XEXP (XEXP (pat, 1), 1), 0, 1), 1));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (pat, 1), 0));
      break;

    case 4508:  /* *fcmuovnx2df_and_combine */
    case 4507:  /* *fcmuovnx4sf_and_combine */
    case 4506:  /* *fcmuovnx8hf_and_combine */
    case 4505:  /* *fcmgtvnx2df_and_combine */
    case 4504:  /* *fcmgevnx2df_and_combine */
    case 4503:  /* *fcmnevnx2df_and_combine */
    case 4502:  /* *fcmeqvnx2df_and_combine */
    case 4501:  /* *fcmlevnx2df_and_combine */
    case 4500:  /* *fcmltvnx2df_and_combine */
    case 4499:  /* *fcmgtvnx4sf_and_combine */
    case 4498:  /* *fcmgevnx4sf_and_combine */
    case 4497:  /* *fcmnevnx4sf_and_combine */
    case 4496:  /* *fcmeqvnx4sf_and_combine */
    case 4495:  /* *fcmlevnx4sf_and_combine */
    case 4494:  /* *fcmltvnx4sf_and_combine */
    case 4493:  /* *fcmgtvnx8hf_and_combine */
    case 4492:  /* *fcmgevnx8hf_and_combine */
    case 4491:  /* *fcmnevnx8hf_and_combine */
    case 4490:  /* *fcmeqvnx8hf_and_combine */
    case 4489:  /* *fcmlevnx8hf_and_combine */
    case 4488:  /* *fcmltvnx8hf_and_combine */
    case 4117:  /* *maddvnx2di */
    case 4116:  /* *maddvnx4si */
    case 4115:  /* *maddvnx8hi */
    case 4114:  /* *maddvnx16qi */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (XEXP (pat, 1), 0), 0, 0));
      ro[2] = *(ro_loc[2] = &XEXP (XVECEXP (XEXP (XEXP (pat, 1), 0), 0, 1), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XVECEXP (XEXP (XEXP (pat, 1), 0), 0, 1), 1));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (pat, 1), 1));
      break;

    case 4917:  /* *divvnx2df3 */
    case 4916:  /* *divvnx4sf3 */
    case 4915:  /* *divvnx8hf3 */
    case 4893:  /* *mulvnx2df3 */
    case 4892:  /* *mulvnx4sf3 */
    case 4891:  /* *mulvnx8hf3 */
    case 4890:  /* *subvnx2df3 */
    case 4889:  /* *subvnx4sf3 */
    case 4888:  /* *subvnx8hf3 */
    case 4887:  /* *addvnx2df3 */
    case 4886:  /* *addvnx4sf3 */
    case 4885:  /* *addvnx8hf3 */
    case 4580:  /* *sminvnx2df3 */
    case 4579:  /* *smaxvnx2df3 */
    case 4578:  /* *sminvnx4sf3 */
    case 4577:  /* *smaxvnx4sf3 */
    case 4576:  /* *sminvnx8hf3 */
    case 4575:  /* *smaxvnx8hf3 */
    case 4574:  /* *uminvnx2di3 */
    case 4573:  /* *umaxvnx2di3 */
    case 4572:  /* *sminvnx2di3 */
    case 4571:  /* *smaxvnx2di3 */
    case 4570:  /* *uminvnx4si3 */
    case 4569:  /* *umaxvnx4si3 */
    case 4568:  /* *sminvnx4si3 */
    case 4567:  /* *smaxvnx4si3 */
    case 4566:  /* *uminvnx8hi3 */
    case 4565:  /* *umaxvnx8hi3 */
    case 4564:  /* *sminvnx8hi3 */
    case 4563:  /* *smaxvnx8hi3 */
    case 4562:  /* *uminvnx16qi3 */
    case 4561:  /* *umaxvnx16qi3 */
    case 4560:  /* *sminvnx16qi3 */
    case 4559:  /* *smaxvnx16qi3 */
    case 4487:  /* *fcmuovnx2df */
    case 4486:  /* *fcmuovnx4sf */
    case 4485:  /* *fcmuovnx8hf */
    case 4484:  /* *fcmgtvnx2df */
    case 4483:  /* *fcmgevnx2df */
    case 4482:  /* *fcmnevnx2df */
    case 4481:  /* *fcmeqvnx2df */
    case 4480:  /* *fcmlevnx2df */
    case 4479:  /* *fcmltvnx2df */
    case 4478:  /* *fcmgtvnx4sf */
    case 4477:  /* *fcmgevnx4sf */
    case 4476:  /* *fcmnevnx4sf */
    case 4475:  /* *fcmeqvnx4sf */
    case 4474:  /* *fcmlevnx4sf */
    case 4473:  /* *fcmltvnx4sf */
    case 4472:  /* *fcmgtvnx8hf */
    case 4471:  /* *fcmgevnx8hf */
    case 4470:  /* *fcmnevnx8hf */
    case 4469:  /* *fcmeqvnx8hf */
    case 4468:  /* *fcmlevnx8hf */
    case 4467:  /* *fcmltvnx8hf */
    case 4234:  /* *vlshrvnx2di3 */
    case 4233:  /* *vashrvnx2di3 */
    case 4232:  /* *vashlvnx2di3 */
    case 4231:  /* *vlshrvnx4si3 */
    case 4230:  /* *vashrvnx4si3 */
    case 4229:  /* *vashlvnx4si3 */
    case 4228:  /* *vlshrvnx8hi3 */
    case 4227:  /* *vashrvnx8hi3 */
    case 4226:  /* *vashlvnx8hi3 */
    case 4225:  /* *vlshrvnx16qi3 */
    case 4224:  /* *vashrvnx16qi3 */
    case 4223:  /* *vashlvnx16qi3 */
    case 4133:  /* *udivvnx2di3 */
    case 4132:  /* *divvnx2di3 */
    case 4131:  /* *udivvnx4si3 */
    case 4130:  /* *divvnx4si3 */
    case 4109:  /* *mulvnx2di3 */
    case 4108:  /* *mulvnx4si3 */
    case 4107:  /* *mulvnx8hi3 */
    case 4106:  /* *mulvnx16qi3 */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (pat, 1), 0, 0));
      ro[2] = *(ro_loc[2] = &XEXP (XVECEXP (XEXP (pat, 1), 0, 1), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XVECEXP (XEXP (pat, 1), 0, 1), 1));
      break;

    case 4979:  /* aarch64_sve_extendvnx4sfvnx2df2 */
    case 4978:  /* aarch64_sve_extendvnx8hfvnx4sf2 */
    case 4977:  /* *truncvnx2dfvnx4sf2 */
    case 4976:  /* *truncvnx4sfvnx8hf2 */
    case 4947:  /* *roundvnx2df2 */
    case 4946:  /* *rintvnx2df2 */
    case 4945:  /* *nearbyintvnx2df2 */
    case 4944:  /* *frintnvnx2df2 */
    case 4943:  /* *floorvnx2df2 */
    case 4942:  /* *ceilvnx2df2 */
    case 4941:  /* *btruncvnx2df2 */
    case 4940:  /* *roundvnx4sf2 */
    case 4939:  /* *rintvnx4sf2 */
    case 4938:  /* *nearbyintvnx4sf2 */
    case 4937:  /* *frintnvnx4sf2 */
    case 4936:  /* *floorvnx4sf2 */
    case 4935:  /* *ceilvnx4sf2 */
    case 4934:  /* *btruncvnx4sf2 */
    case 4933:  /* *roundvnx8hf2 */
    case 4932:  /* *rintvnx8hf2 */
    case 4931:  /* *nearbyintvnx8hf2 */
    case 4930:  /* *frintnvnx8hf2 */
    case 4929:  /* *floorvnx8hf2 */
    case 4928:  /* *ceilvnx8hf2 */
    case 4927:  /* *btruncvnx8hf2 */
    case 4076:  /* *aarch64_sve_rev16vnx16qi */
    case 4075:  /* *aarch64_sve_rev32vnx8hf */
    case 4074:  /* *aarch64_sve_rev32vnx8hi */
    case 4073:  /* *aarch64_sve_rev32vnx16qi */
    case 4072:  /* *aarch64_sve_rev64vnx4sf */
    case 4071:  /* *aarch64_sve_rev64vnx8hf */
    case 4070:  /* *aarch64_sve_rev64vnx4si */
    case 4069:  /* *aarch64_sve_rev64vnx8hi */
    case 4068:  /* *aarch64_sve_rev64vnx16qi */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (pat, 1), 0, 0));
      ro[2] = *(ro_loc[2] = &XVECEXP (XVECEXP (XEXP (pat, 1), 0, 1), 0, 0));
      break;

    case 3937:  /* sve_ld1rvnx2df */
    case 3936:  /* sve_ld1rvnx4sf */
    case 3935:  /* sve_ld1rvnx8hf */
    case 3934:  /* sve_ld1rvnx2di */
    case 3933:  /* sve_ld1rvnx4si */
    case 3932:  /* sve_ld1rvnx8hi */
    case 3931:  /* sve_ld1rvnx16qi */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (pat, 1), 0, 0));
      ro[2] = *(ro_loc[2] = &XEXP (XVECEXP (XEXP (pat, 1), 0, 1), 0));
      ro[3] = *(ro_loc[3] = &XVECEXP (XEXP (pat, 1), 0, 2));
      break;

    case 3930:  /* *vec_duplicatevnx2df_reg */
    case 3929:  /* *vec_duplicatevnx4sf_reg */
    case 3928:  /* *vec_duplicatevnx8hf_reg */
    case 3927:  /* *vec_duplicatevnx2di_reg */
    case 3926:  /* *vec_duplicatevnx4si_reg */
    case 3925:  /* *vec_duplicatevnx8hi_reg */
    case 3924:  /* *vec_duplicatevnx16qi_reg */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XVECEXP (pat, 0, 1), 0));
      break;

    case 3821:  /* mask_scatter_storevnx2df */
    case 3820:  /* mask_scatter_storevnx2di */
    case 3819:  /* mask_scatter_storevnx4sf */
    case 3818:  /* mask_scatter_storevnx4si */
      ro[0] = *(ro_loc[0] = &XVECEXP (XEXP (pat, 1), 0, 1));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (pat, 1), 0, 2));
      ro[2] = *(ro_loc[2] = &XVECEXP (XEXP (pat, 1), 0, 3));
      ro[3] = *(ro_loc[3] = &XVECEXP (XEXP (pat, 1), 0, 4));
      ro[4] = *(ro_loc[4] = &XVECEXP (XEXP (pat, 1), 0, 5));
      ro[5] = *(ro_loc[5] = &XVECEXP (XEXP (pat, 1), 0, 0));
      break;

    case 3817:  /* mask_gather_loadvnx2df */
    case 3816:  /* mask_gather_loadvnx2di */
    case 3815:  /* mask_gather_loadvnx4sf */
    case 3814:  /* mask_gather_loadvnx4si */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (pat, 1), 0, 1));
      ro[2] = *(ro_loc[2] = &XVECEXP (XEXP (pat, 1), 0, 2));
      ro[3] = *(ro_loc[3] = &XVECEXP (XEXP (pat, 1), 0, 3));
      ro[4] = *(ro_loc[4] = &XVECEXP (XEXP (pat, 1), 0, 4));
      ro[5] = *(ro_loc[5] = &XVECEXP (XEXP (pat, 1), 0, 0));
      break;

    case 3994:  /* vec_mask_store_lanesvnx8dfvnx2df */
    case 3993:  /* vec_mask_store_lanesvnx16sfvnx4sf */
    case 3992:  /* vec_mask_store_lanesvnx32hfvnx8hf */
    case 3991:  /* vec_mask_store_lanesvnx8divnx2di */
    case 3990:  /* vec_mask_store_lanesvnx16sivnx4si */
    case 3989:  /* vec_mask_store_lanesvnx32hivnx8hi */
    case 3988:  /* vec_mask_store_lanesvnx64qivnx16qi */
    case 3987:  /* vec_mask_store_lanesvnx6dfvnx2df */
    case 3986:  /* vec_mask_store_lanesvnx12sfvnx4sf */
    case 3985:  /* vec_mask_store_lanesvnx24hfvnx8hf */
    case 3984:  /* vec_mask_store_lanesvnx6divnx2di */
    case 3983:  /* vec_mask_store_lanesvnx12sivnx4si */
    case 3982:  /* vec_mask_store_lanesvnx24hivnx8hi */
    case 3981:  /* vec_mask_store_lanesvnx48qivnx16qi */
    case 3980:  /* vec_mask_store_lanesvnx4dfvnx2df */
    case 3979:  /* vec_mask_store_lanesvnx8sfvnx4sf */
    case 3978:  /* vec_mask_store_lanesvnx16hfvnx8hf */
    case 3977:  /* vec_mask_store_lanesvnx4divnx2di */
    case 3976:  /* vec_mask_store_lanesvnx8sivnx4si */
    case 3975:  /* vec_mask_store_lanesvnx16hivnx8hi */
    case 3974:  /* vec_mask_store_lanesvnx32qivnx16qi */
    case 3813:  /* maskstorevnx2dfvnx2bi */
    case 3812:  /* maskstorevnx4sfvnx4bi */
    case 3811:  /* maskstorevnx8hfvnx8bi */
    case 3810:  /* maskstorevnx2divnx2bi */
    case 3809:  /* maskstorevnx4sivnx4bi */
    case 3808:  /* maskstorevnx8hivnx8bi */
    case 3807:  /* maskstorevnx16qivnx16bi */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (pat, 1), 0, 1));
      ro[2] = *(ro_loc[2] = &XVECEXP (XEXP (pat, 1), 0, 0));
      recog_data.dup_loc[0] = &XVECEXP (XEXP (pat, 1), 0, 2);
      recog_data.dup_num[0] = 0;
      break;

    case 3973:  /* vec_mask_load_lanesvnx8dfvnx2df */
    case 3972:  /* vec_mask_load_lanesvnx16sfvnx4sf */
    case 3971:  /* vec_mask_load_lanesvnx32hfvnx8hf */
    case 3970:  /* vec_mask_load_lanesvnx8divnx2di */
    case 3969:  /* vec_mask_load_lanesvnx16sivnx4si */
    case 3968:  /* vec_mask_load_lanesvnx32hivnx8hi */
    case 3967:  /* vec_mask_load_lanesvnx64qivnx16qi */
    case 3966:  /* vec_mask_load_lanesvnx6dfvnx2df */
    case 3965:  /* vec_mask_load_lanesvnx12sfvnx4sf */
    case 3964:  /* vec_mask_load_lanesvnx24hfvnx8hf */
    case 3963:  /* vec_mask_load_lanesvnx6divnx2di */
    case 3962:  /* vec_mask_load_lanesvnx12sivnx4si */
    case 3961:  /* vec_mask_load_lanesvnx24hivnx8hi */
    case 3960:  /* vec_mask_load_lanesvnx48qivnx16qi */
    case 3959:  /* vec_mask_load_lanesvnx4dfvnx2df */
    case 3958:  /* vec_mask_load_lanesvnx8sfvnx4sf */
    case 3957:  /* vec_mask_load_lanesvnx16hfvnx8hf */
    case 3956:  /* vec_mask_load_lanesvnx4divnx2di */
    case 3955:  /* vec_mask_load_lanesvnx8sivnx4si */
    case 3954:  /* vec_mask_load_lanesvnx16hivnx8hi */
    case 3953:  /* vec_mask_load_lanesvnx32qivnx16qi */
    case 3806:  /* maskloadvnx2dfvnx2bi */
    case 3805:  /* maskloadvnx4sfvnx4bi */
    case 3804:  /* maskloadvnx8hfvnx8bi */
    case 3803:  /* maskloadvnx2divnx2bi */
    case 3802:  /* maskloadvnx4sivnx4bi */
    case 3801:  /* maskloadvnx8hivnx8bi */
    case 3800:  /* maskloadvnx16qivnx16bi */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (pat, 1), 0, 1));
      ro[2] = *(ro_loc[2] = &XVECEXP (XEXP (pat, 1), 0, 0));
      break;

    case 3771:  /* *dmb */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (pat, 1), 0, 1));
      recog_data.dup_loc[0] = &XVECEXP (XEXP (pat, 1), 0, 0);
      recog_data.dup_num[0] = 0;
      break;

    case 3770:  /* aarch64_store_exclusivedi */
    case 3769:  /* aarch64_store_exclusivesi */
    case 3768:  /* aarch64_store_exclusivehi */
    case 3767:  /* aarch64_store_exclusiveqi */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[2] = *(ro_loc[2] = &XVECEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0, 0));
      ro[3] = *(ro_loc[3] = &XVECEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0, 1));
      break;

    case 3754:  /* atomic_nand_fetchdi */
    case 3753:  /* atomic_nand_fetchsi */
    case 3752:  /* atomic_nand_fetchhi */
    case 3751:  /* atomic_nand_fetchqi */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 1));
      ro[3] = *(ro_loc[3] = &XVECEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0, 2));
      ro[4] = *(ro_loc[4] = &XEXP (XVECEXP (pat, 0, 3), 0));
      recog_data.dup_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0);
      recog_data.dup_num[0] = 1;
      recog_data.dup_loc[1] = &XVECEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0, 1);
      recog_data.dup_num[1] = 2;
      recog_data.dup_loc[2] = &XVECEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0, 0);
      recog_data.dup_num[2] = 1;
      break;

    case 3750:  /* aarch64_atomic_and_fetchdi */
    case 3749:  /* aarch64_atomic_xor_fetchdi */
    case 3748:  /* aarch64_atomic_or_fetchdi */
    case 3747:  /* aarch64_atomic_sub_fetchdi */
    case 3746:  /* aarch64_atomic_add_fetchdi */
    case 3745:  /* aarch64_atomic_and_fetchsi */
    case 3744:  /* aarch64_atomic_xor_fetchsi */
    case 3743:  /* aarch64_atomic_or_fetchsi */
    case 3742:  /* aarch64_atomic_sub_fetchsi */
    case 3741:  /* aarch64_atomic_add_fetchsi */
    case 3740:  /* aarch64_atomic_and_fetchhi */
    case 3739:  /* aarch64_atomic_xor_fetchhi */
    case 3738:  /* aarch64_atomic_or_fetchhi */
    case 3737:  /* aarch64_atomic_sub_fetchhi */
    case 3736:  /* aarch64_atomic_add_fetchhi */
    case 3735:  /* aarch64_atomic_and_fetchqi */
    case 3734:  /* aarch64_atomic_xor_fetchqi */
    case 3733:  /* aarch64_atomic_or_fetchqi */
    case 3732:  /* aarch64_atomic_sub_fetchqi */
    case 3731:  /* aarch64_atomic_add_fetchqi */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 1));
      ro[3] = *(ro_loc[3] = &XVECEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0, 2));
      ro[4] = *(ro_loc[4] = &XEXP (XVECEXP (pat, 0, 3), 0));
      recog_data.dup_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0);
      recog_data.dup_num[0] = 1;
      recog_data.dup_loc[1] = &XVECEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0, 1);
      recog_data.dup_num[1] = 2;
      recog_data.dup_loc[2] = &XVECEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0, 0);
      recog_data.dup_num[2] = 1;
      break;

    case 3730:  /* atomic_fetch_nanddi */
    case 3729:  /* atomic_fetch_nandsi */
    case 3728:  /* atomic_fetch_nandhi */
    case 3727:  /* atomic_fetch_nandqi */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XVECEXP (pat, 0, 0), 1));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XVECEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0, 0), 0), 1));
      ro[3] = *(ro_loc[3] = &XVECEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0, 1));
      ro[4] = *(ro_loc[4] = &XEXP (XVECEXP (pat, 0, 3), 0));
      ro[5] = *(ro_loc[5] = &XEXP (XVECEXP (pat, 0, 4), 0));
      recog_data.dup_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0);
      recog_data.dup_num[0] = 1;
      recog_data.dup_loc[1] = &XEXP (XEXP (XVECEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0, 0), 0), 0);
      recog_data.dup_num[1] = 1;
      break;

    case 3726:  /* aarch64_atomic_fetch_adddi_lse */
    case 3725:  /* aarch64_atomic_fetch_xordi_lse */
    case 3724:  /* aarch64_atomic_fetch_bicdi_lse */
    case 3723:  /* aarch64_atomic_fetch_iordi_lse */
    case 3722:  /* aarch64_atomic_fetch_addsi_lse */
    case 3721:  /* aarch64_atomic_fetch_xorsi_lse */
    case 3720:  /* aarch64_atomic_fetch_bicsi_lse */
    case 3719:  /* aarch64_atomic_fetch_iorsi_lse */
    case 3718:  /* aarch64_atomic_fetch_addhi_lse */
    case 3717:  /* aarch64_atomic_fetch_xorhi_lse */
    case 3716:  /* aarch64_atomic_fetch_bichi_lse */
    case 3715:  /* aarch64_atomic_fetch_iorhi_lse */
    case 3714:  /* aarch64_atomic_fetch_addqi_lse */
    case 3713:  /* aarch64_atomic_fetch_xorqi_lse */
    case 3712:  /* aarch64_atomic_fetch_bicqi_lse */
    case 3711:  /* aarch64_atomic_fetch_iorqi_lse */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XVECEXP (pat, 0, 0), 1));
      ro[2] = *(ro_loc[2] = &XVECEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0, 1));
      ro[3] = *(ro_loc[3] = &XVECEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0, 2));
      recog_data.dup_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0);
      recog_data.dup_num[0] = 1;
      recog_data.dup_loc[1] = &XVECEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0, 0);
      recog_data.dup_num[1] = 1;
      break;

    case 3710:  /* aarch64_atomic_fetch_anddi */
    case 3709:  /* aarch64_atomic_fetch_xordi */
    case 3708:  /* aarch64_atomic_fetch_ordi */
    case 3707:  /* aarch64_atomic_fetch_subdi */
    case 3706:  /* aarch64_atomic_fetch_adddi */
    case 3705:  /* aarch64_atomic_fetch_andsi */
    case 3704:  /* aarch64_atomic_fetch_xorsi */
    case 3703:  /* aarch64_atomic_fetch_orsi */
    case 3702:  /* aarch64_atomic_fetch_subsi */
    case 3701:  /* aarch64_atomic_fetch_addsi */
    case 3700:  /* aarch64_atomic_fetch_andhi */
    case 3699:  /* aarch64_atomic_fetch_xorhi */
    case 3698:  /* aarch64_atomic_fetch_orhi */
    case 3697:  /* aarch64_atomic_fetch_subhi */
    case 3696:  /* aarch64_atomic_fetch_addhi */
    case 3695:  /* aarch64_atomic_fetch_andqi */
    case 3694:  /* aarch64_atomic_fetch_xorqi */
    case 3693:  /* aarch64_atomic_fetch_orqi */
    case 3692:  /* aarch64_atomic_fetch_subqi */
    case 3691:  /* aarch64_atomic_fetch_addqi */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XVECEXP (pat, 0, 0), 1));
      ro[2] = *(ro_loc[2] = &XEXP (XVECEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0, 0), 1));
      ro[3] = *(ro_loc[3] = &XVECEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0, 1));
      ro[4] = *(ro_loc[4] = &XEXP (XVECEXP (pat, 0, 3), 0));
      ro[5] = *(ro_loc[5] = &XEXP (XVECEXP (pat, 0, 4), 0));
      recog_data.dup_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0);
      recog_data.dup_num[0] = 1;
      recog_data.dup_loc[1] = &XEXP (XVECEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0, 0), 0);
      recog_data.dup_num[1] = 1;
      break;

    case 3690:  /* atomic_nanddi */
    case 3689:  /* atomic_nandsi */
    case 3688:  /* atomic_nandhi */
    case 3687:  /* atomic_nandqi */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XVECEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0, 0), 0), 1));
      ro[2] = *(ro_loc[2] = &XVECEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0, 1));
      ro[3] = *(ro_loc[3] = &XEXP (XVECEXP (pat, 0, 2), 0));
      ro[4] = *(ro_loc[4] = &XEXP (XVECEXP (pat, 0, 3), 0));
      recog_data.dup_loc[0] = &XEXP (XEXP (XVECEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0, 0), 0), 0);
      recog_data.dup_num[0] = 0;
      break;

    case 3686:  /* aarch64_atomic_adddi_lse */
    case 3685:  /* aarch64_atomic_xordi_lse */
    case 3684:  /* aarch64_atomic_bicdi_lse */
    case 3683:  /* aarch64_atomic_iordi_lse */
    case 3682:  /* aarch64_atomic_addsi_lse */
    case 3681:  /* aarch64_atomic_xorsi_lse */
    case 3680:  /* aarch64_atomic_bicsi_lse */
    case 3679:  /* aarch64_atomic_iorsi_lse */
    case 3678:  /* aarch64_atomic_addhi_lse */
    case 3677:  /* aarch64_atomic_xorhi_lse */
    case 3676:  /* aarch64_atomic_bichi_lse */
    case 3675:  /* aarch64_atomic_iorhi_lse */
    case 3674:  /* aarch64_atomic_addqi_lse */
    case 3673:  /* aarch64_atomic_xorqi_lse */
    case 3672:  /* aarch64_atomic_bicqi_lse */
    case 3671:  /* aarch64_atomic_iorqi_lse */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 0), 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0, 1));
      ro[2] = *(ro_loc[2] = &XVECEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0, 2));
      ro[3] = *(ro_loc[3] = &XEXP (XVECEXP (pat, 0, 1), 0));
      recog_data.dup_loc[0] = &XVECEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0, 0);
      recog_data.dup_num[0] = 0;
      break;

    case 3670:  /* aarch64_atomic_anddi */
    case 3669:  /* aarch64_atomic_xordi */
    case 3668:  /* aarch64_atomic_ordi */
    case 3667:  /* aarch64_atomic_subdi */
    case 3666:  /* aarch64_atomic_adddi */
    case 3665:  /* aarch64_atomic_andsi */
    case 3664:  /* aarch64_atomic_xorsi */
    case 3663:  /* aarch64_atomic_orsi */
    case 3662:  /* aarch64_atomic_subsi */
    case 3661:  /* aarch64_atomic_addsi */
    case 3660:  /* aarch64_atomic_andhi */
    case 3659:  /* aarch64_atomic_xorhi */
    case 3658:  /* aarch64_atomic_orhi */
    case 3657:  /* aarch64_atomic_subhi */
    case 3656:  /* aarch64_atomic_addhi */
    case 3655:  /* aarch64_atomic_andqi */
    case 3654:  /* aarch64_atomic_xorqi */
    case 3653:  /* aarch64_atomic_orqi */
    case 3652:  /* aarch64_atomic_subqi */
    case 3651:  /* aarch64_atomic_addqi */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XVECEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0, 0), 1));
      ro[2] = *(ro_loc[2] = &XVECEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0, 1));
      ro[3] = *(ro_loc[3] = &XEXP (XVECEXP (pat, 0, 2), 0));
      ro[4] = *(ro_loc[4] = &XEXP (XVECEXP (pat, 0, 3), 0));
      recog_data.dup_loc[0] = &XEXP (XVECEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0, 0), 0);
      recog_data.dup_num[0] = 0;
      break;

    case 3650:  /* aarch64_atomic_exchangedi_lse */
    case 3649:  /* aarch64_atomic_exchangesi_lse */
    case 3648:  /* aarch64_atomic_exchangehi_lse */
    case 3647:  /* aarch64_atomic_exchangeqi_lse */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XVECEXP (pat, 0, 0), 1));
      ro[2] = *(ro_loc[2] = &XVECEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0, 0));
      ro[3] = *(ro_loc[3] = &XVECEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0, 1));
      recog_data.dup_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0);
      recog_data.dup_num[0] = 1;
      break;

    case 3646:  /* aarch64_atomic_exchangedi */
    case 3645:  /* aarch64_atomic_exchangesi */
    case 3644:  /* aarch64_atomic_exchangehi */
    case 3643:  /* aarch64_atomic_exchangeqi */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XVECEXP (pat, 0, 0), 1));
      ro[2] = *(ro_loc[2] = &XVECEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0, 0));
      ro[3] = *(ro_loc[3] = &XVECEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0, 1));
      ro[4] = *(ro_loc[4] = &XEXP (XVECEXP (pat, 0, 3), 0));
      recog_data.dup_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0);
      recog_data.dup_num[0] = 1;
      break;

    case 3642:  /* aarch64_compare_and_swapdi_lse */
    case 3641:  /* aarch64_compare_and_swapsi_lse */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XVECEXP (pat, 0, 0), 1));
      ro[2] = *(ro_loc[2] = &XVECEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0, 1));
      ro[3] = *(ro_loc[3] = &XVECEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0, 2));
      recog_data.dup_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0);
      recog_data.dup_num[0] = 1;
      recog_data.dup_loc[1] = &XVECEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0, 0);
      recog_data.dup_num[1] = 0;
      break;

    case 3640:  /* aarch64_compare_and_swaphi_lse */
    case 3639:  /* aarch64_compare_and_swapqi_lse */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0));
      ro[2] = *(ro_loc[2] = &XVECEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0, 1));
      ro[3] = *(ro_loc[3] = &XVECEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0, 2));
      recog_data.dup_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0);
      recog_data.dup_num[0] = 1;
      recog_data.dup_loc[1] = &XVECEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0, 0);
      recog_data.dup_num[1] = 0;
      break;

    case 3638:  /* aarch64_compare_and_swapdi */
    case 3637:  /* aarch64_compare_and_swapsi */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XVECEXP (pat, 0, 1), 1));
      ro[2] = *(ro_loc[2] = &XVECEXP (XEXP (XVECEXP (pat, 0, 2), 1), 0, 0));
      ro[3] = *(ro_loc[3] = &XVECEXP (XEXP (XVECEXP (pat, 0, 2), 1), 0, 1));
      ro[4] = *(ro_loc[4] = &XVECEXP (XEXP (XVECEXP (pat, 0, 2), 1), 0, 2));
      ro[5] = *(ro_loc[5] = &XVECEXP (XEXP (XVECEXP (pat, 0, 2), 1), 0, 3));
      ro[6] = *(ro_loc[6] = &XVECEXP (XEXP (XVECEXP (pat, 0, 2), 1), 0, 4));
      ro[7] = *(ro_loc[7] = &XEXP (XVECEXP (pat, 0, 3), 0));
      recog_data.dup_loc[0] = &XEXP (XVECEXP (pat, 0, 2), 0);
      recog_data.dup_num[0] = 1;
      break;

    case 3636:  /* aarch64_compare_and_swaphi */
    case 3635:  /* aarch64_compare_and_swapqi */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0));
      ro[2] = *(ro_loc[2] = &XVECEXP (XEXP (XVECEXP (pat, 0, 2), 1), 0, 0));
      ro[3] = *(ro_loc[3] = &XVECEXP (XEXP (XVECEXP (pat, 0, 2), 1), 0, 1));
      ro[4] = *(ro_loc[4] = &XVECEXP (XEXP (XVECEXP (pat, 0, 2), 1), 0, 2));
      ro[5] = *(ro_loc[5] = &XVECEXP (XEXP (XVECEXP (pat, 0, 2), 1), 0, 3));
      ro[6] = *(ro_loc[6] = &XVECEXP (XEXP (XVECEXP (pat, 0, 2), 1), 0, 4));
      ro[7] = *(ro_loc[7] = &XEXP (XVECEXP (pat, 0, 3), 0));
      recog_data.dup_loc[0] = &XEXP (XVECEXP (pat, 0, 2), 0);
      recog_data.dup_num[0] = 1;
      break;

    case 3632:  /* aarch64_simd_fmlslq_lane_highv4sf */
    case 3630:  /* aarch64_simd_fmlslq_lane_lowv4sf */
    case 3628:  /* aarch64_simd_fmlsl_laneq_highv2sf */
    case 3626:  /* aarch64_simd_fmlsl_laneq_lowv2sf */
    case 3624:  /* aarch64_simd_fmlslq_laneq_highv4sf */
    case 3622:  /* aarch64_simd_fmlslq_laneq_lowv4sf */
    case 3620:  /* aarch64_simd_fmlsl_lane_highv2sf */
    case 3618:  /* aarch64_simd_fmlsl_lane_lowv2sf */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 2));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 0), 0));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0), 1));
      ro[5] = *(ro_loc[5] = &XVECEXP (XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 0), 1), 0, 0));
      break;

    case 3631:  /* aarch64_simd_fmlalq_lane_highv4sf */
    case 3629:  /* aarch64_simd_fmlalq_lane_lowv4sf */
    case 3627:  /* aarch64_simd_fmlal_laneq_highv2sf */
    case 3625:  /* aarch64_simd_fmlal_laneq_lowv2sf */
    case 3623:  /* aarch64_simd_fmlalq_laneq_highv4sf */
    case 3621:  /* aarch64_simd_fmlalq_laneq_lowv4sf */
    case 3619:  /* aarch64_simd_fmlal_lane_highv2sf */
    case 3617:  /* aarch64_simd_fmlal_lane_lowv2sf */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 2));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 0), 0));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 1));
      ro[5] = *(ro_loc[5] = &XVECEXP (XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 0), 1), 0, 0));
      break;

    case 3616:  /* aarch64_simd_fmlslq_highv4sf */
    case 3615:  /* aarch64_simd_fmlsl_highv2sf */
    case 3612:  /* aarch64_simd_fmlslq_lowv4sf */
    case 3611:  /* aarch64_simd_fmlsl_lowv2sf */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 2));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 0));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0), 1));
      ro[5] = *(ro_loc[5] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 1));
      break;

    case 3614:  /* aarch64_simd_fmlalq_highv4sf */
    case 3613:  /* aarch64_simd_fmlal_highv2sf */
    case 3610:  /* aarch64_simd_fmlalq_lowv4sf */
    case 3609:  /* aarch64_simd_fmlal_lowv2sf */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 2));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 0));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 1));
      ro[5] = *(ro_loc[5] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 1));
      break;

    case 3599:  /* bcaxqv2di4 */
    case 3598:  /* bcaxqv4si4 */
    case 3597:  /* bcaxqv8hi4 */
    case 3596:  /* bcaxqv16qi4 */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 1));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (pat, 1), 0), 1));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0));
      break;

    case 3576:  /* aarch64_be_crypto_sha1hv4si */
    case 3575:  /* aarch64_crypto_sha1hv4si */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XVECEXP (XEXP (pat, 1), 0, 0), 0));
      break;

    case 3573:  /* *aarch64_crypto_aesd_fused */
    case 3572:  /* *aarch64_crypto_aese_fused */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XVECEXP (XEXP (pat, 1), 0, 0), 0, 0));
      ro[2] = *(ro_loc[2] = &XVECEXP (XVECEXP (XEXP (pat, 1), 0, 0), 0, 1));
      break;

    case 3569:  /* *aarch64_crypto_aesdv16qi_xor_combine */
    case 3568:  /* *aarch64_crypto_aesev16qi_xor_combine */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XVECEXP (XEXP (pat, 1), 0, 1), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XVECEXP (XEXP (pat, 1), 0, 1), 1));
      ro[3] = *(ro_loc[3] = &XVECEXP (XEXP (pat, 1), 0, 0));
      break;

    case 3567:  /* *aarch64_crypto_aesdv16qi_xor_combine */
    case 3566:  /* *aarch64_crypto_aesev16qi_xor_combine */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XVECEXP (XEXP (pat, 1), 0, 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XVECEXP (XEXP (pat, 1), 0, 0), 1));
      ro[3] = *(ro_loc[3] = &XVECEXP (XEXP (pat, 1), 0, 1));
      break;

    case 3261:  /* aarch64_vec_store_lanesxi_lanedf */
    case 3260:  /* aarch64_vec_store_lanesxi_lanedi */
    case 3259:  /* aarch64_vec_store_lanesxi_lanev2df */
    case 3258:  /* aarch64_vec_store_lanesxi_lanev4sf */
    case 3257:  /* aarch64_vec_store_lanesxi_lanev2sf */
    case 3256:  /* aarch64_vec_store_lanesxi_lanev8hf */
    case 3255:  /* aarch64_vec_store_lanesxi_lanev4hf */
    case 3254:  /* aarch64_vec_store_lanesxi_lanev2di */
    case 3253:  /* aarch64_vec_store_lanesxi_lanev4si */
    case 3252:  /* aarch64_vec_store_lanesxi_lanev2si */
    case 3251:  /* aarch64_vec_store_lanesxi_lanev8hi */
    case 3250:  /* aarch64_vec_store_lanesxi_lanev4hi */
    case 3249:  /* aarch64_vec_store_lanesxi_lanev16qi */
    case 3248:  /* aarch64_vec_store_lanesxi_lanev8qi */
    case 3205:  /* aarch64_vec_store_lanesci_lanedf */
    case 3204:  /* aarch64_vec_store_lanesci_lanedi */
    case 3203:  /* aarch64_vec_store_lanesci_lanev2df */
    case 3202:  /* aarch64_vec_store_lanesci_lanev4sf */
    case 3201:  /* aarch64_vec_store_lanesci_lanev2sf */
    case 3200:  /* aarch64_vec_store_lanesci_lanev8hf */
    case 3199:  /* aarch64_vec_store_lanesci_lanev4hf */
    case 3198:  /* aarch64_vec_store_lanesci_lanev2di */
    case 3197:  /* aarch64_vec_store_lanesci_lanev4si */
    case 3196:  /* aarch64_vec_store_lanesci_lanev2si */
    case 3195:  /* aarch64_vec_store_lanesci_lanev8hi */
    case 3194:  /* aarch64_vec_store_lanesci_lanev4hi */
    case 3193:  /* aarch64_vec_store_lanesci_lanev16qi */
    case 3192:  /* aarch64_vec_store_lanesci_lanev8qi */
    case 3149:  /* aarch64_vec_store_lanesoi_lanedf */
    case 3148:  /* aarch64_vec_store_lanesoi_lanedi */
    case 3147:  /* aarch64_vec_store_lanesoi_lanev2df */
    case 3146:  /* aarch64_vec_store_lanesoi_lanev4sf */
    case 3145:  /* aarch64_vec_store_lanesoi_lanev2sf */
    case 3144:  /* aarch64_vec_store_lanesoi_lanev8hf */
    case 3143:  /* aarch64_vec_store_lanesoi_lanev4hf */
    case 3142:  /* aarch64_vec_store_lanesoi_lanev2di */
    case 3141:  /* aarch64_vec_store_lanesoi_lanev4si */
    case 3140:  /* aarch64_vec_store_lanesoi_lanev2si */
    case 3139:  /* aarch64_vec_store_lanesoi_lanev8hi */
    case 3138:  /* aarch64_vec_store_lanesoi_lanev4hi */
    case 3137:  /* aarch64_vec_store_lanesoi_lanev16qi */
    case 3136:  /* aarch64_vec_store_lanesoi_lanev8qi */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (pat, 1), 0, 0));
      ro[2] = *(ro_loc[2] = &XVECEXP (XEXP (pat, 1), 0, 2));
      break;

    case 3011:  /* aarch64_cmtstdi */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0), 1));
      break;

    case 2999:  /* aarch64_cmgtudi */
    case 2998:  /* aarch64_cmgeudi */
    case 2997:  /* aarch64_cmleudi */
    case 2996:  /* aarch64_cmltudi */
    case 2962:  /* aarch64_cmgtdi */
    case 2961:  /* aarch64_cmgedi */
    case 2960:  /* aarch64_cmeqdi */
    case 2959:  /* aarch64_cmledi */
    case 2958:  /* aarch64_cmltdi */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 1));
      break;

    case 2685:  /* aarch64_sqdmull2_nv4si_internal */
    case 2684:  /* aarch64_sqdmull2_nv8hi_internal */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 1), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0), 1));
      break;

    case 2683:  /* aarch64_sqdmull2_laneqv4si_internal */
    case 2682:  /* aarch64_sqdmull2_laneqv8hi_internal */
    case 2681:  /* aarch64_sqdmull2_lanev4si_internal */
    case 2680:  /* aarch64_sqdmull2_lanev8hi_internal */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 1), 0), 0), 0));
      ro[3] = *(ro_loc[3] = &XVECEXP (XEXP (XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 1), 0), 0), 1), 0, 0));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0), 1));
      break;

    case 2679:  /* aarch64_sqdmull2v4si_internal */
    case 2678:  /* aarch64_sqdmull2v8hi_internal */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 1), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0), 1));
      recog_data.dup_loc[0] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 1), 0), 1);
      recog_data.dup_num[0] = 3;
      break;

    case 2677:  /* aarch64_sqdmull_nv2si */
    case 2676:  /* aarch64_sqdmull_nv4hi */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 1), 0), 0));
      break;

    case 2675:  /* aarch64_sqdmull_laneqsi */
    case 2674:  /* aarch64_sqdmull_laneqhi */
    case 2673:  /* aarch64_sqdmull_lanesi */
    case 2672:  /* aarch64_sqdmull_lanehi */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 1), 0), 0));
      ro[3] = *(ro_loc[3] = &XVECEXP (XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 1), 0), 1), 0, 0));
      break;

    case 2671:  /* aarch64_sqdmull_laneqv2si */
    case 2670:  /* aarch64_sqdmull_laneqv4hi */
    case 2669:  /* aarch64_sqdmull_lanev2si */
    case 2668:  /* aarch64_sqdmull_lanev4hi */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 1), 0), 0), 0));
      ro[3] = *(ro_loc[3] = &XVECEXP (XEXP (XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 1), 0), 0), 1), 0, 0));
      break;

    case 3084:  /* aarch64_facgtdf */
    case 3083:  /* aarch64_facgedf */
    case 3082:  /* aarch64_facledf */
    case 3081:  /* aarch64_facltdf */
    case 3080:  /* aarch64_facgtsf */
    case 3079:  /* aarch64_facgesf */
    case 3078:  /* aarch64_faclesf */
    case 3077:  /* aarch64_facltsf */
    case 3076:  /* aarch64_facgthf */
    case 3075:  /* aarch64_facgehf */
    case 3074:  /* aarch64_faclehf */
    case 3073:  /* aarch64_faclthf */
    case 3072:  /* aarch64_facgtv2df */
    case 3071:  /* aarch64_facgev2df */
    case 3070:  /* aarch64_faclev2df */
    case 3069:  /* aarch64_facltv2df */
    case 3068:  /* aarch64_facgtv4sf */
    case 3067:  /* aarch64_facgev4sf */
    case 3066:  /* aarch64_faclev4sf */
    case 3065:  /* aarch64_facltv4sf */
    case 3064:  /* aarch64_facgtv2sf */
    case 3063:  /* aarch64_facgev2sf */
    case 3062:  /* aarch64_faclev2sf */
    case 3061:  /* aarch64_facltv2sf */
    case 3060:  /* aarch64_facgtv8hf */
    case 3059:  /* aarch64_facgev8hf */
    case 3058:  /* aarch64_faclev8hf */
    case 3057:  /* aarch64_facltv8hf */
    case 3056:  /* aarch64_facgtv4hf */
    case 3055:  /* aarch64_facgev4hf */
    case 3054:  /* aarch64_faclev4hf */
    case 3053:  /* aarch64_facltv4hf */
    case 2667:  /* aarch64_sqdmullsi */
    case 2666:  /* aarch64_sqdmullhi */
    case 2665:  /* aarch64_sqdmullv2si */
    case 2664:  /* aarch64_sqdmullv4hi */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 1), 0));
      break;

    case 2663:  /* aarch64_sqdmlsl2_nv4si_internal */
    case 2662:  /* aarch64_sqdmlal2_nv4si_internal */
    case 2661:  /* aarch64_sqdmlsl2_nv8hi_internal */
    case 2660:  /* aarch64_sqdmlal2_nv8hi_internal */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 0), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 1), 0), 0));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 0), 0), 1));
      break;

    case 2659:  /* aarch64_sqdmlsl2_laneqv4si_internal */
    case 2658:  /* aarch64_sqdmlal2_laneqv4si_internal */
    case 2657:  /* aarch64_sqdmlsl2_laneqv8hi_internal */
    case 2656:  /* aarch64_sqdmlal2_laneqv8hi_internal */
    case 2655:  /* aarch64_sqdmlsl2_lanev4si_internal */
    case 2654:  /* aarch64_sqdmlal2_lanev4si_internal */
    case 2653:  /* aarch64_sqdmlsl2_lanev8hi_internal */
    case 2652:  /* aarch64_sqdmlal2_lanev8hi_internal */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 0), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 1), 0), 0), 0));
      ro[4] = *(ro_loc[4] = &XVECEXP (XEXP (XEXP (XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 1), 0), 0), 1), 0, 0));
      ro[5] = *(ro_loc[5] = &XEXP (XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 0), 0), 1));
      break;

    case 2651:  /* aarch64_sqdmlsl2v4si_internal */
    case 2650:  /* aarch64_sqdmlal2v4si_internal */
    case 2649:  /* aarch64_sqdmlsl2v8hi_internal */
    case 2648:  /* aarch64_sqdmlal2v8hi_internal */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 0), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 1), 0), 0));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 0), 0), 1));
      recog_data.dup_loc[0] = &XEXP (XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 1), 0), 1);
      recog_data.dup_num[0] = 4;
      break;

    case 2647:  /* aarch64_sqdmlsl_nv2si */
    case 2646:  /* aarch64_sqdmlal_nv2si */
    case 2645:  /* aarch64_sqdmlsl_nv4hi */
    case 2644:  /* aarch64_sqdmlal_nv4hi */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 1), 0), 0));
      break;

    case 2643:  /* aarch64_sqdmlsl_laneqsi */
    case 2642:  /* aarch64_sqdmlal_laneqsi */
    case 2641:  /* aarch64_sqdmlsl_laneqhi */
    case 2640:  /* aarch64_sqdmlal_laneqhi */
    case 2639:  /* aarch64_sqdmlsl_lanesi */
    case 2638:  /* aarch64_sqdmlal_lanesi */
    case 2637:  /* aarch64_sqdmlsl_lanehi */
    case 2636:  /* aarch64_sqdmlal_lanehi */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 1), 0), 0));
      ro[4] = *(ro_loc[4] = &XVECEXP (XEXP (XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 1), 0), 1), 0, 0));
      break;

    case 2635:  /* aarch64_sqdmlsl_laneqv2si */
    case 2634:  /* aarch64_sqdmlal_laneqv2si */
    case 2633:  /* aarch64_sqdmlsl_laneqv4hi */
    case 2632:  /* aarch64_sqdmlal_laneqv4hi */
    case 2631:  /* aarch64_sqdmlsl_lanev2si */
    case 2630:  /* aarch64_sqdmlal_lanev2si */
    case 2629:  /* aarch64_sqdmlsl_lanev4hi */
    case 2628:  /* aarch64_sqdmlal_lanev4hi */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 1), 0), 0), 0));
      ro[4] = *(ro_loc[4] = &XVECEXP (XEXP (XEXP (XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 1), 0), 0), 1), 0, 0));
      break;

    case 2627:  /* aarch64_sqdmlslsi */
    case 2626:  /* aarch64_sqdmlalsi */
    case 2625:  /* aarch64_sqdmlslhi */
    case 2624:  /* aarch64_sqdmlalhi */
    case 2623:  /* aarch64_sqdmlslv2si */
    case 2622:  /* aarch64_sqdmlalv2si */
    case 2621:  /* aarch64_sqdmlslv4hi */
    case 2620:  /* aarch64_sqdmlalv4hi */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 1), 0));
      break;

    case 2619:  /* aarch64_sqrdmlsh_laneqsi */
    case 2618:  /* aarch64_sqrdmlah_laneqsi */
    case 2617:  /* aarch64_sqrdmlsh_laneqhi */
    case 2616:  /* aarch64_sqrdmlah_laneqhi */
    case 2615:  /* aarch64_sqrdmlsh_laneqv4si */
    case 2614:  /* aarch64_sqrdmlah_laneqv4si */
    case 2613:  /* aarch64_sqrdmlsh_laneqv2si */
    case 2612:  /* aarch64_sqrdmlah_laneqv2si */
    case 2611:  /* aarch64_sqrdmlsh_laneqv8hi */
    case 2610:  /* aarch64_sqrdmlah_laneqv8hi */
    case 2609:  /* aarch64_sqrdmlsh_laneqv4hi */
    case 2608:  /* aarch64_sqrdmlah_laneqv4hi */
    case 2607:  /* aarch64_sqrdmlsh_lanesi */
    case 2606:  /* aarch64_sqrdmlah_lanesi */
    case 2605:  /* aarch64_sqrdmlsh_lanehi */
    case 2604:  /* aarch64_sqrdmlah_lanehi */
    case 2603:  /* aarch64_sqrdmlsh_lanev4si */
    case 2602:  /* aarch64_sqrdmlah_lanev4si */
    case 2601:  /* aarch64_sqrdmlsh_lanev2si */
    case 2600:  /* aarch64_sqrdmlah_lanev2si */
    case 2599:  /* aarch64_sqrdmlsh_lanev8hi */
    case 2598:  /* aarch64_sqrdmlah_lanev8hi */
    case 2597:  /* aarch64_sqrdmlsh_lanev4hi */
    case 2596:  /* aarch64_sqrdmlah_lanev4hi */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (pat, 1), 0, 0));
      ro[2] = *(ro_loc[2] = &XVECEXP (XEXP (pat, 1), 0, 1));
      ro[3] = *(ro_loc[3] = &XEXP (XVECEXP (XEXP (pat, 1), 0, 2), 0));
      ro[4] = *(ro_loc[4] = &XVECEXP (XEXP (XVECEXP (XEXP (pat, 1), 0, 2), 1), 0, 0));
      break;

    case 2583:  /* aarch64_sqrdmulh_laneqsi */
    case 2582:  /* aarch64_sqdmulh_laneqsi */
    case 2581:  /* aarch64_sqrdmulh_laneqhi */
    case 2580:  /* aarch64_sqdmulh_laneqhi */
    case 2579:  /* aarch64_sqrdmulh_lanesi */
    case 2578:  /* aarch64_sqdmulh_lanesi */
    case 2577:  /* aarch64_sqrdmulh_lanehi */
    case 2576:  /* aarch64_sqdmulh_lanehi */
    case 2575:  /* aarch64_sqrdmulh_laneqv4si */
    case 2574:  /* aarch64_sqdmulh_laneqv4si */
    case 2573:  /* aarch64_sqrdmulh_laneqv2si */
    case 2572:  /* aarch64_sqdmulh_laneqv2si */
    case 2571:  /* aarch64_sqrdmulh_laneqv8hi */
    case 2570:  /* aarch64_sqdmulh_laneqv8hi */
    case 2569:  /* aarch64_sqrdmulh_laneqv4hi */
    case 2568:  /* aarch64_sqdmulh_laneqv4hi */
    case 2567:  /* aarch64_sqrdmulh_lanev4si */
    case 2566:  /* aarch64_sqdmulh_lanev4si */
    case 2565:  /* aarch64_sqrdmulh_lanev2si */
    case 2564:  /* aarch64_sqdmulh_lanev2si */
    case 2563:  /* aarch64_sqrdmulh_lanev8hi */
    case 2562:  /* aarch64_sqdmulh_lanev8hi */
    case 2561:  /* aarch64_sqrdmulh_lanev4hi */
    case 2560:  /* aarch64_sqdmulh_lanev4hi */
    case 2441:  /* *aarch64_vgetfmulxv2df */
    case 2440:  /* *aarch64_vgetfmulxv4sf */
    case 2439:  /* *aarch64_vgetfmulxv2sf */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (pat, 1), 0, 0));
      ro[2] = *(ro_loc[2] = &XEXP (XVECEXP (XEXP (pat, 1), 0, 1), 0));
      ro[3] = *(ro_loc[3] = &XVECEXP (XEXP (XVECEXP (XEXP (pat, 1), 0, 1), 1), 0, 0));
      break;

    case 2433:  /* *aarch64_mulx_eltv2df */
    case 2432:  /* *aarch64_mulx_eltv4sf */
    case 2431:  /* *aarch64_mulx_eltv2sf */
    case 2430:  /* *aarch64_mulx_elt_to_64v4sf */
    case 2429:  /* *aarch64_mulx_elt_to_128v2sf */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (pat, 1), 0, 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XVECEXP (XEXP (pat, 1), 0, 1), 0), 0));
      ro[3] = *(ro_loc[3] = &XVECEXP (XEXP (XEXP (XVECEXP (XEXP (pat, 1), 0, 1), 0), 1), 0, 0));
      break;

    case 2346:  /* aarch64_uaddw2v4si_internal */
    case 2345:  /* aarch64_saddw2v4si_internal */
    case 2344:  /* aarch64_uaddw2v8hi_internal */
    case 2343:  /* aarch64_saddw2v8hi_internal */
    case 2342:  /* aarch64_uaddw2v16qi_internal */
    case 2341:  /* aarch64_saddw2v16qi_internal */
    case 2340:  /* aarch64_uaddwv4si_internal */
    case 2339:  /* aarch64_saddwv4si_internal */
    case 2338:  /* aarch64_uaddwv8hi_internal */
    case 2337:  /* aarch64_saddwv8hi_internal */
    case 2336:  /* aarch64_uaddwv16qi_internal */
    case 2335:  /* aarch64_saddwv16qi_internal */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 1));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 1));
      break;

    case 2328:  /* aarch64_usubw2v4si_internal */
    case 2327:  /* aarch64_ssubw2v4si_internal */
    case 2326:  /* aarch64_usubw2v8hi_internal */
    case 2325:  /* aarch64_ssubw2v8hi_internal */
    case 2324:  /* aarch64_usubw2v16qi_internal */
    case 2323:  /* aarch64_ssubw2v16qi_internal */
    case 2322:  /* aarch64_usubwv4si_internal */
    case 2321:  /* aarch64_ssubwv4si_internal */
    case 2320:  /* aarch64_usubwv8hi_internal */
    case 2319:  /* aarch64_ssubwv8hi_internal */
    case 2318:  /* aarch64_usubwv16qi_internal */
    case 2317:  /* aarch64_ssubwv16qi_internal */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 1));
      break;

    case 2218:  /* aarch64_simd_bsldi_alt */
    case 2216:  /* *aarch64_simd_bslv2di_alt */
    case 2215:  /* *aarch64_simd_bslv4si_alt */
    case 2214:  /* *aarch64_simd_bslv2si_alt */
    case 2213:  /* *aarch64_simd_bslv8hi_alt */
    case 2212:  /* *aarch64_simd_bslv4hi_alt */
    case 2211:  /* *aarch64_simd_bslv16qi_alt */
    case 2210:  /* *aarch64_simd_bslv8qi_alt */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 0), 1));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 1));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0));
      recog_data.dup_loc[0] = &XEXP (XEXP (pat, 1), 1);
      recog_data.dup_num[0] = 2;
      break;

    case 2217:  /* aarch64_simd_bsldi_internal */
    case 2209:  /* aarch64_simd_bslv2di_internal */
    case 2208:  /* aarch64_simd_bslv4si_internal */
    case 2207:  /* aarch64_simd_bslv2si_internal */
    case 2206:  /* aarch64_simd_bslv8hi_internal */
    case 2205:  /* aarch64_simd_bslv4hi_internal */
    case 2204:  /* aarch64_simd_bslv16qi_internal */
    case 2203:  /* aarch64_simd_bslv8qi_internal */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 0), 1));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 1));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0));
      recog_data.dup_loc[0] = &XEXP (XEXP (pat, 1), 1);
      recog_data.dup_num[0] = 3;
      break;

    case 2058:  /* *aarch64_fcvtuv2dfv2di2_mult */
    case 2057:  /* *aarch64_fcvtv2dfv2di2_mult */
    case 2056:  /* *aarch64_fcvtuv4sfv4si2_mult */
    case 2055:  /* *aarch64_fcvtv4sfv4si2_mult */
    case 2054:  /* *aarch64_fcvtuv2sfv2si2_mult */
    case 2053:  /* *aarch64_fcvtv2sfv2si2_mult */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XVECEXP (XEXP (XEXP (pat, 1), 0), 0, 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XVECEXP (XEXP (XEXP (pat, 1), 0), 0, 0), 1));
      break;

    case 1953:  /* *aarch64_fnma4_elt_to_64v2df */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      ro[2] = *(ro_loc[2] = &XVECEXP (XEXP (XEXP (XEXP (pat, 1), 0), 1), 0, 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (pat, 1), 1), 0));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (pat, 1), 2));
      break;

    case 1952:  /* *aarch64_fnma4_elt_from_dupv2df */
    case 1951:  /* *aarch64_fnma4_elt_from_dupv4sf */
    case 1950:  /* *aarch64_fnma4_elt_from_dupv2sf */
    case 1949:  /* *aarch64_fnma4_elt_from_dupv8hf */
    case 1948:  /* *aarch64_fnma4_elt_from_dupv4hf */
    case 1947:  /* *aarch64_fnma4_elt_from_dupv4si */
    case 1946:  /* *aarch64_fnma4_elt_from_dupv2si */
    case 1945:  /* *aarch64_fnma4_elt_from_dupv8hi */
    case 1944:  /* *aarch64_fnma4_elt_from_dupv4hi */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 1), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (pat, 1), 2));
      break;

    case 1943:  /* *aarch64_fnma4_elt_to_64v4sf */
    case 1942:  /* *aarch64_fnma4_elt_to_128v2sf */
    case 1941:  /* *aarch64_fnma4_eltv2df */
    case 1940:  /* *aarch64_fnma4_eltv4sf */
    case 1939:  /* *aarch64_fnma4_eltv2sf */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 0));
      ro[2] = *(ro_loc[2] = &XVECEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 1), 0, 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (pat, 1), 2));
      break;

    case 1933:  /* *aarch64_fma4_elt_to_64v2df */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      ro[2] = *(ro_loc[2] = &XVECEXP (XEXP (XEXP (XEXP (pat, 1), 0), 1), 0, 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (pat, 1), 1));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (pat, 1), 2));
      break;

    case 1938:  /* fnmav2df4 */
    case 1937:  /* fnmav4sf4 */
    case 1936:  /* fnmav2sf4 */
    case 1935:  /* fnmav8hf4 */
    case 1934:  /* fnmav4hf4 */
    case 1932:  /* *aarch64_fma4_elt_from_dupv2df */
    case 1931:  /* *aarch64_fma4_elt_from_dupv4sf */
    case 1930:  /* *aarch64_fma4_elt_from_dupv2sf */
    case 1929:  /* *aarch64_fma4_elt_from_dupv8hf */
    case 1928:  /* *aarch64_fma4_elt_from_dupv4hf */
    case 1927:  /* *aarch64_fma4_elt_from_dupv4si */
    case 1926:  /* *aarch64_fma4_elt_from_dupv2si */
    case 1925:  /* *aarch64_fma4_elt_from_dupv8hi */
    case 1924:  /* *aarch64_fma4_elt_from_dupv4hi */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (pat, 1), 1));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (pat, 1), 2));
      break;

    case 1923:  /* *aarch64_fma4_elt_to_64v4sf */
    case 1922:  /* *aarch64_fma4_elt_to_128v2sf */
    case 1921:  /* *aarch64_fma4_eltv2df */
    case 1920:  /* *aarch64_fma4_eltv4sf */
    case 1919:  /* *aarch64_fma4_eltv2sf */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XVECEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 1), 0, 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (pat, 1), 1));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (pat, 1), 2));
      break;

    case 2298:  /* aarch64_usublv4si_lo_internal */
    case 2297:  /* aarch64_uaddlv4si_lo_internal */
    case 2296:  /* aarch64_ssublv4si_lo_internal */
    case 2295:  /* aarch64_saddlv4si_lo_internal */
    case 2294:  /* aarch64_usublv8hi_lo_internal */
    case 2293:  /* aarch64_uaddlv8hi_lo_internal */
    case 2292:  /* aarch64_ssublv8hi_lo_internal */
    case 2291:  /* aarch64_saddlv8hi_lo_internal */
    case 2290:  /* aarch64_usublv16qi_lo_internal */
    case 2289:  /* aarch64_uaddlv16qi_lo_internal */
    case 2288:  /* aarch64_ssublv16qi_lo_internal */
    case 2287:  /* aarch64_saddlv16qi_lo_internal */
    case 2286:  /* aarch64_usublv4si_hi_internal */
    case 2285:  /* aarch64_uaddlv4si_hi_internal */
    case 2284:  /* aarch64_ssublv4si_hi_internal */
    case 2283:  /* aarch64_saddlv4si_hi_internal */
    case 2282:  /* aarch64_usublv8hi_hi_internal */
    case 2281:  /* aarch64_uaddlv8hi_hi_internal */
    case 2280:  /* aarch64_ssublv8hi_hi_internal */
    case 2279:  /* aarch64_saddlv8hi_hi_internal */
    case 2278:  /* aarch64_usublv16qi_hi_internal */
    case 2277:  /* aarch64_uaddlv16qi_hi_internal */
    case 2276:  /* aarch64_ssublv16qi_hi_internal */
    case 2275:  /* aarch64_saddlv16qi_hi_internal */
    case 1883:  /* aarch64_simd_vec_umult_hi_v4si */
    case 1882:  /* aarch64_simd_vec_smult_hi_v4si */
    case 1881:  /* aarch64_simd_vec_umult_hi_v8hi */
    case 1880:  /* aarch64_simd_vec_smult_hi_v8hi */
    case 1879:  /* aarch64_simd_vec_umult_hi_v16qi */
    case 1878:  /* aarch64_simd_vec_smult_hi_v16qi */
    case 1877:  /* aarch64_simd_vec_umult_lo_v4si */
    case 1876:  /* aarch64_simd_vec_smult_lo_v4si */
    case 1875:  /* aarch64_simd_vec_umult_lo_v8hi */
    case 1874:  /* aarch64_simd_vec_smult_lo_v8hi */
    case 1873:  /* aarch64_simd_vec_umult_lo_v16qi */
    case 1872:  /* aarch64_simd_vec_smult_lo_v16qi */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 1));
      recog_data.dup_loc[0] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 1);
      recog_data.dup_num[0] = 3;
      break;

    case 1871:  /* *aarch64_umlslv2si */
    case 1870:  /* *aarch64_smlslv2si */
    case 1869:  /* *aarch64_umlslv4hi */
    case 1868:  /* *aarch64_smlslv4hi */
    case 1867:  /* *aarch64_umlslv8qi */
    case 1866:  /* *aarch64_smlslv8qi */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 1), 0));
      break;

    case 1859:  /* *aarch64_umlsl_hiv4si */
    case 1858:  /* *aarch64_smlsl_hiv4si */
    case 1857:  /* *aarch64_umlsl_hiv8hi */
    case 1856:  /* *aarch64_smlsl_hiv8hi */
    case 1855:  /* *aarch64_umlsl_hiv16qi */
    case 1854:  /* *aarch64_smlsl_hiv16qi */
    case 1853:  /* *aarch64_umlsl_lov4si */
    case 1852:  /* *aarch64_smlsl_lov4si */
    case 1851:  /* *aarch64_umlsl_lov8hi */
    case 1850:  /* *aarch64_smlsl_lov8hi */
    case 1849:  /* *aarch64_umlsl_lov16qi */
    case 1848:  /* *aarch64_smlsl_lov16qi */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 0), 1));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 1), 0), 0));
      recog_data.dup_loc[0] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 1), 0), 1);
      recog_data.dup_num[0] = 3;
      break;

    case 1847:  /* *aarch64_umlal_hiv4si */
    case 1846:  /* *aarch64_smlal_hiv4si */
    case 1845:  /* *aarch64_umlal_hiv8hi */
    case 1844:  /* *aarch64_smlal_hiv8hi */
    case 1843:  /* *aarch64_umlal_hiv16qi */
    case 1842:  /* *aarch64_smlal_hiv16qi */
    case 1841:  /* *aarch64_umlal_lov4si */
    case 1840:  /* *aarch64_smlal_lov4si */
    case 1839:  /* *aarch64_umlal_lov8hi */
    case 1838:  /* *aarch64_smlal_lov8hi */
    case 1837:  /* *aarch64_umlal_lov16qi */
    case 1836:  /* *aarch64_smlal_lov16qi */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 1));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0), 1));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 1), 0), 0));
      recog_data.dup_loc[0] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 1), 0), 1);
      recog_data.dup_num[0] = 3;
      break;

    case 1817:  /* aarch64_simd_move_hi_quad_be_v2df */
    case 1816:  /* aarch64_simd_move_hi_quad_be_v4sf */
    case 1815:  /* aarch64_simd_move_hi_quad_be_v8hf */
    case 1814:  /* aarch64_simd_move_hi_quad_be_v2di */
    case 1813:  /* aarch64_simd_move_hi_quad_be_v4si */
    case 1812:  /* aarch64_simd_move_hi_quad_be_v8hi */
    case 1811:  /* aarch64_simd_move_hi_quad_be_v16qi */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (pat, 1), 1), 1));
      recog_data.dup_loc[0] = &XEXP (XEXP (XEXP (pat, 1), 1), 0);
      recog_data.dup_num[0] = 0;
      break;

    case 1810:  /* aarch64_simd_move_hi_quad_v2df */
    case 1809:  /* aarch64_simd_move_hi_quad_v4sf */
    case 1808:  /* aarch64_simd_move_hi_quad_v8hf */
    case 1807:  /* aarch64_simd_move_hi_quad_v2di */
    case 1806:  /* aarch64_simd_move_hi_quad_v4si */
    case 1805:  /* aarch64_simd_move_hi_quad_v8hi */
    case 1804:  /* aarch64_simd_move_hi_quad_v16qi */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 1));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (pat, 1), 0), 1));
      recog_data.dup_loc[0] = &XEXP (XEXP (XEXP (pat, 1), 0), 0);
      recog_data.dup_num[0] = 0;
      break;

    case 1803:  /* move_lo_quad_internal_be_v2df */
    case 1802:  /* move_lo_quad_internal_be_v2di */
    case 1801:  /* move_lo_quad_internal_be_v4sf */
    case 1800:  /* move_lo_quad_internal_be_v8hf */
    case 1799:  /* move_lo_quad_internal_be_v4si */
    case 1798:  /* move_lo_quad_internal_be_v8hi */
    case 1797:  /* move_lo_quad_internal_be_v16qi */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 1));
      break;

    case 1717:  /* *aarch64_mls_elt_to_64v4si */
    case 1716:  /* *aarch64_mls_elt_to_128v2si */
    case 1715:  /* *aarch64_mls_elt_to_64v8hi */
    case 1714:  /* *aarch64_mls_elt_to_128v4hi */
    case 1713:  /* *aarch64_mls_eltv4si */
    case 1712:  /* *aarch64_mls_eltv2si */
    case 1711:  /* *aarch64_mls_eltv8hi */
    case 1710:  /* *aarch64_mls_eltv4hi */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XVECEXP (XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 0), 1), 0, 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (pat, 1), 1), 1));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (pat, 1), 0));
      break;

    case 1699:  /* *aarch64_mla_elt_to_64v4si */
    case 1698:  /* *aarch64_mla_elt_to_128v2si */
    case 1697:  /* *aarch64_mla_elt_to_64v8hi */
    case 1696:  /* *aarch64_mla_elt_to_128v4hi */
    case 1695:  /* *aarch64_mla_eltv4si */
    case 1694:  /* *aarch64_mla_eltv2si */
    case 1693:  /* *aarch64_mla_eltv8hi */
    case 1692:  /* *aarch64_mla_eltv4hi */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XVECEXP (XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0), 1), 0, 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (pat, 1), 0), 1));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (pat, 1), 1));
      break;

    case 4529:  /* *fcmuovnx2df_and */
    case 4528:  /* *fcmuovnx4sf_and */
    case 4527:  /* *fcmuovnx8hf_and */
    case 4526:  /* *fcmgtvnx2df_and */
    case 4525:  /* *fcmgevnx2df_and */
    case 4524:  /* *fcmnevnx2df_and */
    case 4523:  /* *fcmeqvnx2df_and */
    case 4522:  /* *fcmlevnx2df_and */
    case 4521:  /* *fcmltvnx2df_and */
    case 4520:  /* *fcmgtvnx4sf_and */
    case 4519:  /* *fcmgevnx4sf_and */
    case 4518:  /* *fcmnevnx4sf_and */
    case 4517:  /* *fcmeqvnx4sf_and */
    case 4516:  /* *fcmlevnx4sf_and */
    case 4515:  /* *fcmltvnx4sf_and */
    case 4514:  /* *fcmgtvnx8hf_and */
    case 4513:  /* *fcmgevnx8hf_and */
    case 4512:  /* *fcmnevnx8hf_and */
    case 4511:  /* *fcmeqvnx8hf_and */
    case 4510:  /* *fcmlevnx8hf_and */
    case 4509:  /* *fcmltvnx8hf_and */
    case 4190:  /* pred_xorvnx2bi3 */
    case 4189:  /* pred_iorvnx2bi3 */
    case 4188:  /* pred_andvnx2bi3 */
    case 4187:  /* pred_xorvnx4bi3 */
    case 4186:  /* pred_iorvnx4bi3 */
    case 4185:  /* pred_andvnx4bi3 */
    case 4184:  /* pred_xorvnx8bi3 */
    case 4183:  /* pred_iorvnx8bi3 */
    case 4182:  /* pred_andvnx8bi3 */
    case 4181:  /* pred_xorvnx16bi3 */
    case 4180:  /* pred_iorvnx16bi3 */
    case 4179:  /* pred_andvnx16bi3 */
    case 3593:  /* eor3qv2di4 */
    case 3592:  /* eor3qv4si4 */
    case 3591:  /* eor3qv8hi4 */
    case 3590:  /* eor3qv16qi4 */
    case 1691:  /* aarch64_mlav4si */
    case 1690:  /* aarch64_mlav2si */
    case 1689:  /* aarch64_mlav8hi */
    case 1688:  /* aarch64_mlav4hi */
    case 1687:  /* aarch64_mlav16qi */
    case 1686:  /* aarch64_mlav8qi */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 1));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (pat, 1), 0), 1));
      break;

    case 1638:  /* *aarch64_simd_vec_copy_lane_to_64v4sf */
    case 1637:  /* *aarch64_simd_vec_copy_lane_to_128v2sf */
    case 1636:  /* *aarch64_simd_vec_copy_lane_to_64v8hf */
    case 1635:  /* *aarch64_simd_vec_copy_lane_to_128v4hf */
    case 1634:  /* *aarch64_simd_vec_copy_lane_to_64v4si */
    case 1633:  /* *aarch64_simd_vec_copy_lane_to_128v2si */
    case 1632:  /* *aarch64_simd_vec_copy_lane_to_64v8hi */
    case 1631:  /* *aarch64_simd_vec_copy_lane_to_128v4hi */
    case 1630:  /* *aarch64_simd_vec_copy_lane_to_64v16qi */
    case 1629:  /* *aarch64_simd_vec_copy_lane_to_128v8qi */
    case 1628:  /* *aarch64_simd_vec_copy_lanev2df */
    case 1627:  /* *aarch64_simd_vec_copy_lanev4sf */
    case 1626:  /* *aarch64_simd_vec_copy_lanev2sf */
    case 1625:  /* *aarch64_simd_vec_copy_lanev8hf */
    case 1624:  /* *aarch64_simd_vec_copy_lanev4hf */
    case 1623:  /* *aarch64_simd_vec_copy_lanev2di */
    case 1622:  /* *aarch64_simd_vec_copy_lanev4si */
    case 1621:  /* *aarch64_simd_vec_copy_lanev2si */
    case 1620:  /* *aarch64_simd_vec_copy_lanev8hi */
    case 1619:  /* *aarch64_simd_vec_copy_lanev4hi */
    case 1618:  /* *aarch64_simd_vec_copy_lanev16qi */
    case 1617:  /* *aarch64_simd_vec_copy_lanev8qi */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 1));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (pat, 1), 2));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0));
      ro[4] = *(ro_loc[4] = &XVECEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 1), 0, 0));
      break;

    case 1616:  /* aarch64_simd_vec_setv2df */
    case 1615:  /* aarch64_simd_vec_setv4sf */
    case 1614:  /* aarch64_simd_vec_setv2sf */
    case 1613:  /* aarch64_simd_vec_setv8hf */
    case 1612:  /* aarch64_simd_vec_setv4hf */
    case 1611:  /* aarch64_simd_vec_setv2di */
    case 1610:  /* aarch64_simd_vec_setv4si */
    case 1609:  /* aarch64_simd_vec_setv2si */
    case 1608:  /* aarch64_simd_vec_setv8hi */
    case 1607:  /* aarch64_simd_vec_setv4hi */
    case 1606:  /* aarch64_simd_vec_setv16qi */
    case 1605:  /* aarch64_simd_vec_setv8qi */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (pat, 1), 2));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (pat, 1), 1));
      break;

    case 1532:  /* *aarch64_uabdv4si_3 */
    case 1531:  /* *aarch64_sabdv4si_3 */
    case 1530:  /* *aarch64_uabdv2si_3 */
    case 1529:  /* *aarch64_sabdv2si_3 */
    case 1528:  /* *aarch64_uabdv8hi_3 */
    case 1527:  /* *aarch64_sabdv8hi_3 */
    case 1526:  /* *aarch64_uabdv4hi_3 */
    case 1525:  /* *aarch64_sabdv4hi_3 */
    case 1524:  /* *aarch64_uabdv16qi_3 */
    case 1523:  /* *aarch64_sabdv16qi_3 */
    case 1522:  /* *aarch64_uabdv8qi_3 */
    case 1521:  /* *aarch64_sabdv8qi_3 */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (pat, 1), 0), 1));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (pat, 1), 1));
      recog_data.dup_loc[0] = &XEXP (XEXP (XEXP (pat, 1), 1), 1);
      recog_data.dup_num[0] = 2;
      recog_data.dup_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 1), 0);
      recog_data.dup_num[1] = 1;
      break;

    case 1498:  /* *aarch64_mul3_elt_to_64v2df */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      ro[2] = *(ro_loc[2] = &XVECEXP (XEXP (XEXP (XEXP (pat, 1), 0), 1), 0, 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (pat, 1), 1));
      break;

    case 1472:  /* *aarch64_mul3_elt_to_64v4sf */
    case 1471:  /* *aarch64_mul3_elt_to_128v2sf */
    case 1470:  /* *aarch64_mul3_elt_to_64v4si */
    case 1469:  /* *aarch64_mul3_elt_to_128v2si */
    case 1468:  /* *aarch64_mul3_elt_to_64v8hi */
    case 1467:  /* *aarch64_mul3_elt_to_128v4hi */
    case 1466:  /* *aarch64_mul3_eltv2df */
    case 1465:  /* *aarch64_mul3_eltv4sf */
    case 1464:  /* *aarch64_mul3_eltv2sf */
    case 1463:  /* *aarch64_mul3_eltv8hf */
    case 1462:  /* *aarch64_mul3_eltv4hf */
    case 1461:  /* *aarch64_mul3_eltv4si */
    case 1460:  /* *aarch64_mul3_eltv2si */
    case 1459:  /* *aarch64_mul3_eltv8hi */
    case 1458:  /* *aarch64_mul3_eltv4hi */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XVECEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 1), 0, 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (pat, 1), 1));
      break;

    case 1457:  /* aarch64_udot_laneqv16qi */
    case 1456:  /* aarch64_sdot_laneqv16qi */
    case 1455:  /* aarch64_udot_laneqv8qi */
    case 1454:  /* aarch64_sdot_laneqv8qi */
    case 1453:  /* aarch64_udot_lanev16qi */
    case 1452:  /* aarch64_sdot_lanev16qi */
    case 1451:  /* aarch64_udot_lanev8qi */
    case 1450:  /* aarch64_sdot_lanev8qi */
    case 1445:  /* aarch64_fcmlaq_lane270v4sf */
    case 1444:  /* aarch64_fcmlaq_lane180v4sf */
    case 1443:  /* aarch64_fcmlaq_lane90v4sf */
    case 1442:  /* aarch64_fcmlaq_lane0v4sf */
    case 1441:  /* aarch64_fcmlaq_lane270v8hf */
    case 1440:  /* aarch64_fcmlaq_lane180v8hf */
    case 1439:  /* aarch64_fcmlaq_lane90v8hf */
    case 1438:  /* aarch64_fcmlaq_lane0v8hf */
    case 1437:  /* aarch64_fcmla_laneq270v4hf */
    case 1436:  /* aarch64_fcmla_laneq180v4hf */
    case 1435:  /* aarch64_fcmla_laneq90v4hf */
    case 1434:  /* aarch64_fcmla_laneq0v4hf */
    case 1433:  /* aarch64_fcmla_lane270v2df */
    case 1432:  /* aarch64_fcmla_lane180v2df */
    case 1431:  /* aarch64_fcmla_lane90v2df */
    case 1430:  /* aarch64_fcmla_lane0v2df */
    case 1429:  /* aarch64_fcmla_lane270v4sf */
    case 1428:  /* aarch64_fcmla_lane180v4sf */
    case 1427:  /* aarch64_fcmla_lane90v4sf */
    case 1426:  /* aarch64_fcmla_lane0v4sf */
    case 1425:  /* aarch64_fcmla_lane270v2sf */
    case 1424:  /* aarch64_fcmla_lane180v2sf */
    case 1423:  /* aarch64_fcmla_lane90v2sf */
    case 1422:  /* aarch64_fcmla_lane0v2sf */
    case 1421:  /* aarch64_fcmla_lane270v8hf */
    case 1420:  /* aarch64_fcmla_lane180v8hf */
    case 1419:  /* aarch64_fcmla_lane90v8hf */
    case 1418:  /* aarch64_fcmla_lane0v8hf */
    case 1417:  /* aarch64_fcmla_lane270v4hf */
    case 1416:  /* aarch64_fcmla_lane180v4hf */
    case 1415:  /* aarch64_fcmla_lane90v4hf */
    case 1414:  /* aarch64_fcmla_lane0v4hf */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 0));
      ro[2] = *(ro_loc[2] = &XVECEXP (XEXP (XEXP (pat, 1), 1), 0, 0));
      ro[3] = *(ro_loc[3] = &XVECEXP (XEXP (XEXP (pat, 1), 1), 0, 1));
      ro[4] = *(ro_loc[4] = &XVECEXP (XEXP (XEXP (pat, 1), 1), 0, 2));
      break;

    case 1449:  /* aarch64_udotv16qi */
    case 1448:  /* aarch64_sdotv16qi */
    case 1447:  /* aarch64_udotv8qi */
    case 1446:  /* aarch64_sdotv8qi */
    case 1413:  /* aarch64_fcmla270v2df */
    case 1412:  /* aarch64_fcmla180v2df */
    case 1411:  /* aarch64_fcmla90v2df */
    case 1410:  /* aarch64_fcmla0v2df */
    case 1409:  /* aarch64_fcmla270v4sf */
    case 1408:  /* aarch64_fcmla180v4sf */
    case 1407:  /* aarch64_fcmla90v4sf */
    case 1406:  /* aarch64_fcmla0v4sf */
    case 1405:  /* aarch64_fcmla270v2sf */
    case 1404:  /* aarch64_fcmla180v2sf */
    case 1403:  /* aarch64_fcmla90v2sf */
    case 1402:  /* aarch64_fcmla0v2sf */
    case 1401:  /* aarch64_fcmla270v8hf */
    case 1400:  /* aarch64_fcmla180v8hf */
    case 1399:  /* aarch64_fcmla90v8hf */
    case 1398:  /* aarch64_fcmla0v8hf */
    case 1397:  /* aarch64_fcmla270v4hf */
    case 1396:  /* aarch64_fcmla180v4hf */
    case 1395:  /* aarch64_fcmla90v4hf */
    case 1394:  /* aarch64_fcmla0v4hf */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 0));
      ro[2] = *(ro_loc[2] = &XVECEXP (XEXP (XEXP (pat, 1), 1), 0, 0));
      ro[3] = *(ro_loc[3] = &XVECEXP (XEXP (XEXP (pat, 1), 1), 0, 1));
      break;

    case 3916:  /* *vec_extractvnx2dfdf_ext */
    case 3915:  /* *vec_extractvnx4sfsf_ext */
    case 3914:  /* *vec_extractvnx8hfhf_ext */
    case 3913:  /* *vec_extractvnx2didi_ext */
    case 3912:  /* *vec_extractvnx4sisi_ext */
    case 3911:  /* *vec_extractvnx8hihi_ext */
    case 3910:  /* *vec_extractvnx16qiqi_ext */
    case 3909:  /* *vec_extractvnx2dfdf_dup */
    case 3908:  /* *vec_extractvnx4sfsf_dup */
    case 3907:  /* *vec_extractvnx8hfhf_dup */
    case 3906:  /* *vec_extractvnx2didi_dup */
    case 3905:  /* *vec_extractvnx4sisi_dup */
    case 3904:  /* *vec_extractvnx8hihi_dup */
    case 3903:  /* *vec_extractvnx16qiqi_dup */
    case 3902:  /* *vec_extractvnx2dfdf_v128 */
    case 3901:  /* *vec_extractvnx4sfsf_v128 */
    case 3900:  /* *vec_extractvnx8hfhf_v128 */
    case 3899:  /* *vec_extractvnx2didi_v128 */
    case 3898:  /* *vec_extractvnx4sisi_v128 */
    case 3897:  /* *vec_extractvnx8hihi_v128 */
    case 3896:  /* *vec_extractvnx16qiqi_v128 */
    case 2246:  /* aarch64_get_lanev2df */
    case 2245:  /* aarch64_get_lanev4sf */
    case 2244:  /* aarch64_get_lanev2sf */
    case 2243:  /* aarch64_get_lanev8hf */
    case 2242:  /* aarch64_get_lanev4hf */
    case 2241:  /* aarch64_get_lanev2di */
    case 2240:  /* aarch64_get_lanev4si */
    case 2239:  /* aarch64_get_lanev2si */
    case 2238:  /* aarch64_get_lanev8hi */
    case 2237:  /* aarch64_get_lanev4hi */
    case 2236:  /* aarch64_get_lanev16qi */
    case 2235:  /* aarch64_get_lanev8qi */
    case 1158:  /* aarch64_store_lane0v2df */
    case 1157:  /* aarch64_store_lane0v4sf */
    case 1156:  /* aarch64_store_lane0v2sf */
    case 1155:  /* aarch64_store_lane0v8hf */
    case 1154:  /* aarch64_store_lane0v4hf */
    case 1153:  /* aarch64_store_lane0v2di */
    case 1152:  /* aarch64_store_lane0v4si */
    case 1151:  /* aarch64_store_lane0v2si */
    case 1150:  /* aarch64_store_lane0v8hi */
    case 1149:  /* aarch64_store_lane0v4hi */
    case 1148:  /* aarch64_store_lane0v16qi */
    case 1147:  /* aarch64_store_lane0v8qi */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 0));
      ro[2] = *(ro_loc[2] = &XVECEXP (XEXP (XEXP (pat, 1), 1), 0, 0));
      break;

    case 4090:  /* *aarch64_sve_dup_lanevnx2df */
    case 4089:  /* *aarch64_sve_dup_lanevnx4sf */
    case 4088:  /* *aarch64_sve_dup_lanevnx8hf */
    case 4087:  /* *aarch64_sve_dup_lanevnx2di */
    case 4086:  /* *aarch64_sve_dup_lanevnx4si */
    case 4085:  /* *aarch64_sve_dup_lanevnx8hi */
    case 4084:  /* *aarch64_sve_dup_lanevnx16qi */
    case 2234:  /* *aarch64_get_lane_zero_extenddiv8hi */
    case 2233:  /* *aarch64_get_lane_zero_extendsiv8hi */
    case 2232:  /* *aarch64_get_lane_zero_extenddiv4hi */
    case 2231:  /* *aarch64_get_lane_zero_extendsiv4hi */
    case 2230:  /* *aarch64_get_lane_zero_extenddiv16qi */
    case 2229:  /* *aarch64_get_lane_zero_extendsiv16qi */
    case 2228:  /* *aarch64_get_lane_zero_extenddiv8qi */
    case 2227:  /* *aarch64_get_lane_zero_extendsiv8qi */
    case 2226:  /* *aarch64_get_lane_extenddiv8hi */
    case 2225:  /* *aarch64_get_lane_extendsiv8hi */
    case 2224:  /* *aarch64_get_lane_extenddiv4hi */
    case 2223:  /* *aarch64_get_lane_extendsiv4hi */
    case 2222:  /* *aarch64_get_lane_extenddiv16qi */
    case 2221:  /* *aarch64_get_lane_extendsiv16qi */
    case 2220:  /* *aarch64_get_lane_extenddiv8qi */
    case 2219:  /* *aarch64_get_lane_extendsiv8qi */
    case 1134:  /* aarch64_dup_lane_to_64v4sf */
    case 1133:  /* aarch64_dup_lane_to_128v2sf */
    case 1132:  /* aarch64_dup_lane_to_64v8hf */
    case 1131:  /* aarch64_dup_lane_to_128v4hf */
    case 1130:  /* aarch64_dup_lane_to_64v4si */
    case 1129:  /* aarch64_dup_lane_to_128v2si */
    case 1128:  /* aarch64_dup_lane_to_64v8hi */
    case 1127:  /* aarch64_dup_lane_to_128v4hi */
    case 1126:  /* aarch64_dup_lane_to_64v16qi */
    case 1125:  /* aarch64_dup_lane_to_128v8qi */
    case 1124:  /* aarch64_dup_lanev2df */
    case 1123:  /* aarch64_dup_lanev4sf */
    case 1122:  /* aarch64_dup_lanev2sf */
    case 1121:  /* aarch64_dup_lanev8hf */
    case 1120:  /* aarch64_dup_lanev4hf */
    case 1119:  /* aarch64_dup_lanev2di */
    case 1118:  /* aarch64_dup_lanev4si */
    case 1117:  /* aarch64_dup_lanev2si */
    case 1116:  /* aarch64_dup_lanev8hi */
    case 1115:  /* aarch64_dup_lanev4hi */
    case 1114:  /* aarch64_dup_lanev16qi */
    case 1113:  /* aarch64_dup_lanev8qi */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      ro[2] = *(ro_loc[2] = &XVECEXP (XEXP (XEXP (XEXP (pat, 1), 0), 1), 0, 0));
      break;

    case 4975:  /* aarch64_sve_floatunsvnx2divnx2df2 */
    case 4974:  /* aarch64_sve_floatvnx2divnx2df2 */
    case 4973:  /* aarch64_sve_floatunsvnx4sivnx2df2 */
    case 4972:  /* aarch64_sve_floatvnx4sivnx2df2 */
    case 4971:  /* *floatunsvnx2divnx4sf2 */
    case 4970:  /* *floatvnx2divnx4sf2 */
    case 4969:  /* *floatunsvnx4sivnx4sf2 */
    case 4968:  /* *floatvnx4sivnx4sf2 */
    case 4967:  /* *floatunsvnx4sivnx8hf2 */
    case 4966:  /* *floatvnx4sivnx8hf2 */
    case 4965:  /* *floatunsvnx8hivnx8hf2 */
    case 4964:  /* *floatvnx8hivnx8hf2 */
    case 4963:  /* *floatunsvnx16qivnx8hf2 */
    case 4962:  /* *floatvnx16qivnx8hf2 */
    case 4961:  /* *fixuns_truncvnx2dfvnx2di2 */
    case 4960:  /* *fix_truncvnx2dfvnx2di2 */
    case 4959:  /* *fixuns_truncvnx2dfvnx4si2 */
    case 4958:  /* *fix_truncvnx2dfvnx4si2 */
    case 4957:  /* *fixuns_truncvnx4sfvnx2di2 */
    case 4956:  /* *fix_truncvnx4sfvnx2di2 */
    case 4955:  /* *fixuns_truncvnx4sfvnx4si2 */
    case 4954:  /* *fix_truncvnx4sfvnx4si2 */
    case 4953:  /* *fixuns_truncv16hsfvnx4si2 */
    case 4952:  /* *fix_truncv16hsfvnx4si2 */
    case 4951:  /* *fixuns_truncv16hsfvnx8hi2 */
    case 4950:  /* *fix_truncv16hsfvnx8hi2 */
    case 4949:  /* *fixuns_truncv16hsfvnx16qi2 */
    case 4948:  /* *fix_truncv16hsfvnx16qi2 */
    case 4926:  /* *sqrtvnx2df2 */
    case 4925:  /* *negvnx2df2 */
    case 4924:  /* *absvnx2df2 */
    case 4923:  /* *sqrtvnx4sf2 */
    case 4922:  /* *negvnx4sf2 */
    case 4921:  /* *absvnx4sf2 */
    case 4920:  /* *sqrtvnx8hf2 */
    case 4919:  /* *negvnx8hf2 */
    case 4918:  /* *absvnx8hf2 */
    case 4149:  /* *popcountvnx2di2 */
    case 4148:  /* *one_cmplvnx2di2 */
    case 4147:  /* *negvnx2di2 */
    case 4146:  /* *absvnx2di2 */
    case 4145:  /* *popcountvnx4si2 */
    case 4144:  /* *one_cmplvnx4si2 */
    case 4143:  /* *negvnx4si2 */
    case 4142:  /* *absvnx4si2 */
    case 4141:  /* *popcountvnx8hi2 */
    case 4140:  /* *one_cmplvnx8hi2 */
    case 4139:  /* *negvnx8hi2 */
    case 4138:  /* *absvnx8hi2 */
    case 4137:  /* *popcountvnx16qi2 */
    case 4136:  /* *one_cmplvnx16qi2 */
    case 4135:  /* *negvnx16qi2 */
    case 4134:  /* *absvnx16qi2 */
    case 2438:  /* *aarch64_mulx_elt_from_dupv2df */
    case 2437:  /* *aarch64_mulx_elt_from_dupv4sf */
    case 2436:  /* *aarch64_mulx_elt_from_dupv2sf */
    case 2435:  /* *aarch64_mulx_elt_from_dupv8hf */
    case 2434:  /* *aarch64_mulx_elt_from_dupv4hf */
    case 1100:  /* despeculate_simpleti */
    case 1099:  /* despeculate_simpledi */
    case 1098:  /* despeculate_simplesi */
    case 1097:  /* despeculate_simplehi */
    case 1096:  /* despeculate_simpleqi */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (pat, 1), 0, 0));
      ro[2] = *(ro_loc[2] = &XEXP (XVECEXP (XEXP (pat, 1), 0, 1), 0));
      break;

    case 1085:  /* speculation_tracker */
      ro[0] = *(ro_loc[0] = &XVECEXP (XEXP (pat, 1), 0, 1));
      break;

    case 1083:  /* set_fpsr */
    case 1081:  /* set_fpcr */
      ro[0] = *(ro_loc[0] = &XVECEXP (pat, 0, 0));
      break;

    case 1080:  /* stack_protect_test_di */
    case 1079:  /* stack_protect_test_si */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 0), 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0, 0));
      ro[2] = *(ro_loc[2] = &XVECEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0, 1));
      ro[3] = *(ro_loc[3] = &XEXP (XVECEXP (pat, 0, 1), 0));
      break;

    case 1078:  /* stack_protect_set_di */
    case 1077:  /* stack_protect_set_si */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 0), 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0, 0));
      ro[2] = *(ro_loc[2] = &XEXP (XVECEXP (pat, 0, 1), 0));
      break;

    case 3604:  /* aarch64_sm3tt2bqv4si */
    case 3603:  /* aarch64_sm3tt2aqv4si */
    case 3602:  /* aarch64_sm3tt1bqv4si */
    case 3601:  /* aarch64_sm3tt1aqv4si */
    case 1074:  /* probe_sve_stack_clash_di */
    case 1073:  /* probe_sve_stack_clash_si */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (pat, 1), 0, 0));
      ro[2] = *(ro_loc[2] = &XVECEXP (XEXP (pat, 1), 0, 1));
      ro[3] = *(ro_loc[3] = &XVECEXP (XEXP (pat, 1), 0, 2));
      ro[4] = *(ro_loc[4] = &XVECEXP (XEXP (pat, 1), 0, 3));
      break;

    case 1065:  /* stack_tie */
      ro[0] = *(ro_loc[0] = &XVECEXP (XEXP (pat, 1), 0, 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (pat, 1), 0, 1));
      break;

    case 1064:  /* tlsdesc_small_sve_di */
    case 1063:  /* tlsdesc_small_sve_si */
      ro[0] = *(ro_loc[0] = &XVECEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XVECEXP (pat, 0, 51), 0));
      break;

    case 1062:  /* tlsdesc_small_advsimd_di */
    case 1061:  /* tlsdesc_small_advsimd_si */
      ro[0] = *(ro_loc[0] = &XVECEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XVECEXP (pat, 0, 3), 0));
      break;

    case 3764:  /* aarch64_load_exclusivehi */
    case 3763:  /* aarch64_load_exclusiveqi */
    case 1052:  /* tlsie_tiny_sidi */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (XEXP (pat, 1), 0), 0, 0));
      ro[2] = *(ro_loc[2] = &XVECEXP (XEXP (XEXP (pat, 1), 0), 0, 1));
      break;

    case 1046:  /* *tlsgd_small_di */
    case 1045:  /* *tlsgd_small_si */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 0), 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XVECEXP (pat, 0, 1), 0, 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0));
      break;

    case 1084:  /* get_fpsr */
    case 1082:  /* get_fpcr */
    case 1076:  /* reg_stack_protect_address_di */
    case 1075:  /* reg_stack_protect_address_si */
    case 1044:  /* aarch64_load_tp_hard */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      break;

    case 1042:  /* ldr_got_small_28k_sidi */
    case 1039:  /* ldr_got_small_sidi */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XVECEXP (XEXP (XEXP (pat, 1), 0), 0, 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XVECEXP (XEXP (XEXP (pat, 1), 0), 0, 0), 0), 1));
      break;

    case 1041:  /* ldr_got_small_28k_di */
    case 1040:  /* ldr_got_small_28k_si */
    case 1038:  /* ldr_got_small_di */
    case 1037:  /* ldr_got_small_si */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XVECEXP (XEXP (pat, 1), 0, 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XVECEXP (XEXP (pat, 1), 0, 0), 0), 1));
      break;

    case 1031:  /* aarch64_movtfhigh_di */
    case 1030:  /* aarch64_movtihigh_di */
      ro[0] = *(ro_loc[0] = &XEXP (XEXP (pat, 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 0));
      break;

    case 4881:  /* *fold_left_plus_vnx2df */
    case 4880:  /* *fold_left_plus_vnx4sf */
    case 4879:  /* *fold_left_plus_vnx8hf */
    case 4831:  /* fold_extract_last_vnx2df */
    case 4830:  /* fold_extract_last_vnx4sf */
    case 4829:  /* fold_extract_last_vnx8hf */
    case 4828:  /* fold_extract_last_vnx2di */
    case 4827:  /* fold_extract_last_vnx4si */
    case 4826:  /* fold_extract_last_vnx8hi */
    case 4825:  /* fold_extract_last_vnx16qi */
    case 4558:  /* aarch64_sve_dupvnx2di_const */
    case 4557:  /* aarch64_sve_dupvnx4si_const */
    case 4556:  /* aarch64_sve_dupvnx8hi_const */
    case 4555:  /* aarch64_sve_dupvnx16qi_const */
    case 4547:  /* *pred_fcmgtvnx2df */
    case 4546:  /* *pred_fcmgevnx2df */
    case 4545:  /* *pred_fcmnevnx2df */
    case 4544:  /* *pred_fcmeqvnx2df */
    case 4543:  /* *pred_fcmlevnx2df */
    case 4542:  /* *pred_fcmltvnx2df */
    case 4541:  /* *pred_fcmgtvnx4sf */
    case 4540:  /* *pred_fcmgevnx4sf */
    case 4539:  /* *pred_fcmnevnx4sf */
    case 4538:  /* *pred_fcmeqvnx4sf */
    case 4537:  /* *pred_fcmlevnx4sf */
    case 4536:  /* *pred_fcmltvnx4sf */
    case 4535:  /* *pred_fcmgtvnx8hf */
    case 4534:  /* *pred_fcmgevnx8hf */
    case 4533:  /* *pred_fcmnevnx8hf */
    case 4532:  /* *pred_fcmeqvnx8hf */
    case 4531:  /* *pred_fcmlevnx8hf */
    case 4530:  /* *pred_fcmltvnx8hf */
    case 4097:  /* *aarch64_sve_extvnx2df */
    case 4096:  /* *aarch64_sve_extvnx4sf */
    case 4095:  /* *aarch64_sve_extvnx8hf */
    case 4094:  /* *aarch64_sve_extvnx2di */
    case 4093:  /* *aarch64_sve_extvnx4si */
    case 4092:  /* *aarch64_sve_extvnx8hi */
    case 4091:  /* *aarch64_sve_extvnx16qi */
    case 3606:  /* aarch64_sm3partw2qv4si */
    case 3605:  /* aarch64_sm3partw1qv4si */
    case 3600:  /* aarch64_sm3ss1qv4si */
    case 3589:  /* aarch64_crypto_sha512su1qv2di */
    case 3587:  /* aarch64_crypto_sha512h2qv2di */
    case 3586:  /* aarch64_crypto_sha512hqv2di */
    case 3585:  /* aarch64_crypto_sha256su1v4si */
    case 3583:  /* aarch64_crypto_sha256h2v4si */
    case 3582:  /* aarch64_crypto_sha256hv4si */
    case 3581:  /* aarch64_crypto_sha1su0v4si */
    case 3580:  /* aarch64_crypto_sha1pv4si */
    case 3579:  /* aarch64_crypto_sha1mv4si */
    case 3578:  /* aarch64_crypto_sha1cv4si */
    case 3459:  /* aarch64_extv2df */
    case 3458:  /* aarch64_extv4sf */
    case 3457:  /* aarch64_extv2sf */
    case 3456:  /* aarch64_extv8hf */
    case 3455:  /* aarch64_extv4hf */
    case 3454:  /* aarch64_extv2di */
    case 3453:  /* aarch64_extv4si */
    case 3452:  /* aarch64_extv2si */
    case 3451:  /* aarch64_extv8hi */
    case 3450:  /* aarch64_extv4hi */
    case 3449:  /* aarch64_extv16qi */
    case 3448:  /* aarch64_extv8qi */
    case 3374:  /* aarch64_qtbx4v16qi */
    case 3373:  /* aarch64_qtbx4v8qi */
    case 3370:  /* aarch64_qtbx3v16qi */
    case 3369:  /* aarch64_qtbx3v8qi */
    case 3366:  /* aarch64_tbx4v16qi */
    case 3365:  /* aarch64_tbx4v8qi */
    case 3240:  /* aarch64_vec_load_lanesxi_lanedf */
    case 3239:  /* aarch64_vec_load_lanesxi_lanedi */
    case 3238:  /* aarch64_vec_load_lanesxi_lanev2df */
    case 3237:  /* aarch64_vec_load_lanesxi_lanev4sf */
    case 3236:  /* aarch64_vec_load_lanesxi_lanev2sf */
    case 3235:  /* aarch64_vec_load_lanesxi_lanev8hf */
    case 3234:  /* aarch64_vec_load_lanesxi_lanev4hf */
    case 3233:  /* aarch64_vec_load_lanesxi_lanev2di */
    case 3232:  /* aarch64_vec_load_lanesxi_lanev4si */
    case 3231:  /* aarch64_vec_load_lanesxi_lanev2si */
    case 3230:  /* aarch64_vec_load_lanesxi_lanev8hi */
    case 3229:  /* aarch64_vec_load_lanesxi_lanev4hi */
    case 3228:  /* aarch64_vec_load_lanesxi_lanev16qi */
    case 3227:  /* aarch64_vec_load_lanesxi_lanev8qi */
    case 3184:  /* aarch64_vec_load_lanesci_lanedf */
    case 3183:  /* aarch64_vec_load_lanesci_lanedi */
    case 3182:  /* aarch64_vec_load_lanesci_lanev2df */
    case 3181:  /* aarch64_vec_load_lanesci_lanev4sf */
    case 3180:  /* aarch64_vec_load_lanesci_lanev2sf */
    case 3179:  /* aarch64_vec_load_lanesci_lanev8hf */
    case 3178:  /* aarch64_vec_load_lanesci_lanev4hf */
    case 3177:  /* aarch64_vec_load_lanesci_lanev2di */
    case 3176:  /* aarch64_vec_load_lanesci_lanev4si */
    case 3175:  /* aarch64_vec_load_lanesci_lanev2si */
    case 3174:  /* aarch64_vec_load_lanesci_lanev8hi */
    case 3173:  /* aarch64_vec_load_lanesci_lanev4hi */
    case 3172:  /* aarch64_vec_load_lanesci_lanev16qi */
    case 3171:  /* aarch64_vec_load_lanesci_lanev8qi */
    case 3128:  /* aarch64_vec_load_lanesoi_lanedf */
    case 3127:  /* aarch64_vec_load_lanesoi_lanedi */
    case 3126:  /* aarch64_vec_load_lanesoi_lanev2df */
    case 3125:  /* aarch64_vec_load_lanesoi_lanev4sf */
    case 3124:  /* aarch64_vec_load_lanesoi_lanev2sf */
    case 3123:  /* aarch64_vec_load_lanesoi_lanev8hf */
    case 3122:  /* aarch64_vec_load_lanesoi_lanev4hf */
    case 3121:  /* aarch64_vec_load_lanesoi_lanev2di */
    case 3120:  /* aarch64_vec_load_lanesoi_lanev4si */
    case 3119:  /* aarch64_vec_load_lanesoi_lanev2si */
    case 3118:  /* aarch64_vec_load_lanesoi_lanev8hi */
    case 3117:  /* aarch64_vec_load_lanesoi_lanev4hi */
    case 3116:  /* aarch64_vec_load_lanesoi_lanev16qi */
    case 3115:  /* aarch64_vec_load_lanesoi_lanev8qi */
    case 2853:  /* aarch64_usri_ndi */
    case 2852:  /* aarch64_ssri_ndi */
    case 2851:  /* aarch64_usli_ndi */
    case 2850:  /* aarch64_ssli_ndi */
    case 2849:  /* aarch64_usri_nv2di */
    case 2848:  /* aarch64_ssri_nv2di */
    case 2847:  /* aarch64_usli_nv2di */
    case 2846:  /* aarch64_ssli_nv2di */
    case 2845:  /* aarch64_usri_nv4si */
    case 2844:  /* aarch64_ssri_nv4si */
    case 2843:  /* aarch64_usli_nv4si */
    case 2842:  /* aarch64_ssli_nv4si */
    case 2841:  /* aarch64_usri_nv2si */
    case 2840:  /* aarch64_ssri_nv2si */
    case 2839:  /* aarch64_usli_nv2si */
    case 2838:  /* aarch64_ssli_nv2si */
    case 2837:  /* aarch64_usri_nv8hi */
    case 2836:  /* aarch64_ssri_nv8hi */
    case 2835:  /* aarch64_usli_nv8hi */
    case 2834:  /* aarch64_ssli_nv8hi */
    case 2833:  /* aarch64_usri_nv4hi */
    case 2832:  /* aarch64_ssri_nv4hi */
    case 2831:  /* aarch64_usli_nv4hi */
    case 2830:  /* aarch64_ssli_nv4hi */
    case 2829:  /* aarch64_usri_nv16qi */
    case 2828:  /* aarch64_ssri_nv16qi */
    case 2827:  /* aarch64_usli_nv16qi */
    case 2826:  /* aarch64_ssli_nv16qi */
    case 2825:  /* aarch64_usri_nv8qi */
    case 2824:  /* aarch64_ssri_nv8qi */
    case 2823:  /* aarch64_usli_nv8qi */
    case 2822:  /* aarch64_ssli_nv8qi */
    case 2821:  /* aarch64_ursra_ndi */
    case 2820:  /* aarch64_srsra_ndi */
    case 2819:  /* aarch64_usra_ndi */
    case 2818:  /* aarch64_ssra_ndi */
    case 2817:  /* aarch64_ursra_nv2di */
    case 2816:  /* aarch64_srsra_nv2di */
    case 2815:  /* aarch64_usra_nv2di */
    case 2814:  /* aarch64_ssra_nv2di */
    case 2813:  /* aarch64_ursra_nv4si */
    case 2812:  /* aarch64_srsra_nv4si */
    case 2811:  /* aarch64_usra_nv4si */
    case 2810:  /* aarch64_ssra_nv4si */
    case 2809:  /* aarch64_ursra_nv2si */
    case 2808:  /* aarch64_srsra_nv2si */
    case 2807:  /* aarch64_usra_nv2si */
    case 2806:  /* aarch64_ssra_nv2si */
    case 2805:  /* aarch64_ursra_nv8hi */
    case 2804:  /* aarch64_srsra_nv8hi */
    case 2803:  /* aarch64_usra_nv8hi */
    case 2802:  /* aarch64_ssra_nv8hi */
    case 2801:  /* aarch64_ursra_nv4hi */
    case 2800:  /* aarch64_srsra_nv4hi */
    case 2799:  /* aarch64_usra_nv4hi */
    case 2798:  /* aarch64_ssra_nv4hi */
    case 2797:  /* aarch64_ursra_nv16qi */
    case 2796:  /* aarch64_srsra_nv16qi */
    case 2795:  /* aarch64_usra_nv16qi */
    case 2794:  /* aarch64_ssra_nv16qi */
    case 2793:  /* aarch64_ursra_nv8qi */
    case 2792:  /* aarch64_srsra_nv8qi */
    case 2791:  /* aarch64_usra_nv8qi */
    case 2790:  /* aarch64_ssra_nv8qi */
    case 2595:  /* aarch64_sqrdmlshsi */
    case 2594:  /* aarch64_sqrdmlahsi */
    case 2593:  /* aarch64_sqrdmlshhi */
    case 2592:  /* aarch64_sqrdmlahhi */
    case 2591:  /* aarch64_sqrdmlshv4si */
    case 2590:  /* aarch64_sqrdmlahv4si */
    case 2589:  /* aarch64_sqrdmlshv2si */
    case 2588:  /* aarch64_sqrdmlahv2si */
    case 2587:  /* aarch64_sqrdmlshv8hi */
    case 2586:  /* aarch64_sqrdmlahv8hi */
    case 2585:  /* aarch64_sqrdmlshv4hi */
    case 2584:  /* aarch64_sqrdmlahv4hi */
    case 2418:  /* aarch64_rsubhn2v2di */
    case 2417:  /* aarch64_subhn2v2di */
    case 2416:  /* aarch64_raddhn2v2di */
    case 2415:  /* aarch64_addhn2v2di */
    case 2414:  /* aarch64_rsubhn2v4si */
    case 2413:  /* aarch64_subhn2v4si */
    case 2412:  /* aarch64_raddhn2v4si */
    case 2411:  /* aarch64_addhn2v4si */
    case 2410:  /* aarch64_rsubhn2v8hi */
    case 2409:  /* aarch64_subhn2v8hi */
    case 2408:  /* aarch64_raddhn2v8hi */
    case 2407:  /* aarch64_addhn2v8hi */
    case 1552:  /* aarch64_uabalv4si_4 */
    case 1551:  /* aarch64_sabalv4si_4 */
    case 1550:  /* aarch64_uabalv8hi_4 */
    case 1549:  /* aarch64_sabalv8hi_4 */
    case 1548:  /* aarch64_uabalv4hi_4 */
    case 1547:  /* aarch64_sabalv4hi_4 */
    case 1546:  /* aarch64_uabalv16qi_4 */
    case 1545:  /* aarch64_sabalv16qi_4 */
    case 1544:  /* aarch64_uabalv8qi_4 */
    case 1543:  /* aarch64_sabalv8qi_4 */
    case 1025:  /* copysigndf3_insn */
    case 1024:  /* copysignsf3_insn */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (pat, 1), 0, 0));
      ro[2] = *(ro_loc[2] = &XVECEXP (XEXP (pat, 1), 0, 1));
      ro[3] = *(ro_loc[3] = &XVECEXP (XEXP (pat, 1), 0, 2));
      break;

    case 930:  /* *aarch64_fnmadddf4 */
    case 929:  /* *aarch64_fnmaddsf4 */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 0), 2));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (pat, 1), 0), 1));
      break;

    case 928:  /* *aarch64_fnmsdf4 */
    case 927:  /* *aarch64_fnmssf4 */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 2), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (pat, 1), 1));
      break;

    case 926:  /* *aarch64_fmsdf4 */
    case 925:  /* *aarch64_fmssf4 */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 2), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (pat, 1), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (pat, 1), 1));
      break;

    case 924:  /* *aarch64_fnmadf4 */
    case 923:  /* *aarch64_fnmasf4 */
    case 922:  /* *aarch64_fnmahf4 */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 2));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (pat, 1), 1));
      break;

    case 921:  /* *aarch64_fmadf4 */
    case 920:  /* *aarch64_fmasf4 */
    case 919:  /* *aarch64_fmahf4 */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 2));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (pat, 1), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (pat, 1), 1));
      break;

    case 2048:  /* lfrintnuhfhi2 */
    case 2047:  /* lrounduhfhi2 */
    case 2046:  /* lflooruhfhi2 */
    case 2045:  /* lceiluhfhi2 */
    case 2044:  /* lbtruncuhfhi2 */
    case 2043:  /* lfrintnhfhi2 */
    case 2042:  /* lroundhfhi2 */
    case 2041:  /* lfloorhfhi2 */
    case 2040:  /* lceilhfhi2 */
    case 2039:  /* lbtrunchfhi2 */
    case 2038:  /* lfrintnuv2dfv2di2 */
    case 2037:  /* lrounduv2dfv2di2 */
    case 2036:  /* lflooruv2dfv2di2 */
    case 2035:  /* lceiluv2dfv2di2 */
    case 2034:  /* lbtruncuv2dfv2di2 */
    case 2033:  /* lfrintnv2dfv2di2 */
    case 2032:  /* lroundv2dfv2di2 */
    case 2031:  /* lfloorv2dfv2di2 */
    case 2030:  /* lceilv2dfv2di2 */
    case 2029:  /* lbtruncv2dfv2di2 */
    case 2028:  /* lfrintnuv4sfv4si2 */
    case 2027:  /* lrounduv4sfv4si2 */
    case 2026:  /* lflooruv4sfv4si2 */
    case 2025:  /* lceiluv4sfv4si2 */
    case 2024:  /* lbtruncuv4sfv4si2 */
    case 2023:  /* lfrintnv4sfv4si2 */
    case 2022:  /* lroundv4sfv4si2 */
    case 2021:  /* lfloorv4sfv4si2 */
    case 2020:  /* lceilv4sfv4si2 */
    case 2019:  /* lbtruncv4sfv4si2 */
    case 2018:  /* lfrintnuv2sfv2si2 */
    case 2017:  /* lrounduv2sfv2si2 */
    case 2016:  /* lflooruv2sfv2si2 */
    case 2015:  /* lceiluv2sfv2si2 */
    case 2014:  /* lbtruncuv2sfv2si2 */
    case 2013:  /* lfrintnv2sfv2si2 */
    case 2012:  /* lroundv2sfv2si2 */
    case 2011:  /* lfloorv2sfv2si2 */
    case 2010:  /* lceilv2sfv2si2 */
    case 2009:  /* lbtruncv2sfv2si2 */
    case 2008:  /* lfrintnuv8hfv8hi2 */
    case 2007:  /* lrounduv8hfv8hi2 */
    case 2006:  /* lflooruv8hfv8hi2 */
    case 2005:  /* lceiluv8hfv8hi2 */
    case 2004:  /* lbtruncuv8hfv8hi2 */
    case 2003:  /* lfrintnv8hfv8hi2 */
    case 2002:  /* lroundv8hfv8hi2 */
    case 2001:  /* lfloorv8hfv8hi2 */
    case 2000:  /* lceilv8hfv8hi2 */
    case 1999:  /* lbtruncv8hfv8hi2 */
    case 1998:  /* lfrintnuv4hfv4hi2 */
    case 1997:  /* lrounduv4hfv4hi2 */
    case 1996:  /* lflooruv4hfv4hi2 */
    case 1995:  /* lceiluv4hfv4hi2 */
    case 1994:  /* lbtruncuv4hfv4hi2 */
    case 1993:  /* lfrintnv4hfv4hi2 */
    case 1992:  /* lroundv4hfv4hi2 */
    case 1991:  /* lfloorv4hfv4hi2 */
    case 1990:  /* lceilv4hfv4hi2 */
    case 1989:  /* lbtruncv4hfv4hi2 */
    case 1049:  /* tlsie_small_sidi */
    case 910:  /* lfrintnudfdi2 */
    case 909:  /* lroundudfdi2 */
    case 908:  /* lfloorudfdi2 */
    case 907:  /* lceiludfdi2 */
    case 906:  /* lbtruncudfdi2 */
    case 905:  /* lfrintndfdi2 */
    case 904:  /* lrounddfdi2 */
    case 903:  /* lfloordfdi2 */
    case 902:  /* lceildfdi2 */
    case 901:  /* lbtruncdfdi2 */
    case 900:  /* lfrintnudfsi2 */
    case 899:  /* lroundudfsi2 */
    case 898:  /* lfloorudfsi2 */
    case 897:  /* lceiludfsi2 */
    case 896:  /* lbtruncudfsi2 */
    case 895:  /* lfrintndfsi2 */
    case 894:  /* lrounddfsi2 */
    case 893:  /* lfloordfsi2 */
    case 892:  /* lceildfsi2 */
    case 891:  /* lbtruncdfsi2 */
    case 890:  /* lfrintnusfdi2 */
    case 889:  /* lroundusfdi2 */
    case 888:  /* lfloorusfdi2 */
    case 887:  /* lceilusfdi2 */
    case 886:  /* lbtruncusfdi2 */
    case 885:  /* lfrintnsfdi2 */
    case 884:  /* lroundsfdi2 */
    case 883:  /* lfloorsfdi2 */
    case 882:  /* lceilsfdi2 */
    case 881:  /* lbtruncsfdi2 */
    case 880:  /* lfrintnusfsi2 */
    case 879:  /* lroundusfsi2 */
    case 878:  /* lfloorusfsi2 */
    case 877:  /* lceilusfsi2 */
    case 876:  /* lbtruncusfsi2 */
    case 875:  /* lfrintnsfsi2 */
    case 874:  /* lroundsfsi2 */
    case 873:  /* lfloorsfsi2 */
    case 872:  /* lceilsfsi2 */
    case 871:  /* lbtruncsfsi2 */
    case 870:  /* lfrintnuhfdi2 */
    case 869:  /* lrounduhfdi2 */
    case 868:  /* lflooruhfdi2 */
    case 867:  /* lceiluhfdi2 */
    case 866:  /* lbtruncuhfdi2 */
    case 865:  /* lfrintnhfdi2 */
    case 864:  /* lroundhfdi2 */
    case 863:  /* lfloorhfdi2 */
    case 862:  /* lceilhfdi2 */
    case 861:  /* lbtrunchfdi2 */
    case 860:  /* lfrintnuhfsi2 */
    case 859:  /* lrounduhfsi2 */
    case 858:  /* lflooruhfsi2 */
    case 857:  /* lceiluhfsi2 */
    case 856:  /* lbtruncuhfsi2 */
    case 855:  /* lfrintnhfsi2 */
    case 854:  /* lroundhfsi2 */
    case 853:  /* lfloorhfsi2 */
    case 852:  /* lceilhfsi2 */
    case 851:  /* lbtrunchfsi2 */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (XEXP (pat, 1), 0), 0, 0));
      break;

    case 828:  /* rev16di2_alt */
    case 827:  /* rev16si2_alt */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (pat, 1), 0), 1));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (pat, 1), 1), 1));
      recog_data.dup_loc[0] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 0);
      recog_data.dup_num[0] = 1;
      break;

    case 826:  /* rev16di2 */
    case 825:  /* rev16si2 */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (pat, 1), 1), 1));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (pat, 1), 0), 1));
      recog_data.dup_loc[0] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 0);
      recog_data.dup_num[0] = 1;
      break;

    case 802:  /* *extr_insv_lower_regdi */
    case 801:  /* *extr_insv_lower_regsi */
      ro[0] = *(ro_loc[0] = &XEXP (XEXP (pat, 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 0), 1));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (pat, 1), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (pat, 1), 2));
      recog_data.dup_loc[0] = &XEXP (XEXP (pat, 1), 1);
      recog_data.dup_num[0] = 1;
      break;

    case 800:  /* *aarch64_bfidi4_noshift_alt */
    case 799:  /* *aarch64_bfisi4_noshift_alt */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 1), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (pat, 1), 1), 1));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (XEXP (pat, 1), 0), 1));
      break;

    case 798:  /* *aarch64_bfidi4_noshift */
    case 797:  /* *aarch64_bfisi4_noshift */
    case 796:  /* *aarch64_bfidi4_noand_alt */
    case 795:  /* *aarch64_bfisi4_noand_alt */
    case 794:  /* *aarch64_bfidi4_noand */
    case 793:  /* *aarch64_bfisi4_noand */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (pat, 1), 0), 1));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (pat, 1), 1), 0));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (XEXP (pat, 1), 1), 1));
      break;

    case 792:  /* *aarch64_bfidi5_shift_alt */
    case 791:  /* *aarch64_bfisi5_shift_alt */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 1));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (pat, 1), 0), 1));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (XEXP (pat, 1), 1), 0));
      ro[5] = *(ro_loc[5] = &XEXP (XEXP (XEXP (pat, 1), 1), 1));
      break;

    case 790:  /* *aarch64_bfidi5_shift */
    case 789:  /* *aarch64_bfisi5_shift */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (pat, 1), 0), 1));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 0));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 1));
      ro[5] = *(ro_loc[5] = &XEXP (XEXP (XEXP (pat, 1), 1), 1));
      break;

    case 788:  /* *aarch64_bfidisi4 */
    case 787:  /* *aarch64_bfisisi4 */
    case 786:  /* *aarch64_bfidihi4 */
    case 785:  /* *aarch64_bfisihi4 */
    case 784:  /* *aarch64_bfidiqi4 */
    case 783:  /* *aarch64_bfisiqi4 */
      ro[0] = *(ro_loc[0] = &XEXP (XEXP (pat, 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 0), 1));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (pat, 0), 2));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (pat, 1), 0));
      break;

    case 782:  /* *insv_regdi */
    case 781:  /* *insv_regsi */
      ro[0] = *(ro_loc[0] = &XEXP (XEXP (pat, 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 0), 1));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (pat, 0), 2));
      ro[3] = *(ro_loc[3] = &XEXP (pat, 1));
      break;

    case 1918:  /* fmav2df4 */
    case 1917:  /* fmav4sf4 */
    case 1916:  /* fmav2sf4 */
    case 1915:  /* fmav8hf4 */
    case 1914:  /* fmav4hf4 */
    case 780:  /* *extzvdi */
    case 779:  /* *extvdi */
    case 778:  /* *extzvsi */
    case 777:  /* *extvsi */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (pat, 1), 1));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (pat, 1), 2));
      break;

    case 757:  /* *extrsi5_insn_uxtw_alt */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 1), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 1), 1));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 1));
      break;

    case 824:  /* *aarch64_bfxilsi_uxtw */
    case 756:  /* *extrsi5_insn_uxtw */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 1), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 1));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 1), 1));
      break;

    case 755:  /* *extrdi5_insn_alt */
    case 754:  /* *extrsi5_insn_alt */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 1), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (pat, 1), 1), 1));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (XEXP (pat, 1), 0), 1));
      break;

    case 823:  /* *aarch64_bfxildi */
    case 822:  /* *aarch64_bfxilsi */
    case 753:  /* *extrdi5_insn */
    case 752:  /* *extrsi5_insn */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (pat, 1), 1), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (pat, 1), 0), 1));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (XEXP (pat, 1), 1), 1));
      break;

    case 1709:  /* aarch64_mlsv4si */
    case 1708:  /* aarch64_mlsv2si */
    case 1707:  /* aarch64_mlsv8hi */
    case 1706:  /* aarch64_mlsv4hi */
    case 1705:  /* aarch64_mlsv16qi */
    case 1704:  /* aarch64_mlsv8qi */
    case 728:  /* *aarch64_lshr_reg_minusdi3 */
    case 727:  /* *aarch64_ashr_reg_minusdi3 */
    case 726:  /* *aarch64_ashl_reg_minusdi3 */
    case 725:  /* *aarch64_lshr_reg_minussi3 */
    case 724:  /* *aarch64_ashr_reg_minussi3 */
    case 723:  /* *aarch64_ashl_reg_minussi3 */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (pat, 1), 1), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (pat, 1), 1), 1));
      break;

    case 718:  /* *aarch64_ashl_reg_di3_minus_mask */
    case 717:  /* *aarch64_ashl_reg_si3_minus_mask */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (pat, 1), 1), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 1), 0), 0));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 1), 0), 1));
      ro[5] = *(ro_loc[5] = &XEXP (XEXP (XEXP (pat, 1), 1), 1));
      break;

    case 716:  /* *aarch64_rotr_reg_di3_neg_mask2 */
    case 715:  /* *aarch64_lshr_reg_di3_neg_mask2 */
    case 714:  /* *aarch64_ashr_reg_di3_neg_mask2 */
    case 713:  /* *aarch64_ashl_reg_di3_neg_mask2 */
    case 712:  /* *aarch64_rotr_reg_si3_neg_mask2 */
    case 711:  /* *aarch64_lshr_reg_si3_neg_mask2 */
    case 710:  /* *aarch64_ashr_reg_si3_neg_mask2 */
    case 709:  /* *aarch64_ashl_reg_si3_neg_mask2 */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 0), 1));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (pat, 1), 1));
      break;

    case 722:  /* *aarch64_rotr_reg_di3_mask2 */
    case 721:  /* *aarch64_lshr_reg_di3_mask2 */
    case 720:  /* *aarch64_ashr_reg_di3_mask2 */
    case 719:  /* *aarch64_ashl_reg_di3_mask2 */
    case 708:  /* *aarch64_rotr_reg_di3_mask1 */
    case 707:  /* *aarch64_lshr_reg_di3_mask1 */
    case 706:  /* *aarch64_ashr_reg_di3_mask1 */
    case 705:  /* *aarch64_ashl_reg_di3_mask1 */
    case 704:  /* *aarch64_rotr_reg_si3_mask1 */
    case 703:  /* *aarch64_lshr_reg_si3_mask1 */
    case 702:  /* *aarch64_ashr_reg_si3_mask1 */
    case 701:  /* *aarch64_ashl_reg_si3_mask1 */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 1));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (pat, 1), 1));
      break;

    case 700:  /* *and_rotrdi3nr_compare0 */
    case 699:  /* *and_lshrdi3nr_compare0 */
    case 698:  /* *and_ashrdi3nr_compare0 */
    case 697:  /* *and_ashldi3nr_compare0 */
    case 696:  /* *and_rotrsi3nr_compare0 */
    case 695:  /* *and_lshrsi3nr_compare0 */
    case 694:  /* *and_ashrsi3nr_compare0 */
    case 693:  /* *and_ashlsi3nr_compare0 */
      ro[0] = *(ro_loc[0] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 1));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (pat, 1), 0), 1));
      break;

    case 692:  /* *anddi3nr_compare0_zextract */
    case 691:  /* *andsi3nr_compare0_zextract */
      ro[0] = *(ro_loc[0] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 0), 1));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (pat, 1), 0), 2));
      break;

    case 4997:  /* aarch64_sve_uunpklo_vnx4si */
    case 4996:  /* aarch64_sve_sunpklo_vnx4si */
    case 4995:  /* aarch64_sve_uunpkhi_vnx4si */
    case 4994:  /* aarch64_sve_sunpkhi_vnx4si */
    case 4993:  /* aarch64_sve_uunpklo_vnx8hi */
    case 4992:  /* aarch64_sve_sunpklo_vnx8hi */
    case 4991:  /* aarch64_sve_uunpkhi_vnx8hi */
    case 4990:  /* aarch64_sve_sunpkhi_vnx8hi */
    case 4989:  /* aarch64_sve_uunpklo_vnx16qi */
    case 4988:  /* aarch64_sve_sunpklo_vnx16qi */
    case 4987:  /* aarch64_sve_uunpkhi_vnx16qi */
    case 4986:  /* aarch64_sve_sunpkhi_vnx16qi */
    case 4985:  /* aarch64_sve_punpkhi_vnx4bi */
    case 4984:  /* aarch64_sve_punpklo_vnx4bi */
    case 4983:  /* aarch64_sve_punpkhi_vnx8bi */
    case 4982:  /* aarch64_sve_punpklo_vnx8bi */
    case 4981:  /* aarch64_sve_punpkhi_vnx16bi */
    case 4980:  /* aarch64_sve_punpklo_vnx16bi */
    case 4083:  /* *aarch64_sve_revvnx2df */
    case 4082:  /* *aarch64_sve_revvnx4sf */
    case 4081:  /* *aarch64_sve_revvnx8hf */
    case 4080:  /* *aarch64_sve_revvnx2di */
    case 4079:  /* *aarch64_sve_revvnx4si */
    case 4078:  /* *aarch64_sve_revvnx8hi */
    case 4077:  /* *aarch64_sve_revvnx16qi */
    case 3574:  /* aarch64_crypto_sha1hsi */
    case 3571:  /* aarch64_crypto_aesimcv16qi */
    case 3570:  /* aarch64_crypto_aesmcv16qi */
    case 3563:  /* aarch64_urecpev4si */
    case 3562:  /* aarch64_urecpev2si */
    case 3553:  /* aarch64_frecpxdf */
    case 3552:  /* aarch64_frecpxsf */
    case 3551:  /* aarch64_frecpxhf */
    case 3550:  /* aarch64_frecpedf */
    case 3549:  /* aarch64_frecpesf */
    case 3548:  /* aarch64_frecpehf */
    case 3547:  /* aarch64_frecpev2df */
    case 3546:  /* aarch64_frecpev4sf */
    case 3545:  /* aarch64_frecpev2sf */
    case 3544:  /* aarch64_frecpev8hf */
    case 3543:  /* aarch64_frecpev4hf */
    case 3542:  /* aarch64_simd_ld1df_x2 */
    case 3541:  /* aarch64_simd_ld1di_x2 */
    case 3540:  /* aarch64_simd_ld1v2sf_x2 */
    case 3539:  /* aarch64_simd_ld1v2si_x2 */
    case 3538:  /* aarch64_simd_ld1v4hf_x2 */
    case 3537:  /* aarch64_simd_ld1v4hi_x2 */
    case 3536:  /* aarch64_simd_ld1v8qi_x2 */
    case 3535:  /* aarch64_simd_ld1v2df_x2 */
    case 3534:  /* aarch64_simd_ld1v4sf_x2 */
    case 3533:  /* aarch64_simd_ld1v8hf_x2 */
    case 3532:  /* aarch64_simd_ld1v2di_x2 */
    case 3531:  /* aarch64_simd_ld1v4si_x2 */
    case 3530:  /* aarch64_simd_ld1v8hi_x2 */
    case 3529:  /* aarch64_simd_ld1v16qi_x2 */
    case 3516:  /* aarch64_st4df_dreg */
    case 3515:  /* aarch64_st4di_dreg */
    case 3514:  /* aarch64_st4v2sf_dreg */
    case 3513:  /* aarch64_st4v2si_dreg */
    case 3512:  /* aarch64_st4v4hf_dreg */
    case 3511:  /* aarch64_st4v4hi_dreg */
    case 3510:  /* aarch64_st4v8qi_dreg */
    case 3509:  /* aarch64_st3df_dreg */
    case 3508:  /* aarch64_st3di_dreg */
    case 3507:  /* aarch64_st3v2sf_dreg */
    case 3506:  /* aarch64_st3v2si_dreg */
    case 3505:  /* aarch64_st3v4hf_dreg */
    case 3504:  /* aarch64_st3v4hi_dreg */
    case 3503:  /* aarch64_st3v8qi_dreg */
    case 3502:  /* aarch64_st2df_dreg */
    case 3501:  /* aarch64_st2di_dreg */
    case 3500:  /* aarch64_st2v2sf_dreg */
    case 3499:  /* aarch64_st2v2si_dreg */
    case 3498:  /* aarch64_st2v4hf_dreg */
    case 3497:  /* aarch64_st2v4hi_dreg */
    case 3496:  /* aarch64_st2v8qi_dreg */
    case 3495:  /* aarch64_rev16v2df */
    case 3494:  /* aarch64_rev32v2df */
    case 3493:  /* aarch64_rev64v2df */
    case 3492:  /* aarch64_rev16v4sf */
    case 3491:  /* aarch64_rev32v4sf */
    case 3490:  /* aarch64_rev64v4sf */
    case 3489:  /* aarch64_rev16v2sf */
    case 3488:  /* aarch64_rev32v2sf */
    case 3487:  /* aarch64_rev64v2sf */
    case 3486:  /* aarch64_rev16v8hf */
    case 3485:  /* aarch64_rev32v8hf */
    case 3484:  /* aarch64_rev64v8hf */
    case 3483:  /* aarch64_rev16v4hf */
    case 3482:  /* aarch64_rev32v4hf */
    case 3481:  /* aarch64_rev64v4hf */
    case 3480:  /* aarch64_rev16v2di */
    case 3479:  /* aarch64_rev32v2di */
    case 3478:  /* aarch64_rev64v2di */
    case 3477:  /* aarch64_rev16v4si */
    case 3476:  /* aarch64_rev32v4si */
    case 3475:  /* aarch64_rev64v4si */
    case 3474:  /* aarch64_rev16v2si */
    case 3473:  /* aarch64_rev32v2si */
    case 3472:  /* aarch64_rev64v2si */
    case 3471:  /* aarch64_rev16v8hi */
    case 3470:  /* aarch64_rev32v8hi */
    case 3469:  /* aarch64_rev64v8hi */
    case 3468:  /* aarch64_rev16v4hi */
    case 3467:  /* aarch64_rev32v4hi */
    case 3466:  /* aarch64_rev64v4hi */
    case 3465:  /* aarch64_rev16v16qi */
    case 3464:  /* aarch64_rev32v16qi */
    case 3463:  /* aarch64_rev64v16qi */
    case 3462:  /* aarch64_rev16v8qi */
    case 3461:  /* aarch64_rev32v8qi */
    case 3460:  /* aarch64_rev64v8qi */
    case 3359:  /* aarch64_ld4df_dreg */
    case 3358:  /* aarch64_ld4di_dreg */
    case 3357:  /* aarch64_ld4v2sf_dreg */
    case 3356:  /* aarch64_ld4v2si_dreg */
    case 3355:  /* aarch64_ld4v4hf_dreg */
    case 3354:  /* aarch64_ld4v4hi_dreg */
    case 3353:  /* aarch64_ld4v8qi_dreg */
    case 3352:  /* aarch64_ld3df_dreg */
    case 3351:  /* aarch64_ld3di_dreg */
    case 3350:  /* aarch64_ld3v2sf_dreg */
    case 3349:  /* aarch64_ld3v2si_dreg */
    case 3348:  /* aarch64_ld3v4hf_dreg */
    case 3347:  /* aarch64_ld3v4hi_dreg */
    case 3346:  /* aarch64_ld3v8qi_dreg */
    case 3345:  /* aarch64_ld2df_dreg */
    case 3344:  /* aarch64_ld2di_dreg */
    case 3343:  /* aarch64_ld2v2sf_dreg */
    case 3342:  /* aarch64_ld2v2si_dreg */
    case 3341:  /* aarch64_ld2v4hf_dreg */
    case 3340:  /* aarch64_ld2v4hi_dreg */
    case 3339:  /* aarch64_ld2v8qi_dreg */
    case 3335:  /* aarch64_be_st1di */
    case 3334:  /* aarch64_be_st1v2df */
    case 3333:  /* aarch64_be_st1v4sf */
    case 3332:  /* aarch64_be_st1v2sf */
    case 3331:  /* aarch64_be_st1v8hf */
    case 3330:  /* aarch64_be_st1v4hf */
    case 3329:  /* aarch64_be_st1v2di */
    case 3328:  /* aarch64_be_st1v4si */
    case 3327:  /* aarch64_be_st1v2si */
    case 3326:  /* aarch64_be_st1v8hi */
    case 3325:  /* aarch64_be_st1v4hi */
    case 3324:  /* aarch64_be_st1v16qi */
    case 3323:  /* aarch64_be_st1v8qi */
    case 3322:  /* aarch64_be_ld1di */
    case 3321:  /* aarch64_be_ld1v2df */
    case 3320:  /* aarch64_be_ld1v4sf */
    case 3319:  /* aarch64_be_ld1v2sf */
    case 3318:  /* aarch64_be_ld1v8hf */
    case 3317:  /* aarch64_be_ld1v4hf */
    case 3316:  /* aarch64_be_ld1v2di */
    case 3315:  /* aarch64_be_ld1v4si */
    case 3314:  /* aarch64_be_ld1v2si */
    case 3313:  /* aarch64_be_ld1v8hi */
    case 3312:  /* aarch64_be_ld1v4hi */
    case 3311:  /* aarch64_be_ld1v16qi */
    case 3310:  /* aarch64_be_ld1v8qi */
    case 3306:  /* aarch64_st1_x3_df */
    case 3305:  /* aarch64_st1_x3_di */
    case 3304:  /* aarch64_st1_x3_v2df */
    case 3303:  /* aarch64_st1_x3_v4sf */
    case 3302:  /* aarch64_st1_x3_v2sf */
    case 3301:  /* aarch64_st1_x3_v8hf */
    case 3300:  /* aarch64_st1_x3_v4hf */
    case 3299:  /* aarch64_st1_x3_v2di */
    case 3298:  /* aarch64_st1_x3_v4si */
    case 3297:  /* aarch64_st1_x3_v2si */
    case 3296:  /* aarch64_st1_x3_v8hi */
    case 3295:  /* aarch64_st1_x3_v4hi */
    case 3294:  /* aarch64_st1_x3_v16qi */
    case 3293:  /* aarch64_st1_x3_v8qi */
    case 3292:  /* aarch64_st1_x2_df */
    case 3291:  /* aarch64_st1_x2_di */
    case 3290:  /* aarch64_st1_x2_v2df */
    case 3289:  /* aarch64_st1_x2_v4sf */
    case 3288:  /* aarch64_st1_x2_v2sf */
    case 3287:  /* aarch64_st1_x2_v8hf */
    case 3286:  /* aarch64_st1_x2_v4hf */
    case 3285:  /* aarch64_st1_x2_v2di */
    case 3284:  /* aarch64_st1_x2_v4si */
    case 3283:  /* aarch64_st1_x2_v2si */
    case 3282:  /* aarch64_st1_x2_v8hi */
    case 3281:  /* aarch64_st1_x2_v4hi */
    case 3280:  /* aarch64_st1_x2_v16qi */
    case 3279:  /* aarch64_st1_x2_v8qi */
    case 3278:  /* aarch64_ld1_x3_df */
    case 3277:  /* aarch64_ld1_x3_di */
    case 3276:  /* aarch64_ld1_x3_v2df */
    case 3275:  /* aarch64_ld1_x3_v4sf */
    case 3274:  /* aarch64_ld1_x3_v2sf */
    case 3273:  /* aarch64_ld1_x3_v8hf */
    case 3272:  /* aarch64_ld1_x3_v4hf */
    case 3271:  /* aarch64_ld1_x3_v2di */
    case 3270:  /* aarch64_ld1_x3_v4si */
    case 3269:  /* aarch64_ld1_x3_v2si */
    case 3268:  /* aarch64_ld1_x3_v8hi */
    case 3267:  /* aarch64_ld1_x3_v4hi */
    case 3266:  /* aarch64_ld1_x3_v16qi */
    case 3265:  /* aarch64_ld1_x3_v8qi */
    case 3247:  /* aarch64_simd_st4v2df */
    case 3246:  /* aarch64_simd_st4v4sf */
    case 3245:  /* aarch64_simd_st4v8hf */
    case 3244:  /* aarch64_simd_st4v2di */
    case 3243:  /* aarch64_simd_st4v4si */
    case 3242:  /* aarch64_simd_st4v8hi */
    case 3241:  /* aarch64_simd_st4v16qi */
    case 3226:  /* aarch64_simd_ld4rdf */
    case 3225:  /* aarch64_simd_ld4rdi */
    case 3224:  /* aarch64_simd_ld4rv2df */
    case 3223:  /* aarch64_simd_ld4rv4sf */
    case 3222:  /* aarch64_simd_ld4rv2sf */
    case 3221:  /* aarch64_simd_ld4rv8hf */
    case 3220:  /* aarch64_simd_ld4rv4hf */
    case 3219:  /* aarch64_simd_ld4rv2di */
    case 3218:  /* aarch64_simd_ld4rv4si */
    case 3217:  /* aarch64_simd_ld4rv2si */
    case 3216:  /* aarch64_simd_ld4rv8hi */
    case 3215:  /* aarch64_simd_ld4rv4hi */
    case 3214:  /* aarch64_simd_ld4rv16qi */
    case 3213:  /* aarch64_simd_ld4rv8qi */
    case 3212:  /* aarch64_simd_ld4v2df */
    case 3211:  /* aarch64_simd_ld4v4sf */
    case 3210:  /* aarch64_simd_ld4v8hf */
    case 3209:  /* aarch64_simd_ld4v2di */
    case 3208:  /* aarch64_simd_ld4v4si */
    case 3207:  /* aarch64_simd_ld4v8hi */
    case 3206:  /* aarch64_simd_ld4v16qi */
    case 3191:  /* aarch64_simd_st3v2df */
    case 3190:  /* aarch64_simd_st3v4sf */
    case 3189:  /* aarch64_simd_st3v8hf */
    case 3188:  /* aarch64_simd_st3v2di */
    case 3187:  /* aarch64_simd_st3v4si */
    case 3186:  /* aarch64_simd_st3v8hi */
    case 3185:  /* aarch64_simd_st3v16qi */
    case 3170:  /* aarch64_simd_ld3rdf */
    case 3169:  /* aarch64_simd_ld3rdi */
    case 3168:  /* aarch64_simd_ld3rv2df */
    case 3167:  /* aarch64_simd_ld3rv4sf */
    case 3166:  /* aarch64_simd_ld3rv2sf */
    case 3165:  /* aarch64_simd_ld3rv8hf */
    case 3164:  /* aarch64_simd_ld3rv4hf */
    case 3163:  /* aarch64_simd_ld3rv2di */
    case 3162:  /* aarch64_simd_ld3rv4si */
    case 3161:  /* aarch64_simd_ld3rv2si */
    case 3160:  /* aarch64_simd_ld3rv8hi */
    case 3159:  /* aarch64_simd_ld3rv4hi */
    case 3158:  /* aarch64_simd_ld3rv16qi */
    case 3157:  /* aarch64_simd_ld3rv8qi */
    case 3156:  /* aarch64_simd_ld3v2df */
    case 3155:  /* aarch64_simd_ld3v4sf */
    case 3154:  /* aarch64_simd_ld3v8hf */
    case 3153:  /* aarch64_simd_ld3v2di */
    case 3152:  /* aarch64_simd_ld3v4si */
    case 3151:  /* aarch64_simd_ld3v8hi */
    case 3150:  /* aarch64_simd_ld3v16qi */
    case 3135:  /* aarch64_simd_st2v2df */
    case 3134:  /* aarch64_simd_st2v4sf */
    case 3133:  /* aarch64_simd_st2v8hf */
    case 3132:  /* aarch64_simd_st2v2di */
    case 3131:  /* aarch64_simd_st2v4si */
    case 3130:  /* aarch64_simd_st2v8hi */
    case 3129:  /* aarch64_simd_st2v16qi */
    case 3114:  /* aarch64_simd_ld2rdf */
    case 3113:  /* aarch64_simd_ld2rdi */
    case 3112:  /* aarch64_simd_ld2rv2df */
    case 3111:  /* aarch64_simd_ld2rv4sf */
    case 3110:  /* aarch64_simd_ld2rv2sf */
    case 3109:  /* aarch64_simd_ld2rv8hf */
    case 3108:  /* aarch64_simd_ld2rv4hf */
    case 3107:  /* aarch64_simd_ld2rv2di */
    case 3106:  /* aarch64_simd_ld2rv4si */
    case 3105:  /* aarch64_simd_ld2rv2si */
    case 3104:  /* aarch64_simd_ld2rv8hi */
    case 3103:  /* aarch64_simd_ld2rv4hi */
    case 3102:  /* aarch64_simd_ld2rv16qi */
    case 3101:  /* aarch64_simd_ld2rv8qi */
    case 3100:  /* aarch64_simd_ld2v2df */
    case 3099:  /* aarch64_simd_ld2v4sf */
    case 3098:  /* aarch64_simd_ld2v8hf */
    case 3097:  /* aarch64_simd_ld2v2di */
    case 3096:  /* aarch64_simd_ld2v4si */
    case 3095:  /* aarch64_simd_ld2v8hi */
    case 3094:  /* aarch64_simd_ld2v16qi */
    case 3088:  /* aarch64_addpdi */
    case 2525:  /* aarch64_uqmovndi */
    case 2524:  /* aarch64_sqmovndi */
    case 2523:  /* aarch64_uqmovnsi */
    case 2522:  /* aarch64_sqmovnsi */
    case 2521:  /* aarch64_uqmovnhi */
    case 2520:  /* aarch64_sqmovnhi */
    case 2519:  /* aarch64_uqmovnv2di */
    case 2518:  /* aarch64_sqmovnv2di */
    case 2517:  /* aarch64_uqmovnv4si */
    case 2516:  /* aarch64_sqmovnv4si */
    case 2515:  /* aarch64_uqmovnv8hi */
    case 2514:  /* aarch64_sqmovnv8hi */
    case 2513:  /* aarch64_sqmovundi */
    case 2512:  /* aarch64_sqmovunsi */
    case 2511:  /* aarch64_sqmovunhi */
    case 2510:  /* aarch64_sqmovunv2di */
    case 2509:  /* aarch64_sqmovunv4si */
    case 2508:  /* aarch64_sqmovunv8hi */
    case 2202:  /* aarch64_reduc_smin_internalv2df */
    case 2201:  /* aarch64_reduc_smax_internalv2df */
    case 2200:  /* aarch64_reduc_smin_nan_internalv2df */
    case 2199:  /* aarch64_reduc_smax_nan_internalv2df */
    case 2198:  /* aarch64_reduc_smin_internalv4sf */
    case 2197:  /* aarch64_reduc_smax_internalv4sf */
    case 2196:  /* aarch64_reduc_smin_nan_internalv4sf */
    case 2195:  /* aarch64_reduc_smax_nan_internalv4sf */
    case 2194:  /* aarch64_reduc_smin_internalv2sf */
    case 2193:  /* aarch64_reduc_smax_internalv2sf */
    case 2192:  /* aarch64_reduc_smin_nan_internalv2sf */
    case 2191:  /* aarch64_reduc_smax_nan_internalv2sf */
    case 2190:  /* aarch64_reduc_smin_internalv8hf */
    case 2189:  /* aarch64_reduc_smax_internalv8hf */
    case 2188:  /* aarch64_reduc_smin_nan_internalv8hf */
    case 2187:  /* aarch64_reduc_smax_nan_internalv8hf */
    case 2186:  /* aarch64_reduc_smin_internalv4hf */
    case 2185:  /* aarch64_reduc_smax_internalv4hf */
    case 2184:  /* aarch64_reduc_smin_nan_internalv4hf */
    case 2183:  /* aarch64_reduc_smax_nan_internalv4hf */
    case 2182:  /* aarch64_reduc_smin_internalv2si */
    case 2181:  /* aarch64_reduc_smax_internalv2si */
    case 2180:  /* aarch64_reduc_umin_internalv2si */
    case 2179:  /* aarch64_reduc_umax_internalv2si */
    case 2178:  /* aarch64_reduc_smin_internalv4si */
    case 2177:  /* aarch64_reduc_smax_internalv4si */
    case 2176:  /* aarch64_reduc_umin_internalv4si */
    case 2175:  /* aarch64_reduc_umax_internalv4si */
    case 2174:  /* aarch64_reduc_smin_internalv8hi */
    case 2173:  /* aarch64_reduc_smax_internalv8hi */
    case 2172:  /* aarch64_reduc_umin_internalv8hi */
    case 2171:  /* aarch64_reduc_umax_internalv8hi */
    case 2170:  /* aarch64_reduc_smin_internalv4hi */
    case 2169:  /* aarch64_reduc_smax_internalv4hi */
    case 2168:  /* aarch64_reduc_umin_internalv4hi */
    case 2167:  /* aarch64_reduc_umax_internalv4hi */
    case 2166:  /* aarch64_reduc_smin_internalv16qi */
    case 2165:  /* aarch64_reduc_smax_internalv16qi */
    case 2164:  /* aarch64_reduc_umin_internalv16qi */
    case 2163:  /* aarch64_reduc_umax_internalv16qi */
    case 2162:  /* aarch64_reduc_smin_internalv8qi */
    case 2161:  /* aarch64_reduc_smax_internalv8qi */
    case 2160:  /* aarch64_reduc_umin_internalv8qi */
    case 2159:  /* aarch64_reduc_umax_internalv8qi */
    case 2144:  /* reduc_plus_scal_v2df */
    case 2143:  /* reduc_plus_scal_v2sf */
    case 2142:  /* aarch64_reduc_plus_internalv2si */
    case 2141:  /* aarch64_reduc_plus_internalv2di */
    case 2140:  /* aarch64_reduc_plus_internalv4si */
    case 2139:  /* aarch64_reduc_plus_internalv8hi */
    case 2138:  /* aarch64_reduc_plus_internalv4hi */
    case 2137:  /* aarch64_reduc_plus_internalv16qi */
    case 2136:  /* aarch64_reduc_plus_internalv8qi */
    case 1988:  /* roundv2df2 */
    case 1987:  /* rintv2df2 */
    case 1986:  /* nearbyintv2df2 */
    case 1985:  /* frintnv2df2 */
    case 1984:  /* floorv2df2 */
    case 1983:  /* ceilv2df2 */
    case 1982:  /* btruncv2df2 */
    case 1981:  /* roundv4sf2 */
    case 1980:  /* rintv4sf2 */
    case 1979:  /* nearbyintv4sf2 */
    case 1978:  /* frintnv4sf2 */
    case 1977:  /* floorv4sf2 */
    case 1976:  /* ceilv4sf2 */
    case 1975:  /* btruncv4sf2 */
    case 1974:  /* roundv2sf2 */
    case 1973:  /* rintv2sf2 */
    case 1972:  /* nearbyintv2sf2 */
    case 1971:  /* frintnv2sf2 */
    case 1970:  /* floorv2sf2 */
    case 1969:  /* ceilv2sf2 */
    case 1968:  /* btruncv2sf2 */
    case 1967:  /* roundv8hf2 */
    case 1966:  /* rintv8hf2 */
    case 1965:  /* nearbyintv8hf2 */
    case 1964:  /* frintnv8hf2 */
    case 1963:  /* floorv8hf2 */
    case 1962:  /* ceilv8hf2 */
    case 1961:  /* btruncv8hf2 */
    case 1960:  /* roundv4hf2 */
    case 1959:  /* rintv4hf2 */
    case 1958:  /* nearbyintv4hf2 */
    case 1957:  /* frintnv4hf2 */
    case 1956:  /* floorv4hf2 */
    case 1955:  /* ceilv4hf2 */
    case 1954:  /* btruncv4hf2 */
    case 1520:  /* aarch64_absdi */
    case 1519:  /* aarch64_absv2di */
    case 1518:  /* aarch64_absv4si */
    case 1517:  /* aarch64_absv2si */
    case 1516:  /* aarch64_absv8hi */
    case 1515:  /* aarch64_absv4hi */
    case 1514:  /* aarch64_absv16qi */
    case 1513:  /* aarch64_absv8qi */
    case 1489:  /* aarch64_rsqrtedf */
    case 1488:  /* aarch64_rsqrtesf */
    case 1487:  /* aarch64_rsqrtehf */
    case 1486:  /* aarch64_rsqrtev2df */
    case 1485:  /* aarch64_rsqrtev4sf */
    case 1484:  /* aarch64_rsqrtev2sf */
    case 1483:  /* aarch64_rsqrtev8hf */
    case 1482:  /* aarch64_rsqrtev4hf */
    case 1383:  /* aarch64_rbitv16qi */
    case 1382:  /* aarch64_rbitv8qi */
    case 1060:  /* tlsle48_di */
    case 1059:  /* tlsle48_si */
    case 1058:  /* tlsle32_di */
    case 1057:  /* tlsle32_si */
    case 1048:  /* tlsie_small_di */
    case 1047:  /* tlsie_small_si */
    case 1043:  /* ldr_got_tiny */
    case 850:  /* rounddf2 */
    case 849:  /* rintdf2 */
    case 848:  /* nearbyintdf2 */
    case 847:  /* frintndf2 */
    case 846:  /* floordf2 */
    case 845:  /* ceildf2 */
    case 844:  /* btruncdf2 */
    case 843:  /* roundsf2 */
    case 842:  /* rintsf2 */
    case 841:  /* nearbyintsf2 */
    case 840:  /* frintnsf2 */
    case 839:  /* floorsf2 */
    case 838:  /* ceilsf2 */
    case 837:  /* btruncsf2 */
    case 836:  /* roundhf2 */
    case 835:  /* rinthf2 */
    case 834:  /* nearbyinthf2 */
    case 833:  /* frintnhf2 */
    case 832:  /* floorhf2 */
    case 831:  /* ceilhf2 */
    case 830:  /* btrunchf2 */
    case 739:  /* *aarch64_sisd_neg_qi */
    case 680:  /* rbitdi2 */
    case 679:  /* rbitsi2 */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (pat, 1), 0, 0));
      break;

    case 674:  /* *and_one_cmpl_rotrdi3_compare0_no_reuse */
    case 673:  /* *and_one_cmpl_lshrdi3_compare0_no_reuse */
    case 672:  /* *and_one_cmpl_ashrdi3_compare0_no_reuse */
    case 671:  /* *and_one_cmpl_ashldi3_compare0_no_reuse */
    case 670:  /* *and_one_cmpl_rotrsi3_compare0_no_reuse */
    case 669:  /* *and_one_cmpl_lshrsi3_compare0_no_reuse */
    case 668:  /* *and_one_cmpl_ashrsi3_compare0_no_reuse */
    case 667:  /* *and_one_cmpl_ashlsi3_compare0_no_reuse */
      ro[0] = *(ro_loc[0] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0), 1));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (pat, 1), 0), 1));
      break;

    case 666:  /* *and_one_cmpl_rotrsi3_compare0_uxtw */
    case 665:  /* *and_one_cmpl_lshrsi3_compare0_uxtw */
    case 664:  /* *and_one_cmpl_ashrsi3_compare0_uxtw */
    case 663:  /* *and_one_cmpl_ashlsi3_compare0_uxtw */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0), 0), 1));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 1));
      recog_data.dup_loc[0] = &XEXP (XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0), 0), 0), 0);
      recog_data.dup_num[0] = 1;
      recog_data.dup_loc[1] = &XEXP (XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0), 0), 0), 1);
      recog_data.dup_num[1] = 2;
      recog_data.dup_loc[2] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0), 1);
      recog_data.dup_num[2] = 3;
      break;

    case 662:  /* *and_one_cmpl_rotrdi3_compare0 */
    case 661:  /* *and_one_cmpl_lshrdi3_compare0 */
    case 660:  /* *and_one_cmpl_ashrdi3_compare0 */
    case 659:  /* *and_one_cmpl_ashldi3_compare0 */
    case 658:  /* *and_one_cmpl_rotrsi3_compare0 */
    case 657:  /* *and_one_cmpl_lshrsi3_compare0 */
    case 656:  /* *and_one_cmpl_ashrsi3_compare0 */
    case 655:  /* *and_one_cmpl_ashlsi3_compare0 */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0), 0), 1));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 1));
      recog_data.dup_loc[0] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0), 0), 0);
      recog_data.dup_num[0] = 1;
      recog_data.dup_loc[1] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0), 0), 1);
      recog_data.dup_num[1] = 2;
      recog_data.dup_loc[2] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 1);
      recog_data.dup_num[2] = 3;
      break;

    case 654:  /* *eor_one_cmpl_rotrsidi3_alt_ze */
    case 653:  /* *eor_one_cmpl_lshrsidi3_alt_ze */
    case 652:  /* *eor_one_cmpl_ashrsidi3_alt_ze */
    case 651:  /* *eor_one_cmpl_ashlsidi3_alt_ze */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0), 1));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 1));
      break;

    case 1568:  /* abav4si_3 */
    case 1567:  /* abav2si_3 */
    case 1566:  /* abav8hi_3 */
    case 1565:  /* abav4hi_3 */
    case 1564:  /* abav16qi_3 */
    case 1563:  /* abav8qi_3 */
    case 642:  /* xor_one_cmpl_rotrdi3 */
    case 641:  /* ior_one_cmpl_rotrdi3 */
    case 640:  /* and_one_cmpl_rotrdi3 */
    case 639:  /* xor_one_cmpl_lshrdi3 */
    case 638:  /* ior_one_cmpl_lshrdi3 */
    case 637:  /* and_one_cmpl_lshrdi3 */
    case 636:  /* xor_one_cmpl_ashrdi3 */
    case 635:  /* ior_one_cmpl_ashrdi3 */
    case 634:  /* and_one_cmpl_ashrdi3 */
    case 633:  /* xor_one_cmpl_ashldi3 */
    case 632:  /* ior_one_cmpl_ashldi3 */
    case 631:  /* and_one_cmpl_ashldi3 */
    case 630:  /* xor_one_cmpl_rotrsi3 */
    case 629:  /* ior_one_cmpl_rotrsi3 */
    case 628:  /* and_one_cmpl_rotrsi3 */
    case 627:  /* xor_one_cmpl_lshrsi3 */
    case 626:  /* ior_one_cmpl_lshrsi3 */
    case 625:  /* and_one_cmpl_lshrsi3 */
    case 624:  /* xor_one_cmpl_ashrsi3 */
    case 623:  /* ior_one_cmpl_ashrsi3 */
    case 622:  /* and_one_cmpl_ashrsi3 */
    case 621:  /* xor_one_cmpl_ashlsi3 */
    case 620:  /* ior_one_cmpl_ashlsi3 */
    case 619:  /* and_one_cmpl_ashlsi3 */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 1));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (pat, 1), 1));
      break;

    case 618:  /* *and_one_cmpldi3_compare0_no_reuse */
    case 617:  /* *and_one_cmplsi3_compare0_no_reuse */
      ro[0] = *(ro_loc[0] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 0), 1));
      break;

    case 616:  /* *and_one_cmplsi3_compare0_uxtw */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 1));
      recog_data.dup_loc[0] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0), 0), 0);
      recog_data.dup_num[0] = 1;
      recog_data.dup_loc[1] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0), 1);
      recog_data.dup_num[1] = 2;
      break;

    case 549:  /* *and_rotrsi3_compare0_uxtw */
    case 548:  /* *and_lshrsi3_compare0_uxtw */
    case 547:  /* *and_ashrsi3_compare0_uxtw */
    case 546:  /* *and_ashlsi3_compare0_uxtw */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0), 1));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 1));
      recog_data.dup_loc[0] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0), 0), 0);
      recog_data.dup_num[0] = 1;
      recog_data.dup_loc[1] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0), 0), 1);
      recog_data.dup_num[1] = 2;
      recog_data.dup_loc[2] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0), 1);
      recog_data.dup_num[2] = 3;
      break;

    case 523:  /* aarch64_uqdecdi */
    case 522:  /* aarch64_uqdecsi */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (pat, 1), 0), 1));
      recog_data.dup_loc[0] = &XEXP (XEXP (pat, 1), 1);
      recog_data.dup_num[0] = 2;
      break;

    case 519:  /* csneg3_uxtw_insn */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 1), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (pat, 1), 0), 2));
      break;

    case 521:  /* csneg3di_insn */
    case 520:  /* csneg3si_insn */
    case 518:  /* *csinv3di_insn */
    case 517:  /* *csinv3si_insn */
    case 516:  /* csinc3di_insn */
    case 515:  /* csinc3si_insn */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (pat, 1), 1), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (pat, 1), 2));
      break;

    case 2274:  /* *aarch64_combinez_bedf */
    case 2273:  /* *aarch64_combinez_bedi */
    case 2272:  /* *aarch64_combinez_bev2sf */
    case 2271:  /* *aarch64_combinez_bev2si */
    case 2270:  /* *aarch64_combinez_bev4hf */
    case 2269:  /* *aarch64_combinez_bev4hi */
    case 2268:  /* *aarch64_combinez_bev8qi */
    case 514:  /* *csinc2di_insn */
    case 513:  /* *csinc2si_insn */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 1));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (pat, 1), 0));
      break;

    case 5136:  /* vec_shl_insert_vnx2df */
    case 5135:  /* vec_shl_insert_vnx4sf */
    case 5134:  /* vec_shl_insert_vnx8hf */
    case 5133:  /* vec_shl_insert_vnx2di */
    case 5132:  /* vec_shl_insert_vnx4si */
    case 5131:  /* vec_shl_insert_vnx8hi */
    case 5130:  /* vec_shl_insert_vnx16qi */
    case 5003:  /* vec_pack_trunc_vnx2di */
    case 5002:  /* vec_pack_trunc_vnx4si */
    case 5001:  /* vec_pack_trunc_vnx8hi */
    case 5000:  /* vec_pack_trunc_vnx2bi */
    case 4999:  /* vec_pack_trunc_vnx4bi */
    case 4998:  /* vec_pack_trunc_vnx8bi */
    case 4878:  /* *reduc_xor_scal_vnx2di */
    case 4877:  /* *reduc_ior_scal_vnx2di */
    case 4876:  /* *reduc_and_scal_vnx2di */
    case 4875:  /* *reduc_xor_scal_vnx4si */
    case 4874:  /* *reduc_ior_scal_vnx4si */
    case 4873:  /* *reduc_and_scal_vnx4si */
    case 4872:  /* *reduc_xor_scal_vnx8hi */
    case 4871:  /* *reduc_ior_scal_vnx8hi */
    case 4870:  /* *reduc_and_scal_vnx8hi */
    case 4869:  /* *reduc_xor_scal_vnx16qi */
    case 4868:  /* *reduc_ior_scal_vnx16qi */
    case 4867:  /* *reduc_and_scal_vnx16qi */
    case 4866:  /* *reduc_smin_scal_vnx2df */
    case 4865:  /* *reduc_smax_scal_vnx2df */
    case 4864:  /* *reduc_smin_nan_scal_vnx2df */
    case 4863:  /* *reduc_smax_nan_scal_vnx2df */
    case 4862:  /* *reduc_smin_scal_vnx4sf */
    case 4861:  /* *reduc_smax_scal_vnx4sf */
    case 4860:  /* *reduc_smin_nan_scal_vnx4sf */
    case 4859:  /* *reduc_smax_nan_scal_vnx4sf */
    case 4858:  /* *reduc_smin_scal_vnx8hf */
    case 4857:  /* *reduc_smax_scal_vnx8hf */
    case 4856:  /* *reduc_smin_nan_scal_vnx8hf */
    case 4855:  /* *reduc_smax_nan_scal_vnx8hf */
    case 4854:  /* *reduc_smin_scal_vnx2di */
    case 4853:  /* *reduc_smax_scal_vnx2di */
    case 4852:  /* *reduc_umin_scal_vnx2di */
    case 4851:  /* *reduc_umax_scal_vnx2di */
    case 4850:  /* *reduc_smin_scal_vnx4si */
    case 4849:  /* *reduc_smax_scal_vnx4si */
    case 4848:  /* *reduc_umin_scal_vnx4si */
    case 4847:  /* *reduc_umax_scal_vnx4si */
    case 4846:  /* *reduc_smin_scal_vnx8hi */
    case 4845:  /* *reduc_smax_scal_vnx8hi */
    case 4844:  /* *reduc_umin_scal_vnx8hi */
    case 4843:  /* *reduc_umax_scal_vnx8hi */
    case 4842:  /* *reduc_smin_scal_vnx16qi */
    case 4841:  /* *reduc_smax_scal_vnx16qi */
    case 4840:  /* *reduc_umin_scal_vnx16qi */
    case 4839:  /* *reduc_umax_scal_vnx16qi */
    case 4838:  /* *reduc_plus_scal_vnx2df */
    case 4837:  /* *reduc_plus_scal_vnx4sf */
    case 4836:  /* *reduc_plus_scal_vnx8hf */
    case 4835:  /* *reduc_plus_scal_vnx2di */
    case 4834:  /* *reduc_plus_scal_vnx4si */
    case 4833:  /* *reduc_plus_scal_vnx8hi */
    case 4832:  /* *reduc_plus_scal_vnx16qi */
    case 4170:  /* *xorvnx2df3 */
    case 4169:  /* *iorvnx2df3 */
    case 4168:  /* *andvnx2df3 */
    case 4167:  /* *xorvnx4sf3 */
    case 4166:  /* *iorvnx4sf3 */
    case 4165:  /* *andvnx4sf3 */
    case 4164:  /* *xorvnx8hf3 */
    case 4163:  /* *iorvnx8hf3 */
    case 4162:  /* *andvnx8hf3 */
    case 4067:  /* aarch64_sve_uzp2vnx2df */
    case 4066:  /* aarch64_sve_uzp1vnx2df */
    case 4065:  /* aarch64_sve_trn2vnx2df */
    case 4064:  /* aarch64_sve_trn1vnx2df */
    case 4063:  /* aarch64_sve_zip2vnx2df */
    case 4062:  /* aarch64_sve_zip1vnx2df */
    case 4061:  /* aarch64_sve_uzp2vnx4sf */
    case 4060:  /* aarch64_sve_uzp1vnx4sf */
    case 4059:  /* aarch64_sve_trn2vnx4sf */
    case 4058:  /* aarch64_sve_trn1vnx4sf */
    case 4057:  /* aarch64_sve_zip2vnx4sf */
    case 4056:  /* aarch64_sve_zip1vnx4sf */
    case 4055:  /* aarch64_sve_uzp2vnx8hf */
    case 4054:  /* aarch64_sve_uzp1vnx8hf */
    case 4053:  /* aarch64_sve_trn2vnx8hf */
    case 4052:  /* aarch64_sve_trn1vnx8hf */
    case 4051:  /* aarch64_sve_zip2vnx8hf */
    case 4050:  /* aarch64_sve_zip1vnx8hf */
    case 4049:  /* aarch64_sve_uzp2vnx2di */
    case 4048:  /* aarch64_sve_uzp1vnx2di */
    case 4047:  /* aarch64_sve_trn2vnx2di */
    case 4046:  /* aarch64_sve_trn1vnx2di */
    case 4045:  /* aarch64_sve_zip2vnx2di */
    case 4044:  /* aarch64_sve_zip1vnx2di */
    case 4043:  /* aarch64_sve_uzp2vnx4si */
    case 4042:  /* aarch64_sve_uzp1vnx4si */
    case 4041:  /* aarch64_sve_trn2vnx4si */
    case 4040:  /* aarch64_sve_trn1vnx4si */
    case 4039:  /* aarch64_sve_zip2vnx4si */
    case 4038:  /* aarch64_sve_zip1vnx4si */
    case 4037:  /* aarch64_sve_uzp2vnx8hi */
    case 4036:  /* aarch64_sve_uzp1vnx8hi */
    case 4035:  /* aarch64_sve_trn2vnx8hi */
    case 4034:  /* aarch64_sve_trn1vnx8hi */
    case 4033:  /* aarch64_sve_zip2vnx8hi */
    case 4032:  /* aarch64_sve_zip1vnx8hi */
    case 4031:  /* aarch64_sve_uzp2vnx16qi */
    case 4030:  /* aarch64_sve_uzp1vnx16qi */
    case 4029:  /* aarch64_sve_trn2vnx16qi */
    case 4028:  /* aarch64_sve_trn1vnx16qi */
    case 4027:  /* aarch64_sve_zip2vnx16qi */
    case 4026:  /* aarch64_sve_zip1vnx16qi */
    case 4025:  /* *aarch64_sve_uzp2vnx2bi */
    case 4024:  /* *aarch64_sve_uzp1vnx2bi */
    case 4023:  /* *aarch64_sve_trn2vnx2bi */
    case 4022:  /* *aarch64_sve_trn1vnx2bi */
    case 4021:  /* *aarch64_sve_zip2vnx2bi */
    case 4020:  /* *aarch64_sve_zip1vnx2bi */
    case 4019:  /* *aarch64_sve_uzp2vnx4bi */
    case 4018:  /* *aarch64_sve_uzp1vnx4bi */
    case 4017:  /* *aarch64_sve_trn2vnx4bi */
    case 4016:  /* *aarch64_sve_trn1vnx4bi */
    case 4015:  /* *aarch64_sve_zip2vnx4bi */
    case 4014:  /* *aarch64_sve_zip1vnx4bi */
    case 4013:  /* *aarch64_sve_uzp2vnx8bi */
    case 4012:  /* *aarch64_sve_uzp1vnx8bi */
    case 4011:  /* *aarch64_sve_trn2vnx8bi */
    case 4010:  /* *aarch64_sve_trn1vnx8bi */
    case 4009:  /* *aarch64_sve_zip2vnx8bi */
    case 4008:  /* *aarch64_sve_zip1vnx8bi */
    case 4007:  /* *aarch64_sve_uzp2vnx16bi */
    case 4006:  /* *aarch64_sve_uzp1vnx16bi */
    case 4005:  /* *aarch64_sve_trn2vnx16bi */
    case 4004:  /* *aarch64_sve_trn1vnx16bi */
    case 4003:  /* *aarch64_sve_zip2vnx16bi */
    case 4002:  /* *aarch64_sve_zip1vnx16bi */
    case 4001:  /* *aarch64_sve_tblvnx2df */
    case 4000:  /* *aarch64_sve_tblvnx4sf */
    case 3999:  /* *aarch64_sve_tblvnx8hf */
    case 3998:  /* *aarch64_sve_tblvnx2di */
    case 3997:  /* *aarch64_sve_tblvnx4si */
    case 3996:  /* *aarch64_sve_tblvnx8hi */
    case 3995:  /* *aarch64_sve_tblvnx16qi */
    case 3944:  /* *sve_ld1rqd */
    case 3943:  /* *sve_ld1rqw */
    case 3942:  /* *sve_ld1rqh */
    case 3941:  /* *sve_ld1rqd */
    case 3940:  /* *sve_ld1rqw */
    case 3939:  /* *sve_ld1rqh */
    case 3938:  /* *sve_ld1rqb */
    case 3923:  /* extract_last_vnx2df */
    case 3922:  /* extract_last_vnx4sf */
    case 3921:  /* extract_last_vnx8hf */
    case 3920:  /* extract_last_vnx2di */
    case 3919:  /* extract_last_vnx4si */
    case 3918:  /* extract_last_vnx8hi */
    case 3917:  /* extract_last_vnx16qi */
    case 3884:  /* aarch64_pred_movvnx8df */
    case 3883:  /* aarch64_pred_movvnx16sf */
    case 3882:  /* aarch64_pred_movvnx32hf */
    case 3881:  /* aarch64_pred_movvnx8di */
    case 3880:  /* aarch64_pred_movvnx16si */
    case 3879:  /* aarch64_pred_movvnx32hi */
    case 3878:  /* aarch64_pred_movvnx64qi */
    case 3877:  /* aarch64_pred_movvnx6df */
    case 3876:  /* aarch64_pred_movvnx12sf */
    case 3875:  /* aarch64_pred_movvnx24hf */
    case 3874:  /* aarch64_pred_movvnx6di */
    case 3873:  /* aarch64_pred_movvnx12si */
    case 3872:  /* aarch64_pred_movvnx24hi */
    case 3871:  /* aarch64_pred_movvnx48qi */
    case 3870:  /* aarch64_pred_movvnx4df */
    case 3869:  /* aarch64_pred_movvnx8sf */
    case 3868:  /* aarch64_pred_movvnx16hf */
    case 3867:  /* aarch64_pred_movvnx4di */
    case 3866:  /* aarch64_pred_movvnx8si */
    case 3865:  /* aarch64_pred_movvnx16hi */
    case 3864:  /* aarch64_pred_movvnx32qi */
    case 3799:  /* aarch64_pred_movvnx2df */
    case 3798:  /* aarch64_pred_movvnx4sf */
    case 3797:  /* aarch64_pred_movvnx8hf */
    case 3796:  /* aarch64_pred_movvnx2di */
    case 3795:  /* aarch64_pred_movvnx4si */
    case 3794:  /* aarch64_pred_movvnx8hi */
    case 3793:  /* aarch64_pred_movvnx16qi */
    case 3778:  /* *aarch64_sve_movvnx2df_subreg_be */
    case 3777:  /* *aarch64_sve_movvnx4sf_subreg_be */
    case 3776:  /* *aarch64_sve_movvnx8hf_subreg_be */
    case 3775:  /* *aarch64_sve_movvnx2di_subreg_be */
    case 3774:  /* *aarch64_sve_movvnx4si_subreg_be */
    case 3773:  /* *aarch64_sve_movvnx8hi_subreg_be */
    case 3772:  /* *aarch64_sve_movvnx16qi_subreg_be */
    case 3766:  /* aarch64_load_exclusivedi */
    case 3765:  /* aarch64_load_exclusivesi */
    case 3762:  /* atomic_storedi */
    case 3761:  /* atomic_storesi */
    case 3760:  /* atomic_storehi */
    case 3759:  /* atomic_storeqi */
    case 3758:  /* atomic_loaddi */
    case 3757:  /* atomic_loadsi */
    case 3756:  /* atomic_loadhi */
    case 3755:  /* atomic_loadqi */
    case 3634:  /* aarch64_crypto_pmullv2di */
    case 3633:  /* aarch64_crypto_pmulldi */
    case 3608:  /* aarch64_sm4ekeyqv4si */
    case 3607:  /* aarch64_sm4eqv4si */
    case 3588:  /* aarch64_crypto_sha512su0qv2di */
    case 3584:  /* aarch64_crypto_sha256su0v4si */
    case 3577:  /* aarch64_crypto_sha1su1v4si */
    case 3565:  /* aarch64_crypto_aesdv16qi */
    case 3564:  /* aarch64_crypto_aesev16qi */
    case 3561:  /* aarch64_frecpsdf */
    case 3560:  /* aarch64_frecpssf */
    case 3559:  /* aarch64_frecpshf */
    case 3558:  /* aarch64_frecpsv2df */
    case 3557:  /* aarch64_frecpsv4sf */
    case 3556:  /* aarch64_frecpsv2sf */
    case 3555:  /* aarch64_frecpsv8hf */
    case 3554:  /* aarch64_frecpsv4hf */
    case 3447:  /* aarch64_uzp2v2df */
    case 3446:  /* aarch64_uzp1v2df */
    case 3445:  /* aarch64_trn2v2df */
    case 3444:  /* aarch64_trn1v2df */
    case 3443:  /* aarch64_zip2v2df */
    case 3442:  /* aarch64_zip1v2df */
    case 3441:  /* aarch64_uzp2v4sf */
    case 3440:  /* aarch64_uzp1v4sf */
    case 3439:  /* aarch64_trn2v4sf */
    case 3438:  /* aarch64_trn1v4sf */
    case 3437:  /* aarch64_zip2v4sf */
    case 3436:  /* aarch64_zip1v4sf */
    case 3435:  /* aarch64_uzp2v2sf */
    case 3434:  /* aarch64_uzp1v2sf */
    case 3433:  /* aarch64_trn2v2sf */
    case 3432:  /* aarch64_trn1v2sf */
    case 3431:  /* aarch64_zip2v2sf */
    case 3430:  /* aarch64_zip1v2sf */
    case 3429:  /* aarch64_uzp2v8hf */
    case 3428:  /* aarch64_uzp1v8hf */
    case 3427:  /* aarch64_trn2v8hf */
    case 3426:  /* aarch64_trn1v8hf */
    case 3425:  /* aarch64_zip2v8hf */
    case 3424:  /* aarch64_zip1v8hf */
    case 3423:  /* aarch64_uzp2v4hf */
    case 3422:  /* aarch64_uzp1v4hf */
    case 3421:  /* aarch64_trn2v4hf */
    case 3420:  /* aarch64_trn1v4hf */
    case 3419:  /* aarch64_zip2v4hf */
    case 3418:  /* aarch64_zip1v4hf */
    case 3417:  /* aarch64_uzp2v2di */
    case 3416:  /* aarch64_uzp1v2di */
    case 3415:  /* aarch64_trn2v2di */
    case 3414:  /* aarch64_trn1v2di */
    case 3413:  /* aarch64_zip2v2di */
    case 3412:  /* aarch64_zip1v2di */
    case 3411:  /* aarch64_uzp2v4si */
    case 3410:  /* aarch64_uzp1v4si */
    case 3409:  /* aarch64_trn2v4si */
    case 3408:  /* aarch64_trn1v4si */
    case 3407:  /* aarch64_zip2v4si */
    case 3406:  /* aarch64_zip1v4si */
    case 3405:  /* aarch64_uzp2v2si */
    case 3404:  /* aarch64_uzp1v2si */
    case 3403:  /* aarch64_trn2v2si */
    case 3402:  /* aarch64_trn1v2si */
    case 3401:  /* aarch64_zip2v2si */
    case 3400:  /* aarch64_zip1v2si */
    case 3399:  /* aarch64_uzp2v8hi */
    case 3398:  /* aarch64_uzp1v8hi */
    case 3397:  /* aarch64_trn2v8hi */
    case 3396:  /* aarch64_trn1v8hi */
    case 3395:  /* aarch64_zip2v8hi */
    case 3394:  /* aarch64_zip1v8hi */
    case 3393:  /* aarch64_uzp2v4hi */
    case 3392:  /* aarch64_uzp1v4hi */
    case 3391:  /* aarch64_trn2v4hi */
    case 3390:  /* aarch64_trn1v4hi */
    case 3389:  /* aarch64_zip2v4hi */
    case 3388:  /* aarch64_zip1v4hi */
    case 3387:  /* aarch64_uzp2v16qi */
    case 3386:  /* aarch64_uzp1v16qi */
    case 3385:  /* aarch64_trn2v16qi */
    case 3384:  /* aarch64_trn1v16qi */
    case 3383:  /* aarch64_zip2v16qi */
    case 3382:  /* aarch64_zip1v16qi */
    case 3381:  /* aarch64_uzp2v8qi */
    case 3380:  /* aarch64_uzp1v8qi */
    case 3379:  /* aarch64_trn2v8qi */
    case 3378:  /* aarch64_trn1v8qi */
    case 3377:  /* aarch64_zip2v8qi */
    case 3376:  /* aarch64_zip1v8qi */
    case 3375:  /* aarch64_combinev16qi */
    case 3372:  /* aarch64_qtbl4v16qi */
    case 3371:  /* aarch64_qtbl4v8qi */
    case 3368:  /* aarch64_qtbl3v16qi */
    case 3367:  /* aarch64_qtbl3v8qi */
    case 3364:  /* aarch64_tbl3v16qi */
    case 3363:  /* aarch64_tbl3v8qi */
    case 3362:  /* aarch64_tbl2v16qi */
    case 3361:  /* aarch64_tbl1v16qi */
    case 3360:  /* aarch64_tbl1v8qi */
    case 3264:  /* aarch64_rev_reglistxi */
    case 3263:  /* aarch64_rev_reglistci */
    case 3262:  /* aarch64_rev_reglistoi */
    case 3087:  /* aarch64_addpv2si */
    case 3086:  /* aarch64_addpv4hi */
    case 3085:  /* aarch64_addpv8qi */
    case 2922:  /* aarch64_uqrshrn_ndi */
    case 2921:  /* aarch64_sqrshrn_ndi */
    case 2920:  /* aarch64_uqshrn_ndi */
    case 2919:  /* aarch64_sqshrn_ndi */
    case 2918:  /* aarch64_sqrshrun_ndi */
    case 2917:  /* aarch64_sqshrun_ndi */
    case 2916:  /* aarch64_uqrshrn_nsi */
    case 2915:  /* aarch64_sqrshrn_nsi */
    case 2914:  /* aarch64_uqshrn_nsi */
    case 2913:  /* aarch64_sqshrn_nsi */
    case 2912:  /* aarch64_sqrshrun_nsi */
    case 2911:  /* aarch64_sqshrun_nsi */
    case 2910:  /* aarch64_uqrshrn_nhi */
    case 2909:  /* aarch64_sqrshrn_nhi */
    case 2908:  /* aarch64_uqshrn_nhi */
    case 2907:  /* aarch64_sqshrn_nhi */
    case 2906:  /* aarch64_sqrshrun_nhi */
    case 2905:  /* aarch64_sqshrun_nhi */
    case 2904:  /* aarch64_uqrshrn_nv2di */
    case 2903:  /* aarch64_sqrshrn_nv2di */
    case 2902:  /* aarch64_uqshrn_nv2di */
    case 2901:  /* aarch64_sqshrn_nv2di */
    case 2900:  /* aarch64_sqrshrun_nv2di */
    case 2899:  /* aarch64_sqshrun_nv2di */
    case 2898:  /* aarch64_uqrshrn_nv4si */
    case 2897:  /* aarch64_sqrshrn_nv4si */
    case 2896:  /* aarch64_uqshrn_nv4si */
    case 2895:  /* aarch64_sqshrn_nv4si */
    case 2894:  /* aarch64_sqrshrun_nv4si */
    case 2893:  /* aarch64_sqshrun_nv4si */
    case 2892:  /* aarch64_uqrshrn_nv8hi */
    case 2891:  /* aarch64_sqrshrn_nv8hi */
    case 2890:  /* aarch64_uqshrn_nv8hi */
    case 2889:  /* aarch64_sqshrn_nv8hi */
    case 2888:  /* aarch64_sqrshrun_nv8hi */
    case 2887:  /* aarch64_sqshrun_nv8hi */
    case 2886:  /* aarch64_uqshl_ndi */
    case 2885:  /* aarch64_sqshl_ndi */
    case 2884:  /* aarch64_sqshlu_ndi */
    case 2883:  /* aarch64_uqshl_nsi */
    case 2882:  /* aarch64_sqshl_nsi */
    case 2881:  /* aarch64_sqshlu_nsi */
    case 2880:  /* aarch64_uqshl_nhi */
    case 2879:  /* aarch64_sqshl_nhi */
    case 2878:  /* aarch64_sqshlu_nhi */
    case 2877:  /* aarch64_uqshl_nqi */
    case 2876:  /* aarch64_sqshl_nqi */
    case 2875:  /* aarch64_sqshlu_nqi */
    case 2874:  /* aarch64_uqshl_nv2di */
    case 2873:  /* aarch64_sqshl_nv2di */
    case 2872:  /* aarch64_sqshlu_nv2di */
    case 2871:  /* aarch64_uqshl_nv4si */
    case 2870:  /* aarch64_sqshl_nv4si */
    case 2869:  /* aarch64_sqshlu_nv4si */
    case 2868:  /* aarch64_uqshl_nv2si */
    case 2867:  /* aarch64_sqshl_nv2si */
    case 2866:  /* aarch64_sqshlu_nv2si */
    case 2865:  /* aarch64_uqshl_nv8hi */
    case 2864:  /* aarch64_sqshl_nv8hi */
    case 2863:  /* aarch64_sqshlu_nv8hi */
    case 2862:  /* aarch64_uqshl_nv4hi */
    case 2861:  /* aarch64_sqshl_nv4hi */
    case 2860:  /* aarch64_sqshlu_nv4hi */
    case 2859:  /* aarch64_uqshl_nv16qi */
    case 2858:  /* aarch64_sqshl_nv16qi */
    case 2857:  /* aarch64_sqshlu_nv16qi */
    case 2856:  /* aarch64_uqshl_nv8qi */
    case 2855:  /* aarch64_sqshl_nv8qi */
    case 2854:  /* aarch64_sqshlu_nv8qi */
    case 2789:  /* aarch64_urshr_ndi */
    case 2788:  /* aarch64_srshr_ndi */
    case 2787:  /* aarch64_urshr_nv2di */
    case 2786:  /* aarch64_srshr_nv2di */
    case 2785:  /* aarch64_urshr_nv4si */
    case 2784:  /* aarch64_srshr_nv4si */
    case 2783:  /* aarch64_urshr_nv2si */
    case 2782:  /* aarch64_srshr_nv2si */
    case 2781:  /* aarch64_urshr_nv8hi */
    case 2780:  /* aarch64_srshr_nv8hi */
    case 2779:  /* aarch64_urshr_nv4hi */
    case 2778:  /* aarch64_srshr_nv4hi */
    case 2777:  /* aarch64_urshr_nv16qi */
    case 2776:  /* aarch64_srshr_nv16qi */
    case 2775:  /* aarch64_urshr_nv8qi */
    case 2774:  /* aarch64_srshr_nv8qi */
    case 2773:  /* aarch64_ushll2_nv4si */
    case 2772:  /* aarch64_sshll2_nv4si */
    case 2771:  /* aarch64_ushll2_nv8hi */
    case 2770:  /* aarch64_sshll2_nv8hi */
    case 2769:  /* aarch64_ushll2_nv16qi */
    case 2768:  /* aarch64_sshll2_nv16qi */
    case 2767:  /* aarch64_ushll_nv2si */
    case 2766:  /* aarch64_sshll_nv2si */
    case 2765:  /* aarch64_ushll_nv4hi */
    case 2764:  /* aarch64_sshll_nv4hi */
    case 2763:  /* aarch64_ushll_nv8qi */
    case 2762:  /* aarch64_sshll_nv8qi */
    case 2761:  /* aarch64_uqrshldi */
    case 2760:  /* aarch64_sqrshldi */
    case 2759:  /* aarch64_uqshldi */
    case 2758:  /* aarch64_sqshldi */
    case 2757:  /* aarch64_uqrshlsi */
    case 2756:  /* aarch64_sqrshlsi */
    case 2755:  /* aarch64_uqshlsi */
    case 2754:  /* aarch64_sqshlsi */
    case 2753:  /* aarch64_uqrshlhi */
    case 2752:  /* aarch64_sqrshlhi */
    case 2751:  /* aarch64_uqshlhi */
    case 2750:  /* aarch64_sqshlhi */
    case 2749:  /* aarch64_uqrshlqi */
    case 2748:  /* aarch64_sqrshlqi */
    case 2747:  /* aarch64_uqshlqi */
    case 2746:  /* aarch64_sqshlqi */
    case 2745:  /* aarch64_uqrshlv2di */
    case 2744:  /* aarch64_sqrshlv2di */
    case 2743:  /* aarch64_uqshlv2di */
    case 2742:  /* aarch64_sqshlv2di */
    case 2741:  /* aarch64_uqrshlv4si */
    case 2740:  /* aarch64_sqrshlv4si */
    case 2739:  /* aarch64_uqshlv4si */
    case 2738:  /* aarch64_sqshlv4si */
    case 2737:  /* aarch64_uqrshlv2si */
    case 2736:  /* aarch64_sqrshlv2si */
    case 2735:  /* aarch64_uqshlv2si */
    case 2734:  /* aarch64_sqshlv2si */
    case 2733:  /* aarch64_uqrshlv8hi */
    case 2732:  /* aarch64_sqrshlv8hi */
    case 2731:  /* aarch64_uqshlv8hi */
    case 2730:  /* aarch64_sqshlv8hi */
    case 2729:  /* aarch64_uqrshlv4hi */
    case 2728:  /* aarch64_sqrshlv4hi */
    case 2727:  /* aarch64_uqshlv4hi */
    case 2726:  /* aarch64_sqshlv4hi */
    case 2725:  /* aarch64_uqrshlv16qi */
    case 2724:  /* aarch64_sqrshlv16qi */
    case 2723:  /* aarch64_uqshlv16qi */
    case 2722:  /* aarch64_sqshlv16qi */
    case 2721:  /* aarch64_uqrshlv8qi */
    case 2720:  /* aarch64_sqrshlv8qi */
    case 2719:  /* aarch64_uqshlv8qi */
    case 2718:  /* aarch64_sqshlv8qi */
    case 2717:  /* aarch64_urshldi */
    case 2716:  /* aarch64_srshldi */
    case 2715:  /* aarch64_ushldi */
    case 2714:  /* aarch64_sshldi */
    case 2713:  /* aarch64_urshlv2di */
    case 2712:  /* aarch64_srshlv2di */
    case 2711:  /* aarch64_ushlv2di */
    case 2710:  /* aarch64_sshlv2di */
    case 2709:  /* aarch64_urshlv4si */
    case 2708:  /* aarch64_srshlv4si */
    case 2707:  /* aarch64_ushlv4si */
    case 2706:  /* aarch64_sshlv4si */
    case 2705:  /* aarch64_urshlv2si */
    case 2704:  /* aarch64_srshlv2si */
    case 2703:  /* aarch64_ushlv2si */
    case 2702:  /* aarch64_sshlv2si */
    case 2701:  /* aarch64_urshlv8hi */
    case 2700:  /* aarch64_srshlv8hi */
    case 2699:  /* aarch64_ushlv8hi */
    case 2698:  /* aarch64_sshlv8hi */
    case 2697:  /* aarch64_urshlv4hi */
    case 2696:  /* aarch64_srshlv4hi */
    case 2695:  /* aarch64_ushlv4hi */
    case 2694:  /* aarch64_sshlv4hi */
    case 2693:  /* aarch64_urshlv16qi */
    case 2692:  /* aarch64_srshlv16qi */
    case 2691:  /* aarch64_ushlv16qi */
    case 2690:  /* aarch64_sshlv16qi */
    case 2689:  /* aarch64_urshlv8qi */
    case 2688:  /* aarch64_srshlv8qi */
    case 2687:  /* aarch64_ushlv8qi */
    case 2686:  /* aarch64_sshlv8qi */
    case 2559:  /* aarch64_sqrdmulhsi */
    case 2558:  /* aarch64_sqdmulhsi */
    case 2557:  /* aarch64_sqrdmulhhi */
    case 2556:  /* aarch64_sqdmulhhi */
    case 2555:  /* aarch64_sqrdmulhv4si */
    case 2554:  /* aarch64_sqdmulhv4si */
    case 2553:  /* aarch64_sqrdmulhv2si */
    case 2552:  /* aarch64_sqdmulhv2si */
    case 2551:  /* aarch64_sqrdmulhv8hi */
    case 2550:  /* aarch64_sqdmulhv8hi */
    case 2549:  /* aarch64_sqrdmulhv4hi */
    case 2548:  /* aarch64_sqdmulhv4hi */
    case 2507:  /* aarch64_usqadddi */
    case 2506:  /* aarch64_suqadddi */
    case 2505:  /* aarch64_usqaddsi */
    case 2504:  /* aarch64_suqaddsi */
    case 2503:  /* aarch64_usqaddhi */
    case 2502:  /* aarch64_suqaddhi */
    case 2501:  /* aarch64_usqaddqi */
    case 2500:  /* aarch64_suqaddqi */
    case 2499:  /* aarch64_usqaddv2di */
    case 2498:  /* aarch64_suqaddv2di */
    case 2497:  /* aarch64_usqaddv4si */
    case 2496:  /* aarch64_suqaddv4si */
    case 2495:  /* aarch64_usqaddv2si */
    case 2494:  /* aarch64_suqaddv2si */
    case 2493:  /* aarch64_usqaddv8hi */
    case 2492:  /* aarch64_suqaddv8hi */
    case 2491:  /* aarch64_usqaddv4hi */
    case 2490:  /* aarch64_suqaddv4hi */
    case 2489:  /* aarch64_usqaddv16qi */
    case 2488:  /* aarch64_suqaddv16qi */
    case 2487:  /* aarch64_usqaddv8qi */
    case 2486:  /* aarch64_suqaddv8qi */
    case 2428:  /* aarch64_fmulxdf */
    case 2427:  /* aarch64_fmulxsf */
    case 2426:  /* aarch64_fmulxhf */
    case 2425:  /* aarch64_fmulxv2df */
    case 2424:  /* aarch64_fmulxv4sf */
    case 2423:  /* aarch64_fmulxv2sf */
    case 2422:  /* aarch64_fmulxv8hf */
    case 2421:  /* aarch64_fmulxv4hf */
    case 2420:  /* aarch64_pmulv16qi */
    case 2419:  /* aarch64_pmulv8qi */
    case 2406:  /* aarch64_rsubhnv2di */
    case 2405:  /* aarch64_subhnv2di */
    case 2404:  /* aarch64_raddhnv2di */
    case 2403:  /* aarch64_addhnv2di */
    case 2402:  /* aarch64_rsubhnv4si */
    case 2401:  /* aarch64_subhnv4si */
    case 2400:  /* aarch64_raddhnv4si */
    case 2399:  /* aarch64_addhnv4si */
    case 2398:  /* aarch64_rsubhnv8hi */
    case 2397:  /* aarch64_subhnv8hi */
    case 2396:  /* aarch64_raddhnv8hi */
    case 2395:  /* aarch64_addhnv8hi */
    case 2394:  /* aarch64_urhsubv4si */
    case 2393:  /* aarch64_srhsubv4si */
    case 2392:  /* aarch64_uhsubv4si */
    case 2391:  /* aarch64_shsubv4si */
    case 2390:  /* aarch64_urhaddv4si */
    case 2389:  /* aarch64_srhaddv4si */
    case 2388:  /* aarch64_uhaddv4si */
    case 2387:  /* aarch64_shaddv4si */
    case 2386:  /* aarch64_urhsubv2si */
    case 2385:  /* aarch64_srhsubv2si */
    case 2384:  /* aarch64_uhsubv2si */
    case 2383:  /* aarch64_shsubv2si */
    case 2382:  /* aarch64_urhaddv2si */
    case 2381:  /* aarch64_srhaddv2si */
    case 2380:  /* aarch64_uhaddv2si */
    case 2379:  /* aarch64_shaddv2si */
    case 2378:  /* aarch64_urhsubv8hi */
    case 2377:  /* aarch64_srhsubv8hi */
    case 2376:  /* aarch64_uhsubv8hi */
    case 2375:  /* aarch64_shsubv8hi */
    case 2374:  /* aarch64_urhaddv8hi */
    case 2373:  /* aarch64_srhaddv8hi */
    case 2372:  /* aarch64_uhaddv8hi */
    case 2371:  /* aarch64_shaddv8hi */
    case 2370:  /* aarch64_urhsubv4hi */
    case 2369:  /* aarch64_srhsubv4hi */
    case 2368:  /* aarch64_uhsubv4hi */
    case 2367:  /* aarch64_shsubv4hi */
    case 2366:  /* aarch64_urhaddv4hi */
    case 2365:  /* aarch64_srhaddv4hi */
    case 2364:  /* aarch64_uhaddv4hi */
    case 2363:  /* aarch64_shaddv4hi */
    case 2362:  /* aarch64_urhsubv16qi */
    case 2361:  /* aarch64_srhsubv16qi */
    case 2360:  /* aarch64_uhsubv16qi */
    case 2359:  /* aarch64_shsubv16qi */
    case 2358:  /* aarch64_urhaddv16qi */
    case 2357:  /* aarch64_srhaddv16qi */
    case 2356:  /* aarch64_uhaddv16qi */
    case 2355:  /* aarch64_shaddv16qi */
    case 2354:  /* aarch64_urhsubv8qi */
    case 2353:  /* aarch64_srhsubv8qi */
    case 2352:  /* aarch64_uhsubv8qi */
    case 2351:  /* aarch64_shsubv8qi */
    case 2350:  /* aarch64_urhaddv8qi */
    case 2349:  /* aarch64_srhaddv8qi */
    case 2348:  /* aarch64_uhaddv8qi */
    case 2347:  /* aarch64_shaddv8qi */
    case 2135:  /* aarch64_faddpv2df */
    case 2134:  /* aarch64_faddpv4sf */
    case 2133:  /* aarch64_faddpv2sf */
    case 2132:  /* aarch64_faddpv8hf */
    case 2131:  /* aarch64_faddpv4hf */
    case 2130:  /* fminv2df3 */
    case 2129:  /* fmaxv2df3 */
    case 2128:  /* smin_nanv2df3 */
    case 2127:  /* smax_nanv2df3 */
    case 2126:  /* fminv4sf3 */
    case 2125:  /* fmaxv4sf3 */
    case 2124:  /* smin_nanv4sf3 */
    case 2123:  /* smax_nanv4sf3 */
    case 2122:  /* fminv2sf3 */
    case 2121:  /* fmaxv2sf3 */
    case 2120:  /* smin_nanv2sf3 */
    case 2119:  /* smax_nanv2sf3 */
    case 2118:  /* fminv8hf3 */
    case 2117:  /* fmaxv8hf3 */
    case 2116:  /* smin_nanv8hf3 */
    case 2115:  /* smax_nanv8hf3 */
    case 2114:  /* fminv4hf3 */
    case 2113:  /* fmaxv4hf3 */
    case 2112:  /* smin_nanv4hf3 */
    case 2111:  /* smax_nanv4hf3 */
    case 2090:  /* ucvtfv2di3 */
    case 2089:  /* scvtfv2di3 */
    case 2088:  /* ucvtfv4si3 */
    case 2087:  /* scvtfv4si3 */
    case 2086:  /* ucvtfv2si3 */
    case 2085:  /* scvtfv2si3 */
    case 2084:  /* ucvtfv8hi3 */
    case 2083:  /* scvtfv8hi3 */
    case 2082:  /* ucvtfv4hi3 */
    case 2081:  /* scvtfv4hi3 */
    case 2080:  /* fcvtzuv2df3 */
    case 2079:  /* fcvtzsv2df3 */
    case 2078:  /* fcvtzuv4sf3 */
    case 2077:  /* fcvtzsv4sf3 */
    case 2076:  /* fcvtzuv2sf3 */
    case 2075:  /* fcvtzsv2sf3 */
    case 2074:  /* fcvtzuv8hf3 */
    case 2073:  /* fcvtzsv8hf3 */
    case 2072:  /* fcvtzuv4hf3 */
    case 2071:  /* fcvtzsv4hf3 */
    case 1789:  /* aarch64_sminpv2df */
    case 1788:  /* aarch64_smaxpv2df */
    case 1787:  /* aarch64_smin_nanpv2df */
    case 1786:  /* aarch64_smax_nanpv2df */
    case 1785:  /* aarch64_sminpv4sf */
    case 1784:  /* aarch64_smaxpv4sf */
    case 1783:  /* aarch64_smin_nanpv4sf */
    case 1782:  /* aarch64_smax_nanpv4sf */
    case 1781:  /* aarch64_sminpv2sf */
    case 1780:  /* aarch64_smaxpv2sf */
    case 1779:  /* aarch64_smin_nanpv2sf */
    case 1778:  /* aarch64_smax_nanpv2sf */
    case 1777:  /* aarch64_sminpv8hf */
    case 1776:  /* aarch64_smaxpv8hf */
    case 1775:  /* aarch64_smin_nanpv8hf */
    case 1774:  /* aarch64_smax_nanpv8hf */
    case 1773:  /* aarch64_sminpv4hf */
    case 1772:  /* aarch64_smaxpv4hf */
    case 1771:  /* aarch64_smin_nanpv4hf */
    case 1770:  /* aarch64_smax_nanpv4hf */
    case 1769:  /* aarch64_sminpv4si */
    case 1768:  /* aarch64_smaxpv4si */
    case 1767:  /* aarch64_uminpv4si */
    case 1766:  /* aarch64_umaxpv4si */
    case 1765:  /* aarch64_sminpv2si */
    case 1764:  /* aarch64_smaxpv2si */
    case 1763:  /* aarch64_uminpv2si */
    case 1762:  /* aarch64_umaxpv2si */
    case 1761:  /* aarch64_sminpv8hi */
    case 1760:  /* aarch64_smaxpv8hi */
    case 1759:  /* aarch64_uminpv8hi */
    case 1758:  /* aarch64_umaxpv8hi */
    case 1757:  /* aarch64_sminpv4hi */
    case 1756:  /* aarch64_smaxpv4hi */
    case 1755:  /* aarch64_uminpv4hi */
    case 1754:  /* aarch64_umaxpv4hi */
    case 1753:  /* aarch64_sminpv16qi */
    case 1752:  /* aarch64_smaxpv16qi */
    case 1751:  /* aarch64_uminpv16qi */
    case 1750:  /* aarch64_umaxpv16qi */
    case 1749:  /* aarch64_sminpv8qi */
    case 1748:  /* aarch64_smaxpv8qi */
    case 1747:  /* aarch64_uminpv8qi */
    case 1746:  /* aarch64_umaxpv8qi */
    case 1685:  /* vec_shr_v2sf */
    case 1684:  /* vec_shr_v2si */
    case 1683:  /* vec_shr_v4hf */
    case 1682:  /* vec_shr_v4hi */
    case 1681:  /* vec_shr_v8qi */
    case 1680:  /* aarch64_simd_reg_shlv2di_signed */
    case 1679:  /* aarch64_simd_reg_shlv4si_signed */
    case 1678:  /* aarch64_simd_reg_shlv2si_signed */
    case 1677:  /* aarch64_simd_reg_shlv8hi_signed */
    case 1676:  /* aarch64_simd_reg_shlv4hi_signed */
    case 1675:  /* aarch64_simd_reg_shlv16qi_signed */
    case 1674:  /* aarch64_simd_reg_shlv8qi_signed */
    case 1673:  /* aarch64_simd_reg_shlv2di_unsigned */
    case 1672:  /* aarch64_simd_reg_shlv4si_unsigned */
    case 1671:  /* aarch64_simd_reg_shlv2si_unsigned */
    case 1670:  /* aarch64_simd_reg_shlv8hi_unsigned */
    case 1669:  /* aarch64_simd_reg_shlv4hi_unsigned */
    case 1668:  /* aarch64_simd_reg_shlv16qi_unsigned */
    case 1667:  /* aarch64_simd_reg_shlv8qi_unsigned */
    case 1562:  /* aarch64_uadalpv4si_3 */
    case 1561:  /* aarch64_sadalpv4si_3 */
    case 1560:  /* aarch64_uadalpv8hi_3 */
    case 1559:  /* aarch64_sadalpv8hi_3 */
    case 1558:  /* aarch64_uadalpv4hi_3 */
    case 1557:  /* aarch64_sadalpv4hi_3 */
    case 1556:  /* aarch64_uadalpv16qi_3 */
    case 1555:  /* aarch64_sadalpv16qi_3 */
    case 1554:  /* aarch64_uadalpv8qi_3 */
    case 1553:  /* aarch64_sadalpv8qi_3 */
    case 1542:  /* aarch64_uabdl2v4si_3 */
    case 1541:  /* aarch64_sabdl2v4si_3 */
    case 1540:  /* aarch64_uabdl2v8hi_3 */
    case 1539:  /* aarch64_sabdl2v8hi_3 */
    case 1538:  /* aarch64_uabdl2v4hi_3 */
    case 1537:  /* aarch64_sabdl2v4hi_3 */
    case 1536:  /* aarch64_uabdl2v16qi_3 */
    case 1535:  /* aarch64_sabdl2v16qi_3 */
    case 1534:  /* aarch64_uabdl2v8qi_3 */
    case 1533:  /* aarch64_sabdl2v8qi_3 */
    case 1497:  /* aarch64_rsqrtsdf */
    case 1496:  /* aarch64_rsqrtssf */
    case 1495:  /* aarch64_rsqrtshf */
    case 1494:  /* aarch64_rsqrtsv2df */
    case 1493:  /* aarch64_rsqrtsv4sf */
    case 1492:  /* aarch64_rsqrtsv2sf */
    case 1491:  /* aarch64_rsqrtsv8hf */
    case 1490:  /* aarch64_rsqrtsv4hf */
    case 1393:  /* aarch64_fcadd270v2df */
    case 1392:  /* aarch64_fcadd90v2df */
    case 1391:  /* aarch64_fcadd270v4sf */
    case 1390:  /* aarch64_fcadd90v4sf */
    case 1389:  /* aarch64_fcadd270v2sf */
    case 1388:  /* aarch64_fcadd90v2sf */
    case 1387:  /* aarch64_fcadd270v8hf */
    case 1386:  /* aarch64_fcadd90v8hf */
    case 1385:  /* aarch64_fcadd270v4hf */
    case 1384:  /* aarch64_fcadd90v4hf */
    case 1095:  /* *despeculate_copyti_insn */
    case 1094:  /* *despeculate_copydi_insn */
    case 1093:  /* *despeculate_copysi_insn */
    case 1092:  /* *despeculate_copyhi_insn */
    case 1091:  /* *despeculate_copyqi_insn */
    case 1072:  /* probe_stack_range */
    case 1056:  /* tlsle24_di */
    case 1055:  /* tlsle24_si */
    case 1054:  /* tlsle12_di */
    case 1053:  /* tlsle12_si */
    case 1051:  /* tlsie_tiny_di */
    case 1050:  /* tlsie_tiny_si */
    case 1023:  /* fmindf3 */
    case 1022:  /* fmaxdf3 */
    case 1021:  /* smin_nandf3 */
    case 1020:  /* smax_nandf3 */
    case 1019:  /* fminsf3 */
    case 1018:  /* fmaxsf3 */
    case 1017:  /* smin_nansf3 */
    case 1016:  /* smax_nansf3 */
    case 1015:  /* fminhf3 */
    case 1014:  /* fmaxhf3 */
    case 1013:  /* smin_nanhf3 */
    case 1012:  /* smax_nanhf3 */
    case 982:  /* ucvtfhi3 */
    case 981:  /* scvtfhi3 */
    case 980:  /* fcvtzuhf3 */
    case 979:  /* fcvtzshf3 */
    case 978:  /* ucvtfdihf3 */
    case 977:  /* scvtfdihf3 */
    case 976:  /* ucvtfsihf3 */
    case 975:  /* scvtfsihf3 */
    case 974:  /* fcvtzuhfdi3 */
    case 973:  /* fcvtzshfdi3 */
    case 972:  /* fcvtzuhfsi3 */
    case 971:  /* fcvtzshfsi3 */
    case 970:  /* ucvtfdi3 */
    case 969:  /* scvtfdi3 */
    case 968:  /* ucvtfsi3 */
    case 967:  /* scvtfsi3 */
    case 966:  /* fcvtzudf3 */
    case 965:  /* fcvtzsdf3 */
    case 964:  /* fcvtzusf3 */
    case 963:  /* fcvtzssf3 */
    case 738:  /* *aarch64_sshl_2s */
    case 737:  /* *aarch64_sisd_sshl */
    case 736:  /* *aarch64_ushl_2s */
    case 735:  /* *aarch64_sisd_ushl */
    case 512:  /* aarch64_crc32cx */
    case 511:  /* aarch64_crc32cw */
    case 510:  /* aarch64_crc32ch */
    case 509:  /* aarch64_crc32cb */
    case 508:  /* aarch64_crc32x */
    case 507:  /* aarch64_crc32w */
    case 506:  /* aarch64_crc32h */
    case 505:  /* aarch64_crc32b */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (pat, 1), 0, 0));
      ro[2] = *(ro_loc[2] = &XVECEXP (XEXP (pat, 1), 0, 1));
      break;

    case 502:  /* *cmovdi_insn_uxtw */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (pat, 1), 1), 0));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (XEXP (pat, 1), 2), 0));
      break;

    case 501:  /* *cmovsi_insn_uxtw */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (pat, 1), 0), 1));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (XEXP (pat, 1), 0), 2));
      break;

    case 504:  /* *cmovdf_insn */
    case 503:  /* *cmovsf_insn */
    case 500:  /* *cmovdi_insn */
    case 499:  /* *cmovsi_insn */
    case 498:  /* *cmovhi_insn */
    case 497:  /* *cmovqi_insn */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (pat, 1), 1));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (pat, 1), 2));
      break;

    case 496:  /* *cstoresi_neg_uxtw */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0));
      break;

    case 495:  /* cstoredi_neg */
    case 494:  /* cstoresi_neg */
    case 493:  /* cstorehi_neg */
    case 492:  /* cstoreqi_neg */
    case 491:  /* *cstoresi_insn_uxtw */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      break;

    case 490:  /* *compare_cstoredi_insn */
    case 489:  /* *compare_cstoredi_insn */
    case 488:  /* *compare_cstoresi_insn */
    case 487:  /* *compare_cstoresi_insn */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 1));
      break;

    case 486:  /* aarch64_cstoredi */
    case 485:  /* aarch64_cstoresi */
    case 484:  /* aarch64_cstorehi */
    case 483:  /* aarch64_cstoreqi */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (pat, 1));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (pat, 1), 0));
      break;

    case 482:  /* *cmp_swp_zero_extendsi_shft_di */
    case 481:  /* *cmp_swp_extendsi_shft_di */
    case 480:  /* *cmp_swp_zero_extendsi_shft_si */
    case 479:  /* *cmp_swp_extendsi_shft_si */
    case 478:  /* *cmp_swp_zero_extendhi_shft_di */
    case 477:  /* *cmp_swp_extendhi_shft_di */
    case 476:  /* *cmp_swp_zero_extendhi_shft_si */
    case 475:  /* *cmp_swp_extendhi_shft_si */
    case 474:  /* *cmp_swp_zero_extendqi_shft_di */
    case 473:  /* *cmp_swp_extendqi_shft_di */
    case 472:  /* *cmp_swp_zero_extendqi_shft_si */
    case 471:  /* *cmp_swp_extendqi_shft_si */
      ro[0] = *(ro_loc[0] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 0), 1));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (pat, 1), 1));
      break;

    case 458:  /* *cmp_swp_lsr_regdi */
    case 457:  /* *cmp_swp_asr_regdi */
    case 456:  /* *cmp_swp_lsl_regdi */
    case 455:  /* *cmp_swp_lsr_regsi */
    case 454:  /* *cmp_swp_asr_regsi */
    case 453:  /* *cmp_swp_lsl_regsi */
      ro[0] = *(ro_loc[0] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 0), 1));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (pat, 1), 1));
      break;

    case 452:  /* fcmpedf */
    case 451:  /* fcmpesf */
    case 450:  /* fcmpdf */
    case 449:  /* fcmpsf */
    case 448:  /* cmpdi */
    case 447:  /* cmpsi */
      ro[0] = *(ro_loc[0] = &XEXP (XEXP (pat, 1), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 1));
      break;

    case 440:  /* umuldi3_highpart */
    case 439:  /* smuldi3_highpart */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 1), 0));
      break;

    case 438:  /* *umulsidi_neg */
    case 437:  /* *mulsidi_neg */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (pat, 1), 1), 0));
      break;

    case 436:  /* umsubsidi4 */
    case 435:  /* msubsidi4 */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 1), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (pat, 1), 0));
      break;

    case 1865:  /* *aarch64_umlalv2si */
    case 1864:  /* *aarch64_smlalv2si */
    case 1863:  /* *aarch64_umlalv4hi */
    case 1862:  /* *aarch64_smlalv4hi */
    case 1861:  /* *aarch64_umlalv8qi */
    case 1860:  /* *aarch64_smlalv8qi */
    case 434:  /* umaddsidi4 */
    case 433:  /* maddsidi4 */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 1), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (pat, 1), 1));
      break;

    case 2310:  /* aarch64_usublv2si */
    case 2309:  /* aarch64_uaddlv2si */
    case 2308:  /* aarch64_ssublv2si */
    case 2307:  /* aarch64_saddlv2si */
    case 2306:  /* aarch64_usublv4hi */
    case 2305:  /* aarch64_uaddlv4hi */
    case 2304:  /* aarch64_ssublv4hi */
    case 2303:  /* aarch64_saddlv4hi */
    case 2302:  /* aarch64_usublv8qi */
    case 2301:  /* aarch64_uaddlv8qi */
    case 2300:  /* aarch64_ssublv8qi */
    case 2299:  /* aarch64_saddlv8qi */
    case 1823:  /* vec_pack_trunc_v2di */
    case 1822:  /* vec_pack_trunc_v4si */
    case 1821:  /* vec_pack_trunc_v8hi */
    case 432:  /* umulsidi3 */
    case 431:  /* mulsidi3 */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (pat, 1), 1), 0));
      break;

    case 3012:  /* *aarch64_cmtstdi */
    case 611:  /* *xor_one_cmplsidi3_ze */
    case 418:  /* *neg_mul_imm_si2_uxtw */
    case 415:  /* *neg_lsr_si2_uxtw */
    case 414:  /* *neg_asr_si2_uxtw */
    case 413:  /* *neg_lsl_si2_uxtw */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 1));
      break;

    case 406:  /* *neg_lsrdi3_compare0 */
    case 405:  /* *neg_asrdi3_compare0 */
    case 404:  /* *neg_lsldi3_compare0 */
    case 403:  /* *neg_lsrsi3_compare0 */
    case 402:  /* *neg_asrsi3_compare0 */
    case 401:  /* *neg_lslsi3_compare0 */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0), 1));
      recog_data.dup_loc[0] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0), 0);
      recog_data.dup_num[0] = 1;
      recog_data.dup_loc[1] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0), 1);
      recog_data.dup_num[1] = 2;
      break;

    case 400:  /* *negsi2_compare0_uxtw */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0));
      recog_data.dup_loc[0] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0), 0);
      recog_data.dup_num[0] = 1;
      break;

    case 688:  /* *andshi_compare0 */
    case 687:  /* *andshi_compare0 */
    case 686:  /* *andsqi_compare0 */
    case 685:  /* *andsqi_compare0 */
    case 399:  /* negdi2_compare0 */
    case 398:  /* negsi2_compare0 */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0));
      recog_data.dup_loc[0] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0);
      recog_data.dup_num[0] = 1;
      break;

    case 397:  /* *ngcsi_uxtw */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 0), 1));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0));
      break;

    case 4206:  /* *one_cmplvnx2bi3 */
    case 4205:  /* *one_cmplvnx4bi3 */
    case 4204:  /* *one_cmplvnx8bi3 */
    case 4203:  /* *one_cmplvnx16bi3 */
    case 3594:  /* aarch64_rax1qv2di */
    case 2334:  /* aarch64_uaddwv2si */
    case 2333:  /* aarch64_saddwv2si */
    case 2332:  /* aarch64_uaddwv4hi */
    case 2331:  /* aarch64_saddwv4hi */
    case 2330:  /* aarch64_uaddwv8qi */
    case 2329:  /* aarch64_saddwv8qi */
    case 2100:  /* aarch64_float_truncate_hi_v8hf_be */
    case 2099:  /* aarch64_float_truncate_hi_v4sf_be */
    case 396:  /* *ngcdi */
    case 395:  /* *ngcsi */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 1));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      break;

    case 1034:  /* aarch64_movtilow_tilow */
    case 950:  /* *fix_to_zero_extenddfdi2 */
    case 949:  /* *fix_to_zero_extendsfdi2 */
    case 829:  /* *bswapsi2_uxtw */
    case 394:  /* *negsi2_uxtw */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      break;

    case 385:  /* *subdi3_carryinV */
    case 384:  /* *subsi3_carryinV */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 1), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 1), 1));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 1), 0), 1), 0));
      recog_data.dup_loc[0] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0), 0);
      recog_data.dup_num[0] = 1;
      recog_data.dup_loc[1] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0), 1);
      recog_data.dup_num[1] = 2;
      recog_data.dup_loc[2] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 1);
      recog_data.dup_num[2] = 4;
      recog_data.dup_loc[3] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 1), 0), 0);
      recog_data.dup_num[3] = 1;
      recog_data.dup_loc[4] = &XEXP (XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 1), 0), 1), 1);
      recog_data.dup_num[4] = 2;
      break;

    case 383:  /* *subdi3_carryinV_z2 */
    case 382:  /* *subsi3_carryinV_z2 */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 1));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 1), 0), 1));
      recog_data.dup_loc[0] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0);
      recog_data.dup_num[0] = 1;
      recog_data.dup_loc[1] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 1);
      recog_data.dup_num[1] = 3;
      recog_data.dup_loc[2] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 1), 0), 0);
      recog_data.dup_num[2] = 1;
      break;

    case 381:  /* *usubdi3_carryinC */
    case 380:  /* *usubsi3_carryinC */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 1), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 1), 1));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 1));
      recog_data.dup_loc[0] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0), 0);
      recog_data.dup_num[0] = 1;
      recog_data.dup_loc[1] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0), 1);
      recog_data.dup_num[1] = 2;
      break;

    case 379:  /* *usubdi3_carryinC_z2 */
    case 378:  /* *usubsi3_carryinC_z2 */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 1));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 1));
      recog_data.dup_loc[0] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0);
      recog_data.dup_num[0] = 1;
      break;

    case 377:  /* *usubdi3_carryinC_z1 */
    case 376:  /* *usubsi3_carryinC_z1 */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 1), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 1), 1));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 1));
      recog_data.dup_loc[0] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0), 0);
      recog_data.dup_num[0] = 1;
      break;

    case 375:  /* *subsi3_carryin_alt_uxtw */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (pat, 1), 0), 1));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 1));
      break;

    case 374:  /* *subdi3_carryin_alt */
    case 373:  /* *subsi3_carryin_alt */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (pat, 1), 1));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (pat, 1), 0), 1));
      break;

    case 391:  /* *sub_uxtsi_multp2_uxtw */
    case 388:  /* *sub_uxtsi_shift2_uxtw */
    case 366:  /* *sub_extzvsi_multp2_uxtw */
    case 365:  /* *sub_extvsi_multp2_uxtw */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 1), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 1), 0), 1));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 1), 1));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      break;

    case 390:  /* *sub_uxtdi_multp2 */
    case 389:  /* *sub_uxtsi_multp2 */
    case 387:  /* *sub_uxtdi_shift2 */
    case 386:  /* *sub_uxtsi_shift2 */
    case 364:  /* *sub_extzvdi_multp2 */
    case 363:  /* *sub_extvdi_multp2 */
    case 362:  /* *sub_extzvsi_multp2 */
    case 361:  /* *sub_extvsi_multp2 */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 1));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (pat, 1), 1), 1));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (pat, 1), 0));
      break;

    case 360:  /* *sub_zero_extendhi_shft_si_uxtw */
    case 359:  /* *sub_extendhi_shft_si_uxtw */
    case 358:  /* *sub_zero_extendqi_shft_si_uxtw */
    case 357:  /* *sub_extendqi_shft_si_uxtw */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 1), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 1), 1));
      break;

    case 1721:  /* *aarch64_mls_elt_mergev4si */
    case 1720:  /* *aarch64_mls_elt_mergev2si */
    case 1719:  /* *aarch64_mls_elt_mergev8hi */
    case 1718:  /* *aarch64_mls_elt_mergev4hi */
    case 356:  /* *sub_zero_extendsi_shft_di */
    case 355:  /* *sub_extendsi_shft_di */
    case 354:  /* *sub_zero_extendsi_shft_si */
    case 353:  /* *sub_extendsi_shft_si */
    case 352:  /* *sub_zero_extendhi_shft_di */
    case 351:  /* *sub_extendhi_shft_di */
    case 350:  /* *sub_zero_extendhi_shft_si */
    case 349:  /* *sub_extendhi_shft_si */
    case 348:  /* *sub_zero_extendqi_shft_di */
    case 347:  /* *sub_extendqi_shft_di */
    case 346:  /* *sub_zero_extendqi_shft_si */
    case 345:  /* *sub_extendqi_shft_si */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (pat, 1), 1), 1));
      break;

    case 344:  /* *sub_zero_extendhi_si_uxtw */
    case 343:  /* *sub_extendhi_si_uxtw */
    case 342:  /* *sub_zero_extendqi_si_uxtw */
    case 341:  /* *sub_extendqi_si_uxtw */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 1), 0));
      break;

    case 2316:  /* aarch64_usubwv2si */
    case 2315:  /* aarch64_ssubwv2si */
    case 2314:  /* aarch64_usubwv4hi */
    case 2313:  /* aarch64_ssubwv4hi */
    case 2312:  /* aarch64_usubwv8qi */
    case 2311:  /* aarch64_ssubwv8qi */
    case 2098:  /* aarch64_float_truncate_hi_v8hf_le */
    case 2097:  /* aarch64_float_truncate_hi_v4sf_le */
    case 340:  /* *sub_zero_extendsi_di */
    case 339:  /* *sub_extendsi_di */
    case 338:  /* *sub_zero_extendsi_si */
    case 337:  /* *sub_extendsi_si */
    case 336:  /* *sub_zero_extendhi_di */
    case 335:  /* *sub_extendhi_di */
    case 334:  /* *sub_zero_extendhi_si */
    case 333:  /* *sub_extendhi_si */
    case 332:  /* *sub_zero_extendqi_di */
    case 331:  /* *sub_extendqi_di */
    case 330:  /* *sub_zero_extendqi_si */
    case 329:  /* *sub_extendqi_si */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (pat, 1), 1), 0));
      break;

    case 427:  /* *msubsi_uxtw */
    case 328:  /* *sub_mul_imm_si_uxtw */
    case 325:  /* *sub_lsr_si_uxtw */
    case 324:  /* *sub_asr_si_uxtw */
    case 323:  /* *sub_lsl_si_uxtw */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 1), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 1), 1));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      break;

    case 426:  /* *msubdi */
    case 425:  /* *msubsi */
    case 327:  /* *sub_mul_imm_di */
    case 326:  /* *sub_mul_imm_si */
    case 322:  /* *sub_lsr_di */
    case 321:  /* *sub_asr_di */
    case 320:  /* *sub_lsl_di */
    case 319:  /* *sub_lsr_si */
    case 318:  /* *sub_asr_si */
    case 317:  /* *sub_lsl_si */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 1), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (pat, 1), 1), 1));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (pat, 1), 0));
      break;

    case 316:  /* subdi3_compare1 */
    case 315:  /* subsi3_compare1 */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 1));
      recog_data.dup_loc[0] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0);
      recog_data.dup_num[0] = 1;
      recog_data.dup_loc[1] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 1);
      recog_data.dup_num[1] = 2;
      break;

    case 314:  /* subdi3_compare1_imm */
    case 313:  /* subsi3_compare1_imm */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 1));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 1));
      recog_data.dup_loc[0] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0);
      recog_data.dup_num[0] = 1;
      break;

    case 309:  /* negvdi_carryinV */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0), 1), 0));
      recog_data.dup_loc[0] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0), 1);
      recog_data.dup_num[0] = 1;
      recog_data.dup_loc[1] = &XEXP (XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 1), 0), 0), 1);
      recog_data.dup_num[1] = 1;
      break;

    case 308:  /* negdi_carryout */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 1));
      recog_data.dup_loc[0] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0);
      recog_data.dup_num[0] = 1;
      break;

    case 307:  /* *cmpvdi_insn */
    case 306:  /* *cmpvsi_insn */
      ro[0] = *(ro_loc[0] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 1));
      recog_data.dup_loc[0] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 0);
      recog_data.dup_num[0] = 0;
      recog_data.dup_loc[1] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 1), 0);
      recog_data.dup_num[1] = 1;
      break;

    case 305:  /* negvdi_cmp_only */
    case 304:  /* negvsi_cmp_only */
      ro[0] = *(ro_loc[0] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0));
      recog_data.dup_loc[0] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 0);
      recog_data.dup_num[0] = 0;
      break;

    case 303:  /* negvdi_insn */
    case 302:  /* negvsi_insn */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0), 0));
      recog_data.dup_loc[0] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0);
      recog_data.dup_num[0] = 1;
      recog_data.dup_loc[1] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 1), 0), 0);
      recog_data.dup_num[1] = 1;
      break;

    case 301:  /* subvdi_imm */
    case 300:  /* subvsi_imm */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0), 1));
      recog_data.dup_loc[0] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0);
      recog_data.dup_num[0] = 1;
      recog_data.dup_loc[1] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 1);
      recog_data.dup_num[1] = 2;
      recog_data.dup_loc[2] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 1), 0), 0);
      recog_data.dup_num[2] = 1;
      recog_data.dup_loc[3] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 1), 1);
      recog_data.dup_num[3] = 2;
      break;

    case 299:  /* subvdi_insn */
    case 298:  /* subvsi_insn */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0), 1));
      recog_data.dup_loc[0] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0);
      recog_data.dup_num[0] = 1;
      recog_data.dup_loc[1] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 1);
      recog_data.dup_num[1] = 2;
      recog_data.dup_loc[2] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 1), 0), 0);
      recog_data.dup_num[2] = 1;
      recog_data.dup_loc[3] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 1), 1), 0);
      recog_data.dup_num[3] = 2;
      break;

    case 288:  /* *adddi3_carryinV */
    case 287:  /* *addsi3_carryinV */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0), 1), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 1), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0), 0));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 1), 0), 0), 0));
      recog_data.dup_loc[0] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0), 0);
      recog_data.dup_num[0] = 4;
      recog_data.dup_loc[1] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0), 1);
      recog_data.dup_num[1] = 1;
      recog_data.dup_loc[2] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 1);
      recog_data.dup_num[2] = 2;
      recog_data.dup_loc[3] = &XEXP (XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 1), 0), 0), 1);
      recog_data.dup_num[3] = 1;
      recog_data.dup_loc[4] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 1), 0), 1);
      recog_data.dup_num[4] = 2;
      break;

    case 286:  /* *adddi3_carryinV_zero */
    case 285:  /* *addsi3_carryinV_zero */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 1), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 1), 0), 0));
      recog_data.dup_loc[0] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0);
      recog_data.dup_num[0] = 3;
      recog_data.dup_loc[1] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 1);
      recog_data.dup_num[1] = 1;
      recog_data.dup_loc[2] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 1), 0), 1);
      recog_data.dup_num[2] = 1;
      break;

    case 284:  /* *adddi3_carryinC */
    case 283:  /* *addsi3_carryinC */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0), 1), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 1), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0), 0));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0), 0));
      ro[5] = *(ro_loc[5] = &XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 1));
      recog_data.dup_loc[0] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0), 1);
      recog_data.dup_num[0] = 1;
      recog_data.dup_loc[1] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 1);
      recog_data.dup_num[1] = 2;
      break;

    case 282:  /* *adddi3_carryinC_zero */
    case 281:  /* *addsi3_carryinC_zero */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 1), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 1));
      recog_data.dup_loc[0] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 1);
      recog_data.dup_num[0] = 1;
      break;

    case 280:  /* *addsi3_carryin_uxtw */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 1));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (pat, 1), 0), 1));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0));
      break;

    case 279:  /* *adddi3_carryin */
    case 278:  /* *addsi3_carryin */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 0), 1));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (pat, 1), 1));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      break;

    case 294:  /* *add_uxtsi_multp2_uxtw */
    case 291:  /* *add_uxtsi_shift2_uxtw */
    case 277:  /* *add_extzvsi_multp2_uxtw */
    case 276:  /* *add_extvsi_multp2_uxtw */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0), 1));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 1));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (XEXP (pat, 1), 0), 1));
      break;

    case 3010:  /* aarch64_cmtstv2di */
    case 3009:  /* aarch64_cmtstv4si */
    case 3008:  /* aarch64_cmtstv2si */
    case 3007:  /* aarch64_cmtstv8hi */
    case 3006:  /* aarch64_cmtstv4hi */
    case 3005:  /* aarch64_cmtstv16qi */
    case 3004:  /* aarch64_cmtstv8qi */
    case 293:  /* *add_uxtdi_multp2 */
    case 292:  /* *add_uxtsi_multp2 */
    case 290:  /* *add_uxtdi_shift2 */
    case 289:  /* *add_uxtsi_shift2 */
    case 275:  /* *add_extzvdi_multp2 */
    case 274:  /* *add_extvdi_multp2 */
    case 273:  /* *add_extzvsi_multp2 */
    case 272:  /* *add_extvsi_multp2 */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 1));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (pat, 1), 0), 1));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (pat, 1), 1));
      break;

    case 271:  /* *add_zero_extendhi_mult_si_uxtw */
    case 270:  /* *add_extendhi_mult_si_uxtw */
    case 269:  /* *add_zero_extendqi_mult_si_uxtw */
    case 268:  /* *add_extendqi_mult_si_uxtw */
    case 255:  /* *add_zero_extendhi_shft_si_uxtw */
    case 254:  /* *add_extendhi_shft_si_uxtw */
    case 253:  /* *add_zero_extendqi_shft_si_uxtw */
    case 252:  /* *add_extendqi_shft_si_uxtw */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 1));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (pat, 1), 0), 1));
      break;

    case 1703:  /* *aarch64_mla_elt_mergev4si */
    case 1702:  /* *aarch64_mla_elt_mergev2si */
    case 1701:  /* *aarch64_mla_elt_mergev8hi */
    case 1700:  /* *aarch64_mla_elt_mergev4hi */
    case 267:  /* *add_zero_extendsi_mult_di */
    case 266:  /* *add_extendsi_mult_di */
    case 265:  /* *add_zero_extendsi_mult_si */
    case 264:  /* *add_extendsi_mult_si */
    case 263:  /* *add_zero_extendhi_mult_di */
    case 262:  /* *add_extendhi_mult_di */
    case 261:  /* *add_zero_extendhi_mult_si */
    case 260:  /* *add_extendhi_mult_si */
    case 259:  /* *add_zero_extendqi_mult_di */
    case 258:  /* *add_extendqi_mult_di */
    case 257:  /* *add_zero_extendqi_mult_si */
    case 256:  /* *add_extendqi_mult_si */
    case 251:  /* *add_zero_extendsi_shft_di */
    case 250:  /* *add_extendsi_shft_di */
    case 249:  /* *add_zero_extendsi_shft_si */
    case 248:  /* *add_extendsi_shft_si */
    case 247:  /* *add_zero_extendhi_shft_di */
    case 246:  /* *add_extendhi_shft_di */
    case 245:  /* *add_zero_extendhi_shft_si */
    case 244:  /* *add_extendhi_shft_si */
    case 243:  /* *add_zero_extendqi_shft_di */
    case 242:  /* *add_extendqi_shft_di */
    case 241:  /* *add_zero_extendqi_shft_si */
    case 240:  /* *add_extendqi_shft_si */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (pat, 1), 0), 1));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (pat, 1), 1));
      break;

    case 610:  /* *ior_one_cmplsidi3_ze */
    case 609:  /* *and_one_cmplsidi3_ze */
    case 430:  /* *mulsi_neg_uxtw */
    case 239:  /* *add_zero_extendhi_si_uxtw */
    case 238:  /* *add_extendhi_si_uxtw */
    case 237:  /* *add_zero_extendhi_si_uxtw */
    case 236:  /* *add_extendhi_si_uxtw */
    case 235:  /* *add_zero_extendqi_si_uxtw */
    case 234:  /* *add_extendqi_si_uxtw */
    case 233:  /* *add_zero_extendqi_si_uxtw */
    case 232:  /* *add_extendqi_si_uxtw */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (pat, 1), 0), 1));
      break;

    case 4174:  /* bicvnx2di3 */
    case 4173:  /* bicvnx4si3 */
    case 4172:  /* bicvnx8hi3 */
    case 4171:  /* bicvnx16qi3 */
    case 3952:  /* *vec_seriesvnx2di_plus */
    case 3951:  /* *vec_seriesvnx4si_plus */
    case 3950:  /* *vec_seriesvnx8hi_plus */
    case 3949:  /* *vec_seriesvnx16qi_plus */
    case 1481:  /* *aarch64_mul3_elt_from_dupv2df */
    case 1480:  /* *aarch64_mul3_elt_from_dupv4sf */
    case 1479:  /* *aarch64_mul3_elt_from_dupv2sf */
    case 1478:  /* *aarch64_mul3_elt_from_dupv8hf */
    case 1477:  /* *aarch64_mul3_elt_from_dupv4hf */
    case 1476:  /* *aarch64_mul3_elt_from_dupv4si */
    case 1475:  /* *aarch64_mul3_elt_from_dupv2si */
    case 1474:  /* *aarch64_mul3_elt_from_dupv8hi */
    case 1473:  /* *aarch64_mul3_elt_from_dupv4hi */
    case 1356:  /* bicv2di3 */
    case 1355:  /* bicv4si3 */
    case 1354:  /* bicv2si3 */
    case 1353:  /* bicv8hi3 */
    case 1352:  /* bicv4hi3 */
    case 1351:  /* bicv16qi3 */
    case 1350:  /* bicv8qi3 */
    case 1349:  /* ornv2di3 */
    case 1348:  /* ornv4si3 */
    case 1347:  /* ornv2si3 */
    case 1346:  /* ornv8hi3 */
    case 1345:  /* ornv4hi3 */
    case 1344:  /* ornv16qi3 */
    case 1343:  /* ornv8qi3 */
    case 993:  /* *fnmuldf3 */
    case 992:  /* *fnmulsf3 */
    case 814:  /* *zero_extendsi_shft_di */
    case 813:  /* *extendsi_shft_di */
    case 812:  /* *zero_extendsi_shft_si */
    case 811:  /* *extendsi_shft_si */
    case 810:  /* *zero_extendhi_shft_di */
    case 809:  /* *extendhi_shft_di */
    case 808:  /* *zero_extendhi_shft_si */
    case 807:  /* *extendhi_shft_si */
    case 806:  /* *zero_extendqi_shft_di */
    case 805:  /* *extendqi_shft_di */
    case 804:  /* *zero_extendqi_shft_si */
    case 803:  /* *extendqi_shft_si */
    case 608:  /* *ior_one_cmpldi3 */
    case 607:  /* *and_one_cmpldi3 */
    case 606:  /* *ior_one_cmplsi3 */
    case 605:  /* *and_one_cmplsi3 */
    case 429:  /* *muldi_neg */
    case 428:  /* *mulsi_neg */
    case 231:  /* *add_zero_extendsi_di */
    case 230:  /* *add_extendsi_di */
    case 229:  /* *add_zero_extendsi_si */
    case 228:  /* *add_extendsi_si */
    case 227:  /* *add_zero_extendhi_di */
    case 226:  /* *add_extendhi_di */
    case 225:  /* *add_zero_extendhi_si */
    case 224:  /* *add_extendhi_si */
    case 223:  /* *add_zero_extendqi_di */
    case 222:  /* *add_extendqi_di */
    case 221:  /* *add_zero_extendqi_si */
    case 220:  /* *add_extendqi_si */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (pat, 1), 1));
      break;

    case 650:  /* *eor_one_cmpl_rotrdi3_alt */
    case 649:  /* *eor_one_cmpl_lshrdi3_alt */
    case 648:  /* *eor_one_cmpl_ashrdi3_alt */
    case 647:  /* *eor_one_cmpl_ashldi3_alt */
    case 646:  /* *eor_one_cmpl_rotrsi3_alt */
    case 645:  /* *eor_one_cmpl_lshrsi3_alt */
    case 644:  /* *eor_one_cmpl_ashrsi3_alt */
    case 643:  /* *eor_one_cmpl_ashlsi3_alt */
    case 594:  /* *xor_rolsi3_uxtw */
    case 593:  /* *ior_rolsi3_uxtw */
    case 592:  /* *and_rolsi3_uxtw */
    case 591:  /* *xor_rotrsi3_uxtw */
    case 590:  /* *ior_rotrsi3_uxtw */
    case 589:  /* *and_rotrsi3_uxtw */
    case 588:  /* *xor_lshrsi3_uxtw */
    case 587:  /* *ior_lshrsi3_uxtw */
    case 586:  /* *and_lshrsi3_uxtw */
    case 585:  /* *xor_ashrsi3_uxtw */
    case 584:  /* *ior_ashrsi3_uxtw */
    case 583:  /* *and_ashrsi3_uxtw */
    case 582:  /* *xor_ashlsi3_uxtw */
    case 581:  /* *ior_ashlsi3_uxtw */
    case 580:  /* *and_ashlsi3_uxtw */
    case 424:  /* *maddsi_uxtw */
    case 372:  /* *subsi3_carryin_uxtw */
    case 217:  /* *add_lsr_si_uxtw */
    case 216:  /* *add_asr_si_uxtw */
    case 215:  /* *add_lsl_si_uxtw */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 1));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (pat, 1), 0), 1));
      break;

    case 3595:  /* aarch64_xarqv2di */
    case 818:  /* *ashiftdi_extv_bfiz */
    case 817:  /* *ashiftsi_extv_bfiz */
    case 816:  /* *andim_ashiftdi_bfiz */
    case 815:  /* *andim_ashiftsi_bfiz */
    case 579:  /* *xor_roldi3 */
    case 578:  /* *ior_roldi3 */
    case 577:  /* *and_roldi3 */
    case 576:  /* *xor_rolsi3 */
    case 575:  /* *ior_rolsi3 */
    case 574:  /* *and_rolsi3 */
    case 573:  /* *xor_rotrdi3 */
    case 572:  /* *ior_rotrdi3 */
    case 571:  /* *and_rotrdi3 */
    case 570:  /* *xor_lshrdi3 */
    case 569:  /* *ior_lshrdi3 */
    case 568:  /* *and_lshrdi3 */
    case 567:  /* *xor_ashrdi3 */
    case 566:  /* *ior_ashrdi3 */
    case 565:  /* *and_ashrdi3 */
    case 564:  /* *xor_ashldi3 */
    case 563:  /* *ior_ashldi3 */
    case 562:  /* *and_ashldi3 */
    case 561:  /* *xor_rotrsi3 */
    case 560:  /* *ior_rotrsi3 */
    case 559:  /* *and_rotrsi3 */
    case 558:  /* *xor_lshrsi3 */
    case 557:  /* *ior_lshrsi3 */
    case 556:  /* *and_lshrsi3 */
    case 555:  /* *xor_ashrsi3 */
    case 554:  /* *ior_ashrsi3 */
    case 553:  /* *and_ashrsi3 */
    case 552:  /* *xor_ashlsi3 */
    case 551:  /* *ior_ashlsi3 */
    case 550:  /* *and_ashlsi3 */
    case 423:  /* madddi */
    case 422:  /* maddsi */
    case 371:  /* *subdi3_carryin */
    case 370:  /* *subsi3_carryin */
    case 219:  /* *add_mul_imm_di */
    case 218:  /* *add_mul_imm_si */
    case 214:  /* *add_lsr_di */
    case 213:  /* *add_asr_di */
    case 212:  /* *add_lsl_di */
    case 211:  /* *add_lsr_si */
    case 210:  /* *add_asr_si */
    case 209:  /* *add_lsl_si */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (pat, 1), 0), 1));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (pat, 1), 1));
      break;

    case 470:  /* *cmp_swp_zero_extendsi_regdi */
    case 469:  /* *cmp_swp_extendsi_regdi */
    case 468:  /* *cmp_swp_zero_extendsi_regsi */
    case 467:  /* *cmp_swp_extendsi_regsi */
    case 466:  /* *cmp_swp_zero_extendhi_regdi */
    case 465:  /* *cmp_swp_extendhi_regdi */
    case 464:  /* *cmp_swp_zero_extendhi_regsi */
    case 463:  /* *cmp_swp_extendhi_regsi */
    case 462:  /* *cmp_swp_zero_extendqi_regdi */
    case 461:  /* *cmp_swp_extendqi_regdi */
    case 460:  /* *cmp_swp_zero_extendqi_regsi */
    case 459:  /* *cmp_swp_extendqi_regsi */
    case 208:  /* *compare_negdi */
    case 207:  /* *compare_negsi */
      ro[0] = *(ro_loc[0] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 1));
      break;

    case 690:  /* *anddi3nr_compare0 */
    case 689:  /* *andsi3nr_compare0 */
    case 206:  /* aarch64_subdi_compare0 */
    case 205:  /* aarch64_subsi_compare0 */
    case 204:  /* *adddi3nr_compare0 */
    case 203:  /* *addsi3nr_compare0 */
      ro[0] = *(ro_loc[0] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 0), 1));
      break;

    case 202:  /* *subs_extzvdi_multp2 */
    case 201:  /* *subs_extvdi_multp2 */
    case 200:  /* *subs_extzvsi_multp2 */
    case 199:  /* *subs_extvsi_multp2 */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 1), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 1), 0), 1));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 1), 1));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0));
      recog_data.dup_loc[0] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0);
      recog_data.dup_num[0] = 4;
      recog_data.dup_loc[1] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 1), 0), 0);
      recog_data.dup_num[1] = 1;
      recog_data.dup_loc[2] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 1), 0), 1);
      recog_data.dup_num[2] = 2;
      recog_data.dup_loc[3] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 1), 1);
      recog_data.dup_num[3] = 3;
      break;

    case 198:  /* *adds_extzvdi_multp2 */
    case 197:  /* *adds_extvdi_multp2 */
    case 196:  /* *adds_extzvsi_multp2 */
    case 195:  /* *adds_extvsi_multp2 */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0), 0), 1));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0), 1));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 1));
      recog_data.dup_loc[0] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0), 0), 0);
      recog_data.dup_num[0] = 1;
      recog_data.dup_loc[1] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0), 0), 1);
      recog_data.dup_num[1] = 2;
      recog_data.dup_loc[2] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0), 1);
      recog_data.dup_num[2] = 3;
      recog_data.dup_loc[3] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 1);
      recog_data.dup_num[3] = 4;
      break;

    case 194:  /* *subs_zero_extendsi_shift_di */
    case 193:  /* *subs_extendsi_shift_di */
    case 192:  /* *subs_zero_extendsi_shift_si */
    case 191:  /* *subs_extendsi_shift_si */
    case 190:  /* *subs_zero_extendhi_shift_di */
    case 189:  /* *subs_extendhi_shift_di */
    case 188:  /* *subs_zero_extendhi_shift_si */
    case 187:  /* *subs_extendhi_shift_si */
    case 186:  /* *subs_zero_extendqi_shift_di */
    case 185:  /* *subs_extendqi_shift_di */
    case 184:  /* *subs_zero_extendqi_shift_si */
    case 183:  /* *subs_extendqi_shift_si */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 1), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 1), 1));
      recog_data.dup_loc[0] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0);
      recog_data.dup_num[0] = 1;
      recog_data.dup_loc[1] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 1), 0), 0);
      recog_data.dup_num[1] = 2;
      recog_data.dup_loc[2] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 1), 1);
      recog_data.dup_num[2] = 3;
      break;

    case 182:  /* *adds_zero_extendsi_shift_di */
    case 181:  /* *adds_extendsi_shift_di */
    case 180:  /* *adds_zero_extendsi_shift_si */
    case 179:  /* *adds_extendsi_shift_si */
    case 178:  /* *adds_zero_extendhi_shift_di */
    case 177:  /* *adds_extendhi_shift_di */
    case 176:  /* *adds_zero_extendhi_shift_si */
    case 175:  /* *adds_extendhi_shift_si */
    case 174:  /* *adds_zero_extendqi_shift_di */
    case 173:  /* *adds_extendqi_shift_di */
    case 172:  /* *adds_zero_extendqi_shift_si */
    case 171:  /* *adds_extendqi_shift_si */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0), 1));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 1));
      recog_data.dup_loc[0] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0), 0), 0);
      recog_data.dup_num[0] = 1;
      recog_data.dup_loc[1] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0), 1);
      recog_data.dup_num[1] = 2;
      recog_data.dup_loc[2] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 1);
      recog_data.dup_num[2] = 3;
      break;

    case 170:  /* *subs_zero_extendsi_di */
    case 169:  /* *subs_extendsi_di */
    case 168:  /* *subs_zero_extendsi_si */
    case 167:  /* *subs_extendsi_si */
    case 166:  /* *subs_zero_extendhi_di */
    case 165:  /* *subs_extendhi_di */
    case 164:  /* *subs_zero_extendhi_si */
    case 163:  /* *subs_extendhi_si */
    case 162:  /* *subs_zero_extendqi_di */
    case 161:  /* *subs_extendqi_di */
    case 160:  /* *subs_zero_extendqi_si */
    case 159:  /* *subs_extendqi_si */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 1), 0));
      recog_data.dup_loc[0] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0);
      recog_data.dup_num[0] = 1;
      recog_data.dup_loc[1] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 1), 0);
      recog_data.dup_num[1] = 2;
      break;

    case 615:  /* *and_one_cmpldi3_compare0 */
    case 614:  /* *and_one_cmplsi3_compare0 */
    case 158:  /* *adds_zero_extendsi_di */
    case 157:  /* *adds_extendsi_di */
    case 156:  /* *adds_zero_extendsi_si */
    case 155:  /* *adds_extendsi_si */
    case 154:  /* *adds_zero_extendhi_di */
    case 153:  /* *adds_extendhi_di */
    case 152:  /* *adds_zero_extendhi_si */
    case 151:  /* *adds_extendhi_si */
    case 150:  /* *adds_zero_extendqi_di */
    case 149:  /* *adds_extendqi_di */
    case 148:  /* *adds_zero_extendqi_si */
    case 147:  /* *adds_extendqi_si */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 1));
      recog_data.dup_loc[0] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0), 0);
      recog_data.dup_num[0] = 1;
      recog_data.dup_loc[1] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 1);
      recog_data.dup_num[1] = 2;
      break;

    case 146:  /* *subs_mul_imm_di */
    case 145:  /* *subs_mul_imm_si */
    case 142:  /* *subs_shift_imm_di */
    case 141:  /* *subs_shift_imm_di */
    case 140:  /* *subs_shift_imm_di */
    case 139:  /* *subs_shift_imm_si */
    case 138:  /* *subs_shift_imm_si */
    case 137:  /* *subs_shift_imm_si */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 1), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 1), 1));
      recog_data.dup_loc[0] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0);
      recog_data.dup_num[0] = 1;
      recog_data.dup_loc[1] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 1), 0);
      recog_data.dup_num[1] = 2;
      recog_data.dup_loc[2] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 1), 1);
      recog_data.dup_num[2] = 3;
      break;

    case 545:  /* *and_rotrdi3_compare0 */
    case 544:  /* *and_lshrdi3_compare0 */
    case 543:  /* *and_ashrdi3_compare0 */
    case 542:  /* *and_ashldi3_compare0 */
    case 541:  /* *and_rotrsi3_compare0 */
    case 540:  /* *and_lshrsi3_compare0 */
    case 539:  /* *and_ashrsi3_compare0 */
    case 538:  /* *and_ashlsi3_compare0 */
    case 144:  /* *adds_mul_imm_di */
    case 143:  /* *adds_mul_imm_si */
    case 136:  /* *adds_shift_imm_di */
    case 135:  /* *adds_shift_imm_di */
    case 134:  /* *adds_shift_imm_di */
    case 133:  /* *adds_shift_imm_si */
    case 132:  /* *adds_shift_imm_si */
    case 131:  /* *adds_shift_imm_si */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0), 1));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 1));
      recog_data.dup_loc[0] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0), 0);
      recog_data.dup_num[0] = 1;
      recog_data.dup_loc[1] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0), 1);
      recog_data.dup_num[1] = 2;
      recog_data.dup_loc[2] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 1);
      recog_data.dup_num[2] = 3;
      break;

    case 130:  /* adddi3_compareV */
    case 129:  /* addsi3_compareV */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 1), 0));
      recog_data.dup_loc[0] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0);
      recog_data.dup_num[0] = 1;
      recog_data.dup_loc[1] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 1);
      recog_data.dup_num[1] = 2;
      recog_data.dup_loc[2] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 1), 0), 0);
      recog_data.dup_num[2] = 1;
      recog_data.dup_loc[3] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 1), 0), 1);
      recog_data.dup_num[3] = 2;
      break;

    case 128:  /* adddi3_compareV_imm */
    case 127:  /* addsi3_compareV_imm */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 1));
      recog_data.dup_loc[0] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0);
      recog_data.dup_num[0] = 1;
      recog_data.dup_loc[1] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 1);
      recog_data.dup_num[1] = 2;
      recog_data.dup_loc[2] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 1), 0), 0);
      recog_data.dup_num[2] = 1;
      recog_data.dup_loc[3] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 1), 0), 1);
      recog_data.dup_num[3] = 2;
      break;

    case 126:  /* *adddi3_compareV_cconly */
    case 125:  /* *addsi3_compareV_cconly */
      ro[0] = *(ro_loc[0] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 1), 0));
      recog_data.dup_loc[0] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 0);
      recog_data.dup_num[0] = 0;
      recog_data.dup_loc[1] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 1);
      recog_data.dup_num[1] = 1;
      break;

    case 124:  /* *adddi3_compareV_cconly_imm */
    case 123:  /* *addsi3_compareV_cconly_imm */
      ro[0] = *(ro_loc[0] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 0), 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 0), 1));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 1));
      recog_data.dup_loc[0] = &XEXP (XEXP (XEXP (XEXP (pat, 1), 1), 0), 0);
      recog_data.dup_num[0] = 0;
      break;

    case 122:  /* adddi3_compareC */
    case 121:  /* addsi3_compareC */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 1));
      recog_data.dup_loc[0] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0);
      recog_data.dup_num[0] = 1;
      recog_data.dup_loc[1] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 1);
      recog_data.dup_num[1] = 2;
      recog_data.dup_loc[2] = &XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 1);
      recog_data.dup_num[2] = 1;
      break;

    case 120:  /* *adddi3_compareC_cconly */
    case 119:  /* *addsi3_compareC_cconly */
      ro[0] = *(ro_loc[0] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 0), 1));
      recog_data.dup_loc[0] = &XEXP (XEXP (pat, 1), 1);
      recog_data.dup_num[0] = 0;
      break;

    case 537:  /* *andsi3_compare0_uxtw */
    case 312:  /* *subsi3_compare0_uxtw */
    case 118:  /* *addsi3_compare0_uxtw */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 1));
      recog_data.dup_loc[0] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0), 0);
      recog_data.dup_num[0] = 1;
      recog_data.dup_loc[1] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0), 1);
      recog_data.dup_num[1] = 2;
      break;

    case 536:  /* *anddi3_compare0 */
    case 535:  /* *andsi3_compare0 */
    case 311:  /* *subdi3_compare0 */
    case 310:  /* *subsi3_compare0 */
    case 117:  /* adddi3_compare0 */
    case 116:  /* addsi3_compare0 */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 1));
      recog_data.dup_loc[0] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0);
      recog_data.dup_num[0] = 1;
      recog_data.dup_loc[1] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 1);
      recog_data.dup_num[1] = 2;
      break;

    case 3052:  /* aarch64_cmgtdf */
    case 3051:  /* aarch64_cmgedf */
    case 3050:  /* aarch64_cmeqdf */
    case 3049:  /* aarch64_cmledf */
    case 3048:  /* aarch64_cmltdf */
    case 3047:  /* aarch64_cmgtsf */
    case 3046:  /* aarch64_cmgesf */
    case 3045:  /* aarch64_cmeqsf */
    case 3044:  /* aarch64_cmlesf */
    case 3043:  /* aarch64_cmltsf */
    case 3042:  /* aarch64_cmgthf */
    case 3041:  /* aarch64_cmgehf */
    case 3040:  /* aarch64_cmeqhf */
    case 3039:  /* aarch64_cmlehf */
    case 3038:  /* aarch64_cmlthf */
    case 3037:  /* aarch64_cmgtv2df */
    case 3036:  /* aarch64_cmgev2df */
    case 3035:  /* aarch64_cmeqv2df */
    case 3034:  /* aarch64_cmlev2df */
    case 3033:  /* aarch64_cmltv2df */
    case 3032:  /* aarch64_cmgtv4sf */
    case 3031:  /* aarch64_cmgev4sf */
    case 3030:  /* aarch64_cmeqv4sf */
    case 3029:  /* aarch64_cmlev4sf */
    case 3028:  /* aarch64_cmltv4sf */
    case 3027:  /* aarch64_cmgtv2sf */
    case 3026:  /* aarch64_cmgev2sf */
    case 3025:  /* aarch64_cmeqv2sf */
    case 3024:  /* aarch64_cmlev2sf */
    case 3023:  /* aarch64_cmltv2sf */
    case 3022:  /* aarch64_cmgtv8hf */
    case 3021:  /* aarch64_cmgev8hf */
    case 3020:  /* aarch64_cmeqv8hf */
    case 3019:  /* aarch64_cmlev8hf */
    case 3018:  /* aarch64_cmltv8hf */
    case 3017:  /* aarch64_cmgtv4hf */
    case 3016:  /* aarch64_cmgev4hf */
    case 3015:  /* aarch64_cmeqv4hf */
    case 3014:  /* aarch64_cmlev4hf */
    case 3013:  /* aarch64_cmltv4hf */
    case 3003:  /* *aarch64_cmgtudi */
    case 3002:  /* *aarch64_cmgeudi */
    case 3001:  /* *aarch64_cmleudi */
    case 3000:  /* *aarch64_cmltudi */
    case 2995:  /* aarch64_cmgtuv2di */
    case 2994:  /* aarch64_cmgeuv2di */
    case 2993:  /* aarch64_cmleuv2di */
    case 2992:  /* aarch64_cmltuv2di */
    case 2991:  /* aarch64_cmgtuv4si */
    case 2990:  /* aarch64_cmgeuv4si */
    case 2989:  /* aarch64_cmleuv4si */
    case 2988:  /* aarch64_cmltuv4si */
    case 2987:  /* aarch64_cmgtuv2si */
    case 2986:  /* aarch64_cmgeuv2si */
    case 2985:  /* aarch64_cmleuv2si */
    case 2984:  /* aarch64_cmltuv2si */
    case 2983:  /* aarch64_cmgtuv8hi */
    case 2982:  /* aarch64_cmgeuv8hi */
    case 2981:  /* aarch64_cmleuv8hi */
    case 2980:  /* aarch64_cmltuv8hi */
    case 2979:  /* aarch64_cmgtuv4hi */
    case 2978:  /* aarch64_cmgeuv4hi */
    case 2977:  /* aarch64_cmleuv4hi */
    case 2976:  /* aarch64_cmltuv4hi */
    case 2975:  /* aarch64_cmgtuv16qi */
    case 2974:  /* aarch64_cmgeuv16qi */
    case 2973:  /* aarch64_cmleuv16qi */
    case 2972:  /* aarch64_cmltuv16qi */
    case 2971:  /* aarch64_cmgtuv8qi */
    case 2970:  /* aarch64_cmgeuv8qi */
    case 2969:  /* aarch64_cmleuv8qi */
    case 2968:  /* aarch64_cmltuv8qi */
    case 2967:  /* *aarch64_cmgtdi */
    case 2966:  /* *aarch64_cmgedi */
    case 2965:  /* *aarch64_cmeqdi */
    case 2964:  /* *aarch64_cmledi */
    case 2963:  /* *aarch64_cmltdi */
    case 2957:  /* aarch64_cmgtv2di */
    case 2956:  /* aarch64_cmgev2di */
    case 2955:  /* aarch64_cmeqv2di */
    case 2954:  /* aarch64_cmlev2di */
    case 2953:  /* aarch64_cmltv2di */
    case 2952:  /* aarch64_cmgtv4si */
    case 2951:  /* aarch64_cmgev4si */
    case 2950:  /* aarch64_cmeqv4si */
    case 2949:  /* aarch64_cmlev4si */
    case 2948:  /* aarch64_cmltv4si */
    case 2947:  /* aarch64_cmgtv2si */
    case 2946:  /* aarch64_cmgev2si */
    case 2945:  /* aarch64_cmeqv2si */
    case 2944:  /* aarch64_cmlev2si */
    case 2943:  /* aarch64_cmltv2si */
    case 2942:  /* aarch64_cmgtv8hi */
    case 2941:  /* aarch64_cmgev8hi */
    case 2940:  /* aarch64_cmeqv8hi */
    case 2939:  /* aarch64_cmlev8hi */
    case 2938:  /* aarch64_cmltv8hi */
    case 2937:  /* aarch64_cmgtv4hi */
    case 2936:  /* aarch64_cmgev4hi */
    case 2935:  /* aarch64_cmeqv4hi */
    case 2934:  /* aarch64_cmlev4hi */
    case 2933:  /* aarch64_cmltv4hi */
    case 2932:  /* aarch64_cmgtv16qi */
    case 2931:  /* aarch64_cmgev16qi */
    case 2930:  /* aarch64_cmeqv16qi */
    case 2929:  /* aarch64_cmlev16qi */
    case 2928:  /* aarch64_cmltv16qi */
    case 2927:  /* aarch64_cmgtv8qi */
    case 2926:  /* aarch64_cmgev8qi */
    case 2925:  /* aarch64_cmeqv8qi */
    case 2924:  /* aarch64_cmlev8qi */
    case 2923:  /* aarch64_cmltv8qi */
    case 2092:  /* aarch64_simd_vec_unpacks_hi_v4sf */
    case 2091:  /* aarch64_simd_vec_unpacks_hi_v8hf */
    case 2070:  /* aarch64_simd_vec_unpacks_lo_v4sf */
    case 2069:  /* aarch64_simd_vec_unpacks_lo_v8hf */
    case 1835:  /* aarch64_simd_vec_unpacku_hi_v4si */
    case 1834:  /* aarch64_simd_vec_unpacks_hi_v4si */
    case 1833:  /* aarch64_simd_vec_unpacku_hi_v8hi */
    case 1832:  /* aarch64_simd_vec_unpacks_hi_v8hi */
    case 1831:  /* aarch64_simd_vec_unpacku_hi_v16qi */
    case 1830:  /* aarch64_simd_vec_unpacks_hi_v16qi */
    case 1829:  /* aarch64_simd_vec_unpacku_lo_v4si */
    case 1828:  /* aarch64_simd_vec_unpacks_lo_v4si */
    case 1827:  /* aarch64_simd_vec_unpacku_lo_v8hi */
    case 1826:  /* aarch64_simd_vec_unpacks_lo_v8hi */
    case 1825:  /* aarch64_simd_vec_unpacku_lo_v16qi */
    case 1824:  /* aarch64_simd_vec_unpacks_lo_v16qi */
    case 1576:  /* fabddf3 */
    case 1575:  /* fabdsf3 */
    case 1574:  /* fabdhf3 */
    case 1573:  /* fabdv2df3 */
    case 1572:  /* fabdv4sf3 */
    case 1571:  /* fabdv2sf3 */
    case 1570:  /* fabdv8hf3 */
    case 1569:  /* fabdv4hf3 */
    case 995:  /* *fnmuldf3 */
    case 994:  /* *fnmulsf3 */
    case 918:  /* *aarch64_fcvtudfdi2_mult */
    case 917:  /* *aarch64_fcvtdfdi2_mult */
    case 916:  /* *aarch64_fcvtudfsi2_mult */
    case 915:  /* *aarch64_fcvtdfsi2_mult */
    case 914:  /* *aarch64_fcvtusfdi2_mult */
    case 913:  /* *aarch64_fcvtsfdi2_mult */
    case 912:  /* *aarch64_fcvtusfsi2_mult */
    case 911:  /* *aarch64_fcvtsfsi2_mult */
    case 776:  /* *extenddi_ashrhi */
    case 775:  /* *extendsi_ashrhi */
    case 774:  /* *extenddi_ashrqi */
    case 773:  /* *extendsi_ashrqi */
    case 772:  /* *zero_extenddi_lshrhi */
    case 771:  /* *zero_extendsi_lshrhi */
    case 770:  /* *zero_extenddi_lshrqi */
    case 769:  /* *zero_extendsi_lshrqi */
    case 768:  /* *zero_extenddi_ashlhi */
    case 767:  /* *extenddi_ashlhi */
    case 766:  /* *zero_extendsi_ashlhi */
    case 765:  /* *extendsi_ashlhi */
    case 764:  /* *zero_extenddi_ashlqi */
    case 763:  /* *extenddi_ashlqi */
    case 762:  /* *zero_extendsi_ashlqi */
    case 761:  /* *extendsi_ashlqi */
    case 760:  /* *rorsi3_insn_uxtw */
    case 745:  /* *rotrsi3_insn_uxtw */
    case 744:  /* *lshrsi3_insn_uxtw */
    case 743:  /* *ashrsi3_insn_uxtw */
    case 742:  /* *ashlsi3_insn_uxtw */
    case 613:  /* *xor_one_cmpldi3 */
    case 612:  /* *xor_one_cmplsi3 */
    case 604:  /* *one_cmpl_rotrdi2 */
    case 603:  /* *one_cmpl_lshrdi2 */
    case 602:  /* *one_cmpl_ashrdi2 */
    case 601:  /* *one_cmpl_ashldi2 */
    case 600:  /* *one_cmpl_rotrsi2 */
    case 599:  /* *one_cmpl_lshrsi2 */
    case 598:  /* *one_cmpl_ashrsi2 */
    case 597:  /* *one_cmpl_ashlsi2 */
    case 534:  /* *xorsi3_uxtw */
    case 533:  /* *iorsi3_uxtw */
    case 532:  /* *andsi3_uxtw */
    case 446:  /* *udivsi3_uxtw */
    case 445:  /* *divsi3_uxtw */
    case 421:  /* *mulsi3_uxtw */
    case 417:  /* *neg_mul_imm_di2 */
    case 416:  /* *neg_mul_imm_si2 */
    case 412:  /* *neg_lsr_di2 */
    case 411:  /* *neg_asr_di2 */
    case 410:  /* *neg_lsl_di2 */
    case 409:  /* *neg_lsr_si2 */
    case 408:  /* *neg_asr_si2 */
    case 407:  /* *neg_lsl_si2 */
    case 369:  /* *subsi3_carryin_uxtw */
    case 296:  /* *subsi3_uxtw */
    case 113:  /* *addsi3_aarch64_uxtw */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (pat, 1), 0), 1));
      break;

    case 4902:  /* *post_ra_fmulvnx2df3 */
    case 4901:  /* *post_ra_fsubvnx2df3 */
    case 4900:  /* *post_ra_faddvnx2df3 */
    case 4899:  /* *post_ra_fmulvnx4sf3 */
    case 4898:  /* *post_ra_fsubvnx4sf3 */
    case 4897:  /* *post_ra_faddvnx4sf3 */
    case 4896:  /* *post_ra_fmulvnx8hf3 */
    case 4895:  /* *post_ra_fsubvnx8hf3 */
    case 4894:  /* *post_ra_faddvnx8hf3 */
    case 4246:  /* *post_ra_vlshrvnx2di3 */
    case 4245:  /* *post_ra_vashrvnx2di3 */
    case 4244:  /* *post_ra_vashlvnx2di3 */
    case 4243:  /* *post_ra_vlshrvnx4si3 */
    case 4242:  /* *post_ra_vashrvnx4si3 */
    case 4241:  /* *post_ra_vashlvnx4si3 */
    case 4240:  /* *post_ra_vlshrvnx8hi3 */
    case 4239:  /* *post_ra_vashrvnx8hi3 */
    case 4238:  /* *post_ra_vashlvnx8hi3 */
    case 4237:  /* *post_ra_vlshrvnx16qi3 */
    case 4236:  /* *post_ra_vashrvnx16qi3 */
    case 4235:  /* *post_ra_vashlvnx16qi3 */
    case 4178:  /* andvnx2bi3 */
    case 4177:  /* andvnx4bi3 */
    case 4176:  /* andvnx8bi3 */
    case 4175:  /* andvnx16bi3 */
    case 4161:  /* xorvnx2di3 */
    case 4160:  /* iorvnx2di3 */
    case 4159:  /* andvnx2di3 */
    case 4158:  /* xorvnx4si3 */
    case 4157:  /* iorvnx4si3 */
    case 4156:  /* andvnx4si3 */
    case 4155:  /* xorvnx8hi3 */
    case 4154:  /* iorvnx8hi3 */
    case 4153:  /* andvnx8hi3 */
    case 4152:  /* xorvnx16qi3 */
    case 4151:  /* iorvnx16qi3 */
    case 4150:  /* andvnx16qi3 */
    case 4113:  /* *post_ra_mulvnx2di3 */
    case 4112:  /* *post_ra_mulvnx4si3 */
    case 4111:  /* *post_ra_mulvnx8hi3 */
    case 4110:  /* *post_ra_mulvnx16qi3 */
    case 4105:  /* subvnx2di3 */
    case 4104:  /* subvnx4si3 */
    case 4103:  /* subvnx8hi3 */
    case 4102:  /* subvnx16qi3 */
    case 4101:  /* addvnx2di3 */
    case 4100:  /* addvnx4si3 */
    case 4099:  /* addvnx8hi3 */
    case 4098:  /* addvnx16qi3 */
    case 3948:  /* vec_seriesvnx2di */
    case 3947:  /* vec_seriesvnx4si */
    case 3946:  /* vec_seriesvnx8hi */
    case 3945:  /* vec_seriesvnx16qi */
    case 2485:  /* aarch64_uqsubdi */
    case 2484:  /* aarch64_sqsubdi */
    case 2483:  /* aarch64_uqadddi */
    case 2482:  /* aarch64_sqadddi */
    case 2481:  /* aarch64_uqsubsi */
    case 2480:  /* aarch64_sqsubsi */
    case 2479:  /* aarch64_uqaddsi */
    case 2478:  /* aarch64_sqaddsi */
    case 2477:  /* aarch64_uqsubhi */
    case 2476:  /* aarch64_sqsubhi */
    case 2475:  /* aarch64_uqaddhi */
    case 2474:  /* aarch64_sqaddhi */
    case 2473:  /* aarch64_uqsubqi */
    case 2472:  /* aarch64_sqsubqi */
    case 2471:  /* aarch64_uqaddqi */
    case 2470:  /* aarch64_sqaddqi */
    case 2469:  /* aarch64_uqsubv2di */
    case 2468:  /* aarch64_sqsubv2di */
    case 2467:  /* aarch64_uqaddv2di */
    case 2466:  /* aarch64_sqaddv2di */
    case 2465:  /* aarch64_uqsubv4si */
    case 2464:  /* aarch64_sqsubv4si */
    case 2463:  /* aarch64_uqaddv4si */
    case 2462:  /* aarch64_sqaddv4si */
    case 2461:  /* aarch64_uqsubv2si */
    case 2460:  /* aarch64_sqsubv2si */
    case 2459:  /* aarch64_uqaddv2si */
    case 2458:  /* aarch64_sqaddv2si */
    case 2457:  /* aarch64_uqsubv8hi */
    case 2456:  /* aarch64_sqsubv8hi */
    case 2455:  /* aarch64_uqaddv8hi */
    case 2454:  /* aarch64_sqaddv8hi */
    case 2453:  /* aarch64_uqsubv4hi */
    case 2452:  /* aarch64_sqsubv4hi */
    case 2451:  /* aarch64_uqaddv4hi */
    case 2450:  /* aarch64_sqaddv4hi */
    case 2449:  /* aarch64_uqsubv16qi */
    case 2448:  /* aarch64_sqsubv16qi */
    case 2447:  /* aarch64_uqaddv16qi */
    case 2446:  /* aarch64_sqaddv16qi */
    case 2445:  /* aarch64_uqsubv8qi */
    case 2444:  /* aarch64_sqsubv8qi */
    case 2443:  /* aarch64_uqaddv8qi */
    case 2442:  /* aarch64_sqaddv8qi */
    case 2267:  /* *aarch64_combinezdf */
    case 2266:  /* *aarch64_combinezdi */
    case 2265:  /* *aarch64_combinezv2sf */
    case 2264:  /* *aarch64_combinezv2si */
    case 2263:  /* *aarch64_combinezv4hf */
    case 2262:  /* *aarch64_combinezv4hi */
    case 2261:  /* *aarch64_combinezv8qi */
    case 2260:  /* store_pair_lanesdf */
    case 2259:  /* store_pair_lanesdi */
    case 2258:  /* store_pair_lanesv2sf */
    case 2257:  /* store_pair_lanesv2si */
    case 2256:  /* store_pair_lanesv4hf */
    case 2255:  /* store_pair_lanesv4hi */
    case 2254:  /* store_pair_lanesv8qi */
    case 2253:  /* load_pair_lanesdf */
    case 2252:  /* load_pair_lanesdi */
    case 2251:  /* load_pair_lanesv2sf */
    case 2250:  /* load_pair_lanesv2si */
    case 2249:  /* load_pair_lanesv4hf */
    case 2248:  /* load_pair_lanesv4hi */
    case 2247:  /* load_pair_lanesv8qi */
    case 2110:  /* sminv2df3 */
    case 2109:  /* smaxv2df3 */
    case 2108:  /* sminv4sf3 */
    case 2107:  /* smaxv4sf3 */
    case 2106:  /* sminv2sf3 */
    case 2105:  /* smaxv2sf3 */
    case 2104:  /* sminv8hf3 */
    case 2103:  /* smaxv8hf3 */
    case 2102:  /* sminv4hf3 */
    case 2101:  /* smaxv4hf3 */
    case 1903:  /* *divv2df3 */
    case 1902:  /* *divv4sf3 */
    case 1901:  /* *divv2sf3 */
    case 1900:  /* *divv8hf3 */
    case 1899:  /* *divv4hf3 */
    case 1898:  /* mulv2df3 */
    case 1897:  /* mulv4sf3 */
    case 1896:  /* mulv2sf3 */
    case 1895:  /* mulv8hf3 */
    case 1894:  /* mulv4hf3 */
    case 1893:  /* subv2df3 */
    case 1892:  /* subv4sf3 */
    case 1891:  /* subv2sf3 */
    case 1890:  /* subv8hf3 */
    case 1889:  /* subv4hf3 */
    case 1888:  /* addv2df3 */
    case 1887:  /* addv4sf3 */
    case 1886:  /* addv2sf3 */
    case 1885:  /* addv8hf3 */
    case 1884:  /* addv4hf3 */
    case 1745:  /* uminv4si3 */
    case 1744:  /* umaxv4si3 */
    case 1743:  /* sminv4si3 */
    case 1742:  /* smaxv4si3 */
    case 1741:  /* uminv2si3 */
    case 1740:  /* umaxv2si3 */
    case 1739:  /* sminv2si3 */
    case 1738:  /* smaxv2si3 */
    case 1737:  /* uminv8hi3 */
    case 1736:  /* umaxv8hi3 */
    case 1735:  /* sminv8hi3 */
    case 1734:  /* smaxv8hi3 */
    case 1733:  /* uminv4hi3 */
    case 1732:  /* umaxv4hi3 */
    case 1731:  /* sminv4hi3 */
    case 1730:  /* smaxv4hi3 */
    case 1729:  /* uminv16qi3 */
    case 1728:  /* umaxv16qi3 */
    case 1727:  /* sminv16qi3 */
    case 1726:  /* smaxv16qi3 */
    case 1725:  /* uminv8qi3 */
    case 1724:  /* umaxv8qi3 */
    case 1723:  /* sminv8qi3 */
    case 1722:  /* smaxv8qi3 */
    case 1666:  /* aarch64_simd_reg_sshlv2di */
    case 1665:  /* aarch64_simd_reg_sshlv4si */
    case 1664:  /* aarch64_simd_reg_sshlv2si */
    case 1663:  /* aarch64_simd_reg_sshlv8hi */
    case 1662:  /* aarch64_simd_reg_sshlv4hi */
    case 1661:  /* aarch64_simd_reg_sshlv16qi */
    case 1660:  /* aarch64_simd_reg_sshlv8qi */
    case 1659:  /* aarch64_simd_imm_shlv2di */
    case 1658:  /* aarch64_simd_imm_shlv4si */
    case 1657:  /* aarch64_simd_imm_shlv2si */
    case 1656:  /* aarch64_simd_imm_shlv8hi */
    case 1655:  /* aarch64_simd_imm_shlv4hi */
    case 1654:  /* aarch64_simd_imm_shlv16qi */
    case 1653:  /* aarch64_simd_imm_shlv8qi */
    case 1652:  /* aarch64_simd_ashrv2di */
    case 1651:  /* aarch64_simd_ashrv4si */
    case 1650:  /* aarch64_simd_ashrv2si */
    case 1649:  /* aarch64_simd_ashrv8hi */
    case 1648:  /* aarch64_simd_ashrv4hi */
    case 1647:  /* aarch64_simd_ashrv16qi */
    case 1646:  /* aarch64_simd_ashrv8qi */
    case 1645:  /* aarch64_simd_lshrv2di */
    case 1644:  /* aarch64_simd_lshrv4si */
    case 1643:  /* aarch64_simd_lshrv2si */
    case 1642:  /* aarch64_simd_lshrv8hi */
    case 1641:  /* aarch64_simd_lshrv4hi */
    case 1640:  /* aarch64_simd_lshrv16qi */
    case 1639:  /* aarch64_simd_lshrv8qi */
    case 1597:  /* xorv2di3 */
    case 1596:  /* xorv4si3 */
    case 1595:  /* xorv2si3 */
    case 1594:  /* xorv8hi3 */
    case 1593:  /* xorv4hi3 */
    case 1592:  /* xorv16qi3 */
    case 1591:  /* xorv8qi3 */
    case 1590:  /* iorv2di3 */
    case 1589:  /* iorv4si3 */
    case 1588:  /* iorv2si3 */
    case 1587:  /* iorv8hi3 */
    case 1586:  /* iorv4hi3 */
    case 1585:  /* iorv16qi3 */
    case 1584:  /* iorv8qi3 */
    case 1583:  /* andv2di3 */
    case 1582:  /* andv4si3 */
    case 1581:  /* andv2si3 */
    case 1580:  /* andv8hi3 */
    case 1579:  /* andv4hi3 */
    case 1578:  /* andv16qi3 */
    case 1577:  /* andv8qi3 */
    case 1376:  /* mulv4si3 */
    case 1375:  /* mulv2si3 */
    case 1374:  /* mulv8hi3 */
    case 1373:  /* mulv4hi3 */
    case 1372:  /* mulv16qi3 */
    case 1371:  /* mulv8qi3 */
    case 1370:  /* subv2di3 */
    case 1369:  /* subv4si3 */
    case 1368:  /* subv2si3 */
    case 1367:  /* subv8hi3 */
    case 1366:  /* subv4hi3 */
    case 1365:  /* subv16qi3 */
    case 1364:  /* subv8qi3 */
    case 1363:  /* addv2di3 */
    case 1362:  /* addv4si3 */
    case 1361:  /* addv2si3 */
    case 1360:  /* addv8hi3 */
    case 1359:  /* addv4hi3 */
    case 1358:  /* addv16qi3 */
    case 1357:  /* addv8qi3 */
    case 1342:  /* aarch64_simd_mov_from_v2dfhigh */
    case 1341:  /* aarch64_simd_mov_from_v4sfhigh */
    case 1340:  /* aarch64_simd_mov_from_v8hfhigh */
    case 1339:  /* aarch64_simd_mov_from_v2dihigh */
    case 1338:  /* aarch64_simd_mov_from_v4sihigh */
    case 1337:  /* aarch64_simd_mov_from_v8hihigh */
    case 1336:  /* aarch64_simd_mov_from_v16qihigh */
    case 1335:  /* aarch64_simd_mov_from_v2dflow */
    case 1334:  /* aarch64_simd_mov_from_v4sflow */
    case 1333:  /* aarch64_simd_mov_from_v8hflow */
    case 1332:  /* aarch64_simd_mov_from_v2dilow */
    case 1331:  /* aarch64_simd_mov_from_v4silow */
    case 1330:  /* aarch64_simd_mov_from_v8hilow */
    case 1329:  /* aarch64_simd_mov_from_v16qilow */
    case 1036:  /* add_losym_di */
    case 1035:  /* add_losym_si */
    case 1011:  /* smindf3 */
    case 1010:  /* sminsf3 */
    case 1009:  /* smaxdf3 */
    case 1008:  /* smaxsf3 */
    case 998:  /* *divdf3 */
    case 997:  /* *divsf3 */
    case 996:  /* *divhf3 */
    case 991:  /* muldf3 */
    case 990:  /* mulsf3 */
    case 989:  /* mulhf3 */
    case 988:  /* subdf3 */
    case 987:  /* subsf3 */
    case 986:  /* subhf3 */
    case 985:  /* adddf3 */
    case 984:  /* addsf3 */
    case 983:  /* addhf3 */
    case 759:  /* *rordi3_insn */
    case 758:  /* *rorsi3_insn */
    case 751:  /* *lshrhi3_insn */
    case 750:  /* *ashrhi3_insn */
    case 749:  /* *ashlhi3_insn */
    case 748:  /* *lshrqi3_insn */
    case 747:  /* *ashrqi3_insn */
    case 746:  /* *ashlqi3_insn */
    case 741:  /* *rordi3_insn */
    case 740:  /* *rorsi3_insn */
    case 734:  /* *aarch64_ashr_sisd_or_int_di3 */
    case 733:  /* *aarch64_ashr_sisd_or_int_si3 */
    case 732:  /* *aarch64_lshr_sisd_or_int_di3 */
    case 731:  /* *aarch64_lshr_sisd_or_int_si3 */
    case 730:  /* *aarch64_ashl_sisd_or_int_di3 */
    case 729:  /* *aarch64_ashl_sisd_or_int_si3 */
    case 531:  /* xordi3 */
    case 530:  /* iordi3 */
    case 529:  /* anddi3 */
    case 528:  /* xorsi3 */
    case 527:  /* iorsi3 */
    case 526:  /* andsi3 */
    case 525:  /* *aarch64_anddi_imm2 */
    case 524:  /* *aarch64_andsi_imm2 */
    case 444:  /* udivdi3 */
    case 443:  /* divdi3 */
    case 442:  /* udivsi3 */
    case 441:  /* divsi3 */
    case 420:  /* muldi3 */
    case 419:  /* mulsi3 */
    case 368:  /* *subdi3_carryin0 */
    case 367:  /* *subsi3_carryin0 */
    case 297:  /* subdi3 */
    case 295:  /* subsi3 */
    case 115:  /* *adddi3_poly_1 */
    case 114:  /* *addsi3_poly_1 */
    case 112:  /* *adddi3_aarch64 */
    case 111:  /* *addsi3_aarch64 */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (pat, 1), 1));
      break;

    case 100:  /* *load_pair_zero_extendsidi2_aarch64 */
    case 98:  /* *load_pair_extendsidi2_aarch64 */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0));
      break;

    case 3895:  /* *vec_extractvnx2dfdf_0 */
    case 3894:  /* *vec_extractvnx4sfsf_0 */
    case 3893:  /* *vec_extractvnx8hfhf_0 */
    case 3892:  /* *vec_extractvnx2didi_0 */
    case 3891:  /* *vec_extractvnx4sisi_0 */
    case 3890:  /* *vec_extractvnx8hihi_0 */
    case 3889:  /* *vec_extractvnx16qiqi_0 */
    case 3528:  /* *aarch64_simd_ld1rv2df */
    case 3527:  /* *aarch64_simd_ld1rv4sf */
    case 3526:  /* *aarch64_simd_ld1rv2sf */
    case 3525:  /* *aarch64_simd_ld1rv8hf */
    case 3524:  /* *aarch64_simd_ld1rv4hf */
    case 3523:  /* *aarch64_simd_ld1rv2di */
    case 3522:  /* *aarch64_simd_ld1rv4si */
    case 3521:  /* *aarch64_simd_ld1rv2si */
    case 3520:  /* *aarch64_simd_ld1rv8hi */
    case 3519:  /* *aarch64_simd_ld1rv4hi */
    case 3518:  /* *aarch64_simd_ld1rv16qi */
    case 3517:  /* *aarch64_simd_ld1rv8qi */
    case 3093:  /* *sqrtv2df2 */
    case 3092:  /* *sqrtv4sf2 */
    case 3091:  /* *sqrtv2sf2 */
    case 3090:  /* *sqrtv8hf2 */
    case 3089:  /* *sqrtv4hf2 */
    case 2547:  /* aarch64_sqabsdi */
    case 2546:  /* aarch64_sqnegdi */
    case 2545:  /* aarch64_sqabssi */
    case 2544:  /* aarch64_sqnegsi */
    case 2543:  /* aarch64_sqabshi */
    case 2542:  /* aarch64_sqneghi */
    case 2541:  /* aarch64_sqabsqi */
    case 2540:  /* aarch64_sqnegqi */
    case 2539:  /* aarch64_sqabsv2di */
    case 2538:  /* aarch64_sqnegv2di */
    case 2537:  /* aarch64_sqabsv4si */
    case 2536:  /* aarch64_sqnegv4si */
    case 2535:  /* aarch64_sqabsv2si */
    case 2534:  /* aarch64_sqnegv2si */
    case 2533:  /* aarch64_sqabsv8hi */
    case 2532:  /* aarch64_sqnegv8hi */
    case 2531:  /* aarch64_sqabsv4hi */
    case 2530:  /* aarch64_sqnegv4hi */
    case 2529:  /* aarch64_sqabsv16qi */
    case 2528:  /* aarch64_sqnegv16qi */
    case 2527:  /* aarch64_sqabsv8qi */
    case 2526:  /* aarch64_sqnegv8qi */
    case 2158:  /* popcountv16qi2 */
    case 2157:  /* popcountv8qi2 */
    case 2156:  /* clzv4si2 */
    case 2155:  /* clzv2si2 */
    case 2154:  /* clzv8hi2 */
    case 2153:  /* clzv4hi2 */
    case 2152:  /* clzv16qi2 */
    case 2151:  /* clzv8qi2 */
    case 2150:  /* clrsbv4si2 */
    case 2149:  /* clrsbv2si2 */
    case 2148:  /* clrsbv8hi2 */
    case 2147:  /* clrsbv4hi2 */
    case 2146:  /* clrsbv16qi2 */
    case 2145:  /* clrsbv8qi2 */
    case 2096:  /* aarch64_float_truncate_lo_v4hf */
    case 2095:  /* aarch64_float_truncate_lo_v2sf */
    case 2094:  /* aarch64_float_extend_lo_v4sf */
    case 2093:  /* aarch64_float_extend_lo_v2df */
    case 2068:  /* floatunsv2div2df2 */
    case 2067:  /* floatv2div2df2 */
    case 2066:  /* floatunsv4siv4sf2 */
    case 2065:  /* floatv4siv4sf2 */
    case 2064:  /* floatunsv2siv2sf2 */
    case 2063:  /* floatv2siv2sf2 */
    case 2062:  /* floatunsv8hiv8hf2 */
    case 2061:  /* floatv8hiv8hf2 */
    case 2060:  /* floatunsv4hiv4hf2 */
    case 2059:  /* floatv4hiv4hf2 */
    case 2052:  /* floatunshihf2 */
    case 2051:  /* floathihf2 */
    case 2050:  /* fixuns_trunchfhi2 */
    case 2049:  /* fix_trunchfhi2 */
    case 1913:  /* absv2df2 */
    case 1912:  /* absv4sf2 */
    case 1911:  /* absv2sf2 */
    case 1910:  /* absv8hf2 */
    case 1909:  /* absv4hf2 */
    case 1908:  /* negv2df2 */
    case 1907:  /* negv4sf2 */
    case 1906:  /* negv2sf2 */
    case 1905:  /* negv8hf2 */
    case 1904:  /* negv4hf2 */
    case 1820:  /* aarch64_simd_vec_pack_trunc_v2di */
    case 1819:  /* aarch64_simd_vec_pack_trunc_v4si */
    case 1818:  /* aarch64_simd_vec_pack_trunc_v8hi */
    case 1796:  /* move_lo_quad_internal_v2df */
    case 1795:  /* move_lo_quad_internal_v2di */
    case 1794:  /* move_lo_quad_internal_v4sf */
    case 1793:  /* move_lo_quad_internal_v8hf */
    case 1792:  /* move_lo_quad_internal_v4si */
    case 1791:  /* move_lo_quad_internal_v8hi */
    case 1790:  /* move_lo_quad_internal_v16qi */
    case 1604:  /* one_cmplv2di2 */
    case 1603:  /* one_cmplv4si2 */
    case 1602:  /* one_cmplv2si2 */
    case 1601:  /* one_cmplv8hi2 */
    case 1600:  /* one_cmplv4hi2 */
    case 1599:  /* one_cmplv16qi2 */
    case 1598:  /* one_cmplv8qi2 */
    case 1512:  /* absv2di2 */
    case 1511:  /* absv4si2 */
    case 1510:  /* absv2si2 */
    case 1509:  /* absv8hi2 */
    case 1508:  /* absv4hi2 */
    case 1507:  /* absv16qi2 */
    case 1506:  /* absv8qi2 */
    case 1505:  /* negv2di2 */
    case 1504:  /* negv4si2 */
    case 1503:  /* negv2si2 */
    case 1502:  /* negv8hi2 */
    case 1501:  /* negv4hi2 */
    case 1500:  /* negv16qi2 */
    case 1499:  /* negv8qi2 */
    case 1381:  /* bswapv2di2 */
    case 1380:  /* bswapv4si2 */
    case 1379:  /* bswapv2si2 */
    case 1378:  /* bswapv8hi2 */
    case 1377:  /* bswapv4hi2 */
    case 1112:  /* aarch64_simd_dupv2df */
    case 1111:  /* aarch64_simd_dupv4sf */
    case 1110:  /* aarch64_simd_dupv2sf */
    case 1109:  /* aarch64_simd_dupv8hf */
    case 1108:  /* aarch64_simd_dupv4hf */
    case 1107:  /* aarch64_simd_dupv2di */
    case 1106:  /* aarch64_simd_dupv4si */
    case 1105:  /* aarch64_simd_dupv2si */
    case 1104:  /* aarch64_simd_dupv8hi */
    case 1103:  /* aarch64_simd_dupv4hi */
    case 1102:  /* aarch64_simd_dupv16qi */
    case 1101:  /* aarch64_simd_dupv8qi */
    case 1033:  /* aarch64_movtflow_di */
    case 1032:  /* aarch64_movtilow_di */
    case 1029:  /* aarch64_movdi_tfhigh */
    case 1028:  /* aarch64_movdi_tihigh */
    case 1027:  /* aarch64_movdi_tflow */
    case 1026:  /* aarch64_movdi_tilow */
    case 1007:  /* absdf2 */
    case 1006:  /* abssf2 */
    case 1005:  /* abshf2 */
    case 1004:  /* *sqrtdf2 */
    case 1003:  /* *sqrtsf2 */
    case 1002:  /* *sqrthf2 */
    case 1001:  /* negdf2 */
    case 1000:  /* negsf2 */
    case 999:  /* neghf2 */
    case 962:  /* aarch64_fp16_floatunsdihf2 */
    case 961:  /* aarch64_fp16_floatdihf2 */
    case 960:  /* aarch64_fp16_floatunssihf2 */
    case 959:  /* aarch64_fp16_floatsihf2 */
    case 958:  /* floatunssidf2 */
    case 957:  /* floatsidf2 */
    case 956:  /* floatunsdisf2 */
    case 955:  /* floatdisf2 */
    case 954:  /* floatunsdidf2 */
    case 953:  /* floatdidf2 */
    case 952:  /* floatunssisf2 */
    case 951:  /* floatsisf2 */
    case 948:  /* fixuns_truncsfdi2 */
    case 947:  /* fix_truncsfdi2 */
    case 946:  /* fixuns_truncdfsi2 */
    case 945:  /* fix_truncdfsi2 */
    case 944:  /* fixuns_trunchfdi2 */
    case 943:  /* fix_trunchfdi2 */
    case 942:  /* fixuns_trunchfsi2 */
    case 941:  /* fix_trunchfsi2 */
    case 940:  /* fixuns_truncdfdi2 */
    case 939:  /* fix_truncdfdi2 */
    case 938:  /* fixuns_truncsfsi2 */
    case 937:  /* fix_truncsfsi2 */
    case 936:  /* truncdfhf2 */
    case 935:  /* truncsfhf2 */
    case 934:  /* truncdfsf2 */
    case 933:  /* extendhfdf2 */
    case 932:  /* extendhfsf2 */
    case 931:  /* extendsfdf2 */
    case 821:  /* bswaphi2 */
    case 820:  /* bswapdi2 */
    case 819:  /* bswapsi2 */
    case 682:  /* ctzdi2 */
    case 681:  /* ctzsi2 */
    case 678:  /* clrsbdi2 */
    case 677:  /* clrsbsi2 */
    case 676:  /* clzdi2 */
    case 675:  /* clzsi2 */
    case 596:  /* one_cmpldi2 */
    case 595:  /* one_cmplsi2 */
    case 393:  /* negdi2 */
    case 392:  /* negsi2 */
    case 110:  /* *zero_extendqihi2_aarch64 */
    case 109:  /* *extendqihi2_aarch64 */
    case 108:  /* *zero_extendhidi2_aarch64 */
    case 107:  /* *zero_extendhisi2_aarch64 */
    case 106:  /* *zero_extendqidi2_aarch64 */
    case 105:  /* *zero_extendqisi2_aarch64 */
    case 104:  /* *extendhidi2_aarch64 */
    case 103:  /* *extendhisi2_aarch64 */
    case 102:  /* *extendqidi2_aarch64 */
    case 101:  /* *extendqisi2_aarch64 */
    case 99:  /* *zero_extendsidi2_aarch64 */
    case 97:  /* *extendsidi2_aarch64 */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 1), 0));
      break;

    case 96:  /* storewb_pairtf_di */
    case 95:  /* storewb_pairtf_si */
    case 94:  /* storewb_pairti_di */
    case 93:  /* storewb_pairti_si */
    case 92:  /* storewb_pairdf_di */
    case 91:  /* storewb_pairdf_si */
    case 90:  /* storewb_pairsf_di */
    case 89:  /* storewb_pairsf_si */
    case 88:  /* storewb_pairdi_di */
    case 87:  /* storewb_pairdi_si */
    case 86:  /* storewb_pairsi_di */
    case 85:  /* storewb_pairsi_si */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XVECEXP (pat, 0, 1), 1));
      ro[3] = *(ro_loc[3] = &XEXP (XVECEXP (pat, 0, 2), 1));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 1));
      ro[5] = *(ro_loc[5] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 2), 0), 0), 1));
      recog_data.dup_loc[0] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 2), 0), 0), 0);
      recog_data.dup_num[0] = 0;
      recog_data.dup_loc[1] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 0), 0), 0);
      recog_data.dup_num[1] = 0;
      recog_data.dup_loc[2] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 1), 0), 0), 1);
      recog_data.dup_num[2] = 4;
      break;

    case 84:  /* loadwb_pairtf_di */
    case 83:  /* loadwb_pairtf_si */
    case 82:  /* loadwb_pairti_di */
    case 81:  /* loadwb_pairti_si */
    case 80:  /* loadwb_pairdf_di */
    case 79:  /* loadwb_pairdf_si */
    case 78:  /* loadwb_pairsf_di */
    case 77:  /* loadwb_pairsf_si */
    case 76:  /* loadwb_pairdi_di */
    case 75:  /* loadwb_pairdi_si */
    case 74:  /* loadwb_pairsi_di */
    case 73:  /* loadwb_pairsi_si */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XVECEXP (pat, 0, 2), 0));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 1));
      ro[5] = *(ro_loc[5] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 2), 1), 0), 1));
      recog_data.dup_loc[0] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 2), 1), 0), 0);
      recog_data.dup_num[0] = 1;
      recog_data.dup_loc[1] = &XEXP (XEXP (XVECEXP (pat, 0, 1), 1), 0);
      recog_data.dup_num[1] = 1;
      break;

    case 1328:  /* vec_store_pairv2dfv2df */
    case 1327:  /* vec_store_pairv4sfv2df */
    case 1326:  /* vec_store_pairv8hfv2df */
    case 1325:  /* vec_store_pairv2div2df */
    case 1324:  /* vec_store_pairv4siv2df */
    case 1323:  /* vec_store_pairv8hiv2df */
    case 1322:  /* vec_store_pairv16qiv2df */
    case 1321:  /* vec_store_pairv2dfv4sf */
    case 1320:  /* vec_store_pairv4sfv4sf */
    case 1319:  /* vec_store_pairv8hfv4sf */
    case 1318:  /* vec_store_pairv2div4sf */
    case 1317:  /* vec_store_pairv4siv4sf */
    case 1316:  /* vec_store_pairv8hiv4sf */
    case 1315:  /* vec_store_pairv16qiv4sf */
    case 1314:  /* vec_store_pairv2dfv8hf */
    case 1313:  /* vec_store_pairv4sfv8hf */
    case 1312:  /* vec_store_pairv8hfv8hf */
    case 1311:  /* vec_store_pairv2div8hf */
    case 1310:  /* vec_store_pairv4siv8hf */
    case 1309:  /* vec_store_pairv8hiv8hf */
    case 1308:  /* vec_store_pairv16qiv8hf */
    case 1307:  /* vec_store_pairv2dfv2di */
    case 1306:  /* vec_store_pairv4sfv2di */
    case 1305:  /* vec_store_pairv8hfv2di */
    case 1304:  /* vec_store_pairv2div2di */
    case 1303:  /* vec_store_pairv4siv2di */
    case 1302:  /* vec_store_pairv8hiv2di */
    case 1301:  /* vec_store_pairv16qiv2di */
    case 1300:  /* vec_store_pairv2dfv4si */
    case 1299:  /* vec_store_pairv4sfv4si */
    case 1298:  /* vec_store_pairv8hfv4si */
    case 1297:  /* vec_store_pairv2div4si */
    case 1296:  /* vec_store_pairv4siv4si */
    case 1295:  /* vec_store_pairv8hiv4si */
    case 1294:  /* vec_store_pairv16qiv4si */
    case 1293:  /* vec_store_pairv2dfv8hi */
    case 1292:  /* vec_store_pairv4sfv8hi */
    case 1291:  /* vec_store_pairv8hfv8hi */
    case 1290:  /* vec_store_pairv2div8hi */
    case 1289:  /* vec_store_pairv4siv8hi */
    case 1288:  /* vec_store_pairv8hiv8hi */
    case 1287:  /* vec_store_pairv16qiv8hi */
    case 1286:  /* vec_store_pairv2dfv16qi */
    case 1285:  /* vec_store_pairv4sfv16qi */
    case 1284:  /* vec_store_pairv8hfv16qi */
    case 1283:  /* vec_store_pairv2div16qi */
    case 1282:  /* vec_store_pairv4siv16qi */
    case 1281:  /* vec_store_pairv8hiv16qi */
    case 1280:  /* vec_store_pairv16qiv16qi */
    case 1279:  /* load_pairv2dfv2df */
    case 1278:  /* load_pairv4sfv2df */
    case 1277:  /* load_pairv8hfv2df */
    case 1276:  /* load_pairv2div2df */
    case 1275:  /* load_pairv4siv2df */
    case 1274:  /* load_pairv8hiv2df */
    case 1273:  /* load_pairv16qiv2df */
    case 1272:  /* load_pairv2dfv4sf */
    case 1271:  /* load_pairv4sfv4sf */
    case 1270:  /* load_pairv8hfv4sf */
    case 1269:  /* load_pairv2div4sf */
    case 1268:  /* load_pairv4siv4sf */
    case 1267:  /* load_pairv8hiv4sf */
    case 1266:  /* load_pairv16qiv4sf */
    case 1265:  /* load_pairv2dfv8hf */
    case 1264:  /* load_pairv4sfv8hf */
    case 1263:  /* load_pairv8hfv8hf */
    case 1262:  /* load_pairv2div8hf */
    case 1261:  /* load_pairv4siv8hf */
    case 1260:  /* load_pairv8hiv8hf */
    case 1259:  /* load_pairv16qiv8hf */
    case 1258:  /* load_pairv2dfv2di */
    case 1257:  /* load_pairv4sfv2di */
    case 1256:  /* load_pairv8hfv2di */
    case 1255:  /* load_pairv2div2di */
    case 1254:  /* load_pairv4siv2di */
    case 1253:  /* load_pairv8hiv2di */
    case 1252:  /* load_pairv16qiv2di */
    case 1251:  /* load_pairv2dfv4si */
    case 1250:  /* load_pairv4sfv4si */
    case 1249:  /* load_pairv8hfv4si */
    case 1248:  /* load_pairv2div4si */
    case 1247:  /* load_pairv4siv4si */
    case 1246:  /* load_pairv8hiv4si */
    case 1245:  /* load_pairv16qiv4si */
    case 1244:  /* load_pairv2dfv8hi */
    case 1243:  /* load_pairv4sfv8hi */
    case 1242:  /* load_pairv8hfv8hi */
    case 1241:  /* load_pairv2div8hi */
    case 1240:  /* load_pairv4siv8hi */
    case 1239:  /* load_pairv8hiv8hi */
    case 1238:  /* load_pairv16qiv8hi */
    case 1237:  /* load_pairv2dfv16qi */
    case 1236:  /* load_pairv4sfv16qi */
    case 1235:  /* load_pairv8hfv16qi */
    case 1234:  /* load_pairv2div16qi */
    case 1233:  /* load_pairv4siv16qi */
    case 1232:  /* load_pairv8hiv16qi */
    case 1231:  /* load_pairv16qiv16qi */
    case 1230:  /* vec_store_pairdfdf */
    case 1229:  /* vec_store_pairv2sfdf */
    case 1228:  /* vec_store_pairv2sidf */
    case 1227:  /* vec_store_pairv4hfdf */
    case 1226:  /* vec_store_pairv4hidf */
    case 1225:  /* vec_store_pairv8qidf */
    case 1224:  /* vec_store_pairdfv2sf */
    case 1223:  /* vec_store_pairv2sfv2sf */
    case 1222:  /* vec_store_pairv2siv2sf */
    case 1221:  /* vec_store_pairv4hfv2sf */
    case 1220:  /* vec_store_pairv4hiv2sf */
    case 1219:  /* vec_store_pairv8qiv2sf */
    case 1218:  /* vec_store_pairdfv2si */
    case 1217:  /* vec_store_pairv2sfv2si */
    case 1216:  /* vec_store_pairv2siv2si */
    case 1215:  /* vec_store_pairv4hfv2si */
    case 1214:  /* vec_store_pairv4hiv2si */
    case 1213:  /* vec_store_pairv8qiv2si */
    case 1212:  /* vec_store_pairdfv4hf */
    case 1211:  /* vec_store_pairv2sfv4hf */
    case 1210:  /* vec_store_pairv2siv4hf */
    case 1209:  /* vec_store_pairv4hfv4hf */
    case 1208:  /* vec_store_pairv4hiv4hf */
    case 1207:  /* vec_store_pairv8qiv4hf */
    case 1206:  /* vec_store_pairdfv4hi */
    case 1205:  /* vec_store_pairv2sfv4hi */
    case 1204:  /* vec_store_pairv2siv4hi */
    case 1203:  /* vec_store_pairv4hfv4hi */
    case 1202:  /* vec_store_pairv4hiv4hi */
    case 1201:  /* vec_store_pairv8qiv4hi */
    case 1200:  /* vec_store_pairdfv8qi */
    case 1199:  /* vec_store_pairv2sfv8qi */
    case 1198:  /* vec_store_pairv2siv8qi */
    case 1197:  /* vec_store_pairv4hfv8qi */
    case 1196:  /* vec_store_pairv4hiv8qi */
    case 1195:  /* vec_store_pairv8qiv8qi */
    case 1194:  /* load_pairdfdf */
    case 1193:  /* load_pairv2sfdf */
    case 1192:  /* load_pairv2sidf */
    case 1191:  /* load_pairv4hfdf */
    case 1190:  /* load_pairv4hidf */
    case 1189:  /* load_pairv8qidf */
    case 1188:  /* load_pairdfv2sf */
    case 1187:  /* load_pairv2sfv2sf */
    case 1186:  /* load_pairv2siv2sf */
    case 1185:  /* load_pairv4hfv2sf */
    case 1184:  /* load_pairv4hiv2sf */
    case 1183:  /* load_pairv8qiv2sf */
    case 1182:  /* load_pairdfv2si */
    case 1181:  /* load_pairv2sfv2si */
    case 1180:  /* load_pairv2siv2si */
    case 1179:  /* load_pairv4hfv2si */
    case 1178:  /* load_pairv4hiv2si */
    case 1177:  /* load_pairv8qiv2si */
    case 1176:  /* load_pairdfv4hf */
    case 1175:  /* load_pairv2sfv4hf */
    case 1174:  /* load_pairv2siv4hf */
    case 1173:  /* load_pairv4hfv4hf */
    case 1172:  /* load_pairv4hiv4hf */
    case 1171:  /* load_pairv8qiv4hf */
    case 1170:  /* load_pairdfv4hi */
    case 1169:  /* load_pairv2sfv4hi */
    case 1168:  /* load_pairv2siv4hi */
    case 1167:  /* load_pairv4hfv4hi */
    case 1166:  /* load_pairv4hiv4hi */
    case 1165:  /* load_pairv8qiv4hi */
    case 1164:  /* load_pairdfv8qi */
    case 1163:  /* load_pairv2sfv8qi */
    case 1162:  /* load_pairv2siv8qi */
    case 1161:  /* load_pairv4hfv8qi */
    case 1160:  /* load_pairv4hiv8qi */
    case 1159:  /* load_pairv8qiv8qi */
    case 72:  /* store_pair_dw_tftf */
    case 71:  /* store_pair_dw_dfdf */
    case 70:  /* store_pair_dw_dfdi */
    case 69:  /* store_pair_dw_didf */
    case 68:  /* store_pair_dw_didi */
    case 67:  /* store_pair_sw_sfsf */
    case 66:  /* store_pair_sw_sisf */
    case 65:  /* store_pair_sw_sfsi */
    case 64:  /* store_pair_sw_sisi */
    case 63:  /* load_pair_dw_tftf */
    case 62:  /* load_pair_dw_dfdf */
    case 61:  /* load_pair_dw_dfdi */
    case 60:  /* load_pair_dw_didf */
    case 59:  /* load_pair_dw_didi */
    case 58:  /* load_pair_sw_sfsf */
    case 57:  /* load_pair_sw_sisf */
    case 56:  /* load_pair_sw_sfsi */
    case 55:  /* load_pair_sw_sisi */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XVECEXP (pat, 0, 0), 1));
      ro[2] = *(ro_loc[2] = &XEXP (XVECEXP (pat, 0, 1), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XVECEXP (pat, 0, 1), 1));
      break;

    case 49:  /* insv_immdi */
    case 48:  /* insv_immsi */
      ro[0] = *(ro_loc[0] = &XEXP (XEXP (pat, 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (pat, 0), 2));
      ro[2] = *(ro_loc[2] = &XEXP (pat, 1));
      break;

    case 3888:  /* *aarch64_sve_movvnx2bi */
    case 3887:  /* *aarch64_sve_movvnx4bi */
    case 3886:  /* *aarch64_sve_movvnx8bi */
    case 3885:  /* *aarch64_sve_movvnx16bi */
    case 3863:  /* *aarch64_sve_movvnx8df_le */
    case 3862:  /* *aarch64_sve_movvnx16sf_le */
    case 3861:  /* *aarch64_sve_movvnx32hf_le */
    case 3860:  /* *aarch64_sve_movvnx8di_le */
    case 3859:  /* *aarch64_sve_movvnx16si_le */
    case 3858:  /* *aarch64_sve_movvnx32hi_le */
    case 3857:  /* *aarch64_sve_movvnx64qi_le */
    case 3856:  /* *aarch64_sve_movvnx6df_le */
    case 3855:  /* *aarch64_sve_movvnx12sf_le */
    case 3854:  /* *aarch64_sve_movvnx24hf_le */
    case 3853:  /* *aarch64_sve_movvnx6di_le */
    case 3852:  /* *aarch64_sve_movvnx12si_le */
    case 3851:  /* *aarch64_sve_movvnx24hi_le */
    case 3850:  /* *aarch64_sve_movvnx48qi_le */
    case 3849:  /* *aarch64_sve_movvnx4df_le */
    case 3848:  /* *aarch64_sve_movvnx8sf_le */
    case 3847:  /* *aarch64_sve_movvnx16hf_le */
    case 3846:  /* *aarch64_sve_movvnx4di_le */
    case 3845:  /* *aarch64_sve_movvnx8si_le */
    case 3844:  /* *aarch64_sve_movvnx16hi_le */
    case 3843:  /* *aarch64_sve_movvnx32qi_le */
    case 3842:  /* *aarch64_sve_movvnx8df_le */
    case 3841:  /* *aarch64_sve_movvnx16sf_le */
    case 3840:  /* *aarch64_sve_movvnx32hf_le */
    case 3839:  /* *aarch64_sve_movvnx8di_le */
    case 3838:  /* *aarch64_sve_movvnx16si_le */
    case 3837:  /* *aarch64_sve_movvnx32hi_le */
    case 3836:  /* *aarch64_sve_movvnx64qi_le */
    case 3835:  /* *aarch64_sve_movvnx6df_le */
    case 3834:  /* *aarch64_sve_movvnx12sf_le */
    case 3833:  /* *aarch64_sve_movvnx24hf_le */
    case 3832:  /* *aarch64_sve_movvnx6di_le */
    case 3831:  /* *aarch64_sve_movvnx12si_le */
    case 3830:  /* *aarch64_sve_movvnx24hi_le */
    case 3829:  /* *aarch64_sve_movvnx48qi_le */
    case 3828:  /* *aarch64_sve_movvnx4df_le */
    case 3827:  /* *aarch64_sve_movvnx8sf_le */
    case 3826:  /* *aarch64_sve_movvnx16hf_le */
    case 3825:  /* *aarch64_sve_movvnx4di_le */
    case 3824:  /* *aarch64_sve_movvnx8si_le */
    case 3823:  /* *aarch64_sve_movvnx16hi_le */
    case 3822:  /* *aarch64_sve_movvnx32qi_le */
    case 3792:  /* *aarch64_sve_movvnx2df_be */
    case 3791:  /* *aarch64_sve_movvnx4sf_be */
    case 3790:  /* *aarch64_sve_movvnx8hf_be */
    case 3789:  /* *aarch64_sve_movvnx2di_be */
    case 3788:  /* *aarch64_sve_movvnx4si_be */
    case 3787:  /* *aarch64_sve_movvnx8hi_be */
    case 3786:  /* *aarch64_sve_movvnx16qi_be */
    case 3785:  /* *aarch64_sve_movvnx2df_le */
    case 3784:  /* *aarch64_sve_movvnx4sf_le */
    case 3783:  /* *aarch64_sve_movvnx8hf_le */
    case 3782:  /* *aarch64_sve_movvnx2di_le */
    case 3781:  /* *aarch64_sve_movvnx4si_le */
    case 3780:  /* *aarch64_sve_movvnx8hi_le */
    case 3779:  /* *aarch64_sve_movvnx16qi_le */
    case 3338:  /* *aarch64_be_movxi */
    case 3337:  /* *aarch64_be_movci */
    case 3336:  /* *aarch64_be_movoi */
    case 3309:  /* *aarch64_movxi */
    case 3308:  /* *aarch64_movci */
    case 3307:  /* *aarch64_movoi */
    case 1146:  /* *aarch64_simd_movv2df */
    case 1145:  /* *aarch64_simd_movv4sf */
    case 1144:  /* *aarch64_simd_movv8hf */
    case 1143:  /* *aarch64_simd_movv2di */
    case 1142:  /* *aarch64_simd_movv4si */
    case 1141:  /* *aarch64_simd_movv8hi */
    case 1140:  /* *aarch64_simd_movv16qi */
    case 1139:  /* *aarch64_simd_movv2sf */
    case 1138:  /* *aarch64_simd_movv2si */
    case 1137:  /* *aarch64_simd_movv4hf */
    case 1136:  /* *aarch64_simd_movv4hi */
    case 1135:  /* *aarch64_simd_movv8qi */
    case 54:  /* *movtf_aarch64 */
    case 53:  /* *movdf_aarch64 */
    case 52:  /* *movsf_aarch64 */
    case 51:  /* *movhf_aarch64 */
    case 50:  /* *movti_aarch64 */
    case 47:  /* *movdi_aarch64 */
    case 46:  /* *movsi_aarch64 */
    case 45:  /* *movhi_aarch64 */
    case 44:  /* *movqi_aarch64 */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (pat, 1));
      break;

    case 43:  /* *sibcall_value_insn */
    case 41:  /* *call_value_insn */
      ro[0] = *(ro_loc[0] = &XEXP (XVECEXP (pat, 0, 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 1));
      break;

    case 42:  /* *sibcall_insn */
    case 40:  /* *call_insn */
      ro[0] = *(ro_loc[0] = &XEXP (XEXP (XVECEXP (pat, 0, 0), 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XVECEXP (pat, 0, 0), 1));
      break;

    case 39:  /* *cbgedi1 */
    case 38:  /* *cbltdi1 */
    case 37:  /* *cbgesi1 */
    case 36:  /* *cbltsi1 */
    case 35:  /* *cbgehi1 */
    case 34:  /* *cblthi1 */
    case 33:  /* *cbgeqi1 */
    case 32:  /* *cbltqi1 */
      ro[0] = *(ro_loc[0] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 1), 0));
      break;

    case 31:  /* *tbnedi1 */
    case 30:  /* *tbeqdi1 */
    case 29:  /* *tbnesi1 */
    case 28:  /* *tbeqsi1 */
      ro[0] = *(ro_loc[0] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0), 2));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 1), 0));
      break;

    case 27:  /* *cbnedi1 */
    case 26:  /* *cbeqdi1 */
    case 25:  /* *cbnesi1 */
    case 24:  /* *cbeqsi1 */
      ro[0] = *(ro_loc[0] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 1), 0));
      break;

    case 20:  /* prefetch */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 0));
      ro[1] = *(ro_loc[1] = &XEXP (pat, 1));
      ro[2] = *(ro_loc[2] = &XEXP (pat, 2));
      break;

    case 1090:  /* speculation_barrier */
    case 1089:  /* bti_jc */
    case 1088:  /* bti_j */
    case 1087:  /* bti_c */
    case 1086:  /* bti_noarg */
    case 1071:  /* blockage */
    case 1070:  /* xpaclri */
    case 1069:  /* auti1716 */
    case 1068:  /* paci1716 */
    case 1067:  /* autisp */
    case 1066:  /* pacisp */
    case 23:  /* simple_return */
    case 22:  /* *do_return */
    case 21:  /* trap */
    case 19:  /* nop */
      break;

    case 18:  /* *casesi_dispatch */
      ro[0] = *(ro_loc[0] = &XVECEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0, 0));
      ro[1] = *(ro_loc[1] = &XVECEXP (XEXP (XEXP (XVECEXP (pat, 0, 0), 1), 0), 0, 1));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XVECEXP (pat, 0, 4), 0), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XVECEXP (pat, 0, 2), 0));
      ro[4] = *(ro_loc[4] = &XEXP (XVECEXP (pat, 0, 3), 0));
      break;

    case 17:  /* *compare_condjumpdi */
    case 16:  /* *compare_condjumpdi */
    case 15:  /* *compare_condjumpdi */
    case 14:  /* *compare_condjumpdi */
    case 13:  /* *compare_condjumpsi */
    case 12:  /* *compare_condjumpsi */
    case 11:  /* *compare_condjumpsi */
    case 10:  /* *compare_condjumpsi */
      ro[0] = *(ro_loc[0] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 0), 1));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (pat, 1), 1), 0));
      break;

    case 9:  /* condjump */
      ro[0] = *(ro_loc[0] = &XEXP (XEXP (pat, 1), 0));
      ro[1] = *(ro_loc[1] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (pat, 1), 1), 0));
      break;

    case 8:  /* fccmpedf */
    case 7:  /* fccmpesf */
    case 6:  /* fccmpdf */
    case 5:  /* fccmpsf */
    case 4:  /* ccmpdi */
    case 3:  /* ccmpsi */
      ro[0] = *(ro_loc[0] = &XEXP (XEXP (XEXP (pat, 1), 0), 0));
      ro[1] = *(ro_loc[1] = &XEXP (pat, 0));
      ro[2] = *(ro_loc[2] = &XEXP (XEXP (XEXP (pat, 1), 1), 0));
      ro[3] = *(ro_loc[3] = &XEXP (XEXP (XEXP (pat, 1), 1), 1));
      ro[4] = *(ro_loc[4] = &XEXP (XEXP (pat, 1), 0));
      ro[5] = *(ro_loc[5] = &XVECEXP (XEXP (XEXP (pat, 1), 2), 0, 0));
      break;

    case 684:  /* *andhi_compare0 */
    case 683:  /* *andqi_compare0 */
    case 2:  /* jump */
      ro[0] = *(ro_loc[0] = &XEXP (XEXP (pat, 1), 0));
      break;

    case 1:  /* indirect_jump */
      ro[0] = *(ro_loc[0] = &XEXP (pat, 1));
      break;

    }
}