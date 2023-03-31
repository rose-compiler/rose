//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmExprListExp                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
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
    this->p_expressions = x;
    set_isModified(true);
}

SgAsmExprListExp::~SgAsmExprListExp() {
    destructorHelper();
}

SgAsmExprListExp::SgAsmExprListExp() {}

void
SgAsmExprListExp::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
