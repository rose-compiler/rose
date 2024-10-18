// SgAsmM68kInstruction member definitions.
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include "AsmUnparser_compat.h"
#include <Rose/CommandLine.h>
#include <Rose/Diagnostics.h>
#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/DispatcherM68k.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/PartialSymbolicSemantics.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/SymbolicSemantics.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>

using namespace Rose;                                   // temporary until this lives in "rose"
using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;

unsigned
SgAsmM68kInstruction::get_anyKind() const {
    return p_kind;
}

#endif
