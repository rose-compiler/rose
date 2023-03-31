/**  Base class for integer values.
 *
 *  An integer value consists of an offset from an optional base node.  The base node must be some kind of object with a
 *  virtual address, such as an instruction, symbol, segment, etc.  If no base node is associated with the
 *  SgAsmIntegerValueExpression (the default situation), then a zero base address is used.
 *
 *  When a (new) base object is associated with an SgAsmIntegerValueExpression via the makeRelativeTo() method, the value
 *  of the expression does not change.  However, the value does change when the address of the associated base node
 *  changes.  For instance, one way to represent a function call to "main" is to have a CALL instruction whose operand is
 *  an SgAsmIntegerValueExpression that has a base which is either the entry instruction of "main" or the symbol for
 *  "main".  That way, if the address of "main" changes then the target address in the CALL instruction also changes.
 *
 *  The base class stores the bits that are interpretted as the signed offset. The offset is accessed with
 *  get_relativeValue() and set_relativeValue() methods. The class also defines get_absoluteValue() and
 *  set_aabsoluteValue() methods that operate on the absolute value (which isn't actually stored anywhere). */
class SgAsmIntegerValueExpression: public SgAsmConstantExpression {
    /** Property: Base node associated with an integer.
     *
     * When setting this property, the base node is changed without updating this object's relative value, thus this
     * object's absolute value changes.  The value returned by get_absoluteValue() will probably differ from what it would
     * have returned before calling set_baseNode().  If this is not the behavior that's needed, see the makeRelativeTo()
     * method.
     *
     * The base node is not considered to be a child of this node in the AST. */
    [[using Rosebud: rosetta]]
    SgNode* baseNode = nullptr;

public:
    /** Construct a new value of specified type.
     *
     *  Creates a new AST node having value @p n of type @p type. See also, the constructor that takes a bit vector as the
     *  first argument, which is useful when the value is non-integral or is too large to fit in a @c uint64_t. */
    SgAsmIntegerValueExpression(uint64_t n, SgAsmType *type);

    /** Construct a new value of specified type.
     *
     *  Creates a new AST node having value @p bv of type @p type.  See also, the constructor that takes a @c uint64_t as
     *  the first argument which might be simpler to call in situations where the value is an integral type not more than
     *  64 bits. */
    SgAsmIntegerValueExpression(const Sawyer::Container::BitVector &bv, SgAsmType *type);

    /** Returns the base address of an addressable IR node. */
    static uint64_t virtualAddress(SgNode*);

    /** Returns a label for the value.
     *
     *  The label consists of the base object name (if available) or address, followed by a plus sign or minus sign,
     *  followed by the offset from that object.  The empty string is returned if this integer value expression has no base
     *  object (i.e., it's absolute).
     *
     *  If the base object has no name and the integer value points directly at the object (offset=0) then one of two
     *  things happen: if @p quiet is true, the empty string is returned, otherwise the label is the name of the node type
     *  enclosed in an extra set of angle brackets.  This is useful to indicate that a value is relative rather than
     *  absolute.  For instance, the instruction listing "call 0x004126bb" is ambiguous as to whether 0x004126bb points to
     *  a known, unnamed function, a non-entry instruction within a function, or some memory location we didn't
     *  disassemble.  But when labeled with @p quiet being false, the output will be:
     *
     *  <ul>
     *    <li>call 0x004126bb<main>; points to a function with a name</li>
     *    <li>call 0x004126bb<<Func>>; points to a function without a name</li>
     *    <li>call 0x004126bb<<Insn>>; points to an instruction that's not a function entry point</li>
     *    <li>call 0x004126bb; points to something that's not been disassembled</li>
     *  </ul> */
    std::string get_label(bool quiet=false) const;

    /** Return the number of significant bits in the value. */
    size_t get_significantBits() const;

    /** Makes the value of this integer relative to some other addressable node.
     *
     *  The absolute value of this expression is unchanged by this operation. The @p baseNode must be a type of IR node
     *  that has a virtual address, such as another instruction.  If @p baseNode is the null pointer, then the
     *  "relativeness" of this constant is removed (i.e., it will be relative to zero). */
    void makeRelativeTo(SgNode *baseNode);

    /** Returns the base address.
     *
     *  The base address is the virtual address of the associated IR node, or zero if no IR node is associated with this
     *  integer value. */
    uint64_t get_baseAddress() const;

    /** Returns the current absolute value zero filled to 64 bits.
     *
     *  The absolute value is the 64-bit sum of the 64-bit address of the base node (or zero if no base node is associated
     *  with this object) and the 64-bit offset. However, this function returns only the specified number of low-order bits
     *  zero extended to the 64-bit return type.  If @p nbits is zero, then get_significantBits() is called. */
    uint64_t get_absoluteValue(size_t nbits=0) const;

    /** Set absolute value.
     *
     *  Changes the absolute value of this integer expression without changing the base node. */
    void set_absoluteValue(uint64_t);

    /** Returns the current absolute value (base+offset) as a signed value. */
    int64_t get_signedValue() const;

    /** Get relative value.
     *
     *  Interprets the bit vector as a signed value, sign extends it to 64-bits if necessary, and returns it. */
    int64_t get_relativeValue() const;

    /** Set relative value without changing the base value.
     *
     *  The relative value is interpretted as a signed value of the specified
     *  width (defaulting to 64-bits). */
    void set_relativeValue(int64_t v, size_t nbits=64);

    uint64_t get_value() const { return get_absoluteValue(); }
};
