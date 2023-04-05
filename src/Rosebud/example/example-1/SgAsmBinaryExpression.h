/** Base class for expressions with two operands. */
[[Rosebud::abstract]]
class SgAsmBinaryExpression: public SgAsmExpression {
    /** First operand. */
    [[Rosebud::ctor_arg]]
    TreeEdge<SgAsmExpression> lhs;

    /** Second operand. */
    [[Rosebud::ctor_arg]]
    TreeEdge<SgAsmExpression> rhs;
};
