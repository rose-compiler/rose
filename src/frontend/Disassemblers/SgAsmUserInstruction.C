#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

unsigned
SgAsmUserInstruction::get_anyKind() const {
    return p_kind;
}

#endif
