#ifdef ROSE_IMPL
#include <SgAsmElfRelocEntryList.h>
#endif

/** Represents an ELF relocation section. */
class SgAsmElfRelocSection: public SgAsmElfSection {
public:
    /** Property: Whether entries in this section use the addend format. */
    [[using Rosebud: rosetta]]
    bool usesAddend = true;

    /** Property: Section targeted by these relocations. */
    [[using Rosebud: rosetta]]
    SgAsmElfSection* targetSection = nullptr;

    /** Property: List of entries.
     *
     *  This is an AST node whose only purpose is to hold the list. It's done this way due to limitations of ROSETTA. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmElfRelocEntryList* entries = createAndParent<SgAsmElfRelocEntryList>(this);

public:
    SgAsmElfRelocSection(SgAsmElfFileHeader *fhdr, SgAsmElfSymbolSection *symsec,SgAsmElfSection* targetsec);

    using SgAsmElfSection::calculateSizes;
    /** Parse an existing ELF Rela Section */
    virtual SgAsmElfRelocSection *parse() override;

    /** Return sizes for various parts of the table. See doc for SgAsmElfSection::calculateSizes. */
    virtual rose_addr_t calculateSizes(size_t *total, size_t *required, size_t *optional, size_t *entcount) const override;

    virtual bool reallocate() override;

    /** Write section back to disk */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging info */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    bool get_uses_addend() const ROSE_DEPRECATED("use get_usesAddend");
    void set_uses_addend(bool) ROSE_DEPRECATED("use set_usesAddend");
    SgAsmElfSection* get_target_section() const ROSE_DEPRECATED("use get_targetSection");
    void set_target_section(SgAsmElfSection*) ROSE_DEPRECATED("use set_targetSection");
    using SgAsmElfSection::calculate_sizes;
    virtual rose_addr_t calculate_sizes(size_t*, size_t*, size_t*, size_t*) const override ROSE_DEPRECATED("use calculateSizes");
};
