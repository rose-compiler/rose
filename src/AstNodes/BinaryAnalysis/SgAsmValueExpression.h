/** Base class for values.
 *
 *  Assembly instruction (@ref SgAsmInstruction) operands are represented by @ref SgAsmExpression nodes in the AST. If the
 *  expression has a numeric value then an @ref SgAsmValueExpression is used.  Values of various types (integers and
 *  floating-point values of various sizes) are represented by subclasses of @ref SgAsmValueExpression.  Values that are
 *  addresses or references to data will have symbols in a function symbol table.  All other values are assumed to be
 *  literals and will not have associated symbols. */
[[Rosebud::abstract]]
class SgAsmValueExpression: public SgAsmExpression {
public:
    /** Property: Alternate expression without constant folding.
     *
     *  This is an optional expression that's equivalent to this expression but without constant folding.
     *
     *  Note: All of the ROSE disassemblers always set this to null regardless of whether they do any constant folding. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmValueExpression* unfoldedExpression = nullptr;

    /** Property: Where this expression is encoded within the instruction.
     *
     *  This is the bit offset into the instruction's raw bytes where this expression is encoded. If it is not supported by
     *  the architectures, it will be set to zero and the "bit_size" property will also be zero.  Bits are numbered so that
     *  bits zero through seven are in the first byte, bits eight through 15 are in the second byte, etc. Within a byte,
     *  bits are numbered so that lower indexes are less significant bits. */
    [[using Rosebud: rosetta]]
    unsigned short bitOffset = 0;

    /** Property: Where this expression is encoded within the instruction.
     *
     *  This is the size in bits of the encoding for this expression within the instruction.  If it is not supported by
     *  the architecture, it will be set to zero. */
    [[using Rosebud: rosetta]]
    unsigned short bitSize = 0;

    /** Property: Symbol corresponding to this expression.
     *
     *  If a symbol refers to this expression then it is linked by this property, otherwise null.  The symbol is not
     *  considered to be a child of this node in the AST. */
    [[using Rosebud: rosetta, serialize()]]
    SgSymbol* symbol = nullptr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    SgAsmValueExpression* get_unfolded_expression_tree() const ROSE_DEPRECATED("use get_unfoldedExpression");
    void set_unfolded_expression_tree(SgAsmValueExpression*) ROSE_DEPRECATED("use set_unfoldedExpression");
    unsigned short get_bit_offset() const ROSE_DEPRECATED("use get_bitOffset");
    void set_bit_offset(unsigned short) ROSE_DEPRECATED("use set_bitOffset");
    unsigned short get_bit_size() const ROSE_DEPRECATED("use get_bitSize");
    void set_bit_size(unsigned short) ROSE_DEPRECATED("use set_bitSize");
};
