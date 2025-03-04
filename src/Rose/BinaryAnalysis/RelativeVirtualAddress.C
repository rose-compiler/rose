#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/RelativeVirtualAddress.h>

#include <SgAsmGenericHeader.h>
#include <SgAsmGenericSection.h>
#include <SgAsmGenericString.h>

#include <Sawyer/Assert.h>

namespace Rose {
namespace BinaryAnalysis {

RelativeVirtualAddress::RelativeVirtualAddress() {}

RelativeVirtualAddress::RelativeVirtualAddress(Address rva, SgAsmGenericSection *section/*=nullptr*/)
    : rva_(rva) {
    bindSection(section);
}

RelativeVirtualAddress::RelativeVirtualAddress(const RelativeVirtualAddress &other) {
    rva_ = other.rva_;
    section_ = other.section_;
}

RelativeVirtualAddress
RelativeVirtualAddress::operator=(const RelativeVirtualAddress &other) {
    rva_ = other.rva_;
    section_ = other.section_;
    return *this;
}

RelativeVirtualAddress
RelativeVirtualAddress::sectionRelative(SgAsmGenericSection *section, Address offset) {
    ASSERT_not_null(section);
    ASSERT_require(section->isMapped());
    return RelativeVirtualAddress(section->get_mappedPreferredRva() + offset, section);
}

// [Robb Matzke 2024-02-05]: deprecated
RelativeVirtualAddress
RelativeVirtualAddress::section_relative(SgAsmGenericSection *section, Address offset) {
    return sectionRelative(section, offset);
}

bool
RelativeVirtualAddress::isBound() const {
    return section_ != nullptr;
}

// [Robb Matzke 2024-02-05]: deprecated
bool
RelativeVirtualAddress::is_bound() const {
    return isBound();
}

Address
RelativeVirtualAddress::rva() const {
    if (section_) {
        assert(section_->isMapped());
        return rva_ + section_->get_mappedPreferredRva();
    } else {
        return rva_;
    }
}

// [Robb Matzke 2024-02-05]: deprecated
Address
RelativeVirtualAddress::get_rva() const {
    return rva();
}

RelativeVirtualAddress&
RelativeVirtualAddress::rva(Address newRva) {
    rva_ = newRva;
    if (section_) {
        assert(section_->isMapped());
        rva_ -= section_->get_mappedPreferredRva();
    }
    return *this;
}

// [Robb Matzke 2024-02-05]: deprecated
RelativeVirtualAddress&
RelativeVirtualAddress::set_rva(Address newRva) {
    return rva(newRva);
}

SgAsmGenericSection *
RelativeVirtualAddress::boundSection() const {
    return section_;
}

// [Robb Matzke 2024-02-05]: deprecated
SgAsmGenericSection *
RelativeVirtualAddress::get_section() const
{
    return boundSection();
}

RelativeVirtualAddress&
RelativeVirtualAddress::bindSection(SgAsmGenericSection *newSection) {
    assert(newSection == nullptr || newSection->isMapped());
    if (section_) {
        rva_ += section_->get_mappedPreferredRva();
        section_ = nullptr;
    }
    if (newSection)
        rva_ -= newSection->get_mappedPreferredRva();
    section_ = newSection;
    return *this;
}

// [Robb Matzke 2024-02-05]: deprecated
RelativeVirtualAddress&
RelativeVirtualAddress::set_section(SgAsmGenericSection *new_section) {
    return bindSection(new_section);
}

RelativeVirtualAddress&
RelativeVirtualAddress::bindBestSection(SgAsmGenericHeader *fhdr) {
    assert(fhdr != nullptr);
    Address va = rva() + fhdr->get_baseVa();
    SgAsmGenericSection *secbind = fhdr->get_bestSectionByVa(va, true);
    return bindSection(secbind);
}

// [Robb Matzke 2024-02-05]: deprecated
RelativeVirtualAddress&
RelativeVirtualAddress::bind(SgAsmGenericHeader *fhdr) {
    return bindBestSection(fhdr);
}

Sawyer::Optional<Address>
RelativeVirtualAddress::va() const {
    if (section_) {
        ASSERT_require(section_->isMapped());
        return rva_ + section_->get_mappedActualVa();
    } else {
        return Sawyer::Nothing();
    }
}

// [Robb Matzke 2024-02-05]: deprecated
Address
RelativeVirtualAddress::get_va() const {
    return va().orElse(rva_);
}

Sawyer::Optional<Address>
RelativeVirtualAddress::boundOffset() const {
    if (section_) {
        return rva_;
    } else {
        return Sawyer::Nothing();
    }
}

Address
RelativeVirtualAddress::get_rel() const {
    return boundOffset().orElse(rva_);
}

Address
RelativeVirtualAddress::offsetFrom(SgAsmGenericSection *section) const {
    ASSERT_not_null(section);
    ASSERT_require(section->isMapped());
    return rva() - section->get_mappedPreferredRva();
}

// [Robb Matzke 2024-02-05]: deprecated
Address
RelativeVirtualAddress::get_rel(SgAsmGenericSection *s) {
    return offsetFrom(s);
}

void
RelativeVirtualAddress::increment(Address amount) {
    rva_ += amount;
}

std::string
RelativeVirtualAddress::toString() const {
    char s[1024];
    snprintf(s, sizeof(s), "0x%08" PRIx64 " (%" PRIu64 ")", rva(), rva());
    std::string ss = s;

    if (boundSection()) {
        snprintf(s, sizeof(s), " + 0x%08" PRIx64 " (%" PRIu64 ")", *boundOffset(), *boundOffset());
        ss += " <" + boundSection()->get_name()->get_string(true) + s + ">";
    }
    return ss;
}

// [Robb Matzke 2024-02-05]: deprecated
std::string
RelativeVirtualAddress::to_string() const {
    return toString();
}

} // namespace
} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Root namespace
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::ostream &
operator<<(std::ostream &os, const Rose::BinaryAnalysis::RelativeVirtualAddress &rva) {
    os << rva.toString();
    return os;
}

Rose::BinaryAnalysis::Address
operator+(const Rose::BinaryAnalysis::RelativeVirtualAddress &a1, const Rose::BinaryAnalysis::RelativeVirtualAddress &a2) {
    return a1.rva() + a2.rva();
}

Rose::BinaryAnalysis::Address
operator-(const Rose::BinaryAnalysis::RelativeVirtualAddress &a1, const Rose::BinaryAnalysis::RelativeVirtualAddress &a2) {
    return a1.rva() - a2.rva();
}

bool
operator< (const Rose::BinaryAnalysis::RelativeVirtualAddress &a1, const Rose::BinaryAnalysis::RelativeVirtualAddress &a2) {
    return a1.rva() <  a2.rva();
}

bool
operator<=(const Rose::BinaryAnalysis::RelativeVirtualAddress &a1, const Rose::BinaryAnalysis::RelativeVirtualAddress &a2) {
    return a1.rva() <= a2.rva();
}

bool
operator>(const Rose::BinaryAnalysis::RelativeVirtualAddress &a1, const Rose::BinaryAnalysis::RelativeVirtualAddress &a2) {
    return a1.rva() >  a2.rva();
}

bool
operator>=(const Rose::BinaryAnalysis::RelativeVirtualAddress &a1, const Rose::BinaryAnalysis::RelativeVirtualAddress &a2) {
    return a1.rva() >= a2.rva();
}

bool
operator==(const Rose::BinaryAnalysis::RelativeVirtualAddress &a1, const Rose::BinaryAnalysis::RelativeVirtualAddress &a2) {
    return a1.rva() == a2.rva();
}

bool
operator!=(const Rose::BinaryAnalysis::RelativeVirtualAddress &a1, const Rose::BinaryAnalysis::RelativeVirtualAddress &a2) {
    return a1.rva() != a2.rva();
}

#endif
