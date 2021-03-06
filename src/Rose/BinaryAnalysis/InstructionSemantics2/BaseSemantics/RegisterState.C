#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics/RegisterState.h>

#include <Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics/Exception.h>
#include <Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics/Formatter.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace BaseSemantics {

std::ostream&
operator<<(std::ostream &o, const RegisterState &x) {
    x.print(o);
    return o;
}

std::ostream&
operator<<(std::ostream &o, const RegisterState::WithFormatter &x) {
    x.print(o);
    return o;
}

void
RegisterState::print(std::ostream &stream, const std::string prefix) const {
    Formatter fmt;
    fmt.set_line_prefix(prefix);
    print(stream, fmt);
}

RegisterState::WithFormatter
RegisterState::operator+(const std::string &linePrefix) {
    static Formatter fmt;
    fmt.set_line_prefix(linePrefix);
    return with_format(fmt);
}
    
} // namespace
} // namespace
} // namespace
} // namespace

BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::RegisterState);

#endif
