#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

void
SgAsmOperandList::append_operand(SgAsmExpression* operand) {
    appendOperand(operand);
}

void
SgAsmOperandList::appendOperand(SgAsmExpression* operand) {
    ASSERT_not_null(operand);
    p_operands.push_back(operand);
    operand->set_parent(this);
}

#endif
