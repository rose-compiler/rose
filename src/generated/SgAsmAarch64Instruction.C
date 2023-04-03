#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmAarch64Instruction            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#ifdef ROSE_ENABLE_ASM_AARCH64
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
Rose::BinaryAnalysis::Aarch64InstructionKind const&
SgAsmAarch64Instruction::get_kind() const {
    return p_kind;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmAarch64Instruction::set_kind(Rose::BinaryAnalysis::Aarch64InstructionKind const& x) {
    this->p_kind = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
Rose::BinaryAnalysis::Aarch64InstructionCondition const&
SgAsmAarch64Instruction::get_condition() const {
    return p_condition;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmAarch64Instruction::set_condition(Rose::BinaryAnalysis::Aarch64InstructionCondition const& x) {
    this->p_condition = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
bool const&
SgAsmAarch64Instruction::get_updatesFlags() const {
    return p_updatesFlags;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmAarch64Instruction::set_updatesFlags(bool const& x) {
    this->p_updatesFlags = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmAarch64Instruction::~SgAsmAarch64Instruction() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmAarch64Instruction::SgAsmAarch64Instruction()
    : p_kind(Rose::BinaryAnalysis::Aarch64InstructionKind::ARM64_INS_INVALID)
    , p_condition(Rose::BinaryAnalysis::Aarch64InstructionCondition::ARM64_CC_INVALID)
    , p_updatesFlags(false) {}

#line 305 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
//    property=mnemonic         class=SgAsmInstruction
//    property=kind             class=SgAsmAarch64Instruction
//    property=condition        class=SgAsmAarch64Instruction
#line 313 "src/Rosebud/RosettaGenerator.C"
SgAsmAarch64Instruction::SgAsmAarch64Instruction(rose_addr_t const& address,
                                                 std::string const& mnemonic,
                                                 Rose::BinaryAnalysis::Aarch64InstructionKind const& kind,
                                                 Rose::BinaryAnalysis::Aarch64InstructionCondition const& condition)
    : SgAsmInstruction(address, mnemonic)
    , p_kind(kind)
    , p_condition(condition) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmAarch64Instruction::initializeProperties() {
    p_kind = Rose::BinaryAnalysis::Aarch64InstructionKind::ARM64_INS_INVALID;
    p_condition = Rose::BinaryAnalysis::Aarch64InstructionCondition::ARM64_CC_INVALID;
    p_updatesFlags = false;
}

#endif
#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
