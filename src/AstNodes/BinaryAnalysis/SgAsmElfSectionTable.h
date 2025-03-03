#include <Rose/BinaryAnalysis/Address.h>

/** Represents an ELF section table.
 *
 *  The ELF Section Table is itself a section.  The entries of the table are stored with the section they describe rather
 *  than storing them all in the SgAsmSectionTable node.  We can reconstruct the ELF Section Table since sections have
 *  unique ID numbers that are their original indices in the ELF Section Table. */
class SgAsmElfSectionTable: public SgAsmGenericSection {
public:
    /** Constructor creates section table within the AST. */
    explicit SgAsmElfSectionTable(SgAsmElfFileHeader*);

    /** Parses an ELF Section Table.
     *
     *  Parses an ELF section table and constructs and parses all sections reachable from the table. The section is
     *  extended as necessary based on the number of entries and the size of each entry. Returns a pointer to this
     *  object. */
    virtual SgAsmElfSectionTable *parse() override;

    /** Attaches a previously unattached ELF Section to the section table.
     *
     *  If @p section is an ELF String Section (SgAsmElfStringSection) that contains an ELF String Table
     *  (SgAsmElfStringTable) and the ELF Section Table has no associated string table then the @p section will be used as
     *  the string table to hold the section names.
     *
     *  This method complements SgAsmElfSection::init_from_section_table. This method initializes the section table from
     *  the section while init_from_section_table() initializes the section from the section table.
     *
     *  Returns the new section table entry linked into the AST. */
    SgAsmElfSectionTableEntry *addSection(SgAsmElfSection*);

    /** Returns info about the size of the entries based on information already available.
     *
     *  Any or all arguments may be null pointers if the caller is not interested in the value. */
    Rose::BinaryAnalysis::Address calculateSizes(size_t *entsize, size_t *required, size_t *optional, size_t *entcount) const;

    virtual bool reallocate() override;

    /** Write the section table section back to disk */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging info */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    SgAsmElfSectionTableEntry *add_section(SgAsmElfSection*) ROSE_DEPRECATED("use addSection");
    Rose::BinaryAnalysis::Address calculate_sizes(size_t*, size_t*, size_t*, size_t*) const ROSE_DEPRECATED("use calculateSizes");
};
