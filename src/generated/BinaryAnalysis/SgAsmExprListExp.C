//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmExprListExp            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmExprListExp_IMPL
#include <sage3basic.h>

SgAsmExpressionPtrList const&
SgAsmExprListExp::get_expressions() const {
    return p_expressions;
}

SgAsmExpressionPtrList&
SgAsmExprListExp::get_expressions() {
    return p_expressions;
}

void
SgAsmExprListExp::set_expressions(SgAsmExpressionPtrList const& x) {
    changeChildPointer(this->p_expressions, const_cast<SgAsmExpressionPtrList&>(x));
    set_isModified(true);
}

SgAsmExprListExp::~SgAsmExprListExp() {
    destructorHelper();
}

SgAsmExprListExp::SgAsmExprListExp() {}

void
SgAsmExprListExp::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
