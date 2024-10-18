#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <SgAsmM68kInstruction.h>

unsigned
SgAsmM68kInstruction::get_anyKind() const {
    return p_kind;
}

#endif
