#include <Rose/BinaryAnalysis/Address.h>

class SgAsmLEPageTable: public SgAsmGenericSection {
public:
    [[using Rosebud: rosetta, traverse, large]]
    SgAsmLEPageTableEntryPtrList entries;

public:
    SgAsmLEPageTable(SgAsmLEFileHeader *fhdr, Rose::BinaryAnalysis::Address offset, Rose::BinaryAnalysis::Address size);
    virtual void unparse(std::ostream&) const override;
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
    SgAsmLEPageTableEntry *get_page(size_t idx);
};
