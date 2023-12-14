/** Base class for unary expressions. */
[[Rosebud::abstract]]
class SgAsmUnaryExpression: public SgAsmExpression {
public:
    /** Property: Operand for a unary expression. */
    [[using Rosebud: rosetta, ctor_arg, traverse]]
    SgAsmExpression* operand = nullptr;
};
