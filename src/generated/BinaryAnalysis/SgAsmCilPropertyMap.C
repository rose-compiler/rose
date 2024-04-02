//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilPropertyMap            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCilPropertyMap_IMPL
#include <SgAsmCilPropertyMap.h>

uint32_t const&
SgAsmCilPropertyMap::get_Parent() const {
    return p_Parent;
}

void
SgAsmCilPropertyMap::set_Parent(uint32_t const& x) {
    this->p_Parent = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilPropertyMap::get_PropertyList() const {
    return p_PropertyList;
}

void
SgAsmCilPropertyMap::set_PropertyList(uint32_t const& x) {
    this->p_PropertyList = x;
    set_isModified(true);
}

SgAsmCilPropertyMap::~SgAsmCilPropertyMap() {
    destructorHelper();
}

SgAsmCilPropertyMap::SgAsmCilPropertyMap()
    : p_Parent(0)
    , p_PropertyList(0) {}

void
SgAsmCilPropertyMap::initializeProperties() {
    p_Parent = 0;
    p_PropertyList = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
