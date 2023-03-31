/** Base class for references to a machine register. */
class SgAsmRegisterReferenceExpression: public SgAsmExpression {
    /** Property: Descriptor for accessed register. */
    [[using Rosebud: rosetta, ctor_arg]]
    Rose::BinaryAnalysis::RegisterDescriptor descriptor;

    /** Property: Post-increment or pre-decrement amount.
     *
     *  This is a value that's added or subtracted from a register each time the containing instruction is executed. */
    [[using Rosebud: rosetta]]
    int adjustment = 0;
};
