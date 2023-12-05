// SgAsmAarch64Instruction member definitions.
#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_AARCH64
#include <sage3basic.h>

#include <Rose/BinaryAnalysis/Disassembler/Base.h>

using namespace Rose;                                   // temporary until this lives in "rose"
using namespace Rose::BinaryAnalysis;                   // temporary

unsigned
SgAsmAarch64Instruction::get_anyKind() const {
    return p_kind;
}

#endif
