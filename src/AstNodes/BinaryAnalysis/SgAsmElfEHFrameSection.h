#include <Rose/BinaryAnalysis/Address.h>

#ifdef ROSE_IMPL
#include <SgAsmElfEHFrameEntryCIList.h>
#endif

/** Represents an ELF EH frame section. */
class SgAsmElfEHFrameSection: public SgAsmElfSection {
public:
    /** Property: CI entries.
     *
     *  See official ELF specification.  This property points to an AST node containing the list rather than the direct
     *  list due to limitations of ROSETTA. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmElfEHFrameEntryCIList* ci_entries = createAndParent<SgAsmElfEHFrameEntryCIList>(this);

public:
    /** Non-parsing constructor. */
    explicit SgAsmElfEHFrameSection(SgAsmElfFileHeader*);

    /** Initialize by parsing a file. */
    virtual SgAsmElfEHFrameSection *parse() override;

    /** Return sizes for various parts of the table.
     *
     *  See documentation for @ref SgAsmElfSection::calculateSizes. Since EH Frame Sections are run-length encoded, we
     *  need to actually unparse the section in order to determine its size. */
    virtual Rose::BinaryAnalysis::Address
    calculateSizes(size_t *total, size_t *required, size_t *optional, size_t *entcount) const override;

    /** Write data to .eh_frame section */
    virtual void unparse(std::ostream&) const override;

    /** Unparses the section into the optional output stream and returns the number of bytes written.
     *
     *  If there is no output stream we still go through the actions but don't write anything. This is the only way to
     *  determine the amount of memory required to store the section since the section is run-length encoded. */
    Rose::BinaryAnalysis::Address unparse(std::ostream*) const;

    /** Print some debugging info */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    virtual Rose::BinaryAnalysis::Address calculate_sizes(size_t*, size_t*, size_t*, size_t*) const override
        ROSE_DEPRECATED("use calculateSizes");
};
