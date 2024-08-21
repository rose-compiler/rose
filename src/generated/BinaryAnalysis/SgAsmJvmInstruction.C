//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmInstruction            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmInstruction_IMPL
#include <SgAsmJvmInstruction.h>

Rose::BinaryAnalysis::JvmInstructionKind const&
SgAsmJvmInstruction::get_kind() const {
    return p_kind;
}

void
SgAsmJvmInstruction::set_kind(Rose::BinaryAnalysis::JvmInstructionKind const& x) {
    this->p_kind = x;
    set_isModified(true);
}

SgAsmJvmInstruction::~SgAsmJvmInstruction() {
    destructorHelper();
}

SgAsmJvmInstruction::SgAsmJvmInstruction()
    : p_kind(Rose::BinaryAnalysis::JvmInstructionKind::unknown) {}

// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
//    property=architectureId   class=SgAsmInstruction
//    property=mnemonic         class=SgAsmInstruction
//    property=kind             class=SgAsmJvmInstruction
SgAsmJvmInstruction::SgAsmJvmInstruction(rose_addr_t const& address,
                                         uint8_t const& architectureId,
                                         std::string const& mnemonic,
                                         Rose::BinaryAnalysis::JvmInstructionKind const& kind)
    : SgAsmInstruction(address, architectureId, mnemonic)
    , p_kind(kind) {}

void
SgAsmJvmInstruction::initializeProperties() {
    p_kind = Rose::BinaryAnalysis::JvmInstructionKind::unknown;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
