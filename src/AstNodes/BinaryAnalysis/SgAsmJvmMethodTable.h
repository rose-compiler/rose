/** Represents a JVM method_info table/array.
 *
 *  The JVM Constant Pool is itself a section.  The entries of the table are stored with the section they describe rather than
 *  storing them all in the SgAsmSectionTable node.  We can reconstruct the JVM Section Table since sections have unique ID numbers
 *  that are their original indices in the JVM Section Table. */
class SgAsmJvmMethodTable: public SgAsmGenericSection {
    /** Property: List of pointers to methods. */
    [[using Rosebud: rosetta, traverse, large]]
    SgAsmJvmMethodPtrList methods;

public:
    /** Initialize the method table before parsing.
     *
     *  This is the preferred constructor to use before parsing.  It shall set its parent. */
    explicit SgAsmJvmMethodTable(SgAsmJvmFileHeader*);

    /** Parses a JVM method table.
     *
     *  Parses a JVM method table and constructs and parses all methods reachable from the method table section. Returns a pointer
     *  to this object. */
    virtual SgAsmJvmMethodTable* parse() override;

    /** Write method table to a binary file. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
