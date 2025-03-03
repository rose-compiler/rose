#include <Rose/BinaryAnalysis/Address.h>

class SgAsmNEStringTable: public SgAsmGenericSection {
public:
    SgAsmNEStringTable(SgAsmNEFileHeader *fhdr, Rose::BinaryAnalysis::Address offset, Rose::BinaryAnalysis::Address size);
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
    std::string get_string(Rose::BinaryAnalysis::Address offset);
};
