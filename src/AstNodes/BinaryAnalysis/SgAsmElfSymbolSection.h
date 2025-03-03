#include <Rose/BinaryAnalysis/Address.h>

#ifdef ROSE_IMPL
#include <SgAsmElfSymbolList.h>
#endif

/** ELF file section containing symbols. */
class SgAsmElfSymbolSection: public SgAsmElfSection {
public:
    /** Property: Whether this section represents dynamic linking symbols. */
    [[using Rosebud: rosetta]]
    bool isDynamic = false;

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
    virtual void finishParsing() override;

    /** Given a symbol, return its index in this symbol table. */
    size_t indexOf(SgAsmElfSymbol*);

    using SgAsmElfSection::calculateSizes;
    /** Return sizes for various parts of the table.
     *
     *  See documentation for @ref SgAsmElfSection::calculateSizes. */
    virtual Rose::BinaryAnalysis::Address
    calculateSizes(size_t *total, size_t *required, size_t *optional, size_t *nentries) const override;

    /** Called prior to unparsing.
     *
     *  Updates symbol entries with name offsets. */
    virtual bool reallocate() override;

    /** Write symbol table sections back to disk. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging info */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    bool get_is_dynamic() const ROSE_DEPRECATED("use get_isDynamic");
    void set_is_dynamic(bool) ROSE_DEPRECATED("use set_isDynamic");
    virtual void finish_parsing() override ROSE_DEPRECATED("use finishParsing");
    size_t index_of(SgAsmElfSymbol*) ROSE_DEPRECATED("use indexOf");
    using SgAsmElfSection::calculate_sizes;
    virtual Rose::BinaryAnalysis::Address calculate_sizes(size_t*, size_t*, size_t*, size_t*) const override
        ROSE_DEPRECATED("use calculateSizes");
};
