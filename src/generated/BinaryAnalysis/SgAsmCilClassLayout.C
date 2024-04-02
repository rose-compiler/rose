//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilClassLayout            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCilClassLayout_IMPL
#include <sage3basic.h>

uint16_t const&
SgAsmCilClassLayout::get_PackingSize() const {
    return p_PackingSize;
}

void
SgAsmCilClassLayout::set_PackingSize(uint16_t const& x) {
    this->p_PackingSize = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilClassLayout::get_ClassSize() const {
    return p_ClassSize;
}

void
SgAsmCilClassLayout::set_ClassSize(uint32_t const& x) {
    this->p_ClassSize = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilClassLayout::get_Parent() const {
    return p_Parent;
}

void
SgAsmCilClassLayout::set_Parent(uint32_t const& x) {
    this->p_Parent = x;
    set_isModified(true);
}

SgAsmCilClassLayout::~SgAsmCilClassLayout() {
    destructorHelper();
}

SgAsmCilClassLayout::SgAsmCilClassLayout()
    : p_PackingSize(0)
    , p_ClassSize(0)
    , p_Parent(0) {}

void
SgAsmCilClassLayout::initializeProperties() {
    p_PackingSize = 0;
    p_ClassSize = 0;
    p_Parent = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
