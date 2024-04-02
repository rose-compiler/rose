//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmUnaryPlus            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmUnaryPlus_IMPL
#include <SgAsmUnaryPlus.h>

SgAsmUnaryPlus::~SgAsmUnaryPlus() {
    destructorHelper();
}

SgAsmUnaryPlus::SgAsmUnaryPlus() {}

// The association between constructor arguments and their classes:
//    property=operand          class=SgAsmUnaryExpression
SgAsmUnaryPlus::SgAsmUnaryPlus(SgAsmExpression* const& operand)
    : SgAsmUnaryExpression(operand) {}

void
SgAsmUnaryPlus::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
