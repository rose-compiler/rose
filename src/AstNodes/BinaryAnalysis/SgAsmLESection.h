#ifdef ROSE_IMPL
#include <SgAsmLESectionTableEntry.h>
#endif

class SgAsmLESection: public SgAsmGenericSection {
public:
    [[using Rosebud: rosetta, traverse]]
    SgAsmLESectionTableEntry* sectionTableEntry = nullptr;

public:
    explicit SgAsmLESection(SgAsmLEFileHeader*);
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    SgAsmLESectionTableEntry* get_st_entry() const ROSE_DEPRECATED("use get_sectionTableEntry");
    void set_st_entry(SgAsmLESectionTableEntry*) ROSE_DEPRECATED("use set_sectionTableEntry");
};
