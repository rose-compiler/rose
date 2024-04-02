//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmRegisterNames            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmRegisterNames_IMPL
#include <SgAsmRegisterNames.h>

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
    changeChildPointer(this->p_registers, const_cast<SgAsmRegisterReferenceExpressionPtrList&>(x));
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

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
