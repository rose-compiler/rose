#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
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

SValue::WithFormatter
SValue::operator+(const std::string &linePrefix) {
    static Formatter fmt;
    fmt.set_line_prefix(linePrefix);
    return with_format(fmt);
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
