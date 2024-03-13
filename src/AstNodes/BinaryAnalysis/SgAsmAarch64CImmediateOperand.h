#ifdef ROSE_ENABLE_ASM_AARCH64

/** C-Immediate operand for SYS, AT, CFP, CPP, DC, DVP, IC, and TLBI instructions. */
class SgAsmAarch64CImmediateOperand: public SgAsmExpression {
public:
    /** Property: C-immediate value.
     *
     *  The C-immediate value for the instruction. */
    [[using Rosebud: ctor_arg]]
    unsigned immediate;
};

#endif // ROSE_ENABLE_ASM_AARCH64
