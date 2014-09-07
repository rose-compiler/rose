// ROSETTA has no way to exclude certain IR node definitions based on the user's choice of languages.  Therefore, we need to
// provide (stub) definitions for those nodes' virtual methods in order to get things to compile when those nodes aren't
// actually being used anywhere.
#include "sage3basic.h"
#include "Diagnostics.h"
#include "Disassembler.h"

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
bool SgAsmInstruction::terminates_basic_block() { return false; }
bool SgAsmInstruction::is_unknown() const { return false; }
bool SgAsmInstruction::has_effect() { return false; }
bool SgAsmInstruction::has_effect(const std::vector<SgAsmInstruction*>&, bool, bool) { return false; }
bool SgAsmInstruction::is_function_call(const std::vector<SgAsmInstruction*>&, rose_addr_t*, rose_addr_t*) { return false; }
bool SgAsmInstruction::is_function_return(const std::vector<SgAsmInstruction*>&) { return false; }
bool SgAsmInstruction::get_branch_target(rose_addr_t*) { return false; }
std::set<rose_addr_t> SgAsmInstruction::get_successors(bool* complete) { return std::set<rose_addr_t>();}

std::set<rose_addr_t>
SgAsmInstruction::get_successors(const std::vector<SgAsmInstruction*>&, bool*, MemoryMap*) {
    return std::set<rose_addr_t>();
}

std::vector<std::pair<size_t,size_t> >
SgAsmInstruction::find_noop_subsequences(const std::vector<SgAsmInstruction*>&, bool, bool) {
    return std::vector<std::pair<size_t,size_t> >();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmArmInstruction

bool SgAsmArmInstruction::terminates_basic_block() { return false; }
bool SgAsmArmInstruction::is_unknown() const { return false; }
std::set<rose_addr_t> SgAsmArmInstruction::get_successors(bool* complete) { return std::set<rose_addr_t>(); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmM68kInstruction

bool SgAsmM68kInstruction::terminates_basic_block() { return false; }
bool SgAsmM68kInstruction::is_function_call(const std::vector<SgAsmInstruction*>&, rose_addr_t*, rose_addr_t*) { return false; }
bool SgAsmM68kInstruction::is_function_return(const std::vector<SgAsmInstruction*>&) { return false; }
bool SgAsmM68kInstruction::is_unknown() const { return false; }
std::set<rose_addr_t> SgAsmM68kInstruction::get_successors(bool*) { return std::set<rose_addr_t>(); }
std::set<rose_addr_t> SgAsmM68kInstruction::get_successors(const std::vector<SgAsmInstruction*>&, bool*, MemoryMap*) {
    return std::set<rose_addr_t>();
}
bool SgAsmM68kInstruction::get_branch_target(rose_addr_t*) { return false; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmMipsInstruction

bool SgAsmMipsInstruction::terminates_basic_block() { return false; }
bool SgAsmMipsInstruction::is_unknown() const { return false; }
bool SgAsmMipsInstruction::is_function_call(const std::vector<SgAsmInstruction*>&, rose_addr_t*, rose_addr_t*) { return false; }
bool SgAsmMipsInstruction::is_function_return(const std::vector<SgAsmInstruction*>&) { return false; }
bool SgAsmMipsInstruction::get_branch_target(rose_addr_t*) { return false; }
std::set<rose_addr_t> SgAsmMipsInstruction::get_successors(bool*) { return std::set<rose_addr_t>();}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmPowerpcInstruction

bool SgAsmPowerpcInstruction::terminates_basic_block() { return false; }
bool SgAsmPowerpcInstruction::is_unknown() const { return false; } 
std::set<rose_addr_t> SgAsmPowerpcInstruction::get_successors(bool*) { return std::set<rose_addr_t>();}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmx86Instruction

bool SgAsmx86Instruction::terminates_basic_block() { return false; }
bool SgAsmx86Instruction::is_unknown() const { return false; }
bool SgAsmx86Instruction::has_effect() { return false; }
bool SgAsmx86Instruction::has_effect(const std::vector<SgAsmInstruction*>&, bool, bool) { return false; }
bool SgAsmx86Instruction::is_function_call(const std::vector<SgAsmInstruction*>&, rose_addr_t*, rose_addr_t*) { return false; }
bool SgAsmx86Instruction::is_function_return(const std::vector<SgAsmInstruction*>&) { return false; }
bool SgAsmx86Instruction::get_branch_target(rose_addr_t*) { return false; }
std::set<rose_addr_t> SgAsmx86Instruction::get_successors(bool*) { return std::set<rose_addr_t>();}

std::set<rose_addr_t>
SgAsmx86Instruction::get_successors(const std::vector<SgAsmInstruction*>&, bool*, MemoryMap*) {
    return std::set<rose_addr_t>();
}

std::vector<std::pair<size_t,size_t> >
SgAsmx86Instruction::find_noop_subsequences(const std::vector<SgAsmInstruction*>&, bool, bool) {
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

void Disassembler::initDiagnostics() {}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Partitioner

void Partitioner::initDiagnostics() {}
