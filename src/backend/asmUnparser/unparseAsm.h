#ifndef ROSE_UNPARSEASM_H
#define ROSE_UNPARSEASM_H

std::string unparseX86Register(
         X86RegisterClass cl,
         int num,
         X86PositionInRegister pos);

std::string unparseX86Expression(SgAsmExpression* expr, bool leaMode = false);
std::string unparseX86Instruction(SgAsmx86Instruction* insn);
std::string unparseX86InstructionWithAddress(SgAsmx86Instruction* insn);

enum ArmSignForExpressionUnparsing {
  arm_sign_none,
  arm_sign_plus,
  arm_sign_minus
};

std::string unparseArmRegister(
         SgAsmArmRegisterReferenceExpression::arm_register_enum code);
std::string unparseArmCondition(ArmInstructionCondition cond);
std::string unparseArmExpression(SgAsmExpression* expr, ArmSignForExpressionUnparsing sign = arm_sign_none);
std::string unparseArmInstruction(SgAsmArmInstruction* insn);
std::string unparseArmInstructionWithAddress(SgAsmArmInstruction* insn);

std::string unparseInstruction(SgAsmInstruction* insn);
std::string unparseInstructionWithAddress(SgAsmInstruction* insn);

// Fixed to take a SgAsmStatement and added support for comments.
std::string unparseAsmStatement(SgAsmStatement* stmt);

void unparseAsmStatementToFile(const std::string& filename, SgAsmStatement* stmt);
void unparseAsmFileToFile(const std::string& filename, SgAsmFile* file);

#endif /* ROSE_UNPARSEASM_H */
