//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmBinaryRor            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmBinaryRor_IMPL
#include <sage3basic.h>

SgAsmBinaryRor::~SgAsmBinaryRor() {
    destructorHelper();
}

SgAsmBinaryRor::SgAsmBinaryRor() {}

// The association between constructor arguments and their classes:
//    property=lhs              class=SgAsmBinaryExpression
//    property=rhs              class=SgAsmBinaryExpression
SgAsmBinaryRor::SgAsmBinaryRor(SgAsmExpression* const& lhs,
                               SgAsmExpression* const& rhs)
    : SgAsmBinaryExpression(lhs, rhs) {}

void
SgAsmBinaryRor::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
