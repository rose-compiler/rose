//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilAssemblyRefOS            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCilAssemblyRefOS_IMPL
#include <SgAsmCilAssemblyRefOS.h>

uint32_t const&
SgAsmCilAssemblyRefOS::get_OSPlatformID() const {
    return p_OSPlatformID;
}

void
SgAsmCilAssemblyRefOS::set_OSPlatformID(uint32_t const& x) {
    this->p_OSPlatformID = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilAssemblyRefOS::get_OSMajorVersion() const {
    return p_OSMajorVersion;
}

void
SgAsmCilAssemblyRefOS::set_OSMajorVersion(uint32_t const& x) {
    this->p_OSMajorVersion = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilAssemblyRefOS::get_OSMinorVersion() const {
    return p_OSMinorVersion;
}

void
SgAsmCilAssemblyRefOS::set_OSMinorVersion(uint32_t const& x) {
    this->p_OSMinorVersion = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilAssemblyRefOS::get_AssemblyRefOS() const {
    return p_AssemblyRefOS;
}

void
SgAsmCilAssemblyRefOS::set_AssemblyRefOS(uint32_t const& x) {
    this->p_AssemblyRefOS = x;
    set_isModified(true);
}

SgAsmCilAssemblyRefOS::~SgAsmCilAssemblyRefOS() {
    destructorHelper();
}

SgAsmCilAssemblyRefOS::SgAsmCilAssemblyRefOS()
    : p_OSPlatformID(0)
    , p_OSMajorVersion(0)
    , p_OSMinorVersion(0)
    , p_AssemblyRefOS(0) {}

void
SgAsmCilAssemblyRefOS::initializeProperties() {
    p_OSPlatformID = 0;
    p_OSMajorVersion = 0;
    p_OSMinorVersion = 0;
    p_AssemblyRefOS = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
