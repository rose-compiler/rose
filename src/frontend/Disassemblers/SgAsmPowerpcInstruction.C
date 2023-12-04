// SgAsmPowerpcInstruction member definitions.
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>

using namespace Rose;                                   // temporary until this lives in "rose"
using namespace Rose::BinaryAnalysis;

// class method
RegisterDictionary::Ptr
SgAsmPowerpcInstruction::registersForWidth(size_t nBits) {
    switch (nBits) {
        case 32: return Architecture::findByName("ppc32-be").orThrow()->registerDictionary();
        case 64: return Architecture::findByName("ppc64-be").orThrow()->registerDictionary();
        default: ASSERT_not_reachable("invalid PowerPC instruction size");
    }
}

unsigned
SgAsmPowerpcInstruction::get_anyKind() const {
    return p_kind;
}

// Return control flow successors. See base class for full documentation.
AddressSet
SgAsmPowerpcInstruction::getSuccessors(bool &complete) {
    AddressSet retval;
    complete = true; /*assume retval is the complete set of successors for now*/

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

            // Fall-through address only happens for conditional branches. If the BO field of a B-form conditional branch is
            // equal to 1x1xx (where x is 0 or 1) then the branch is unconditional.
            if (nOperands() < 1 || (operand(0)->asUnsigned().orElse(0) & 0x14) != 0x14)
                retval.insert(get_address()+get_size());
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
            if (nOperands() < 1 || (operand(0)->asUnsigned().orElse(0) & 0x14) != 0x14)
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
            complete = false;
            break;

        default:
            /* All others fall through to next instruction */
            retval.insert(get_address()+get_size());
            break;
    }
    return retval;
}

bool
SgAsmPowerpcInstruction::isFunctionReturnFast(const std::vector<SgAsmInstruction*> &insns) {
    if (insns.empty())
        return false;
    SgAsmPowerpcInstruction *insn = isSgAsmPowerpcInstruction(insns.back());
    if (!insn)
        return false;

    // Quick method based only on the kind of instruction.  Returns are normally coded as
    //    BCLR BO, BI, BH
    // where the BO field is the 5-bit constant 0b1x1xx where the x means 0 or 1
    // where the BI field is anything (usually zero)
    // where the BH field is zero
    if (insn->get_kind() == powerpc_bclr && insn->nOperands() == 3 &&
        (insn->operand(0)->asUnsigned().orElse(0) & 0x14) == 0x14 &&
        insn->operand(2)->asUnsigned().orElse(1) == 0)
        return true;

    return false;
}

bool
SgAsmPowerpcInstruction::isFunctionReturnSlow(const std::vector<SgAsmInstruction*> &insns) {
    return isFunctionReturnFast(insns);
}

// Determines whether this is the special PowerPC "unknown" instruction.
bool
SgAsmPowerpcInstruction::isUnknown() const
{
    return powerpc_unknown_instruction == get_kind();
}


std::string
SgAsmPowerpcInstruction::conditionalBranchDescription() const {
    std::string retval;
    unsigned atBits = 0;
    SgAsmExpression *arg0 = get_operandList()->get_operands().empty() ? NULL : get_operandList()->get_operands()[0];
    if (SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(arg0)) {
        unsigned bo = ival->get_absoluteValue();
        if ((bo & ~1u) == 0) {       // 0000z
            retval = "if --CTR != 0 && condition clear";
        } else if ((bo & ~1u) == 2) { // 0001z
            retval = "if --CTR == 0 && condition clear";
        } else if ((bo & ~3u) == 4) { // 001at
            retval = "if condition clear";
            atBits = bo & 3u;
        } else if ((bo & ~1u) == 8) { // 0100z
            retval = "if --CTR != 0 && condition set";
        } else if ((bo & ~1u) == 10) { // 0101z
            retval = "if --CTR == 0 && condition set";
        } else if ((bo & ~3u) == 12) { // 011at
            retval = "if condition set";
            atBits = bo & 3u;
        } else if ((bo & ~9u) == 16) { // 1a00t
            retval = "if --CTR != 0";
            atBits = ((bo & 8u) >> 2) | (bo & 1u);
        } else if ((bo & ~9u) == 18) { // 1a01t
            retval = "if --CTR == 0";
            atBits = ((bo & 8u) >> 2) | (bo & 1u);
        } else if ((bo & ~11u) == 20) { // 1z1zz
            retval = "unconditionally";
        }

        switch (atBits) {
            case 0: break; // no hint
            case 1: break; // invalid, reserved
            case 2: retval += " (unlikely)"; break;
            case 3: retval += " (likely)"; break;
        }
    }
    return retval;
}

#endif
