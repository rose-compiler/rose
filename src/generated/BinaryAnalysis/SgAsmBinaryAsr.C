//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmBinaryAsr            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmBinaryAsr_IMPL
#include <SgAsmBinaryAsr.h>

SgAsmBinaryAsr::~SgAsmBinaryAsr() {
    destructorHelper();
}

SgAsmBinaryAsr::SgAsmBinaryAsr() {}

// The association between constructor arguments and their classes:
//    property=lhs              class=SgAsmBinaryExpression
//    property=rhs              class=SgAsmBinaryExpression
SgAsmBinaryAsr::SgAsmBinaryAsr(SgAsmExpression* const& lhs,
                               SgAsmExpression* const& rhs)
    : SgAsmBinaryExpression(lhs, rhs) {}

void
SgAsmBinaryAsr::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
