/* SgAsmJvmInstruction member definitions */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"


using namespace Rose;                                   // temporary until this lives in "rose"
using namespace Rose::BinaryAnalysis;
using JvmOp = JvmInstructionKind;

unsigned
SgAsmJvmInstruction::get_anyKind() const {
  return static_cast<unsigned>(p_kind);
}

#endif
