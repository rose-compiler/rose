#ifdef ROSE_ENABLE_ASM_AARCH64

#include <Rose/BinaryAnalysis/InstructionEnumsAarch64.h>

/** Operand for an ARM AArch64 A64 AT instruction. */
class SgAsmAarch64AtOperand: public SgAsmExpression {
    /** Property: AT Operation.
     *
     *  An enum representing the operation to be performed. */
    [[using Rosebud: ctor_arg]]
    Rose::BinaryAnalysis::Aarch64AtOperation operation;
};

#endif // ROSE_ENABLE_ASM_AARCH64
