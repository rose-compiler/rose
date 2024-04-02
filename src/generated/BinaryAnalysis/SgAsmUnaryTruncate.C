//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmUnaryTruncate            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmUnaryTruncate_IMPL
#include <SgAsmUnaryTruncate.h>

SgAsmUnaryTruncate::~SgAsmUnaryTruncate() {
    destructorHelper();
}

SgAsmUnaryTruncate::SgAsmUnaryTruncate() {}

// The association between constructor arguments and their classes:
//    property=operand          class=SgAsmUnaryExpression
SgAsmUnaryTruncate::SgAsmUnaryTruncate(SgAsmExpression* const& operand)
    : SgAsmUnaryExpression(operand) {}

void
SgAsmUnaryTruncate::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
