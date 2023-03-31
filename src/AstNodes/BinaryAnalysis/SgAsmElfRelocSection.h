/** Represents an ELF relocation section. */
class SgAsmElfRelocSection: public SgAsmElfSection {
    /** Property: Whether entries in this section use the addend format. */
    [[using Rosebud: rosetta]]
    bool uses_addend = true;

    /** Property: Section targeted by these relocations. */
    [[using Rosebud: rosetta]]
    SgAsmElfSection* target_section = nullptr;

    /** Property: List of entries.
     *
     *  This is an AST node whose only purpose is to hold the list. It's done this way due to limitations of ROSETTA. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmElfRelocEntryList* entries = createAndParent<SgAsmElfRelocEntryList>(this);

public:
    SgAsmElfRelocSection(SgAsmElfFileHeader *fhdr, SgAsmElfSymbolSection *symsec,SgAsmElfSection* targetsec);

    using SgAsmElfSection::calculate_sizes;
    /** Parse an existing ELF Rela Section */
    virtual SgAsmElfRelocSection *parse() override;

    /** Return sizes for various parts of the table. See doc for SgAsmElfSection::calculate_sizes. */
    virtual rose_addr_t calculate_sizes(size_t *total, size_t *required, size_t *optional, size_t *entcount) const override;

    /** Pre-unparsing adjustments */
    virtual bool reallocate() override;

    /** Write section back to disk */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging info */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
