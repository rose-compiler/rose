/* Old functions for backward compatitility, reimplemented in terms of AsmUnparser. */
#ifndef ROSE_ASM_UNPARSER_COMPAT_H
#define ROSE_ASM_UNPARSER_COMPAT_H

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/BasicTypes.h>
#include "AsmUnparser.h"

/** Returns the string representation of an assembly instruction, sans address. */
ROSE_DLL_API std::string unparseInstruction(SgAsmInstruction*, const Rose::BinaryAnalysis::AsmUnparser::LabelMap *labels =nullptr);
ROSE_DLL_API std::string unparseInstruction(SgAsmInstruction*, const Rose::BinaryAnalysis::AsmUnparser::LabelMap *labels,
                                            const Rose::BinaryAnalysis::RegisterDictionaryPtr &registers);

/** Returns the string representation of an assembly instruction with address. */
ROSE_DLL_API std::string unparseInstructionWithAddress(SgAsmInstruction*,
                                                       const Rose::BinaryAnalysis::AsmUnparser::LabelMap *labels = nullptr);
ROSE_DLL_API std::string unparseInstructionWithAddress(SgAsmInstruction*,
                                                       const Rose::BinaryAnalysis::AsmUnparser::LabelMap *labels,
                                                       const Rose::BinaryAnalysis::RegisterDictionaryPtr &registers);

/** Returns a string representing the part of the assembly instruction before the first operand. */
ROSE_DLL_API std::string unparseMnemonic(SgAsmInstruction*);

/** Returns the string representation of an instruction operand (SgAsmExpression), the format of which might depend on the
 *  instruction with which it's associated. */
ROSE_DLL_API std::string unparseExpression(SgAsmExpression*, const Rose::BinaryAnalysis::AsmUnparser::LabelMap *labels,
                                           const Rose::BinaryAnalysis::RegisterDictionaryPtr &registers);

ROSE_DLL_API std::string unparseAsmStatement(SgAsmStatement*);

ROSE_DLL_API void unparseAsmStatementToFile(const std::string& filename, SgAsmStatement*);

ROSE_DLL_API std::string unparseAsmInterpretation(SgAsmInterpretation*);

#endif
#endif
