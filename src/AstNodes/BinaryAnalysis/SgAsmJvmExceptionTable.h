/** Represents a JVM exception_info table/array.
 *
 *  The JVM exceptions table is an SgAsmJvmNode containing SgAsmJvmException entries. */
class SgAsmJvmExceptionTable: public SgAsmJvmNode {
    /** Property: List of pointers to exceptions. */
    [[using Rosebud: rosetta, large]]
    SgAsmJvmExceptionPtrList exceptions;

public:
    /** Initialize the exception table before parsing.
     *
     *  This is the preferred constructor to use before parsing.  It shall set its parent. */
    explicit SgAsmJvmExceptionTable(SgAsmJvmCodeAttribute*);

    /** Parses a JVM exception table.
     *
     *  Parses a JVM exception table and constructs and parses all exceptions reachable from the table.  Returns a pointer to this
     *  object. */
    SgAsmJvmExceptionTable* parse(SgAsmJvmConstantPool*);

    /** Write exception table to a binary file. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
