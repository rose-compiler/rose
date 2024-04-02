//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmUnaryRrx            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmUnaryRrx_IMPL
#include <SgAsmUnaryRrx.h>

SgAsmUnaryRrx::~SgAsmUnaryRrx() {
    destructorHelper();
}

SgAsmUnaryRrx::SgAsmUnaryRrx() {}

// The association between constructor arguments and their classes:
//    property=operand          class=SgAsmUnaryExpression
SgAsmUnaryRrx::SgAsmUnaryRrx(SgAsmExpression* const& operand)
    : SgAsmUnaryExpression(operand) {}

void
SgAsmUnaryRrx::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
