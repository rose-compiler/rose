#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Variables/BaseVariable.h>

#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/BasicTypes.h>

namespace BS = Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics;


namespace Rose {
namespace BinaryAnalysis {
namespace Variables {

BaseVariable::BaseVariable() {}

BaseVariable::BaseVariable(size_t maxSizeBytes, const std::vector<InstructionAccess> &definingInstructionVas,
                           const std::string &name)
    // following arithmetic is to work around lack of SSIZE_MAX on windows. The maxSizeBytes should not be more than the
    // maximum value of the signed type with the same conversion rank.
    : maxSizeBytes_(std::min(maxSizeBytes, ((size_t)(1) << (8*sizeof(size_t)-1))-1)),
      insns_(definingInstructionVas), name_(name) {}

BaseVariable::BaseVariable(const BaseVariable&) = default;
BaseVariable& BaseVariable::operator=(const BaseVariable&) = default;
BaseVariable::~BaseVariable() {}

Address
BaseVariable::maxSizeBytes() const {
    return maxSizeBytes_;
}

void
BaseVariable::maxSizeBytes(Address size) {
    ASSERT_require(size > 0);
    maxSizeBytes_ = size;
}

const std::vector<InstructionAccess>&
BaseVariable::instructionsAccessing() const {
    return insns_;
}

void
BaseVariable::instructionsAccessing(const std::vector<InstructionAccess> &ia) {
    insns_ = ia;
}

AccessFlags
BaseVariable::accessFlags() const {
    AccessFlags retval;
    for (const InstructionAccess &ia: insns_)
        retval.set(ia.access());
    return retval;
}

BS::InputOutputPropertySet
BaseVariable::ioProperties() const {
    BS::InputOutputPropertySet retval;
    for (const InstructionAccess &ia: insns_) {
        for (const Access access: ia.access().split()) {
            switch (access) {
                case Access::READ:
                    retval.insert(BS::IO_READ);
                    break;
                case Access::WRITE:
                    retval.insert(BS::IO_WRITE);
                    break;
                default:
                    ASSERT_not_reachable("invalid access");
            }
        }
    }
    return retval;
}

const std::string&
BaseVariable::name() const {
    return name_;
}

void
BaseVariable::name(const std::string &s) {
    name_ = s;
}

void
BaseVariable::insertAccess(const Address address, const AccessFlags access) {
    for (InstructionAccess &ia: insns_) {
        if (ia.address() && address == *ia.address()) {
            ia.access().set(access);
            return;
        }
    }

    insns_.push_back(InstructionAccess(address, access));
}

} // namespace
} // namespace
} // namespace

#endif
