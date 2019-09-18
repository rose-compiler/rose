/* Old functions for backward compatitility, reimplemented in terms of AsmUnparser. */
#ifndef ROSE_ASM_UNPARSER_COMPAT_H
#define ROSE_ASM_UNPARSER_COMPAT_H

#include "AsmUnparser.h"

/** Returns the string representation of an assembly instruction, sans address. */
ROSE_DLL_API std::string unparseInstruction(SgAsmInstruction*, const Rose::BinaryAnalysis::AsmUnparser::LabelMap *labels=NULL,
                                            const Rose::BinaryAnalysis::RegisterDictionary *registers=NULL);

/** Returns the string representation of an assembly instruction with address. */
ROSE_DLL_API std::string unparseInstructionWithAddress(SgAsmInstruction*,
                                                       const Rose::BinaryAnalysis::AsmUnparser::LabelMap *labels=NULL,
                                                       const Rose::BinaryAnalysis::RegisterDictionary *registers=NULL);

/** Returns a string representing the part of the assembly instruction before the first operand. */
ROSE_DLL_API std::string unparseMnemonic(SgAsmInstruction*);

/** Returns the string representation of an instruction operand (SgAsmExpression), the format of which might depend on the
 *  instruction with which it's associated. */
ROSE_DLL_API std::string unparseExpression(SgAsmExpression*, const Rose::BinaryAnalysis::AsmUnparser::LabelMap *labels,
                                           const Rose::BinaryAnalysis::RegisterDictionary *registers);

ROSE_DLL_API std::string unparseAsmStatement(SgAsmStatement*);

ROSE_DLL_API void unparseAsmStatementToFile(const std::string& filename, SgAsmStatement*);

ROSE_DLL_API std::string unparseAsmInterpretation(SgAsmInterpretation*);

/*-------------------------------------------------------------------------------------------------------------------------------
 * X86
 *-------------------------------------------------------------------------------------------------------------------------------*/

std::string unparseX86Mnemonic(SgAsmX86Instruction*);
std::string unparseX86Expression(SgAsmExpression*, const Rose::BinaryAnalysis::AsmUnparser::LabelMap*,
                                 const Rose::BinaryAnalysis::RegisterDictionary*);
std::string unparseX86Expression(SgAsmExpression*, const Rose::BinaryAnalysis::AsmUnparser::LabelMap*,
                                 const Rose::BinaryAnalysis::RegisterDictionary*, bool leaMode);
std::string unparseX86Register(Rose::BinaryAnalysis::RegisterDescriptor, const Rose::BinaryAnalysis::RegisterDictionary*);
std::string unparseX86Register(SgAsmInstruction*, Rose::BinaryAnalysis::RegisterDescriptor,
                               const Rose::BinaryAnalysis::RegisterDictionary*);

/*-------------------------------------------------------------------------------------------------------------------------------
 * ARM
 *-------------------------------------------------------------------------------------------------------------------------------*/
enum ArmSignForExpressionUnparsing 
{
  arm_sign_none,
  arm_sign_plus,
  arm_sign_minus
} ;

std::string unparseArmMnemonic(SgAsmArmInstruction*);
std::string unparseArmExpression(SgAsmExpression*, const Rose::BinaryAnalysis::AsmUnparser::LabelMap*,
                                 const Rose::BinaryAnalysis::RegisterDictionary*);

/*-------------------------------------------------------------------------------------------------------------------------------
 * PowerPC
 *-------------------------------------------------------------------------------------------------------------------------------*/

std::string unparsePowerpcMnemonic(SgAsmPowerpcInstruction*);
std::string unparsePowerpcExpression(SgAsmExpression*, const Rose::BinaryAnalysis::AsmUnparser::LabelMap*,
                                     const Rose::BinaryAnalysis::RegisterDictionary*);

/*-------------------------------------------------------------------------------------------------------------------------------
 * MIPS
 *-------------------------------------------------------------------------------------------------------------------------------*/

std::string unparseMipsMnemonic(SgAsmMipsInstruction*);
std::string unparseMipsExpression(SgAsmExpression*, const Rose::BinaryAnalysis::AsmUnparser::LabelMap*,
                                  const Rose::BinaryAnalysis::RegisterDictionary*);

/*-------------------------------------------------------------------------------------------------------------------------------
 * M68k
 *-------------------------------------------------------------------------------------------------------------------------------*/

std::string unparseM68kMnemonic(SgAsmM68kInstruction*);
std::string unparseM68kExpression(SgAsmExpression*, const Rose::BinaryAnalysis::AsmUnparser::LabelMap*,
                                  const Rose::BinaryAnalysis::RegisterDictionary*);


#endif
