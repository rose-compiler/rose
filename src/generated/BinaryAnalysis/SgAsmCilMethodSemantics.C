//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilMethodSemantics            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#include <sage3basic.h>

uint16_t const&
SgAsmCilMethodSemantics::get_Semantics() const {
    return p_Semantics;
}

void
SgAsmCilMethodSemantics::set_Semantics(uint16_t const& x) {
    this->p_Semantics = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilMethodSemantics::get_Method() const {
    return p_Method;
}

void
SgAsmCilMethodSemantics::set_Method(uint32_t const& x) {
    this->p_Method = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilMethodSemantics::get_Association() const {
    return p_Association;
}

void
SgAsmCilMethodSemantics::set_Association(uint32_t const& x) {
    this->p_Association = x;
    set_isModified(true);
}

SgAsmCilMethodSemantics::~SgAsmCilMethodSemantics() {
    destructorHelper();
}

SgAsmCilMethodSemantics::SgAsmCilMethodSemantics() {}

void
SgAsmCilMethodSemantics::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
