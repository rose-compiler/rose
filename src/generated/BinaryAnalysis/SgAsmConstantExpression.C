//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmConstantExpression            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmConstantExpression_IMPL
#include <sage3basic.h>

Sawyer::Container::BitVector const&
SgAsmConstantExpression::get_bitVector() const {
    return p_bitVector;
}

Sawyer::Container::BitVector&
SgAsmConstantExpression::get_bitVector() {
    return p_bitVector;
}

void
SgAsmConstantExpression::set_bitVector(Sawyer::Container::BitVector const& x) {
    this->p_bitVector = x;
    set_isModified(true);
}

SgAsmConstantExpression::~SgAsmConstantExpression() {
    destructorHelper();
}

SgAsmConstantExpression::SgAsmConstantExpression() {}

void
SgAsmConstantExpression::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
