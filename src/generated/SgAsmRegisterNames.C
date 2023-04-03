#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmRegisterNames            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmRegisterReferenceExpressionPtrList const&
SgAsmRegisterNames::get_registers() const {
    return p_registers;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
SgAsmRegisterReferenceExpressionPtrList&
SgAsmRegisterNames::get_registers() {
    return p_registers;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmRegisterNames::set_registers(SgAsmRegisterReferenceExpressionPtrList const& x) {
    this->p_registers = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmRegisterNames::get_mask() const {
    return p_mask;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmRegisterNames::set_mask(unsigned const& x) {
    this->p_mask = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmRegisterNames::~SgAsmRegisterNames() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmRegisterNames::SgAsmRegisterNames()
    : p_mask(0) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmRegisterNames::initializeProperties() {
    p_mask = 0;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
