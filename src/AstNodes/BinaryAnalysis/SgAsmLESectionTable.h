#include <Rose/BinaryAnalysis/Address.h>

class SgAsmLESectionTable: public SgAsmGenericSection {
public:
    SgAsmLESectionTable(SgAsmLEFileHeader *fhdr, Rose::BinaryAnalysis::Address offset, Rose::BinaryAnalysis::Address size);
    virtual void unparse(std::ostream&) const override;
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
