//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilImplMap            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCilImplMap_IMPL
#include <SgAsmCilImplMap.h>

uint16_t const&
SgAsmCilImplMap::get_MappingFlags() const {
    return p_MappingFlags;
}

void
SgAsmCilImplMap::set_MappingFlags(uint16_t const& x) {
    this->p_MappingFlags = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilImplMap::get_MemberForwarded() const {
    return p_MemberForwarded;
}

void
SgAsmCilImplMap::set_MemberForwarded(uint32_t const& x) {
    this->p_MemberForwarded = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilImplMap::get_ImportName() const {
    return p_ImportName;
}

void
SgAsmCilImplMap::set_ImportName(uint32_t const& x) {
    this->p_ImportName = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilImplMap::get_ImportScope() const {
    return p_ImportScope;
}

void
SgAsmCilImplMap::set_ImportScope(uint32_t const& x) {
    this->p_ImportScope = x;
    set_isModified(true);
}

SgAsmCilImplMap::~SgAsmCilImplMap() {
    destructorHelper();
}

SgAsmCilImplMap::SgAsmCilImplMap()
    : p_MappingFlags(0)
    , p_MemberForwarded(0)
    , p_ImportName(0)
    , p_ImportScope(0) {}

void
SgAsmCilImplMap::initializeProperties() {
    p_MappingFlags = 0;
    p_MemberForwarded = 0;
    p_ImportName = 0;
    p_ImportScope = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
