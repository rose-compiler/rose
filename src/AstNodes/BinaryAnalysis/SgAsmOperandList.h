/** List of operands for an instruction. */
class SgAsmOperandList: public SgAsmNode {
    /** Property: Ordered list of instruction operands.
     *
     *  The operand list is its own Sage node type (rather than operands being stored directly in the instruction node)
     *  because of limitations of ROSETTA. */
    [[using Rosebud: rosetta, traverse, large]]
    SgAsmExpressionPtrList operands;

public:
    /** Append another operand expression to this node. */
    void append_operand(SgAsmExpression* operand);
};
