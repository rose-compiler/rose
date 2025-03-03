#include <Rose/BinaryAnalysis/Address.h>

/** Represents an ELF segment table.
 *
 * The ELF Segment Table is an ELF Section that has entries describing the various segments of the ELF file.  Each segment
 * is also an SgAsmElfSection and the entries of the ELF Segment Table are associated with the SgAsmElfSection they
 * describe.  The ELF Segment Table can be reconstructed by traversing the AST and finding the SgAsmElfSegmentTableEntry
 * nodes. */
class SgAsmElfSegmentTable: public SgAsmGenericSection {
public:
    /** Constuct segment table linked into the AST. */
    explicit SgAsmElfSegmentTable(SgAsmElfFileHeader*);

    /** Parses an ELF Segment (Program Header) Table.
     *
     *  Parses an ELF segment table and constructs and parses all segments reachable from the table. The section is
     *  extended as necessary based on the number of entries and teh size of each entry. */
    virtual SgAsmElfSegmentTable *parse() override;

    /** Attaches new segments to the segment table.
     *
     *  Attaches a previously unattached ELF Segment (@ref SgAsmElfSection) to the ELF Segment Table (@ref
     *  SgAsmElfSegmentTable). This method complements @ref SgAsmElfSection::initFromSegmentTable. This method
     *  initializes the segment table from the segment while init_from_segment_table initializes the segment from the
     *  segment table.
     *
     *  ELF Segments are represented by @ref SgAsmElfSection objects since ELF Segments and ELF Sections overlap very much in their
     *  features and thus should share an interface. An @ref SgAsmElfSection can appear in the ELF Section Table and/or the ELF
     *  Segment Table and you can determine where it was located with its @ref SgAsmElfSection::sectionEntry and @ref
     *  SgAsmElfSection::segmentEntry properties.
     *
     *  Returns the new segment table entry linked into the AST. */
    SgAsmElfSegmentTableEntry *addSection(SgAsmElfSection*);

    /** Returns info about the size of the entries based on information already available.
     *
     *  Any or all arguments may be null pointers if the caller is not interested in the value. */
    Rose::BinaryAnalysis::Address calculateSizes(size_t *entsize, size_t *required, size_t *optional, size_t *entcount) const;

    virtual bool reallocate() override;

    /** Write the segment table to disk. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging info */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    SgAsmElfSegmentTableEntry *add_section(SgAsmElfSection*) ROSE_DEPRECATED("use addSection");
    Rose::BinaryAnalysis::Address calculate_sizes(size_t*, size_t*, size_t*, size_t*) const ROSE_DEPRECATED("use calculateSizes");
};
