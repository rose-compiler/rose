/* Old functions for backward compatitility, reimplemented in terms of AsmUnparser. */
#ifndef ROSE_ASM_UNPARSER_COMPAT_H
#define ROSE_ASM_UNPARSER_COMPAT_H

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/BasicTypes.h>
#include "AsmUnparser.h"

// [Robb Matzke 2024-10-18]: Deprecated
ROSE_DLL_API std::string unparseInstruction(SgAsmInstruction*, const Rose::BinaryAnalysis::AsmUnparser::LabelMap *labels = nullptr)
    ROSE_DEPRECATED("use SgAsmInstruction::toStringNoAddr instead");
ROSE_DLL_API std::string unparseInstruction(SgAsmInstruction*, const Rose::BinaryAnalysis::AsmUnparser::LabelMap *labels,
                                            const Rose::BinaryAnalysis::RegisterDictionaryPtr &registers)
    ROSE_DEPRECATED("use SgAsmInstruction::toStringNoAddr instead");
ROSE_DLL_API std::string unparseInstructionWithAddress(SgAsmInstruction*,
                                                       const Rose::BinaryAnalysis::AsmUnparser::LabelMap *labels = nullptr)
    ROSE_DEPRECATED("use SgAsmInstruction::toString instead");
ROSE_DLL_API std::string unparseInstructionWithAddress(SgAsmInstruction*,
                                                       const Rose::BinaryAnalysis::AsmUnparser::LabelMap *labels,
                                                       const Rose::BinaryAnalysis::RegisterDictionaryPtr &registers)
    ROSE_DEPRECATED("use SgAsmInstruction::toString instead");

#endif
#endif
