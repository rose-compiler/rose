#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/RegisterState.h>

#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Exception.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Formatter.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {
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

RegisterState::RegisterState() {}

RegisterState::RegisterState(const SValue::Ptr &protoval, const RegisterDictionary::Ptr &regdict)
    : protoval_(protoval), regdict(regdict) {
    ASSERT_not_null(protoval_);
}

RegisterState::~RegisterState() {}

RegisterState::Ptr
RegisterState::promote(const RegisterState::Ptr &x) {
    ASSERT_not_null(x);
    return x;
}

RegisterDictionary::Ptr
RegisterState::registerDictionary() const {
    return regdict;
}

void
RegisterState::registerDictionary(const RegisterDictionary::Ptr &rd) {
    regdict = rd;
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

BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics::RegisterState);

#endif
