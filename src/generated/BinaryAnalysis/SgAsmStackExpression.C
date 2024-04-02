//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmStackExpression            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmStackExpression_IMPL
#include <SgAsmStackExpression.h>

int const&
SgAsmStackExpression::get_stackPosition() const {
    return p_stackPosition;
}

void
SgAsmStackExpression::set_stackPosition(int const& x) {
    this->p_stackPosition = x;
    set_isModified(true);
}

SgAsmStackExpression::~SgAsmStackExpression() {
    destructorHelper();
}

SgAsmStackExpression::SgAsmStackExpression()
    : p_stackPosition(0) {}

// The association between constructor arguments and their classes:
//    property=stackPosition    class=SgAsmStackExpression
SgAsmStackExpression::SgAsmStackExpression(int const& stackPosition)
    : p_stackPosition(stackPosition) {}

void
SgAsmStackExpression::initializeProperties() {
    p_stackPosition = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
