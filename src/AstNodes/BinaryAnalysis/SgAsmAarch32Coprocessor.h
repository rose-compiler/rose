/** Operand referencing a Co-processor. */
class SgAsmAarch32Coprocessor: public SgAsmExpression {
public:
    /** Property: Coprocessor number. */
    [[using Rosebud: ctor_arg]]
    int coprocessor;
};
