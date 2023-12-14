/** JVM NestHost attribute.
 *
 *  The NestHost attribute records the nest host of the nest to which the current class or
 *  interface claims to belong, see section 4.7.28 of the JVM specification. */
class SgAsmJvmNestHost: public SgAsmJvmAttribute {
public:
    /** Property: classes
     *
     *  The value in the host_class_index item must be a valid index into the constant_pool table.
     *  The constant_pool entry at that index must be a CONSTANT_Class_info structure
     *  representing a class or interface which is the nest host for the current class or
     *  interface (see section 4.4.1 of the JVM specification). */
    [[using Rosebud: rosetta]]
    uint16_t host_class_index = 0;

public:
    /** Initialize the attribute before parsing.
     *
     *  This is the preferred constructor to use before parsing as it sets its parent. */
    explicit SgAsmJvmNestHost(SgAsmJvmAttributeTable* table);

    /** Initialize the attribute by parsing the file. */
    virtual SgAsmJvmNestHost* parse(SgAsmJvmConstantPool* pool) override;

    /** Write object to a binary file. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
