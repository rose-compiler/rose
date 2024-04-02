//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilFieldLayout            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCilFieldLayout_IMPL
#include <SgAsmCilFieldLayout.h>

uint32_t const&
SgAsmCilFieldLayout::get_Offset() const {
    return p_Offset;
}

void
SgAsmCilFieldLayout::set_Offset(uint32_t const& x) {
    this->p_Offset = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilFieldLayout::get_Field() const {
    return p_Field;
}

void
SgAsmCilFieldLayout::set_Field(uint32_t const& x) {
    this->p_Field = x;
    set_isModified(true);
}

SgAsmCilFieldLayout::~SgAsmCilFieldLayout() {
    destructorHelper();
}

SgAsmCilFieldLayout::SgAsmCilFieldLayout()
    : p_Offset(0)
    , p_Field(0) {}

void
SgAsmCilFieldLayout::initializeProperties() {
    p_Offset = 0;
    p_Field = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
