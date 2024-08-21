//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilInstruction            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCilInstruction_IMPL
#include <SgAsmCilInstruction.h>

Rose::BinaryAnalysis::CilInstructionKind const&
SgAsmCilInstruction::get_kind() const {
    return p_kind;
}

void
SgAsmCilInstruction::set_kind(Rose::BinaryAnalysis::CilInstructionKind const& x) {
    this->p_kind = x;
    set_isModified(true);
}

SgAsmCilInstruction::~SgAsmCilInstruction() {
    destructorHelper();
}

SgAsmCilInstruction::SgAsmCilInstruction()
    : p_kind(Rose::BinaryAnalysis::Cil_unknown_instruction) {}

// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
//    property=architectureId   class=SgAsmInstruction
//    property=mnemonic         class=SgAsmInstruction
//    property=kind             class=SgAsmCilInstruction
SgAsmCilInstruction::SgAsmCilInstruction(rose_addr_t const& address,
                                         uint8_t const& architectureId,
                                         std::string const& mnemonic,
                                         Rose::BinaryAnalysis::CilInstructionKind const& kind)
    : SgAsmInstruction(address, architectureId, mnemonic)
    , p_kind(kind) {}

void
SgAsmCilInstruction::initializeProperties() {
    p_kind = Rose::BinaryAnalysis::Cil_unknown_instruction;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
