//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCommonSubExpression            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCommonSubExpression_IMPL
#include <sage3basic.h>

SgAsmExpression* const&
SgAsmCommonSubExpression::get_subexpression() const {
    return p_subexpression;
}

void
SgAsmCommonSubExpression::set_subexpression(SgAsmExpression* const& x) {
    changeChildPointer(this->p_subexpression, const_cast<SgAsmExpression*&>(x));
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

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
