//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmM68kInstruction            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmM68kInstruction_IMPL
#include <SgAsmM68kInstruction.h>

Rose::BinaryAnalysis::M68kInstructionKind const&
SgAsmM68kInstruction::get_kind() const {
    return p_kind;
}

void
SgAsmM68kInstruction::set_kind(Rose::BinaryAnalysis::M68kInstructionKind const& x) {
    this->p_kind = x;
    set_isModified(true);
}

SgAsmM68kInstruction::~SgAsmM68kInstruction() {
    destructorHelper();
}

SgAsmM68kInstruction::SgAsmM68kInstruction()
    : p_kind(Rose::BinaryAnalysis::m68k_unknown_instruction) {}

// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
//    property=architectureName class=SgAsmInstruction
//    property=mnemonic         class=SgAsmInstruction
//    property=kind             class=SgAsmM68kInstruction
SgAsmM68kInstruction::SgAsmM68kInstruction(rose_addr_t const& address,
                                           std::string const& architectureName,
                                           std::string const& mnemonic,
                                           Rose::BinaryAnalysis::M68kInstructionKind const& kind)
    : SgAsmInstruction(address, architectureName, mnemonic)
    , p_kind(kind) {}

void
SgAsmM68kInstruction::initializeProperties() {
    p_kind = Rose::BinaryAnalysis::m68k_unknown_instruction;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
