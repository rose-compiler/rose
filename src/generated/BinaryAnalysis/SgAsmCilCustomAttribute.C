//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilCustomAttribute            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#include <sage3basic.h>

uint32_t const&
SgAsmCilCustomAttribute::get_Parent() const {
    return p_Parent;
}

void
SgAsmCilCustomAttribute::set_Parent(uint32_t const& x) {
    this->p_Parent = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilCustomAttribute::get_Type() const {
    return p_Type;
}

void
SgAsmCilCustomAttribute::set_Type(uint32_t const& x) {
    this->p_Type = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilCustomAttribute::get_Value() const {
    return p_Value;
}

void
SgAsmCilCustomAttribute::set_Value(uint32_t const& x) {
    this->p_Value = x;
    set_isModified(true);
}

SgAsmCilCustomAttribute::~SgAsmCilCustomAttribute() {
    destructorHelper();
}

SgAsmCilCustomAttribute::SgAsmCilCustomAttribute()
    : p_Parent({})
    , p_Type({})
    , p_Value({}) {}

void
SgAsmCilCustomAttribute::initializeProperties() {
    p_Parent = {};
    p_Type = {};
    p_Value = {};
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
