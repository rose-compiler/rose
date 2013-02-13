/* SgAsmArmInstruction member definitions.  Do not move them to src/ROSETTA/Grammar/BinaryInstruction.code (or any *.code file)
 * because then they won't get indexed/formatted/etc. by C-aware tools. */

#include "sage3basic.h"
#include "Disassembler.h"

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
                SgAsmArmRegisterReferenceExpression *reg = isSgAsmArmRegisterReferenceExpression(elist->get_expressions()[i]);
                ROSE_ASSERT(reg);
                if (reg->get_descriptor().get_major()==arm_regclass_gpr && reg->get_descriptor().get_minor()==15) {
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
                SgAsmArmRegisterReferenceExpression *rre = isSgAsmArmRegisterReferenceExpression(exprs[0]);
                if (rre &&
                    rre->get_descriptor().get_major()==arm_regclass_gpr && rre->get_descriptor().get_minor()==15) {
                    return true;
                }
            }
        }
    }
    return false;
}

/** Return control flow successors. See base class for full documentation. */
Disassembler::AddressSet
SgAsmArmInstruction::get_successors(bool *complete) {
    Disassembler::AddressSet retval;
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
            SgAsmArmRegisterReferenceExpression *rre = isSgAsmArmRegisterReferenceExpression(exprs[0]);
            ROSE_ASSERT(rre);
            if (rre->get_descriptor().get_major()==arm_regclass_gpr && rre->get_descriptor().get_minor()==15) {
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
SgAsmArmInstruction::terminates_basic_block() {
    if (get_kind()==arm_unknown_instruction)
        return true;
    return modifies_ip(this);
}

// Determines whether this is the special ARM "unkown" instruction. See base class for documentation.
bool
SgAsmArmInstruction::is_unknown() const
{
    return arm_unknown_instruction == get_kind();
}

