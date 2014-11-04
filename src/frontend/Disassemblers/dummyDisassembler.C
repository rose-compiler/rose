// ROSETTA has no way to exclude certain IR node definitions based on the user's choice of languages.  Therefore, we need to
// provide (stub) definitions for those nodes' virtual methods in order to get things to compile when those nodes aren't
// actually being used anywhere.
#include "sage3basic.h"
#include "Diagnostics.h"
#include "Disassembler.h"
#include "Partitioner.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmFloatValueExpression

double SgAsmFloatValueExpression::get_nativeValue() const { abort(); }
void SgAsmFloatValueExpression::set_nativeValue(double) { abort(); }
void SgAsmFloatValueExpression::updateBitVector() { abort(); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmIntegerValueExpression

uint64_t SgAsmIntegerValueExpression::virtualAddress(SgNode*) { abort(); }
std::string SgAsmIntegerValueExpression::get_label(bool) const { abort(); }
size_t SgAsmIntegerValueExpression::get_significantBits() const { abort(); }
void SgAsmIntegerValueExpression::makeRelativeTo(SgNode*) { abort(); }
uint64_t SgAsmIntegerValueExpression::get_baseAddress() const { abort(); }
uint64_t SgAsmIntegerValueExpression::get_absoluteValue(size_t) const { abort(); }
int64_t SgAsmIntegerValueExpression::get_signedValue() const { abort(); }
void SgAsmIntegerValueExpression::set_absoluteValue(uint64_t) { abort(); }
int64_t SgAsmIntegerValueExpression::get_relativeValue() const { abort(); }
void SgAsmIntegerValueExpression::set_relativeValue(int64_t, size_t) { abort(); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmInstruction

size_t SgAsmInstruction::get_size() const { return 0; }
bool SgAsmInstruction::terminatesBasicBlock() { return false; }
bool SgAsmInstruction::isUnknown() const { return false; }
bool SgAsmInstruction::hasEffect() { return false; }
bool SgAsmInstruction::hasEffect(const std::vector<SgAsmInstruction*>&, bool, bool) { return false; }
bool SgAsmInstruction::isFunctionCallFast(const std::vector<SgAsmInstruction*>&, rose_addr_t*, rose_addr_t*) { return false; }
bool SgAsmInstruction::isFunctionCallSlow(const std::vector<SgAsmInstruction*>&, rose_addr_t*, rose_addr_t*) { return false; }
bool SgAsmInstruction::isFunctionReturnFast(const std::vector<SgAsmInstruction*>&) { return false; }
bool SgAsmInstruction::isFunctionReturnSlow(const std::vector<SgAsmInstruction*>&) { return false; }
bool SgAsmInstruction::getBranchTarget(rose_addr_t*) { return false; }
std::set<rose_addr_t> SgAsmInstruction::getSuccessors(bool* complete) { return std::set<rose_addr_t>();}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmArmInstruction

bool SgAsmArmInstruction::terminatesBasicBlock() { return false; }
bool SgAsmArmInstruction::isUnknown() const { return false; }
std::set<rose_addr_t> SgAsmArmInstruction::getSuccessors(bool* complete) { return std::set<rose_addr_t>(); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmM68kInstruction

bool SgAsmM68kInstruction::terminatesBasicBlock() { return false; }
bool SgAsmM68kInstruction::isFunctionCallFast(const std::vector<SgAsmInstruction*>&, rose_addr_t*, rose_addr_t*) { return false; }
bool SgAsmM68kInstruction::isFunctionCallSlow(const std::vector<SgAsmInstruction*>&, rose_addr_t*, rose_addr_t*) { return false; }
bool SgAsmM68kInstruction::isFunctionReturnFast(const std::vector<SgAsmInstruction*>&) { return false; }
bool SgAsmM68kInstruction::isFunctionReturnSlow(const std::vector<SgAsmInstruction*>&) { return false; }
bool SgAsmM68kInstruction::isUnknown() const { return false; }
bool SgAsmM68kInstruction::getBranchTarget(rose_addr_t*) { return false; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmMipsInstruction

bool SgAsmMipsInstruction::terminatesBasicBlock() { return false; }
bool SgAsmMipsInstruction::isUnknown() const { return false; }
bool SgAsmMipsInstruction::isFunctionCallFast(const std::vector<SgAsmInstruction*>&, rose_addr_t*, rose_addr_t*) { return false; }
bool SgAsmMipsInstruction::isFunctionCallSlow(const std::vector<SgAsmInstruction*>&, rose_addr_t*, rose_addr_t*) { return false; }
bool SgAsmMipsInstruction::isFunctionReturnFast(const std::vector<SgAsmInstruction*>&) { return false; }
bool SgAsmMipsInstruction::isFunctionReturnSlow(const std::vector<SgAsmInstruction*>&) { return false; }
bool SgAsmMipsInstruction::getBranchTarget(rose_addr_t*) { return false; }
std::set<rose_addr_t> SgAsmMipsInstruction::getSuccessors(bool*) { return std::set<rose_addr_t>();}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmPowerpcInstruction

bool SgAsmPowerpcInstruction::terminatesBasicBlock() { return false; }
bool SgAsmPowerpcInstruction::isUnknown() const { return false; } 
std::set<rose_addr_t> SgAsmPowerpcInstruction::getSuccessors(bool*) { return std::set<rose_addr_t>();}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmX86Instruction

bool SgAsmX86Instruction::terminatesBasicBlock() { return false; }
bool SgAsmX86Instruction::isUnknown() const { return false; }
bool SgAsmX86Instruction::hasEffect() { return false; }
bool SgAsmX86Instruction::hasEffect(const std::vector<SgAsmInstruction*>&, bool, bool) { return false; }
bool SgAsmX86Instruction::isFunctionCallFast(const std::vector<SgAsmInstruction*>&, rose_addr_t*, rose_addr_t*) { return false; }
bool SgAsmX86Instruction::isFunctionCallSlow(const std::vector<SgAsmInstruction*>&, rose_addr_t*, rose_addr_t*) { return false; }
bool SgAsmX86Instruction::isFunctionReturnFast(const std::vector<SgAsmInstruction*>&) { return false; }
bool SgAsmX86Instruction::isFunctionReturnSlow(const std::vector<SgAsmInstruction*>&) { return false; }
bool SgAsmX86Instruction::getBranchTarget(rose_addr_t*) { return false; }

std::vector<std::pair<size_t,size_t> >
SgAsmX86Instruction::findNoopSubsequences(const std::vector<SgAsmInstruction*>&, bool, bool) {
    return std::vector<std::pair<size_t,size_t> >();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmType

void SgAsmType::check() const {}
size_t SgAsmType::get_nBytes() const { return 0; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmScalarType

void SgAsmScalarType::check() const {}
size_t SgAsmScalarType::get_nBits() const { return 0; }
std::string SgAsmScalarType::toString() const { return std::string(); }
size_t SgAsmScalarType::get_majorNBytes() const { return 0; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmIntegerType

void SgAsmIntegerType::check() const {}
std::string SgAsmIntegerType::toString() const { return std::string(); }
bool SgAsmIntegerType::get_isSigned() const { return false; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmFloatType

void SgAsmFloatType::check() const {}
std::string SgAsmFloatType::toString() const { return std::string(); }
size_t SgAsmFloatType::get_significandOffset() const { return 0; }
size_t SgAsmFloatType::get_significandNBits() const { return 0; }
size_t SgAsmFloatType::get_signBitOffset() const { return 0; }
size_t SgAsmFloatType::get_exponentOffset() const { return 0; }
size_t SgAsmFloatType::get_exponentNBits() const { return 0; }
uint64_t SgAsmFloatType::get_exponentBias() const { return 0; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmVectorType

void SgAsmVectorType::check() const {}
std::string SgAsmVectorType::toString() const { return std::string(); }
size_t SgAsmVectorType::get_nBits() const { return 0; }
size_t SgAsmVectorType::get_nElmts() const { return 0; }
SgAsmType* SgAsmVectorType::get_elmtType() const { return 0; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Disassembler
namespace rose {
namespace BinaryAnalysis{
void Disassembler::initDiagnostics() {}
} // namespace
} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Partitioner

namespace rose {
namespace BinaryAnalysis{
void Partitioner::initDiagnostics() {}
} // namespace
} // namespace
