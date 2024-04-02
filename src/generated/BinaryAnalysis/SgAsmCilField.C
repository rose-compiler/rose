//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilField            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCilField_IMPL
#include <sage3basic.h>

uint16_t const&
SgAsmCilField::get_Flags() const {
    return p_Flags;
}

void
SgAsmCilField::set_Flags(uint16_t const& x) {
    this->p_Flags = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilField::get_Name() const {
    return p_Name;
}

void
SgAsmCilField::set_Name(uint32_t const& x) {
    this->p_Name = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilField::get_Signature() const {
    return p_Signature;
}

void
SgAsmCilField::set_Signature(uint32_t const& x) {
    this->p_Signature = x;
    set_isModified(true);
}

SgAsmCilField::~SgAsmCilField() {
    destructorHelper();
}

SgAsmCilField::SgAsmCilField()
    : p_Flags(0)
    , p_Name(0)
    , p_Signature(0) {}

void
SgAsmCilField::initializeProperties() {
    p_Flags = 0;
    p_Name = 0;
    p_Signature = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
