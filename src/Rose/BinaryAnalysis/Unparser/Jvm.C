#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Unparser/Jvm.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Supporting functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string
unparseJvmMnemonic(SgAsmJvmInstruction* insn) {
    ASSERT_not_null(insn);
    return insn->get_mnemonic();
}

void
Jvm::emitInstructionMnemonic(std::ostream &out, SgAsmInstruction *insn, State&) const {
    if (insn) {
        out << insn->get_mnemonic();
    }
}

} // namespace
} // namespace
} // namespace

#endif
