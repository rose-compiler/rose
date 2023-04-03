#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmAarch32Instruction            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#ifdef ROSE_ENABLE_ASM_AARCH32
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
Rose::BinaryAnalysis::Aarch32InstructionKind const&
SgAsmAarch32Instruction::get_kind() const {
    return p_kind;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmAarch32Instruction::set_kind(Rose::BinaryAnalysis::Aarch32InstructionKind const& x) {
    this->p_kind = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
Rose::BinaryAnalysis::Aarch32InstructionCondition const&
SgAsmAarch32Instruction::get_condition() const {
    return p_condition;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmAarch32Instruction::set_condition(Rose::BinaryAnalysis::Aarch32InstructionCondition const& x) {
    this->p_condition = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
bool const&
SgAsmAarch32Instruction::get_updatesFlags() const {
    return p_updatesFlags;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmAarch32Instruction::set_updatesFlags(bool const& x) {
    this->p_updatesFlags = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
bool const&
SgAsmAarch32Instruction::get_writesBack() const {
    return p_writesBack;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmAarch32Instruction::set_writesBack(bool const& x) {
    this->p_writesBack = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
bool const&
SgAsmAarch32Instruction::get_writesToIp() const {
    return p_writesToIp;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmAarch32Instruction::set_writesToIp(bool const& x) {
    this->p_writesToIp = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmAarch32Instruction::~SgAsmAarch32Instruction() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmAarch32Instruction::SgAsmAarch32Instruction()
    : p_kind(Rose::BinaryAnalysis::Aarch32InstructionKind::ARM_INS_INVALID)
    , p_condition(Rose::BinaryAnalysis::Aarch32InstructionCondition::ARM_CC_INVALID)
    , p_updatesFlags(false)
    , p_writesBack(false)
    , p_writesToIp(false) {}

#line 305 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
//    property=mnemonic         class=SgAsmInstruction
//    property=kind             class=SgAsmAarch32Instruction
//    property=condition        class=SgAsmAarch32Instruction
#line 313 "src/Rosebud/RosettaGenerator.C"
SgAsmAarch32Instruction::SgAsmAarch32Instruction(rose_addr_t const& address,
                                                 std::string const& mnemonic,
                                                 Rose::BinaryAnalysis::Aarch32InstructionKind const& kind,
                                                 Rose::BinaryAnalysis::Aarch32InstructionCondition const& condition)
    : SgAsmInstruction(address, mnemonic)
    , p_kind(kind)
    , p_condition(condition) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmAarch32Instruction::initializeProperties() {
    p_kind = Rose::BinaryAnalysis::Aarch32InstructionKind::ARM_INS_INVALID;
    p_condition = Rose::BinaryAnalysis::Aarch32InstructionCondition::ARM_CC_INVALID;
    p_updatesFlags = false;
    p_writesBack = false;
    p_writesToIp = false;
}

#endif
#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
