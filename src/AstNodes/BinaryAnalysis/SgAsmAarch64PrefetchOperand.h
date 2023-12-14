#ifdef ROSE_ENABLE_ASM_AARCH64

#include <Rose/BinaryAnalysis/InstructionEnumsAarch64.h>

/** Operand for an ARM AArch64 A64 prefetch instruction. */
class SgAsmAarch64PrefetchOperand: public SgAsmExpression {
public:
    /** Property: Prefetch operation.
     *
     *  An enum representing the operation to be performed. */
    [[using Rosebud: ctor_arg]]
    Rose::BinaryAnalysis::Aarch64PrefetchOperation operation;
};

#endif // ROSE_ENABLE_ASM_AARCH64
