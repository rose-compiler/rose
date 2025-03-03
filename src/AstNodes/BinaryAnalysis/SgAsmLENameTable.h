#include <Rose/BinaryAnalysis/Address.h>

class SgAsmLENameTable: public SgAsmGenericSection {
public:
    [[using Rosebud: rosetta]]
    SgStringList names;

    [[using Rosebud: rosetta]]
    SgUnsignedList ordinals;

public:
    /* This table contains a module name followed by the list of exported function names. Each name is associated with an "ordinal"
     * which serves as an index into the Entry Table. The ordinal for the first string (module name) is meaningless and should be
     * zero. In the non-resident name table the first entry is a module description and the functions are not always resident in
     * system memory (they are discardable). */
    SgAsmLENameTable(SgAsmLEFileHeader *fhdr, Rose::BinaryAnalysis::Address offset);
    virtual void unparse(std::ostream&) const override;
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
