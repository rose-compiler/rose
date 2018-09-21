#include <sage3basic.h>
#include <Partitioner2/ModulesPowerpc.h>
#include <Partitioner2/Partitioner.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {
namespace ModulesPowerpc {

std::vector<Function::Ptr>
MatchStwuPrologue::functions() const {
    std::vector<Function::Ptr> retval;
    if (function_)
        retval.push_back(function_);
    return retval;
}

bool
MatchStwuPrologue::match(const Partitioner &partitioner, rose_addr_t anchor) {
    if (partitioner.instructionExists(anchor))
        return false;                                   // already in the CFG/AUM
    SgAsmPowerpcInstruction *insn = isSgAsmPowerpcInstruction(partitioner.discoverInstruction(anchor));
    if (!insn)
        return false;                                   // cannot decode an instruction at this address

    // Look for "stwu r1, u32 [r1 + DELTA]" where DELTA is a negative value
    if (insn->get_kind() == powerpc_stwu && insn->nOperands() == 2) {
        // First operand must be "r1"
        SgAsmDirectRegisterExpression *reg = isSgAsmDirectRegisterExpression(insn->operand(0));
        RegisterDescriptor r1 = reg ? reg->get_descriptor() : RegisterDescriptor();
        if (r1.majorNumber() != powerpc_regclass_gpr || r1.minorNumber() != 1 ||
            r1.offset() != 0 || (r1.nBits() != 32 && r1.nBits() != 64))
            return false;                               // not the usual frame pointer register, r1

        // Second operand must be either:
        //    'u32 [r1 + DELTA]' where DELTA is negative, or
        //    'u64 [r1 + DELTA]' where DELTA is negative
        SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(insn->operand(1));
        SgAsmBinaryAdd *addr = mre ? isSgAsmBinaryAdd(mre->get_address()) : NULL;
        SgAsmDirectRegisterExpression *dre = addr ? isSgAsmDirectRegisterExpression(addr->get_lhs()) : NULL;
        RegisterDescriptor r2 = dre ? dre->get_descriptor() : RegisterDescriptor();
        if (r2 != r1)
            return false;
        int64_t delta = addr->get_rhs()->asSigned().orElse(0);
        if (delta >= 0)
            return false;

        function_ = Function::instance(anchor, SgAsmFunction::FUNC_PATTERN);
        function_->reasonComment("matched STWU r1, [r1 + <delta>] s.t. delta < 0");
        return true;
    }

    return false;
}

} // namespace
} // namespace
} // namespace
} // namespace
