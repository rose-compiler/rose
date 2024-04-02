//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmBinaryLsr            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmBinaryLsr_IMPL
#include <SgAsmBinaryLsr.h>

SgAsmBinaryLsr::~SgAsmBinaryLsr() {
    destructorHelper();
}

SgAsmBinaryLsr::SgAsmBinaryLsr() {}

// The association between constructor arguments and their classes:
//    property=lhs              class=SgAsmBinaryExpression
//    property=rhs              class=SgAsmBinaryExpression
SgAsmBinaryLsr::SgAsmBinaryLsr(SgAsmExpression* const& lhs,
                               SgAsmExpression* const& rhs)
    : SgAsmBinaryExpression(lhs, rhs) {}

void
SgAsmBinaryLsr::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
