//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilEventMap                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
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

SgAsmCilEventMap::SgAsmCilEventMap() {}

void
SgAsmCilEventMap::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
