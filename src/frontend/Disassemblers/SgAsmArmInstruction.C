/* SgAsmArmInstruction member definitions.  Do not move them to src/ROSETTA/Grammar/BinaryInstruction.code (or any *.code file)
 * because then they won't get indexed/formatted/etc. by C-aware tools. */
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"

#include "Disassembler.h"

using namespace Rose;                                   // temporary until this lives in "rose"
using namespace Rose::BinaryAnalysis;                   // temporary

unsigned
SgAsmArmInstruction::get_anyKind() const {
    return p_kind;
}

/* Returns true if the instruction modifies the instruction pointer (r15). */
static bool modifies_ip(SgAsmArmInstruction *insn) 
{
    switch (insn->get_kind()) {

        /* Branch instructions */
        case arm_b:
        case arm_bl:
        case arm_blx:
        case arm_bx:
        case arm_bxj:
            return true;

        /* Comparison instructions */
        case arm_cmn:
        case arm_cmp:
        case arm_teq:
        case arm_tst:
            return false;

        /* Load multiple registers instructions. Second argument is the set of registers to load.  If the instruction
         * pointer (r15) can be one of them. */
        case arm_ldm:
        case arm_ldmda:
        case arm_ldmdb:
        case arm_ldmia:
        case arm_ldmib: {
            const std::vector<SgAsmExpression*> &exprs = insn->get_operandList()->get_operands();
            ROSE_ASSERT(exprs.size()>=2);
            SgAsmExprListExp *elist = isSgAsmExprListExp(exprs[1]);
            if (!elist) {
                SgAsmUnaryArmSpecialRegisterList *rlist = isSgAsmUnaryArmSpecialRegisterList(exprs[1]);
                ROSE_ASSERT(rlist);
                elist = isSgAsmExprListExp(rlist->get_operand());
                ROSE_ASSERT(elist);
            }
            for (size_t i=0; i<elist->get_expressions().size(); i++) {
                SgAsmRegisterReferenceExpression *reg = isSgAsmRegisterReferenceExpression(elist->get_expressions()[i]);
                ROSE_ASSERT(reg);
                if (reg->get_descriptor().majorNumber()==arm_regclass_gpr && reg->get_descriptor().minorNumber()==15) {
                    return true;
                }
            }
            return false;
        }

        /* Interrupt-causing instructions */
        case arm_bkpt:
        case arm_swi:
        case arm_undefined:
            return true;

        /* Other instructions modify the instruction pointer if it's the first (destination) argument. */
        default: {
            const std::vector<SgAsmExpression*> &exprs = insn->get_operandList()->get_operands();
            if (exprs.size()>=1) {
                SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(exprs[0]);
                if (rre &&
                    rre->get_descriptor().majorNumber()==arm_regclass_gpr && rre->get_descriptor().minorNumber()==15) {
                    return true;
                }
            }
        }
    }
    return false;
}

/** Return control flow successors. See base class for full documentation. */
BinaryAnalysis::Disassembler::AddressSet
SgAsmArmInstruction::getSuccessors(bool *complete) {
    BinaryAnalysis::Disassembler::AddressSet retval;
    const std::vector<SgAsmExpression*> &exprs = get_operandList()->get_operands();
    *complete = true; /*assume retval is the complete set of successors for now*/

    switch (get_kind()) {
        case arm_b:
        case arm_bl:
        case arm_blx:
        case arm_bx: {
            /* Branch target */
            ROSE_ASSERT(exprs.size()==1);
            SgAsmExpression *dest = exprs[0];
            if (isSgAsmValueExpression(dest)) {
                rose_addr_t target_va = SageInterface::getAsmConstant(isSgAsmValueExpression(dest));
                retval.insert(target_va);
            } else {
                /* Could also be a register reference expression, but we don't know the successor in that case. */
                *complete = false;
            }
            
            /* Fall-through address */
            if (get_condition()!=arm_cond_al)
                retval.insert(get_address()+4);
            break;
        }

        case arm_bxj: {
            /* First argument is the register that holds the next instruction pointer value to use in the case that Jazelle is
             * not available. We only know the successor if the register is the instruction pointer, in which case the
             * successor is the fall-through address. */
            ROSE_ASSERT(exprs.size()==1);
            SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(exprs[0]);
            ROSE_ASSERT(rre);
            if (rre->get_descriptor().majorNumber()==arm_regclass_gpr && rre->get_descriptor().minorNumber()==15) {
                retval.insert(get_address()+4);
            } else {
                *complete = false;
            }
            break;
        }
            
        case arm_cmn:
        case arm_cmp:
        case arm_teq:
        case arm_tst:
            /* Comparison and test instructions don't ever affect the instruction pointer; they only fall through */
            retval.insert(get_address()+4);
            break;

        case arm_bkpt:
        case arm_swi:
        case arm_undefined:
        case arm_unknown_instruction:
            /* No known successors for interrupt-generating instructions */
            break;

        default:
            if (!modifies_ip(this) || get_condition()!=arm_cond_al) {
                retval.insert(get_address()+4);
            } else {
                *complete = false;
            }
            break;
    }
    return retval;
}

// Does instruction terminate basic block? See base class for full documentation.
bool
SgAsmArmInstruction::terminatesBasicBlock() {
    if (get_kind()==arm_unknown_instruction)
        return true;
    return modifies_ip(this);
}

// Determines whether this is the special ARM "unkown" instruction. See base class for documentation.
bool
SgAsmArmInstruction::isUnknown() const
{
    return arm_unknown_instruction == get_kind();
}


std::string
SgAsmArmInstruction::description() const {
    switch (get_kind()) {
        case arm_unknown_instruction:   return "";
        case arm_adc:                   return "add with carry";
        case arm_adcs:                  return "add with carry and update condition word";
        case arm_add:                   return "add";
        case arm_adds:                  return "add and update condition word";
        case arm_and:                   return "bitwise AND";
        case arm_ands:                  return "bitwise AND and update condition word";
        case arm_b:                     return "branch";
        case arm_bic:                   return "bit clear (op1 AND NOT op2)";
        case arm_bics:                  return "bit clear and update condition word";
        case arm_bkpt:                  return "breakpoint";
        case arm_bl:                    return "branch with link";
        case arm_blx:                   return "branch with link and exchange";
        case arm_bx:                    return "branch and exchange";
        case arm_bxj:                   return "branch and change to Java state";
        case arm_clz:                   return "count leading zeros";
        case arm_cmn:                   return "compare negative";
        case arm_cmp:                   return "compare";
        case arm_eor:                   return "bitwise exclusive-OR";
        case arm_eors:                  return "bitwise exclusive-EOR and update condition word";
        case arm_ldm:                   return "pop or block data load";
        case arm_ldmda:                 return "pop or block data load, decrement after";
        case arm_ldmdb:                 return "pop or block data load, decrement before";
        case arm_ldmia:                 return "pop or block data load, increment after";
        case arm_ldmib:                 return "pop or block data load, increment before";
        case arm_ldr:                   return "load word";
        case arm_ldrb:                  return "load byte";
        case arm_ldrbt:                 return "load byte user mode privilege";
        case arm_ldrd:                  return "load doubleword";
        case arm_ldrsb:                 return "load signed byte";
        case arm_ldrsh:                 return "load signed halfword";
        case arm_ldrt:                  return "load word user mode privilege";
        case arm_ldruh:                 return "";      // synonym for ldrh?
        case arm_mla:                   return "multiply and accumulate";
        case arm_mlas:                  return "multiply and accumulate and update condition word";
        case arm_mov:                   return "copy";
        case arm_movs:                  return "copy and update condition word";
        case arm_mrs:                   return "copy PSR to register";
        case arm_msr:                   return "copy register to PSR";
        case arm_mul:                   return "multiply";
        case arm_muls:                  return "multiply and update condition word";
        case arm_mvn:                   return "copy NOT";
        case arm_mvns:                  return "copy NOT and update condition word";
        case arm_orr:                   return "bitwise OR";
        case arm_orrs:                  return "bitwise OR and update condition word";
        case arm_qadd:                  return "saturating add";
        case arm_qdadd:                 return "double saturating add";
        case arm_qdsub:                 return "double saturating subtract";
        case arm_qsub:                  return "saturating subtract";
        case arm_rsb:                   return "reverse subtract";
        case arm_rsbs:                  return "reverse subtract and update condition word";
        case arm_rsc:                   return "reverse subtract with carry";
        case arm_rscs:                  return "reverse subtract with carry and update condition word";
        case arm_sbc:                   return "subtract with carry";
        case arm_sbcs:                  return "subtract with carry and update condition word";
        case arm_setend:                return "set endianness";
        case arm_smlabb:                return "signed multiply long and accumulate 16 * 16 bit";
        case arm_smlabt:                return "signed multiply long and accumulate 16 * 16 bit";
        case arm_smlal:                 return "signed multiply long and accumulate long";
        case arm_smlalbb:               return "signed multiply long 16 * 16 bit and accumulate long";
        case arm_smlalbt:               return "signed multiply long 16 * 16 bit and accumulate long";
        case arm_smlals:                return "signed multiply long and accumulate long and update condition word";
        case arm_smlaltb:               return "signed multiply long 16 * 16 bit and accumulate long";
        case arm_smlaltt:               return "signed multiply long 16 * 16 bit and accumulate long";
        case arm_smlatb:                return "signed multiply long and accumulate 16 * 16 bit";
        case arm_smlatt:                return "signed multiply long and accumulate 16 * 16 bit";
        case arm_smlawb:                return "signed multiply long and accumulate 32 * 16 bit";
        case arm_smlawt:                return "signed multiply long and accumulate 32 * 16 bit";
        case arm_smluwb:                return "";      // should this be smulwb?
        case arm_smluwt:                return "";      // should this be smulwt?
        case arm_smulbb:                return "signed multiply long 16 * 16 bit";
        case arm_smulbt:                return "signed multiply long 16 * 16 bit";
        case arm_smull:                 return "signed multiply long";
        case arm_smulls:                return "signed multiply long and update condition word";
        case arm_smultb:                return "signed multiply long 16 * 16 bit";
        case arm_smultt:                return "signed multiply long 16 * 16 bit";
        case arm_stm:                   return "push or block data store";
        case arm_stmda:                 return "push or block data store, decrement after";
        case arm_stmdb:                 return "push or block data store, decrement before";
        case arm_stmia:                 return "push or block data store, increment after";
        case arm_stmib:                 return "push or block data store, increment before";
        case arm_str:                   return "store word";
        case arm_strb:                  return "store byte";
        case arm_strbt:                 return "store byte user mode privilege";
        case arm_strd:                  return "store doubleword";
        case arm_strh:                  return "store halfword";
        case arm_strt:                  return "store word user mode privilege";
        case arm_sub:                   return "subtract";
        case arm_subs:                  return "subtract and update condition word";
        case arm_swi:                   return "software interrupt";
        case arm_swp:                   return "swap word";
        case arm_teq:                   return "test equivalence";
        case arm_tst:                   return "test";
        case arm_umaal:                 return "multiply double accumulate long";
        case arm_umlal:                 return "multiply unsigned accumulate long";
        case arm_umlals:                return "multiply unsigned accumulate long and update condition word";
        case arm_umull:                 return "multiply unsigned long";
        case arm_umulls:                return "multiply unsigned long and update condition word";
        case arm_undefined:             return "";
        case arm_last_instruction:      ASSERT_not_reachable("invalid ARM instruction kind");
    }
    ASSERT_not_reachable("invalid ARM instruction kind: " + StringUtility::numberToString(get_kind()));
}

#endif
