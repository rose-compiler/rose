#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include <sage3basic.h>
#include <Partitioner2/ModulesMips.h>

#include <Partitioner2/Partitioner.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {
namespace ModulesMips {

using namespace Rose::Diagnostics;

bool
MatchRetAddiu::match(const Partitioner &partitioner, rose_addr_t anchor) {
    if (anchor & 3)
        return false;                                   // MIPS instructions must be 4-byte aligned

    // First look for "JR RA"
    if (SgAsmMipsInstruction *insn = isSgAsmMipsInstruction(partitioner.discoverInstruction(anchor))) {
        static const RegisterDescriptor REG_RA(mips_regclass_gpr, 31, 0, 32);
        if (insn->get_kind() != mips_jr)
            return false;
        const SgAsmExpressionPtrList &args = insn->get_operandList()->get_operands();
        if (args.size() != 1)
            return false;
        SgAsmDirectRegisterExpression *rre = isSgAsmDirectRegisterExpression(args[0]);
        if (!rre || rre->get_descriptor() != REG_RA)
            return false;
    } else {
        return false;
    }
    
    // Then look for "ADDIU SP, SP, C" where C is a positive integer
    if (SgAsmMipsInstruction *insn = isSgAsmMipsInstruction(partitioner.discoverInstruction(anchor + 4))) {
        static const RegisterDescriptor REG_SP(mips_regclass_gpr, 29, 0, 32);
        if (insn->get_kind() != mips_addiu)
            return false;
        const SgAsmExpressionPtrList &args = insn->get_operandList()->get_operands();
        if (args.size() != 3)
            return false;
        SgAsmDirectRegisterExpression *rre = isSgAsmDirectRegisterExpression(args[0]);
        if (!rre || rre->get_descriptor() != REG_SP)
            return false;
        rre = isSgAsmDirectRegisterExpression(args[1]);
        if (!rre || rre->get_descriptor() != REG_SP)
            return false;
        SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(args[2]);
        if (!ival || ival->get_signedValue() <= 0)
            return false;
    } else {
        return false;
    }

    // We found both instructions. The function starts at the ADDIU instruction.
    function_ = Function::instance(anchor + 4, SgAsmFunction::FUNC_PATTERN);
    function_->reasonComment("matched JR RA; ADDIU SP, SP, <positive_const>");
    return true;
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
