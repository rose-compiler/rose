/* SgAsmPowerpcInstruction member definitions.  Do not move them to src/ROSETTA/Grammar/BinaryInstruction.code (or any *.code
 * file) because then they won't get indexed/formatted/etc. by C-aware tools. */

#include "sage3basic.h"
#include "Disassembler.h"

// Return control flow successors. See base class for full documentation.
Disassembler::AddressSet
SgAsmPowerpcInstruction::get_successors(bool *complete) {
    Disassembler::AddressSet retval;
    *complete = true; /*assume retval is the complete set of successors for now*/

    switch (get_kind()) {
        case powerpc_bc:
        case powerpc_bca:
        case powerpc_bcl:
        case powerpc_bcla: {
            /* Conditional branches: bcX BO,BI,TARGET */
            const std::vector<SgAsmExpression*> &exprs = get_operandList()->get_operands();
            ROSE_ASSERT(exprs.size()==3);
            ROSE_ASSERT(isSgAsmValueExpression(exprs[2]));
            SgAsmValueExpression *ve = isSgAsmValueExpression(exprs[2]);
            assert(ve!=NULL);
            rose_addr_t target = SageInterface::getAsmConstant(ve);
            retval.insert(target);
            retval.insert(get_address()+get_size());
            break;
        }

        case powerpc_bcctr:
        case powerpc_bcctrl:
        case powerpc_bclr:
        case powerpc_bclrl:
            /* Conditional branches to count register; target is unknown */
            *complete = false;
            retval.insert(get_address()+get_size());
            break;

        case powerpc_b:
        case powerpc_ba:
        case powerpc_bl:
        case powerpc_bla: {
            /* Unconditional branches */
            const std::vector<SgAsmExpression*> &exprs = get_operandList()->get_operands();
            ROSE_ASSERT(exprs.size()==1);
            ROSE_ASSERT(isSgAsmValueExpression(exprs[0]));
            SgAsmValueExpression *ve = isSgAsmValueExpression(exprs[0]);
            assert(ve!=NULL);
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
            *complete = false;
            break;

        default:
            /* All others fall through to next instruction */
            retval.insert(get_address()+get_size());
            break;
    }
    return retval;
}

// Does instruction terminate basic block? See base class for full documentation.
bool
SgAsmPowerpcInstruction::terminates_basic_block() {
    switch (get_kind()) {
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

// Determines whether this is the special PowerPC "unknown" instruction.
bool
SgAsmPowerpcInstruction::is_unknown() const
{
    return powerpc_unknown_instruction == get_kind();
}

