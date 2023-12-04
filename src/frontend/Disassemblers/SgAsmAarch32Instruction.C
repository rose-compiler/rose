// SgAsmAarch32Instructoin member definitions.
#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_AARCH32
#include <sage3basic.h>

#include <Rose/BinaryAnalysis/Disassembler/Base.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;

unsigned
SgAsmAarch32Instruction::get_anyKind() const {
    return p_kind;
}

bool
SgAsmAarch32Instruction::isUnknown() const {
    return Aarch32InstructionKind::ARM_INS_INVALID == get_kind();
}

#endif
