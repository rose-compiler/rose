#include <sageContainer.h>

/** List of operands for an instruction. */
class SgAsmOperandList: public SgAsmNode {
public:
    /** Property: Ordered list of instruction operands.
     *
     *  The operand list is its own Sage node type (rather than operands being stored directly in the instruction node)
     *  because of limitations of ROSETTA. */
    [[using Rosebud: rosetta, traverse, large]]
    SgAsmExpressionPtrList operands;

public:
    /** Append another operand expression to this node. */
    void appendOperand(SgAsmExpression* operand);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    void append_operand(SgAsmExpression*) ROSE_DEPRECATED("use appendOperand");

};
