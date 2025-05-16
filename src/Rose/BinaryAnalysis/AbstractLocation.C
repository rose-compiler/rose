#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/AbstractLocation.h>

#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Formatter.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/SValue.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/BinaryAnalysis/RegisterNames.h>
#include <Rose/StringUtility/Diagnostics.h>

#include <ostream>
#include <sstream>
#include <string>
#include <vector>

namespace Rose {
namespace BinaryAnalysis {

AbstractLocation::AbstractLocation() {}

AbstractLocation::~AbstractLocation() {}

AbstractLocation::AbstractLocation(const AbstractLocation &other)
    : reg_(other.reg_), addr_(other.addr_), nBytes_(other.nBytes_), regdict_(other.regdict_) {}

AbstractLocation&
AbstractLocation::operator=(const AbstractLocation &other) {
    reg_ = other.reg_;
    addr_ = other.addr_;
    nBytes_ = other.nBytes_;
    regdict_ = other.regdict_;
    return *this;
}

AbstractLocation::AbstractLocation(RegisterDescriptor reg)
    : reg_(reg), nBytes_(0) {
    ASSERT_require(reg);
}

AbstractLocation::AbstractLocation(RegisterDescriptor reg, const RegisterDictionary::Ptr &regdict)
    : reg_(reg), nBytes_(0), regdict_(regdict) {
    ASSERT_require(reg);
}

AbstractLocation::AbstractLocation(const Address &addr, size_t nBytes)
    : addr_(addr), nBytes_(nBytes) {
    ASSERT_not_null(addr);
}

AbstractLocation
AbstractLocation::parse(const std::string&) {
    ASSERT_not_implemented("[Robb Matzke 2022-07-14]");
}


bool
AbstractLocation::isValid() const {
    return isRegister() || isAddress();
}

uint64_t
AbstractLocation::hash() const {
    Combinatorics::HasherSha256Builtin hasher;
    hasher.insert(reg_.raw());
    if (addr_)
        addr_->hash(hasher);
    hasher.insert(nBytes_);
    return hasher.toU64();
}

std::string
AbstractLocation::toString() const {
    return printableName();
}

void
AbstractLocation::print(std::ostream &out) const {
    if (isRegister()) {
        out <<RegisterNames()(reg_, regdict_);
    } else if (isAddress()) {
        out <<*addr_;
        if (nBytes_ > 0)
            out <<"+" <<StringUtility::plural(nBytes_, "bytes");
    } else {
        out <<"NO_LOCATION";
    }
}

void
AbstractLocation::print(std::ostream &out, const RegisterDictionary::Ptr &regdict) const {
    InstructionSemantics::BaseSemantics::Formatter fmt;
    print(out, regdict, fmt);
}

void
AbstractLocation::print(std::ostream &out, InstructionSemantics::BaseSemantics::Formatter &fmt) const {
    print(out, RegisterDictionary::Ptr(), fmt);
}

void
AbstractLocation::print(std::ostream &out, const RegisterDictionary::Ptr &regdict,
                        InstructionSemantics::BaseSemantics::Formatter &fmt) const {
    if (isRegister()) {
        out <<RegisterNames()(reg_, regdict ? regdict : regdict_);
    } else if (isAddress()) {
        out <<(*addr_+fmt);
        if (nBytes_>0)
            out <<"+" <<StringUtility::plural(nBytes_, "bytes");
    } else {
        out <<"NO_LOCATION";
    }
}

std::string
AbstractLocation::printableName() const {
    std::ostringstream ss;
    print(ss);
    return ss.str();
}

bool
AbstractLocation::isEqual(const Location &other_) const {
    auto other = dynamic_cast<const AbstractLocation*>(&other_);
    // Don't compare regdict_ since it doesn't contribute to the location; it's only used for output
    return other && reg_ == other->reg_ && addr_ == other->addr_ && nBytes_ == other->nBytes_;
}

int
AbstractLocation::compare(const AbstractLocation &other) const {
    if (reg_.isValid() != other.reg_.isValid()) {
        return reg_.isValid() ? 1 : -1;
    } else if (reg_.isValid()) {
        ASSERT_require(other.reg_.isValid());
        if (reg_.raw() != other.reg_.raw())
            return reg_.raw() < other.reg_.raw() ? -1 : 1;
    }

    if (!!addr_ != !!other.addr_) {
        return addr_ ? 1 : -1;
    } else if (!!addr_) {
        // When both addresses are present, we essentially do a structural comparison by comparing hashes.
        ASSERT_not_null(other.addr_);
        Rose::Combinatorics::HasherSha256Builtin hasher1, hasher2;
        addr_->hash(hasher1);
        other.addr_->hash(hasher2);
        std::vector<uint8_t> digest1 = hasher1.digest();
        std::vector<uint8_t> digest2 = hasher2.digest();
        ASSERT_require(digest1.size() == digest2.size());
        auto mismatch = std::mismatch(digest1.begin(), digest1.end(), digest2.begin());
        if (mismatch.first != digest1.end()) {
            ASSERT_require(mismatch.second != digest2.end());
            ASSERT_require(*mismatch.first != *mismatch.second);
            return *mismatch.first < *mismatch.second ? -1 : 1;
        } else {
            ASSERT_require(mismatch.second != digest2.end());
        }
    }

    if (nBytes_ != other.nBytes_)
        return nBytes_ < other.nBytes_ ? -1 : 1;

    // regdict_ is not compared since it doesn't contribute to the location; it's only used for output.
    auto e1 = isEqual(other);
    auto e2 = other.isEqual(*this);
    ASSERT_require(e1 || e2); // one way must be equal, or we would have already returned above
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

bool
AbstractLocation::operator<(const Location &other_) const {
    auto other = dynamic_cast<const AbstractLocation*>(&other_);
    return other && compare(*other) < 0;
}

bool
AbstractLocation::operator<=(const Location &other_) const {
    auto other = dynamic_cast<const AbstractLocation*>(&other_);
    return other && compare(*other) <= 0;
}

bool
AbstractLocation::operator>(const Location &other_) const {
    auto other = dynamic_cast<const AbstractLocation*>(&other_);
    return other && compare(*other) > 0;
}

bool
AbstractLocation::operator>=(const Location &other_) const {
    auto other = dynamic_cast<const AbstractLocation*>(&other_);
    return other && compare(*other) >= 0;
}

bool
AbstractLocation::isRegister() const {
    return !reg_.isEmpty();
}

bool
AbstractLocation::isAddress() const {
    return addr_ != nullptr;
}

RegisterDescriptor
AbstractLocation::getRegister() const {
    return reg_;
}

const AbstractLocation::Address
AbstractLocation::getAddress() const {
    return addr_;
}

size_t
AbstractLocation::nBytes() const {
    ASSERT_require(isAddress());
    return nBytes_;
}

bool
AbstractLocation::mayAlias(const AbstractLocation &other, const SmtSolverPtr &solver) const {
    if (isRegister() && other.isRegister()) {
        return reg_ == other.reg_;
    } else if (isAddress() && other.isAddress()) {
        return addr_->mayEqual(other.addr_, solver);
    } else if (!isValid() && !other.isValid()) {
        return true;
    } else {
        return false;
    }
}

bool
AbstractLocation::mustAlias(const AbstractLocation &other, const SmtSolverPtr &solver) const {
    if (isRegister() && other.isRegister()) {
        return reg_ == other.reg_;
    } else if (isAddress() && other.isAddress()) {
        return addr_->mustEqual(other.addr_, solver);
    } else if (!isValid() && !other.isValid()) {
        return true;
    } else {
        return false;
    }
}

} // namespace
} // namespace

#endif
