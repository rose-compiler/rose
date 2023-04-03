#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmNullInstruction            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmNullInstruction::~SgAsmNullInstruction() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmNullInstruction::SgAsmNullInstruction() {}

#line 305 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
//    property=mnemonic         class=SgAsmInstruction
#line 313 "src/Rosebud/RosettaGenerator.C"
SgAsmNullInstruction::SgAsmNullInstruction(rose_addr_t const& address,
                                           std::string const& mnemonic)
    : SgAsmInstruction(address, mnemonic) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNullInstruction::initializeProperties() {
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
