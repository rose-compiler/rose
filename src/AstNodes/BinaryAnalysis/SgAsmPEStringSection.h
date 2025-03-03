#include <Rose/BinaryAnalysis/Address.h>

/** String section. */
class SgAsmPEStringSection: public SgAsmPESection {
public:
    /** Property: String table.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    SgAsmCoffStrtab* strtab = nullptr;

public:
    SgAsmPEStringSection(SgAsmPEFileHeader*);
    virtual SgAsmPEStringSection* parse() override;
    virtual bool reallocate() override;
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
    virtual void set_size(Rose::BinaryAnalysis::Address newsize) override;
    virtual void unparse(std::ostream&) const override;
};
