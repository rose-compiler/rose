#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmRegisterNames            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmRegisterReferenceExpressionPtrList const&
SgAsmRegisterNames::get_registers() const {
    return p_registers;
}

#line 552 "src/Rosebud/RosettaGenerator.C"
SgAsmRegisterReferenceExpressionPtrList&
SgAsmRegisterNames::get_registers() {
    return p_registers;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmRegisterNames::set_registers(SgAsmRegisterReferenceExpressionPtrList const& x) {
    this->p_registers = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmRegisterNames::get_mask() const {
    return p_mask;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmRegisterNames::set_mask(unsigned const& x) {
    this->p_mask = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmRegisterNames::~SgAsmRegisterNames() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmRegisterNames::SgAsmRegisterNames()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_mask(0) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmRegisterNames::initializeProperties() {
    p_mask = 0;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
