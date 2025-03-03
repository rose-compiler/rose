#include <Rose/BinaryAnalysis/Address.h>

#ifdef ROSE_IMPL
#include <SgAsmNEStringTable.h>
#endif

class SgAsmNEModuleTable: public SgAsmGenericSection {
public:
    [[using Rosebud: rosetta, traverse]]
    SgAsmNEStringTable* strtab = nullptr;

    [[using Rosebud: rosetta]]
    SgAddressList nameOffsets;

    [[using Rosebud: rosetta]]
    SgStringList names;

public:
    SgAsmNEModuleTable(SgAsmNEFileHeader *fhdr, SgAsmNEStringTable *strtab, Rose::BinaryAnalysis::Address offset,
                       Rose::BinaryAnalysis::Address size);
    virtual void unparse(std::ostream&) const override;
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    const SgAddressList& get_name_offsets() const ROSE_DEPRECATED("use get_nameOffsets");
    void set_name_offsets(const SgAddressList&) ROSE_DEPRECATED("use set_nameOffsets");
};
