/** ELF file section containing symbols. */
class SgAsmElfSymbolSection: public SgAsmElfSection {
    /** Property: Whether this section represents dynamic linking symbols. */
    [[using Rosebud: rosetta]]
    bool is_dynamic = false;

    /** Property: Symbols.
     *
     *  List of symbols contained in this symbol table.  The actual list is stored in a separate AST instead of being stored
     *  directly in this node due to limitations of ROSETTA. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmElfSymbolList* symbols = createAndParent<SgAsmElfSymbolList>(this);

public:
    /** Non-parsing constructor */
    SgAsmElfSymbolSection(SgAsmElfFileHeader *fhdr, SgAsmElfStringSection *strsec);

    /** Initialize by parsing a file. */
    virtual SgAsmElfSymbolSection* parse() override;

    /** Update section pointers for locally-bound symbols.
     *
     *  Now that the section table has been read and all non-synthesized sections have been created, we can update
     *  pointers to other things.
     *
     *  The st_shndx is the index (ID) of the section to which the symbol is bound. Special values are:
     *
     *  @li 0x0000: no section (section table entry zero should be all zeros anyway)
     *  @li 0xff00-0xffff: reserved values, not an index
     *  @li 0xff00-0xff1f: processor specific values
     *  @li 0xfff1: symbol has absolute value not affected by relocation
     *  @li 0xfff2: symbol is fortran common or unallocated C extern */
    virtual void finish_parsing() override;

    /** Given a symbol, return its index in this symbol table. */
    size_t index_of(SgAsmElfSymbol*);

    using SgAsmElfSection::calculate_sizes;
    /** Return sizes for various parts of the table.
     *
     *  See documentation for @ref SgAsmElfSection::calculate_sizes. */
    virtual rose_addr_t calculate_sizes(size_t *total, size_t *required, size_t *optional, size_t *nentries) const override;

    /** Called prior to unparsing.
     *
     *  Updates symbol entries with name offsets. */
    virtual bool reallocate() override;

    /** Write symbol table sections back to disk. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging info */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
