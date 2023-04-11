/** Base class for expressions with two operands. */
[[Rosebud::abstract]]
class TestBinExpr: public TestExpr {
    /** First operand. */
    [[Rosebud::ctor_arg]]
    Rose::Tree::Edge<TestExpr> lhs;

    /** Second operand. */
    [[Rosebud::ctor_arg]]
    Rose::Tree::Edge<TestExpr> rhs;
};
