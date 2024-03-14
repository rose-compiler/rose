#ifdef ROSE_IMPL
#include <SgAsmNESectionTableEntry.h>
#include <SgAsmNERelocTable.h>
#endif

class SgAsmNESection: public SgAsmGenericSection {
public:
    [[using Rosebud: rosetta, traverse]]
    SgAsmNESectionTableEntry* sectionTableEntry = nullptr;

    [[using Rosebud: rosetta, traverse]]
    SgAsmNERelocTable* relocationTable = nullptr;

public:
    explicit SgAsmNESection(SgAsmNEFileHeader *fhdr);
    virtual void unparse(std::ostream&) const override;
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    SgAsmNESectionTableEntry* get_st_entry() const ROSE_DEPRECATED("use get_sectionTableEntry");
    void set_st_entry(SgAsmNESectionTableEntry*) ROSE_DEPRECATED("use set_sectionTableEntry");
    SgAsmNERelocTable* get_reloc_table() const ROSE_DEPRECATED("use get_relocationTable");
    void set_reloc_table(SgAsmNERelocTable*) ROSE_DEPRECATED("use set_relocationTable");
};
