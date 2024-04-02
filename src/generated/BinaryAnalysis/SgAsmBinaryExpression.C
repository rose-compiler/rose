//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmBinaryExpression            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmBinaryExpression_IMPL
#include <SgAsmBinaryExpression.h>

SgAsmExpression* const&
SgAsmBinaryExpression::get_lhs() const {
    return p_lhs;
}

void
SgAsmBinaryExpression::set_lhs(SgAsmExpression* const& x) {
    changeChildPointer(this->p_lhs, const_cast<SgAsmExpression*&>(x));
    set_isModified(true);
}

SgAsmExpression* const&
SgAsmBinaryExpression::get_rhs() const {
    return p_rhs;
}

void
SgAsmBinaryExpression::set_rhs(SgAsmExpression* const& x) {
    changeChildPointer(this->p_rhs, const_cast<SgAsmExpression*&>(x));
    set_isModified(true);
}

SgAsmBinaryExpression::~SgAsmBinaryExpression() {
    destructorHelper();
}

SgAsmBinaryExpression::SgAsmBinaryExpression()
    : p_lhs(nullptr)
    , p_rhs(nullptr) {}

// The association between constructor arguments and their classes:
//    property=lhs              class=SgAsmBinaryExpression
//    property=rhs              class=SgAsmBinaryExpression
SgAsmBinaryExpression::SgAsmBinaryExpression(SgAsmExpression* const& lhs,
                                             SgAsmExpression* const& rhs)
    : p_lhs(initParentPointer(lhs, this))
    , p_rhs(initParentPointer(rhs, this)) {}

void
SgAsmBinaryExpression::initializeProperties() {
    p_lhs = nullptr;
    p_rhs = nullptr;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
