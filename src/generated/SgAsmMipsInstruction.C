#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmMipsInstruction            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
Rose::BinaryAnalysis::MipsInstructionKind const&
SgAsmMipsInstruction::get_kind() const {
    return p_kind;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmMipsInstruction::set_kind(Rose::BinaryAnalysis::MipsInstructionKind const& x) {
    this->p_kind = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmMipsInstruction::~SgAsmMipsInstruction() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmMipsInstruction::SgAsmMipsInstruction()
    : p_kind(Rose::BinaryAnalysis::mips_unknown_instruction) {}

#line 305 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
//    property=mnemonic         class=SgAsmInstruction
//    property=kind             class=SgAsmMipsInstruction
#line 313 "src/Rosebud/RosettaGenerator.C"
SgAsmMipsInstruction::SgAsmMipsInstruction(rose_addr_t const& address,
                                           std::string const& mnemonic,
                                           Rose::BinaryAnalysis::MipsInstructionKind const& kind)
    : SgAsmInstruction(address, mnemonic)
    , p_kind(kind) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmMipsInstruction::initializeProperties() {
    p_kind = Rose::BinaryAnalysis::mips_unknown_instruction;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
