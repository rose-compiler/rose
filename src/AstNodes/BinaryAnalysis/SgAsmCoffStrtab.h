/** COFF symbol string table. */
class SgAsmCoffStrtab: public SgAsmGenericStrtab {
public:
    explicit SgAsmCoffStrtab(class SgAsmPESection *containing_section);
    void destructorHelper() override;
    virtual void unparse(std::ostream&) const;
    virtual SgAsmStringStorage *createStorage(rose_addr_t offset, bool shared) override;
    virtual rose_addr_t get_storageSize(const SgAsmStringStorage*) override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    virtual SgAsmStringStorage *create_storage(rose_addr_t, bool) override ROSE_DEPRECATED("use createStorage");
    virtual rose_addr_t get_storage_size(const SgAsmStringStorage*) override ROSE_DEPRECATED("use get_storageSize");
};
