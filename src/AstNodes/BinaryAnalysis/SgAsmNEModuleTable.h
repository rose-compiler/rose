class SgAsmNEModuleTable: public SgAsmGenericSection {
    [[using Rosebud: rosetta, traverse]]
    SgAsmNEStringTable* strtab = nullptr;

    [[using Rosebud: rosetta]]
    SgAddressList name_offsets;

    [[using Rosebud: rosetta]]
    SgStringList names;

public:
    SgAsmNEModuleTable(SgAsmNEFileHeader *fhdr, SgAsmNEStringTable *strtab, rose_addr_t offset, rose_addr_t size);
    virtual void unparse(std::ostream&) const override;
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
