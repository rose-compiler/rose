#include <Rose/BinaryAnalysis/Address.h>

class SgAsmNESectionTable: public SgAsmGenericSection {
public:
    [[using Rosebud: rosetta]]
    unsigned flags = 0;

    [[using Rosebud: rosetta]]
    unsigned sector = 0;

    [[using Rosebud: rosetta]]
    Rose::BinaryAnalysis::Address physicalSize = 0;

    [[using Rosebud: rosetta]]
    Rose::BinaryAnalysis::Address virtualSize = 0;

public:
    explicit SgAsmNESectionTable(SgAsmNEFileHeader*);
    virtual void unparse(std::ostream&) const override;
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    Rose::BinaryAnalysis::Address get_physical_size() const ROSE_DEPRECATED("use get_physicalSize");
    void set_physical_size(Rose::BinaryAnalysis::Address) ROSE_DEPRECATED("use set_physicalSize");
    Rose::BinaryAnalysis::Address get_virtual_size() const ROSE_DEPRECATED("use get_virtualSize");
    void set_virtual_size(Rose::BinaryAnalysis::Address) ROSE_DEPRECATED("use set_virtualSize");
};
