#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#define M68K 0

using namespace Rose;                                   // temporary until this lives in "rose"
using namespace Rose::BinaryAnalysis;

bool
SgAsmCilInstruction::terminatesBasicBlock() {
    return true;
}

bool
SgAsmCilInstruction::isFunctionCallFast(const std::vector<SgAsmInstruction*>&, rose_addr_t* /*target,inout*/,
                                         rose_addr_t*/*ret,inout*/) {
    return false;
}

bool
SgAsmCilInstruction::isFunctionCallSlow(const std::vector<SgAsmInstruction*>&, rose_addr_t* /*target,inout*/,
                                         rose_addr_t*/*ret,inout*/) {
    return false;
}

bool
SgAsmCilInstruction::isFunctionReturnFast(const std::vector<SgAsmInstruction*>&) {
    return false;
}

bool
SgAsmCilInstruction::isFunctionReturnSlow(const std::vector<SgAsmInstruction*>&) {
    return false;
}

Sawyer::Optional<rose_addr_t>
SgAsmCilInstruction::branchTarget() {
    return Sawyer::Nothing();
}

Rose::BinaryAnalysis::AddressSet
SgAsmCilInstruction::getSuccessors(bool &complete) {
    complete = false;
    return AddressSet();
}

Rose::BinaryAnalysis::AddressSet
SgAsmCilInstruction::getSuccessors(const std::vector<SgAsmInstruction*>&, bool &complete, const MemoryMap::Ptr&) {
    complete = false;
    return AddressSet();
}

bool
SgAsmCilInstruction::isUnknown() const {
    return true;
}

unsigned
SgAsmCilInstruction::get_anyKind() const {
 // return (unsigned)null_unknown;
    return p_kind;
}


std::string
SgAsmCilInstruction::description() const {
    switch (get_kind()) {
        case Cil_unknown_instruction:  return "";
        case Cil_nop:                  return "no operation";
#if M68K
        case Cil_add:                  return "add";
        case Cil_adda:                 return "add address";
        case Cil_addi:                 return "add immediate";
        case Cil_addq:                 return "add quick";
        case Cil_addx:                 return "add extended";
        case Cil_and:                  return "logical AND";
        case Cil_andi:                 return "immediate AND";
        case Cil_asl:                  return "arithmetic shift left";
        case Cil_asr:                  return "arithmetic shift right";
        case Cil_bcc:                  return "branch carry clear";
        case Cil_bcs:                  return "branch carry set";
        case Cil_beq:                  return "branch equal";
        case Cil_bge:                  return "branch greater or equal";
        case Cil_bgt:                  return "branch greater than";
        case Cil_bhi:                  return "branch high";
        case Cil_ble:                  return "branch less or equal";
        case Cil_bls:                  return "branch lower or same";
        case Cil_blt:                  return "branch less than";
        case Cil_bmi:                  return "branch minus";
        case Cil_bne:                  return "branch not equal";
        case Cil_bpl:                  return "branch plus";
        case Cil_bvc:                  return "branch overflow clear";
        case Cil_bvs:                  return "branch overflow set";
        case Cil_bchg:                 return "test bit and change";
        case Cil_bclr:                 return "test bit and clear";
        case Cil_bfchg:                return "test bit field and change";
        case Cil_bfclr:                return "test bit field and clear";
        case Cil_bfexts:               return "extract bit field signed";
        case Cil_bfextu:               return "extract bit field unsigned";
        // Cil_bfffo,                  find first one in bit field
        case Cil_bfins:                return "insert bit field";
        case Cil_bfset:                return "test bit field and set";
        case Cil_bftst:                return "test bit field";
        // Cil_bitrev,                 bit reverse register
        case Cil_bkpt:                 return "breakpoint";
        case Cil_bra:                  return "branch always";
        case Cil_bset:                 return "test bit and set";
        case Cil_bsr:                  return "branch to subroutine";
        case Cil_btst:                 return "test bit";
        // Cil_byterev,                byte reverse register
        case Cil_callm:                return "call module";
        case Cil_cas:                  return "compare and swap with operand";
        case Cil_cas2:                 return "compare and swap with operands";
        case Cil_chk:                  return "check register against bounds";
        case Cil_chk2:                 return "check register against bounds";
        case Cil_clr:                  return "clear";
        case Cil_cmp:                  return "compare";
        case Cil_cmp2:                 return "compare register against bounds";
        case Cil_cmpa:                 return "compare address";
        case Cil_cmpi:                 return "compare immediate";
        case Cil_cmpm:                 return "compare memory";
        case Cil_cpusha:               return "push and invalidate all";
        case Cil_cpushl:               return "push and invalidate cache lines";
        case Cil_cpushp:               return "push and invalidate cache pages";
        case Cil_dbt:                  return "decrement and branch if true";
        case Cil_dbf:                  return "decrement and branch if false";
        case Cil_dbhi:                 return "decrement and branch if high";
        case Cil_dbls:                 return "decrement and branch if lower or same";
        case Cil_dbcc:                 return "decrement and branch if cary clear";
        case Cil_dbcs:                 return "decrement and branch if carry set";
        case Cil_dbne:                 return "decrement and branch if not equal";
        case Cil_dbeq:                 return "decrement and branch if equal";
        case Cil_dbvc:                 return "decrement and branch if overflow clear";
        case Cil_dbvs:                 return "decrement and branch if overflow set";
        case Cil_dbpl:                 return "decrement and branch if plus";
        case Cil_dbmi:                 return "decrement and branch if minus";
        case Cil_dbge:                 return "decrement and branch if greater or equal";
        case Cil_dblt:                 return "decrement and branch if less than";
        case Cil_dbgt:                 return "decrement and branch if greater than";
        case Cil_dble:                 return "decrement and branch if less than or equal";
        case Cil_divs:                 return "signed divide with optional remainder";
        case Cil_divsl:                return "signed divide 32-bit quotient with remainder";
        case Cil_divu:                 return "unsigned divide with optional remainder";
        case Cil_divul:                return "unsigned divide 32-bit quotient with remainder";
        case Cil_eor:                  return "exclusive-OR logical";
        case Cil_eori:                 return "exclusive-OR immediate";
        case Cil_exg:                  return "exchange registers";
        case Cil_ext:                  return "sign extend";
        case Cil_extb:                 return "sign extend byte to longword";
        case Cil_fabs:                 return "fp absolute value with FPCR rounding";
        case Cil_fadd:                 return "fp add with FPCR rounding";
        case Cil_fbeq:                 return "fp branch if equal";
        case Cil_fbne:                 return "fp branch if not equal";
        case Cil_fbgt:                 return "fp branch if greater than";
        case Cil_fbngt:                return "fp branch if not greater than";
        case Cil_fbge:                 return "fp branch if greater than or equal";
        case Cil_fbnge:                return "fp branch if not greater than or equal";
        case Cil_fblt:                 return "fp branch if less than";
        case Cil_fbnlt:                return "fp branch if not less than";
        case Cil_fble:                 return "fp branch if less than or equal";
        case Cil_fbnle:                return "fp branch if not less than or equal";
        case Cil_fbgl:                 return "fp branch if greater or less than";
        case Cil_fbngl:                return "fp branch if not greater or less than";
        case Cil_fbgle:                return "fp branch if greater, less, or equal";
        case Cil_fbngle:               return "fp branch if not greater, less, or equal";
        case Cil_fbogt:                return "fp branch if ordered greater than";
        case Cil_fbule:                return "fp branch if unordered or less or equal";
        case Cil_fboge:                return "fp branch if ordered greater than or equal";
        case Cil_fbult:                return "fp branch if unordered less than";
        case Cil_fbolt:                return "fp branch if ordered less than";
        case Cil_fbuge:                return "fp branch if unordered or greater than or equal";
        case Cil_fbole:                return "fp branch if ordered less than or equal";
        case Cil_fbugt:                return "fp branch if unordered or greater than";
        case Cil_fbogl:                return "fp branch if ordered greater or less than";
        case Cil_fbueq:                return "fp branch if unordered or equal";
        case Cil_fbor:                 return "fp branch if ordered";
        case Cil_fbun:                 return "fp branch if unordered";
        case Cil_fbf:                  return "fp branch if false";
        case Cil_fbt:                  return "fp branch if true";
        case Cil_fbsf:                 return "fp branch if signaling false";
        case Cil_fbst:                 return "fp branch if signaling true";
        case Cil_fbseq:                return "fp branch if signaling equal";
        case Cil_fbsne:                return "fp branch if signaling not equal";
        case Cil_fcmp:                 return "fp compare";
        case Cil_fdabs:                return "fp absolute value with double-precision rounding";
        case Cil_fdadd:                return "fp add with double-precision rounding";
        case Cil_fddiv:                return "fp divide with double-precision rounding";
        case Cil_fdiv:                 return "fp divide with FPCR rounding";
        case Cil_fdmove:               return "copy fp data with double-precision rounding";
        case Cil_fdmul:                return "fp multiple with double-precision rounding";
        case Cil_fdneg:                return "fp negation with double-precision rounding";
        case Cil_fdsqrt:               return "fp square root with double-precision rounding";
        case Cil_fdsub:                return "fp subtract with double-precision rounding";
        // Cil_ff1,                    find first one in register
        case Cil_fint:                 return "fp integer part";
        case Cil_fintrz:               return "fp integer part rounded-to-zero";
        case Cil_fmove:                return "copy fp data with FPCR rounding";
        case Cil_fmovem:               return "copy multiple fp data registers";
        case Cil_fmul:                 return "fp multiply with FPCR rounding";
        case Cil_fneg:                 return "fp negate with FPCR rounding";
        case Cil_fnop:                 return "fp no operation";
        // Cil_frestore,
        case Cil_fsabs:                return "fp absolute value with single-precision rounding";
        case Cil_fsadd:                return "fp add with single-precision rounding";
        // Cil_fsave,
        case Cil_fsdiv:                return "fp divide with single-precision rounding";
        case Cil_fsmove:               return "copy fp data with single-precision rounding";
        case Cil_fsmul:                return "fp multiply with single-precision rounding";
        case Cil_fsneg:                return "fp negation with single-precision rounding";
        case Cil_fsqrt:                return "fp square root with FPCR rounding";
        case Cil_fssqrt:               return "fp square root with single-precision rounding";
        case Cil_fssub:                return "fp subtract with single-precision rounding";
        case Cil_fsub:                 return "fp subtract with FPCR rounding";
        case Cil_ftst:                 return "fp test";
        // Cil_halt,                   halt the CPU
        case Cil_illegal:              return "take illegal instruction trap";
        // Cil_intouch,
        case Cil_jmp:                  return "jump";
        case Cil_jsr:                  return "jump to subroutine";
        case Cil_lea:                  return "load effective address";
        case Cil_link:                 return "link and allocate";
        case Cil_lsl:                  return "logical shift left";
        case Cil_lsr:                  return "logical shift right";
        case Cil_mac:                  return "multiply accumulate";
        case Cil_mov3q:                return "copy 3-bit data quick";
        case Cil_movclr:               return "copy from MAC ACC register and clear";
        case Cil_move:                 return "copy data";
        case Cil_move_acc:             return "copy MAC ACC register";
        case Cil_move_accext:          return "copy MAC ACCext register";
        case Cil_move_ccr:             return "copy condition code register";
        case Cil_move_macsr:           return "copy MAC status register";
        case Cil_move_mask:            return "copy MAC MASK register";
        case Cil_move_sr:              return "copy status register";
        case Cil_move16:               return "copy 16-byte block";
        case Cil_movea:                return "copy address";
        case Cil_movec:                return "copy control register";
        case Cil_movem:                return "copy multiple registers";
        case Cil_movep:                return "copy peripheral data";
        case Cil_moveq:                return "copy quick";
        case Cil_msac:                 return "multiply subtract";
        case Cil_muls:                 return "signed multiply";
        case Cil_mulu:                 return "unsigned multiply";
        case Cil_mvs:                  return "copy with sign extend";
        case Cil_mvz:                  return "copy with zero fill";
        case Cil_nbcd:                 return "negate decimal with extend";
        case Cil_neg:                  return "negate";
        case Cil_negx:                 return "negate with extend";
        case Cil_nop:                  return "no operation";
        case Cil_not:                  return "logical complement";
        case Cil_or:                   return "inclusive-OR logical";
        case Cil_ori:                  return "inclusive-OR immediate";
        case Cil_pack:                 return "pack";
        case Cil_pea:                  return "push effective address";
        // Cil_pulse,                  generate unique processor status
        // Cil_rems,                   signed divide remainder -- see divs instead
        // Cil_remu,                   unsigned divide remainder -- see divu instead
        case Cil_rol:                  return "rotate left without extend";
        case Cil_ror:                  return "rotate right without extend";
        case Cil_roxl:                 return "rotate left with extend";
        case Cil_roxr:                 return "rotate right with extend";
        case Cil_rtd:                  return "return and deallocate";
        case Cil_rtm:                  return "return from module";
        // Cil_rte,                    return from exception
        case Cil_rtr:                  return "return and restore condition codes";
        case Cil_rts:                  return "return from subroutine";
        // Cil_sats,                   signed saturate
        case Cil_sbcd:                 return "subtract decimal with extend";
        case Cil_st:                   return "set if true";
        case Cil_sf:                   return "set if false";
        case Cil_shi:                  return "set if high";
        case Cil_sls:                  return "set if lower or same";
        case Cil_scc:                  return "set if carry clear (HS)";
        case Cil_scs:                  return "set if carry set (LO)";
        case Cil_sne:                  return "set if not equal";
        case Cil_seq:                  return "set if equal";
        case Cil_svc:                  return "set if overflow clear";
        case Cil_svs:                  return "set if overflow set";
        case Cil_spl:                  return "set if plus";
        case Cil_smi:                  return "set if minus";
        case Cil_sge:                  return "set if greater or equal";
        case Cil_slt:                  return "set if less than";
        case Cil_sgt:                  return "set if greater than";
        case Cil_sle:                  return "set if less or equal";
        // Cil_stop,
        case Cil_sub:                  return "subtract";
        case Cil_suba:                 return "subtract address";
        case Cil_subi:                 return "subtract immediate";
        case Cil_subq:                 return "subtract quick";
        case Cil_subx:                 return "subtract extended";
        case Cil_swap:                 return "swap register halves";
        case Cil_tas:                  return "test and set";
        // Cil_tpf,                    trap false (no operation)
        case Cil_trap:                 return "trap";
        case Cil_trapt:                return "trap if true";
        case Cil_trapf:                return "trap if false";
        case Cil_traphi:               return "trap if high";
        case Cil_trapls:               return "trap if lower or same";
        case Cil_trapcc:               return "trap if carry clear (HS)";
        case Cil_trapcs:               return "trap if carry set (LO)";
        case Cil_trapne:               return "trap if not equal";
        case Cil_trapeq:               return "trap if equal";
        case Cil_trapvc:               return "trap if overflow clear";
        case Cil_trapvs:               return "trap if overflow set";
        case Cil_trappl:               return "trap if plus";
        case Cil_trapmi:               return "trap if minus";
        case Cil_trapge:               return "trap if greater or equal";
        case Cil_traplt:               return "trap if less than";
        case Cil_trapgt:               return "trap if greater than";
        case Cil_traple:               return "trap if less or equal";
        case Cil_trapv:                return "trap on overflow";
        case Cil_tst:                  return "test";
        case Cil_unlk:                 return "unlink";
        case Cil_unpk:                 return "unpack binary coded decimal";
        // Cil_wddata,                 write to debug data
        // Cil_wdebug,
#endif
        case Cil_last_instruction:     ASSERT_not_reachable("not a valid CIL instruction kind");
    }
    ASSERT_not_reachable("invalid CIL instruction kind: " + StringUtility::numberToString(get_kind()));
}

#endif
