#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

using namespace Rose;                                   // temporary until this lives in "rose"
using namespace Rose::BinaryAnalysis;

unsigned
SgAsmNullInstruction::get_anyKind() const {
    return (unsigned)null_unknown;
}

#endif
