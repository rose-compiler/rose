// SgAsmMipsInstruction member definitions.
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

using namespace Rose;
using namespace Rose::BinaryAnalysis;

unsigned
SgAsmMipsInstruction::get_anyKind() const {
    return p_kind;
}

#endif
