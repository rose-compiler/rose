//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilAssemblyOS                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

uint32_t const&
SgAsmCilAssemblyOS::get_OSPlatformID() const {
    return p_OSPlatformID;
}

void
SgAsmCilAssemblyOS::set_OSPlatformID(uint32_t const& x) {
    this->p_OSPlatformID = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilAssemblyOS::get_OSMajorVersion() const {
    return p_OSMajorVersion;
}

void
SgAsmCilAssemblyOS::set_OSMajorVersion(uint32_t const& x) {
    this->p_OSMajorVersion = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilAssemblyOS::get_OSMinorVersion() const {
    return p_OSMinorVersion;
}

void
SgAsmCilAssemblyOS::set_OSMinorVersion(uint32_t const& x) {
    this->p_OSMinorVersion = x;
    set_isModified(true);
}

SgAsmCilAssemblyOS::~SgAsmCilAssemblyOS() {
    destructorHelper();
}

SgAsmCilAssemblyOS::SgAsmCilAssemblyOS() {}

void
SgAsmCilAssemblyOS::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
