//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilEventMap            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCilEventMap_IMPL
#include <sage3basic.h>

uint32_t const&
SgAsmCilEventMap::get_Parent() const {
    return p_Parent;
}

void
SgAsmCilEventMap::set_Parent(uint32_t const& x) {
    this->p_Parent = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilEventMap::get_EventList() const {
    return p_EventList;
}

void
SgAsmCilEventMap::set_EventList(uint32_t const& x) {
    this->p_EventList = x;
    set_isModified(true);
}

SgAsmCilEventMap::~SgAsmCilEventMap() {
    destructorHelper();
}

SgAsmCilEventMap::SgAsmCilEventMap()
    : p_Parent(0)
    , p_EventList(0) {}

void
SgAsmCilEventMap::initializeProperties() {
    p_Parent = 0;
    p_EventList = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
