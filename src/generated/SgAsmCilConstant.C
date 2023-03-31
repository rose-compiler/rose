//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilConstant                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

uint8_t const&
SgAsmCilConstant::get_Type() const {
    return p_Type;
}

void
SgAsmCilConstant::set_Type(uint8_t const& x) {
    this->p_Type = x;
    set_isModified(true);
}

uint8_t const&
SgAsmCilConstant::get_Padding() const {
    return p_Padding;
}

void
SgAsmCilConstant::set_Padding(uint8_t const& x) {
    this->p_Padding = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilConstant::get_Parent() const {
    return p_Parent;
}

void
SgAsmCilConstant::set_Parent(uint32_t const& x) {
    this->p_Parent = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilConstant::get_Value() const {
    return p_Value;
}

void
SgAsmCilConstant::set_Value(uint32_t const& x) {
    this->p_Value = x;
    set_isModified(true);
}

SgAsmCilConstant::~SgAsmCilConstant() {
    destructorHelper();
}

SgAsmCilConstant::SgAsmCilConstant() {}

void
SgAsmCilConstant::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
