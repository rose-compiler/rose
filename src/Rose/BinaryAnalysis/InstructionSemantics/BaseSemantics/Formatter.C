#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Formatter.h>

#include <Rose/BinaryAnalysis/RegisterDictionary.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {
namespace BaseSemantics {

Formatter::~Formatter() {}

Formatter::Formatter() {}

Formatter::Formatter(const std::string &prefix)
    : line_prefix(prefix) {}

RegisterDictionary::Ptr
Formatter::registerDictionary() const {
    return regdict;
}

void
Formatter::registerDictionary(const RegisterDictionary::Ptr &rd) {
    regdict = rd;
}


} // namespace
} // namespace
} // namespace
} // namespace

#endif
