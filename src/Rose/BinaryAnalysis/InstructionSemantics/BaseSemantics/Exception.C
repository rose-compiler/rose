#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Exception.h>

#include <SgAsmInstruction.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {
namespace BaseSemantics {

void
Exception::print(std::ostream &o) const
{
    o <<"Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics::Exception: " <<what();
    if (insn)
        o <<": " <<insn->toString();
    o <<"\n";
}

std::ostream& operator<<(std::ostream &o, const Exception &x) {
    x.print(o);
    return o;
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
