//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCommonSubExpression            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmExpression* const&
SgAsmCommonSubExpression::get_subexpression() const {
    return p_subexpression;
}

void
SgAsmCommonSubExpression::set_subexpression(SgAsmExpression* const& x) {
    this->p_subexpression = x;
    set_isModified(true);
}

SgAsmCommonSubExpression::~SgAsmCommonSubExpression() {
    destructorHelper();
}

SgAsmCommonSubExpression::SgAsmCommonSubExpression()
    : p_subexpression(nullptr) {}

void
SgAsmCommonSubExpression::initializeProperties() {
    p_subexpression = nullptr;
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
