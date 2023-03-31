//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilParam                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
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

SgAsmCilParam::SgAsmCilParam() {}

void
SgAsmCilParam::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
