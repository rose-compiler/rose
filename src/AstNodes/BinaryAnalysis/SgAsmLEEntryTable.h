#include <Rose/BinaryAnalysis/Address.h>

class SgAsmLEEntryTable: public SgAsmGenericSection {
public:
    [[using Rosebud: rosetta]]
    SgSizeTList bundle_sizes;

    [[using Rosebud: rosetta, traverse, large]]
    SgAsmLEEntryPointPtrList entries;

public:
    SgAsmLEEntryTable(SgAsmLEFileHeader *fhdr, Rose::BinaryAnalysis::Address offset);
    virtual void unparse(std::ostream&) const override;
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
