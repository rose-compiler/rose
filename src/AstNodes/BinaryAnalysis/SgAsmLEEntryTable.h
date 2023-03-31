class SgAsmLEEntryTable: public SgAsmGenericSection {
    [[using Rosebud: rosetta]]
    SgSizeTList bundle_sizes;

    [[using Rosebud: rosetta, traverse, large]]
    SgAsmLEEntryPointPtrList entries;

public:
    SgAsmLEEntryTable(SgAsmLEFileHeader *fhdr, rose_addr_t offset);
    virtual void unparse(std::ostream&) const override;
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
