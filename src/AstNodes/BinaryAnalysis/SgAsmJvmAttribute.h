/** Base class for a JVM attribute. */
class SgAsmJvmAttribute: public SgAsmJvmNode {
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

protected:
#if 0 // [Robb Matzke 2023-03-23]: not defined anywhere?
    /** Protected constructor for derived classes. */
    explicit SgAsmJvmAttribute(const SgAsmJvmConstantPool*);
#endif

public:
    /** Factory method returning a derived class instance. */
    static SgAsmJvmAttribute* create_attribute(SgAsmJvmConstantPool*);

    /** Initialize an attribute by parsing the file. */
    virtual SgAsmJvmAttribute* parse(SgAsmJvmConstantPool*);

    /** Print some debugging information */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
