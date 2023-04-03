#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilInstruction            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
Rose::BinaryAnalysis::CilInstructionKind const&
SgAsmCilInstruction::get_kind() const {
    return p_kind;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCilInstruction::set_kind(Rose::BinaryAnalysis::CilInstructionKind const& x) {
    this->p_kind = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmCilInstruction::~SgAsmCilInstruction() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmCilInstruction::SgAsmCilInstruction()
    : p_kind(Rose::BinaryAnalysis::Cil_unknown_instruction) {}

#line 305 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
//    property=mnemonic         class=SgAsmInstruction
//    property=kind             class=SgAsmCilInstruction
#line 313 "src/Rosebud/RosettaGenerator.C"
SgAsmCilInstruction::SgAsmCilInstruction(rose_addr_t const& address,
                                         std::string const& mnemonic,
                                         Rose::BinaryAnalysis::CilInstructionKind const& kind)
    : SgAsmInstruction(address, mnemonic)
    , p_kind(kind) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCilInstruction::initializeProperties() {
    p_kind = Rose::BinaryAnalysis::Cil_unknown_instruction;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
