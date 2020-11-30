#include <featureTests.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include <sage3basic.h>
#include <BinaryUnparserNull.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Unparser {

void
Null::emitInstructionMnemonic(std::ostream &out, SgAsmInstruction *insn, State&) const {
    out <<insn->get_mnemonic();
}

} // namespace
} // namespace
} // namespace

#endif
