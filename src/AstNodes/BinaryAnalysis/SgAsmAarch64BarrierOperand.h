#ifdef ROSE_ENABLE_ASM_AARCH64

#include <Rose/BinaryAnalysis/InstructionEnumsAarch64.h>

/** Barriar operation operand for ISB, DMB, and DSB instructions. */
class SgAsmAarch64BarrierOperand: public SgAsmExpression {
    /** Property: Barrier operation. */
    [[using Rosebud: ctor_arg]]
    Rose::BinaryAnalysis::Aarch64BarrierOperation operation;
};

#endif // ROSE_ENABLE_ASM_AARCH64
