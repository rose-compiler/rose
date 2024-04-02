//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilAssemblyRef            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCilAssemblyRef_IMPL
#include <sage3basic.h>

uint16_t const&
SgAsmCilAssemblyRef::get_MajorVersion() const {
    return p_MajorVersion;
}

void
SgAsmCilAssemblyRef::set_MajorVersion(uint16_t const& x) {
    this->p_MajorVersion = x;
    set_isModified(true);
}

uint16_t const&
SgAsmCilAssemblyRef::get_MinorVersion() const {
    return p_MinorVersion;
}

void
SgAsmCilAssemblyRef::set_MinorVersion(uint16_t const& x) {
    this->p_MinorVersion = x;
    set_isModified(true);
}

uint16_t const&
SgAsmCilAssemblyRef::get_BuildNumber() const {
    return p_BuildNumber;
}

void
SgAsmCilAssemblyRef::set_BuildNumber(uint16_t const& x) {
    this->p_BuildNumber = x;
    set_isModified(true);
}

uint16_t const&
SgAsmCilAssemblyRef::get_RevisionNumber() const {
    return p_RevisionNumber;
}

void
SgAsmCilAssemblyRef::set_RevisionNumber(uint16_t const& x) {
    this->p_RevisionNumber = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilAssemblyRef::get_Flags() const {
    return p_Flags;
}

void
SgAsmCilAssemblyRef::set_Flags(uint32_t const& x) {
    this->p_Flags = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilAssemblyRef::get_PublicKeyOrToken() const {
    return p_PublicKeyOrToken;
}

void
SgAsmCilAssemblyRef::set_PublicKeyOrToken(uint32_t const& x) {
    this->p_PublicKeyOrToken = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilAssemblyRef::get_Name() const {
    return p_Name;
}

void
SgAsmCilAssemblyRef::set_Name(uint32_t const& x) {
    this->p_Name = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilAssemblyRef::get_Culture() const {
    return p_Culture;
}

void
SgAsmCilAssemblyRef::set_Culture(uint32_t const& x) {
    this->p_Culture = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilAssemblyRef::get_HashValue() const {
    return p_HashValue;
}

void
SgAsmCilAssemblyRef::set_HashValue(uint32_t const& x) {
    this->p_HashValue = x;
    set_isModified(true);
}

SgAsmCilAssemblyRef::~SgAsmCilAssemblyRef() {
    destructorHelper();
}

SgAsmCilAssemblyRef::SgAsmCilAssemblyRef()
    : p_MajorVersion(0)
    , p_MinorVersion(0)
    , p_BuildNumber(0)
    , p_RevisionNumber(0)
    , p_Flags(0)
    , p_PublicKeyOrToken(0)
    , p_Name(0)
    , p_Culture(0)
    , p_HashValue(0) {}

void
SgAsmCilAssemblyRef::initializeProperties() {
    p_MajorVersion = 0;
    p_MinorVersion = 0;
    p_BuildNumber = 0;
    p_RevisionNumber = 0;
    p_Flags = 0;
    p_PublicKeyOrToken = 0;
    p_Name = 0;
    p_Culture = 0;
    p_HashValue = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
