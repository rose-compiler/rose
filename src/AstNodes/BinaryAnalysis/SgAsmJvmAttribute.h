/** Base class for a JVM attribute. */
[[Rosebud::abstract]]
class SgAsmJvmAttribute: public SgAsmJvmNode {
public:
    /** Property: attribute_name_index
     *
     *  The value of the attribute_name_index item must be a valid index into the constant_pool table (see JVM specification). */
    [[using Rosebud: rosetta]]
    uint16_t attribute_name_index = 0;

    /** Property: attribute_length
     *
     *  The value of the attribute_length item indicates the length of the attribute, not including the attribute_name_index nor
     *  attribute_length. */
    [[using Rosebud: rosetta]]
    uint32_t attribute_length = 0;

public:
    /** Factory method returning a derived class instance. */
    static SgAsmJvmAttribute* instance(SgAsmJvmConstantPool* pool, SgAsmJvmAttributeTable* parent);

    /** Initialize an attribute by parsing the file. */
    virtual SgAsmJvmAttribute* parse(SgAsmJvmConstantPool* pool);

    /** Write attribute to a binary file. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
