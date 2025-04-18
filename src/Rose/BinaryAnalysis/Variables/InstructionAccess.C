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
InstructionAccess::accessString(const AccessFlags access) {
    if (access.isSet(Access::READ)) {
        if (access.isSet(Access::WRITE)) {
            return "read/write";
        } else {
            return "read";
        }
    } else if (access.isSet(Access::WRITE)) {
        return "write";
    } else {
        return "implied access";
    }
}

std::string
InstructionAccess::accessString() const {
    return accessString(access_);
}

std::string
InstructionAccess::toString() const {
    if (address()) {
        return accessString() + " at instruction " + StringUtility::addrToString(*address());
    } else if (access_.isAnySet()) {
        return "implied " + accessString(access_);
    } else {
        return "implied access";
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
