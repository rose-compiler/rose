// SgAsmX86Instruction member definitions.
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include "AsmUnparser_compat.h"
#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/SymbolicSemantics.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/PartialSymbolicSemantics.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherX86.h>
#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/CommandLine.h>
#include <Rose/Diagnostics.h>
#include "x86InstructionProperties.h"

using namespace Rose;                                   // temporary until this lives in "rose"
using namespace Rose::Diagnostics;
using namespace Rose::BinaryAnalysis;

unsigned
SgAsmX86Instruction::get_anyKind() const {
    return p_kind;
}

// class method
X86InstructionSize
SgAsmX86Instruction::instructionSizeForWidth(size_t nbits) {
    switch (nbits) {
        case 16: return x86_insnsize_16;
        case 32: return x86_insnsize_32;
        case 64: return x86_insnsize_64;
    }
    ASSERT_not_reachable("invalid width: " + StringUtility::numberToString(nbits));
}

// class method
size_t
SgAsmX86Instruction::widthForInstructionSize(X86InstructionSize isize) {
    switch (isize) {
        case x86_insnsize_16: return 16;
        case x86_insnsize_32: return 32;
        case x86_insnsize_64: return 64;
        default: ASSERT_not_reachable("invalid x86 instruction size");
    }
}

// class method
RegisterDictionary::Ptr
SgAsmX86Instruction::registersForInstructionSize(X86InstructionSize isize) {
    switch (isize) {
        case x86_insnsize_16: return Architecture::findByName("intel-80286").orThrow()->registerDictionary();
        case x86_insnsize_32: return Architecture::findByName("intel-pentium4").orThrow()->registerDictionary();
        case x86_insnsize_64: return Architecture::findByName("amd64").orThrow()->registerDictionary();
        default: ASSERT_not_reachable("invalid x86 instruction size");
    }
}

// class method
RegisterDictionary::Ptr
SgAsmX86Instruction::registersForWidth(size_t nbits) {
    return registersForInstructionSize(instructionSizeForWidth(nbits));
}

#endif
