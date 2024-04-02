//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmBinaryMod            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmBinaryMod_IMPL
#include <SgAsmBinaryMod.h>

SgAsmBinaryMod::~SgAsmBinaryMod() {
    destructorHelper();
}

SgAsmBinaryMod::SgAsmBinaryMod() {}

// The association between constructor arguments and their classes:
//    property=lhs              class=SgAsmBinaryExpression
//    property=rhs              class=SgAsmBinaryExpression
SgAsmBinaryMod::SgAsmBinaryMod(SgAsmExpression* const& lhs,
                               SgAsmExpression* const& rhs)
    : SgAsmBinaryExpression(lhs, rhs) {}

void
SgAsmBinaryMod::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
