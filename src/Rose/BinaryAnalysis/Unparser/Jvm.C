#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/Unparser/Jvm.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

void
Jvm::emitInstructionMnemonic(std::ostream &out, SgAsmInstruction *insn, State&) const {
    out <<insn->get_mnemonic();
}

} // namespace
} // namespace
} // namespace

#endif
