//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmMipsInstruction            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#include <sage3basic.h>

Rose::BinaryAnalysis::MipsInstructionKind const&
SgAsmMipsInstruction::get_kind() const {
    return p_kind;
}

void
SgAsmMipsInstruction::set_kind(Rose::BinaryAnalysis::MipsInstructionKind const& x) {
    this->p_kind = x;
    set_isModified(true);
}

SgAsmMipsInstruction::~SgAsmMipsInstruction() {
    destructorHelper();
}

SgAsmMipsInstruction::SgAsmMipsInstruction()
    : p_kind(Rose::BinaryAnalysis::mips_unknown_instruction) {}

// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
//    property=mnemonic         class=SgAsmInstruction
//    property=kind             class=SgAsmMipsInstruction
SgAsmMipsInstruction::SgAsmMipsInstruction(rose_addr_t const& address,
                                           std::string const& mnemonic,
                                           Rose::BinaryAnalysis::MipsInstructionKind const& kind)
    : SgAsmInstruction(address, mnemonic)
    , p_kind(kind) {}

void
SgAsmMipsInstruction::initializeProperties() {
    p_kind = Rose::BinaryAnalysis::mips_unknown_instruction;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
