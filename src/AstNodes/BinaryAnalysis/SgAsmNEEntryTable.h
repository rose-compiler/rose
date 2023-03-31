class SgAsmNEEntryTable: public SgAsmGenericSection {
    [[using Rosebud: rosetta]]
    SgSizeTList bundle_sizes;

    [[using Rosebud: rosetta, traverse, large]]
    SgAsmNEEntryPointPtrList entries;

public:
    SgAsmNEEntryTable(SgAsmNEFileHeader *fhdr, rose_addr_t offset, rose_addr_t size);
    void populate_entries();
    virtual void unparse(std::ostream&) const override;
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
