/** JVM MethodParametersEntry.
 *
 *  Each method parameter is described by a parameters entry anonymous structure.
 *  See the JVM specification, section 4.7.24. */
class SgAsmJvmMethodParametersEntry: public SgAsmJvmNode {
public:
    /** Property: name_index
     *
     *  The value of the name_index item must either be zero or a valid index into the constant_pool table
     *  (see JVM specification). */
    [[using Rosebud: rosetta]]
    uint16_t name_index = 0;

    /** Property: access_flags
     *
     *  The value of the access_flags item represents information about the formal parameter (see JVM specification). */
    [[using Rosebud: rosetta]]
    uint16_t access_flags = 0;

 public:
    /** Initialize the object before parsing.
     *
     *  This is the preferred constructor to use before parsing.  It shall set its parent. */
    explicit SgAsmJvmMethodParametersEntry(SgAsmJvmMethodParameters*);

    /** Initialize the object by parsing content from the class file. */
    SgAsmJvmMethodParametersEntry* parse(SgAsmJvmConstantPool*);

    /** Write line number entry to a binary file. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
