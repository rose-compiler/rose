#include <Rose/BinaryAnalysis/Address.h>

class SgAsmLERelocTable: public SgAsmGenericSection {
public:
    [[using Rosebud: rosetta, traverse, large]]
    SgAsmLERelocEntryPtrList entries;

public:
    SgAsmLERelocTable(SgAsmLEFileHeader *fhdr, Rose::BinaryAnalysis::Address offset);
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
