#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_AARCH64
#include <SgAsmAarch64Instruction.h>

unsigned
SgAsmAarch64Instruction::get_anyKind() const {
    return p_kind;
}

#endif
