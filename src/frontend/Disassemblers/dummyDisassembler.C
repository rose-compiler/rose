// ROSETTA has no way to exclude certain IR node definitions based on the user's choice of languages.  Therefore, we need to
// provide (stub) definitions for those nodes' virtual methods in order to get things to compile when those nodes aren't
// actually being used anywhere.
#include "sage3basic.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmIntegerValueExpression

uint64_t SgAsmIntegerValueExpression::virtual_address(SgNode*) { abort(); }
std::string SgAsmIntegerValueExpression::get_label(bool) const { abort(); }
void SgAsmIntegerValueExpression::set_significant_bits(size_t) { abort(); }
size_t SgAsmIntegerValueExpression::get_significant_bits() const { abort(); }
void SgAsmIntegerValueExpression::make_relative_to(SgNode*) { abort(); }
uint64_t SgAsmIntegerValueExpression::get_base_address() const { abort(); }
uint64_t SgAsmIntegerValueExpression::get_absolute_value(size_t) const { abort(); }
int64_t SgAsmIntegerValueExpression::get_absolute_signed_value() const { abort(); }
void SgAsmIntegerValueExpression::set_absolute_value(uint64_t) { abort(); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmByteValueExpression

uint8_t SgAsmByteValueExpression::get_value() const { abort(); }
void SgAsmByteValueExpression::set_value(uint8_t) { abort(); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmWordValueExpression

uint16_t SgAsmWordValueExpression::get_value() const { abort(); }
void SgAsmWordValueExpression::set_value(uint16_t) { abort(); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmDoubleWordValueExpression

uint32_t SgAsmDoubleWordValueExpression::get_value() const { abort(); }
void SgAsmDoubleWordValueExpression::set_value(uint32_t) { abort(); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmQuadWordValueExpression

uint64_t SgAsmQuadWordValueExpression::get_value() const { abort(); }
void SgAsmQuadWordValueExpression::set_value(uint64_t) { abort(); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmInstruction

size_t SgAsmInstruction::get_size() const { return 0; }
bool SgAsmInstruction::terminates_basic_block() { return false; }
bool SgAsmInstruction::is_unknown() const { return false; }
bool SgAsmInstruction::has_effect() { return false; }
bool SgAsmInstruction::has_effect(const std::vector<SgAsmInstruction*>&, bool, bool) { return false; }
bool SgAsmInstruction::is_function_call(const std::vector<SgAsmInstruction*>&, rose_addr_t*) { return false; }
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
// SgAsmMipsInstruction

bool SgAsmMipsInstruction::terminates_basic_block() { return false; }
bool SgAsmMipsInstruction::is_unknown() const { return false; }
bool SgAsmMipsInstruction::is_function_call(const std::vector<SgAsmInstruction*>&, rose_addr_t*) { return false; }
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
bool SgAsmx86Instruction::is_function_call(const std::vector<SgAsmInstruction*>&, rose_addr_t*) { return false; }
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




