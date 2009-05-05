#ifndef ROSE_UNPARSEASM_H
#define ROSE_UNPARSEASM_H

/*-------------------------------------------------------------------------------------------------------------------------------
 * X86
 *-------------------------------------------------------------------------------------------------------------------------------*/

std::string unparseX86Mnemonic(SgAsmx86Instruction*);
std::string unparseX86Expression(SgAsmExpression*);

/*-------------------------------------------------------------------------------------------------------------------------------
 * ARM
 *-------------------------------------------------------------------------------------------------------------------------------*/
enum ArmSignForExpressionUnparsing {
  arm_sign_none,
  arm_sign_plus,
  arm_sign_minus
};

std::string unparseArmMnemonic(SgAsmArmInstruction*);
std::string unparseArmExpression(SgAsmExpression*);
std::string unparseX86Register(X86RegisterClass, int reg, X86PositionInRegister);

/*-------------------------------------------------------------------------------------------------------------------------------
 * PowerPC
 *-------------------------------------------------------------------------------------------------------------------------------*/

std::string unparsePowerpcMnemonic(SgAsmPowerpcInstruction*);
std::string unparsePowerpcExpression(SgAsmExpression*);

/*-------------------------------------------------------------------------------------------------------------------------------
 * Generic unparsing declarations
 *-------------------------------------------------------------------------------------------------------------------------------*/
std::string unparseInstruction(SgAsmInstruction* insn);
std::string unparseInstructionWithAddress(SgAsmInstruction* insn);
std::string unparseMnemonic(SgAsmInstruction *insn);
std::string unparseExpression(SgAsmExpression *expr);

// Fixed to take a SgAsmStatement and added support for comments.
std::string unparseAsmStatement(SgAsmStatement* stmt);

// DQ (8/30/2008): Added prototype so this can be called from the unparser.
// As part of work to merge the executable file format dump with instructions.
std::string unparseAsmInterpretation(SgAsmInterpretation* interp);

void unparseAsmStatementToFile(const std::string& filename, SgAsmStatement* stmt);
void unparseAsmFileToFile(const std::string& filename, SgAsmFile* file);

#endif /* ROSE_UNPARSEASM_H */
