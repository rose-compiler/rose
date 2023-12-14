/** Base class for binary expressions. */
[[Rosebud::abstract]]
class SgAsmBinaryExpression: public SgAsmExpression {
public:
    /** Property: Left-hand side operand. */
    [[using Rosebud: rosetta, ctor_arg, traverse]]
    SgAsmExpression* lhs = nullptr;

    /** Property: Right-hand side operand. */
    [[using Rosebud: rosetta, ctor_arg, traverse]]
    SgAsmExpression* rhs = nullptr;
};
