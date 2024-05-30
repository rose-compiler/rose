#ifdef ROSE_ENABLE_ASM_AARCH64

#include <Rose/BinaryAnalysis/InstructionEnumsAarch64.h>

/** Operand for an ARM AArch64 A64 MSR instruction. */
class SgAsmAarch64PState: public SgAsmExpression {
public:
    /** Property: PState.
     *
     *  An enum representing the operand. */
    [[using Rosebud: ctor_arg]]
    Rose::BinaryAnalysis::Aarch64PState pstate;
};

#endif // ROSE_ENABLE_ASM_AARCH64
