/** JVM ModuleMainClass attribute.
 *
 *  A ModuleMainClass attribute stores a signature for a class, interface, constructor, method, field, or record component, see
 *  section 4.7.27 of the JVM specification. */
class SgAsmJvmModuleMainClass: public SgAsmJvmAttribute {
    /** Property: main_class_index
     *
     *  The value of the main_class_index item must be a valid index into the constant_pool table (see JVM specification). */
    [[using Rosebud: rosetta]]
    uint16_t main_class_index = 0;

public:
    /** Initialize the object before parsing.
     *
     *  This is the preferred constructor to use before parsing.  It shall set its parent. */
    explicit SgAsmJvmModuleMainClass(SgAsmJvmExceptionTable* table);

    /** Initialize the attribute by parsing the file. */
    virtual SgAsmJvmAttribute* parse(SgAsmJvmConstantPool* pool) override;

    /** Write module main class to a binary file. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
