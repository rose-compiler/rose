#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_AARCH32
#include <SgAsmAarch32Instruction.h>

unsigned
SgAsmAarch32Instruction::get_anyKind() const {
    return p_kind;
}

#endif
