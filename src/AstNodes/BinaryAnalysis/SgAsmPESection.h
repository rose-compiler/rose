#ifdef ROSE_IMPL
#include <SgAsmPESectionTableEntry.h>
#endif

/** Base class for PE sections. */
class SgAsmPESection: public SgAsmGenericSection {
public:
    /** Property: Section table entry.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmPESectionTableEntry* section_entry = nullptr;

public:
    explicit SgAsmPESection(SgAsmPEFileHeader*);

    /** Initializes the section from data parsed from the PE Section Table.
     *
     *  This includes the section offset, size, memory mapping, alignments, permissions, etc. This function complements
     *  SgAsmPESectionTable::add_section(): this function initializes this section from the section table while
     *  add_section() initializes the section table from the section. */
    SgAsmPESection *initFromSectionTable(SgAsmPESectionTableEntry *entry, int id);
    virtual bool reallocate() override;
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    SgAsmPESection *init_from_section_table(SgAsmPESectionTableEntry*, int) ROSE_DEPRECATED("use initFromSectionTable");
};
