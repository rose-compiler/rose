/** JVM Exception.
 *
 *  Each exception is described by an anonymous structure defined in Code_attribute, section 4.7.3 of the JVM specification. */
class SgAsmJvmException: public SgAsmJvmNode {
    /** Property: start_pc
     *
     *  The value of the start_pc item must be a valid index into the constant_pool table (see JVM specification).  Present in an
     *  exception table of a Code_attribute (see section 4.7.3 of the JVM specification). */
    [[using Rosebud: rosetta]]
    uint16_t start_pc = 0;

    /** Property: end_pc
     *
     *  The value of the end_pc item must be a valid index into the constant_pool table (see JVM specification).  Present in an
     *  exception table of a Code_attribute (see section 4.7.3 of the JVM specification). */
    [[using Rosebud: rosetta]]
    uint16_t end_pc = 0;

    /** Property: handler_pc
     *
     *  The value of the handler_pc item must be a valid index into the constant_pool table (see JVM specification).  Present in an
     *  exception table of a Code_attribute (see section 4.7.3 of the JVM specification). */
    [[using Rosebud: rosetta]]
    uint16_t handler_pc = 0;

    /** Property: catch_type
     *
     *  The value of the catch_type item must be a valid index into the constant_pool table (see JVM specification).  Present in an
     *  exception table of a Code_attribute (see section 4.7.3 of the JVM specification). */
    [[using Rosebud: rosetta]]
    uint16_t catch_type = 0;

 public:
    /** Initialize the object before parsing.
     *
     *  This is the preferred constructor to use before parsing.  It shall set its parent. */
    explicit SgAsmJvmException(SgAsmJvmExceptionTable*);

    /** Initialize the object by parsing content from the class file. */
    SgAsmJvmException* parse(SgAsmJvmConstantPool*);

    /** Write exception to a binary file. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
