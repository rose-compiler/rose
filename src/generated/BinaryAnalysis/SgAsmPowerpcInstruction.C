//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmPowerpcInstruction            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmPowerpcInstruction_IMPL
#include <SgAsmPowerpcInstruction.h>

Rose::BinaryAnalysis::PowerpcInstructionKind const&
SgAsmPowerpcInstruction::get_kind() const {
    return p_kind;
}

void
SgAsmPowerpcInstruction::set_kind(Rose::BinaryAnalysis::PowerpcInstructionKind const& x) {
    this->p_kind = x;
    set_isModified(true);
}

SgAsmPowerpcInstruction::~SgAsmPowerpcInstruction() {
    destructorHelper();
}

SgAsmPowerpcInstruction::SgAsmPowerpcInstruction()
    : p_kind(Rose::BinaryAnalysis::powerpc_unknown_instruction) {}

// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
//    property=architectureId   class=SgAsmInstruction
//    property=mnemonic         class=SgAsmInstruction
//    property=kind             class=SgAsmPowerpcInstruction
SgAsmPowerpcInstruction::SgAsmPowerpcInstruction(rose_addr_t const& address,
                                                 uint8_t const& architectureId,
                                                 std::string const& mnemonic,
                                                 Rose::BinaryAnalysis::PowerpcInstructionKind const& kind)
    : SgAsmInstruction(address, architectureId, mnemonic)
    , p_kind(kind) {}

void
SgAsmPowerpcInstruction::initializeProperties() {
    p_kind = Rose::BinaryAnalysis::powerpc_unknown_instruction;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
