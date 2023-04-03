#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmM68kInstruction            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
Rose::BinaryAnalysis::M68kInstructionKind const&
SgAsmM68kInstruction::get_kind() const {
    return p_kind;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmM68kInstruction::set_kind(Rose::BinaryAnalysis::M68kInstructionKind const& x) {
    this->p_kind = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmM68kInstruction::~SgAsmM68kInstruction() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmM68kInstruction::SgAsmM68kInstruction()
    : p_kind(Rose::BinaryAnalysis::m68k_unknown_instruction) {}

#line 305 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
//    property=mnemonic         class=SgAsmInstruction
//    property=kind             class=SgAsmM68kInstruction
#line 313 "src/Rosebud/RosettaGenerator.C"
SgAsmM68kInstruction::SgAsmM68kInstruction(rose_addr_t const& address,
                                           std::string const& mnemonic,
                                           Rose::BinaryAnalysis::M68kInstructionKind const& kind)
    : SgAsmInstruction(address, mnemonic)
    , p_kind(kind) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmM68kInstruction::initializeProperties() {
    p_kind = Rose::BinaryAnalysis::m68k_unknown_instruction;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
