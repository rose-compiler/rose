/* These are backward compatibility functions now implemented in terms of AsmUnparser */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"
#include "AsmUnparser_compat.h"

#include <Rose/Affirm.h>
#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/ControlFlow.h>
#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/BinaryAnalysis/Unparser/Aarch32.h>
#include <Rose/BinaryAnalysis/Unparser/Aarch64.h>
#include <Rose/BinaryAnalysis/Unparser/Cil.h>
#include <Rose/BinaryAnalysis/Unparser/Jvm.h>
#include <Rose/BinaryAnalysis/Unparser/M68k.h>
#include <Rose/BinaryAnalysis/Unparser/Mips.h>
#include <Rose/BinaryAnalysis/Unparser/Powerpc.h>
#include <Rose/BinaryAnalysis/Unparser/X86.h>
#include <Rose/Diagnostics.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Rose::BinaryAnalysis::Unparser;

// [Robb Matzke 2024-10-18]: deprecated
std::string unparseInstruction(SgAsmInstruction *insn, const Rose::BinaryAnalysis::AsmUnparser::LabelMap*) {
    return notnull(insn)->toStringNoAddr();
}

// [Robb Matzke 2024-10-18]: deprecated
std::string unparseInstruction(SgAsmInstruction* insn, const AsmUnparser::LabelMap*, const RegisterDictionary::Ptr&) {
    return notnull(insn)->toStringNoAddr();
}

// [Robb Matzke 2024-10-18]: deprecated
std::string unparseInstructionWithAddress(SgAsmInstruction *insn, const Rose::BinaryAnalysis::AsmUnparser::LabelMap*) {
    return notnull(insn)->toString();
}

// [Robb Matzke 2024-10-18]: deprecated
std::string unparseInstructionWithAddress(SgAsmInstruction* insn, const AsmUnparser::LabelMap*, const RegisterDictionary::Ptr&) {
    if (insn) {
        return insn->toString();
    } else {
        return "BOGUS:NULL";
    }
}

// [Robb Matzke 2024-10-18]: deprecated
std::string unparseExpression(SgAsmExpression *expr, const Rose::BinaryAnalysis::AsmUnparser::LabelMap*,
                              const Rose::BinaryAnalysis::RegisterDictionaryPtr&) {
    return notnull(expr)->toString();
}

#endif
