#include <Rose/BinaryAnalysis/Address.h>

/** COFF symbol string table. */
class SgAsmCoffStrtab: public SgAsmGenericStrtab {
public:
    explicit SgAsmCoffStrtab(class SgAsmPESection *containing_section);
    void destructorHelper() override;
    virtual void unparse(std::ostream&) const;
    virtual SgAsmStringStorage *createStorage(Rose::BinaryAnalysis::Address offset, bool shared) override;
    virtual Rose::BinaryAnalysis::Address get_storageSize(const SgAsmStringStorage*) override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual SgAsmStringStorage *create_storage(Rose::BinaryAnalysis::Address, bool) override ROSE_DEPRECATED("use createStorage");
    virtual Rose::BinaryAnalysis::Address get_storage_size(const SgAsmStringStorage*) override
        ROSE_DEPRECATED("use get_storageSize");
};
