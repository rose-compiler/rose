/** Base class for unary expressions. */
class SgAsmUnaryExpression: public SgAsmExpression {
    /** Property: Operand for a unary expression. */
    [[using Rosebud: rosetta, ctor_arg, traverse]]
    SgAsmExpression* operand = nullptr;
};
