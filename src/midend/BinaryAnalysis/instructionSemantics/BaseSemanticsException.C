#include <sage3basic.h>
#include <BaseSemanticsException.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
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
