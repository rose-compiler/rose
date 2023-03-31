/** Base class for binary expressions. */
class SgAsmBinaryExpression: public SgAsmExpression {
    /** Property: Left-hand side operand. */
    [[using Rosebud: rosetta, ctor_arg, traverse]]
    SgAsmExpression* lhs = nullptr;

    /** Property: Right-hand side operand. */
    [[using Rosebud: rosetta, ctor_arg, traverse]]
    SgAsmExpression* rhs = nullptr;
};
