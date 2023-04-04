//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilGenericParam            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

uint16_t const&
SgAsmCilGenericParam::get_Number() const {
    return p_Number;
}

void
SgAsmCilGenericParam::set_Number(uint16_t const& x) {
    this->p_Number = x;
    set_isModified(true);
}

uint16_t const&
SgAsmCilGenericParam::get_Flags() const {
    return p_Flags;
}

void
SgAsmCilGenericParam::set_Flags(uint16_t const& x) {
    this->p_Flags = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilGenericParam::get_Owner() const {
    return p_Owner;
}

void
SgAsmCilGenericParam::set_Owner(uint32_t const& x) {
    this->p_Owner = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilGenericParam::get_Name() const {
    return p_Name;
}

void
SgAsmCilGenericParam::set_Name(uint32_t const& x) {
    this->p_Name = x;
    set_isModified(true);
}

SgAsmCilGenericParam::~SgAsmCilGenericParam() {
    destructorHelper();
}

SgAsmCilGenericParam::SgAsmCilGenericParam() {}

void
SgAsmCilGenericParam::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
