#include <Rose/BinaryAnalysis/Address.h>

class SgAsmNEEntryTable: public SgAsmGenericSection {
public:
    [[using Rosebud: rosetta]]
    SgSizeTList bundle_sizes;

    [[using Rosebud: rosetta, traverse, large]]
    SgAsmNEEntryPointPtrList entries;

public:
    SgAsmNEEntryTable(SgAsmNEFileHeader *fhdr, Rose::BinaryAnalysis::Address offset, Rose::BinaryAnalysis::Address size);
    void populate_entries();
    virtual void unparse(std::ostream&) const override;
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
