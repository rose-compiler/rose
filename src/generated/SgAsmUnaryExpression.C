//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmUnaryExpression            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmExpression* const&
SgAsmUnaryExpression::get_operand() const {
    return p_operand;
}

void
SgAsmUnaryExpression::set_operand(SgAsmExpression* const& x) {
    this->p_operand = x;
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
    : p_operand(operand) {}

void
SgAsmUnaryExpression::initializeProperties() {
    p_operand = nullptr;
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
