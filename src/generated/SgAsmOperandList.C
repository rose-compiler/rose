#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmOperandList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmExpressionPtrList const&
SgAsmOperandList::get_operands() const {
    return p_operands;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
SgAsmExpressionPtrList&
SgAsmOperandList::get_operands() {
    return p_operands;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmOperandList::set_operands(SgAsmExpressionPtrList const& x) {
    this->p_operands = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmOperandList::~SgAsmOperandList() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmOperandList::SgAsmOperandList() {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmOperandList::initializeProperties() {
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
