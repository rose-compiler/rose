#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmNullInstruction            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmNullInstruction::~SgAsmNullInstruction() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmNullInstruction::SgAsmNullInstruction() {}

#line 349 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
//    property=mnemonic         class=SgAsmInstruction
#line 357 "src/Rosebud/RosettaGenerator.C"
SgAsmNullInstruction::SgAsmNullInstruction(rose_addr_t const& address,
                                           std::string const& mnemonic)
    : SgAsmInstruction(address, mnemonic) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNullInstruction::initializeProperties() {
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
