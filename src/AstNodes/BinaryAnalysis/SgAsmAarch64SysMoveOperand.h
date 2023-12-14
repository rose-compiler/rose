#ifdef ROSE_ENABLE_ASM_AARCH64

/** Describes a system register for the ARM AArch64 A64 MRS and MSR instructions. */
class SgAsmAarch64SysMoveOperand: public SgAsmExpression {
public:
    /** Property: system register access bits.
     *
     *  The bits describing how to access a system register. These come directly from the encoded instruction. */
    [[using Rosebud: ctor_arg]]
    unsigned access;
};

#endif // ROSE_ENABLE_ASM_AARCH64
