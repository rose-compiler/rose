#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <SgAsmMipsInstruction.h>

unsigned
SgAsmMipsInstruction::get_anyKind() const {
    return p_kind;
}

#endif
