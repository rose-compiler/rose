//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmAarch64Instruction            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#ifdef ROSE_ENABLE_ASM_AARCH64
#define ROSE_SgAsmAarch64Instruction_IMPL
#include <SgAsmAarch64Instruction.h>

Rose::BinaryAnalysis::Aarch64InstructionKind const&
SgAsmAarch64Instruction::get_kind() const {
    return p_kind;
}

void
SgAsmAarch64Instruction::set_kind(Rose::BinaryAnalysis::Aarch64InstructionKind const& x) {
    this->p_kind = x;
    set_isModified(true);
}

Rose::BinaryAnalysis::Aarch64InstructionCondition const&
SgAsmAarch64Instruction::get_condition() const {
    return p_condition;
}

void
SgAsmAarch64Instruction::set_condition(Rose::BinaryAnalysis::Aarch64InstructionCondition const& x) {
    this->p_condition = x;
    set_isModified(true);
}

bool const&
SgAsmAarch64Instruction::get_updatesFlags() const {
    return p_updatesFlags;
}

void
SgAsmAarch64Instruction::set_updatesFlags(bool const& x) {
    this->p_updatesFlags = x;
    set_isModified(true);
}

SgAsmAarch64Instruction::~SgAsmAarch64Instruction() {
    destructorHelper();
}

SgAsmAarch64Instruction::SgAsmAarch64Instruction()
    : p_kind(Rose::BinaryAnalysis::Aarch64InstructionKind::ARM64_INS_INVALID)
    , p_condition(Rose::BinaryAnalysis::Aarch64InstructionCondition::ARM64_CC_INVALID)
    , p_updatesFlags(false) {}

// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
//    property=architectureId   class=SgAsmInstruction
//    property=kind             class=SgAsmAarch64Instruction
//    property=condition        class=SgAsmAarch64Instruction
SgAsmAarch64Instruction::SgAsmAarch64Instruction(rose_addr_t const& address,
                                                 uint8_t const& architectureId,
                                                 Rose::BinaryAnalysis::Aarch64InstructionKind const& kind,
                                                 Rose::BinaryAnalysis::Aarch64InstructionCondition const& condition)
    : SgAsmInstruction(address, architectureId)
    , p_kind(kind)
    , p_condition(condition)
    , p_updatesFlags(false) {}

void
SgAsmAarch64Instruction::initializeProperties() {
    p_kind = Rose::BinaryAnalysis::Aarch64InstructionKind::ARM64_INS_INVALID;
    p_condition = Rose::BinaryAnalysis::Aarch64InstructionCondition::ARM64_CC_INVALID;
    p_updatesFlags = false;
}

#endif
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
