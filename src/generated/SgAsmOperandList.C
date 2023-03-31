//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmOperandList                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmExpressionPtrList const&
SgAsmOperandList::get_operands() const {
    return p_operands;
}

SgAsmExpressionPtrList&
SgAsmOperandList::get_operands() {
    return p_operands;
}

void
SgAsmOperandList::set_operands(SgAsmExpressionPtrList const& x) {
    this->p_operands = x;
    set_isModified(true);
}

SgAsmOperandList::~SgAsmOperandList() {
    destructorHelper();
}

SgAsmOperandList::SgAsmOperandList() {}

void
SgAsmOperandList::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
