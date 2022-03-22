#ifndef _CLANG_TO_ROSE_SUPPORT_HPP_
# define _CLANG_TO_ROSE_SUPPORT_HPP_
#include "sage3basic.h"

SgAsmOp::asm_operand_modifier_enum get_sgAsmOperandModifier(std::string modifier);
SgAsmOp::asm_operand_constraint_enum get_sgAsmOperandConstraint(std::string constraint);

#endif /* _CLANG_TO_ROSE_SUPPORT_HPP_ */
