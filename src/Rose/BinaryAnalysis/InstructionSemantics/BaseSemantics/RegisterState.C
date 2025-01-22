#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/RegisterState.h>

#include <Rose/As.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Exception.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {
namespace BaseSemantics {

RegisterState::RegisterState()
    : AddressSpace(AddressSpace::Purpose::REGISTERS, "registers") {}

RegisterState::RegisterState(const SValue::Ptr &protoval, const RegisterDictionary::Ptr &regdict)
    : AddressSpace(AddressSpace::Purpose::REGISTERS, "registers"), protoval_(protoval), regdict(regdict) {
    ASSERT_not_null(protoval_);
}

RegisterState::~RegisterState() {}

RegisterState::Ptr
RegisterState::promote(const AddressSpace::Ptr &x) {
    Ptr retval = as<RegisterState>(x);
    ASSERT_not_null(retval);
    return retval;
}

RegisterDictionary::Ptr
RegisterState::registerDictionary() const {
    return regdict;
}

void
RegisterState::registerDictionary(const RegisterDictionary::Ptr &rd) {
    regdict = rd;
}

} // namespace
} // namespace
} // namespace
} // namespace

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
BOOST_CLASS_EXPORT_IMPLEMENT(Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics::RegisterState);
#endif

#endif
