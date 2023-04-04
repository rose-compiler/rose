#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmOperandList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmExpressionPtrList const&
SgAsmOperandList::get_operands() const {
    return p_operands;
}

#line 552 "src/Rosebud/RosettaGenerator.C"
SgAsmExpressionPtrList&
SgAsmOperandList::get_operands() {
    return p_operands;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmOperandList::set_operands(SgAsmExpressionPtrList const& x) {
    this->p_operands = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmOperandList::~SgAsmOperandList() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmOperandList::SgAsmOperandList() {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmOperandList::initializeProperties() {
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
