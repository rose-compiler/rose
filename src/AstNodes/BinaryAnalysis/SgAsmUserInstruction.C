#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <SgAsmUserInstruction.h>

unsigned
SgAsmUserInstruction::get_anyKind() const {
    return p_kind;
}

#endif
