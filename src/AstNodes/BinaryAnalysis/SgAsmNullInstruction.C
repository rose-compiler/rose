#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <SgAsmNullInstruction.h>

unsigned
SgAsmNullInstruction::get_anyKind() const {
    return (unsigned)null_unknown;
}

#endif
