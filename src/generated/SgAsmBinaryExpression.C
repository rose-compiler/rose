//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmBinaryExpression            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmExpression* const&
SgAsmBinaryExpression::get_lhs() const {
    return p_lhs;
}

void
SgAsmBinaryExpression::set_lhs(SgAsmExpression* const& x) {
    this->p_lhs = x;
    set_isModified(true);
}

SgAsmExpression* const&
SgAsmBinaryExpression::get_rhs() const {
    return p_rhs;
}

void
SgAsmBinaryExpression::set_rhs(SgAsmExpression* const& x) {
    this->p_rhs = x;
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
    : p_lhs(lhs)
    , p_rhs(rhs) {}

void
SgAsmBinaryExpression::initializeProperties() {
    p_lhs = nullptr;
    p_rhs = nullptr;
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
