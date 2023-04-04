#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmAarch32Instruction            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#ifdef ROSE_ENABLE_ASM_AARCH32
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
Rose::BinaryAnalysis::Aarch32InstructionKind const&
SgAsmAarch32Instruction::get_kind() const {
    return p_kind;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmAarch32Instruction::set_kind(Rose::BinaryAnalysis::Aarch32InstructionKind const& x) {
    this->p_kind = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
Rose::BinaryAnalysis::Aarch32InstructionCondition const&
SgAsmAarch32Instruction::get_condition() const {
    return p_condition;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmAarch32Instruction::set_condition(Rose::BinaryAnalysis::Aarch32InstructionCondition const& x) {
    this->p_condition = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
bool const&
SgAsmAarch32Instruction::get_updatesFlags() const {
    return p_updatesFlags;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmAarch32Instruction::set_updatesFlags(bool const& x) {
    this->p_updatesFlags = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
bool const&
SgAsmAarch32Instruction::get_writesBack() const {
    return p_writesBack;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmAarch32Instruction::set_writesBack(bool const& x) {
    this->p_writesBack = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
bool const&
SgAsmAarch32Instruction::get_writesToIp() const {
    return p_writesToIp;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmAarch32Instruction::set_writesToIp(bool const& x) {
    this->p_writesToIp = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmAarch32Instruction::~SgAsmAarch32Instruction() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmAarch32Instruction::SgAsmAarch32Instruction()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_kind(Rose::BinaryAnalysis::Aarch32InstructionKind::ARM_INS_INVALID)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_condition(Rose::BinaryAnalysis::Aarch32InstructionCondition::ARM_CC_INVALID)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_updatesFlags(false)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_writesBack(false)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_writesToIp(false) {}

#line 349 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
//    property=mnemonic         class=SgAsmInstruction
//    property=kind             class=SgAsmAarch32Instruction
//    property=condition        class=SgAsmAarch32Instruction
#line 357 "src/Rosebud/RosettaGenerator.C"
SgAsmAarch32Instruction::SgAsmAarch32Instruction(rose_addr_t const& address,
                                                 std::string const& mnemonic,
                                                 Rose::BinaryAnalysis::Aarch32InstructionKind const& kind,
                                                 Rose::BinaryAnalysis::Aarch32InstructionCondition const& condition)
    : SgAsmInstruction(address, mnemonic)
    , p_kind(kind)
    , p_condition(condition)
    , p_updatesFlags(false)
    , p_writesBack(false)
    , p_writesToIp(false) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmAarch32Instruction::initializeProperties() {
    p_kind = Rose::BinaryAnalysis::Aarch32InstructionKind::ARM_INS_INVALID;
    p_condition = Rose::BinaryAnalysis::Aarch32InstructionCondition::ARM_CC_INVALID;
    p_updatesFlags = false;
    p_writesBack = false;
    p_writesToIp = false;
}

#endif
#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
