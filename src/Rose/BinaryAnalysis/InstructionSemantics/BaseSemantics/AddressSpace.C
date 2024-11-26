#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/AddressSpace.h>

#include <Rose/Affirm.h>
#include <Rose/As.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Formatter.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/MemoryState.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/RegisterState.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/RiscOperators.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/SValue.h>
#include <Rose/StringUtility/Escape.h>

#include <regex>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {
namespace BaseSemantics {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AddressSpaceAddress
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AddressSpaceAddress::~AddressSpaceAddress() {}

AddressSpaceAddress::AddressSpaceAddress() {}

AddressSpaceAddress::AddressSpaceAddress(const RegisterDescriptor addr)
    : reg_(addr) {}

AddressSpaceAddress::AddressSpaceAddress(const Address addr)
    : concrete_(addr) {}

AddressSpaceAddress::AddressSpaceAddress(const SValue::Ptr &addr)
    : abstract_(addr) {}

bool
AddressSpaceAddress::isEmpty() const {
    return !reg_ && !concrete_ && !abstract_;
}

RegisterDescriptor
AddressSpaceAddress::registerDescriptor() const {
    return reg_;
}

const Sawyer::Optional<Address>&
AddressSpaceAddress::concrete() const {
    return concrete_;
}

SValue::Ptr
AddressSpaceAddress::abstract() const {
    return abstract_;
}

Sawyer::Optional<Address>
AddressSpaceAddress::toConcrete() const {
    if (reg_) {
        return ((reg_.majorNumber() * (RegisterDescriptor::maxMinor() + 1) * (RegisterDescriptor::maxOffset() + 1)) +
                (reg_.minorNumber() + (RegisterDescriptor::maxOffset() + 1)) +
                (reg_.offset() / 8));
    } else if (concrete_) {
        return *concrete_;
    } else if (abstract_) {
        return abstract_->toUnsigned();
    } else {
        return Sawyer::Nothing();
    }
}

SValue::Ptr
AddressSpaceAddress::toAbstract(RiscOperators &addrOps) const {
    if (abstract_) {
        return addrOps.unsignedExtend(abstract_, 64);
    } else if (const auto addr = toConcrete()) {
        return addrOps.number_(64, *addr);
    } else {
        return {};
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AddressSpace::WithFormatter
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AddressSpace::WithFormatter::~WithFormatter() {}

AddressSpace::WithFormatter::WithFormatter(const AddressSpace::Ptr &obj, Formatter &fmt)
    : obj(notnull(obj)), fmt(fmt) {}

void
AddressSpace::WithFormatter::print(std::ostream &out) const {
    obj->print(out, fmt);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AddressSpace
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AddressSpace::~AddressSpace() {}

AddressSpace::AddressSpace()
    : purpose_(Purpose::UNSPECIFIED) {}

AddressSpace::AddressSpace(const Purpose p, const std::string &s)
    : purpose_(Purpose::UNSPECIFIED), name_(s) {
    purpose(p);
}

AddressSpace::AddressSpace(const AddressSpace &other)
    : purpose_(other.purpose_), name_(other.name_) {}

AddressSpace::Purpose
AddressSpace::purpose() const {
    ASSERT_require2(purpose_ != Purpose::UNSPECIFIED, "apparently default constructed and not fully initialized");
    return purpose_;
}

void
AddressSpace::purpose(const AddressSpace::Purpose p) {
    ASSERT_require(p != Purpose::UNSPECIFIED);
    purpose_ = p;
}

const std::string&
AddressSpace::name() const {
    return name_;
}

void
AddressSpace::name(const std::string &s) {
    name_ = s;
}

SValue::Ptr
AddressSpace::read(const AddressSpaceAddress &addr, const SValue::Ptr &dflt, RiscOperators &addrOps, RiscOperators &valOps) {
    ASSERT_forbid(addr.isEmpty());
    ASSERT_not_null(dflt);

    // For now, we'll do some down-casting in order to use the old virtual functions.
    if (auto regState = as<RegisterState>(this)) {
        ASSERT_require(addr.registerDescriptor());
        return regState->readRegister(addr.registerDescriptor(), dflt, &valOps);
    } else if (auto memState = as<MemoryState>(this)) {
        return memState->readMemory(affirm(addr.abstract(), "address is not abstract"), dflt, &addrOps, &valOps);
    } else {
        ASSERT_not_reachable("unknown address space");
    }
}

SValue::Ptr
AddressSpace::peek(const AddressSpaceAddress &addr, const SValue::Ptr &dflt, RiscOperators &addrOps, RiscOperators &valOps) {
    ASSERT_forbid(addr.isEmpty());
    ASSERT_not_null(dflt);

    // For now, we'll do some down-casting in order to use the old virtual functions.
    if (auto regState = as<RegisterState>(this)) {
        ASSERT_require(addr.registerDescriptor());
        return regState->peekRegister(addr.registerDescriptor(), dflt, &valOps);
    } else if (auto memState = as<MemoryState>(this)) {
        return memState->peekMemory(affirm(addr.abstract(), "address is not abstract"), dflt, &addrOps, &valOps);
    } else {
        ASSERT_not_reachable("unknown address space");
    }
}

void
AddressSpace::write(const AddressSpaceAddress &addr, const SValue::Ptr &dflt, RiscOperators &addrOps, RiscOperators &valOps) {
    ASSERT_forbid(addr.isEmpty());
    ASSERT_not_null(dflt);

    // For now, we'll do some down-casting in order to use the old virtual functions.
    if (auto regState = as<RegisterState>(this)) {
        ASSERT_require(addr.registerDescriptor());
        regState->writeRegister(addr.registerDescriptor(), dflt, &valOps);
    } else if (auto memState = as<MemoryState>(this)) {
        memState->writeMemory(affirm(addr.abstract(), "address is not abstract"), dflt, &addrOps, &valOps);
    } else {
        ASSERT_not_reachable("unknown address space");
    }
}

std::string
AddressSpace::printableName() const {
    std::regex re("[A-Za-z][_0-9A-Za-z]*");
    if (std::regex_match(name(), re)) {
        return name();
    } else {
        return "\"" + StringUtility::cEscape(name()) + "\"";
    }
}

void
AddressSpace::print(std::ostream &stream, const std::string &prefix) const {
    Formatter fmt;
    fmt.set_line_prefix(prefix);
    print(stream, fmt);
}

AddressSpace::WithFormatter
AddressSpace::operator+(const std::string &linePrefix) {
    static Formatter fmt;
    fmt.set_line_prefix(linePrefix);
    return with_format(fmt);
}

AddressSpace::WithFormatter
AddressSpace::with_format(Formatter &fmt) {
    return WithFormatter(shared_from_this(), fmt);
}

AddressSpace::WithFormatter
AddressSpace::operator+(Formatter &fmt) {
    return with_format(fmt);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Free functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::ostream&
operator<<(std::ostream &out, const AddressSpace &x) {
    x.print(out);
    return out;
}

std::ostream&
operator<<(std::ostream &out, const AddressSpace::WithFormatter &x) {
    x.print(out);
    return out;
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
