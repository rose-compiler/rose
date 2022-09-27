#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Formatter.h>

#include <Rose/BinaryAnalysis/RegisterDictionary.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {
namespace BaseSemantics {

Formatter::Formatter()
    : suppress_initial_values(false), indentation_suffix("  "), show_latest_writers(true),
      show_properties(true) {}


Formatter::~Formatter() {}

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
