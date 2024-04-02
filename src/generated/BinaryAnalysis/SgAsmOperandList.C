//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmOperandList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmOperandList_IMPL
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
    changeChildPointer(this->p_operands, const_cast<SgAsmExpressionPtrList&>(x));
    set_isModified(true);
}

SgAsmOperandList::~SgAsmOperandList() {
    destructorHelper();
}

SgAsmOperandList::SgAsmOperandList() {}

void
SgAsmOperandList::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
