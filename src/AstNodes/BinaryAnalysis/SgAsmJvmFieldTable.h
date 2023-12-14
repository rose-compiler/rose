/** Represents a JVM field_info table/array.
 *
 *  The JVM fields table is a section.  The entries of the table are stored with the section they describe rather than storing them
 *  all in the SgAsmSectionTable node.  We can reconstruct the JVM Section Table since sections have unique ID numbers that are
 *  their original indices in the JVM Section Table. */
class SgAsmJvmFieldTable: public SgAsmGenericSection {
public:
    /** Property: List of pointers to fields. */
    [[using Rosebud: rosetta, traverse, large]]
    SgAsmJvmFieldPtrList fields;

public:
    /** Initialize the field table before parsing.
     *
     *  This is the preferred constructor to use before parsing.  It shall set its parent. */
    explicit SgAsmJvmFieldTable(SgAsmJvmFileHeader*);

    /** Parses a JVM field table.
     *
     *  Parses a JVM field table and constructs and parses all fields reachable from the table. Returns a pointer to this object. */
    virtual SgAsmJvmFieldTable* parse() override;

    /** Write field table to a binary file. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
