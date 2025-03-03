#include <Rose/BinaryAnalysis/Address.h>

#ifdef ROSE_IMPL
#include <SgAsmElfSectionTableEntry.h>
#include <SgAsmElfSegmentTableEntry.h>
#endif

/** Base class for ELF file sections. */
class SgAsmElfSection: public SgAsmGenericSection {
public:
    /** Property: Linked section.
     *
     *  Points to an optional related section. See official ELF specification.
     *
     * @{ */
    [[using Rosebud: rosetta, mutators()]]
    SgAsmElfSection* linkedSection = nullptr;
    virtual void set_linkedSection(SgAsmElfSection*);
    /** @} */

    /** Property: The section table entry corresponding to this section. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmElfSectionTableEntry* sectionEntry = nullptr;

    /** Property: The segment table entry corresponding to this section. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmElfSegmentTableEntry* segmentEntry = nullptr;

public:
    /** Constructor for sections not yet in a table.
     *
     *  This constructs a section that is in neither the ELF Section Table nor the ELF Segment Table yet, but eventually
     *  will be. */
    explicit SgAsmElfSection(SgAsmGenericHeader*);

    /** Initializes the section from data parsed from the ELF Section Table.
     *
     *  This includes the section name, offset, size, memory mapping, and alignments. The @p id is the index into the section
     *  table. This function complements @ref SgAsmElfSectionTable::addSection in that this function initializes this section from
     *  the section table while @ref SgAsmElfSectionTable::addSection initializes the section table from the section. */
    SgAsmElfSection *initFromSectionTable(SgAsmElfSectionTableEntry*, SgAsmElfStringSection*, int id);

    /** Initializes the section from data parse from the ELF Segment Table.
     *
     *  This is similar to @ref initFromSectionTable but for segments instead of sections. */
    SgAsmElfSection *initFromSegmentTable(SgAsmElfSegmentTableEntry*, bool mmapOnly=false);

    /** Returns info about the size of the entries based on information already available.
     *
     *  Any or all arguments may be null pointers if the caller is not interested in the value. Return values are:
     *
     *  @li @p entsize is the size of each entry, sum of required and optional parts. This comes from the sh_entsize member
     *  of this section's ELF Section Table Entry, adjusted upward to be large enough to hold the required part of each
     *  entry (see "required").
     *
     *  @li @p required is the size of the required (leading) part of each entry. The size of the required part is based
     *  on the ELF word size.
     *
     *  @li @p optional is the size of the optional (trailing) part of each entry. If the section has been parsed then
     *  the optional size will be calculated from the entry with the largest "extra" (aka, optional) data. Otherwise this
     *  is calculated as the difference between the @p entsize" and the @p required" sizes.
     *
     *  @li entcount is the total number of entries in this section. If the section has been parsed then this is the
     *  actual number of parsed entries, otherwise its the section size divided by the @p entsize.
     *
     *  Return value is the total size needed for the section. In all cases, it is the product of @p entsize and @p
     *  entcount. */
    Rose::BinaryAnalysis::Address calculateSizes(size_t r32size, size_t r64size, const std::vector<size_t> &optsizes,
                                           size_t *entsize, size_t *required, size_t *optional, size_t *entcount) const;

    virtual void finishParsing();

    /** Base implementation for calculating sizes.
     *
     *  Most subclasses will override this virtual function in order to return more useful values. This implementation
     *  returns the following values:
     *
     *  @li @p entsize is size stored in the ELF Section Table's sh_entsize member, or size of entire section if not a
     *  table.
     *
     *  @li @p required is the same as @p entsize.
     *
     *  @li @p optional is zero.
     *
     *  @li @p entcount is the number of entries, each of size entsize, that can fit in the section.
     *
     *  The return size is the product of @p entsize and @p entcount, which, if this section is a table (nonzero
     *  sh_entsize), could be smaller than the total size of the section. */
    virtual Rose::BinaryAnalysis::Address
    calculateSizes(size_t *entsize, size_t *required, size_t *optional, size_t *entcount) const;

    virtual bool reallocate() override;

    /** Print some debugging info */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;

    /** Make this section's name to be stored in the specified string table. */
    void allocateNameToStorage(SgAsmElfStringSection*);

    /** Obtain ELF header.
     *
     *  This is just a convenience function so we don't need to constantly cast the return value from the @ref header property. */
    SgAsmElfFileHeader *get_elfHeader() const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    SgAsmElfSection* get_linked_section() const ROSE_DEPRECATED("use get_linkedSection");
    virtual void set_linked_section(SgAsmElfSection*) ROSE_DEPRECATED("use set_linkSection");
    SgAsmElfSectionTableEntry* get_section_entry() const ROSE_DEPRECATED("use get_sectionEntry");
    void set_section_entry(SgAsmElfSectionTableEntry*) ROSE_DEPRECATED("use set_sectionEntry");
    SgAsmElfSegmentTableEntry* get_segment_entry() const ROSE_DEPRECATED("use get_segmentEntry");
    void set_segment_entry(SgAsmElfSegmentTableEntry*) ROSE_DEPRECATED("use set_segmentEntry");
    SgAsmElfSection *init_from_section_table(SgAsmElfSectionTableEntry*, SgAsmElfStringSection*, int)
        ROSE_DEPRECATED("use initFromSectionTable");
    SgAsmElfSection *init_from_segment_table(SgAsmElfSegmentTableEntry*, bool mmap_only=false)
        ROSE_DEPRECATED("use initFromSegmentTable");
    Rose::BinaryAnalysis::Address
    calculate_sizes(size_t, size_t, const std::vector<size_t>&, size_t*, size_t*, size_t*, size_t*) const
        ROSE_DEPRECATED("use calculateSizes");
    virtual void finish_parsing() ROSE_DEPRECATED("use finishParsing");
    virtual Rose::BinaryAnalysis::Address
    calculate_sizes(size_t*, size_t*, size_t*, size_t*) const ROSE_DEPRECATED("use calculateSizes");
    void allocate_name_to_storage(SgAsmElfStringSection*) ROSE_DEPRECATED("use allocateNameToStorage");
    SgAsmElfFileHeader *get_elf_header() const ROSE_DEPRECATED("use get_elfHeader");
};
