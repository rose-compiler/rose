//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmAarch32Instruction            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#ifdef ROSE_ENABLE_ASM_AARCH32
#define ROSE_SgAsmAarch32Instruction_IMPL
#include <SgAsmAarch32Instruction.h>

Rose::BinaryAnalysis::Aarch32InstructionKind const&
SgAsmAarch32Instruction::get_kind() const {
    return p_kind;
}

void
SgAsmAarch32Instruction::set_kind(Rose::BinaryAnalysis::Aarch32InstructionKind const& x) {
    this->p_kind = x;
    set_isModified(true);
}

Rose::BinaryAnalysis::Aarch32InstructionCondition const&
SgAsmAarch32Instruction::get_condition() const {
    return p_condition;
}

void
SgAsmAarch32Instruction::set_condition(Rose::BinaryAnalysis::Aarch32InstructionCondition const& x) {
    this->p_condition = x;
    set_isModified(true);
}

bool const&
SgAsmAarch32Instruction::get_updatesFlags() const {
    return p_updatesFlags;
}

void
SgAsmAarch32Instruction::set_updatesFlags(bool const& x) {
    this->p_updatesFlags = x;
    set_isModified(true);
}

bool const&
SgAsmAarch32Instruction::get_writesBack() const {
    return p_writesBack;
}

void
SgAsmAarch32Instruction::set_writesBack(bool const& x) {
    this->p_writesBack = x;
    set_isModified(true);
}

bool const&
SgAsmAarch32Instruction::get_writesToIp() const {
    return p_writesToIp;
}

void
SgAsmAarch32Instruction::set_writesToIp(bool const& x) {
    this->p_writesToIp = x;
    set_isModified(true);
}

SgAsmAarch32Instruction::~SgAsmAarch32Instruction() {
    destructorHelper();
}

SgAsmAarch32Instruction::SgAsmAarch32Instruction()
    : p_kind(Rose::BinaryAnalysis::Aarch32InstructionKind::ARM_INS_INVALID)
    , p_condition(Rose::BinaryAnalysis::Aarch32InstructionCondition::ARM_CC_INVALID)
    , p_updatesFlags(false)
    , p_writesBack(false)
    , p_writesToIp(false) {}

// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
//    property=architectureId   class=SgAsmInstruction
//    property=kind             class=SgAsmAarch32Instruction
//    property=condition        class=SgAsmAarch32Instruction
SgAsmAarch32Instruction::SgAsmAarch32Instruction(rose_addr_t const& address,
                                                 uint8_t const& architectureId,
                                                 Rose::BinaryAnalysis::Aarch32InstructionKind const& kind,
                                                 Rose::BinaryAnalysis::Aarch32InstructionCondition const& condition)
    : SgAsmInstruction(address, architectureId)
    , p_kind(kind)
    , p_condition(condition)
    , p_updatesFlags(false)
    , p_writesBack(false)
    , p_writesToIp(false) {}

void
SgAsmAarch32Instruction::initializeProperties() {
    p_kind = Rose::BinaryAnalysis::Aarch32InstructionKind::ARM_INS_INVALID;
    p_condition = Rose::BinaryAnalysis::Aarch32InstructionCondition::ARM_CC_INVALID;
    p_updatesFlags = false;
    p_writesBack = false;
    p_writesToIp = false;
}

#endif
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
