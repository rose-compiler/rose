//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilAssembly            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCilAssembly_IMPL
#include <SgAsmCilAssembly.h>

uint32_t const&
SgAsmCilAssembly::get_HashAlgId() const {
    return p_HashAlgId;
}

void
SgAsmCilAssembly::set_HashAlgId(uint32_t const& x) {
    this->p_HashAlgId = x;
    set_isModified(true);
}

uint16_t const&
SgAsmCilAssembly::get_MajorVersion() const {
    return p_MajorVersion;
}

void
SgAsmCilAssembly::set_MajorVersion(uint16_t const& x) {
    this->p_MajorVersion = x;
    set_isModified(true);
}

uint16_t const&
SgAsmCilAssembly::get_MinorVersion() const {
    return p_MinorVersion;
}

void
SgAsmCilAssembly::set_MinorVersion(uint16_t const& x) {
    this->p_MinorVersion = x;
    set_isModified(true);
}

uint16_t const&
SgAsmCilAssembly::get_BuildNumber() const {
    return p_BuildNumber;
}

void
SgAsmCilAssembly::set_BuildNumber(uint16_t const& x) {
    this->p_BuildNumber = x;
    set_isModified(true);
}

uint16_t const&
SgAsmCilAssembly::get_RevisionNumber() const {
    return p_RevisionNumber;
}

void
SgAsmCilAssembly::set_RevisionNumber(uint16_t const& x) {
    this->p_RevisionNumber = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilAssembly::get_Flags() const {
    return p_Flags;
}

void
SgAsmCilAssembly::set_Flags(uint32_t const& x) {
    this->p_Flags = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilAssembly::get_PublicKey() const {
    return p_PublicKey;
}

void
SgAsmCilAssembly::set_PublicKey(uint32_t const& x) {
    this->p_PublicKey = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilAssembly::get_Name() const {
    return p_Name;
}

void
SgAsmCilAssembly::set_Name(uint32_t const& x) {
    this->p_Name = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilAssembly::get_Culture() const {
    return p_Culture;
}

void
SgAsmCilAssembly::set_Culture(uint32_t const& x) {
    this->p_Culture = x;
    set_isModified(true);
}

SgAsmCilAssembly::~SgAsmCilAssembly() {
    destructorHelper();
}

SgAsmCilAssembly::SgAsmCilAssembly()
    : p_HashAlgId(0)
    , p_MajorVersion(0)
    , p_MinorVersion(0)
    , p_BuildNumber(0)
    , p_RevisionNumber(0)
    , p_Flags(0)
    , p_PublicKey(0)
    , p_Name(0)
    , p_Culture(0) {}

void
SgAsmCilAssembly::initializeProperties() {
    p_HashAlgId = 0;
    p_MajorVersion = 0;
    p_MinorVersion = 0;
    p_BuildNumber = 0;
    p_RevisionNumber = 0;
    p_Flags = 0;
    p_PublicKey = 0;
    p_Name = 0;
    p_Culture = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
