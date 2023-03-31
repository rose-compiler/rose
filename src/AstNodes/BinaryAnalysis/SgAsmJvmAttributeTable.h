/** Represents a JVM attribute_info table/array.
 *
 *  A JVM attribute table is a section.  The entries of the table are stored with the section they describe rather than storing them
 *  all in the SgAsmSectionTable node.  We can reconstruct the JVM Section Table since sections have unique ID numbers that are
 *  their original indices in the JVM Section Table. */
class SgAsmJvmAttributeTable: public SgAsmGenericSection {
    /** Property: List of pointers to attributes. */
    [[using Rosebud: rosetta, traverse, large]]
    SgAsmJvmAttributePtrList attributes;

public:
    /** Initialize the attribute table before parsing. */
    explicit SgAsmJvmAttributeTable(SgAsmJvmFileHeader*, SgAsmNode*);

    /** Parses a JVM attribute table.
     *
     *  Parses a JVM attribute table and constructs and parses all attributes reachable from the table section. Returns a pointer to
     *  this object. */
    using SgAsmGenericSection::parse; // Removes warning messages on some compilers
                                      // ("hidden virtual overloaded function")
    SgAsmJvmAttributeTable* parse(SgAsmJvmConstantPool*);

    /** Print some debugging information */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
