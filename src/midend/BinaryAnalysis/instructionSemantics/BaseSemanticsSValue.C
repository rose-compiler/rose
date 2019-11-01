#include <sage3basic.h>
#include <BaseSemanticsSValue.h>
#include <BaseSemanticsFormatter.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace BaseSemantics {

std::ostream&
operator<<(std::ostream &o, const SValue &x) {
    x.print(o);
    return o;
}

std::ostream&
operator<<(std::ostream &o, const SValue::WithFormatter &x) {
    x.print(o);
    return o;
}

void
SValue::print(std::ostream &stream) const {
    Formatter fmt;
    print(stream, fmt);
}

} // namespace
} // namespace
} // namespace
} // namespace
