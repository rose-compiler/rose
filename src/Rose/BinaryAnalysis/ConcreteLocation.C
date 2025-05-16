#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/ConcreteLocation.h>

#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/BinaryAnalysis/RegisterNames.h>
#include <Rose/StringUtility/NumberToString.h>

#include <Combinatorics.h>                              // rose

#include <sstream>

namespace Rose {
namespace BinaryAnalysis {

ConcreteLocation::ConcreteLocation() {}

ConcreteLocation::~ConcreteLocation() {}

ConcreteLocation::ConcreteLocation(const ConcreteLocation &other)
    : reg_(other.reg_), va_(other.va_), regdict_(other.regdict_) {}

ConcreteLocation&
ConcreteLocation::operator=(const ConcreteLocation &other) {
    reg_ = other.reg_;
    va_ = other.va_;
    regdict_ = other.regdict_;
    return *this;
}

ConcreteLocation::ConcreteLocation(RegisterDescriptor reg)
    : reg_(reg) {
    ASSERT_require(reg.isValid());
}

ConcreteLocation::ConcreteLocation(RegisterDescriptor reg, const RegisterDictionary::Ptr &regdict)
    : reg_(reg), regdict_(regdict) {
    ASSERT_require(reg.isValid());
}

ConcreteLocation::ConcreteLocation(Address va)
    : va_(va) {}

ConcreteLocation::ConcreteLocation(RegisterDescriptor reg, int64_t offset)
    : reg_(reg), va_(offset) {
    ASSERT_require(reg.isValid());
}

ConcreteLocation::ConcreteLocation(RegisterDescriptor reg, int64_t offset, const RegisterDictionary::Ptr &regdict)
    : reg_(reg), va_(offset), regdict_(regdict) {
    ASSERT_require(reg.isValid());
}

ConcreteLocation
ConcreteLocation::parse(const std::string&) {
    ASSERT_not_implemented("[Robb Matzke 2022-07-14]");
}

bool
ConcreteLocation::isValid() const {
    return reg_ || va_;
}

uint64_t
ConcreteLocation::hash() const {
    Combinatorics::HasherSha256Builtin hasher;
    hasher.insert(reg_.raw());
    if (va_)
        hasher.insert(*va_);
    return hasher.make64Bits();
}

std::string
ConcreteLocation::toString() const {
    std::ostringstream ss;
    ss <<*this;
    return ss.str();
}

std::string
ConcreteLocation::printableName() const {
    return toString();
}

void
ConcreteLocation::print(std::ostream &out) const {
    print(out, regdict_);
}

void
ConcreteLocation::print(std::ostream &out, const RegisterDictionary::Ptr &regdict) const {
    RegisterNames regnames(regdict);
    switch (type()) {
        case NO_LOCATION:
            out <<"nowhere";
            break;

        case REGISTER:
            out <<regnames(reg());
            break;

        case RELATIVE:
            out <<"mem[" <<regnames(reg());
            if (offset() >= 0) {
                out <<"+" <<offset() <<"]";
            } else {
                out <<"-" <<-offset() <<"]";
            }
            break;

        case ABSOLUTE:
            out <<"mem[" <<StringUtility::addrToString(address()) <<"]";
            break;
    }
}

bool
ConcreteLocation::isEqual(const Location &other_) const {
    auto other = dynamic_cast<const ConcreteLocation*>(&other_);
    // The regdict_ is not considered significant for equality since it's just for printing.
    return other && reg_ == other->reg_ && va_.isEqual(other->va_);
}

bool
ConcreteLocation::operator<(const Location &other_) const {
    auto other = dynamic_cast<const ConcreteLocation*>(&other_);
    return other && compare(*other) < 0;
}

bool
ConcreteLocation::operator<=(const Location &other_) const {
    auto other = dynamic_cast<const ConcreteLocation*>(&other_);
    return other && compare(*other) <= 0;
}

bool
ConcreteLocation::operator>(const Location &other_) const {
    auto other = dynamic_cast<const ConcreteLocation*>(&other_);
    return other && compare(*other) > 0;
}

bool
ConcreteLocation::operator>=(const Location &other_) const {
    auto other = dynamic_cast<const ConcreteLocation*>(&other_);
    return other && compare(*other) >= 0;
}

int
ConcreteLocation::compare(const ConcreteLocation &other) const {
    if (!!reg_ != !!other.reg_) {
        return reg_ ? 1 : -1;
    } else if (reg_) {
        ASSERT_require(other.reg_);
        if (reg_.raw() != other.reg_.raw())
            return reg_.raw() < other.reg_.raw() ? -1 : 1;
    }

    if (!!va_ != !!other.va_) {
        return va_ ? 1 : -1;
    } else if (va_) {
        ASSERT_require(other.va_);
        if (*va_ != *other.va_)
            return *va_ < *other.va_ ? -1 : 1;
    }

    bool e1 = isEqual(other);
    bool e2 = other.isEqual(*this);
    ASSERT_require(e1 || e2);                           // one must be equal, or we would have already returned above
    if (e1 && e2) {
        return 0;                                       // essentially the algorithm for Location::operator==
    } else if (e1) {
        ASSERT_forbid(e2);
        return 1;
    } else {
        ASSERT_require(!e1 && e2);
        return -1;
    }
}

RegisterDictionary::Ptr
ConcreteLocation::registerDictionary() const {
    return regdict_;
}

void
ConcreteLocation::registerDictionary(const RegisterDictionary::Ptr &regdict) {
    regdict_ = regdict;
}

ConcreteLocation::Type
ConcreteLocation::type() const {
    if (reg_) {
        return va_ ? RELATIVE : REGISTER;
    } else if (va_) {
        return ABSOLUTE;
    } else {
        return NO_LOCATION;
    }
}

RegisterDescriptor
ConcreteLocation::reg() const {
    return reg_;
}

Address
ConcreteLocation::address() const {
    return reg_ ? 0 : *va_;
}

int64_t
ConcreteLocation::offset() const {
    return reg_ ? (int64_t)*va_ : 0;
}

} // namespace
} // namespace

#endif
