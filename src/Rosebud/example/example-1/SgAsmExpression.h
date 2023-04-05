/** Base class for expressions. */
[[Rosebud::abstract]]
class SgAsmExpression: public TreeNode {
    /** Property: Type of expression.
     *
     *  The type node is not considered to be a child of this node in the AST. */
    [[Rosebud::ctor_arg]]
    SgAsmTypePtr type;

    /** Property: Optional single-line comment. */
    [[Rosebud::property]]
    std::string comment;
};

using SgAsmExpressionList = TreeListNode<SgAsmExpression>;
