/** JVM SourceFile attribute.
 *
 *  A SourceFile attribute stores an index to the name of the class file, see section 4.7.10 of the JVM specification. */
class SgAsmJvmSourceFile: public SgAsmJvmAttribute {
    /** Property: sourcefile_index
     *
     *  The value of the sourcefile_index item must be a valid index into the constant_pool table (see JVM specification). */
    [[using Rosebud: rosetta]]
    uint16_t sourcefile_index = 0;

public:
    /** Initialize the attribute by parsing the file. */
    virtual SgAsmJvmAttribute* parse(SgAsmJvmConstantPool* pool) override;

    /** Write source file to a binary file. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
