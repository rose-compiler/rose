#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmAarch64Instruction            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#ifdef ROSE_ENABLE_ASM_AARCH64
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
Rose::BinaryAnalysis::Aarch64InstructionKind const&
SgAsmAarch64Instruction::get_kind() const {
    return p_kind;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmAarch64Instruction::set_kind(Rose::BinaryAnalysis::Aarch64InstructionKind const& x) {
    this->p_kind = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
Rose::BinaryAnalysis::Aarch64InstructionCondition const&
SgAsmAarch64Instruction::get_condition() const {
    return p_condition;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmAarch64Instruction::set_condition(Rose::BinaryAnalysis::Aarch64InstructionCondition const& x) {
    this->p_condition = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
bool const&
SgAsmAarch64Instruction::get_updatesFlags() const {
    return p_updatesFlags;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmAarch64Instruction::set_updatesFlags(bool const& x) {
    this->p_updatesFlags = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmAarch64Instruction::~SgAsmAarch64Instruction() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmAarch64Instruction::SgAsmAarch64Instruction()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_kind(Rose::BinaryAnalysis::Aarch64InstructionKind::ARM64_INS_INVALID)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_condition(Rose::BinaryAnalysis::Aarch64InstructionCondition::ARM64_CC_INVALID)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_updatesFlags(false) {}

#line 349 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
//    property=mnemonic         class=SgAsmInstruction
//    property=kind             class=SgAsmAarch64Instruction
//    property=condition        class=SgAsmAarch64Instruction
#line 357 "src/Rosebud/RosettaGenerator.C"
SgAsmAarch64Instruction::SgAsmAarch64Instruction(rose_addr_t const& address,
                                                 std::string const& mnemonic,
                                                 Rose::BinaryAnalysis::Aarch64InstructionKind const& kind,
                                                 Rose::BinaryAnalysis::Aarch64InstructionCondition const& condition)
    : SgAsmInstruction(address, mnemonic)
    , p_kind(kind)
    , p_condition(condition)
    , p_updatesFlags(false) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmAarch64Instruction::initializeProperties() {
    p_kind = Rose::BinaryAnalysis::Aarch64InstructionKind::ARM64_INS_INVALID;
    p_condition = Rose::BinaryAnalysis::Aarch64InstructionCondition::ARM64_CC_INVALID;
    p_updatesFlags = false;
}

#endif
#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
