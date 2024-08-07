/** JVM EnclosingMethod attribute.
 *
 *  The EnclosingMethod attribute is a fixed-length attribute in the attributes table of a ClassFile structure (4.1).
 *  A class must have an EnclosingMethod attribute if and only if it represents a local class or an anonymous class,
 *  see section 4.7.7 of the JVM specification. */
class SgAsmJvmEnclosingMethod: public SgAsmJvmAttribute {
public:
    /** Property: class_index
     *
     *  The value of the class_index item must be a valid index into the constant_pool table (see JVM specification). */
    [[using Rosebud: rosetta]]
    uint16_t class_index = 0;

    /** Property: method_index
     *
     *  The value of the method_index item either must be zero or a valid index into the constant_pool table
     *  (see JVM specification). */
    [[using Rosebud: rosetta]]
    uint16_t method_index = 0;

public:
    /** Initialize the object before parsing.
     *
     *  This is the preferred constructor to use before parsing.  It shall set its parent. */
    explicit SgAsmJvmEnclosingMethod(SgAsmJvmAttributeTable* table);

    /** Initialize the attribute by parsing the file. */
    virtual SgAsmJvmEnclosingMethod* parse(SgAsmJvmConstantPool* pool) override;

    /** Write enclosing method to a binary file. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
