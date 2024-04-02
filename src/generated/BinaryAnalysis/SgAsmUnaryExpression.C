//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmUnaryExpression            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmUnaryExpression_IMPL
#include <sage3basic.h>

SgAsmExpression* const&
SgAsmUnaryExpression::get_operand() const {
    return p_operand;
}

void
SgAsmUnaryExpression::set_operand(SgAsmExpression* const& x) {
    changeChildPointer(this->p_operand, const_cast<SgAsmExpression*&>(x));
    set_isModified(true);
}

SgAsmUnaryExpression::~SgAsmUnaryExpression() {
    destructorHelper();
}

SgAsmUnaryExpression::SgAsmUnaryExpression()
    : p_operand(nullptr) {}

// The association between constructor arguments and their classes:
//    property=operand          class=SgAsmUnaryExpression
SgAsmUnaryExpression::SgAsmUnaryExpression(SgAsmExpression* const& operand)
    : p_operand(initParentPointer(operand, this)) {}

void
SgAsmUnaryExpression::initializeProperties() {
    p_operand = nullptr;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
