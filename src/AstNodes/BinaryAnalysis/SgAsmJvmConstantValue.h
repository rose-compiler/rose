/** JVM ConstantValue attribute.
 *
 *  A ConstantValue attribute represents the value of a constant expression, see section 4.7.2 of the JVM specification.
 */
class SgAsmJvmConstantValue: public SgAsmJvmAttribute {
public:
    /** Property: constantvalue_index
     *
     *  The value of the constantvalue_index item must be a valid index into the constant_pool table (see JVM specification). */
    [[using Rosebud: rosetta]]
    uint16_t constantvalue_index = 0;

public:
    /** Initialize the object before parsing.
     *
     *  This is the preferred constructor to use before parsing.  It shall set its parent. */
    explicit SgAsmJvmConstantValue(SgAsmJvmAttributeTable* table);

    /** Initialize the attribute by parsing the file. */
    virtual SgAsmJvmAttribute* parse(SgAsmJvmConstantPool* pool) override;

    /** Write constant value to a binary file. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
