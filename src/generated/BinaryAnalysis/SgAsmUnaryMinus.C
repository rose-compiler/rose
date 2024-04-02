//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmUnaryMinus            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmUnaryMinus_IMPL
#include <SgAsmUnaryMinus.h>

SgAsmUnaryMinus::~SgAsmUnaryMinus() {
    destructorHelper();
}

SgAsmUnaryMinus::SgAsmUnaryMinus() {}

// The association between constructor arguments and their classes:
//    property=operand          class=SgAsmUnaryExpression
SgAsmUnaryMinus::SgAsmUnaryMinus(SgAsmExpression* const& operand)
    : SgAsmUnaryExpression(operand) {}

void
SgAsmUnaryMinus::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
