//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmNullInstruction            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#include <sage3basic.h>

SgAsmNullInstruction::~SgAsmNullInstruction() {
    destructorHelper();
}

SgAsmNullInstruction::SgAsmNullInstruction() {}

// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
//    property=mnemonic         class=SgAsmInstruction
SgAsmNullInstruction::SgAsmNullInstruction(rose_addr_t const& address,
                                           std::string const& mnemonic)
    : SgAsmInstruction(address, mnemonic) {}

void
SgAsmNullInstruction::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
