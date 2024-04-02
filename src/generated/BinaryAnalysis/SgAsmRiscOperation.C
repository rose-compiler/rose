//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmRiscOperation            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmRiscOperation_IMPL
#include <SgAsmRiscOperation.h>

SgAsmRiscOperation::RiscOperator const&
SgAsmRiscOperation::get_riscOperator() const {
    return p_riscOperator;
}

void
SgAsmRiscOperation::set_riscOperator(SgAsmRiscOperation::RiscOperator const& x) {
    this->p_riscOperator = x;
    set_isModified(true);
}

SgAsmExprListExp* const&
SgAsmRiscOperation::get_operands() const {
    return p_operands;
}

void
SgAsmRiscOperation::set_operands(SgAsmExprListExp* const& x) {
    changeChildPointer(this->p_operands, const_cast<SgAsmExprListExp*&>(x));
    set_isModified(true);
}

SgAsmRiscOperation::~SgAsmRiscOperation() {
    destructorHelper();
}

SgAsmRiscOperation::SgAsmRiscOperation()
    : p_riscOperator(OP_NONE)
    , p_operands(nullptr) {}

// The association between constructor arguments and their classes:
//    property=riscOperator     class=SgAsmRiscOperation
SgAsmRiscOperation::SgAsmRiscOperation(SgAsmRiscOperation::RiscOperator const& riscOperator)
    : p_riscOperator(riscOperator)
    , p_operands(nullptr) {}

void
SgAsmRiscOperation::initializeProperties() {
    p_riscOperator = OP_NONE;
    p_operands = nullptr;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
