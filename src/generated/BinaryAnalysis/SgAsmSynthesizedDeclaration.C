//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmSynthesizedDeclaration            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmSynthesizedDeclaration_IMPL
#include <SgAsmSynthesizedDeclaration.h>

SgAsmSynthesizedDeclaration::~SgAsmSynthesizedDeclaration() {
    destructorHelper();
}

SgAsmSynthesizedDeclaration::SgAsmSynthesizedDeclaration() {}

// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
SgAsmSynthesizedDeclaration::SgAsmSynthesizedDeclaration(Rose::BinaryAnalysis::Address const& address)
    : SgAsmStatement(address) {}

void
SgAsmSynthesizedDeclaration::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
