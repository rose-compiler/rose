/** ELF section containing dynamic linking information. */
class SgAsmElfDynamicSection: public SgAsmElfSection {
    /** Property: List of entries.
     *
     *  This points to a node whose only purpose is to hold the list. The list cannot be contained directly here where it's
     *  needed due to limitations of ROSETTA. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmElfDynamicEntryList* entries = createAndParent<SgAsmElfDynamicEntryList>(this);

public:
    /** Constructor that links object into the AST. */
    SgAsmElfDynamicSection(SgAsmElfFileHeader *fhdr, SgAsmElfStringSection *strsec);

    /** Initialize object by parsing file. */
    virtual SgAsmElfDynamicSection* parse() override;

    /** Finish initializing the section entries. */
    virtual void finish_parsing() override;

    using SgAsmElfSection::calculate_sizes;
    /** Return sizes for various parts of the table. See documentation for @ref SgAsmElfSection::calculate_sizes. */
    virtual rose_addr_t calculate_sizes(size_t *total, size_t *required, size_t *optional, size_t *entcount) const override;

    /** Called prior to unparse to make things consistent. */
    virtual bool reallocate() override;

    /** Write the dynamic section back to disk */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging info */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
