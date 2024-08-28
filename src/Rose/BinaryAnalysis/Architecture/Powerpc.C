#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Architecture/Powerpc.h>

#include <Rose/BinaryAnalysis/CallingConvention/StoragePool.h>
#include <Rose/BinaryAnalysis/Disassembler/Powerpc.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherPowerpc.h>
#include <Rose/BinaryAnalysis/Partitioner2/ModulesPowerpc.h>
#include <Rose/BinaryAnalysis/Unparser/Powerpc.h>
#include <stringify.h>                                  // ROSE

#include <SgAsmPowerpcInstruction.h>
#include <Cxx_GrammarDowncast.h>
#include <sageInterface.h>

#include <boost/lexical_cast.hpp>

namespace CC = Rose::BinaryAnalysis::CallingConvention;

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

Powerpc::Powerpc(size_t bytesPerWord, ByteOrder::Endianness byteOrder)
    : Base("ppc" + boost::lexical_cast<std::string>(8*bytesPerWord) +
           (ByteOrder::ORDER_MSB == byteOrder ? "-be" : (ByteOrder::ORDER_LSB == byteOrder ? "-el" : "")),
           bytesPerWord, byteOrder) {}

Powerpc::~Powerpc() {}

const CallingConvention::Dictionary&
Powerpc::callingConventions() const {
    static SAWYER_THREAD_TRAITS::Mutex mutex;
    SAWYER_THREAD_TRAITS::LockGuard lock(mutex);

    if (!callingConventions_.isCached()) {
        CC::Dictionary dict;
        dict.push_back(cc_ibm(bitsPerWord()));
        callingConventions_ = dict;
    }

    return callingConventions_.get();
}

Disassembler::Base::Ptr
Powerpc::newInstructionDecoder() const {
    auto decoder = Disassembler::Powerpc::instance(shared_from_this());
    decoder->strictReserved(true);
    return decoder;
}

Sawyer::Container::Interval<size_t>
Powerpc::bytesPerInstruction() const {
    return 4;
}

Alignment
Powerpc::instructionAlignment() const {
    return Alignment(4, bitsPerWord());
}

std::string
Powerpc::instructionMnemonic(const SgAsmInstruction *insn) const {
    if (isUnknown(insn))
        return "unknown";

    ASSERT_not_null(isSgAsmPowerpcInstruction(insn));
    const std::string s = stringify::Rose::BinaryAnalysis::PowerpcInstructionKind(insn->get_anyKind(), "powerpc_");
    if (s.size() > 7 && boost::ends_with(s, "_record")) {
        return s.substr(0, s.size()-7) + ".";
    } else {
        return s;
    }
}

std::string
Powerpc::instructionDescription(const SgAsmInstruction *insn_) const {
    auto insn = isSgAsmPowerpcInstruction(insn_);
    ASSERT_not_null(insn);
    switch (insn->get_kind()) {
        case powerpc_unknown_instruction:  return "";
        case powerpc_add:              return "add";
        case powerpc_add_record:       return "add";
        case powerpc_addo:             return "add check overflow";
        case powerpc_addo_record:      return "add check overflow";
        case powerpc_addc:             return "add carrying";
        case powerpc_addc_record:      return "add carrying";
        case powerpc_addco:            return "add carrying check overflow";
        case powerpc_addco_record:     return "add carrying check overflow";
        case powerpc_adde:             return "add extended";
        case powerpc_adde_record:      return "add extended";
        case powerpc_addeo:            return "add extended check overflow";
        case powerpc_addeo_record:     return "add extended check overflow";
        case powerpc_addi:             return "add immediate";
        case powerpc_addic:            return "add immediate carrying";
        case powerpc_addic_record:     return "add immediate carrying and record";
        case powerpc_addis:            return "add immediate shifted";
        case powerpc_addme:            return "add to minus one extended";
        case powerpc_addme_record:     return "add to minus one extended";
        case powerpc_addmeo:           return "add to minus one extended check overflow";
        case powerpc_addmeo_record:    return "add to minus one extended check overflow";
        case powerpc_addze:            return "add to zero extended";
        case powerpc_addze_record:     return "add to zero extended";
        case powerpc_addzeo:           return "add to zero extended check overflow";
        case powerpc_addzeo_record:    return "add to zero extended check overflow";
        case powerpc_and:              return "AND";
        case powerpc_and_record:       return "AND";
        case powerpc_andc:             return "AND with complement";
        case powerpc_andc_record:      return "AND with complement";
        case powerpc_andi_record:      return "AND immediate";
        case powerpc_andis_record:     return "AND immediate shifted";
        case powerpc_b:                return "branch relative";
        case powerpc_ba:               return "branch absolute";
        case powerpc_bl:               return "branch relative and link";
        case powerpc_bla:              return "branch absolute and link";
        case powerpc_bc:               return "branch " + insn->conditionalBranchDescription();
        case powerpc_bca:              return "branch " + insn->conditionalBranchDescription();
        case powerpc_bcl:              return "branch and link " + insn->conditionalBranchDescription();
        case powerpc_bcla:             return "branch and link " + insn->conditionalBranchDescription();
        case powerpc_bcctr:            return "branch to count register " + insn->conditionalBranchDescription();
        case powerpc_bcctrl:           return "branch to count register and link " + insn->conditionalBranchDescription();
        case powerpc_bclr:             return "branch to link register " + insn->conditionalBranchDescription();
        case powerpc_bclrl:            return "branch to link register and link " + insn->conditionalBranchDescription();
        case powerpc_cmp:              return "signed compare";
        case powerpc_cmpi:             return "signed compare immediate";
        case powerpc_cmpl:             return "unsigned compare"; // "l" for "logical", meaning "unsigned"
        case powerpc_cmpli:            return "unsigned compare immediate"; // "l" for "logical", meaning "unsigned"
        case powerpc_cntlzd:           return "count leading zeros doubleword";
        case powerpc_cntlzd_record:    return "count leading zeros doubleword";
        case powerpc_cntlzw:           return "count leading zeros word";
        case powerpc_cntlzw_record:    return "count leading zeros word";
        case powerpc_crand:            return "condition register AND";
        case powerpc_crandc:           return "condition register AND with complement";
        case powerpc_creqv:            return "condition register equivalent";
        case powerpc_crnand:           return "condition register NAND";
        case powerpc_crnor:            return "condition register NOR";
        case powerpc_cror:             return "condition register OR";
        case powerpc_crorc:            return "condition register OR with complement";
        case powerpc_crxor:            return "condition register XOR";
        case powerpc_dcbf:             return "data cache block flush";
        case powerpc_dcba:             return "data cache block allocate";
        case powerpc_dcbi:             return "data cache block invalidate";
        case powerpc_dcbst:            return "data cache block store";
        case powerpc_dcbt:             return "data cache block touch";
        case powerpc_dcbtst:           return "data cache block touch for store";
        case powerpc_dcbz:             return "data cache block set to zero";
        case powerpc_divd:             return "divide doubleword";
        case powerpc_divd_record:      return "divide doubleword";
        case powerpc_divdo:            return "divide doubleword check overflow";
        case powerpc_divdo_record:     return "divide doubleword check overflow";
        case powerpc_divdu:            return "divide doubleword unsigned";
        case powerpc_divdu_record:     return "divide doubleword unsigned";
        case powerpc_divduo:           return "divide doubleword unsigned check overflow";
        case powerpc_divduo_record:    return "divide doubleword unsigned check overflow";
        case powerpc_divw:             return "divide word";
        case powerpc_divw_record:      return "divide word";
        case powerpc_divwo:            return "divide word check overflow";
        case powerpc_divwo_record:     return "divide word check overflow";
        case powerpc_divwu:            return "divide word unsigned";
        case powerpc_divwu_record:     return "divide word unsigned";
        case powerpc_divwuo:           return "divide word unsigned check overflow";
        case powerpc_divwuo_record:    return "divide word unsigned check overflow";
        case powerpc_dst:              return "data stream touch";
        case powerpc_dstt:             return "data stream touch";
        case powerpc_dstst:            return "data stream touch for store";
        case powerpc_dststt:           return "data stream touch for store";
        case powerpc_dss:              return "data stream stop";
        case powerpc_dssall:           return "data stream stop all";
        case powerpc_eciwx:            return "external control in word indexed (opt.)";
        case powerpc_ecowx:            return "external control out word indexed (opt.)";
        case powerpc_eieio:            return "enforce in-order execution of I/O";
        case powerpc_eqv:              return "equivalent";
        case powerpc_eqv_record:       return "equivalent";
        case powerpc_extsb:            return "extend sign byte";
        case powerpc_extsb_record:     return "extend sign byte";
        case powerpc_extsh:            return "extend sign halfword";
        case powerpc_extsh_record:     return "extend sign halfword";
        case powerpc_extsw:            return "extend sign word";
        case powerpc_extsw_record:     return "extend sign word";
        case powerpc_fabs:             return "fp absolute value";
        case powerpc_fabs_record:      return "fp absolute value";
        case powerpc_fadd:             return "fp add";
        case powerpc_fadd_record:      return "fp add";
        case powerpc_fadds:            return "fp add single";
        case powerpc_fadds_record:     return "fp add single";
        case powerpc_fcfid:            return "fp convert from integer doubleword";
        case powerpc_fcfid_record:     return "fp convert from integer doubleword";
        case powerpc_fcmpo:            return "fp compare ordered";
        case powerpc_fcmpu:            return "fp compare unordered";
        case powerpc_fctid:            return "fp convert to integer doubleword";
        case powerpc_fctid_record:     return "fp convert to integer doubleword";
        case powerpc_fctidz:           return "fp convert to integer doubleword with round toward zero";
        case powerpc_fctidz_record:    return "fp convert to integer doubleword with round toward zero";
        case powerpc_fctiw:            return "fp convert to integer word";
        case powerpc_fctiw_record:     return "fp convert to integer word";
        case powerpc_fctiwz:           return "fp convert to integer word with round to zero";
        case powerpc_fctiwz_record:    return "fp convert to integer word with round to zero";
        case powerpc_fdiv:             return "fp divide";
        case powerpc_fdiv_record:      return "fp divide";
        case powerpc_fdivs:            return "fp divide single";
        case powerpc_fdivs_record:     return "fp divide single";
        case powerpc_fmadd:            return "fp multiply-add";
        case powerpc_fmadd_record:     return "fp multiply-add";
        case powerpc_fmadds:           return "fp multiply-add single";
        case powerpc_fmadds_record:    return "fp multiply-add single";
        case powerpc_fmr:              return "fp copy register";
        case powerpc_fmr_record:       return "fp copy register";
        case powerpc_fmsub:            return "fp multiply-subtract";
        case powerpc_fmsub_record:     return "fp multiply-subtract";
        case powerpc_fmsubs:           return "fp multiply-subtract single";
        case powerpc_fmsubs_record:    return "fp multiply-subtract single";
        case powerpc_fmul:             return "fp multiply";
        case powerpc_fmul_record:      return "fp multiply";
        case powerpc_fmuls:            return "fp multiply single";
        case powerpc_fmuls_record:     return "fp multiply single";
        case powerpc_fnabs:            return "fp negative absolute value";
        case powerpc_fnabs_record:     return "fp negative absolute value";
        case powerpc_fneg:             return "fp negate";
        case powerpc_fneg_record:      return "fp negate";
        case powerpc_fnmadd:           return "fp negative multiply-add";
        case powerpc_fnmadd_record:    return "fp negative multiply-add";
        case powerpc_fnmadds:          return "fp negative multiply-add single";
        case powerpc_fnmadds_record:   return "fp negative multiply-add single";
        case powerpc_fnmsub:           return "fp negative multiply-subtract";
        case powerpc_fnmsub_record:    return "fp negative multiply-subtract";
        case powerpc_fnmsubs:          return "fp negative multiply-subtract single";
        case powerpc_fnmsubs_record:   return "fp negative multiply-subtract single";
        case powerpc_fpmul:            return "FP2 fp parallel multiply (BGL specific)";
        case powerpc_fxmul:            return "FP2 fp cross multiply (BGL specific)";
        case powerpc_fxpmul:           return "FP2 fp cross copy-primary multiply (BGL specific)";
        case powerpc_fxsmul:           return "FP2 fp cross copy-secondary multiply (BGL specific)";
        case powerpc_fpadd:            return "FP2 fp parallel add (BGL specific)";
        case powerpc_fpsub:            return "FP2 fp parallel subtract (BGL specific)";
        case powerpc_fpre:             return "FP2 fp parallel reciprocal estimate (BGL specific)";
        case powerpc_fprsqrte:         return "FP2 fp parallel reciprocal square root estimate (BGL specific)";
        case powerpc_fpmr:             return "";
        case powerpc_fpabs:            return "";
        case powerpc_lfssx:            return "";
        case powerpc_fpneg:            return "";
        case powerpc_lfssux:           return "";
        case powerpc_fprsp:            return "";
        case powerpc_lfsdx:            return "";
        case powerpc_fpnabs:           return "";
        case powerpc_lfsdux:           return "";
        case powerpc_lfxsx:            return "";
        case powerpc_fsmr:             return "";
        case powerpc_lfxsux:           return "";
        case powerpc_lfxdx:            return "";
        case powerpc_fsabs:            return "";
        case powerpc_lfxdux:           return "";
        case powerpc_lfpsx:            return "";
        case powerpc_fsneg:            return "";
        case powerpc_lfpsux:           return "";
        case powerpc_lfpdx:            return "";
        case powerpc_fsnabs:           return "";
        case powerpc_lfpdux:           return "";
        case powerpc_stfpiwx:          return "";
        case powerpc_fxmr:             return "";
        case powerpc_fpctiw:           return "";
        case powerpc_stfssx:           return "";
        case powerpc_stfssux:          return "";
        case powerpc_fpctiwz:          return "";
        case powerpc_stfsdx:           return "";
        case powerpc_stfsdux:          return "";
        case powerpc_stfxsx:           return "";
        case powerpc_fsmtp:            return "";
        case powerpc_stfxsux:          return "";
        case powerpc_stfxdx:           return "";
        case powerpc_stfxdux:          return "";
        case powerpc_stfpsx:           return "";
        case powerpc_fsmfp:            return "";
        case powerpc_stfpsux:          return "";
        case powerpc_stfpdx:           return "";
        case powerpc_stfpdux:          return "";
        case powerpc_fpsel:            return "";
        case powerpc_fpmadd:           return "";
        case powerpc_fpmsub:           return "";
        case powerpc_fxmadd:           return "";
        case powerpc_fxcpmadd:         return "";
        case powerpc_fxcsmadd:         return "";
        case powerpc_fpnmadd:          return "";
        case powerpc_fxnmadd:          return "";
        case powerpc_fxcpnmadd:        return "";
        case powerpc_fxcsnmadd:        return "";
        case powerpc_fxcpnpma:         return "";
        case powerpc_fxmsub:           return "";
        case powerpc_fxcsnpma:         return "";
        case powerpc_fxcpmsub:         return "";
        case powerpc_fxcpnsma:         return "";
        case powerpc_fxcsmsub:         return "";
        case powerpc_fxcsnsma:         return "";
        case powerpc_fpnmsub:          return "";
        case powerpc_fxcxma:           return "";
        case powerpc_fxnmsub:          return "";
        case powerpc_fxcxnpma:         return "";
        case powerpc_fxcpnmsub:        return "";
        case powerpc_fxcxnsma:         return "";
        case powerpc_fxcsnmsub:        return "";
        case powerpc_fxcxnms:          return "last FP2 specific enum value";
        case powerpc_fre:              return "fp reciprocal estimate single (optional)";
        case powerpc_fre_record:       return "fp reciprocal estimate single (optional)";
        case powerpc_fres:             return "fp reciprocal estimate single (optional)";
        case powerpc_fres_record:      return "fp reciprocal estimate single (optional)";
        case powerpc_frsp:             return "fp round to single precision";
        case powerpc_frsp_record:      return "fp round to single precision";
        case powerpc_frsqrte:          return "fp reciprocal square root estimate (optional)";
        case powerpc_frsqrte_record:   return "fp reciprocal square root estimate (optional)";
        case powerpc_frsqrtes:         return "fp reciprocal square root estimate (optional)";
        case powerpc_frsqrtes_record:  return "fp reciprocal square root estimate (optional)";
        case powerpc_fsel:             return "fp select (optional)";
        case powerpc_fsel_record:      return "fp select (optional)";
        case powerpc_fsqrt:            return "fp square root (optional)";
        case powerpc_fsqrt_record:     return "fp square root (optional)";
        case powerpc_fsqrts:           return "fp square root (optional)";
        case powerpc_fsqrts_record:    return "fp square root (optional)";
        case powerpc_fsub:             return "fp subtract";
        case powerpc_fsub_record:      return "fp subtract";
        case powerpc_fsubs:            return "fp subtract single";
        case powerpc_fsubs_record:     return "fp subtract single";
        case powerpc_icbi:             return "instruction cache block invalidate";
        case powerpc_isync:            return "instruction synchronize";
        case powerpc_lbz:              return "load byte and zero";
        case powerpc_lbzu:             return "load byte and zero with update";
        case powerpc_lbzux:            return "load byte and zero with update indexed";
        case powerpc_lbzx:             return "load byte and zero indexed";
        case powerpc_ld:               return "load doubleword";
        case powerpc_ldarx:            return "load doubleword and reserve indexed";
        case powerpc_ldu:              return "load doubleword with update";
        case powerpc_ldux:             return "load doubleword with update indexed";
        case powerpc_ldx:              return "load doubleword indexed";
        case powerpc_lfd:              return "load fp double";
        case powerpc_lfdu:             return "load fp double with update";
        case powerpc_lfdux:            return "load fp double with update indexed";
        case powerpc_lfdx:             return "load fp double indexed";
        case powerpc_lfs:              return "load fp single";
        case powerpc_lfsu:             return "load fp single with update";
        case powerpc_lfsux:            return "load fp single with update indexed";
        case powerpc_lfsx:             return "load fp single indexed";
        case powerpc_lha:              return "load half algebraic";
        case powerpc_lhau:             return "load half algebraic with update";
        case powerpc_lhaux:            return "load half algebraic with update indexed";
        case powerpc_lhax:             return "load half algebraic indexed";
        case powerpc_lhbrx:            return "load half byte-reversed indexed";
        case powerpc_lhz:              return "load half and zero";
        case powerpc_lhzu:             return "load half and zero with update";
        case powerpc_lhzux:            return "load half and zero with update indexed";
        case powerpc_lhzx:             return "load half and zero indexed";
        case powerpc_lmw:              return "load multiple word";
        case powerpc_lswi:             return "load string word immediate";
        case powerpc_lswx:             return "load string word indexed";
        case powerpc_lwa:              return "load word algebraic";
        case powerpc_lwarx:            return "load word and reserve indexed";
        case powerpc_lwaux:            return "load word algebraic with update indexed";
        case powerpc_lwax:             return "load word algebraic indexed";
        case powerpc_lwbrx:            return "load word byte-reversed indexed";
        case powerpc_lwz:              return "load word and zero";
        case powerpc_lwzu:             return "load word with zero update";
        case powerpc_lwzux:            return "load word and zero with update indexed";
        case powerpc_lwzx:             return "load word and zero indexed";
        case powerpc_mcrf:             return "copy condition register field";
        case powerpc_mcrfs:            return "copy to condition register from FPSCR";
        case powerpc_mcrxr:            return "copy to condition register from XER";
        case powerpc_mfcr:             return "copy from condition register";
        case powerpc_mffs:             return "copy from FPSCR";
        case powerpc_mffs_record:      return "copy from FPSCR";
        case powerpc_mfmsr:            return "copy from machine state register";
        case powerpc_mfspr:            return "copy from special-purpose register";
        case powerpc_mfsr:             return "copy from segment register";
        case powerpc_mfsrin:           return "copy from segment register indirect";
        case powerpc_mftb:             return "copy from time base";
        case powerpc_mtcrf:            return "copy to condition register fields";
        case powerpc_mtfsb0:           return "copy to FPSCR bit 0";
        case powerpc_mtfsb0_record:    return "copy to FPSCR bit 0";
        case powerpc_mtfsb1:           return "copy to FPSCR bit 1";
        case powerpc_mtfsb1_record:    return "copy to FPSCR bit 1";
        case powerpc_mtfsf:            return "copy to FPSCR fields";
        case powerpc_mtfsf_record:     return "copy to FPSCR fields";
        case powerpc_mtfsfi:           return "copy to FPSCR field immediate";
        case powerpc_mtfsfi_record:    return "copy to FPSCR field immediate";
        case powerpc_mtmsr:            return "copy to machine state register";
        case powerpc_mtmsrd:           return "copy to machine state register";
        case powerpc_mtspr:            return "copy to special-purpose register";
        case powerpc_mtsr:             return "copy to segment register";
        case powerpc_mtsrd:            return "copy to segment register";
        case powerpc_mtsrdin:          return "copy to segment register indirect";
        case powerpc_mtsrin:           return "copy to segment register indirect";
        case powerpc_mulhd:            return "multiply high doubleword";
        case powerpc_mulhd_record:     return "multiply high doubleword";
        case powerpc_mulhdu:           return "multiply high doubleword unsigned";
        case powerpc_mulhdu_record:    return "multiply high doubleword unsigned";
        case powerpc_mulhw:            return "multiply high word";
        case powerpc_mulhw_record:     return "multiply high word";
        case powerpc_mulhwu:           return "multiply high word unsigned";
        case powerpc_mulhwu_record:    return "multiply high word unsigned";
        case powerpc_mulld:            return "multiply low doubleword";
        case powerpc_mulld_record:     return "multiply low doubleword";
        case powerpc_mulldo:           return "multiply low doubleword check overflow";
        case powerpc_mulldo_record:    return "multiply low doubleword check overflow";
        case powerpc_mulli:            return "multiply low immediate";
        case powerpc_mullw:            return "multiply low word";
        case powerpc_mullw_record:     return "multiply low word";
        case powerpc_mullwo:           return "multiply low word check overflow";
        case powerpc_mullwo_record:    return "multiply low word check overflow";
        case powerpc_nand:             return "NAND";
        case powerpc_nand_record:      return "NAND";
        case powerpc_neg:              return "negate";
        case powerpc_neg_record:       return "negate";
        case powerpc_nego:             return "negate check overflow";
        case powerpc_nego_record:      return "negate check overflow";
        case powerpc_nor:              return "NOR";
        case powerpc_nor_record:       return "NOR";
        case powerpc_or:               return "OR";
        case powerpc_or_record:        return "OR";
        case powerpc_orc:              return "OR with complement";
        case powerpc_orc_record:       return "OR with complement";
        case powerpc_ori:              return "OR immediate";
        case powerpc_oris:             return "OR immediate shifted";
        case powerpc_popcntb:          return "population count bytes";
        case powerpc_rfi:              return "return from interrupt";
        case powerpc_rfid:             return "return from interrupt";
        case powerpc_rldcl:            return "rotate left doubleword then clear left";
        case powerpc_rldcl_record:     return "rotate left doubleword then clear left";
        case powerpc_rldcr:            return "rotate left doubleword then clear right";
        case powerpc_rldcr_record:     return "rotate left doubleword then clear right";
        case powerpc_rldic:            return "rotate left doubleword immediate then clear";
        case powerpc_rldic_record:     return "rotate left doubleword immediate then clear";
        case powerpc_rldicl:           return "rotate left doubleword immediate then clear left";
        case powerpc_rldicl_record:    return "rotate left doubleword immediate then clear left";
        case powerpc_rldicr:           return "rotate left doubleword immediate then clear right";
        case powerpc_rldicr_record:    return "rotate left doubleword immediate then clear right";
        case powerpc_rldimi:           return "rotate left doubleword immediate then mask insert";
        case powerpc_rldimi_record:    return "rotate left doubleword immediate then mask insert";
        case powerpc_rlwimi:           return "rotate left word immediate then mask insert";
        case powerpc_rlwimi_record:    return "rotate left word immediate then mask insert";
        case powerpc_rlwinm:           return "rotate left word immediate then AND with mask";
        case powerpc_rlwinm_record:    return "rotate left word immediate then AND with mask";
        case powerpc_rlwnm:            return "rotate left word then AND with mask";
        case powerpc_rlwnm_record:     return "rotate left word then AND with mask";
        case powerpc_sc:               return "system call";
        case powerpc_slbia:            return "SLB invalidate all";
        case powerpc_slbie:            return "SLB invalidate entry";
        case powerpc_sld:              return "shift left doubleword";
        case powerpc_sld_record:       return "shift left doubleword";
        case powerpc_slw:              return "shift left word";
        case powerpc_slw_record:       return "shift left word";
        case powerpc_srad:             return "shift right algebraic doubleword";
        case powerpc_srad_record:      return "shift right algebraic doubleword";
        case powerpc_sradi:            return "shift right algebraic doubleword immediate";
        case powerpc_sradi_record:     return "shift right algebraic doubleword immediate";
        case powerpc_srd:              return "shift right doubleword";
        case powerpc_srd_record:       return "shift right doubleword";
        case powerpc_sraw:             return "shift right algebraic word";
        case powerpc_sraw_record:      return "shift right algebraic word";
        case powerpc_srawi:            return "shift right algebraic word immediate";
        case powerpc_srawi_record:     return "shift right algebraic word immediate";
        case powerpc_srw:              return "shift right word";
        case powerpc_srw_record:       return "shift right word";
        case powerpc_stb:              return "store byte";
        case powerpc_stbu:             return "store byte with update";
        case powerpc_stbux:            return "store byte with update indexed";
        case powerpc_stbx:             return "store byte indexed";
        case powerpc_std:              return "store doubleword";
        case powerpc_stdcx_record:     return "store doubleword conditional indexed";
        case powerpc_stdu:             return "store doubleword with update";
        case powerpc_stdux:            return "store doubleword with update indexed";
        case powerpc_stdx:             return "store doubleword indexed";
        case powerpc_stfd:             return "store fp double";
        case powerpc_stfdu:            return "store fp double with update";
        case powerpc_stfdux:           return "store fp double with update indexed";
        case powerpc_stfdx:            return "store fp double indexed";
        case powerpc_stfiwx:           return "store fp as integer word indexed (optional)";
        case powerpc_stfs:             return "store fp single";
        case powerpc_stfsu:            return "store fp single with update";
        case powerpc_stfsux:           return "store fp single with update indexed";
        case powerpc_stfsx:            return "store fp single indexed";
        case powerpc_sth:              return "store half";
        case powerpc_sthbrx:           return "store half byte-reverse indexed";
        case powerpc_sthu:             return "store half with update";
        case powerpc_sthux:            return "store half with update indexed";
        case powerpc_sthx:             return "store half indexed";
        case powerpc_stmw:             return "store multiple word";
        case powerpc_stswi:            return "store string word immediate";
        case powerpc_stswx:            return "store string word indexed";
        case powerpc_stw:              return "store";
        case powerpc_stwbrx:           return "store word byte-reversed indexed";
        case powerpc_stwcx_record:     return "store word conditional indexed";
        case powerpc_stwu:             return "store word with update";
        case powerpc_stwux:            return "store word with update indexed";
        case powerpc_stwx:             return "store word indexed";
        case powerpc_subf:             return "subtract from";
        case powerpc_subf_record:      return "subtract from";
        case powerpc_subfo:            return "subtract from check overflow";
        case powerpc_subfo_record:     return "subtract from check overflow";
        case powerpc_subfc:            return "subtract from carrying";
        case powerpc_subfc_record:     return "subtract from carrying";
        case powerpc_subfco:           return "subtract from carrying check overflow";
        case powerpc_subfco_record:    return "subtract from carrying check overflow";
        case powerpc_subfe:            return "subtract from extended";
        case powerpc_subfe_record:     return "subtract from extended";
        case powerpc_subfeo:           return "subtract from extended check overflow";
        case powerpc_subfeo_record:    return "subtract from extended check overflow";
        case powerpc_subfic:           return "subtract from immediate carrying";
        case powerpc_subfme:           return "subtract from minus one extended";
        case powerpc_subfme_record:    return "subtract from minus one extended";
        case powerpc_subfmeo:          return "subtract from minus one extended check overflow";
        case powerpc_subfmeo_record:   return "subtract from minus one extended check overflow";
        case powerpc_subfze:           return "subtract from zero extended";
        case powerpc_subfze_record:    return "subtract from zero extended";
        case powerpc_subfzeo:          return "subtract from zero extended check overflow";
        case powerpc_subfzeo_record:   return "subtract from zero extended check overflow";
        case powerpc_sync:             return "synchronize";
        case powerpc_td:               return "trap doubleword";
        case powerpc_tdi:              return "trap doubleword immediate";
        case powerpc_tlbia:            return "translation look-aside buffer invalidate all (optional)";
        case powerpc_tlbie:            return "translation look-aside buffer invalidate entry (optional)";
        case powerpc_tlbsync:          return "translation look-aside buffer synchronize (optional)";
        case powerpc_tw:               return "trap word";
        case powerpc_twi:              return "trap word immediate";
        case powerpc_xor:              return "XOR";
        case powerpc_xor_record:       return "XOR";
        case powerpc_xori:             return "XOR immediate";
        case powerpc_xoris:            return "XOR immediate shift";
        case powerpc_last_instruction: ASSERT_not_reachable("not a valid powerpc instruction kind");
    }
    ASSERT_not_reachable("invalid powerpc instruction kind: " + StringUtility::numberToString(insn->get_kind()));
}

bool
Powerpc::isUnknown(const SgAsmInstruction *insn_) const {
    auto insn = isSgAsmPowerpcInstruction(insn_);
    ASSERT_not_null(insn);
    return powerpc_unknown_instruction == insn->get_kind();
}

bool
Powerpc::terminatesBasicBlock(SgAsmInstruction *insn_) const {
    auto insn = isSgAsmPowerpcInstruction(insn_);
    ASSERT_not_null(insn);
    switch (insn->get_kind()) {
        case powerpc_unknown_instruction:
        case powerpc_b:         /* branch instructions... */
        case powerpc_ba:
        case powerpc_bl:
        case powerpc_bla:
        case powerpc_bc:
        case powerpc_bca:
        case powerpc_bcl:
        case powerpc_bcla:
        case powerpc_bcctr:
        case powerpc_bcctrl:
        case powerpc_bclr:
        case powerpc_bclrl:
        case powerpc_tw:        /* trap instructions... */
        case powerpc_twi:
        case powerpc_sc:        /* system call */
        case powerpc_rfi:       /* return from interrupt */
            return true;

        default:
            return false;
    }
}

bool
Powerpc::isFunctionCallFast(const std::vector<SgAsmInstruction*> &insns, rose_addr_t *target, rose_addr_t *return_va) const {
    if (insns.empty())
        return false;
    auto insn = isSgAsmPowerpcInstruction(insns.back());
    ASSERT_not_null(insn);

    // Quick method based only on the kind of instruction
    rose_addr_t tgt;
    if (insn->get_kind() == powerpc_bl && insn->nOperands() == 1 && insn->operand(0)->asUnsigned().assignTo(tgt)) {
        if (target)
            *target = tgt;
        if (return_va)
            *return_va = insn->get_address() + insn->get_size();
        return true;
    } else if (insn->get_kind() == powerpc_bclrl && insn->nOperands() == 3 &&
               (insn->operand(0)->asUnsigned().orElse(0) & 0x14) == 0x14 &&
               insn->operand(2)->asUnsigned().orElse(1) == 0) {
        // Indirect function call, assuming the LR register is dynamically initialized with the target address.
        if (return_va)
            *return_va = insn->get_address() + insn->get_size();
        return true;
    } else if (insn->get_kind() == powerpc_bcctrl && insn->nOperands() == 3 &&
               (insn->operand(0)->asUnsigned().orElse(0) & 0x14) == 0x14 &&
               insn->operand(2)->asUnsigned().orElse(1) == 0) {
        // Indirect function call, as in:
        //   mtspr    ctr, r9                                  ; copy to special-purpose register
        //   bcctrl   0x14<20>, cr0.lt, 0                      ; branch to count register and link unconditionally
        if (return_va)
            *return_va = insn->get_address() + insn->get_size();
        return true;
    }

    return false;
}

bool
Powerpc::isFunctionReturnFast(const std::vector<SgAsmInstruction*> &insns) const {
    if (insns.empty())
        return false;
    auto last = isSgAsmPowerpcInstruction(insns.back());
    ASSERT_not_null(last);

    // Quick method based only on the kind of instruction.  Returns are normally coded as
    //    BCLR BO, BI, BH
    // where the BO field is the 5-bit constant 0b1x1xx where the x means 0 or 1
    // where the BI field is anything (usually zero)
    // where the BH field is zero
    return (last->get_kind() == powerpc_bclr && last->nOperands() == 3 &&
            (last->operand(0)->asUnsigned().orElse(0) & 0x14) == 0x14 &&
            last->operand(2)->asUnsigned().orElse(1) == 0);
}

AddressSet
Powerpc::getSuccessors(SgAsmInstruction *insn_, bool &complete) const {
    auto insn = isSgAsmPowerpcInstruction(insn_);
    ASSERT_not_null(insn);

    AddressSet retval;
    complete = true; /*assume retval is the complete set of successors for now*/

    switch (insn->get_kind()) {
        case powerpc_bc:
        case powerpc_bca:
        case powerpc_bcl:
        case powerpc_bcla: {
            /* Conditional branches: bcX BO,BI,TARGET */
            const std::vector<SgAsmExpression*> &exprs = insn->get_operandList()->get_operands();
            ASSERT_require(exprs.size() == 3);
            ASSERT_require(isSgAsmValueExpression(exprs[2]));
            SgAsmValueExpression *ve = isSgAsmValueExpression(exprs[2]);
            ASSERT_not_null(ve);
            rose_addr_t target = SageInterface::getAsmConstant(ve);
            retval.insert(target);

            // Fall-through address only happens for conditional branches. If the BO field of a B-form conditional branch is
            // equal to 1x1xx (where x is 0 or 1) then the branch is unconditional.
            if (insn->nOperands() < 1 || (insn->operand(0)->asUnsigned().orElse(0) & 0x14) != 0x14)
                retval.insert(insn->get_address() + insn->get_size());
            break;
        }

        case powerpc_bcctr:
        case powerpc_bcctrl:
        case powerpc_bclr:
        case powerpc_bclrl:
            /* Conditional branches to count register; target is unknown */
            complete = false;

            // Fall-through address only happens for conditional branches. If the BO field of a XL-form conditional branch is
            // equal to 1x1xx (where x is 0 or 1) then the branch is unconditional.
            if (insn->nOperands() < 1 || (insn->operand(0)->asUnsigned().orElse(0) & 0x14) != 0x14)
                retval.insert(insn->get_address() + insn->get_size());
            break;

        case powerpc_b:
        case powerpc_ba:
        case powerpc_bl:
        case powerpc_bla: {
            /* Unconditional branches */
            const std::vector<SgAsmExpression*> &exprs = insn->get_operandList()->get_operands();
            ASSERT_require(exprs.size() == 1);
            ASSERT_not_null(isSgAsmValueExpression(exprs[0]));
            SgAsmValueExpression *ve = isSgAsmValueExpression(exprs[0]);
            ASSERT_not_null(ve);
            rose_addr_t target = SageInterface::getAsmConstant(ve);
            retval.insert(target);
            break;
        }

        case powerpc_unknown_instruction:
        case powerpc_tw:
        case powerpc_twi:
        case powerpc_rfi:
        case powerpc_sc:
            /* No known successors */
            complete = false;
            break;

        default:
            /* All others fall through to next instruction */
            retval.insert(insn->get_address() + insn->get_size());
            break;
    }
    return retval;
}

Unparser::Base::Ptr
Powerpc::newUnparser() const {
    return Unparser::Powerpc::instance(shared_from_this());
}

InstructionSemantics::BaseSemantics::DispatcherPtr
Powerpc::newInstructionDispatcher(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr &ops) const {
    return InstructionSemantics::DispatcherPowerpc::instance(shared_from_this(), ops);
}

std::vector<Partitioner2::FunctionPrologueMatcher::Ptr>
Powerpc::functionPrologueMatchers(const Partitioner2::EnginePtr&) const {
    std::vector<Partitioner2::FunctionPrologueMatcher::Ptr> retval;
    retval.push_back(Partitioner2::ModulesPowerpc::MatchStwuPrologue::instance());
    return retval;
}

CallingConvention::Definition::Ptr
Powerpc::cc_ibm(const size_t bitsPerWord) const {
    // See https://www.ibm.com/support/knowledgecenter/en/ssw_aix_72/com.ibm.aix.alangref/idalangref_reg_use_conv.htm
    RegisterDictionary::Ptr regdict = registerDictionary();
    auto cc = CC::Definition::instance("IBM", "powerpc-" + boost::lexical_cast<std::string>(bitsPerWord) + " ibm", constPtr());
    cc->bitsPerWord(bitsPerWord);

    const RegisterDescriptor SP = regdict->stackPointerRegister();

    //==== Address locations ====
    cc->instructionPointerRegister(regdict->instructionPointerRegister());
    cc->returnAddressLocation(ConcreteLocation(regdict->callReturnRegister(), regdict));

    //==== Stack characteristics ====
    cc->stackPointerRegister(SP);
    cc->stackDirection(CC::StackDirection::GROWS_DOWN);
    cc->nonParameterStackSize(0);                       // return address is in link register

    //==== Function parameters ====

    // Non-FP parameters
    {
        auto pool = CC::StoragePoolEnumerated::instance("non-fp arguments", CC::isNonFpNotWiderThan(bitsPerWord));
        auto insert = [&cc, &regdict, &pool](const std::string &registerName) {
            const RegisterDescriptor reg = regdict->findOrThrow(registerName);
            cc->appendInputParameter(reg);
            pool->append(ConcreteLocation(reg, regdict));
        };

        insert("r3");
        insert("r4");
        insert("r5");
        insert("r6");
        insert("r7");
        insert("r8");
        insert("r9");
        insert("r10");
        cc->argumentValueAllocator()->append(pool);
    }

    // FP parameters
    {
        auto pool = CC::StoragePoolEnumerated::instance("fp arguments", CC::isFpNotWiderThan(bitsPerWord));
        auto insert = [&cc, &regdict, &pool](const std::string &registerName) {
            const RegisterDescriptor reg = regdict->findOrThrow(registerName);
            cc->appendInputParameter(reg);
            pool->append(ConcreteLocation(reg, regdict));
        };

        insert("f1");
        insert("f2");
        insert("f3");
        insert("f4");
        insert("f5");
        insert("f6");
        insert("f7");
        insert("f8");
        insert("f9");
        insert("f10");
        insert("f11");
        insert("f12");
        insert("f13");
    }

    // Stack is generally not used for passing arguments
    cc->stackParameterOrder(CC::StackParameterOrder::RIGHT_TO_LEFT);
    cc->stackCleanup(CC::StackCleanup::BY_CALLER);

    //==== Return values ====

    // Non-FP return values
    {
        auto pool = CC::StoragePoolEnumerated::instance("non-fp returns", CC::isNonFpNotWiderThan(bitsPerWord));
        auto insert = [&cc, &regdict, &pool](const std::string &registerName) {
            const RegisterDescriptor reg = regdict->findOrThrow(registerName);
            cc->appendOutputParameter(reg);
            pool->append(ConcreteLocation(reg, regdict));
        };

        insert("r3");
        insert("r4");
    }

    // FP return values
    {
        auto pool = CC::StoragePoolEnumerated::instance("non-fp returns", CC::isFpNotWiderThan(bitsPerWord));
        auto insert = [&cc, &regdict, &pool](const std::string &registerName) {
            const RegisterDescriptor reg = regdict->findOrThrow(registerName);
            cc->appendOutputParameter(reg);
            pool->append(ConcreteLocation(reg, regdict));
        };

        insert("f1");
        insert("f2");
        insert("f3");
        insert("f4");
    }

    //==== Scratch registers ====
    // function arguments that are not return values, plus others.
    cc->scratchRegisters().insert(regdict->findOrThrow("r0"));
    cc->scratchRegisters().insert(regdict->findOrThrow("r5"));
    cc->scratchRegisters().insert(regdict->findOrThrow("r6"));
    cc->scratchRegisters().insert(regdict->findOrThrow("r7"));
    cc->scratchRegisters().insert(regdict->findOrThrow("r8"));
    cc->scratchRegisters().insert(regdict->findOrThrow("r9"));
    cc->scratchRegisters().insert(regdict->findOrThrow("r10"));
    cc->scratchRegisters().insert(regdict->findOrThrow("r11"));
    cc->scratchRegisters().insert(regdict->findOrThrow("r12"));

    cc->scratchRegisters().insert(regdict->findOrThrow("f0"));
    cc->scratchRegisters().insert(regdict->findOrThrow("f5"));
    cc->scratchRegisters().insert(regdict->findOrThrow("f6"));
    cc->scratchRegisters().insert(regdict->findOrThrow("f7"));
    cc->scratchRegisters().insert(regdict->findOrThrow("f8"));
    cc->scratchRegisters().insert(regdict->findOrThrow("f9"));
    cc->scratchRegisters().insert(regdict->findOrThrow("f10"));
    cc->scratchRegisters().insert(regdict->findOrThrow("f11"));
    cc->scratchRegisters().insert(regdict->findOrThrow("f12"));
    cc->scratchRegisters().insert(regdict->findOrThrow("f13"));

    cc->scratchRegisters().insert(regdict->findOrThrow("cr"));
    cc->scratchRegisters().insert(regdict->findOrThrow("fpscr"));
    cc->scratchRegisters().insert(regdict->instructionPointerRegister());

    //==== Callee-saved registers ====
    // Everything else
    RegisterParts regParts = regdict->getAllParts() - cc->getUsedRegisterParts();
    std::vector<RegisterDescriptor> registers = regParts.extract(regdict);
    cc->calleeSavedRegisters().insert(registers.begin(), registers.end());

    return cc;
}

} // namespace
} // namespace
} // namespace

#endif
