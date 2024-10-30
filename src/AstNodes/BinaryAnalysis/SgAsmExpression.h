/** Base class for expressions. */
[[Rosebud::abstract]]
class SgAsmExpression: public SgAsmNode {
public:
    /** Property: Type of expression.
     *
     *  The type node is not considered a child of this node in the AST. */
    [[using Rosebud: rosetta]]
    SgAsmType* type = nullptr;

    /** Property: Comment.
     *
     *  User-defined comment for an expression. */
    [[using Rosebud: rosetta]]
    std::string comment;

public:
    /** Property: Width of expression in bits.
     *
     *  Returns the width of the expression in bits according to its data type. The "type" property must be non-null. */
    size_t get_nBits() const;

    /** Return a constant if possible.
     *
     *  If this expression is an integer expression with a constant that fits in a 64-bit unsigned type, then return it,
     *  otherwise return nothing. */
    Sawyer::Optional<uint64_t> asUnsigned() const;

    /** Return a signed constant if possible.
     *
     *  If this expression is an integer expression with a constant that fits in a 64-bit signed type, then return it,
     *  otherwise return nothing. */
    Sawyer::Optional<int64_t> asSigned() const;

    /** Converts an expression to a string.
     *
     *  The string is a single line with no leading or trailing white space. This expression must be a child of a @ref
     *  SgAsmInstruction because the instruction set architecture might have an influence on the way the expression is
     *  displayed. Attempting to call this method on a detached expression will return the string "detached_expression".
     *
     *  If this expression is not attached to an instruction, it can still be unparsed, just not with this member function. Instead,
     *  obtain an instruction unparser from the @ref Rose::BinaryAnalysis::Architecture::Base "architecture" and use that unparser
     *  to unparse this expression. */
    virtual std::string toString() const;
};
