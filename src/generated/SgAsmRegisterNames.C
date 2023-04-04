//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmRegisterNames            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmRegisterReferenceExpressionPtrList const&
SgAsmRegisterNames::get_registers() const {
    return p_registers;
}

SgAsmRegisterReferenceExpressionPtrList&
SgAsmRegisterNames::get_registers() {
    return p_registers;
}

void
SgAsmRegisterNames::set_registers(SgAsmRegisterReferenceExpressionPtrList const& x) {
    this->p_registers = x;
    set_isModified(true);
}

unsigned const&
SgAsmRegisterNames::get_mask() const {
    return p_mask;
}

void
SgAsmRegisterNames::set_mask(unsigned const& x) {
    this->p_mask = x;
    set_isModified(true);
}

SgAsmRegisterNames::~SgAsmRegisterNames() {
    destructorHelper();
}

SgAsmRegisterNames::SgAsmRegisterNames()
    : p_mask(0) {}

void
SgAsmRegisterNames::initializeProperties() {
    p_mask = 0;
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
