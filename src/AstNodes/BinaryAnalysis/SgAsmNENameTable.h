#include <Rose/BinaryAnalysis/Address.h>

class SgAsmNENameTable: public SgAsmGenericSection {
public:
    [[using Rosebud: rosetta]]
    SgStringList names;

    [[using Rosebud: rosetta]]
    SgUnsignedList ordinals;

public:
    SgAsmNENameTable(SgAsmNEFileHeader *fhdr, Rose::BinaryAnalysis::Address offset);
    virtual void unparse(std::ostream&) const override;
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
    std::vector<std::string> get_namesByOrdinal(unsigned ordinal);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    std::vector<std::string> get_names_by_ordinal(unsigned) ROSE_DEPRECATED("use get_namesByOrdinal");

};
