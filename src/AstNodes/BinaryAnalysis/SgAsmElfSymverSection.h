/** The ELF symbol version table.
 *
 *  Often named ".gnu.version section", this section points to an @ref SgAsmElfSymverEntryPtrList containing a list of
 *  SgAsmElfSymverEntry objects. */
class SgAsmElfSymverSection: public SgAsmElfSection {
    /** Property: List of table entries.
     *
     *  The list of entries in this section. The only reason the list is stored in a separate AST node instead of being
     *  stored here directly is due to ROSETTA limitations. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmElfSymverEntryList* entries = createAndParent<SgAsmElfSymverEntryList>(this);

public:
    /** Construct section and link it into the AST. */
    explicit SgAsmElfSymverSection(SgAsmElfFileHeader*);

    /** Initializes section by parsing the file. */
    virtual SgAsmElfSymverSection* parse() override;

    using SgAsmElfSection::calculate_sizes;
    /** Return sizes for various parts of the table.
     *
     *  See documentation for @ref SgAsmElfSection::calculate_sizes. */
    virtual rose_addr_t calculate_sizes(size_t *total, size_t *required, size_t *optional, size_t *nentries) const override;

    /** Write symver table sections back to disk */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging info */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
