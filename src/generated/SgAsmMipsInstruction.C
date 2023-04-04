#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmMipsInstruction            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
Rose::BinaryAnalysis::MipsInstructionKind const&
SgAsmMipsInstruction::get_kind() const {
    return p_kind;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmMipsInstruction::set_kind(Rose::BinaryAnalysis::MipsInstructionKind const& x) {
    this->p_kind = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmMipsInstruction::~SgAsmMipsInstruction() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmMipsInstruction::SgAsmMipsInstruction()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_kind(Rose::BinaryAnalysis::mips_unknown_instruction) {}

#line 349 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
//    property=mnemonic         class=SgAsmInstruction
//    property=kind             class=SgAsmMipsInstruction
#line 357 "src/Rosebud/RosettaGenerator.C"
SgAsmMipsInstruction::SgAsmMipsInstruction(rose_addr_t const& address,
                                           std::string const& mnemonic,
                                           Rose::BinaryAnalysis::MipsInstructionKind const& kind)
    : SgAsmInstruction(address, mnemonic)
    , p_kind(kind) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmMipsInstruction::initializeProperties() {
    p_kind = Rose::BinaryAnalysis::mips_unknown_instruction;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
