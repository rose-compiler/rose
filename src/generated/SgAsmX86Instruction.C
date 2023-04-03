#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmX86Instruction            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
Rose::BinaryAnalysis::X86InstructionKind const&
SgAsmX86Instruction::get_kind() const {
    return p_kind;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmX86Instruction::set_kind(Rose::BinaryAnalysis::X86InstructionKind const& x) {
    this->p_kind = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
Rose::BinaryAnalysis::X86InstructionSize const&
SgAsmX86Instruction::get_baseSize() const {
    return p_baseSize;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmX86Instruction::set_baseSize(Rose::BinaryAnalysis::X86InstructionSize const& x) {
    this->p_baseSize = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
Rose::BinaryAnalysis::X86InstructionSize const&
SgAsmX86Instruction::get_operandSize() const {
    return p_operandSize;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmX86Instruction::set_operandSize(Rose::BinaryAnalysis::X86InstructionSize const& x) {
    this->p_operandSize = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
Rose::BinaryAnalysis::X86InstructionSize const&
SgAsmX86Instruction::get_addressSize() const {
    return p_addressSize;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmX86Instruction::set_addressSize(Rose::BinaryAnalysis::X86InstructionSize const& x) {
    this->p_addressSize = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
bool const&
SgAsmX86Instruction::get_lockPrefix() const {
    return p_lockPrefix;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmX86Instruction::set_lockPrefix(bool const& x) {
    this->p_lockPrefix = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
Rose::BinaryAnalysis::X86RepeatPrefix const&
SgAsmX86Instruction::get_repeatPrefix() const {
    return p_repeatPrefix;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmX86Instruction::set_repeatPrefix(Rose::BinaryAnalysis::X86RepeatPrefix const& x) {
    this->p_repeatPrefix = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
Rose::BinaryAnalysis::X86BranchPrediction const&
SgAsmX86Instruction::get_branchPrediction() const {
    return p_branchPrediction;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmX86Instruction::set_branchPrediction(Rose::BinaryAnalysis::X86BranchPrediction const& x) {
    this->p_branchPrediction = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
Rose::BinaryAnalysis::X86SegmentRegister const&
SgAsmX86Instruction::get_segmentOverride() const {
    return p_segmentOverride;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmX86Instruction::set_segmentOverride(Rose::BinaryAnalysis::X86SegmentRegister const& x) {
    this->p_segmentOverride = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmX86Instruction::~SgAsmX86Instruction() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmX86Instruction::SgAsmX86Instruction()
    : p_kind(Rose::BinaryAnalysis::x86_unknown_instruction)
    , p_baseSize(Rose::BinaryAnalysis::x86_insnsize_none)
    , p_operandSize(Rose::BinaryAnalysis::x86_insnsize_none)
    , p_addressSize(Rose::BinaryAnalysis::x86_insnsize_none)
    , p_lockPrefix(false)
    , p_repeatPrefix(Rose::BinaryAnalysis::x86_repeat_none)
    , p_branchPrediction(Rose::BinaryAnalysis::x86_branch_prediction_none)
    , p_segmentOverride(Rose::BinaryAnalysis::x86_segreg_none) {}

#line 305 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=address          class=SgAsmStatement
//    property=mnemonic         class=SgAsmInstruction
//    property=kind             class=SgAsmX86Instruction
//    property=baseSize         class=SgAsmX86Instruction
//    property=operandSize      class=SgAsmX86Instruction
//    property=addressSize      class=SgAsmX86Instruction
#line 313 "src/Rosebud/RosettaGenerator.C"
SgAsmX86Instruction::SgAsmX86Instruction(rose_addr_t const& address,
                                         std::string const& mnemonic,
                                         Rose::BinaryAnalysis::X86InstructionKind const& kind,
                                         Rose::BinaryAnalysis::X86InstructionSize const& baseSize,
                                         Rose::BinaryAnalysis::X86InstructionSize const& operandSize,
                                         Rose::BinaryAnalysis::X86InstructionSize const& addressSize)
    : SgAsmInstruction(address, mnemonic)
    , p_kind(kind)
    , p_baseSize(baseSize)
    , p_operandSize(operandSize)
    , p_addressSize(addressSize) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmX86Instruction::initializeProperties() {
    p_kind = Rose::BinaryAnalysis::x86_unknown_instruction;
    p_baseSize = Rose::BinaryAnalysis::x86_insnsize_none;
    p_operandSize = Rose::BinaryAnalysis::x86_insnsize_none;
    p_addressSize = Rose::BinaryAnalysis::x86_insnsize_none;
    p_lockPrefix = false;
    p_repeatPrefix = Rose::BinaryAnalysis::x86_repeat_none;
    p_branchPrediction = Rose::BinaryAnalysis::x86_branch_prediction_none;
    p_segmentOverride = Rose::BinaryAnalysis::x86_segreg_none;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
