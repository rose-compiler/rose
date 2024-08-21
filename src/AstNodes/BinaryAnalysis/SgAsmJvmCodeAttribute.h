/** JVM Code attribute.
 *
 *  A Code attribute contains the Java Virtual Machine instructions and auxiliary information for a method, including an instance
 *  initialization method and a class or interface initialization method, see section 4.7.3 of the JVM specification.
 */
class SgAsmJvmCodeAttribute: public SgAsmJvmAttribute {
public:
    /** Property: max_stack
     *
     *  The value of the max_stack item gives the maximum depth of the operand stack of this method at any point during execution of
     *  the method. */
    [[using Rosebud: rosetta]]
    uint16_t max_stack = 0;

    /** Property: max_locals
     *
     *  The value of the max_locals item gives the number of local variables in the local variable array allocated upon invocation
     *  of this method. */
    [[using Rosebud: rosetta]]
    uint16_t max_locals = 0;

    /** Property: code_length
     *
     *  The value of the code_length item gives the number of local variables in the local variable array allocated upon invocation
     *  of this method. */
    [[using Rosebud: rosetta]]
    uint32_t code_length = 0;

    // FIXME[Robb Matzke 2023-03-20]: is the lack of serialization a bug?
    /** Property: code
     *
     *  The code array gives the actual bytes of Java Virtual Machine code that implement the method. */
    [[using Rosebud: rosetta, serialize()]]
    const char* code = nullptr;

    /** Property: Exception table.
     *
     *  Points to the AST node that represents the exception table for this code attribute. */
    [[using Rosebud: rosetta]]
    SgAsmJvmExceptionTable* exception_table = nullptr;

    /** Property: Attribute table.
     *
     *  Points to the AST node that represents the attribute table for this code attribute. */
    [[using Rosebud: rosetta]]
    SgAsmJvmAttributeTable* attribute_table = nullptr;

    /** Property: Code offset.
     *
     *  Offset in bytes from the start of the file for the byte-code array of the method.  A convenience property @a not in the JVM
     *  specification. */
    [[using Rosebud: rosetta]]
    rose_addr_t code_offset = 0;

public:
    /** Initialize the object before parsing.
     *
     *  This is the preferred constructor to use before parsing.  It shall set its parent. */
    explicit SgAsmJvmCodeAttribute(SgAsmJvmAttributeTable* table);

    /** Initialize the attribute by parsing the file. */
    virtual SgAsmJvmAttribute* parse(SgAsmJvmConstantPool* pool) override;

    /** Write code attribute to a binary file. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
