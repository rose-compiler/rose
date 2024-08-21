//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmNullInstruction            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmNullInstruction_IMPL
#include <SgAsmNullInstruction.h>

SgAsmNullInstruction::~SgAsmNullInstruction() {
    destructorHelper();
}

SgAsmNullInstruction::SgAsmNullInstruction() {}

// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
//    property=architectureId   class=SgAsmInstruction
//    property=mnemonic         class=SgAsmInstruction
SgAsmNullInstruction::SgAsmNullInstruction(rose_addr_t const& address,
                                           uint8_t const& architectureId,
                                           std::string const& mnemonic)
    : SgAsmInstruction(address, architectureId, mnemonic) {}

void
SgAsmNullInstruction::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
