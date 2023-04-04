#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilInstruction            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
Rose::BinaryAnalysis::CilInstructionKind const&
SgAsmCilInstruction::get_kind() const {
    return p_kind;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCilInstruction::set_kind(Rose::BinaryAnalysis::CilInstructionKind const& x) {
    this->p_kind = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmCilInstruction::~SgAsmCilInstruction() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmCilInstruction::SgAsmCilInstruction()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_kind(Rose::BinaryAnalysis::Cil_unknown_instruction) {}

#line 349 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
//    property=mnemonic         class=SgAsmInstruction
//    property=kind             class=SgAsmCilInstruction
#line 357 "src/Rosebud/RosettaGenerator.C"
SgAsmCilInstruction::SgAsmCilInstruction(rose_addr_t const& address,
                                         std::string const& mnemonic,
                                         Rose::BinaryAnalysis::CilInstructionKind const& kind)
    : SgAsmInstruction(address, mnemonic)
    , p_kind(kind) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCilInstruction::initializeProperties() {
    p_kind = Rose::BinaryAnalysis::Cil_unknown_instruction;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
