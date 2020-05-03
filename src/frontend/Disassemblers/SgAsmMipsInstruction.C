/* SgAsmMipsInstruction member definitions.  Do not move them to src/ROSETTA/Grammar/BinaryInstruction.code (or any *.code
 * file) because then they won't get indexed/formatted/etc. by C-aware tools. */
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"

using namespace Rose;
using namespace Rose::BinaryAnalysis;

unsigned
SgAsmMipsInstruction::get_anyKind() const {
    return p_kind;
}

// see base class
bool
SgAsmMipsInstruction::terminatesBasicBlock()
{
    switch (get_kind()) {
        case mips_beq:
        case mips_beql:
        case mips_bgez:
        case mips_bgezal:
        case mips_bgezall:
        case mips_bgezl:
        case mips_bgtz:
        case mips_bgtzl:
        case mips_blez:
        case mips_blezl:
        case mips_bltz:
        case mips_bltzal:
        case mips_bltzall:
        case mips_bltzl:
        case mips_bne:
        case mips_bnel:
        case mips_break: // ???
        case mips_j:
        case mips_jal:
        case mips_jalr:
        case mips_jalx:
        case mips_jr:
        case mips_jr_hb:
        case mips_syscall:
        case mips_teq:
        case mips_teqi:
        case mips_tge:
        case mips_tgei:
        case mips_tgeiu:
        case mips_tgeu:
        case mips_tlt:
        case mips_tlti:
        case mips_tltiu:
        case mips_tltu:
        case mips_tne:
        case mips_tnei:
            return true;
        default:
            return false;
    }
}

// see base class
bool
SgAsmMipsInstruction::isFunctionCallFast(const std::vector<SgAsmInstruction*> &insns, rose_addr_t *target, rose_addr_t *return_va)
{
    if (insns.size()==0)
        return false;
    SgAsmMipsInstruction *last = isSgAsmMipsInstruction(insns.back());
    if (!last)
        return false;
    switch (last->get_kind()) {
        case mips_bgezal:
        case mips_bgezall:
        case mips_bltzal:
        case mips_bltzall:
        case mips_jal:
        case mips_jalr:
        case mips_jalr_hb:
        case mips_jalx: {
            (void) last->getBranchTarget(target); // target will not be changed if unknown
            if (return_va)
                *return_va = last->get_address() + last->get_size();
            return true;
        }
        default:
            return false;
    }
}

// see base class
bool
SgAsmMipsInstruction::isFunctionCallSlow(const std::vector<SgAsmInstruction*> &insns, rose_addr_t *target, rose_addr_t *return_va)
{
    return isFunctionCallFast(insns, target, return_va);
}

// see base class
bool
SgAsmMipsInstruction::isFunctionReturnFast(const std::vector<SgAsmInstruction*> &insns)
{
    if (insns.empty())
        return false;
    SgAsmMipsInstruction *last = isSgAsmMipsInstruction(insns.back());
    if (!last)
        return false;
    if (last->get_kind()!=mips_jr)
        return false;
    const SgAsmExpressionPtrList &args = last->get_operandList()->get_operands();
    if (args.size()<1)
        return false;
    SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(args[0]);
    if (!rre)
        return false;
    if (rre->get_descriptor().majorNumber()!=mips_regclass_gpr || rre->get_descriptor().minorNumber()!=31)
        return false;
    return true; // this is a "JR ra" instruction.
}

// see base class
bool
SgAsmMipsInstruction::isFunctionReturnSlow(const std::vector<SgAsmInstruction*> &insns)
{
    return isFunctionReturnFast(insns);
}

// see base class
std::set<rose_addr_t>
SgAsmMipsInstruction::getSuccessors(bool *complete_)
{
    bool complete = false;
    rose_addr_t target_va = 0;
    std::set<rose_addr_t> successors;
    switch (get_kind()) {
        case mips_break:
        case mips_j:
        case mips_jal:
        case mips_jalr:
        case mips_jalx:
        case mips_jr:
        case mips_jr_hb:
        case mips_syscall:
            // unconditional branch
            if ((complete=getBranchTarget(&target_va)))
                successors.insert(target_va);
            break;

        case mips_beq:
        case mips_beql:
        case mips_bgez:
        case mips_bgezal:
        case mips_bgezall:
        case mips_bgezl:
        case mips_bgtz:
        case mips_bgtzl:
        case mips_blez:
        case mips_blezl:
        case mips_bltz:
        case mips_bltzal:
        case mips_bltzall:
        case mips_bltzl:
        case mips_bne:
        case mips_bnel:
        case mips_teq:
        case mips_teqi:
        case mips_tge:
        case mips_tgei:
        case mips_tgeiu:
        case mips_tgeu:
        case mips_tlt:
        case mips_tlti:
        case mips_tltiu:
        case mips_tltu:
        case mips_tne:
        case mips_tnei:
            // conditional branch
            if ((complete=getBranchTarget(&target_va)))
                successors.insert(target_va);
            successors.insert(get_address() + get_size()); // fall through address
            break;

        default:
            // fall through
            successors.insert(get_address() + get_size());
            complete = true;
    }
    if (complete_)
        *complete_ = complete;
    return successors;
}

// see base class
bool
SgAsmMipsInstruction::isUnknown() const
{
    return mips_unknown_instruction == get_kind();
}

bool
SgAsmMipsInstruction::getBranchTarget(rose_addr_t *target)
{
    SgAsmExpressionPtrList &args = get_operandList()->get_operands();
    switch (get_kind()) {
        case mips_j:
        case mips_jal:
        case mips_jalx: {
            // target address stored in first argument
            assert(args.size()>=1);
            if (target) {
                SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(args[0]);
                assert(ival!=NULL);
                *target = ival->get_absoluteValue();
            }
            return true;
        }

        case mips_bgez:
        case mips_bgezal:
        case mips_bgezall:
        case mips_bgezl:
        case mips_bgtz:
        case mips_bgtzl:
        case mips_blez:
        case mips_blezl:
        case mips_bltz:
        case mips_bltzal:
        case mips_bltzall:
        case mips_bltzl: {
            // target address stored in the second argument
            assert(args.size()>=2);
            if (target) {
                SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(args[1]);
                assert(ival!=NULL);
                *target = ival->get_absoluteValue();
            }
            return true;
        }

        case mips_beq:
        case mips_beql:
        case mips_bne:
        case mips_bnel: {
            // target address stored in the third argument
            assert(args.size()>=3);
            if (target) {
                SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(args[2]);
                assert(ival!=NULL);
                *target = ival->get_absoluteValue();
            }
            return true;
        }

        default:
            // no known target; do not modify *target
            return false;
    }
}

std::string
SgAsmMipsInstruction::description() const {
    switch (get_kind()) {
        case mips_unknown_instruction:  return "";
        case mips_abs_s:                return "fp absolute value single precision";
        case mips_abs_d:                return "fp absolute value double precision";
        case mips_abs_ps:               return "fp absolute value pair of single precision";
        case mips_add:                  return "add word";
        case mips_add_s:                return "fp add single precision";
        case mips_add_d:                return "fp add double precision";
        case mips_add_ps:               return "fp add pair of single precision";
        case mips_addi:                 return "add immediate word";
        case mips_addiu:                return "add immediate unsigned word";
        case mips_addu:                 return "add unsigned word";
        case mips_alnv_ps:              return "fp align variable pair of single precision";
        case mips_and:                  return "bitwise logical AND";
        case mips_andi:                 return "bitwise logical AND immediate";
        case mips_bc1f:                 return "branch on FP false";
        case mips_bc1fl:                return "branch on FP false likely";
        case mips_bc1t:                 return "branch on FP true";
        case mips_bc1tl:                return "branch on FP true likely";
        case mips_bc2f:                 return "branch on COP2 false";
        case mips_bc2fl:                return "branch on COP2 false likely";
        case mips_bc2t:                 return "branch on COP2 true";
        case mips_bc2tl:                return "branch on COP2 true likely";
        case mips_beq:                  return "branch on equal";
        case mips_beql:                 return "branch on equal likely";
        case mips_bgez:                 return "branch on greater than or equal to zero";
        case mips_bgezal:               return "branch on greater than or equal to zero and link";
        case mips_bgezall:              return "branch on greater than or equal to zero and link likely";
        case mips_bgezl:                return "branch on greater than or equal to zero likely";
        case mips_bgtz:                 return "branch on greater than zero";
        case mips_bgtzl:                return "branch on greater than zero likely";
        case mips_blez:                 return "branch on less than or equal to zero";
        case mips_blezl:                return "branch on less than or equal to zero likely";
        case mips_bltz:                 return "branch on less than zero";
        case mips_bltzal:               return "branch on less than zero and link";
        case mips_bltzall:              return "branch on less than zero and link likely";
        case mips_bltzl:                return "branch on less than zero likely";
        case mips_bne:                  return "branch on not equal";
        case mips_bnel:                 return "branch on not equal likely";
        case mips_break:                return "break point";
        case mips_c_f_s:                return "fp compare false single precision";
        case mips_c_un_s:               return "fp compare unordered single precision";
        case mips_c_eq_s:               return "fp compare equal single precision";
        case mips_c_ueq_s:              return "fp compare unordered equal";
        case mips_c_olt_s:              return "fp compare ordered or less than single precision";
        case mips_c_ult_s:              return "fp compare unordered or less than single precision";
        case mips_c_ole_s:              return "fp compare ordered or less than or equal single precision";
        case mips_c_ule_s:              return "fp compare unordered or less than or equal single precision";
        case mips_c_sf_s:               return "fp compare signaling false single precision";
        case mips_c_ngle_s:             return "fp compare not greater than or less than or equal single precision";
        case mips_c_seq_s:              return "fp compare signaling equal single precision";
        case mips_c_ngl_s:              return "fp compare not greater than or less than single precision";
        case mips_c_lt_s:               return "fp compare less than single precision";
        case mips_c_nge_s:              return "fp compare not greater than or equal single precision";
        case mips_c_le_s:               return "fp compare less than or equal single precision";
        case mips_c_ngt_s:              return "fp compare not greater than single precision";
        case mips_c_f_d:                return "fp compare false double precision";
        case mips_c_un_d:               return "fp compare unordered double precision";
        case mips_c_eq_d:               return "fp compare equal double precision";
        case mips_c_ueq_d:              return "fp compare unordered equal";
        case mips_c_olt_d:              return "fp compare ordered or less than double precision";
        case mips_c_ult_d:              return "fp compare unordered or less than double precision";
        case mips_c_ole_d:              return "fp compare ordered or less than or equal double precision";
        case mips_c_ule_d:              return "fp compare unordered or less than or equal double precision";
        case mips_c_sf_d:               return "fp compare signaling false double precision";
        case mips_c_ngle_d:             return "fp compare not greater than or less than or equal double precision";
        case mips_c_seq_d:              return "fp compare signaling equal double precision";
        case mips_c_ngl_d:              return "fp compare not greater than or less than double precision";
        case mips_c_lt_d:               return "fp compare less than double precision";
        case mips_c_nge_d:              return "fp compare not greater than or equal double precision";
        case mips_c_le_d:               return "fp compare less than or equal double precision";
        case mips_c_ngt_d:              return "fp compare not greater than double precision";
        case mips_c_f_ps:               return "fp compare false pair of single precision";
        case mips_c_un_ps:              return "fp compare unordered pair of single precision";
        case mips_c_eq_ps:              return "fp compare equal pair of single precision";
        case mips_c_ueq_ps:             return "fp compare unordered equal";
        case mips_c_olt_ps:             return "fp compare ordered or less than pair of single precision";
        case mips_c_ult_ps:             return "fp compare unordered or less than pair of single precision";
        case mips_c_ole_ps:             return "fp compare ordered or less than or equal pair of single precision";
        case mips_c_ule_ps:             return "fp compare unordered or less than or equal pair of single precision";
        case mips_c_sf_ps:              return "fp compare signaling false pair of single precision";
        case mips_c_ngle_ps:            return "fp compare not greater than or less than or equal pair of single precision";
        case mips_c_seq_ps:             return "fp compare signaling equal pair of single precision";
        case mips_c_ngl_ps:             return "fp compare not greater than or less than pair of single precision";
        case mips_c_lt_ps:              return "fp compare less than pair of single precision";
        case mips_c_nge_ps:             return "fp compare not greater than or equal pair of single precision";
        case mips_c_le_ps:              return "fp compare less than or equal pair of single precision";
        case mips_c_ngt_ps:             return "fp compare not greater than pair of single precision";
        case mips_cache:                return "perform cache operation";
        case mips_cachee:               return "perform cache operation EVA";
        case mips_ceil_l_s:             return "fixed point ceiling convert to long fixed point";
        case mips_ceil_l_d:             return "fixed point ceiling convert to long fixed point";
        case mips_ceil_w_s:             return "fixed point ceiling convert to word fixed point";
        case mips_ceil_w_d:             return "fixed point ceiling convert to word fixed point";
        case mips_cfc1:                 return "copy control word from fp";
        case mips_cfc2:                 return "copy control word from coprocessor 2";
        case mips_clo:                  return "count leading ones in word";
        case mips_clz:                  return "count leading zeros in word";
        case mips_cop2:                 return "coprocessor operation to coprocessor 2";
        case mips_ctc1:                 return "copy control word to fp";
        case mips_ctc2:                 return "copy control word to coprocessor 2";
        case mips_cvt_d_s:              return "fp convert to double fp";
        case mips_cvt_d_w:              return "fp convert to double fp";
        case mips_cvt_d_l:              return "fp convert to double fp";
        case mips_cvt_l_s:              return "fp convert to long fixed point";
        case mips_cvt_l_d:              return "fp convert to long fixed point";
        case mips_cvt_ps_s:             return "fp convert to paired single";
        case mips_cvt_s_d:              return "fp convert to single fp";
        case mips_cvt_s_w:              return "fp convert to single fp";
        case mips_cvt_s_l:              return "fp convert to single fp";
        case mips_cvt_s_pl:             return "fp convert pair lower to single fp";
        case mips_cvt_s_pu:             return "fp convert pair upper to single fp";
        case mips_cvt_w_s:              return "fp convert to word fixed point";
        case mips_cvt_w_d:              return "fp convert to word fixed point";
        case mips_di:                   return "disable interrupts";
        case mips_div:                  return "divide word";
        case mips_div_s:                return "fp divide";
        case mips_div_d:                return "fp divide";
        case mips_divu:                 return "divide unsigned word";
        case mips_ehb:                  return "execution hazard barrier";
        case mips_ei:                   return "enable interrupts";
        case mips_eret:                 return "exception return";
        case mips_ext:                  return "extract bit field";
        case mips_floor_l_s:            return "fp floor convert to long fixed point";
        case mips_floor_l_d:            return "fp floor convert to long fixed point";
        case mips_floor_w_s:            return "fp floor convert to word fixed point";
        case mips_floor_w_d:            return "fp floor convert to word fixed point";
        case mips_ins:                  return "insert bit field";
        case mips_j:                    return "jump";
        case mips_jal:                  return "jump and link";
        case mips_jalr:                 return "jump and link register";
        case mips_jalr_hb:              return "jump and link register with hazard barrier";
        case mips_jalx:                 return "jump and link exchange";
        case mips_jr:                   return "jump register";
        case mips_jr_hb:                return "jump register with hazard barrier";
        case mips_lb:                   return "load byte";
        case mips_lbe:                  return "load byte EVA";
        case mips_lbu:                  return "load byte unsigned";
        case mips_lbue:                 return "load byte unsigned EVA";
        case mips_ldc1:                 return "load doubleword to fp";
        case mips_ldc2:                 return "load doubleword to coprocessor 2";
        case mips_ldxc1:                return "load doubleword indexed to fp";
        case mips_lh:                   return "load halfword";
        case mips_lhe:                  return "load halfword EVA";
        case mips_lhu:                  return "load halfword unsigned";
        case mips_lhue:                 return "load halfword unsigned EVA";
        case mips_ll:                   return "load linked word";
        case mips_lle:                  return "load linked word EVA";
        case mips_lui:                  return "load upper immediate";
        case mips_luxc1:                return "load doubleword indexed unaligned to fp";
        case mips_lw:                   return "load word";
        case mips_lwc1:                 return "load word to fp";
        case mips_lwc2:                 return "load word to coprocessor 2";
        case mips_lwe:                  return "load word EVA";
        case mips_lwl:                  return "load word left";
        case mips_lwle:                 return "load word left EVA";
        case mips_lwr:                  return "load word right";
        case mips_lwre:                 return "load word right EVA";
        case mips_lwxc1:                return "load word indexed to fp";
        case mips_madd:                 return "multiply and add word to hi, lo";
        case mips_madd_s:               return "fp multiply add";
        case mips_madd_d:               return "fp multiply add";
        case mips_madd_ps:              return "fp multiply add";
        case mips_maddu:                return "multiply and add unsigned word to hi, lo";
        case mips_mfc0:                 return "copy from coprocessor 0";
        case mips_mfc1:                 return "copy word from coprocessor 1";
        case mips_mfc2:                 return "copy word from coprocessor 2";
        case mips_mfhc1:                return "copy word from high half of fp register";
        case mips_mfhc2:                return "copy word from high half of coprocessor 2";
        case mips_mfhi:                 return "copy from hi register";
        case mips_mflo:                 return "copy from lo register";
        case mips_mov_s:                return "fp copy";
        case mips_mov_d:                return "fp copy";
        case mips_mov_ps:               return "fp copy";
        case mips_movf:                 return "copy conditional on fp false";
        case mips_movf_s:               return "fp copy conditional on fp false";
        case mips_movf_d:               return "fp copy conditional on fp false";
        case mips_movf_ps:              return "fp copy conditional on fp false";
        case mips_movn:                 return "copy conditional on not zero";
        case mips_movn_s:               return "copy fp conditional on not zero";
        case mips_movn_d:               return "copy fp conditional on not zero";
        case mips_movn_ps:              return "copy fp conditional on not zero";
        case mips_movt:                 return "copy conditional on floating piont true";
        case mips_movt_s:               return "fp copy conditional on fp true";
        case mips_movt_d:               return "fp copy conditional on fp true";
        case mips_movt_ps:              return "fp copy conditional on fp true";
        case mips_movz:                 return "copy conditional on zero";
        case mips_movz_s:               return "fp copy conditional on zero";
        case mips_movz_d:               return "fp copy conditional on zero";
        case mips_movz_ps:              return "fp copy conditional on zero";
        case mips_msub:                 return "multiply and subtract word";
        case mips_msub_s:               return "fp multiple and subtract";
        case mips_msub_d:               return "fp multiple and subtract";
        case mips_msub_ps:              return "fp multiple and subtract";
        case mips_msubu:                return "multiply and subtract word to hi, lo";
        case mips_mtc0:                 return "copy to coprocessor 0";
        case mips_mtc1:                 return "copy word to fp";
        case mips_mtc2:                 return "copy word to coprocessor 2";
        case mips_mthc1:                return "copy word to high half of fp register";
        case mips_mthc2:                return "copy word to high half of coprocessor 2 register";
        case mips_mthi:                 return "copy to hi register";
        case mips_mtlo:                 return "copy to lo register";
        case mips_mul:                  return "multiply word to GPR";
        case mips_mul_s:                return "fp multiply";
        case mips_mul_d:                return "fp multiply";
        case mips_mul_ps:               return "fp multiply";
        case mips_mult:                 return "multiply word";
        case mips_multu:                return "multiply unsigned word";
        case mips_neg_s:                return "fp negate";
        case mips_neg_d:                return "fp negate";
        case mips_neg_ps:               return "fp negate";
        case mips_nmadd_s:              return "fp negative multiply add";
        case mips_nmadd_d:              return "fp negative multiply add";
        case mips_nmadd_ps:             return "fp negative multiply add";
        case mips_nmsub_s:              return "fp negative multply subtract";
        case mips_nmsub_d:              return "fp negative multply subtract";
        case mips_nmsub_ps:             return "fp negative multply subtract";
        case mips_nop:                  return "no operation";
        case mips_nor:                  return "not OR";
        case mips_or:                   return "bitwise OR";
        case mips_ori:                  return "bitwise OR immediate";
        case mips_pause:                return "wait for the LLBit to clear";
        case mips_pll_ps:               return "pair lower lower";
        case mips_plu_ps:               return "pair lower upper";
        case mips_pref:                 return "prefetch";
        case mips_prefe:                return "prefetch EVA";
        case mips_prefx:                return "prefetch indexed";
        case mips_pul_ps:               return "pair upper lower";
        case mips_puu_ps:               return "pair upper upper";
        case mips_rdhwr:                return "read hardware register";
        case mips_rdpgpr:               return "read GPR from previous shadow set";
        case mips_recip_s:              return "reciprocal approximation";
        case mips_recip_d:              return "reciprocal approximation";
        case mips_rotr:                 return "rotate word right";
        case mips_rotrv:                return "rotate word right variable";
        case mips_round_l_s:            return "fp round to long fixed point";
        case mips_round_l_d:            return "fp round to long fixed point";
        case mips_round_w_s:            return "fp round to word fixed point";
        case mips_round_w_d:            return "fp round to word fixed point";
        case mips_rsqrt_s:              return "reciprocal square root approximation";
        case mips_rsqrt_d:              return "reciprocal square root approximation";
        case mips_sb:                   return "store byte";
        case mips_sbe:                  return "store byte EVA";
        case mips_sc:                   return "store conditional word";
        case mips_sce:                  return "store conditional word EVA";
        case mips_sdc1:                 return "store doubleword from fp";
        case mips_sdc2:                 return "store doubleword from coprocessor 2";
        case mips_sdxc1:                return "store doubleword indexed from fp";
        case mips_seb:                  return "sign extend byte";
        case mips_seh:                  return "sign extend halfword";
        case mips_sh:                   return "store halfword";
        case mips_she:                  return "store halfword EVA";
        case mips_sll:                  return "shift word left logical";
        case mips_sllv:                 return "shift word left logical variable";
        case mips_slt:                  return "set on less than";
        case mips_slti:                 return "set on less than immediate";
        case mips_sltiu:                return "set on less than immediate unsigned";
        case mips_sltu:                 return "set on less than unsigned";
        case mips_sqrt_s:               return "fp square root";
        case mips_sqrt_d:               return "fp square root";
        case mips_sra:                  return "shift word right arithmetic";
        case mips_srav:                 return "shift word right arithmetic variable";
        case mips_srl:                  return "shift right logical";
        case mips_srlv:                 return "shift right logical value";
        case mips_ssnop:                return "superscalar no operation";
        case mips_sub:                  return "subtract word";
        case mips_sub_s:                return "subtract fp";
        case mips_sub_d:                return "subtract fp";
        case mips_sub_ps:               return "subtract fp";
        case mips_subu:                 return "subtract unsigned word";
        case mips_suxc1:                return "store doubleword indexed unaligned from fp";
        case mips_sw:                   return "store word";
        case mips_swc1:                 return "store word from fp";
        case mips_swc2:                 return "store word from coprocessor 2";
        case mips_swe:                  return "store word EVA";
        case mips_swl:                  return "store word left";
        case mips_swle:                 return "store word left EVA";
        case mips_swr:                  return "store word right";
        case mips_swre:                 return "store word right EVA";
        case mips_swxc1:                return "store word indexed from fp";
        case mips_sync:                 return "synchronize";
        case mips_synci:                return "synchronize caches to make instruction writes effective";
        case mips_syscall:              return "system call";
        case mips_teq:                  return "trap if equal";
        case mips_teqi:                 return "trap if equal immediate";
        case mips_tge:                  return "trap if greater or equal";
        case mips_tgei:                 return "trap if greater or equal immediate";
        case mips_tgeiu:                return "trap if greater or equal immediate unsigned";
        case mips_tgeu:                 return "trap if greater or equal unsigned";
        case mips_tlbinv:               return "TLB invalidate";
        case mips_tlbinvf:              return "TLB invalidate flush";
        case mips_tlbp:                 return "probe TLB for matching entry";
        case mips_tlbr:                 return "read indexed TLB entry";
        case mips_tlbwi:                return "write indexed TLB entry";
        case mips_tlbwr:                return "write random TLB entry";
        case mips_tlt:                  return "trap if less than";
        case mips_tlti:                 return "trap if less than immediate";
        case mips_tltiu:                return "trap if less than immediate unsigned";
        case mips_tltu:                 return "trap if less than unsigned";
        case mips_tne:                  return "trap if not equal";
        case mips_tnei:                 return "trap if not equal immediate";
        case mips_trunc_l_s:            return "fp truncate to long fixed point";
        case mips_trunc_l_d:            return "fp truncate to long fixed point";
        case mips_trunc_w_s:            return "fp truncate to word fixed point";
        case mips_trunc_w_d:            return "fp truncate to word fixed point";
        case mips_wait:                 return "enter standby mode";
        case mips_wrpgpr:               return "write to GPR in previous shadow set";
        case mips_wsbh:                 return "word swap bytes within halfwords";
        case mips_xor:                  return "exclusive OR";
        case mips_xori:                 return "exclusive OR immediate";
        case mips_last_instruction:     ASSERT_not_reachable("not a valid mips instruction kind");
    }
    ASSERT_not_reachable("invalid mips instruction kind: " + StringUtility::numberToString(get_kind()));
}

#endif
