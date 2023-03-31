/** List of expression nodes. */
class SgAsmExprListExp: public SgAsmExpression {
    /** Property: Pointers to expressions.
     *
     *  List of expression nodes are stored in their own @ref SgAsmExprListExp node instead of in parent AST nodes due to
     *  limitations of ROSETTA. */
    [[using Rosebud: rosetta, traverse, large]]
    SgAsmExpressionPtrList expressions;
};
