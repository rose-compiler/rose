#include <Rose/Tree/List.h>

/** Base class for expressions. */
[[Rosebud::abstract]]
class TestExpr: public Rose::Tree::Base {
    /** Property: Type of expression.
     *
     *  The type node is not considered to be a child of this node in the AST. */
    [[Rosebud::ctor_arg]]
    TestTypePtr type;

    /** Property: Optional single-line comment. */
    [[Rosebud::property]]
    std::string comment;
};

using TestExprList = Rose::Tree::List<TestExpr>;
