#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Variables/InstructionAccess.h>

#include <Rose/StringUtility/NumberToString.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Variables {

InstructionAccess::InstructionAccess(const AccessFlags access)
    : access_(access) {}

InstructionAccess::InstructionAccess(const Address address, const AccessFlags access)
    : address_(address), access_(access) {}

Sawyer::Optional<Address>
InstructionAccess::address() const {
    return address_;
}

AccessFlags
InstructionAccess::access() const {
    return access_;
}

AccessFlags&
InstructionAccess::access() {
    return access_;
}

std::string
InstructionAccess::accessString() const {
    if (access_.isSet(Access::READ)) {
        if (access_.isSet(Access::WRITE)) {
            return "read/write";
        } else {
            return "read";
        }
    } else if (access_.isSet(Access::WRITE)) {
        return "write";
    } else {
        return "no access";
    }
}

std::string
InstructionAccess::toString() const {
    if (address()) {
        return accessString() + " at " + StringUtility::addrToString(*address());
    } else {
        return "implied variable";
    }
}

std::ostream&
operator<<(std::ostream &out, const InstructionAccess &ia) {
    out <<ia.toString();
    return out;
}

} // namespace
} // namespace
} // namespace

#endif
