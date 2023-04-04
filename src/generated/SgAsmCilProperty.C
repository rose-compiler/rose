//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilProperty            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

uint16_t const&
SgAsmCilProperty::get_Flags() const {
    return p_Flags;
}

void
SgAsmCilProperty::set_Flags(uint16_t const& x) {
    this->p_Flags = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilProperty::get_Name() const {
    return p_Name;
}

void
SgAsmCilProperty::set_Name(uint32_t const& x) {
    this->p_Name = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilProperty::get_Type() const {
    return p_Type;
}

void
SgAsmCilProperty::set_Type(uint32_t const& x) {
    this->p_Type = x;
    set_isModified(true);
}

SgAsmCilProperty::~SgAsmCilProperty() {
    destructorHelper();
}

SgAsmCilProperty::SgAsmCilProperty() {}

void
SgAsmCilProperty::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
