//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilParam            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCilParam_IMPL
#include <sage3basic.h>

uint16_t const&
SgAsmCilParam::get_Flags() const {
    return p_Flags;
}

void
SgAsmCilParam::set_Flags(uint16_t const& x) {
    this->p_Flags = x;
    set_isModified(true);
}

uint16_t const&
SgAsmCilParam::get_Sequence() const {
    return p_Sequence;
}

void
SgAsmCilParam::set_Sequence(uint16_t const& x) {
    this->p_Sequence = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilParam::get_Name() const {
    return p_Name;
}

void
SgAsmCilParam::set_Name(uint32_t const& x) {
    this->p_Name = x;
    set_isModified(true);
}

SgAsmCilParam::~SgAsmCilParam() {
    destructorHelper();
}

SgAsmCilParam::SgAsmCilParam()
    : p_Flags(0)
    , p_Sequence(0)
    , p_Name(0) {}

void
SgAsmCilParam::initializeProperties() {
    p_Flags = 0;
    p_Sequence = 0;
    p_Name = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
