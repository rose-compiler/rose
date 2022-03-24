#ifndef _CLANG_TO_ROSE_SUPPORT_HPP_
# define _CLANG_TO_ROSE_SUPPORT_HPP_

SgAsmOp::asm_operand_modifier_enum get_sgAsmOperandModifier(std::string modifier);
SgAsmOp::asm_operand_constraint_enum get_sgAsmOperandConstraint(std::string constraint);
SgInitializedName::asm_register_name_enum get_sgAsmRegister(std::string reg);

#endif /* _CLANG_TO_ROSE_SUPPORT_HPP_ */
